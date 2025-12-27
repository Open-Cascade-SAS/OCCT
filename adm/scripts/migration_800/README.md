# OCCT 8.0.0 Modernization Scripts

This folder contains Python scripts for migrating OCCT codebase to modern C++ syntax for the 8.0.0 release.

## Overview

The migration modernizes the following patterns:

| Phase | Description | Approximate Count |
|-------|-------------|-------------------|
| 1-2 | `Handle(Class)` → `occ::handle<Class>` | ~82,600 |
| 1-2 | `Handle(T)::DownCast()` → `occ::down_cast<T>()` | ~8,000 |
| 3 | `Standard_Boolean/Integer/Real` → `bool/int/double` | ~161,000 |
| 3 | `Standard_True/False` → `true/false` | ~37,000 |
| 4 | `Standard_OVERRIDE` → `override` | ~7,100 |
| 4 | `Standard_NODISCARD` → `[[nodiscard]]` | ~430 |
| 4 | Other macro replacements | ~200 |
| 5 | Remove redundant `DEFINE_STANDARD_HANDLE` | ~1,970 |
| 6 | Deprecated typedef/using cleanup | ~1,800 |
| 7-9 | NCollection typedef replacement and header cleanup | ~31,000 |
| 10 | Cleanup forwarding/include-only headers | varies |
| 11 | Cleanup redundant access specifiers | varies |

## Requirements

- Python 3.6 or later
- No external dependencies (uses only standard library)

## Quick Start

### Full Migration (Recommended)

**Linux/macOS:**
```bash
cd /path/to/OCCT
./adm/scripts/migration_800/run_migration.sh src
```

**Windows:**
```batch
cd \path\to\OCCT
adm\scripts\migration_800\run_migration.bat src
```

### Dry Run (Preview Changes)

**Linux/macOS:**
```bash
./adm/scripts/migration_800/run_migration.sh src --dry-run
```

**Windows:**
```batch
adm\scripts\migration_800\run_migration.bat src --dry-run
```

## Individual Scripts

Each script can be run independently for granular control:

### migrate_handles.py

Migrates `Handle(Class)` patterns to `occ::handle<Class>` and `Handle(T)::DownCast()` to `occ::down_cast<T>()`.

```bash
python3 migrate_handles.py [options] <path>

Options:
  --dry-run    Preview changes without modifying files
  --verbose    Show detailed progress
  --backup     Create backup files before modification
```

### migrate_standard_types.py

Replaces `Standard_*` type aliases with native C++ types.

```bash
python3 migrate_standard_types.py [options] <path>

Options:
  --dry-run    Preview changes without modifying files
  --verbose    Show detailed progress
  --backup     Create backup files before modification
```

**Replacements:**
| Original | Replacement |
|----------|-------------|
| `Standard_Boolean` | `bool` |
| `Standard_Integer` | `int` |
| `Standard_Real` | `double` |
| `Standard_ShortReal` | `float` |
| `Standard_Character` | `char` |
| `Standard_Byte` | `uint8_t` |
| `Standard_Address` | `void*` |
| `const Standard_Address` | `void* const` (preserves const pointer semantics) |
| `Standard_Size` | `size_t` |
| `Standard_Time` | `std::time_t` |
| `Standard_ExtCharacter` | `char16_t` |
| `Standard_Utf16Char` | `char16_t` |
| `Standard_Utf32Char` | `char32_t` |
| `Standard_WideChar` | `wchar_t` |
| `Standard_Utf8Char` | `char` |
| `Standard_Utf8UChar` | `unsigned char` |
| `Standard_CString` | `const char*` |
| `Standard_ExtString` | `const char16_t*` |
| `Standard_True` | `true` |
| `Standard_False` | `false` |

**Note:** `const Standard_Address` is correctly converted to `void* const` (const pointer), not `const void*` (pointer to const), which would have different semantics.

### migrate_macros.py

Replaces deprecated `Standard_*` macros with C++ keywords/attributes.

