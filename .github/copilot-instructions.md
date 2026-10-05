# AI instructions for OCCT-based development

Apply these rules to OCCT and OCCT-based applications. Applications keep their own layout and naming conventions; the final section covers the OCCT repository.

Use the checked-out implementation and build configuration as the source of truth; verify header comments against the code when behavior matters. OCCT releases differ; use APIs supported by the target version in changed code. The preferences and prohibitions below are project policy, not claims about every existing call site. Verify API guarantees for the selected overload, container family, and build configuration.

## 1. Fix the cause, not the symptom

- For a defect fix, establish the expected behavior, actual behavior, and smallest practical reproducer before changing code. Separate observations from hypotheses.
- Trace the failing data and control flow back to the first violated contract or incorrect calculation. Follow calls into low-level OCCT algorithms and related methods when necessary. The visible failure may be far from its origin.
- Fix the component that produces the incorrect result, not each caller that encounters it. Check other callers before changing a shared contract.
- Do not hide a defect with arbitrary tolerance increases, coordinate shifts, retries, extra shape healing, skipped entities, forced success, catch-all handlers, or special cases for one input.
- Input validation is appropriate at a documented boundary. It is not a substitute for fixing an internal invariant that valid input breaks. A null check must not silently turn an unexpected failure into an empty successful result.
- If the cause is outside the editable repository, identify the responsible implementation and evidence. Report the blocker or propose an upstream fix; do not silently introduce an application workaround. A temporary mitigation requires explicit user approval and a separate explanation of the unresolved defect.
- Add a regression test that fails for the original reason before the fix and passes afterward. Include nearby boundary cases and relevant callers when a shared algorithm changes.

## 2. Working procedure

1. Read applicable repository instructions, the relevant implementation, its callers, and existing tests. Inspect local changes and preserve work that is not yours.
2. Identify contracts, ownership, index bounds, units, tolerances, and failure reporting before choosing a change. Search for an existing OCCT facility before writing another one.
3. Define the acceptance criteria. For nontrivial work, state a short plan and the evidence needed to verify it. Ask only when missing information materially affects correctness or scope.
4. Implement the smallest complete change, including affected declarations, callers, source lists, tests, and documentation. For read-only tasks, report findings without changing files or build settings.
5. Run the narrowest relevant test first, then broader checks appropriate to the affected component. In OCCT, start with a focused GTest filter or the relevant Draw cases for Draw-specific changes; see section 9 for runner details. Use the repository's configured build rather than inventing paths or flags.
6. Review the diff for unintended behavior changes, allocation costs, invalidated references, and missing failure paths.
7. Report the result, changed behavior, files, exact validation performed, and any remaining uncertainty; include the root cause for defect fixes. Distinguish passing tests from checks not run, and report blockers when validation is blocked. Never claim passing builds, tests, or measured performance from inspection alone.

Keep changes focused on the requested task and, for defect fixes, the cause. Avoid unrelated cleanup, broad renaming, speculative refactoring, broad legacy-syntax migrations unrelated to the task, and new abstractions without a concrete need. Keep dependency and build-setting changes within that scope. Do not alter unrelated files, discard meaningful code, or weaken tests just to obtain a clean build. Do not commit or create branches unless explicitly requested.

### Follow the active subsystem

- Choose APIs from the subsystem being changed, its callers, and its tests, not from a fixed list of familiar OCCT classes.
- For BRepGraph work, inspect the relevant BRepGraph implementation and contracts first. Do not default to a legacy BRep algorithm or add conversions between representations without a concrete need.
- Reuse existing operations at the correct dependency level. Keep representation changes explicit and account for their ownership, correctness, and allocation costs.
- In applications, use the established test setup rather than adding a new framework without need. OCCT test selection and registration are covered in section 9.

## 3. Language and written output

