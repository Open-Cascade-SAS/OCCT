# Persistent Identity for References in BRepGraph

## 1. Current State

### 1.1 Three-Pillar ID Model

BRepGraph currently has three ID types:

| ID Type | Struct | Fields | Purpose | Storage |
|---------|--------|--------|---------|---------|
| **NodeId** | `BRepGraph_NodeId` | `Kind + Index` | Volatile position in per-kind entity vector | `DefStore<T>::Entities` |
| **RepId** | `BRepGraph_RepId` | `Kind + Index` | Volatile position in per-kind representation vector | `RepStore<T>::Entities` |
| **UID** | `BRepGraph_UID` | `Kind + Counter + Generation` | Persistent identity across Compact/removal | `DefStore<T>::UIDs` (parallel vector) |

### 1.2 Entity Kinds (NodeId::Kind)

```
Topology:  Solid(0), Shell(1), Face(2), Wire(3), Edge(4), Vertex(5),
           Compound(6), CompSolid(7), CoEdge(8)
Assembly:  Product(10), Occurrence(11)
```

Every entity inherits `BaseDef` which carries: `Id`, `Cache`, `MutationGen`, `IsModified`, `IsRemoved`.

### 1.3 Reference Structs (Inline, No Persistent ID)

References are inline structs stored in parent entity vectors. They encode **usage context** (orientation, location) of a child entity within a parent:

| Ref Struct | Parent → Child | Fields | Data Weight |
|------------|----------------|--------|-------------|
| `ShellUsage` | Solid → Shell | ShellDefId, Orientation, LocalLocation | Light |
| `FaceUsage` | Shell → Face | FaceDefId, Orientation, LocalLocation | Light |
| `WireUsage` | Face → Wire | WireDefId, IsOuter, Orientation, LocalLocation | Light |
| `CoEdgeUsage` | Wire → CoEdge | CoEdgeDefId, LocalLocation | Minimal |
| `VertexUsage` | Edge/Face → Vertex | VertexDefId, Orientation, LocalLocation | Light |
| `SolidUsage` | CompSolid → Solid | SolidDefId, Orientation, LocalLocation | Light |
| `ChildUsage` | Compound → any | ChildDefId(NodeId), Orientation, LocalLocation | Light |
| `OccurrenceUsage` | Product → Occurrence | OccurrenceDefId | Minimal |

### 1.4 Already-Promoted References

Two reference types have already been promoted to full entities:

1. **CoEdge (Kind=8)**: Originally edge-in-wire reference. Promoted because it carries heavy data (PCurve, UV endpoints, parameters, continuity, seam pairing, polygon representations). Has its own `BaseDef`, UID, MutationGen.

2. **Occurrence (Kind=11)**: Assembly instance. Promoted because it forms a DAG structure (parent occurrence, parent product, placement). Has its own `BaseDef`, UID, MutationGen.

### 1.5 What References Lack Today

- **No persistent identity**: ShellUsage, FaceUsage, WireUsage, VertexUsage, SolidUsage, ChildUsage have no UID
- **No mutation tracking**: If a FaceUsage's orientation changes (face flipped in shell), there's no per-ref MutationGen — only the parent entity's MutationGen increments
- **No VersionStamp**: Cannot stamp or detect staleness of a specific reference
- **No serialization anchor**: Cannot round-trip reference identity across save/load
- **No external ID mapping**: Cannot map STEP entity IDs or XCAF labels to references

---

## 2. Why References Need Identity

### 2.1 External System Requirements

#### STEP AP203/AP214/AP242

STEP models topology as a hierarchy of entities, each with its own `#entity_id`:

```
#100 = MANIFOLD_SOLID_BREP('solid', #101);
#101 = CLOSED_SHELL('shell', (#102, #103, ...));
#102 = ADVANCED_FACE('face1', (#110), #200, .T.);  ← face-in-shell with orientation
#110 = FACE_OUTER_BOUND('wire1', #111, .T.);        ← wire-in-face with orientation
#111 = EDGE_LOOP('loop', (#120, #121, ...));
#120 = ORIENTED_EDGE('', *, *, #300, .T.);           ← edge-in-loop ≈ CoEdge
```

Key observations:
- `ADVANCED_FACE` is both the face entity AND its usage in a shell (orientation `.T.`/`.F.` baked in)
- `FACE_OUTER_BOUND` / `FACE_BOUND` are wire-in-face references with their own `#id`
- `ORIENTED_EDGE` is the CoEdge (already covered)
- AP242 adds `item_identified_representation_usage` for persistent external IDs on any item

**Need**: Each face-in-shell, wire-in-face reference must be mappable to a STEP entity `#id`. Currently impossible without RefId.

#### CATIA V5/V6

