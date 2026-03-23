# BRepGraphInc TODO Plan

This plan focuses on clarity, performance, and robustness for the backend.

## Phase A: Clarity and Contracts

1. Add debug validators
- ValidateEntityIds
- ValidateReverseIndex
- ValidateTShapeMapping

2. Add invariant comments at mutation boundaries
- populate
- mutator
- reconstruct

3. Keep docs synchronized
- README.md
- ARCHITECTURE.md

Acceptance:

- validators pass on existing sewing and build tests in debug mode
- docs match runtime architecture

## Phase B: High-ROI Performance

1. Optimize reverse-index dedup strategy
- avoid repeated linear duplicate checks in high-volume build paths

2. Incremental append UID policy
- allocate UIDs only for appended ranges when possible

3. Add phase timers
- traverse
- extract
- register
- post-pass
- reverse-index build
- reconstruct

Acceptance:

- measurable speedup on grid and profiling sewing workloads

## Phase C: Mutation Safety

1. Enforce update order
1) entity edits
2) reverse-index updates
3) cache invalidation
4) history updates

2. Add mutation stress tests
- repeated split on same wire chain
- split plus replace mixes
- rebuild and reconstruct checks after each cycle

Acceptance:

- no invariant violations under stress tests

## Phase D: Consumer Hardening

1. Add workload presets for healing and sewing profiles
2. Add optional policy toggles for expensive populate post-passes
3. Add deterministic checks for parallel and sequential behavior

Acceptance:

- stable behavior and bounded performance variance across workloads

## Priority Order

1. Reverse-index optimization
2. Append UID incrementality
3. Debug validators
4. Timers and targeted profiles
5. Optional post-pass policies
