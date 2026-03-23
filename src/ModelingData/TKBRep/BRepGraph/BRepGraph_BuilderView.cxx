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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Builder.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_PackedMap.hxx>
#include <Poly_Triangulation.hxx>

#include <shared_mutex>

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddVertexDef(const gp_Pnt& thePoint, double theTolerance)
{
  BRepGraph_TopoNode::VertexDef& aVtxDef = myGraph->myData->myVertexDefs.Appended();
  const int                      aIdx    = myGraph->myData->myVertexDefs.Length() - 1;
  aVtxDef.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aIdx);
  aVtxDef.Point     = thePoint;
  aVtxDef.Tolerance = theTolerance;
  myGraph->allocateUID(aVtxDef.Id);
  return aVtxDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                                    BRepGraph_NodeId          theEndVtx,
                                                    const Handle(Geom_Curve)& theCurve,
                                                    double                    theFirst,
                                                    double                    theLast,
                                                    double                    theTolerance)
{
  BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph->myData->myEdgeDefs.Appended();
  const int                    aIdx      = myGraph->myData->myEdgeDefs.Length() - 1;
  anEdgeDef.Id               = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  anEdgeDef.StartVertexDefId = theStartVtx;
  anEdgeDef.EndVertexDefId   = theEndVtx;
  anEdgeDef.ParamFirst       = theFirst;
  anEdgeDef.ParamLast        = theLast;
  anEdgeDef.Tolerance        = theTolerance;
  anEdgeDef.SameParameter    = true;
  anEdgeDef.SameRange        = true;
  myGraph->allocateUID(anEdgeDef.Id);

  if (!theCurve.IsNull())
  {
    anEdgeDef.CurveNodeId = myGraph->registerCurve(theCurve);
    if (anEdgeDef.CurveNodeId.IsValid())
      BRepGraph_BackRefManager::BindEdgeToCurve(*myGraph, anEdgeDef.Id,
                                                 anEdgeDef.CurveNodeId.Index);
  }

  return anEdgeDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddWireDef(
  const NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry>& theEdges)
{
  BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myWireDefs.Appended();
  const int                    aIdx     = myGraph->myData->myWireDefs.Length() - 1;
  aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    aWireDef.OrderedEdges.Append(theEdges.Value(anEdgeIdx));

    const int anEdgeDefIdx = theEdges.Value(anEdgeIdx).EdgeDefId.Index;
    BRepGraph_BackRefManager::BindEdgeToWire(*myGraph, anEdgeDefIdx, aIdx);
  }

  // Check closure: first edge start == last edge end.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aFirst = theEdges.First();
    const BRepGraph_TopoNode::WireDef::EdgeEntry& aLast  = theEdges.Last();
    const BRepGraph_TopoNode::EdgeDef& aFirstEdge =
      myGraph->myData->myEdgeDefs.Value(aFirst.EdgeDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge =
      myGraph->myData->myEdgeDefs.Value(aLast.EdgeDefId.Index);

    BRepGraph_NodeId aFirstVtx = (aFirst.OrientationInWire == TopAbs_FORWARD)
                                   ? aFirstEdge.StartVertexDefId
                                   : aFirstEdge.EndVertexDefId;
    BRepGraph_NodeId aLastVtx  = (aLast.OrientationInWire == TopAbs_FORWARD)
                                   ? aLastEdge.EndVertexDefId
                                   : aLastEdge.StartVertexDefId;
    aWireDef.IsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
  }

  return aWireDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFaceDef(
  const Handle(Geom_Surface)&                 theSurface,
  BRepGraph_NodeId                            theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  double                                      theTolerance)
{
  BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaceDefs.Appended();
  const int                    aIdx     = myGraph->myData->myFaceDefs.Length() - 1;
  aFaceDef.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx);
  aFaceDef.Tolerance = theTolerance;
  myGraph->allocateUID(aFaceDef.Id);

  aFaceDef.SurfNodeId =
    myGraph->registerSurface(theSurface, Handle(Poly_Triangulation)());
  if (aFaceDef.SurfNodeId.IsValid())
    BRepGraph_BackRefManager::BindFaceToSurface(*myGraph, aFaceDef.Id,
                                                 aFaceDef.SurfNodeId.Index);

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaceUsages.Appended();
  const int                      aFaceUsageIdx = myGraph->myData->myFaceUsages.Length() - 1;
  aFaceUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsageIdx);
  aFaceUsage.DefId   = aFaceDef.Id;
  myGraph->myData->myFaceDefs.ChangeValue(aIdx).Usages.Append(aFaceUsage.UsageId);

  if (theOuterWire.IsValid())
  {
    BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph->myData->myWireUsages.Appended();
    const int                      aWireUsageIdx = myGraph->myData->myWireUsages.Length() - 1;
    aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsageIdx);
    aWireUsage.DefId          = theOuterWire;
    aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
    myGraph->myData->myWireDefs.ChangeValue(theOuterWire.Index)
      .Usages.Append(aWireUsage.UsageId);
    myGraph->myData->myFaceUsages.ChangeValue(aFaceUsageIdx).OuterWireUsage = aWireUsage.UsageId;
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;
    BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph->myData->myWireUsages.Appended();
    const int                      aWireUsageIdx = myGraph->myData->myWireUsages.Length() - 1;
    aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsageIdx);
    aWireUsage.DefId          = aWireDefId;
    aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
    myGraph->myData->myWireDefs.ChangeValue(aWireDefId.Index)
      .Usages.Append(aWireUsage.UsageId);
    myGraph->myData->myFaceUsages.ChangeValue(aFaceUsageIdx)
      .InnerWireUsages.Append(aWireUsage.UsageId);
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShellDef()
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShellDefs.Appended();
  const int                     aIdx      = myGraph->myData->myShellDefs.Length() - 1;
  aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);
  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolidDef()
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph->myData->mySolidDefs.Appended();
  const int                     aIdx      = myGraph->myData->mySolidDefs.Length() - 1;
  aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);
  return aSolidDef.Id;
}

