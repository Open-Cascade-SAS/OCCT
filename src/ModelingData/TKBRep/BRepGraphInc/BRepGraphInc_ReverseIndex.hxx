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
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

namespace BRepGraphInc
{
struct EdgeEntity;
struct CoEdgeEntity;
struct WireEntity;
struct FaceEntity;
struct ShellEntity;
struct SolidEntity;
struct CompoundEntity;
struct CompSolidEntity;
struct ProductEntity;
struct OccurrenceEntity;
} // namespace BRepGraphInc

//! @brief Reverse incidence indices for O(1) upward navigation.
//!
//! Built from entity vectors and relationship-geometry rows after
//! population.  Indices are rebuilt entirely on each mutation
//! (rebuild cost is O(N) which is acceptable for typical models).
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

  //! Rebuild all reverse indices from the entity tables.
  //! Edge-to-face index is derived from inline PCurve entries on EdgeEntity.
  //! @param[in] theEdges   edge entity vector (for vertex-to-edge, edge-to-face)
  //! @param[in] theWires   wire entity vector (for edge-to-wire)
  //! @param[in] theFaces   face entity vector (for wire-to-face)
  //! @param[in] theShells  shell entity vector (for face-to-shell)
  //! @param[in] theSolids  solid entity vector (for shell-to-solid)
  Standard_EXPORT void Build(
    const NCollection_Vector<BRepGraphInc::EdgeEntity>&      theEdges,
    const NCollection_Vector<BRepGraphInc::CoEdgeEntity>&    theCoEdges,
    const NCollection_Vector<BRepGraphInc::WireEntity>&      theWires,
    const NCollection_Vector<BRepGraphInc::FaceEntity>&      theFaces,
    const NCollection_Vector<BRepGraphInc::ShellEntity>&     theShells,
    const NCollection_Vector<BRepGraphInc::SolidEntity>&     theSolids,
    const NCollection_Vector<BRepGraphInc::CompoundEntity>&  theCompounds,
    const NCollection_Vector<BRepGraphInc::CompSolidEntity>& theCompSolids);

  //! Incrementally update reverse indices for entities appended after a previous Build().
  //! Only processes entities from the old counts to the current vector lengths.
  //! Compound/CompSolid reverse indices are not updated incrementally -
  //! these containers are populated once during Build() and not mutated post-build.
  //! @param[in] theOldNbEdges   edge count before the append operation
  //! @param[in] theOldNbWires   wire count before the append operation
  //! @param[in] theOldNbFaces   face count before the append operation
  //! @param[in] theOldNbShells  shell count before the append operation
  //! @param[in] theOldNbSolids  solid count before the append operation
  Standard_EXPORT void BuildDelta(const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
                                  const NCollection_Vector<BRepGraphInc::CoEdgeEntity>& theCoEdges,
                                  const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
                                  const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
                                  const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
                                  const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids,
                                  const int theOldNbEdges,
                                  const int theOldNbWires,
                                  const int theOldNbFaces,
                                  const int theOldNbShells,
                                  const int theOldNbSolids);

  //! Build product-to-occurrences reverse index.
  //! @param[in] theOccurrences occurrence entity vector
  //! @param[in] theNbProducts  total number of products (for pre-sizing)
  Standard_EXPORT void BuildProductOccurrences(
    const NCollection_Vector<BRepGraphInc::OccurrenceEntity>& theOccurrences,
    const int                                                 theNbProducts);

  //! Return wire indices containing the given edge.
  const NCollection_Vector<BRepGraph_WireId>* WiresOfEdge(const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToWires, theEdgeId.Index);
  }

  //! Return face indices containing the given edge (from EdgeFaceGeom rows).
  const NCollection_Vector<BRepGraph_FaceId>* FacesOfEdge(const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToFaces, theEdgeId.Index);
  }

  //! Return coedge indices referencing the given edge.
  const NCollection_Vector<BRepGraph_CoEdgeId>* CoEdgesOfEdge(const BRepGraph_EdgeId theEdgeId) const
  {
    return seekVec(myEdgeToCoEdges, theEdgeId.Index);
  }

  //! Return cached face count for an edge - O(1).
  //! Populated during Build() and updated incrementally by BindEdgeToFace().
  int FaceCountOfEdge(const BRepGraph_EdgeId theEdgeId) const
  {
    if (theEdgeId.Index < 0 || theEdgeId.Index >= myEdgeFaceCount.Length())
      return 0;
    return myEdgeFaceCount.Value(theEdgeId.Index);
  }

  //! Return edge indices incident to the given vertex.
  const NCollection_Vector<BRepGraph_EdgeId>* EdgesOfVertex(const BRepGraph_VertexId theVertexId) const
  {
    return seekVec(myVertexToEdges, theVertexId.Index);
  }

  //! Return face indices containing the given wire.
  const NCollection_Vector<BRepGraph_FaceId>* FacesOfWire(const BRepGraph_WireId theWireId) const
  {
    return seekVec(myWireToFaces, theWireId.Index);
  }

  //! Return shell indices containing the given face.
  const NCollection_Vector<BRepGraph_ShellId>* ShellsOfFace(const BRepGraph_FaceId theFaceId) const
  {
    return seekVec(myFaceToShells, theFaceId.Index);
  }

  //! Return solid indices containing the given shell.
  const NCollection_Vector<BRepGraph_SolidId>* SolidsOfShell(const BRepGraph_ShellId theShellId) const
  {
    return seekVec(myShellToSolids, theShellId.Index);
  }

  //! Return compound indices containing the given solid as a ChildRef.
  const NCollection_Vector<BRepGraph_CompoundId>* CompoundsOfSolid(
    const BRepGraph_SolidId theSolidId) const
  {
    return seekVec(myCompoundsOfSolid, theSolidId.Index);
  }

  //! Return compsolid indices containing the given solid as a SolidRef.
  const NCollection_Vector<BRepGraph_CompSolidId>* CompSolidsOfSolid(
    const BRepGraph_SolidId theSolidId) const
  {
    return seekVec(myCompSolidsOfSolid, theSolidId.Index);
  }

  //! Return compound indices containing the given shell as a ChildRef.
  const NCollection_Vector<BRepGraph_CompoundId>* CompoundsOfShell(
    const BRepGraph_ShellId theShellId) const
  {
    return seekVec(myCompoundsOfShell, theShellId.Index);
  }

  //! Return compound indices containing the given face as a ChildRef.
  const NCollection_Vector<BRepGraph_CompoundId>* CompoundsOfFace(
    const BRepGraph_FaceId theFaceId) const
  {
    return seekVec(myCompoundsOfFace, theFaceId.Index);
  }

  //! Return compound indices containing the given compound as a ChildRef.
  const NCollection_Vector<BRepGraph_CompoundId>* CompoundsOfCompound(
    const BRepGraph_CompoundId theCompoundId) const
  {
    return seekVec(myCompoundsOfCompound, theCompoundId.Index);
  }

  //! Return compound indices containing the given compsolid as a ChildRef.
  const NCollection_Vector<BRepGraph_CompoundId>* CompoundsOfCompSolid(
    const BRepGraph_CompSolidId theCompSolidId) const
  {
    return seekVec(myCompoundsOfCompSolid, theCompSolidId.Index);
  }

  //! Return wire indices containing the given coedge.
  const NCollection_Vector<BRepGraph_WireId>* WiresOfCoEdge(
    const BRepGraph_CoEdgeId theCoEdgeId) const
  {
    return seekVec(myCoEdgeToWires, theCoEdgeId.Index);
  }

  //! Return occurrence indices that reference the given product.
  const NCollection_Vector<BRepGraph_OccurrenceId>* OccurrencesOfProduct(
    const BRepGraph_ProductId theProductId) const
  {
    return seekVec(myProductToOccurrences, theProductId.Index);
  }

  // --- Safe reference accessors (return empty vector instead of nullptr) ---

  //! Return wire indices containing the given edge (safe reference, never null).
  const NCollection_Vector<BRepGraph_WireId>& WiresOfEdgeRef(const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToWires, theEdgeId.Index);
  }

  //! Return face indices containing the given edge (safe reference, never null).
  const NCollection_Vector<BRepGraph_FaceId>& FacesOfEdgeRef(const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToFaces, theEdgeId.Index);
  }

  //! Return coedge indices referencing the given edge (safe reference, never null).
  const NCollection_Vector<BRepGraph_CoEdgeId>& CoEdgesOfEdgeRef(
    const BRepGraph_EdgeId theEdgeId) const
  {
    return seekRef(myEdgeToCoEdges, theEdgeId.Index);
  }

  //! Return face indices containing the given wire (safe reference, never null).
  const NCollection_Vector<BRepGraph_FaceId>& FacesOfWireRef(const BRepGraph_WireId theWireId) const
  {
    return seekRef(myWireToFaces, theWireId.Index);
  }

  //! Verify reverse index consistency against forward entity tables.
  //! For each forward ref (e.g., wire->edge), checks that the corresponding
  //! reverse entry exists (edge->wire). Intended for debug validation.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool Validate(
    const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
    const NCollection_Vector<BRepGraphInc::CoEdgeEntity>& theCoEdges,
    const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
    const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
    const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
    const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids) const;

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

  //! Register an edge as belonging to a face (O(1) amortized, deduplicates).
  Standard_EXPORT void BindEdgeToFace(const BRepGraph_EdgeId theEdgeId,
                                      const BRepGraph_FaceId theFaceId);

  //! Remove a face from the edge-to-face index for a given edge.
  Standard_EXPORT void UnbindEdgeFromFace(const BRepGraph_EdgeId theEdgeId,
                                          const BRepGraph_FaceId theFaceId);

