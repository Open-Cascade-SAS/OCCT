#!/usr/bin/env python3
# Copyright (c) 2025 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

"""
OCCT 8.0.0 Unused Typedef Cleanup Script

Removes unused typedef declarations from source files.
- For .hxx, .lxx files: checks for global usage across all source files
- For .cxx, .pxx, .gxx, .c, .mm files: checks for local usage within the same file

Handles multi-line typedefs properly.

Usage:
    python3 cleanup_unused_typedefs.py [options] <src_directory>

Options:
    --dry-run    Show what would be done without making changes
    --verbose    Show detailed progress
    --output     Output JSON file for results
"""

import argparse
import json
import os
import re
from pathlib import Path
from typing import Dict, List, Set, Tuple
from dataclasses import dataclass, field


@dataclass
class TypedefInfo:
    """Information about a typedef declaration."""
    name: str
    start_line: int
    end_line: int
    content: str  # Full typedef text (may span multiple lines)
    file_path: str
    is_ncollection: bool = False


@dataclass
class CleanupResult:
    """Result of cleanup operations."""
    typedefs_removed: List[Dict] = field(default_factory=list)
    files_modified: List[str] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)


class UnusedTypedefCleaner:
    """Removes unused typedef declarations."""

    # Extensions that require global usage check
    GLOBAL_CHECK_EXTENSIONS = {'.hxx', '.lxx'}

    # Extensions that require local usage check only
    LOCAL_CHECK_EXTENSIONS = {'.cxx', '.pxx', '.gxx', '.c', '.mm'}

    SKIP_DIRS = {'build', 'install', '.git', '__pycache__', 'mac64', 'win64', 'lin64'}

    def __init__(self, src_dir: str, dry_run: bool = False, verbose: bool = False):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.verbose = verbose
        # Build global index: typedef_name -> set of files where it's used
        self.global_usage: Dict[str, Set[str]] = {}
        self.all_typedefs: List[TypedefInfo] = []

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def get_source_files(self) -> List[Path]:
        """Get all source files."""
        all_extensions = self.GLOBAL_CHECK_EXTENSIONS | self.LOCAL_CHECK_EXTENSIONS
        files = []
        for root, dirs, filenames in os.walk(self.src_dir):
            dirs[:] = [d for d in dirs if d not in self.SKIP_DIRS]
            for filename in filenames:
                if any(filename.endswith(ext) for ext in all_extensions):
                    files.append(Path(root) / filename)
        return sorted(files)

    @staticmethod
    def _line_has_semicolon(line: str) -> bool:
        """Check if line contains a semicolon (ignoring comments)."""
        # Remove C++ style comments
        comment_pos = line.find('//')
        if comment_pos != -1:
            line = line[:comment_pos]
        return ';' in line

    def find_typedefs_in_file(self, filepath: Path, content: str) -> List[TypedefInfo]:
        """Find all typedef declarations in a file, including multi-line ones."""
        typedefs = []
        try:
            rel_path = str(filepath.relative_to(self.src_dir))
        except ValueError:
            rel_path = str(filepath)

        lines = content.split('\n')
        i = 0
        while i < len(lines):
            line = lines[i]
            stripped = line.strip()

            # Check if line starts with typedef at column 0 (not indented - skip class members)
            # Only process typedefs that start at the beginning of the line (top-level)
            if line.startswith('typedef ') or (stripped.startswith('typedef ') and not line[0].isspace()):
                start_line = i + 1  # 1-based
                typedef_lines = [line]

                # Check if typedef continues to next lines (no semicolon)
                while not self._line_has_semicolon(line) and i + 1 < len(lines):
                    i += 1
                    line = lines[i]
                    typedef_lines.append(line)

                end_line = i + 1  # 1-based
                typedef_content = '\n'.join(typedef_lines)

                # Extract typedef name (last identifier before semicolon)
                # Handle: typedef NCollection_Sequence<...> TypeName;
                name_match = re.search(r'\b(\w+)\s*;', typedef_content)
                if name_match:
                    typedef_name = name_match.group(1)

                    # Skip standard types
                    if typedef_name.startswith('Standard_') or typedef_name.startswith('Handle_'):
                        i += 1
                        continue

                    # Check if it's NCollection typedef (base type starts with NCollection_)
                    # Match: typedef NCollection_Something<...> Name;
                    # Don't match: typedef std::false_type propagate_on_container_move_assignment;
                    is_ncollection = bool(re.search(
                        r'typedef\s+NCollection_\w+',
                        typedef_content
                    ))

                    typedefs.append(TypedefInfo(
                        name=typedef_name,
                        start_line=start_line,
                        end_line=end_line,
                        content=typedef_content,
                        file_path=rel_path,
                        is_ncollection=is_ncollection
                    ))

            i += 1

        return typedefs

    def build_global_usage_index(self, files: List[Path]):
        """Build index of all typedef usages across all files in single pass."""
        print("Building global usage index...")

        # First pass: collect all typedefs
        file_contents: Dict[str, str] = {}
        for filepath in files:
            try:
                content = filepath.read_text(encoding='utf-8', errors='replace')
                rel_path = str(filepath.relative_to(self.src_dir))
                file_contents[rel_path] = content

                typedefs = self.find_typedefs_in_file(filepath, content)
                self.all_typedefs.extend(typedefs)
            except Exception as e:
                print(f"Error reading {filepath}: {e}")

        print(f"Found {len(self.all_typedefs)} NCollection typedefs")

        # Filter to only NCollection typedefs
        ncollection_typedefs = [t for t in self.all_typedefs if t.is_ncollection]
        print(f"Processing {len(ncollection_typedefs)} NCollection typedefs")

        # Build combined pattern for all typedef names
        typedef_names = list(set(t.name for t in ncollection_typedefs))
        if not typedef_names:
            return

        # Sort by length (longest first) for correct matching
        typedef_names.sort(key=len, reverse=True)

        # Initialize usage dict
        for name in typedef_names:
            self.global_usage[name] = set()

        # Single pass through all files to find usages
        pattern = re.compile(r'\b(' + '|'.join(re.escape(n) for n in typedef_names) + r')\b')

        for rel_path, content in file_contents.items():
            for match in pattern.finditer(content):
                typedef_name = match.group(1)
                self.global_usage[typedef_name].add(rel_path)

        print("Global usage index built")

    def is_typedef_used(self, typedef: TypedefInfo, file_content: str) -> bool:
        """Check if typedef is used (considers global vs local based on file type)."""
        ext = Path(typedef.file_path).suffix

        if ext in self.GLOBAL_CHECK_EXTENSIONS:
            # Check global usage
            usage_files = self.global_usage.get(typedef.name, set())

            # Remove self-references (the typedef definition itself)
            # Check if used in any other file OR used elsewhere in same file
            other_files = usage_files - {typedef.file_path}
            if other_files:
                return True

            # Check if used elsewhere in the same file (not just definition)
            lines = file_content.split('\n')
            content_without_typedef = '\n'.join(
                line for i, line in enumerate(lines, 1)
                if i < typedef.start_line or i > typedef.end_line
            )
            pattern = r'\b' + re.escape(typedef.name) + r'\b'
            return bool(re.search(pattern, content_without_typedef))
        else:
            # Local check only
            lines = file_content.split('\n')
            content_without_typedef = '\n'.join(
                line for i, line in enumerate(lines, 1)
                if i < typedef.start_line or i > typedef.end_line
            )
            pattern = r'\b' + re.escape(typedef.name) + r'\b'
            return bool(re.search(pattern, content_without_typedef))

    def remove_typedefs_from_file(self, filepath: Path,
                                   typedefs_to_remove: List[TypedefInfo]) -> bool:
        """Remove specified typedef lines from a file."""
        try:
            content = filepath.read_text(encoding='utf-8', errors='replace')
        except Exception:
            return False

        lines = content.split('\n')

        # Collect all line ranges to remove
        lines_to_remove: Set[int] = set()
        for typedef in typedefs_to_remove:
            for line_num in range(typedef.start_line, typedef.end_line + 1):
                lines_to_remove.add(line_num)

        # Build new content without the typedef lines
        new_lines = []
        for i, line in enumerate(lines, 1):
            if i not in lines_to_remove:
                new_lines.append(line)

        new_content = '\n'.join(new_lines)

        # Clean up multiple blank lines
        new_content = re.sub(r'\n{3,}', '\n\n', new_content)

        if new_content != content:
            if not self.dry_run:
                filepath.write_text(new_content, encoding='utf-8')
            return True

        return False

    def run(self) -> CleanupResult:
        """Run the cleanup process."""
        result = CleanupResult()

        # Get all source files
        files = self.get_source_files()
        print(f"Found {len(files)} source files")

        # Build global usage index (single pass)
        self.build_global_usage_index(files)

        # Group typedefs by file
        typedefs_by_file: Dict[str, List[TypedefInfo]] = {}
        for typedef in self.all_typedefs:
            if typedef.is_ncollection:
                if typedef.file_path not in typedefs_by_file:
                    typedefs_by_file[typedef.file_path] = []
                typedefs_by_file[typedef.file_path].append(typedef)

        # Process each file
        files_to_modify: Dict[str, List[TypedefInfo]] = {}

        for rel_path, typedefs in typedefs_by_file.items():
            filepath = self.src_dir / rel_path
            try:
                content = filepath.read_text(encoding='utf-8', errors='replace')
            except Exception:
                continue

            unused_typedefs = []
            for typedef in typedefs:
                if not self.is_typedef_used(typedef, content):
                    unused_typedefs.append(typedef)
                    self.log(f"  Unused: {typedef.name} in {rel_path}:{typedef.start_line}")

            if unused_typedefs:
                files_to_modify[str(filepath)] = unused_typedefs

        # Remove unused typedefs
        print(f"\n{'DRY RUN: ' if self.dry_run else ''}Removing unused typedefs from {len(files_to_modify)} files...")

        for filepath_str, typedefs in files_to_modify.items():
            filepath = Path(filepath_str)
            if self.remove_typedefs_from_file(filepath, typedefs):
                result.files_modified.append(filepath_str)
                for typedef in typedefs:
                    result.typedefs_removed.append({
                        'name': typedef.name,
                        'file': typedef.file_path,
                        'start_line': typedef.start_line,
                        'end_line': typedef.end_line,
                        'is_ncollection': typedef.is_ncollection
                    })

        return result


