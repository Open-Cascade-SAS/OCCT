# BRepGraph Performance Baselines

Hardware: Apple M4, 10 cores, 16 GB RAM
Config: RelWithDebInfo

## Core (BRepGraph_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|
| Build 100 faces | seq | 0.000508 | 0.000691 | 0.000557 | 0.000529 | 0.000544 | +7% |
| Build 1000 faces | seq | 0.006630 | 0.009284 | 0.007372 | 0.006567 | 0.006915 | +4% |
| Build 1000 faces | parallel | 0.006626 | 0.009353 | 0.007292 | 0.006764 | 0.006408 | -3% |
| Build 10000 faces | seq | 0.091667 | 0.136744 | 0.098299 | 0.093896 | 0.095669 | +4% |
| Build 10000 faces | parallel | 0.086331 | 0.130992 | 0.093386 | 0.091926 | 0.088084 | +2% |
| Reconstruct 10000 faces | - | 0.017034 | 0.018292 | 0.017795 | 0.016580 | 0.015884 | -7% |
| SpatialQuery 10000 faces | - | 0.011979 | 0.011988 | 0.012316 | 0.012737 | 0.000740 | **-94%** |

## Algorithms (BRepGraphAlgo_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Grouped (s) | Dense RevIdx (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------|-------------------|-------------------|
| Sewing 500 faces | parallel | 0.090420 | 0.089111 | 0.089873 | 0.087665 | 0.093905 | +4% |
| Sewing 500 faces | seq | 0.168491 | 0.164901 | 0.168284 | 0.162548 | 0.168708 | 0% |
| Deduplicate+Compact 500 copies | - | 0.007665 | 0.012223 | 0.008819 | 0.010113 | 0.005390 | **-30%** |

## Notes

- Baseline: 2026-03-17, UIDs opt-in (disabled by default)
- After T1.2: 2026-03-18, UIDs always-on via two DataMap::Bind() per node in allocateUID()
- Array UIDs: 2026-03-18, per-kind NCollection_Vector forward + lazy DataMap reverse
- Grouped: 2026-03-18, 33 fields -> 11 TopoKindData/GeomKindData template structs
- Dense RevIdx: 2026-03-19, ReverseIndex DataMap->Vector, SpatialView direct edge->faces
- Build overhead reduced: T1.2 was +36-52%, Array UIDs +7-11%, Grouped -1% to +6%
- Reconstruct improved: -7% (dense ReverseIndex + better cache locality)
- SpatialQuery 17x faster: direct edge->faces lookup eliminates 2-hop wire traversal + PackedMap
- Sewing within noise: FaceCountForEdge retains wire-based path for mutation correctness
- Compact improved: -30% (dense vector iteration faster than DataMap iteration in rebuild)
- Forward lookup (UIDs().Of): O(1) direct array index, no lazy init, no hashing
- Reverse lookup (UIDs().NodeIdFrom/Has): lazy DataMap, built on first access
