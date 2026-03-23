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

gp_Trsf BRepGraph::SpatialView::GlobalTransform(BRepGraph_UsageId theUsage) const
{
  if (!theUsage.IsValid())
    return gp_Trsf();

  switch (theUsage.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Solid:
      return myGraph->myData->mySolids.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Shell:
      return myGraph->myData->myShells.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Face:
      return myGraph->myData->myFaces.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Wire:
      return myGraph->myData->myWires.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Edge:
      return myGraph->myData->myEdges.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Vertex:
      return myGraph->myData->myVertices.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::Compound:
      return myGraph->myData->myCompounds.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeId::Kind::CompSolid:
      return myGraph->myData->myCompSolids.Usages.Value(theUsage.Index).GlobalLocation.Transformation();
    default: return gp_Trsf();
  }
}

//=================================================================================================

gp_Trsf BRepGraph::SpatialView::GlobalTransform(BRepGraph_NodeId theDefId) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theDefId);
  if (aDef == nullptr || aDef->Usages.IsEmpty())
    return gp_Trsf();
  return GlobalTransform(aDef->Usages.Value(0));
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SameDomainFaces(
  BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
  if (aFaceDef.Surface.IsNull())
    return aResult;

  // Scan all face defs for matching surface
  for (int aFaceIdx = 0; aFaceIdx < myGraph->myData->myFaces.Defs.Length(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aOtherFace = myGraph->myData->myFaces.Defs.Value(aFaceIdx);
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
  const NCollection_Vector<int>*       aWiresPtr =
    myGraph->myData->myEdgeToWires.Seek(theEdgeDef.Index);
  const NCollection_Vector<int>& aWires = aWiresPtr != nullptr ? *aWiresPtr : THE_EMPTY;

  NCollection_PackedMap<int> aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myWires.Defs.Value(aWires.Value(aWIdx));
    for (int aUsIdx = 0; aUsIdx < aWireDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myGraph->myData->myWires.Usages.Value(aWireDef.Usages.Value(aUsIdx).Index);
      if (aWireUsage.OwnerFaceUsage.IsValid())
      {
        BRepGraph_NodeId aFaceDefId = myGraph->DefOf(aWireUsage.OwnerFaceUsage);
        if (aFaceDefId.IsValid() && aFaceSet.Add(aFaceDefId.Index))
          aResult.Append(aFaceDefId);
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

  NCollection_PackedMap<int> aEdgesA;
  for (int aUsIdx = 0;
       aUsIdx < myGraph->myData->myFaces.Defs.Value(theFaceA.Index).Usages.Length();
       ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaces.Usages.Value(
      myGraph->myData->myFaces.Defs.Value(theFaceA.Index).Usages.Value(aUsIdx).Index);
    auto collectWireEdges = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myGraph->myData->myWires.Usages.Value(theWireUsageId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireUsage.EdgeUsages.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId anEdgeDefId = myGraph->DefOf(aWireUsage.EdgeUsages.Value(anEdgeIdx));
        aEdgesA.Add(anEdgeDefId.Index);
      }
    };
    collectWireEdges(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      collectWireEdges(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  NCollection_PackedMap<int> aAdded;
  for (int aUsIdx = 0;
       aUsIdx < myGraph->myData->myFaces.Defs.Value(theFaceB.Index).Usages.Length();
       ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaces.Usages.Value(
      myGraph->myData->myFaces.Defs.Value(theFaceB.Index).Usages.Value(aUsIdx).Index);
    auto checkWireEdges = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myGraph->myData->myWires.Usages.Value(theWireUsageId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireUsage.EdgeUsages.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId anEdgeDefId = myGraph->DefOf(aWireUsage.EdgeUsages.Value(anEdgeIdx));
        if (aEdgesA.Contains(anEdgeDefId.Index) && aAdded.Add(anEdgeDefId.Index))
          aResult.Append(anEdgeDefId);
      }
    };
    checkWireEdges(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      checkWireEdges(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentFaces(
  BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;

  for (int aUsIdx = 0;
       aUsIdx < myGraph->myData->myFaces.Defs.Value(theFaceDef.Index).Usages.Length();
       ++aUsIdx)
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaces.Usages.Value(
      myGraph->myData->myFaces.Defs.Value(theFaceDef.Index).Usages.Value(aUsIdx).Index);
    auto processWire = [&](BRepGraph_UsageId theWireUsageId) {
      if (!theWireUsageId.IsValid())
        return;
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myGraph->myData->myWires.Usages.Value(theWireUsageId.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireUsage.EdgeUsages.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId                     anEdgeDefId = myGraph->DefOf(aWireUsage.EdgeUsages.Value(anEdgeIdx));
        NCollection_Vector<BRepGraph_NodeId> aFaces      = FacesOfEdge(anEdgeDefId);
        for (int aFIdx = 0; aFIdx < aFaces.Length(); ++aFIdx)
        {
          if (aFaces.Value(aFIdx) != theFaceDef && aFaceSet.Add(aFaces.Value(aFIdx).Index))
            aResult.Append(aFaces.Value(aFIdx));
        }
      }
    };
    processWire(aFaceUsage.OuterWireUsage);
    for (int aWIdx = 0; aWIdx < aFaceUsage.InnerWireUsages.Length(); ++aWIdx)
      processWire(aFaceUsage.InnerWireUsages.Value(aWIdx));
  }

  return aResult;
}