- Use ASCII only in newly written code, comments, documentation, and task reports. No typographic quotes, Unicode arrows, mathematical symbols, decorative characters, or emoji. Preserve existing encoded test data; use escapes for new tests that require non-ASCII input.
- Use direct technical language and OCCT terminology. Name the actual object or operation: shape, face, edge, curve, surface, triangulation, buffer, result, allocation, conversion, or traversal.
- Do not introduce vague AI-style wording such as "opaque", "probe", "materialization", "payload", or "playground" in new names or prose. Also avoid filler such as "leverage", "seamless", "robust solution", "delve", "unlock", or "orchestrate" where a precise verb or explanation is available.
- Existing API identifiers and genuinely required domain terms must remain accurate; do not rename an established API merely to avoid a word. Explain its concrete meaning instead of repeating jargon.
- Write short, direct sentences in simple English (B1 level). Keep required technical terms. Use "OCCT" without expanding the name.
- Focus on critical decisions, constraints, and evidence. Omit obvious explanations, repeated rules, and narration of routine steps.

## 4. Ownership, lifetime, and operation status

- Use `occ::handle<T>` for dynamically owned `Standard_Transient`-derived objects, not separate owning raw pointers, `std::shared_ptr`, or `std::unique_ptr`. Preserve supported stack/value use, such as local adaptors. Never create a handle from a stack object or an object with another destruction owner. Use RAII for other resources.
- A borrowed pointer or reference required by an existing API is not ownership. Keep its owning object alive and do not store the borrow beyond that lifetime. Never manually delete an object managed by a handle.
- Use `occ::down_cast<Derived>(aHandle)` for handle downcasts. Establish non-nullness before dereferencing a nullable handle or downcast result. Follow a proven non-null contract rather than adding redundant checks that hide bugs.
- Use `TopoDS::Face`, `TopoDS::Edge`, and other `TopoDS` helpers for topological casts. Establish non-nullness before querying `ShapeType()`, then establish the required type through a check or proven contract. Preserve null shapes where permitted. Do not rely on mismatch exceptions when checks are disabled, or bypass checks with casts. Helper results borrow the input wrapper; copy the typed wrapper if it must outlive that input.
- Check the operation's actual status API before consuming results: `IsDone()`, error reports, return codes, or another documented mechanism. Not every OCCT API provides `IsDone()`.
- Preserve useful failure information. Catch specific exceptions only where recovery or translation belongs; do not swallow `Standard_Failure` or replace failure with a default result.
- Respect iterator, pointer, reference, and view invalidation after mutation, reallocation, moving, clearing, and allocator reset. Do not return references to local objects or temporary results.
- A copied handle shares an object. A copied `TopoDS_Shape` normally shares its underlying topology and geometry. Do not assume either is a deep copy; inspect mutation and copy requirements explicitly.
- Prefer `const T&` for borrowed, read-only containers, strings, and expensive objects; use `T&` only when mutation is intended. Bind returned references by reference when no independent copy is needed. Use `const auto&` in read-only range loops over expensive elements.
- Pass cheap scalar values and lightweight views such as `std::string_view` by value. Copy a handle when shared ownership must outlive the call; a borrowed reference alone does not retain the object.
- `const` on a handle or shape wrapper does not make all shared objects immutable. Reference counting does not make shared geometry, caches, algorithms, or allocators safe for concurrent mutation, and allocator locking does not make containers or elements thread-safe. Check the called API and shared-data contract; establish ownership and synchronization before adding parallelism.

## 5. Containers and strings

### Allowed types

Use `NCollection_*` containers and `TCollection_AsciiString` / `TCollection_ExtendedString` for owned collections and strings.

- Do not introduce STL containers or standard owned strings, including `std::vector`, `std::list`, `std::deque`, and standard associative containers, except `std::pair`, `std::tuple`, `std::array`, `std::queue`, and `std::stack`. Queue and stack adapters may use their implementation storage; this does not permit separate use of `std::deque`. An OCCT allocator does not create an exception.
- Existing external interfaces may expose such types. Consume required interface types without spreading them into new internal storage. Seek approval if an unavoidable integration requires a new prohibited container.
- Standard algorithms, type traits, `std::move`, `std::optional`, and `std::variant` remain available; the restrictions apply to owned collections and strings. Non-owning `std::string_view` and `std::u16string_view` are allowed for read-only string access.
- Use explicit `NCollection_*<T>` types, not deprecated compatibility aliases such as `TopTools_ListOfShape` or `TColgp_Array1OfPnt`. These aliases still exist in this checkout. `TColStd_PackedMapOfInteger` also remains available as an alias for `NCollection_PackedMap<int>`.
- Avoid `NCollection_Sequence` in new code. Select storage from the access pattern, element size, lifetime, and ordering requirements.

