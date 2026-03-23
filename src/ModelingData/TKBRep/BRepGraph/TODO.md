# BRepGraph Architecture Improvement Roadmap

## Context

BRepGraph is a bidirectional topology-geometry graph layered over OCCT's TopoDS/BRep model. It provides efficient read-only queries, parallel analysis, and utility algorithms (sewing, copy, transform, deduplication, validation, compaction). However, it has architectural gaps that limit its use for mutation-heavy workflows (booleans, fillets, local operations). This roadmap defines improvements to make BRepGraph a robust, future-proof foundation for CAD algorithm development.

**Current state (2026-03-17):** 12 view classes, 11 algorithms (BRepGraphAlgo), BRepGraphCheck validation, optional UID system, append-only history, user attribute system with GUID registry. Cached UV bounds (`BRepGraphAlgo_UVBounds`) and bounding boxes (`BRepGraphAlgo_BndLib`) use per-node user attributes. PCurve continuity populated via `BRep_Tool::MaxContinuity`. 225+ passing tests.

---

## Current Architecture

```mermaid
graph TB
    subgraph "Application Layer"
        A1[BRepGraphAlgo_Sewing]
        A2[BRepGraphAlgo_Copy]
        A3[BRepGraphAlgo_Transform]
        A4[BRepGraphAlgo_Deduplicate]
        A5[BRepGraphAlgo_BndLib]
        A6[BRepGraphAlgo_FClass2d]
        A7[BRepGraphAlgo_Validate]
        A8[BRepGraphAlgo_Compact]
        A9[BRepGraphAlgo_AttrTransfer]
        A10[BRepGraphCheck_Analyzer]
        A11[BRepGraphAlgo_UVBounds]
    end

    subgraph "View API Layer"
        V1[DefsView]
        V2[UsagesView]
        V3[GeomView]
        V4[SpatialView]
        V5[RelEdgesView]
        V6[AnalyzeView]
        V7[ShapesView]
        V8[CacheView]
        V9[AttrsView]
        V10[MutView]
        V11[BuilderView]
        V12[Iterator]
    end

    subgraph "Core Data Layer"
        D1[BRepGraph_Data - PIMPL]
        D2[Def Vectors - Solid/Shell/Face/Wire/Edge/Vertex]
        D3[Usage Vectors - per occurrence]
        D4[Geom Vectors - Surf/Curve/PCurve]
        D5[Reverse Indices - myEdgeToWires, myTShapeToDefId]
        D6[History - append-only log]
        D7[UID System - opt-in]
        D8[Attribute Registry - GUID-based]
    end

    subgraph "Storage Layer"
        S1[TopoDS_Shape / BRep_TFace / BRep_TEdge]
        S2[Geom_Surface / Geom_Curve / Geom2d_Curve]
    end

    A1 & A2 & A3 & A4 & A5 & A6 & A7 & A8 & A9 & A10 & A11 --> V1 & V2 & V3 & V4 & V5 & V6 & V7 & V8 & V9 & V10 & V11 & V12
    V1 & V2 & V3 & V4 & V5 & V6 & V7 & V8 & V9 & V10 & V11 & V12 --> D1
    D1 --> D2 & D3 & D4 & D5 & D6 & D7 & D8
    D1 -.->|Build/Reconstruct| S1 & S2
```

## Gap Analysis

```mermaid
graph LR
    subgraph "Strong (Read Path)"
        R1[Topology queries - O1 indexed]
        R2[Geometry access - direct handles]
        R3[Spatial adjacency - prebuilt]
        R4[Parallel analysis - OSD_Parallel]
        R5[Lazy caching - thread-safe]
    end

    subgraph "Weak (Write Path)"
        W1[Manual back-ref maintenance]
        W2[No rollback on failure]
        W3[NodeId invalidated by Compact]
        W4[Only SplitEdge mutation]
        W5[No mutation notifications]
    end

    subgraph "Missing"
        M1[Serialization / persistence]
        M2[Non-manifold topology]
        M3[Graph merging]
        M4[Incremental build]
        M5[Topology healing]
    end

    style R1 fill:#2d6a2d,color:#fff
    style R2 fill:#2d6a2d,color:#fff
    style R3 fill:#2d6a2d,color:#fff
    style R4 fill:#2d6a2d,color:#fff
    style R5 fill:#2d6a2d,color:#fff
    style W1 fill:#8b6914,color:#fff
    style W2 fill:#8b6914,color:#fff
    style W3 fill:#8b6914,color:#fff
    style W4 fill:#8b6914,color:#fff
    style W5 fill:#8b6914,color:#fff
    style M1 fill:#8b1a1a,color:#fff
    style M2 fill:#8b1a1a,color:#fff
    style M3 fill:#8b1a1a,color:#fff
    style M4 fill:#8b1a1a,color:#fff
    style M5 fill:#8b1a1a,color:#fff
```

