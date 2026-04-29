// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepGraphInc_ReverseIndex_HeaderFile
#define _BRepGraphInc_ReverseIndex_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraphInc_Storage;

namespace BRepGraphInc
{
struct VertexDef;
struct EdgeDef;
struct CoEdgeDef;
struct WireDef;
struct FaceDef;
struct ShellDef;
struct SolidDef;
struct CompoundDef;
struct CompSolidDef;
struct ProductDef;
struct OccurrenceDef;
struct ShellRef;
struct FaceRef;
struct WireRef;
struct CoEdgeRef;
struct SolidRef;
struct ChildRef;
struct VertexRef;
} // namespace BRepGraphInc

//! @brief Backend reverse incidence indices for O(1) upward navigation.
//!
//! Built from entity and reference-entry tables after population.
//! Full ReverseIndex::Build() is used for initial construction, while builder-side
//! mutations maintain the index incrementally through targeted bind/unbind
//! operations and ReverseIndex::BuildDelta() for append workflows.
//!
//! ## Two query tiers
//! Pointer-returning methods (e.g. WiresOfEdge() -> nullptr for empty) serve
//! performance-critical backend code that avoids static-empty-vector overhead.
//! Safe-reference methods (e.g. WiresOfEdgeRef() -> static empty vector) serve
//! the public facade (TopoView delegates to Ref variants).
class BRepGraphInc_ReverseIndex
{
public:
  DEFINE_STANDARD_ALLOC

  //! Set allocator for internal index tables.
  void SetAllocator(const occ::handle<NCollection_BaseAllocator>& theAlloc)
  {
    myAllocator = theAlloc;
  }

  //! Clear all indices.
  Standard_EXPORT void Clear();

  //! Rebuild all reverse indices from storage tables.
  //! Thin wrapper over the explicit-table overload retained for compatibility.
  Standard_EXPORT void Build(const BRepGraphInc_Storage& theStorage);

  //! Rebuild all reverse indices from the entity and reference-entry tables.
  //! Edge-to-face index is derived from CoEdge.FaceDefId links.
  //! @pre SetAllocator() must have been called (uses myAllocator for inner vectors).
  //! @param[in] theEdges      edge entity vector (for vertex-to-edge, edge-to-face)
  //! @param[in] theCoEdges    coedge entity vector (for edge-to-coedge and edge-to-face)
  //! @param[in] theWires      wire entity vector (parent validation for coedge refs)
  //! @param[in] theFaces      face entity vector (parent validation for wire refs)
  //! @param[in] theShells     shell entity vector (parent validation for face refs)
  //! @param[in] theSolids     solid entity vector (parent validation for shell refs)
  //! @param[in] theCompounds  compound entity vector (parent validation for child refs)
  //! @param[in] theCompSolids compsolid entity vector (parent validation for solid refs)
  //! @param[in] theShellRefs  shell ref-entry table (solid -> shell reverse)
  //! @param[in] theFaceRefs   face ref-entry table (shell -> face reverse)
  //! @param[in] theWireRefs   wire ref-entry table (face -> wire reverse)
  //! @param[in] theCoEdgeRefs coedge ref-entry table (wire -> coedge/edge reverse)
  //! @param[in] theSolidRefs  solid ref-entry table (compsolid -> solid reverse)
  //! @param[in] theChildRefs  child ref-entry table (compound child reverse)
  //! @param[in] theVertexRefs vertex ref-entry table (edge vertex resolution)
  Standard_EXPORT void Build(
    const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
    const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
    const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
    const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
    const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
    const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
    const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
    const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
    const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
    const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
    const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
    const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
    const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
    const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs);

