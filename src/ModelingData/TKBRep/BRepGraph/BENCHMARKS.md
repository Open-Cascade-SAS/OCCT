# BRepGraph Performance Baselines

Hardware: Apple M4, 10 cores, 16 GB RAM
Config: RelWithDebInfo

## Core (BRepGraph_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Deferred (s) | O(1) FaceCount (s) | Review Fix (s) | KDTree Dedup (s) | Cache Inval (s) | DeferredScope (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|-------------|----------------|--------------|---------------------|----------------|------------------|-----------------|------------|-------------------|
| Build 100 faces | seq | 0.000508 | 0.000691 | 0.000557 | 0.000529 | 0.000544 | 0.000506 | 0.000582 | 0.000480 | 0.000543 | 0.000535 | 0.000491 | 0.000540 | 0.000520 | 0.000534 | +5% |
| Build 1000 faces | seq | 0.006630 | 0.009284 | 0.007372 | 0.006567 | 0.006915 | 0.006203 | 0.006616 | 0.005832 | 0.006564 | 0.006982 | 0.006224 | 0.006800 | 0.007934 | 0.006659 | 0% |
| Build 1000 faces | parallel | 0.006626 | 0.009353 | 0.007292 | 0.006764 | 0.006408 | 0.006557 | 0.007961 | 0.005704 | 0.006195 | 0.006340 | 0.005971 | 0.006444 | 0.009214 | 0.006076 | -8% |
| Build 10000 faces | seq | 0.091667 | 0.136744 | 0.098299 | 0.093896 | 0.095669 | 0.089093 | 0.087900 | 0.081625 | 0.091012 | 0.093535 | 0.091299 | 0.089184 | 0.090464 | 0.097125 | +6% |
| Build 10000 faces | parallel | 0.086331 | 0.130992 | 0.093386 | 0.091926 | 0.088084 | 0.078611 | 0.076609 | 0.072994 | 0.078123 | 0.087002 | 0.077040 | 0.079027 | 0.078143 | 0.082372 | -5% |
| Reconstruct 10000 faces | - | 0.017034 | 0.018292 | 0.017795 | 0.016580 | 0.015884 | 0.015663 | 0.016304 | 0.015995 | 0.016870 | 0.017983 | 0.016769 | 0.017343 | 0.016965 | 0.017287 | +1% |
| SpatialQuery 10000 faces | - | 0.011979 | 0.011988 | 0.012316 | 0.012737 | 0.000740 | 0.000434 | 0.000428 | 0.000460 | 0.000485 | 0.000545 | 0.000330 | 0.000371 | 0.000340 | 0.000378 | **-97%** |

## Algorithms (BRepGraphAlgo_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Deferred (s) | O(1) FaceCount (s) | Review Fix (s) | KDTree Dedup (s) | Cache Inval (s) | DeferredScope (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|-------------|----------------|--------------|---------------------|----------------|------------------|-----------------|------------|-------------------|
| Sewing 500 faces | parallel | 0.090420 | 0.089111 | 0.089873 | 0.087665 | 0.093905 | 0.089684 | 0.042964 | 0.037183 | 0.040604 | 0.039473 | 0.036571 | 0.040277 | 0.038696 | 0.038320 | **-58%** |
| Sewing 500 faces | seq | 0.168491 | 0.164901 | 0.168284 | 0.162548 | 0.168708 | 0.167740 | 0.124732 | 0.093939 | 0.097588 | 0.095534 | 0.093600 | 0.094757 | 0.095963 | 0.094683 | **-44%** |
| Deduplicate+Compact 500 copies | - | 0.007665 | 0.012223 | 0.008819 | 0.010113 | 0.005390 | 0.005019 | 0.006232 | 0.004097 | 0.005701 | 0.004766 | 0.004525 | 0.004539 | 0.004410 | 0.004356 | **-43%** |

## Sewing Profiling (BRepGraphAlgo_SewingTest)

| Benchmark | Mode | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Deferred (s) | O(1) FaceCount (s) | Review Fix (s) | KDTree Dedup (s) | Cache Inval (s) | DeferredScope (s) | Delta vs Alloc Prop |
|-----------|------|--------------------|-------------|----------------|--------------|---------------------|----------------|------------------|-----------------|------------|---------------------|
| 2500 faces (50x50) | seq | — | 0.048373 | 0.045253 | 0.049053 | 0.051594 | 0.045293 | 0.046022 | 0.045272 | 0.045011 | 0% |
| 2500 faces (50x50) | parallel | — | 0.052911 | 0.049522 | 0.037282 | 0.036822 | 0.032046 | 0.035192 | 0.032717 | 0.033176 | **-33%** |
| 2500 faces (50x50) | no history | — | 0.045610 | 0.045583 | 0.048532 | 0.049729 | 0.044238 | 0.046142 | 0.045087 | 0.044502 | -2% |
| 1200 faces (200 boxes) | parallel | — | 0.022005 | 0.020513 | 0.016265 | 0.016749 | 0.014637 | 0.015204 | 0.015524 | 0.014788 | **-28%** |

