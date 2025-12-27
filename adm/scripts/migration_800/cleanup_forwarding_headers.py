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
OCCT 8.0.0 Forwarding Header Cleanup Script

Removes headers that only contain #include directives (forwarding headers).
Replaces usages of these headers with their includes directly.

Usage:
    python3 cleanup_forwarding_headers.py [options] <src_directory>

Options:
    --dry-run    Show what would be done without making changes
    --output     Output JSON file for collected forwarding headers
"""

import argparse
import json
import os
import re
from pathlib import Path
from typing import Dict, List, Set, Tuple


class ForwardingHeaderCleaner:
    """Finds and removes forwarding headers."""

    HEADER_EXTENSIONS = {'.hxx', '.h', '.lxx'}
    SOURCE_EXTENSIONS = {'.cxx', '.hxx', '.pxx', '.lxx', '.gxx', '.h', '.cpp', '.c'}
    SKIP_DIRS = {'build', 'install', '.git', '__pycache__', 'mac64', 'win64', 'lin64'}

    # Pattern to extract #include directives
    INCLUDE_PATTERN = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)

    def __init__(self, src_dir: str, dry_run: bool = False):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.forwarding_headers: Dict[str, List[str]] = {}  # header -> list of includes
        self.all_headers: Set[str] = set()  # all existing headers
        self.file_contents: Dict[str, str] = {}  # filepath -> content (for modified files)

    def find_all_headers(self):
        """Find all existing header files."""
        for root, dirs, files in os.walk(self.src_dir):
            dirs[:] = [d for d in dirs if d not in self.SKIP_DIRS]
            for file in files:
                if any(file.endswith(ext) for ext in self.HEADER_EXTENSIONS):
                    self.all_headers.add(file)

    def is_forwarding_header(self, filepath: Path) -> Tuple[bool, List[str]]:
        """Check if a header is a forwarding header (only contains includes)."""
        try:
            content = filepath.read_text(encoding='utf-8', errors='replace')
        except Exception:
            return False, []

        # Remove copyright comment block at the beginning
        lines = content.split('\n')
        code_lines = []
        in_copyright = True

        for line in lines:
            stripped = line.strip()
            if in_copyright:
                if stripped.startswith('//') or stripped == '':
                    continue
                in_copyright = False
            code_lines.append(line)

        code_content = '\n'.join(code_lines)

        # Remove header guard
        code_content = re.sub(r'#ifndef\s+\w+_HeaderFile\s*\n', '', code_content)
        code_content = re.sub(r'#define\s+\w+_HeaderFile\s*\n', '', code_content)
        code_content = re.sub(r'#endif[^\n]*\n?', '', code_content)

        # Extract includes
        includes = self.INCLUDE_PATTERN.findall(code_content)

        # Remove includes from content
        remaining = self.INCLUDE_PATTERN.sub('', code_content).strip()

        # If only whitespace remains and there are includes, it's a forwarding header
        if not remaining and includes:
            return True, includes

        return False, []

    def find_forwarding_headers(self):
        """Find all forwarding headers."""
        print("Scanning for forwarding headers...")

        for root, dirs, files in os.walk(self.src_dir):
            dirs[:] = [d for d in dirs if d not in self.SKIP_DIRS]
            for file in files:
                if any(file.endswith(ext) for ext in self.HEADER_EXTENSIONS):
                    filepath = Path(root) / file
                    is_fwd, includes = self.is_forwarding_header(filepath)
                    if is_fwd:
                        rel_path = str(filepath.relative_to(self.src_dir))
                        self.forwarding_headers[rel_path] = includes

        print(f"Found {len(self.forwarding_headers)} forwarding headers")

    def resolve_includes_transitively(self) -> Dict[str, List[str]]:
        """Resolve forwarding header includes transitively.

        If forwarding header A includes forwarding header B,
        replace B with B's includes in A's replacement list.
        """
        # Build lookup by basename
        fwd_by_name: Dict[str, List[str]] = {}
        for header_path, includes in self.forwarding_headers.items():
            header_name = os.path.basename(header_path)
            fwd_by_name[header_name] = includes

        forwarding_basenames = set(fwd_by_name.keys())

        def resolve(header_name: str, visited: Set[str]) -> List[str]:
            if header_name in visited:
                return []  # Avoid circular references
            visited.add(header_name)

            includes = fwd_by_name.get(header_name, [])
            resolved = []
            for inc in includes:
                if inc in forwarding_basenames:
                    # This include is also a forwarding header - resolve transitively
                    resolved.extend(resolve(inc, visited.copy()))
                else:
                    if inc not in resolved:
                        resolved.append(inc)
            return resolved

        # Resolve all forwarding headers
        resolved_replacements: Dict[str, List[str]] = {}
        for header_name in fwd_by_name:
            resolved = resolve(header_name, set())
            # Remove duplicates while preserving order
            seen = set()
            unique = []
            for inc in resolved:
                if inc not in seen:
                    seen.add(inc)
                    unique.append(inc)
            resolved_replacements[header_name] = unique

        return resolved_replacements

    def process_all_files(self) -> Tuple[int, int]:
        """Process all source files in a single pass to replace includes."""
        if not self.forwarding_headers:
            return 0, 0

        # Build lookup with transitive resolution
        header_replacements = self.resolve_includes_transitively()

        # Build combined pattern for all forwarding headers
        header_names = list(header_replacements.keys())
        if not header_names:
            return 0, 0

        # Sort by length (longest first) for correct matching
        header_names.sort(key=len, reverse=True)
        pattern = re.compile(
            r'^(\s*)#\s*include\s*[<"](' + '|'.join(re.escape(h) for h in header_names) + r')[>"]',
            re.MULTILINE
        )

        files_updated = 0
        total_replacements = 0

        # Single pass through all source files
        for root, dirs, files in os.walk(self.src_dir):
            dirs[:] = [d for d in dirs if d not in self.SKIP_DIRS]
            for file in files:
                if any(file.endswith(ext) for ext in self.SOURCE_EXTENSIONS):
                    filepath = Path(root) / file
                    try:
                        content = filepath.read_text(encoding='utf-8', errors='replace')
                    except Exception:
                        continue

                    original = content
                    replacements_in_file = 0

                    def replace_include(match):
                        nonlocal replacements_in_file
                        indent = match.group(1)
                        header_name = match.group(2)
                        replacements = header_replacements.get(header_name, [])
                        if replacements:
                            replacements_in_file += 1
                            return '\n'.join(f'{indent}#include <{inc}>' for inc in replacements)
                        return match.group(0)

                    content = pattern.sub(replace_include, content)

                    if content != original:
                        files_updated += 1
                        total_replacements += replacements_in_file
                        if not self.dry_run:
                            filepath.write_text(content, encoding='utf-8')

        return files_updated, total_replacements

    def remove_forwarding_headers(self) -> Tuple[int, int]:
        """Remove forwarding header files and update CMAKE."""
        headers_removed = 0
        cmake_updated = 0

        # Group by directory for CMAKE updates
        headers_by_dir: Dict[str, Set[str]] = {}
        for header_path in self.forwarding_headers:
            dir_path = os.path.dirname(header_path)
            header_name = os.path.basename(header_path)
            if dir_path not in headers_by_dir:
                headers_by_dir[dir_path] = set()
            headers_by_dir[dir_path].add(header_name)

        for dir_path, header_names in headers_by_dir.items():
            # Remove headers
            for header_name in header_names:
                header_path = self.src_dir / dir_path / header_name
                if header_path.exists():
                    if not self.dry_run:
                        header_path.unlink()
                    headers_removed += 1

            # Update FILES.cmake
            cmake_path = self.src_dir / dir_path / 'FILES.cmake'
            if cmake_path.exists():
                try:
                    content = cmake_path.read_text(encoding='utf-8')
                    original = content
                    for header_name in header_names:
                        pattern = re.compile(r'^\s*' + re.escape(header_name) + r'\s*$', re.MULTILINE)
                        content = pattern.sub('', content)
                    content = re.sub(r'\n{3,}', '\n\n', content)
                    if content != original:
                        if not self.dry_run:
                            cmake_path.write_text(content, encoding='utf-8')
                        cmake_updated += 1
                except Exception:
                    pass

        return headers_removed, cmake_updated

    def get_json_data(self) -> dict:
        """Get collected data as JSON-serializable dict."""
        forwarding_list = []
        for header, includes in self.forwarding_headers.items():
            forwarding_list.append({
                'header_file': header,
                'includes': includes
            })

        return {
            'forwarding_headers': forwarding_list,
            'total_count': len(self.forwarding_headers)
        }

    def run(self) -> dict:
        """Run the cleanup."""
        self.find_all_headers()
        self.find_forwarding_headers()

        result = self.get_json_data()

        if not self.forwarding_headers:
            print("No forwarding headers found.")
            return result

        # Replace includes in all files (single pass)
        print(f"\nReplacing includes in source files...")
        files_updated, total_replacements = self.process_all_files()
        print(f"Updated {files_updated} files with {total_replacements} replacements")

        # Remove forwarding headers and update CMAKE
        print(f"\n{'DRY RUN: ' if self.dry_run else ''}Removing {len(self.forwarding_headers)} forwarding headers...")
        headers_removed, cmake_updated = self.remove_forwarding_headers()

        result['headers_removed'] = headers_removed
        result['files_updated'] = files_updated
        result['cmake_updated'] = cmake_updated

        print(f"\n{'='*60}")
        print("FORWARDING HEADER CLEANUP SUMMARY")
        print('='*60)
        print(f"Forwarding headers {'to be ' if self.dry_run else ''}removed: {headers_removed}")
        print(f"Source files updated: {files_updated}")
        print(f"CMAKE files updated: {cmake_updated}")

        return result


def main():
    parser = argparse.ArgumentParser(description='Remove forwarding headers')
    parser.add_argument('src_dir', help='Source directory to process')
    parser.add_argument('--dry-run', action='store_true', help='Show changes without applying')
    parser.add_argument('--output', help='Output JSON file (default: script_dir/forwarding_headers.json)')

    args = parser.parse_args()

    if not os.path.isdir(args.src_dir):
        print(f"Error: {args.src_dir} is not a directory")
        return 1

    cleaner = ForwardingHeaderCleaner(args.src_dir, args.dry_run)
    result = cleaner.run()

    # Save results to JSON
    script_dir = Path(__file__).parent
    output_file = args.output if args.output else script_dir / 'forwarding_headers.json'
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=2, ensure_ascii=False)
    print(f"\nResults saved to: {output_file}")

    return 0


if __name__ == '__main__':
    exit(main())
