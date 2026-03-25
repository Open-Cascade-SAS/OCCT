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
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//=================================================================================================

void BRepGraph_Mutator::SplitEdge(BRepGraph&             theGraph,
                                  const BRepGraph_NodeId theEdgeDef,
                                  const BRepGraph_NodeId theSplitVertex,
                                  const double           theSplitParam,
                                  BRepGraph_NodeId&      theSubA,
                                  BRepGraph_NodeId&      theSubB)
{
  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraph_TopoNode::EdgeDef& anOrig =
    theGraph.myData->myIncStorage.Edge(BRepGraph_EdgeId::FromNodeId(theEdgeDef));

  const BRepGraph_Curve3DRepId  aOrigCurve3DRepId     = anOrig.Curve3DRepId;
  const double                  aOrigTolerance        = anOrig.Tolerance;
  const bool                    aOrigSameParameter    = anOrig.SameParameter;
  const double                  aOrigParamFirst       = anOrig.ParamFirst;
  const double                  aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_NodeId        aOrigStartVertexDefId = anOrig.StartVertexDefId();
  const BRepGraph_NodeId        aOrigEndVertexDefId   = anOrig.EndVertexDefId();
  const BRepGraphInc::VertexRef aOrigStartVertexRef   = anOrig.StartVertex;
  const BRepGraphInc::VertexRef aOrigEndVertexRef     = anOrig.EndVertex;
  const bool                    aOrigSameRange        = anOrig.SameRange;

  // Copy wire indices: ReverseIdx may be rebuilt below.
  const NCollection_Vector<BRepGraph_WireId>* aOrigWiresPtr =
    theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeDef.Index));
  const NCollection_Vector<BRepGraph_WireId> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<BRepGraph_WireId>();

  // Allocate SubA slot.
  BRepGraph_TopoNode::EdgeDef& aSubADef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubAIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubADef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
  theSubA                               = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference - use index).
  BRepGraph_TopoNode::EdgeDef& aSubBDef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubBIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubBDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
  theSubB                               = aSubBDef.Id;

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraph_TopoNode::EdgeDef& aSubA =
      theGraph.myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubAIdx));
    aSubA.Curve3DRepId          = aOrigCurve3DRepId;
    aSubA.Tolerance             = aOrigTolerance;
    aSubA.SameParameter         = aOrigSameParameter;
    aSubA.SameRange             = false;
    aSubA.IsDegenerate          = false;
    aSubA.StartVertex           = aOrigStartVertexRef; // copy full ref with Location
    aSubA.EndVertex.VertexDefId = theSplitVertex.IsValid()
                                    ? BRepGraph_VertexId::FromNodeId(theSplitVertex)
                                    : BRepGraph_VertexId();
    aSubA.EndVertex.Orientation = TopAbs_REVERSED;
    aSubA.ParamFirst            = aOrigParamFirst;
    aSubA.ParamLast             = theSplitParam;
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraph_TopoNode::EdgeDef& aSubB =
      theGraph.myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubBIdx));
    aSubB.Curve3DRepId            = aOrigCurve3DRepId;
    aSubB.Tolerance               = aOrigTolerance;
    aSubB.SameParameter           = aOrigSameParameter;
    aSubB.SameRange               = false;
    aSubB.IsDegenerate            = false;
    aSubB.StartVertex.VertexDefId = theSplitVertex.IsValid()
                                      ? BRepGraph_VertexId::FromNodeId(theSplitVertex)
                                      : BRepGraph_VertexId();
    aSubB.StartVertex.Orientation = TopAbs_FORWARD;
    aSubB.EndVertex               = aOrigEndVertexRef; // copy full ref with Location
    aSubB.ParamFirst              = theSplitParam;
    aSubB.ParamLast               = aOrigParamLast;
  }

  theGraph.allocateUID(theSubA);
  theGraph.allocateUID(theSubB);

  // Update incidence: wire CoEdgeRefs and mark original as removed.
  {
    // Update WireEntity.CoEdgeRefs: replace original edge's coedge with SubA+SubB coedges.
    const NCollection_Vector<BRepGraph_WireId>* aWireIndices =
      theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeDef.Index));
    if (aWireIndices != nullptr)
    {
      for (int aWIdx = 0; aWIdx < aWireIndices->Length(); ++aWIdx)
      {
        const int aWireIdx = aWireIndices->Value(aWIdx).Index;
        if (aWireIdx < 0 || aWireIdx >= theGraph.myData->myIncStorage.NbWires())
          continue;
        BRepGraphInc::WireEntity& aWire =
          theGraph.myData->myIncStorage.ChangeWire(BRepGraph_WireId(aWireIdx));
        for (int aCERIdx = 0; aCERIdx < aWire.CoEdgeRefs.Length(); ++aCERIdx)
        {
          const int aOldCoEdgeIdx = aWire.CoEdgeRefs.Value(aCERIdx).CoEdgeDefId.Index;
          const BRepGraphInc::CoEdgeEntity& aOldCoEdge =
            theGraph.myData->myIncStorage.CoEdge(BRepGraph_CoEdgeId(aOldCoEdgeIdx));
          if (aOldCoEdge.EdgeDefId == theEdgeDef)
          {
            const TopAbs_Orientation aOrigOri = aOldCoEdge.Sense;

            // Replace in-place: update existing coedge to point to SubA.
            theGraph.myData->myIncStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aOldCoEdgeIdx))
              .EdgeDefId = BRepGraph_EdgeId(aSubAIdx);

            // Create a new coedge for SubB and insert after SubA.
            BRepGraphInc::CoEdgeEntity& aSubBCoEdge = theGraph.myData->myIncStorage.AppendCoEdge();
            const int aSubBCoEdgeIdx                = theGraph.myData->myIncStorage.NbCoEdges() - 1;
            aSubBCoEdge.Id                          = BRepGraph_NodeId::CoEdge(aSubBCoEdgeIdx);
            aSubBCoEdge.EdgeDefId                   = BRepGraph_EdgeId(aSubBIdx);
            aSubBCoEdge.Sense                       = aOrigOri;
            aSubBCoEdge.FaceDefId                   = aOldCoEdge.FaceDefId;

            BRepGraphInc::CoEdgeRef aSubBRef;
            aSubBRef.CoEdgeDefId = BRepGraph_CoEdgeId(aSubBCoEdgeIdx);
            aWire.CoEdgeRefs.InsertAfter(aCERIdx, aSubBRef);
            break;
          }
        }
      }
    }

    // Mark original edge as removed in incidence.
    if (theEdgeDef.Index >= 0 && theEdgeDef.Index < theGraph.myData->myIncStorage.NbEdges())
      theGraph.myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId::FromNodeId(theEdgeDef)).IsRemoved =
        true;
  }

  // Split PCurve entries for each CoEdge referencing the original edge.
  // Copy CoEdge data before mutation (vector may reallocate).
  NCollection_Vector<BRepGraphInc::CoEdgeEntity> aOrigCoEdges;
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      theGraph.myData->myIncStorage.ReverseIndex().CoEdgesOfEdge(
        BRepGraph_EdgeId(theEdgeDef.Index));
    if (aCoEdgeIdxs != nullptr)
    {
      for (int i = 0; i < aCoEdgeIdxs->Length(); ++i)
        aOrigCoEdges.Append(theGraph.myData->myIncStorage.CoEdge(aCoEdgeIdxs->Value(i)));
    }
  }

  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
  {
    const BRepGraphInc::CoEdgeEntity& aCE = aOrigCoEdges.Value(aCEIdx);

    // Compute 2D split parameter.
    double aPCSplit;
    if (aOrigSameRange)
    {
      aPCSplit = theSplitParam;
    }
    else
    {
      const double aPCRange = aCE.ParamLast - aCE.ParamFirst;
      if (aParamRange > 0.0)
        aPCSplit = aCE.ParamFirst + ((theSplitParam - aOrigParamFirst) / aParamRange) * aPCRange;
      else
        aPCSplit = 0.5 * (aCE.ParamFirst + aCE.ParamLast);
    }

    // Create CoEdge for SubA.
    BRepGraphInc::CoEdgeEntity& aCoEdgeSubA    = theGraph.myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeSubAIdx = theGraph.myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubA.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeSubAIdx);
    aCoEdgeSubA.EdgeDefId                      = BRepGraph_EdgeId(aSubAIdx);
    aCoEdgeSubA.FaceDefId                      = aCE.FaceDefId;
    aCoEdgeSubA.Sense                          = aCE.Sense;
    aCoEdgeSubA.Curve2DRepId                   = aCE.Curve2DRepId;
    aCoEdgeSubA.ParamFirst                     = aCE.ParamFirst;
    aCoEdgeSubA.ParamLast                      = aPCSplit;
    aCoEdgeSubA.Continuity                     = aCE.Continuity;

    // Create CoEdge for SubB.
    BRepGraphInc::CoEdgeEntity& aCoEdgeSubB    = theGraph.myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeSubBIdx = theGraph.myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubB.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeSubBIdx);
    aCoEdgeSubB.EdgeDefId                      = BRepGraph_EdgeId(aSubBIdx);
    aCoEdgeSubB.FaceDefId                      = aCE.FaceDefId;
    aCoEdgeSubB.Sense                          = aCE.Sense;
    aCoEdgeSubB.Curve2DRepId                   = aCE.Curve2DRepId;
    aCoEdgeSubB.ParamFirst                     = aPCSplit;
    aCoEdgeSubB.ParamLast                      = aCE.ParamLast;
    aCoEdgeSubB.Continuity                     = aCE.Continuity;
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurve3DRepId.IsValid())
  {
    const occ::handle<Geom_Curve>& aOrigCurve3d =
      theGraph.myData->myIncStorage.Curve3DRep(aOrigCurve3DRepId).Curve;
    if (!aOrigCurve3d.IsNull())
    {
      BRep_Builder aBB;

      const TopoDS_Shape aStartVShape = theGraph.Shapes().Shape(aOrigStartVertexDefId);
      const TopoDS_Shape aSplitVShape = theGraph.Shapes().Shape(theSplitVertex);
      const TopoDS_Shape aEndVShape   = theGraph.Shapes().Shape(aOrigEndVertexDefId);

      TopoDS_Edge aSubAEdge;
      aBB.MakeEdge(aSubAEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
      if (!aStartVShape.IsNull())
        aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
      theGraph.myData->myIncStorage.BindOriginal(theSubA, aSubAEdge);

      TopoDS_Edge aSubBEdge;
      aBB.MakeEdge(aSubBEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
      if (!aEndVShape.IsNull())
        aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
      theGraph.myData->myIncStorage.BindOriginal(theSubB, aSubBEdge);
    }
  }

  // Update edge-to-wire reverse index incrementally.
  BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const BRepGraph_WireId aWireId = aOrigWires.Value(aWIdx);
    aRevIdx.UnbindEdgeFromWire(BRepGraph_EdgeId(theEdgeDef.Index), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubAIdx), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubBIdx), aWireId);
    theGraph.markModified(aWireId);
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc::EdgeEntity& aSubAEnt =
      theGraph.myData->myIncStorage.Edge(BRepGraph_EdgeId(aSubAIdx));
    const BRepGraphInc::EdgeEntity& aSubBEnt =
      theGraph.myData->myIncStorage.Edge(BRepGraph_EdgeId(aSubBIdx));
    BRepGraphInc_ReverseIndex& aRevIdxMut = theGraph.myData->myIncStorage.ChangeReverseIndex();

    if (aSubAEnt.StartVertex.VertexDefId.IsValid())
      aRevIdxMut.BindVertexToEdge(aSubAEnt.StartVertex.VertexDefId, BRepGraph_EdgeId(aSubAIdx));
    if (aSubAEnt.EndVertex.VertexDefId.IsValid())
      aRevIdxMut.BindVertexToEdge(aSubAEnt.EndVertex.VertexDefId, BRepGraph_EdgeId(aSubAIdx));
    if (aSubBEnt.StartVertex.VertexDefId.IsValid())
      aRevIdxMut.BindVertexToEdge(aSubBEnt.StartVertex.VertexDefId, BRepGraph_EdgeId(aSubBIdx));
    if (aSubBEnt.EndVertex.VertexDefId.IsValid())
      aRevIdxMut.BindVertexToEdge(aSubBEnt.EndVertex.VertexDefId, BRepGraph_EdgeId(aSubBIdx));

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(BRepGraph_VertexId(aOrigStartVertexDefId.Index),
                                      BRepGraph_EdgeId(theEdgeDef.Index));
    if (aOrigEndVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(BRepGraph_VertexId(aOrigEndVertexDefId.Index),
                                      BRepGraph_EdgeId(theEdgeDef.Index));

    // Edge-to-face: derive from original edge's CoEdges (same faces apply to both sub-edges).
    for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aOrigCoEdges.Value(aCEIdx);
      if (aCE.FaceDefId.IsValid())
      {
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubAIdx), aCE.FaceDefId);
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubBIdx), aCE.FaceDefId);
      }
    }
  }

  theGraph.markModified(theEdgeDef);
  theGraph.markModified(theSubA);
  theGraph.markModified(theSubB);

  Standard_ASSERT_VOID(theGraph.myData->myIncStorage.ValidateReverseIndex(),
                       "SplitEdge: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph_Mutator::ReplaceEdgeInWire(BRepGraph&             theGraph,
                                          const BRepGraph_WireId theWireDefId,
                                          const BRepGraph_EdgeId theOldEdgeDef,
                                          const BRepGraph_EdgeId theNewEdgeDef,
                                          const bool             theReversed)
{
  BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myIncStorage.ChangeWire(theWireDefId);

  // Update incidence CoEdgeRefs on the WireEntity.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
  {
    const int aCoEdgeEntIdx = aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId.Index;
    BRepGraphInc::CoEdgeEntity& aCoEdge =
      theGraph.myData->myIncStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aCoEdgeEntIdx));
    if (aCoEdge.EdgeDefId == theOldEdgeDef)
    {
      aCoEdge.EdgeDefId = theNewEdgeDef;
      if (theReversed)
        aCoEdge.Sense = TopAbs::Reverse(aCoEdge.Sense);

      // Update reverse indices incrementally.
      BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
      aRevIdx.ReplaceEdgeInWireMap(theOldEdgeDef, theNewEdgeDef, theWireDefId);
      aRevIdx.BindEdgeToCoEdge(theNewEdgeDef, BRepGraph_CoEdgeId(aCoEdgeEntIdx));

      // Update edge-to-face: bind new edge, unbind old edge for all faces of this wire.
      // Wire-to-face mappings are built from FaceEntity.WireRefs during Build() and are
      // stable across edge mutations - only face-level operations modify them.
      const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(theWireDefId);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_FaceId aFaceId = aFaces->Value(aFIdx);
          aRevIdx.BindEdgeToFace(theNewEdgeDef, aFaceId);
          aRevIdx.UnbindEdgeFromFace(theOldEdgeDef, aFaceId);
        }
      }
    }
  }

  theGraph.markModified(BRepGraph_NodeId::Wire(theWireDefId.Index));

  Standard_ASSERT_VOID(theGraph.myData->myIncStorage.ValidateReverseIndex(),
                       "ReplaceEdgeInWire: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph_Mutator::CommitMutation(BRepGraph& theGraph)
{
  Standard_ASSERT_VOID(theGraph.myData->myIncStorage.ValidateReverseIndex(),
                       "CommitMutation: reverse index inconsistency");

  // Verify active counts match actual entity state.
  const BRepGraphInc_Storage& aStg = theGraph.myData->myIncStorage;

  int aNbVtx = 0, aNbEdg = 0, aNbWir = 0, aNbFac = 0;
  int aNbShl = 0, aNbSol = 0, aNbCmp = 0, aNbCS = 0;
  for (int i = 0; i < aStg.NbVertices(); ++i)
    if (!aStg.Vertex(BRepGraph_VertexId(i)).IsRemoved)
      ++aNbVtx;
  for (int i = 0; i < aStg.NbEdges(); ++i)
    if (!aStg.Edge(BRepGraph_EdgeId(i)).IsRemoved)
      ++aNbEdg;
  for (int i = 0; i < aStg.NbWires(); ++i)
    if (!aStg.Wire(BRepGraph_WireId(i)).IsRemoved)
      ++aNbWir;
  for (int i = 0; i < aStg.NbFaces(); ++i)
    if (!aStg.Face(BRepGraph_FaceId(i)).IsRemoved)
      ++aNbFac;
  for (int i = 0; i < aStg.NbShells(); ++i)
    if (!aStg.Shell(BRepGraph_ShellId(i)).IsRemoved)
      ++aNbShl;
  for (int i = 0; i < aStg.NbSolids(); ++i)
    if (!aStg.Solid(BRepGraph_SolidId(i)).IsRemoved)
      ++aNbSol;
  for (int i = 0; i < aStg.NbCompounds(); ++i)
    if (!aStg.Compound(BRepGraph_CompoundId(i)).IsRemoved)
      ++aNbCmp;
  for (int i = 0; i < aStg.NbCompSolids(); ++i)
    if (!aStg.CompSolid(BRepGraph_CompSolidId(i)).IsRemoved)
      ++aNbCS;

  Standard_ASSERT_VOID(aNbVtx == aStg.NbActiveVertices(),
                       "CommitMutation: active vertex count mismatch");
  Standard_ASSERT_VOID(aNbEdg == aStg.NbActiveEdges(),
                       "CommitMutation: active edge count mismatch");
  Standard_ASSERT_VOID(aNbWir == aStg.NbActiveWires(),
                       "CommitMutation: active wire count mismatch");
  Standard_ASSERT_VOID(aNbFac == aStg.NbActiveFaces(),
                       "CommitMutation: active face count mismatch");
  Standard_ASSERT_VOID(aNbShl == aStg.NbActiveShells(),
                       "CommitMutation: active shell count mismatch");
  Standard_ASSERT_VOID(aNbSol == aStg.NbActiveSolids(),
                       "CommitMutation: active solid count mismatch");
  Standard_ASSERT_VOID(aNbCmp == aStg.NbActiveCompounds(),
                       "CommitMutation: active compound count mismatch");
  Standard_ASSERT_VOID(aNbCS == aStg.NbActiveCompSolids(),
                       "CommitMutation: active compsolid count mismatch");

  // Suppress unused-variable warnings in non-debug builds where Standard_ASSERT_VOID is a no-op.
  (void)aNbVtx;
  (void)aNbEdg;
  (void)aNbWir;
  (void)aNbFac;
  (void)aNbShl;
  (void)aNbSol;
  (void)aNbCmp;
  (void)aNbCS;
}