## Notes

- Baseline: 2026-03-17, UIDs opt-in (disabled by default)
- After T1.2: 2026-03-18, UIDs always-on via two DataMap::Bind() per node in allocateUID()
- Array UIDs: 2026-03-18, per-kind NCollection_Vector forward + lazy DataMap reverse
- Grouped: 2026-03-18, 33 fields -> 11 TopoKindData/GeomKindData template structs
- Dense RevIdx: 2026-03-19, ReverseIndex DataMap->Vector, direct TopoView edge->faces lookup
- Build overhead reduced: T1.2 was +36-52%, Array UIDs +7-11%, Grouped -1% to +6%
- Reconstruct improved: -7% (dense ReverseIndex + better cache locality)
- SpatialQuery 17x faster: direct edge->faces lookup eliminates 2-hop wire traversal + PackedMap
- Static Sewing: 2026-03-19, BRepGraphAlgo_Sewing redesigned to static API, graph-only mergeMatchedEdges, BRepGraphAlgo_SameParameter replaces BRepLib::SameParameter
- Perf Opt: 2026-03-19, 5 sewing performance optimizations:
  - Opt 1: Local adjacency replaces persistent RelEdge storage (eliminates ~18% RelEdge alloc/destroy)
  - Opt 2: Precomputed faceIdx per free edge (O(1) same-face check vs O(P*Q) PCurve iteration)
  - Opt 3: Inline PCurve-based FaceCountForEdge (avoids PackedMap in RelEdgesView)
  - Opt 4: SameParameter analytic fast paths for line/circle on plane/cylinder
  - Opt 5: Batch History.RecordBatch (single HistoryRecord vs N per-merge records)
- Sewing 500 faces: **-52% parallel, -26% sequential** (combined effect of all 5 optimizations)
- Compact improved: -35% (dense vector iteration faster than DataMap iteration in rebuild)
- Forward lookup (UIDs().Of): O(1) direct array index, no lazy init, no hashing
- Reverse lookup (UIDs().NodeIdFrom/Has): lazy DataMap, built on first access
- Alloc Prop: 2026-03-19, IncAllocator propagation + KDTree callback API:
  - IncAllocator propagated to all BRepGraph_Data DataMaps, BRepGraph_History containers + inner vectors
  - ReverseIndex inner vectors constructed with IncAllocator (Build + BuildDelta)
  - KDTree ForEachInRange zero-allocation callback API replaces RangeSearch in sewing
  - Sewing adjacency lists use cross-phase IncAllocator
  - Sewing history accumulation vectors use scoped temp allocator
- Sewing 500 faces: **-59% parallel, -44% sequential** vs baseline (cumulative with Perf Opt)
- Build 10000 faces: **-11% seq, -15% parallel** vs baseline (ReverseIndex + IncAllocator destruction)
- Deferred: 2026-03-19, batch cache invalidation for parallel mutation loops:
  - BeginDeferredInvalidation/EndDeferredInvalidation API on BRepGraph
  - markModified() in deferred mode only sets IsModified flag — no mutex, no upward propagation
  - EndDeferredInvalidation() bulk-clears shape cache (single lock) and propagates IsModified upward in one pass
  - Wrapped SameParameter::Perform and Sewing::processEdges parallel loops
- Core build/reconstruct benchmarks unchanged (deferred mode only affects mutation, not build/read paths; variation is run-to-run noise)
- Sewing 2500 faces parallel: **-25%** vs Alloc Prop (mutex contention eliminated in SameParameter + processEdges)
- Sewing 1200 faces parallel: **-21%** vs Alloc Prop
- Sequential sewing shows noise (+6-8%) — expected since single-threaded has no mutex contention to eliminate
- O(1) FaceCount: 2026-03-20, fix sewing mutation model for correct O(1) NbFacesOfEdge:
  - Sewing uses `Builder().RemoveNode()` for replaced edges (proper active count maintenance)
  - `ReplaceEdgeInWire` binds new edge + unbinds old edge from faces in single loop
  - `FaceCountForEdge` simplified to direct O(1) delegation (removes PackedMap allocation per call)
  - `FreeEdges` and multiple-edge detection switched to `Topo().Edges().NbFaces()` with `IsRemoved` filter
  - `UnbindEdgeFromFace` added to ReverseIndex (mirrors UnbindVertexFromEdge pattern)
  - `CommitMutation` guardrails: validates reverse index + active counts at end of Sewing, Compact, Deduplicate
  - CommitMutation assertions are debug-only (`Standard_ASSERT_VOID` is no-op in non-debug) — zero overhead