  //! Incrementally update reverse indices for entities/ref-parents appended after a previous
  //! ReverseIndex::Build(). Only processes entities from the old counts to the current vector
  //! lengths and appended reference entries.
  //! @param[in] theOldNbEdges   edge count before the append operation
  //! @param[in] theOldNbWires   wire count before the append operation
  //! @param[in] theOldNbFaces   face count before the append operation
  //! @param[in] theOldNbShells  shell count before the append operation
  //! @param[in] theOldNbSolids  solid count before the append operation
  //! @param[in] theOldNbCompounds compound count before the append operation
  //! @param[in] theOldNbCompSolids compsolid count before the append operation
  //! @param[in] theOldNbChildRefs  ChildRef count before the append operation
  //! @param[in] theOldNbSolidRefs  SolidRef count before the append operation
  Standard_EXPORT void BuildDelta(
    const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
    const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
    const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
    const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
    const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
    const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
    const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
    const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
    const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
    const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
    const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
    const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
    const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
    const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs,
    const uint32_t                                              theOldNbEdges,
    const uint32_t                                              theOldNbWires,
    const uint32_t                                              theOldNbFaces,
    const uint32_t                                              theOldNbShells,
    const uint32_t                                              theOldNbSolids,
    const uint32_t                                              theOldNbCompounds,
    const uint32_t                                              theOldNbCompSolids,
    const uint32_t                                              theOldNbChildRefs,
    const uint32_t                                              theOldNbSolidRefs);

  //! Build product-to-occurrences reverse index.
  //! @param[in] theOccurrences occurrence entity vector
  //! @param[in] theNbProducts  total number of products (for pre-sizing)
  Standard_EXPORT void BuildProductOccurrences(
    const NCollection_DynamicArray<BRepGraphInc::OccurrenceDef>& theOccurrences,
    const uint32_t                                               theNbProducts);

