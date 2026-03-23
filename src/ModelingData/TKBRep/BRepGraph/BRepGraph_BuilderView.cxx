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
  // Defs is a reference to myIncStorage.Vertices — single append.
  BRepGraph_TopoNode::VertexDef& aVtxDef = myGraph->myData->myVertices.Defs.Appended();
  const int                      aIdx    = myGraph->myData->myVertices.Defs.Length() - 1;
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
  BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph->myData->myEdges.Defs.Appended();
  const int                    aIdx      = myGraph->myData->myEdges.Defs.Length() - 1;
  anEdgeDef.Id               = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  anEdgeDef.StartVertexDefId = theStartVtx;
  anEdgeDef.EndVertexDefId   = theEndVtx;
  anEdgeDef.StartVertexIdx   = theStartVtx.IsValid() ? theStartVtx.Index : -1;
  anEdgeDef.EndVertexIdx     = theEndVtx.IsValid() ? theEndVtx.Index : -1;
  anEdgeDef.ParamFirst       = theFirst;
  anEdgeDef.ParamLast        = theLast;
  anEdgeDef.Tolerance        = theTolerance;
  anEdgeDef.SameParameter    = true;
  anEdgeDef.SameRange        = true;
  myGraph->allocateUID(anEdgeDef.Id);

  if (!theCurve.IsNull())
    anEdgeDef.Curve3d = theCurve;

  return anEdgeDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddWireDef(
  const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges)
{
  BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myWires.Defs.Appended();
  const int                    aIdx     = myGraph->myData->myWires.Defs.Length() - 1;
  aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  BRepGraph_TopoNode::WireUsage& aWireUsage = myGraph->myData->myWires.Usages.Appended();
  const int                      aWireUsageIdx = myGraph->myData->myWires.Usages.Length() - 1;
  aWireUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWireUsageIdx);
  aWireUsage.DefId   = aWireDef.Id;
  myGraph->myData->myWires.Defs.ChangeValue(aIdx).Usages.Append(aWireUsage.UsageId);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_NodeId     anEdgeDefId = theEdges.Value(anEdgeIdx).first;
    const TopAbs_Orientation   anOri       = theEdges.Value(anEdgeIdx).second;

    // EdgeUsage.
    BRepGraph_TopoNode::EdgeUsage& anEdgeUsage = myGraph->myData->myEdges.Usages.Appended();
    const int anEdgeUsageIdx = myGraph->myData->myEdges.Usages.Length() - 1;
    anEdgeUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, anEdgeUsageIdx);
    anEdgeUsage.DefId       = anEdgeDefId;
    anEdgeUsage.Orientation = anOri;
    anEdgeUsage.ParentUsage = aWireUsage.UsageId;
    myGraph->myData->myEdges.Defs.ChangeValue(anEdgeDefId.Index)
      .Usages.Append(anEdgeUsage.UsageId);
    aWireUsage.EdgeUsages.Append(anEdgeUsage.UsageId);
    BRepGraph_BackRefManager::BindEdgeToWire(*myGraph, anEdgeDefId.Index, aIdx);

    // EdgeRef on WireEntity.
    BRepGraphInc::EdgeRef anEdgeRef;
    anEdgeRef.EdgeIdx     = anEdgeDefId.Index;
    anEdgeRef.Orientation = anOri;
    myGraph->myData->myWires.Defs.ChangeValue(aIdx).EdgeRefs.Append(anEdgeRef);
  }

  // Check closure.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_NodeId   aFirstDefId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri   = theEdges.First().second;
    const BRepGraph_NodeId   aLastDefId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri    = theEdges.Last().second;

    const BRepGraph_TopoNode::EdgeDef& aFirstEdge =
      myGraph->myData->myEdges.Defs.Value(aFirstDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge =
      myGraph->myData->myEdges.Defs.Value(aLastDefId.Index);

    BRepGraph_NodeId aFirstVtx = (aFirstOri == TopAbs_FORWARD)
                                   ? aFirstEdge.StartVertexDefId
                                   : aFirstEdge.EndVertexDefId;
    BRepGraph_NodeId aLastVtx  = (aLastOri == TopAbs_FORWARD)
                                   ? aLastEdge.EndVertexDefId
                                   : aLastEdge.StartVertexDefId;

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    myGraph->myData->myWires.Usages.ChangeValue(aWireUsageIdx).IsClosed = aIsClosed;
    myGraph->myData->myWires.Defs.ChangeValue(aIdx).IsClosed = aIsClosed;
  }

  return myGraph->myData->myWires.Defs.Value(aIdx).Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFaceDef(
  const Handle(Geom_Surface)&                 theSurface,
  BRepGraph_NodeId                            theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  double                                      theTolerance)
{
  BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myFaces.Defs.Appended();
  const int                    aIdx     = myGraph->myData->myFaces.Defs.Length() - 1;
  aFaceDef.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx);
  aFaceDef.Tolerance = theTolerance;
  myGraph->allocateUID(aFaceDef.Id);
  aFaceDef.Surface = theSurface;

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaces.Usages.Appended();
  const int                      aFaceUsageIdx = myGraph->myData->myFaces.Usages.Length() - 1;
  aFaceUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsageIdx);
  aFaceUsage.DefId   = aFaceDef.Id;
  myGraph->myData->myFaces.Defs.ChangeValue(aIdx).Usages.Append(aFaceUsage.UsageId);

  // Link wire usages and wire refs.
  if (theOuterWire.IsValid())
  {
    const BRepGraph_TopoNode::WireDef& aOuterWireDef =
      myGraph->myData->myWires.Defs.Value(theOuterWire.Index);
    if (!aOuterWireDef.Usages.IsEmpty())
    {
      BRepGraph_UsageId aWireUsageId = aOuterWireDef.Usages.Value(0);
      myGraph->myData->myWires.Usages.ChangeValue(aWireUsageId.Index)
        .OwnerFaceUsage = aFaceUsage.UsageId;
      myGraph->myData->myFaces.Usages.ChangeValue(aFaceUsageIdx)
        .OuterWireUsage = aWireUsageId;
    }

    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireIdx = theOuterWire.Index;
    aWireRef.IsOuter = true;
    myGraph->myData->myFaces.Defs.ChangeValue(aIdx).WireRefs.Append(aWireRef);
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;
    const BRepGraph_TopoNode::WireDef& aInnerWireDef =
      myGraph->myData->myWires.Defs.Value(aWireDefId.Index);
    if (!aInnerWireDef.Usages.IsEmpty())
    {
      BRepGraph_UsageId aWireUsageId = aInnerWireDef.Usages.Value(0);
      myGraph->myData->myWires.Usages.ChangeValue(aWireUsageId.Index)
        .OwnerFaceUsage = aFaceUsage.UsageId;
      myGraph->myData->myFaces.Usages.ChangeValue(aFaceUsageIdx)
        .InnerWireUsages.Append(aWireUsageId);
    }

    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireIdx = aWireDefId.Index;
    aWireRef.IsOuter = false;
    myGraph->myData->myFaces.Defs.ChangeValue(aIdx).WireRefs.Append(aWireRef);
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShellDef()
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myShells.Defs.Appended();
  const int                     aIdx      = myGraph->myData->myShells.Defs.Length() - 1;
  aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);

  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolidDef()
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph->myData->mySolids.Defs.Appended();
  const int                     aIdx      = myGraph->myData->mySolids.Defs.Length() - 1;
  aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);

  return aSolidDef.Id;
}