- Benchmarks unchanged vs Deferred — correctness-focused change, not a performance optimization
- Parallel sewing profiling stable: -26% / -18% vs Alloc Prop (carried from Deferred)
- Review Fix: 2026-03-20, code review bugfixes and optimizations:
  - SplitEdge wire ordering: InsertAfter() maintains traversal order (was Append to end)
  - FindOriginal cycle protection: iteration limit prevents infinite loops
  - FindDerived: delegates to iterative BFS (was unbounded recursion)
  - EndDeferredInvalidation: skips removed edges in propagation
  - ReverseIndex inner vector capacity reduced 256 → 16 (less memory bloat)
  - NCollection_DynamicArray: added InsertAfter/InsertBefore methods
- Build performance improved slightly from reduced ReverseIndex allocation overhead
- SpatialQuery improved to -97% vs baseline (reduced inner vector memory pressure)
- Sewing 2500 faces parallel: **-35% vs Alloc Prop** (improved from -26%), best so far
- Sewing 1200 faces parallel: **-29% vs Alloc Prop** (improved from -18%)
- KDTree Dedup: 2026-03-20, replaced spatial hash with KDTree for vertex deduplication:
  - Spatial hash (hand-rolled int64_t key with signed overflow UB) replaced by NCollection_KDTree<gp_Pnt, 3>
  - KDTree ForEachInRange replaces O(bucket^2) pairwise comparisons with O(n log n) range search
  - Vertex data stored as std::pair<gp_Pnt, int> with temp allocator
  - Removed cstdint dependency (no more int64_t spatial keys)
  - BuildDelta inner vector capacity aligned to 16 (was inconsistent 256 vs 16)
  - Deduplicate+Compact throughput stable (~0.0045s, within run-to-run noise)
  - Core benchmarks within noise — KDTree change only affects Deduplicate path
- Cache Inval: 2026-03-20, automatic transient-cache invalidation in markModified():
  - markModified() now calls InvalidateAll() on node cache alongside shape-cache clearing
  - EndDeferredInvalidation() sweeps all modified entities and invalidates their caches
  - Removed redundant manual Cache().Invalidate() call in Sewing::processEdges
  - UVBounds/BndLib caches now auto-invalidate when topology is mutated via Mut() API
  - Core benchmarks within noise — InvalidateAll() is no-op on empty caches (checks IsEmpty() first)
  - Build 1000 faces spike (+20-39%) is run-to-run noise: 10000 faces shows -1% seq, -9% parallel
  - Sewing/profiling benchmarks stable: no regression from centralized invalidation
- MutGuard: 2026-03-20, unified RAII mutation API + markModified optimization:
  - `Builder().Mut*()` now returns `BRepGraph_MutGuard<>` (deferred markModified) instead of raw reference (eager)
  - Builder-side mutable definition accessors were inlined to the final RAII mutation entry points
  - markModified(NodeId, BaseDef&) overload skips ChangeTopoDef() + mutableCache() redundant dispatch
  - All mutable definition call sites migrated: `.Field` → `->Field`, captured refs use MutGuard type
  - SameParameter::enforceImpl() wraps all 6 edge mutations in single MutGuard scope (1 markModified vs 6)
  - Core benchmarks within noise — mutation path optimization, not build/read path
  - Sewing profiling stable: deferred markModified is equivalent to eager for single-field mutations

## How to Run

```bash
# Build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_GTEST=ON
cmake --build . --config RelWithDebInfo --parallel

# Binary path depends on platform/build layout:
#   Standard cmake:  ./bin/OpenCascadeGTest
#   macOS Xcode:     ./mac64/clang/bini/OpenCascadeGTest
#   Windows MSVC:    ./win64/vc14/bini/OpenCascadeGTest

# Core benchmarks (Build, Reconstruct, SpatialQuery)
./bin/OpenCascadeGTest --gtest_filter="BRepGraph_Benchmark.*" --gtest_also_run_disabled_tests

# Algorithm benchmarks (Sewing 500 faces, Deduplicate+Compact)
./bin/OpenCascadeGTest --gtest_filter="BRepGraphAlgo_Benchmark.*" --gtest_also_run_disabled_tests

# Sewing profiling (2500 faces grid, 1200 faces boxes — 50 iterations each)
./bin/OpenCascadeGTest --gtest_filter="BRepGraphAlgo_SewingTest.Profiling_*"

# All BRepGraph tests (functional correctness)
./bin/OpenCascadeGTest --gtest_filter="*BRepGraph*"
```
