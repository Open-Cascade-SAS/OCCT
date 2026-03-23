# BRepGraph Assembly Model — Intrinsic Design

This document specifies the assembly extension for BRepGraph: Products (reusable
shape definitions) and Occurrences (placed instances forming a DAG).

**Architecture**: Intrinsic.  Assembly is part of the core graph, not a Layer plugin.
Every `BRepGraph` always has at least one root Product.  A "pure topology" graph
(e.g., `graph.Build(aBox)`) automatically gets a single root Product whose
`ShapeRootId` points to the top-level topology node.  Algorithms always see a
uniform model — no need to check "is this an assembly graph?".

Location propagation works naturally: each Occurrence carries a `TopLoc_Location`,
and global placement is composition through the occurrence path.

**Visualizations**: [TODO_Assembly_Visualization.md](TODO_Assembly_Visualization.md) — Mermaid diagrams
for DAG structure, API distribution, compact remap flow, location propagation, and phase dependencies.

---

## Phase 1: Data Model

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

Add helpers and factories:

```cpp
//! True if this kind represents a topology entity (Solid through CompSolid).
static bool IsTopologyKind(Kind theKind)
{
  return static_cast<int>(theKind) <= 7;
}

//! True if this kind represents an assembly entity (Product or Occurrence).
static bool IsAssemblyKind(Kind theKind)
{
  return theKind == Kind::Product || theKind == Kind::Occurrence;
}

//! @name Static factory methods for assembly NodeId construction.
static BRepGraph_NodeId Product(int theIdx)    { return {Kind::Product, theIdx}; }
static BRepGraph_NodeId Occurrence(int theIdx) { return {Kind::Occurrence, theIdx}; }
```

Update `BRepGraph_UID`:

```cpp
bool IsAssembly() const
{
  return myKind == BRepGraph_NodeId::Kind::Product
      || myKind == BRepGraph_NodeId::Kind::Occurrence;
}
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

| Method | Product Action | Occurrence Action |
|--------|---------------|-------------------|
| `TopoDef(NodeId)` | `&myIncStorage.Product(idx)` | `&myIncStorage.Occurrence(idx)` |
| `ChangeTopoDef(NodeId)` | `&myIncStorage.ChangeProduct(idx)` | `&myIncStorage.ChangeOccurrence(idx)` |
| `mutableCache(NodeId)` | `&...ChangeProduct(idx).Cache` | `&...ChangeOccurrence(idx).Cache` |
| `markModified(NodeId)` | Set flag, no upward propagation (products are roots) | Set flag, propagate to parent product |
| `invalidateSubgraphImpl(NodeId)` | Recurse into ShapeRootId + OccurrenceRefs | Recurse into referenced product |
| `allocateUID(NodeId)` | Allocate UID with `Kind::Product` | Allocate UID with `Kind::Occurrence` |

### 1.5 Auto Root Product in BRepGraph_Builder

After `BRepGraphInc_Populate::Perform()` and `populateUIDs()`, automatically
create a single root Product:

```cpp
// Determine the top-level topology NodeId from the input shape type.
// Uses the first entity of the matching kind (index 0).
BRepGraph_NodeId aTopologyRoot;
switch (theShape.ShapeType())
{
  case TopAbs_COMPOUND:  aTopologyRoot = BRepGraph_NodeId::Compound(0);  break;
  case TopAbs_COMPSOLID: aTopologyRoot = BRepGraph_NodeId::CompSolid(0); break;
  case TopAbs_SOLID:     aTopologyRoot = BRepGraph_NodeId::Solid(0);     break;
  case TopAbs_SHELL:     aTopologyRoot = BRepGraph_NodeId::Shell(0);     break;
  case TopAbs_FACE:      aTopologyRoot = BRepGraph_NodeId::Face(0);      break;
  case TopAbs_WIRE:      aTopologyRoot = BRepGraph_NodeId::Wire(0);      break;
  case TopAbs_EDGE:      aTopologyRoot = BRepGraph_NodeId::Edge(0);      break;
  case TopAbs_VERTEX:    aTopologyRoot = BRepGraph_NodeId::Vertex(0);    break;
  default: break; // aTopologyRoot remains invalid
}

