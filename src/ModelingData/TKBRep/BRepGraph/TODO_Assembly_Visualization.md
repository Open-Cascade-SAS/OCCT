# BRepGraph Assembly Model — Visualizations

Mermaid diagrams illustrating the intrinsic assembly architecture.
See [TODO_Assembly.md](TODO_Assembly.md) for the full design specification.

---

## 1. Product/Occurrence DAG

A typical assembly: two parts (Bolt, Plate) with shared instances.

```mermaid
graph TD
    P0["Product 0<br/><b>Assembly</b><br/>(no ShapeRootId)"]
    O0["Occurrence 0<br/>Placement: T1"]
    O1["Occurrence 1<br/>Placement: T2"]
    O2["Occurrence 2<br/>Placement: T3"]

    P1["Product 1<br/><b>Part: Bolt</b><br/>ShapeRootId → Solid(0)"]
    P2["Product 2<br/><b>Part: Plate</b><br/>ShapeRootId → Solid(1)"]

    P0 -->|OccurrenceUsage| O0
    P0 -->|OccurrenceUsage| O1
    P0 -->|OccurrenceUsage| O2

    O0 -->|ProductDefId| P1
    O1 -->|ProductDefId| P1
    O2 -->|ProductDefId| P2

    S0["Solid(0)<br/>Bolt geometry"]
    S1["Solid(1)<br/>Plate geometry"]

    P1 -.->|ShapeRootId| S0
    P2 -.->|ShapeRootId| S1

    style P0 fill:#4a9eff,color:#fff
    style P1 fill:#2ecc71,color:#fff
    style P2 fill:#2ecc71,color:#fff
    style O0 fill:#f39c12,color:#fff
    style O1 fill:#f39c12,color:#fff
    style O2 fill:#f39c12,color:#fff
    style S0 fill:#95a5a6,color:#fff
    style S1 fill:#95a5a6,color:#fff
```

**DAG sharing**: Product 1 (Bolt) is referenced by two occurrences (O0, O1),
each with a different placement.  The bolt geometry (Solid 0) exists once.

---

## 2. Deep Nesting (Multi-Level Assembly)

```mermaid
graph TD
    Car["Product: Car<br/><b>Assembly</b>"]
    OccChassis["Occurrence<br/>Placement: Identity"]
    OccWheel1["Occurrence<br/>Placement: T_FL"]
    OccWheel2["Occurrence<br/>Placement: T_FR"]

    Chassis["Product: Chassis<br/><b>Assembly</b>"]
    OccAxle1["Occurrence<br/>Placement: T_front"]
    OccAxle2["Occurrence<br/>Placement: T_rear"]

    Axle["Product: Axle<br/><b>Part</b><br/>ShapeRootId → Solid(2)"]
    Wheel["Product: Wheel<br/><b>Part</b><br/>ShapeRootId → Solid(3)"]

    Car -->|OccurrenceUsage| OccChassis
    Car -->|OccurrenceUsage| OccWheel1
    Car -->|OccurrenceUsage| OccWheel2

    OccChassis -->|ProductDefId| Chassis
    OccWheel1 -->|ProductDefId| Wheel
    OccWheel2 -->|ProductDefId| Wheel

    Chassis -->|OccurrenceUsage| OccAxle1
    Chassis -->|OccurrenceUsage| OccAxle2

    OccAxle1 -->|ProductDefId| Axle
    OccAxle2 -->|ProductDefId| Axle

    style Car fill:#4a9eff,color:#fff
    style Chassis fill:#4a9eff,color:#fff
    style Axle fill:#2ecc71,color:#fff
    style Wheel fill:#2ecc71,color:#fff
    style OccChassis fill:#f39c12,color:#fff
    style OccWheel1 fill:#f39c12,color:#fff
    style OccWheel2 fill:#f39c12,color:#fff
    style OccAxle1 fill:#f39c12,color:#fff
    style OccAxle2 fill:#f39c12,color:#fff
```

**OccurrenceLocation** for OccAxle1: `T_identity * T_front` (composed root-to-leaf).

---

## 3. Single-Shape Graph (Degenerate Case)

Every graph has a root Product — even a simple `Build(aBox)`:

```mermaid
graph LR
    P["Product 0<br/><b>Part</b><br/>ShapeRootId → Solid(0)"]
    S["Solid(0)"]
    Sh["Shell(0)"]
    F0["Face(0)"]
    F1["Face(1)"]
    Fn["...Face(5)"]

    P -.->|ShapeRootId| S
    S --> Sh
    Sh --> F0
    Sh --> F1
    Sh --> Fn

    style P fill:#2ecc71,color:#fff
    style S fill:#95a5a6,color:#fff
    style Sh fill:#95a5a6,color:#fff
    style F0 fill:#95a5a6,color:#fff
    style F1 fill:#95a5a6,color:#fff
    style Fn fill:#95a5a6,color:#fff
```