  //! Return wire indices containing the given edge.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_WireId>* WiresOfEdge(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToWires, theEdgeId.Index);
  }

  //! Return face indices containing the given edge (derived from CoEdge.FaceDefId links).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_FaceId>* FacesOfEdge(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToFaces, theEdgeId.Index);
  }

  //! Return coedge indices referencing the given edge.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CoEdgeId>* CoEdgesOfEdge(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToCoEdges, theEdgeId.Index);
  }

  //! Return the number of faces incident to an edge - O(1).
  //! Derived directly from the edge-to-faces adjacency vector to keep a single source of truth.
  [[nodiscard]] uint32_t NbFacesOfEdge(const BRepGraph_EdgeId theEdgeId) const
  {
    const NCollection_DynamicArray<BRepGraph_FaceId>* aFaces =
      seekVec(myEdgeToFaces, theEdgeId.Index);
    return aFaces != nullptr ? static_cast<uint32_t>(aFaces->Size()) : 0u;
  }

  //! Return edge indices incident to the given vertex.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_EdgeId>* EdgesOfVertex(
    const BRepGraph_VertexId theVertexId) const
  {
    return seekVec(myVertexToEdges, theVertexId.Index);
  }

  //! Return face indices containing the given wire.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_FaceId>* FacesOfWire(
    const BRepGraph_WireId theWireId) const
  {
    return seekVec(myWireToFaces, theWireId.Index);
  }

  //! Return shell indices containing the given face.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_ShellId>* ShellsOfFace(
    const BRepGraph_FaceId theFaceId) const
  {
    return seekVec(myFaceToShells, theFaceId.Index);
  }

  //! Return solid indices containing the given shell.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_SolidId>* SolidsOfShell(
    const BRepGraph_ShellId theShellId) const
  {
    return seekVec(myShellToSolids, theShellId.Index);
  }

  //! Return compound indices containing the given solid as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfSolid(
    const BRepGraph_SolidId theSolidId) const
  {
    return seekVec(myCompoundsOfSolid, theSolidId.Index);
  }

  //! Return compsolid indices containing the given solid as a SolidInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompSolidId>* CompSolidsOfSolid(
    const BRepGraph_SolidId theSolidId) const
  {
    return seekVec(myCompSolidsOfSolid, theSolidId.Index);
  }

  //! Return compound indices containing the given shell as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfShell(
    const BRepGraph_ShellId theShellId) const
  {
    return seekVec(myCompoundsOfShell, theShellId.Index);
  }

  //! Return compound indices containing the given face as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfFace(
    const BRepGraph_FaceId theFaceId) const
  {
    return seekVec(myCompoundsOfFace, theFaceId.Index);
  }

  //! Return compound indices containing the given compound as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfCompound(
    const BRepGraph_CompoundId theCompoundId) const
  {
    return seekVec(myCompoundsOfCompound, theCompoundId.Index);
  }

  //! Return compound indices containing the given compsolid as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfCompSolid(
    const BRepGraph_CompSolidId theCompSolidId) const
  {
    return seekVec(myCompoundsOfCompSolid, theCompSolidId.Index);
  }

  //! Return compound indices containing the given wire as a NodeInstance.
  //! OCCT `TopoDS_Compound` can legally hold atomic topology (wire / edge /
  //! vertex); these reverse maps round-trip that case.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfWire(
    const BRepGraph_WireId theWireId) const
  {
    return seekVec(myCompoundsOfWire, theWireId.Index);
  }

  //! Return compound indices containing the given edge as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfEdge(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myCompoundsOfEdge, theEdgeId.Index);
  }

  //! Return compound indices containing the given vertex as a NodeInstance.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CompoundId>* CompoundsOfVertex(
    const BRepGraph_VertexId theVertexId) const
  {
    return seekVec(myCompoundsOfVertex, theVertexId.Index);
  }

  //! Return wire indices containing the given coedge.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_WireId>* WiresOfCoEdge(
    const BRepGraph_CoEdgeId theCoEdgeId) const
  {
    return seekVec(myCoEdgeToWires, theCoEdgeId.Index);
  }

  //! Return occurrence indices that reference the given product.
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_OccurrenceId>* OccurrencesOfProduct(
    const BRepGraph_ProductId theProductId) const
  {
    return seekVec(myProductToOccurrences, theProductId.Index);
  }

  // --- Safe reference accessors (return empty vector instead of nullptr) ---

  //! Return wire indices containing the given edge (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_WireId>& WiresOfEdgeRef(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToWires, theEdgeId.Index);
  }

  //! Return face indices containing the given edge (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_FaceId>& FacesOfEdgeRef(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToFaces, theEdgeId.Index);
  }

  //! Return coedge indices referencing the given edge (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_CoEdgeId>& CoEdgesOfEdgeRef(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToCoEdges, theEdgeId.Index);
  }

  //! Return face indices containing the given wire (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_FaceId>& FacesOfWireRef(
    const BRepGraph_WireId theWireId) const
  {
    return seekRef(myWireToFaces, theWireId.Index);
  }

  //! Return edge indices incident to the given vertex (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_EdgeId>& EdgesOfVertexRef(
    const BRepGraph_VertexId theVertexId) const
  {
    return seekRef(myVertexToEdges, theVertexId.Index);
  }

  //! Return shell indices containing the given face (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_ShellId>& ShellsOfFaceRef(
    const BRepGraph_FaceId theFaceId) const
  {
    return seekRef(myFaceToShells, theFaceId.Index);
  }

  //! Return solid indices containing the given shell (safe reference, never null).
  [[nodiscard]] const NCollection_DynamicArray<BRepGraph_SolidId>& SolidsOfShellRef(
    const BRepGraph_ShellId theShellId) const
  {
    return seekRef(myShellToSolids, theShellId.Index);
  }

  //! Verify reverse index consistency against forward entity/reference-entry tables.
  //! For each forward ref (e.g., wire->edge), checks that the corresponding
  //! reverse entry exists (edge->wire). Intended for debug validation.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool Validate(
    const NCollection_DynamicArray<BRepGraphInc::VertexDef>&    theVertices,
    const NCollection_DynamicArray<BRepGraphInc::EdgeDef>&      theEdges,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeDef>&    theCoEdges,
    const NCollection_DynamicArray<BRepGraphInc::WireDef>&      theWires,
    const NCollection_DynamicArray<BRepGraphInc::FaceDef>&      theFaces,
    const NCollection_DynamicArray<BRepGraphInc::ShellDef>&     theShells,
    const NCollection_DynamicArray<BRepGraphInc::SolidDef>&     theSolids,
    const NCollection_DynamicArray<BRepGraphInc::CompoundDef>&  theCompounds,
    const NCollection_DynamicArray<BRepGraphInc::CompSolidDef>& theCompSolids,
    const NCollection_DynamicArray<BRepGraphInc::ShellRef>&     theShellRefs,
    const NCollection_DynamicArray<BRepGraphInc::FaceRef>&      theFaceRefs,
    const NCollection_DynamicArray<BRepGraphInc::WireRef>&      theWireRefs,
    const NCollection_DynamicArray<BRepGraphInc::CoEdgeRef>&    theCoEdgeRefs,
    const NCollection_DynamicArray<BRepGraphInc::SolidRef>&     theSolidRefs,
    const NCollection_DynamicArray<BRepGraphInc::ChildRef>&     theChildRefs,
    const NCollection_DynamicArray<BRepGraphInc::VertexRef>&    theVertexRefs) const;

  // --- Incremental mutation ---

  //! Register an edge as belonging to a wire (O(1) amortized).
  Standard_EXPORT void BindEdgeToWire(const BRepGraph_EdgeId theEdgeId,
                                      const BRepGraph_WireId theWireId);

  //! Remove a wire from the edge-to-wire index for a given edge.
  Standard_EXPORT void UnbindEdgeFromWire(const BRepGraph_EdgeId theEdgeId,
                                          const BRepGraph_WireId theWireId);

  //! Replace an edge in the edge-to-wire index for a specific wire.
  Standard_EXPORT void ReplaceEdgeInWireMap(const BRepGraph_EdgeId theOldEdgeId,
                                            const BRepGraph_EdgeId theNewEdgeId,
                                            const BRepGraph_WireId theWireId);

  //! Register a vertex as incident to an edge (O(1) amortized, deduplicates).
  Standard_EXPORT void BindVertexToEdge(const BRepGraph_VertexId theVertexId,
                                        const BRepGraph_EdgeId   theEdgeId);

  //! Remove an edge from the vertex-to-edge index for a given vertex.
  Standard_EXPORT void UnbindVertexFromEdge(const BRepGraph_VertexId theVertexId,
                                            const BRepGraph_EdgeId   theEdgeId);

  //! Register a coedge as referencing an edge (O(1) amortized).
  Standard_EXPORT void BindEdgeToCoEdge(const BRepGraph_EdgeId   theEdgeId,
                                        const BRepGraph_CoEdgeId theCoEdgeId);

  //! Remove a coedge from the edge-to-coedge index for a given edge.
  Standard_EXPORT void UnbindEdgeFromCoEdge(const BRepGraph_EdgeId   theEdgeId,
                                            const BRepGraph_CoEdgeId theCoEdgeId);

  //! Register a coedge as belonging to a wire (O(1) amortized).
  Standard_EXPORT void BindCoEdgeToWire(const BRepGraph_CoEdgeId theCoEdgeId,
                                        const BRepGraph_WireId   theWireId);

  //! Remove a wire from the coedge-to-wire index for a given coedge.
  Standard_EXPORT void UnbindCoEdgeFromWire(const BRepGraph_CoEdgeId theCoEdgeId,
                                            const BRepGraph_WireId   theWireId);

  //! Register an edge as belonging to a face (O(1) amortized, deduplicates).
  Standard_EXPORT void BindEdgeToFace(const BRepGraph_EdgeId theEdgeId,
                                      const BRepGraph_FaceId theFaceId);

  //! Remove a face from the edge-to-face index for a given edge.
  Standard_EXPORT void UnbindEdgeFromFace(const BRepGraph_EdgeId theEdgeId,
                                          const BRepGraph_FaceId theFaceId);

  //! Register a wire as belonging to a face (O(1) amortized, deduplicates).
  Standard_EXPORT void BindWireToFace(const BRepGraph_WireId theWireId,
                                      const BRepGraph_FaceId theFaceId);

  //! Remove a face from the wire-to-face index for a given wire.
  Standard_EXPORT void UnbindWireFromFace(const BRepGraph_WireId theWireId,
                                          const BRepGraph_FaceId theFaceId);

  //! Register a face as belonging to a shell (O(1) amortized, deduplicates).
  Standard_EXPORT void BindFaceToShell(const BRepGraph_FaceId  theFaceId,
                                       const BRepGraph_ShellId theShellId);

  //! Remove a shell from the face-to-shell index for a given face.
  Standard_EXPORT void UnbindFaceFromShell(const BRepGraph_FaceId  theFaceId,
                                           const BRepGraph_ShellId theShellId);

  //! Register a shell as belonging to a solid (O(1) amortized, deduplicates).
  Standard_EXPORT void BindShellToSolid(const BRepGraph_ShellId theShellId,
                                        const BRepGraph_SolidId theSolidId);

  //! Remove a solid from the shell-to-solid index for a given shell.
  Standard_EXPORT void UnbindShellFromSolid(const BRepGraph_ShellId theShellId,
                                            const BRepGraph_SolidId theSolidId);

  //! Register a solid as belonging to a compsolid (O(1) amortized, deduplicates).
  Standard_EXPORT void BindSolidToCompSolid(const BRepGraph_SolidId     theSolidId,
                                            const BRepGraph_CompSolidId theCompSolidId);

  //! Remove a compsolid from the solid-to-compsolid index for a given solid.
  Standard_EXPORT void UnbindSolidFromCompSolid(const BRepGraph_SolidId     theSolidId,
                                                const BRepGraph_CompSolidId theCompSolidId);

  //! Register a child node as belonging to a compound (dispatched on NodeKind).
  //! Routes to the appropriate per-kind compound reverse map. No-op for unsupported kinds.
  Standard_EXPORT void BindCompoundChild(const BRepGraph_NodeId     theChildDefId,
                                         const BRepGraph_CompoundId theCompoundId);

  //! Remove a compound from the per-kind compound reverse map for a given child node.
  Standard_EXPORT void UnbindCompoundChild(const BRepGraph_NodeId     theChildDefId,
                                           const BRepGraph_CompoundId theCompoundId);

  //! Register an occurrence as referencing a product (O(1) amortized, deduplicates).
  Standard_EXPORT void BindProductOccurrence(const BRepGraph_OccurrenceId theOccurrenceId,
                                             const BRepGraph_ProductId    theProductId);

  //! Remove an occurrence from the product-to-occurrences index for a given product.
  Standard_EXPORT void UnbindProductOccurrence(const BRepGraph_OccurrenceId theOccurrenceId,
                                               const BRepGraph_ProductId    theProductId);