// Create a default root product.
auto& aProduct = theGraph.myData->myIncStorage.AppendProduct();
aProduct.Id = BRepGraph_NodeId::Product(0);
aProduct.ShapeRootId = aTopologyRoot;
theGraph.allocateUID(aProduct.Id);
```

**Single-shape-to-product mapping:**
- `TopoDS_Solid` root → Product with `ShapeRootId = Solid(0)`
- `TopoDS_Compound` root → Product with `ShapeRootId = Compound(0)`
- Any shape type maps to the corresponding topology root NodeId
- Zero occurrences for single-shape graphs

### 1.6 Files Modified

| File | Change |
|------|--------|
| `BRepGraph_NodeId.hxx` | Add `Kind::Product = 10`, `Kind::Occurrence = 11`, factories, `IsAssemblyKind()`, `IsTopologyKind()` |
| `BRepGraph_UID.hxx` | Add `IsAssembly()` helper |
| `BRepGraphInc_Entity.hxx` | Add `OccurrenceRef`, `ProductEntity`, `OccurrenceEntity` |
| `BRepGraph_TopoNode.hxx` | Add `ProductDef`, `OccurrenceDef` aliases |
| `BRepGraphInc_Storage.hxx/.cxx` | Add vectors, UIDs, counts, accessors, append, clear |
| `BRepGraph.cxx` | Extend dispatch switches |
| `BRepGraph_Builder.hxx/.cxx` | Auto root product creation, extend `populateUIDs()` |

---

## Phase 2: Core API Integration

Assembly queries and mutations are distributed across existing views.
No separate assembly class is needed because Products and Occurrences are
intrinsic node kinds — they participate in dispatch, UIDs, history, and compact
just like topology kinds.  Adding a Layer would create a removable wrapper
around non-removable data, which is architecturally inconsistent.

### 2.1 DefsView — Const Accessors

In `BRepGraph_DefsView.hxx`, add:

```cpp
// --- Assembly definition accessors ---

int NbProducts() const;
int NbOccurrences() const;
int NbActiveProducts() const;
int NbActiveOccurrences() const;

const BRepGraph_TopoNode::ProductDef& Product(int theIdx) const;
const BRepGraph_TopoNode::OccurrenceDef& Occurrence(int theIdx) const;

//! Return all product NodeIds not referenced by any occurrence (assembly roots).
NCollection_Vector<BRepGraph_NodeId> RootProducts() const;

//! True if the product has child occurrences.
bool IsAssembly(int theProductIdx) const;

//! True if the product has a valid ShapeRootId and no child occurrences.
bool IsPart(int theProductIdx) const;

//! Number of child occurrences of a product.
int NbComponents(int theProductIdx) const;

//! Return the i-th child occurrence NodeId of a product.
BRepGraph_NodeId Component(int theProductIdx, int theIdx) const;
```

Update `NbNodes()` to include `NbProducts() + NbOccurrences()`.

### 2.2 BuilderView — Mutations

In `BRepGraph_BuilderView.hxx`, add:

```cpp
//! Create a part product pointing to an existing topology root.
//! @param[in] theShapeRoot root topology NodeId (Solid, Compound, etc.)
//! @return NodeId of the new product
BRepGraph_NodeId AddProduct(BRepGraph_NodeId theShapeRoot);

//! Create an assembly product (no shape root; children added via AddOccurrence).
//! @return NodeId of the new assembly product
BRepGraph_NodeId AddAssemblyProduct();

//! Create an occurrence linking a parent product to a referenced product.
//! Appends an OccurrenceRef to the parent product's OccurrenceRefs.
//! @param[in] theParentProduct the assembly product owning this occurrence
//! @param[in] theReferencedProduct the product being instanced
//! @param[in] thePlacement local placement relative to parent
//! @return NodeId of the new occurrence
BRepGraph_NodeId AddOccurrence(BRepGraph_NodeId       theParentProduct,
                               BRepGraph_NodeId       theReferencedProduct,
                               const TopLoc_Location& thePlacement);
