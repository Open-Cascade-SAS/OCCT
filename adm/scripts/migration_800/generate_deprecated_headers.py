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
OCCT 8.0.0 Deprecated Header Generator

Generates deprecated header files from collected_typedefs.json for backward compatibility.
These headers provide deprecated typedef aliases that emit compile-time warnings.

Usage:
    python3 generate_deprecated_headers.py [options]

Options:
    --input FILE        Input JSON from collect_typedefs.py (default: collected_typedefs.json)
    --output-dir DIR    Output directory for deprecated headers (default: src/Deprecated)
    --dry-run           Preview changes without writing files
    --verbose           Show detailed progress
"""

import os
import json
import argparse
from pathlib import Path
from typing import Dict, List, Set, Optional
from collections import defaultdict

# OCCT license header
LICENSE_HEADER = """// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//! @file {filename}
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use {replacement} directly instead.
"""

# Mapping of collection types to their NCollection headers
COLLECTION_HEADERS = {
    'Array1': 'NCollection_Array1.hxx',
    'Array2': 'NCollection_Array2.hxx',
    'HArray1': 'NCollection_HArray1.hxx',
    'HArray2': 'NCollection_HArray2.hxx',
    'List': 'NCollection_List.hxx',
    'Sequence': 'NCollection_Sequence.hxx',
    'HSequence': 'NCollection_HSequence.hxx',
    'Map': 'NCollection_Map.hxx',
    'DataMap': 'NCollection_DataMap.hxx',
    'IndexedMap': 'NCollection_IndexedMap.hxx',
    'IndexedDataMap': 'NCollection_IndexedDataMap.hxx',
    'DoubleMap': 'NCollection_DoubleMap.hxx',
    'Vector': 'NCollection_Vector.hxx',
    'Vec2': 'NCollection_Vec2.hxx',
    'Vec3': 'NCollection_Vec3.hxx',
    'Vec4': 'NCollection_Vec4.hxx',
    'Mat4': 'NCollection_Mat4.hxx',
    'UBTree': 'NCollection_UBTree.hxx',
    'CellFilter': 'NCollection_CellFilter.hxx',
    'Shared': 'NCollection_Shared.hxx',
}


def convert_occ_handle(type_str: str) -> str:
    """Convert occ::handle<T> to opencascade::handle<T>."""
    return type_str.replace('occ::handle', 'opencascade::handle')


def get_package_from_typedef(typedef_name: str) -> str:
    """Extract package name from typedef name (e.g., TColStd_Array1OfInteger -> TColStd)."""
    if '_' in typedef_name:
        return typedef_name.split('_')[0]
    return ''


def get_header_basename(header_path: str) -> str:
    """Get the basename of the header file."""
    return os.path.basename(header_path)


def extract_dependencies(full_type: str, template_params: str) -> Set[str]:
    """Extract type dependencies that need to be forward-declared or included."""
    deps = set()

    # Extract types from template parameters
    # This is a simplified extraction - may need refinement for complex cases
    params = template_params.replace('occ::handle<', '').replace('>', '')
    for param in params.split(','):
        param = param.strip()
        # Skip primitive types
        if param in ('int', 'double', 'float', 'char', 'bool', 'unsigned int',
                     'uint8_t', 'int64_t', 'size_t', 'void*', 'Standard_Integer',
                     'Standard_Real', 'Standard_Boolean'):
            continue
        # Extract class name if it's a handle
        if '::' in param and 'handle' not in param:
            continue
        if param and not param.startswith('NCollection_'):
            deps.add(param)

    return deps


# Mapping of common types to their headers for proper includes
TYPE_TO_HEADER = {
    'TopoDS_Shape': 'TopoDS_Shape.hxx',
    'TopoDS_Edge': 'TopoDS_Edge.hxx',
    'TopoDS_Face': 'TopoDS_Face.hxx',
    'TopoDS_Vertex': 'TopoDS_Vertex.hxx',
    'TopoDS_Wire': 'TopoDS_Wire.hxx',
    'TopoDS_Shell': 'TopoDS_Shell.hxx',
    'TopoDS_Solid': 'TopoDS_Solid.hxx',
    'TopoDS_Compound': 'TopoDS_Compound.hxx',
    'gp_Pnt': 'gp_Pnt.hxx',
    'gp_Pnt2d': 'gp_Pnt2d.hxx',
    'gp_Vec': 'gp_Vec.hxx',
    'gp_Vec2d': 'gp_Vec2d.hxx',
    'gp_Dir': 'gp_Dir.hxx',
    'gp_Dir2d': 'gp_Dir2d.hxx',
    'gp_XYZ': 'gp_XYZ.hxx',
    'gp_XY': 'gp_XY.hxx',
    'gp_Trsf': 'gp_Trsf.hxx',
    'gp_Trsf2d': 'gp_Trsf2d.hxx',
    'gp_GTrsf2d': 'gp_GTrsf2d.hxx',
    'gp_Ax1': 'gp_Ax1.hxx',
    'gp_Ax2': 'gp_Ax2.hxx',
    'gp_Ax3': 'gp_Ax3.hxx',
    'gp_Lin': 'gp_Lin.hxx',
    'gp_Lin2d': 'gp_Lin2d.hxx',
    'gp_Circ': 'gp_Circ.hxx',
    'gp_Circ2d': 'gp_Circ2d.hxx',
    'gp_Pln': 'gp_Pln.hxx',
    'gp_Sphere': 'gp_Sphere.hxx',
    'gp_Cylinder': 'gp_Cylinder.hxx',
    'gp_Cone': 'gp_Cone.hxx',
    'gp_Torus': 'gp_Torus.hxx',
    'TCollection_AsciiString': 'TCollection_AsciiString.hxx',
    'TCollection_ExtendedString': 'TCollection_ExtendedString.hxx',
    'TCollection_HAsciiString': 'TCollection_HAsciiString.hxx',
    'TCollection_HExtendedString': 'TCollection_HExtendedString.hxx',
    'Standard_GUID': 'Standard_GUID.hxx',
    'Standard_Transient': 'Standard_Transient.hxx',
    'Bnd_Box': 'Bnd_Box.hxx',
    'Bnd_Box2d': 'Bnd_Box2d.hxx',
}


def get_includes_for_type(type_str: str) -> List[str]:
    """Get required includes for a type used in template parameters."""
    includes = []

    # Check for handles
    if 'opencascade::handle' in type_str or 'occ::handle' in type_str:
        includes.append('Standard_Handle.hxx')

    # Extract type names and add includes
    for type_name, header in TYPE_TO_HEADER.items():
        if type_name in type_str:
            includes.append(header)

    return includes


def generate_header_content(header_file: str, typedefs: List[Dict],
                           package_name: str,
                           original_includes: Optional[List[str]] = None) -> str:
    """Generate the content of a deprecated header file.

    Args:
        header_file: Original header file path
        typedefs: List of typedef dictionaries
        package_name: Package name
        original_includes: List of includes from original header (if available)
    """
    filename = get_header_basename(header_file)
    guard_name = f"_{filename.replace('.', '_')}"

    # Collect all collection types used
    collection_types = set()
    all_includes = set()

    for td in typedefs:
        col_type = td['collection_type']
        if col_type in COLLECTION_HEADERS:
            collection_types.add(col_type)

        # Get includes needed for template parameters (fallback when original includes not available)
        if not original_includes:
            full_type = convert_occ_handle(td['full_type'])
            template_params = td.get('template_params', '')
            type_includes = get_includes_for_type(full_type)
            all_includes.update(type_includes)

    # Determine replacement text for documentation
    if len(typedefs) == 1:
        replacement = convert_occ_handle(typedefs[0]['full_type'])
    else:
        replacement = "NCollection types"

    # Build the header content
    lines = []

    # License header
    lines.append(LICENSE_HEADER.format(filename=filename, replacement=replacement))

    # Include guard
    lines.append(f"#ifndef {guard_name}")
    lines.append(f"#define {guard_name}")
    lines.append("")

    # All includes together - Standard_Macro first, then others
    lines.append("#include <Standard_Macro.hxx>")

    if original_includes:
        # Use original includes from the source header (preferred)
        for inc in original_includes:
            # Skip the header's own potential circular include and Standard_Macro.hxx (already included)
            if inc != filename and not inc.endswith(filename) and inc != 'Standard_Macro.hxx':
                lines.append(f"#include <{inc}>")
    else:
        # Fallback: Include required NCollection headers based on typedef analysis
        for col_type in sorted(collection_types):
            if col_type in COLLECTION_HEADERS:
                lines.append(f"#include <{COLLECTION_HEADERS[col_type]}>")

        # Include type dependencies detected from template parameters
        for inc in sorted(all_includes):
            lines.append(f"#include <{inc}>")

    lines.append("")

    # Header deprecation warning (after all includes)
    lines.append(f'Standard_HEADER_DEPRECATED("{filename} is deprecated since OCCT 8.0.0. '
                f'Use {replacement} directly.")')

    lines.append("")

    # Generate deprecated typedefs
    for td in typedefs:
        typedef_name = td['typedef_name']
        full_type = convert_occ_handle(td['full_type'])
        is_iterator = td.get('is_iterator', False)

        # Deprecation message
        dep_msg = f"{typedef_name} is deprecated, use {full_type} directly"

        # Generate the typedef with deprecation attribute (original typedef syntax)
        lines.append(f'Standard_DEPRECATED("{dep_msg}")')
        lines.append(f'typedef {full_type} {typedef_name};')

    lines.append("")
    lines.append(f"#endif // {guard_name}")
    lines.append("")

    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(description='OCCT Deprecated Header Generator')
    parser.add_argument('--input', '-i', default='collected_typedefs.json',
                       help='Input JSON file (default: collected_typedefs.json)')
    parser.add_argument('--output-dir', '-o', default=None,
                       help='Output directory (default: src/Deprecated/NCollectionAliases)')
    parser.add_argument('--dry-run', action='store_true',
                       help='Preview without writing files')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')

    args = parser.parse_args()

    print("OCCT Deprecated Header Generator")
    print("=" * 60)

    # Determine paths
    script_dir = Path(__file__).parent
    input_file = script_dir / args.input if not os.path.isabs(args.input) else Path(args.input)

    if args.output_dir:
        output_dir = Path(args.output_dir)
    else:
        # Default: src/Deprecated/NCollectionAliases relative to OCCT root
        occt_root = script_dir.parent.parent.parent
        output_dir = occt_root / 'src' / 'Deprecated' / 'NCollectionAliases'

    print(f"Input file: {input_file}")
    print(f"Output directory: {output_dir}")
    if args.dry_run:
        print("(DRY RUN - no files will be written)")
    print()

    # Load JSON data
    with open(input_file, 'r') as f:
        data = json.load(f)

    # Get typedef-only headers
    typedef_only_headers = set(data.get('typedef_only_headers', []))
    print(f"Found {len(typedef_only_headers)} typedef-only headers")

    # Group typedefs by header file
    headers_data = data.get('headers', {})

    # Track statistics
    headers_created = 0
    typedefs_processed = 0
    packages = set()

    # Process each typedef-only header
    for header_path in sorted(typedef_only_headers):
        header_info = headers_data.get(header_path, {})
        typedefs = header_info.get('typedefs', [])

        if not typedefs:
            continue

        # Skip headers without underscore in typedef name (typically local aliases)
        if all('_' not in td['typedef_name'] for td in typedefs):
            if args.verbose:
                print(f"  Skipping {header_path} (no package-prefixed typedefs)")
            continue

        # Filter to only package-prefixed typedefs
        package_typedefs = [td for td in typedefs if '_' in td['typedef_name']]
        if not package_typedefs:
            continue

        # Extract package name from first typedef
        package_name = get_package_from_typedef(package_typedefs[0]['typedef_name'])
        packages.add(package_name)

        # Determine output path - simple flat structure
        filename = get_header_basename(header_path)
        out_path = output_dir / filename

        # Get original includes if available in JSON
        original_includes = header_info.get('includes', None)

        # Generate header content
        content = generate_header_content(header_path, package_typedefs, package_name,
                                         original_includes)

        if args.verbose:
            print(f"  {header_path} -> {filename}")

        if not args.dry_run:
            out_path.parent.mkdir(parents=True, exist_ok=True)
            with open(out_path, 'w') as f:
                f.write(content)

        headers_created += 1
        typedefs_processed += len(package_typedefs)

    # Summary
    print()
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Headers created: {headers_created}")
    print(f"Typedefs processed: {typedefs_processed}")
    print(f"Original packages: {len(packages)}")
    print(f"Output directory: {output_dir}")

    if args.verbose:
        print("\nOriginal packages:")
        for pkg in sorted(packages):
            print(f"  {pkg}")

    print("\nDone!")
    print("Headers are ready to be installed via CMakeLists.txt")
    if args.dry_run:
        print("Run without --dry-run to create files")


def generate_flat_cmake_files(output_dir: Path, all_files: List[str], verbose: bool):
    """Generate FILES.cmake for flat structure (single Deprecated package)."""
    cmake_dir = output_dir / 'TKDeprecated' / 'Deprecated'
    cmake_file = cmake_dir / 'FILES.cmake'

    content = """# Auto-generated deprecated typedef headers
