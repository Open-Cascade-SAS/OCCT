# BRepGraph Performance Baselines

Hardware: Apple M4, 10 cores, 16 GB RAM
Config: RelWithDebInfo

## Core (BRepGraph_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|-------------|----------------|-------------------|
| Build 100 faces | seq | 0.000508 | 0.000691 | 0.000557 | 0.000529 | 0.000544 | 0.000506 | 0.000582 | 0.000480 | **-6%** |
| Build 1000 faces | seq | 0.006630 | 0.009284 | 0.007372 | 0.006567 | 0.006915 | 0.006203 | 0.006616 | 0.005832 | **-12%** |
| Build 1000 faces | parallel | 0.006626 | 0.009353 | 0.007292 | 0.006764 | 0.006408 | 0.006557 | 0.007961 | 0.005704 | **-14%** |
| Build 10000 faces | seq | 0.091667 | 0.136744 | 0.098299 | 0.093896 | 0.095669 | 0.089093 | 0.087900 | 0.081625 | **-11%** |
| Build 10000 faces | parallel | 0.086331 | 0.130992 | 0.093386 | 0.091926 | 0.088084 | 0.078611 | 0.076609 | 0.072994 | **-15%** |
| Reconstruct 10000 faces | - | 0.017034 | 0.018292 | 0.017795 | 0.016580 | 0.015884 | 0.015663 | 0.016304 | 0.015995 | **-6%** |
| SpatialQuery 10000 faces | - | 0.011979 | 0.011988 | 0.012316 | 0.012737 | 0.000740 | 0.000434 | 0.000428 | 0.000460 | **-96%** |

## Algorithms (BRepGraphAlgo_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|-------------|----------------|-------------------|
| Sewing 500 faces | parallel | 0.090420 | 0.089111 | 0.089873 | 0.087665 | 0.093905 | 0.089684 | 0.042964 | 0.037183 | **-59%** |
| Sewing 500 faces | seq | 0.168491 | 0.164901 | 0.168284 | 0.162548 | 0.168708 | 0.167740 | 0.124732 | 0.093939 | **-44%** |
| Deduplicate+Compact 500 copies | - | 0.007665 | 0.012223 | 0.008819 | 0.010113 | 0.005390 | 0.005019 | 0.006232 | 0.004097 | **-47%** |

## Sewing Profiling (BRepGraphAlgo_SewingTest)

| Benchmark | Mode | Static Sewing (s) | Perf Opt (s) | Alloc Prop (s) | Delta vs Perf Opt |
|-----------|------|--------------------|-------------|----------------|-------------------|
| 2500 faces (50x50) | seq | — | 0.048373 | 0.045253 | **-6%** |
| 2500 faces (50x50) | parallel | — | 0.052911 | 0.049522 | **-6%** |
| 2500 faces (50x50) | no history | — | 0.045610 | 0.045583 | 0% |
| 1200 faces (200 boxes) | parallel | — | 0.022005 | 0.020513 | **-7%** |

## Notes

- Baseline: 2026-03-17, UIDs opt-in (disabled by default)
- After T1.2: 2026-03-18, UIDs always-on via two DataMap::Bind() per node in allocateUID()
- Array UIDs: 2026-03-18, per-kind NCollection_Vector forward + lazy DataMap reverse
- Grouped: 2026-03-18, 33 fields -> 11 TopoKindData/GeomKindData template structs
- Dense RevIdx: 2026-03-19, ReverseIndex DataMap->Vector, SpatialView direct edge->faces
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