---

## Dependency Graph

```mermaid
graph TD
    T1_3["T1.3 Benchmarks<br/><b>S</b> | ⏳ in progress"]
    T1_4["T1.4 PCurve Continuity<br/><b>S</b> | ✅ done"]
    T1_5["T1.5 Cached UV Bounds<br/><b>M</b> | ✅ done"]
    T1_1["T1.1 Back-Ref Automation<br/><b>M</b> | foundation"]
    T1_2["T1.2 Mandatory UID<br/><b>M</b> | foundation"]
    T2_1["T2.1 Transactions<br/><b>L</b>"]
    T2_2["T2.2 Mutation Primitives<br/><b>L</b>"]
    T2_3["T2.3 Observers<br/><b>M</b>"]
    T3_1["T3.1 Shell Builder<br/><b>M</b>"]
    T3_2["T3.2 CurveAdaptor<br/><b>M</b> | independent"]
    T3_3["T3.3 Healing<br/><b>XL</b>"]
    T3_4["T3.4 Graph Merge<br/><b>L</b>"]
    T4_1["T4.1 Serialization<br/><b>XL</b>"]
    T4_2["T4.2 Cell-Complex<br/><b>XL</b>"]
    T4_3["T4.3 Incremental Build<br/><b>XL</b>"]

    T1_1 --> T2_1
    T1_1 --> T2_3
    T2_1 --> T2_2
    T2_2 --> T3_1
    T2_1 --> T3_3
    T2_2 --> T3_3
    T2_1 --> T3_4
    T1_1 --> T3_4
    T1_2 --> T3_4
    T1_2 --> T4_1
    T3_1 --> T4_2
    T2_2 --> T4_2
    T1_1 --> T4_3
    T1_2 --> T4_3
    T2_1 --> T4_3
    T1_4 -.->|benefits| T3_2
    T1_5 -.->|benefits| T3_3

    style T1_1 fill:#1a5276,color:#fff
    style T1_2 fill:#1a5276,color:#fff
    style T1_3 fill:#1a5276,color:#fff
    style T1_4 fill:#1a5276,color:#fff
    style T1_5 fill:#1a5276,color:#fff
    style T2_1 fill:#6c3483,color:#fff
    style T2_2 fill:#6c3483,color:#fff
    style T2_3 fill:#6c3483,color:#fff
    style T3_1 fill:#117a65,color:#fff
    style T3_2 fill:#117a65,color:#fff
    style T3_3 fill:#117a65,color:#fff
    style T3_4 fill:#117a65,color:#fff
    style T4_1 fill:#7d6608,color:#fff
    style T4_2 fill:#7d6608,color:#fff
    style T4_3 fill:#7d6608,color:#fff
```

---

## Tier 1 — Foundation (unblocks everything else)

### T1.1: Automated Back-Reference Maintenance

> **The single most impactful improvement.** Eliminates an entire class of bugs for every future algorithm.

**What:** Centralize all back-reference updates (`Surf.FaceDefUsers`, `Curve.EdgeDefUsers`, `myEdgeToWires`) behind a single internal manager. Every mutation path (MutView, BuilderView, Mutator) routes geometry-link changes through this single point.

**Why:** Manual back-ref maintenance is the #1 source of bugs. Deduplicate edge merge leaves stale `Curve.EdgeDefUsers`. Every new algorithm must replicate this logic or risk silent corruption.