# These headers provide backward compatibility aliases for OCCT 8.0.0
set(OCCT_Deprecated_FILES
"""
    for f in sorted(all_files):
        content += f"  {f}\n"
    content += ")\n"

    cmake_dir.mkdir(parents=True, exist_ok=True)
    with open(cmake_file, 'w') as f:
        f.write(content)

    if verbose:
        print(f"  Created {cmake_file.relative_to(output_dir)}")


def generate_cmake_files(output_dir: Path, packages: Set[str],
                        typedef_only_headers: Set[str], headers_data: Dict,
                        verbose: bool):
    """Generate FILES.cmake for each package."""
    # Group headers by package
    package_files = defaultdict(list)

    for header_path in sorted(typedef_only_headers):
        header_info = headers_data.get(header_path, {})
        typedefs = header_info.get('typedefs', [])

        if not typedefs:
            continue

        # Get package-prefixed typedefs
        package_typedefs = [td for td in typedefs if '_' in td['typedef_name']]
        if not package_typedefs:
            continue

        package_name = get_package_from_typedef(package_typedefs[0]['typedef_name'])
        filename = get_header_basename(header_path)
        package_files[package_name].append(filename)

    # Write FILES.cmake for each package
    for package_name, files in sorted(package_files.items()):
        cmake_dir = output_dir / 'TKDeprecated' / package_name
        cmake_file = cmake_dir / 'FILES.cmake'

        content = f"""# Auto-generated deprecated typedef headers for {package_name}
set(OCCT_{package_name}_FILES_Deprecated
"""
        for f in sorted(files):
            content += f"  {f}\n"
        content += ")\n"

        cmake_dir.mkdir(parents=True, exist_ok=True)
        with open(cmake_file, 'w') as f:
            f.write(content)

        if verbose:
            print(f"  Created {cmake_file.relative_to(output_dir)}")


def generate_module_structure(output_dir: Path, packages: Set[str], verbose: bool,
                             flat: bool = False):
    """Generate MODULES.cmake, TOOLKITS.cmake, PACKAGES.cmake structure."""
    deprecated_dir = output_dir
    tk_dir = deprecated_dir / 'TKDeprecated'

    # Create TOOLKITS.cmake for Deprecated module
    toolkits_file = deprecated_dir / 'TOOLKITS.cmake'
    toolkits_content = """# Deprecated module - backward compatibility typedefs
# This module provides deprecated typedef headers for backward compatibility.
# All headers emit deprecation warnings when included.
set(Deprecated_TOOLKITS
  TKDeprecated
)
"""
    with open(toolkits_file, 'w') as f:
        f.write(toolkits_content)
    if verbose:
        print(f"  Created TOOLKITS.cmake")

    # Create PACKAGES.cmake for TKDeprecated toolkit
    packages_file = tk_dir / 'PACKAGES.cmake'

    if flat:
        packages_content = """# Packages in TKDeprecated toolkit
# Using flat structure with single Deprecated package
set(TKDeprecated_PACKAGES
  Deprecated
)
"""
    else:
        packages_content = "# Packages in TKDeprecated toolkit\nset(TKDeprecated_PACKAGES\n"
        for pkg in sorted(packages):
            packages_content += f"  {pkg}\n"
        packages_content += ")\n"

    tk_dir.mkdir(parents=True, exist_ok=True)
    with open(packages_file, 'w') as f:
        f.write(packages_content)
    if verbose:
        print(f"  Created TKDeprecated/PACKAGES.cmake")

    # Create EXTERNLIB.cmake
    externlib_file = tk_dir / 'EXTERNLIB.cmake'
    externlib_content = """# External library dependencies for TKDeprecated
# This toolkit depends on TKernel for Standard_Macro.hxx
set(TKDeprecated_EXTERNLIB
  TKernel
)
"""
    with open(externlib_file, 'w') as f:
        f.write(externlib_content)
    if verbose:
        print(f"  Created TKDeprecated/EXTERNLIB.cmake")


if __name__ == '__main__':
    main()
