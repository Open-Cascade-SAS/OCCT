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
OCCT 8.0.0 Typedef Header Removal Script

Removes typedef-only header files identified by collect_typedefs.py.
Also updates FILES.cmake files to remove references to deleted headers.

Usage:
    python3 remove_typedef_headers.py [options] <src_directory>

Options:
    --input FILE        Input JSON from collect_typedefs.py (default: collected_typedefs.json)
    --dry-run           Preview changes without modifying/deleting files
    --verbose           Show detailed progress
    --keep-backup       Move files to backup directory instead of deleting
    --jobs N            Number of parallel jobs (default: CPU count, use 1 for debugging)
"""

import os
import re
import json
import shutil
import argparse
from pathlib import Path
from typing import Dict, List, Set, Optional, Tuple
from dataclasses import dataclass, field
from concurrent.futures import ProcessPoolExecutor, as_completed

# Global state for parallel workers
HEADER_REPLACEMENTS: Dict[str, List[str]] = {}
DRY_RUN: bool = False

SKIP_DIRS = {'build', 'install', '.git', '__pycache__', 'mac64', 'win64', 'lin64'}
SOURCE_EXTENSIONS = {'.cxx', '.hxx', '.pxx', '.lxx', '.gxx', '.h', '.cpp', '.c'}


def init_worker(header_replacements: Dict[str, List[str]], dry_run: bool):
    """Initialize worker with shared data."""
    global HEADER_REPLACEMENTS, DRY_RUN
    HEADER_REPLACEMENTS = header_replacements
    DRY_RUN = dry_run


def process_file(file_path_str: str) -> Dict:
    """
    Process a single file - replace includes of typedef headers.
    Returns dict with file path, replacement count, and modified status.
    """
    result = {
        'file_path': file_path_str,
        'replacements': 0,
        'modified': False,
        'error': None
    }

    try:
        with open(file_path_str, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()
    except Exception as e:
        result['error'] = str(e)
        return result

    original = content
    replacements = 0

    for old_header, new_headers in HEADER_REPLACEMENTS.items():
        # Match #include <OldHeader.hxx> or #include "OldHeader.hxx"
        pattern = rf'#\s*include\s*[<"]({re.escape(old_header)})[>"]'

        def make_replacement(match):
            nonlocal replacements
            replacements += 1
            # Build replacement includes
            new_includes = '\n'.join(f'#include <{h}>' for h in new_headers)
            return new_includes

        content = re.sub(pattern, make_replacement, content)

    if content != original:
        result['modified'] = True
        result['replacements'] = replacements
        if not DRY_RUN:
            with open(file_path_str, 'w', encoding='utf-8') as f:
                f.write(content)

    return result


def get_source_files(src_dir: Path) -> List[str]:
    """Get all source files to process."""
    files = []
    for root, dirs, filenames in os.walk(src_dir):
        # Skip unwanted directories
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]

        for filename in filenames:
            if any(filename.endswith(ext) for ext in SOURCE_EXTENSIONS):
                files.append(str(Path(root) / filename))
    return files


@dataclass
class RemovalResult:
    """Result of removal operations."""
    headers_removed: List[str] = field(default_factory=list)
    cmake_files_updated: List[str] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)


class TypedefHeaderRemover:
    """Removes typedef-only headers and updates build files."""

    def __init__(self, src_dir: str, collected_data: Dict,
                 dry_run: bool = False, verbose: bool = False,
                 keep_backup: bool = False, jobs: int = 4):
        self.src_dir = Path(src_dir)
        self.dry_run = dry_run
        self.verbose = verbose
        self.keep_backup = keep_backup
        self.jobs = jobs
        self.backup_dir = Path('backup_typedef_headers')

        # Get typedef-only headers from collected data
        self.typedef_only_headers: List[str] = collected_data.get('typedef_only_headers', [])

        # Build a map from header name to replacement includes
        self.header_replacements: Dict[str, List[str]] = {}

        # Set of headers that are being removed (by basename for quick lookup)
        removed_header_basenames = {os.path.basename(h) for h in self.typedef_only_headers}

        # First pass: read each typedef header to get its raw includes
        raw_includes: Dict[str, List[str]] = {}
        for header_path in self.typedef_only_headers:
            header_name = os.path.basename(header_path)
            full_path = self.src_dir / header_path

            if full_path.exists():
                try:
                    content = full_path.read_text(encoding='utf-8', errors='replace')
                    includes = []
                    include_pattern = re.compile(r'#\s*include\s*[<"]([^>"]+)[>"]')
                    for match in include_pattern.finditer(content):
                        inc_file = match.group(1)
                        # Keep all includes (including NCollection ones)
                        includes.append(inc_file)
                    raw_includes[header_name] = includes
                except Exception:
                    raw_includes[header_name] = []
            else:
                raw_includes[header_name] = []

        # Second pass: resolve transitive dependencies
        def resolve_includes(header_name: str, visited: Set[str]) -> List[str]:
            if header_name in visited:
                return []  # Avoid circular references
            visited.add(header_name)

            resolved = []
            for inc in raw_includes.get(header_name, []):
                if inc in removed_header_basenames:
                    # This include is also being removed - resolve it transitively
                    resolved.extend(resolve_includes(inc, visited.copy()))
                else:
                    if inc not in resolved:
                        resolved.append(inc)
            return resolved

        for header_path in self.typedef_only_headers:
            header_name = os.path.basename(header_path)
            resolved = resolve_includes(header_name, set())
            if resolved:
                # Remove duplicates while preserving order
                seen = set()
                unique_resolved = []
                for inc in resolved:
                    if inc not in seen:
                        seen.add(inc)
                        unique_resolved.append(inc)
                self.header_replacements[header_name] = unique_resolved

        print(f"Found {len(self.typedef_only_headers)} typedef-only headers to remove")
        print(f"Built include replacement map for {len(self.header_replacements)} headers")

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def update_cmake_file(self, cmake_path: Path, headers_to_remove: Set[str]) -> bool:
        """
        Update FILES.cmake to remove references to deleted headers.
        Returns True if file was modified.
        """
        try:
            with open(cmake_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {cmake_path}: {e}")
            return False

        original = content
        modified = content

        for header_name in headers_to_remove:
            # Pattern 1: Just the header name on a line
            pattern1 = rf'^\s*{re.escape(header_name)}\s*$'
            modified = re.sub(pattern1, '', modified, flags=re.MULTILINE)

            # Pattern 2: Header with possible variable prefix
            pattern2 = rf'^\s*\$\{{[^}}]+\}}_{re.escape(header_name)}\s*$'
            modified = re.sub(pattern2, '', modified, flags=re.MULTILINE)

        # Clean up multiple blank lines
        modified = re.sub(r'\n{3,}', '\n\n', modified)

        if modified != original:
            if not self.dry_run:
                with open(cmake_path, 'w', encoding='utf-8') as f:
                    f.write(modified)
            return True

        return False

    def replace_all_includes(self) -> Tuple[int, int]:
        """
        Replace all includes of typedef-only headers with their replacement includes.
        Uses parallel processing for speed.
        Returns (total_replacements, files_modified).
        """
        if not self.header_replacements:
            return 0, 0

        print(f"\nReplacing includes of typedef-only headers...")
        print(f"Using {self.jobs} parallel workers")

        # Get all source files
        files = get_source_files(self.src_dir)
        print(f"Processing {len(files)} source files...")

        total_replacements = 0
        files_modified = 0
        processed = 0

        if self.jobs == 1:
            # Sequential processing (for debugging)
            global HEADER_REPLACEMENTS, DRY_RUN
            HEADER_REPLACEMENTS = self.header_replacements
            DRY_RUN = self.dry_run

            for file_path in files:
                processed += 1
                if processed % 2000 == 0:
                    print(f"Progress: {processed}/{len(files)}")

                result = process_file(file_path)
                if result['modified']:
                    files_modified += 1
                    total_replacements += result['replacements']
                    if self.verbose:
                        print(f"  Modified: {result['file_path']}")
        else:
            # Parallel processing
            with ProcessPoolExecutor(
                max_workers=self.jobs,
                initializer=init_worker,
                initargs=(self.header_replacements, self.dry_run)
            ) as executor:
                futures = {executor.submit(process_file, f): f for f in files}

                for future in as_completed(futures):
                    processed += 1
                    if processed % 2000 == 0:
                        print(f"Progress: {processed}/{len(files)}")

                    try:
                        result = future.result()
                        if result['modified']:
                            files_modified += 1
                            total_replacements += result['replacements']
                            if self.verbose:
                                print(f"  Modified: {result['file_path']}")
                    except Exception as e:
                        print(f"Error: {futures[future]}: {e}")

        print(f"Replaced {total_replacements} includes in {files_modified} files")
        return total_replacements, files_modified

    def remove_header(self, header_path: str, result: RemovalResult):
        """Remove a single header file."""
        full_path = self.src_dir / header_path

        if not full_path.exists():
            self.log(f"  Header not found (already removed?): {header_path}")
            return

        self.log(f"  Removing: {header_path}")

        if not self.dry_run:
            if self.keep_backup:
                backup_path = self.backup_dir / header_path
                backup_path.parent.mkdir(parents=True, exist_ok=True)
                shutil.move(str(full_path), str(backup_path))
            else:
                full_path.unlink()

        result.headers_removed.append(header_path)

    def run(self) -> RemovalResult:
        """Run the removal process."""
        result = RemovalResult()

        if not self.typedef_only_headers:
            print("No typedef-only headers to remove.")
            return result

        # First, replace all includes of typedef-only headers
        self.replace_all_includes()

        print(f"\n{'DRY RUN: ' if self.dry_run else ''}Removing {len(self.typedef_only_headers)} typedef-only headers...")

        # Group headers by their directory (for CMAKE updates)
        headers_by_dir: Dict[str, Set[str]] = {}

        for header_path in self.typedef_only_headers:
            dir_path = os.path.dirname(header_path)
            header_name = os.path.basename(header_path)

            if dir_path not in headers_by_dir:
                headers_by_dir[dir_path] = set()
            headers_by_dir[dir_path].add(header_name)

        # Process each directory
        for dir_path, header_names in headers_by_dir.items():
            self.log(f"\nProcessing directory: {dir_path}")

            # Remove the headers
            for header_name in header_names:
                header_path = os.path.join(dir_path, header_name)
                self.remove_header(header_path, result)

            # Update FILES.cmake
            cmake_path = self.src_dir / dir_path / 'FILES.cmake'
            if cmake_path.exists():
                if self.update_cmake_file(cmake_path, header_names):
                    self.log(f"  Updated: {cmake_path}")
                    result.cmake_files_updated.append(str(cmake_path))

        return result


def print_summary(result: RemovalResult, dry_run: bool):
    """Print removal summary."""
    print("\n" + "=" * 60)
    print("TYPEDEF HEADER REMOVAL SUMMARY")
    print("=" * 60)

    if dry_run:
        print("(DRY RUN - no files were modified)")

    print(f"\nHeaders {'to be ' if dry_run else ''}removed: {len(result.headers_removed)}")
    print(f"CMAKE files {'to be ' if dry_run else ''}updated: {len(result.cmake_files_updated)}")

    if result.errors:
        print(f"\nErrors ({len(result.errors)}):")
        for error in result.errors[:10]:
            print(f"  {error}")

    if result.headers_removed:
        print(f"\nRemoved headers (first 20):")
        for header in result.headers_removed[:20]:
            print(f"  {header}")
        if len(result.headers_removed) > 20:
            print(f"  ... and {len(result.headers_removed) - 20} more")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Typedef Header Removal Script'
    )
    parser.add_argument(
        'src_directory',
        nargs='?',
        default='.',
        help='Source directory (default: current directory)'
    )
    parser.add_argument(
        '--input', '-i',
        default='collected_typedefs.json',
        help='Input JSON file from collect_typedefs.py (default: collected_typedefs.json)'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='Preview changes without modifying/deleting files'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show detailed progress'
    )
    parser.add_argument(
        '--keep-backup',
        action='store_true',
        help='Move files to backup directory instead of deleting'
    )
    parser.add_argument(
        '--jobs', '-j',
        type=int,
        default=4,
        help='Number of parallel jobs (default: 4, use 1 for debugging)'
    )

    args = parser.parse_args()

    print("OCCT Typedef Header Remover")
    print("=" * 60)

    # Load collected data
    input_path = Path(args.input)
    if not input_path.exists():
        print(f"Error: {input_path} not found. Run collect_typedefs.py first.")
        return 1

    with open(input_path, 'r', encoding='utf-8') as f:
        collected_data = json.load(f)

    remover = TypedefHeaderRemover(
        src_dir=args.src_directory,
        collected_data=collected_data,
        dry_run=args.dry_run,
        verbose=args.verbose,
        keep_backup=args.keep_backup,
        jobs=args.jobs
    )

    result = remover.run()

    # Print summary
    print_summary(result, args.dry_run)

    # Save results to script directory
    script_dir = Path(__file__).parent
    output_file = script_dir / 'removal_results.json'
    results_dict = {
        'headers_removed': result.headers_removed,
        'cmake_files_updated': result.cmake_files_updated,
        'errors': result.errors,
        'dry_run': args.dry_run
    }
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(results_dict, f, indent=2, ensure_ascii=False)
    print(f"\nResults saved to: {output_file}")

    if args.dry_run:
        print("\nRun without --dry-run to apply changes")
    else:
        if args.keep_backup:
            print(f"\nBackups saved to: backup_typedef_headers/")
        print("\nTypedef header removal complete!")
        print("Don't forget to rebuild and test the project.")


if __name__ == '__main__':
    main()
