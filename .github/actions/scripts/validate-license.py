#!/usr/bin/env python3
"""
Script to validate and add OCCT license headers to C++ source files.

Validates that files contain the Open CASCADE Technology license header.
Automatically adds missing headers with the current year.

Processes: .cxx, .hxx, .pxx, .lxx files
"""

import os
import re
import sys
from datetime import datetime
from pathlib import Path
from typing import List, Tuple, Optional


# The canonical license text (normalized for comparison)
CANONICAL_LICENSE_LINES = [
    "This file is part of Open CASCADE Technology software library.",
    "",
    "This library is free software; you can redistribute it and/or modify it under",
    "the terms of the GNU Lesser General Public License version 2.1 as published",
    "by the Free Software Foundation, with special exception defined in the file",
    "OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT",
    "distribution for complete text of the license and disclaimer of any warranty.",
    "",
    "Alternatively, this file may be used under the terms of Open CASCADE",
    "commercial license or contractual agreement.",
]


def normalize_text(text: str) -> str:
    """Normalize text for comparison by removing extra whitespace and converting to lowercase."""
    # Remove multiple spaces, convert to lowercase, strip whitespace
    return ' '.join(text.lower().split())


def generate_license_header(year: Optional[int] = None) -> str:
    """
    Generate the license header with C++ style comments.

    Args:
        year: Optional year to use. If None, uses current year.

    Returns:
        Complete license header as string
    """
    if year is None:
        year = datetime.now().year

    header_lines = [
        f"// Copyright (c) {year} OPEN CASCADE SAS",
        "//",
        "// This file is part of Open CASCADE Technology software library.",
        "//",
        "// This library is free software; you can redistribute it and/or modify it under",
        "// the terms of the GNU Lesser General Public License version 2.1 as published",
        "// by the Free Software Foundation, with special exception defined in the file",
        "// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT",
        "// distribution for complete text of the license and disclaimer of any warranty.",
        "//",
        "// Alternatively, this file may be used under the terms of Open CASCADE",
        "// commercial license or contractual agreement.",
        ""  # Empty line after header
    ]

    return '\n'.join(header_lines) + '\n'


def extract_license_text(lines: List[str], start_idx: int, max_lines: int = 20) -> Tuple[List[str], int]:
    """
    Extract potential license text from comment block starting at start_idx.

    Returns:
        Tuple of (extracted_text_lines, end_index)
    """
    extracted = []
    current_idx = start_idx
    in_block_comment = False

    while current_idx < len(lines) and current_idx < start_idx + max_lines:
        line = lines[current_idx].rstrip()

        # Check for start of block comment
        if line.strip() == '/*' or line.strip().startswith('/*'):
            in_block_comment = True
            # Extract text after /* if any
            text_after = line.strip()[2:].strip()
            if text_after and not text_after.startswith('*'):
                extracted.append(text_after)
            current_idx += 1
            continue

        # Check for end of block comment
        if '*/' in line:
            in_block_comment = False
            # Extract text before */ if any
            text_before = line.split('*/')[0].strip().lstrip('*').strip()
            if text_before:
                extracted.append(text_before)
            current_idx += 1
            break

        # Inside block comment
        if in_block_comment:
            # Remove leading spaces and asterisks
            text_content = line.strip().lstrip('*').strip()
            extracted.append(text_content)
            current_idx += 1
            continue

        # Check for // style comment
        if line.strip().startswith('//'):
            text_content = line.strip()[2:].strip()
            extracted.append(text_content)
            current_idx += 1
            continue

        # If we're not in a comment and line is not a comment, stop
        if line.strip() and not in_block_comment:
            break

        current_idx += 1

    return extracted, current_idx


def check_license_header(filepath: str) -> Tuple[bool, Optional[int], int]:
    """
    Check if file has a valid license header.

    Returns:
        Tuple of (has_valid_license, line_where_found, header_end_line)
        If no valid license found, returns (False, None, 0)
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False, None, 0

    if not lines:
        return False, None, 0

    # Check first 30 lines for license header
    for start_line in range(min(30, len(lines))):
        extracted, end_idx = extract_license_text(lines, start_line, max_lines=25)

        if len(extracted) < 5:  # Too short to be a license
            continue

        # Compare normalized text
        matched_lines = 0
        for canonical_line in CANONICAL_LICENSE_LINES:
            normalized_canonical = normalize_text(canonical_line)

            # Skip empty lines in comparison
            if not normalized_canonical:
                continue

            # Check if any extracted line matches this canonical line
            for extracted_line in extracted:
                normalized_extracted = normalize_text(extracted_line)

                if normalized_canonical in normalized_extracted or normalized_extracted in normalized_canonical:
                    matched_lines += 1
                    break

        # If we matched most of the key lines (allowing some variation)
        if matched_lines >= 7:  # At least 7 out of ~9 non-empty lines
            return True, start_line, end_idx

    return False, None, 0


def add_license_header(filepath: str, dry_run: bool = False) -> bool:
    """
    Add license header to file if missing.

    Returns:
        True if file was modified, False otherwise
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False

    # Generate header with current year
    header = generate_license_header()

    # Check if file starts with shebang
    new_content = content
    if content.startswith('#!'):
        # Preserve shebang line
        first_newline = content.find('\n')
        if first_newline != -1:
            shebang = content[:first_newline + 1]
            rest = content[first_newline + 1:]
            new_content = shebang + '\n' + header + '\n' + rest
        else:
            new_content = content + '\n' + header
    else:
        # Add header at the beginning
        new_content = header + '\n' + content

    if not dry_run:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(new_content)
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False

    return True


