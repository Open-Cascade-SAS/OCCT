# BRepGraph Assembly Model — Detailed Design

This document specifies the assembly extension for BRepGraph: Products (reusable
shape definitions) and Occurrences (placed instances forming a DAG).

**Architecture**: Foundation + Plugin.  The foundation adds `Kind::Product` and
`Kind::Occurrence` as first-class node kinds in the core graph (UIDs, history,
compact).  The plugin (`BRepGraph_AssemblyLayer`) provides assembly-specific
query API and lifecycle management, registered on demand.

---

## Phase 1: Data Model (Foundation)

### 1.1 New Kind Values

In `BRepGraph_NodeId.hxx`, extend `Kind` enum:

```cpp
enum class Kind : int
{
  Solid     = 0,
  Shell     = 1,
  Face      = 2,
  Wire      = 3,
  Edge      = 4,
  Vertex    = 5,
  Compound  = 6,
  CompSolid = 7,
  // --- Assembly kinds (gap 8-9 reserved for future topology) ---
  Product    = 10,
  Occurrence = 11
};
```

Add helpers:

```cpp
//! True if this kind represents an assembly entity (Product or Occurrence).
static bool IsAssemblyKind(Kind theKind)
{
  return theKind == Kind::Product || theKind == Kind::Occurrence;
}

//! @name Static factory methods for assembly NodeId construction.
static BRepGraph_NodeId Product(int theIdx)    { return {Kind::Product, theIdx}; }
static BRepGraph_NodeId Occurrence(int theIdx) { return {Kind::Occurrence, theIdx}; }
```

Update `BRepGraph_UID::IsTopology()`:

```cpp
bool IsTopology()  const { return static_cast<int>(myKind) <= 7; }
bool IsAssembly()  const { return myKind == BRepGraph_NodeId::Kind::Product
                               || myKind == BRepGraph_NodeId::Kind::Occurrence; }
```

### 1.2 New Entity Structs

In `BRepGraphInc_Entity.hxx`, add within `namespace BRepGraphInc`:

```cpp
//! Reference from a product to one of its child occurrences.
struct OccurrenceRef
{
  int OccurrenceIdx = -1; //!< Index into myOccurrences vector
};

//! Product entity: a reusable shape definition that may be an assembly or a part.
//!
//! - Part: ShapeRootId points to the root topology node (Solid, Compound, etc.).
//!         OccurrenceRefs is empty.
//! - Assembly: ShapeRootId is invalid. OccurrenceRefs lists child occurrences,
//!            each of which references another product.
struct ProductEntity : public BaseEntity
{
  //! Root topology node for parts. Invalid for assemblies.
  BRepGraph_NodeId ShapeRootId;

  //! Child occurrences (non-empty for assemblies, empty for parts).
  NCollection_Vector<OccurrenceRef> OccurrenceRefs;

  void InitVectors(const Handle(NCollection_BaseAllocator)& theAlloc)
  {
    BRepGraphInc_InitVec(OccurrenceRefs, theAlloc, 4);
  }
};

//! Occurrence entity: a placed instance of a product within a parent product.
//!
//! The DAG structure is: ParentProduct --[OccurrenceRef]--> Occurrence --[ProductIdx]--> ReferencedProduct.
//! Each occurrence carries a TopLoc_Location for its placement relative to the parent.
struct OccurrenceEntity : public BaseEntity
{
  //! Index into myProducts — the product this occurrence instantiates.
  int ProductIdx = -1;

  //! Index into myProducts — the parent assembly that owns this occurrence.
  int ParentProductIdx = -1;

  //! Local placement relative to the parent product's coordinate system.
  TopLoc_Location Placement;
};
```

Add type aliases in `BRepGraph_TopoNode.hxx`:

```cpp
using ProductDef    = BRepGraphInc::ProductEntity;
using OccurrenceDef = BRepGraphInc::OccurrenceEntity;
```

### 1.3 Storage Extension

In `BRepGraphInc_Storage`, add:

**Private members:**

```cpp
NCollection_Vector<BRepGraphInc::ProductEntity>    myProducts;
NCollection_Vector<BRepGraphInc::OccurrenceEntity> myOccurrences;

NCollection_Vector<BRepGraph_UID> myProductUIDs;
NCollection_Vector<BRepGraph_UID> myOccurrenceUIDs;

int myNbActiveProducts    = 0;
int myNbActiveOccurrences = 0;
```