CATIA uses **persistent naming** for parametric stability:
- Each topology element has a "BRep Access" name computed from its context path
- Path example: `Solid.1/Shell.1/Face.3/Wire.1/Edge.2`
- The path identifies the reference (Face.3 in Shell.1), not just the entity (Face #3)
- When the model is recomputed, persistent names allow tracking which face "survived"

**Need**: Stable identity for each level of the containment path. RefId provides this.

#### ACIS (Spatial)

ACIS models all topology as `ENTITY` objects with unique pointers:
- `BODY → LUMP → SHELL → FACE → LOOP → COEDGE → EDGE → VERTEX`
- Every level is a separate allocated object with its own identity
- `FACE` in ACIS is not shared across shells — each shell-face link is a distinct `FACE` entity
- `COEDGE` ≈ BRepGraph CoEdge (already an entity)
- `LOOP` ≈ Wire with its face context

**Need**: Reference-level identity matches ACIS's "everything is an entity" model.

#### Parasolid (Siemens)

Similar to ACIS but uses different terminology:
- `BODY → REGION → SHELL → FACE → LOOP → FIN → EDGE → VERTEX`
- `FIN` ≈ CoEdge (oriented edge use)
- Every element has a persistent **tag** (integer ID that survives model edits)
- Tags are assigned to all topology objects, including usage-context objects

**Need**: Parasolid tags map directly to UIDs on both entities and references.

#### XCAF / XBF (OCCT Document Framework)

XCAF uses `TDF_Label` paths as persistent identifiers:
```
0:1:1:1      — Assembly product
0:1:1:1:1    — Child occurrence 1
0:1:1:2      — Part shape
0:1:1:2:1    — Sub-shape: face 1 of part
0:1:1:2:2    — Sub-shape: edge 3 of part
```

Key points:
- Sub-shape labels are created via `XCAFDoc_ShapeTool::FindSubShape()`
- Each sub-shape label can carry attributes (color, material, name)
- The label path IS the persistent identifier
- Labels can be created for shapes at any level — including face-in-shell, wire-in-face

**Need**: BRepGraph RefId → TDF_Label mapping. Each ref gets a label where attributes can be stored.

For **XBF** (binary format): TDF_Label tree is serialized. RefUID would serialize alongside entity UIDs, giving the binary file complete identity coverage.

### 2.2 Internal Use Cases

1. **Granular change tracking**: Detect "face was reoriented in shell" vs "face geometry changed"
2. **Caching**: Cache per-reference computed data (e.g., face normal in shell context, composed location)
3. **History tracking**: Record "this face-in-shell reference was split into two during boolean operation"
4. **Undo/redo**: Identify specific references to restore
5. **Validation**: Verify that a specific reference still exists after model editing

---

## 3. Design Options

### Option A: UID Field on Existing Ref Structs

Add `BRepGraph_UID` directly to each inline Ref struct:

```cpp
struct FaceUsage
{
  BRepGraph_FaceId   FaceDefId;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
  TopLoc_Location    LocalLocation;
  BRepGraph_UID      RefUID;        // NEW
  uint32_t           MutationGen;   // NEW (optional)
};
```

**How UID is allocated**: During `Build()` and `AddFaceToShell()`, call `allocateUID()` with a ref-specific Kind. UIDs stored inline on ref structs.

**Pros**:
- Minimal structural change — refs stay inline in parent entity vectors
- Cache-friendly: ref data and its UID are co-located
- Easy migration: just add fields to existing structs
- No new storage vectors

**Cons**:
- **Mixed ID space**: Ref UIDs use the same `BRepGraph_UID` type as entity UIDs. No compile-time separation. A RefUID could be accidentally passed where an entity UID is expected.
- **UID lifecycle complexity**: Who manages ref UIDs? Builder must track them during Add/Remove operations. No parallel UID vector — UIDs scattered across parent entity vectors.
- **Difficult reverse lookup**: Given a UID, finding the ref requires scanning all parent entities' ref vectors. Much worse than entity UID lookup (which is per-kind vector scan).
- **No standalone MutationGen**: If ref changes, parent entity's MutationGen increments. Granular per-ref tracking requires adding MutationGen to each ref struct.
- **Serialization awkward**: UIDs interleaved with ref data instead of stored in parallel vectors.
- **No separate NbActive count**: Can't count active refs independently from entities.

**Verdict**: Quick to implement but architecturally messy. Creates a second-class citizen in the UID system.

---

### Option B: Promote All Refs to Full Entity Kinds

Extend `NodeId::Kind` with new entity kinds for each reference type:

```cpp
enum class Kind : int
{
  // Existing topology entities
  Solid = 0, Shell = 1, Face = 2, Wire = 3, Edge = 4, Vertex = 5,
  Compound = 6, CompSolid = 7, CoEdge = 8,

  // Existing assembly entities
  Product = 10, Occurrence = 11,

  // NEW: Reference entities (promoted from inline refs)
  ShellUse   = 20,  // was ShellUsage (Shell used in Solid)
  FaceUse    = 21,  // was FaceUsage (Face used in Shell)
  WireUse    = 22,  // was WireUsage (Wire used in Face)
  VertexUse  = 23,  // was VertexUsage (Vertex on Edge/Face)
  SolidUse   = 24,  // was SolidUsage (Solid in CompSolid)
  ChildUse   = 25,  // was ChildUsage (any in Compound)
};
```

Each promoted ref becomes a full entity with `BaseDef`, stored in `DefStore<XxxUseEntity>`:

```cpp
struct FaceUseEntity : public BaseDef
{
  BRepGraph_FaceId   FaceDefId;
  BRepGraph_ShellId  ParentShellId;  // back-pointer
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};
```

Parent entities store typed IDs instead of inline refs:
```cpp
struct ShellDef : public BaseDef
{
  NCollection_Vector<BRepGraph_FaceUseId> FaceUseIds;  // was NCollection_Vector<FaceUsage>
};
```

**Pros**:
- **Uniform**: Everything with a UID is an entity. Single `DefStore` pattern handles all.
- **Automatic MutationGen**: `BaseDef` already has it. `markModified()` works out of the box.
- **VersionStamp works**: `UIDs().StampOf(FaceUseId)` — no API changes needed.
- **Clean UID lifecycle**: `allocateUID()` works identically for use-entities and topology entities.
- **Reverse index**: Can add reverse lookups (e.g., `FaceUsesOfShell()`) using existing `ReverseIndex` pattern.
- **Follows ACIS/Parasolid model**: Industry-standard approach.
- **CoEdge precedent**: Already proven that promoting refs to entities works well.

**Cons**:
- **Entity count explosion**: A box (6 faces, 24 edges) gains ~47 use-entities: 1 ShellUse + 6 FaceUse + 6 WireUse + 24 CoEdge (existing) + ~10 VertexUse. For large models (100K faces), adds ~300K use-entities.
- **Storage overhead**: Each `BaseDef` is ~40-48 bytes (Id + Cache + MutationGen + IsModified + IsRemoved). Multiplied by 300K = ~14 MB overhead for a 100K-face model. Not prohibitive but noticeable.
- **More Kind values**: Every switch/dispatch over `Kind` grows. Affects `TopoDef()`, `allocateUID()`, entity accessors, views.
- **Deeper indirection**: Shell → FaceUseId → FaceUseEntity → FaceDefId → FaceDef. One extra hop.
- **API surface growth**: New typed IDs (`BRepGraph_FaceUseId`, `BRepGraph_WireUseId`, etc.), new accessors on views, new mutation methods on BuilderView.
- **Mixed semantics**: "Use" entities are fundamentally different from "definition" entities. Mixing them in the same `Kind` enum and `DefStore` obscures the conceptual distinction.
- **CoEdgeUsage becomes redundant**: CoEdgeUsage is already just `{CoEdgeDefId, LocalLocation}`. If we also promote it to a "CoEdgeUse" entity, CoEdge itself becomes awkward (CoEdge is already a promoted ref, so CoEdgeUse would be a "use of a use").

**Verdict**: Architecturally clean for the UID system but adds significant weight. Better suited if use-entities need to carry substantial per-use data (like CoEdge does).

---

### Option C: New RefId Type with Dedicated Reference Tables (User's Proposal)

Introduce a **third ID type** (`BRepGraph_RefId`) separate from both `NodeId` and `RepId`:

```cpp
struct BRepGraph_RefId
{
  // Unified naming — same as entity kinds, parent is data not type
  enum class Kind : int
  {
    Shell   = 0,  // Shell ref (parent: Solid)
    Face    = 1,  // Face ref (parent: Shell, Compound)
    Wire    = 2,  // Wire ref (parent: Face)
    CoEdge  = 3,  // CoEdge ref (parent: Wire)
    Vertex  = 4,  // Vertex ref (parent: Edge or Face)
    Solid   = 5,  // Solid ref (parent: CompSolid)
    Child   = 6,  // Any-kind child ref (parent: Compound)
  };

  Kind RefKind;
  int  Index;

  // Typed wrapper (same pattern as NodeId::Typed<Kind>)
  template <Kind TheKind>
  struct Typed { int Index; /* ... */ };
};

// Type aliases
using BRepGraph_ShellRefId  = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Shell>;
using BRepGraph_FaceRefId   = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Face>;
using BRepGraph_WireRefId   = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Wire>;
using BRepGraph_CoEdgeRefId = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::CoEdge>;
using BRepGraph_VertexRefId = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Vertex>;
using BRepGraph_SolidRefId  = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Solid>;
using BRepGraph_ChildRefId  = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::Child>;
```

Each ref kind gets its own storage in a new `RefStore<T>` pattern (analogous to `DefStore<T>` and `RepStore<T>`):

```cpp
template <typename RefT>
struct RefStore
{
  NCollection_Vector<RefT>        Refs;
  NCollection_Vector<BRepGraph_RefUID> UIDs;  // parallel UID vector
  int                             NbActive = 0;
};
```

Reference structs (stored in RefStore, not inline in parent).
Unified naming — same names as current inline refs, parent is a generic `NodeId` field:

```cpp
namespace BRepGraphInc
{

struct BaseRef
{
  BRepGraph_RefId  RefId;        // Typed address (RefKind + index)
  BRepGraph_NodeId ParentId;     // Generic parent entity id
  uint32_t         MutationGen = 0;
  bool             IsRemoved   = false;
};

struct ShellUsage : public BaseRef
{
  BRepGraph_ShellId  ShellDefId;
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};

struct FaceUsage : public BaseRef
{
  BRepGraph_FaceId   FaceDefId;
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};

struct WireUsage : public BaseRef
{
  BRepGraph_WireId   WireDefId;
  bool               IsOuter;
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};

struct CoEdgeUsage : public BaseRef
{
  BRepGraph_CoEdgeId CoEdgeDefId;
  TopLoc_Location    LocalLocation;
};

struct VertexUsage : public BaseRef
{
  BRepGraph_VertexId VertexDefId;
  TopAbs_Orientation Orientation;     // FORWARD/REVERSED/INTERNAL/EXTERNAL
  TopLoc_Location    LocalLocation;
};

struct SolidUsage : public BaseRef
{
  BRepGraph_SolidId  SolidDefId;
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};

struct ChildUsage : public BaseRef
{
  BRepGraph_NodeId   ChildDefId;      // any-kind child
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};
}
```

Parent entities store typed RefIds instead of inline Ref structs:

```cpp
struct ShellDef : public BaseDef
{
  bool IsClosed;
  NCollection_Vector<BRepGraph_FaceRefId> FaceRefIds;  // was FaceUsage inline
  NCollection_Vector<BRepGraph_ChildRefId> FreeChildRefIds;
};
```

#### New UID Type for References

```cpp
struct BRepGraph_RefUID
{
  size_t              myCounter;
  BRepGraph_RefId::Kind myRefKind;
  uint32_t            myGeneration;

  // Same API as BRepGraph_UID: IsValid(), Kind(), Counter(), Generation()
  // operator== based on (RefKind, Counter) only
};
```

Or alternatively, reuse `BRepGraph_UID` by extending `NodeId::Kind` with ref kinds (but this violates the "new type of ID" requirement).

#### New View: RefsView

```cpp
class BRepGraph::RefsView
{
public:
  // Access ref by typed id (same names as entity views)
  [[nodiscard]] const BRepGraphInc::FaceUsage& Face(BRepGraph_FaceRefId) const;
  [[nodiscard]] const BRepGraphInc::WireUsage& Wire(BRepGraph_WireRefId) const;
  [[nodiscard]] const BRepGraphInc::ShellUsage& Shell(BRepGraph_ShellRefId) const;
  [[nodiscard]] const BRepGraphInc::VertexUsage& Vertex(BRepGraph_VertexRefId) const;
  // ... etc

  // Counts
  [[nodiscard]] int NbFaceRefs() const;
  [[nodiscard]] int NbWireRefs() const;
  [[nodiscard]] int NbVertexRefs() const;
  // ... etc

  // UID operations
  [[nodiscard]] BRepGraph_RefUID UIDOf(BRepGraph_RefId) const;
  [[nodiscard]] BRepGraph_RefId RefIdFrom(const BRepGraph_RefUID&) const;

  // VersionStamp
  [[nodiscard]] BRepGraph_VersionStamp StampOf(BRepGraph_RefId) const;
  [[nodiscard]] bool IsStale(const BRepGraph_VersionStamp&) const;

  // Navigation: refs by parent entity
  [[nodiscard]] NCollection_Vector<BRepGraph_FaceRefId> FaceRefsOf(BRepGraph_ShellId) const;
  [[nodiscard]] NCollection_Vector<BRepGraph_WireRefId> WireRefsOf(BRepGraph_FaceId) const;
  [[nodiscard]] NCollection_Vector<BRepGraph_VertexRefId> VertexRefsOf(BRepGraph_EdgeId) const;
  // ... etc
};
```

**Pros**:
- **Clean separation**: RefId is a distinct type from NodeId. Compile-time safety prevents mixing entity IDs with ref IDs.
- **Parallel UID vectors**: Same proven pattern as DefStore. UID lifecycle is clear.
- **Per-ref MutationGen**: BaseRef has its own MutationGen. Granular tracking of ref changes.
- **VersionStamp compatible**: Stamps work for refs just like for entities.
- **Lightweight BaseRef**: Only `RefId + MutationGen + IsRemoved` (~12 bytes). Much lighter than full BaseDef (~48 bytes) because refs don't need Cache.
- **Back-pointers**: Each ref knows its parent, enabling upward navigation without reverse indices.
- **Storage locality**: Refs of the same kind stored contiguously. Good cache behavior for bulk operations.
- **Natural API**: `graph.Refs().FaceInShell(id)` reads clearly. Separate from entity access.
- **Serialization clean**: Parallel UID vectors serialize the same way as entity UIDs.
- **Follows three-pillar pattern**: NodeId/RepId/RefId — entities/representations/references.

**Cons**:
- **New infrastructure**: Need new `RefStore<T>` template, new `RefId` type with `Typed<Kind>`, new `RefUID` type (or reuse existing UID), new `RefsView`, new builder methods.
- **Two levels of indirection**: Shell → FaceInShellRefId → FaceInShellRef → FaceDefId → FaceDef. Same as Option B.
- **Separate MutationGen semantics**: When a ref changes, does the parent entity's MutationGen also increment? Need to define propagation rules.
- **CoEdgeUsage/OccurrenceUsage overlap**: CoEdge and Occurrence are already entities with UIDs. Adding CoEdgeInWireRef and OccurrenceInProductRef creates a "ref of a promoted ref" — need to clarify identity semantics (the ref's UID identifies the usage in a wire, the CoEdge's UID identifies the CoEdge entity itself).
- **Ref reverse indices**: May need a separate reverse index system for refs (parent→ref, child→ref lookups). Could share with existing ReverseIndex or require new infrastructure.
- **Code volume**: New files: RefId.hxx, RefUID.hxx, BaseRef in Entity.hxx, RefStore in Storage, RefsView.hxx/.cxx, builder methods. ~8-10 new/modified files.

**Verdict**: Architecturally the cleanest option. Follows the established three-pillar pattern. More upfront work but pays off in long-term clarity and maintainability.

---

### Option D: Composite Identity (Computed, No Storage)

Reference identity derived from (ParentUID, ChildUID, role):

```cpp
struct BRepGraph_RefIdentity
{
  BRepGraph_UID ParentUID;   // e.g., Shell's UID
  BRepGraph_UID ChildUID;    // e.g., Face's UID
  uint8_t       Role;        // enum: ShellInSolid, FaceInShell, etc.

  // Equality = (ParentUID, ChildUID, Role)
  // HashValue = hash(ParentUID, ChildUID, Role)
};
```

No storage changes. Identity computed on demand from the current graph state.

**Pros**:
- **Zero storage overhead**: No new vectors, no new fields
- **No UID allocation**: Identity is deterministic from context
- **Simple implementation**: Just a struct with hash/equality
- **Natural for STEP mapping**: STEP entity IDs can map to (parent#, child#, relationship)

**Cons**:
- **Not truly persistent**: If the parent or child is compacted/reindexed, the composite identity changes (since it depends on entity UIDs which use Counter, this is actually stable)
- **Ambiguous with shared entities**: If the same face appears twice in the same shell (rare but possible with compounds), the composite identity is not unique. Need an index or ordinal disambiguator.
- **No MutationGen**: Cannot detect "ref orientation changed" independently — only entity-level MutationGen exists.
- **No VersionStamp**: Cannot stamp a ref. Can only stamp the parent entity.
- **No serialization anchor**: The composite identity is implicit. Serializing requires storing (ParentUID, ChildUID, Role) tuples explicitly.
- **Not a true ID**: There's no "ref index" to look up. Every query requires scanning parent's ref vector.
- **Cannot carry attributes**: No label/handle to attach per-ref metadata (color, name, etc.)

**Verdict**: Minimal effort but doesn't satisfy the core requirements (per-ref mutation tracking, serialization, attribute attachment). Only suitable as a short-term bridge.

---

### Option E: Hybrid — RefId for Heavy Refs, Composite for Light Refs

Promote only refs that carry significant contextual data or that external systems treat as separate entities. Use composite identity for trivially thin refs.

**Promote to RefId table** (heavy / externally significant):
- `FaceInShell` — STEP `ADVANCED_FACE`, CATIA persistent naming level, attribute carrier (color/material)
- `WireOnFace` — STEP `FACE_BOUND`/`FACE_OUTER_BOUND`, carries IsOuter flag
- `CoEdgeInWire` — already covered by CoEdge entity, but the wire-link itself may need identity

**Keep as composite identity** (thin / rarely addressed externally):
- `ShellInSolid` — typically 1:1 relationship, rarely addressed independently
- `VertexOnEdge` — addressed via edge, rarely needs own identity
- `VertexOnFace` — rare (only INTERNAL/EXTERNAL vertices on faces)
- `SolidInCompSolid` — rare topology, typically 1:1
- `ChildInCompound` — heterogeneous, complex to type

**Pros**:
- Less infrastructure than full Option C
- Focuses effort where external systems need it most
- Lighter for common cases

**Cons**:
- **Inconsistent**: Some refs have IDs, others don't. API is non-uniform.
- **Judgment calls**: Which refs are "heavy" enough? Debatable and may change as requirements evolve.
- **Migration risk**: If a "light" ref later needs promotion, it's a breaking change.
- **STEP mapping incomplete**: Even `CLOSED_SHELL` (shell-in-solid) can have attributes in AP242.

**Verdict**: Pragmatic compromise but creates long-term architectural debt. If the goal is full external system compatibility, this falls short.

---

## 4. Comparison Matrix

| Criterion | A: UID on Ref | B: Promote to Entity | C: RefId Table | D: Composite | E: Hybrid |
|-----------|:---:|:---:|:---:|:---:|:---:|
| Compile-time safety (ID types) | - | + | ++ | + | + |
| Per-ref MutationGen | +/- | ++ | ++ | - | +/- |
| VersionStamp support | +/- | ++ | ++ | - | +/- |
| Storage efficiency | ++ | - | + | ++ | + |
| Cache locality | ++ | + | + | ++ | + |
| UID lifecycle clarity | - | ++ | ++ | N/A | + |
| Serialization | - | ++ | ++ | - | + |
| Reverse lookup | - | + | ++ | - | + |
| External system mapping (STEP/CATIA) | + | ++ | ++ | +/- | + |
| XCAF/XBF compatibility | + | ++ | ++ | - | + |
| Existing code disruption | + | -- | - | ++ | +/- |
| API surface simplicity | + | - | +/- | ++ | - |
| Conceptual clarity | - | +/- | ++ | + | - |
| Long-term extensibility | - | + | ++ | - | - |
| Implementation effort | ++ (low) | -- (high) | - (medium) | ++ (minimal) | +/- |

**Legend**: `++` excellent, `+` good, `+/-` mixed, `-` weak, `--` poor

---

## 5. Detailed Analysis: Option C (RefId Table)

This section deep-dives into Option C since it aligns with the user's proposal and scores highest on the comparison matrix.

### 5.1 RefId Type Design

```cpp
// File: BRepGraph_RefId.hxx

struct BRepGraph_RefId
{
  enum class Kind : int
  {
    ShellInSolid     = 0,
    FaceInShell      = 1,
    WireOnFace       = 2,
    CoEdgeInWire     = 3,
    VertexOnEdge     = 4,
    VertexOnFace     = 5,
    SolidInCompSolid = 6,
    ChildInCompound  = 7,
  };

  Kind RefKind;
  int  Index;

  // Typed<Kind> wrapper (identical pattern to NodeId::Typed)
  template <Kind TheKind>
  struct Typed { int Index; /* same API as NodeId::Typed */ };

  // Static helpers
  static bool IsTopologyRef(Kind k) { return static_cast<int>(k) <= 7; }
};

// Type aliases
using BRepGraph_ShellInSolidRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::ShellInSolid>;
using BRepGraph_FaceInShellRefId      = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::FaceInShell>;
using BRepGraph_WireOnFaceRefId       = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::WireOnFace>;
using BRepGraph_CoEdgeInWireRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::CoEdgeInWire>;
using BRepGraph_VertexOnEdgeRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::VertexOnEdge>;
using BRepGraph_VertexOnFaceRefId     = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::VertexOnFace>;
using BRepGraph_SolidInCompSolidRefId = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::SolidInCompSolid>;
using BRepGraph_ChildInCompoundRefId  = BRepGraph_RefId::Typed<BRepGraph_RefId::Kind::ChildInCompound>;
```

### 5.2 UID Decision: Reuse BRepGraph_UID or New BRepGraph_RefUID?

**Option C.1: Reuse BRepGraph_UID**

Extend `BRepGraph_UID` to work with both NodeId::Kind and RefId::Kind:

```cpp
struct BRepGraph_UID
{
  size_t   myCounter;
  int      myKindValue;     // holds either NodeId::Kind or RefId::Kind
  bool     myIsRef;         // discriminator: entity UID vs ref UID
  uint32_t myGeneration;
};
```

Pros: Single UID counter, single VersionStamp type works for both.
Cons: Mixed semantics. `myIsRef` flag is fragile. Breaks existing UID API.

**Option C.2: New BRepGraph_RefUID type**

```cpp
struct BRepGraph_RefUID
{
  size_t                myCounter;
  BRepGraph_RefId::Kind myRefKind;
  uint32_t              myGeneration;
  // Same API as BRepGraph_UID
};
```

Pros: Clean separation. No changes to existing BRepGraph_UID.
Cons: Two UID types. VersionStamp needs to work with both. Two counters to manage.

**Option C.3: Unified counter, separate types**

Both BRepGraph_UID and BRepGraph_RefUID share a single atomic counter (`myNextUIDCounter`), but are separate types:

```cpp
// Both types use the same global counter
// Entity UID counter: 1, 3, 5, 7, ...
// Ref UID counter: 2, 4, 6, 8, ...
// OR: single counter, both increment it (interleaved)
```

Pros: Global uniqueness across entities and refs. Single counter simplifies serialization.
Cons: Slightly more complex allocation.

**Recommendation**: Option C.3 — single counter guarantees global uniqueness. The types remain separate for compile-time safety, but the counter space is shared so no UID can ever collide between an entity and a ref.

### 5.3 BaseRef Design

```cpp
struct BaseRef
{
  BRepGraph_RefId RefId;         // Typed address (RefKind + index)
  uint32_t        MutationGen = 0;  // Per-ref mutation counter
  bool            IsRemoved   = false;  // Soft-removal flag
};
```

**No Cache field**: Unlike BaseDef, refs don't need lazily-computed cached data. They're too thin for that overhead.

**No IsModified flag**: MutationGen is sufficient. IsModified is used for Build()-cycle tracking on entities; refs don't participate in Build() cycles the same way.

BaseRef is ~12-16 bytes vs BaseDef's ~48 bytes — significantly lighter.

### 5.4 Storage Pattern

```cpp
// In BRepGraphInc_Storage:
template <typename RefT>
struct RefStore
{
  NCollection_Vector<RefT>             Refs;
  NCollection_Vector<BRepGraph_RefUID> UIDs;  // parallel vector
  int                                  NbActive = 0;

  RefStore() = default;
  RefStore(int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
    : Refs(theBlockSize, theAlloc), UIDs(theBlockSize, theAlloc) {}

  int Nb() const { return Refs.Length(); }
  const RefT& Get(int theIdx) const { return Refs.Value(theIdx); }
  RefT& Change(int theIdx) { return Refs.ChangeValue(theIdx); }
  RefT& Append(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    ++NbActive;
    return Refs.Appended();
  }
  void Clear() { Refs.Clear(); UIDs.Clear(); NbActive = 0; }
};
```

### 5.5 Parent Entity Changes

```cpp
// BEFORE (current):
struct ShellDef : public BaseDef
{
  bool IsClosed;
  NCollection_Vector<FaceUsage> FaceRefs;
  NCollection_Vector<ChildUsage> FreeChildRefs;
};

// AFTER (with RefId):
struct ShellDef : public BaseDef
{
  bool IsClosed;
  NCollection_Vector<BRepGraph_FaceInShellRefId> FaceRefIds;  // indices into RefStore
  NCollection_Vector<ChildUsage> FreeChildRefs;  // or also promoted
};
```

### 5.6 Traversal Impact

```cpp
// BEFORE (inline refs):
const ShellDef& shell = storage.Shells().Entry(shellId);
for (const FaceUsage& faceRef : shell.FaceRefs)
{
  const FaceDef& face = storage.Faces().Entry(faceRef.FaceDefId);
  TopAbs_Orientation orient = faceRef.Orientation;
}

// AFTER (RefId table — same ref name, just looked up by RefId):
const ShellDef& shell = storage.Shells().Entry(shellId);
for (const BRepGraph_FaceRefId& refId : shell.FaceRefIds)
{
  const FaceUsage& faceRef = graph.Refs().Faces().Entry(refId);    // same FaceUsage name
  const FaceDef& face = storage.Faces().Entry(faceRef.FaceDefId);
  TopAbs_Orientation orient = faceRef.Orientation;
}
```

One extra indirection, but enables per-ref identity and mutation tracking.

### 5.7 CoEdge and Occurrence: Overlap Resolution

**Problem**: CoEdge is already an entity (Kind=8) that represents edge-in-face usage. Adding `CoEdgeInWire` ref creates a second layer: Wire → CoEdgeInWireRef → CoEdge entity → Edge entity.

**Resolution strategies**:

1. **Keep CoEdgeInWire ref**: CoEdge's UID identifies the PCurve ownership. CoEdgeInWireRef's UID identifies the wire-to-coedge link. Both are valid identity levels. A CoEdge can appear in multiple wires (shared loops), so the wire-link needs its own identity.

2. **Skip CoEdgeInWire ref**: CoEdgeUsage is already minimal (just CoEdgeDefId + LocalLocation). If CoEdge always appears in exactly one wire (typical), the ref doesn't need its own identity — CoEdge's UID suffices. Only add CoEdgeInWire if shared-loop scenarios arise.

3. **Same for OccurrenceUsage**: Occurrence entity already has a UID. OccurrenceUsage is just `{OccurrenceDefId}`. Skip OccurrenceInProduct ref unless occurrences can be shared across products (currently they cannot — each occurrence has exactly one parent product).

**Recommendation**: Start without CoEdgeInWire and OccurrenceInProduct ref kinds. Add them later only if multi-parent scenarios emerge. This keeps the initial scope focused.

### 5.8 VersionStamp Extension

Two options for VersionStamp with refs:

**A. Extend existing BRepGraph_VersionStamp to hold either UID or RefUID**:
```cpp
struct BRepGraph_VersionStamp
{
  BRepGraph_UID    myUID;          // entity UID (invalid if ref stamp)
  BRepGraph_RefUID myRefUID;       // ref UID (invalid if entity stamp)
  uint32_t         myMutationGen;
  uint32_t         myGeneration;

  bool IsEntityStamp() const { return myUID.IsValid(); }
  bool IsRefStamp() const { return myRefUID.IsValid(); }
};
```

**B. Separate BRepGraph_RefVersionStamp type**:
```cpp
struct BRepGraph_RefVersionStamp
{
  BRepGraph_RefUID myRefUID;
  uint32_t         myMutationGen;
  uint32_t         myGeneration;
  // Same API as BRepGraph_VersionStamp
};
```

**C. Unified via variant or tagged union**:
```cpp
struct BRepGraph_VersionStamp
{
  // discriminated union over entity/ref
  enum class Domain { Entity, Ref };
  Domain   myDomain;
  size_t   myCounter;      // from UID or RefUID
  int      myKindValue;    // NodeId::Kind or RefId::Kind
  uint32_t myMutationGen;
  uint32_t myGeneration;
};
```

**Recommendation**: Option C (unified) keeps a single stamp type that works for both entities and refs. The `Domain` discriminator is explicit. ToGUID works identically.

### 5.9 Mutation Propagation Rules

When a ref changes (e.g., FaceInShellRef orientation flipped):
1. Ref's own `MutationGen` increments
2. Should the parent entity (Shell) also be marked modified?

**Yes — bubble up**: Parent entity's MutationGen increments too. This ensures that consumers watching the parent see changes. The ref change is a mutation of the parent's structure.

**Rationale**: A shell whose face was reoriented is structurally different. Algorithms that depend on shell orientation (e.g., solid volume computation) need to know.

**Implementation**: `BuilderView::MutRef<FaceInShellRef>()` destructor calls `markModified()` on both the ref and its parent entity.

### 5.10 Build() Flow Changes

During `BRepGraphInc_Populate` (called by `Build()`):

```
// Current flow:
1. Create entities (Vertex, Edge, ..., Shell, Solid)
2. Create inline refs (ShellUsage, FaceUsage, WireUsage, VertexUsage) inside parent entities
3. Allocate UIDs for entities
4. Build reverse indices

// New flow with RefId:
1. Create entities (same)
2. Create ref entries in RefStore tables
3. Store RefIds on parent entities (instead of inline refs)
4. Allocate UIDs for entities AND refs (shared counter)
5. Build reverse indices (extended to cover ref-to-parent and ref-to-child)
```

---

## 6. Storage Overhead Estimates

### 6.1 Per-Element Overhead (Option C)

| Component | Size | Notes |
|-----------|------|-------|
| BaseRef | ~16 bytes | RefId(8) + MutationGen(4) + IsRemoved(1) + padding(3) |
| FaceInShellRef | ~16 + 12 + padding = ~32 bytes | + FaceDefId(4) + ParentShellId(4) + Orientation(4) + Location(handle ~8) |
| Parallel RefUID | ~24 bytes | Counter(8) + Kind(4) + Generation(4) + padding |
| Total per ref | ~56 bytes | BaseRef + data + UID |

### 6.2 Model Size Estimates

| Model | Faces | Edges | Additional Refs (approx.) | Overhead |
|-------|-------|-------|--------------------------|----------|
| Simple box | 6 | 12 | 1 ShellInSolid + 6 FaceInShell + 6 WireOnFace + ~16 VertexOnEdge | ~1.6 KB |
| Medium part | 1K | 3K | ~1K FaceInShell + ~1K WireOnFace + ~6K VertexOnEdge | ~450 KB |
| Complex automotive | 100K | 300K | ~100K FaceInShell + ~100K WireOnFace + ~600K VertexOnEdge | ~45 MB |
| Large assembly | 1M | 3M | ~1M FaceInShell + ~1M WireOnFace + ~6M VertexOnEdge | ~450 MB |

**Note**: VertexOnEdge refs dominate because each edge has 2 boundary vertices (start + end). For large models, consider deferring VertexOnEdge refs (they're rarely addressed independently in external systems).

### 6.3 Optimization: Deferred/Optional Ref Kinds

Not all ref kinds need to be populated at Build() time. Consider a tiered approach:

**Always populated**:
- `FaceInShell` — most externally important, attribute carrier
- `WireOnFace` — STEP `FACE_BOUND` mapping

**Populated on demand**:
- `ShellInSolid` — often 1:1, rarely addressed
- `VertexOnEdge` — high cardinality, rarely needs own identity
- `VertexOnFace` — rare (only INTERNAL/EXTERNAL vertices)
- `SolidInCompSolid` — rare topology
- `ChildInCompound` — heterogeneous, complex

This reduces default overhead to ~12 bytes × (NbFaces + NbWires) ≈ ~2.2 MB for 100K faces.

---

## 7. Migration Path

### Phase 1: Infrastructure (non-breaking)
1. Add `BRepGraph_RefId.hxx` with Kind enum and Typed<> template
2. Add `BaseRef` to entity header
3. Add `RefStore<T>` to storage
4. Add ref entity structs (FaceInShellRef, WireOnFaceRef, etc.)
5. Add `BRepGraph::RefsView` (read-only)
6. Extend BuilderView with ref mutation API
7. Tests

### Phase 2: Populate refs during Build() (non-breaking)
1. Extend `BRepGraphInc_Populate` to create ref entries in parallel with inline refs
2. Parent entities keep inline refs (backward compatible)
3. RefStore is populated additionally — dual storage temporarily
4. Tests verify ref data matches inline ref data

### Phase 3: Migrate to RefId storage (breaking)
1. Replace inline ref vectors with RefId vectors on parent entities
2. Update all traversal code to use ref tables
3. Remove old inline ref structs
4. Update Reconstruct (round-trip) to use ref tables
5. Full test suite

### Phase 4: External mapping API
1. Add STEP entity ID → RefUID mapping
2. Add XCAF label → RefUID mapping
3. Add serialization support (RefUID in XBF)

---

## 8. Interaction with Existing Systems

### 8.1 Reverse Index

Current `BRepGraphInc_ReverseIndex` maps child→parent for entities:
- `EdgesOfVertex()`, `WiresOfEdge()`, `FacesOfEdge()`, etc.

With RefId, reverse indices can be enriched:
- `FaceRefsOfShell(ShellId)` → which FaceInShellRefIds belong to this shell
- `ShellRefsOfFace(FaceId)` → which FaceInShellRefIds reference this face
- Enables O(1) "which shells contain this face?" queries WITH per-usage context

### 8.2 History Tracking

Current `BRepGraph_HistoryData` tracks entity-level modifications:
```cpp
NCollection_DataMap<BRepGraph_NodeId, NCollection_Vector<BRepGraph_NodeId>> myModified;
```

With RefId, history can also track ref-level changes:
- "FaceInShellRef #42 was deleted" (face removed from shell)
- "FaceInShellRef #42 orientation was flipped"
- "FaceInShellRef #42 was replaced by FaceInShellRef #43" (face substitution)

### 8.3 Named Layers / Attributes

Current transient cache system attaches cache values to `NodeId`:
```cpp
aGraph.Cache().Set(NodeId, theColorKind, theColorValue);
```

With `RefId`, a future persistent attribute system could also attach data to refs,
for example per-face-in-shell color/material/name in usage context rather than only
per-entity transient cache values.

This is critical for XCAF where face color/material/name are per-usage-context, not per-entity.

---

## 9. Open Questions

1. **Should CoEdgeInWire and OccurrenceInProduct be ref kinds, or are CoEdge/Occurrence entity UIDs sufficient?**
   - CoEdge appears in exactly one wire currently. If shared loops emerge, CoEdgeInWire becomes necessary.
   - Occurrence has exactly one parent product. OccurrenceInProduct ref seems redundant.

2. **Should RefUID share the global counter with entity UID, or have a separate counter?**
   - Shared counter: global uniqueness, simpler serialization.
   - Separate counter: smaller ref counter values, no counter exhaustion interference.

3. **Should VertexOnEdge refs be populated by default, or on-demand?**
   - High cardinality (~2x edges). Not commonly addressed by external systems.
   - Could be populated lazily when first requested.

4. **Should VersionStamp be unified (single type for entities and refs) or separate types?**

5. **Should `BaseRef` include a `BRepGraph_NodeCache` field for caching, or keep it minimal?**
   - Refs are thin. Cache would add ~16-32 bytes per ref.
   - Could be optional (null pointer to shared cache block).

6. **Mutation propagation: when a ref changes, should the parent entity's MutationGen also increment?**
   - Proposed: yes, bubble up.
   - Alternative: ref changes are independent; consumers watch refs directly.

---

## 10. Recommendation

**Option C (RefId Table)** is the strongest design for long-term CATIA/ACIS/STEP/XCAF compatibility. It:

- Creates a clean third pillar (NodeId / RepId / RefId) with consistent patterns
- Gives every reference persistent identity, mutation tracking, and version stamping
- Maps naturally to external systems (STEP entities, CATIA persistent names, XCAF labels)
- Has manageable storage overhead (can be tiered for large models)
- Follows the proven DefStore/RepStore pattern
- Can be implemented incrementally without breaking existing code

**Implementation order**: Phase 1 (infrastructure) → Phase 2 (dual storage) → Phase 3 (migration) → Phase 4 (external mapping).

**Start with**: FaceInShell and WireOnFace ref kinds (most externally significant). Add others as needed.

---

## 11. Deep Analysis: Reference Sharing

### 11.1 What Is Reference Sharing?

Reference sharing means multiple parent entities point to the **same RefId** instead of each having their own ref entry. The ref entry in the RefStore table is shared — it exists once but is referenced from multiple places.

```
Without sharing (1:1):
  Shell_A → FaceInShellRef #1 (FaceId=5, Orient=FWD, Loc=L1)
  Shell_B → FaceInShellRef #2 (FaceId=5, Orient=FWD, Loc=L1)  ← duplicate data

With sharing (N:1):
  Shell_A ──┐
             ├──→ FaceInShellRef #1 (FaceId=5, Orient=FWD, Loc=L1)
  Shell_B ──┘
```

### 11.2 Where Sharing Occurs in BRep Topology

**Topology-level sharing in TopoDS/BRep**:

| Relationship | Shared? | How? |
|---|---|---|
| Solid → Shell | Rarely. Each solid typically has its own shell(s). Shared shells would mean two solids share a boundary — geometrically unusual. |
| Shell → Face | **Yes, common**. Two shells can share a face (e.g., internal face between two cavities in a solid, or between adjacent solids in a CompSolid). The face entity is shared; each shell references it with potentially different orientation. |
| Face → Wire | Rarely. Shared wires would mean two faces have identical boundary loops — possible with identical geometry but unusual in practice. |
| Wire → CoEdge | **No**. Each CoEdge is face-specific (owns its own PCurve). Two wires on different faces use different CoEdge entities even if they reference the same edge. |
| Edge → Vertex | **Yes, very common**. Multiple edges share the same vertex at a junction point. Each edge references the vertex with its own orientation and location. |
| Compound → Child | **Yes**. Same sub-shape can appear in multiple compounds. |
| Product → Occurrence | **No**. Each occurrence has exactly one parent product (DAG tree, not DAG mesh at the occurrence level). |

**Key insight**: Entity sharing (same FaceId in multiple shells) is common. But the **reference context** (orientation, location) is typically different per parent, making the **reference itself** non-sharable in most cases.

### 11.3 When Does Ref Sharing Actually Help?

#### Case 1: Identical Usage Context (Rare for Topology)

For a shared ref to work, two parents must reference the same child with **identical** orientation AND location:
- Same FaceId
- Same Orientation (both FORWARD, or both REVERSED)
- Same TopLoc_Location (same transformation)

In BRep topology this is **rare** because:
- Shared faces between shells typically have **opposite** orientations (one shell sees the face as FORWARD, the other as REVERSED)
- Shared vertices on edges have different orientations (FORWARD for start vertex, REVERSED for end vertex)
- Locations often differ per-context

**Example where sharing fails**:
```
Shell_A → Face_5 (FORWARD)   ← different orientation
Shell_B → Face_5 (REVERSED)  ← cannot share the ref
```

**Example where sharing works**:
```
Compound_A → Solid_7 (FORWARD, Identity location)
Compound_B → Solid_7 (FORWARD, Identity location)  ← identical context, can share
```

Compound → Child refs with identity location are a realistic sharing case.

#### Case 2: Assembly-Level Sharing (Product/Occurrence)

In assemblies, the same part (Product) is instanced multiple times. Each instance (Occurrence) has a different placement. Currently:
- Product entity is shared (by ID)
- Occurrence entities are distinct (each has its own placement)
- OccurrenceUsage is just `{OccurrenceDefId}` — trivially thin, nothing to share

Assembly sharing is already handled at the **entity level** (shared Product), not at the ref level.

#### Case 3: Location Deduplication

If many refs share the same `TopLoc_Location`, sharing refs deduplicates the location data. But `TopLoc_Location` already uses handle-based sharing internally (location is a `Handle(TopLoc_Datum3D)` chain). So the actual data isn't duplicated even in non-shared refs — only the handle bookkeeping differs.

**Bottom line**: `TopLoc_Location` is already efficiently shared via handles. Ref-level sharing doesn't save significant memory here.

### 11.4 Sharing: Complexity Costs

If refs can be shared (N:1 parent-to-ref), the following complexities arise:

1. **Reference counting or parent list**: Need to track how many parents reference each ref. Otherwise, removing a ref when one parent drops it would break other parents.

2. **Mutation semantics**: If Shell_A and Shell_B share FaceInShellRef #1, and Shell_A wants to change the orientation — does it:
   - Mutate the shared ref (affects Shell_B too)? **Copy-on-write needed**.
   - Create a new ref and update Shell_A's pointer? This is **unshare on mutation**.

3. **MutationGen ambiguity**: If the shared ref's MutationGen increments, both parents appear modified. Is this correct? If Shell_A flipped orientation, Shell_B didn't change — but the shared ref's MutationGen says it did.

4. **Deletion**: If Shell_A is removed, its FaceInShellRef #1 survives (Shell_B still needs it). Need refcount check before removal.

5. **History**: "FaceInShellRef #1 was modified" — which parent's context was modified? Need parent discriminator.

6. **Serialization**: Must serialize ref-to-parent associations (many-to-many), not just ref data.

### 11.5 Sharing: When It Makes Sense

| Context | Sharing Useful? | Why / Why Not |
|---|---|---|
| **Assembly (Product/Occurrence)** | **Already handled** | Products are shared entities. Occurrences are distinct with unique placements. Ref sharing adds nothing. |
| **Shell → Face** | **Unlikely** | Shared faces have opposite orientations. Ref data differs. |
| **Edge → Vertex** | **Unlikely** | Same vertex at start of one edge, end of another — different orientation. |
| **Compound → Child** | **Maybe** | Same child in multiple compounds with same orient/location. Low frequency. |
| **Face → Wire** | **No** | Wires are rarely shared across faces. |
| **Wire → CoEdge** | **No** | CoEdges are face-specific. |
| **CompSolid → Solid** | **Maybe** | Multiple compsolids sharing the same solid. Rare topology. |

### 11.6 Sharing Analysis Summary

**Sharing at the ref level provides limited practical benefit for BRep topology because:**
1. Entity sharing is already handled (same FaceId in multiple shells) — the entity is shared, not the reference
2. Reference context (orientation, location) typically differs per parent, making refs non-sharable
3. Location data is already deduplicated via handle-based sharing in `TopLoc_Location`
4. Sharing introduces significant complexity (refcount, copy-on-write, mutation ambiguity)
5. The main external-system use case (assemblies) is already served by Product/Occurrence entity sharing

**Where sharing DOES make sense:**
- **Compound → ChildUsage**: Same sub-shape in multiple compounds with identical context. Moderate benefit.
- **Multi-body models**: CompSolid containing solids that also appear in compounds. Edge case.
- **Future custom containers**: User-defined grouping where entities appear in multiple groups with same context.

### 11.7 Sharing Design Options

#### S1: No Sharing (1:1 parent-to-ref)
Each parent has its own ref entry. Simplest lifecycle. Duplicate data is minimal (refs are thin; locations are handle-shared internally).

#### S2: Optional Sharing (with refcount)
RefStore tracks reference count. Parents share refs when context is identical. Copy-on-write on mutation. Refcount decremented on parent removal.

```cpp
struct BaseRef
{
  BRepGraph_RefId RefId;
  uint32_t MutationGen = 0;
  bool     IsRemoved   = false;
  int      RefCount    = 1;        // NEW: sharing support
};
```

#### S3: Sharing via Parent List
Instead of refcount, each ref stores a list of parent RefIds:

```cpp
struct BaseRef
{
  BRepGraph_RefId RefId;
  uint32_t MutationGen = 0;
  bool     IsRemoved   = false;
  NCollection_Vector<BRepGraph_NodeId> Parents;  // all parents referencing this ref
};
```

#### S4: Sharing via Indirection Table
A separate table maps (ParentId, ChildId) → RefId. Parents don't store RefIds directly; they look up refs via the indirection table.

```cpp
// Key: (ParentNodeId, ChildNodeId, RefKind)
// Value: RefId
NCollection_DataMap<RefLookupKey, BRepGraph_RefId> myRefIndex;
```

**Comparison**:

| Aspect | S1: No sharing | S2: Refcount | S3: Parent list | S4: Indirection |
|---|---|---|---|---|
| Memory (duplicate refs) | Slightly higher | Lower | Lower | Lower |
| Mutation | Direct | Copy-on-write | Copy-on-write | Copy-on-write |
| Deletion | Direct | Check refcount | Remove from list | Remove from map |
| Complexity | Minimal | Medium | Medium | High |
| Traversal speed | O(1) | O(1) | O(1) | O(1) + map lookup |
| Serialization | Simple | Simple + refcount | Complex (parent lists) | Complex (map) |

---

## 12. Naming Convention Options

### N1: `{Child}In{Parent}` / `{Child}On{Parent}`

```
ShellInSolid, FaceInShell, WireOnFace, CoEdgeInWire,
VertexOnEdge, VertexOnFace, SolidInCompSolid, ChildInCompound
```

**Pros**: Explicit direction. No collision with existing `XxxRef` names. Self-documenting.
**Cons**: Long names. Two prepositions (In/On) — inconsistent. Over-specified: bakes parent type into name, but a VertexUsage can be on an Edge OR a Face. If a FaceUsage could also appear in a Compound, the name `FaceInShell` is misleading.

### N2: `{Child}Use`

```
ShellUse, FaceUse, WireUse, CoEdgeUse,
VertexUse, SolidUse, ChildUse
```

**Pros**: Short. Follows ACIS/Parasolid convention. Single naming pattern.
**Cons**: Doesn't specify parent. `ChildUse` is generic.

### N3: `{Child}Ref` — Unified, Same as Entity Pattern (Recommended)

**Key insight**: Entities are named by what they ARE (`CoEdgeDef`, not `EdgeOnFaceEntity`). References should follow the same principle — named by the CHILD type, not by the parent-child relationship.

```
RefId::Kind enum:  Shell, Face, Wire, CoEdge, Vertex, Solid, Child
Ref structs:       ShellUsage, FaceUsage, WireUsage, CoEdgeUsage, VertexUsage, SolidUsage, ChildUsage
Typed RefIds:      BRepGraph_ShellRefId, BRepGraph_FaceRefId, BRepGraph_WireRefId, ...
```

The parent identity is a **data field** on the ref, not encoded in the type name:

```cpp
struct FaceUsage : public BaseRef
{
  BRepGraph_FaceId   FaceDefId;
  BRepGraph_NodeId   ParentId;      // generic — Shell, Compound, or any parent
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};

struct VertexUsage : public BaseRef
{
  BRepGraph_VertexId VertexDefId;
  BRepGraph_NodeId   ParentId;      // could be Edge or Face — ONE ref kind covers both
  TopAbs_Orientation Orientation;
  TopLoc_Location    LocalLocation;
};
```

**This mirrors how entities work**: `VertexDef` doesn't distinguish "vertex used by edge" vs "vertex used by face". It's just a vertex. The context is in the reference, not the type.

**Pros**:
- Familiar names (same as current inline structs).
- Unified: one `VertexUsage` kind covers all vertex usages (edge boundary, face internal, etc.).
- Parent context is data, not type — flexible for future parent kinds.
- Fewer RefId::Kind values (7 instead of 8+).
- Clean migration: inline `BRepGraphInc::FaceUsage` → table-based `BRepGraphInc::FaceUsage` (same name, richer struct).

**Cons**:
- Collides with current inline struct names during migration. Can be resolved by:
  a) Replacing inline structs entirely (breaking, Phase 3), or
  b) Namespacing: inline refs stay in `BRepGraphInc::`, table refs in a new namespace.
- `ParentId` is generic `BRepGraph_NodeId` instead of typed (e.g., `BRepGraph_ShellId`). Requires runtime Kind check to determine parent type.

### N4: `{Parent}_{Child}Ref`

```
Solid_ShellRef, Shell_FaceRef, Face_WireRef, Wire_CoEdgeRef,
Edge_VertexRef, Face_VertexRef, CompSolid_SolidRef, Compound_ChildRef
```

**Pros**: Unique. Both parent and child in name. Matches BRepGraph naming style.
**Cons**: Very long. Doesn't follow BRepGraph's `Package_ClassName` convention well. Same over-specification problem as N1.

### N5: `BRepGraph_{RefKind}` (prefixed by package)

```
BRepGraph_ShellRef, BRepGraph_FaceRef, BRepGraph_WireRef, ...
```

Full qualified ref-entity names. RefId enum kind is clean:

```cpp
enum class Kind { Shell, Face, Wire, CoEdge, Vertex, Solid, Child };
```

**Pros**: Familiar OCCT naming. Package prefix clear.
**Cons**: Already implied by OCCT convention (structs in `BRepGraphInc::` namespace).

### N6: Topology-oriented names

```
ShellMembership, FaceMembership, WireMembership, CoEdgeMembership, VertexMembership
```

**Pros**: Clear semantic meaning — these represent memberships/containments.
**Cons**: Long. Unusual in BRep terminology.

### N7: Summary and Recommendation

The entity-aligned pattern (N3) is cleanest because:
1. **Entities already set the precedent**: `CoEdge` not `EdgeOnFace`, `Occurrence` not `ProductInstance`
2. **Parent is data, not type**: A `VertexUsage` can be on an Edge or Face — the ParentId field tells you which
3. **Fewer kinds**: No need for `VertexOnEdge` vs `VertexOnFace` — just `Vertex` ref kind
4. **Familiar**: Same names already used in the codebase

RefId::Kind would exactly mirror NodeId::Kind for topology (minus Assembly):

```
NodeId::Kind:  Solid(0), Shell(1), Face(2), Wire(3), Edge(4), Vertex(5),
               Compound(6), CompSolid(7), CoEdge(8), Product(10), Occurrence(11)

RefId::Kind:   Shell(0), Face(1), Wire(2), CoEdge(3), Vertex(4),
               Solid(5), Child(6)
```

---

## 13. CoEdge and Occurrence: Two-Level Identity Analysis

### 13.1 The Two-Level Pattern

CoEdge and Occurrence are unique: they are **already entities** (have BaseDef, UID, storage vector) but are also **referenced** from parent entities via thin inline refs.

```
                Entity Level (has UID)          Reference Level (no UID currently)
                ─────────────────────           ─────────────────────────────────
CoEdge:         CoEdgeDef                    CoEdgeUsage in WireDef
                Kind=8, has PCurve,             {CoEdgeDefId, LocalLocation}
                UV, params, seam pair

Occurrence:     OccurrenceDef                OccurrenceUsage in ProductDef
                Kind=11, has Placement,         {OccurrenceDefId}
                ProductDefId, ParentProductId
```

### 13.2 Does CoEdgeUsage Need a RefId?

**Arguments FOR RefId on CoEdgeUsage**:
1. **Wire membership identity**: The fact that CoEdge #42 is the 3rd element in Wire #7 is a structural relationship that could be tracked.
2. **LocalLocation on ref**: CoEdgeUsage carries a `LocalLocation`. If this changes, it's a ref-level mutation distinct from CoEdge entity mutation.
3. **Shared CoEdges (future)**: If two wires on the same face could share a CoEdge (shared loops/holes), the wire-to-coedge link would need its own identity.
4. **STEP mapping**: `ORIENTED_EDGE` in STEP includes both the edge-use data AND its position in an `EDGE_LOOP`. Having CoEdgeInWire RefId maps to the loop-membership aspect.
5. **Uniformity**: If all other refs get RefIds, omitting CoEdgeUsage is inconsistent.

**Arguments AGAINST RefId on CoEdgeUsage**:
1. **CoEdge entity UID suffices**: In practice, each CoEdge appears in exactly one wire. The CoEdge entity's UID already uniquely identifies the wire membership.
2. **Minimal ref data**: CoEdgeUsage is just `{CoEdgeDefId, LocalLocation}`. The location is almost always identity. The ref carries no semantically significant data beyond the CoEdge itself.
3. **High cardinality**: Number of CoEdgeRefs ≈ Number of CoEdges. Adding RefIds doubles the identity tracking for the same structural information.
4. **No real-world need**: No external system (STEP, CATIA, XCAF) requires a separate ID for "CoEdge's position in a wire" vs "the CoEdge itself."

### 13.3 Does OccurrenceUsage Need a RefId?

**Arguments FOR RefId on OccurrenceUsage**:
1. **Product-to-Occurrence link identity**: The fact that Occurrence #5 is a child of Product #2 is a structural relationship.
2. **Uniformity** with other ref kinds.

**Arguments AGAINST RefId on OccurrenceUsage**:
1. **Occurrence entity UID suffices**: Each Occurrence has exactly one parent Product (stored on `OccurrenceDef::ParentProductDefId`). The Occurrence entity's UID fully identifies the relationship.
2. **OccurrenceUsage is trivially thin**: Just `{OccurrenceDefId}`. No orientation, no location. The ref carries zero contextual data — it's purely a containment pointer.
3. **Assembly identity already complete**: Product UID + Occurrence UID fully identifies any assembly instance. No gap.

### 13.4 Options

| Option | CoEdge Ref | Occurrence Ref | Notes |
|---|---|---|---|
| **T1: Include both** | CoEdgeInWire RefId | OccurrenceInProduct RefId | Full uniformity. Every parent→child link has RefId. |
| **T2: Include CoEdge only** | CoEdgeInWire RefId | No RefId | CoEdgeUsage has LocalLocation worth tracking. OccurrenceUsage is trivially empty. |
| **T3: Exclude both** | No RefId | No RefId | Entity UIDs suffice. Add later if needed. |
| **T4: Include neither now, design for future** | Reserved Kind enum slots | Reserved Kind enum slots | Allocate Kind values but don't implement storage. Forward-compatible. |

---

## 14. UID Counter Strategies

### U1: Single Shared Counter

```cpp
struct BRepGraph_Data {
  std::atomic<size_t> myNextUIDCounter{1};  // shared by entities AND refs
};
```

Entity UID counter=1, ref UID counter=2, entity UID counter=3, etc. Interleaved.

**Pros**: Global uniqueness guaranteed. One counter to serialize/restore. Simple.
**Cons**: Counter values don't indicate entity vs ref. Faster counter growth.

### U2: Separate Counters

```cpp
struct BRepGraph_Data {
  std::atomic<size_t> myNextEntityUIDCounter{1};
  std::atomic<size_t> myNextRefUIDCounter{1};
};
```

**Pros**: Counter values are more compact per-domain. Entity counter doesn't grow due to ref allocations.
**Cons**: Two counters to manage. Need domain discriminator in UID/RefUID type to prevent collisions.

### U3: Partitioned Counter Space

```cpp
// Entities: counter values 1 .. 2^62
// Refs: counter values 2^62+1 .. 2^63
struct BRepGraph_Data {
  std::atomic<size_t> myNextUIDCounter{1};           // entities
  std::atomic<size_t> myNextRefUIDCounter{(1ULL << 62) + 1};  // refs
};
```

**Pros**: Single conceptual space, no collisions. Counter value indicates domain.
**Cons**: Wastes counter space. Fragile boundary. Serialization must preserve partitioning.

### U4: Domain Tag in UID

```cpp
struct BRepGraph_UID {
  enum class Domain : uint8_t { Entity, Ref };
  size_t   myCounter;
  Domain   myDomain;
  // ... Kind (NodeId::Kind or RefId::Kind based on Domain)
};
```

Single counter, but each UID carries a domain tag. No collision possible.

**Pros**: Clean. Single counter. Domain-aware.
**Cons**: Extends UID struct by 1 byte + padding. All existing UID code needs to handle Domain.

---

## 15. VersionStamp Unification Options

### V1: Single Unified Type

```cpp
struct BRepGraph_VersionStamp
{
  enum class Domain : uint8_t { Entity, Ref };
  Domain   myDomain;
  size_t   myCounter;      // from UID
  int      myKindValue;    // NodeId::Kind or RefId::Kind
  uint32_t myMutationGen;
  uint32_t myGeneration;
};
```

**Pros**: One stamp type for everything. ToGUID, IsStale, IsSameNode work uniformly.
**Cons**: Slightly larger. Domain discriminator adds complexity to comparison/hash.

### V2: Separate Types

```cpp
struct BRepGraph_VersionStamp     { BRepGraph_UID myUID; uint32_t myMutationGen, myGeneration; };
struct BRepGraph_RefVersionStamp  { BRepGraph_RefUID myRefUID; uint32_t myMutationGen, myGeneration; };
```

**Pros**: Compile-time safety. Cannot mix entity stamp with ref stamp.
**Cons**: Duplicate API. Two stamp types in caching/validation code.

### V3: Template-Based

```cpp
template <typename UIDType>
struct BRepGraph_VersionStampT
{
  UIDType  myUID;
  uint32_t myMutationGen;
  uint32_t myGeneration;
};

using BRepGraph_VersionStamp    = BRepGraph_VersionStampT<BRepGraph_UID>;
using BRepGraph_RefVersionStamp = BRepGraph_VersionStampT<BRepGraph_RefUID>;
```

**Pros**: Single implementation, two types. Best of both worlds.
**Cons**: Template in public API. Some OCCT consumers prefer non-template types.

---

## 16. Mutation Propagation Strategies

When a ref is mutated (e.g., FaceInShellRef orientation changes):

### P1: Bubble Up to Parent Entity

Ref's MutationGen increments AND parent entity's MutationGen increments.

**Pros**: Consumers watching Shell entity see changes to its face refs. Conservative — nothing is missed.
**Cons**: Parent entity appears "modified" even though its own data didn't change. Shell's geometry hasn't changed — only a face's orientation in it changed. May cause false invalidations.

### P2: Ref-Only

Only the ref's MutationGen increments. Parent entity unchanged.

**Pros**: Precise — parent stamp only stale when parent's own data changes. No false invalidations.
**Cons**: Consumers must explicitly watch refs. Missing ref changes is a subtle bug source.

### P3: Configurable (per-operation)

Builder API allows choosing:
```cpp
builder.MutFaceRef(refId, BubbleUp::Yes);  // or BubbleUp::No
```

**Pros**: Maximum flexibility.
**Cons**: API complexity. User must make the right choice each time.

### P4: Bubble Up to Parent + Child Entity

Ref change propagates to BOTH the parent entity AND the child entity.

**Pros**: Both sides see the change. Shell knows "something changed in my face references." Face knows "my usage context changed."
**Cons**: Most aggressive. Many false invalidations. Child entity data didn't actually change.

---

## 17. Ref Kind Scope Options

### K1: All 8 Ref Kinds (Full Coverage)

```
ShellInSolid, FaceInShell, WireOnFace, CoEdgeInWire,
VertexOnEdge, VertexOnFace, SolidInCompSolid, ChildInCompound
```

Total refs for a box: ~1 + 6 + 6 + 24 + 16 + 0 + 0 + 0 = ~53
Total refs for 100K faces: ~1 + 100K + 100K + 300K + 600K + ~0 + ~0 + ~0 ≈ 1.1M

### K2: Core 4 (Most Significant)

```
FaceInShell, WireOnFace, VertexOnEdge, ShellInSolid
```

Skip CoEdgeInWire (entity UID suffices), VertexOnFace (rare), SolidInCompSolid (rare), ChildInCompound (complex).

### K3: External-System Focused (3)

```
FaceInShell, WireOnFace, ShellInSolid
```

Maps to STEP entities. Skip vertex refs (STEP handles vertices via edge entity). Skip CoEdge (already entity).

### K4: Minimal (2)

```
FaceInShell, WireOnFace
```

Only the refs that carry the most external significance. Face-in-shell = STEP/CATIA/XCAF attribute carrier. Wire-on-face = STEP FACE_BOUND. Others added later.

### K5: Tiered (All Defined, Partial Default)

Define all 8 Kind enum values. Only populate FaceInShell + WireOnFace by default during Build(). Others populated on-demand via explicit API call:

```cpp
graph.Builder().PopulateRefs(BRepGraph_RefId::Kind::VertexOnEdge);
```

**Pros**: Full enum for forward compatibility. Lazy population for large models.
**Cons**: Complex lifecycle — some ref kinds may be unpopulated.

---

## 18. BaseRef Weight Options

### W1: Minimal BaseRef (12-16 bytes)

```cpp
struct BaseRef
{
  BRepGraph_RefId RefId;        // 8 bytes (Kind + Index)
  uint32_t        MutationGen;  // 4 bytes
  bool            IsRemoved;    // 1 byte + 3 padding
};
// Total: ~16 bytes
```

No Cache, no IsModified. Lightest possible.

### W2: BaseRef with IsModified (16 bytes, same)

```cpp
struct BaseRef
{
  BRepGraph_RefId RefId;
  uint32_t        MutationGen;
  bool            IsModified;   // Build-cycle tracking
  bool            IsRemoved;
};
```

Adds monotonic modification flag like BaseDef.

### W3: BaseRef with Back-Pointer (24+ bytes)

```cpp
struct BaseRef
{
  BRepGraph_RefId    RefId;
  BRepGraph_NodeId   ParentId;     // back-pointer to parent entity
  uint32_t           MutationGen;
  bool               IsRemoved;
};
```

Back-pointer enables upward navigation without reverse index. But parent is also stored in each specific ref struct (FaceInShellRef.ParentShellId), making it redundant.

### W4: BaseRef with Cache (32+ bytes)

```cpp
struct BaseRef
{
  BRepGraph_RefId    RefId;
  uint32_t           MutationGen;
  bool               IsRemoved;
  BRepGraph_NodeCache Cache;       // lazily-computed data
};
```

Allows caching per-ref derived data (e.g., composed location, oriented surface). Heavy.

---

## 19. External System Mapping Details

### 19.1 STEP AP203/AP214

| BRepGraph Concept | STEP Entity | STEP Fields | RefId Needed? |
|---|---|---|---|
| Solid entity | `MANIFOLD_SOLID_BREP` | name, outer shell | Entity UID |
| ShellInSolid ref | Part of `MANIFOLD_SOLID_BREP` | implicit (outer shell field) | RefId helps track the link |
| Shell entity | `CLOSED_SHELL` / `OPEN_SHELL` | name, list of faces | Entity UID |
| FaceInShell ref | Implicit in `CLOSED_SHELL.cfs_faces` list | position in list | RefId needed for persistent ordering |
| Face entity | `ADVANCED_FACE` | name, bounds, surface, same_sense | Entity UID |
| WireOnFace ref | `FACE_BOUND` / `FACE_OUTER_BOUND` | bound, orientation | **RefId needed** — this IS a STEP entity |
| Wire entity | `EDGE_LOOP` | name, list of oriented_edges | Entity UID |
| CoEdge entity | `ORIENTED_EDGE` | edge_element, orientation | Entity UID |
| Edge entity | `EDGE_CURVE` | edge_start, edge_end, edge_geometry | Entity UID |
| Vertex entity | `VERTEX_POINT` | vertex_geometry | Entity UID |
| VertexOnEdge ref | Fields on `EDGE_CURVE` | edge_start, edge_end | RefId optional (encoded in edge) |

**Critical for STEP**: `FACE_BOUND` / `FACE_OUTER_BOUND` is a first-class STEP entity with its own `#id`. Without WireOnFace RefId, there's no natural anchor for this STEP entity.

### 19.2 STEP AP242 (Persistent IDs)

AP242 adds `item_identified_representation_usage` which can assign external persistent IDs to any `representation_item`. This means EVERY topology element — including face-bounds, oriented-edges — can have an externally-defined UUID.

BRepGraph mapping:
- Entity UID → internal persistent ID for entities
- RefUID → internal persistent ID for references
- ToGUID() → external UUID derivation for both

### 19.3 CATIA V5 Persistent Naming

CATIA identifies topology by **path from root**:

```
Body.1 / Shell.1 / Face.3 / Wire.1 / Edge.2
  ↑        ↑        ↑        ↑        ↑
Solid   ShellInSolid FaceInShell WireOnFace CoEdge/Edge
```

Each path segment needs a stable identifier:
- `Shell.1` → ShellInSolid RefId (or ShellId if only one)
- `Face.3` → FaceInShell RefId (3rd face in this shell)
- `Wire.1` → WireOnFace RefId (1st wire on this face)
- `Edge.2` → CoEdgeInWire RefId (or CoEdge entity UID)

Without RefIds, the path must use ordinal indices (which change on reorder).

### 19.4 XCAF / XBF

XCAF label hierarchy:
```
0:1:1:1       — Shape (Product)
0:1:1:1:1     — Sub-shape: Face 1
0:1:1:1:1:1   — Sub-sub-shape: Wire 1 on Face 1
0:1:1:1:1:2   — Sub-sub-shape: Edge 1 on Face 1
```

Mapping:
- `0:1:1:1` → Product entity UID
- `0:1:1:1:1` → Face entity UID (or FaceInShell RefId for face-in-context identity)
- `0:1:1:1:1:1` → WireOnFace RefId (wire in context of face)

Sub-shape labels carry attributes (color, material). These attributes are per-usage:
- Face color on label `0:1:1:1:1` → attribute on FaceInShell ref
- Not on Face entity itself (which may appear in multiple shells with different colors)

**XBF binary format**: Serializes the TDF_Label tree. Each label gets a sequential integer ID. RefUIDs would be stored as label attributes, enabling round-trip persistence.

---

## 20. Summary of All Open Decisions

| # | Decision | Options | Notes |
|---|---|---|---|
| 1 | Design approach | A (UID on ref), B (promote to entity), **C (RefId table)**, D (composite), E (hybrid) | User leaning toward C |
| 2 | UID counter | U1 (shared), U2 (separate), U3 (partitioned), U4 (domain tag) | User selected shared |
| 3 | Ref kind scope | K1 (all 8), K2 (core 4), K3 (3), K4 (minimal 2), K5 (tiered) | User selected all |
| 4 | Ref sharing | S1 (no sharing), S2 (refcount), S3 (parent list), S4 (indirection) | Under analysis |
| 5 | Naming convention | N1 (ChildInParent), N2 (ChildUse), N3 (ChildUsage replace), N4 (Parent_ChildRef), N5 (BRepGraph_), N6 (Membership) | Open |
| 6 | CoEdge/Occurrence refs | T1 (both), T2 (CoEdge only), T3 (neither), T4 (reserved slots) | Open |
| 7 | VersionStamp | V1 (unified), V2 (separate), V3 (template) | User selected unified |
| 8 | Mutation propagation | P1 (bubble up), P2 (ref-only), P3 (configurable), P4 (up+down) | User selected bubble up |
| 9 | BaseRef weight | W1 (minimal 16B), W2 (+IsModified), W3 (+back-pointer), W4 (+cache) | Open |
| 10 | Population timing | All at Build(), on-demand, tiered default | User selected all at Build() |
| 11 | UID foundation | F1 (unified type), F2 (shared counter, typed), F3 (template), F4 (base+derived) | See section 21 |
| 12 | Rep UID | Add UID vectors to RepStore, or not | See section 22 |
| 13 | Attribute UID | UID for color/PMI/named-layer entries, or not | See section 23 |

---

## 21. TopoDS Original Sharing Model vs BRepGraph

### 21.1 How TopoDS Shares Topology

In the original TopoDS/BRep model, sharing works through a two-level design:

```
TopoDS_Shape (value type, always COPIED):
  ├── Handle(TopoDS_TShape)  ← SHARED (reference-counted entity)
  ├── TopLoc_Location         ← per-instance context
  └── TopAbs_Orientation      ← per-instance context
```

**Entity = `TopoDS_TShape`** (reference-counted via Handle):
- `BRep_TFace` carries: Surface, Triangulations, Tolerance, NaturalRestriction
- `BRep_TEdge` carries: Curves (3D + PCurves), Tolerance, Flags
- `BRep_TVertex` carries: Point, Tolerance
- Children stored as `NCollection_List<TopoDS_Shape>` — a list of value-type COPIES

**Reference = `TopoDS_Shape`** (value type, never shared by pointer):
- Each parent's children list holds a **copy** of `TopoDS_Shape`
- The copy carries its own Location + Orientation (context)
- The underlying TShape handle is the **same pointer** — this IS the sharing

**`TopoDS_Builder::Add()` always copies:**
```cpp
TopoDS_Shape aChild = aComponent;  // VALUE COPY
aTShape->myShapes.Append(aChild);  // Appends copy to list
```

### 21.2 BRepGraph Mirrors This Model Exactly

| TopoDS Concept | BRepGraph Equivalent | Sharing Level |
|---|---|---|
| `TopoDS_TShape` (entity) | `FaceDef`, `EdgeDef`, etc. in DefStore | Entity shared by ID (same FaceId from multiple shells) |
| `TopoDS_Shape` (reference) | `FaceUsage`, `WireUsage`, etc. inline in parent | Reference NOT shared (each parent has own copy) |
| `Handle(TShape)` pointer identity | Entity ID equality (same FaceId) | Same entity, different context |
| `TopLoc_Location` on Shape | `LocalLocation` on Ref | Per-instance context |
| `TopAbs_Orientation` on Shape | `Orientation` on Ref | Per-instance context |
| `Geom_Surface` handle sharing | SurfaceRep dedup (same RepId) | Geometry shared between faces |
| `Poly_Triangulation` NOT shared | TriangulationRep per face | Mesh NOT shared |

**Key parallel**: In both models, the "reference" is a lightweight wrapper (context + entity pointer) that is always copied, never shared. Sharing happens at the entity level, not the reference level.

### 21.3 What TopoDS Tells Us About Ref Sharing

In TopoDS, when the same face appears in two shells:
```
Shell_A's children: [ Shape(TFace_ptr=0x1234, Orient=FWD, Loc=L1),  ... ]
Shell_B's children: [ Shape(TFace_ptr=0x1234, Orient=REV, Loc=L2),  ... ]
                              ^same handle^         ^different context^
```

- The TFace entity is shared (same pointer)
- The TopoDS_Shape wrapper is **NOT** shared — it's a separate copy with different orient/loc
- TopoDS has **zero mechanism** for sharing references (no reference counting on Shape wrappers)

**This design is intentional**: each usage context is independent. Changing Shell_A's face orientation doesn't affect Shell_B.

### 21.4 Implications for BRepGraph RefId Sharing

Since the original TopoDS model intentionally does NOT share references, and BRepGraph faithfully mirrors this design:

1. **Entity sharing works** (same FaceId from multiple ShellDef::FaceRefs) — already implemented
2. **Reference sharing is architecturally absent** in the original model and rarely useful in practice
3. **Orientation almost always differs** between usages of the same entity — making ref sharing impossible in most cases
4. **The main real-world sharing** is at the entity level (shared faces, shared edges, shared vertices)

**However**, with the RefId table design, sharing BECOMES POSSIBLE if desired:
- Multiple parents could point to the same RefId (flat table enables this)
- Would need copy-on-write when context changes
- Main benefit: Compound → ChildUsage where same sub-shape appears in multiple compounds with identity transform

### 21.5 Geometry Sharing in BRep vs BRepGraph

BRep explicitly documents what is shared and what is not:

```cpp
// BRep_TFace.hxx comment:
// "The Surface may be shared by different TFaces but
//  not the Triangulation, because the Triangulation
//  may be modified by the edges."
```

| Geometry | BRep Sharing | BRepGraph Sharing |
|---|---|---|
| `Geom_Surface` | Shared via Handle | Shared via SurfaceRepId (deduped in Populate) |
| `Geom_Curve` (3D) | Shared via Handle | Shared via Curve3DRepId (deduped) |
| `Geom2d_Curve` (PCurve) | Per-edge-face pair | Per-CoEdge (Curve2DRepId, deduped) |
| `Poly_Triangulation` | **NOT shared** | Per-face (TriangulationRepId, NOT deduped) |
| `Poly_Polygon3D` | Per-edge | Per-edge (Polygon3DRepId, deduped) |

---

## 22. UID for Geometry Representations (RepId → RepUID)

### 22.1 Current State

RepStore currently has **NO UID vector** (unlike DefStore):

```cpp
template <typename RepT>
struct RepStore
{
  NCollection_Vector<RepT> Entities;
  int                      NbActive = 0;
  // NO UIDs vector!
};
```

`BaseRep` has `MutationGen` and `IsRemoved` but no UID:
```cpp
struct BaseRep
{
  BRepGraph_RepId Id;           // volatile index
  uint32_t        MutationGen;  // per-rep mutation counter
  bool            IsRemoved;    // soft-removal
  // NO UID!
};
```

### 22.2 Why Reps Might Need UIDs

| Use Case | Description | External System |
|---|---|---|
| **Geometry versioning** | Track "this surface was re-approximated" across compaction | Internal caching |
| **Mesh versioning** | Track triangulation re-meshing per face | Visualization invalidation |
| **STEP round-trip** | `representation_item` in STEP has `#entity_id` | AP203/AP214 |
| **AP242 persistent ID** | Geometry items can have `item_identified_representation_usage` | AP242 |
| **XCAF geometry labels** | Geometry can have TDF_Label with attributes | XDE |
| **Serialization** | Round-trip rep identity across save/load | XBF |
| **Surface sharing tracking** | Know "these 5 faces share the same surface rep" after deserialization | Internal |
| **PCurve identity** | Track specific PCurve across CoEdge mutations | DE migration |

### 22.3 Design: Add UID to RepStore

Minimal change — follow the DefStore pattern:

```cpp
template <typename RepT>
struct RepStore
{
  NCollection_Vector<RepT>        Entities;
  NCollection_Vector<BRepGraph_RepUID> UIDs;  // NEW: parallel UID vector
  int                             NbActive = 0;
};
```

Rep UIDs would use the same global counter as entity UIDs (shared counter approach).

`BRepGraph_RepUID` can either be:
- A separate type with `RepId::Kind`
- Or reuse `BRepGraph_UID` with the Kind extended to cover rep kinds (but this mixes entity and rep kinds)

### 22.4 Rep UID Options

**R1: Add RepUID (separate type, shared counter)**

```cpp
struct BRepGraph_RepUID
{
  size_t             myCounter;
  BRepGraph_RepId::Kind myKind;    // Surface, Curve3D, Curve2D, etc.
  uint32_t           myGeneration;
};
```

Pros: Clean separation. Rep UID cannot be confused with entity UID.
Cons: New type. VersionStamp must handle it.

**R2: Extend BRepGraph_UID to cover rep kinds**

Add rep kinds to `NodeId::Kind` (or make Kind a superset):
```
Kind::Surface = 30, Kind::Curve3D = 31, Kind::Curve2D = 32, ...
```

Pros: Single UID type for everything.
Cons: Mixes entity and rep kinds in one enum. `IsTopologyKind()` must exclude rep kinds.

**R3: No RepUID — reps don't need persistent identity**

Reps are always accessed via their owning entity (Face→SurfaceRepId, CoEdge→Curve2DRepId). The entity's UID + rep kind is sufficient to identify the geometry.

Pros: No new types. Simpler.
Cons: Cannot independently track "this surface was modified." Cannot assign external IDs to geometry directly.

**R4: Composite rep identity (EntityUID + RepKind)**

```cpp
struct BRepGraph_RepIdentity
{
  BRepGraph_UID OwnerUID;       // entity that owns this rep
  BRepGraph_RepId::Kind RepKind;  // what kind of rep
};
```

Pros: No extra storage. Computed from context.
Cons: Shared reps (same Surface used by 5 faces) have ambiguous owner. Which face's UID to use?

### 22.5 Rep Sharing Complicates UID Assignment

**Critical issue**: Geometry reps are **deduped/shared** during Build(). The same `SurfaceRep` can be referenced by multiple faces:

```
Face #0 → SurfaceRepId(0)  ←─┐
Face #1 → SurfaceRepId(0)  ←─┤ same surface (handle-deduped)
Face #2 → SurfaceRepId(0)  ←─┘
Face #3 → SurfaceRepId(1)      different surface
```

If we assign a UID to SurfaceRep(0), it identifies "this specific surface geometry" — shared across 3 faces. This is correct and useful: the UID identifies the geometry, not the face.

But if the surface is mutated, ALL 3 faces are affected. This is the same as entity sharing (shared vertex at junction of edges).

**Rep UID works well with sharing** — the UID identifies the geometry data, and multiple entities referencing it is the expected pattern.

---

## 23. UID for Attributes (Color, PMI, Named Layers)

### 23.1 Current Transient Cache System

Transient cache values are stored via `CacheView` on `BRepGraph_TransientCache`:

```cpp
class CacheView {
  void Set(NodeId theNode, Handle(BRepGraph_CacheKind) theKind, Handle(BRepGraph_CacheValue) theValue);
  Handle(BRepGraph_CacheValue) Get(NodeId theNode, Handle(BRepGraph_CacheKind) theKind) const;
  bool Remove(NodeId theNode, Handle(BRepGraph_CacheKind) theKind);
};
```

- Cache values are keyed by `(NodeId, CacheKind)` — the descriptor identifies the cache kind
- Cache values are attached to **entities only** (`NodeId`) — not to refs or reps
- No persistent identity for the cache value itself
- No mutation tracking on cache values (only on the owning entity)

### 23.2 What Attribute UID Would Enable

| Use Case | Description | External System |
|---|---|---|
| **Color identity** | Track "this color assignment" across modifications | XCAF color labels |
| **PMI identity** | Each PMI annotation has its own persistent ID | STEP AP242 `draughting_model` |
| **Material identity** | Track material assignments | XCAF material labels |
| **Named layer identity** | Track "this layer" across graph rebuilds | STEP AP214 layers |
| **Attribute versioning** | Detect "color was changed on this face" vs "face geometry changed" | Caching |
| **Attribute sharing** | Same color/material applied to multiple entities | XCAF shared attributes |

### 23.3 Types of Attributes That Need Identity

| Attribute Type | Granularity | Sharing | STEP Mapping | XCAF Mapping |
|---|---|---|---|---|
| **Color** | Per-entity or per-ref (face-in-shell can have different color) | Highly shared (many faces = same color) | `STYLED_ITEM` + `COLOUR_RGB` | `XCAFDoc_Color` on label |
| **Material** | Per-entity or per-product | Shared across parts | `MATERIAL_DESIGNATION` | `XCAFDoc_Material` on label |
| **PMI/GD&T** | Per-annotation (linked to faces/edges) | Not shared (each annotation unique) | `draughting_model_item_association` | `XCAFDoc_Dimension/Datum/Tolerance` |
| **Named Layer** | Per-layer (contains multiple entities) | Layer is a group; many entities share same layer | `PRESENTATION_LAYER_ASSIGNMENT` | `XCAFDoc_Layer` on label |
| **Custom Name** | Per-entity | Not shared | `product_definition.name` | `TDataStd_Name` on label |
| **Validation Properties** | Per-entity (volume, area, centroid) | Not shared | `property_definition + representation` | `XCAFDoc_Area/Volume/Centroid` |

### 23.4 Attribute UID Design Options

**A1: No Attribute UID — use owning entity UID**

Attribute identity = (EntityUID, AttrKey). No separate UID for the attribute.

Pros: Simple. Works for per-entity attributes.
Cons: Cannot track shared attributes (same color on 100 faces). Cannot track attribute-only changes.

**A2: UID on Persistent Metadata Value**

If a handle-based persistent metadata design is ever used, add UID to a dedicated
metadata value type outside `TransientCache`:

```cpp
class BRepGraph_PersistentMetadataValue : public Standard_Transient
{
  BRepGraph_AttrUID myUID;  // NEW
  uint32_t myMutationGen;   // NEW
};
```

Pros: Each attribute instance has its own identity. Shared attributes (same Handle on multiple entities) have one UID.
Cons: UID lifecycle tied to Handle reference counting. Parallel UID vector pattern doesn't apply (attributes aren't in flat vectors).

