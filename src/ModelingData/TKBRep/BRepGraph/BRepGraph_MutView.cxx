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
#include <BRepGraphInc_Storage.hxx>

#include <Geom2d_Curve.hxx>

//=================================================================================================

void BRepGraph::MutView::AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                          BRepGraph_NodeId            theFaceDef,
                                          const occ::handle<Geom2d_Curve>& theCurve2d,
                                          double                      theFirst,
                                          double                      theLast,
                                          TopAbs_Orientation          theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Create CoEdge entity for the new PCurve binding.
  BRepGraphInc::CoEdgeEntity& aCoEdge = aStorage.AppendCoEdge();
  const int aCoEdgeIdx = aStorage.NbCoEdges() - 1;
  aCoEdge.Id        = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
  aCoEdge.EdgeIdx   = theEdgeDef.Index;
  aCoEdge.FaceDefId = theFaceDef;
  aCoEdge.Sense     = theEdgeOrientation;
  if (!theCurve2d.IsNull())
  {
    BRepGraphInc::Curve2DRep& aCurve2DRep = aStorage.AppendCurve2DRep();
    const int aCurve2DRepIdx = aStorage.NbCurves2D() - 1;
    aCurve2DRep.Id    = BRepGraph_RepId::Curve2D(aCurve2DRepIdx);
    aCurve2DRep.Curve = theCurve2d;
    aCoEdge.Curve2DRepIdx = aCurve2DRepIdx;
  }
  aCoEdge.ParamFirst = theFirst;
  aCoEdge.ParamLast  = theLast;

  // Update reverse indices.
  aStorage.ChangeReverseIndex().BindEdgeToCoEdge(theEdgeDef.Index, aCoEdgeIdx);
  aStorage.ChangeReverseIndex().BindEdgeToFace(theEdgeDef.Index, theFaceDef.Index);

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