//=================================================================================================

BRepGraph_UsageId BRepGraph::BuilderView::AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                         BRepGraph_NodeId   theFaceDef,
                                                         TopAbs_Orientation theOri)
{
  BRepGraph_TopoNode::ShellDef& aShellDef =
    myGraph->myData->myShellDefs.ChangeValue(theShellDef.Index);
  BRepGraph_UsageId aShellUsageId;
  if (aShellDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph->myData->myShellUsages.Appended();
    const int aShellUsIdx = myGraph->myData->myShellUsages.Length() - 1;
    aShellUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsIdx);
    aShellUsage.DefId   = theShellDef;
    aShellDef.Usages.Append(aShellUsage.UsageId);
    aShellUsageId = aShellUsage.UsageId;
  }
  else
  {
    aShellUsageId = aShellDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaceUsages.Appended();
  const int                      aFaceUsIdx = myGraph->myData->myFaceUsages.Length() - 1;
  aFaceUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsIdx);
  aFaceUsage.DefId       = theFaceDef;
  aFaceUsage.Orientation = theOri;
  aFaceUsage.ParentUsage = aShellUsageId;
  myGraph->myData->myFaceDefs.ChangeValue(theFaceDef.Index).Usages.Append(aFaceUsage.UsageId);
  myGraph->myData->myShellUsages.ChangeValue(aShellUsageId.Index)
    .FaceUsages.Append(aFaceUsage.UsageId);

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myFaceDefs.Value(theFaceDef.Index);
  if (aFaceDef.Usages.Length() > 1)
  {
    const BRepGraph_TopoNode::FaceUsage& aFirstFaceUsage =
      myGraph->myData->myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
    if (aFirstFaceUsage.OuterWireUsage.IsValid())
    {
      BRepGraph_NodeId aOuterWireDef = myGraph->DefOf(aFirstFaceUsage.OuterWireUsage);
      BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph->myData->myWireUsages.Appended();
      const int aWireUsIdx = myGraph->myData->myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsIdx);
      aWireUsage.DefId          = aOuterWireDef;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      myGraph->myData->myWireDefs.ChangeValue(aOuterWireDef.Index)
        .Usages.Append(aWireUsage.UsageId);
      myGraph->myData->myFaceUsages.ChangeValue(aFaceUsIdx).OuterWireUsage = aWireUsage.UsageId;
    }
    for (int aWIdx = 0; aWIdx < aFirstFaceUsage.InnerWireUsages.Length(); ++aWIdx)
    {
      BRepGraph_NodeId aInnerWireDef =
        myGraph->DefOf(aFirstFaceUsage.InnerWireUsages.Value(aWIdx));
      BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph->myData->myWireUsages.Appended();
      const int aWireUsIdx = myGraph->myData->myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsIdx);
      aWireUsage.DefId          = aInnerWireDef;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      myGraph->myData->myWireDefs.ChangeValue(aInnerWireDef.Index)
        .Usages.Append(aWireUsage.UsageId);
      myGraph->myData->myFaceUsages.ChangeValue(aFaceUsIdx)
        .InnerWireUsages.Append(aWireUsage.UsageId);
    }
  }

  myGraph->markModified(theShellDef);
  return aFaceUsage.UsageId;
}