**A3: Attribute Registry with UID**

Separate persistent attribute store (flat vector) with parallel UID vector:

```cpp
struct AttrStore
{
  NCollection_Vector<Handle(BRepGraph_PersistentMetadataValue)> Attrs;
  NCollection_Vector<BRepGraph_AttrUID>                          UIDs;
  int NbActive = 0;
};
```

Entities reference attributes by `BRepGraph_AttrId` (index into AttrStore) instead of storing them directly on NodeCache.

Pros: Follows DefStore/RepStore pattern. Deduplication of shared attributes.
Cons: Indirection. Changing from NodeCache-based to AttrStore-based storage is a significant refactor.

**A4: UID only for specific attribute types (PMI, Named Layers)**

PMI annotations and named layers are "first-class entities" in STEP/XCAF — they deserve their own entity kinds:

```cpp
// New entity kinds:
Kind::Annotation = 30,    // PMI annotation (GD&T, dimension, note)
Kind::Layer      = 31,    // Named layer / presentation layer
Kind::Material   = 32,    // Material definition
```

Simple attributes (color, name) stay as NodeCache entries without UID.

Pros: Important attributes get full entity treatment. Simple attributes stay simple.
Cons: Growing Kind enum. Mixed approach.

### 23.5 Attribute Identity and XCAF Labels