```bash
python3 migrate_macros.py [options] <path>

Options:
  --dry-run    Preview changes without modifying files
  --verbose    Show detailed progress
  --backup     Create backup files before modification
  --verify     Only verify remaining patterns (no changes)
```

**Replacements:**
| Original | Replacement |
|----------|-------------|
| `Standard_OVERRIDE` | `override` |
| `Standard_NODISCARD` | `[[nodiscard]]` |
| `Standard_FALLTHROUGH` | `[[fallthrough]];` |
| `Standard_Noexcept` | `noexcept` |
| `Standard_DELETE` | `= delete` |
| `Standard_THREADLOCAL` | `thread_local` |
| `Standard_ATOMIC(T)` | `std::atomic<T>` |

**Note:** `Standard_UNUSED` requires manual migration due to `[[maybe_unused]]` having stricter placement rules.

### collect_typedefs.py

Collects NCollection typedef information from public headers (.hxx, .lxx, .h).

```bash
python3 collect_typedefs.py [options] <path>

Options:
  --output FILE    Output JSON file (default: collected_typedefs.json)
  --verbose        Show detailed progress
```

**Features:**
- Collects container typedefs (e.g., `typedef NCollection_Map<T> MyMap;`)
- Collects iterator typedefs (e.g., `typedef NCollection_Map<T>::Iterator MyMapIterator;`)
- Resolves alias typedefs (typedef-on-typedef chains)
- Detects namespace context for qualified usage (e.g., `IMeshData::CircleCellFilter`)
- Identifies typedef-only headers (can be removed) vs mixed headers
- Normalizes multi-line typedef parameters to single line

**Output:** `collected_typedefs.json` containing:
- List of all typedefs with their full NCollection type
- List of typedef-only headers (removable)
- List of mixed headers (keep file, replace usages only)

### replace_typedefs.py

Replaces typedef usages with direct NCollection_* types.

```bash
python3 replace_typedefs.py [options] <path>

Options:
  --input FILE     Input JSON from collect_typedefs.py
  --dry-run        Preview changes without modifying files
  --verbose        Show detailed progress
  --jobs N         Number of parallel workers (default: 4)
  --file PATH      Process only a specific file
```

**Features:**
- Parallel processing for fast execution
- Handles namespace-qualified typedefs (e.g., `IMeshData::CircleCellFilter`)
- Properly handles `#define` macros (keeps replacements on single line)
- Skips typedef definitions themselves
- Multi-pass processing for typedef-on-typedef chains

### remove_typedef_headers.py

Removes typedef-only header files and updates FILES.cmake.

```bash
python3 remove_typedef_headers.py [options] <path>

Options:
  --input FILE     Input JSON from collect_typedefs.py
  --dry-run        Preview changes without modifying files
  --verbose        Show detailed progress
  --keep-backup    Move files to backup directory instead of deleting
```

### cleanup_access_specifiers.py

Cleans up redundant access specifiers in C++ files.

```bash
python3 cleanup_access_specifiers.py [options] <path>

Options:
  --dry-run        Preview changes without modifying files
  --verbose        Show detailed progress
  --jobs N         Number of parallel workers (default: 4)
```

**Patterns cleaned:**
| Pattern | Result |
|---------|--------|
| `public:\nprivate:` | `private:` |
| `protected:\npublic:` | `public:` |
| `private:\n};` | `};` |
| Empty sections with only comments | Removed |

### cleanup_define_handle.py

Removes redundant `DEFINE_STANDARD_HANDLE` patterns.

```bash
python3 cleanup_define_handle.py [options] <path>

Options:
  --dry-run    Preview changes without modifying files
  --verbose    Show detailed progress
  --backup     Create backup files before modification
```

**Two cases handled:**

1. **Adjacent forward declaration + macro** (both lines removed):
```cpp
// Before:                              // After:
class MyClass;                          // (both lines removed)
DEFINE_STANDARD_HANDLE(MyClass, Base)
```

2. **Standalone macro** (only macro line removed):
```cpp
// Before:                              // After:
#include <...>                          #include <...>

DEFINE_STANDARD_HANDLE(MyClass, Base)   // (line removed)

class MyClass : public Base {           class MyClass : public Base {
```

