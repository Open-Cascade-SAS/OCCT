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
OCCT 8.0 Modernization Script - Standard_* Macro Migration

This script replaces deprecated Standard_* macros with their C++ equivalents:
- Standard_OVERRIDE -> override
- Standard_NODISCARD -> [[nodiscard]]
- Standard_FALLTHROUGH -> [[fallthrough]];
- Standard_Noexcept -> noexcept
- Standard_DELETE -> = delete
- Standard_THREADLOCAL -> thread_local
- Standard_ATOMIC(T) -> std::atomic<T>
- Standard_UNUSED -> [[maybe_unused]]

Usage:
    python migrate_macros.py [options] <path>

Options:
    --dry-run       Preview changes without modifying files
    --verbose       Show detailed progress
    --backup        Create backup files before modification
    --verify        Only verify remaining patterns (no changes)
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass


@dataclass
class MacroReplacement:
    """Defines a macro replacement rule."""
    pattern: re.Pattern
    replacement: str
    description: str


# Define macro replacement rules
MACRO_REPLACEMENTS: List[MacroReplacement] = [
    MacroReplacement(
        pattern=re.compile(r'\bStandard_OVERRIDE\b'),
        replacement='override',
        description='Standard_OVERRIDE -> override'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_NODISCARD\b'),
        replacement='[[nodiscard]]',
        description='Standard_NODISCARD -> [[nodiscard]]'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_FALLTHROUGH\b'),
        replacement='[[fallthrough]];',
        description='Standard_FALLTHROUGH -> [[fallthrough]];'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_Noexcept\b'),
        replacement='noexcept',
        description='Standard_Noexcept -> noexcept'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_DELETE\b'),
        replacement='= delete',
        description='Standard_DELETE -> = delete'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_THREADLOCAL\b'),
        replacement='thread_local',
        description='Standard_THREADLOCAL -> thread_local'
    ),
    MacroReplacement(
        pattern=re.compile(r'\bStandard_ATOMIC\s*\(\s*([^)]+)\s*\)'),
        replacement=r'std::atomic<\1>',
        description='Standard_ATOMIC(T) -> std::atomic<T>'
    ),
    # Note: Standard_UNUSED requires manual migration due to stricter placement
    # rules of [[maybe_unused]] compared to __attribute__((unused)).
    # The script will report occurrences but not auto-replace.
]

# Patterns that require manual review (not auto-replaced)
MANUAL_REVIEW_PATTERNS: List[MacroReplacement] = [
    MacroReplacement(
        pattern=re.compile(r'\bStandard_UNUSED\b'),
        replacement='[[maybe_unused]]',
        description='Standard_UNUSED -> [[maybe_unused]] (MANUAL REVIEW REQUIRED)'
    ),
]

# Files to exclude from migration (core infrastructure files)
EXCLUDED_FILES = {
    'Standard_Macro.hxx',
    'Standard_Handle.hxx',
    'Standard_Type.hxx',
    'Standard_TypeDef.hxx',
    'Standard_Transient.hxx',
}

# Directories to exclude
EXCLUDED_DIRS = {
    '.git',
    'build',
    '__pycache__',
}


