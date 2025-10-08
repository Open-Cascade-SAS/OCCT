#!/usr/bin/env python3
"""
Script to clean up duplicate #include directives and self-includes in C++ source files.
Removes:
1. Duplicate #include statements (keeps only the first occurrence)
2. Self-includes (e.g., Foo.hxx including "Foo.hxx")

Processes: .cxx, .hxx, .pxx, .lxx, .gxx files
"""

import os
import re
from pathlib import Path
from typing import List, Tuple


def get_filename_without_extension(filepath: str) -> str:
    """Get the filename without extension."""
    return Path(filepath).stem


def process_file(filepath: str, dry_run: bool = False) -> Tuple[bool, int, bool]:
    """
    Process a single file to remove duplicate includes and self-includes.

    Returns:
        Tuple of (modified, num_duplicates_removed, had_self_include)
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return False, 0, False

    # Pattern to match #include directives and preprocessor directives
    include_pattern = re.compile(r'^\s*#\s*include\s+[<"]([^>"]+)[>"]')
    preprocessor_pattern = re.compile(r'^\s*#\s*(if|ifdef|ifndef|elif|else|endif)')

    ifdef_stack = [set()]  # Stack of seen includes per preprocessor scope
    new_lines = []
    duplicates_removed = 0
    had_self_include = False
    in_block_comment = False

    base_filename = get_filename_without_extension(filepath)
    file_extension = Path(filepath).suffix

    for line in lines:
        # Track multi-line comments
        if '/*' in line:
            in_block_comment = True
        if '*/' in line:
            in_block_comment = False
            new_lines.append(line)
            continue

        # Skip lines in block comments or single-line comments
        stripped = line.lstrip()
        if in_block_comment or stripped.startswith('//'):
            new_lines.append(line)
            continue

        # Track preprocessor scope changes
        prep_match = preprocessor_pattern.match(line)
        if prep_match:
            directive = prep_match.group(1)
            if directive in ('if', 'ifdef', 'ifndef'):
                # Start new scope, inheriting parent scope's includes
                parent_includes = ifdef_stack[-1].copy() if ifdef_stack else set()
                ifdef_stack.append(parent_includes)
            elif directive == 'endif':
                # End scope
                if len(ifdef_stack) > 1:
                    ifdef_stack.pop()
            elif directive in ('elif', 'else'):
                # Reset current scope to parent scope (alternative branches)
                if len(ifdef_stack) > 1:
                    parent_includes = ifdef_stack[-2].copy()
                    ifdef_stack[-1] = parent_includes
                elif len(ifdef_stack) == 1:
                    ifdef_stack[-1] = set()
            new_lines.append(line)
            continue

        match = include_pattern.match(line)

        if match:
            included_file = match.group(1)

            # Skip malformed includes (e.g., empty or just extension)
            if not included_file or included_file.startswith('.') or len(included_file) <= 4:
                new_lines.append(line)
                continue

            included_basename = Path(included_file).stem
            included_extension = Path(included_file).suffix

            # Skip .gxx includes from duplicate checking
            if included_extension == '.gxx':
                new_lines.append(line)
                continue

            # Check for self-include (same name AND same extension)
            if included_basename == base_filename and included_extension == file_extension:
                had_self_include = True
                print(f"  Removing self-include: {line.strip()}")
                continue

            # Check for duplicate only in current scope
            current_scope = ifdef_stack[-1]
            if included_file in current_scope:
                duplicates_removed += 1
                print(f"  Removing duplicate: {line.strip()}")
                continue

            current_scope.add(included_file)

        new_lines.append(line)

    # Check if file was modified
    modified = (len(new_lines) != len(lines))

    if modified and not dry_run:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.writelines(new_lines)
        except Exception as e:
            print(f"Error writing {filepath}: {e}")
            return False, 0, False

    return modified, duplicates_removed, had_self_include


def find_files(root_dir: str, extensions: List[str]) -> List[str]:
    """Find all files with specified extensions in the directory tree."""
    files = []
    for ext in extensions:
        files.extend(Path(root_dir).rglob(f"*{ext}"))
    return [str(f) for f in files]


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description='Clean up duplicate #include directives and self-includes in C++ files'
    )
    parser.add_argument(
        'path',
        nargs='?',
        default='src',
        help='Root directory to process (default: src)'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Show what would be changed without modifying files'
    )
    parser.add_argument(
        '--extensions',
        nargs='+',
        default=['.cxx', '.hxx', '.pxx', '.lxx', '.gxx'],
        help='File extensions to process (default: .cxx .hxx .pxx .lxx .gxx)'
    )
    parser.add_argument(
        '--files',
        nargs='+',
        help='Specific files to process (overrides path scanning)'
    )

    args = parser.parse_args()

    if args.files:
        # Process specific files
        files = [os.path.abspath(f) for f in args.files if os.path.isfile(f)]
        print(f"Processing {len(files)} specific files")
    else:
        # Scan directory
        root_dir = os.path.abspath(args.path)

        if not os.path.isdir(root_dir):
            print(f"Error: {root_dir} is not a directory")
            return 1

        print(f"Scanning for files in: {root_dir}")
        print(f"Extensions: {', '.join(args.extensions)}")
        files = find_files(root_dir, args.extensions)
        print(f"Found {len(files)} files to process")

    if args.dry_run:
        print("DRY RUN MODE - No files will be modified\n")

    total_modified = 0
    total_duplicates = 0
    total_self_includes = 0

    for filepath in sorted(files):
        modified, duplicates, self_include = process_file(filepath, args.dry_run)

        if modified:
            total_modified += 1
            total_duplicates += duplicates
            if self_include:
                total_self_includes += 1

            print(f"Modified: {filepath}")
            if duplicates > 0:
                print(f"  - Removed {duplicates} duplicate include(s)")
            if self_include:
                print(f"  - Removed self-include")
            print()

    print("\n" + "="*70)
    print("SUMMARY")
    print("="*70)
    print(f"Files processed: {len(files)}")
    print(f"Files modified: {total_modified}")
    print(f"Duplicate includes removed: {total_duplicates}")
    print(f"Files with self-includes fixed: {total_self_includes}")

    if args.dry_run:
        print("\nThis was a dry run. Use without --dry-run to apply changes.")

    return 0


if __name__ == '__main__':
    exit(main())
