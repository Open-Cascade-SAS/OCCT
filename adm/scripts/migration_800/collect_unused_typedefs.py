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
OCCT 8.0.0 Unused Typedef Collection Script

Scans source files and collects information about unused NCollection typedef declarations.
Outputs a JSON file that can be used by:
- apply_typedef_cleanup.py to remove unused typedefs from OCCT
- External projects to update their code (replace typedef names with actual types)

The output JSON contains:
- typedef name and replacement type for find-and-replace operations
- includes and forward declarations needed when typedef header is removed
- list of files that will be modified

Usage:
    python3 collect_unused_typedefs.py [options] <src_directory>

Options:
    --verbose    Show detailed progress
    --output     Output JSON file (default: unused_typedefs.json)
"""

import argparse
import json
import os
import re
from pathlib import Path
from typing import Dict, List, Optional, Set
from dataclasses import dataclass, field


@dataclass
class TypedefInfo:
    """Information about a typedef declaration."""
    name: str
    replacement: str  # The type that replaces this typedef
    start_line: int
    end_line: int
    content: str  # Full typedef text (may span multiple lines)
    file_path: str
    is_ncollection: bool = False


@dataclass
class TypedefOnlyFileInfo:
    """Information about a header file containing only typedef(s)."""
    file_path: str
    includes: List[str]  # #include directives (full lines)
    forward_decls: List[str]  # Forward declarations (class/struct Name;)
    typedefs: List[TypedefInfo]


@dataclass
class CollectionResult:
    """Result of collection operations."""
    unused_typedefs: List[Dict] = field(default_factory=list)
    files_to_delete: List[Dict] = field(default_factory=list)
    files_to_modify: List[Dict] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)


class UnusedTypedefCollector:
    """Collects information about unused typedef declarations."""

    # Extensions that require global usage check
    GLOBAL_CHECK_EXTENSIONS = {'.hxx', '.lxx'}

    # Extensions that require local usage check only
    LOCAL_CHECK_EXTENSIONS = {'.cxx', '.pxx', '.gxx', '.c', '.mm'}

    SKIP_DIRS = {'build', 'install', '.git', '__pycache__', 'mac64', 'win64', 'lin64'}

    def __init__(self, src_dir: str, verbose: bool = False):
        self.src_dir = Path(src_dir)
        self.verbose = verbose
        # Build global index: typedef_name -> set of files where it's used
        self.global_usage: Dict[str, Set[str]] = {}
        self.all_typedefs: List[TypedefInfo] = []
        # Store file contents for include propagation
        self.file_contents: Dict[str, str] = {}

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

    @staticmethod
    def extract_replacement_type(typedef_content: str) -> str:
        """
        Extract the replacement type from a typedef declaration.

        Example:
            typedef NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>> Select3D_IndexedMapOfEntity;
        Returns:
            NCollection_IndexedMap<occ::handle<Select3D_SensitiveEntity>>
        """
        # Normalize whitespace
        content = ' '.join(typedef_content.split())

        # Remove 'typedef ' prefix
        if content.startswith('typedef '):
            content = content[8:]

        # Find the typedef name (last identifier before semicolon)
        # We need to handle template types like NCollection_Map<Key, Value>

        # Remove trailing semicolon and whitespace
        content = content.rstrip('; \t\n')

        # The replacement type is everything except the last identifier
        # Split from the right to find the typedef name
        # Handle cases like: NCollection_DataMap<K, V> TypedefName

        # Find the last space that's not inside angle brackets
        depth = 0
        last_space_pos = -1
        for i, char in enumerate(content):
            if char == '<':
                depth += 1
            elif char == '>':
                depth -= 1
            elif char == ' ' and depth == 0:
                last_space_pos = i

        if last_space_pos > 0:
            return content[:last_space_pos].strip()

        return content

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
                    is_ncollection = bool(re.search(
                        r'typedef\s+NCollection_\w+',
                        typedef_content
                    ))

                    # Extract the replacement type
                    replacement = self.extract_replacement_type(typedef_content)

                    typedefs.append(TypedefInfo(
                        name=typedef_name,
                        replacement=replacement,
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
        for filepath in files:
            try:
                content = filepath.read_text(encoding='utf-8', errors='replace')
                rel_path = str(filepath.relative_to(self.src_dir))
                self.file_contents[rel_path] = content

                typedefs = self.find_typedefs_in_file(filepath, content)
                self.all_typedefs.extend(typedefs)
            except Exception as e:
                print(f"Error reading {filepath}: {e}")

        print(f"Found {len(self.all_typedefs)} typedefs total")

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

        for rel_path, content in self.file_contents.items():
            # Remove #include lines to avoid counting includes as usage
            content_without_includes = '\n'.join(
                line for line in content.split('\n')
                if not line.strip().startswith('#include')
            )
            for match in pattern.finditer(content_without_includes):
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
            # Exclude #include lines and the typedef definition itself
            lines = file_content.split('\n')
            content_without_typedef = '\n'.join(
                line for i, line in enumerate(lines, 1)
                if (i < typedef.start_line or i > typedef.end_line)
                and not line.strip().startswith('#include')
            )
            pattern = r'\b' + re.escape(typedef.name) + r'\b'
            return bool(re.search(pattern, content_without_typedef))
        else:
            # Local check only
            # Exclude #include lines and the typedef definition itself
            lines = file_content.split('\n')
            content_without_typedef = '\n'.join(
                line for i, line in enumerate(lines, 1)
                if (i < typedef.start_line or i > typedef.end_line)
                and not line.strip().startswith('#include')
            )
            pattern = r'\b' + re.escape(typedef.name) + r'\b'
            return bool(re.search(pattern, content_without_typedef))

    def is_typedef_only_header(self, filepath: Path, content: str,
                                typedefs: List[TypedefInfo]) -> Optional[TypedefOnlyFileInfo]:
        """
        Check if a header file contains only typedef(s), includes, and forward declarations.
        Returns TypedefOnlyFileInfo if it's a typedef-only file, None otherwise.
        """
        if filepath.suffix not in self.GLOBAL_CHECK_EXTENSIONS:
            return None

        lines = content.split('\n')
        includes: List[str] = []
        forward_decls: List[str] = []

        # Line ranges occupied by typedefs
        typedef_lines: Set[int] = set()
        for td in typedefs:
            for line_num in range(td.start_line, td.end_line + 1):
                typedef_lines.add(line_num)

        for i, line in enumerate(lines, 1):
            stripped = line.strip()

            # Skip empty lines, comments, header guards
            if not stripped:
                continue
            if stripped.startswith('//'):
                continue
            if stripped.startswith('/*') or stripped.startswith('*') or stripped.endswith('*/'):
                continue
            if stripped.startswith('#ifndef') or stripped.startswith('#define') or stripped.startswith('#endif'):
                continue
            if stripped.startswith('#pragma'):
                continue

            # Check if line is part of a typedef
            if i in typedef_lines:
                continue

            # Check for includes
            if stripped.startswith('#include'):
                includes.append(stripped)
                continue

            # Check for forward declarations (class/struct Name;)
            fwd_match = re.match(r'^(class|struct)\s+(\w+)\s*;$', stripped)
            if fwd_match:
                forward_decls.append(stripped)
                continue

            # Any other code means this is not a typedef-only file
            return None

        # Must have at least one typedef
        if not typedefs:
            return None

        try:
            rel_path = str(filepath.relative_to(self.src_dir))
        except ValueError:
            rel_path = str(filepath)

        return TypedefOnlyFileInfo(
            file_path=rel_path,
            includes=includes,
            forward_decls=forward_decls,
            typedefs=typedefs
        )

    def find_files_including(self, header_name: str) -> List[str]:
        """Find all files that include the given header."""
        # Match both #include <header> and #include "header"
        pattern = re.compile(
            r'#include\s*[<"]' + re.escape(header_name) + r'[>"]'
        )
        includers = []
        for rel_path, content in self.file_contents.items():
            if pattern.search(content):
                includers.append(rel_path)
        return includers

    def run(self) -> CollectionResult:
        """Run the collection process."""
        result = CollectionResult()

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

        # Process each file - identify unused typedefs and typedef-only files
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
                # Check if this is a typedef-only header file
                # where ALL typedefs are unused
                if len(unused_typedefs) == len(typedefs):
                    typedef_only_info = self.is_typedef_only_header(filepath, content, typedefs)
                    if typedef_only_info:
                        header_name = filepath.name
                        includers = self.find_files_including(header_name)
                        # Remove self from includers
                        includers = [inc for inc in includers if inc != rel_path]

                        self.log(f"  Typedef-only file to delete: {rel_path}")

                        # Add to files_to_delete
                        result.files_to_delete.append({
                            'file': rel_path,
                            'typedefs': [
                                {
                                    'name': t.name,
                                    'replacement': t.replacement,
                                    'content': t.content
                                }
                                for t in typedef_only_info.typedefs
                            ],
                            'includes': typedef_only_info.includes,
                            'forward_decls': typedef_only_info.forward_decls,
                            'includers': includers
                        })

                        # Add typedefs to unused list
                        for typedef in unused_typedefs:
                            result.unused_typedefs.append({
                                'name': typedef.name,
                                'replacement': typedef.replacement,
                                'file': typedef.file_path,
                                'start_line': typedef.start_line,
                                'end_line': typedef.end_line,
                                'content': typedef.content,
                                'file_will_be_deleted': True
                            })
                        continue

                # File has other content, just remove the typedef lines
                result.files_to_modify.append({
                    'file': rel_path,
                    'typedefs': [
                        {
                            'name': t.name,
                            'replacement': t.replacement,
                            'start_line': t.start_line,
                            'end_line': t.end_line,
                            'content': t.content
                        }
                        for t in unused_typedefs
                    ]
                })

                for typedef in unused_typedefs:
                    result.unused_typedefs.append({
                        'name': typedef.name,
                        'replacement': typedef.replacement,
                        'file': typedef.file_path,
                        'start_line': typedef.start_line,
                        'end_line': typedef.end_line,
                        'content': typedef.content,
                        'file_will_be_deleted': False
                    })

        return result


def print_summary(result: CollectionResult):
    """Print collection summary."""
    print("\n" + "=" * 60)
    print("UNUSED TYPEDEF COLLECTION SUMMARY")
    print("=" * 60)

    print(f"\nTotal unused typedefs found: {len(result.unused_typedefs)}")
    print(f"Typedef-only files to delete: {len(result.files_to_delete)}")
    print(f"Files to modify (remove typedefs): {len(result.files_to_modify)}")

    if result.files_to_delete:
        print(f"\nTypedef-only files to delete (first 10):")
        for fd in result.files_to_delete[:10]:
            typedef_names = [t['name'] for t in fd['typedefs']]
            print(f"  {fd['file']}")
            print(f"    Typedefs: {', '.join(typedef_names)}")
            print(f"    -> Replacement: {fd['typedefs'][0]['replacement']}")
            print(f"    Includers: {len(fd['includers'])}")
        if len(result.files_to_delete) > 10:
            print(f"  ... and {len(result.files_to_delete) - 10} more")

    if result.files_to_modify:
        print(f"\nFiles to modify (first 10):")
        for fm in result.files_to_modify[:10]:
            typedef_names = [t['name'] for t in fm['typedefs']]
            print(f"  {fm['file']}: {', '.join(typedef_names)}")
        if len(result.files_to_modify) > 10:
            print(f"  ... and {len(result.files_to_modify) - 10} more")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Unused Typedef Collection Script'
    )
    parser.add_argument(
        'src_directory',
        nargs='?',
        default='.',
        help='Source directory (default: current directory)'
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

    print("OCCT Unused Typedef Collector")
    print("=" * 60)

    collector = UnusedTypedefCollector(
        src_dir=args.src_directory,
        verbose=args.verbose
    )

    result = collector.run()

    # Print summary
    print_summary(result)

    # Save results
    script_dir = Path(__file__).parent
    output_file = args.output or (script_dir / 'unused_typedefs.json')
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump({
            'description': 'Unused NCollection typedefs in OCCT. Use replacement field to update external code.',
            'unused_typedefs': result.unused_typedefs,
            'files_to_delete': result.files_to_delete,
            'files_to_modify': result.files_to_modify,
            'errors': result.errors
        }, f, indent=2, ensure_ascii=False)
    print(f"\nResults saved to: {output_file}")
    print("\nUse apply_typedef_cleanup.py to apply changes to OCCT source")
    print("Use unused_typedefs.json to update external projects:")
    print("  - Replace typedef names with 'replacement' values")
    print("  - Add includes from 'files_to_delete[].includes'")
    print("  - Add forward declarations from 'files_to_delete[].forward_decls'")


if __name__ == '__main__':
    main()
