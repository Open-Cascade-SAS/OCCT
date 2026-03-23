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
#include <BRepGraph_Mutator.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ShapesView.hxx>

#include <BRep_Builder.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>

//=================================================================================================

void BRepGraph_Mutator::SplitEdge(BRepGraph&        theGraph,
                                  BRepGraph_NodeId  theEdgeDef,
                                  BRepGraph_NodeId  theSplitVertex,
                                  double            theSplitParam,
                                  BRepGraph_NodeId& theSubA,
                                  BRepGraph_NodeId& theSubB)
{
  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraph_TopoNode::EdgeDef& anOrig = theGraph.myData->myEdges.Defs.Value(theEdgeDef.Index);

  const BRepGraph_NodeId aOrigCurveNodeId      = anOrig.CurveNodeId;
  const double           aOrigTolerance        = anOrig.Tolerance;
  const bool             aOrigSameParameter    = anOrig.SameParameter;
  const double           aOrigParamFirst       = anOrig.ParamFirst;
  const double           aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_NodeId aOrigStartVertexDefId = anOrig.StartVertexDefId;
  const BRepGraph_NodeId aOrigEndVertexDefId   = anOrig.EndVertexDefId;
  const bool             aOrigSameRange        = anOrig.SameRange;

  // Copy PCurve entries and wire list before any mutation.
  NCollection_Vector<BRepGraph_TopoNode::EdgeDef::PCurveEntry> aOrigPCurves = anOrig.PCurves;
  // Copy required: the loop below mutates myEdgeToWires (same key + new Bind calls can rehash).
  const NCollection_Vector<int>* aOrigWiresPtr = theGraph.myData->myEdgeToWires.Seek(theEdgeDef.Index);
  const NCollection_Vector<int>  aOrigWires    = aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<int>();

  // Allocate SubA slot.
  BRepGraph_TopoNode::EdgeDef& aSubADef = theGraph.myData->myEdges.Defs.Appended();
  const int                    aSubAIdx = theGraph.myData->myEdges.Defs.Length() - 1;
  aSubADef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
  theSubA     = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference -- use index).
  BRepGraph_TopoNode::EdgeDef& aSubBDef = theGraph.myData->myEdges.Defs.Appended();
  const int                    aSubBIdx = theGraph.myData->myEdges.Defs.Length() - 1;
  aSubBDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
  theSubB     = aSubBDef.Id;

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraph_TopoNode::EdgeDef& aSubA = theGraph.myData->myEdges.Defs.ChangeValue(aSubAIdx);
    aSubA.CurveNodeId      = aOrigCurveNodeId;
    aSubA.Tolerance        = aOrigTolerance;
    aSubA.SameParameter    = aOrigSameParameter;
    aSubA.SameRange        = false;
    aSubA.IsDegenerate     = false;
    aSubA.StartVertexDefId = aOrigStartVertexDefId;
    aSubA.EndVertexDefId   = theSplitVertex;
    aSubA.ParamFirst       = aOrigParamFirst;
    aSubA.ParamLast        = theSplitParam;
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraph_TopoNode::EdgeDef& aSubB = theGraph.myData->myEdges.Defs.ChangeValue(aSubBIdx);
    aSubB.CurveNodeId      = aOrigCurveNodeId;
    aSubB.Tolerance        = aOrigTolerance;
    aSubB.SameParameter    = aOrigSameParameter;
    aSubB.SameRange        = false;
    aSubB.IsDegenerate     = false;
    aSubB.StartVertexDefId = theSplitVertex;
    aSubB.EndVertexDefId   = aOrigEndVertexDefId;
    aSubB.ParamFirst       = theSplitParam;
    aSubB.ParamLast        = aOrigParamLast;
  }

  theGraph.allocateUID(theSubA);
  theGraph.allocateUID(theSubB);

  // Register curve back-references for sub-edges.
  if (aOrigCurveNodeId.IsValid())
  {
    BRepGraph_BackRefManager::BindEdgeToCurve(theGraph, theSubA, aOrigCurveNodeId.Index);
    BRepGraph_BackRefManager::BindEdgeToCurve(theGraph, theSubB, aOrigCurveNodeId.Index);
  }

  // Split PCurve nodes for each PCurve entry in original.
  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aPCIdx = 0; aPCIdx < aOrigPCurves.Length(); ++aPCIdx)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = aOrigPCurves.Value(aPCIdx);
    const BRepGraph_GeomNode::PCurve& aPCNode =
      theGraph.myData->myPCurves.Nodes.Value(aPCEntry.PCurveNodeId.Index);

    // Compute 2D split parameter.
    double aPCSplit;
    if (aOrigSameRange)
    {
      aPCSplit = theSplitParam;
    }
    else
    {
      const double aPCRange = aPCNode.ParamLast - aPCNode.ParamFirst;
      if (aParamRange > 0.0)
        aPCSplit = aPCNode.ParamFirst
                   + ((theSplitParam - aOrigParamFirst) / aParamRange) * aPCRange;
      else
        aPCSplit = 0.5 * (aPCNode.ParamFirst + aPCNode.ParamLast);
    }

    // PCurve for SubA: [PCFirst, aPCSplit].
    const BRepGraph_NodeId aPCSubAId = theGraph.createPCurveNode(
      aPCNode.Curve2d, theSubA, aPCEntry.FaceDefId, aPCNode.ParamFirst, aPCSplit);
    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCSubAEntry;
    aPCSubAEntry.PCurveNodeId    = aPCSubAId;
    aPCSubAEntry.FaceDefId       = aPCEntry.FaceDefId;
    aPCSubAEntry.EdgeOrientation = aPCEntry.EdgeOrientation;
    theGraph.myData->myEdges.Defs.ChangeValue(aSubAIdx).PCurves.Append(aPCSubAEntry);

    // PCurve for SubB: [aPCSplit, PCLast].
    const BRepGraph_NodeId aPCSubBId = theGraph.createPCurveNode(
      aPCNode.Curve2d, theSubB, aPCEntry.FaceDefId, aPCSplit, aPCNode.ParamLast);
    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCSubBEntry;
    aPCSubBEntry.PCurveNodeId    = aPCSubBId;
    aPCSubBEntry.FaceDefId       = aPCEntry.FaceDefId;
    aPCSubBEntry.EdgeOrientation = aPCEntry.EdgeOrientation;
    theGraph.myData->myEdges.Defs.ChangeValue(aSubBIdx).PCurves.Append(aPCSubBEntry);
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurveNodeId.IsValid())
  {
    const BRepGraph_GeomNode::Curve& aCurveNode =
      theGraph.myData->myCurves.Nodes.Value(aOrigCurveNodeId.Index);
    BRep_Builder aBB;

    const TopoDS_Shape aStartVShape = theGraph.Shapes().Shape( aOrigStartVertexDefId);
    const TopoDS_Shape aSplitVShape = theGraph.Shapes().Shape( theSplitVertex);
    const TopoDS_Shape aEndVShape   = theGraph.Shapes().Shape( aOrigEndVertexDefId);

    TopoDS_Edge aSubAEdge;
    aBB.MakeEdge(aSubAEdge, aCurveNode.CurveGeom, aCurveNode.CurveLocation, aOrigTolerance);
    aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
    if (!aStartVShape.IsNull()) aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
    if (!aSplitVShape.IsNull()) aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
    theGraph.myData->myOriginalShapes.Bind(theSubA, aSubAEdge);

    TopoDS_Edge aSubBEdge;
    aBB.MakeEdge(aSubBEdge, aCurveNode.CurveGeom, aCurveNode.CurveLocation, aOrigTolerance);
    aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
    if (!aSplitVShape.IsNull()) aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
    if (!aEndVShape.IsNull())   aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
    theGraph.myData->myOriginalShapes.Bind(theSubB, aSubBEdge);
  }

  // Update wire OrderedEdges: replace original entry with two sub-entries.
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const int aWireIdx = aOrigWires.Value(aWIdx);
    BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWires.Defs.ChangeValue(aWireIdx);

    // Rebuild OrderedEdges replacing the original entry with SubA + SubB (or SubB + SubA).
    NCollection_Vector<BRepGraph_TopoNode::WireDef::EdgeEntry> aNewEdges;
    for (int aEIdx = 0; aEIdx < aWireDef.OrderedEdges.Length(); ++aEIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(aEIdx);
      if (anEntry.EdgeDefId != theEdgeDef)
      {
        aNewEdges.Append(anEntry);
      }
      else if (anEntry.OrientationInWire != TopAbs_REVERSED)
      {
        // Forward traversal: Start -> SplitVtx (SubA), SplitVtx -> End (SubB).
        BRepGraph_TopoNode::WireDef::EdgeEntry aEntA;
        aEntA.EdgeDefId         = theSubA;
        aEntA.OrientationInWire = TopAbs_FORWARD;
        aNewEdges.Append(aEntA);
        BRepGraph_TopoNode::WireDef::EdgeEntry aEntB;
        aEntB.EdgeDefId         = theSubB;
        aEntB.OrientationInWire = TopAbs_FORWARD;
        aNewEdges.Append(aEntB);
      }
      else
      {
        // Reversed traversal: End -> SplitVtx (SubB rev), SplitVtx -> Start (SubA rev).
        BRepGraph_TopoNode::WireDef::EdgeEntry aEntB;
        aEntB.EdgeDefId         = theSubB;
        aEntB.OrientationInWire = TopAbs_REVERSED;
        aNewEdges.Append(aEntB);
        BRepGraph_TopoNode::WireDef::EdgeEntry aEntA;
        aEntA.EdgeDefId         = theSubA;
        aEntA.OrientationInWire = TopAbs_REVERSED;
        aNewEdges.Append(aEntA);
      }
    }
    aWireDef.OrderedEdges = std::move(aNewEdges);

    // Update myEdgeToWires: remove this wire from orig, register for SubA and SubB.
    BRepGraph_BackRefManager::UnbindEdgeFromWire(theGraph, theEdgeDef.Index, aWireIdx);
    BRepGraph_BackRefManager::BindEdgeToWire(theGraph, aSubAIdx, aWireIdx);
    BRepGraph_BackRefManager::BindEdgeToWire(theGraph, aSubBIdx, aWireIdx);

    theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aWireIdx));
  }

  theGraph.markModified(theEdgeDef);
  theGraph.markModified(theSubA);
  theGraph.markModified(theSubB);
}

//=================================================================================================

void BRepGraph_Mutator::ReplaceEdgeInWire(BRepGraph&       theGraph,
                                          int              theWireDefIdx,
                                          BRepGraph_NodeId theOldEdgeDef,
                                          BRepGraph_NodeId theNewEdgeDef,
                                          bool             theReversed)
{
  BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myWires.Defs.ChangeValue(theWireDefIdx);
  for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
      aWireDef.OrderedEdges.ChangeValue(anEdgeIdx);
    if (anEntry.EdgeDefId == theOldEdgeDef)
    {
      anEntry.EdgeDefId = theNewEdgeDef;
      if (theReversed)
      {
        anEntry.OrientationInWire = TopAbs::Reverse(anEntry.OrientationInWire);
      }

      // Update edge-to-wire reverse index.
      BRepGraph_BackRefManager::ReplaceEdgeInWireMap(theGraph, theOldEdgeDef.Index,
                                                      theNewEdgeDef.Index, theWireDefIdx);
    }
  }
  theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWireDefIdx));
}
