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
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//=================================================================================================

void BRepGraph_Mutator::SplitEdge(BRepGraph&        theGraph,
                                  BRepGraph_NodeId  theEdgeDef,
                                  BRepGraph_NodeId  theSplitVertex,
                                  double            theSplitParam,
                                  BRepGraph_NodeId& theSubA,
                                  BRepGraph_NodeId& theSubB)
{
  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraph_TopoNode::EdgeDef& anOrig = theGraph.myData->myIncStorage.Edge(theEdgeDef.Index);

  const Handle(Geom_Curve) aOrigCurve3d        = anOrig.Curve3d;
  const double           aOrigTolerance        = anOrig.Tolerance;
  const bool             aOrigSameParameter    = anOrig.SameParameter;
  const double           aOrigParamFirst       = anOrig.ParamFirst;
  const double           aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_NodeId aOrigStartVertexDefId = anOrig.StartVertexDefId();
  const BRepGraph_NodeId aOrigEndVertexDefId   = anOrig.EndVertexDefId();
  const bool             aOrigSameRange        = anOrig.SameRange;

  // Copy PCurve entries before any mutation.
  NCollection_Vector<BRepGraph_TopoNode::EdgeDef::PCurveEntry> aOrigPCurves = anOrig.PCurves;
  // Copy wire indices: ReverseIdx may be rebuilt below.
  const NCollection_Vector<int>* aOrigWiresPtr =
    theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(theEdgeDef.Index);
  const NCollection_Vector<int> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<int>();

  // Allocate SubA slot.
  BRepGraph_TopoNode::EdgeDef& aSubADef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubAIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubADef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
  theSubA     = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference -- use index).
  BRepGraph_TopoNode::EdgeDef& aSubBDef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubBIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubBDef.Id = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
  theSubB     = aSubBDef.Id;

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraph_TopoNode::EdgeDef& aSubA = theGraph.myData->myIncStorage.ChangeEdge(aSubAIdx);
    aSubA.Curve3d          = aOrigCurve3d;
    aSubA.Tolerance        = aOrigTolerance;
    aSubA.SameParameter    = aOrigSameParameter;
    aSubA.SameRange        = false;
    aSubA.IsDegenerate     = false;
    aSubA.StartVertexIdx = aOrigStartVertexDefId.IsValid() ? aOrigStartVertexDefId.Index : -1;
    aSubA.EndVertexIdx   = theSplitVertex.IsValid() ? theSplitVertex.Index : -1;
    aSubA.ParamFirst       = aOrigParamFirst;
    aSubA.ParamLast        = theSplitParam;
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraph_TopoNode::EdgeDef& aSubB = theGraph.myData->myIncStorage.ChangeEdge(aSubBIdx);
    aSubB.Curve3d          = aOrigCurve3d;
    aSubB.Tolerance        = aOrigTolerance;
    aSubB.SameParameter    = aOrigSameParameter;
    aSubB.SameRange        = false;
    aSubB.IsDegenerate     = false;
    aSubB.StartVertexIdx = theSplitVertex.IsValid() ? theSplitVertex.Index : -1;
    aSubB.EndVertexIdx   = aOrigEndVertexDefId.IsValid() ? aOrigEndVertexDefId.Index : -1;
    aSubB.ParamFirst       = theSplitParam;
    aSubB.ParamLast        = aOrigParamLast;
  }

  theGraph.allocateUID(theSubA);
  theGraph.allocateUID(theSubB);

  // Update incidence: wire EdgeRefs and mark original as removed.
  {
    // Update WireEntity.EdgeRefs: replace original edge with SubA+SubB in all containing wires.
    const NCollection_Vector<int>* aWireIndices =
      theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(theEdgeDef.Index);
    if (aWireIndices != nullptr)
    {
      for (int aWIdx = 0; aWIdx < aWireIndices->Length(); ++aWIdx)
      {
        const int aWireIdx = aWireIndices->Value(aWIdx);
        if (aWireIdx < 0 || aWireIdx >= theGraph.myData->myIncStorage.NbWires())
          continue;
        BRepGraphInc::WireEntity& aWire = theGraph.myData->myIncStorage.ChangeWire(aWireIdx);
        for (int aERIdx = 0; aERIdx < aWire.EdgeRefs.Length(); ++aERIdx)
        {
          if (aWire.EdgeRefs.Value(aERIdx).EdgeIdx == theEdgeDef.Index)
          {
            const TopAbs_Orientation aOrigOri = aWire.EdgeRefs.Value(aERIdx).Orientation;
            // Replace in-place: change current to SubA.
            aWire.EdgeRefs.ChangeValue(aERIdx).EdgeIdx = aSubAIdx;
            // Insert SubB after SubA.
            BRepGraphInc::EdgeRef aSubBRef;
            aSubBRef.EdgeIdx     = aSubBIdx;
            aSubBRef.Orientation = aOrigOri;
            aWire.EdgeRefs.Append(aSubBRef);
            break;
          }
        }
      }
    }

    // Mark original edge as removed in incidence.
    if (theEdgeDef.Index >= 0 && theEdgeDef.Index < theGraph.myData->myIncStorage.NbEdges())
      theGraph.myData->myIncStorage.ChangeEdge(theEdgeDef.Index).IsRemoved = true;
  }

  // Split PCurve entries for each PCurve in original (inline data, no separate PCurve nodes).
  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aPCIdx = 0; aPCIdx < aOrigPCurves.Length(); ++aPCIdx)
  {
    const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry = aOrigPCurves.Value(aPCIdx);

    // Compute 2D split parameter.
    double aPCSplit;
    if (aOrigSameRange)
    {
      aPCSplit = theSplitParam;
    }
    else
    {
      const double aPCRange = aPCEntry.ParamLast - aPCEntry.ParamFirst;
      if (aParamRange > 0.0)
        aPCSplit = aPCEntry.ParamFirst
                   + ((theSplitParam - aOrigParamFirst) / aParamRange) * aPCRange;
      else
        aPCSplit = 0.5 * (aPCEntry.ParamFirst + aPCEntry.ParamLast);
    }

    // PCurve for SubA: [PCFirst, aPCSplit].
    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCSubAEntry;
    aPCSubAEntry.Curve2d         = aPCEntry.Curve2d;
    aPCSubAEntry.FaceDefId       = aPCEntry.FaceDefId;
    aPCSubAEntry.ParamFirst      = aPCEntry.ParamFirst;
    aPCSubAEntry.ParamLast       = aPCSplit;
    aPCSubAEntry.Continuity      = aPCEntry.Continuity;
    aPCSubAEntry.EdgeOrientation = aPCEntry.EdgeOrientation;
    theGraph.myData->myIncStorage.ChangeEdge(aSubAIdx).PCurves.Append(aPCSubAEntry);

    // PCurve for SubB: [aPCSplit, PCLast].
    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCSubBEntry;
    aPCSubBEntry.Curve2d         = aPCEntry.Curve2d;
    aPCSubBEntry.FaceDefId       = aPCEntry.FaceDefId;
    aPCSubBEntry.ParamFirst      = aPCSplit;
    aPCSubBEntry.ParamLast       = aPCEntry.ParamLast;
    aPCSubBEntry.Continuity      = aPCEntry.Continuity;
    aPCSubBEntry.EdgeOrientation = aPCEntry.EdgeOrientation;
    theGraph.myData->myIncStorage.ChangeEdge(aSubBIdx).PCurves.Append(aPCSubBEntry);
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (!aOrigCurve3d.IsNull())
  {
    BRep_Builder aBB;

    const TopoDS_Shape aStartVShape = theGraph.Shapes().Shape( aOrigStartVertexDefId);
    const TopoDS_Shape aSplitVShape = theGraph.Shapes().Shape( theSplitVertex);
    const TopoDS_Shape aEndVShape   = theGraph.Shapes().Shape( aOrigEndVertexDefId);

    TopoDS_Edge aSubAEdge;
    aBB.MakeEdge(aSubAEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
    aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
    if (!aStartVShape.IsNull()) aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
    if (!aSplitVShape.IsNull()) aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
    theGraph.myData->myIncStorage.BindOriginal(theSubA, aSubAEdge);

    TopoDS_Edge aSubBEdge;
    aBB.MakeEdge(aSubBEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
    aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
    if (!aSplitVShape.IsNull()) aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
    if (!aEndVShape.IsNull())   aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
    theGraph.myData->myIncStorage.BindOriginal(theSubB, aSubBEdge);
  }

  // Update edge-to-wire reverse index incrementally.
  BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const int aWireIdx = aOrigWires.Value(aWIdx);
    aRevIdx.UnbindEdgeFromWire(theEdgeDef.Index, aWireIdx);
    aRevIdx.BindEdgeToWire(aSubAIdx, aWireIdx);
    aRevIdx.BindEdgeToWire(aSubBIdx, aWireIdx);
    theGraph.markModified(BRepGraph_NodeId::Wire(aWireIdx));
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc::EdgeEntity& aSubAEnt = theGraph.myData->myIncStorage.Edge(aSubAIdx);
    const BRepGraphInc::EdgeEntity& aSubBEnt = theGraph.myData->myIncStorage.Edge(aSubBIdx);
    BRepGraphInc_ReverseIndex& aRevIdxMut = theGraph.myData->myIncStorage.ChangeReverseIndex();

    if (aSubAEnt.StartVertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubAEnt.StartVertexIdx, aSubAIdx);
    if (aSubAEnt.EndVertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubAEnt.EndVertexIdx, aSubAIdx);
    if (aSubBEnt.StartVertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubBEnt.StartVertexIdx, aSubBIdx);
    if (aSubBEnt.EndVertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubBEnt.EndVertexIdx, aSubBIdx);

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigStartVertexDefId.Index, theEdgeDef.Index);
    if (aOrigEndVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigEndVertexDefId.Index, theEdgeDef.Index);

    // Edge-to-face: derive from PCurves on sub-edges.
    for (int aPCIdx = 0; aPCIdx < aSubAEnt.PCurves.Length(); ++aPCIdx)
      aRevIdxMut.BindEdgeToFace(aSubAIdx, aSubAEnt.PCurves.Value(aPCIdx).FaceDefId.Index);
    for (int aPCIdx = 0; aPCIdx < aSubBEnt.PCurves.Length(); ++aPCIdx)
      aRevIdxMut.BindEdgeToFace(aSubBIdx, aSubBEnt.PCurves.Value(aPCIdx).FaceDefId.Index);
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
  BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myIncStorage.ChangeWire(theWireDefIdx);

  // Update incidence EdgeRefs on the WireEntity.
  for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.EdgeRefs.Length(); ++anEdgeIdx)
  {
    BRepGraphInc::EdgeRef& aER = aWireDef.EdgeRefs.ChangeValue(anEdgeIdx);
    if (aER.EdgeIdx == theOldEdgeDef.Index)
    {
      aER.EdgeIdx = theNewEdgeDef.Index;
      if (theReversed)
        aER.Orientation = TopAbs::Reverse(aER.Orientation);

      // Update edge-to-wire reverse index incrementally.
      theGraph.myData->myIncStorage.ChangeReverseIndex().ReplaceEdgeInWireMap(
        theOldEdgeDef.Index, theNewEdgeDef.Index, theWireDefIdx);
    }
  }

  theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWireDefIdx));
}
