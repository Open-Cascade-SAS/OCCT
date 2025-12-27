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
OCCT 8.0 Modernization Script - DEFINE_STANDARD_HANDLE Cleanup

This script removes DEFINE_STANDARD_HANDLE macro calls which only create
deprecated Handle_ClassName typedefs (not needed with modern occ::handle<T>).

Two cases are handled:
1. Forward declaration immediately followed by DEFINE_STANDARD_HANDLE:
   Both lines are removed (the forward decl was only needed for the macro)

    Before:                                    After:
        class MyClass;                         (both lines removed)
        DEFINE_STANDARD_HANDLE(MyClass, Base)

2. DEFINE_STANDARD_HANDLE without adjacent forward declaration:
   Only the macro line is removed

    Before:                                    After:
        ...                                    ...
        DEFINE_STANDARD_HANDLE(MyClass, Base)  (line removed)

Usage:
    python cleanup_define_handle.py [options] <path>

Options:
    --dry-run       Preview changes without modifying files
    --verbose       Show detailed progress
    --backup        Create backup files before modification
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import Dict, List, Tuple
from dataclasses import dataclass


@dataclass
class CleanupResult:
    """Result of cleaning up a file."""
    file_path: str
    patterns_removed: int
    adjacent_pairs_removed: int
    classes_affected: List[str]


# Pattern 1: Forward declaration immediately followed by DEFINE_STANDARD_HANDLE
# Both lines will be removed
ADJACENT_PATTERN = re.compile(
    r'^class\s+([A-Z][a-zA-Z0-9_]*)\s*;\s*\n'
    r'DEFINE_STANDARD_HANDLE\s*\(\s*\1\s*,\s*[A-Z][a-zA-Z0-9_]*\s*\)\s*\n?',
    re.MULTILINE
)

# Pattern 2: Standalone DEFINE_STANDARD_HANDLE (not immediately after forward decl)
# Only the macro line is removed
STANDALONE_PATTERN = re.compile(
    r'^DEFINE_STANDARD_HANDLE\s*\(\s*([A-Z][a-zA-Z0-9_]*)\s*,\s*[A-Z][a-zA-Z0-9_]*\s*\)\s*\n?',
    re.MULTILINE
)

# Files to exclude from migration (core infrastructure files)
EXCLUDED_FILES = {
    'Standard_Handle.hxx',
    'Standard_Type.hxx',
    'Standard_TypeDef.hxx',
    'Standard_Transient.hxx',
    'Standard_Macro.hxx',
    'Standard_DefineHandle.hxx',
}

# Directories to exclude
EXCLUDED_DIRS = {
    '.git',
    'build',
    '__pycache__',
}