### cleanup_deprecated_typedefs.py

Finds and removes deprecated typedef/using declarations marked with `Standard_DEPRECATED`.
Replaces all usages with the original type and removes the deprecated lines.

```bash
python3 cleanup_deprecated_typedefs.py [options] <path>

Options:
  --dry-run        Preview changes without modifying files
  --verbose        Show detailed progress
  --jobs N         Number of parallel workers (default: 4)
  --output FILE    Custom output file path
```

**Features:**
- Finds `Standard_DEPRECATED(...)` followed by `typedef` or `using` declarations
- Replaces all usages of deprecated name with the original type
- Removes the deprecated typedef/using lines from headers
- Identifies headers that become include-only after cleanup
- Saves results to JSON for external project migration

**Example cleanup:**
```cpp
// Before:                                       // After:
Standard_DEPRECATED("Use PrsMgr_...")            // (lines removed)
typedef PrsMgr_Presentation PrsMgr_Pres3d;

void Foo(PrsMgr_Pres3d& p);                      void Foo(PrsMgr_Presentation& p);
```

**Output files:**
- `deprecated_typedef_results.json` - Cleanup operation results
- `collected_deprecated_typedefs.json` - Deprecated typedefs for external project migration

### cleanup_unused_typedefs.py

Removes typedef/using declarations that are no longer used after migration.

```bash
python3 cleanup_unused_typedefs.py [options] <path>

Options:
  --dry-run        Preview changes without modifying files
  --verbose        Show detailed progress
  --jobs N         Number of parallel workers (default: 4)
```

### verify_migration.py

Verifies migration completeness by scanning for remaining legacy patterns.

```bash
python3 verify_migration.py [options] <path>

Options:
  --verbose    Show detailed matches
  --output     Save report to file
  --json       Output report as JSON
  --check-new  Also check for new pattern usage
```

### occt_modernize.py

Comprehensive all-in-one migration script (alternative to running individual scripts).

```bash
python3 occt_modernize.py [options] <path>

Options:
  --dry-run           Preview changes without modifying files
  --phase <N>         Run only specific phase (1-5)
  --skip-phase <N>    Skip specific phase
  --verbose           Show detailed progress
```

## Excluded Files

The following files are excluded from migration (core infrastructure):

- `Standard_Handle.hxx` - Contains `occ` namespace definition
- `Standard_Type.hxx` - Contains type helpers
- `Standard_Macro.hxx` - Macro definitions
- `Standard_TypeDef.hxx` - Type definitions (compatibility layer)
- `Standard_Transient.hxx` - Base class for handles

## Post-Migration Steps

After running the migration:

1. **Format the code:**
   ```bash
   # Run your CI/CD formatting tool (clang-format)
   ```

2. **Build the project:**
   ```bash
   cmake --build build --config Release
   ```

3. **Run tests:**
   ```bash
   ctest --test-dir build
   ```

4. **Review manual items:**
   - `Standard_UNUSED` usages (requires manual placement of `[[maybe_unused]]`)
   - Any files reported in verification with remaining patterns

## Rollback

All changes are tracked in git. To rollback:

```bash
git checkout -- src/
```

Or revert specific commits if changes were committed incrementally.

## Troubleshooting

### Script fails with encoding errors

Some files may have non-UTF-8 encoding. The scripts handle this with `errors='replace'`, but if issues persist:

```bash
# Find files with non-ASCII characters
find src -name "*.hxx" -o -name "*.cxx" | xargs file | grep -v ASCII
```

### Patterns not being replaced

Check if the file is in the excluded list or if the pattern has unusual formatting. Run with `--verbose` for details:

```bash
python3 migrate_handles.py --dry-run --verbose src/path/to/file.hxx
```

### Build errors after migration

1. Ensure all phases completed successfully
2. Check for `Standard_UNUSED` manual review items
3. Run verification to find remaining patterns:
   ```bash
   python3 verify_migration.py --verbose src
   ```

## License

These scripts are part of Open CASCADE Technology and are distributed under the LGPL 2.1 license.
