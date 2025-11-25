# OCCT AI Assistant Guidelines

This document provides comprehensive guidance for AI assistants (GitHub Copilot, Claude, ChatGPT, and others) working with the Open CASCADE Technology (OCCT) C++17+ 3D CAD/CAM/CAE library.

---

## 1. Core Directives

> **IMPORTANT:** These are the most critical rules. Follow them strictly.

1.  **Memory Management is Paramount:**
    - **ALWAYS** use `Handle(ClassName)` for any class inheriting from `Standard_Transient` (e.g., `Geom_*`, `Poly_*`, `AIS_*`, `V3d_*`). This is for reference-counted objects.
    - **NEVER** use raw pointers (`ClassName*`) for these types, as it will cause memory leaks.
    - **Correct:** `Handle(Geom_Circle) aCircle = new Geom_Circle(...);`
    - **Wrong:** `Geom_Circle* aCircle = new Geom_Circle(...);`

2.  **Check Operation Status:**
    - After using an API that performs a geometric operation (e.g., `BRepAlgoAPI_Fuse`, `BRepBuilderAPI_MakeEdge`), **ALWAYS** check if the operation was successful using the `IsDone()` method before accessing the result.
    - **Correct:**
      ```cpp
      BRepAlgoAPI_Fuse aFuser(theShape1, theShape2);
      if (aFuser.IsDone())
      {
        const TopoDS_Shape& aResult = aFuser.Shape();
      }
      else
      {
        // Handle error
      }
      ```

3.  **Strict Naming and File Conventions:**
    - Adhere to the strict `Package_ClassName` convention.
    - Place new files in the correct directory: `src/Module/Toolkit/Package/`.
    - After adding a file, **ALWAYS** update the corresponding `src/Module/Toolkit/Package/FILES.cmake` file.

4.  **Use Modern C++ Idioms:**
    - Use range-based `for` loops and structured bindings where applicable.
    - Use the modern `TopExp_Explorer` constructor style.
    - **Correct:** `for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next()) { ... }`
    - **Wrong:** `for (TopExp_Explorer anExp; anExp.Init(theShape, TopAbs_FACE); anExp.More(); anExp.Next()) { ... }`

5.  **Safe Type Casting:**
    - When downcasting topological shapes, **ALWAYS** use the `TopoDS` helper functions to avoid errors.
    - **Correct:** `const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());`
    - **Wrong:** `const TopoDS_Face& aFace = (const TopoDS_Face&)anExp.Current();`

6.  **Handle Downcasting:**
    - Use `Handle(DerivedClass)::DownCast(BaseHandle)` to safely downcast handles.
    - **Correct:**
      ```cpp
      const Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(aCurveHandle);
      ```
    - **Wrong:**
      ```cpp
      // Do not use C-style casts on handles
      Geom_Circle* aCircle = (Geom_Circle*)aHandle.Get();
      ```

7.  **Validate Handles for Null Safety:**
    - **ALWAYS** check that a `Handle(ClassName)` is not null before dereferencing it:
      ```cpp
      if (!theHandle.IsNull())
      {
        // use theHandle
      }
      ```

---

## 2. Project Overview

Open CASCADE Technology (OCCT) is a comprehensive C++ software development platform for 3D surface and solid modeling, CAD data exchange, and visualization. It's a modern C++17+ library providing services for CAD/CAM/CAE applications.

### Architecture & Source Organization

- **`src/`**: Source code, organized by a `Module/Toolkit/Package` hierarchy.
- **`tests/`**: Draw Harness test files, organized by functionality.
- **`adm/`**: Administrative and build configuration files.
- **`samples/`**: Example applications.

---

## 3. Code Conventions

### Naming Patterns