**Public accessors** (following existing pattern):

```cpp
int NbProducts()    const { return myProducts.Length(); }
int NbOccurrences() const { return myOccurrences.Length(); }

int NbActiveProducts()    const { return myNbActiveProducts; }
int NbActiveOccurrences() const { return myNbActiveOccurrences; }

const BRepGraphInc::ProductEntity&    Product(int theIdx)    const;
const BRepGraphInc::OccurrenceEntity& Occurrence(int theIdx) const;

BRepGraphInc::ProductEntity&    ChangeProduct(int theIdx);
BRepGraphInc::OccurrenceEntity& ChangeOccurrence(int theIdx);

BRepGraphInc::ProductEntity&    AppendProduct();
BRepGraphInc::OccurrenceEntity& AppendOccurrence();
```

**UID dispatch**: Extend `UIDs()` / `ChangeUIDs()` switch to handle `Kind::Product` and `Kind::Occurrence`.

**DecrementActiveCount**: Add cases for `Kind::Product` and `Kind::Occurrence`.

**Clear**: Reset product/occurrence vectors and counters.

### 1.4 Dispatch Updates in BRepGraph.cxx

Extend the following switch statements to handle `Kind::Product` and `Kind::Occurrence`:

| Method | Action |
|--------|--------|
| `TopoDef(NodeId)` | Return `const BaseEntity&` from `myIncStorage.Product/Occurrence` |
| `ChangeTopoDef(NodeId)` | Return `BaseEntity&` via `ChangeProduct/ChangeOccurrence` |
| `mutableCache(NodeId)` | Return `NodeCache&` from the entity |
| `allocateUID(NodeId)` | Allocate UID with `Kind::Product` or `Kind::Occurrence` |
| `markModified(NodeId)` | Set `IsModified` flag on product/occurrence entity |

### 1.5 Files Modified

| File | Change |
|------|--------|
| `BRepGraph_NodeId.hxx` | Add `Kind::Product = 10`, `Kind::Occurrence = 11`, factories, `IsAssemblyKind()` |
| `BRepGraph_UID.hxx` | Add `IsAssembly()` helper |
| `BRepGraphInc_Entity.hxx` | Add `OccurrenceRef`, `ProductEntity`, `OccurrenceEntity` |
| `BRepGraph_TopoNode.hxx` | Add `ProductDef`, `OccurrenceDef` aliases |
| `BRepGraphInc_Storage.hxx/.cxx` | Add vectors, UIDs, counts, accessors, append, clear |
| `BRepGraph.cxx` | Extend dispatch switches |

---

## Phase 2: AssemblyLayer Plugin

### 2.1 Class Design

```
BRepGraph_AssemblyLayer : public BRepGraph_Layer
```

**Registered via**: `graph.RegisterLayer(new BRepGraph_AssemblyLayer)`

**Zero overhead**: When not registered, the graph only carries empty product/occurrence
vectors in storage. No virtual dispatch, no query overhead.

### 2.2 Internal State

```cpp
//! Reverse index: for each product, the set of occurrences that reference it.
//! Built lazily on first query, invalidated by OnNodeRemoved/OnCompact/InvalidateAll.
NCollection_DataMap<int, NCollection_Vector<int>> myProductToOccurrences;
bool myReverseIndexDirty = true;
```

### 2.3 Query API

```cpp
//! Layer identity.
const TCollection_AsciiString& Name() const override; // returns "Assembly"

//! Return product NodeIds not referenced by any occurrence (assembly roots).
NCollection_Vector<BRepGraph_NodeId> RootProducts(const BRepGraph& theGraph) const;

//! True if the product has child occurrences (is an assembly, not a part).
bool IsAssembly(const BRepGraph& theGraph, BRepGraph_NodeId theProductId) const;

//! True if the product has a ShapeRootId and no child occurrences (is a leaf part).
bool IsPart(const BRepGraph& theGraph, BRepGraph_NodeId theProductId) const;

//! Number of child occurrences of a product.
int NbComponents(const BRepGraph& theGraph, BRepGraph_NodeId theProductId) const;

//! Return the i-th child occurrence NodeId of a product.
BRepGraph_NodeId Component(const BRepGraph& theGraph, BRepGraph_NodeId theProductId, int theIdx) const;

//! Number of occurrences that reference a given product (usage count).
int NbUsages(const BRepGraph& theGraph, BRepGraph_NodeId theProductId) const;

//! Compute the composed global placement for a path of occurrence indices.
//! Path is root-to-leaf: {occ0, occ1, ..., occN}.
TopLoc_Location GlobalPlacement(const BRepGraph& theGraph,
                                const NCollection_Vector<BRepGraph_NodeId>& thePath) const;
```

