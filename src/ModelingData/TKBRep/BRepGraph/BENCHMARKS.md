# BRepGraph Performance Baselines

Hardware: Apple M4, 10 cores, 16 GB RAM
Config: RelWithDebInfo
Date: 2026-03-17

## Core (BRepGraph_Benchmark)

| Benchmark | Mode | Avg (s) | Iters |
|-----------|------|---------|-------|
| Build 100 faces | seq | 0.000508 | 20 |
| Build 1000 faces | seq | 0.006630 | 20 |
| Build 1000 faces | parallel | 0.006626 | 20 |
| Build 10000 faces | seq | 0.091667 | 20 |
| Build 10000 faces | parallel | 0.086331 | 20 |
| Reconstruct 10000 faces | - | 0.017034 | 20 |
| SpatialQuery 10000 faces | - | 0.011979 | 20 |

## Algorithms (BRepGraphAlgo_Benchmark)

| Benchmark | Mode | Avg (s) | Iters |
|-----------|------|---------|-------|
| Sewing 500 faces | parallel | 0.090420 | 20 |
| Sewing 500 faces | seq | 0.168491 | 20 |
| Deduplicate+Compact 500 copies | - | 0.007665 | 20 |
