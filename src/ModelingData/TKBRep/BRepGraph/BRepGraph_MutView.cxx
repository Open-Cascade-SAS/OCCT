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

#include <BRepGraph_MutView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Mutator.hxx>

#include <Geom2d_Curve.hxx>

//=================================================================================================

BRepGraph_TopoNode::EdgeDef& BRepGraph::MutView::EdgeDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theIdx));
  return myGraph->myData->myEdgeDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::WireDef& BRepGraph::MutView::WireDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theIdx));
  return myGraph->myData->myWireDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::VertexDef& BRepGraph::MutView::VertexDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theIdx));
  return myGraph->myData->myVertexDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::FaceDef& BRepGraph::MutView::FaceDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theIdx));
  return myGraph->myData->myFaceDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::ShellDef& BRepGraph::MutView::ShellDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, theIdx));
  return myGraph->myData->myShellDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::SolidDef& BRepGraph::MutView::SolidDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, theIdx));
  return myGraph->myData->mySolidDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::CompoundDef& BRepGraph::MutView::CompoundDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, theIdx));
  return myGraph->myData->myCompoundDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::CompSolidDef& BRepGraph::MutView::CompSolidDef(int theIdx)
{
  myGraph->markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, theIdx));
  return myGraph->myData->myCompSolidDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_GeomNode::Surf& BRepGraph::MutView::SurfNode(int theIdx)
{
  return myGraph->myData->mySurfaces.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_GeomNode::Curve& BRepGraph::MutView::CurveNode(int theIdx)
{
  return myGraph->myData->myCurves.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_GeomNode::PCurve& BRepGraph::MutView::PCurveNode(int theIdx)
{
  return myGraph->myData->myPCurves.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::MutView::AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                                     BRepGraph_NodeId            theFaceDef,
                                                     const Handle(Geom2d_Curve)& theCurve2d,
                                                     double                      theFirst,
                                                     double                      theLast,
                                                     TopAbs_Orientation          theEdgeOrientation)
{
  BRepGraph_NodeId aPCId =
    myGraph->createPCurveNode(theCurve2d, theEdgeDef, theFaceDef, theFirst, theLast);

  BRepGraph_TopoNode::EdgeDef& anEdgeDef =
    myGraph->myData->myEdgeDefs.ChangeValue(theEdgeDef.Index);
  BRepGraph_TopoNode::EdgeDef::PCurveEntry aNewEntry;
  aNewEntry.PCurveNodeId    = aPCId;
  aNewEntry.FaceDefId       = theFaceDef;
  aNewEntry.EdgeOrientation = theEdgeOrientation;
  anEdgeDef.PCurves.Append(aNewEntry);

  myGraph->markModified(theEdgeDef);
  return aPCId;
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
  BRepGraph_RelEdge anEdge;
  anEdge.RelKind   = theKind;
  anEdge.Source = theFrom;
  anEdge.Target = theTo;

  myGraph->myData->myOutRelEdges.TryBind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  myGraph->myData->myInRelEdges.TryBind(theTo, NCollection_Vector<BRepGraph_RelEdge>());

  NCollection_Vector<BRepGraph_RelEdge>& anOutVec = myGraph->myData->myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  myGraph->myData->myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph::MutView::RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                        BRepGraph_NodeId  theTo,
                                        BRepGraph_RelEdge::Kind theKind)
{
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec =
    myGraph->myData->myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Target == theTo)
      {
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  NCollection_Vector<BRepGraph_RelEdge>* anInVec =
    myGraph->myData->myInRelEdges.ChangeSeek(theTo);
  if (anInVec != nullptr)
  {
    for (int anIdx = anInVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anInVec->Value(anIdx);
      if (anEdge.RelKind == theKind && anEdge.Source == theFrom)
      {
        if (anIdx < anInVec->Length() - 1)
          anInVec->ChangeValue(anIdx) = anInVec->Value(anInVec->Length() - 1);
        anInVec->EraseLast();
      }
    }
  }
}
