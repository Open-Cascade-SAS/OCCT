# BRepGraph Architecture TODO

Priorities weighted by profiling data (Grid50x50 sequential, 11.22G total) and architectural impact.

Legend: [Perf] = measurable performance gain, [Arch] = architectural improvement, [Stab] = stability/correctness

---

## Phase 0: Immediate (2 days)

### ~~Batch cache invalidation~~ - DONE (2026-03-19)
- `BeginDeferredInvalidation()` / `EndDeferredInvalidation()` API on BRepGraph
- `markModified()` in deferred mode only sets `IsModified` flag - no mutex, no upward propagation
- `EndDeferredInvalidation()` bulk-clears shape cache + single upward propagation pass
- Applied in: SameParameter::Perform, Sewing::processEdges
- **Result**: Sewing 2500 faces parallel -25%, 1200 faces parallel -21%

### ~~CommitMutation guardrails~~ - DONE (2026-03-20)
- `BRepGraph_Mutator::CommitMutation()` validates reverse index + active entity counts
- Called at end of Sewing::Perform, Compact::Perform, Deduplicate::Perform
- Exposed via `Builder().CommitMutation()` for algorithm use

### ~~NbActiveXXX counts~~ - DONE (2026-03-19)
- `NbActiveEdges()`, `NbActiveFaces()` etc. in `BRepGraphInc_Storage`
- `DecrementActiveCount()` called by `BuilderView::RemoveNode()`
- Validated by `CommitMutation` guardrails

---

## Phase 1: High-ROI (1 week)

### ~~Named Attribute Layers (Infrastructure)~~ - DONE (2026-03-20)
- `BRepGraph_Layer` abstract base class with lifecycle callbacks:
  - `Name()`, `OnNodeRemoved(nodeId, replacement)`, `OnCompact(maps)`, `InvalidateAll()`, `Clear()`
- Registration: `BRepGraph::LayerRegistry().RegisterLayer()`, `LayerRegistry().FindLayer()`, `LayerRegistry().UnregisterLayer()`
- `RemoveNode(NodeId, NodeId replacement)` overload dispatches `OnNodeRemoved` to all layers
- Compact saves/restores layers around graph swap, dispatches `OnCompact` with remap maps
- Sewing and Deduplicate pass replacement NodeId to RemoveNode for data migration

### Named Attribute Layers (DE Layers) [Arch] ★★★★
- Built-in layers for DataExchange (replaces XCAFDoc_*Tool pattern):
  - `ColorLayer` → replaces XCAFDoc_ColorTool: Quantity_ColorRGBA per node (Gen/Surf/Curv types)
  - `MaterialLayer` → replaces XCAFDoc_MaterialTool: name, density, description per node
  - `VisMaterialLayer` → replaces XCAFDoc_VisMaterialTool: PBR/Common material per node
  - `LayerGroupLayer` → replaces XCAFDoc_LayerTool: layer membership + visibility per node
  - `DimTolLayer` → replaces XCAFDoc_DimTolTool: GD&T annotations per node
  - `ValidationLayer` → replaces XCAFDoc_Volume/Area/Centroid: computed properties per node
- Built-in layers for algorithms: `AnalysisLayer` (BndLib, UVBounds, FClass2d caches)
- **Why**: foundation for OCAF-free DataExchange pipeline
- **Depends on**: Named Layers infrastructure (done)
- **Design direction**:
  - persistent metadata stays on top of the GUID-keyed layer system, not in `TransientCache`
  - `UID` / `RefUID` are the persistence anchors; `NodeId` / `RefId` remain runtime addresses
  - occurrence-context fallback (occurrence override -> product) should stay outside core storage and be added later through `PathView` or layer-side helpers

### ~~Incremental reverse-index delta~~ - DONE (2026-03-19)
- `Populate::Append` now uses `BuildDeltaReverseIndex` instead of full `BuildReverseIndex`
- Captures entity counts before append, only processes new entities
- **Result**: O(delta) instead of O(N) for append operations