| Element Type             | Pattern               | Example                        |
| ------------------------ | --------------------- | ------------------------------ |
| **Classes**              | `Package_ClassName`   | `TopoDS_Shape`, `BRep_Builder` |
| **Public Methods**       | `MethodName`          | `IsDone()`, `Shape()`          |
| **Private Methods**      | `methodName`          | `myInternalMethod()`           |
| **Method Parameters**    | `theParameterName`    | `theShape`, `theTolerance`     |
| **Local Variables**      | `aVariableName`       | `aBox`, `anExplorer`           |
| **Class Member Fields**  | `myFieldName`         | `myShape`, `myIsDone`          |
| **Struct Member Fields** | `FieldVariableName`   | `Point`, `Value`               |
| **Global Variables**     | `THE_GLOBAL_VARIABLE` | `THE_DEFAULT_PRECISION`        |

### Primitive Types

Use standard C++ primitive types in new code:

| Type     | Usage                        |
| -------- | ---------------------------- |
| `int`    | Integer values               |
| `double` | Floating-point numbers       |
| `bool`   | Boolean values (true/false)  |
| `float`  | Single-precision when needed |

> **Note:** Legacy code uses `Standard_Integer`, `Standard_Real`, `Standard_Boolean`. These are typedefs to the native types above. In new code, prefer native types directly.

### Strings and Collections

**ALWAYS** use OCCT collection classes instead of STL containers:

| OCCT Type                         | Instead of                | Description                |
| --------------------------------- | ------------------------- | -------------------------- |
| `TCollection_AsciiString`         | `std::string`             | ASCII string               |
| `TCollection_ExtendedString`      | `std::wstring`            | Unicode string             |
| `NCollection_Array1<T>`           | `std::vector<T>`          | Fixed-size array (1-based) |
| `NCollection_Vector<T>`           | `std::vector<T>`          | Dynamic array              |
| `NCollection_List<T>`             | `std::list<T>`            | Doubly-linked list         |
| `NCollection_Sequence<T>`         | `std::deque<T>`           | Indexed sequence           |
| `NCollection_Map<K>`              | `std::unordered_set<K>`   | Hash set                   |
| `NCollection_DataMap<K,V>`        | `std::unordered_map<K,V>` | Hash map                   |
| `NCollection_IndexedMap<K>`       | -                         | Indexed hash set           |
| `NCollection_IndexedDataMap<K,V>` | -                         | Indexed hash map           |

**If STL containers are absolutely necessary**, use OCCT's allocators:

```cpp
#include <NCollection_Allocator.hxx>
#include <NCollection_OccAllocator.hxx>
#include <NCollection_IncAllocator.hxx>

// Option 1: NCollection_Allocator - uses standard OCCT memory allocation
std::vector<gp_Pnt, NCollection_Allocator<gp_Pnt>> aPoints;
std::list<TopoDS_Shape, NCollection_Allocator<TopoDS_Shape>> aShapes;

// Option 2: NCollection_OccAllocator - with custom memory pool (preferred for performance)
Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();
NCollection_OccAllocator<gp_Pnt> anAllocator(anIncAlloc);
std::vector<gp_Pnt, NCollection_OccAllocator<gp_Pnt>> aPooledPoints(anAllocator);
```

### Modern C++ Features

This is a C++17+ codebase. Use modern features where appropriate:
- Range-based `for` loops
- Structured bindings: `for (const auto& [aKey, aValue] : aMap)`
- `std::optional` for optional return values
- `if constexpr` for compile-time conditions
- `[[nodiscard]]`, `[[maybe_unused]]` attributes where appropriate

### Use of `auto` Keyword

Use `auto` **only** in the following cases:
- Where syntax requires it (templates, structured bindings, lambdas)
- To omit the type of object in range-based `for` loops
- To omit the type of container iterators: `auto anIter = aContainer.begin()`

**Avoid** `auto` in these cases:
- To omit long type names (use type aliases instead)
- To omit "obvious" return types
- Simply to avoid typing the actual type

Readability is more important than brevity. Always prefer explicit types.

### Const Correctness

Use `const` for variables that will not be modified after initialization:

```cpp
const double aTolerance = 0.001;
const TopoDS_Shape aShape = aBuilder.Shape();
```

Use `constexpr` for values that can be computed at compile time:

```cpp
constexpr int THE_MAX_ITERATIONS = 100;
constexpr double THE_PI = 3.14159265358979323846;
```

