# OCCT Copilot Instructions

This file provides comprehensive guidance for AI assistants working with the Open CASCADE Technology (OCCT) C++17+ 3D CAD/CAM/CAE library.

---

## 1. Core Directives for AI Assistant

> **IMPORTANT:** These are the most critical rules. Follow them strictly.

1.  **Memory Management is Paramount:**
    - **ALWAYS** use `Handle(ClassName)` for any class inheriting from `Standard_Transient` (e.g., `Geom_*`, `Poly_*`, `AIS_*`, `V3d_*`). This is for reference-counted objects.
    - **NEVER** use raw pointers (`ClassName*`) for these types, as it will cause memory leaks.
    - **Correct:** `Handle(Geom_Circle) aCircle = new Geom_Circle(...);`
    - **Wrong:** `Geom_Circle* aCircle = new Geom_Circle(...);`

2.  **Check Operation Status:**
    - After using an API that performs a geometric operation (e.g., `BRepAlgoAPI_Fuse`, `BRepBuilderAPI_MakeShape`), **ALWAYS** check if the operation was successful using the `IsDone()` method before accessing the result.
    - **Correct:**
      ```cpp
      BRepAlgoAPI_Fuse aFuser(theShape1, theShape2);
      if (aFuser.IsDone()) {
          auto aResult = aFuser.Shape();
      } else {
          // Handle error
      }
      ```

3.  **Strict Naming and File Conventions:**
    - Adhere to the strict `Package_ClassName` convention.
    - Place new files in the correct directory: `src/Module/Toolkit/Package/`.
    - After adding a file, **ALWAYS** update the corresponding `src/Module/Toolkit/FILES.cmake` file.

4.  **Use Modern C++ Idioms:**
    - Prefer `auto` for variable declarations where the type is clear, especially with iterators.
    - Use range-based `for` loops and structured bindings where applicable.
    - Use the modern `TopExp_Explorer` constructor style.
    - **Correct:** `for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next()) { ... }`
    - **Wrong:** `for (TopExp_Explorer anExp; anExp.Init(theShape, TopAbs_FACE); anExp.More(); anExp.Next()) { ... }`

5.  **Safe Type Casting:**
    - When downcasting topological shapes, **ALWAYS** use the `TopoDS` helper functions to avoid errors.
    - **Correct:** `auto aFace = TopoDS::Face(anExp.Current());`
    - **Wrong:** `auto aFace = (const TopoDS_Face&)anExp.Current();`

6.  **Handle Downcasting:**
    - Use `Handle(DerivedClass)::DownCast(BaseHandle)` to safely downcast handles.
    - **Correct:**
      ```cpp
      Handle(Geom_Circle) circ = Handle(Geom_Circle)::DownCast(someCurveHandle);
      ```
    - **Wrong:**
      ```cpp
      // Do not use C-style casts on handles
      Geom_Circle* circ = (Geom_Circle*)someHandle.Get();
      ```

## 1.5. Validate Handles for Null Safety:
- **ALWAYS** check that a `Handle(ClassName)` is not null before dereferencing it:
  ```cpp
  if (!theHandle.IsNull()) {
      // use theHandle
  }
  ```

---

## 2. Project Overview

Open CASCADE Technology (OCCT) is a comprehensive C++ software development platform for 3D surface and solid modeling, CAD data exchange, and visualization. It's a modern C++17+ library providing services for CAD/CAM/CAE applications.

### Architecture & Source Organization

- **`src/`**: Source code, organized by a `Module/Toolkit/Package` hierarchy.
- **`tests/`**: Draw Harness test files, organized by functionality.
- **`adm/`**: Administrative and build configuration files (CMake).
- **`samples/`**: Example applications.

---

## 3. Code Conventions

### Naming Patterns

| Element Type                | Pattern                      | Example                          |
| --------------------------- | ---------------------------- | -------------------------------- |
| **Classes**                 | `Package_ClassName`          | `TopoDS_Shape`, `BRep_Builder`   |
| **Public Methods**          | `MethodName`                 | `IsDone()`, `Shape()`            |
| **Private Methods**         | `methodName`                 | `myInternalMethod()`             |
| **Method Parameters**       | `theParameterName`           | `theShape`, `theTolerance`       |
| **Local Variables**         | `aVariableName`              | `aBox`, `anExplorer`             |
| **Class Member Fields**     | `myFieldName`                | `myShape`, `myIsDone`            |
| **Struct Member Fields**    | `FieldVariableName`          | `Point`, `Value`                 |
| **Global Variables**        | `THE_GLOBAL_VARIABLE`        | `THE_DEFAULT_PRECISION`          |

### Type Mappings

| OCCT Type           | C++ Equivalent | Notes                               |
| ------------------- | -------------- | ----------------------------------- |
| `Standard_Real`     | `double`       | Use for all floating-point numbers. |
| `Standard_Integer`  | `int`          | Use for all integer values.         |
| `Standard_Boolean`  | `bool`         | Use for `true`/`false` values.      |