In XCAF, the TDF_Label itself IS the attribute container:
```
0:1:1:2:1        ← Face label
  XCAFDoc_Color  ← Color attribute ON the label
  TDataStd_Name  ← Name attribute ON the label
```

The label has identity (its path). Attributes don't have separate identity — they're properties of the label.

In BRepGraph with RefId:
```
FaceInShellRef #42 has UID → maps to XCAF label 0:1:1:2:1
  Color stored as attribute on FaceInShellRef → maps to XCAFDoc_Color on label
```

**Key insight**: With RefId + Attributes on refs, the ref's UID serves as the attribute container identity. The attribute itself doesn't need a separate UID — it's a property of the ref/entity that has the UID.

**Exception**: PMI annotations ARE first-class objects in STEP/XCAF with their own identity. They link TO topology but exist independently. These SHOULD be entities (not attributes).

---

## 24. Unified UID Foundation

### 24.1 The Question

BRepGraph currently has domains that need persistent identity:

| Domain | Current ID | Has UID? | Needs UID? |
|---|---|---|---|
| **Entities** (topology + assembly) | NodeId | YES | Already done |
| **References** (containment links) | Inline struct | NO | YES (this design) |
| **Representations** (geometry/mesh) | RepId | NO | YES (for STEP/XCAF) |
| **Attributes** (color, PMI, etc.) | int key on NodeCache | NO | MAYBE (PMI yes, color debatable) |

