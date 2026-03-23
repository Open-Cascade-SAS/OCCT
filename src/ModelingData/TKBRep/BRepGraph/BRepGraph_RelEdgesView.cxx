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

#include <BRepGraph_RelEdgesView.hxx>
#include <BRepGraph_Data.hxx>

#include <NCollection_PackedMap.hxx>

//=================================================================================================

int BRepGraph::RelEdgesView::NbFrom(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges =
    myGraph->myData->myOutRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

//=================================================================================================

int BRepGraph::RelEdgesView::NbTo(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges =
    myGraph->myData->myInRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_RelEdge>* BRepGraph::RelEdgesView::OutOf(
  BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myOutRelEdges.Seek(theNode);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_RelEdge>* BRepGraph::RelEdgesView::InOf(
  BRepGraph_NodeId theNode) const
{
  return myGraph->myData->myInRelEdges.Seek(theNode);
}

//=================================================================================================

int BRepGraph::RelEdgesView::FaceCountForEdge(int theEdgeDefIdx) const
{
  // Use the wire-based (topological) path: edge → wires → faces.
  // The direct PCurve-based edge→faces path may disagree after mutations
  // (e.g., sewing) where edges are placed into wires without full PCurve registration.
  const BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ReverseIndex();
  const NCollection_Vector<int>& aWires = aRevIdx.WiresOfEdgeRef(theEdgeDefIdx);
  NCollection_PackedMap<int> aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const NCollection_Vector<int>& aFaces = aRevIdx.FacesOfWireRef(aWires.Value(aWIdx));
    for (int aFIdx = 0; aFIdx < aFaces.Length(); ++aFIdx)
      aFaceSet.Add(aFaces.Value(aFIdx));
  }
  return aFaceSet.Extent();
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::RelEdgesView::WiresOfEdge(int theEdgeDefIdx) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdgeRef(theEdgeDefIdx);
}