private:
  //! Dense vector type: outer index = entity key, inner vector = typed adjacency list.
  template <typename T>
  using TypedIndexTable = NCollection_Vector<NCollection_Vector<T>>;

  //! Bounds-checked lookup returning nullptr for out-of-range or empty slots.
  template <typename T>
  static const NCollection_Vector<T>* seekVec(const TypedIndexTable<T>& theIdx, const int theKey)
  {
    if (theKey < 0 || theKey >= theIdx.Length())
      return nullptr;
    const NCollection_Vector<T>& aVec = theIdx.Value(theKey);
    return aVec.IsEmpty() ? nullptr : &aVec;
  }

  //! Bounds-checked lookup returning a const reference (empty vector for missing keys).
  template <typename T>
  static const NCollection_Vector<T>& seekRef(const TypedIndexTable<T>& theIdx, const int theKey)
  {
    const NCollection_Vector<T>* aPtr = seekVec(theIdx, theKey);
    if (aPtr != nullptr)
      return *aPtr;
    static const NCollection_Vector<T> THE_EMPTY;
    return THE_EMPTY;
  }

  //! Ensure theIdx has at least theSize slots (pre-sizing with empty vectors).
  //! If theAlloc is non-null, inner vectors are constructed with it.
  template <typename T>
  static void preSize(TypedIndexTable<T>&                             theIdx,
                      const int                                       theSize,
                      const occ::handle<NCollection_BaseAllocator>& theAlloc =
                        occ::handle<NCollection_BaseAllocator>())
  {
    theIdx.Clear();
    for (int i = 0; i < theSize; ++i)
    {
      if (!theAlloc.IsNull())
      {
        theIdx.Append(NCollection_Vector<T>(16, theAlloc));
      }
      else
      {
        theIdx.Appended();
      }
    }
  }

  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  template <typename T>
  static void appendUnique(TypedIndexTable<T>& theIdx, const int theKey, const T theVal)
  {
    Standard_ASSERT_RETURN(theKey >= 0, "appendUnique: negative key", );
    // Grow if needed for incremental mutation after Build().
    if (theKey >= theIdx.Length())
      theIdx.SetValue(theKey, NCollection_Vector<T>());

    NCollection_Vector<T>& aVec = theIdx.ChangeValue(theKey);
    for (int i = 0; i < aVec.Length(); ++i)
    {
      if (aVec.Value(i) == theVal)
        return;
    }
    aVec.Append(theVal);
  }

  //! Add theVal to the vector at theKey unconditionally (no duplicate check).
  //! Used during Build() where freshly-cleared indices guarantee no duplicates.
  template <typename T>
  static void appendDirect(TypedIndexTable<T>& theIdx, const int theKey, const T theVal)
  {
    Standard_ASSERT_RETURN(theKey >= 0, "appendDirect: negative key", );
    // During Build(), outer vector is pre-sized so theKey < Length().
    // For safety, grow if somehow out of range.
    if (theKey >= theIdx.Length())
      theIdx.SetValue(theKey, NCollection_Vector<T>());

    theIdx.ChangeValue(theKey).Append(theVal);
  }

  occ::handle<NCollection_BaseAllocator> myAllocator;

  TypedIndexTable<BRepGraph_WireId>        myEdgeToWires;
  TypedIndexTable<BRepGraph_FaceId>        myEdgeToFaces;
  TypedIndexTable<BRepGraph_CoEdgeId>      myEdgeToCoEdges;
  TypedIndexTable<BRepGraph_EdgeId>        myVertexToEdges;
  TypedIndexTable<BRepGraph_FaceId>        myWireToFaces;
  TypedIndexTable<BRepGraph_ShellId>       myFaceToShells;
  TypedIndexTable<BRepGraph_SolidId>       myShellToSolids;
  TypedIndexTable<BRepGraph_OccurrenceId>  myProductToOccurrences;

  TypedIndexTable<BRepGraph_CompoundId>    myCompoundsOfSolid;     //!< Solid -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompSolidId>   myCompSolidsOfSolid;    //!< Solid -> parent CompSolid indices.
  TypedIndexTable<BRepGraph_CompoundId>    myCompoundsOfShell;     //!< Shell -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId>    myCompoundsOfFace;      //!< Face -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId>    myCompoundsOfCompound;  //!< Compound -> parent Compound indices.
  TypedIndexTable<BRepGraph_CompoundId>    myCompoundsOfCompSolid; //!< CompSolid -> parent Compound indices.
  TypedIndexTable<BRepGraph_WireId>        myCoEdgeToWires;        //!< CoEdge -> parent Wire indices.

  NCollection_Vector<int> myEdgeFaceCount; //!< Cached face count per edge, O(1) lookup.
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