### ~~FaceCountForEdge fast path~~ - DONE (2026-03-20)
- `FaceCountForEdge` simplified to O(1) delegation to `ReverseIndex::NbFacesOfEdge()`
- `UnbindEdgeFromFace` added to ReverseIndex for edge-to-face maintenance
- `ReplaceEdgeInWire` binds new edge + unbinds old edge from wire's faces in single loop
- Sewing `mergeMatchedEdges` uses `Builder().RemoveNode()` for replaced edges
- `FreeEdges` and multiple-edge detection switched to O(1) `Topo().Edges().NbFaces()` with `IsRemoved` filter

### ~~DeferredScope RAII~~ - DONE (2026-03-20)
- `BRepGraph_DeferredScope` RAII class: `BeginDeferredInvalidation()` on construct, `EndDeferredInvalidation()` + `CommitMutation()` on destruct
- Re-entrant: nested guards are no-ops, only outermost flushes and commits
- Applied in: SameParameter::Perform, Sewing::processEdges

### ~~Seam detection strengthening~~ - DONE (2026-03-20)
- `canSewSameFaceEdges` now evaluates PCurves at midpoints for explicit opposite-side verification
- Midpoint UV separation must exceed 25% of surface period in the closed direction
- Catches same-side false positives where bounding box inner/outer distance was inconclusive
- Ported from `BRepBuilderAPI_Sewing::SameParameterEdge` UV-distance check pattern

### ~~UVBounds/BndLib automatic invalidation~~ - DONE (2026-03-20)
- `markModified()` now calls `InvalidateAll()` on node cache alongside shape-cache clearing
- `EndDeferredInvalidation()` sweeps all modified entities (all 8 kinds) and invalidates caches
- Removed redundant manual `Cache().Invalidate()` in Sewing::processEdges
- UVBounds/BndLib caches auto-invalidate when topology is mutated via `Mut()` API
- **Result**: zero manual invalidation needed; no measurable performance impact

### ~~Incremental modes for Deduplicate/Compact~~ - DEFERRED
- Re-sewing after sewing is a rare workflow; premature optimization
- Compact already exits early when nothing removed
- Revisit if iterative Build→Append→Dedup pipelines become common

### ~~BRepGraph_Tool (Centralized Geometry Access)~~ - DONE (2026-03-22)
- `BRepGraph_Tool` with nested helpers: Vertex, Edge, CoEdge, Face, Wire
- Analogue of `BRep_Tool` for BRepGraph: Pnt, Tolerance, Curve, Surface, PCurveGeometry, Range, EvalD0, etc.
- Replaces raw geometry-field access through backend definitions; public geometry queries are centralized in `BRepGraph_Tool`
- ~50 files migrated (16 production + 32 test + 5 internal)

### ~~Explorer / TopologyPath / PathView~~ - DONE (2026-03-25)
- `BRepGraph_TopologyPath`: root-to-leaf step sequence spanning assembly and topology relations
- `BRepGraph_ChildExplorer`: context-preserving downward walker visiting each occurrence (not just definitions)
- `BRepGraph_ParentExplorer`: path-aware upward walker with accumulated context per ancestor occurrence
- `PathView` via `Paths()`: GlobalLocation, GlobalOrientation, PathsTo, NodeLocations, CommonAncestor, FilterByInclude/Exclude, IsAncestorOf, AllNodesOnPath
- `BRepGraph_PCurveContext`: composite key (Edge, Face, Orientation) for PCurve identification

### O(1) UID reverse lookup [Perf] ★★★
- `UIDsView::NodeIdFrom` / `Has` currently do linear scan over per-kind UID vector
- Add lazy `NCollection_DataMap<uint64_t, int>` per kind (counter → index), built on first access
- Invalidated on Compact (which reassigns indices)
- Deferred until production callers exist (currently test-only)

---

## Phase 2: Modularity (2 weeks)

### ~~Event-Driven Invalidation Bus~~ - DONE (2026-03-20)
- Extended `BRepGraph_Layer` with `SubscribedKinds()` bitmask + `OnNodeModified()` / `OnNodesModified()` callbacks
- Immediate mode: `markModified()` dispatches `OnNodeModified()` to layers matching kind bitmask
- Deferred mode: `EndDeferredInvalidation()` collects modified NodeIds during Stage 3 sweep, dispatches `OnNodesModified()` once
- Zero-cost: `myHasModificationSubscribers` flag skips all dispatch when no layer subscribes
- **Result**: Layers can now react to geometry mutations, not just structural changes (OnNodeRemoved)

