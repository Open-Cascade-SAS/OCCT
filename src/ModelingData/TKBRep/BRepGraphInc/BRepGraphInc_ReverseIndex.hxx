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
struct EdgeFaceGeom;
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

  //! Rebuild all reverse indices from the entity tables and relationship rows.
  //! @param[in] theEdges     edge entity vector (for vertex-to-edge)
  //! @param[in] theWires     wire entity vector (for edge-to-wire)
  //! @param[in] theFaces     face entity vector (for wire-to-face)
  //! @param[in] theEdgeFaceGeoms  relationship geometry rows (for edge-to-face)
  Standard_EXPORT void Build(const NCollection_Vector<BRepGraphInc::EdgeEntity>&   theEdges,
                             const NCollection_Vector<BRepGraphInc::WireEntity>&   theWires,
                             const NCollection_Vector<BRepGraphInc::FaceEntity>&   theFaces,
                             const NCollection_Vector<BRepGraphInc::EdgeFaceGeom>& theEdgeFaceGeoms);

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

private:
  //! Add theVal to the vector at theKey, creating if needed.  Skips duplicates.
  static void appendUnique(NCollection_DataMap<int, NCollection_Vector<int>>& theMap,
                           int theKey, int theVal);

  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToWires;
  NCollection_DataMap<int, NCollection_Vector<int>> myEdgeToFaces;
  NCollection_DataMap<int, NCollection_Vector<int>> myVertexToEdges;
  NCollection_DataMap<int, NCollection_Vector<int>> myWireToFaces;
};

#endif // _BRepGraphInc_ReverseIndex_HeaderFile
