@echo off
REM Copyright (c) 2025 OPEN CASCADE SAS
REM
REM OCCT 8.0.0 Modernization Script Runner
REM External-project migration runner (reuses pre-generated JSON files)

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%~1"
if "%SRC_DIR%"=="" set "SRC_DIR=src"
set "TYPEDEF_JSON=%SCRIPT_DIR%collected_typedefs.json"

echo ==============================================
echo OCCT 8.0.0 Modernization
echo ==============================================
echo Source directory: %SRC_DIR%
echo Typedef JSON: %TYPEDEF_JSON%
echo.

if not exist "%TYPEDEF_JSON%" (
    echo ERROR: Required JSON not found: %TYPEDEF_JSON%
    echo Generate it in OCCT first, then reuse it for external projects.
    exit /b 1
)

REM Check if dry-run
set "DRY_RUN="
if "%~2"=="--dry-run" (
    set "DRY_RUN=--dry-run"
    echo MODE: DRY RUN ^(no files will be modified^)
) else (
    echo MODE: LIVE ^(files will be modified^)
)
echo.

REM Confirm if not dry-run
if "%DRY_RUN%"=="" (
    set /p "CONFIRM=This will modify files in %SRC_DIR%. Continue? (y/N) "
    if /i not "!CONFIRM!"=="y" (
        echo Aborted.
        exit /b 1
    )
)

REM Phase 1 & 2: Handle migration (includes DownCast)
echo.
echo ==============================================
echo Phase 1 ^& 2: Handle Migration
echo ==============================================
python "%SCRIPT_DIR%migrate_handles.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Phase 3: Standard_* type migration
echo.
echo ==============================================
echo Phase 3: Standard_* Type Migration
echo ==============================================
python "%SCRIPT_DIR%migrate_standard_types.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Phase 4: Standard_* macro migration
echo.
echo ==============================================
echo Phase 4: Standard_* Macro Migration
echo ==============================================
python "%SCRIPT_DIR%migrate_macros.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Phase 5: DEFINE_STANDARD_HANDLE cleanup
echo.
echo ==============================================
echo Phase 5: DEFINE_STANDARD_HANDLE Cleanup
echo ==============================================
python "%SCRIPT_DIR%cleanup_define_handle.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Phase 6: Replace typedef usages with direct NCollection types (reuse pre-generated JSON)
echo.
echo ==============================================
echo Phase 6: Replace Typedef Usages ^(Reused JSON^)
echo ==============================================
python "%SCRIPT_DIR%replace_typedefs.py" %DRY_RUN% "%SRC_DIR%" --input "%TYPEDEF_JSON%"
if errorlevel 1 goto :error

REM Phase 7: Cleanup unused typedefs
echo.
echo ==============================================
echo Phase 7: Cleanup Unused Typedefs
echo ==============================================
python "%SCRIPT_DIR%cleanup_unused_typedefs.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Phase 8: Cleanup redundant access specifiers
echo.
echo ==============================================
echo Phase 8: Cleanup Access Specifiers
echo ==============================================
python "%SCRIPT_DIR%cleanup_access_specifiers.py" %DRY_RUN% "%SRC_DIR%"
if errorlevel 1 goto :error

REM Verification
echo.
echo ==============================================
echo Verification
echo ==============================================
python "%SCRIPT_DIR%verify_migration.py" "%SRC_DIR%"

echo.
echo ==============================================
echo Migration Complete
echo ==============================================
echo NOTE: File-removal phases are intentionally skipped for external projects.
echo.
echo Next steps:
echo 1. Run CI/CD formatting (clang-format)
echo 2. Build the project: cmake --build build
echo 3. Run tests: ctest --test-dir build
echo.

exit /b 0

:error
echo.
echo ERROR: Migration failed!
exit /b 1
