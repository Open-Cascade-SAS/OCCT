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
  Standard_EXPORT void Build(const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
                             const NCollection_Vector<BRepGraphInc::CoEdgeEntity>& theCoEdges,
                             const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
                             const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
                             const NCollection_Vector<BRepGraphInc::ShellEntity>&  theShells,
                             const NCollection_Vector<BRepGraphInc::SolidEntity>&  theSolids);

  //! Incrementally update reverse indices for entities appended after a previous Build().
  //! Only processes entities from the old counts to the current vector lengths.
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
  const NCollection_Vector<int>* WiresOfEdge(const int theEdgeIdx) const
  {
    return seekVec(myEdgeToWires, theEdgeIdx);
  }

  //! Return face indices containing the given edge (from EdgeFaceGeom rows).
  const NCollection_Vector<int>* FacesOfEdge(const int theEdgeIdx) const
  {
    return seekVec(myEdgeToFaces, theEdgeIdx);
  }

  //! Return coedge indices referencing the given edge.
  const NCollection_Vector<int>* CoEdgesOfEdge(const int theEdgeIdx) const
  {
    return seekVec(myEdgeToCoEdges, theEdgeIdx);
  }

  //! Return cached face count for an edge - O(1).
  //! Populated during Build() and updated incrementally by BindEdgeToFace().
  int FaceCountOfEdge(const int theEdgeIdx) const
  {
    if (theEdgeIdx < 0 || theEdgeIdx >= myEdgeFaceCount.Length())
      return 0;
    return myEdgeFaceCount.Value(theEdgeIdx);
  }

  //! Return edge indices incident to the given vertex.
  const NCollection_Vector<int>* EdgesOfVertex(const int theVertexIdx) const
  {
    return seekVec(myVertexToEdges, theVertexIdx);
  }

  //! Return face indices containing the given wire.
  const NCollection_Vector<int>* FacesOfWire(const int theWireIdx) const
  {
    return seekVec(myWireToFaces, theWireIdx);
  }

  //! Return shell indices containing the given face.
  const NCollection_Vector<int>* ShellsOfFace(const int theFaceIdx) const
  {
    return seekVec(myFaceToShells, theFaceIdx);
  }

  //! Return solid indices containing the given shell.
  const NCollection_Vector<int>* SolidsOfShell(const int theShellIdx) const
  {
    return seekVec(myShellToSolids, theShellIdx);
  }

  //! Return occurrence indices that reference the given product.
  const NCollection_Vector<int>* OccurrencesOfProduct(const int theProductIdx) const
  {
    return seekVec(myProductToOccurrences, theProductIdx);
  }

  // --- Safe reference accessors (return empty vector instead of nullptr) ---

  //! Return wire indices containing the given edge (safe reference, never null).
  const NCollection_Vector<int>& WiresOfEdgeRef(const int theEdgeIdx) const
  {
    return seekRef(myEdgeToWires, theEdgeIdx);
  }

  //! Return face indices containing the given edge (safe reference, never null).
  const NCollection_Vector<int>& FacesOfEdgeRef(const int theEdgeIdx) const
  {
    return seekRef(myEdgeToFaces, theEdgeIdx);
  }

  //! Return coedge indices referencing the given edge (safe reference, never null).
  const NCollection_Vector<int>& CoEdgesOfEdgeRef(const int theEdgeIdx) const
  {
    return seekRef(myEdgeToCoEdges, theEdgeIdx);
  }

  //! Return face indices containing the given wire (safe reference, never null).
  const NCollection_Vector<int>& FacesOfWireRef(const int theWireIdx) const
  {
    return seekRef(myWireToFaces, theWireIdx);
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
  Standard_EXPORT void BindEdgeToWire(const int theEdgeIdx, const int theWireIdx);

  //! Remove a wire from the edge-to-wire index for a given edge.
  Standard_EXPORT void UnbindEdgeFromWire(const int theEdgeIdx, const int theWireIdx);

  //! Replace an edge in the edge-to-wire index for a specific wire.
  Standard_EXPORT void ReplaceEdgeInWireMap(const int theOldEdgeIdx,
                                            const int theNewEdgeIdx,
                                            const int theWireIdx);

  //! Register a vertex as incident to an edge (O(1) amortized, deduplicates).
  Standard_EXPORT void BindVertexToEdge(const int theVertexIdx, const int theEdgeIdx);

  //! Remove an edge from the vertex-to-edge index for a given vertex.
  Standard_EXPORT void UnbindVertexFromEdge(const int theVertexIdx, const int theEdgeIdx);

  //! Register a coedge as referencing an edge (O(1) amortized).
  Standard_EXPORT void BindEdgeToCoEdge(const int theEdgeIdx, const int theCoEdgeIdx);

  //! Register an edge as belonging to a face (O(1) amortized, deduplicates).
  Standard_EXPORT void BindEdgeToFace(const int theEdgeIdx, const int theFaceIdx);

  //! Remove a face from the edge-to-face index for a given edge.
  Standard_EXPORT void UnbindEdgeFromFace(const int theEdgeIdx, const int theFaceIdx);

private:
  //! Dense vector type: outer index = entity key, inner vector = adjacency list.
  using IndexTable = NCollection_Vector<NCollection_Vector<int>>;

  //! Bounds-checked lookup returning nullptr for out-of-range or empty slots.
  static const NCollection_Vector<int>* seekVec(const IndexTable& theIdx, const int theKey)
  {
    if (theKey < 0 || theKey >= theIdx.Length())
      return nullptr;
    const NCollection_Vector<int>& aVec = theIdx.Value(theKey);
    return aVec.IsEmpty() ? nullptr : &aVec;
  }

  //! Bounds-checked lookup returning a const reference (empty vector for missing keys).
  static const NCollection_Vector<int>& seekRef(const IndexTable& theIdx, const int theKey)
  {
    const NCollection_Vector<int>* aPtr = seekVec(theIdx, theKey);
    return aPtr != nullptr ? *aPtr : THE_EMPTY_VEC;
  }

  static const NCollection_Vector<int> THE_EMPTY_VEC; //!< Shared empty vector for safe ref returns.

  //! Ensure theIdx has at least theSize slots (pre-sizing with empty vectors).
  //! If theAlloc is non-null, inner vectors are constructed with it.
  static void preSize(IndexTable&                                   theIdx,
                      const int                                     theSize,
                      const occ::handle<NCollection_BaseAllocator>& theAlloc =
                        occ::handle<NCollection_BaseAllocator>());

  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  static void appendUnique(IndexTable& theIdx, const int theKey, const int theVal);

  //! Add theVal to the vector at theKey unconditionally (no duplicate check).
  //! Used during Build() where freshly-cleared indices guarantee no duplicates.
  static void appendDirect(IndexTable& theIdx, const int theKey, const int theVal);

  occ::handle<NCollection_BaseAllocator> myAllocator;

  IndexTable myEdgeToWires;
  IndexTable myEdgeToFaces;
  IndexTable myEdgeToCoEdges;
  IndexTable myVertexToEdges;
  IndexTable myWireToFaces;
  IndexTable myFaceToShells;
  IndexTable myShellToSolids;
  IndexTable myProductToOccurrences;

  NCollection_Vector<int> myEdgeFaceCount; //!< Cached face count per edge, O(1) lookup.
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
