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
#include <TopLoc_Location.hxx>

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::GlobalTransform(const BRepGraph_NodeId theDefId) const
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
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);
  if (aFaceDef.SurfaceRepIdx < 0)
    return aResult;

  // Scan all face defs for matching surface rep index
  for (int aFaceIdx = 0; aFaceIdx < myGraph->myData->myIncStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aOtherFace = myGraph->myData->myIncStorage.Face(aFaceIdx);
    if (aOtherFace.SurfaceRepIdx == aFaceDef.SurfaceRepIdx && aOtherFace.Id != theFaceDef)
      aResult.Append(aOtherFace.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::FacesOfEdge(
  const BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<int>* aFaces =
    myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdge(theEdgeDef.Index);
  if (aFaces != nullptr)
  {
    for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      aResult.Append(BRepGraph_NodeId::Face(aFaces->Value(aFIdx)));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SharedEdges(
  const BRepGraph_NodeId theFaceA,
  const BRepGraph_NodeId theFaceB) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDefA = myGraph->myData->myIncStorage.Face(theFaceA.Index);

  NCollection_PackedMap<int> aEdgesA;
  auto collectWireEdges = [&](int theWireDefIdx) {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(theWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      aEdgesA.Add(aCoEdge.EdgeIdx);
    }
  };
  for (int aWIdx = 0; aWIdx < aFaceDefA.WireRefs.Length(); ++aWIdx)
    collectWireEdges(aFaceDefA.WireRefs.Value(aWIdx).WireIdx);

  const BRepGraph_TopoNode::FaceDef& aFaceDefB = myGraph->myData->myIncStorage.Face(theFaceB.Index);

  NCollection_PackedMap<int> aAdded;
  auto checkWireEdges = [&](int theWireDefIdx) {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(theWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      const int anEdgeDefIdx = aCoEdge.EdgeIdx;
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
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myIncStorage.Face(theFaceDef.Index);

  const BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ReverseIndex();
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const int aWireDefIdx = aFaceDef.WireRefs.Value(aWireRefIdx).WireIdx;
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myIncStorage.Wire(aWireDefIdx);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        myGraph->myData->myIncStorage.CoEdge(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx);
      const int anEdgeDefIdx = aCoEdge.EdgeIdx;
      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfEdge(anEdgeDefIdx);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const int aFaceIdx = aFaces->Value(aFIdx);
          if (aFaceIdx != theFaceDef.Index && aFaceSet.Add(aFaceIdx))
            aResult.Append(BRepGraph_NodeId::Face(aFaceIdx));
        }
      }
    }
  }

  return aResult;
}

//=================================================================================================

TopLoc_Location BRepGraph::SpatialView::GlobalPlacement(const int theOccurrenceIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theOccurrenceIdx < 0 || theOccurrenceIdx >= aStorage.NbOccurrences())
    return TopLoc_Location();

  // Compose placements from leaf to root via ParentOccurrenceIdx tree.
  // global = ancestor_placement * ... * parent_placement * leaf_placement.
  // Pre-multiplying each ancestor's placement accumulates the correct transform.
  TopLoc_Location aGlobal = aStorage.Occurrence(theOccurrenceIdx).Placement;
  int aParentOccIdx = aStorage.Occurrence(theOccurrenceIdx).ParentOccurrenceIdx;

  // In a valid tree, the chain length cannot exceed NbOccurrences.
  // This naturally guards against cycles in malformed graphs.
  const int aNbOccurrences = aStorage.NbOccurrences();
  int aSteps = 0;

  while (aParentOccIdx >= 0 && aParentOccIdx < aNbOccurrences && aSteps < aNbOccurrences)
  {
    ++aSteps;
    const BRepGraphInc::OccurrenceEntity& aParentOcc = aStorage.Occurrence(aParentOccIdx);
    if (aParentOcc.IsRemoved)
      break;
    aGlobal = aParentOcc.Placement * aGlobal;
    aParentOccIdx = aParentOcc.ParentOccurrenceIdx;
  }

  return aGlobal;
}
