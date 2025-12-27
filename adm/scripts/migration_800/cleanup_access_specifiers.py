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
OCCT 8.0.0 Access Specifier Cleanup Script

Cleans up redundant access specifiers in C++ files:
- Back-to-back specifiers: public:\nprivate: -> private:
- Trailing specifiers at end of class: private:\n}; -> };
- Empty access specifier sections

Usage:
    python3 cleanup_access_specifiers.py [options] <src_directory>

Options:
    --dry-run           Preview changes without modifying files
    --verbose           Show detailed progress
    --file PATH         Process only a specific file
    --jobs N            Number of parallel jobs (default: 4)
"""

import os
import re
import argparse
from pathlib import Path
from typing import Dict, List, Set
from concurrent.futures import ProcessPoolExecutor, as_completed

# Global state for workers
DRY_RUN: bool = False

SKIP_DIRS = {'install', 'build', 'mac64', 'win64', 'lin64', '.git', '__pycache__'}
SOURCE_EXTENSIONS = {'.hxx', '.cxx', '.lxx', '.pxx', '.gxx', '.h', '.c'}

# Access specifier pattern
ACCESS_SPECIFIER = r'(?:public|protected|private)\s*:'


def process_file(file_path_str: str) -> Dict:
    """Process a single file."""
    file_path = Path(file_path_str)
    result = {
        'file_path': file_path_str,
        'changes': [],
        'modified': False,
        'error': None
    }

    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        result['error'] = str(e)
        return result

    original = content

    # Pattern 1: Back-to-back access specifiers (keep only the last one)
    # Matches: public:\n  private: or public:\nprivate: etc.
    pattern1 = re.compile(
        r'(' + ACCESS_SPECIFIER + r')\s*\n\s*(' + ACCESS_SPECIFIER + r')',
        re.MULTILINE
    )
    while pattern1.search(content):
        content = pattern1.sub(r'\2', content)
        result['changes'].append('back-to-back specifiers')

    # Pattern 2: Access specifier followed only by closing brace (trailing specifier)
    # Matches: private:\n}; or public:\n  };
    pattern2 = re.compile(
        r'(' + ACCESS_SPECIFIER + r')\s*\n(\s*}\s*;)',
        re.MULTILINE
    )
    if pattern2.search(content):
        content = pattern2.sub(r'\2', content)
        result['changes'].append('trailing specifier before };')

    # Pattern 3: Access specifier followed by another access specifier with only whitespace/comments
    # This catches cases with empty lines between them
    pattern3 = re.compile(
        r'(' + ACCESS_SPECIFIER + r')\s*\n(?:\s*//[^\n]*\n)*\s*(' + ACCESS_SPECIFIER + r')',
        re.MULTILINE
    )
    while pattern3.search(content):
        content = pattern3.sub(r'\2', content)
        result['changes'].append('specifiers with only comments between')

    # Pattern 4: Multiple consecutive blank lines after access specifier cleanup
    content = re.sub(r'\n{3,}', '\n\n', content)

    if content != original:
        result['modified'] = True
        if not DRY_RUN:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)

    return result


def get_source_files(src_dir: Path) -> List[str]:
    """Get all source files."""
    files = []
    for ext in SOURCE_EXTENSIONS:
        for f in src_dir.rglob(f'*{ext}'):
            if not any(skip in f.parts for skip in SKIP_DIRS):
                files.append(str(f))
    return sorted(files)


def main():
    global DRY_RUN

    parser = argparse.ArgumentParser(description='OCCT Access Specifier Cleanup Script')
    parser.add_argument('src_directory', nargs='?', default='.', help='Source directory')
    parser.add_argument('--dry-run', action='store_true', help='Preview only')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    parser.add_argument('--file', dest='single_file', help='Process single file')
    parser.add_argument('--jobs', '-j', type=int, default=4, help='Parallel jobs (default: 4)')

    args = parser.parse_args()
    DRY_RUN = args.dry_run

    print("OCCT Access Specifier Cleanup")
    print("=" * 60)
    if args.dry_run:
        print("(DRY RUN)")

    # Get files
    src_dir = Path(args.src_directory)
    files = [args.single_file] if args.single_file else get_source_files(src_dir)
    print(f"Processing {len(files)} files...")

    # Process files
    total_changes = 0
    modified_count = 0
    processed = 0

    for file_path in files:
        processed += 1
        if processed % 2000 == 0:
            print(f"Progress: {processed}/{len(files)}")

        result = process_file(file_path)
        if result['modified']:
            modified_count += 1
            total_changes += len(result['changes'])
            if args.verbose:
                print(f"  Modified: {result['file_path']}")
                for change in result['changes']:
                    print(f"    - {change}")

    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Files processed: {len(files)}")
    print(f"Files modified: {modified_count}")
    print(f"Total cleanups: {total_changes}")

    if args.dry_run:
        print("\nRun without --dry-run to apply changes")


if __name__ == '__main__':
    main()
