#!/bin/bash
# Copyright (c) 2025 OPEN CASCADE SAS
#
# OCCT 8.0.0 Modernization Script Runner
# This script runs the migration in the correct order

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="${1:-src}"

echo "=============================================="
echo "OCCT 8.0.0 Modernization"
echo "=============================================="
echo "Source directory: $SRC_DIR"
echo ""

# Check if dry-run
DRY_RUN=""
if [[ "$2" == "--dry-run" ]]; then
    DRY_RUN="--dry-run"
    echo "MODE: DRY RUN (no files will be modified)"
else
    echo "MODE: LIVE (files will be modified)"
fi
echo ""

# Confirm
if [[ -z "$DRY_RUN" ]]; then
    read -p "This will modify files in $SRC_DIR. Continue? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted."
        exit 1
    fi
fi

# Phase 1 & 2: Handle migration (includes DownCast)
echo ""
echo "=============================================="
echo "Phase 1 & 2: Handle Migration"
echo "=============================================="
python3 "$SCRIPT_DIR/migrate_handles.py" $DRY_RUN "$SRC_DIR"

# Phase 3: Standard_* type migration
echo ""
echo "=============================================="
echo "Phase 3: Standard_* Type Migration"
echo "=============================================="
python3 "$SCRIPT_DIR/migrate_standard_types.py" $DRY_RUN "$SRC_DIR"

# Phase 4: Standard_* macro migration
echo ""
echo "=============================================="
echo "Phase 4: Standard_* Macro Migration"
echo "=============================================="
python3 "$SCRIPT_DIR/migrate_macros.py" $DRY_RUN "$SRC_DIR"

# Phase 5: DEFINE_STANDARD_HANDLE cleanup
echo ""
echo "=============================================="
echo "Phase 5: DEFINE_STANDARD_HANDLE Cleanup"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_define_handle.py" $DRY_RUN "$SRC_DIR"

# Phase 6: Cleanup deprecated typedef/using declarations
echo ""
echo "=============================================="
echo "Phase 6: Deprecated Typedef/Using Cleanup"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_deprecated_typedefs.py" $DRY_RUN "$SRC_DIR"

# Phase 7: Collect NCollection typedefs
echo ""
echo "=============================================="
echo "Phase 7: Collect NCollection Typedefs"
echo "=============================================="
python3 "$SCRIPT_DIR/collect_typedefs.py" "$SRC_DIR" --output "$SCRIPT_DIR/collected_typedefs.json"

# Phase 8: Replace typedef usages with direct NCollection types
echo ""
echo "=============================================="
echo "Phase 8: Replace Typedef Usages"
echo "=============================================="
python3 "$SCRIPT_DIR/replace_typedefs.py" $DRY_RUN "$SRC_DIR" --input "$SCRIPT_DIR/collected_typedefs.json"

# Phase 9: Remove typedef-only headers
echo ""
echo "=============================================="
echo "Phase 9: Remove Typedef-Only Headers"
echo "=============================================="
python3 "$SCRIPT_DIR/remove_typedef_headers.py" $DRY_RUN "$SRC_DIR" --input "$SCRIPT_DIR/collected_typedefs.json"

# Phase 10: Cleanup forwarding/include-only headers
echo ""
echo "=============================================="
echo "Phase 10: Cleanup Forwarding Headers"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_forwarding_headers.py" $DRY_RUN "$SRC_DIR"

# Phase 11: Cleanup unused typedefs
echo ""
echo "=============================================="
echo "Phase 11: Cleanup Unused Typedefs"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_unused_typedefs.py" $DRY_RUN "$SRC_DIR"

# Phase 12: Cleanup redundant access specifiers
echo ""
echo "=============================================="
echo "Phase 12: Cleanup Access Specifiers"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_access_specifiers.py" $DRY_RUN "$SRC_DIR"

# Verification
echo ""
echo "=============================================="
echo "Verification"
echo "=============================================="
python3 "$SCRIPT_DIR/verify_migration.py" "$SRC_DIR"

echo ""
echo "=============================================="
echo "Migration Complete"
echo "=============================================="
echo ""
echo "Next steps:"
echo "1. Run CI/CD formatting (clang-format)"
echo "2. Build the project: cmake --build build"
echo "3. Run tests: ctest --test-dir build"
echo ""
