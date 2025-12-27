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
OCCT 8.0.0 Modernization Script

This script performs the following transformations:
1. Handle(ClassName) -> occ::handle<ClassName>
2. Handle(ClassName)::DownCast(...) -> occ::down_cast<ClassName>(...)
3. ->IsKind(STANDARD_TYPE(T)) -> occ::is_kind<T>(...)
4. ->IsInstance(STANDARD_TYPE(T)) -> occ::is_instance<T>(...)
5. Standard_Boolean -> bool
6. Standard_Integer -> int
7. Standard_Real -> double
8. Standard_True -> true
9. Standard_False -> false
10. Standard_Address -> void*
11. Standard_ShortReal -> float
12. Standard_Character -> char
13. Standard_Byte -> unsigned char

Note: Standard_*.hxx includes are NOT removed as they contain utility functions
      (Abs, IsEven, Square, RealEpsilon, etc.)

Usage:
    python3 occt_modernize.py [options] <src_directory>

Options:
    --dry-run           Preview changes without modifying files
    --backup            Create .bak backup files before modification
    --verbose           Show detailed progress
    --phase=N           Run specific phase (1=handles, 2=downcast, 3=iskind, 4=types)
    --exclude=PATTERN   Exclude files matching pattern (can be repeated)
    --verify            Check for remaining patterns after migration
    --log=FILE          Write detailed log to file
    --stats             Show statistics only, don't modify files
