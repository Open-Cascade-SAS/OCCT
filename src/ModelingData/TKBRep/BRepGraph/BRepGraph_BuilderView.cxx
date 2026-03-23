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
  BRepGraph_TopoNode::VertexDef& aVtxDef = myGraph->myData->myIncStorage.AppendVertex();
  const int                      aIdx    = myGraph->myData->myIncStorage.NbVertices() - 1;
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
  BRepGraph_TopoNode::EdgeDef& anEdgeDef = myGraph->myData->myIncStorage.AppendEdge();
  const int                    aIdx      = myGraph->myData->myIncStorage.NbEdges() - 1;
  anEdgeDef.Id             = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  anEdgeDef.StartVertexIdx = theStartVtx.IsValid() ? theStartVtx.Index : -1;
  anEdgeDef.EndVertexIdx   = theEndVtx.IsValid() ? theEndVtx.Index : -1;
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
  BRepGraph_TopoNode::WireDef& aWireDef = myGraph->myData->myIncStorage.AppendWire();
  const int                    aIdx     = myGraph->myData->myIncStorage.NbWires() - 1;
  aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_NodeId     anEdgeDefId = theEdges.Value(anEdgeIdx).first;
    const TopAbs_Orientation   anOri       = theEdges.Value(anEdgeIdx).second;

    // EdgeRef on WireEntity.
    BRepGraphInc::EdgeRef anEdgeRef;
    anEdgeRef.EdgeIdx     = anEdgeDefId.Index;
    anEdgeRef.Orientation = anOri;
    myGraph->myData->myIncStorage.ChangeWire(aIdx).EdgeRefs.Append(anEdgeRef);

    myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToWire(anEdgeDefId.Index, aIdx);
  }

  // Check closure.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_NodeId   aFirstDefId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri   = theEdges.First().second;
    const BRepGraph_NodeId   aLastDefId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri    = theEdges.Last().second;

    const BRepGraph_TopoNode::EdgeDef& aFirstEdge =
      myGraph->myData->myIncStorage.Edge(aFirstDefId.Index);
    const BRepGraph_TopoNode::EdgeDef& aLastEdge =
      myGraph->myData->myIncStorage.Edge(aLastDefId.Index);

    BRepGraph_NodeId aFirstVtx = (aFirstOri == TopAbs_FORWARD)
                                   ? aFirstEdge.StartVertexDefId()
                                   : aFirstEdge.EndVertexDefId();
    BRepGraph_NodeId aLastVtx  = (aLastOri == TopAbs_FORWARD)
                                   ? aLastEdge.EndVertexDefId()
                                   : aLastEdge.StartVertexDefId();

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    myGraph->myData->myIncStorage.ChangeWire(aIdx).IsClosed = aIsClosed;
  }

  return myGraph->myData->myIncStorage.Wire(aIdx).Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFaceDef(
  const Handle(Geom_Surface)&                 theSurface,
  BRepGraph_NodeId                            theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  double                                      theTolerance)
{
  BRepGraph_TopoNode::FaceDef& aFaceDef = myGraph->myData->myIncStorage.AppendFace();
  const int                    aIdx     = myGraph->myData->myIncStorage.NbFaces() - 1;
  aFaceDef.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx);
  aFaceDef.Tolerance = theTolerance;
  myGraph->allocateUID(aFaceDef.Id);
  aFaceDef.Surface = theSurface;

  // Link wire refs.
  if (theOuterWire.IsValid())
  {
    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireIdx = theOuterWire.Index;
    aWireRef.IsOuter = true;
    myGraph->myData->myIncStorage.ChangeFace(aIdx).WireRefs.Append(aWireRef);
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;

    BRepGraphInc::WireRef aWireRef;
    aWireRef.WireIdx = aWireDefId.Index;
    aWireRef.IsOuter = false;
    myGraph->myData->myIncStorage.ChangeFace(aIdx).WireRefs.Append(aWireRef);
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShellDef()
{
  BRepGraph_TopoNode::ShellDef& aShellDef = myGraph->myData->myIncStorage.AppendShell();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbShells() - 1;
  aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);

  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolidDef()
{
  BRepGraph_TopoNode::SolidDef& aSolidDef = myGraph->myData->myIncStorage.AppendSolid();
  const int                     aIdx      = myGraph->myData->myIncStorage.NbSolids() - 1;
  aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);

  return aSolidDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                            BRepGraph_NodeId   theFaceDef,
                                            TopAbs_Orientation theOri)
{
  // Append FaceRef to shell entity.
  if (theShellDef.Index >= 0
      && theShellDef.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(theShellDef.Index).IsRemoved
      && theFaceDef.Index >= 0
      && theFaceDef.Index < myGraph->myData->myIncStorage.NbFaces()
      && !myGraph->myData->myIncStorage.Face(theFaceDef.Index).IsRemoved)
  {
    BRepGraphInc::FaceRef aFR;
    aFR.FaceIdx     = theFaceDef.Index;
    aFR.Orientation = theOri;
    myGraph->myData->myIncStorage.ChangeShell(theShellDef.Index).FaceRefs.Append(aFR);
  }

  myGraph->markModified(theShellDef);
}

