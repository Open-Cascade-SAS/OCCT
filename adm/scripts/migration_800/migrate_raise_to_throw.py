#!/usr/bin/env python3
"""
Script to migrate Standard_*::Raise() calls to throw Standard_*() statements.

Pattern: Standard_SomeException::Raise("message") -> throw Standard_SomeException("message")

Also removes unnecessary 'return;' or 'return "";' statements after throw.

Usage:
    python3 migrate_raise_to_throw.py [--dry-run]
"""

import re
import os
import sys
import argparse

# Pattern to match Standard_*::Raise calls (single or multi-line)
# Captures: (ExceptionType, arguments)
RAISE_PATTERN = re.compile(
    r'(Standard_\w+)::Raise\s*\(\s*([^;]*?)\s*\)\s*;',
    re.DOTALL
)

# Pattern to match 'return "";' or 'return;' that might follow a throw
RETURN_AFTER_THROW = re.compile(
    r'(throw\s+Standard_\w+\s*\([^)]*\)\s*;)\s*\n(\s*)(return\s*"?"?\s*;)',
    re.DOTALL
)

# Special case for #define YY_FATAL_ERROR
YY_FATAL_PATTERN = re.compile(
    r'#define\s+YY_FATAL_ERROR\s*\(\s*msg\s*\)\s*Standard_Failure::Raise\s*\(\s*msg\s*\)\s*;'
)

# Patterns to skip (method definitions, not static calls)
SKIP_PATTERNS = [
    r'void\s+Standard_\w+::Raise\s*\(',  # Method definitions
    r'//.*Standard_\w+::Raise',           # Comments
]


def find_occt_root():
    """Find OCCT root directory by looking for src/ directory."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Navigate up from adm/scripts/migration_800 to root
    root = os.path.dirname(os.path.dirname(os.path.dirname(script_dir)))
    if os.path.isdir(os.path.join(root, 'src')):
        return root
    # Try current directory
    if os.path.isdir('src'):
        return os.getcwd()
    return None


def should_skip_line(line):
    """Check if line should be skipped (method definition or comment)."""
    for pattern in SKIP_PATTERNS:
        if re.search(pattern, line):
            return True
    return False


def find_files_with_raise(src_dir):
    """Find all .cxx and .hxx files containing Standard_*::Raise calls."""
    files_to_process = []

    for root, dirs, files in os.walk(src_dir):
        for filename in files:
            if filename.endswith(('.cxx', '.hxx', '.lex')):
                filepath = os.path.join(root, filename)
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
                        content = f.read()

                    # Check if file contains Raise pattern
                    if RAISE_PATTERN.search(content) or YY_FATAL_PATTERN.search(content):
                        # Verify it's not just method definitions
                        lines_with_raise = [line for line in content.split('\n')
                                          if 'Standard_' in line and '::Raise' in line]
                        has_static_calls = any(not should_skip_line(line)
                                              for line in lines_with_raise)
                        if has_static_calls:
                            files_to_process.append(filepath)
                except Exception as e:
                    print(f"  Warning: Could not read {filepath}: {e}")

    return files_to_process


def process_file(filepath, dry_run=False):
    """Process a single file to replace Raise calls with throw."""

    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    original_content = content
    changes = []

    # Handle special case for YY_FATAL_ERROR macro
    if YY_FATAL_PATTERN.search(content):
        content = YY_FATAL_PATTERN.sub(
            '#define YY_FATAL_ERROR(msg) throw Standard_Failure(msg);',
            content
        )
        changes.append("YY_FATAL_ERROR macro")

    # Replace Standard_*::Raise(...) with throw Standard_*(...)
    def replace_raise(match):
        exception_type = match.group(1)
        args = match.group(2).strip()
        # Clean up multi-line arguments
        args = ' '.join(args.split())
        changes.append(f"{exception_type}::Raise -> throw")
        return f'throw {exception_type}({args});'

    content = RAISE_PATTERN.sub(replace_raise, content)

    # Remove 'return "";' or 'return;' that follows a throw
    def remove_return_after_throw(match):
        throw_stmt = match.group(1)
        changes.append("removed return after throw")
        return throw_stmt

    content = RETURN_AFTER_THROW.sub(remove_return_after_throw, content)

    if content != original_content:
        if not dry_run:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
        return changes
    return None


def main():
    parser = argparse.ArgumentParser(
        description='Migrate Standard_*::Raise() to throw Standard_*()'
    )
    parser.add_argument('--dry-run', action='store_true',
                       help='Show what would be changed without modifying files')
    args = parser.parse_args()

    occt_root = find_occt_root()
    if not occt_root:
        print("Error: Could not find OCCT root directory")
        sys.exit(1)

    src_dir = os.path.join(occt_root, 'src')
    print(f"OCCT root: {occt_root}")
    print(f"Scanning: {src_dir}")
    print("=" * 60)

    if args.dry_run:
        print("DRY RUN - no files will be modified")
        print("=" * 60)

    # Find files to process
    files = find_files_with_raise(src_dir)
    print(f"Found {len(files)} files with Standard_*::Raise calls")
    print("=" * 60)

    modified_count = 0
    for filepath in sorted(files):
        rel_path = os.path.relpath(filepath, occt_root)
        changes = process_file(filepath, args.dry_run)
        if changes:
            modified_count += 1
            action = "WOULD MODIFY" if args.dry_run else "MODIFIED"
            print(f"  {action}: {rel_path}")
            for change in changes:
                print(f"    - {change}")

    print("=" * 60)
    action = "Would modify" if args.dry_run else "Modified"
    print(f"{action} {modified_count} files")


if __name__ == "__main__":
    main()
