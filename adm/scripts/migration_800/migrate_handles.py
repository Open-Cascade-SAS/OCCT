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
Handle Migration Script for OCCT 8.0.0

Transformations:
1. Handle(ClassName) -> occ::handle<ClassName>
2. Handle(ClassName)::DownCast(x) -> occ::down_cast<ClassName>(x)

Usage:
    python3 migrate_handles.py [options] <src_directory>

Options:
    --dry-run           Preview changes without modifying files
    --verbose           Show detailed progress
    --downcast-only     Only migrate DownCast patterns
    --handle-only       Only migrate Handle patterns (not DownCast)
    --file=PATH         Process only a specific file
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import List, Tuple, Set, Optional
from dataclasses import dataclass, field


@dataclass
class MigrationResult:
    """Result of processing a single file."""
    file_path: str
    handle_count: int = 0
    downcast_count: int = 0
    modified: bool = False
    error: Optional[str] = None


class HandleMigrator:
    """Specialized migrator for Handle patterns."""

    # Files to skip (core infrastructure)
    SKIP_FILES = {
        'Standard_Handle.hxx',
        'Standard_TypeDef.hxx',
        'Standard_Transient.hxx',
        'Standard_Transient.cxx',
        'Standard_Std.hxx',
        'occ.hxx',
    }

    def __init__(self, src_dir: str, dry_run: bool = False, verbose: bool = False):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.verbose = verbose
        self.results: List[MigrationResult] = []

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def should_skip(self, file_path: Path) -> bool:
        """Check if file should be skipped."""
        return file_path.name in self.SKIP_FILES

    def read_file(self, file_path: Path) -> str:
        """Read file content."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                return f.read()
        except UnicodeDecodeError:
            with open(file_path, 'r', encoding='latin-1') as f:
                return f.read()

    def write_file(self, file_path: Path, content: str):
        """Write content to file."""
        if not self.dry_run:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)

    def migrate_downcast(self, content: str) -> Tuple[str, int]:
        """
        Replace Handle(ClassName)::DownCast(...) with occ::down_cast<ClassName>(...).

        Handles various patterns:
        - Handle(Geom_Circle)::DownCast(aCurve)
        - Handle(Geom_Circle)::DownCast(GetCurve())
        - Handle(Geom_Circle)::DownCast(aHandle.get())
        """
        count = 0

        # Pattern matches Handle(ClassName)::DownCast(
        # ClassName: starts with uppercase, contains alphanumeric and underscore
        pattern = r'\bHandle\(([A-Z][a-zA-Z0-9_]*)\)::DownCast\('

        def replacer(match):
            nonlocal count
            class_name = match.group(1)
            count += 1
            return f'occ::down_cast<{class_name}>('

        new_content = re.sub(pattern, replacer, content)
        return new_content, count

    def migrate_handle(self, content: str) -> Tuple[str, int]:
        """
        Replace Handle(ClassName) with occ::handle<ClassName>.

        This should be called AFTER migrate_downcast to avoid double-processing.

        Handles patterns:
        - Handle(Geom_Circle) aCircle
        - const Handle(Geom_Circle)& theCircle
        - Handle(Geom_Circle) Method()
        - NCollection_List<Handle(Geom_Curve)>
        """
        count = 0

        # Pattern matches Handle(ClassName) NOT followed by ::DownCast
        # Uses negative lookahead (?!::DownCast)
        pattern = r'\bHandle\(([A-Z][a-zA-Z0-9_]*)\)(?!::DownCast)'

        def replacer(match):
            nonlocal count
            class_name = match.group(1)
            count += 1
            return f'occ::handle<{class_name}>'

        new_content = re.sub(pattern, replacer, content)
        return new_content, count

    def process_file(self, file_path: Path, handle_only: bool = False,
                     downcast_only: bool = False) -> MigrationResult:
        """Process a single file."""
        result = MigrationResult(file_path=str(file_path))

        if self.should_skip(file_path):
            self.log(f"Skipping: {file_path}")
            return result

        self.log(f"Processing: {file_path}")

        try:
            content = self.read_file(file_path)
            original = content

            # Process DownCast FIRST (before Handle)
            if not handle_only:
                content, result.downcast_count = self.migrate_downcast(content)
                if result.downcast_count > 0:
                    self.log(f"  DownCast replacements: {result.downcast_count}")

            # Then process Handle patterns
            if not downcast_only:
                content, result.handle_count = self.migrate_handle(content)
                if result.handle_count > 0:
                    self.log(f"  Handle replacements: {result.handle_count}")

            if content != original:
                result.modified = True
                self.write_file(file_path, content)

        except Exception as e:
            result.error = str(e)
            print(f"Error processing {file_path}: {e}", file=sys.stderr)

        return result

    def get_source_files(self) -> List[Path]:
        """Get all source files."""
        files = []
        for ext in ('*.hxx', '*.cxx', '*.lxx', '*.pxx', '*.gxx', '*.h', '*.c', '*.mm'):
            files.extend(self.src_dir.rglob(ext))
        return sorted(files)

    def run(self, handle_only: bool = False, downcast_only: bool = False,
            single_file: Optional[str] = None):
        """Run the migration."""
        if single_file:
            files = [Path(single_file)]
        else:
            files = self.get_source_files()

        print(f"Processing {len(files)} files...")
        print(f"Dry run: {self.dry_run}")

        total_handles = 0
        total_downcasts = 0
        modified_files = 0

        for i, file_path in enumerate(files, 1):
            if i % 100 == 0:
                print(f"Progress: {i}/{len(files)}")

            result = self.process_file(file_path, handle_only, downcast_only)
            self.results.append(result)

            total_handles += result.handle_count
            total_downcasts += result.downcast_count
            if result.modified:
                modified_files += 1

        # Print summary
        print("\n" + "=" * 50)
        print("Migration Summary")
        print("=" * 50)
        print(f"Files processed:      {len(files)}")
        print(f"Files modified:       {modified_files}")
        print(f"Handle replacements:  {total_handles}")
        print(f"DownCast replacements:{total_downcasts}")
        print("=" * 50)

        errors = [r for r in self.results if r.error]
        if errors:
            print(f"\nErrors ({len(errors)}):")
            for r in errors[:10]:
                print(f"  {r.file_path}: {r.error}")


def main():
    parser = argparse.ArgumentParser(description='OCCT Handle Migration Script')
    parser.add_argument('src_directory', nargs='?', default='.',
                        help='Source directory to process')
    parser.add_argument('--dry-run', action='store_true',
                        help='Preview changes without modifying files')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Show detailed progress')
    parser.add_argument('--downcast-only', action='store_true',
                        help='Only migrate DownCast patterns')
    parser.add_argument('--handle-only', action='store_true',
                        help='Only migrate Handle patterns')
    parser.add_argument('--file', dest='single_file',
                        help='Process only a specific file')

    args = parser.parse_args()

    if args.handle_only and args.downcast_only:
        print("Error: Cannot specify both --handle-only and --downcast-only")
        sys.exit(1)

    migrator = HandleMigrator(
        src_dir=args.src_directory,
        dry_run=args.dry_run,
        verbose=args.verbose
    )

    migrator.run(
        handle_only=args.handle_only,
        downcast_only=args.downcast_only,
        single_file=args.single_file
    )


if __name__ == '__main__':
    main()