Prefer `const` references to avoid unnecessary copies:

```cpp
void ProcessShape(const TopoDS_Shape& theShape);

for (const TopoDS_Face& aFace : aFaces)
{
  // ...
}
```

---

## 4. Step-by-Step Workflow: Adding a New Class

This example demonstrates adding a new class `BRepLib_MyNewClass` to the `BRepLib` package in the `TKTopAlgo` toolkit.

**1. Create Header and Source Files:**
```bash
cd src/ModelingAlgorithms/TKTopAlgo/BRepLib
touch BRepLib_MyNewClass.hxx BRepLib_MyNewClass.cxx
```

**2. Implement the Class:**
Add content following all code conventions (see sections 3 and 7).

**3. Add Files to Package CMake:**
Edit `src/ModelingAlgorithms/TKTopAlgo/BRepLib/FILES.cmake`:
```cmake
set(OCCT_BRepLib_FILES
  ...
  BRepLib_MyNewClass.hxx
  BRepLib_MyNewClass.cxx
)
```

**4. Create a GTest:**
```bash
cd src/ModelingAlgorithms/TKTopAlgo/GTests
touch BRepLib_MyNewClass_Test.cxx
```

**5. Add GTest to CMake:**
Edit `src/ModelingAlgorithms/TKTopAlgo/GTests/FILES.cmake`:
```cmake
set(OCCT_TKTopAlgo_GTests_FILES
  ...
  BRepLib_MyNewClass_Test.cxx
)
```

**6. Build and Run:**
```bash
cd build
cmake .. -DBUILD_GTEST=ON
cmake --build . --config Release
./bin/OpenCascadeGTest --gtest_filter="*MyNewClass*"
```

---

## 5. Build and Test System

### Build System (CMake)
- **Primary build system:** CMake 3.16+
- **Build Directory:** Always build out-of-source (e.g., `build/`)
- **Quick Build:**
  ```bash
  mkdir -p build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_GTEST=ON
  cmake --build . --config Release --parallel
  ```
- **Environment:** Before running OCCT executables, source the environment:
  - Linux/macOS: `source build/env.sh`
  - Windows: `build\env.bat`

### Testing Frameworks
- **Draw Harness:** Tcl-based interactive testing in `tests/`. Run with `DRAWEXE`.
- **GTest:** C++ unit tests in `src/.../GTests/`. Enable with `-DBUILD_GTEST=ON`.

---

## 6. Common Patterns & Key Packages

### Common Operations

- **Shape Creation:** `BRepPrimAPI_MakeBox`, `BRepPrimAPI_MakeCylinder`
- **Boolean Operations:** `BRepAlgoAPI_Fuse`, `BRepAlgoAPI_Cut`, `BRepAlgoAPI_Common`
- **Shape Exploration:** `TopExp_Explorer`
- **Transformations:** `gp_Trsf` with `BRepBuilderAPI_Transform`

### Key Packages

| Package       | Purpose                               | Module             |
| ------------- | ------------------------------------- | ------------------ |
| `gp`          | Geometric Primitives (Points, Vecs)   | FoundationClasses  |
| `Geom`        | Geometric entities (Curves, Surfaces) | ModelingData       |
| `TopoDS`      | Topological Data Structures           | ModelingData       |
| `TopExp`      | Exploring topological shapes          | ModelingData       |
| `BRepAlgoAPI` | High-level modeling algorithms        | ModelingAlgorithms |
| `BRepPrimAPI` | Geometric primitives creation         | ModelingAlgorithms |
| `AIS`         | Application Interactive Services      | Visualization      |

### Common Headers
```cpp
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Geom_Circle.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_Failure.hxx>
```

---

## 7. Code Documentation Style

### Header Files (.hxx) - Method Documentation

Use Doxygen-style comments with `//!` prefix:

```cpp
//! Brief description of the method functionality.
//! Additional details if needed.
//! @param[in] theInputParam description of input parameter
//! @param[out] theOutputParam description of output parameter
//! @param[in,out] theInOutParam description of in/out parameter
//! @return description of return value
Standard_EXPORT bool MethodName(const InputType& theInputParam,
                                OutputType&      theOutputParam);
```