```

**Extend RemoveNode/RemoveSubgraph:**
- Removing a Product cascades removal to all its child occurrences.
- Removing an Occurrence removes it from the parent Product's OccurrenceRefs.
- `RemoveSubgraph(Product)` removes the product, its occurrences, and recursively
  the topology subgraph under ShapeRootId.

### 2.3 MutView — RAII Guards

In `BRepGraph_MutView.hxx`, add:

```cpp
BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef>    ProductDef(int theIdx);
BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> OccurrenceDef(int theIdx);
```

In `BRepGraph.hxx`, add corresponding MutRef factories:

```cpp
Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef>    MutProduct(int theIdx);
Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> MutOccurrence(int theIdx);
```

### 2.4 Iterator Support

In `BRepGraph_Iterator.hxx`, add template specializations for `ProductDef` and
`OccurrenceDef`.  Works automatically because both inherit from `BaseEntity`
which has `IsRemoved` — the `skipRemoved()` SFINAE mechanism already handles them.

### 2.5 SpatialView — GlobalPlacement

In `BRepGraph_SpatialView.hxx`, add:

```cpp
//! Compute the composed global placement for an occurrence path.
//! Path is root-to-leaf: {occ0, occ1, ..., occN}.
TopLoc_Location GlobalPlacement(const NCollection_Vector<BRepGraph_NodeId>& thePath) const;

