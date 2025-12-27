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
OCCT 8.0.0 Deprecated Typedef Cleanup Script

Finds and removes deprecated typedef/using declarations marked with Standard_DEPRECATED.
Replaces all usages of the deprecated name with the original type.
Files that become include-only after cleanup are marked for processing by
cleanup_forwarding_headers.py.

Usage:
    python3 cleanup_deprecated_typedefs.py [options] <src_directory>

Options:
    --dry-run           Preview changes without modifying files
    --verbose           Show detailed progress
    --jobs N            Number of parallel jobs (default: 4)
    --output FILE       Output JSON file with results
"""

import os
import re
import json
import argparse
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from dataclasses import dataclass, field
from concurrent.futures import ProcessPoolExecutor, as_completed

# Global state for parallel workers
TYPEDEF_MAP: Dict[str, str] = {}  # deprecated_name -> original_type
TYPEDEF_PATTERN: Optional[re.Pattern] = None
DRY_RUN: bool = False

SKIP_DIRS = {'build', 'install', '.git', '__pycache__', 'mac64', 'win64', 'lin64'}
SOURCE_EXTENSIONS = {'.cxx', '.hxx', '.pxx', '.lxx', '.gxx', '.h', '.cpp', '.c'}
HEADER_EXTENSIONS = {'.hxx', '.h', '.pxx', '.lxx', '.gxx'}

# Core files to exclude from modification
EXCLUDED_FILES = {
    'Standard_Handle.hxx',
    'Standard_Macro.hxx',
    'Standard_TypeDef.hxx',
}

@dataclass
class DeprecatedTypedef:
    """Represents a deprecated typedef or using declaration."""
    deprecated_name: str          # The name being deprecated (e.g., PrsMgr_PresentationManager3d)
    original_type: str            # The type it aliases (e.g., PrsMgr_PresentationManager)
    header_file: str              # Header file containing the typedef
    line_numbers: List[int]       # Line numbers (deprecation + typedef)
    is_handle: bool = False       # Whether it's a Handle_ typedef
    declaration_type: str = 'typedef'  # 'typedef' or 'using'


@dataclass
class CleanupResult:
    """Result of cleanup operations."""
    typedefs_found: List[DeprecatedTypedef] = field(default_factory=list)
    files_modified: int = 0
    total_replacements: int = 0
    typedef_lines_removed: int = 0
    include_only_headers: List[str] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)


def init_worker(typedef_map: Dict[str, str], pattern_str: str, dry_run: bool):
    """Initialize worker with shared data."""
    global TYPEDEF_MAP, TYPEDEF_PATTERN, DRY_RUN
    TYPEDEF_MAP = typedef_map
    TYPEDEF_PATTERN = re.compile(pattern_str)
    DRY_RUN = dry_run


def process_file_replacements(file_path_str: str) -> Dict:
    """Process a single file - replace deprecated typedef usages."""
    result = {
        'file_path': file_path_str,
        'replacements': {},
        'modified': False,
        'error': None
    }

    # Skip excluded files
    if os.path.basename(file_path_str) in EXCLUDED_FILES:
        return result

    try:
        with open(file_path_str, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()
    except Exception as e:
        result['error'] = str(e)
        return result

    original = content

    # Replace all deprecated typedef usages
    for match in TYPEDEF_PATTERN.finditer(content):
        deprecated_name = match.group(0)
        if deprecated_name in TYPEDEF_MAP:
            original_type = TYPEDEF_MAP[deprecated_name]
            content = re.sub(r'\b' + re.escape(deprecated_name) + r'\b',
                           original_type, content)
            result['replacements'][deprecated_name] = \
                result['replacements'].get(deprecated_name, 0) + 1

    if content != original:
        result['modified'] = True
        if not DRY_RUN:
            with open(file_path_str, 'w', encoding='utf-8') as f:
                f.write(content)

    return result


def get_source_files(src_dir: Path) -> List[str]:
    """Get all source files to process."""
    files = []
    for root, dirs, filenames in os.walk(src_dir):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for filename in filenames:
            if any(filename.endswith(ext) for ext in SOURCE_EXTENSIONS):
                files.append(str(Path(root) / filename))
    return files


def get_header_files(src_dir: Path) -> List[str]:
    """Get all header files to scan for deprecated typedefs."""
    files = []
    for root, dirs, filenames in os.walk(src_dir):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for filename in filenames:
            if any(filename.endswith(ext) for ext in HEADER_EXTENSIONS):
                if filename not in EXCLUDED_FILES:
                    files.append(str(Path(root) / filename))
    return files


class DeprecatedTypedefCollector:
    """Collects deprecated typedefs from header files."""

    # Pattern for Standard_DEPRECATED followed IMMEDIATELY by typedef on next line
    # Uses non-greedy matching and explicit line structure
    # Matches: Standard_DEPRECATED("message" "continuation")\n  typedef Type Name;
    DEPRECATED_TYPEDEF_PATTERN = re.compile(
        r'^(\s*)Standard_DEPRECATED\s*\(\s*"[^"]*"(?:\s*\n?\s*"[^"]*")*\s*\)\s*\n'
        r'\1\s*typedef\s+(\S+)\s+(\w+)\s*;',
        re.MULTILINE
    )

    # Pattern for Standard_DEPRECATED followed IMMEDIATELY by using on next line
    DEPRECATED_USING_PATTERN = re.compile(
        r'^(\s*)Standard_DEPRECATED\s*\(\s*"[^"]*"(?:\s*\n?\s*"[^"]*")*\s*\)\s*\n'
        r'\1\s*using\s+(\w+)\s*=\s*([^;]+);',
        re.MULTILINE
    )

    def __init__(self, src_dir: Path, verbose: bool = False):
        self.src_dir = src_dir
        self.verbose = verbose
        self.typedefs: List[DeprecatedTypedef] = []

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def collect(self) -> List[DeprecatedTypedef]:
        """Collect all deprecated typedefs from headers."""
        header_files = get_header_files(self.src_dir)
        print(f"Scanning {len(header_files)} header files for deprecated typedefs...")

        for header_path in header_files:
            self._scan_header(header_path)

        print(f"Found {len(self.typedefs)} deprecated typedefs")
        return self.typedefs

    def _scan_header(self, header_path: str):
        """Scan a single header for deprecated typedefs."""
        try:
            with open(header_path, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
        except Exception as e:
            self.log(f"Error reading {header_path}: {e}")
            return

        rel_path = str(Path(header_path).relative_to(self.src_dir))

        # Find deprecated typedefs
        for match in self.DEPRECATED_TYPEDEF_PATTERN.finditer(content):
            indent = match.group(1)  # Captured indentation
            original_type = match.group(2)
            deprecated_name = match.group(3)

            # Skip typedefs inside classes/namespaces (they have leading whitespace)
            # The pattern captures indentation in group 1
            if indent:
                self.log(f"  Skipping class-local typedef: {deprecated_name} in {rel_path}")
                continue

            # Calculate line numbers
            start_pos = match.start()
            line_start = content.count('\n', 0, start_pos) + 1
            line_end = content.count('\n', 0, match.end()) + 1

            is_handle = deprecated_name.startswith('Handle_') or \
                       original_type.startswith('Handle_')

            typedef = DeprecatedTypedef(
                deprecated_name=deprecated_name,
                original_type=original_type,
                header_file=rel_path,
                line_numbers=list(range(line_start, line_end + 1)),
                is_handle=is_handle,
                declaration_type='typedef'
            )
            self.typedefs.append(typedef)
            self.log(f"  Found typedef: {deprecated_name} -> {original_type} in {rel_path}")

        # Find deprecated using declarations
        for match in self.DEPRECATED_USING_PATTERN.finditer(content):
            indent = match.group(1)  # Captured indentation
            deprecated_name = match.group(2)
            original_type = match.group(3).strip()

            # Skip usings inside classes/namespaces (they have leading whitespace)
            # The pattern captures indentation in group 1
            if indent:
                self.log(f"  Skipping class-local using: {deprecated_name} in {rel_path}")
                continue

            start_pos = match.start()
            line_start = content.count('\n', 0, start_pos) + 1
            line_end = content.count('\n', 0, match.end()) + 1

            is_handle = deprecated_name.startswith('Handle_') or \
                       original_type.startswith('Handle_')

            typedef = DeprecatedTypedef(
                deprecated_name=deprecated_name,
                original_type=original_type,
                header_file=rel_path,
                line_numbers=list(range(line_start, line_end + 1)),
                is_handle=is_handle,
                declaration_type='using'
            )
            self.typedefs.append(typedef)
            self.log(f"  Found using: {deprecated_name} -> {original_type} in {rel_path}")


class DeprecatedTypedefCleaner:
    """Removes deprecated typedefs and replaces their usages."""

    def __init__(self, src_dir: Path, typedefs: List[DeprecatedTypedef],
                 dry_run: bool = False, verbose: bool = False, jobs: int = 4):
        self.src_dir = src_dir
        self.typedefs = typedefs
        self.dry_run = dry_run
        self.verbose = verbose
        self.jobs = jobs

        # Build typedef map for replacements
        self.typedef_map: Dict[str, str] = {}
        for td in typedefs:
            self.typedef_map[td.deprecated_name] = td.original_type

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def replace_usages(self) -> Tuple[int, Dict[str, int]]:
        """Replace all usages of deprecated typedefs."""
        if not self.typedef_map:
            return 0, {}

        print(f"\nReplacing {len(self.typedef_map)} deprecated typedef usages...")

        # Build pattern for all deprecated names
        names = sorted(self.typedef_map.keys(), key=len, reverse=True)
        pattern_str = r'\b(' + '|'.join(re.escape(n) for n in names) + r')\b'

        files = get_source_files(self.src_dir)
        print(f"Processing {len(files)} files...")

        total_replacements: Dict[str, int] = {}
        files_modified = 0
        processed = 0

        with ProcessPoolExecutor(
            max_workers=self.jobs,
            initializer=init_worker,
            initargs=(self.typedef_map, pattern_str, self.dry_run)
        ) as executor:
            futures = {executor.submit(process_file_replacements, f): f for f in files}

            for future in as_completed(futures):
                processed += 1
                if processed % 2000 == 0:
                    print(f"Progress: {processed}/{len(files)}")

                try:
                    result = future.result()
                    if result['modified']:
                        files_modified += 1
                        for name, count in result['replacements'].items():
                            total_replacements[name] = \
                                total_replacements.get(name, 0) + count
                        self.log(f"  Modified: {result['file_path']}")
                except Exception as e:
                    print(f"Error: {futures[future]}: {e}")

        total = sum(total_replacements.values())
        print(f"Replaced {total} usages in {files_modified} files")
        return files_modified, total_replacements

    def remove_typedef_lines(self) -> Tuple[int, List[str]]:
        """Remove deprecated typedef lines from headers."""
        print(f"\nRemoving deprecated typedef lines...")

        # Group typedefs by header
        typedefs_by_header: Dict[str, List[DeprecatedTypedef]] = {}
        for td in self.typedefs:
            if td.header_file not in typedefs_by_header:
                typedefs_by_header[td.header_file] = []
            typedefs_by_header[td.header_file].append(td)

        lines_removed = 0
        include_only_headers = []

        for header_path, header_typedefs in typedefs_by_header.items():
            full_path = self.src_dir / header_path

            try:
                with open(full_path, 'r', encoding='utf-8', errors='replace') as f:
                    content = f.read()
            except Exception as e:
                print(f"Error reading {header_path}: {e}")
                continue

            original = content

            # Remove each deprecated typedef block
            for td in header_typedefs:
                # Pattern to match the full deprecated typedef block
                # including Standard_WarningsDisable/Restore if present
                patterns = [
                    # With warnings disable/restore wrapper
                    (r'#include\s*<Standard_WarningsDisable\.hxx>\s*\n'
                     r'Standard_DEPRECATED\s*\([^)]*\)\s*\n'
                     r'\s*typedef\s+' + re.escape(td.original_type) + r'\s+' +
                     re.escape(td.deprecated_name) + r'\s*;\s*\n'
                     r'#include\s*<Standard_WarningsRestore\.hxx>'),
                    # Without wrapper - multi-line deprecation message
                    (r'Standard_DEPRECATED\s*\(\s*"[^"]*(?:"\s*\n\s*"[^"]*)*"\s*\)\s*\n'
                     r'\s*typedef\s+' + re.escape(td.original_type) + r'\s+' +
                     re.escape(td.deprecated_name) + r'\s*;'),
                    # Simple single-line deprecation
                    (r'Standard_DEPRECATED\s*\([^)]*\)\s*\n'
                     r'\s*typedef\s+' + re.escape(td.original_type) + r'\s+' +
                     re.escape(td.deprecated_name) + r'\s*;'),
                    # Using declaration with wrapper
                    (r'#include\s*<Standard_WarningsDisable\.hxx>\s*\n'
                     r'Standard_DEPRECATED\s*\([^)]*\)\s*\n'
                     r'\s*using\s+' + re.escape(td.deprecated_name) + r'\s*=[^;]+;\s*\n'
                     r'#include\s*<Standard_WarningsRestore\.hxx>'),
                    # Using declaration without wrapper
                    (r'Standard_DEPRECATED\s*\([^)]*\)\s*\n'
                     r'\s*using\s+' + re.escape(td.deprecated_name) + r'\s*=[^;]+;'),
                ]

                for pattern in patterns:
                    new_content = re.sub(pattern, '', content, flags=re.MULTILINE)
                    if new_content != content:
                        lines_removed += content.count('\n') - new_content.count('\n')
                        content = new_content
                        break

            # Clean up multiple blank lines
            content = re.sub(r'\n{3,}', '\n\n', content)

            if content != original:
                self.log(f"  Cleaned: {header_path}")

                # Check if file is now include-only
                if self._is_include_only(content):
                    include_only_headers.append(header_path)
                    self.log(f"    -> Now include-only")

                if not self.dry_run:
                    with open(full_path, 'w', encoding='utf-8') as f:
                        f.write(content)

        print(f"Removed {lines_removed} lines from {len(typedefs_by_header)} headers")
        if include_only_headers:
            print(f"Headers now include-only: {len(include_only_headers)}")

        return lines_removed, include_only_headers

    def _is_include_only(self, content: str) -> bool:
        """Check if content contains only includes, comments, and preprocessor guards."""
        lines = content.split('\n')

        for line in lines:
            stripped = line.strip()

            # Skip empty lines
            if not stripped:
                continue

            # Skip comments
            if stripped.startswith('//') or stripped.startswith('/*') or \
               stripped.startswith('*') or stripped.endswith('*/'):
                continue

            # Skip preprocessor directives
            if stripped.startswith('#'):
                continue

            # Any other content means not include-only
            return False

        return True

    def run(self) -> CleanupResult:
        """Run the full cleanup process."""
        result = CleanupResult()
        result.typedefs_found = self.typedefs

        # Remove typedef lines FIRST, before replacing usages
        # This prevents the deprecated name from being replaced in the typedef
        # declaration itself, which would make the removal pattern fail to match
        lines_removed, include_only = self.remove_typedef_lines()
        result.typedef_lines_removed = lines_removed
        result.include_only_headers = include_only

        # Replace usages in remaining code
        files_modified, replacements = self.replace_usages()
        result.files_modified = files_modified
        result.total_replacements = sum(replacements.values())

        return result


def print_summary(result: CleanupResult, dry_run: bool):
    """Print cleanup summary."""
    print("\n" + "=" * 60)
    print("DEPRECATED TYPEDEF CLEANUP SUMMARY")
    print("=" * 60)

    if dry_run:
        print("(DRY RUN - no files were modified)")

    print(f"\nDeprecated typedefs found: {len(result.typedefs_found)}")
    print(f"Files modified: {result.files_modified}")
    print(f"Total replacements: {result.total_replacements}")
    print(f"Typedef lines removed: {result.typedef_lines_removed}")

    if result.include_only_headers:
        print(f"\nHeaders now include-only ({len(result.include_only_headers)}):")
        for header in result.include_only_headers[:10]:
            print(f"  {header}")
        if len(result.include_only_headers) > 10:
            print(f"  ... and {len(result.include_only_headers) - 10} more")

    if result.typedefs_found:
        print(f"\nDeprecated typedefs processed (first 20):")
        for td in result.typedefs_found[:20]:
            print(f"  {td.deprecated_name} -> {td.original_type}")
        if len(result.typedefs_found) > 20:
            print(f"  ... and {len(result.typedefs_found) - 20} more")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Deprecated Typedef Cleanup Script'
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
        '--jobs', '-j',
        type=int,
        default=4,
        help='Number of parallel jobs (default: 4)'
    )
    parser.add_argument(
        '--output', '-o',
        default=None,
        help='Output JSON file with results'
    )

    args = parser.parse_args()

    print("OCCT Deprecated Typedef Cleanup")
    print("=" * 60)

    src_dir = Path(args.src_directory)
    if not src_dir.exists():
        print(f"Error: {src_dir} does not exist")
        return 1

    # Collect deprecated typedefs
    collector = DeprecatedTypedefCollector(src_dir, verbose=args.verbose)
    typedefs = collector.collect()

    if not typedefs:
        print("\nNo deprecated typedefs found.")
        return 0

    # Clean up
    cleaner = DeprecatedTypedefCleaner(
        src_dir=src_dir,
        typedefs=typedefs,
        dry_run=args.dry_run,
        verbose=args.verbose,
        jobs=args.jobs
    )

    result = cleaner.run()

    # Print summary
    print_summary(result, args.dry_run)

    # Save results - main results file
    script_dir = Path(__file__).parent
    if args.output:
        output_path = Path(args.output)
    else:
        output_path = script_dir / 'deprecated_typedef_results.json'

    results_dict = {
        'typedefs_found': [
            {
                'deprecated_name': td.deprecated_name,
                'original_type': td.original_type,
                'header_file': td.header_file,
                'is_handle': td.is_handle
            }
            for td in result.typedefs_found
        ],
        'files_modified': result.files_modified,
        'total_replacements': result.total_replacements,
        'typedef_lines_removed': result.typedef_lines_removed,
        'include_only_headers': result.include_only_headers,
        'dry_run': args.dry_run
    }

    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(results_dict, f, indent=2, ensure_ascii=False)
    print(f"\nResults saved to: {output_path}")

    # Save collected deprecated typedefs for external project reuse
    # Format compatible with replace_typedefs.py
    collected_path = script_dir / 'collected_deprecated_typedefs.json'
    collected_dict = {
        'description': 'Deprecated typedef/using declarations from OCCT - for migration of external projects',
        'typedefs': [
            {
                'typedef_name': td.deprecated_name,
                'replacement_type': td.original_type,
                'header_file': td.header_file,
                'is_handle': td.is_handle,
                'declaration_type': td.declaration_type
            }
            for td in result.typedefs_found
        ],
        'typedef_only_headers': result.include_only_headers
    }

    with open(collected_path, 'w', encoding='utf-8') as f:
        json.dump(collected_dict, f, indent=2, ensure_ascii=False)
    print(f"Collected typedefs saved to: {collected_path}")

    if args.dry_run:
        print("\nRun without --dry-run to apply changes")
    else:
        print("\nDeprecated typedef cleanup complete!")
        if result.include_only_headers:
            print("Run cleanup_forwarding_headers.py to process include-only headers.")


if __name__ == '__main__':
    main()