Zero occurrences.  Algorithms see the same uniform model.

---

## 4. API Distribution Across Views

```mermaid
graph TB
    subgraph BRepGraph
        direction TB
        MutProduct["MutProduct(idx)"]
        MutOccurrence["MutOccurrence(idx)"]
    end

    subgraph TopoPathView["TopoView / PathView (const)"]
        direction TB
        DV1["Product(idx) / Occurrence(idx)"]
        DV2["NbProducts() / NbOccurrences()"]
        DV3["RootProducts()"]
        DV4["IsAssembly(idx) / IsPart(idx)"]
        DV5["NbComponents(idx) / Component(idx, i)"]
    end

    subgraph BuilderView["BuilderView (mutation)"]
        direction TB
        BV1["AddProduct(shapeRoot)"]
        BV2["AddAssemblyProduct()"]
        BV3["AddOccurrence(parent, ref, placement)"]
        BV4["RemoveNode — cascade for Products"]
        BV5["RemoveSubgraph — deep removal"]
    end

    subgraph MutGraph["BRepGraph (RAII guards)"]
        direction TB
        MV1["MutProduct(idx)"]
        MV2["MutOccurrence(idx)"]
    end

    subgraph PathView["PathView (assembly context)"]
        direction TB
        SV1["GlobalLocation(path)"]
        SV2["OccurrenceLocation(occIdx)"]
    end

    subgraph ResolveLater["Deferred attribute resolution"]
        direction TB
        AQ1["ResolveLayerValue?(occ)"]
        AQ2["Leaf-part traversal via Explorer"]
        AQ3["Occurrence paths via PathView"]
    end

    style TopoPathView fill:#e8f4fd,stroke:#4a9eff
    style BuilderView fill:#fdf2e8,stroke:#f39c12
    style MutGraph fill:#e8fdf0,stroke:#2ecc71
    style PathView fill:#f0e8fd,stroke:#9b59b6
    style ResolveLater fill:#fde8e8,stroke:#e74c3c
    style BRepGraph fill:#f5f5f5,stroke:#333
```

---

## 5. Entity Struct Hierarchy

```mermaid
classDiagram
    class BaseDef {
        +BRepGraph_NodeId Id
        +BRepGraph_NodeCache Cache
        +bool IsModified
        +bool IsRemoved
    }

    class ProductDef {
        +BRepGraph_NodeId ShapeRootId
        +NCollection_Vector~OccurrenceUsage~ OccurrenceRefs
        +InitVectors(allocator)
    }

    class OccurrenceDef {
        +BRepGraph_ProductId ProductDefId
        +BRepGraph_ProductId ParentProductDefId
        +BRepGraph_OccurrenceId ParentOccurrenceDefId
        +TopLoc_Location Placement
    }

    class OccurrenceUsage {
        +BRepGraph_OccurrenceId OccurrenceDefId
    }

    BaseDef <|-- ProductDef
    BaseDef <|-- OccurrenceDef
    ProductDef *-- OccurrenceUsage : OccurrenceRefs

    class SolidDef {
        +NCollection_Vector~ShellUsage~ ShellRefs
    }
    class CompoundDef {
        +NCollection_Vector~ChildUsage~ ChildRefs
    }

    BaseDef <|-- SolidDef
    BaseDef <|-- CompoundDef

    note for ProductDef "Part: ShapeRootId valid, OccurrenceRefs empty\nAssembly: ShapeRootId invalid, OccurrenceRefs non-empty"
```

---

## 6. Compact Remap Flow (Phase 3)

```mermaid
flowchart LR
    subgraph Before["Before Compact"]
        P0b["Product 0 ✓"]
        P1b["Product 1 ✗ removed"]
        P2b["Product 2 ✓"]
        O0b["Occ 0 ✓"]
        O1b["Occ 1 ✗ removed"]
        O2b["Occ 2 ✓"]
    end

    subgraph RemapMap["Unified Remap Map"]
        R1["Product(0) → Product(0)"]
        R2["Product(2) → Product(1)"]
        R3["Occ(0) → Occ(0)"]
        R4["Occ(2) → Occ(1)"]
    end

    subgraph After["After Compact"]
        P0a["Product 0"]
        P1a["Product 1<br/>(was Product 2)"]
        O0a["Occ 0"]
        O1a["Occ 1<br/>(was Occ 2)"]
    end

    Before --> RemapMap
    RemapMap --> After

    style P1b fill:#e74c3c,color:#fff
    style O1b fill:#e74c3c,color:#fff
    style P0b fill:#2ecc71,color:#fff
    style P2b fill:#2ecc71,color:#fff
    style O0b fill:#2ecc71,color:#fff
    style O2b fill:#2ecc71,color:#fff
```