//! Compute the global placement for a single occurrence.
//! Walks the ParentProductIdx chain upward, composing Placement values.
TopLoc_Location GlobalPlacement(int theOccurrenceIdx) const;
```

### 2.6 Reverse Index — Product-to-Occurrences

In `BRepGraphInc_ReverseIndex.hxx`, add:

```cpp
//! Return occurrence indices that reference the given product.
const NCollection_Vector<int>* OccurrencesOfProduct(int theProductIdx) const;
```

`OccurrenceEntity::ParentProductIdx` already provides occurrence-to-parent-product
reverse lookup at O(1) — no separate index needed for that direction.

Extend `Build()` to process product/occurrence vectors for the forward direction.

### 2.7 Files Modified

| File | Change |
|------|--------|
| `BRepGraph_DefsView.hxx/.cxx` | Product/Occurrence const accessors, RootProducts, IsAssembly, IsPart, NbComponents, Component |
| `BRepGraph_BuilderView.hxx/.cxx` | AddProduct, AddAssemblyProduct, AddOccurrence, extend RemoveNode/RemoveSubgraph |
| `BRepGraph_MutView.hxx` | ProductDef, OccurrenceDef RAII guard accessors |
| `BRepGraph.hxx/.cxx` | MutProduct, MutOccurrence declarations and implementations |
| `BRepGraph_Iterator.hxx` | ProductDef, OccurrenceDef specializations |
| `BRepGraph_SpatialView.hxx/.cxx` | GlobalPlacement overloads |
| `BRepGraphInc_ReverseIndex.hxx/.cxx` | OccurrencesOfProduct, extend Build() |

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

### 3.3 Compact Pipeline Extension

1. **Collect active**: Copy non-removed products/occurrences to new vectors
   (same pattern as topology kinds).
2. **Build remap entries**: `NodeId::Product(oldIdx) -> NodeId::Product(newIdx)` for each.
3. **Build unified map**: Merge all per-kind maps into single `DataMap<NodeId, NodeId>`.
4. **UID transfer**: Copy UIDs at old indices to new positions.
5. **Cross-reference fixup**: Remap `ProductIdx`, `ParentProductIdx`,
   `ShapeRootId`, `OccurrenceRefs` using `theRemapMap.Find()`.
   Assert on missing entries — a valid cross-reference must always have
   a remap entry (removed nodes are already excluded in step 1).
6. **Layer dispatch**: Pass unified remap map to all registered layers.

Add to `BRepGraphAlgo_Compact::Result`:
```cpp
int NbRemovedProducts    = 0;
int NbRemovedOccurrences = 0;
```

### 3.4 Files Modified

| File | Change |
|------|--------|
| `BRepGraph_Layer.hxx` | Change `OnCompact` signature to unified `DataMap<NodeId, NodeId>` |
| `BRepGraph_NameLayer.hxx/.cxx` | Simplify to single map lookup |
| `BRepGraphAlgo_Compact.hxx` | Add Product/Occurrence to Result |
| `BRepGraphAlgo_Compact.cxx` | Add assembly compaction, build unified map, dispatch |

---

## Phase 4: XDE Population Bridge

### 4.1 Location

New class in DataExchange module (TKBRep has no TKXCAF dependency):

```
src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_PopulateAssembly.hxx/.cxx
```

### 4.2 Algorithm

```cpp
class BRepGraphDE_PopulateAssembly
{
public:
  //! Populate a BRepGraph with assembly structure from an XDE document.
  //! The graph must already have topology populated (via Build).
  //! Uses BuilderView::AddProduct()/AddOccurrence() directly.
  //! @param[in,out] theGraph graph with topology already built
  //! @param[in] theShapeTool XDE shape tool
  static void Perform(BRepGraph&                       theGraph,
                      const Handle(XCAFDoc_ShapeTool)& theShapeTool);
};
```

**Walk logic:**
1. Replace the auto-created root Product with XDE structure.
2. Iterate free shapes via `GetFreeShapes()`.
3. For each free shape label:
   - If simple shape → `AddProduct(shapeRootNodeId)` via `FindNode(shape)`.
   - If assembly → `AddAssemblyProduct()`, recurse into components.
4. For each component:
   - Get location via `GetLocation(componentLabel)`.
   - Resolve referenced shape label.
   - Shared-part deduplication via `TDF_Label → int` map.
   - `AddOccurrence(parent, referenced, location)`.

No layer registration needed — uses `BuilderView` directly.

### 4.3 Files

| Action | File |
|--------|------|
| **Create** | `src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_PopulateAssembly.hxx` |
| **Create** | `src/DataExchange/TKXCAF/BRepGraphDE/BRepGraphDE_PopulateAssembly.cxx` |
| **Create** | `src/DataExchange/TKXCAF/BRepGraphDE/FILES.cmake` |
| **Modify** | `src/DataExchange/TKXCAF/PACKAGES.cmake` — add BRepGraphDE package |

---

## Phase 5: Reconstruction

### 5.1 Extend BRepGraphInc_Reconstruct::Node()

**Kind::Product (part):**
```cpp
// Reconstruct the topology root
aResult = Node(theStorage, aProduct.ShapeRootId, theCache);
```

**Kind::Product (assembly):**
```cpp
// Create Compound from child occurrence reconstructions
TopoDS_Compound aComp;
aBB.MakeCompound(aComp);
for (int i = 0; i < aProduct.OccurrenceRefs.Length(); ++i)
{
  const int anOccIdx = aProduct.OccurrenceRefs.Value(i).OccurrenceIdx;
  TopoDS_Shape aChild = Node(theStorage, BRepGraph_NodeId::Occurrence(anOccIdx), theCache);
  if (!aChild.IsNull())
    aBB.Add(aComp, aChild);
}
aResult = aComp;
```

**Kind::Occurrence:**
```cpp
// Reconstruct referenced product, apply Placement
aResult = Node(theStorage, BRepGraph_NodeId::Product(anOcc.ProductIdx), theCache);
if (!aResult.IsNull() && !anOcc.Placement.IsIdentity())
  aResult.Location(anOcc.Placement);
