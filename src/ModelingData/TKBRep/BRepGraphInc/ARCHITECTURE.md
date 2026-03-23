# BRepGraphInc Architecture

This document describes the backend architecture of BRepGraphInc in implementation terms.

## 1) Backend Position

```mermaid
flowchart LR
  A[Algorithms] --> G[BRepGraph facade]
  G --> D[BRepGraph_Data]
  D --> S[BRepGraphInc_Storage]
  S --> P[BRepGraphInc_Populate]
  S --> R[BRepGraphInc_Reconstruct]
  S --> X[BRepGraphInc_ReverseIndex]
```

BRepGraphInc is not a user-facing API. It is the runtime model that powers BRepGraph.

## 2) Storage Topology

```mermaid
flowchart TB
  subgraph Storage[BRepGraphInc_Storage]
    E[Entity Tables]
    RX[Reverse Index]
    TM[TShape to NodeId]
    OR[Original Shapes]
    UID[UID Vectors]
  end

  E --> RX
  E --> UID
  E --> OR
  TM --> E
```

Entity tables:

- VertexEntity
- EdgeEntity
- WireEntity
- FaceEntity
- ShellEntity
- SolidEntity
- CompoundEntity
- CompSolidEntity

## 3) Incidence Semantics

```mermaid
flowchart LR
  F[FaceEntity] -->|WireRef| W[WireEntity]
  W -->|EdgeRef| E[EdgeEntity]
  E -->|Start/End idx| V[VertexEntity]
  SH[ShellEntity] -->|FaceRef| F
  SO[SolidEntity] -->|ShellRef| SH
  CO[CompoundEntity] -->|ChildRef| SO
  CS[CompSolidEntity] -->|SolidRef| SO
```

Guideline:

- intrinsic data lives on entities,
- occurrence context (orientation/location) lives on refs.

## 4) Build Flow

```mermaid
sequenceDiagram
  participant T as TopoDS Input
  participant P as Populate
  participant S as Storage
  participant X as ReverseIndex

  T->>P: Perform(shape, parallel)
  P->>P: Traverse hierarchy
  P->>P: Parallel face extraction
  P->>S: Register entities and refs
  P->>P: Post-passes
  P->>X: Build from storage
  X->>S: Reverse maps ready
```

## 5) Reconstruction Flow

```mermaid
sequenceDiagram
  participant C as Caller
  participant R as Reconstruct
  participant S as Storage
  participant K as Cache

  C->>R: Node(storage, id, cache)
  R->>K: Lookup id
  alt hit
    K-->>R: cached shape
  else miss
    R->>S: Read entity and refs
    R->>R: Build TopoDS shape
    R->>K: Insert shape
  end
  R-->>C: shape
```

## 6) Reverse Index Contract

Reverse index maps support upward adjacency queries:

- edge -> wires
- edge -> faces
- vertex -> edges
- wire -> faces
- face -> shells
- shell -> solids

Contract:

- any forward relation used by query code must have matching reverse rows.

## 7) Mutation Contract

After each mutator boundary, the following must hold:

1. entity state is internally valid,
2. reverse index matches current refs,
3. cache invalidation is applied for impacted nodes,
4. history coherence is preserved.

Recommended operation order:

1. edit entity/ref rows,
2. update reverse index incrementally,
3. invalidate caches,
4. append history record.

## 8) Known Performance Priorities

Primary:

- reverse-index dedup strategy in build/maintenance paths,
- append-mode UID maintenance,
- populate post-pass costs.

Secondary in common workloads:

- edge-face context cardinality scans (often low row count).

## 9) Validation Targets

Debug-only validators should check:

- entity id/kind consistency,
- mapping consistency for TShape to NodeId,
- reverse-index coherence with current refs,
- removed-node filtering expectations.