Cross-references (`ProductDefId`, `ParentProductDefId`, `ShapeRootId`, `OccurrenceRefs`)
are remapped using `theRemapMap.Find()`.  Missing entries trigger an assertion.

---

## 7. Phase Dependency Graph

```mermaid
flowchart TD
    Ph1["Phase 1<br/>Data Model<br/>+ auto root Product"]
    Ph2["Phase 2<br/>Core API Integration<br/>TopoView / PathView,<br/>BuilderView, MutProduct,<br/>Iterator, ReverseIndex"]
    Ph3["Phase 3<br/>OnCompact<br/>Signature Fix"]
    Ph4["Phase 4<br/>XDE Population<br/>Bridge"]
    Ph5["Phase 5<br/>Facade Reconstruction<br/>(done)"]
    Ph6["Phase 6<br/>Deferred attribute<br/>resolution"]
    Ph7["Phase 7<br/>Testing"]

    Ph1 --> Ph2
    Ph2 --> Ph3
    Ph2 --> Ph4
    Ph2 --> Ph5
    Ph2 --> Ph6
    Ph3 --> Ph7
    Ph4 --> Ph7
    Ph5 --> Ph7
    Ph6 --> Ph7

    style Ph1 fill:#4a9eff,color:#fff
    style Ph2 fill:#4a9eff,color:#fff
    style Ph3 fill:#f39c12,color:#fff
    style Ph4 fill:#f39c12,color:#fff
    style Ph5 fill:#f39c12,color:#fff
    style Ph6 fill:#f39c12,color:#fff
    style Ph7 fill:#2ecc71,color:#fff
```

---

## 8. XDE-to-BRepGraph Population (Phase 4)

```mermaid
flowchart LR
    subgraph XDE["XDE Document"]
        L0["Label: Assembly A"]
        L1["Label: Component → Part B"]
        L2["Label: Component → Part B"]
        L3["Label: Component → Part C"]
        LB["Label: Part B (shape)"]
        LC["Label: Part C (shape)"]
    end

    subgraph Bridge["BRepGraphDE_PopulateAssembly"]
        Walk["Walk label tree"]
        Dedup["TDF_Label → int\ndeduplication map"]
    end

    subgraph Graph["BRepGraph"]
        GP0["Product: A (assembly)"]
        GO0["Occ 0 → B, Placement T1"]
        GO1["Occ 1 → B, Placement T2"]
        GO2["Occ 2 → C, Placement T3"]
        GP1["Product: B (part)"]
        GP2["Product: C (part)"]
    end

    XDE --> Bridge
    Bridge --> Graph
    L1 -.-> GO0
    L2 -.-> GO1
    L3 -.-> GO2
    LB -.->|shared| GP1
    LC -.-> GP2

    style XDE fill:#f5f5f5,stroke:#333
    style Bridge fill:#fdf2e8,stroke:#f39c12
    style Graph fill:#e8f4fd,stroke:#4a9eff
```

Part B appears once as a Product despite two component references — shared via
`TDF_Label → int` deduplication map.

---

## 9. Location Propagation

```mermaid
flowchart TD
    Root["Product: Car (root)<br/>Location: Identity"]
    Occ1["Occurrence<br/>Placement: T_chassis"]
    Chassis["Product: Chassis"]
    Occ2["Occurrence<br/>Placement: T_front_axle"]
    Axle["Product: Axle (part)"]

    Root -->|OccurrenceUsage| Occ1
    Occ1 -->|ProductDefId| Chassis
    Chassis -->|OccurrenceUsage| Occ2
    Occ2 -->|ProductDefId| Axle

    GlobalLoc["OccurrenceLocation(Occ2) =<br/>T_chassis * T_front_axle"]

    Occ2 -.-> GlobalLoc

    style Root fill:#4a9eff,color:#fff
    style Chassis fill:#4a9eff,color:#fff
    style Axle fill:#2ecc71,color:#fff
    style Occ1 fill:#f39c12,color:#fff
    style Occ2 fill:#f39c12,color:#fff
    style GlobalLoc fill:#fff,stroke:#e74c3c,stroke-width:2px
```

`PathView::OccurrenceLocation(occId)` walks `ParentOccurrenceDefId` chain upward,
composing `TopLoc_Location` values from root to leaf.