//=================================================================================================

BRepGraph_UsageId BRepGraph::BuilderView::AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                                         BRepGraph_NodeId   theFaceDef,
                                                         TopAbs_Orientation theOri)
{
  // Append FaceRef to shell entity (Defs references incidence storage).
  if (theShellDef.Index >= 0
      && theShellDef.Index < myGraph->myData->myShells.Defs.Length()
      && !myGraph->myData->myShells.Defs.Value(theShellDef.Index).IsRemoved
      && theFaceDef.Index >= 0
      && theFaceDef.Index < myGraph->myData->myFaces.Defs.Length()
      && !myGraph->myData->myFaces.Defs.Value(theFaceDef.Index).IsRemoved)
  {
    BRepGraphInc::FaceRef aFR;
    aFR.FaceIdx     = theFaceDef.Index;
    aFR.Orientation = theOri;
    myGraph->myData->myShells.Defs.ChangeValue(theShellDef.Index).FaceRefs.Append(aFR);
  }

  // Shell usage.
  BRepGraph_TopoNode::ShellDef& aShellDef =
    myGraph->myData->myShells.Defs.ChangeValue(theShellDef.Index);
  BRepGraph_UsageId aShellUsageId;
  if (aShellDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph->myData->myShells.Usages.Appended();
    const int aShellUsIdx = myGraph->myData->myShells.Usages.Length() - 1;
    aShellUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsIdx);
    aShellUsage.DefId   = theShellDef;
    aShellDef.Usages.Append(aShellUsage.UsageId);
    aShellUsageId = aShellUsage.UsageId;
  }
  else
  {
    aShellUsageId = aShellDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::FaceUsage& aFaceUsage = myGraph->myData->myFaces.Usages.Appended();
  const int                      aFaceUsIdx = myGraph->myData->myFaces.Usages.Length() - 1;
  aFaceUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFaceUsIdx);
  aFaceUsage.DefId       = theFaceDef;
  aFaceUsage.Orientation = theOri;
  aFaceUsage.ParentUsage = aShellUsageId;
  myGraph->myData->myFaces.Defs.ChangeValue(theFaceDef.Index).Usages.Append(aFaceUsage.UsageId);
  myGraph->myData->myShells.Usages.ChangeValue(aShellUsageId.Index)
    .FaceUsages.Append(aFaceUsage.UsageId);

  const BRepGraph_TopoNode::FaceDef& aFaceDef =
    myGraph->myData->myFaces.Defs.Value(theFaceDef.Index);
  if (aFaceDef.Usages.Length() > 1)
  {
    auto cloneWireUsage = [&](BRepGraph_UsageId theSrcWireUsageId) -> BRepGraph_UsageId {
      const BRepGraph_TopoNode::WireUsage& aSrcWU =
        myGraph->myData->myWires.Usages.Value(theSrcWireUsageId.Index);
      BRepGraph_NodeId aWireDefId = aSrcWU.DefId;

      BRepGraph_TopoNode::WireUsage& aNewWU = myGraph->myData->myWires.Usages.Appended();
      const int aNewWUIdx = myGraph->myData->myWires.Usages.Length() - 1;
      aNewWU.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aNewWUIdx);
      aNewWU.DefId          = aWireDefId;
      aNewWU.OwnerFaceUsage = myGraph->myData->myFaces.Usages.Value(aFaceUsIdx).UsageId;
      aNewWU.IsClosed       = aSrcWU.IsClosed;
      myGraph->myData->myWires.Defs.ChangeValue(aWireDefId.Index)
        .Usages.Append(aNewWU.UsageId);

      for (int aEUIdx = 0; aEUIdx < aSrcWU.EdgeUsages.Length(); ++aEUIdx)
      {
        const BRepGraph_TopoNode::EdgeUsage& aSrcEU =
          myGraph->myData->myEdges.Usages.Value(aSrcWU.EdgeUsages.Value(aEUIdx).Index);
        BRepGraph_TopoNode::EdgeUsage& aNewEU = myGraph->myData->myEdges.Usages.Appended();
        const int aNewEUIdx = myGraph->myData->myEdges.Usages.Length() - 1;
        aNewEU.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, aNewEUIdx);
        aNewEU.DefId          = aSrcEU.DefId;
        aNewEU.Orientation    = aSrcEU.Orientation;
        aNewEU.LocalLocation  = aSrcEU.LocalLocation;
        aNewEU.GlobalLocation = aSrcEU.GlobalLocation;
        aNewEU.ParentUsage    = aNewWU.UsageId;
        aNewEU.StartVertexUsage = aSrcEU.StartVertexUsage;
        aNewEU.EndVertexUsage   = aSrcEU.EndVertexUsage;
        myGraph->myData->myEdges.Defs.ChangeValue(aSrcEU.DefId.Index)
          .Usages.Append(aNewEU.UsageId);
        aNewWU.EdgeUsages.Append(aNewEU.UsageId);
      }

      return aNewWU.UsageId;
    };

    const BRepGraph_TopoNode::FaceUsage& aFirstFaceUsage =
      myGraph->myData->myFaces.Usages.Value(aFaceDef.Usages.Value(0).Index);
    if (aFirstFaceUsage.OuterWireUsage.IsValid())
    {
      BRepGraph_UsageId aNewWUId = cloneWireUsage(aFirstFaceUsage.OuterWireUsage);
      myGraph->myData->myFaces.Usages.ChangeValue(aFaceUsIdx).OuterWireUsage = aNewWUId;
    }
    for (int aWIdx = 0; aWIdx < aFirstFaceUsage.InnerWireUsages.Length(); ++aWIdx)
    {
      BRepGraph_UsageId aNewWUId = cloneWireUsage(aFirstFaceUsage.InnerWireUsages.Value(aWIdx));
      myGraph->myData->myFaces.Usages.ChangeValue(aFaceUsIdx)
        .InnerWireUsages.Append(aNewWUId);
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
  // Append ShellRef to solid entity.
  if (theSolidDef.Index >= 0
      && theSolidDef.Index < myGraph->myData->mySolids.Defs.Length()
      && !myGraph->myData->mySolids.Defs.Value(theSolidDef.Index).IsRemoved
      && theShellDef.Index >= 0
      && theShellDef.Index < myGraph->myData->myShells.Defs.Length()
      && !myGraph->myData->myShells.Defs.Value(theShellDef.Index).IsRemoved)
  {
    BRepGraphInc::ShellRef aSR;
    aSR.ShellIdx    = theShellDef.Index;
    aSR.Orientation = theOri;
    myGraph->myData->mySolids.Defs.ChangeValue(theSolidDef.Index).ShellRefs.Append(aSR);
  }

  // Solid usage.
  BRepGraph_TopoNode::SolidDef& aSolidDef =
    myGraph->myData->mySolids.Defs.ChangeValue(theSolidDef.Index);
  BRepGraph_UsageId aSolidUsageId;
  if (aSolidDef.Usages.IsEmpty())
  {
    BRepGraph_TopoNode::SolidUsage& aSolidUsage = myGraph->myData->mySolids.Usages.Appended();
    const int aSolidUsIdx = myGraph->myData->mySolids.Usages.Length() - 1;
    aSolidUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSolidUsIdx);
    aSolidUsage.DefId   = theSolidDef;
    aSolidDef.Usages.Append(aSolidUsage.UsageId);
    aSolidUsageId = aSolidUsage.UsageId;
  }
  else
  {
    aSolidUsageId = aSolidDef.Usages.Value(0);
  }

  BRepGraph_TopoNode::ShellUsage& aShellUsage = myGraph->myData->myShells.Usages.Appended();
  const int                       aShellUsIdx = myGraph->myData->myShells.Usages.Length() - 1;
  aShellUsage.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aShellUsIdx);
  aShellUsage.DefId       = theShellDef;
  aShellUsage.Orientation = theOri;
  aShellUsage.ParentUsage = aSolidUsageId;
  myGraph->myData->myShells.Defs.ChangeValue(theShellDef.Index)
    .Usages.Append(aShellUsage.UsageId);
  myGraph->myData->mySolids.Usages.ChangeValue(aSolidUsageId.Index)
    .ShellUsages.Append(aShellUsage.UsageId);

  myGraph->markModified(theSolidDef);
  return aShellUsage.UsageId;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{
  BRepGraph_TopoNode::CompoundDef& aCompDef = myGraph->myData->myCompounds.Defs.Appended();
  const int                        aIdx     = myGraph->myData->myCompounds.Defs.Length() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildDefs.Length(); ++aChildIdx)
  {
    aCompDef.ChildDefIds.Append(theChildDefs.Value(aChildIdx));

    const BRepGraph_NodeId& aChild = theChildDefs.Value(aChildIdx);
    BRepGraphInc::ChildRef aCR;
    aCR.Kind     = static_cast<int>(aChild.NodeKind);
    aCR.ChildIdx = aChild.Index;
    aCompDef.ChildRefs.Append(aCR);
  }

  BRepGraph_TopoNode::CompoundUsage& aCompUsage = myGraph->myData->myCompounds.Usages.Appended();
  const int aCompUsIdx = myGraph->myData->myCompounds.Usages.Length() - 1;
  aCompUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCompUsIdx);
  aCompUsage.DefId   = aCompDef.Id;
  myGraph->myData->myCompounds.Defs.ChangeValue(aIdx).Usages.Append(aCompUsage.UsageId);

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{
  BRepGraph_TopoNode::CompSolidDef& aCSolDef = myGraph->myData->myCompSolids.Defs.Appended();
  const int                         aIdx     = myGraph->myData->myCompSolids.Defs.Length() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidDefs.Length(); ++aSolIdx)
  {
    aCSolDef.SolidDefIds.Append(theSolidDefs.Value(aSolIdx));

    BRepGraphInc::SolidRef aSR;
    aSR.SolidIdx = theSolidDefs.Value(aSolIdx).Index;
    aCSolDef.SolidRefs.Append(aSR);
  }

  BRepGraph_TopoNode::CompSolidUsage& aCSolUsage =
    myGraph->myData->myCompSolids.Usages.Appended();
  const int aCSolUsIdx = myGraph->myData->myCompSolids.Usages.Length() - 1;
  aCSolUsage.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSolUsIdx);
  aCSolUsage.DefId   = aCSolDef.Id;
  myGraph->myData->myCompSolids.Defs.ChangeValue(aIdx).Usages.Append(aCSolUsage.UsageId);

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
  if (!theNode.IsValid())
    return;

  // Mark removed on the entity (which is the sole definition store).
  BRepGraph_TopoNode::BaseDef* aDef =
    const_cast<BRepGraph_TopoNode::BaseDef*>(myGraph->TopoDef(theNode));
  if (aDef != nullptr)
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

  // Traverse children via incidence refs (on the entities themselves now).
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myCompounds.Defs.Length())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          myGraph->myData->myCompounds.Defs.Value(theNode.Index);
        for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(i);
          RemoveSubgraph(BRepGraph_NodeId(
            static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx));
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myCompSolids.Defs.Length())
      {
        const BRepGraphInc::CompSolidEntity& aCSol =
          myGraph->myData->myCompSolids.Defs.Value(theNode.Index);
        for (int i = 0; i < aCSol.SolidRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Solid(aCSol.SolidRefs.Value(i).SolidIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->mySolids.Defs.Length())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          myGraph->myData->mySolids.Defs.Value(theNode.Index);
        for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Shell(aSolid.ShellRefs.Value(i).ShellIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myShells.Defs.Length())
      {
        const BRepGraphInc::ShellEntity& aShell =
          myGraph->myData->myShells.Defs.Value(theNode.Index);
        for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Face(aShell.FaceRefs.Value(i).FaceIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myFaces.Defs.Length())
      {
        const BRepGraphInc::FaceEntity& aFace =
          myGraph->myData->myFaces.Defs.Value(theNode.Index);
        for (int i = 0; i < aFace.WireRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Wire(aFace.WireRefs.Value(i).WireIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myWires.Defs.Length())
      {
        const BRepGraphInc::WireEntity& aWire =
          myGraph->myData->myWires.Defs.Value(theNode.Index);
        for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Edge(aWire.EdgeRefs.Value(i).EdgeIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myEdges.Defs.Length())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          myGraph->myData->myEdges.Defs.Value(theNode.Index);
        if (anEdge.StartVertexIdx >= 0)
          RemoveNode(BRepGraph_NodeId::Vertex(anEdge.StartVertexIdx));
        if (anEdge.EndVertexIdx >= 0)
          RemoveNode(BRepGraph_NodeId::Vertex(anEdge.EndVertexIdx));
      }
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
  const NCollection_Vector<int>* aFaces =
    myGraph->myData->myIncStorage.ReverseIdx.FacesOfEdge(theEdgeDefIdx);
  return aFaces != nullptr ? aFaces->Length() : 0;
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::BuilderView::WiresOfEdge(int theEdgeDefIdx) const
{
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>* aResult =
    myGraph->myData->myIncStorage.ReverseIdx.WiresOfEdge(theEdgeDefIdx);
  return aResult != nullptr ? *aResult : THE_EMPTY;
}