//=================================================================================================

BRepGraph_UsageId BRepGraph::BuilderView::AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                                          BRepGraph_NodeId   theShellDef,
                                                          TopAbs_Orientation theOri)
{
  BRepGraph_TopoNode::SolidDef& aSolidDef =
    myGraph->myData->mySolidDefs.ChangeValue(theSolidDef.Index);
  BRepGraph_UsageId aSolidUsageId;
  if (aSolidDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::SolidUsage& aSolidUsage = myGraph->myData->mySolidUsages.Appended();
    const int aSolidUsIdx = myGraph->myData->mySolidUsages.Length() - 1;
    aSolidUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSolidUsIdx);
    aSolidUsage.DefId   = theSolidDef;
    aSolidDef.Usages.Append(aSolidUsage.UsageId);
    aSolidUsageId = aSolidUsage.UsageId;
  }
  else
  {
    aSolidUsageId = aSolidDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph->myData->myShellUsages.Appended();
  const int                       aShellUsIdx = myGraph->myData->myShellUsages.Length() - 1;
  aShellUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsIdx);
  aShellUsage.DefId       = theShellDef;
  aShellUsage.Orientation = theOri;
  aShellUsage.ParentUsage = aSolidUsageId;
  myGraph->myData->myShellDefs.ChangeValue(theShellDef.Index)
    .Usages.Append(aShellUsage.UsageId);
  myGraph->myData->mySolidUsages.ChangeValue(aSolidUsageId.Index)
    .ShellUsages.Append(aShellUsage.UsageId);

  myGraph->markModified(theSolidDef);
  return aShellUsage.UsageId;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{
  BRepGraph_TopoNode::CompoundDef& aCompDef = myGraph->myData->myCompoundDefs.Appended();
  const int                        aIdx     = myGraph->myData->myCompoundDefs.Length() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildDefs.Length(); ++aChildIdx)
    aCompDef.ChildDefIds.Append(theChildDefs.Value(aChildIdx));

  BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph->myData->myCompoundUsages.Appended();
  const int aCompUsIdx = myGraph->myData->myCompoundUsages.Length() - 1;
  aCompUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCompUsIdx);
  aCompUsage.DefId   = aCompDef.Id;
  myGraph->myData->myCompoundDefs.ChangeValue(aIdx).Usages.Append(aCompUsage.UsageId);

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{
  BRepGraph_TopoNode::CompSolidDef& aCSolDef = myGraph->myData->myCompSolidDefs.Appended();
  const int                         aIdx     = myGraph->myData->myCompSolidDefs.Length() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidDefs.Length(); ++aSolIdx)
    aCSolDef.SolidDefIds.Append(theSolidDefs.Value(aSolIdx));

  BRepGraph_TopoNode::CompSolidUsage& aCSolUsage =
    myGraph->myData->myCompSolidUsages.Appended();
  const int aCSolUsIdx = myGraph->myData->myCompSolidUsages.Length() - 1;
  aCSolUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSolUsIdx);
  aCSolUsage.DefId   = aCSolDef.Id;
  myGraph->myData->myCompSolidDefs.ChangeValue(aIdx).Usages.Append(aCSolUsage.UsageId);

  return aCSolDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AppendShape(const TopoDS_Shape& theShape, bool theParallel)
{
  BRepGraph_Builder::Append(*myGraph, theShape, theParallel);
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(BRepGraph_NodeId theNode)
{
  BRepGraph_TopoNode::BaseDef* aDef =
    const_cast<BRepGraph_TopoNode::BaseDef*>(myGraph->TopoDef(theNode));
  if (aDef == nullptr)
    return;

  aDef->IsRemoved = true;

  BRepGraph_BackRefManager::ClearRelEdges(*myGraph, theNode);

  BRepGraph_NodeCache* aCache = myGraph->mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    myGraph->myData->myCurrentShapes.UnBind(theNode);
  }
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveSubgraph(BRepGraph_NodeId theNode)
{
  RemoveNode(theNode);

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraph_TopoNode::CompoundDef& aCompDef =
        myGraph->myData->myCompoundDefs.Value(theNode.Index);
      for (int aChildIdx = 0; aChildIdx < aCompDef.ChildDefIds.Length(); ++aChildIdx)
        RemoveSubgraph(aCompDef.ChildDefIds.Value(aChildIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraph_TopoNode::CompSolidDef& aCSolDef =
        myGraph->myData->myCompSolidDefs.Value(theNode.Index);
      for (int aSolIdx = 0; aSolIdx < aCSolDef.SolidDefIds.Length(); ++aSolIdx)
        RemoveSubgraph(aCSolDef.SolidDefIds.Value(aSolIdx));
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef =
        myGraph->myData->mySolidDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          myGraph->myData->mySolidUsages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
          RemoveSubgraph(myGraph->DefOf(aUsage.ShellUsages.Value(aShellIter)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef =
        myGraph->myData->myShellDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myGraph->myData->myShellUsages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
          RemoveSubgraph(myGraph->DefOf(aUsage.FaceUsages.Value(aFaceIter)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef =
        myGraph->myData->myFaceDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myGraph->myData->myFaceUsages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          RemoveSubgraph(myGraph->DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          RemoveSubgraph(myGraph->DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef =
        myGraph->myData->myWireDefs.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        BRepGraph_NodeId anEdgeDefId = aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId;
        RemoveSubgraph(anEdgeDefId);
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef =
        myGraph->myData->myEdgeDefs.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId.IsValid())
        RemoveNode(anEdgeDef.StartVertexDefId);
      if (anEdgeDef.EndVertexDefId.IsValid())
        RemoveNode(anEdgeDef.EndVertexDefId);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

bool BRepGraph::BuilderView::IsRemoved(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

//=================================================================================================

int BRepGraph::BuilderView::FaceCountForEdge(int theEdgeDefIdx) const
{
  const NCollection_Vector<int>& aWires = WiresOfEdge(theEdgeDefIdx);
  NCollection_PackedMap<int>     aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef =
      myGraph->myData->myWireDefs.Value(aWires.Value(aWIdx));
    for (int aUsIdx = 0; aUsIdx < aWireDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myGraph->myData->myWireUsages.Value(aWireDef.Usages.Value(aUsIdx).Index);
      if (aWireUsage.OwnerFaceUsage.IsValid())
      {
        BRepGraph_NodeId aFaceDefId = myGraph->DefOf(aWireUsage.OwnerFaceUsage);
        if (aFaceDefId.IsValid())
          aFaceSet.Add(aFaceDefId.Index);
      }
    }
  }
  return aFaceSet.Extent();
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::BuilderView::WiresOfEdge(int theEdgeDefIdx) const
{
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>* aResult = myGraph->myData->myEdgeToWires.Seek(theEdgeDefIdx);
  return aResult != nullptr ? *aResult : THE_EMPTY;
}
