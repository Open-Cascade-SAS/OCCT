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
OCCT 8.0.0 Typedef Collection Script

Collects all NCollection typedefs from OCCT source headers and classifies them:
- typedef_only_headers: Headers containing ONLY typedefs (can be removed)
- mixed_headers: Headers containing typedefs AND classes/functions (keep file, replace usages)

Output: collected_typedefs.json

Usage:
    python3 collect_typedefs.py [options] <src_directory>

Options:
    --output FILE       Output JSON file (default: collected_typedefs.json)
    --verbose           Show detailed progress
"""

import os
import re
import json
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass, field, asdict


@dataclass
class TypedefInfo:
    """Information about a single typedef."""
    typedef_name: str
    collection_type: str           # e.g., Map, List, Array1
    template_params: str           # e.g., "BOPDS_Pair"
    full_type: str                 # e.g., "NCollection_Map<BOPDS_Pair>"
    is_iterator: bool
    line_text: str                 # Original line text
    header_file: str               # Relative path to header
    namespace: str = ""            # Namespace context (empty if global)


@dataclass
class HeaderInfo:
    """Information about a header file containing typedefs."""
    relative_path: str
    is_typedef_only: bool          # True if header contains ONLY typedefs
    typedefs: List[TypedefInfo] = field(default_factory=list)
    includes: List[str] = field(default_factory=list)  # List of #include statements
    has_class: bool = False
    has_function: bool = False
    has_template_class: bool = False
    has_enum: bool = False
    has_other_content: bool = False


class TypedefCollector:
    """Collects and classifies NCollection typedefs."""

    # Directories to skip
    SKIP_DIRS = {'install', 'build', 'mac64', 'win64', 'lin64', '.git', '__pycache__'}

    # File extensions to process for typedef collection (public headers only)
    # NOTE: .cxx and .pxx files are excluded - their typedefs are internal/implementation details
    HEADER_EXTENSIONS = {'.hxx', '.lxx', '.h'}

    # Pattern for NCollection container typedef (handles nested templates like occ::handle<T>)
    # typedef NCollection_Map<BOPDS_Pair> BOPDS_MapOfPair;
    # typedef NCollection_Array2<occ::handle<Standard_Transient>> TColStd_Array2OfTransient;
    CONTAINER_TYPEDEF_PATTERN = re.compile(
        r'^typedef\s+NCollection_(\w+)\s*<(.+?)>\s+(\w+)\s*;',
        re.MULTILINE
    )

    # Pattern for NCollection iterator typedef (handles nested templates)
    # typedef NCollection_Map<BOPDS_Pair>::Iterator BOPDS_MapIteratorOfMapOfPair;
    # Also handles: typedef NCollection_Map<...>::\n  Iterator Name;
    ITERATOR_TYPEDEF_PATTERN = re.compile(
        r'^typedef\s+NCollection_(\w+)\s*<(.+?)>::\s*Iterator\s+(\w+)\s*;',
        re.MULTILINE
    )

    # Pattern for typedef-on-typedef (alias typedef)
    # typedef ExistingTypedef NewName;
    ALIAS_TYPEDEF_PATTERN = re.compile(
        r'^typedef\s+(\w+)\s+(\w+)\s*;',
        re.MULTILINE
    )

    # Pattern for iterator typedef referencing another typedef
    # typedef TypedefName::Iterator NewIteratorName;
    TYPEDEF_ITERATOR_PATTERN = re.compile(
        r'^typedef\s+(\w+)::Iterator\s+(\w+)\s*;',
        re.MULTILINE
    )

    # Patterns to detect non-typedef content
    CLASS_PATTERN = re.compile(r'^class\s+\w+', re.MULTILINE)
    STRUCT_PATTERN = re.compile(r'^struct\s+\w+', re.MULTILINE)
    TEMPLATE_CLASS_PATTERN = re.compile(r'^\s*template\s*<[^>]*>\s*class\s+\w+', re.MULTILINE)
    # Also detect template struct (used in namespaces for type traits)
    TEMPLATE_STRUCT_PATTERN = re.compile(r'^\s*template\s*<[^>]*>\s*struct\s+\w+', re.MULTILINE)
    # Detect template specializations
    TEMPLATE_SPEC_PATTERN = re.compile(r'^\s*template\s*<\s*>\s*struct\s+\w+', re.MULTILINE)
    FUNCTION_PATTERN = re.compile(r'^(?:Standard_EXPORT\s+)?(?:\w+::\w+|\w+)\s+\w+\s*\([^;]*\)\s*(?:const)?\s*(?:override)?\s*;', re.MULTILINE)
    ENUM_PATTERN = re.compile(r'^enum\s+(?:class\s+)?\w+', re.MULTILINE)
    DEFINE_HANDLE_PATTERN = re.compile(r'^DEFINE_\w+HANDLE\s*\(', re.MULTILINE)
    DEFINE_HARRAY_PATTERN = re.compile(r'^DEFINE_HARRAY[12]\s*\(', re.MULTILINE)

    # Pattern to find all typedef statements (not just NCollection ones)
    ANY_TYPEDEF_PATTERN = re.compile(r'^typedef\s+.+;\s*$', re.MULTILINE)

    # Pattern to extract #include statements
    INCLUDE_PATTERN = re.compile(r'^\s*#include\s*[<"]([^>"]+)[>"]', re.MULTILINE)

    def __init__(self, src_dir: str, verbose: bool = False):
        self.src_dir = Path(src_dir)
        self.verbose = verbose
        self.headers: Dict[str, HeaderInfo] = {}
        self.all_typedefs: List[TypedefInfo] = []
        # Map of typedef_name -> TypedefInfo for resolving aliases
        self.typedef_map: Dict[str, TypedefInfo] = {}
        # Store alias typedefs for second pass
        self.pending_aliases: List[Tuple[str, str, str, str]] = []  # (source_type, new_name, line_text, header_path)
        # Store iterator typedefs that reference other typedefs (TypedefName::Iterator)
        self.pending_iterators: List[Tuple[str, str, str, str]] = []  # (base_typedef, new_name, line_text, header_path)

    def log(self, message: str):
        """Print message if verbose."""
        if self.verbose:
            print(message)

    def should_skip_dir(self, path: Path) -> bool:
        """Check if directory should be skipped."""
        return any(skip in path.parts for skip in self.SKIP_DIRS)

    def get_source_files(self) -> List[Path]:
        """Get all header files (excluding .cxx - internal typedefs)."""
        files = []
        for ext in self.HEADER_EXTENSIONS:
            pattern = f'*{ext}'
            for file in self.src_dir.rglob(pattern):
                if not self.should_skip_dir(file):
                    files.append(file)
        return sorted(files)

    def strip_comments(self, content: str) -> str:
        """Remove C/C++ comments from content for analysis."""
        # Remove single-line comments
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        # Remove multi-line comments
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        return content

    def normalize_multiline_typedefs(self, content: str) -> str:
        """
        Normalize multi-line typedefs into single lines for pattern matching.

        Handles cases like:
        typedef NCollection_DataMap<TopoDS_Shape,
                                    TopOpeBRepDS_ListOfShapeOn1State,
                                    TopTools_ShapeMapHasher>::Iterator
          TopOpeBRepDS_DataMapIteratorOfDataMapOfShapeListOfShapeOn1State;
        """
        result = []
        lines = content.split('\n')
        i = 0
        while i < len(lines):
            line = lines[i]
            stripped = line.strip()

            # Check if this line starts a typedef
            if stripped.startswith('typedef '):
                # Collect all lines until we find the semicolon
                typedef_lines = [line]
                while ';' not in typedef_lines[-1] and i + 1 < len(lines):
                    i += 1
                    typedef_lines.append(lines[i])

                # Join the lines and normalize whitespace
                full_typedef = ' '.join(l.strip() for l in typedef_lines)
                # Collapse multiple spaces to single space (but preserve leading indent)
                leading_spaces = len(line) - len(line.lstrip())
                full_typedef = ' ' * leading_spaces + ' '.join(full_typedef.split())
                result.append(full_typedef)
            else:
                result.append(line)
            i += 1

        return '\n'.join(result)

    def has_non_typedef_content(self, content: str, ncollection_typedef_count: int) -> Tuple[bool, bool, bool, bool, bool]:
        """
        Check if content has non-typedef declarations.
        Returns: (has_class, has_function, has_template_class, has_enum, has_other)

        ncollection_typedef_count is used to check if there are non-NCollection typedefs.
        If there are more total typedefs than NCollection typedefs, has_other will be True.
        """
        # Strip comments for more accurate analysis
        clean_content = self.strip_comments(content)
        # Normalize multi-line typedefs for counting
        normalized_content = self.normalize_multiline_typedefs(clean_content)

        # Check for class/struct declarations
        has_class = bool(self.CLASS_PATTERN.search(clean_content)) or \
                    bool(self.STRUCT_PATTERN.search(clean_content))

        # Check for template classes or template structs
        has_template_class = bool(self.TEMPLATE_CLASS_PATTERN.search(clean_content)) or \
                             bool(self.TEMPLATE_STRUCT_PATTERN.search(clean_content)) or \
                             bool(self.TEMPLATE_SPEC_PATTERN.search(clean_content))

        # Check for function declarations (simple heuristic)
        has_function = bool(self.FUNCTION_PATTERN.search(clean_content))

        # Check for enums
        has_enum = bool(self.ENUM_PATTERN.search(clean_content))

        # Check for DEFINE_HANDLE and DEFINE_HARRAY macros
        has_other = bool(self.DEFINE_HANDLE_PATTERN.search(clean_content)) or \
                    bool(self.DEFINE_HARRAY_PATTERN.search(clean_content))

        # Count total typedefs in file
        all_typedefs = self.ANY_TYPEDEF_PATTERN.findall(normalized_content)
        total_typedef_count = len(all_typedefs)

        # Count TypedefName::Iterator style typedefs (which are collected as pending_iterators)
        # These will be resolved later but should be counted as NCollection typedefs
        typedef_iterator_count = len(self.TYPEDEF_ITERATOR_PATTERN.findall(normalized_content))

        # If there are more typedefs than (direct NCollection + iterator alias) typedefs,
        # mark as having other content
        expected_count = ncollection_typedef_count + typedef_iterator_count
        if total_typedef_count > expected_count:
            has_other = True

        return has_class, has_function, has_template_class, has_enum, has_other

    def find_namespace_at_position(self, content: str, pos: int) -> str:
        """Find the namespace context at a given position in the content."""
        before = content[:pos]

        # Find all namespace declarations with their opening brace positions
        namespace_pattern = re.compile(r'namespace\s+(\w+)\s*\{')
        namespace_starts = []  # List of (brace_pos, ns_name)

        for match in namespace_pattern.finditer(before):
            ns_name = match.group(1)
            brace_pos = match.end() - 1  # Position of opening brace
            namespace_starts.append((brace_pos, ns_name))

        if not namespace_starts:
            return ""

        # Track namespace stack by following braces
        ns_stack = []  # Stack of namespace names
        ns_start_positions = {pos: name for pos, name in namespace_starts}

        brace_depth = 0
        i = 0
        while i < len(before):
            if before[i] == '{':
                if i in ns_start_positions:
                    ns_stack.append(ns_start_positions[i])
                else:
                    ns_stack.append(None)  # Non-namespace brace
                brace_depth += 1
            elif before[i] == '}':
                brace_depth -= 1
                if ns_stack:
                    ns_stack.pop()
            i += 1

        # Return the innermost namespace (ignoring nested non-namespace scopes)
        for ns in reversed(ns_stack):
            if ns is not None:
                return ns

        return ""

    def extract_includes(self, content: str) -> List[str]:
        """Extract all #include statements from content."""
        includes = []
        for match in self.INCLUDE_PATTERN.finditer(content):
            include_path = match.group(1)
            includes.append(include_path)
        return includes

    def extract_typedefs(self, content: str, header_path: str) -> List[TypedefInfo]:
        """Extract NCollection typedefs from content."""
        typedefs = []

        # Normalize multi-line typedefs into single lines for pattern matching
        normalized_content = self.normalize_multiline_typedefs(content)

        # Find container typedefs
        for match in self.CONTAINER_TYPEDEF_PATTERN.finditer(normalized_content):
            collection_type = match.group(1)
            # Normalize whitespace - collapse newlines and multiple spaces to single space
            template_params = ' '.join(match.group(2).split())
            typedef_name = match.group(3)
            namespace = self.find_namespace_at_position(normalized_content, match.start())

            typedef = TypedefInfo(
                typedef_name=typedef_name,
                collection_type=collection_type,
                template_params=template_params,
                full_type=f"NCollection_{collection_type}<{template_params}>",
                is_iterator=False,
                line_text=match.group(0),
                header_file=header_path,
                namespace=namespace
            )
            typedefs.append(typedef)
            self.typedef_map[typedef_name] = typedef
            ns_prefix = f"{namespace}::" if namespace else ""
            self.log(f"    Container: {ns_prefix}{typedef_name} -> NCollection_{collection_type}<{template_params}>")

        # Find iterator typedefs
        for match in self.ITERATOR_TYPEDEF_PATTERN.finditer(normalized_content):
            collection_type = match.group(1)
            # Normalize whitespace - collapse newlines and multiple spaces to single space
            template_params = ' '.join(match.group(2).split())
            typedef_name = match.group(3)
            namespace = self.find_namespace_at_position(normalized_content, match.start())

            typedef = TypedefInfo(
                typedef_name=typedef_name,
                collection_type=collection_type,
                template_params=template_params,
                full_type=f"NCollection_{collection_type}<{template_params}>::Iterator",
                is_iterator=True,
                line_text=match.group(0),
                header_file=header_path,
                namespace=namespace
            )
            typedefs.append(typedef)
            self.typedef_map[typedef_name] = typedef
            ns_prefix = f"{namespace}::" if namespace else ""
            self.log(f"    Iterator: {ns_prefix}{typedef_name} -> NCollection_{collection_type}<{template_params}>::Iterator")

        # Find alias typedefs (typedef-on-typedef)
        # These will be resolved in a second pass after all direct typedefs are collected
        for match in self.ALIAS_TYPEDEF_PATTERN.finditer(normalized_content):
            source_type = match.group(1)
            new_name = match.group(2)
            line_text = match.group(0)

            # Skip if this is already captured as a direct NCollection typedef
            if new_name in self.typedef_map:
                continue

            # Skip if source type starts with NCollection_ (already handled above)
            if source_type.startswith('NCollection_'):
                continue

            # Store for second pass resolution
            self.pending_aliases.append((source_type, new_name, line_text, header_path))

        # Find iterator typedefs that reference other typedefs (TypedefName::Iterator)
        for match in self.TYPEDEF_ITERATOR_PATTERN.finditer(normalized_content):
            base_typedef = match.group(1)
            new_name = match.group(2)
            line_text = match.group(0)

            # Skip if this is already captured as a direct NCollection iterator typedef
            if new_name in self.typedef_map:
                continue

            # Skip if base type starts with NCollection_ (already handled above)
            if base_typedef.startswith('NCollection_'):
                continue

            # Store for second pass resolution
            self.pending_iterators.append((base_typedef, new_name, line_text, header_path))

        return typedefs

    def resolve_aliases(self) -> List[TypedefInfo]:
        """Resolve alias typedefs (typedef-on-typedef) in multiple passes."""
        resolved = []
        max_passes = 10
        pass_num = 0

        while self.pending_aliases and pass_num < max_passes:
            pass_num += 1
            still_pending = []
            resolved_this_pass = 0

            for source_type, new_name, line_text, header_path in self.pending_aliases:
                # Check if source type is now known
                if source_type in self.typedef_map:
                    source_typedef = self.typedef_map[source_type]

                    # Create new typedef info based on the source
                    typedef = TypedefInfo(
                        typedef_name=new_name,
                        collection_type=source_typedef.collection_type,
                        template_params=source_typedef.template_params,
                        full_type=source_typedef.full_type,
                        is_iterator=source_typedef.is_iterator,
                        line_text=line_text,
                        header_file=header_path
                    )
                    resolved.append(typedef)
                    self.typedef_map[new_name] = typedef
                    resolved_this_pass += 1
                    self.log(f"    Alias: {new_name} -> {source_type} -> {source_typedef.full_type}")
                else:
                    # Still can't resolve, keep for next pass
                    still_pending.append((source_type, new_name, line_text, header_path))

            self.pending_aliases = still_pending

            if resolved_this_pass == 0:
                # No progress, remaining aliases reference unknown types
                break

        if self.pending_aliases and self.verbose:
            print(f"  Warning: {len(self.pending_aliases)} alias typedefs could not be resolved")

        # Resolve iterator typedefs (TypedefName::Iterator)
        pass_num = 0
        while self.pending_iterators and pass_num < max_passes:
            pass_num += 1
            still_pending = []
            resolved_this_pass = 0

            for base_typedef, new_name, line_text, header_path in self.pending_iterators:
                # Check if base typedef is now known
                if base_typedef in self.typedef_map:
                    base_info = self.typedef_map[base_typedef]

                    # Create iterator typedef based on the base typedef
                    typedef = TypedefInfo(
                        typedef_name=new_name,
                        collection_type=base_info.collection_type,
                        template_params=base_info.template_params,
                        full_type=f"NCollection_{base_info.collection_type}<{base_info.template_params}>::Iterator",
                        is_iterator=True,
                        line_text=line_text,
                        header_file=header_path
                    )
                    resolved.append(typedef)
                    self.typedef_map[new_name] = typedef
                    resolved_this_pass += 1
                    self.log(f"    TypedefIterator: {new_name} -> {base_typedef}::Iterator -> {typedef.full_type}")
                else:
                    # Still can't resolve, keep for next pass
                    still_pending.append((base_typedef, new_name, line_text, header_path))

            self.pending_iterators = still_pending

            if resolved_this_pass == 0:
                break

        if self.pending_iterators and self.verbose:
            print(f"  Warning: {len(self.pending_iterators)} iterator typedefs could not be resolved")

        return resolved

    def analyze_header(self, file_path: Path) -> Optional[HeaderInfo]:
        """Analyze a single header file."""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return None

        relative_path = str(file_path.relative_to(self.src_dir))

        # Extract typedefs (this also collects pending aliases)
        typedefs = self.extract_typedefs(content, relative_path)

        # Return None only if no direct typedefs AND no pending aliases from this file
        # We'll create HeaderInfo for alias-only headers during resolve_aliases
        if not typedefs:
            return None

        self.log(f"Processing: {relative_path}")

        # Extract includes from the header
        includes = self.extract_includes(content)

        # Check for non-typedef content
        # Pass the count of NCollection typedefs we found to detect non-NCollection typedefs
        has_class, has_function, has_template_class, has_enum, has_other = \
            self.has_non_typedef_content(content, len(typedefs))

        # Determine if this is a typedef-only header
        is_typedef_only = not (has_class or has_function or has_template_class or
                              has_enum or has_other)

        header_info = HeaderInfo(
            relative_path=relative_path,
            is_typedef_only=is_typedef_only,
            typedefs=typedefs,
            includes=includes,
            has_class=has_class,
            has_function=has_function,
            has_template_class=has_template_class,
            has_enum=has_enum,
            has_other_content=has_other
        )

        if is_typedef_only:
            self.log(f"  -> Typedef-only header (can be removed)")
        else:
            reasons = []
            if has_class:
                reasons.append("class")
            if has_template_class:
                reasons.append("template class")
            if has_function:
                reasons.append("function")
            if has_enum:
                reasons.append("enum")
            if has_other:
                reasons.append("other (DEFINE_*)")
            self.log(f"  -> Mixed header (contains: {', '.join(reasons)})")

        return header_info

    def collect(self) -> Dict:
        """Collect all typedefs from source files."""
        files = self.get_source_files()
        print(f"Scanning {len(files)} source files...")

        typedef_only_headers = []
        mixed_headers = []

        for file_path in files:
            header_info = self.analyze_header(file_path)

            if header_info:
                self.headers[header_info.relative_path] = header_info
                self.all_typedefs.extend(header_info.typedefs)

                if header_info.is_typedef_only:
                    typedef_only_headers.append(header_info.relative_path)
                else:
                    mixed_headers.append(header_info.relative_path)

        # Resolve alias typedefs (typedef-on-typedef)
        print(f"Resolving {len(self.pending_aliases)} alias typedefs...")
        resolved_aliases = self.resolve_aliases()
        self.all_typedefs.extend(resolved_aliases)

        # Update headers with resolved aliases
        for typedef in resolved_aliases:
            header_path = typedef.header_file
            if header_path in self.headers:
                self.headers[header_path].typedefs.append(typedef)
            else:
                # New header with only alias typedef - need to check if it has other content
                full_path = self.src_dir / header_path
                includes = []
                try:
                    with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    # Extract includes
                    includes = self.extract_includes(content)
                    # Check for non-typedef content (passing 1 since we have at least 1 typedef)
                    has_class, has_function, has_template_class, has_enum, has_other = \
                        self.has_non_typedef_content(content, 1)
                    is_typedef_only = not (has_class or has_function or has_template_class or
                                          has_enum or has_other)
                except Exception:
                    # If we can't read the file, assume typedef-only
                    is_typedef_only = True
                    has_class = has_function = has_template_class = has_enum = has_other = False

                header_info = HeaderInfo(
                    relative_path=header_path,
                    is_typedef_only=is_typedef_only,
                    typedefs=[typedef],
                    includes=includes,
                    has_class=has_class,
                    has_function=has_function,
                    has_template_class=has_template_class,
                    has_enum=has_enum,
                    has_other_content=has_other
                )
                self.headers[header_path] = header_info
                if is_typedef_only:
                    typedef_only_headers.append(header_path)
                else:
                    mixed_headers.append(header_path)

        print(f"Total typedefs after alias resolution: {len(self.all_typedefs)}")

        # Build result structure
        result = {
            'summary': {
                'total_headers_with_typedefs': len(self.headers),
                'typedef_only_headers_count': len(typedef_only_headers),
                'mixed_headers_count': len(mixed_headers),
                'total_typedefs': len(self.all_typedefs),
                'container_typedefs': sum(1 for t in self.all_typedefs if not t.is_iterator),
                'iterator_typedefs': sum(1 for t in self.all_typedefs if t.is_iterator),
                'alias_typedefs': len(resolved_aliases)
            },
            'typedef_only_headers': typedef_only_headers,
            'mixed_headers': mixed_headers,
            'typedefs': [asdict(t) for t in self.all_typedefs],
            'headers': {path: asdict(info) for path, info in self.headers.items()},
            'collection_types': self._count_collection_types()
        }

        return result

    def _count_collection_types(self) -> Dict[str, int]:
        """Count typedefs by collection type."""
        counts = {}
        for typedef in self.all_typedefs:
            col_type = typedef.collection_type
            counts[col_type] = counts.get(col_type, 0) + 1
        return dict(sorted(counts.items(), key=lambda x: -x[1]))