### Modern C++ Encouraged
This is a C++17+ codebase. Proactively use modern features to improve code quality:
- `auto`
- Range-based `for` loops
- Structured bindings: `for (const auto& [key, value] : aMap)`
- `std::optional` for optional return values where appropriate.
- `if constexpr` for compile-time conditions.

---

## 4. Step-by-Step Workflow Example: Adding a New Class and Test

This example demonstrates the end-to-end process for adding a new class `BRepTest_MyNewClass` to the `TKTopAlgo` toolkit and creating a corresponding GTest.

**1. Create Header and Source Files:**
Navigate to the correct package directory and create the files.
```bash
# Navigate to the BRepTest package in the ModelingAlgorithms module
cd src/ModelingAlgorithms/TKTopAlgo/BRepTest
touch BRepTest_MyNewClass.hxx BRepTest_MyNewClass.cxx
```

**2. Implement the Class:**
Add content to `BRepTest_MyNewClass.hxx` and `.cxx`, following all code conventions.

**3. Add Files to CMake:**
Edit the toolkit's `FILES.cmake` to register the new files.
```bash
# Edit the CMake file for TKTopAlgo
vim src/ModelingAlgorithms/TKTopAlgo/FILES.cmake
```
Add the new files to the `OCCT_<PackageName>_FILES` list:
```cmake
# In FILES.cmake
...
set (OCCT_BRepTest_FILES
    ...
    BRepTest_MyNewClass.hxx
    BRepTest_MyNewClass.cxx
    ...
)
```

**4. Create a GTest:**
Navigate to the GTest directory for the toolkit and create a test file.
```bash
# Navigate to the GTest directory for TKTopAlgo
cd src/ModelingAlgorithms/TKTopAlgo/GTests
touch BRepTest_MyNewClass_Test.cxx
```
Write the test implementation in the new file.

**5. Add GTest to CMake:**
Edit the same `FILES.cmake` to add the test file.
```cmake
# In FILES.cmake
...
set (OCCT_TKTopAlgo_GTests_FILES
    ...
    GTests/BRepTest_MyNewClass_Test.cxx
    ...
)
```

**6. Build and Run Test:**
From the `build` directory, build the project and run the tests.
```bash
# Navigate to build directory
cd build

# Re-run CMake to pick up new files (usually not needed, but good practice)
cmake .. -DBUILD_GTEST=ON

# Build the project
cmake --build . --config Release

# Run the tests
./bin/OpenCascadeGTest --gtest_filter=*MyNewClass*
```

---

## 5. Build and Test System

### Build System (CMake)
- **Primary build system:** CMake 3.16+ recommended.
- **Build Directory:** Always build in a separate directory (e.g., `build/`).
- **Quick Build:**
  ```bash
  mkdir -p build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_GTEST=ON
  cmake --build . --config Release --parallel
  ```
- **Environment:** Before running any OCCT executable (including tests), you **must** source the environment script: `source build/env.sh` (or `build\env.bat` on Windows).

### Testing Frameworks
- **Draw Harness:** Tcl-based framework for interactive testing. Located in `tests/`. Run with `build/DRAWEXE`.
- **GTest:** C++ unit testing framework. Tests are located in `src/.../GTests/`. Enable with `-DBUILD_GTEST=ON`.

---

## 6. Common Patterns & Key Packages

### Common Operations

- **Shape Creation:** Use `BRepPrimAPI_` classes (`MakeBox`, `MakeCylinder`).
- **Boolean Operations:** Use `BRepAlgoAPI_` classes (`Fuse`, `Cut`, `Common`).
- **Shape Exploration:** Use `TopExp_Explorer`.
- **Transformations:** Use `gp_Trsf` and `BRepBuilderAPI_Transform`.

### Key Packages

| Package     | Purpose                               | Module                |
| ----------- | ------------------------------------- | --------------------- |
| `gp`        | Geometric Primitives (Points, Vecs)   | FoundationClasses     |
| `Geom`      | Geometric entities (Curves, Surfaces) | ModelingData          |
| `TopoDS`    | Topological Data Structures (Shapes)  | ModelingData          |
| `TopExp`    | Exploring topological shapes          | ModelingData          |
| `BRepAlgoAPI` | High-level modeling algorithms      | ModelingAlgorithms    |
| `BRepPrimAPI` | Geometric primitives creation       | ModelingAlgorithms    |
| `AIS`       | Application Interactive Services      | Visualization         |

### Common Headers
```cpp
#include <Standard_Failure.hxx>
#include <Handle_Geom_Circle.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
```

---

## 7. Key Files & Platform Notes

- **`adm/MODULES`**: Defines all modules and toolkits.
- **`src/*/FILES.cmake`**: Lists all source/header files for a toolkit. **You must edit this when adding/removing files.**
- **`build/env.sh/bat`**: The crucial environment script.
