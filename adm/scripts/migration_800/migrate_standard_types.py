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
Standard_* Type Migration Script for OCCT 8.0.0

Type Transformations:
- Standard_Boolean -> bool
- Standard_Integer -> int
- Standard_Real -> double
- Standard_ShortReal -> float
- Standard_Character -> char
- Standard_Byte -> uint8_t
- Standard_Address -> void*
- const Standard_Address -> void* const (preserves const pointer semantics)
- Standard_Size -> size_t
- Standard_Time -> std::time_t
- Standard_ExtCharacter -> char16_t
- Standard_Utf16Char -> char16_t
- Standard_Utf32Char -> char32_t
- Standard_WideChar -> wchar_t
- Standard_Utf8Char -> char
- Standard_Utf8UChar -> unsigned char
- Standard_CString -> const char*
- Standard_ExtString -> const char16_t*
- Standard_True -> true
- Standard_False -> false

Special handling:
- Function-style casts like Standard_CString(x) -> static_cast<const char*>(x)
- Multi-token type casts like Standard_Utf8UChar(x) -> static_cast<unsigned char>(x)
- const Standard_CString -> const char* (avoids 'const const char*')

Usage:
    python3 migrate_standard_types.py [options] <src_directory>

Options:
    --dry-run           Preview changes without modifying files
    --verbose           Show detailed progress
    --types-only        Only migrate types (not includes)
    --includes-only     Only clean up includes
    --no-values         Don't replace Standard_True/False
    --file=PATH         Process only a specific file
    --skip-address      Don't replace Standard_Address