class MacroMigrator:
    """Handles migration of Standard_* macros to C++ equivalents."""

    def __init__(self, dry_run: bool = False, verbose: bool = False, backup: bool = False):
        self.dry_run = dry_run
        self.verbose = verbose
        self.backup = backup
        self.stats: Dict[str, int] = {rule.description: 0 for rule in MACRO_REPLACEMENTS}
        self.manual_review_stats: Dict[str, int] = {rule.description: 0 for rule in MANUAL_REVIEW_PATTERNS}
        self.manual_review_files: Dict[str, List[str]] = {rule.description: [] for rule in MANUAL_REVIEW_PATTERNS}
        self.files_modified = 0
        self.files_scanned = 0

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

    def process_content(self, content: str, filepath: Path) -> Tuple[str, Dict[str, int], Dict[str, int]]:
        """Process content and return modified content with stats."""
        local_stats: Dict[str, int] = {}
        manual_stats: Dict[str, int] = {}
        modified = content

        for rule in MACRO_REPLACEMENTS:
            # Count matches
            matches = len(rule.pattern.findall(modified))
            if matches > 0:
                local_stats[rule.description] = matches
                # Apply replacement
                modified = rule.pattern.sub(rule.replacement, modified)

        # Check for manual review patterns (don't replace, just report)
        for rule in MANUAL_REVIEW_PATTERNS:
            matches = len(rule.pattern.findall(content))
            if matches > 0:
                manual_stats[rule.description] = matches
                if str(filepath) not in self.manual_review_files[rule.description]:
                    self.manual_review_files[rule.description].append(str(filepath))

        return modified, local_stats, manual_stats

    def process_file(self, filepath: Path) -> bool:
        """Process a single file. Returns True if file was modified."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
        except IOError as e:
            print(f"Error reading {filepath}: {e}", file=sys.stderr)
            return False

        self.files_scanned += 1
        modified, local_stats, manual_stats = self.process_content(content, filepath)

        # Update manual review stats
        for key, count in manual_stats.items():
            self.manual_review_stats[key] += count

        if not local_stats:
            return False

        # Update global stats
        for key, count in local_stats.items():
            self.stats[key] += count

        if self.verbose:
            print(f"\n{filepath}:")
            for desc, count in local_stats.items():
                print(f"  {desc}: {count}")

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

    def verify_patterns(self, dirpath: Path) -> Dict[str, List[str]]:
        """Verify remaining patterns in directory."""
        remaining: Dict[str, List[str]] = {rule.description: [] for rule in MACRO_REPLACEMENTS}

        for root, dirs, files in os.walk(dirpath):
            dirs[:] = [d for d in dirs if d not in EXCLUDED_DIRS]

            for filename in files:
                filepath = Path(root) / filename
                if not self.should_process_file(filepath):
                    continue

                try:
                    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
                        content = f.read()
                except IOError:
                    continue

                for rule in MACRO_REPLACEMENTS:
                    if rule.pattern.search(content):
                        remaining[rule.description].append(str(filepath))

        return remaining

    def print_summary(self) -> None:
        """Print migration summary."""
        print("\n" + "=" * 60)
        print("MACRO MIGRATION SUMMARY")
        print("=" * 60)

        if self.dry_run:
            print("(DRY RUN - no files were modified)")

        print(f"\nFiles scanned: {self.files_scanned}")
        print(f"Files {'to be ' if self.dry_run else ''}modified: {self.files_modified}")

        print("\nReplacements by type:")
        total = 0
        for desc, count in self.stats.items():
            if count > 0:
                print(f"  {desc}: {count}")
                total += count

        print(f"\nTotal replacements: {total}")

        # Print manual review patterns
        manual_total = sum(self.manual_review_stats.values())
        if manual_total > 0:
            print("\n" + "-" * 60)
            print("MANUAL REVIEW REQUIRED:")
            print("-" * 60)
            for desc, count in self.manual_review_stats.items():
                if count > 0:
                    print(f"\n  {desc}: {count} occurrences")
                    files = self.manual_review_files.get(desc, [])
                    for f in files[:5]:
                        print(f"    - {f}")
                    if len(files) > 5:
                        print(f"    ... and {len(files) - 5} more files")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0 Modernization - Standard_* Macro Migration'
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
    parser.add_argument(
        '--verify',
        action='store_true',
        help='Only verify remaining patterns (no changes)'
    )

    args = parser.parse_args()
    path = Path(args.path)

    if not path.exists():
        print(f"Error: Path does not exist: {path}", file=sys.stderr)
        sys.exit(1)

    migrator = MacroMigrator(
        dry_run=args.dry_run,
        verbose=args.verbose,
        backup=args.backup
    )

    if args.verify:
        print(f"Verifying patterns in: {path}")
        remaining = migrator.verify_patterns(path)

        has_remaining = False
        for desc, files in remaining.items():
            if files:
                has_remaining = True
                print(f"\n{desc}:")
                for f in files[:10]:  # Show first 10
                    print(f"  {f}")
                if len(files) > 10:
                    print(f"  ... and {len(files) - 10} more files")

        if not has_remaining:
            print("\nNo deprecated macro patterns found!")
        sys.exit(0 if not has_remaining else 1)

    print(f"Processing: {path}")
    if args.dry_run:
        print("(DRY RUN - no files will be modified)")

    if path.is_file():
        migrator.process_file(path)
    else:
        migrator.process_directory(path)

    migrator.print_summary()


if __name__ == '__main__':
    main()
