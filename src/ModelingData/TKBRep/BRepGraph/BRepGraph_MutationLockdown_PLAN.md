# BRepGraph Mutation-Surface Lockdown — Implementation Plan

**Target branch base:** `fb92c080fa` (Phases 1–5 of incremental reverse-index already merged; 905 BRepGraph tests + 7007 full-suite green at this commit)
**Working tree at hand-off:** `BRepGraph_MutGuard.hxx` is **already partly migrated** — it has const-only `operator->/*`, a `myDirty` flag, `MarkDirty()` / `IsDirty()` / `Internal()` methods, conditional notification in `~MutGuard`. This breaks the build (~228 callsites can't compile through const operator->). That breakage is intentional — it's the starting wedge for the migration.
**Audience:** the AI/engineer who will execute this refactor end-to-end.

---

## 1. Goal

Lock down the BRepGraph mutation API so that:

1. External callers cannot mutate any field of a `Def`, `Ref`, or `Rep` entity via `Mut*()` field-access. All field writes must go through Editor's typed setters.
2. The `Mut*()` family is recast as a **scope token**: it batches notifications so multiple field writes inside one scope coalesce into a single `markModified` / `markRefModified` / `markRepModified` call.
3. A "forgot to update the reverse-index" bug becomes a compile error: any connectivity-vector mutation (e.g. `WireRefIds.Append`, `VertexRefIds.EraseLast`) is unreachable from outside `BRepGraphInc_Storage`.
4. No backdoor `Privileged` accessor; no full reader-side migration of every `aFace.WireRefIds.Size()` access. Reads stay zero-cost.

Phases 1–5 already migrated all connectivity writes to incremental `Bind/Unbind` primitives, so this refactor is **purely about tightening the public API surface**, not about correctness of reverse-index maintenance.

---

## 2. Why this matters (background)

`BRepGraph_MutGuard<T>` today does two things at once:

- **Field access.** `operator->` returns a non-const `T*`, so callers do `aGuard->Tolerance = 0.5;`.
- **Scope batching.** `~MutGuard` calls `markModified(id)` exactly once on scope exit, regardless of how many fields were touched.

The first role is a bug surface: any Def/Ref struct field is freely writable, including connectivity vectors (`WireRefIds`, `VertexRefIds`, `CoEdgeRefIds` …). A caller can do
```cpp
aGuard->WireRefIds.Append(someRefId);   // forward field updated
// reverse index NOT updated — graph corrupt
```
and the compiler is silent.

The second role (scope batching) is genuinely useful and we keep it.

**Design fix:** drop field-access via `Mut*()`; keep batching. All field writes go through Editor's typed setter API; setters internally pair forward writes with reverse-index updates (already in Storage from Phase 1–5).

---

## 3. Architecture

### 3.1 `BRepGraph_MutGuard<T>` becomes read-only + dirty-tracking

```cpp
template <typename T>
class BRepGraph_MutGuard
{
public:
  BRepGraph_MutGuard(BRepGraph*, T*, TypeId);
  ~BRepGraph_MutGuard();                     // fires notify only if myDirty

  BRepGraph_MutGuard(BRepGraph_MutGuard&&) noexcept;
  BRepGraph_MutGuard& operator=(BRepGraph_MutGuard&&) noexcept;
  BRepGraph_MutGuard(const BRepGraph_MutGuard&)            = delete;
  BRepGraph_MutGuard& operator=(const BRepGraph_MutGuard&) = delete;

  // READ-ONLY external access.
  [[nodiscard]] const T* operator->() const;
  [[nodiscard]] const T& operator*()  const;

  [[nodiscard]] explicit operator bool() const noexcept;
  [[nodiscard]] TypeId   Id()    const noexcept;
  [[nodiscard]] BRepGraph* Graph() const noexcept;

  // Used by Editor's setters to flag pending notification.
  void MarkDirty() noexcept;
  [[nodiscard]] bool IsDirty() const noexcept;

  // Internal mutable accessor used ONLY by Editor's setter implementations.
  // Not intended for caller-side use; see §3.4 on access control.
  [[nodiscard]] T& Internal() const noexcept;

private:
  BRepGraph* myGraph;
  T*         myEntity;
  TypeId     myId;
  bool       myDirty;   // NEW
};
```

Behavioural change vs. today:

- **Notification is conditional on `myDirty`.** If a caller takes a guard but never invokes any setter, no notification fires. This matches the design intent ("setter explicitly says: I changed something") but **breaks ~15 existing tests** that take a guard and assert `OwnGen` bumped without writing any field. Those tests must be updated to either (a) call a setter or `MarkDirty()` explicitly, or (b) be reframed to no longer assert on side-effects of an unused guard. See §6.4.

- **`operator->` / `operator*` are const-only.** Every `aGuard->Field = X;` ceases to compile. Every site must migrate to a typed setter. Counted ~228 mutation sites across production + tests (see §6).

### 3.2 Editor's typed setter API

For each `Ops` class in `BRepGraph::EditorView`, add typed setters covering every field that production or tests currently write.

Two forms per field:
- **One-shot:** takes `TypedId`. Does the mutation and fires `markModified` immediately. Used when the caller has only one field to set.
- **Batched:** takes `BRepGraph_MutGuard<T>&`. Does the mutation, calls `guard.MarkDirty()`, and lets the guard's destructor fire `markModified` once at scope end. Used when multiple writes coalesce.

Internally both forms call `Storage::ChangeXxx(id)` directly (Editor is friended via existing scaffolding) and write the field. For connectivity-pair atomics (e.g. setting an edge's start vertex ref), the setter calls Storage's atomic that pairs the field write with the reverse-index update — those atomics already exist from Phase 1–5 work.

Field inventory (gathered by grepping `Mut(...)->Field [+-]?=` across `src/ModelingData/TKBRep`):

**Def-side writes (used by current code):**

| Def | Field | Setter signatures |
|---|---|---|
| `VertexDef` | `Tolerance` | `VertexOps::SetTolerance(VertexId, double)` / `(MutGuard<VertexDef>&, double)` |
| `VertexDef` | `Point`     | `VertexOps::SetPoint(VertexId, gp_Pnt)` / `(MutGuard<VertexDef>&, gp_Pnt)` |
| `VertexDef` | `Point.Transform(gp_Trsf)` | `VertexOps::TransformPoint(VertexId, const gp_Trsf&)` / `(MutGuard<VertexDef>&, const gp_Trsf&)` |
| `EdgeDef` | `Tolerance`     | `EdgeOps::SetTolerance` / `EdgeOps::AddTolerance` (for `+=`) |
| `EdgeDef` | `IsClosed`      | `EdgeOps::SetIsClosed` |
| `EdgeDef` | `IsDegenerate`  | `EdgeOps::SetIsDegenerate` |
| `EdgeDef` | `SameParameter` | `EdgeOps::SetSameParameter` |
| `EdgeDef` | `SameRange`     | `EdgeOps::SetSameRange` |
| `EdgeDef` | `ParamFirst` / `ParamLast` | `EdgeOps::SetParamRange(id, first, last)` |
| `WireDef` | `IsClosed`      | `WireOps::SetIsClosed` |
| `FaceDef` | `Tolerance`             | `FaceOps::SetTolerance` |
| `FaceDef` | `NaturalRestriction`    | `FaceOps::SetNaturalRestriction` |
| `FaceDef` | `TriangulationRepId`    | `FaceOps::SetTriangulationRep` (also accepts invalid id to clear) |
| `FaceDef` | `SurfaceRepId`          | `FaceOps::SetSurfaceRep` |
| `CoEdgeDef` | `ParamFirst` / `ParamLast` | `CoEdgeOps::SetParamRange` |
| `CoEdgeDef` | `UV`                | `CoEdgeOps::SetUV` |
| `CoEdgeDef` | `Continuity`        | `CoEdgeOps::SetContinuity` |
| `CoEdgeDef` | `SeamPairId` / `SeamContinuity` | `CoEdgeOps::SetSeamPair(id, mateId, GeomAbs_Shape)` / `CoEdgeOps::ClearSeamPair(id)` |
| `CoEdgeDef` | `PolygonOnTriRepId` | `CoEdgeOps::SetPolygonOnTriRep` |

**Ref-side writes:**

| Ref | Field | Setter signatures |
|---|---|---|
| `VertexRef`, `WireRef`, `FaceRef`, `ChildRef`, `CoEdgeRef`, `ShellRef`, `SolidRef` | `Orientation` | `<Ops>::SetRefOrientation(RefId, TopAbs_Orientation)` |
| same set + `OccurrenceRef` | `LocalLocation` | `<Ops>::SetRefLocalLocation(RefId, TopLoc_Location)` |
| `WireRef` | `IsOuter` | `WireOps::SetRefIsOuter(WireRefId, bool)` |
| `OccurrenceRef` | `ParentId` | `OccurrenceOps::SetRefParent(OccurrenceRefId, NodeId)` (used by Compact/Copy occurrence rebuild) |

**Connectivity-rewrite setters (special-cased, paired with reverse-index updates):**

| Ref | Field | Setter |
|---|---|---|
| `VertexRef` | `VertexDefId` | `VertexOps::RebindRef(VertexRefId, VertexId newDef)` — already covered by `EdgeOps::ReplaceVertex`; verify Compact/Copy go through the existing API |
| `FaceRef` | `FaceDefId` | similar; goes via existing `Editor::ReplaceFace` if it exists, otherwise via Storage atomic + Editor wrapper |
| `ChildRef` | `ChildDefId` | similar |

The connectivity-rewrite setters MUST call the matching `Storage` atomic that pairs the new-def bind with the old-def unbind. Phase 1–5 added these atomics; this refactor wires Editor methods to them.

**Rep-side writes:**

| Rep | Field | Setter |
|---|---|---|
| `Curve3DRep`, `Curve2DRep` | `Curve` (handle) | `RepOps::SetCurve3D(Curve3DRepId, handle<Geom_Curve>)`, `SetCurve2D(...)` |
| `Polygon3DRep`, `Polygon2DRep` | `Polygon` (handle) | `RepOps::SetPolygon3D(...)`, `SetPolygon2D(...)` |
| `SurfaceRep` | (whatever fields tests/prod touch) | enumerate per actual usage |

Total surface: roughly **~30–35 setters × 2 forms ≈ 60–70 method declarations**. Mechanical to write but voluminous.

### 3.3 Soft-delete (`IsRemoved`)

`->IsRemoved = true/false` writes appear in ~11 sites. These are part of soft-delete logic, already wrapped by `Editor::Gen().RemoveNode()`, `RemoveSubgraph()`, `RemoveRef()`. Audit: any direct `aGuard->IsRemoved = X` outside those wrappers is suspect — should be migrated to the existing wrapper, not to a new `SetIsRemoved` setter (deleting an entity has cascade implications that a raw setter shouldn't expose).

### 3.4 Why `Internal()` exists and why it's safe

The Editor's setter implementations are not friends of `BRepGraph_MutGuard` (the friend list would be unwieldy). Instead `BRepGraph_MutGuard` exposes a public `Internal()` method that returns `T&`. Externally, **don't use it** — it's documented as "for Editor setter implementations only". It's an API smell, not an enforced barrier.

If the user wants to enforce this further, the alternative is:
- Delete `Internal()`.
- Make every setter friend of `BRepGraph_MutGuard`. Painful — 60+ friend declarations.
- Or: setters don't take `MutGuard&` at all, only `TypedId`. They look up `Storage::ChangeXxx(id)` themselves. The batched form is then implemented as: setter takes `TypedId`, but if the caller wants batching they call `editor.Edges().Mut(id)` first to construct a guard whose destructor will be the only `markModified` (because the setter doesn't call `markModified` directly — it just writes the field via `Storage::ChangeXxx(id)` which already does cache invalidation but not subtree-gen propagation).

This second option is cleaner: only ONE form of setter, and `Mut(id)` is purely a "I'm about to do multiple setter calls, please defer the markModified" optimization. **Recommend this option** — it eliminates the dual-form clutter.

```cpp
// Recommended pattern (single setter form, Mut purely batches):
{
  BRepGraph_MutGuard<EdgeDef> e = editor.Edges().Mut(edgeId);
  editor.Edges().SetTolerance     (edgeId, 0.5);
  editor.Edges().SetSameParameter (edgeId, true);
}  // ~e fires markModified once
```

The setter looks up by id every call (extra lookup) but it's an O(1) array access and avoids the `Internal()` smell. The Mut guard's job is to suppress the per-setter `markModified` and emit one at scope end.

To implement that, setters need to know "is there an active Mut on this id?" — implement via a small thread-local or per-graph "deferred ids" set. Or simpler: the `Mut(id)` factory installs the id into a per-graph set; setters check membership and skip the immediate notify if present; the destructor fires the deferred notify and removes the id from the set.

A simpler still implementation: setters always call `Storage::ChangeXxx(id)` (which does NOT mark modified), and ALWAYS skip immediate notify; instead the **caller** is responsible for either having a `Mut(id)` in scope (which fires notify on destruction) or calling `editor.Edges().NotifyModified(id)` manually. That's worse for the caller.

**Pick the deferred-set approach** — easy to implement, zero burden on callers.

```cpp
class BRepGraph::EditorView
{
  // ... existing ...
  // Per-graph set of ids whose markModified is deferred.
  // Populated by Mut() factories, drained on ~MutGuard.
  // Setters consult this set to decide whether to notify immediately.
  NCollection_Map<BRepGraph_NodeId, BRepGraph_NodeIdHasher> myDeferredNodeIds;
  NCollection_Map<BRepGraph_RefId,  BRepGraph_RefIdHasher>  myDeferredRefIds;
  NCollection_Map<BRepGraph_RepId,  BRepGraph_RepIdHasher>  myDeferredRepIds;
};
```

This re-uses existing `BRepGraph_DeferredScope` machinery if applicable. Audit `BRepGraph_DeferredScope` first.

### 3.5 Storage: leave as-is

**Don't change `BRepGraphInc_Storage` privacy.** `BRepGraphInc_Storage::ChangeFace`,
`ChangeEdge`, etc. stay public on the class. The user's stance: Storage is not part
of the externally exported API surface — it lives in the `BRepGraphInc/` directory
under `TKBRep` and is not used outside the toolkit. Adding a friend list and
making `ChangeXxx` private adds boilerplate without buying real protection.

The lockdown is enforced at the **Editor + MutGuard** layer:
- `MutGuard::operator->` returns `const T*` only.
- External callers cannot reach `Storage` directly through the public BRepGraph API
  (it's not exported as a stable public surface — only `BRepGraph::Topo()`,
  `Refs()`, `Reps()`, `Editor()` views are).
- Editor's setter implementations call `Storage::ChangeXxx` directly. No friend
  scaffolding needed.

The only invariant that matters is: external code cannot mutate fields. That's
already satisfied by const-only `operator->` on `MutGuard` plus the absence of
any other public mutable accessor.

---

## 4. File-by-file change list

### Headers (declarations)

**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_MutGuard.hxx`** — **already done in working tree.**
The file at hand-off has:
- const-only `operator->` and `operator*` (read-only external field access)
- `myDirty` flag, `MarkDirty()` / `IsDirty()` methods
- `Internal()` accessor returning `T&` for Editor setter implementations
- `~MutGuard` notifies only when `myDirty` is true
- Updated Doxygen comments referencing the typed-setter API

No further changes to this file unless §3.4 settles on dropping `Internal()`.

**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_EditorView.hxx`**
- Add ~30 setter declarations across `VertexOps`, `EdgeOps`, `WireOps`, `FaceOps`, `CoEdgeOps`, `ShellOps`, `SolidOps`, `CompoundOps`, `CompSolidOps`, `ProductOps`, `OccurrenceOps`, `RepOps`, `GenOps`. Mostly `void Set<Field>(<TypedId|MutGuard&>, <ValueType>);`.
- Add the deferred-id sets (or alternative batching impl) — see §3.4.

**`src/ModelingData/TKBRep/BRepGraphInc/BRepGraphInc_Storage.hxx`** — **no changes.**
ChangeXxx accessors stay public on Storage. Storage is internal to TKBRep and
not part of the externally exported API; there is no value in privatising the
mutable accessors. See §3.5.

### Implementations

**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_EditorView.cxx`** *(or split into `BRepGraph_EditorView_Setters.cxx` to keep file size manageable; the existing `BRepGraph_EditorView_Mut.cxx` already contains the `Mut*()` factories)*
- Implement every declared setter. Pattern:
  ```cpp
  void BRepGraph::EditorView::EdgeOps::SetTolerance(BRepGraph_EdgeId theId, double theVal)
  {
    BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    aStorage.ChangeEdge(theId).Tolerance = theVal;
    if (!myGraph->editor().isDeferred(BRepGraph_NodeId(theId)))
      myGraph->markModified(theId, aStorage.Edge(theId));
  }

  void BRepGraph::EditorView::EdgeOps::SetTolerance(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut, double theVal)
  {
    BRepGraphInc_Storage& aStorage = theMut.Graph()->myData->myIncStorage;
    aStorage.ChangeEdge(theMut.Id()).Tolerance = theVal;
    theMut.MarkDirty();
  }
  ```
  (Both forms identical except for who fires `markModified`.)

**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_EditorView_Mut.cxx`**
- Update `Mut*()` factories to register the id into the deferred set (or whatever batching mechanism is chosen).

**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_Compact.cxx`** — migrate ~30 callsites
**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_Copy.cxx`** — migrate ~15 callsites
**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_Transform.cxx`** — migrate ~3 callsites
**`src/ModelingData/TKBRep/BRepGraph/BRepGraph_Deduplicate.cxx`** — migrate ~10 callsites

Compact and Copy do extensive bulk-fill (5–10 fields per cloned Def). For these specifically, they are friends of `BRepGraphInc_Storage`, so they can write `aStorage.ChangeFace(id).Tolerance = X;` directly — bypass the typed setter. This is the explicit design escape hatch for system-internal rebuilders.

### Tests — ~30 files

`src/ModelingData/TKBRep/GTests/BRepGraph_*_Test.cxx` — every file in the list below has at least one `aGuard->Field = X` site:

```
BRepGraph_Assembly_Test.cxx          BRepGraph_Build_Test.cxx
BRepGraph_Builder_Test.cxx           BRepGraph_ChildExplorer_Test.cxx
BRepGraph_Compact_Test.cxx           BRepGraph_Copy_Test.cxx
BRepGraph_DefsIterator_Test.cxx      BRepGraph_DeferredInvalidation_Test.cxx
BRepGraph_EventBus_Test.cxx          BRepGraph_Fuzz_Test.cxx
BRepGraph_History_Test.cxx           BRepGraph_MeshCache_Test.cxx
BRepGraph_MutGuard_Test.cxx          BRepGraph_MutationGen_Test.cxx
BRepGraph_Reconstruct_Test.cxx       BRepGraph_RefId_Test.cxx
BRepGraph_RefsIterator_Test.cxx      BRepGraph_ScenarioMatrix_Test.cxx
BRepGraph_Test.cxx                   BRepGraph_Validate_Test.cxx
BRepGraph_VersionStamp_Test.cxx      BRepGraph_Views_Test.cxx
```

Mechanical migration: every `aGuard->Field = X;` becomes `editor.X().SetField(<aGuard or id>, X);`.

Special test failures expected from the conditional-notify behaviour change in §3.1: tests that take a guard solely to assert `OwnGen` bumped without writing any field. Find them via:
```bash
grep -rEn "Mut(Ref)?\(.*\)\s*;\s*$" src/ModelingData/TKBRep/GTests
```
and:
```bash
grep -B1 -A10 "Mut(Ref)?\(" src/ModelingData/TKBRep/GTests/BRepGraph_MutGuard_Test.cxx \
                              src/ModelingData/TKBRep/GTests/BRepGraph_Views_Test.cxx \
                              src/ModelingData/TKBRep/GTests/BRepGraph_DeferredInvalidation_Test.cxx \
                              src/ModelingData/TKBRep/GTests/BRepGraph_MutationGen_Test.cxx \
                              src/ModelingData/TKBRep/GTests/BRepGraph_MeshCache_Test.cxx
```

For each such test: either call a real setter (preferred — exercises the real path), or call `aGuard.MarkDirty();` (preserves test intent if the test is genuinely about "guard always notifies").

The 15 tests that broke during the dirty-flag prototype run:
```
BRepGraph_DeferredInvalidationTest.DeferredMode_DirectFaceMutation_PropagatesUp
BRepGraph_DeferredInvalidationTest.DeferredMode_DirectShellMutation_PropagatesUp
BRepGraph_DeferredInvalidationTest.DeferredMode_DirectWireMutation_PropagatesUp
BRepGraph_MeshCacheTest.CacheStaleAfterSurfaceRepMutation
BRepGraph_MutGuardTest.MoveAssignmentFlushesThenTransfers
BRepGraph_MutGuardTest.MovedFrom_DoesNotDoubleNotify
BRepGraph_MutationGenTest.RepMutation_SurfacePropagatesSubtreeGenToFace
BRepGraph_MutationGenTest.RepMutation_Curve3DPropagatesSubtreeGenToEdge
BRepGraph_MutationGenTest.RepMutation_Curve2DPropagatesSubtreeGenToCoEdge
BRepGraphTest.OwnGen_MutableEdge_PropagatesSubtreeGenUp
BRepGraph_ViewsTest.MutView_EdgeDef_IncrementsOwnGen
```
These all assert "taking a guard bumps OwnGen / propagates Subtree" without writing a field. Update each to call a real setter (e.g. `editor.Edges().SetTolerance(id, samevalue)` if the test really wants to assert "any edge mutation propagates", or `aGuard.MarkDirty()` if it wants to assert "guard scope alone is enough").

---

## 5. Don't forget — starting state checks

When you start, verify:
```bash
git rev-parse HEAD                        # → fb92c080fa
git status --short
# Expected:
#  M src/ModelingData/TKBRep/BRepGraph/BRepGraph_MutGuard.hxx
# ?? src/ModelingData/TKBRep/BRepGraph/BRepGraph_MutationLockdown_PLAN.md
```

The MutGuard.hxx modification is intentional (see §4 file list). The build at
this state will NOT compile — ~228 mutation sites need migration to typed setters.

To verify the pre-migration baseline, you can temporarily `git stash` the
MutGuard.hxx change and confirm 905 BRepGraph tests + 7007 full-suite tests pass
(with the 3 pre-existing failures noted in memory):
```bash
git stash push src/ModelingData/TKBRep/BRepGraph/BRepGraph_MutGuard.hxx
cd build && cmake --build . --config RelWithDebInfo --target OpenCascadeGTest -- -j8
./mac64/clang/bini/OpenCascadeGTest --gtest_filter="BRepGraph*"   # 905 PASS
git stash pop
# Then proceed with migration.
```
**Important:** only stash for the verification check. Don't `git stash` / `git reset`
during the migration without explicit user OK.

**Build hygiene:**
- Use `cmake --build build --config RelWithDebInfo --target TKBRep` for incremental TKBRep builds — fast.
- For full GTest: `cmake --build build --config RelWithDebInfo --target OpenCascadeGTest`.
- Never `clean rebuild`. Never `git stash` / `git reset` without explicit user OK.
- Run full TKBRep tests after each phase.

---

## 6. Recommended execution order

The plan is large. Sequence so that each step is verifiable on its own and exposes design problems early.

### Phase A — pilot one Def kind end-to-end (FaceDef)

1. Update `BRepGraph_MutGuard.hxx` per §3.1 — const operator->, dirty flag, MarkDirty, Internal.
2. Decide batching mechanism (deferred-set vs `Internal()` + setter direct mutation) per §3.4. Recommend deferred-set.
3. Add **just** the FaceDef setters in `EditorView.hxx` + impl: `SetTolerance`, `SetNaturalRestriction`, `SetTriangulationRep`, `SetSurfaceRep`. One-shot form first; deferred-set form second.
4. Migrate FaceDef sites only:
   - In production: Compact (1 site), Copy (1 site), Transform (1 site).
   - In tests: any `Faces().Mut(...)->Field = X;` site.
5. Build TKBRep + GTest.
6. Run BRepGraph tests. Fix anything that broke. Pre-existing 3 failures stay.
7. **Stop here. Verify.** This pilot exposes friend scaffold issues, batching-set issues, and naming collisions before you commit to writing 60+ setters.

### Phase B — expand to remaining Defs

Repeat Phase A for VertexDef, EdgeDef, WireDef, CoEdgeDef, ShellDef, SolidDef, CompoundDef, CompSolidDef, ProductDef, OccurrenceDef. Mechanical.

### Phase C — Refs

Same pattern. SetRefOrientation / SetRefLocalLocation are shared across most Ref kinds; consider a templated helper. SetRefIsOuter (WireRef-only), SetRefParent (OccurrenceRef-only).

### Phase D — Reps

`RepOps::SetCurve3D`, `SetCurve2D`, `SetPolygon3D`, `SetPolygon2D`, plus whatever `SurfaceRep` and `TriangulationRep` writes appear in actual code. Audit by grep first.

### Phase E — Test reframing

Migrate test sites that broke at Phase A. Ensure all 905 BRepGraph tests pass. Run full suite to confirm no full-suite regressions.

---

## 7. Acceptance criteria

1. **Build is green** at `RelWithDebInfo` on TKBRep and the full GTest target.
2. `OpenCascadeGTest --gtest_filter="BRepGraph*"` reports **905 PASSED**.
3. `OpenCascadeGTest` reports **7007 PASSED** (with the 3 pre-existing failures documented in `~/.claude/projects/-Users-dpasukhi-work-OCCT/memory/project_3_preexisting_failures.md`).
4. **No matches for these patterns outside `BRepGraph_Compact.cxx`, `BRepGraph_Copy.cxx`, `BRepGraphInc_Populate.cxx`, and the Editor setter implementation file:**
   - `<MutGuardVar>->FieldName = ` for any field
   - `<MutGuardVar>->ConnVector.Append(`, `.EraseLast(` etc.
5. `BRepGraph_MutGuard<T>::operator->` returns `const T*` only. No `T* operator->()` overload exists.
6. All Doxygen comments on `MutGuard` and `Editor::*Ops` reflect the new contract.

Verification command for #4:
```bash
grep -rEn "->(Tolerance|IsClosed|IsDegenerate|SameParameter|SameRange|NaturalRestriction|TriangulationRepId|SurfaceRepId|ParamFirst|ParamLast|UV|Continuity|SeamPairId|SeamContinuity|PolygonOnTriRepId|Point|LocalLocation|Orientation|IsOuter|VertexDefId|FaceDefId|ChildDefId|WireDefId|EdgeDefId|CoEdgeDefId|Curve|Polygon|ParentId)\s*[+\-]?=" --include="*.cxx" --include="*.hxx" src/ModelingData/TKBRep | \
  grep -vE "BRepGraph_(Compact|Copy|EditorView)|BRepGraphInc_Populate"
# Should return zero results.
```

Storage's `ChangeXxx` accessors stay public — they're internal to TKBRep and
not part of the externally exported API surface (the lockdown is at the Editor
+ MutGuard layer, not at Storage).

---

## 8. Inventory — exact field-write sites today

Generated 2026-04-26 against `fb92c080fa` to give the implementer an exact migration target. Numbers may drift slightly during the migration; re-run grep before starting each phase.

```bash
grep -rEhn "^\s*[a-zA-Z_]+(Ref|Edge|Face|Wire|Vertex|CoEdge|Shell|Solid|Compound|Product|Occurrence|New|Old|Cur|Mut)\w*->[A-Za-z][A-Za-z0-9]*\s*[+\-]?=" \
  --include="*.cxx" src/ModelingData/TKBRep | grep -v "this->" | sed -E 's/.*->([A-Za-z]+).*/\1/' | sort | uniq -c | sort -rn
```

| Count | Field |
|---:|---|
| 19 | Tolerance |
| 17 | LocalLocation |
| 11 | Orientation |
| 11 | IsRemoved (soft-delete; route via Editor::Gen wrappers) |
|  5 | VertexDefId |
|  5 | NaturalRestriction |
|  5 | Curve |
|  4 | UV |
|  4 | TriangulationRepId |
|  4 | Point |
|  4 | IsClosed |
|  4 | FaceDefId |
|  3 | SameRange |
|  3 | SameParameter |
|  3 | IsDegenerate |
|  3 | ChildDefId |
|  2 | Polygon |
|  2 | ParamLast |
|  2 | ParamFirst |
|  2 | IsOuter |
|  2 | Continuity |
|  1 | WireDefId |
|  1 | SurfaceRepId |
|  1 | SeamPairId |
|  1 | SeamContinuity |
|  1 | PolygonOnTriRepId |
|  1 | ParentId |

**Production callsite list** (grep `Mut(Ref)?\(` outside test dirs):
```
BRepGraph_Transform.cxx           — 3 sites  (Vertex/Face/Edge/Occurrence)
BRepGraph_Copy.cxx                — 5 sites  (Edge ×2, Face ×2, plus Mut for notify-only)
BRepGraph_Compact.cxx             — 25+ sites (Edge, Wire, Face, CoEdge, Shell, refs, child refs)
BRepGraph_Deduplicate.cxx         — 12 sites (Face, Edge defs and various Refs)
BRepGraph_EditorView.cxx          — 1 site   (CoEdge re-edit)
```

**Test callsite list:** see §4 file list (~30 test files).

---

## 9. Open design choices to settle before Phase A

| Question | Recommendation |
|---|---|
| Batching mechanism: per-graph deferred-id set vs `Internal()`-on-MutGuard | **Deferred-id set on EditorView.** Cleaner external API; setter takes only `TypedId`; `Mut(id)` is purely a "defer notify" hint. |
| Storage-friend scaffolding | **Skip — Storage stays as-is.** §3.5 explains. |
| Connectivity-vector field privacy on Def/Ref structs | **Don't tackle in this phase.** Reader code reads connectivity vectors freely (`aFace.WireRefIds.Size()`); making them private would require migrating every reader. Lock-down at the MutGuard layer is sufficient — `MutGuard::operator->` returns `const T*`, so `aMutGuard->WireRefIds.Append(...)` doesn't compile externally. That gives compile-time enforcement without reader churn. |
| `Internal()` accessor on MutGuard | **Drop entirely** if Recommendation #1 is taken. |
| `IsRemoved` setter | **Don't add a public setter.** Soft-delete is `Editor::Gen::RemoveNode/RemoveSubgraph/RemoveRef`. Audit existing `IsRemoved = ` writes and route them through the wrappers if not already. |

---

## 10. Hand-off

When picked up: read this file, then read `~/.claude/projects/-Users-dpasukhi-work-OCCT/memory/MEMORY.md` for project-wide conventions (commit style, build commands, comment-minimization rules, etc.) — those rules apply.

Authoritative starting commit: `fb92c080fa`.
Branch: `brep_graph_upd` (push target: `dpasukhi/brep_graph_upd`).
Don't `git stash` / `git reset` working changes without explicit user OK.
Run `./mac64/clang/bini/OpenCascadeGTest --gtest_filter="BRepGraph*"` after each meaningful step.