```

### 5.2 BRepGraphDE_ReconstructAssembly

New class for BRepGraph-to-XDE export (migration interop).
Walks product/occurrence entities and creates matching XDE label structure.

### 5.3 Files

| File | Change |
|------|--------|
| `BRepGraphInc_Reconstruct.cxx` | Add `Kind::Product`, `Kind::Occurrence` cases |
| `BRepGraphDE_ReconstructAssembly.hxx/.cxx` | **Create** — BRepGraph to XDE export |

---

## Phase 6: DE Metadata on Assembly Nodes

The existing Layer system works on any `BRepGraph_NodeId`, including Product and
Occurrence kinds.  No special support needed:

```cpp
aColorLayer->SetColor(BRepGraph_NodeId::Product(0), Quantity_ColorRGBA(RED));   // product-level default
aColorLayer->SetColor(BRepGraph_NodeId::Occurrence(3), Quantity_ColorRGBA(BLUE)); // instance override
```

### 6.1 BRepGraph_AssemblyQuery Utility

New stateless utility (not a Layer) for assembly-specific complex queries:

**File:** `BRepGraph_AssemblyQuery.hxx/.cxx`

```cpp
namespace BRepGraph_AssemblyQuery
{
  //! Resolve an attribute for an occurrence: check occurrence first, fall back to product.
  //! TLayer must provide Find(BRepGraph_NodeId) returning a pointer (nullptr if absent).
  template <typename TLayer>
  auto ResolveAttribute(const TLayer&    theLayer,
                        BRepGraph_NodeId theOccurrenceId,
                        const BRepGraph& theGraph)
    -> decltype(theLayer.Find(theOccurrenceId));

  //! Find all leaf part products reachable from a product (recursive).
  NCollection_Vector<BRepGraph_NodeId> LeafParts(const BRepGraph& theGraph,
                                                  BRepGraph_NodeId theProductId);

  //! Build the occurrence path from root to the given occurrence.
  NCollection_Vector<BRepGraph_NodeId> OccurrencePath(const BRepGraph& theGraph,
                                                      int theOccurrenceIdx);
}
```

### 6.2 Files

| Action | File |
|--------|------|
| **Create** | `BRepGraph_AssemblyQuery.hxx` |
| **Create** | `BRepGraph_AssemblyQuery.cxx` |
| **Modify** | `FILES.cmake` — add new files |

---

## Phase 7: Testing

### 7.1 Core Assembly Tests

File: `src/ModelingData/TKBRep/GTests/BRepGraph_Assembly_Test.cxx`

| Test | Description |
|------|-------------|
| `Build_SingleSolid_AutoCreatesRootProduct` | `Build(aBox)` creates 1 product, 0 occurrences |
| `Build_Compound_AutoCreatesRootProduct` | `Build(aCompound)` creates 1 product with `ShapeRootId = Compound(0)` |
| `AddProduct_IsPart` | Create product with ShapeRootId, verify IsPart/IsAssembly |
| `AddAssemblyProduct_IsAssembly` | Create assembly product, verify IsAssembly/IsPart |
| `AddOccurrence_LinksCorrectly` | Build 2-level assembly, verify NbComponents, Component() |
| `DAGSharing_NbUsages` | Single part referenced by 3 occurrences, verify usage count |
| `MultipleRoots` | Two independent products, verify RootProducts() returns both |
| `DeepNesting_GlobalPlacement` | 4-level assembly, verify location composition |
| `Iterator_Product` | `BRepGraph_Iterator<ProductDef>` visits all non-removed products |
| `Iterator_Occurrence` | `BRepGraph_Iterator<OccurrenceDef>` visits all non-removed occurrences |
| `RemoveOccurrence` | Remove occurrence, verify parent product's OccurrenceRefs updated |
| `RemoveProduct_Cascade` | Remove product, verify child occurrences cascade-removed |
| `MutProduct_RAII` | MutProduct() guard marks modified on scope exit |
| `MutOccurrence_Placement` | Modify placement via MutOccurrence, verify markModified |
| `CompactRemap_Assembly` | Compact after removals, verify cross-references remapped |
| `HistoryTracking_UIDs` | UIDs assigned to products/occurrences survive compact |
| `ResolveAttribute` | Set color on product, override on occurrence, verify resolution |
| `NbNodes_IncludesAssembly` | `NbNodes()` includes product + occurrence counts |

### 7.2 XDE Round-Trip Tests

File: `src/DataExchange/TKXCAF/GTests/BRepGraphDE_Assembly_Test.cxx`

| Test | Description |
|------|-------------|
| `PopulateFromXDE` | Load STEP assembly, populate graph, verify product/occurrence counts |
| `SharedParts` | Assembly with shared parts, verify single ProductEntity per unique part |
| `RoundTrip` | Populate → Reconstruct → compare XDE label trees |

### 7.3 Benchmarks

| Benchmark | Goal |
|-----------|------|
| `PureTopology_SingleProduct` | Verify negligible overhead: Build/Query times unchanged vs baseline |
| `Assembly_1000Parts` | Populate 1000-part flat assembly, measure AddProduct/AddOccurrence throughput |
| `Assembly_DeepNesting_10Levels` | GlobalPlacement composition at depth 10 |

### 7.4 Files

| Action | File |
|--------|------|
| **Create** | `src/ModelingData/TKBRep/GTests/BRepGraph_Assembly_Test.cxx` |
| **Modify** | `src/ModelingData/TKBRep/GTests/FILES.cmake` — add `BRepGraph_Assembly_Test.cxx` |
| **Create** | `src/DataExchange/TKXCAF/GTests/BRepGraphDE_Assembly_Test.cxx` |
| **Modify** | `src/DataExchange/TKXCAF/GTests/FILES.cmake` — add `BRepGraphDE_Assembly_Test.cxx` |

---

## Dependency Graph

```
Phase 1 (Data Model + auto root Product)
  |
  v