def print_summary(result: CleanupResult, dry_run: bool):
    """Print cleanup summary."""
    print("\n" + "=" * 60)
    print("UNUSED TYPEDEF CLEANUP SUMMARY")
    print("=" * 60)

    if dry_run:
        print("(DRY RUN - no files were modified)")

    print(f"\nTypedefs {'to be ' if dry_run else ''}removed: {len(result.typedefs_removed)}")
    print(f"Files {'to be ' if dry_run else ''}modified: {len(result.files_modified)}")

    if result.typedefs_removed:
        print(f"\nRemoved typedefs (first 20):")
        for td in result.typedefs_removed[:20]:
            line_info = f"{td['start_line']}"
            if td['end_line'] != td['start_line']:
                line_info += f"-{td['end_line']}"
            print(f"  {td['name']} in {td['file']}:{line_info}")
        if len(result.typedefs_removed) > 20:
            print(f"  ... and {len(result.typedefs_removed) - 20} more")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Unused Typedef Cleanup Script'
    )
    parser.add_argument(
        'src_directory',
        nargs='?',
        default='.',
        help='Source directory (default: current directory)'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Preview changes without modifying files'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show detailed progress'
    )
    parser.add_argument(
        '--output', '-o',
        default=None,
        help='Output JSON file for results'
    )

    args = parser.parse_args()

    print("OCCT Unused Typedef Cleaner")
    print("=" * 60)

    cleaner = UnusedTypedefCleaner(
        src_dir=args.src_directory,
        dry_run=args.dry_run,
        verbose=args.verbose
    )

    result = cleaner.run()

    # Print summary
    print_summary(result, args.dry_run)

    # Save results
    script_dir = Path(__file__).parent
    output_file = args.output or (script_dir / 'unused_typedefs.json')
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump({
            'typedefs_removed': result.typedefs_removed,
            'files_modified': result.files_modified,
            'errors': result.errors,
            'dry_run': args.dry_run
        }, f, indent=2, ensure_ascii=False)
    print(f"\nResults saved to: {output_file}")

    if args.dry_run:
        print("\nRun without --dry-run to apply changes")


if __name__ == '__main__':
    main()