```mermaid
graph LR
    subgraph "Before (current)"
        MA[MutView] -->|manual update| BR1[Surf.FaceDefUsers]
        MB[BuilderView] -->|manual update| BR2[Curve.EdgeDefUsers]
        MC[Mutator] -->|manual update| BR3[myEdgeToWires]
        MD[Deduplicate] -->|manual update| BR1 & BR2
    end

    subgraph "After (proposed)"
        MA2[MutView] --> BRM[BackRef Manager]
        MB2[BuilderView] --> BRM
        MC2[Mutator] --> BRM
        MD2[Deduplicate] --> BRM
        BRM --> BR4[All back-references]
    end

    style BRM fill:#2d6a2d,color:#fff
```

**Files:** `BRepGraph_Data.hxx`, `BRepGraph_MutView.cxx`, `BRepGraph_Mutator.cxx`, `BRepGraph_BuilderView.cxx`, `BRepGraphAlgo_Deduplicate.cxx`

**Complexity:** M | **Dependencies:** None

---

### T1.2: Mandatory UID (Stable Node Identity)

**What:** Make UID always-on (remove `SetUIDEnabled()` toggle). Assign UID to every node during `Build()` and programmatic construction. Provide O(1) `UIDToNodeId()`/`NodeIdToUID()`. Preserve UIDs across compaction.

**Why:** `BRepGraph_NodeId` is an index invalidated by compaction. History records, external caches, and algorithm state holding NodeIds break silently.

```mermaid
graph LR
    subgraph "Before: NodeId = raw index"
        N1["Face(2)"] -->|Compact| N2["Face(2) = DIFFERENT face!"]
    end

    subgraph "After: UID = stable identity"
        U1["UID{Face,42,gen=1}"] -->|Compact| U2["UID{Face,42,gen=1} = SAME face"]
    end

    style N2 fill:#8b1a1a,color:#fff
    style U2 fill:#2d6a2d,color:#fff
```

**Files:** `BRepGraph_UID.hxx`, `BRepGraph_Data.hxx`, `BRepGraph.hxx`, `BRepGraphAlgo_Compact.cxx`

**Complexity:** M | **Dependencies:** None

---

### T1.3: Performance Benchmark Suite ⏳ IN PROGRESS

**What:** Parameterized GTest benchmarks: `Build()` time (100/1K/10K faces), `Reconstruct` round-trip, Sewing throughput, Deduplicate+Compact cycle, spatial query throughput.

**Why:** Without benchmarks, regressions from T1.1/T1.2 overhead go undetected. Must measure before changing.

**Files:** `BRepGraph_Benchmark_Test.cxx`, `BRepGraphAlgo_Benchmark_Test.cxx` + `FILES.cmake`

**Status:** Initial benchmark files implemented with smoke tests (enabled) and timing benchmarks (DISABLED by default). Covers Build (100/1K/10K faces), Sewing throughput, Deduplicate+Compact cycle. Remaining: Reconstruct round-trip, spatial query throughput benchmarks.

**Complexity:** S | **Dependencies:** None (do first)

---

### T1.4: Populate PCurve.Continuity ✅ DONE

**What:** During `Build()`, compute `PCurve.Continuity` from `BRep_Tool::MaxContinuity()` for each edge.

**Why:** Field declared in `BRepGraph_GeomNode::PCurve` but always `GeomAbs_C0`. Future fillet/offset algorithms need correct data.

**Files:** `BRepGraph_Builder.cxx`, `BRepGraph.cxx`, `BRepGraph.hxx`

