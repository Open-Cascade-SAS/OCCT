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
Migration Verification Script for OCCT 8.0.0

This script scans the codebase to find remaining legacy patterns that should
have been migrated. Useful for:
1. Verifying migration completeness
2. Finding patterns that were missed
3. Identifying files that may need manual review

Usage:
    python3 verify_migration.py [options] <src_directory>

Options:
    --verbose           Show all matches
    --output=FILE       Write report to file
    --json              Output as JSON
"""

import os
import re
import sys
import json
import argparse
from pathlib import Path
from typing import List, Dict, Tuple
from dataclasses import dataclass, field, asdict
from collections import defaultdict


@dataclass
class PatternMatch:
    """A single pattern match."""
    file: str
    line_number: int
    line_content: str
    pattern: str


@dataclass
class VerificationReport:
    """Complete verification report."""
    total_files_scanned: int = 0
    patterns_checked: List[str] = field(default_factory=list)
    matches_by_pattern: Dict[str, List[PatternMatch]] = field(default_factory=lambda: defaultdict(list))
    summary: Dict[str, int] = field(default_factory=dict)

    def add_match(self, pattern: str, match: PatternMatch):
        self.matches_by_pattern[pattern].append(match)

    def compute_summary(self):
        for pattern, matches in self.matches_by_pattern.items():
            self.summary[pattern] = len(matches)


class MigrationVerifier:
    """Verifies that migration patterns have been applied."""

    # Files to skip (core infrastructure where patterns are defined)
    SKIP_FILES = {
        'Standard_Handle.hxx',
        'Standard_TypeDef.hxx',
        'Standard_Transient.hxx',
        'Standard_Transient.cxx',
        'Standard_Type.hxx',
        'Standard_Type.cxx',
        'Standard_Macro.hxx',
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
        # Migration scripts themselves
        'occt_modernize.py',
        'migrate_handles.py',
        'migrate_standard_types.py',
        'migrate_macros.py',
        'verify_migration.py',
    }

    # Patterns to check for (should NOT be present after migration)
    LEGACY_PATTERNS = {
        # Handle patterns
        'Handle(Class)': r'\bHandle\([A-Z][a-zA-Z0-9_]*\)',
        '::DownCast': r'::DownCast\(',

        # Standard types
        'Standard_Boolean': r'\bStandard_Boolean\b',
        'Standard_Integer': r'\bStandard_Integer\b',
        'Standard_Real': r'\bStandard_Real\b',
        'Standard_ShortReal': r'\bStandard_ShortReal\b',
        'Standard_Character': r'\bStandard_Character\b',
        'Standard_Byte': r'\bStandard_Byte\b',
        'Standard_Address': r'\bStandard_Address\b',

        # Standard values
        'Standard_True': r'\bStandard_True\b',
        'Standard_False': r'\bStandard_False\b',

        # Legacy includes
        '#include <Standard_Boolean.hxx>': r'#include\s*[<"]Standard_Boolean\.hxx[>"]',
        '#include <Standard_Integer.hxx>': r'#include\s*[<"]Standard_Integer\.hxx[>"]',
        '#include <Standard_Real.hxx>': r'#include\s*[<"]Standard_Real\.hxx[>"]',

        # Legacy macros (should be replaced with C++ keywords)
        'Standard_OVERRIDE': r'\bStandard_OVERRIDE\b',
        'Standard_NODISCARD': r'\bStandard_NODISCARD\b',
        'Standard_FALLTHROUGH': r'\bStandard_FALLTHROUGH\b',
        'Standard_Noexcept': r'\bStandard_Noexcept\b',
        'Standard_DELETE': r'\bStandard_DELETE\b',
        'Standard_THREADLOCAL': r'\bStandard_THREADLOCAL\b',
        'Standard_ATOMIC': r'\bStandard_ATOMIC\s*\(',
        'Standard_UNUSED': r'\bStandard_UNUSED\b',

        # Redundant handle macros (should be removed)
        'DEFINE_STANDARD_HANDLE': r'\bDEFINE_STANDARD_HANDLE\s*\(',
    }

    # New patterns that SHOULD be present after migration
    NEW_PATTERNS = {
        'occ::handle<': r'occ::handle<[A-Z]',
        'occ::down_cast<': r'occ::down_cast<[A-Z]',
    }

    def __init__(self, src_dir: str, verbose: bool = False):
        self.src_dir = Path(src_dir)
        self.verbose = verbose
        self.report = VerificationReport()

    def should_skip(self, file_path: Path) -> bool:
        """Check if file should be skipped."""
        return file_path.name in self.SKIP_FILES

    def read_file(self, file_path: Path) -> List[str]:
        """Read file lines."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                return f.readlines()
        except UnicodeDecodeError:
            with open(file_path, 'r', encoding='latin-1') as f:
                return f.readlines()

    def get_source_files(self) -> List[Path]:
        """Get all source files."""
        files = []
        for ext in ('*.hxx', '*.cxx', '*.lxx', '*.pxx'):
            files.extend(self.src_dir.rglob(ext))
        return sorted(files)

    def scan_file(self, file_path: Path, patterns: Dict[str, str]) -> List[PatternMatch]:
        """Scan a file for patterns."""
        matches = []

        if self.should_skip(file_path):
            return matches

        try:
            lines = self.read_file(file_path)
            for line_num, line in enumerate(lines, 1):
                for pattern_name, regex in patterns.items():
                    if re.search(regex, line):
                        matches.append(PatternMatch(
                            file=str(file_path),
                            line_number=line_num,
                            line_content=line.strip()[:100],
                            pattern=pattern_name
                        ))
        except Exception as e:
            print(f"Error scanning {file_path}: {e}", file=sys.stderr)

        return matches

    def verify_legacy_patterns(self) -> VerificationReport:
        """Verify that legacy patterns have been removed."""
        files = self.get_source_files()
        self.report.total_files_scanned = len(files)
        self.report.patterns_checked = list(self.LEGACY_PATTERNS.keys())

        print(f"Scanning {len(files)} files for legacy patterns...")

        for i, file_path in enumerate(files, 1):
            if i % 500 == 0:
                print(f"Progress: {i}/{len(files)}")

            matches = self.scan_file(file_path, self.LEGACY_PATTERNS)
            for match in matches:
                self.report.add_match(match.pattern, match)

        self.report.compute_summary()
        return self.report

    def verify_new_patterns(self) -> Dict[str, int]:
        """Verify that new patterns are present (migration was applied)."""
        files = self.get_source_files()
        counts = {name: 0 for name in self.NEW_PATTERNS}

        print(f"Scanning {len(files)} files for new patterns...")

        for file_path in files:
            if self.should_skip(file_path):
                continue
            try:
                content = self.read_file(file_path)
                content_str = ''.join(content)
                for pattern_name, regex in self.NEW_PATTERNS.items():
                    matches = re.findall(regex, content_str)
                    counts[pattern_name] += len(matches)
            except Exception:
                pass

        return counts

    def print_report(self):
        """Print the verification report."""
        print("\n" + "=" * 70)
        print("OCCT Migration Verification Report")
        print("=" * 70)
        print(f"\nFiles scanned: {self.report.total_files_scanned}")

        print("\n" + "-" * 70)
        print("Legacy Patterns (should be ZERO after migration)")
        print("-" * 70)

        all_clean = True
        for pattern in sorted(self.report.summary.keys()):
            count = self.report.summary[pattern]
            status = "OK" if count == 0 else f"REMAINING: {count}"
            if count > 0:
                all_clean = False
            print(f"  {pattern:40s}: {status}")

        if all_clean:
            print("\n  All legacy patterns have been migrated!")
        else:
            print("\n  Some legacy patterns remain. See details below.")

        # Show details for remaining patterns
        if self.verbose and not all_clean:
            print("\n" + "-" * 70)
            print("Detailed Matches (first 5 per pattern)")
            print("-" * 70)

            for pattern, matches in self.report.matches_by_pattern.items():
                if matches:
                    print(f"\n{pattern} ({len(matches)} matches):")
                    for match in matches[:5]:
                        print(f"  {match.file}:{match.line_number}")
                        print(f"    {match.line_content}")
                    if len(matches) > 5:
                        print(f"  ... and {len(matches) - 5} more")

        print("\n" + "=" * 70)

    def save_report_json(self, output_file: str):
        """Save report as JSON."""
        data = {
            'total_files_scanned': self.report.total_files_scanned,
            'patterns_checked': self.report.patterns_checked,
            'summary': self.report.summary,
            'matches': {
                pattern: [asdict(m) for m in matches]
                for pattern, matches in self.report.matches_by_pattern.items()
            }
        }
        with open(output_file, 'w') as f:
            json.dump(data, f, indent=2)
        print(f"\nReport saved to: {output_file}")

    def save_report_text(self, output_file: str):
        """Save report as text."""
        with open(output_file, 'w') as f:
            f.write("OCCT Migration Verification Report\n")
            f.write("=" * 70 + "\n\n")
            f.write(f"Files scanned: {self.report.total_files_scanned}\n\n")

            f.write("Summary:\n")
            f.write("-" * 70 + "\n")
            for pattern in sorted(self.report.summary.keys()):
                count = self.report.summary[pattern]
                f.write(f"  {pattern:40s}: {count}\n")

            f.write("\n\nDetailed Matches:\n")
            f.write("-" * 70 + "\n")
            for pattern, matches in self.report.matches_by_pattern.items():
                if matches:
                    f.write(f"\n{pattern} ({len(matches)} matches):\n")
                    for match in matches:
                        f.write(f"  {match.file}:{match.line_number}\n")
                        f.write(f"    {match.line_content}\n")

        print(f"\nReport saved to: {output_file}")