### Sequential storage

| Requirement | Preferred type and constraints |
| --- | --- |
| Growable contiguous storage, especially small elements | `NCollection_LinearVector<T>`. Growth can relocate elements and invalidate references. |
| Growable storage for large elements or large allocations | `NCollection_DynamicArray<T>`. Segmented storage avoids relocating existing elements on append. It is not a contiguous buffer. |
| Fixed-size one- or two-dimensional storage | `NCollection_Array1<T>` / `NCollection_Array2<T>`. Respect explicit bounds; do not assume all arrays start at 1. |
| A view over existing contiguous elements | Non-owning `Array1` / `Array2` constructors; see layout and lifetime rules below. |
| Temporary contiguous storage with a small inline-buffer case | `NCollection_LocalArray<T, N>`. The small buffer is inside the object; larger requests use heap storage. Keep inline capacity reasonable, especially for stack objects. |
| Required linked-node operations | `NCollection_List<T>`. Do not use a linked list for ordinary indexed access or cache-friendly traversal. |

### Array access and views

- Prefer `At()` / `ChangeAt()`: their `size_t` offsets are always zero-based, regardless of lower bounds. `Array2` accepts row/column offsets or a flat offset. Use `Value()` / `ChangeValue()` only when working with declared bounds; do not mix the two index conventions.
- Use non-owning constructors or `ToArray1()` to borrow contiguous storage from `LinearVector`, `LocalArray`, or an existing interface-provided `std::vector`. Wrap constructed elements, not capacity; do not dereference empty storage.
- For `Array2`, verify at least `rows * columns` elements in contiguous row-major order, with offset `row * columns + column`. Check element-count and byte-count overflow and the dimension limits of all operations used, not just construction. Wrapping does not validate buffer length or convert padded, strided, column-major, or pointer-to-pointer layouts.
- Keep the owner alive and its storage stable while a view is used. As a conservative rule, do not resize, move, clear, or reset its allocator during that time. Never write through a view of const storage, even if a helper exposes mutable access.
- Borrow arrays by `const&` for reading or `&` for mutation. Unlike a span, copying an array copies elements; resizing or assignment may detach a view into owned storage.

### String views

- Prefer a string view for synchronous read-only text access when the API does not need an owning TCollection string. Do not change established public signatures merely to introduce views.
- This checkout supports `static_cast<std::string_view>(anAsciiString)` and conversion of `TCollection_ExtendedString` to `std::u16string_view`. These borrow the stored characters without copying or changing their encoding.
- The backing storage must outlive string views. As a conservative rule, do not retain views across string mutation, move, or swap: these may change contents, range, or storage ownership without updating the view. Use owned text when an independent lifetime is needed.
- TCollection strings have a trailing null terminator; full views of unchanged strings retain it in the backing storage. General views and subviews may not end at a terminator. Prefer length-aware APIs; a C-string API ignores the view length and stops at the first null, including an embedded null.
- When converting a view back to owned TCollection storage, verify the selected overload's empty-input, embedded-null, size-range, and source-aliasing behavior. Do not assume every view overload preserves arbitrary counted text.
- String-view indices are zero-based; many TCollection operations use one-based indices. Check each API's index and not-found conventions. UTF-16 view indices count code units, not Unicode characters.

### Maps and sets

Decide first whether traversal order is required, whether integer index access is required, and whether entry addresses must remain stable.

| Requirement | Preferred types |
| --- | --- |
| Unordered lookup with light keys/values | `NCollection_FlatMap<K>` / `NCollection_FlatDataMap<K, V>` |
| Unordered lookup with heavy keys/values or node-based storage needs | `NCollection_Map<K>` / `NCollection_DataMap<K, V>` |
| Insertion-order traversal without integer index access | `NCollection_OrderedMap<K>` / `NCollection_OrderedDataMap<K, V>` |
| Lookup plus dense integer index access and index-order traversal | `NCollection_IndexedMap<K>` / `NCollection_IndexedDataMap<K, V>` |

