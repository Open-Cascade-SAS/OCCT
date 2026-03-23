# BRepGraph Architecture TODO

## Phase 0: Immediate (2 days)

### CommitMutation guardrails
- Add `Mutator::CommitMutation` calls in Sewing, Compact, Deduplicate
- Ensures mutation safety is explicit, not implicit

### Remove wire-based fallback in FaceCountForEdge
- `RelEdgesView::FaceCountForEdge` creates PackedMap per call (slow)
- Use PCurve-based `BuilderView::FaceCountForEdge` (O(1) reverse index)
- After mutations, update reverse index incrementally instead of traversing wires

### NbActiveXXX counts
- Validate and Compact should expose `NbActiveEdges()`, `NbActiveFaces()` etc.
- Skip removed nodes in count methods

### Seam detection strengthening
- `canSewSameFaceEdges` uses bounding-box heuristics
- Add explicit opposite-side check using PCurve UV ranges on periodic surfaces

---

## Phase 1: High-ROI (1 week)

### Incremental reverse-index delta
- After `Append` operations, update reverse index incrementally instead of full rebuild
- Currently `ReverseIndex::Build` is O(N) — make it O(delta)

### Precomputed FaceCount
- Store face count per edge in the reverse index during build
- Eliminate runtime traversal in `FaceCountForEdge`

### Algorithm Traits / Policy system
- `template<class Policy> class BRepGraphAlgo_Traits`
- Control UID allocation, invalidation depth, history granularity per algorithm
- Domain-specific variants (MeshGraphTraits vs CADGraphTraits)

### Incremental modes for Deduplicate/Compact
- `AnalyzeOnly`, `DeltaOnly`, `Incremental` flags
- O(changed nodes) instead of O(N)

---

## Phase 2: Modularity (2 weeks)

### Core/Extension RelEdge split
- Keep only fundamental kinds (Contains, OuterWire, SameDomain, DerivedFrom) in main graph
- Move algorithm-specific relations to named `BRepGraph_RelationStore` via `ExtensionView`

### Full Graph Layers / Plugins
- `BRepGraph::Layer` interface; each layer owns its relations, attributes, caches
- Main graph broadcasts invalidation events (OnGraphChanged)

### Event-Driven Invalidation Bus
- Replace direct `markModified`/`invalidateSubgraphImpl` with lightweight event bus
- Layers, caches, external plugins subscribe to events
- Enables reactive updates and zero-cost disabled layers

### Symmetrical Relation Map
- For symmetric relations, store each pair once using canonical (minId, maxId) key
- Halves storage for bidirectional relations

### UVBounds/BndLib cache improvements
- Hook invalidation into `CacheView::InvalidateSubgraph`
- Currently manual invalidation

---

## Phase 3: Production Readiness

### Versioned Persistent Schema
- Full Save/Load with schema versioning
- UID + generation give stable identity across sessions
- Binary or JSON format

### Fingerprinting & Quick Equality
- Topological fingerprints (hash of reverse-index adjacency + geometry hashes)
- O(1) shape comparison, fast deduplication, change detection

### Lock-Free Query Paths
- All const views (DefsView, SpatialView, RelEdgesView) completely lock-free
- Atomic flags + generational counters
- Only mutation paths use shared_mutex

### Sewing + SameParameter parallel optimization
- `markModified` in SameParameter::Enforce acquires shared_mutex per call (125M in profile)
- Eliminate shared-state contention in parallel phases
- Per-thread accumulation buffers for all outputs
- Batch cache invalidation after parallel phase instead of per-edge

### Diagnostic & Profiling Layer
- Built-in optional layer recording operation counts, hot paths, memory usage
- Enabled in debug or with a flag

---

## Phase 4: Advanced

### GPU Export for Algorithms
- BndLib and Sewing candidate detection export to GPU buffers
- Via proposed GPU export view

### Partitioned Graph (Spatial/Logical Buckets)
- Lightweight partitioning by bounding box or topological component
- Out-of-core loading, parallel processing per partition

### Hybrid In-Memory / Out-of-Core Storage
- Optional backing store (memory-mapped file or SQLite)
- Entities stay in incidence tables; cold data is paged
- For very large assemblies

### Parallel Sewing Framework using SubGraphs
- Decompose into independent subgraphs
- Sew each subgraph in parallel
- Merge results