### ~~OnCompact Unified Remap Map~~ - DONE (2026-03-20)
- Replaced 6-argument `OnCompact(vertexMap, edgeMap, wireMap, faceMap, shellMap, solidMap)` with single `DataMap<BRepGraph_NodeId, BRepGraph_NodeId>`
- Fixed data-loss bug: Compound/CompSolid remaps were built by Compact but never passed to layers
- Unified map built in `BRepGraphAlgo_Compact` from all 8 per-kind maps, extensible for future kinds
- **Result**: All 8 node kinds correctly remapped for layers; simpler layer implementation

### Core/Extension RelEdge split [Arch] ★★★
- Keep only fundamental relations in main graph
- Move algorithm-specific relations to named `BRepGraph_RelationStore` via `ExtensionView`
- Layers (Phase 1) naturally provide the storage mechanism

### Versioned Layer Snapshots [Arch] ★★★
- Layers can create named snapshots (`"BeforeSewing"`, `"AfterCompact"`)
- Compare state before/after algorithms for debugging and undo
- Memory-efficient: snapshot only dirty entries

### Symmetrical Relation Map [Perf] ★★
- For symmetric relations, store each pair once using canonical (minId, maxId) key
- Halves storage for bidirectional relations

---

## Phase 3: Production Readiness

### ~~Per-node MutationGeneration~~ - DONE (2026-03-20)
- `uint32_t MutationGen` added to `BaseDef`, incremented by `markModified()`
- Enables per-node change detection across serialization boundaries
- Propagated-modified parents do NOT get MutationGen incremented (only directly mutated nodes)
- Survives Compact (carried with BaseDef fields)
- Prerequisite for efficient delta-based Save/Load and parametric history

### Versioned Persistent Schema [Arch] ★★★★
- Full Save/Load with schema versioning
- UID (Kind + Counter) is the persistent identity; MutationGen tracks freshness
- Layer-aware: serialize only needed layers
- Binary or JSON format
- MUST persist:
  - topology / assembly defs and refs
  - reps needed for reconstruction
  - UID / RefUID vectors and next UID counter
  - direct mutation freshness (`OwnGen`)
  - explicitly persistent layer payloads
- MUST NOT persist:
  - `TransientCache`
  - reconstructed shape cache
  - reverse indices / lazy UID lookup maps
  - propagation / deferred-mutation bookkeeping
- OPTIONAL:
  - history log
  - heavy tessellation payloads when regeneration is acceptable
- Reuse existing history + `OnCompact(remap)` as the remap contract; persistence should anchor on UID / RefUID rather than saved NodeId / RefId

### ~~Assembly model extension (Phases 1+2)~~ - DONE (2026-03-20)
- **Full design**: [TODO_Assembly.md](TODO_Assembly.md) - see for complete implementation details
- **Intrinsic architecture**: assembly is core, not a Layer plugin — every graph has a root Product
- `Kind::Product = 10`, `Kind::Occurrence = 11` as first-class node kinds with UIDs, history, compact
- API distributed across current surfaces: TopoView / PathView (queries, RootProducts, IsAssembly, IsPart), BuilderView (AddProduct, AddOccurrence, RemoveNode cascade), `BRepGraph::MutProduct()` / `MutOccurrence()` (RAII guards), PathView (`OccurrenceLocation()`), Iterator (ProductDef, OccurrenceDef)
- `Build(aBox)` auto-creates root Product; algorithms always see a uniform model
- Product→Occurrence→Product DAG; each occurrence carries `TopLoc_Location` + `ParentOccurrenceDefId` for tree-structured placement chains
- Self-reference prevention, removed-product guards, DAG-safe occurrence placement via `ParentOccurrenceDefId` walk
- 30 GTests covering data model, API, mutations, DAG sharing, deep nesting, facade reconstruction, error paths, and cascading removal
- ~~Phase 3 OnCompact signature fix~~ - DONE: `DataMap<BRepGraph_NodeId, BRepGraph_NodeId>` unified remap map