- Light entries include `<int, int>` and `<occ::handle<T>, occ::handle<U>>`. Flat containers store entries in a slot array; mutation can relocate entries, even without growth. Apply the performance measurement rules below when performance matters.
- `Map`, `DataMap`, `FlatMap`, and `FlatDataMap` are unordered. Never depend on observed hash-table iteration order.
- Ordered containers preserve insertion order, not sorted key order. Indexed containers use 1-based indices; removal or swapping can change indices and traversal order. Do not persist an index as a stable identity across such operations.
- If sorted key order is required, implement it explicitly with suitable OCCT storage and sorting; neither "Ordered" nor "Flat" means sorted.
- Hashing and equality must express the intended identity and remain consistent. Never mutate a key in a way that changes its hash or equality while stored. For shapes, choose location/orientation identity semantics deliberately.

### Map lookup and insertion

- Avoid membership checks followed by the same lookup or insertion. Use `Seek` / `ChangeSeek` for optional pointer access where available; check for null before use. Use reference-returning lookup when presence is guaranteed, and use the insertion result instead of checking membership first. Check its type: indexed methods may return the existing or new index, not a boolean insertion flag. Method names and failure contracts differ by container family.
- In `DataMap`, `OrderedDataMap`, `FlatDataMap`, and `IndexedDataMap`, `Bind` / `Emplace` overwrite values; `TryBind` / `TryEmplace` preserve existing values. Use `TryBound` / `TryEmplaced` when the existing or new value reference is needed, avoiding another lookup. Verify other families separately.
- Set `Add` preserves an existing key, while set `Emplace` may replace an equivalent key. `IndexedDataMap::Add` preserves existing values. Do not assume STL insertion semantics from a similar method name.
- Pass mapped-value constructor arguments to data-map `TryEmplace` / `TryEmplaced` to avoid constructing the candidate value on a hit. Argument expressions still execute before the call: an expensive factory is not deferred. Keep an explicit lookup when needed to avoid that work. This guarantee does not cover key construction, table growth, or `TryBind` / `TryBound`.
- A duplicate insertion does not universally mean no allocation, moves, or reference invalidation. Inspect the actual method before reusing moved arguments or retained entry references, especially with flat containers.

### Capacity, reuse, and loops

- When a credible size estimate exists, configure initial capacity or reserve storage at construction or immediately afterward. Constructor sizing may be lazy: node-map constructors record a bucket count, while `ReSize()` allocates tables. Avoid repeated growth and arbitrary oversized reservations.
- Use the actual API: `LinearVector::Reserve(n)`, flat map `Reserve(n)`, or node-map `ReSize(n)`. Reserve capacity does not create elements, and map bucket allocation does not allocate every future node.
- `LinearVector(n)` reserves capacity without creating elements. `DynamicArray` constructor sizing controls its block increment; it is not an initial element count, and the current API has no public `Reserve()` or `Resize()`. Verify these details against the target version.
- Do not create a scratch container inside a sequential loop when it can be reused. Construct it outside, clear it each iteration, and retain suitable capacity. This applies to nested loops as well.
- For parallel loops, keep scratch storage local to each worker or task. Reuse it within that scope when possible; do not move it into shared mutable storage outside the parallel loop.
- A per-iteration container is allowed when its contents must outlive the iteration through ownership transfer or shared ownership. Use `std::move` when the receiving API supports transfer; do not accidentally copy it or keep references to a cleared scratch container. Merely passing a temporary container to a helper does not justify repeated allocation.
- In the listed containers with `Clear(bool)`, clearing destroys elements: `Clear(false)` retains reusable container storage; `Clear(true)` relinquishes it through the allocation mechanism. This does not reclaim pool bytes when allocator `Free()` is a no-op. Node maps retain tables, not their destroyed nodes. Lists use a different clear API.

### Incremental allocation

- Consider `occ::handle<NCollection_IncAllocator>` for repeated list, map, or dynamic-array workloads that append many objects and release them together, especially when individual erasure is unnecessary. Use only containers that support the allocator.
- Pool allocation is a lifetime decision, not an automatic optimization. Individual `Free()` calls do not reclaim incremental storage; repeated clearing without pool reset can still increase memory use.
- Before pool reset, destroy all live pool-backed elements and discard every container's retained pool-storage references. Clearing or destroying containers can satisfy this; container objects need not always be destroyed. `Reset()` does not call element destructors, and previous allocations must not be used afterward.
- `Reset(false)` rewinds retained blocks for reuse; `Reset(true)` releases them. Use the explicit flag when intent matters.
- Do not combine `DynamicArray::Clear(false)` with a reset of its allocator: the array still retains pool-backed storage. Use `Clear(true)` or destruction first. Audit every other user of a shared allocator before reset.
- Do not reset a pool while any view, result, moved container, or other thread can still access its allocations.