"""

import os
import re
import sys
import argparse
import shutil
from pathlib import Path
from typing import List, Tuple, Dict, Set, Optional
from dataclasses import dataclass, field
from datetime import datetime
import json


@dataclass
class MigrationStats:
    """Statistics for migration operations."""
    files_processed: int = 0
    files_modified: int = 0
    handle_replacements: int = 0
    downcast_replacements: int = 0
    iskind_replacements: int = 0
    isinstance_replacements: int = 0
    type_replacements: Dict[str, int] = field(default_factory=dict)
    errors: List[str] = field(default_factory=list)
    skipped_files: List[str] = field(default_factory=list)

    def to_dict(self) -> dict:
        return {
            'files_processed': self.files_processed,
            'files_modified': self.files_modified,
            'handle_replacements': self.handle_replacements,
            'downcast_replacements': self.downcast_replacements,
            'iskind_replacements': self.iskind_replacements,
            'isinstance_replacements': self.isinstance_replacements,
            'type_replacements': self.type_replacements,
            'errors': self.errors,
            'skipped_files': self.skipped_files
        }


class OCCTModernizer:
    """Main class for OCCT code modernization."""

    # Files to exclude from migration (core infrastructure)
    EXCLUDE_FILES = {
        'Standard_Handle.hxx',
        'Standard_TypeDef.hxx',
        'Standard_Transient.hxx',
        'Standard_Transient.cxx',
        'Standard_Type.hxx',
        'Standard_Type.cxx',
        'Standard_Std.hxx',
        'Standard_Boolean.hxx',
        'Standard_Integer.hxx',
        'Standard_Real.hxx',
        'Standard_Real.cxx',
        'Standard_ShortReal.hxx',
        'Standard_Character.hxx',
        'Standard_Byte.hxx',
        'Standard_PrimitiveTypes.hxx',
        'occ.hxx',
    }

    # Standard type mappings
    TYPE_MAPPINGS = {
        'Standard_Boolean': 'bool',
        'Standard_Integer': 'int',
        'Standard_Real': 'double',
        'Standard_ShortReal': 'float',
        'Standard_Character': 'char',
        'Standard_Byte': 'unsigned char',
        'Standard_Address': 'void*',
    }

    # Value mappings
    VALUE_MAPPINGS = {
        'Standard_True': 'true',
        'Standard_False': 'false',
    }

    def __init__(self, src_dir: str, dry_run: bool = False, backup: bool = False,
                 verbose: bool = False, exclude_patterns: List[str] = None):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.backup = backup
        self.verbose = verbose
        self.exclude_patterns = exclude_patterns or []
        self.stats = MigrationStats()
        self.log_entries: List[str] = []

    def log(self, message: str, level: str = 'INFO'):
        """Log a message."""
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        entry = f"[{timestamp}] [{level}] {message}"
        self.log_entries.append(entry)
        if self.verbose or level in ('ERROR', 'WARNING'):
            print(entry)

    def should_process_file(self, file_path: Path) -> bool:
        """Check if a file should be processed."""
        # Check file extension
        if file_path.suffix not in ('.hxx', '.cxx', '.lxx', '.pxx'):
            return False

        # Check excluded files
        if file_path.name in self.EXCLUDE_FILES:
            self.stats.skipped_files.append(str(file_path))
            return False

        # Check exclude patterns
        for pattern in self.exclude_patterns:
            if re.search(pattern, str(file_path)):
                self.stats.skipped_files.append(str(file_path))
                return False

        return True

    def get_source_files(self) -> List[Path]:
        """Get all source files to process."""
        files = []
        for ext in ('*.hxx', '*.cxx', '*.lxx', '*.pxx'):
            files.extend(self.src_dir.rglob(ext))
        return sorted(files)

    def create_backup(self, file_path: Path):
        """Create a backup of the file."""
        if self.backup and not self.dry_run:
            backup_path = file_path.with_suffix(file_path.suffix + '.bak')
            shutil.copy2(file_path, backup_path)

    def read_file(self, file_path: Path) -> str:
        """Read file content with encoding handling."""
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

    # =========================================================================
    # Phase 1: Handle(Class) -> occ::handle<Class>
    # =========================================================================

    def migrate_handle_pattern(self, content: str) -> Tuple[str, int]:
        """
        Replace Handle(ClassName) with occ::handle<ClassName>.
        Does NOT replace Handle(ClassName)::DownCast patterns (handled in phase 2).
        """
        count = 0

        # Pattern: Handle(ClassName) not followed by ::DownCast
        # We need to be careful with nested parentheses and templates
        pattern = r'\bHandle\(([A-Z][a-zA-Z0-9_]*)\)(?!::DownCast)'

        def replace_handle(match):
            nonlocal count
            class_name = match.group(1)
            count += 1
            return f'occ::handle<{class_name}>'

        new_content = re.sub(pattern, replace_handle, content)
        return new_content, count

    # =========================================================================
    # Phase 2: Handle(Class)::DownCast -> occ::down_cast<Class>
    # =========================================================================

    def migrate_downcast_pattern(self, content: str) -> Tuple[str, int]:
        """
        Replace Handle(ClassName)::DownCast(...) with occ::down_cast<ClassName>(...).
        """
        count = 0

        # Pattern: Handle(ClassName)::DownCast(
        pattern = r'\bHandle\(([A-Z][a-zA-Z0-9_]*)\)::DownCast\('

        def replace_downcast(match):
            nonlocal count
            class_name = match.group(1)
            count += 1
            return f'occ::down_cast<{class_name}>('

        new_content = re.sub(pattern, replace_downcast, content)
        return new_content, count

    # =========================================================================
    # Phase 3: IsKind/IsInstance -> occ::is_kind/occ::is_instance
    # =========================================================================

    def migrate_iskind_isinstance_pattern(self, content: str) -> Tuple[str, int, int]:
        """
        Replace ->IsKind(STANDARD_TYPE(T)) with occ::is_kind<T>(...).
        Replace ->IsInstance(STANDARD_TYPE(T)) with occ::is_instance<T>(...).
        Returns (new_content, iskind_count, isinstance_count).
        """
        iskind_count = 0
        isinstance_count = 0

        # Pattern for IsKind: ->IsKind(STANDARD_TYPE(ClassName))
        # We need to capture the object before -> and the class name
        iskind_pattern = r'(\w+)->IsKind\s*\(\s*STANDARD_TYPE\s*\(\s*([A-Z][a-zA-Z0-9_]*)\s*\)\s*\)'

        def replace_iskind(match):
            nonlocal iskind_count
            obj_name = match.group(1)
            class_name = match.group(2)
            iskind_count += 1
            return f'occ::is_kind<{class_name}>({obj_name})'

        content = re.sub(iskind_pattern, replace_iskind, content)

        # Pattern for IsInstance: ->IsInstance(STANDARD_TYPE(ClassName))
        isinstance_pattern = r'(\w+)->IsInstance\s*\(\s*STANDARD_TYPE\s*\(\s*([A-Z][a-zA-Z0-9_]*)\s*\)\s*\)'

        def replace_isinstance(match):
            nonlocal isinstance_count
            obj_name = match.group(1)
            class_name = match.group(2)
            isinstance_count += 1
            return f'occ::is_instance<{class_name}>({obj_name})'

        content = re.sub(isinstance_pattern, replace_isinstance, content)

        return content, iskind_count, isinstance_count

    # =========================================================================
    # Phase 4: Standard_* types -> native types
    # =========================================================================

    def migrate_standard_types(self, content: str) -> Tuple[str, Dict[str, int]]:
        """
        Replace Standard_* types with native C++ types.
        """
        counts = {}

        # Replace type mappings
        for old_type, new_type in self.TYPE_MAPPINGS.items():
            # Use word boundary to avoid partial matches
            pattern = rf'\b{old_type}\b'
            matches = len(re.findall(pattern, content))
            if matches > 0:
                content = re.sub(pattern, new_type, content)
                counts[old_type] = matches

        # Replace value mappings
        for old_val, new_val in self.VALUE_MAPPINGS.items():
            pattern = rf'\b{old_val}\b'
            matches = len(re.findall(pattern, content))
            if matches > 0:
                content = re.sub(pattern, new_val, content)
                counts[old_val] = matches

        return content, counts

    # =========================================================================
    # Main processing
    # =========================================================================

    def process_file(self, file_path: Path, phases: Set[int]) -> bool:
        """
        Process a single file through specified phases.
        Returns True if the file was modified.
        """
        if not self.should_process_file(file_path):
            return False

        self.stats.files_processed += 1
        self.log(f"Processing: {file_path}")

        try:
            original_content = self.read_file(file_path)
            content = original_content
            modified = False

            # Phase 2 must come before Phase 1 to avoid double-processing
            # We need to handle DownCast first, then remaining Handle patterns

            # Phase 2: DownCast patterns (must be first!)
            if 2 in phases:
                content, downcast_count = self.migrate_downcast_pattern(content)
                if downcast_count > 0:
                    self.stats.downcast_replacements += downcast_count
                    modified = True
                    self.log(f"  DownCast replacements: {downcast_count}")

            # Phase 1: Handle patterns (after DownCast!)
            if 1 in phases:
                content, handle_count = self.migrate_handle_pattern(content)
                if handle_count > 0:
                    self.stats.handle_replacements += handle_count
                    modified = True
                    self.log(f"  Handle replacements: {handle_count}")

            # Phase 3: IsKind/IsInstance patterns
            if 3 in phases:
                content, iskind_count, isinstance_count = self.migrate_iskind_isinstance_pattern(content)
                if iskind_count > 0:
                    self.stats.iskind_replacements += iskind_count
                    modified = True
                    self.log(f"  IsKind replacements: {iskind_count}")
                if isinstance_count > 0:
                    self.stats.isinstance_replacements += isinstance_count
                    modified = True
                    self.log(f"  IsInstance replacements: {isinstance_count}")

            # Phase 4: Standard_* types
            if 4 in phases:
                content, type_counts = self.migrate_standard_types(content)
                if type_counts:
                    for type_name, count in type_counts.items():
                        self.stats.type_replacements[type_name] = \
                            self.stats.type_replacements.get(type_name, 0) + count
                    modified = True
                    self.log(f"  Type replacements: {type_counts}")

            if modified:
                self.create_backup(file_path)
                self.write_file(file_path, content)
                self.stats.files_modified += 1
                return True

        except Exception as e:
            error_msg = f"Error processing {file_path}: {e}"
            self.stats.errors.append(error_msg)
            self.log(error_msg, 'ERROR')

        return False

    def run(self, phases: Set[int] = None):
        """Run the modernization process."""
        if phases is None:
            phases = {1, 2, 3, 4}

        self.log(f"Starting OCCT modernization")
        self.log(f"Source directory: {self.src_dir}")
        self.log(f"Phases to run: {sorted(phases)}")
        self.log(f"Dry run: {self.dry_run}")

        files = self.get_source_files()
        total_files = len(files)
        self.log(f"Found {total_files} source files")

        for i, file_path in enumerate(files, 1):
            if i % 100 == 0:
                print(f"Progress: {i}/{total_files} files processed...")
            self.process_file(file_path, phases)

        self.log(f"\nMigration complete!")
        self.print_summary()

    def print_summary(self):
        """Print migration summary."""
        print("\n" + "=" * 60)
        print("OCCT Modernization Summary")
        print("=" * 60)
        print(f"Files processed:        {self.stats.files_processed}")
        print(f"Files modified:         {self.stats.files_modified}")
        print(f"Files skipped:          {len(self.stats.skipped_files)}")
        print(f"Handle replacements:    {self.stats.handle_replacements}")
        print(f"DownCast replacements:  {self.stats.downcast_replacements}")
        print(f"IsKind replacements:    {self.stats.iskind_replacements}")
        print(f"IsInstance replacements:{self.stats.isinstance_replacements}")

        if self.stats.type_replacements:
            print("\nType replacements:")
            for type_name, count in sorted(self.stats.type_replacements.items()):
                print(f"  {type_name}: {count}")

        if self.stats.errors:
            print(f"\nErrors ({len(self.stats.errors)}):")
            for error in self.stats.errors[:10]:
                print(f"  {error}")
            if len(self.stats.errors) > 10:
                print(f"  ... and {len(self.stats.errors) - 10} more")

        print("=" * 60)

    def verify(self) -> Dict[str, int]:
        """Verify remaining patterns after migration."""
        remaining = {
            'Handle(': 0,
            '::DownCast': 0,
            '->IsKind(STANDARD_TYPE': 0,
            '->IsInstance(STANDARD_TYPE': 0,
            'Standard_Boolean': 0,
            'Standard_Integer': 0,
            'Standard_Real': 0,
            'Standard_True': 0,
            'Standard_False': 0,
        }

        files = self.get_source_files()
        for file_path in files:
            if not self.should_process_file(file_path):
                continue
            try:
                content = self.read_file(file_path)
                for pattern in remaining:
                    remaining[pattern] += len(re.findall(re.escape(pattern), content))
            except Exception:
                pass

        print("\n" + "=" * 60)
        print("Verification - Remaining Patterns")
        print("=" * 60)
        for pattern, count in remaining.items():
            status = "OK" if count == 0 else f"REMAINING: {count}"
            print(f"  {pattern:20s}: {status}")
        print("=" * 60)

        return remaining

    def save_log(self, log_file: str):
        """Save log to file."""
        with open(log_file, 'w') as f:
            f.write('\n'.join(self.log_entries))
            f.write('\n\nStatistics:\n')
            f.write(json.dumps(self.stats.to_dict(), indent=2))


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Modernization Script',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )

    parser.add_argument('src_directory', nargs='?', default='.',
                        help='Source directory to process (default: current directory)')
    parser.add_argument('--dry-run', action='store_true',
                        help='Preview changes without modifying files')
    parser.add_argument('--backup', action='store_true',
                        help='Create .bak backup files before modification')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Show detailed progress')
    parser.add_argument('--phase', type=int, action='append', dest='phases',
                        help='Run specific phase (1=handles, 2=downcast, 3=types, 4=includes)')
    parser.add_argument('--exclude', action='append', dest='exclude_patterns',
                        help='Exclude files matching pattern (regex)')
    parser.add_argument('--verify', action='store_true',
                        help='Check for remaining patterns after migration')
    parser.add_argument('--log', dest='log_file',
                        help='Write detailed log to file')
    parser.add_argument('--stats', action='store_true',
                        help='Show statistics only, don\'t modify files')

    args = parser.parse_args()

    # Determine phases to run
    if args.phases:
        phases = set(args.phases)
    else:
        phases = {1, 2, 3, 4}

    # Validate phases
    valid_phases = {1, 2, 3, 4}
    if not phases.issubset(valid_phases):
        print(f"Error: Invalid phase(s). Valid phases are: {valid_phases}")
        sys.exit(1)

    # Create modernizer
    modernizer = OCCTModernizer(
        src_dir=args.src_directory,
        dry_run=args.dry_run or args.stats,
        backup=args.backup,
        verbose=args.verbose,
        exclude_patterns=args.exclude_patterns or []
    )

    # Run migration or verification
    if args.verify:
        modernizer.verify()
    else:
        modernizer.run(phases)

    # Save log if requested
    if args.log_file:
        modernizer.save_log(args.log_file)
        print(f"\nLog saved to: {args.log_file}")

    # Return non-zero if there were errors
    sys.exit(1 if modernizer.stats.errors else 0)


if __name__ == '__main__':
    main()