**Rules:**
- `@param[in]` for input parameters
- `@param[out]` for output parameters
- `@param[in,out]` for bidirectional parameters
- `@return` for return value description

### Source Files (.cxx) - Method Separators

Each method **MUST** be preceded by:
1. A separator line of exactly 100 characters: `//` followed by 98 `=` signs
2. An empty line after the separator

```cpp
//==================================================================================================

void MyClass::MyMethod(const TopoDS_Shape& theShape)
{
  // Implementation
}

//==================================================================================================

bool MyClass::AnotherMethod()
{
  // Implementation
}
```

**DO NOT use old-style function guards:**
```cpp
// WRONG - Do not use this style:
//==================================================================================================
// purpose: Description of what the method does
// function: MyClass::MyMethod
//==================================================================================================
```

### Technical Comments

Place implementation notes inside the method body:
- At the beginning for overall approach
- Inline where specific logic needs explanation

```cpp
//==================================================================================================

void MyClass::ComplexMethod(const TopoDS_Shape& theShape)
{
  // Using iterative approach for better performance with large shape hierarchies.
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    // Handle degenerate faces separately
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    // ...
  }
}
```

---

## 8. GTest Guidelines

### Test Location and Naming

- Location: `src/Module/Toolkit/GTests/`
- File naming: `ClassName_Test.cxx` or `PackageName_Test.cxx`

### Test Structure

```cpp
#include <gtest/gtest.h>

#include <MyClass.hxx>

// Test fixture (optional)
class MyClassTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Setup code
  }
};

// Test with fixture
TEST_F(MyClassTest, MethodName_Scenario)
{
  // Arrange
  MyClass anObject;

  // Act
  const bool isSuccess = anObject.SomeMethod();

  // Assert
  EXPECT_TRUE(isSuccess);
}

// Standalone test
TEST(MyClassTest, BasicFunctionality)
{
  // Test implementation
}
```

### Naming Convention

Pattern: `TestFixture.MethodOrFeature_Scenario_ExpectedBehavior`

Examples:
- `DE_WrapperTest.Read_ValidSTEPFile_ReturnsTrue`
- `BRepAlgoAPI_FuseTest.TwoBoxes_ProducesValidShape`
- `gp_PntTest.Distance_SamePoint_ReturnsZero`

### Assertions

- `EXPECT_*` - non-fatal (test continues)
- `ASSERT_*` - fatal (test stops)

Common assertions:
- `EXPECT_TRUE(condition)` / `EXPECT_FALSE(condition)`
- `EXPECT_EQ(actual, expected)` / `EXPECT_NE(actual, expected)`
- `EXPECT_NEAR(actual, expected, tolerance)` - for floating-point
- `EXPECT_THROW(statement, exception_type)` - for exceptions

### Running Tests

```bash
# Build
cmake .. -DBUILD_GTEST=ON
cmake --build . --config Release

# Run all
./bin/OpenCascadeGTest

# Run filtered
./bin/OpenCascadeGTest --gtest_filter="*MyClass*"

# List tests
./bin/OpenCascadeGTest --gtest_list_tests
```

---

## 9. Key Project Files

| File                                        | Purpose                                                |
| ------------------------------------------- | ------------------------------------------------------ |
| `src/MODULES.cmake`                         | Defines all modules                                    |
| `src/Module/TOOLKITS.cmake`                 | Defines toolkits within a module                       |
| `src/Module/Toolkit/PACKAGES.cmake`         | Defines packages within a toolkit                      |
| `src/Module/Toolkit/Package/FILES.cmake`    | Lists source/header files (**edit when adding files**) |
| `src/Module/Toolkit/GTests/FILES.cmake`     | Lists GTest source files                               |
| `src/Module/Toolkit/EXTERNLIB.cmake`        | External library dependencies                          |
| `build/env.sh` / `build/env.bat`            | Environment setup script                               |