Can ONE UID foundation serve all of these?

### 24.2 Option F1: Single Universal UID Type

```cpp
struct BRepGraph_UID
{
  enum class Domain : uint8_t { Entity, Ref, Rep, Attr };

  size_t   myCounter;      // global monotonic (shared across all domains)
  Domain   myDomain;       // which domain this UID belongs to
  int      myKindValue;    // NodeId::Kind, RefId::Kind, RepId::Kind, or AttrKind
  uint32_t myGeneration;
};
```

**Pros**:
- ONE type rules all. Simple API surface.
- Global uniqueness guaranteed (one counter).
- VersionStamp works uniformly.
- ToGUID produces globally unique Standard_GUID.
- Serialization: one UID format.

**Cons**:
- `myKindValue` meaning depends on `myDomain` — runtime dispatch needed.
- No compile-time safety: can pass entity UID where ref UID expected.
- Every consumer must check Domain before interpreting Kind.
- Bloats UID size by 1 byte + padding.

### 24.3 Option F2: Shared Counter, Separate Types

```cpp
// All types share one atomic counter: myData->myNextUIDCounter
struct BRepGraph_UID {
  size_t myCounter; NodeId::Kind myKind; uint32_t myGeneration;
};
struct BRepGraph_RefUID {
  size_t myCounter; RefId::Kind myKind; uint32_t myGeneration;
};
struct BRepGraph_RepUID {
  size_t myCounter; RepId::Kind myKind; uint32_t myGeneration;
};
```