**Implementation notes:** Uses `BRep_Tool::MaxContinuity(edge)` inline during parallel Phase 2 — zero overhead (reads edge's own regularity records). The `createPCurveNode()` signature was extended with a `theContinuity` parameter (default `GeomAbs_C0` for backward compatibility). Also replaced `TopExp::Vertices()` with a local `edgeVertices()` that filters non-vertex children. Tests added in `BRepGraph_Geometry_Test.cxx`.

**Complexity:** S | **Dependencies:** None

---

### T1.5: Cached UV Bounds for Faces ✅ DONE

**What:** `BRepGraphAlgo_UVBounds` class with GUID-based caching on face nodes, computing UV parametric bounds from face PCurves with natural-restriction detection, periodicity clamping, BSpline pseudo-periodicity handling, and `RectangularTrimmedSurface` unwrapping.

**Why:** UV bounds were recomputed on every call in `BRepGraphAlgo_BndLib` (3 call sites), and lacked the BSpline pseudo-periodicity check from `BRepTools::AddUVBounds`. Caching avoids redundant work for future algorithms.

**Files:** New `BRepGraphAlgo_UVBounds.hxx/.cxx`, modified `BRepGraphAlgo_BndLib.cxx` (thin wrapper), `BRepGraph.hxx` (friend), `FILES.cmake`

**Consumers:**
- `BRepGraphAlgo_BndLib` — face bounding box computation (3 call sites via `findExactUVBounds`)
- `BRepGraphCheck_Solid` — candidate for improvement (currently uses raw `Surface->Bounds()` with ±1e6 clamping instead of face-specific UV domain)

**Complexity:** M | **Dependencies:** None

---

## Tier 2 — Mutation Safety (make mutations robust)

### T2.1: Transaction / Mutation Guard

**What:** RAII `BRepGraph_Transaction` class with snapshot/commit/rollback semantics and exclusive write access enforcement.

```mermaid
sequenceDiagram
    participant Algo as Algorithm
    participant Tx as Transaction
    participant Graph as BRepGraph
    participant Val as Validate

    Algo->>Graph: BeginTransaction()
    Graph->>Tx: Create (snapshot state)
    Algo->>Graph: Mut().SplitFace(...)
    Algo->>Graph: Mut().MergeEdge(...)
    Note over Algo: If exception thrown here...
    Algo->>Tx: Commit()
    Tx->>Val: Validate(graph)
    alt Valid
        Val-->>Tx: OK
        Tx-->>Algo: Success
    else Invalid
        Val-->>Tx: Errors found
        Tx->>Graph: Rollback to snapshot
        Tx-->>Algo: Failure
    end
    Note over Tx: ~Transaction() auto-rollbacks if not committed
```

**Why:** No concurrent mutation detection, no rollback, no post-mutation validation. Failed algorithms leave graph inconsistent.

**Files:** New `BRepGraph_Transaction.hxx/.cxx`, `BRepGraph.hxx`, `BRepGraph_MutView.hxx`

**Complexity:** L | **Dependencies:** T1.1

---

### T2.2: Higher-Level Mutation Primitives

**What:** Extend `BRepGraph_Mutator` with:

| Operation | Description | Current alternative |
|-----------|-------------|-------------------|
| `SplitFace` | Split face along wire, produce two new defs | None |
| `MergeEdge` | Merge coincident edges (inverse of SplitEdge) | Manual in Sewing |
| `MergeVertex` | Merge vertices, rewrite all referencing edges | Manual in Deduplicate |
| `RemoveEdgeFromWire` | Remove edge and reconnect wire | None |

**Why:** Only `SplitEdge` and `ReplaceEdgeInWire` exist. Every algorithm reimplements mutation logic, causing back-reference bugs.

**Files:** `BRepGraph_Mutator.hxx/.cxx`, `BRepGraph_MutView.hxx`, `BRepGraphAlgo_Sewing.cxx`

**Complexity:** L | **Dependencies:** T1.1, T2.1

---

### T2.3: Observer / Event System

**What:** `BRepGraph_Observer` interface with virtual callbacks fired from the back-ref manager (T1.1).

```mermaid
graph TD
    M[Mutation via MutView] --> BRM[BackRef Manager]
    BRM --> BR[Update back-references]
    BRM --> OBS[Fire observer events]
    OBS --> O1[BndLib cache invalidation]
    OBS --> O2[Mesher triangulation update]
    OBS --> O3[User algorithm callback]

    style OBS fill:#6c3483,color:#fff
```

**Why:** Cache invalidation is manual. Third-party code has no notification mechanism. Observers decouple mutation from invalidation.

**Files:** New `BRepGraph_Observer.hxx`, `BRepGraph.hxx`, `BRepGraph_MutView.cxx`, `BRepGraph_Mutator.cxx`

**Complexity:** M | **Dependencies:** T1.1

---

## Tier 3 — Algorithms (new graph-native capabilities)

### T3.1: Graph-Native Shell Builder

**What:** `BRepGraphAlgo_MakeShell` -- given face NodeIds, construct ShellDef by detecting shared edges, orienting faces, detecting openness/closedness.

**Why:** Shell construction is fundamental for Sewing (phase 7), future booleans, any face-creating algorithm. Currently reimplemented inline.

**Files:** New `BRepGraphAlgo_MakeShell.hxx/.cxx`, `BRepGraph_SpatialView.hxx`

**Complexity:** M | **Dependencies:** T1.1, T2.2

---

### T3.2: Per-Usage Location-Aware Curve/Surface Adaptors

**What:** `GeomView::CurveAdaptorForUsage(UsageId)` -- lightweight adaptor evaluating geometry with usage's `GlobalLocation` applied, without copying.

**Why:** Currently callers manually compose curve + `Spatial().GlobalTransform()`. Error-prone. Critical for intersection, booleans, geometric queries.

**Files:** `BRepGraph_GeomView.hxx/.cxx`

**Complexity:** M | **Dependencies:** None (benefits from T1.4)

---

### T3.3: Graph-Based Topology Healing

**What:** `BRepGraphAlgo_Heal` -- fix issues detected by BRepGraphCheck: gap-closing, tolerance adjustment, face normal consistency. Operates directly on graph.

```mermaid
graph LR
    BC[BRepGraphCheck_Analyzer] -->|detect issues| R[BRepGraphCheck_Report]
    R -->|read issues| H[BRepGraphAlgo_Heal]
    H -->|fix via| M[Mutation Primitives T2.2]
    M -->|modified| G[BRepGraph]
    G -->|re-validate| BC

    style H fill:#117a65,color:#fff
```

**Why:** BRepGraphCheck detects issues but provides no repair. Currently requires TopoDS round-trip. Graph-native healer closes the detect-fix loop.

**Files:** New `BRepGraphAlgo_Heal.hxx/.cxx`

**Complexity:** XL | **Dependencies:** T2.1, T2.2

---

### T3.4: Graph Merging

**What:** `BRepGraphAlgo_Merge` -- combine two BRepGraph instances into one. Remap NodeIds, optionally deduplicate shared geometry, preserve UIDs.

**Why:** Needed for assembly operations, boolean pre-processing, incremental model building. Without this, combining shapes requires `AppendShape()` which loses graph metadata.

**Files:** New `BRepGraphAlgo_Merge.hxx/.cxx`

**Complexity:** L | **Dependencies:** T1.1, T1.2, T2.1

---

## Tier 4 — Advanced (long-term evolution)

### T4.1: Serialization / Persistence

**What:** Binary format for BRepGraph with UID-based cross-references. `BRepGraphAlgo_Serialize::Write/Read`. Virtual serialize on `BRepGraph_UserAttribute`.

**Why:** Only persistence path is TopoDS reconstruction + STEP/BRep writers, losing all graph metadata. Enables: disk caching, inter-process transfer, undo/redo checkpoints.

**Files:** New `BRepGraph_Serialize.hxx/.cxx`, `BRepGraph_UserAttribute.hxx`

**Complexity:** XL | **Dependencies:** T1.2

---

### T4.2: Cell-Complex Abstraction (Non-Manifold)

**What:** Extend graph with `CellDef` nodes for non-manifold topology where an edge can be shared by more than two faces.

```mermaid
graph TB
    subgraph "Current: Manifold only"
        E1[Edge] --- F1[Face A]
        E1 --- F2[Face B]
    end

    subgraph "Proposed: Non-manifold via CellDef"
        E2[Edge] --- FA[Face A]
        E2 --- FB[Face B]
        E2 --- FC[Face C]
        FA & FB --> C1[Cell 1]
        FB & FC --> C2[Cell 2]
    end

    style C1 fill:#7d6608,color:#fff
    style C2 fill:#7d6608,color:#fff
```

**Why:** Current model assumes manifold topology. Non-manifold needed for: FEM, T-junctions, unregularized boolean results.

**Files:** `BRepGraph_NodeId.hxx`, `BRepGraph_TopoNode.hxx`, `BRepGraph_Data.hxx`, `BRepGraph_SpatialView.hxx`, `BRepGraphCheck_Analyzer.hxx`

**Complexity:** XL | **Dependencies:** T2.2, T3.1

---

### T4.3: Incremental Build / Partial Update

**What:** `IncrementalAdd(shape)` with dedup against existing defs. `IncrementalRemove(nodeId)` with reference cleanup. `IncrementalRebuild(nodeIds)` for targeted re-extraction.

**Why:** For interactive CAD, full `Build()` after every edit is too expensive. Proper incremental build preserves O(delta) cost.

**Files:** `BRepGraph_Builder.hxx/.cxx`, `BRepGraph_Data.hxx`, `BRepGraph_BuilderView.hxx`

**Complexity:** XL | **Dependencies:** T1.1, T1.2, T2.1

---

## Target Architecture (after all tiers)

```mermaid
graph TB
    subgraph "Application Layer"
        AA1[Sewing]
        AA2[Copy / Transform]
        AA3[Deduplicate + Compact]
        AA4[Heal - T3.3]
        AA5[MakeShell - T3.1]
        AA6[Merge - T3.4]
        AA7[Future: Boolean, Fillet, Offset...]
    end

    subgraph "Transaction Layer - T2.1"
        TX[BRepGraph_Transaction]
        TX -->|commit/rollback| CORE
    end

    subgraph "View API + Observers - T2.3"
        VV1[Read Views: Defs, Usages, Geom, Spatial, ...]
        VV2[Write Views: Mut, Builder]
        VV3[Observers]
        VV2 -->|notify| VV3
    end

    subgraph "Core Data Layer"
        CORE[BRepGraph_Data]
        BRM2[BackRef Manager - T1.1]
        UID2[Mandatory UID - T1.2]
        CELL[CellDef - T4.2]
        SER[Serialization - T4.1]
        CORE --- BRM2 & UID2
        CORE -.- CELL & SER
    end

    subgraph "Storage Layer"
        SS1[TopoDS_Shape]
        SS2[Geom_Surface / Geom_Curve]
    end

    AA1 & AA2 & AA3 & AA4 & AA5 & AA6 & AA7 --> TX
    TX --> VV1 & VV2
    VV1 & VV2 --> CORE
    CORE -.->|Build/Reconstruct| SS1 & SS2

    style TX fill:#6c3483,color:#fff
    style BRM2 fill:#1a5276,color:#fff
    style UID2 fill:#1a5276,color:#fff
    style CELL fill:#7d6608,color:#fff
    style SER fill:#7d6608,color:#fff
    style AA4 fill:#117a65,color:#fff
    style AA5 fill:#117a65,color:#fff
    style AA6 fill:#117a65,color:#fff
    style AA7 fill:#117a65,color:#fff
```

---

## Recommended Implementation Order

| # | Item | Tier | Complexity | Rationale |
|---|------|------|-----------|-----------|
| 1 | T1.3 Benchmarks | 1 | S | Measure before changing — **in progress** |
| 2 | T1.4 PCurve Continuity | 1 | S | **Done** — uses `BRep_Tool::MaxContinuity` |
| 2b | T1.5 Cached UV Bounds | 1 | M | **Done** — `BRepGraphAlgo_UVBounds` with GUID caching |
| 3 | T1.1 Back-Ref Automation | 1 | M | Biggest risk-reducer |
| 4 | T1.2 Mandatory UID | 1 | M | Enables stable identity |
| 5 | T2.1 Transactions | 2 | L | Enables safe multi-step mutations |
| 6 | T2.3 Observers | 2 | M | Transaction-aware events and cache invalidation |
| 7 | T2.2 Mutation Primitives | 2 | L | Enables higher-level algorithms |
| 8 | T3.2 CurveAdaptor | 3 | M | Independent utility |
| 9 | T3.1 Shell Builder | 3 | M | Simplifies Sewing, unblocks T4.2 |
| 10 | T3.4 Graph Merge | 3 | L | Enables assembly workflows |
| 11 | T3.3 Healing | 3 | XL | Closes detect-fix loop |
| 12 | T4.1 Serialization | 4 | XL | Persistence layer |
| 13 | T4.2 Cell-Complex | 4 | XL | Non-manifold support |
| 14 | T4.3 Incremental Build | 4 | XL | Interactive performance |

**Complexity key:** S = days, M = 1-2 weeks, L = 2-4 weeks, XL = 1-2 months
