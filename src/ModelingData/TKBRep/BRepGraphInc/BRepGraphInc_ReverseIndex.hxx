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
struct WireEntity;
struct FaceEntity;
struct ShellEntity;
struct SolidEntity;
}

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
  void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
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
  Standard_EXPORT void Build(const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
                             const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
                             const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
                             const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
                             const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids);

  //! Incrementally update reverse indices for entities appended after a previous Build().
  //! Only processes entities from the old counts to the current vector lengths.
  //! @param[in] theOldNbEdges   edge count before the append operation
  //! @param[in] theOldNbWires   wire count before the append operation
  //! @param[in] theOldNbFaces   face count before the append operation
  //! @param[in] theOldNbShells  shell count before the append operation
  //! @param[in] theOldNbSolids  solid count before the append operation
  Standard_EXPORT void BuildDelta(const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
                                  const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
                                  const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
                                  const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
                                  const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids,
                                  int theOldNbEdges,
                                  int theOldNbWires,
                                  int theOldNbFaces,
                                  int theOldNbShells,
                                  int theOldNbSolids);

  //! Return wire indices containing the given edge.
  const NCollection_Vector<int>* WiresOfEdge(int theEdgeIdx) const
  {
    return seekVec(myEdgeToWires, theEdgeIdx);
  }

  //! Return face indices containing the given edge (from EdgeFaceGeom rows).
  const NCollection_Vector<int>* FacesOfEdge(int theEdgeIdx) const
  {
    return seekVec(myEdgeToFaces, theEdgeIdx);
  }

  //! Return cached face count for an edge — O(1).
  //! Populated during Build() and updated incrementally by BindEdgeToFace().
  int FaceCountOfEdge(int theEdgeIdx) const
  {
    if (theEdgeIdx < 0 || theEdgeIdx >= myEdgeFaceCount.Length())
      return 0;
    return myEdgeFaceCount.Value(theEdgeIdx);
  }

  //! Return edge indices incident to the given vertex.
  const NCollection_Vector<int>* EdgesOfVertex(int theVertexIdx) const
  {
    return seekVec(myVertexToEdges, theVertexIdx);
  }

  //! Return face indices containing the given wire.
  const NCollection_Vector<int>* FacesOfWire(int theWireIdx) const
  {
    return seekVec(myWireToFaces, theWireIdx);
  }

  //! Return shell indices containing the given face.
  const NCollection_Vector<int>* ShellsOfFace(int theFaceIdx) const
  {
    return seekVec(myFaceToShells, theFaceIdx);
  }

  //! Return solid indices containing the given shell.
  const NCollection_Vector<int>* SolidsOfShell(int theShellIdx) const
  {
    return seekVec(myShellToSolids, theShellIdx);
  }

  // --- Safe reference accessors (return empty vector instead of nullptr) ---

  //! Return wire indices containing the given edge (safe reference, never null).
  const NCollection_Vector<int>& WiresOfEdgeRef(int theEdgeIdx) const
  {
    return seekRef(myEdgeToWires, theEdgeIdx);
  }

  //! Return face indices containing the given edge (safe reference, never null).
  const NCollection_Vector<int>& FacesOfEdgeRef(int theEdgeIdx) const
  {
    return seekRef(myEdgeToFaces, theEdgeIdx);
  }

  //! Return face indices containing the given wire (safe reference, never null).
  const NCollection_Vector<int>& FacesOfWireRef(int theWireIdx) const
  {
    return seekRef(myWireToFaces, theWireIdx);
  }

  //! Verify reverse index consistency against forward entity tables.
  //! For each forward ref (e.g., wire->edge), checks that the corresponding
  //! reverse entry exists (edge->wire). Intended for debug validation.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool Validate(
    const NCollection_Vector<BRepGraphInc::EdgeEntity>&  theEdges,
    const NCollection_Vector<BRepGraphInc::WireEntity>&  theWires,
    const NCollection_Vector<BRepGraphInc::FaceEntity>&  theFaces,
    const NCollection_Vector<BRepGraphInc::ShellEntity>& theShells,
    const NCollection_Vector<BRepGraphInc::SolidEntity>& theSolids) const;

  // --- Incremental mutation ---

  //! Register an edge as belonging to a wire (O(1) amortized).
  Standard_EXPORT void BindEdgeToWire(int theEdgeIdx, int theWireIdx);

  //! Remove a wire from the edge-to-wire index for a given edge.
  Standard_EXPORT void UnbindEdgeFromWire(int theEdgeIdx, int theWireIdx);

  //! Replace an edge in the edge-to-wire index for a specific wire.
  Standard_EXPORT void ReplaceEdgeInWireMap(int theOldEdgeIdx, int theNewEdgeIdx, int theWireIdx);

  //! Register a vertex as incident to an edge (O(1) amortized, deduplicates).
  Standard_EXPORT void BindVertexToEdge(int theVertexIdx, int theEdgeIdx);

  //! Remove an edge from the vertex-to-edge index for a given vertex.
  Standard_EXPORT void UnbindVertexFromEdge(int theVertexIdx, int theEdgeIdx);

  //! Register an edge as belonging to a face (O(1) amortized, deduplicates).
  Standard_EXPORT void BindEdgeToFace(int theEdgeIdx, int theFaceIdx);

private:
  //! Dense vector type: outer index = entity key, inner vector = adjacency list.
  using IndexTable = NCollection_Vector<NCollection_Vector<int>>;

  //! Bounds-checked lookup returning nullptr for out-of-range or empty slots.
  static const NCollection_Vector<int>* seekVec(const IndexTable& theIdx, int theKey)
  {
    if (theKey < 0 || theKey >= theIdx.Length())
      return nullptr;
    const NCollection_Vector<int>& aVec = theIdx.Value(theKey);
    return aVec.IsEmpty() ? nullptr : &aVec;
  }

  //! Bounds-checked lookup returning a const reference (empty vector for missing keys).
  static const NCollection_Vector<int>& seekRef(const IndexTable& theIdx, int theKey)
  {
    const NCollection_Vector<int>* aPtr = seekVec(theIdx, theKey);
    return aPtr != nullptr ? *aPtr : THE_EMPTY_VEC;
  }

  static const NCollection_Vector<int> THE_EMPTY_VEC; //!< Shared empty vector for safe ref returns.

  //! Ensure theIdx has at least theSize slots (pre-sizing with empty vectors).
  static void preSize(IndexTable& theIdx, int theSize);

  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  static void appendUnique(IndexTable& theIdx, int theKey, int theVal);

  //! Add theVal to the vector at theKey unconditionally (no duplicate check).
  //! Used during Build() where freshly-cleared indices guarantee no duplicates.
  static void appendDirect(IndexTable& theIdx, int theKey, int theVal);

  Handle(NCollection_BaseAllocator) myAllocator;

  IndexTable myEdgeToWires;
  IndexTable myEdgeToFaces;
  IndexTable myVertexToEdges;
  IndexTable myWireToFaces;
  IndexTable myFaceToShells;
  IndexTable myShellToSolids;

  NCollection_Vector<int> myEdgeFaceCount; //!< Cached face count per edge, O(1) lookup.
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