**Pros**:
- Compile-time safety: types don't mix.
- Global uniqueness via shared counter.
- Each type carries its own Kind enum — no runtime dispatch.
- Existing BRepGraph_UID unchanged (backward compatible).
- Clean extension: add RepUID, RefUID as needed.

**Cons**:
- Multiple UID types to maintain.
- VersionStamp/ToGUID must handle all types (overloads or template).
- Counter values interleaved across domains (entity=1, ref=2, rep=3, entity=4, ...).

### 24.4 Option F3: Template Generic UID

```cpp
template <typename KindEnum>
struct BRepGraph_GenericUID
{
  size_t   myCounter;
  KindEnum myKind;
  uint32_t myGeneration;

  [[nodiscard]] bool IsValid() const { return myCounter > 0; }
  [[nodiscard]] size_t Counter() const { return myCounter; }
  [[nodiscard]] KindEnum Kind() const { return myKind; }
  [[nodiscard]] uint32_t Generation() const { return myGeneration; }

  bool operator==(const BRepGraph_GenericUID& o) const
  { return myKind == o.myKind && myCounter == o.myCounter; }

  [[nodiscard]] size_t HashValue() const { /* ... */ }
};

using BRepGraph_UID    = BRepGraph_GenericUID<BRepGraph_NodeId::Kind>;
using BRepGraph_RefUID = BRepGraph_GenericUID<BRepGraph_RefId::Kind>;
using BRepGraph_RepUID = BRepGraph_GenericUID<BRepGraph_RepId::Kind>;
```