### Performance decisions

- Remove repeated work and unnecessary traversals before adding caches or parallelism. Consider algorithmic complexity, allocation count, peak memory, and data locality.
- Avoid unnecessary container copies, conversions, and temporary buffers. Use references or views for borrowing and moves for ownership transfer when their lifetime contracts allow it.
- Keep retained capacity proportional to the workload; do not keep a rare large allocation alive without a reuse need. A cache requires clear ownership, invalidation, and memory limits.
- For performance changes, compare representative inputs before and after using the same build settings. Measure runtime and memory where relevant, and verify unchanged results.

## 6. Geometry and topology correctness

- Keep coordinate systems, locations, orientations, and units explicit. Do not confuse 3D curve parameters, surface UV parameters, and physical distances.
- Use established OCCT precision facilities and relevant entity tolerances. Explain any new threshold in terms of the algorithm and units; do not choose an epsilon simply because it makes a failing case pass.
- Consider degenerate edges, seams, periodic surfaces, reversed orientations, empty shapes, and boundary parameter values when the affected algorithm permits them.
- Check mathematical and API preconditions: normalization thresholds, valid ranges, finite values where required, and valid derivative assumptions. For example, `gp_Vec::Normalize()` requires a magnitude above `gp::Resolution()`, not merely above zero. Handle documented degeneracy at the responsible algorithm level.
- Preserve topology/geometry consistency, including pcurves, locations, and tolerances when affected. Do not use meshing or display changes to conceal a modeling error.
- When topology changes, use validity checks for the active representation, such as `BRepCheck_Analyzer` for `TopoDS` shapes or the relevant BRepGraph checks. Do not add a conversion only to use a familiar checker. Validate the requested geometric result too; validity alone does not prove correctness.

### Numerical algorithms

- In new numerical code, prefer the `Math*_*` facilities (`MathLin`, `MathRoot`, `MathSys`, `MathOpt`, `MathInteg`, `MathPoly`, and `MathUtils`) over legacy `math_*` algorithms. Check the selected API's preconditions, tolerances, and result status; do not assume a drop-in replacement or migrate unrelated code.
- Numerical containers such as `math_Vector`, `math_Matrix`, and `math_IntegerVector` remain allowed. This preference concerns algorithms, not numerical storage types.

### Evaluation and bounding boxes

- In new geometry evaluation code, use `EvalD0`, `EvalD1`, `EvalD2`, `EvalD3`, and `EvalDN` instead of `Value`, `D0`...`DN` where supported. Prefer C++17 structured bindings for derivative result structs, for example `const auto [aPoint, aD1] = aCurve->EvalD1(aU);`. Request only the derivative order needed.
- For one-off evaluation, use the `Geom_*` object directly. For repeated evaluation, prefer reusing an appropriate adaptor, such as `GeomAdaptor_Curve` or `GeomAdaptor_Surface`, with its `EvalD*` methods. Preserve the required parameter bounds, derivative-side behavior, and placement; do not bypass an existing adaptor's semantics.
- For grid evaluation, prefer `GeomGridEval_*` evaluators and their `EvaluateGrid` / `EvaluateGridD*` methods over point-by-point loops. Reuse the evaluator and keep any borrowed adaptor alive. Check parameter layout and result dimensions; a surface grid evaluates the Cartesian product of U and V arrays.
- For `Bnd_Box` / `Bnd_Box2d`, prefer the `Limits` result from `Get()` over output parameters. Use named fields or structured bindings; the 3D order is `Xmin, Xmax, Ymin, Ymax, Zmin, Zmax`, unlike the legacy output-parameter order. Establish non-voidness before access and account for gap and open directions when finite limits are required.

## 7. C++ style and compatibility

