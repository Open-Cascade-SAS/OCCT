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
  const BRepGraph_TopoNode::EdgeDef& anOrig = theGraph.myData->myIncStorage.Edge(theEdgeDef.Index);

  const int                     aOrigCurve3DRepIdx    = anOrig.Curve3DRepIdx;
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
  const NCollection_Vector<int>* aOrigWiresPtr =
    theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(theEdgeDef.Index);
  const NCollection_Vector<int> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<int>();

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
    BRepGraph_TopoNode::EdgeDef& aSubA = theGraph.myData->myIncStorage.ChangeEdge(aSubAIdx);
    aSubA.Curve3DRepIdx                = aOrigCurve3DRepIdx;
    aSubA.Tolerance                    = aOrigTolerance;
    aSubA.SameParameter                = aOrigSameParameter;
    aSubA.SameRange                    = false;
    aSubA.IsDegenerate                 = false;
    aSubA.StartVertex                  = aOrigStartVertexRef; // copy full ref with Location
    aSubA.EndVertex.VertexIdx          = theSplitVertex.IsValid() ? theSplitVertex.Index : -1;
    aSubA.EndVertex.Orientation        = TopAbs_REVERSED;
    aSubA.ParamFirst                   = aOrigParamFirst;
    aSubA.ParamLast                    = theSplitParam;
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraph_TopoNode::EdgeDef& aSubB = theGraph.myData->myIncStorage.ChangeEdge(aSubBIdx);
    aSubB.Curve3DRepIdx                = aOrigCurve3DRepIdx;
    aSubB.Tolerance                    = aOrigTolerance;
    aSubB.SameParameter                = aOrigSameParameter;
    aSubB.SameRange                    = false;
    aSubB.IsDegenerate                 = false;
    aSubB.StartVertex.VertexIdx        = theSplitVertex.IsValid() ? theSplitVertex.Index : -1;
    aSubB.StartVertex.Orientation      = TopAbs_FORWARD;
    aSubB.EndVertex                    = aOrigEndVertexRef; // copy full ref with Location
    aSubB.ParamFirst                   = theSplitParam;
    aSubB.ParamLast                    = aOrigParamLast;
  }

  theGraph.allocateUID(theSubA);
  theGraph.allocateUID(theSubB);

  // Update incidence: wire CoEdgeRefs and mark original as removed.
  {
    // Update WireEntity.CoEdgeRefs: replace original edge's coedge with SubA+SubB coedges.
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
        for (int aCERIdx = 0; aCERIdx < aWire.CoEdgeRefs.Length(); ++aCERIdx)
        {
          const int aOldCoEdgeIdx = aWire.CoEdgeRefs.Value(aCERIdx).CoEdgeIdx;
          const BRepGraphInc::CoEdgeEntity& aOldCoEdge =
            theGraph.myData->myIncStorage.CoEdge(aOldCoEdgeIdx);
          if (aOldCoEdge.EdgeIdx == theEdgeDef.Index)
          {
            const TopAbs_Orientation aOrigOri = aOldCoEdge.Sense;

            // Replace in-place: update existing coedge to point to SubA.
            theGraph.myData->myIncStorage.ChangeCoEdge(aOldCoEdgeIdx).EdgeIdx = aSubAIdx;

            // Create a new coedge for SubB and insert after SubA.
            BRepGraphInc::CoEdgeEntity& aSubBCoEdge = theGraph.myData->myIncStorage.AppendCoEdge();
            const int aSubBCoEdgeIdx                = theGraph.myData->myIncStorage.NbCoEdges() - 1;
            aSubBCoEdge.Id                          = BRepGraph_NodeId::CoEdge(aSubBCoEdgeIdx);
            aSubBCoEdge.EdgeIdx                     = aSubBIdx;
            aSubBCoEdge.Sense                       = aOrigOri;
            aSubBCoEdge.FaceDefId                   = aOldCoEdge.FaceDefId;

            BRepGraphInc::CoEdgeRef aSubBRef;
            aSubBRef.CoEdgeIdx = aSubBCoEdgeIdx;
            aWire.CoEdgeRefs.InsertAfter(aCERIdx, aSubBRef);
            break;
          }
        }
      }
    }

    // Mark original edge as removed in incidence.
    if (theEdgeDef.Index >= 0 && theEdgeDef.Index < theGraph.myData->myIncStorage.NbEdges())
      theGraph.myData->myIncStorage.ChangeEdge(theEdgeDef.Index).IsRemoved = true;
  }

  // Split PCurve entries for each CoEdge referencing the original edge.
  // Copy CoEdge data before mutation (vector may reallocate).
  NCollection_Vector<BRepGraphInc::CoEdgeEntity> aOrigCoEdges;
  {
    const NCollection_Vector<int>* aCoEdgeIdxs =
      theGraph.myData->myIncStorage.ReverseIndex().CoEdgesOfEdge(theEdgeDef.Index);
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
    aCoEdgeSubA.EdgeIdx                        = aSubAIdx;
    aCoEdgeSubA.FaceDefId                      = aCE.FaceDefId;
    aCoEdgeSubA.Sense                          = aCE.Sense;
    aCoEdgeSubA.Curve2DRepIdx                  = aCE.Curve2DRepIdx;
    aCoEdgeSubA.ParamFirst                     = aCE.ParamFirst;
    aCoEdgeSubA.ParamLast                      = aPCSplit;
    aCoEdgeSubA.Continuity                     = aCE.Continuity;

    // Create CoEdge for SubB.
    BRepGraphInc::CoEdgeEntity& aCoEdgeSubB    = theGraph.myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeSubBIdx = theGraph.myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubB.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeSubBIdx);
    aCoEdgeSubB.EdgeIdx                        = aSubBIdx;
    aCoEdgeSubB.FaceDefId                      = aCE.FaceDefId;
    aCoEdgeSubB.Sense                          = aCE.Sense;
    aCoEdgeSubB.Curve2DRepIdx                  = aCE.Curve2DRepIdx;
    aCoEdgeSubB.ParamFirst                     = aPCSplit;
    aCoEdgeSubB.ParamLast                      = aCE.ParamLast;
    aCoEdgeSubB.Continuity                     = aCE.Continuity;
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurve3DRepIdx >= 0)
  {
    const occ::handle<Geom_Curve>& aOrigCurve3d =
      theGraph.myData->myIncStorage.Curve3DRep(aOrigCurve3DRepIdx).Curve;
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
    const int aWireIdx = aOrigWires.Value(aWIdx);
    aRevIdx.UnbindEdgeFromWire(theEdgeDef.Index, aWireIdx);
    aRevIdx.BindEdgeToWire(aSubAIdx, aWireIdx);
    aRevIdx.BindEdgeToWire(aSubBIdx, aWireIdx);
    theGraph.markModified(BRepGraph_NodeId::Wire(aWireIdx));
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc::EdgeEntity& aSubAEnt   = theGraph.myData->myIncStorage.Edge(aSubAIdx);
    const BRepGraphInc::EdgeEntity& aSubBEnt   = theGraph.myData->myIncStorage.Edge(aSubBIdx);
    BRepGraphInc_ReverseIndex&      aRevIdxMut = theGraph.myData->myIncStorage.ChangeReverseIndex();

    if (aSubAEnt.StartVertex.VertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubAEnt.StartVertex.VertexIdx, aSubAIdx);
    if (aSubAEnt.EndVertex.VertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubAEnt.EndVertex.VertexIdx, aSubAIdx);
    if (aSubBEnt.StartVertex.VertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubBEnt.StartVertex.VertexIdx, aSubBIdx);
    if (aSubBEnt.EndVertex.VertexIdx >= 0)
      aRevIdxMut.BindVertexToEdge(aSubBEnt.EndVertex.VertexIdx, aSubBIdx);

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigStartVertexDefId.Index, theEdgeDef.Index);
    if (aOrigEndVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigEndVertexDefId.Index, theEdgeDef.Index);

    // Edge-to-face: derive from original edge's CoEdges (same faces apply to both sub-edges).
    for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCE = aOrigCoEdges.Value(aCEIdx);
      if (aCE.FaceDefId.IsValid())
      {
        aRevIdxMut.BindEdgeToFace(aSubAIdx, aCE.FaceDefId.Index);
        aRevIdxMut.BindEdgeToFace(aSubBIdx, aCE.FaceDefId.Index);
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
                                          const int              theWireDefIdx,
                                          const BRepGraph_NodeId theOldEdgeDef,
                                          const BRepGraph_NodeId theNewEdgeDef,
                                          const bool             theReversed)
{
  BRepGraph_TopoNode::WireDef& aWireDef = theGraph.myData->myIncStorage.ChangeWire(theWireDefIdx);

  // Update incidence CoEdgeRefs on the WireEntity.
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
  {
    const int                   aCoEdgeEntIdx = aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeIdx;
    BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.myData->myIncStorage.ChangeCoEdge(aCoEdgeEntIdx);
    if (aCoEdge.EdgeIdx == theOldEdgeDef.Index)
    {
      aCoEdge.EdgeIdx = theNewEdgeDef.Index;
      if (theReversed)
        aCoEdge.Sense = TopAbs::Reverse(aCoEdge.Sense);

      // Update reverse indices incrementally.
      BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
      aRevIdx.ReplaceEdgeInWireMap(theOldEdgeDef.Index, theNewEdgeDef.Index, theWireDefIdx);
      aRevIdx.BindEdgeToCoEdge(theNewEdgeDef.Index, aCoEdgeEntIdx);

      // Update edge-to-face: bind new edge, unbind old edge for all faces of this wire.
      // Wire-to-face mappings are built from FaceEntity.WireRefs during Build() and are
      // stable across edge mutations - only face-level operations modify them.
      const NCollection_Vector<int>* aFaces = aRevIdx.FacesOfWire(theWireDefIdx);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const int aFaceIdx = aFaces->Value(aFIdx);
          aRevIdx.BindEdgeToFace(theNewEdgeDef.Index, aFaceIdx);
          aRevIdx.UnbindEdgeFromFace(theOldEdgeDef.Index, aFaceIdx);
        }
      }
    }
  }

  theGraph.markModified(BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWireDefIdx));

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
    if (!aStg.Vertex(i).IsRemoved)
      ++aNbVtx;
  for (int i = 0; i < aStg.NbEdges(); ++i)
    if (!aStg.Edge(i).IsRemoved)
      ++aNbEdg;
  for (int i = 0; i < aStg.NbWires(); ++i)
    if (!aStg.Wire(i).IsRemoved)
      ++aNbWir;
  for (int i = 0; i < aStg.NbFaces(); ++i)
    if (!aStg.Face(i).IsRemoved)
      ++aNbFac;
  for (int i = 0; i < aStg.NbShells(); ++i)
    if (!aStg.Shell(i).IsRemoved)
      ++aNbShl;
  for (int i = 0; i < aStg.NbSolids(); ++i)
    if (!aStg.Solid(i).IsRemoved)
      ++aNbSol;
  for (int i = 0; i < aStg.NbCompounds(); ++i)
    if (!aStg.Compound(i).IsRemoved)
      ++aNbCmp;
  for (int i = 0; i < aStg.NbCompSolids(); ++i)
    if (!aStg.CompSolid(i).IsRemoved)
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