def main():
    parser = argparse.ArgumentParser(description='OCCT Migration Verification Script')
    parser.add_argument('src_directory', nargs='?', default='.',
                        help='Source directory to scan')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Show detailed matches')
    parser.add_argument('--output', dest='output_file',
                        help='Save report to file')
    parser.add_argument('--json', action='store_true',
                        help='Output report as JSON')
    parser.add_argument('--check-new', action='store_true',
                        help='Also check for new pattern usage')

    args = parser.parse_args()

    verifier = MigrationVerifier(
        src_dir=args.src_directory,
        verbose=args.verbose
    )

    # Verify legacy patterns
    verifier.verify_legacy_patterns()
    verifier.print_report()

    # Check new patterns if requested
    if args.check_new:
        print("\n" + "-" * 70)
        print("New Pattern Usage (confirms migration was applied)")
        print("-" * 70)
        new_counts = verifier.verify_new_patterns()
        for pattern, count in new_counts.items():
            print(f"  {pattern:40s}: {count}")

    # Save report if requested
    if args.output_file:
        if args.json:
            verifier.save_report_json(args.output_file)
        else:
            verifier.save_report_text(args.output_file)

    # Return non-zero if legacy patterns remain
    remaining = sum(verifier.report.summary.values())
    sys.exit(0 if remaining == 0 else 1)


if __name__ == '__main__':
    main()
