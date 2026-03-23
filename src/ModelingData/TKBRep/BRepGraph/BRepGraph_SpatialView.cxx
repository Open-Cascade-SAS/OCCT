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

#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_Data.hxx>

#include <NCollection_PackedMap.hxx>

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::GlobalTransform(BRepGraph_NodeId theDefId) const
{
  if (!theDefId.IsValid())
    return gp_Trsf();

  // Read per-node location from myNodeLocations if available.
  const TopLoc_Location* aNodeLoc = myGraph->myData->myNodeLocations.Seek(theDefId);
  if (aNodeLoc != nullptr)
    return aNodeLoc->Transformation();

  return gp_Trsf();
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SameDomainFaces(
  BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);
  if (aFaceDef.Surface.IsNull())
    return aResult;

  // Scan all face defs for matching surface
  for (int aFaceIdx = 0; aFaceIdx < myGraph->myData->myIncStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aOtherFace = myGraph->myData->myIncStorage.Face(aFaceIdx);
    if (aOtherFace.Surface.get() == aFaceDef.Surface.get() && aOtherFace.Id != theFaceDef)
      aResult.Append(aOtherFace.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::FacesOfEdge(
  BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>* aWiresPtr =
    myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdge(theEdgeDef.Index);
  const NCollection_Vector<int>& aWires = aWiresPtr != nullptr ? *aWiresPtr : THE_EMPTY;

  NCollection_PackedMap<int> aFaceSet;
  const BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ReverseIndex();
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(aWires.Value(aWIdx));
    if (aFaces != nullptr)
    {
      for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      {
        const int aFaceIdx = aFaces->Value(aFIdx);
        if (aFaceSet.Add(aFaceIdx))
          aResult.Append(BRepGraph_NodeId::Face(aFaceIdx));
      }
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SharedEdges(
  BRepGraph_NodeId theFaceA,
  BRepGraph_NodeId theFaceB) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDefA = myGraph->myData->myIncStorage.Face(theFaceA.Index);

  NCollection_PackedMap<int> aEdgesA;
  auto collectWireEdges = [&](int theWireDefIdx) {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(theWireDefIdx);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
    {
      aEdgesA.Add(aWireDef.EdgeRefs.Value(anEdgeIdx).EdgeIdx);
    }
  };
  for (int aWIdx = 0; aWIdx < aFaceDefA.WireRefs.Length(); ++aWIdx)
    collectWireEdges(aFaceDefA.WireRefs.Value(aWIdx).WireIdx);

  const BRepGraph_TopoNode::FaceDef& aFaceDefB = myGraph->myData->myIncStorage.Face(theFaceB.Index);

  NCollection_PackedMap<int> aAdded;
  auto checkWireEdges = [&](int theWireDefIdx) {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(theWireDefIdx);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
    {
      const int anEdgeDefIdx = aWireDef.EdgeRefs.Value(anEdgeIdx).EdgeIdx;
      if (aEdgesA.Contains(anEdgeDefIdx) && aAdded.Add(anEdgeDefIdx))
      {
        aResult.Append(myGraph->myData->myIncStorage.Edge(anEdgeDefIdx).Id);
      }
    }
  };
  for (int aWIdx = 0; aWIdx < aFaceDefB.WireRefs.Length(); ++aWIdx)
    checkWireEdges(aFaceDefB.WireRefs.Value(aWIdx).WireIdx);

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentFaces(
  BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);

  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const int aWireDefIdx = aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx;
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(aWireDefIdx);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
    {
      const int anEdgeDefIdx = aWireDef.EdgeRefs.Value(anEdgeIdx).EdgeIdx;
      BRepGraph_NodeId anEdgeDefId = myGraph->myData->myIncStorage.Edge(anEdgeDefIdx).Id;
      NCollection_Vector<BRepGraph_NodeId> aFaces = FacesOfEdge(anEdgeDefId);
      for (int aFIdx = 0; aFIdx < aFaces.Length(); ++aFIdx)
      {
        if (aFaces.Value(aFIdx) != theFaceDef && aFaceSet.Add(aFaces.Value(aFIdx).Index))
          aResult.Append(aFaces.Value(aFIdx));
      }
    }
  }

  return aResult;
}