private:
  //! Dense vector type: outer index = entity key, inner vector = typed adjacency list.
  template <typename T>
  using TypedIndexTable = NCollection_DynamicArray<NCollection_DynamicArray<T>>;

  //! Bounds-checked lookup returning nullptr for out-of-range or empty slots.
  template <typename T>
  static const NCollection_DynamicArray<T>* seekVec(const TypedIndexTable<T>& theIdx,
                                                    const uint32_t            theKey)
  {
    if (theKey >= theIdx.Size())
      return nullptr;
    const NCollection_DynamicArray<T>& aVec = theIdx.Value(static_cast<size_t>(theKey));
    return aVec.IsEmpty() ? nullptr : &aVec;
  }

  //! Bounds-checked lookup returning a const reference (empty vector for missing keys).
  template <typename T>
  static const NCollection_DynamicArray<T>& seekRef(const TypedIndexTable<T>& theIdx,
                                                    const uint32_t            theKey)
  {
    const NCollection_DynamicArray<T>* aPtr = seekVec(theIdx, theKey);
    if (aPtr != nullptr)
      return *aPtr;
    static const NCollection_DynamicArray<T> THE_EMPTY;
    return THE_EMPTY;
  }

  //! Ensure theIdx has at least theSize slots (pre-sizing with empty vectors).
  //! If theAlloc is non-null, inner vectors are constructed with it.
  template <typename T>
  static void ensureSize(TypedIndexTable<T>&                           theIdx,
                         const uint32_t                                theSize,
                         const occ::handle<NCollection_BaseAllocator>& theAlloc =
                           occ::handle<NCollection_BaseAllocator>())
  {
    if (theSize <= theIdx.Size())
      return;

    if (!theAlloc.IsNull())
    {
      for (size_t i = theIdx.Size(), aNb = static_cast<size_t>(theSize); i < aNb; ++i)
      {
        theIdx.Append(NCollection_DynamicArray<T>(16, theAlloc));
      }
    }
    else
    {
      for (size_t i = theIdx.Size(), aNb = static_cast<size_t>(theSize); i < aNb; ++i)
      {
        theIdx.Appended();
      }
    }
  }

  //! Ensure theVec has at least theSize elements.
  //! New elements are default-constructed (zero for scalar types).
  template <typename T>
  static void ensureSize(NCollection_DynamicArray<T>& theVec, const uint32_t theSize)
  {
    while (theVec.Size() < static_cast<size_t>(theSize))
    {
      theVec.Appended();
    }
  }

  //! Resize theIdx exactly to theSize slots (clears previous content first).
  template <typename T>
  static void preSize(TypedIndexTable<T>&                           theIdx,
                      const uint32_t                                theSize,
                      const occ::handle<NCollection_BaseAllocator>& theAlloc =
                        occ::handle<NCollection_BaseAllocator>())
  {
    theIdx.Clear();
    ensureSize(theIdx, theSize, theAlloc);
  }

  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  template <typename T>
  static void appendUnique(TypedIndexTable<T>& theIdx, const uint32_t theKey, const T theVal)
  {
    if (theKey >= theIdx.Size())
      ensureSize(theIdx, theKey + 1u);

    NCollection_DynamicArray<T>& aVec = theIdx.ChangeValue(static_cast<size_t>(theKey));
    for (const T& anElem : aVec)
    {
      if (anElem == theVal)
        return;
    }
    aVec.Append(theVal);
  }

  //! Add theVal to the vector at theKey unconditionally (no duplicate check).
  //! Used during ReverseIndex::Build() where freshly-cleared indices guarantee no duplicates.
  template <typename T>
  static void appendDirect(TypedIndexTable<T>& theIdx, const uint32_t theKey, const T theVal)
  {
    if (theKey >= theIdx.Size())
      ensureSize(theIdx, theKey + 1u);

    theIdx.ChangeValue(static_cast<size_t>(theKey)).Append(theVal);
  }

  //! Remove first occurrence of theVal from the vector at theKey via swap-with-last + erase-last.
  //! No-op if theKey is out of range or theVal is absent. O(N) lookup, O(1) removal.
  template <typename T>
  static void eraseSwapLast(TypedIndexTable<T>& theIdx, const uint32_t theKey, const T theVal)
  {
    if (theKey >= theIdx.Size())
      return;
    NCollection_DynamicArray<T>& aVec = theIdx.ChangeValue(static_cast<size_t>(theKey));
    const size_t                 aNb  = aVec.Size();
    for (size_t i = 0; i < aNb; ++i)
    {
      if (aVec.Value(i) == theVal)
      {
        if (i + 1u < aNb)
          aVec.ChangeValue(i) = aVec.Value(aNb - 1u);
        aVec.EraseLast();
        return;
      }
    }
  }

  occ::handle<NCollection_BaseAllocator> myAllocator;

  TypedIndexTable<BRepGraph_WireId>       myEdgeToWires;
  TypedIndexTable<BRepGraph_FaceId>       myEdgeToFaces;
  TypedIndexTable<BRepGraph_CoEdgeId>     myEdgeToCoEdges;
  TypedIndexTable<BRepGraph_EdgeId>       myVertexToEdges;
  TypedIndexTable<BRepGraph_FaceId>       myWireToFaces;
  TypedIndexTable<BRepGraph_ShellId>      myFaceToShells;
  TypedIndexTable<BRepGraph_SolidId>      myShellToSolids;
  TypedIndexTable<BRepGraph_OccurrenceId> myProductToOccurrences;

  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfSolid; //!< Solid -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompSolidId>
                                        myCompSolidsOfSolid; //!< Solid -> parent CompSolid indices.
  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfShell;  //!< Shell -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfFace;   //!< Face -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId>
    myCompoundsOfCompound; //!< Compound -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId>
    myCompoundsOfCompSolid; //!< CompSolid -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfWire;   //!< Wire -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfEdge;   //!< Edge -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId> myCompoundsOfVertex; //!< Vertex -> parent Compound indices.
  TypedIndexTable<BRepGraph_WireId>     myCoEdgeToWires;     //!< CoEdge -> parent Wire indices.

  uint32_t myNbIndexedCoEdges =
    0; //!< Number of coedges indexed by ReverseIndex::Build()/BuildDelta().
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