def find_files(root_dir: str, extensions: List[str]) -> List[str]:
    """Find all files with specified extensions in the directory tree."""
    files = []
    for ext in extensions:
        files.extend(Path(root_dir).rglob(f"*{ext}"))
    return [str(f) for f in files]


def process_file(filepath: str, fix: bool = False, dry_run: bool = False) -> Tuple[bool, bool]:
    """
    Process a single file to check/fix license header.

    Returns:
        Tuple of (has_license, was_modified)
    """
    has_license, _, _ = check_license_header(filepath)

    if has_license:
        return True, False

    # License is missing
    if fix:
        modified = add_license_header(filepath, dry_run)
        return False, modified

    return False, False


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description='Validate and add OCCT license headers to C++ source files'
    )
    parser.add_argument(
        'path',
        nargs='?',
        default='src',
        help='Root directory or file to process (default: src)'
    )
    parser.add_argument(
        '--fix',
        action='store_true',
        help='Automatically add missing license headers'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Show what would be changed without modifying files (implies --fix)'
    )
    parser.add_argument(
        '--extensions',
        nargs='+',
        default=['.cxx', '.hxx', '.pxx', '.lxx'],
        help='File extensions to process (default: .cxx .hxx .pxx .lxx)'
    )
    parser.add_argument(
        '--files',
        nargs='+',
        help='Specific files to process (overrides path scanning)'
    )
    parser.add_argument(
        '--ci',
        action='store_true',
        help='CI mode: exit with error code if any file is missing license'
    )

    args = parser.parse_args()

    # Dry run implies fix mode
    if args.dry_run:
        args.fix = True

    # Get list of files to process
    if args.files:
        # Process specific files
        files = [os.path.abspath(f) for f in args.files if os.path.isfile(f)]
        if len(files) == 0:
            print("Error: No valid files specified")
            return 1
    else:
        # Process directory or single file
        path = os.path.abspath(args.path)

        if os.path.isfile(path):
            files = [path]
        elif os.path.isdir(path):
            print(f"Scanning for files in: {path}")
            print(f"Extensions: {', '.join(args.extensions)}")
            files = find_files(path, args.extensions)
            print(f"Found {len(files)} files to process\n")
        else:
            print(f"Error: {path} is not a valid file or directory")
            return 1

    if args.dry_run:
        print("DRY RUN MODE - No files will be modified\n")

    # Process files
    missing_license = []
    fixed_files = []

    for filepath in sorted(files):
        has_license, was_modified = process_file(filepath, args.fix, args.dry_run)

        if not has_license and not was_modified:
            missing_license.append(filepath)
        elif was_modified:
            fixed_files.append(filepath)

    # Print results
    if missing_license:
        print(f"\n{'='*70}")
        print(f"FILES MISSING LICENSE HEADER ({len(missing_license)})")
        print(f"{'='*70}")
        for filepath in missing_license:
            print(f"  {filepath}")

    if fixed_files:
        print(f"\n{'='*70}")
        print(f"FILES {'THAT WOULD BE ' if args.dry_run else ''}FIXED ({len(fixed_files)})")
        print(f"{'='*70}")
        for filepath in fixed_files:
            print(f"  {filepath}")

    # Summary
    print(f"\n{'='*70}")
    print("SUMMARY")
    print(f"{'='*70}")
    print(f"Files processed: {len(files)}")
    print(f"Files with valid license: {len(files) - len(missing_license) - len(fixed_files)}")
    print(f"Files missing license: {len(missing_license)}")
    if args.fix:
        print(f"Files {'that would be ' if args.dry_run else ''}fixed: {len(fixed_files)}")

    if args.dry_run:
        print("\nThis was a dry run. Use --fix without --dry-run to apply changes.")

    # Exit with error in CI mode if any files are missing license
    if args.ci and (missing_license or (fixed_files and not args.fix)):
        print("\n[CI MODE] License validation FAILED")
        return 1

    if not args.fix and missing_license:
        print(f"\nUse --fix to automatically add license headers to files missing them.")
        return 1

    return 0


if __name__ == '__main__':
    exit(main())