### 2.4 Builder Methods

Builder methods live on the layer (not on BuilderView) because assembly structure
is higher-level than topology mutation:

```cpp
//! Create a part product pointing to an existing topology root.
//! @param[in,out] theGraph the graph to modify
//! @param[in] theShapeRoot root topology NodeId (Solid, Compound, etc.)
//! @return new product NodeId
BRepGraph_NodeId AddProduct(BRepGraph& theGraph, BRepGraph_NodeId theShapeRoot);

//! Create an assembly product (no shape root, children added via AddOccurrence).
BRepGraph_NodeId AddAssemblyProduct(BRepGraph& theGraph);

//! Create an occurrence linking a parent product to a referenced product with placement.
//! @param[in,out] theGraph the graph to modify
//! @param[in] theParentProduct the assembly product owning this occurrence
//! @param[in] theReferencedProduct the product being instanced
//! @param[in] thePlacement local placement relative to parent
//! @return new occurrence NodeId
BRepGraph_NodeId AddOccurrence(BRepGraph&             theGraph,
                               BRepGraph_NodeId       theParentProduct,
                               BRepGraph_NodeId       theReferencedProduct,
                               const TopLoc_Location& thePlacement);
```

### 2.5 Lifecycle Callbacks

**OnNodeRemoved(theNode, theReplacement)**:
- If `theNode` is a Product: remove all its child occurrences (cascade).
  If `theReplacement` is valid, migrate OccurrenceRefs to replacement product.
