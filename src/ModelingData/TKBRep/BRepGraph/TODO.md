# BRepGraph Architecture TODO

Priorities weighted by profiling data (Grid50x50 sequential, 11.22G total) and architectural impact.

Legend: [Perf] = measurable performance gain, [Arch] = architectural improvement, [Stab] = stability/correctness

---

## Phase 0: Immediate (2 days)

### ~~Batch cache invalidation~~ — DONE (2026-03-19)
- `BeginDeferredInvalidation()` / `EndDeferredInvalidation()` API on BRepGraph
- `markModified()` in deferred mode only sets `IsModified` flag — no mutex, no upward propagation
- `EndDeferredInvalidation()` bulk-clears shape cache + single upward propagation pass
- Applied in: SameParameter::Perform, Sewing::processEdges
- **Result**: Sewing 2500 faces parallel -25%, 1200 faces parallel -21%

### ~~CommitMutation guardrails~~ — DONE (2026-03-20)
- `BRepGraph_Mutator::CommitMutation()` validates reverse index + active entity counts
- Called at end of Sewing::Perform, Compact::Perform, Deduplicate::Perform
- Exposed via `MutView::CommitMutation()` for algorithm use

### ~~NbActiveXXX counts~~ — DONE (2026-03-19)
- `NbActiveEdges()`, `NbActiveFaces()` etc. in `BRepGraphInc_Storage`
- `DecrementActiveCount()` called by `BuilderView::RemoveNode()`
- Validated by `CommitMutation` guardrails

---

## Phase 1: High-ROI (1 week)

### Named Attribute Layers [Arch] ★★★★★
- `BRepGraph_Layer` abstract base class with lifecycle callbacks:
  - `Name()`, `InvalidateAll()`, `OnNodeRemoved(nodeId)`, `OnNodeReplaced(oldId, newId)`, `OnCompact(oldToNewMap)`
- Registration: `BRepGraph::RegisterLayer()`, access: `BRepGraph::Layer(name)`
- Layers notified by existing mutation paths (RemoveNode, CommitMutation, Compact swap)
- Built on existing UserAttribute/AttrRegistry infrastructure — layers add grouping, bulk ops, and lifecycle on top
- Read-only layers (analysis) can skip mutex entirely
- **Built-in layers for algorithms**: `AnalysisLayer` (BndLib, UVBounds, FClass2d caches), `SewingLayer` (candidates, merge state)
- **Built-in layers for DataExchange** (replaces XCAFDoc_*Tool pattern — metadata directly on topology nodes):
  - `ColorLayer` → replaces XCAFDoc_ColorTool: Quantity_ColorRGBA per node (Gen/Surf/Curv types)
  - `MaterialLayer` → replaces XCAFDoc_MaterialTool: name, density, description per node
  - `VisMaterialLayer` → replaces XCAFDoc_VisMaterialTool: PBR/Common material per node
  - `NameLayer` → replaces TDataStd_Name: TCollection_ExtendedString per node
  - `LayerGroupLayer` → replaces XCAFDoc_LayerTool: layer membership + visibility per node
  - `DimTolLayer` → replaces XCAFDoc_DimTolTool: GD&T annotations per node
  - `ValidationLayer` → replaces XCAFDoc_Volume/Area/Centroid: computed properties per node
- **Why**: foundation for OCAF-free DataExchange pipeline. Current OCAF approach requires shape-to-label lookup + TDataStd_TreeNode reference chains. BRepGraph stores metadata directly on topology nodes with O(1) access. Automatic migration during Sew/Compact/Deduplicate via layer callbacks eliminates manual attribute transfer.
- **What it enables**: DE providers (STEP, IGES, etc.) reading into BRepGraph + layers instead of OCAF document; third-party attribute plugins; serializable attribute groups

### ~~Incremental reverse-index delta~~ — DONE (2026-03-19)
- `Populate::Append` now uses `BuildDeltaReverseIndex` instead of full `BuildReverseIndex`
- Captures entity counts before append, only processes new entities
- **Result**: O(delta) instead of O(N) for append operations

### ~~FaceCountForEdge fast path~~ — DONE (2026-03-20)
- `FaceCountForEdge` simplified to O(1) delegation to `ReverseIndex::FaceCountOfEdge()`
- `UnbindEdgeFromFace` added to ReverseIndex for edge-to-face maintenance
- `ReplaceEdgeInWire` binds new edge + unbinds old edge from wire's faces in single loop
- Sewing `mergeMatchedEdges` uses `Builder().RemoveNode()` for replaced edges
- `FreeEdges` and multiple-edge detection switched to O(1) `DefsView::FaceCountOfEdge` with `IsRemoved` filter

