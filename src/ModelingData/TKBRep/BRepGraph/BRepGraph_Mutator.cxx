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

#include <BRepGraph_Mutator.hxx>
#include <BRepGraph.hxx>

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
  const BRepGraph_TopoNode::EdgeDef& anOrig = theGraph.myEdgeDefs.Value(theEdgeDef.Index);

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
  NCollection_Vector<int> aOrigWires = theGraph.WiresOfEdge(theEdgeDef.Index);

  // Get location from first usage (for sub-edge TopoDS shape registration).
  TopLoc_Location aEdgeLoc;
  if (!anOrig.Usages.IsEmpty())
    aEdgeLoc = theGraph.myEdgeUsages.Value(anOrig.Usages.Value(0).Index).LocalLocation;

  // Allocate SubA slot.
  BRepGraph_TopoNode::EdgeDef& aSubADef = theGraph.myEdgeDefs.Appended();
  const int                    aSubAIdx = theGraph.myEdgeDefs.Length() - 1;
  aSubADef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Edge, aSubAIdx);
  theSubA     = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference — use index).
  BRepGraph_TopoNode::EdgeDef& aSubBDef = theGraph.myEdgeDefs.Appended();
  const int                    aSubBIdx = theGraph.myEdgeDefs.Length() - 1;
  aSubBDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Edge, aSubBIdx);
  theSubB     = aSubBDef.Id;

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraph_TopoNode::EdgeDef& aSubA = theGraph.myEdgeDefs.ChangeValue(aSubAIdx);
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
    BRepGraph_TopoNode::EdgeDef& aSubB = theGraph.myEdgeDefs.ChangeValue(aSubBIdx);
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
    theGraph.myCurves.ChangeValue(aOrigCurveNodeId.Index).EdgeDefUsers.Append(theSubA);
    theGraph.myCurves.ChangeValue(aOrigCurveNodeId.Index).EdgeDefUsers.Append(theSubB);
  }

  // Split PCurve nodes for each PCurve entry in original.
  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aPCIdx = 0; aPCIdx < aOrigPCurves.Length(); ++aPCIdx)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = aOrigPCurves.Value(aPCIdx);
    const BRepGraph_GeomNode::PCurve& aPCNode =
      theGraph.myPCurves.Value(aPCEntry.PCurveNodeId.Index);

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
    theGraph.myEdgeDefs.ChangeValue(aSubAIdx).PCurves.Append(aPCSubAEntry);

    // PCurve for SubB: [aPCSplit, PCLast].
    const BRepGraph_NodeId aPCSubBId = theGraph.createPCurveNode(
      aPCNode.Curve2d, theSubB, aPCEntry.FaceDefId, aPCSplit, aPCNode.ParamLast);
    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCSubBEntry;
    aPCSubBEntry.PCurveNodeId    = aPCSubBId;
    aPCSubBEntry.FaceDefId       = aPCEntry.FaceDefId;
    aPCSubBEntry.EdgeOrientation = aPCEntry.EdgeOrientation;
    theGraph.myEdgeDefs.ChangeValue(aSubBIdx).PCurves.Append(aPCSubBEntry);
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurveNodeId.IsValid())
  {
    const BRepGraph_GeomNode::Curve& aCurveNode =
      theGraph.myCurves.Value(aOrigCurveNodeId.Index);
    BRep_Builder aBB;

    const TopoDS_Shape aStartVShape = theGraph.Shape(aOrigStartVertexDefId);
    const TopoDS_Shape aSplitVShape = theGraph.Shape(theSplitVertex);
    const TopoDS_Shape aEndVShape   = theGraph.Shape(aOrigEndVertexDefId);

    TopoDS_Edge aSubAEdge;
    aBB.MakeEdge(aSubAEdge, aCurveNode.CurveGeom, aEdgeLoc, aOrigTolerance);
    aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
    if (!aStartVShape.IsNull()) aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
    if (!aSplitVShape.IsNull()) aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
    theGraph.myOriginalShapes.Bind(theSubA, aSubAEdge);

    TopoDS_Edge aSubBEdge;
    aBB.MakeEdge(aSubBEdge, aCurveNode.CurveGeom, aEdgeLoc, aOrigTolerance);
    aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
    if (!aSplitVShape.IsNull()) aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
    if (!aEndVShape.IsNull())   aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
    theGraph.myOriginalShapes.Bind(theSubB, aSubBEdge);
  }

  // Update wire OrderedEdges: replace original entry with two sub-entries.
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const int aWireIdx = aOrigWires.Value(aWIdx);
    BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myWireDefs.ChangeValue(aWireIdx);

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
    if (theGraph.myEdgeToWires.IsBound(theEdgeDef.Index))
    {
      NCollection_Vector<int>& anOrigWireRefs =
        theGraph.myEdgeToWires.ChangeFind(theEdgeDef.Index);
      for (int aWI = 0; aWI < anOrigWireRefs.Length(); ++aWI)
      {
        if (anOrigWireRefs.Value(aWI) == aWireIdx)
        {
          if (aWI < anOrigWireRefs.Length() - 1)
            anOrigWireRefs.ChangeValue(aWI) = anOrigWireRefs.Value(anOrigWireRefs.Length() - 1);
          anOrigWireRefs.EraseLast();
          break;
        }
      }
    }
    if (!theGraph.myEdgeToWires.IsBound(aSubAIdx))
      theGraph.myEdgeToWires.Bind(aSubAIdx, NCollection_Vector<int>());
    theGraph.myEdgeToWires.ChangeFind(aSubAIdx).Append(aWireIdx);

    if (!theGraph.myEdgeToWires.IsBound(aSubBIdx))
      theGraph.myEdgeToWires.Bind(aSubBIdx, NCollection_Vector<int>());
    theGraph.myEdgeToWires.ChangeFind(aSubBIdx).Append(aWireIdx);

    theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWireIdx));
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
  BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myWireDefs.ChangeValue(theWireDefIdx);
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
      if (theGraph.myEdgeToWires.IsBound(theOldEdgeDef.Index))
      {
        NCollection_Vector<int>& anOldWires =
          theGraph.myEdgeToWires.ChangeFind(theOldEdgeDef.Index);
        for (int aWIdx = 0; aWIdx < anOldWires.Length(); ++aWIdx)
        {
          if (anOldWires.Value(aWIdx) == theWireDefIdx)
          {
            if (aWIdx < anOldWires.Length() - 1)
              anOldWires.ChangeValue(aWIdx) = anOldWires.Value(anOldWires.Length() - 1);
            anOldWires.EraseLast();
            break;
          }
        }
      }
      if (!theGraph.myEdgeToWires.IsBound(theNewEdgeDef.Index))
        theGraph.myEdgeToWires.Bind(theNewEdgeDef.Index, NCollection_Vector<int>());
      theGraph.myEdgeToWires.ChangeFind(theNewEdgeDef.Index).Append(theWireDefIdx);
    }
  }
  theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, theWireDefIdx));
}