Phase 2 (Core API: DefsView, BuilderView, MutView, Iterator, SpatialView, ReverseIndex)
  |
  +---> Phase 3 (OnCompact unified map, Compact for Product/Occurrence)
  +---> Phase 5 (Reconstruct for Product/Occurrence)
  +---> Phase 6 (BRepGraph_AssemblyQuery utility)
  +---> Phase 4 (XDE Population Bridge — needs BuilderView API)
  |
  v
Phase 7 (Testing — parallel with all phases)
```

## API Distribution (replacing BRepGraph_AssemblyLayer)

| Responsibility | Location |
|---------------|----------|
| Const accessors (`Product()`, `RootProducts()`, `IsAssembly()`, `IsPart()`) | `DefsView` |
| Mutations (`AddProduct()`, `AddOccurrence()`) | `BuilderView` |
| RAII guards (`MutProduct()`, `MutOccurrence()`) | `MutView` + `BRepGraph` |
| `GlobalPlacement()` | `SpatialView` |
| Reverse index (product→occurrences) | `BRepGraphInc_ReverseIndex` |
| Removal cascade | `BuilderView::RemoveNode()` |
| Compact remap | `BRepGraphAlgo_Compact` directly |
| Removal cascade (product→occurrences) | `BuilderView::RemoveNode()` / `RemoveSubgraph()` |
| `ResolveAttribute()`, `LeafParts()`, `OccurrencePath()` | `BRepGraph_AssemblyQuery` utility |

## Design Decisions & Rationale

| Decision | Rationale |
|----------|-----------|
| **Intrinsic, not Layer** | Assembly is fundamental to model identity. A Layer can be unregistered, leaving the graph inconsistent. Intrinsic assembly means the graph is always self-consistent and algorithms always see a uniform model. |
| **Auto root Product on Build()** | Every graph has at least one Product. Algorithms never branch on "is this an assembly graph?" — it always is. Single-shape = degenerate case with one Product. |
| **Methods on Views** | Follows existing pattern exactly: const on DefsView, mutations on BuilderView, spatial on SpatialView. No special-case API surface. |
| **Always via Occurrence** | Product→Occurrence→Product. Every placement is explicit with its own NodeId. No implicit "direct child" shortcuts. |
| **Single ShapeRootId** | One NodeId pointing to root topology. Mixed products (part + sub-assembly) not supported — use nested products. |
| **OccurrenceEntity stores ParentProductIdx** | O(1) parent lookup without separate reverse index. |
| **BRepGraph_AssemblyQuery for complex queries** | Stateless utility for LeafParts, OccurrencePath, ResolveAttribute — queries that don't belong on views. |
| **OnCompact unified map** | Replaces 6-argument signature. Fixes missing Compound/CompSolid remapping, extensible for all kinds forever. |
| **XDE bridge in DataExchange** | TKBRep has no TKXCAF dependency. Uses BuilderView directly, no layer registration. |
| **External references deferred** | Core product/occurrence model first. Cross-document references are a future extension. |