**Pros**:
- Single implementation, multiple types.
- Compile-time safety (types don't mix).
- DRY — no code duplication.
- Can share counter or have separate counters.

**Cons**:
- Template in public API (OCCT traditionally avoids templates in public headers, though BRepGraph already uses them).
- `std::hash` specialization needed per instantiation.
- NCollection_DataMap needs explicit instantiation.
- Cannot have virtual methods or Standard_Transient inheritance.

### 24.5 Option F4: Base + Typed Wrappers

```cpp
// Base: domain-agnostic identity
struct BRepGraph_BaseUID
{
  size_t   myCounter;
  uint32_t myGeneration;

  [[nodiscard]] bool IsValid() const { return myCounter > 0; }
  [[nodiscard]] size_t Counter() const { return myCounter; }
  [[nodiscard]] uint32_t Generation() const { return myGeneration; }
};

// Typed wrappers (not inheritance — composition or strong typedef)
struct BRepGraph_UID
{
  BRepGraph_BaseUID   Base;
  BRepGraph_NodeId::Kind Kind;
};

struct BRepGraph_RefUID
{
  BRepGraph_BaseUID   Base;
  BRepGraph_RefId::Kind Kind;
};

struct BRepGraph_RepUID
{
  BRepGraph_BaseUID   Base;
  BRepGraph_RepId::Kind Kind;
};
```

**Pros**:
- Common base for shared operations (counter comparison, generation check).
- Typed wrappers prevent mixing.
- No templates in public API.
- Base can be used for domain-agnostic operations (e.g., "is this UID from the current generation?").

**Cons**:
- More boilerplate than template approach.
- Composition vs inheritance debate.
- HashValue/operator== must be implemented per type.

### 24.6 Option F5: Tagged Union (Variant)

```cpp
struct BRepGraph_AnyUID
{
  enum class Domain : uint8_t { Entity, Ref, Rep };

  size_t   myCounter;
  Domain   myDomain;
  uint32_t myGeneration;

  union {
    BRepGraph_NodeId::Kind EntityKind;
    BRepGraph_RefId::Kind  RefKind;
    BRepGraph_RepId::Kind  RepKind;
  } myKind;
};
```

**Pros**: Single type. Compact. Domain-discriminated union.
**Cons**: Union access needs domain check. No compile-time safety.

### 24.7 Foundation Comparison

| Criterion | F1: Universal | F2: Shared counter | F3: Template | F4: Base+Typed | F5: Variant |
|---|:---:|:---:|:---:|:---:|:---:|
| Compile-time safety | - | ++ | ++ | ++ | - |
| Code duplication | ++ | - | ++ | +/- | ++ |
| API simplicity | ++ | +/- | + | +/- | + |
| Backward compatible | - | ++ | +/- | + | - |
| OCCT style fit | + | ++ | +/- | ++ | +/- |
| Extension ease | ++ | + | ++ | + | + |
| VersionStamp integration | ++ | - | ++ | + | ++ |
| Serialization | ++ | + | + | + | + |

### 24.8 Recommendation Analysis

**F2 (shared counter, separate types)** is the most practical:
- Backward compatible: existing `BRepGraph_UID` unchanged
- Add `BRepGraph_RefUID` and `BRepGraph_RepUID` as new types when needed
- One counter ensures global uniqueness
- Each type is self-contained with its own Kind enum
- Follows OCCT's preference for concrete types over templates

**F3 (template)** is the most elegant:
- DRY implementation — one template covers all
- BRepGraph already uses templates extensively (Typed<Kind>, DefStore<T>)
- Could start with F2 and refactor to F3 later

---

## 25. Complete Domain Architecture

With all four identity domains, the full BRepGraph ID architecture would be:

```
                    Volatile ID              Persistent UID           Store
                    ───────────              ──────────────           ─────
Entities:           NodeId (Kind+Index)      BRepGraph_UID            DefStore<T>
References:         RefId  (Kind+Index)      BRepGraph_RefUID         RefStore<T>
Representations:    RepId  (Kind+Index)      BRepGraph_RepUID         RepStore<T>

                         ↕ All share one atomic counter ↕

                    VersionStamp = (UID + MutationGen + Generation)
                    ToGUID       = hash(GraphGUID + VersionStamp fields) → Standard_GUID
```

**Attributes**: Not a separate ID domain in the current direction. Persistent metadata should live in layers keyed by entity/ref UID, while recomputable values stay in `TransientCache`. PMI annotations that need identity become entity kinds (`Kind::Annotation`).

### 25.1 UID Allocation Flow

```
Build() or AddXxx():
  counter = myNextUIDCounter.fetch_add(1)  // ONE counter for ALL domains
  generation = myGeneration.load()

  // Entity:
  BRepGraph_UID(NodeId::Kind, counter, generation)
    → stored in DefStore<T>::UIDs parallel vector

  // Reference:
  BRepGraph_RefUID(RefId::Kind, counter, generation)
    → stored in RefStore<T>::UIDs parallel vector

  // Representation:
  BRepGraph_RepUID(RepId::Kind, counter, generation)
    → stored in RepStore<T>::UIDs parallel vector
```

### 25.2 VersionStamp Across Domains

If unified (V1):
```cpp
struct BRepGraph_VersionStamp
{
  enum class Domain : uint8_t { Entity, Ref, Rep };
  Domain   myDomain;
  size_t   myCounter;     // from UID
  int      myKindValue;   // NodeId/RefId/RepId Kind
  uint32_t myMutationGen;
  uint32_t myGeneration;
};
```

If template (V3):
```cpp
template <typename UIDType>
struct BRepGraph_VersionStampT { UIDType myUID; uint32_t myMutationGen, myGeneration; };
```

### 25.3 Standard_GUID Derivation

ToGUID works identically across all domains:
```
Input:  GraphGUID + Counter + KindValue + Domain + MutationGen + Generation
Output: 128-bit Standard_GUID (deterministic hash)
```

The Domain byte ensures that entity GUID, ref GUID, and rep GUID never collide even if they share the same counter value (impossible with shared counter, but defensive).

---

## 26. Updated Summary of All Decisions

| # | Decision | Options | Status |
|---|---|---|---|
| 1 | Design approach for refs | A (UID on ref), B (promote to entity), **C (RefId table)**, D (composite), E (hybrid) | User selected C |
| 2 | UID counter | **U1 (shared)**, U2 (separate), U3 (partitioned), U4 (domain tag) | User selected shared |
| 3 | Ref kind scope | **K1 (all 8)**, K2 (core 4), K3 (3), K4 (minimal 2), K5 (tiered) | User selected all |
| 4 | Ref sharing | S1 (no sharing), S2 (refcount), S3 (parent list), S4 (indirection) | Analysis shows minimal practical benefit (section 21) |
| 5 | Naming convention | N1 (ChildInParent), N2 (ChildUse), N3 (ChildUsage replace), N4 (Parent_ChildRef), N5 (BRepGraph_), N6 (Membership) | Open |
| 6 | CoEdge/Occurrence refs | T1 (both), T2 (CoEdge only), T3 (neither), T4 (reserved slots) | Open |
| 7 | VersionStamp | **V1 (unified)**, V2 (separate), V3 (template) | User selected unified |
| 8 | Mutation propagation | **P1 (bubble up)**, P2 (ref-only), P3 (configurable), P4 (up+down) | User selected bubble up |
| 9 | BaseRef weight | W1 (minimal 16B), W2 (+IsModified), W3 (+back-pointer), W4 (+cache) | Open |
| 10 | Population timing | **All at Build()**, on-demand, tiered default | User selected all at Build() |
| 11 | UID foundation | F1 (universal), **F2 (shared counter, typed)**, F3 (template), F4 (base+typed), F5 (variant) | Open |
| 12 | Rep UID | R1 (separate RepUID), R2 (extend UID), R3 (no RepUID), R4 (composite) | Open |
| 13 | Attribute UID | A1 (no attr UID), A2 (UID on Handle), A3 (AttrStore), A4 (entity kinds for PMI/Layer) | Open |
