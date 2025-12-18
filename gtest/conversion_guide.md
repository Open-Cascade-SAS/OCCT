# OCCT Tcl-to-Gtest Migration Guide & Prompts

To ensure a smooth and orderly conversion of the vast number of tests in `tests/` into Gtest format, follow this structured approach.

## 1. Preparation Strategy

Don't convert everything at once. Divide the work into "Functional Domains":
- **Geometry**: Primitive creation, intersections, projections.
- **Topology**: Boolean operations, fillets, offsets.
- **OCAF**: Attribute management, document undo/redo.
- **Data Exchange**: STEP/IGES/GLTF loading (non-interactive).

---

## 2. The "Orderly Migration" Prompt

Use the following prompt to initialize the AI for a specific set of tests:

```markdown
### Role & Context
You are an expert in Open CASCADE (OCCT) and Google Test (Gtest). Your task is to convert Tcl-based automated tests into optimized, direct C++ API Gtest cases.

### Input Files
- Tcl script to convert: [Path to Tcl file]
- Reference test file to append: [Path to Bugs_Regression_GTest.cxx]

### Conversion Requirements
1. **No Tcl Bridge**: Do NOT use `Draw_Interpretor` or `Eval`. Call OCCT C++ classes directly.
2. **Handle Management**: Use `Handle(T)` for transient objects.
3. **Assertions**: 
   - Use `ASSERT_NO_THROW` for operations that shouldn't crash.
   - Use `EXPECT_TRUE(App.IsDone())` for algorithm status.
   - Use `EXPECT_NEAR(val, expected, 1e-12)` for geometric values.
4. **Setup Logic**: Translate Tcl commands like `box`, `sphere`, `circle` into their `BRepPrimAPI` or `GC_Make` equivalents.
5. **Clean Headers**: Identify and include only the necessary OCCT headers.

### Steps to Follow
1. Read the Tcl script and identify the core algorithm being tested.
2. Find the OCCT C++ classes that implement those Draw commands (e.g., `appsurf` -> `GeomFill_AppSurf`).
3. Generate a professional C++ test function following the naming convention: `TEST(TestSuite, BugID_Description)`.
4. Suggest any new libraries that need to be added to `gtest/CMakeLists.txt` based on the new dependencies.
```

---

## 3. Standardized Test Templates

To maintain code uniformity, use the following templates for every new test case.

### Case Template A: Basic Geometric Check
Use this for simple shape creation and measurement.
```cpp
/**
 * @brief Regression test for [Bug ID/Feature].
 * Detailed description: [What it tests and why].
 */
TEST(TestSuite, BugID_ShortDescription)
{
  // 1. Inputs & Parameters
  const Standard_Real aTol = Precision::Confusion();
  gp_Pnt aPnt(0.0, 0.0, 0.0);

  // 2. Algorithm Execution
  BRepBuilderAPI_MakeVertex aMakeVertex(aPnt);
  ASSERT_TRUE(aMakeVertex.IsDone());
  const TopoDS_Shape& aResult = aMakeVertex.Shape();

  // 3. Assertions & Validation
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult.ShapeType(), TopAbs_VERTEX);
  // EXPECT_NEAR(...);
}
```

### Case Template B: Complex Algorithm (Exception Trap)
Use this for operations known to crash or throw in legacy versions.
```cpp
/**
 * @brief Exception verification for [Command Name].
 * Original Tcl: [Tcl commands snippet]
 */
TEST(TestSuite, BugID_StabilityVerification)
{
  // 1. Data Setup (Curves, Surfaces, etc.)
  // Handle(Geom_Curve) aCurve = ...

  // 2. Algorithm Initialization
  // GeomFill_AppSurf anAlgo(...);

  // 3. Stability Check
  // We use ASSERT_NO_THROW to ensure the algorithm doesn't crash
  ASSERT_NO_THROW({
    // anAlgo.Perform(...);
  });

  // 4. Result Verification
  // EXPECT_TRUE(anAlgo.IsDone());
}
```

---

## 4. The "Validation & Order" Prompt

After the AI generates the code, use this prompt to ensure correctness:

