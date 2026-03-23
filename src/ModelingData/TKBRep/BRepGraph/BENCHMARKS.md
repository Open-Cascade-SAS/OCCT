# BRepGraph Performance Baselines

Hardware: Apple M4, 10 cores, 16 GB RAM
Config: RelWithDebInfo

## Core (BRepGraph_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------------|
| Build 100 faces | seq | 0.000508 | 0.000691 | 0.000557 | +10% |
| Build 1000 faces | seq | 0.006630 | 0.009284 | 0.007372 | +11% |
| Build 1000 faces | parallel | 0.006626 | 0.009353 | 0.007292 | +10% |
| Build 10000 faces | seq | 0.091667 | 0.136744 | 0.098299 | +7% |
| Build 10000 faces | parallel | 0.086331 | 0.130992 | 0.093386 | +8% |
| Reconstruct 10000 faces | - | 0.017034 | 0.018292 | 0.017795 | +4% |
| SpatialQuery 10000 faces | - | 0.011979 | 0.011988 | 0.012316 | +3% |

## Algorithms (BRepGraphAlgo_Benchmark)

| Benchmark | Mode | Baseline (s) | After T1.2 (s) | Array UIDs (s) | Delta vs Baseline |
|-----------|------|-------------|----------------|-----------------|-------------------|
| Sewing 500 faces | parallel | 0.090420 | 0.089111 | 0.089873 | -1% |
| Sewing 500 faces | seq | 0.168491 | 0.164901 | 0.168284 | ~0% |
| Deduplicate+Compact 500 copies | - | 0.007665 | 0.012223 | 0.008819 | +15% |

## Notes

- Baseline: 2026-03-17, UIDs opt-in (disabled by default)
- After T1.2: 2026-03-18, UIDs always-on via two DataMap::Bind() per node in allocateUID()
- Array UIDs: 2026-03-18, per-kind NCollection_Vector forward + lazy DataMap reverse
- Build overhead reduced: T1.2 was +36-52%, now +7-11% (one vector Append + atomic inc per node)
- Compact overhead reduced: +59% -> +15% (forward vectors transferred directly, reverse map lazy)
- Forward lookup (UIDs().Of): O(1) direct array index, no lazy init, no hashing
- Reverse lookup (UIDs().NodeIdFrom/Has): lazy DataMap, built on first access
