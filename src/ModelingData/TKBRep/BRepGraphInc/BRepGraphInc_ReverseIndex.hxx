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

#include <NCollection_DataMap.hxx>
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

  //! Return wire indices containing the given edge.
  const NCollection_Vector<int>* WiresOfEdge(int theEdgeIdx) const
  {
    return myEdgeToWires.Seek(theEdgeIdx);
  }

  //! Return face indices containing the given edge (from EdgeFaceGeom rows).
  const NCollection_Vector<int>* FacesOfEdge(int theEdgeIdx) const
  {
    return myEdgeToFaces.Seek(theEdgeIdx);
  }

  //! Return edge indices incident to the given vertex.
  const NCollection_Vector<int>* EdgesOfVertex(int theVertexIdx) const
  {
    return myVertexToEdges.Seek(theVertexIdx);
  }

  //! Return face indices containing the given wire.
  const NCollection_Vector<int>* FacesOfWire(int theWireIdx) const
  {
    return myWireToFaces.Seek(theWireIdx);
  }

  //! Return shell indices containing the given face.
  const NCollection_Vector<int>* ShellsOfFace(int theFaceIdx) const
  {
    return myFaceToShells.Seek(theFaceIdx);
  }

  //! Return solid indices containing the given shell.
  const NCollection_Vector<int>* SolidsOfShell(int theShellIdx) const
  {
    return myShellToSolids.Seek(theShellIdx);
  }

  // --- Incremental mutation ---

  //! Register an edge as belonging to a wire (O(1) amortized).
  Standard_EXPORT void BindEdgeToWire(int theEdgeIdx, int theWireIdx);

  //! Remove a wire from the edge-to-wire index for a given edge.
  Standard_EXPORT void UnbindEdgeFromWire(int theEdgeIdx, int theWireIdx);

  //! Replace an edge in the edge-to-wire index for a specific wire.
  Standard_EXPORT void ReplaceEdgeInWireMap(int theOldEdgeIdx, int theNewEdgeIdx, int theWireIdx);

private:
  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  static void appendUnique(NCollection_DataMap<int, NCollection_Vector<int>>& theMap,
                           int theKey, int theVal);

  //! Add theVal to the vector at theKey unconditionally (no duplicate check).
  //! Used during Build() where freshly-cleared indices guarantee no duplicates.
  static void appendDirect(NCollection_DataMap<int, NCollection_Vector<int>>& theMap,
                           int theKey, int theVal);

  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToFaces;
  NCollection_DataMap<int, NCollection_Vector<int>> myVertexToEdges;
  NCollection_DataMap<int, NCollection_Vector<int>> myWireToFaces;
  NCollection_DataMap<int, NCollection_Vector<int>> myFaceToShells;
  NCollection_DataMap<int, NCollection_Vector<int>> myShellToSolids;
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