def print_summary(result: Dict):
    """Print collection summary."""
    print("\n" + "=" * 60)
    print("TYPEDEF COLLECTION SUMMARY")
    print("=" * 60)

    summary = result['summary']
    print(f"\nHeaders with NCollection typedefs: {summary['total_headers_with_typedefs']}")
    print(f"  Typedef-only headers (removable): {summary['typedef_only_headers_count']}")
    print(f"  Mixed headers (keep, replace usages): {summary['mixed_headers_count']}")

    print(f"\nTotal typedefs found: {summary['total_typedefs']}")
    print(f"  Container typedefs: {summary['container_typedefs']}")
    print(f"  Iterator typedefs: {summary['iterator_typedefs']}")
    print(f"  Alias typedefs: {summary.get('alias_typedefs', 0)}")

    print("\nCollection types:")
    for col_type, count in result['collection_types'].items():
        print(f"  NCollection_{col_type:20}: {count:4} typedefs")

    print("\nTypedef-only headers (first 10):")
    for header in result['typedef_only_headers'][:10]:
        print(f"  {header}")
    if len(result['typedef_only_headers']) > 10:
        print(f"  ... and {len(result['typedef_only_headers']) - 10} more")

    print("\nMixed headers (first 10):")
    for header in result['mixed_headers'][:10]:
        print(f"  {header}")
    if len(result['mixed_headers']) > 10:
        print(f"  ... and {len(result['mixed_headers']) - 10} more")


def main():
    parser = argparse.ArgumentParser(
        description='OCCT 8.0.0 Typedef Collection Script'
    )
    parser.add_argument(
        'src_directory',
        nargs='?',
        default='.',
        help='Source directory to scan (default: current directory)'
    )
    parser.add_argument(
        '--output', '-o',
        default='collected_typedefs.json',
        help='Output JSON file (default: collected_typedefs.json)'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show detailed progress'
    )

    args = parser.parse_args()

    print("OCCT Typedef Collector")
    print("=" * 60)

    collector = TypedefCollector(
        src_dir=args.src_directory,
        verbose=args.verbose
    )

    result = collector.collect()

    # Save to JSON
    output_path = Path(args.output)
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=2, ensure_ascii=False)

    print(f"\nResults saved to: {output_path}")

    # Print summary
    print_summary(result)

    print("\nNext step: Run replace_typedefs.py to replace typedef usages")


if __name__ == '__main__':
    main()