### Assembly model extension (Remaining phases) [Arch] ★★★★
- Phase 4: XDE Population Bridge — `BRepGraphDE_PopulateAssembly` in DataExchange/TKXCAF
- Phase 5: XDE Export — BRepGraph assembly -> XDE document structure
- Phase 6: DE Metadata on Assembly Nodes — colors, materials, layers on assembly nodes
- usage-context attribute resolution helper (deferred until DE layers exist)
- Replaces XCAFDoc_ShapeTool's Shape/Reference/Component model
- XDE bridge lives in DataExchange/TKXCAF (uses TKXCAF internally; TKBRep itself has no TKXCAF dependency)

### Compact remapping in history [Arch] ★★★
- Record old→new index maps as history entries during Compact
- Allows external code holding NodeIds to remap after Compact
- Currently Compact transfers UIDs but doesn't expose the remapping

### Public/Internal Boundary Cleanup [Arch] ★★★
- Keep `BRepGraphInc_*` as backend storage implementation and avoid widening its exposure unnecessarily
- Highest-priority cleanup:
  - remove unneeded public includes of `BRepGraphInc_Definition.hxx` / `Reference.hxx` / `Representation.hxx` where the public contract does not require them
  - document intentional direct exposure in `RefsView`, `BRepGraph_Tool`, and `BRepGraph_WireExplorer` instead of adding wrappers or aliases
  - keep tests aligned with the existing public surface without introducing facade-owned replacement structs
- Defer deeper boundary redesign until there is a concrete external consumer need; no wrapper/value layer is planned in the current direction

### Fingerprinting & Quick Equality [Perf] ★★★
- Topological fingerprints (hash of reverse-index adjacency + geometry hashes)
- O(1) shape comparison, fast deduplication, change detection

### Lock-Free Query Paths [Perf] ★★★
- All const views (`TopoView`, `PathView`, `RefsView`) completely lock-free
- Atomic flags + generational counters
- Only mutation paths use shared_mutex
- **Profile context**: const query paths currently have no contention, but future parallel algorithms may need this

### Diagnostic & Profiling Layer [Arch] ★★
- Built-in optional layer recording operation counts, hot paths, memory usage
- Enabled in debug or with a flag
- Leverages layer infrastructure from Phase 1

---

## Phase 4: Advanced

### Parallel Sewing Framework using SubGraphs [Perf] ★★★★
- Partition disconnected components by grouping faces through `BRepGraph_ParentExplorer`
- Sew each subgraph in parallel with independent IncAllocators
- Merge results into parent graph
- **Profile context**: Sewing is 49% of total time; parallelizing across components would be transformative for large assemblies

### GPU Export for Algorithms [Perf] ★★★
- BndLib and Sewing candidate detection export to GPU buffers
- KDTree range search and BBox overlap are GPU-friendly

### Partitioned Graph (Spatial/Logical Buckets) [Arch] ★★★
- Lightweight partitioning by bounding box or topological component
- Out-of-core loading, parallel processing per partition

### Hybrid In-Memory / Out-of-Core Storage [Arch] ★★
- Optional backing store (memory-mapped file or SQLite)
- Entities stay in incidence tables; cold data is paged
- For very large assemblies

---

## Completed / Rejected

### ~~Allocator Propagation~~ — DONE (2026-03-19)
- IncAllocator propagated to all BRepGraph_Data containers, History, ReverseIndex inner vectors
- KDTree ForEachInRange callback API eliminates DynamicArray overhead
- Sewing adjacency lists use cross-phase IncAllocator
- **Result**: Sewing -44% seq, -59% parallel vs baseline; Build -11% seq, -15% parallel

### ~~SoA Entity Layout~~ — REJECTED
- Profiling shows entity traversal is not the bottleneck
- AoS provides better locality for per-entity operations (mutation, reconstruction)
- Current bottlenecks are geometric computation, not entity layout

### ~~Precomputed FaceCount~~ — DONE (2026-03-19)
- `myEdgeFaceCount` vector built during `ReverseIndex::Build`
- O(1) lookup via `NbFacesOfEdge()`

### ~~History label granularity~~ — DONE (2026-03-19)
- `ExtraInfo` field on HistoryRecord
- `RecordBatch` for efficient bulk recording

### ~~Visitor + Registry pattern~~ — REJECTED
- Over-engineered for current algorithm set
- Named layers (Phase 1) provide sufficient extensibility without the complexity

### ~~Algorithm Traits / Policy system~~ — DEFERRED
- Useful but not urgent — current Options/Result structs per algorithm work well
- Revisit when algorithm count grows beyond current set
