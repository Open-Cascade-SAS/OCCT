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
- Lightweight `BRepGraph_Layer` base class extending current UserAttribute system (not replacing it)
- Each layer: name, bulk `InvalidateAll()`, attach/detach, optional `OnGraphChanged(nodeId)` callback
- Registration via `BRepGraph::RegisterLayer()`, access via `BRepGraph::Layer(name)`
- Built-in layers: `AnalysisLayer` (BndLib, UVBounds, FClass2d caches), `SewingLayer` (candidates, merge state)
- Connects to batch invalidation: after batch completes, notify registered layers
- Read-only layers (analysis) can skip mutex entirely
- **Why**: current UserAttribute system works but lacks bulk operations, layer isolation, and event-driven invalidation. Algorithms manually invalidate individual attributes — error-prone and hard to extend
- **What it enables**: third-party attribute plugins, automatic invalidation on mutation, serializable attribute groups

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

### MutationGuard RAII [Stab] ★★★
- RAII wrapper: `BeginDeferredInvalidation()` on construct, `EndDeferredInvalidation()` + `CommitMutation()` on destruct
- Guarantees atomicity — no partial mutations visible outside scope
- Debug-mode `ValidateReverseIndex()` in destructor
- Simplifies algorithm code: replace manual begin/end/commit with single guard scope

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