```markdown
Review the generated C++ test code for:
1. **Header Completeness**: Are all classes like `gp_Pnt`, `Precision`, and `BRep_Tool` included?
2. **Tolerance**: Are geometric comparisons using an appropriate epsilon (e.g., `Precision::Confusion()` or `1e-7`)?
3. **Naming**: Does the test name clearly link to the original bug report or feature?
4. **Memory**: Are there any potential memory leaks or incorrect `Handle` usages?
5. **CMake**: List the specific `TK*` toolkits that must be linked in `gtest/CMakeLists.txt` for this test to compile.
```

---

## 5. Technical Mapping Reference (Cheat Sheet)

| Tcl Command | OCCT C++ Class | Header |
| :--- | :--- | :--- |
| `box` | `BRepPrimAPI_MakeBox` | `BRepPrimAPI_MakeBox.hxx` |
| `vertex` | `BRepBuilderAPI_MakeVertex` | `BRepBuilderAPI_MakeVertex.hxx` |
| `circle` | `gp_Circ` / `Geom_Circle` | `gp_Circ.hxx` |
| `bop` | `BRepAlgoAPI_BooleanOperation` | `BRepAlgoAPI_BooleanOperation.hxx` |
| `whatis` | Internal checks | Check `Shape().ShapeType()` |
| `appsurf` | `GeomFill_AppSurf` | `GeomFill_AppSurf.hxx` |

---

## 6. Web-AI Mega-Prompt (Self-Contained Context)

If you are using a web-based AI (like ChatGPT or Gemini Web App) that doesn't have access to your local project, copy and paste the entire block below as your first message. Replace the placeholder with your Tcl code.

```markdown
# Role: Expert OCCT C++ Developer
You are tasked with converting an Open CASCADE (OCCT) Tcl test script into a C++ Google Test (Gtest) case.

# Project Context:
- Framework: Gtest (Google Test).
- Target Style: Direct C++ API calls (no Tcl bridge, no Draw_Interpretor).
- Memory Management: OCCT uses reference-counted handles. Use `Handle(ClassName) object = new ClassName(...)`.
- Naming Convention: Test suite `Bugs`, test name `BugID_Description`. Local variables use `a` prefix (e.g., `aPnt`).

# Technical Reference (OCCT Mapping):
- `box` -> `BRepPrimAPI_MakeBox`
- `vertex` -> `BRepBuilderAPI_MakeVertex`
- `circle` -> `gp_Circ` / `Geom_Circle`
- `appsurf` -> `GeomFill_AppSurf` (requires `GeomFill_SectionGenerator` and `GeomFill_Line`)
- Toolkits typically needed: `TKMath`, `TKernel`, `TKBRep`, `TKGeomBase`, `TKGeomAlgo`.

# Output Requirements:
1. Provide a professional C++ test case using the structure:
   ```cpp
   #include <gtest/gtest.h>
   // Necessary OCCT headers...

   TEST(Bugs, BugID_Description) {
     // 1. Data setup
     // 2. Algorithm execution
     // 3. ASSERT_NO_THROW(...) or EXPECT_TRUE(algo.IsDone())
   }
   ```
2. Mention any OCCT toolkits (TK*) that need to be linked.

# Tcl Script to Convert:
[PASTE YOUR TCL CODE HERE]
```

---

## 7. Automated Collection Tool (Python)

To handle a large number of tests (like those in `tests/bugs/filling`), use the provided Python script to collect, filter, and batch tests.

### Location:
`gtest/tools/collect_tests.py`

### How to use:
1. **Run the script** on a specific test category folder:
   ```bash
   python gtest/tools/collect_tests.py tests/bugs/filling --batch 5 --out filling_batches.md
   ```

2. **What it does**:
   - **Filters**: Automatically skips interactive tests (e.g., those using `vinit`, `vdisplay`) which are unsuitable for simple C++ unit tests.
   - **Naming**: Suggests professional `TestSuite` and `TestCase` names based on OCCT folder structure.
   - **Batching**: Groups multiple scripts into a single Markdown block to maximize AI context efficiency while keeping it organized.

3. **Output**:
   It generates a `.md` file. Open this file, copy a batch block, and paste it to the AI along with the **Web-AI Mega-Prompt**.