- Follow the target project's configured C++ standard; OCCT code here uses C++17 or later. Do not raise the language requirement to simplify one change.
- Use native types (`int`, `double`, `bool`, `float`, `size_t`, `char16_t`, and appropriate fixed-width integers), not deprecated `Standard_*` primitive aliases.
- Use explicit types. Allow `auto` for syntax that requires it, lambdas, structured bindings, iterators, and range-based loop elements; not merely to shorten a type name or hide an "obvious" return type.
- Use `if constexpr` for compile-time branches where appropriate. Use `[[nodiscard]]` for results callers must check and `[[maybe_unused]]` for intentional unused declarations, not to hide unresolved warnings.
- Use `const` for unmodified values and `constexpr` for compile-time constants. Apply the borrowing and lifetime rules in section 4 and the copy/move guidance in section 5.
- Prefer `size_t` for container sizes, capacities, byte counts, and non-negative offsets. Use NCollection `Size()` and `size_t` overloads where available instead of narrowing to legacy `int` counts.
- Keep signed types for signed bounds, differences, and negative sentinels. Validate conversions, narrowing, subtraction, and size multiplication. `size_t` does not imply zero-based indexing or unrestricted large-size support: indexed maps store `int` indices, and some array/map operations narrow internally. Check the full operation path for large inputs.
- Prefer range-based loops for whole-container traversal where supported. Use indices when positions matter and explicit iterators when their operations are needed. Check iterator categories and element operations before applying standard algorithms.
- Prefer const-container iteration or its `cbegin()` / `cend()` for reading. Check view constness separately: a mutable `Items()` view retains mutable value references even through a const view wrapper. Use the const container's `Items()` overload for read-only entries.
- Direct iteration over `DataMap`, `OrderedDataMap`, `FlatDataMap`, and `IndexedDataMap` yields mapped values. For keys and values, use iterator key access or `Items()` where available. Structured bindings work with `Items()` entries, not by assuming the direct range yields pairs.
- For explorer-based traversal, initialize the explorer directly: `for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())`.
- Include directly required headers. Avoid reliance on transitive includes, unnecessary public-header dependencies, and unrequested public API/ABI changes.

Common legacy replacements when writing new code:

| Legacy type or value | Use |
| --- | --- |
| `Standard_Integer`, `Standard_Real`, `Standard_ShortReal` | `int`, `double`, `float` |
| `Standard_Boolean`, `Standard_True`, `Standard_False` | `bool`, `true`, `false` |
| `Standard_Character`, `Standard_ExtCharacter` | `char`, `char16_t` |
| `Standard_Byte`, `Standard_Size` | `uint8_t`, `size_t` |
| `Standard_Address`, `Standard_CString` | `void*`, `const char*` where the API requires them |
| `TopTools_ListOfShape` | `NCollection_List<TopoDS_Shape>` |
| `TopTools_MapOfShape` | `NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>` |
| `TColgp_Array1OfPnt`, `TColStd_Array1OfReal` | `NCollection_Array1<gp_Pnt>`, `NCollection_Array1<double>` |

These are type replacements, not automatic container choices. Select new storage using section 5.

Preserve custom hashers when replacing aliases. `TopTools_ShapeMapHasher` compares shapes with `IsSame()`, ignoring orientation; the default shape comparison uses `IsEqual()`, which includes orientation.

## 8. Comments and source layout

- Write production comments, not change history. Do not describe a bug fix, the old code, or why this patch was made inside the code; keep that in the change report. Document the lasting contract or constraint when needed.
- Comment non-trivial logic: intent, invariants, numerical assumptions, ownership, or ordering constraints. Keep comments short and in simple English; do not restate the code.
- A small ASCII diagram inside a function or method body is useful when it explains geometry, indexing, or data flow better than prose.

### Header documentation

Apply these requirements to new or materially changed declarations. Do not expand documentation of untouched code during an unrelated fix.

- Put API documentation on declarations, not definitions. Use `//!` Doxygen comments; declarations inside `.cxx` files may use ordinary `//` comments.
- Give each class a clear description of its purpose, use, and important behavior. Explain non-trivial contracts, ownership, and limitations in enough detail for correct use. Include focused usage samples with `@code` / `@endcode`; avoid repeating method documentation.
- Document public methods with their contract, meaningful parameter descriptions, return values, failure behavior, and ownership rules. Use `@param[in]`, `@param[out]`, `@param[in,out]`, and `@return` where applicable.
- Give private and protected methods short, useful comments about their role or constraints. Give each field a one-line comment describing its meaning, units, or invariant.

