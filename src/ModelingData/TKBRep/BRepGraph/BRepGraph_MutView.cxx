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
#include <BRepGraph_MutView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Mutator.hxx>

#include <Geom2d_Curve.hxx>

//=================================================================================================

BRepGraph_TopoNode::EdgeDef& BRepGraph::MutView::EdgeDef(int theIdx)
{
  BRepGraph_TopoNode::EdgeDef& aDef = myGraph->myData->myIncStorage.ChangeEdge(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Edge(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::WireDef& BRepGraph::MutView::WireDef(int theIdx)
{
  BRepGraph_TopoNode::WireDef& aDef = myGraph->myData->myIncStorage.ChangeWire(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Wire(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::VertexDef& BRepGraph::MutView::VertexDef(int theIdx)
{
  BRepGraph_TopoNode::VertexDef& aDef = myGraph->myData->myIncStorage.ChangeVertex(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Vertex(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::FaceDef& BRepGraph::MutView::FaceDef(int theIdx)
{
  BRepGraph_TopoNode::FaceDef& aDef = myGraph->myData->myIncStorage.ChangeFace(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Face(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::ShellDef& BRepGraph::MutView::ShellDef(int theIdx)
{
  BRepGraph_TopoNode::ShellDef& aDef = myGraph->myData->myIncStorage.ChangeShell(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Shell(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::SolidDef& BRepGraph::MutView::SolidDef(int theIdx)
{
  BRepGraph_TopoNode::SolidDef& aDef = myGraph->myData->myIncStorage.ChangeSolid(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Solid(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::CompoundDef& BRepGraph::MutView::CompoundDef(int theIdx)
{
  BRepGraph_TopoNode::CompoundDef& aDef = myGraph->myData->myIncStorage.ChangeCompound(theIdx);
  myGraph->markModified(BRepGraph_NodeId::Compound(theIdx), aDef);
  return aDef;
}

//=================================================================================================

BRepGraph_TopoNode::CompSolidDef& BRepGraph::MutView::CompSolidDef(int theIdx)
{
  BRepGraph_TopoNode::CompSolidDef& aDef = myGraph->myData->myIncStorage.ChangeCompSolid(theIdx);
  myGraph->markModified(BRepGraph_NodeId::CompSolid(theIdx), aDef);
  return aDef;
}

//=================================================================================================

void BRepGraph::MutView::AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                          BRepGraph_NodeId            theFaceDef,
                                          const Handle(Geom2d_Curve)& theCurve2d,
                                          double                      theFirst,
                                          double                      theLast,
                                          TopAbs_Orientation          theEdgeOrientation)
{
  BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myIncStorage.ChangeEdge(theEdgeDef.Index);
  BRepGraph_TopoNode::EdgeDef::PCurveEntry aNewEntry;
  aNewEntry.Curve2d         = theCurve2d;
  aNewEntry.FaceDefId       = theFaceDef;
  aNewEntry.ParamFirst      = theFirst;
  aNewEntry.ParamLast       = theLast;
  aNewEntry.EdgeOrientation = theEdgeOrientation;
  anEdgeDef.PCurves.Append(aNewEntry);

  // Update edge-to-face reverse index so FaceCountOfEdge stays in sync.
  myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToFace(
    theEdgeDef.Index, theFaceDef.Index);

  myGraph->markModified(theEdgeDef);
}

//=================================================================================================

void BRepGraph::MutView::ReplaceEdgeInWire(int              theWireDefIdx,
                                           BRepGraph_NodeId theOldEdgeDef,
                                           BRepGraph_NodeId theNewEdgeDef,
                                           bool             theReversed)
{
  BRepGraph_Mutator::ReplaceEdgeInWire(
    *myGraph, theWireDefIdx, theOldEdgeDef, theNewEdgeDef, theReversed);
}

//=================================================================================================

void BRepGraph::MutView::SplitEdge(BRepGraph_NodeId  theEdgeDef,
                                   BRepGraph_NodeId  theSplitVertex,
                                   double            theSplitParam,
                                   BRepGraph_NodeId& theSubA,
                                   BRepGraph_NodeId& theSubB)
{
  BRepGraph_Mutator::SplitEdge(
    *myGraph, theEdgeDef, theSplitVertex, theSplitParam, theSubA, theSubB);
}

//=================================================================================================

int BRepGraph::MutView::AddRelEdge(BRepGraph_NodeId  theFrom,
                                   BRepGraph_NodeId  theTo,
                                   BRepGraph_RelEdge::Kind theKind)
{
  return BRepGraph_BackRefManager::AddRelEdge(*myGraph, theFrom, theTo, theKind);
}

//=================================================================================================

void BRepGraph::MutView::RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                        BRepGraph_NodeId  theTo,
                                        BRepGraph_RelEdge::Kind theKind)
{
  BRepGraph_BackRefManager::RemoveRelEdges(*myGraph, theFrom, theTo, theKind);
}

//=================================================================================================

void BRepGraph::MutView::CommitMutation()
{
  BRepGraph_Mutator::CommitMutation(*myGraph);
}
