# BRepGraph Assembly Model

## Architecture Summary

Assembly is **intrinsic** to BRepGraph — not a Layer plugin. Every graph has at least
one root Product. `Build(aBox)` auto-creates a single root Product with
`ShapeRootId → Solid(0)`. Algorithms always see a uniform model.

- `Kind::Product = 10`, `Kind::Occurrence = 11` as first-class node kinds
- Product→Occurrence→Product DAG; each Occurrence carries `TopLoc_Location`
- GlobalPlacement via `ParentOccurrenceDefId` chain composition
- API distributed across existing views (TopoView, BuilderView, PathView)

**Visualizations**: [TODO_Assembly_Visualization.md](TODO_Assembly_Visualization.md)

---

## Completed

### Data Model and Core API — DONE (2026-03-20)
- `ProductDef` / `OccurrenceDef` / `OccurrenceUsage` in `BRepGraphInc_Definition.hxx`
- `DefStore<T>` in `BRepGraphInc_Storage` with typed-id accessors
- BRepGraph dispatch (TopoDef, markModified, invalidateSubgraph, allocateUID)
- Auto root Product creation in `BRepGraph_Builder`
- TopoView / PathView: `Product()`, `Occurrence()`, `RootProducts()`, `IsAssembly()`, `IsPart()`, `NbComponents()`, `Component()`
- BuilderView: `AddProduct()`, `AddAssemblyProduct()`, `AddOccurrence()`, RemoveNode/RemoveSubgraph cascade
- PathView: `OccurrenceLocation(occId)` via ParentOccurrenceDefId walk
- MutRef: `MutProduct()`, `MutOccurrence()` RAII guards
- Iterator: `BRepGraph_Iterator<ProductDef>`, `BRepGraph_Iterator<OccurrenceDef>`
- ReverseIndex: `OccurrencesOfProduct()`, `BuildProductOccurrences()`

### Assembly Reconstruction at ShapesView — DONE (2026-03-29)
- `BRepGraph_ShapesView` now reconstructs Product nodes in product-local coordinates
- part Products reuse the topology reconstruction backend and apply `RootOrientation` / `RootLocation`
- assembly Products rebuild a `TopoDS_Compound` from child occurrences without pushing assembly logic into `BRepGraphInc_Reconstruct`
- Occurrence reconstruction now applies the cumulative placement chain via `PathView::OccurrenceLocation()`
- coverage verifies built root-product reconstruction, shared-product assembly compounds, and nested occurrence placement

### OnCompact Signature Unification — DONE (2026-03-20)
- Unified `DataMap<BRepGraph_NodeId, BRepGraph_NodeId>` replaces 6-argument `OnCompact`
- Product/Occurrence compaction with cross-reference fixup
- Fixed Compound/CompSolid remap data-loss bug

### Core Assembly Tests — DONE (2026-03-20)
- 26 GTests in `src/ModelingData/TKBRep/GTests/BRepGraph_Assembly_Test.cxx`
- Covers: auto root product, AddProduct/AddOccurrence, DAG sharing, deep nesting,
  GlobalPlacement, iterators, removal cascade, MutRef guards, compact remap, UIDs,
  reverse index, error paths (invalid parent, self-reference, removed product),
  circular parent termination

---

## Remaining Work

### XDE Population Bridge [Arch] ★★★★

Populate BRepGraph assembly structure from an XDE document. Lives in DataExchange
module (TKBRep has no TKXCAF dependency).

**Location**: `src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_PopulateAssembly.hxx/.cxx`

**Algorithm**:
1. Graph must already have topology populated via `Build()`
2. Replace auto-created root Product with XDE structure
3. Walk `XCAFDoc_ShapeTool` free shapes:
   - Simple shape → `AddProduct(shapeRootNodeId)` via `Shapes().FindNode(shape)`
   - Assembly → `AddAssemblyProduct()`, recurse into components
4. For each component: get location, resolve referenced shape, deduplicate via `TDF_Label → ProductId` map
5. `AddOccurrence(parent, referenced, location)` for each component

**Files**:
| Action | File |
|--------|------|
| Create | `src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_PopulateAssembly.hxx/.cxx` |
| Create | `src/DataExchange/TKXCAF/BRepGraphDE/FILES.cmake` |
| Modify | `src/DataExchange/TKXCAF/PACKAGES.cmake` |

### XDE Export (BRepGraph → XDE) [Arch] ★★★