class DefineHandleCleaner:
    """Handles cleanup of redundant DEFINE_STANDARD_HANDLE patterns."""

    def __init__(self, dry_run: bool = False, verbose: bool = False, backup: bool = False):
        self.dry_run = dry_run
        self.verbose = verbose
        self.backup = backup
        self.files_scanned = 0
        self.files_modified = 0
        self.total_patterns_removed = 0
        self.results: List[CleanupResult] = []

    def should_process_file(self, filepath: Path) -> bool:
        """Check if file should be processed."""
        # Check extension
        if filepath.suffix not in ('.hxx', '.cxx', '.lxx', '.pxx', '.gxx', '.h', '.c', '.mm'):
            return False

        # Check excluded files
        if filepath.name in EXCLUDED_FILES:
            return False

        # Check excluded directories
        for part in filepath.parts:
            if part in EXCLUDED_DIRS:
                return False

        return True

    def process_content(self, content: str) -> Tuple[str, List[str], int]:
        """Process content and return modified content with list of affected classes.

        Returns:
            Tuple of (modified_content, affected_classes, adjacent_pairs_count)
        """
        affected_classes: List[str] = []
        adjacent_count = 0
        modified = content

        # Step 1: Find and remove adjacent pairs (forward decl + macro)
        for match in ADJACENT_PATTERN.finditer(content):
            class_name = match.group(1)
            affected_classes.append(f"{class_name} (with forward decl)")
            adjacent_count += 1

        # Remove adjacent pairs - both forward declaration and macro
        modified = ADJACENT_PATTERN.sub('', modified)

        # Step 2: Find and remove standalone DEFINE_STANDARD_HANDLE
        for match in STANDALONE_PATTERN.finditer(modified):
            class_name = match.group(1)
            affected_classes.append(class_name)

        # Remove standalone macros
        modified = STANDALONE_PATTERN.sub('', modified)

        return modified, affected_classes, adjacent_count

    def process_file(self, filepath: Path) -> bool:
        """Process a single file. Returns True if file was modified."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
        except IOError as e:
            print(f"Error reading {filepath}: {e}", file=sys.stderr)
            return False

        self.files_scanned += 1
        modified, affected_classes, adjacent_count = self.process_content(content)

        if not affected_classes:
            return False

        patterns_removed = len(affected_classes)
        self.total_patterns_removed += patterns_removed

        result = CleanupResult(
            file_path=str(filepath),
            patterns_removed=patterns_removed,
            adjacent_pairs_removed=adjacent_count,
            classes_affected=affected_classes
        )
        self.results.append(result)

        if self.verbose:
            print(f"\n{filepath}:")
            print(f"  Removed {patterns_removed} DEFINE_STANDARD_HANDLE pattern(s)")
            if adjacent_count > 0:
                print(f"    ({adjacent_count} with adjacent forward declarations)")
            for cls in affected_classes[:5]:
                print(f"    - {cls}")
            if len(affected_classes) > 5:
                print(f"    ... and {len(affected_classes) - 5} more")

        if not self.dry_run:
            if self.backup:
                backup_path = filepath.with_suffix(filepath.suffix + '.bak')
                try:
                    with open(backup_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                except IOError as e:
                    print(f"Error creating backup {backup_path}: {e}", file=sys.stderr)

            try:
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(modified)
            except IOError as e:
                print(f"Error writing {filepath}: {e}", file=sys.stderr)
                return False

        self.files_modified += 1
        return True

    def process_directory(self, dirpath: Path) -> None:
        """Recursively process all files in directory."""
        for root, dirs, files in os.walk(dirpath):
            # Filter out excluded directories
            dirs[:] = [d for d in dirs if d not in EXCLUDED_DIRS]

            for filename in files:
                filepath = Path(root) / filename
                if self.should_process_file(filepath):
                    self.process_file(filepath)

    def print_summary(self) -> None:
        """Print cleanup summary."""
        print("\n" + "=" * 60)
        print("DEFINE_STANDARD_HANDLE CLEANUP SUMMARY")
        print("=" * 60)

        if self.dry_run:
            print("(DRY RUN - no files were modified)")

        print(f"\nFiles scanned: {self.files_scanned}")
        print(f"Files {'to be ' if self.dry_run else ''}modified: {self.files_modified}")
        print(f"Total patterns {'to be ' if self.dry_run else ''}removed: {self.total_patterns_removed}")

        if self.verbose and self.results:
            print("\n" + "-" * 60)
            print("Files with changes:")
            print("-" * 60)
            for result in self.results[:20]:
                print(f"  {result.file_path}: {result.patterns_removed} pattern(s)")
            if len(self.results) > 20:
                print(f"  ... and {len(self.results) - 20} more files")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0 Modernization - DEFINE_STANDARD_HANDLE Cleanup'
    )
    parser.add_argument(
        'path',
        type=str,
        help='Path to source directory or file'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Preview changes without modifying files'
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Show detailed progress'
    )
    parser.add_argument(
        '--backup',
        action='store_true',
        help='Create backup files before modification'
    )

    args = parser.parse_args()
    path = Path(args.path)

    if not path.exists():
        print(f"Error: Path does not exist: {path}", file=sys.stderr)
        sys.exit(1)

    cleaner = DefineHandleCleaner(
        dry_run=args.dry_run,
        verbose=args.verbose,
        backup=args.backup
    )

    print(f"Processing: {path}")
    if args.dry_run:
        print("(DRY RUN - no files will be modified)")

    if path.is_file():
        cleaner.process_file(path)
    else:
        cleaner.process_directory(path)

    cleaner.print_summary()


if __name__ == '__main__':
    main()