### Source files

- When implementation notes are needed, put them inside the method body: the overall approach near the start, and local constraints next to the relevant calculation.
- For new method and function definitions, use a separator (`//` followed by 98 `=` characters) and a blank line. This is the requested source-layout policy, not a rule enforced by clang-format; existing separator lengths vary. Do not add `function:` / `purpose:` banners or repeat header comments.
- For classes and structs declared in `.cxx` files, keep method declarations in the class and place definitions outside it. Do not introduce inline method bodies in these declarations.
- Put file-local helper functions and implementation-only classes or structs in an anonymous namespace. Keep their definitions in that namespace, with the same separators.

## 9. OCCT repository conventions

### Names and files

| Element | Convention |
| --- | --- |
| Class | `Package_ClassName` |
| Public method | `MethodName` |
| Private method | `methodName` |
| Parameter | `theParameter` |
| Local variable | `aValue`, `anIterator` |
| Class field | `myField` |
| Struct field | `FieldName` |
| Global constant | `THE_CONSTANT` |

- Put declarations and definitions in `.hxx` and `.cxx` files using the existing package style and repository formatter.

### Adding a class or package

1. Find the owning package and toolkit from its responsibility and dependency level. Inspect a nearby class and test before choosing the design.
2. Add the `.hxx` declaration and any required implementation files. Follow the package's template and inline conventions; do not create an empty `.cxx` for a header-only class. Follow neighboring include guards, export declarations (`Standard_EXPORT` where required), and RTTI conventions when applicable.
3. Register each added source or header in the package's `FILES.cmake`. Add focused tests using the test framework and registration rules below.
4. If a new package or dependency is needed, follow the metadata rules below. Do not create a new toolkit for a class that fits an existing one.
5. Regenerate the configured build if needed, build the affected targets, and run the new tests. Check public headers and linking, not only compilation of the implementation.

### Repository and build metadata

| Path | Purpose |
| --- | --- |
| `src/MODULES.cmake` | Module list |
| `src/Module/TOOLKITS.cmake` | Toolkits in a module |
| `src/Module/Toolkit/PACKAGES.cmake` | Packages in a toolkit |
| `src/Module/Toolkit/CMakeLists.txt`, `src/Module/Toolkit/FILES.cmake` | Toolkit target configuration and toolkit-level files |
| `src/Module/Toolkit/Package/FILES.cmake` | Source and header list |
| `src/Module/Toolkit/GTests/FILES.cmake` | GTest source list |
| `src/Module/Toolkit/EXTERNLIB.cmake` | Toolkit dependencies |
| `CMakeLists.txt`, `adm/` | Build configuration and supporting scripts |
| `tests/`, `samples/` | Draw regression tests and application examples |
| `.clang-format` | C++ formatting rules |

`Module`, `Toolkit`, and `Package` in this table describe the hierarchy, not literal directory names. Locate generated environment scripts and executables in the actual build; their paths vary by platform and configuration.

- When removing files, update the owning package's `FILES.cmake` as well. Follow existing custom CMake registration for generated files.
- Update `PACKAGES.cmake`, `EXTERNLIB.cmake`, or module/toolkit lists only when the change actually affects those boundaries. Keep lower-level packages independent of higher-level ones.

### Test framework and registration

- Prefer GTest for AI-assisted development: new tests, bug regressions, and direct C++ contract checks. Put tests in `src/Module/Toolkit/GTests/`, named `ClassName_Test.cxx` or `PackageName_Test.cxx`; update the corresponding `FILES.cmake`.
- Use Draw Harness when testing Draw commands or when the required integration setup depends on Draw. Existing Draw coverage alone is not a reason to prefer it over GTest. Keep relevant existing Draw regressions and run them when affected.
- Draw Harness uses Tcl tests in `tests/` and runs through `DRAWEXE`. Follow the relevant suite's setup, result checks, and test-data handling.
- Use the repository's test frameworks rather than standalone test programs.

### Build and test dependencies