Walk Product/Occurrence entities and create matching XDE label structure.
Needed for migration interop: BRepGraph-based algorithms exporting results back to XDE.

**Location**: `src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_ReconstructAssembly.hxx/.cxx`

### Assembly-Aware Attribute Resolution [Arch] ★★

The Layer system already works on any NodeId including Product/Occurrence.
What's missing is **inheritance resolution**: when querying a color for an occurrence,
fall back to the referenced product if the occurrence has no override.

**Rethink**: An older draft proposed a separate assembly helper covering
`ResolveAttribute()`, `LeafParts()`, and `OccurrencePath()`. Since then:
- `OccurrencePath` → now covered by `PathView::PathsTo()` and `PathView::OccurrenceLocation()`
- `LeafParts` → achievable via `BRepGraph_ChildExplorer` from a Product root with target `Kind::Face` or `Kind::Solid`

Only `ResolveAttribute` remains genuinely useful. Two options:
1. **Template helper on BRepGraph_Layer**: `ResolveForOccurrence(occId)` that checks occurrence then falls back to product. Simple, no new file.
2. **On PathView** as `ResolveLayerValue()`: consistent with PathView's role as the occurrence-context resolver.

**Decision**: Defer until DE Layers (ColorLayer, MaterialLayer) exist — the concrete
use case will clarify which pattern fits. Don't build the abstraction before the consumer.

### XDE Round-Trip Tests [Test] ★★★

Depends on XDE Population Bridge and XDE Export.

| Test | Description |
|------|-------------|
| `PopulateFromXDE` | Load STEP assembly, populate graph, verify product/occurrence counts |
| `SharedParts` | Assembly with shared parts, verify single ProductDef per unique part |
| `RoundTrip` | Populate → Reconstruct → compare XDE label trees |

**File**: `src/DataExchange/TKXCAF/GTests/BRepGraphDE_Assembly_Test.cxx`

### Benchmarks [Perf] ★★

| Benchmark | Goal |
|-----------|------|
| `PureTopology_SingleProduct` | Verify negligible overhead vs baseline |
| `Assembly_1000Parts` | Measure AddProduct/AddOccurrence throughput |
| `Assembly_DeepNesting_10Levels` | GlobalPlacement composition at depth 10 |

---

## Dependency Graph

```
Reconstruction ────────────────────────┐
                                       v
XDE Population Bridge ──> XDE Export ──> XDE Round-Trip Tests
                                       ^
Assembly Attribute Resolution ─────────┘ (deferred until DE Layers exist)
```

## API Distribution

| Responsibility | Location |
|---------------|----------|
| Const accessors (`Product()`, `RootProducts()`, `IsAssembly()`, `IsPart()`) | `TopoView` / `PathView` |
| Mutations (`AddProduct()`, `AddOccurrence()`) | `BuilderView` |
| RAII guards (`MutProduct()`, `MutOccurrence()`) | `BRepGraph` |
| `OccurrenceLocation()` | `PathView` |
| Occurrence path, location, attribute context | `PathView` |
| Assembly traversal into topology | `BRepGraph_ChildExplorer` |
| Reverse index (product→occurrences) | `BRepGraphInc_ReverseIndex` |
| Removal cascade (product→occurrences) | `BuilderView::RemoveNode()` / `RemoveSubgraph()` |
| Compact remap | `BRepGraphAlgo_Compact` |

## Design Decisions & Rationale

| Decision | Rationale |
|----------|-----------|
| **Intrinsic, not Layer** | Assembly is fundamental to model identity. A Layer can be unregistered, leaving the graph inconsistent. |
| **Auto root Product on Build()** | Algorithms never branch on "is this an assembly graph?" — it always is. |
| **Methods on Views** | Follows existing pattern: const on TopoView / PathView, mutations on BuilderView, spatial context on PathView. |
| **Always via Occurrence** | Product→Occurrence→Product. Every placement is explicit with its own NodeId. |
| **Single ShapeRootId** | One NodeId pointing to root topology. Mixed products use nested products. |
| **PathView covers the earlier helper scope** | `PathsTo()`, `OccurrenceLocation()`, and Explorer cover occurrence paths and leaf-part traversal. Only ResolveAttribute remains, deferred until DE Layers exist. |
| **Reconstruction at ShapesView level** | Placement composition is a facade concern, not a storage concern. BRepGraphInc_Reconstruct stays topology-only. |
| **XDE bridge in DataExchange** | TKBRep has no TKXCAF dependency. Uses BuilderView directly. |