### ~~MutationGuard RAII~~ — DONE (2026-03-20)
- `BRepGraph_MutationGuard` RAII class: `BeginDeferredInvalidation()` on construct, `EndDeferredInvalidation()` + `CommitMutation()` on destruct
- Re-entrant: nested guards are no-ops, only outermost flushes and commits
- Applied in: SameParameter::Perform, Sewing::processEdges

### Seam detection strengthening [Stab] ★★★
- `canSewSameFaceEdges` uses bounding-box heuristics
- Add explicit opposite-side check using PCurve UV ranges on periodic surfaces

### UVBounds/BndLib automatic invalidation [Stab] ★★★
- Hook invalidation into `CacheView::InvalidateSubgraph` (or layer events once available)
- Currently manual invalidation — algorithms must remember to invalidate caches

### Incremental modes for Deduplicate/Compact [Perf] ★★★
- `AnalyzeOnly`, `DeltaOnly`, `Incremental` flags
- O(changed nodes) instead of O(N)

### O(1) UID reverse lookup [Perf] ★★★
- `UIDsView::NodeIdFrom` / `Has` currently do linear scan over per-kind UID vector
- Add lazy `NCollection_DataMap<uint64_t, int>` per kind (counter → index), built on first access
- Invalidated on Compact (which reassigns indices)

---

## Phase 2: Modularity (2 weeks)

### Event-Driven Invalidation Bus [Arch] ★★★★
- Replace direct `markModified`/`invalidateSubgraphImpl` with lightweight event dispatch
- Layers subscribe to node modification events by kind (Edge, Face, etc.)
- Enables reactive updates and zero-cost disabled layers
- **Connects to**: batch invalidation (events dispatched after batch), attribute layers (layers subscribe)
- **Profile context**: `invalidateSubgraphImpl` traverses full downward hierarchy per mutation — events allow lazy/selective invalidation

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

### Per-node MutationGeneration [Arch] ★★★★
- Add `uint32_t MutationGen` to `BaseEntity`, incremented by `markModified()`
- Enables per-node change detection across serialization boundaries
- Prerequisite for efficient delta-based Save/Load and parametric history

### Versioned Persistent Schema [Arch] ★★★★
- Full Save/Load with schema versioning
- UID (Kind + Counter) is the persistent identity; MutationGen tracks freshness
- Layer-aware: serialize only needed layers
- Binary or JSON format

### Assembly model extension [Arch] ★★★★
- Location-aware compound/component refs for product structure
- Extend CompoundEntity's shell/solid refs with `TopLoc_Location` per ref
- Assembly = Compound where each ref has a non-identity location; Instance = ref to shape def + location
- Replaces XCAFDoc_ShapeTool's Shape/Reference/Component model
- Required for full DE serialization (assemblies with placed instances)
- Start with Option A (extend existing entities), evolve to dedicated AssemblyEntity if needed

### Compact remapping in history [Arch] ★★★
- Record old→new index maps as history entries during Compact
- Allows external code holding NodeIds to remap after Compact
- Currently Compact transfers UIDs but doesn't expose the remapping

### Fingerprinting & Quick Equality [Perf] ★★★
- Topological fingerprints (hash of reverse-index adjacency + geometry hashes)
- O(1) shape comparison, fast deduplication, change detection

### Lock-Free Query Paths [Perf] ★★★
- All const views (DefsView, SpatialView, RelEdgesView) completely lock-free
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
- Decompose into independent subgraphs via `BRepGraph_Analyze::Decompose`
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
- O(1) lookup via `FaceCountOfEdge()`

### ~~History label granularity~~ — DONE (2026-03-19)
- `ExtraInfo` field on HistoryRecord
- `RecordBatch` for efficient bulk recording

### ~~Visitor + Registry pattern~~ — REJECTED
- Over-engineered for current algorithm set
- Named layers (Phase 1) provide sufficient extensibility without the complexity

### ~~Algorithm Traits / Policy system~~ — DEFERRED
- Useful but not urgent — current Options/Result structs per algorithm work well
- Revisit when algorithm count grows beyond current set