- If `theNode` is an Occurrence: remove from parent product's OccurrenceRefs.
  No cascade (occurrences don't own children).
- Mark reverse index dirty.

**OnCompact(remapMaps)**:
- Remap `ProductIdx`, `ParentProductIdx` in all active occurrences.
- Remap `ShapeRootId` in all active products.
- Remap `OccurrenceRefs` indices in all active products.
- Rebuild reverse index.

**InvalidateAll()**: Mark reverse index dirty.

**Clear()**: Clear reverse index, reset dirty flag.

### 2.6 Files

| Action | File |
|--------|------|
| **Create** | `BRepGraph_AssemblyLayer.hxx` |
| **Create** | `BRepGraph_AssemblyLayer.cxx` |
| **Modify** | `FILES.cmake` — add new files |

---

## Phase 3: OnCompact Signature Unification

### 3.1 Problem

Current `OnCompact` takes 6 per-kind `DataMap<int,int>` arguments:

```cpp
virtual void OnCompact(const NCollection_DataMap<int, int>& theVertexMap,
                       const NCollection_DataMap<int, int>& theEdgeMap,
                       const NCollection_DataMap<int, int>& theWireMap,
                       const NCollection_DataMap<int, int>& theFaceMap,
                       const NCollection_DataMap<int, int>& theShellMap,
                       const NCollection_DataMap<int, int>& theSolidMap) = 0;
```

Issues:
- Missing Compound/CompSolid remapping (existing bug).
- Not extensible for Product/Occurrence without adding more arguments.
- Verbose call sites.

### 3.2 New Signature

```cpp
//! Called after Compact with a unified old->new remap map.
//! Layer must remap all internal NodeId references using this map.
//! @param[in] theRemapMap maps old NodeId to new NodeId for all remapped nodes
virtual void OnCompact(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theRemapMap) = 0;
```

### 3.3 Migration

| File | Change |
|------|--------|
| `BRepGraph_Layer.hxx` | Change `OnCompact` signature |
| `BRepGraph_NameLayer.hxx/.cxx` | Update implementation, replace 6-way `remapNodeId` with single map lookup |
| `BRepGraph_AssemblyLayer.cxx` | Implement with unified map |
| `BRepGraphAlgo_Compact.cxx` | Build unified `DataMap<NodeId, NodeId>` covering all kinds, pass to layers |

### 3.4 Compact Pipeline Extension

Add Product/Occurrence to the compact pipeline:

1. **Collect active**: Copy non-removed products/occurrences to new vectors.
2. **Build remap entries**: `NodeId::Product(oldIdx) -> NodeId::Product(newIdx)` for each.
3. **UID transfer**: Copy UIDs at old indices to new positions.
4. **Cross-reference fixup**: After vector copy, remap all `ProductIdx`,
   `ParentProductIdx`, `ShapeRootId`, `OccurrenceRefs` using the remap map.
5. **Layer dispatch**: Pass unified remap map to all registered layers.

---

## Phase 4: XDE Population Bridge

### 4.1 Location

New class in DataExchange module (TKBRep has no TKXCAF dependency):

```
src/DataExchange/TKXDE/BRepGraphDE/BRepGraphDE_PopulateAssembly.hxx/.cxx
```

### 4.2 Algorithm

```cpp
class BRepGraphDE_PopulateAssembly
{
public:
  //! Populate a BRepGraph with assembly structure from an XDE document.
  //! @param[in,out] theGraph already populated with topology (via BRepGraphInc_Populate)
  //! @param[in] theShapeTool XDE shape tool (provides label tree, locations, references)
  //!
  //! Registers BRepGraph_AssemblyLayer on the graph if not already present.
  void Perform(BRepGraph& theGraph,
               const Handle(XCAFDoc_ShapeTool)& theShapeTool);
};
```

**Walk logic**:
1. Iterate free shapes via `GetFreeShapes()`.
2. For each free shape label:
   - If simple shape → create ProductEntity with ShapeRootId linked to the
     topology node found by TShape lookup in `myIncStorage`.
   - If assembly → create ProductEntity (no ShapeRootId), recurse into components.
3. For each component:
   - Create OccurrenceEntity with `GetLocation()` as Placement.
   - Resolve the referenced shape label.
   - If referenced product already exists (shared part deduplication via
     `TDF_Label -> int` map), reuse it. Otherwise create new ProductEntity.
4. Register `BRepGraph_AssemblyLayer` and call `AddProduct`/`AddOccurrence`.

### 4.3 Files

| Action | File |
|--------|------|
| **Create** | `BRepGraphDE_PopulateAssembly.hxx` |
| **Create** | `BRepGraphDE_PopulateAssembly.cxx` |
| **Modify** | DataExchange `FILES.cmake` |

---

## Phase 5: Reconstruction

### 5.1 Extend BRepGraphInc_Reconstruct::Node()

Add cases for the new kinds:

- **Kind::Product (part)**: Reconstruct the ShapeRootId topology node via existing
  `Node()` dispatch. Return the reconstructed shape.
- **Kind::Product (assembly)**: Iterate OccurrenceRefs, reconstruct each occurrence
  child, create a `TopoDS_Compound` containing all children.
- **Kind::Occurrence**: Reconstruct the referenced product's shape, apply
  `Placement` via `TopoDS_Shape::Located()`.

### 5.2 BRepGraphDE_ReconstructAssembly

New class for BRepGraph-to-XDE export (migration interop):

```
src/DataExchange/TKXDE/BRepGraphDE/BRepGraphDE_ReconstructAssembly.hxx/.cxx
```

Walks product/occurrence entities and creates matching XDE label structure
with `XCAFDoc_ShapeTool::AddShape`, `AddComponent`, `SetShape`.

### 5.3 Files

| File | Change |
|------|--------|
| `BRepGraphInc_Reconstruct.cxx` | Add `Kind::Product`, `Kind::Occurrence` cases |
| `BRepGraphDE_ReconstructAssembly.hxx/.cxx` | **Create** — BRepGraph to XDE export |

---

## Phase 6: DE Metadata on Assembly Nodes

The existing Layer system works on any `BRepGraph_NodeId`, including Product and
Occurrence kinds. No special support needed — layers index by NodeId, not by Kind:

```cpp
aColorLayer->SetColor(BRepGraph_NodeId::Product(0), Quantity_ColorRGBA(RED));   // product-level default
aColorLayer->SetColor(BRepGraph_NodeId::Occurrence(3), Quantity_ColorRGBA(BLUE)); // instance override
```

### 6.1 ResolveAttribute Helper

Add to `BRepGraph_AssemblyLayer`:

```cpp
//! Resolve an attribute for an occurrence: check occurrence first, fall back to product.
//! @tparam TLayer a layer type with Find(NodeId) returning const T*
//! @tparam T the attribute value type
//! @param[in] theLayer the attribute layer to query
//! @param[in] theOccurrenceId the occurrence to resolve for
//! @param[in] theGraph the owning graph
//! @return pointer to attribute value, or nullptr if neither occurrence nor product has it
template <typename TLayer, typename T>
const T* ResolveAttribute(const TLayer&    theLayer,
                          BRepGraph_NodeId theOccurrenceId,
                          const BRepGraph& theGraph) const;
```

Implementation: query `theLayer.Find(theOccurrenceId)`. If null, look up
`OccurrenceEntity::ProductIdx`, query `theLayer.Find(NodeId::Product(productIdx))`.

---

## Phase 7: Testing

### 7.1 Unit Tests

File: `src/ModelingData/TKBRep/GTests/BRepGraph_AssemblyLayer_Test.cxx`

| Test | Description |
|------|-------------|
| `AddPartProduct` | Create product with ShapeRootId, verify IsPart/IsAssembly |
| `AddAssemblyWithOccurrences` | Build 2-level assembly, verify NbComponents, Component() |
| `DAGSharing` | Single part referenced by 3 occurrences, verify NbUsages == 3 |
| `MultipleRoots` | Two independent products, verify RootProducts returns both |
| `DeepNesting` | 4-level assembly, verify GlobalPlacement composition |
| `RemoveOccurrence` | Remove occurrence, verify parent product's OccurrenceRefs updated |
| `RemoveProduct_Cascade` | Remove product, verify child occurrences cascade-removed |
| `CompactRemap` | Compact after removals, verify all cross-references remapped correctly |
| `HistoryTracking` | Verify UIDs assigned to products/occurrences survive compact |
| `LayerLifecycle` | Register/unregister layer, verify no leaks |
| `ZeroOverhead` | Pure topology graph without layer, verify no assembly vectors allocated beyond empty |
| `ResolveAttribute` | Set color on product, override on occurrence, verify resolution |

### 7.2 XDE Round-Trip Tests

Location: DataExchange GTests

| Test | Description |
|------|-------------|
| `PopulateFromXDE` | Load STEP assembly, populate graph, verify product/occurrence counts |
| `SharedParts` | Assembly with shared parts, verify single ProductEntity per unique part |
| `RoundTrip` | Populate → Reconstruct → compare XDE label trees |

### 7.3 Benchmarks

Add to `BENCHMARKS.md`:

| Benchmark | Goal |
|-----------|------|
| `PureTopology_NoAssemblyLayer` | Verify zero overhead: Build/Query times unchanged vs baseline |
| `Assembly_1000Parts` | Populate 1000-part flat assembly, measure AddProduct/AddOccurrence throughput |
| `Assembly_DeepNesting_10Levels` | GlobalPlacement composition at depth 10 |

---

## Dependency Graph

```
Phase 1 (Data Model)
  |
  v
Phase 2 (AssemblyLayer)
  |
  +---> Phase 3 (OnCompact signature fix)
  |
  +---> Phase 4 (XDE Population)
  |       |
  |       v
  |     Phase 5 (Reconstruction)
  |
  +---> Phase 6 (DE Metadata Layers)

Phase 7 (Testing) — parallel with all phases
```

## Design Decisions & Rationale

| Decision | Rationale |
|----------|-----------|
| **Always via Occurrence** | Product→Occurrence→Product. Every placement is explicit with its own NodeId. No implicit "direct child" shortcuts that would complicate the DAG. |
| **Single ShapeRootId per product** | One NodeId pointing to the root topology (Solid, Compound, etc.). Mixed products (part + sub-assembly) are not supported — use nested products. |
| **Layer-based query API** | Assembly queries (`RootProducts`, `GlobalPlacement`, etc.) live in `BRepGraph_AssemblyLayer`, keeping the core BRepGraph class focused on topology. |
| **Builder methods on layer** | Assembly structure is higher-level than topology mutation. `AddProduct`/`AddOccurrence` on the layer avoids bloating `BuilderView`. |
| **OnCompact unified map** | Replaces 6-argument signature. Fixes missing Compound/CompSolid remapping, extensible for Product/Occurrence without signature changes. |
| **XDE bridge in DataExchange** | TKBRep has no TKXCAF dependency. Population and reconstruction of XDE structures belong in the DataExchange module. |
| **External references deferred** | Core product/occurrence model first. Cross-document references (e.g., externally referenced parts) are a future extension. |
