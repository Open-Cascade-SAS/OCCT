#!/bin/bash
# Copyright (c) 2025 OPEN CASCADE SAS
#
# OCCT 8.0.0 Modernization Script Runner
# External-project migration runner (reuses pre-generated JSON files)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="${1:-src}"
TYPEDEF_JSON="$SCRIPT_DIR/collected_typedefs.json"

echo "=============================================="
echo "OCCT 8.0.0 Modernization"
echo "=============================================="
echo "Source directory: $SRC_DIR"
echo "Typedef JSON: $TYPEDEF_JSON"
echo ""

if [[ ! -f "$TYPEDEF_JSON" ]]; then
    echo "ERROR: Required JSON not found: $TYPEDEF_JSON"
    echo "Generate it in OCCT first, then reuse it for external projects."
    exit 1
fi

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

# Phase 6: Replace typedef usages with direct NCollection types (reuse pre-generated JSON)
echo ""
echo "=============================================="
echo "Phase 6: Replace Typedef Usages (Reused JSON)"
echo "=============================================="
python3 "$SCRIPT_DIR/replace_typedefs.py" $DRY_RUN "$SRC_DIR" --input "$TYPEDEF_JSON"

# Phase 7: Cleanup unused typedefs
echo ""
echo "=============================================="
echo "Phase 7: Cleanup Unused Typedefs"
echo "=============================================="
python3 "$SCRIPT_DIR/cleanup_unused_typedefs.py" $DRY_RUN "$SRC_DIR"

# Phase 8: Cleanup redundant access specifiers
echo ""
echo "=============================================="
echo "Phase 8: Cleanup Access Specifiers"
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
echo "NOTE: File-removal phases are intentionally skipped for external projects."
echo ""
echo "Next steps:"
echo "1. Run CI/CD formatting (clang-format)"
echo "2. Build the project: cmake --build build"
echo "3. Run tests: ctest --test-dir build"
echo ""
