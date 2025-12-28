#!/usr/bin/env python3
"""
Migration script for OCCT H-collection macros to templates.

This script converts files using DEFINE_HARRAY1, DEFINE_HARRAY2, and DEFINE_HSEQUENCE
macros to use the new NCollection_HArray1, NCollection_HArray2, and NCollection_HSequence
template classes with typedef aliases.

Usage:
    python3 migrate_hcollections.py [--dry-run] [--verbose]
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import Optional, Tuple, Dict, List

# Root of the OCCT source tree
OCCT_ROOT = Path(__file__).parent.parent.parent.parent

# Regex patterns for macro detection (single-line)
DEFINE_HARRAY1_PATTERN = re.compile(r'^\s*DEFINE_HARRAY1\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HARRAY2_PATTERN = re.compile(r'^\s*DEFINE_HARRAY2\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HSEQUENCE_PATTERN = re.compile(r'^\s*DEFINE_HSEQUENCE\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)

# Regex patterns for multi-line macros
DEFINE_HARRAY1_MULTILINE_PATTERN = re.compile(
    r'^\s*DEFINE_HARRAY1\s*\(\s*(\w+)\s*,\s*\n\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HARRAY2_MULTILINE_PATTERN = re.compile(
    r'^\s*DEFINE_HARRAY2\s*\(\s*(\w+)\s*,\s*\n\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HSEQUENCE_MULTILINE_PATTERN = re.compile(
    r'^\s*DEFINE_HSEQUENCE\s*\(\s*(\w+)\s*,\s*\n\s*(\w+)\s*\)\s*;?\s*$', re.MULTILINE)

# Regex patterns for macros with direct NCollection_Array/Sequence types
DEFINE_HARRAY1_DIRECT_PATTERN = re.compile(
    r'^\s*DEFINE_HARRAY1\s*\(\s*(\w+)\s*,\s*NCollection_Array1\s*<\s*(.+?)\s*>\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HARRAY2_DIRECT_PATTERN = re.compile(
    r'^\s*DEFINE_HARRAY2\s*\(\s*(\w+)\s*,\s*NCollection_Array2\s*<\s*(.+?)\s*>\s*\)\s*;?\s*$', re.MULTILINE)
DEFINE_HSEQUENCE_DIRECT_PATTERN = re.compile(
    r'^\s*DEFINE_HSEQUENCE\s*\(\s*(\w+)\s*,\s*NCollection_Sequence\s*<\s*(.+?)\s*>\s*\)\s*;?\s*$', re.MULTILINE)

# Pattern to find element type from underlying array/sequence typedefs
ARRAY1_TYPEDEF_PATTERN = re.compile(r'typedef\s+NCollection_Array1\s*<\s*(.+?)\s*>\s*(\w+)\s*;')
ARRAY2_TYPEDEF_PATTERN = re.compile(r'typedef\s+NCollection_Array2\s*<\s*(.+?)\s*>\s*(\w+)\s*;')
SEQUENCE_TYPEDEF_PATTERN = re.compile(r'typedef\s+NCollection_Sequence\s*<\s*(.+?)\s*>\s*(\w+)\s*;')

# Cache for element type lookups
element_type_cache: Dict[str, str] = {}


def find_element_type(underlying_type: str, collection_kind: str) -> Optional[str]:
    """
    Find the element type for an underlying array/sequence type.

    Args:
        underlying_type: The underlying type name (e.g., 'TColStd_Array1OfReal')
        collection_kind: 'Array1', 'Array2', or 'Sequence'

    Returns:
        The element type or None if not found
    """
    if underlying_type in element_type_cache:
        return element_type_cache[underlying_type]

    # Search for the typedef in the source tree
    src_dir = OCCT_ROOT / 'src'

    if collection_kind == 'Array1':
        pattern = ARRAY1_TYPEDEF_PATTERN
    elif collection_kind == 'Array2':
        pattern = ARRAY2_TYPEDEF_PATTERN
    else:
        pattern = SEQUENCE_TYPEDEF_PATTERN

    # Walk through all .hxx files
    for root, dirs, files in os.walk(src_dir):
        for filename in files:
            if filename.endswith('.hxx'):
                filepath = Path(root) / filename
                try:
                    content = filepath.read_text(encoding='utf-8', errors='ignore')
                    for match in pattern.finditer(content):
                        element_type = match.group(1).strip()
                        type_name = match.group(2).strip()
                        element_type_cache[type_name] = element_type
                        if type_name == underlying_type:
                            return element_type
                except Exception:
                    continue

    return element_type_cache.get(underlying_type)


def get_new_template_header(collection_kind: str) -> str:
    """Get the new template header file name."""
    if collection_kind == 'Array1':
        return 'NCollection_HArray1.hxx'
    elif collection_kind == 'Array2':
        return 'NCollection_HArray2.hxx'
    else:
        return 'NCollection_HSequence.hxx'


def get_new_template_class(collection_kind: str) -> str:
    """Get the new template class name."""
    if collection_kind == 'Array1':
        return 'NCollection_HArray1'
    elif collection_kind == 'Array2':
        return 'NCollection_HArray2'
    else:
        return 'NCollection_HSequence'


def migrate_file(filepath: Path, dry_run: bool = False, verbose: bool = False) -> bool:
    """
    Migrate a single file from macro-based to template-based H-collection.

    Returns True if the file was modified.
    """
    try:
        content = filepath.read_text(encoding='utf-8')
    except Exception as e:
        print(f"Error reading {filepath}: {e}", file=sys.stderr)
        return False

    original_content = content
    modified = False

    # Check for each macro type (standard single-line patterns)
    for pattern, collection_kind, old_include in [
        (DEFINE_HARRAY1_PATTERN, 'Array1', 'NCollection_DefineHArray1.hxx'),
        (DEFINE_HARRAY2_PATTERN, 'Array2', 'NCollection_DefineHArray2.hxx'),
        (DEFINE_HSEQUENCE_PATTERN, 'Sequence', 'NCollection_DefineHSequence.hxx'),
    ]:
        match = pattern.search(content)
        if match:
            class_name = match.group(1)
            underlying_type = match.group(2)

            # Find the element type
            element_type = find_element_type(underlying_type, collection_kind)

            if element_type is None:
                print(f"Warning: Could not find element type for {underlying_type} in {filepath}",
                      file=sys.stderr)
                continue

            new_template_header = get_new_template_header(collection_kind)
            new_template_class = get_new_template_class(collection_kind)

            # Build new typedef line
            new_typedef = f"typedef {new_template_class}<{element_type}> {class_name};"

            if verbose:
                print(f"  {filepath.name}:")
                print(f"    Macro: DEFINE_H{collection_kind.upper()}({class_name}, {underlying_type})")
                print(f"    Element type: {element_type}")
                print(f"    New typedef: {new_typedef}")

            # Replace the macro with typedef
            content = pattern.sub(new_typedef, content)

            # Replace the old include with the new one
            content = content.replace(f'#include <{old_include}>', f'#include <{new_template_header}>')

            modified = True

    # Check for multi-line macro patterns
    for pattern, collection_kind, old_include in [
        (DEFINE_HARRAY1_MULTILINE_PATTERN, 'Array1', 'NCollection_DefineHArray1.hxx'),
        (DEFINE_HARRAY2_MULTILINE_PATTERN, 'Array2', 'NCollection_DefineHArray2.hxx'),
        (DEFINE_HSEQUENCE_MULTILINE_PATTERN, 'Sequence', 'NCollection_DefineHSequence.hxx'),
    ]:
        match = pattern.search(content)
        if match:
            class_name = match.group(1)
            underlying_type = match.group(2)

            element_type = find_element_type(underlying_type, collection_kind)

            if element_type is None:
                print(f"Warning: Could not find element type for {underlying_type} in {filepath}",
                      file=sys.stderr)
                continue

            new_template_header = get_new_template_header(collection_kind)
            new_template_class = get_new_template_class(collection_kind)
            new_typedef = f"typedef {new_template_class}<{element_type}> {class_name};"

            if verbose:
                print(f"  {filepath.name}: (multi-line)")
                print(f"    New typedef: {new_typedef}")

            content = pattern.sub(new_typedef, content)
            content = content.replace(f'#include <{old_include}>', f'#include <{new_template_header}>')
            modified = True

    # Check for direct NCollection_Array/Sequence types in macro
    for pattern, collection_kind, old_include in [
        (DEFINE_HARRAY1_DIRECT_PATTERN, 'Array1', 'NCollection_DefineHArray1.hxx'),
        (DEFINE_HARRAY2_DIRECT_PATTERN, 'Array2', 'NCollection_DefineHArray2.hxx'),
        (DEFINE_HSEQUENCE_DIRECT_PATTERN, 'Sequence', 'NCollection_DefineHSequence.hxx'),
    ]:
        match = pattern.search(content)
        if match:
            class_name = match.group(1)
            element_type = match.group(2).strip()

            new_template_header = get_new_template_header(collection_kind)
            new_template_class = get_new_template_class(collection_kind)
            new_typedef = f"typedef {new_template_class}<{element_type}> {class_name};"

            if verbose:
                print(f"  {filepath.name}: (direct type)")
                print(f"    Element type: {element_type}")
                print(f"    New typedef: {new_typedef}")

            content = pattern.sub(new_typedef, content)
            content = content.replace(f'#include <{old_include}>', f'#include <{new_template_header}>')
            modified = True

    if modified and content != original_content:
        if not dry_run:
            try:
                filepath.write_text(content, encoding='utf-8')
                if verbose:
                    print(f"  Modified: {filepath}")
            except Exception as e:
                print(f"Error writing {filepath}: {e}", file=sys.stderr)
                return False
        else:
            print(f"Would modify: {filepath}")
        return True

    return False


def find_macro_files() -> List[Path]:
    """Find all files containing DEFINE_HARRAY1, DEFINE_HARRAY2, or DEFINE_HSEQUENCE macros."""
    files = []
    src_dir = OCCT_ROOT / 'src'

    # All patterns to check
    all_patterns = [
        DEFINE_HARRAY1_PATTERN, DEFINE_HARRAY2_PATTERN, DEFINE_HSEQUENCE_PATTERN,
        DEFINE_HARRAY1_MULTILINE_PATTERN, DEFINE_HARRAY2_MULTILINE_PATTERN, DEFINE_HSEQUENCE_MULTILINE_PATTERN,
        DEFINE_HARRAY1_DIRECT_PATTERN, DEFINE_HARRAY2_DIRECT_PATTERN, DEFINE_HSEQUENCE_DIRECT_PATTERN,
    ]

    for root, dirs, filenames in os.walk(src_dir):
        for filename in filenames:
            if filename.endswith('.hxx'):
                filepath = Path(root) / filename
                try:
                    content = filepath.read_text(encoding='utf-8', errors='ignore')
                    if any(p.search(content) for p in all_patterns):
                        files.append(filepath)
                except Exception:
                    continue

    return sorted(files)


def build_element_type_cache():
    """Pre-build the element type cache by scanning all typedef files."""
    print("Building element type cache...")
    src_dir = OCCT_ROOT / 'src'

    patterns = [
        (ARRAY1_TYPEDEF_PATTERN, 'Array1'),
        (ARRAY2_TYPEDEF_PATTERN, 'Array2'),
        (SEQUENCE_TYPEDEF_PATTERN, 'Sequence'),
    ]

    for root, dirs, files in os.walk(src_dir):
        for filename in files:
            if filename.endswith('.hxx'):
                filepath = Path(root) / filename
                try:
                    content = filepath.read_text(encoding='utf-8', errors='ignore')
                    for pattern, kind in patterns:
                        for match in pattern.finditer(content):
                            element_type = match.group(1).strip()
                            type_name = match.group(2).strip()
                            element_type_cache[type_name] = element_type
                except Exception:
                    continue

    print(f"  Found {len(element_type_cache)} type definitions")


def main():
    parser = argparse.ArgumentParser(
        description='Migrate OCCT H-collection macros to templates')
    parser.add_argument('--dry-run', action='store_true',
                        help='Show what would be done without making changes')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Show detailed output')
    parser.add_argument('--file', type=str,
                        help='Migrate only a specific file')
    args = parser.parse_args()

    # Build the element type cache first
    build_element_type_cache()

    if args.file:
        files = [Path(args.file)]
    else:
        files = find_macro_files()

    print(f"Found {len(files)} files with H-collection macros")

    modified_count = 0
    for filepath in files:
        if migrate_file(filepath, args.dry_run, args.verbose):
            modified_count += 1

    print(f"\n{'Would modify' if args.dry_run else 'Modified'} {modified_count} files")

    return 0


if __name__ == '__main__':
    sys.exit(main())