- Build out of source. Reuse the existing CMake configuration, target names, and selected toolkits; enable `BUILD_GTEST` only as needed. Do not copy a generic `cmake ..` command that may change options or use the wrong source directory. Run the executable from the matching build configuration.
- `OpenCascadeGTest` is one common executable. It links all existing static/shared library targets in the final `BUILD_TOOLKITS` list and Google Test. Third-party link dependencies propagate through the configured toolkit targets. A test's source package does not restrict its link access.
- Do not add production toolkit dependencies just to support a test. Common test linking can hide missing production dependencies; verify the affected toolkit's own build and link requirements separately.

### Running tests

- Use `--gtest_list_tests` on the configured test executable to find suites, then `--gtest_filter=SuiteName.TestName` for focused runs. Verify that the intended tests actually ran; zero matching or executed tests is not a pass.
- Run GTests through CTest or directly with `OpenCascadeGTest`. Prefer direct execution for focused runs: one runner process executes selected tests sequentially, avoiding repeated startup. Tests and OCCT algorithms can use their own threads or subprocesses; this is separate from test scheduling.
- CTest starts a process per registered entry and can run entries in parallel with `-j`. Typed or parameterized entries may group several GTest instances. Use `ctest -N` to inspect names, `ctest -R` with `--output-on-failure` to run a selection, and `-C` for multi-configuration builds when required. Do not assume either runner is always faster.
- Rerun CMake with the existing configuration after changing test declarations, manifests, or relevant build definitions. This checkout uses `gtest_add_tests` with `SKIP_DEPENDENCY` and suppresses automatic CMake regeneration. Registration scans source text; compare CTest entries with the matching binary's test listing, especially for conditional or macro-generated tests.
- On macOS and Linux, prefer direct `OpenCascadeGTest` execution without sourcing `env.sh` first. Verify the executable path and library loading in the selected build; no script is inherently required, but loader paths can depend on the build and installation layout. A successful test listing verifies startup, not resource-dependent test execution.
- Direct execution does not inherit CTest's working directory or test environment. For tests that need resources or specific runtime settings, use CTest or supply the same required settings from the configured test properties.
- For other executables and platforms, use the build's environment setup (`env.sh` or `env.bat`) only when required.

### GTest design and assertions

- Regression tests must check behavior, not implementation accidents. Keep tests and their geometry deterministic; do not rely on hash iteration order, timing, or machine-specific file locations.
- For numerical or allocation-sensitive changes, include representative boundary cases and relevant performance/memory checks.
- Use `TEST` for independent cases. Use `TEST_F` and a `testing::Test` fixture only when several cases need shared setup or cleanup; do not add empty fixtures.
- Follow the surrounding suite names. Prefer `ClassNameTest.MethodOrFeature_Scenario_ExpectedBehavior` when adding a new suite, for example `gp_PntTest.Distance_SamePoint_ReturnsZero`.
- Keep setup, operation, and checks distinct in the test body. Each test must run on its own; do not depend on another test's state or execution order.
- Use `ASSERT_*` for prerequisites that make later access unsafe, such as failed construction, null results, or missing array entries. Use `EXPECT_*` for independent checks so one failure does not hide the others.
- Use `EXPECT_EQ` for exact values and `EXPECT_NEAR` with a justified tolerance for computed floating-point values. Use `EXPECT_THROW` with the expected exception type when testing a failure contract.
- Before using `EXPECT_THROW` or `ASSERT_THROW`, inspect the throwing path and its build guards. Checks such as `Standard_OutOfRange_Raise_if` are disabled by `No_Exception` or the corresponding per-exception macro, such as `No_Standard_OutOfRange`. Guard the exception assertion and its invalid-input call with the same conditions; do not execute that call when its safety check is compiled out.
- Do not replace a disabled exception assertion with `EXPECT_NO_THROW`: unchecked invalid input may cause undefined behavior. Keep valid-input coverage active. `No_Exception` does not disable C++ exceptions in general. Explicit throws and `Standard_OutOfRange_Always_Raise_if` are not disabled by that macro alone; inspect surrounding guards and the implementation's compile configuration, including out-of-line library code.
- Check the requested result, not just `IsDone()` or absence of an exception. Depending on the operation, check dimensions, distances, topology, orientation, or validity.
- Include `<gtest/gtest.h>` and apply the direct-header rule in section 7.
- Keep GTests self-contained: create test inputs and geometry in code, without external data files. Keep new geometry small when possible.