//=================================================================================================

void BRepGraph::BuilderView::AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                             BRepGraph_NodeId   theShellDef,
                                             TopAbs_Orientation theOri)
{
  // Append ShellRef to solid entity.
  if (theSolidDef.Index >= 0
      && theSolidDef.Index < myGraph->myData->myIncStorage.NbSolids()
      && !myGraph->myData->myIncStorage.Solid(theSolidDef.Index).IsRemoved
      && theShellDef.Index >= 0
      && theShellDef.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(theShellDef.Index).IsRemoved)
  {
    BRepGraphInc::ShellRef aSR;
    aSR.ShellIdx    = theShellDef.Index;
    aSR.Orientation = theOri;
    myGraph->myData->myIncStorage.ChangeSolid(theSolidDef.Index).ShellRefs.Append(aSR);
  }

  myGraph->markModified(theSolidDef);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompoundDef(
  const NCollection_Vector<BRepGraph_NodeId>& theChildDefs)
{
  BRepGraph_TopoNode::CompoundDef& aCompDef = myGraph->myData->myIncStorage.AppendCompound();
  const int                        aIdx     = myGraph->myData->myIncStorage.NbCompounds() - 1;
  aCompDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildDefs.Length(); ++aChildIdx)
  {
    const BRepGraph_NodeId& aChild = theChildDefs.Value(aChildIdx);
    BRepGraphInc::ChildRef aCR;
    aCR.Kind     = static_cast<int>(aChild.NodeKind);
    aCR.ChildIdx = aChild.Index;
    aCompDef.ChildRefs.Append(aCR);
  }

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolidDef(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs)
{
  BRepGraph_TopoNode::CompSolidDef& aCSolDef = myGraph->myData->myIncStorage.AppendCompSolid();
  const int                         aIdx     = myGraph->myData->myIncStorage.NbCompSolids() - 1;
  aCSolDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidDefs.Length(); ++aSolIdx)
  {
    BRepGraphInc::SolidRef aSR;
    aSR.SolidIdx = theSolidDefs.Value(aSolIdx).Index;
    aCSolDef.SolidRefs.Append(aSR);
  }

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
  if (aDef != nullptr && !aDef->IsRemoved)
  {
    aDef->IsRemoved = true;
    myGraph->myData->myIncStorage.DecrementActiveCount(theNode.NodeKind);
  }

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

  // Traverse children via incidence refs.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompounds())
      {
        const BRepGraphInc::CompoundEntity& aComp =
          myGraph->myData->myIncStorage.Compound(theNode.Index);
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
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompSolids())
      {
        const BRepGraphInc::CompSolidEntity& aCSol =
          myGraph->myData->myIncStorage.CompSolid(theNode.Index);
        for (int i = 0; i < aCSol.SolidRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Solid(aCSol.SolidRefs.Value(i).SolidIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbSolids())
      {
        const BRepGraphInc::SolidEntity& aSolid =
          myGraph->myData->myIncStorage.Solid(theNode.Index);
        for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Shell(aSolid.ShellRefs.Value(i).ShellIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbShells())
      {
        const BRepGraphInc::ShellEntity& aShell =
          myGraph->myData->myIncStorage.Shell(theNode.Index);
        for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Face(aShell.FaceRefs.Value(i).FaceIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbFaces())
      {
        const BRepGraphInc::FaceEntity& aFace =
          myGraph->myData->myIncStorage.Face(theNode.Index);
        for (int i = 0; i < aFace.WireRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Wire(aFace.WireRefs.Value(i).WireIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbWires())
      {
        const BRepGraphInc::WireEntity& aWire =
          myGraph->myData->myIncStorage.Wire(theNode.Index);
        for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Edge(aWire.EdgeRefs.Value(i).EdgeIdx));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbEdges())
      {
        const BRepGraphInc::EdgeEntity& anEdge =
          myGraph->myData->myIncStorage.Edge(theNode.Index);
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
  // Direct PCurve-based path: valid during/after build when PCurves and wires are consistent.
  const NCollection_Vector<int>* aFaces =
    myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdge(theEdgeDefIdx);
  return aFaces != nullptr ? aFaces->Length() : 0;
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::BuilderView::WiresOfEdge(int theEdgeDefIdx) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdgeRef(theEdgeDefIdx);
}