"""

import os
import re
import sys
import argparse
from pathlib import Path
from typing import List, Tuple, Dict, Optional
from dataclasses import dataclass, field


@dataclass
class TypeMigrationResult:
    """Result of processing a single file."""
    file_path: str
    type_replacements: Dict[str, int] = field(default_factory=dict)
    include_removals: int = 0
    modified: bool = False
    error: Optional[str] = None

    @property
    def total_replacements(self) -> int:
        return sum(self.type_replacements.values())


class StandardTypeMigrator:
    """Migrator for Standard_* types to native C++ types."""

    # Files to skip (these define the types)
    SKIP_FILES = {
        'Standard_TypeDef.hxx',
        'Standard_Boolean.hxx',
        'Standard_Integer.hxx',
        'Standard_Real.hxx',
        'Standard_Real.cxx',
        'Standard_ShortReal.hxx',
        'Standard_Character.hxx',
        'Standard_Byte.hxx',
        'Standard_PrimitiveTypes.hxx',
        'Standard_Macro.hxx',
        'Standard_CString.hxx',
        # Note: Standard_CStringHasher.hxx is NOT skipped - we fix its parameter type
    }

    # Type mappings: old -> new
    TYPE_MAPPINGS = {
        'Standard_Boolean': 'bool',
        'Standard_Integer': 'int',
        'Standard_UInteger': 'unsigned int',
        'Standard_Real': 'double',
        'Standard_ShortReal': 'float',
        'Standard_Character': 'char',
        'Standard_Byte': 'uint8_t',
        'Standard_Address': 'void*',
        'Standard_Size': 'size_t',
        'Standard_Time': 'std::time_t',
        # C++17 guaranteed character types
        'Standard_ExtCharacter': 'char16_t',
        'Standard_Utf16Char': 'char16_t',
        'Standard_Utf32Char': 'char32_t',
        'Standard_WideChar': 'wchar_t',
        'Standard_Utf8Char': 'char',
        'Standard_Utf8UChar': 'unsigned char',
        # String types
        'Standard_CString': 'const char*',
        'Standard_ExtString': 'const char16_t*',
    }

    # Value mappings
    VALUE_MAPPINGS = {
        'Standard_True': 'true',
        'Standard_False': 'false',
    }

    # Includes that should NOT be removed - they contain utility functions!
    # Standard_Integer.hxx: Abs(), IsEven(), IsOdd(), Square(), IntegerFirst(), IntegerLast()
    # Standard_Real.hxx: RealSmall(), RealEpsilon(), RealFirst(), RealLast(), etc.
    # Keep this set empty to prevent accidental removal
    REMOVABLE_INCLUDES = set()  # Disabled - do NOT remove these includes

    def __init__(self, src_dir: str, dry_run: bool = False, verbose: bool = False,
                 skip_address: bool = False, skip_values: bool = False):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.verbose = verbose
        self.skip_address = skip_address
        self.skip_values = skip_values
        self.results: List[TypeMigrationResult] = []

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

    def migrate_types(self, content: str) -> Tuple[str, Dict[str, int]]:
        """
        Replace Standard_* types with native C++ types.

        Careful handling:
        - Use word boundaries to avoid partial matches
        - Standard_Integer should not match Standard_IntegerHasher
        - Handle pointer/reference contexts correctly
        - Do NOT replace inside #include statements
        - 'const Standard_Address' -> 'void* const' (not 'const void*')
        """
        counts = {t: 0 for t in self.TYPE_MAPPINGS}
        counts['const Standard_Address'] = 0  # Track const Address separately

        # Build mappings to use (excluding Standard_Address - handled specially)
        mappings = dict(self.TYPE_MAPPINGS)
        if 'Standard_Address' in mappings:
            del mappings['Standard_Address']

        # Process line by line to skip include statements
        lines = content.split('\n')
        new_lines = []

        for line in lines:
            # Skip include lines - don't modify them
            if re.match(r'\s*#\s*include\s*[<"]', line):
                new_lines.append(line)
                continue

            # Skip #define X Standard_Address patterns - these are macro aliases
            # that rely on typedef semantics for const placement
            if re.match(r'\s*#\s*define\s+\w+\s+Standard_Address\s*$', line):
                new_lines.append(line)
                continue

            modified_line = line

            # Handle Standard_Address specially (if not skipped)
            if not self.skip_address:
                # First: replace 'const Standard_Address' with 'void* const'
                # This must be done BEFORE replacing 'Standard_Address'
                const_addr_pattern = r'\bconst\s+Standard_Address\b'
                const_matches = re.findall(const_addr_pattern, modified_line)
                if const_matches:
                    counts['const Standard_Address'] += len(const_matches)
                    modified_line = re.sub(const_addr_pattern, 'void* const', modified_line)

                # Handle Standard_Address function-style casts specially BEFORE general replacement
                # Standard_Address(expr) -> (void*)(expr)
                # This is needed because 'void*(expr)' is invalid C++ syntax
                # We use C-style cast to preserve original semantics (can cast away const)
                # But we must NOT match:
                #   - function pointer typedefs: typedef Standard_Address(*FuncPtr)(...)
                #   - type conversion operators: operator Standard_Address()
                # So we only match when followed by ( but NOT (* or followed by )
                # Also skip if preceded by 'operator ' (conversion operators)
                if 'operator Standard_Address' not in modified_line:
                    addr_cast_pattern = r'\bStandard_Address\s*\((?![*)])'
                    if re.search(addr_cast_pattern, modified_line):
                        addr_cast_matches = len(re.findall(addr_cast_pattern, modified_line))
                        counts['Standard_Address'] = counts.get('Standard_Address', 0) + addr_cast_matches
                        modified_line = re.sub(addr_cast_pattern, '(void*)(', modified_line)

                # Then: replace remaining 'Standard_Address' with 'void*'
                addr_pattern = r'\bStandard_Address\b'
                addr_matches = re.findall(addr_pattern, modified_line)
                if addr_matches:
                    counts['Standard_Address'] += len(addr_matches)
                    modified_line = re.sub(addr_pattern, 'void*', modified_line)

            # Handle Standard_Byte function-style casts specially BEFORE general replacement
            # Standard_Byte(x) -> static_cast<uint8_t>(x)
            # This is needed because 'uint8_t(x)' may not work on all compilers
            byte_cast_pattern = r'\bStandard_Byte\s*\('
            if re.search(byte_cast_pattern, modified_line):
                byte_cast_matches = len(re.findall(byte_cast_pattern, modified_line))
                counts['Standard_Byte'] = counts.get('Standard_Byte', 0) + byte_cast_matches
                modified_line = re.sub(byte_cast_pattern, 'static_cast<uint8_t>(', modified_line)

            # Handle Standard_Utf8UChar function-style casts specially BEFORE general replacement
            # Standard_Utf8UChar(x) -> static_cast<unsigned char>(x)
            # This is needed because 'unsigned char(x)' is invalid C++ syntax
            utf8uchar_cast_pattern = r'\bStandard_Utf8UChar\s*\('
            if re.search(utf8uchar_cast_pattern, modified_line):
                utf8uchar_cast_matches = len(re.findall(utf8uchar_cast_pattern, modified_line))
                counts['Standard_Utf8UChar'] = counts.get('Standard_Utf8UChar', 0) + utf8uchar_cast_matches
                modified_line = re.sub(utf8uchar_cast_pattern, 'static_cast<unsigned char>(', modified_line)

            # Handle multi-variable declarations for pointer types BEFORE general replacement
            # Standard_CString s1, s2; -> const char* s1; const char* s2;
            # This is needed because 'const char* s1, s2;' makes s2 a char, not a pointer
            # Pattern: Standard_CString followed by identifier, comma, identifier(s), semicolon
            # Must not be inside function parameters (no closing paren before semicolon)
            multivar_pattern = r'^(\s*)Standard_CString\s+(\w+(?:\s*,\s*\w+)+)\s*;'
            multivar_match = re.match(multivar_pattern, modified_line)
            if multivar_match and '(' not in modified_line:
                indent = multivar_match.group(1)
                vars_str = multivar_match.group(2)
                var_names = [v.strip() for v in vars_str.split(',')]
                # Create separate declarations
                new_decls = [f'{indent}const char* {v};' for v in var_names]
                modified_line = '\n'.join(new_decls)
                counts['Standard_CString'] = counts.get('Standard_CString', 0) + len(var_names)

            # Same for Standard_ExtString
            multivar_ext_pattern = r'^(\s*)Standard_ExtString\s+(\w+(?:\s*,\s*\w+)+)\s*;'
            multivar_ext_match = re.match(multivar_ext_pattern, modified_line)
            if multivar_ext_match and '(' not in modified_line:
                indent = multivar_ext_match.group(1)
                vars_str = multivar_ext_match.group(2)
                var_names = [v.strip() for v in vars_str.split(',')]
                new_decls = [f'{indent}const char16_t* {v};' for v in var_names]
                modified_line = '\n'.join(new_decls)
                counts['Standard_ExtString'] = counts.get('Standard_ExtString', 0) + len(var_names)

            # Handle Standard_CString function-style casts specially BEFORE general replacement
            # Standard_CString(x) -> static_cast<const char*>(x)
            # This is needed because 'const char*(x)' is invalid C++ syntax
            # Skip if it looks like:
            #   - typedef: 'typedef ... Standard_CString(...'
            #   - conversion operator: 'operator Standard_CString()'
            if not re.match(r'\s*typedef\b', modified_line) and 'operator Standard_CString' not in modified_line:
                cstring_cast_pattern = r'\bStandard_CString\s*\('
                if re.search(cstring_cast_pattern, modified_line):
                    cstring_cast_matches = len(re.findall(cstring_cast_pattern, modified_line))
                    counts['Standard_CString'] = counts.get('Standard_CString', 0) + cstring_cast_matches
                    modified_line = re.sub(cstring_cast_pattern, 'static_cast<const char*>(', modified_line)

            # Handle Standard_ExtString function-style casts specially BEFORE general replacement
            # Standard_ExtString(x) -> static_cast<const char16_t*>(x)
            # Skip conversion operators like 'operator Standard_ExtString()'
            if not re.match(r'\s*typedef\b', modified_line) and 'operator Standard_ExtString' not in modified_line:
                extstring_cast_pattern = r'\bStandard_ExtString\s*\('
                if re.search(extstring_cast_pattern, modified_line):
                    extstring_cast_matches = len(re.findall(extstring_cast_pattern, modified_line))
                    counts['Standard_ExtString'] = counts.get('Standard_ExtString', 0) + extstring_cast_matches
                    modified_line = re.sub(extstring_cast_pattern, 'static_cast<const char16_t*>(', modified_line)

            # Handle 'const Standard_CString' specially to avoid 'const const char*'
            # Standard_CString is already 'const char*', so 'const Standard_CString' becomes just 'const char*'
            const_cstring_pattern = r'\bconst\s+Standard_CString\b'
            if re.search(const_cstring_pattern, modified_line):
                const_cstring_matches = len(re.findall(const_cstring_pattern, modified_line))
                counts['Standard_CString'] = counts.get('Standard_CString', 0) + const_cstring_matches
                modified_line = re.sub(const_cstring_pattern, 'const char*', modified_line)

            # Handle 'const Standard_ExtString' specially to avoid 'const const char16_t*'
            const_extstring_pattern = r'\bconst\s+Standard_ExtString\b'
            if re.search(const_extstring_pattern, modified_line):
                const_extstring_matches = len(re.findall(const_extstring_pattern, modified_line))
                counts['Standard_ExtString'] = counts.get('Standard_ExtString', 0) + const_extstring_matches
                modified_line = re.sub(const_extstring_pattern, 'const char16_t*', modified_line)

            # Apply other type replacements
            for old_type, new_type in mappings.items():
                pattern = rf'\b{old_type}\b'
                matches = re.findall(pattern, modified_line)
                if matches:
                    counts[old_type] += len(matches)
                    modified_line = re.sub(pattern, new_type, modified_line)

            new_lines.append(modified_line)

        # Log the counts
        for old_type, count in counts.items():
            if count > 0:
                if old_type == 'const Standard_Address':
                    self.log(f"    const Standard_Address -> void* const: {count}")
                elif old_type == 'Standard_Address':
                    self.log(f"    Standard_Address -> void*: {count}")
                else:
                    self.log(f"    {old_type} -> {mappings.get(old_type, '?')}: {count}")

        # Filter out zero counts
        counts = {k: v for k, v in counts.items() if v > 0}

        return '\n'.join(new_lines), counts

    def migrate_values(self, content: str) -> Tuple[str, Dict[str, int]]:
        """Replace Standard_True/False with true/false."""
        counts = {}

        if self.skip_values:
            return content, counts

        for old_val, new_val in self.VALUE_MAPPINGS.items():
            pattern = rf'\b{old_val}\b'
            matches = re.findall(pattern, content)
            if matches:
                count = len(matches)
                content = re.sub(pattern, new_val, content)
                counts[old_val] = count
                self.log(f"    {old_val} -> {new_val}: {count}")

        return content, counts

    def clean_includes(self, content: str) -> Tuple[str, int]:
        """
        Remove unnecessary Standard_*.hxx includes.

        These headers now only contain:
        #include <Standard_TypeDef.hxx>

        So they can be safely removed if Standard_TypeDef.hxx or other
        headers that include it are already present.
        """
        count = 0
        lines = content.split('\n')
        new_lines = []

        for line in lines:
            should_keep = True

            for removable in self.REMOVABLE_INCLUDES:
                if f'#include <{removable}>' in line or f'#include "{removable}"' in line:
                    should_keep = False
                    count += 1
                    self.log(f"    Removing: #include <{removable}>")
                    break

            if should_keep:
                new_lines.append(line)

        return '\n'.join(new_lines), count

    def apply_specific_fixes(self, file_path: Path, content: str) -> Tuple[str, int]:
        """
        Apply file-specific fixes for known issues.
        Returns (modified_content, fix_count).
        """
        fix_count = 0

        # Fix FSD files: Add 'static' to MAGICNUMBER to avoid duplicate symbol
        if file_path.name in ('FSD_BinaryFile.cxx', 'FSD_CmpFile.cxx', 'FSD_File.cxx'):
            # Pattern: 'const char* MAGICNUMBER' at start of line (with optional whitespace)
            pattern = r'^(const\s+char\s*\*\s*MAGICNUMBER\s*=)'
            if re.search(pattern, content, re.MULTILINE):
                content = re.sub(pattern, r'static \1', content, flags=re.MULTILINE)
                fix_count += 1
                self.log(f"    Added 'static' to MAGICNUMBER declaration")

            # Also fix other global variables in FSD_File.cxx
            if file_path.name == 'FSD_File.cxx':
                pattern2 = r'^(const\s+char\s*\*\s*ENDOFNORMALEXTENDEDSECTION\s*=)'
                if re.search(pattern2, content, re.MULTILINE):
                    content = re.sub(pattern2, r'static \1', content, flags=re.MULTILINE)
                    fix_count += 1
                pattern3 = r'^(const\s+int\s+SIZEOFNORMALEXTENDEDSECTION\s*=)'
                if re.search(pattern3, content, re.MULTILINE):
                    content = re.sub(pattern3, r'static \1', content, flags=re.MULTILINE)
                    fix_count += 1

        # Fix Standard_CStringHasher: Change 'const char*&' to 'const char*'
        if file_path.name == 'Standard_CStringHasher.hxx':
            pattern = r'const\s+char\s*\*\s*&\s+theString'
            if re.search(pattern, content):
                content = re.sub(pattern, 'const char* theString', content)
                fix_count += 1
                self.log(f"    Fixed Standard_CStringHasher parameter type")

        return content, fix_count

    def process_file(self, file_path: Path, types_only: bool = False,
                     includes_only: bool = False) -> TypeMigrationResult:
        """Process a single file."""
        result = TypeMigrationResult(file_path=str(file_path))

        if self.should_skip(file_path):
            self.log(f"Skipping: {file_path}")
            return result

        self.log(f"Processing: {file_path}")

        try:
            content = self.read_file(file_path)
            original = content

            if not includes_only:
                # Migrate types
                content, type_counts = self.migrate_types(content)
                result.type_replacements.update(type_counts)

                # Migrate values (Standard_True/False)
                content, value_counts = self.migrate_values(content)
                result.type_replacements.update(value_counts)

            if not types_only:
                # Clean up includes
                content, result.include_removals = self.clean_includes(content)

            # Apply file-specific fixes (FSD duplicate symbols, CStringHasher, etc.)
            content, specific_fixes = self.apply_specific_fixes(file_path, content)
            if specific_fixes > 0:
                result.type_replacements['specific_fixes'] = specific_fixes

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

    def run(self, types_only: bool = False, includes_only: bool = False,
            single_file: Optional[str] = None):
        """Run the migration."""
        if single_file:
            files = [Path(single_file)]
        else:
            files = self.get_source_files()

        print(f"Processing {len(files)} files...")
        print(f"Dry run: {self.dry_run}")
        if self.skip_address:
            print("Skipping Standard_Address replacement")
        if self.skip_values:
            print("Skipping Standard_True/False replacement")

        total_counts: Dict[str, int] = {}
        total_includes = 0
        modified_files = 0

        for i, file_path in enumerate(files, 1):
            if i % 100 == 0:
                print(f"Progress: {i}/{len(files)}")

            result = self.process_file(file_path, types_only, includes_only)
            self.results.append(result)

            for type_name, count in result.type_replacements.items():
                total_counts[type_name] = total_counts.get(type_name, 0) + count
            total_includes += result.include_removals
            if result.modified:
                modified_files += 1

        # Print summary
        print("\n" + "=" * 50)
        print("Type Migration Summary")
        print("=" * 50)
        print(f"Files processed:      {len(files)}")
        print(f"Files modified:       {modified_files}")
        print(f"Include removals:     {total_includes}")
        print("\nType replacements:")
        for type_name in sorted(total_counts.keys()):
            print(f"  {type_name:25s}: {total_counts[type_name]:>6d}")
        print(f"  {'TOTAL':25s}: {sum(total_counts.values()):>6d}")
        print("=" * 50)

        errors = [r for r in self.results if r.error]
        if errors:
            print(f"\nErrors ({len(errors)}):")
            for r in errors[:10]:
                print(f"  {r.file_path}: {r.error}")


def main():
    parser = argparse.ArgumentParser(description='OCCT Standard_* Type Migration Script')
    parser.add_argument('src_directory', nargs='?', default='.',
                        help='Source directory to process')
    parser.add_argument('--dry-run', action='store_true',
                        help='Preview changes without modifying files')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Show detailed progress')
    parser.add_argument('--types-only', action='store_true',
                        help='Only migrate types (not includes)')
    parser.add_argument('--includes-only', action='store_true',
                        help='Only clean up includes')
    parser.add_argument('--no-values', action='store_true',
                        help="Don't replace Standard_True/False")
    parser.add_argument('--skip-address', action='store_true',
                        help="Don't replace Standard_Address")
    parser.add_argument('--file', dest='single_file',
                        help='Process only a specific file')

    args = parser.parse_args()

    if args.types_only and args.includes_only:
        print("Error: Cannot specify both --types-only and --includes-only")
        sys.exit(1)

    migrator = StandardTypeMigrator(
        src_dir=args.src_directory,
        dry_run=args.dry_run,
        verbose=args.verbose,
        skip_address=args.skip_address,
        skip_values=args.no_values
    )

    migrator.run(
        types_only=args.types_only,
        includes_only=args.includes_only,
        single_file=args.single_file
    )


if __name__ == '__main__':
    main()
