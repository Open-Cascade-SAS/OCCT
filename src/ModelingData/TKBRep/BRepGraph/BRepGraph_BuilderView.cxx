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

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph_Layer.hxx>
#include <BRepGraph_ParamLayer.hxx>
#include <BRepGraph_RegularityLayer.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_PackedMap.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>

#include <shared_mutex>

namespace
{

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectCompoundChildren(
  const BRepGraph&       theGraph,
  const BRepGraph_NodeId theCompoundNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs = theGraph.Refs();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbChildRefs(); ++aRefIdx)
  {
    const BRepGraphInc::ChildRef& aRef = aRefs.Child(BRepGraph_ChildRefId(aRefIdx));
    if (aRef.ParentId == theCompoundNodeId && !aRef.IsRemoved && aRef.ChildDefId.IsValid())
      aChildNodes.Append(aRef.ChildDefId);
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectCompSolidChildren(
  const BRepGraph&       theGraph,
  const BRepGraph_NodeId theCompSolidNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs     = theGraph.Refs();
  const int                            aNbSolids = theGraph.Topo().NbSolids();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbSolidRefs(); ++aRefIdx)
  {
    const BRepGraphInc::SolidRef& aRef = aRefs.Solid(BRepGraph_SolidRefId(aRefIdx));
    if (aRef.ParentId == theCompSolidNodeId && !aRef.IsRemoved
        && aRef.SolidDefId.IsValid(aNbSolids))
      aChildNodes.Append(BRepGraph_NodeId::Solid(aRef.SolidDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectSolidChildren(const BRepGraph&       theGraph,
                                                          const BRepGraph_NodeId theSolidNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs     = theGraph.Refs();
  const int                            aNbShells = theGraph.Topo().NbShells();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbShellRefs(); ++aRefIdx)
  {
    const BRepGraphInc::ShellRef& aRef = aRefs.Shell(BRepGraph_ShellRefId(aRefIdx));
    if (aRef.ParentId == theSolidNodeId && !aRef.IsRemoved && aRef.ShellDefId.IsValid(aNbShells))
      aChildNodes.Append(BRepGraph_NodeId::Shell(aRef.ShellDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectShellChildren(const BRepGraph&       theGraph,
                                                          const BRepGraph_NodeId theShellNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs    = theGraph.Refs();
  const int                            aNbFaces = theGraph.Topo().NbFaces();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbFaceRefs(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aRef = aRefs.Face(BRepGraph_FaceRefId(aRefIdx));
    if (aRef.ParentId == theShellNodeId && !aRef.IsRemoved && aRef.FaceDefId.IsValid(aNbFaces))
      aChildNodes.Append(BRepGraph_NodeId::Face(aRef.FaceDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectFaceChildren(const BRepGraph&       theGraph,
                                                         const BRepGraph_NodeId theFaceNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::RefsView&           aRefs    = theGraph.Refs();
  const int                            aNbWires = theGraph.Topo().NbWires();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbWireRefs(); ++aRefIdx)
  {
    const BRepGraphInc::WireRef& aRef = aRefs.Wire(BRepGraph_WireRefId(aRefIdx));
    if (aRef.ParentId == theFaceNodeId && !aRef.IsRemoved && aRef.WireDefId.IsValid(aNbWires))
      aChildNodes.Append(BRepGraph_NodeId::Wire(aRef.WireDefId.Index));
  }
  return aChildNodes;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectWireChildren(const BRepGraph&       theGraph,
                                                         const BRepGraph_NodeId theWireNodeId)
{
  NCollection_Vector<BRepGraph_NodeId> aChildNodes;
  const BRepGraph::TopoView&           aTopo      = theGraph.Topo();
  const BRepGraph::RefsView&           aRefs      = theGraph.Refs();
  const int                            aNbCoEdges = aTopo.NbCoEdges();
  for (int aRefIdx = 0; aRefIdx < aRefs.NbCoEdgeRefs(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRef& aRef = aRefs.CoEdge(BRepGraph_CoEdgeRefId(aRefIdx));
    if (aRef.ParentId != theWireNodeId || aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(aNbCoEdges))
      continue;

    const BRepGraphInc::CoEdgeDef& aCoEdge = aTopo.CoEdge(aRef.CoEdgeDefId);
    if (aCoEdge.EdgeDefId.IsValid(aTopo.NbEdges()))
      aChildNodes.Append(BRepGraph_NodeId::Edge(aCoEdge.EdgeDefId.Index));
  }
  return aChildNodes;
}

const char* kindName(const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return "Vertices";
    case BRepGraph_NodeId::Kind::Edge:
      return "Edges";
    case BRepGraph_NodeId::Kind::CoEdge:
      return "CoEdges";
    case BRepGraph_NodeId::Kind::Wire:
      return "Wires";
    case BRepGraph_NodeId::Kind::Face:
      return "Faces";
    case BRepGraph_NodeId::Kind::Shell:
      return "Shells";
    case BRepGraph_NodeId::Kind::Solid:
      return "Solids";
    case BRepGraph_NodeId::Kind::Compound:
      return "Compounds";
    case BRepGraph_NodeId::Kind::CompSolid:
      return "CompSolids";
    case BRepGraph_NodeId::Kind::Product:
      return "Products";
    case BRepGraph_NodeId::Kind::Occurrence:
      return "Occurrences";
  }
  return "Unknown";
}

//=================================================================================================

static bool isNodeIndexInRange(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId theNode)
{
  if (theNode.Index < 0)
    return false;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theNode.Index < theStorage.NbVertices();
    case BRepGraph_NodeId::Kind::Edge:
      return theNode.Index < theStorage.NbEdges();
    case BRepGraph_NodeId::Kind::CoEdge:
      return theNode.Index < theStorage.NbCoEdges();
    case BRepGraph_NodeId::Kind::Wire:
      return theNode.Index < theStorage.NbWires();
    case BRepGraph_NodeId::Kind::Face:
      return theNode.Index < theStorage.NbFaces();
    case BRepGraph_NodeId::Kind::Shell:
      return theNode.Index < theStorage.NbShells();
    case BRepGraph_NodeId::Kind::Solid:
      return theNode.Index < theStorage.NbSolids();
    case BRepGraph_NodeId::Kind::Compound:
      return theNode.Index < theStorage.NbCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:
      return theNode.Index < theStorage.NbCompSolids();
    case BRepGraph_NodeId::Kind::Product:
      return theNode.Index < theStorage.NbProducts();
    case BRepGraph_NodeId::Kind::Occurrence:
      return theNode.Index < theStorage.NbOccurrences();
  }

  return false;
}

//=================================================================================================

static void rebindCoEdgesForEdgeReplacement(BRepGraphInc_Storage& theStorage,
                                            const BRepGraph_EdgeId theSourceEdgeId,
                                            const BRepGraph_EdgeId theReplacementEdgeId)
{
  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.ReverseIndex().CoEdgesOfEdgeRef(theSourceEdgeId);
  const int aNbCoEdges = aCoEdgeIdxs.Length();
  NCollection_LocalArray<BRepGraph_CoEdgeId, 16> aCoEdgeSnapshot(aNbCoEdges);
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    aCoEdgeSnapshot[aCoEdgeIdx] = aCoEdgeIdxs.Value(aCoEdgeIdx);
  }

  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeSnapshot[aCoEdgeIdx];
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()))
      continue;

    BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);
    if (aCoEdge.IsRemoved || aCoEdge.EdgeDefId != theSourceEdgeId)
      continue;

    theStorage.ChangeReverseIndex().UnbindEdgeFromCoEdge(theSourceEdgeId, aCoEdgeId);
    aCoEdge.EdgeDefId = theReplacementEdgeId;
    theStorage.ChangeReverseIndex().BindEdgeToCoEdge(theReplacementEdgeId, aCoEdgeId);
  }
}

//=================================================================================================

static void unbindCoEdgesOfRemovedEdge(BRepGraphInc_Storage& theStorage, const BRepGraph_EdgeId theEdgeId)
{
  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    theStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdgeId);
  const int aNbCoEdges = aCoEdgeIdxs.Length();
  NCollection_LocalArray<BRepGraph_CoEdgeId, 16> aCoEdgeSnapshot(aNbCoEdges);
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    aCoEdgeSnapshot[aCoEdgeIdx] = aCoEdgeIdxs.Value(aCoEdgeIdx);
  }

  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aNbCoEdges; ++aCoEdgeIdx)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeSnapshot[aCoEdgeIdx];
    if (!aCoEdgeId.IsValid(theStorage.NbCoEdges()))
      continue;

    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeId);
    if (!aCoEdge.IsRemoved && aCoEdge.EdgeDefId == theEdgeId)
      theStorage.ChangeReverseIndex().UnbindEdgeFromCoEdge(theEdgeId, aCoEdgeId);
  }
}

//=================================================================================================

int countActiveByKind(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId::Kind theKind)
{
  int aCount = 0;
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      for (int i = 0; i < theStorage.NbVertices(); ++i)
        if (!theStorage.Vertex(BRepGraph_VertexId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Edge:
      for (int i = 0; i < theStorage.NbEdges(); ++i)
        if (!theStorage.Edge(BRepGraph_EdgeId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::CoEdge:
      for (int i = 0; i < theStorage.NbCoEdges(); ++i)
        if (!theStorage.CoEdge(BRepGraph_CoEdgeId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Wire:
      for (int i = 0; i < theStorage.NbWires(); ++i)
        if (!theStorage.Wire(BRepGraph_WireId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Face:
      for (int i = 0; i < theStorage.NbFaces(); ++i)
        if (!theStorage.Face(BRepGraph_FaceId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Shell:
      for (int i = 0; i < theStorage.NbShells(); ++i)
        if (!theStorage.Shell(BRepGraph_ShellId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Solid:
      for (int i = 0; i < theStorage.NbSolids(); ++i)
        if (!theStorage.Solid(BRepGraph_SolidId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Compound:
      for (int i = 0; i < theStorage.NbCompounds(); ++i)
        if (!theStorage.Compound(BRepGraph_CompoundId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::CompSolid:
      for (int i = 0; i < theStorage.NbCompSolids(); ++i)
        if (!theStorage.CompSolid(BRepGraph_CompSolidId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Product:
      for (int i = 0; i < theStorage.NbProducts(); ++i)
        if (!theStorage.Product(BRepGraph_ProductId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Occurrence:
      for (int i = 0; i < theStorage.NbOccurrences(); ++i)
        if (!theStorage.Occurrence(BRepGraph_OccurrenceId(i)).IsRemoved)
          ++aCount;
      return aCount;
  }
  return 0;
}

int cachedActiveByKind(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theStorage.NbActiveVertices();
    case BRepGraph_NodeId::Kind::Edge:
      return theStorage.NbActiveEdges();
    case BRepGraph_NodeId::Kind::CoEdge:
      return theStorage.NbActiveCoEdges();
    case BRepGraph_NodeId::Kind::Wire:
      return theStorage.NbActiveWires();
    case BRepGraph_NodeId::Kind::Face:
      return theStorage.NbActiveFaces();
    case BRepGraph_NodeId::Kind::Shell:
      return theStorage.NbActiveShells();
    case BRepGraph_NodeId::Kind::Solid:
      return theStorage.NbActiveSolids();
    case BRepGraph_NodeId::Kind::Compound:
      return theStorage.NbActiveCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:
      return theStorage.NbActiveCompSolids();
    case BRepGraph_NodeId::Kind::Product:
      return theStorage.NbActiveProducts();
    case BRepGraph_NodeId::Kind::Occurrence:
      return theStorage.NbActiveOccurrences();
  }
  return 0;
}

const NCollection_Vector<BRepGraph_CoEdgeRefId>& wireCoEdgeRefIds(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  return theStorage.Wire(theWireId).CoEdgeRefIds;
}

//! Initialize a sub-edge definition produced by SplitEdge.
//! Copies shared properties from the original edge and assigns boundary vertex ref ids.
//! Vertex ref entries must already exist in storage; only their RefId indices are passed.
void initSubEdgeEntity(BRepGraphInc::EdgeDef&       theSub,
                       const BRepGraph_Curve3DRepId theCurve3DRepId,
                       const double                 theTolerance,
                       const bool                   theSameParameter,
                       const BRepGraph_VertexRefId  theStartVertexRefId,
                       const BRepGraph_VertexRefId  theEndVertexRefId,
                       const double                 theParamFirst,
                       const double                 theParamLast)
{
  theSub.Curve3DRepId     = theCurve3DRepId;
  theSub.Tolerance        = theTolerance;
  theSub.SameParameter    = theSameParameter;
  theSub.SameRange        = false;
  theSub.IsDegenerate     = false;
  theSub.StartVertexRefId = theStartVertexRefId;
  theSub.EndVertexRefId   = theEndVertexRefId;
  theSub.ParamFirst       = theParamFirst;
  theSub.ParamLast        = theParamLast;
}

//! Initialize a sub-CoEdge definition produced by SplitEdge.
void initSubCoEdgeEntity(BRepGraphInc::CoEdgeDef&     theCE,
                         const BRepGraph_EdgeId       theEdgeId,
                         const BRepGraph_FaceId       theFaceId,
                         const TopAbs_Orientation     theSense,
                         const BRepGraph_Curve2DRepId theCurve2DRepId,
                         const double                 theParamFirst,
                         const double                 theParamLast,
                         const GeomAbs_Shape          theContinuity)
{
  theCE.EdgeDefId    = theEdgeId;
  theCE.FaceDefId    = theFaceId;
  theCE.Sense        = theSense;
  theCE.Curve2DRepId = theCurve2DRepId;
  theCE.ParamFirst   = theParamFirst;
  theCE.ParamLast    = theParamLast;
  theCE.Continuity   = theContinuity;
}

} // namespace

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddVertex(const gp_Pnt& thePoint,
                                                   const double  theTolerance)
{
  BRepGraphInc::VertexDef& aVtxDef = myGraph->myData->myIncStorage.AppendVertex();
  const int                aIdx    = myGraph->myData->myIncStorage.NbVertices() - 1;
  aVtxDef.Id                       = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, aIdx);
  aVtxDef.Point                    = thePoint;
  aVtxDef.Tolerance                = theTolerance;
  myGraph->allocateUID(aVtxDef.Id);

  return aVtxDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddEdge(const BRepGraph_NodeId         theStartVtx,
                                                 const BRepGraph_NodeId         theEndVtx,
                                                 const occ::handle<Geom_Curve>& theCurve,
                                                 const double                   theFirst,
                                                 const double                   theLast,
                                                 const double                   theTolerance)
{
  BRepGraphInc::EdgeDef& anEdgeDef = myGraph->myData->myIncStorage.AppendEdge();
  const int              aIdx      = myGraph->myData->myIncStorage.NbEdges() - 1;
  anEdgeDef.Id                     = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aIdx);
  if (theStartVtx.IsValid())
  {
    BRepGraphInc::VertexRef& aStartVtxRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                aStartVtxRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aStartVtxRef.RefId                       = BRepGraph_RefId::Vertex(aStartVtxRefIdx);
    aStartVtxRef.ParentId                    = anEdgeDef.Id;
    aStartVtxRef.VertexDefId                 = BRepGraph_VertexId::FromNodeId(theStartVtx);
    aStartVtxRef.Orientation                 = TopAbs_FORWARD;
    myGraph->allocateRefUID(aStartVtxRef.RefId);
    anEdgeDef.StartVertexRefId = BRepGraph_VertexRefId(aStartVtxRefIdx);
  }
  if (theEndVtx.IsValid())
  {
    BRepGraphInc::VertexRef& anEndVtxRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                anEndVtxRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    anEndVtxRef.RefId                       = BRepGraph_RefId::Vertex(anEndVtxRefIdx);
    anEndVtxRef.ParentId                    = anEdgeDef.Id;
    anEndVtxRef.VertexDefId                 = BRepGraph_VertexId::FromNodeId(theEndVtx);
    anEndVtxRef.Orientation                 = TopAbs_REVERSED;
    myGraph->allocateRefUID(anEndVtxRef.RefId);
    anEdgeDef.EndVertexRefId = BRepGraph_VertexRefId(anEndVtxRefIdx);
  }
  anEdgeDef.ParamFirst    = theFirst;
  anEdgeDef.ParamLast     = theLast;
  anEdgeDef.Tolerance     = theTolerance;
  anEdgeDef.SameParameter = true;
  anEdgeDef.SameRange     = true;
  myGraph->allocateUID(anEdgeDef.Id);

  if (!theCurve.IsNull())
  {
    BRepGraphInc::Curve3DRep& aCurveRep    = myGraph->myData->myIncStorage.AppendCurve3DRep();
    const int                 aCurveRepIdx = myGraph->myData->myIncStorage.NbCurves3D() - 1;
    aCurveRep.Id                           = BRepGraph_RepId::Curve3D(aCurveRepIdx);
    aCurveRep.Curve                        = theCurve;
    anEdgeDef.Curve3DRepId                 = BRepGraph_Curve3DRepId(aCurveRepIdx);
  }

  return anEdgeDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddWire(
  const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges)
{
  BRepGraphInc::WireDef& aWireDef = myGraph->myData->myIncStorage.AppendWire();
  const int              aIdx     = myGraph->myData->myIncStorage.NbWires() - 1;
  aWireDef.Id                     = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, aIdx);
  myGraph->allocateUID(aWireDef.Id);

  for (int anEdgeIdx = 0; anEdgeIdx < theEdges.Length(); ++anEdgeIdx)
  {
    const BRepGraph_NodeId   anEdgeDefId = theEdges.Value(anEdgeIdx).first;
    const TopAbs_Orientation anOri       = theEdges.Value(anEdgeIdx).second;

    // Create CoEdge entity for this edge-wire binding.
    BRepGraphInc::CoEdgeDef& aCoEdge    = myGraph->myData->myIncStorage.AppendCoEdge();
    const int                aCoEdgeIdx = myGraph->myData->myIncStorage.NbCoEdges() - 1;
    aCoEdge.Id        = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, aCoEdgeIdx);
    aCoEdge.EdgeDefId = BRepGraph_EdgeId::FromNodeId(anEdgeDefId);
    aCoEdge.Sense     = anOri;
    myGraph->allocateUID(aCoEdge.Id);

    // CoEdgeRef in ref-table.
    BRepGraphInc::CoEdgeRef& aCoEdgeRef    = myGraph->myData->myIncStorage.AppendCoEdgeRef();
    const int                aCoEdgeRefIdx = myGraph->myData->myIncStorage.NbCoEdgeRefs() - 1;
    aCoEdgeRef.RefId                       = BRepGraph_RefId::CoEdge(aCoEdgeRefIdx);
    aCoEdgeRef.ParentId                    = BRepGraph_NodeId::Wire(aIdx);
    aCoEdgeRef.CoEdgeDefId                 = BRepGraph_CoEdgeId(aCoEdgeIdx);
    myGraph->allocateRefUID(aCoEdgeRef.RefId);
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx))
      .CoEdgeRefIds.Append(BRepGraph_CoEdgeRefId(aCoEdgeRefIdx));

    myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToWire(aCoEdge.EdgeDefId,
                                                                      BRepGraph_WireId(aIdx));
    myGraph->myData->myIncStorage.ChangeReverseIndex().BindEdgeToCoEdge(
      aCoEdge.EdgeDefId,
      BRepGraph_CoEdgeId(aCoEdgeIdx));
    myGraph->myData->myIncStorage.ChangeReverseIndex().BindCoEdgeToWire(
      BRepGraph_CoEdgeId(aCoEdgeIdx),
      BRepGraph_WireId(aIdx));
  }

  // Check closure.
  if (!theEdges.IsEmpty())
  {
    const BRepGraph_NodeId   aFirstEdgeNodeId = theEdges.First().first;
    const TopAbs_Orientation aFirstOri        = theEdges.First().second;
    const BRepGraph_NodeId   aLastEdgeNodeId  = theEdges.Last().first;
    const TopAbs_Orientation aLastOri         = theEdges.Last().second;

    const BRepGraphInc::EdgeDef& aFirstEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aFirstEdgeNodeId.Index));
    const BRepGraphInc::EdgeDef& aLastEdge =
      myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(aLastEdgeNodeId.Index));

    const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
    const BRepGraph_VertexRefId aFirstRefId =
      (aFirstOri == TopAbs_FORWARD) ? aFirstEdge.StartVertexRefId : aFirstEdge.EndVertexRefId;
    const BRepGraph_VertexRefId aLastRefId =
      (aLastOri == TopAbs_FORWARD) ? aLastEdge.EndVertexRefId : aLastEdge.StartVertexRefId;
    const BRepGraph_NodeId aFirstVtx =
      aFirstRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aStorage.VertexRef(aFirstRefId).VertexDefId.Index)
        : BRepGraph_NodeId();
    const BRepGraph_NodeId aLastVtx =
      aLastRefId.IsValid()
        ? BRepGraph_NodeId::Vertex(aStorage.VertexRef(aLastRefId).VertexDefId.Index)
        : BRepGraph_NodeId();

    const bool aIsClosed = aFirstVtx.IsValid() && aLastVtx.IsValid() && aFirstVtx == aLastVtx;
    myGraph->myData->myIncStorage.ChangeWire(BRepGraph_WireId(aIdx)).IsClosed = aIsClosed;
  }

  return myGraph->myData->myIncStorage.Wire(BRepGraph_WireId(aIdx)).Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddFace(
  const occ::handle<Geom_Surface>&            theSurface,
  const BRepGraph_NodeId                      theOuterWire,
  const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
  const double                                theTolerance)
{
  BRepGraphInc::FaceDef& aFaceDef = myGraph->myData->myIncStorage.AppendFace();
  const int              aIdx     = myGraph->myData->myIncStorage.NbFaces() - 1;
  aFaceDef.Id                     = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, aIdx);
  aFaceDef.Tolerance              = theTolerance;
  myGraph->allocateUID(aFaceDef.Id);
  if (!theSurface.IsNull())
  {
    BRepGraphInc::SurfaceRep& aSurfRep    = myGraph->myData->myIncStorage.AppendSurfaceRep();
    const int                 aSurfRepIdx = myGraph->myData->myIncStorage.NbSurfaces() - 1;
    aSurfRep.Id                           = BRepGraph_RepId::Surface(aSurfRepIdx);
    aSurfRep.Surface                      = theSurface;
    aFaceDef.SurfaceRepId                 = BRepGraph_SurfaceRepId(aSurfRepIdx);
  }

  // Link wire refs.
  if (theOuterWire.IsValid())
  {
    BRepGraphInc::WireRef& aWireRef    = myGraph->myData->myIncStorage.AppendWireRef();
    const int              aWireRefIdx = myGraph->myData->myIncStorage.NbWireRefs() - 1;
    aWireRef.RefId                     = BRepGraph_RefId::Wire(aWireRefIdx);
    aWireRef.ParentId                  = BRepGraph_NodeId::Face(aIdx);
    aWireRef.WireDefId                 = BRepGraph_WireId::FromNodeId(theOuterWire);
    aWireRef.IsOuter                   = true;
    myGraph->allocateRefUID(aWireRef.RefId);
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx))
      .WireRefIds.Append(BRepGraph_WireRefId(aWireRefIdx));
  }
  for (int aWireIdx = 0; aWireIdx < theInnerWires.Length(); ++aWireIdx)
  {
    const BRepGraph_NodeId& aWireDefId = theInnerWires.Value(aWireIdx);
    if (!aWireDefId.IsValid())
      continue;

    BRepGraphInc::WireRef& aWireRef    = myGraph->myData->myIncStorage.AppendWireRef();
    const int              aWireRefIdx = myGraph->myData->myIncStorage.NbWireRefs() - 1;
    aWireRef.RefId                     = BRepGraph_RefId::Wire(aWireRefIdx);
    aWireRef.ParentId                  = BRepGraph_NodeId::Face(aIdx);
    aWireRef.WireDefId                 = BRepGraph_WireId::FromNodeId(aWireDefId);
    aWireRef.IsOuter                   = false;
    myGraph->allocateRefUID(aWireRef.RefId);
    myGraph->myData->myIncStorage.ChangeFace(BRepGraph_FaceId(aIdx))
      .WireRefIds.Append(BRepGraph_WireRefId(aWireRefIdx));
  }

  return aFaceDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddShell()
{
  BRepGraphInc::ShellDef& aShellDef = myGraph->myData->myIncStorage.AppendShell();
  const int               aIdx      = myGraph->myData->myIncStorage.NbShells() - 1;
  aShellDef.Id                      = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, aIdx);
  myGraph->allocateUID(aShellDef.Id);

  return aShellDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddSolid()
{
  BRepGraphInc::SolidDef& aSolidDef = myGraph->myData->myIncStorage.AppendSolid();
  const int               aIdx      = myGraph->myData->myIncStorage.NbSolids() - 1;
  aSolidDef.Id                      = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, aIdx);
  myGraph->allocateUID(aSolidDef.Id);

  return aSolidDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AddFaceToShell(const BRepGraph_NodeId   theShellEntity,
                                            const BRepGraph_NodeId   theFaceEntity,
                                            const TopAbs_Orientation theOri)
{
  // Append FaceUsage to shell entity.
  if (theShellEntity.Index >= 0 && theShellEntity.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellEntity.Index)).IsRemoved
      && theFaceEntity.Index >= 0 && theFaceEntity.Index < myGraph->myData->myIncStorage.NbFaces()
      && !myGraph->myData->myIncStorage.Face(BRepGraph_FaceId(theFaceEntity.Index)).IsRemoved)
  {
    BRepGraphInc::FaceRef& aFREntry = myGraph->myData->myIncStorage.AppendFaceRef();
    const int              aFRIdx   = myGraph->myData->myIncStorage.NbFaceRefs() - 1;
    aFREntry.RefId                  = BRepGraph_RefId::Face(aFRIdx);
    aFREntry.ParentId               = theShellEntity;
    aFREntry.FaceDefId              = BRepGraph_FaceId::FromNodeId(theFaceEntity);
    aFREntry.Orientation            = theOri;
    myGraph->allocateRefUID(aFREntry.RefId);
    myGraph->myData->myIncStorage.ChangeShell(BRepGraph_ShellId(theShellEntity.Index))
      .FaceRefIds.Append(BRepGraph_FaceRefId(aFRIdx));
  }

  myGraph->markModified(theShellEntity);
}

//=================================================================================================

void BRepGraph::BuilderView::AddShellToSolid(const BRepGraph_NodeId   theSolidEntity,
                                             const BRepGraph_NodeId   theShellEntity,
                                             const TopAbs_Orientation theOri)
{
  // Append ShellUsage to solid entity.
  if (theSolidEntity.Index >= 0 && theSolidEntity.Index < myGraph->myData->myIncStorage.NbSolids()
      && !myGraph->myData->myIncStorage.Solid(BRepGraph_SolidId(theSolidEntity.Index)).IsRemoved
      && theShellEntity.Index >= 0
      && theShellEntity.Index < myGraph->myData->myIncStorage.NbShells()
      && !myGraph->myData->myIncStorage.Shell(BRepGraph_ShellId(theShellEntity.Index)).IsRemoved)
  {
    BRepGraphInc::ShellRef& aSREntry = myGraph->myData->myIncStorage.AppendShellRef();
    const int               aSRIdx   = myGraph->myData->myIncStorage.NbShellRefs() - 1;
    aSREntry.RefId                   = BRepGraph_RefId::Shell(aSRIdx);
    aSREntry.ParentId                = theSolidEntity;
    aSREntry.ShellDefId              = BRepGraph_ShellId::FromNodeId(theShellEntity);
    aSREntry.Orientation             = theOri;
    myGraph->allocateRefUID(aSREntry.RefId);
    myGraph->myData->myIncStorage.ChangeSolid(BRepGraph_SolidId(theSolidEntity.Index))
      .ShellRefIds.Append(BRepGraph_ShellRefId(aSRIdx));
  }

  myGraph->markModified(theSolidEntity);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompound(
  const NCollection_Vector<BRepGraph_NodeId>& theChildEntities)
{
  BRepGraphInc::CompoundDef& aCompDef = myGraph->myData->myIncStorage.AppendCompound();
  const int                  aIdx     = myGraph->myData->myIncStorage.NbCompounds() - 1;
  aCompDef.Id                         = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, aIdx);
  myGraph->allocateUID(aCompDef.Id);

  for (int aChildIdx = 0; aChildIdx < theChildEntities.Length(); ++aChildIdx)
  {
    const BRepGraph_NodeId& aChild   = theChildEntities.Value(aChildIdx);
    BRepGraphInc::ChildRef& aCREntry = myGraph->myData->myIncStorage.AppendChildRef();
    const int               aCRIdx   = myGraph->myData->myIncStorage.NbChildRefs() - 1;
    aCREntry.RefId                   = BRepGraph_RefId::Child(aCRIdx);
    aCREntry.ParentId                = aCompDef.Id;
    aCREntry.ChildDefId              = aChild;
    myGraph->allocateRefUID(aCREntry.RefId);
    aCompDef.ChildRefIds.Append(BRepGraph_ChildRefId(aCRIdx));
  }

  return aCompDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddCompSolid(
  const NCollection_Vector<BRepGraph_NodeId>& theSolidEntities)
{
  BRepGraphInc::CompSolidDef& aCSolDef = myGraph->myData->myIncStorage.AppendCompSolid();
  const int                   aIdx     = myGraph->myData->myIncStorage.NbCompSolids() - 1;
  aCSolDef.Id                          = BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, aIdx);
  myGraph->allocateUID(aCSolDef.Id);

  for (int aSolIdx = 0; aSolIdx < theSolidEntities.Length(); ++aSolIdx)
  {
    BRepGraphInc::SolidRef& aSREntry = myGraph->myData->myIncStorage.AppendSolidRef();
    const int               aSRIdx   = myGraph->myData->myIncStorage.NbSolidRefs() - 1;
    aSREntry.RefId                   = BRepGraph_RefId::Solid(aSRIdx);
    aSREntry.ParentId                = aCSolDef.Id;
    aSREntry.SolidDefId = BRepGraph_SolidId::FromNodeId(theSolidEntities.Value(aSolIdx));
    myGraph->allocateRefUID(aSREntry.RefId);
    aCSolDef.SolidRefIds.Append(BRepGraph_SolidRefId(aSRIdx));
  }

  return aCSolDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddProduct(const BRepGraph_NodeId theShapeRoot)
{
  BRepGraphInc::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
  const int                 aIdx        = myGraph->myData->myIncStorage.NbProducts() - 1;
  aProductDef.Id                        = BRepGraph_NodeId::Product(aIdx);
  aProductDef.ShapeRootId               = theShapeRoot;
  myGraph->allocateUID(aProductDef.Id);

  return aProductDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddAssemblyProduct()
{
  BRepGraphInc::ProductDef& aProductDef = myGraph->myData->myIncStorage.AppendProduct();
  const int                 aIdx        = myGraph->myData->myIncStorage.NbProducts() - 1;
  aProductDef.Id                        = BRepGraph_NodeId::Product(aIdx);
  // ShapeRootId left invalid - this is an assembly.
  myGraph->allocateUID(aProductDef.Id);

  return aProductDef.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddOccurrence(const BRepGraph_NodeId theParentProduct,
                                                       const BRepGraph_NodeId theReferencedProduct,
                                                       const TopLoc_Location& thePlacement)
{
  // Delegate with no parent occurrence (top-level).
  return AddOccurrence(theParentProduct, theReferencedProduct, thePlacement, BRepGraph_NodeId());
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::BuilderView::AddOccurrence(const BRepGraph_NodeId theParentProduct,
                                                       const BRepGraph_NodeId theReferencedProduct,
                                                       const TopLoc_Location& thePlacement,
                                                       const BRepGraph_NodeId theParentOccurrence)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Validate that both arguments are active Product nodes with valid indices.
  if (theParentProduct.NodeKind != BRepGraph_NodeId::Kind::Product || theParentProduct.Index < 0
      || theParentProduct.Index >= aStorage.NbProducts()
      || aStorage.Product(BRepGraph_ProductId(theParentProduct.Index)).IsRemoved)
    return BRepGraph_NodeId();
  if (theReferencedProduct.NodeKind != BRepGraph_NodeId::Kind::Product
      || theReferencedProduct.Index < 0 || theReferencedProduct.Index >= aStorage.NbProducts()
      || aStorage.Product(BRepGraph_ProductId(theReferencedProduct.Index)).IsRemoved)
    return BRepGraph_NodeId();
  // Prevent self-referencing cycles in the assembly DAG.
  if (theParentProduct.Index == theReferencedProduct.Index)
    return BRepGraph_NodeId();
  // Validate parent occurrence if provided.
  // ParentOccurrenceIdx = -1 when theParentOccurrence is invalid (top-level).
  if (theParentOccurrence.IsValid()
      && (theParentOccurrence.NodeKind != BRepGraph_NodeId::Kind::Occurrence
          || theParentOccurrence.Index < 0 || theParentOccurrence.Index >= aStorage.NbOccurrences()
          || aStorage.Occurrence(BRepGraph_OccurrenceId(theParentOccurrence.Index)).IsRemoved))
    return BRepGraph_NodeId();

  BRepGraphInc::OccurrenceDef& anOccDef = aStorage.AppendOccurrence();
  const int                    anOccIdx = aStorage.NbOccurrences() - 1;
  anOccDef.Id                           = BRepGraph_NodeId::Occurrence(anOccIdx);
  anOccDef.ProductDefId                 = BRepGraph_ProductId::FromNodeId(theReferencedProduct);
  anOccDef.ParentProductDefId           = BRepGraph_ProductId::FromNodeId(theParentProduct);
  anOccDef.ParentOccurrenceDefId        = theParentOccurrence.IsValid()
                                            ? BRepGraph_OccurrenceId::FromNodeId(theParentOccurrence)
                                            : BRepGraph_OccurrenceId();
  anOccDef.Placement                    = thePlacement;
  myGraph->allocateUID(anOccDef.Id);

  // Add OccurrenceRef to the parent product.
  BRepGraphInc::OccurrenceRef& anOccRef    = aStorage.AppendOccurrenceRef();
  const int                    anOccRefIdx = aStorage.NbOccurrenceRefs() - 1;
  anOccRef.RefId                           = BRepGraph_RefId::Occurrence(anOccRefIdx);
  anOccRef.ParentId                        = theParentProduct;
  anOccRef.OccurrenceDefId                 = BRepGraph_OccurrenceId(anOccIdx);
  myGraph->allocateRefUID(anOccRef.RefId);
  aStorage.ChangeProduct(BRepGraph_ProductId(theParentProduct.Index))
    .OccurrenceRefIds.Append(BRepGraph_OccurrenceRefId(anOccRefIdx));

  // Rebuild product→occurrence reverse index to keep it in sync
  // after appending a new occurrence entity.
  aStorage.ChangeReverseIndex().BuildProductOccurrences(aStorage.myOccurrences.Entities,
                                                        aStorage.NbProducts());

  return anOccDef.Id;
}

//=================================================================================================

void BRepGraph::BuilderView::AppendShape(const TopoDS_Shape& theShape, const bool theParallel)
{
  BRepGraph_Builder::Append(*myGraph, theShape, theParallel);
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(const BRepGraph_NodeId theNode)
{
  RemoveNode(theNode, BRepGraph_NodeId());
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveNode(const BRepGraph_NodeId theNode,
                                        const BRepGraph_NodeId theReplacement)
{
  if (!theNode.IsValid())
    return;

  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // When removing an Edge with a replacement, reparent all CoEdges from the
  // removed edge to the replacement edge. This prevents orphaned CoEdges
  // that would be excluded from queries via CoEdgesOfEdge().
  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge && theReplacement.IsValid()
      && theReplacement.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                           "RemoveNode: source edge index is out of range",
                           Standard_VOID_RETURN);
    Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theReplacement),
                           "RemoveNode: replacement edge index is out of range",
                           Standard_VOID_RETURN);
    Standard_ASSERT_RETURN(
      !aStorage.Edge(BRepGraph_EdgeId(theReplacement.Index)).IsRemoved,
      "RemoveNode: replacement edge must be active",
      Standard_VOID_RETURN);

    rebindCoEdgesForEdgeReplacement(aStorage,
                                    BRepGraph_EdgeId::FromNodeId(theNode),
                                    BRepGraph_EdgeId::FromNodeId(theReplacement));
  }

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
    case BRepGraph_NodeId::Kind::Edge:
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Wire:
    case BRepGraph_NodeId::Kind::Face:
    case BRepGraph_NodeId::Kind::Shell:
    case BRepGraph_NodeId::Kind::Solid:
    case BRepGraph_NodeId::Kind::Compound:
    case BRepGraph_NodeId::Kind::CompSolid:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      break;
    default:
      Standard_ASSERT_RETURN(false,
                             "RemoveNode: unsupported node kind",
                             Standard_VOID_RETURN);
  }
  Standard_ASSERT_RETURN(isNodeIndexInRange(aStorage, theNode),
                         "RemoveNode: node index is out of range",
                         Standard_VOID_RETURN);

  if (theNode.NodeKind == BRepGraph_NodeId::Kind::Edge)
  {
    // Keep reverse edge->coedge table coherent for pure removals too.
    unbindCoEdgesOfRemovedEdge(aStorage, BRepGraph_EdgeId::FromNodeId(theNode));
  }

  // Mark removed on the entity (which is the sole definition store).
  BRepGraphInc::BaseDef* aDef = myGraph->ChangeTopoEntity(theNode);
  if (aDef != nullptr && !aDef->IsRemoved)
  {
    myGraph->myData->myIncStorage.MarkRemoved(theNode);
  }

  // Increment OwnGen + SubtreeGen so generation-based cache freshness detects the removal.
  BRepGraphInc::BaseDef* aRemovedDef = myGraph->ChangeTopoEntity(theNode);
  if (aRemovedDef != nullptr)
  {
    ++aRemovedDef->OwnGen;
    ++aRemovedDef->SubtreeGen;
  }

  {
    std::unique_lock<std::shared_mutex> aWriteLock(myGraph->myData->myCurrentShapesMutex);
    myGraph->myData->myCurrentShapes.UnBind(theNode);
  }

  // Notify registered layers.
  myGraph->dispatchLayerOnNodeRemoved(theNode, theReplacement);
}

//=================================================================================================

void BRepGraph::BuilderView::RemoveSubgraph(const BRepGraph_NodeId theNode)
{
  RemoveNode(theNode);

  // Traverse children via reference-entry tables.
  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Compound: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompounds())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectCompoundChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::CompSolid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbCompSolids())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectCompSolidChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Solid: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbSolids())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectSolidChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Shell: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbShells())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectShellChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Face: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbFaces())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectFaceChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Wire: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbWires())
      {
        const NCollection_Vector<BRepGraph_NodeId> aChildNodes =
          collectWireChildren(*myGraph, theNode);
        for (int i = 0; i < aChildNodes.Length(); ++i)
          RemoveSubgraph(aChildNodes.Value(i));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Edge: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbEdges())
      {
        const BRepGraphInc::EdgeDef& anEdge =
          myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId(theNode.Index));
        const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
        if (anEdge.StartVertexRefId.IsValid())
        {
          const BRepGraph_VertexId aStartVtxId =
            aStorage.VertexRef(anEdge.StartVertexRefId).VertexDefId;
          if (aStartVtxId.IsValid())
            RemoveNode(aStartVtxId);
        }
        if (anEdge.EndVertexRefId.IsValid())
        {
          const BRepGraph_VertexId anEndVtxId =
            aStorage.VertexRef(anEdge.EndVertexRefId).VertexDefId;
          if (anEndVtxId.IsValid())
            RemoveNode(anEndVtxId);
        }
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Product: {
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbProducts())
      {
        const BRepGraphInc_Storage&     aStorage = myGraph->myData->myIncStorage;
        const BRepGraphInc::ProductDef& aProduct =
          aStorage.Product(BRepGraph_ProductId(theNode.Index));
        // Snapshot occurrence indices before iterating, because RemoveSubgraph(Occurrence)
        // modifies the parent's OccurrenceRefIds via swap-remove.
        NCollection_Vector<int> anOccIndices;
        for (int i = 0; i < aProduct.OccurrenceRefIds.Length(); ++i)
          anOccIndices.Append(
            aStorage.OccurrenceRef(aProduct.OccurrenceRefIds.Value(i)).OccurrenceDefId.Index);
        for (int i = 0; i < anOccIndices.Length(); ++i)
          RemoveSubgraph(BRepGraph_NodeId::Occurrence(anOccIndices.Value(i)));
      }
      break;
    }
    case BRepGraph_NodeId::Kind::Occurrence: {
      // Remove from parent product's OccurrenceRefIds.
      if (theNode.Index >= 0 && theNode.Index < myGraph->myData->myIncStorage.NbOccurrences())
      {
        const BRepGraphInc::OccurrenceDef& anOcc =
          myGraph->myData->myIncStorage.Occurrence(BRepGraph_OccurrenceId(theNode.Index));
        if (anOcc.ParentProductDefId.IsValid()
            && anOcc.ParentProductDefId.Index < myGraph->myData->myIncStorage.NbProducts())
        {
          NCollection_Vector<BRepGraph_OccurrenceRefId>& aRefIds =
            myGraph->myData->myIncStorage.ChangeProduct(anOcc.ParentProductDefId).OccurrenceRefIds;
          for (int i = 0; i < aRefIds.Length(); ++i)
          {
            BRepGraphInc::OccurrenceRef& aRef =
              myGraph->myData->myIncStorage.ChangeOccurrenceRef(aRefIds.Value(i));
            if (aRef.OccurrenceDefId.Index == theNode.Index)
            {
              if (!aRef.IsRemoved)
              {
                myGraph->myData->myIncStorage.MarkRemovedRef(aRef.RefId);
              }
              if (i < aRefIds.Length() - 1)
                aRefIds.ChangeValue(i) = aRefIds.Value(aRefIds.Length() - 1);
              aRefIds.EraseLast();
              myGraph->markModified(anOcc.ParentProductDefId);
              break;
            }
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::BuilderView::CreateCurve2DRep(
  const occ::handle<Geom2d_Curve>& theCurve2d)
{
  if (theCurve2d.IsNull())
    return BRepGraph_Curve2DRepId();

  BRepGraphInc::Curve2DRep& aRep  = myGraph->myData->myIncStorage.AppendCurve2DRep();
  const int                 anIdx = myGraph->myData->myIncStorage.NbCurves2D() - 1;
  aRep.Id                         = BRepGraph_RepId::Curve2D(anIdx);
  aRep.Curve                      = theCurve2d;
  return BRepGraph_Curve2DRepId(anIdx);
}

//=================================================================================================

void BRepGraph::BuilderView::AddPCurveToEdge(const BRepGraph_NodeId           theEdgeEntity,
                                             const BRepGraph_NodeId           theFaceEntity,
                                             const occ::handle<Geom2d_Curve>& theCurve2d,
                                             const double                     theFirst,
                                             const double                     theLast,
                                             const TopAbs_Orientation         theEdgeOrientation)
{
  BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  // Create CoEdge entity for the new PCurve binding.
  BRepGraphInc::CoEdgeDef& aCoEdge    = aStorage.AppendCoEdge();
  const int                aCoEdgeIdx = aStorage.NbCoEdges() - 1;
  aCoEdge.Id                          = BRepGraph_NodeId::CoEdge(aCoEdgeIdx);
  aCoEdge.EdgeDefId                   = BRepGraph_EdgeId::FromNodeId(theEdgeEntity);
  aCoEdge.FaceDefId                   = BRepGraph_FaceId::FromNodeId(theFaceEntity);
  aCoEdge.Sense                       = theEdgeOrientation;
  if (!theCurve2d.IsNull())
  {
    BRepGraphInc::Curve2DRep& aCurve2DRep    = aStorage.AppendCurve2DRep();
    const int                 aCurve2DRepIdx = aStorage.NbCurves2D() - 1;
    aCurve2DRep.Id                           = BRepGraph_RepId::Curve2D(aCurve2DRepIdx);
    aCurve2DRep.Curve                        = theCurve2d;
    aCoEdge.Curve2DRepId                     = BRepGraph_Curve2DRepId(aCurve2DRepIdx);
  }
  aCoEdge.ParamFirst = theFirst;
  aCoEdge.ParamLast  = theLast;

  // Update reverse indices.
  aStorage.ChangeReverseIndex().BindEdgeToCoEdge(BRepGraph_EdgeId(theEdgeEntity.Index),
                                                 BRepGraph_CoEdgeId(aCoEdgeIdx));
  aStorage.ChangeReverseIndex().BindEdgeToFace(BRepGraph_EdgeId(theEdgeEntity.Index),
                                               BRepGraph_FaceId(theFaceEntity.Index));
  myGraph->allocateUID(aCoEdge.Id);

  myGraph->markModified(theEdgeEntity);
}

//=================================================================================================

void BRepGraph::BuilderView::BeginDeferredInvalidation()
{
  myGraph->myData->myDeferredMode = true;
}

//=================================================================================================

void BRepGraph::BuilderView::EndDeferredInvalidation() noexcept
{
  if (!myGraph->myData->myDeferredMode)
    return;

  myGraph->myData->myDeferredMode = false;

  NCollection_Vector<BRepGraph_NodeId>& aDeferredList = myGraph->myData->myDeferredModified;
  if (aDeferredList.IsEmpty())
    return;

  // Shape cache uses SubtreeGen validation — no bulk clear needed.
  // Stale entries are detected on read via StoredSubtreeGen != entity.SubtreeGen.

  // Propagate SubtreeGen upward from each directly-modified node.
  // Dense per-kind visited flags for O(1) lookup without hashing overhead.
  const BRepGraphInc_ReverseIndex& aRevIdx  = myGraph->myData->myIncStorage.ReverseIndex();
  const BRepGraphInc_Storage&      aStorage = myGraph->myData->myIncStorage;

  // Dense visited arrays indexed by entity index per kind.
  // NCollection_Array1 with bool values: O(1) checked/set by index.
  static constexpr int THE_KIND_COUNT = static_cast<int>(BRepGraph_NodeId::Kind::Occurrence) + 1;
  NCollection_Array1<bool> aVisArrays[THE_KIND_COUNT];
  {
    const int aKindCounts[THE_KIND_COUNT] = {
      aStorage.NbSolids(),     // Kind::Solid      = 0
      aStorage.NbShells(),     // Kind::Shell      = 1
      aStorage.NbFaces(),      // Kind::Face       = 2
      aStorage.NbWires(),      // Kind::Wire       = 3
      aStorage.NbEdges(),      // Kind::Edge       = 4
      aStorage.NbVertices(),   // Kind::Vertex     = 5
      aStorage.NbCompounds(),  // Kind::Compound   = 6
      aStorage.NbCompSolids(), // Kind::CompSolid  = 7
      aStorage.NbCoEdges(),    // Kind::CoEdge     = 8
      0,                       // gap at 9
      aStorage.NbProducts(),   // Kind::Product    = 10
      aStorage.NbOccurrences() // Kind::Occurrence = 11
    };
    for (int aKindIdx = 0; aKindIdx < THE_KIND_COUNT; ++aKindIdx)
    {
      const int aCount = aKindCounts[aKindIdx];
      if (aCount > 0)
        aVisArrays[aKindIdx].Resize(0, aCount - 1, false);
    }
  }

  // Helper lambda: check-and-set visited flag.
  const auto markVisited = [&aVisArrays](const BRepGraph_NodeId theNode) -> bool {
    const int aKindIdx = static_cast<int>(theNode.NodeKind);
    NCollection_Array1<bool>& aArr = aVisArrays[aKindIdx];
    if (aArr.IsEmpty() || theNode.Index > aArr.Upper())
      return false;
    if (aArr.Value(theNode.Index))
      return false;
    aArr.SetValue(theNode.Index, true);
    return true;
  };

  // BFS-style upward propagation: process nodes front-to-back, appending
  // newly discovered parents at the end. The loop index advances through
  // the growing vector, so each node is visited exactly once.
  NCollection_Vector<BRepGraph_NodeId> aAllModified;
  aAllModified.SetIncrement(aDeferredList.Length() * 2);
  int aModifiedKindsMask = 0;

  // Seed with directly modified nodes.
  for (int i = 0; i < aDeferredList.Length(); ++i)
  {
    const BRepGraph_NodeId aNodeId = aDeferredList.Value(i);
    if (markVisited(aNodeId))
    {
      aAllModified.Append(aNodeId);
      aModifiedKindsMask |= BRepGraph_Layer::KindBit(aNodeId.NodeKind);
    }
  }

  // Propagate upward level by level.
  // Process the list from front to back; newly appended parents will be
  // reached in subsequent iterations of the same loop.
  for (int i = 0; i < aAllModified.Length(); ++i)
  {
    const BRepGraph_NodeId aNodeId = aAllModified.Value(i);

    // Collect parent NodeIds via reverse index and increment SubtreeGen.
    // NOT OwnGen — parent's own data didn't change.
    // The visited set ensures each parent is incremented exactly once per flush,
    // even in diamond topologies. This matches immediate-mode LastPropWave semantics.
    switch (aNodeId.NodeKind)
    {
      case BRepGraph_NodeId::Kind::Vertex:
        // Vertex modifications don't propagate in deferred mode.
        break;
      case BRepGraph_NodeId::Kind::Edge: {
        const NCollection_Vector<BRepGraph_WireId>* aWires =
          aRevIdx.WiresOfEdge(BRepGraph_EdgeId(aNodeId.Index));
        if (aWires != nullptr)
          for (int w = 0; w < aWires->Length(); ++w)
          {
            const BRepGraph_NodeId aParentId = aWires->Value(w);
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->ChangeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::CoEdge:
        break;
      case BRepGraph_NodeId::Kind::Wire: {
        const NCollection_Vector<BRepGraph_FaceId>* aFaces =
          aRevIdx.FacesOfWire(BRepGraph_WireId(aNodeId.Index));
        if (aFaces != nullptr)
          for (int f = 0; f < aFaces->Length(); ++f)
          {
            const BRepGraph_NodeId aParentId = aFaces->Value(f);
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->ChangeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::Face: {
        const NCollection_Vector<BRepGraph_ShellId>* aShells =
          aRevIdx.ShellsOfFace(BRepGraph_FaceId(aNodeId.Index));
        if (aShells != nullptr)
          for (int s = 0; s < aShells->Length(); ++s)
          {
            const BRepGraph_NodeId aParentId = aShells->Value(s);
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->ChangeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      case BRepGraph_NodeId::Kind::Shell: {
        const NCollection_Vector<BRepGraph_SolidId>* aSolids =
          aRevIdx.SolidsOfShell(BRepGraph_ShellId(aNodeId.Index));
        if (aSolids != nullptr)
          for (int s = 0; s < aSolids->Length(); ++s)
          {
            const BRepGraph_NodeId aParentId = aSolids->Value(s);
            if (!markVisited(aParentId))
              continue;
            BRepGraphInc::BaseDef* aParent = myGraph->ChangeTopoEntity(aParentId);
            if (aParent == nullptr || aParent->IsRemoved)
              continue;
            ++aParent->SubtreeGen;
            aAllModified.Append(aParentId);
            aModifiedKindsMask |= BRepGraph_Layer::KindBit(aParentId.NodeKind);
          }
        break;
      }
      default:
        break;
    }
  }

  // Dispatch batch modification event to subscribing layers.
  if (myGraph->myHasModificationSubscribers && !aAllModified.IsEmpty())
    myGraph->dispatchNodesModified(aAllModified, aModifiedKindsMask);

  // Clear deferred list for next scope.
  aDeferredList.Clear();
}

//=================================================================================================

bool BRepGraph::BuilderView::IsDeferredMode() const
{
  return myGraph->myData->myDeferredMode;
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::EdgeDef> BRepGraph::BuilderView::MutEdge(
  const BRepGraph_EdgeId theEdge)
{
  return BRepGraph_MutGuard<BRepGraphInc::EdgeDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeEdge(theEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, theEdge.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> BRepGraph::BuilderView::MutCoEdge(
  const BRepGraph_CoEdgeId theCoEdge)
{
  return BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCoEdge(theCoEdge),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CoEdge, theCoEdge.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexDef> BRepGraph::BuilderView::MutVertex(
  const BRepGraph_VertexId theVertex)
{
  return BRepGraph_MutGuard<BRepGraphInc::VertexDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeVertex(theVertex),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Vertex, theVertex.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireDef> BRepGraph::BuilderView::MutWire(
  const BRepGraph_WireId theWire)
{
  return BRepGraph_MutGuard<BRepGraphInc::WireDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeWire(theWire),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Wire, theWire.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceDef> BRepGraph::BuilderView::MutFace(
  const BRepGraph_FaceId theFace)
{
  return BRepGraph_MutGuard<BRepGraphInc::FaceDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeFace(theFace),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Face, theFace.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellDef> BRepGraph::BuilderView::MutShell(
  const BRepGraph_ShellId theShell)
{
  return BRepGraph_MutGuard<BRepGraphInc::ShellDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeShell(theShell),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Shell, theShell.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidDef> BRepGraph::BuilderView::MutSolid(
  const BRepGraph_SolidId theSolid)
{
  return BRepGraph_MutGuard<BRepGraphInc::SolidDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeSolid(theSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Solid, theSolid.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompoundDef> BRepGraph::BuilderView::MutCompound(
  const BRepGraph_CompoundId theCompound)
{
  return BRepGraph_MutGuard<BRepGraphInc::CompoundDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCompound(theCompound),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Compound, theCompound.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> BRepGraph::BuilderView::MutCompSolid(
  const BRepGraph_CompSolidId theCompSolid)
{
  return BRepGraph_MutGuard<BRepGraphInc::CompSolidDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCompSolid(theCompSolid),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::CompSolid, theCompSolid.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ProductDef> BRepGraph::BuilderView::MutProduct(
  const BRepGraph_ProductId theProduct)
{
  return BRepGraph_MutGuard<BRepGraphInc::ProductDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeProduct(theProduct),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Product, theProduct.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> BRepGraph::BuilderView::MutOccurrence(
  const BRepGraph_OccurrenceId theOccurrence)
{
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeOccurrence(theOccurrence),
    BRepGraph_NodeId(BRepGraph_NodeId::Kind::Occurrence, theOccurrence.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ShellRef> BRepGraph::BuilderView::MutShellRef(
  const BRepGraph_ShellRefId theShellRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::ShellRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeShellRef(theShellRef),
    BRepGraph_RefId::Shell(theShellRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::FaceRef> BRepGraph::BuilderView::MutFaceRef(
  const BRepGraph_FaceRefId theFaceRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::FaceRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeFaceRef(theFaceRef),
    BRepGraph_RefId::Face(theFaceRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::WireRef> BRepGraph::BuilderView::MutWireRef(
  const BRepGraph_WireRefId theWireRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::WireRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeWireRef(theWireRef),
    BRepGraph_RefId::Wire(theWireRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> BRepGraph::BuilderView::MutCoEdgeRef(
  const BRepGraph_CoEdgeRefId theCoEdgeRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCoEdgeRef(theCoEdgeRef),
    BRepGraph_RefId::CoEdge(theCoEdgeRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::VertexRef> BRepGraph::BuilderView::MutVertexRef(
  const BRepGraph_VertexRefId theVertexRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::VertexRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeVertexRef(theVertexRef),
    BRepGraph_RefId::Vertex(theVertexRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SolidRef> BRepGraph::BuilderView::MutSolidRef(
  const BRepGraph_SolidRefId theSolidRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::SolidRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeSolidRef(theSolidRef),
    BRepGraph_RefId::Solid(theSolidRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::ChildRef> BRepGraph::BuilderView::MutChildRef(
  const BRepGraph_ChildRefId theChildRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::ChildRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeChildRef(theChildRef),
    BRepGraph_RefId::Child(theChildRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> BRepGraph::BuilderView::MutOccurrenceRef(
  const BRepGraph_OccurrenceRefId theOccurrenceRef)
{
  return BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeOccurrenceRef(theOccurrenceRef),
    BRepGraph_RefId::Occurrence(theOccurrenceRef.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> BRepGraph::BuilderView::MutSurface(
  const BRepGraph_SurfaceRepId theSurface)
{
  return BRepGraph_MutGuard<BRepGraphInc::SurfaceRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeSurfaceRep(theSurface),
    BRepGraph_RepId::Surface(theSurface.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> BRepGraph::BuilderView::MutCurve3D(
  const BRepGraph_Curve3DRepId theCurve)
{
  return BRepGraph_MutGuard<BRepGraphInc::Curve3DRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCurve3DRep(theCurve),
    BRepGraph_RepId::Curve3D(theCurve.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> BRepGraph::BuilderView::MutCurve2D(
  const BRepGraph_Curve2DRepId theCurve)
{
  return BRepGraph_MutGuard<BRepGraphInc::Curve2DRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeCurve2DRep(theCurve),
    BRepGraph_RepId::Curve2D(theCurve.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::TriangulationRep> BRepGraph::BuilderView::MutTriangulation(
  const BRepGraph_TriangulationRepId theTriangulation)
{
  return BRepGraph_MutGuard<BRepGraphInc::TriangulationRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangeTriangulationRep(theTriangulation),
    BRepGraph_RepId::Triangulation(theTriangulation.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> BRepGraph::BuilderView::MutPolygon3D(
  const BRepGraph_Polygon3DRepId thePolygon)
{
  return BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangePolygon3DRep(thePolygon),
    BRepGraph_RepId::Polygon3D(thePolygon.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep> BRepGraph::BuilderView::MutPolygon2D(
  const BRepGraph_Polygon2DRepId thePolygon)
{
  return BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangePolygon2DRep(thePolygon),
    BRepGraph_RepId::Polygon2D(thePolygon.Index));
}

//=================================================================================================

BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep> BRepGraph::BuilderView::MutPolygonOnTri(
  const BRepGraph_PolygonOnTriRepId thePolygon)
{
  return BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>(
    myGraph,
    &myGraph->myData->myIncStorage.ChangePolygonOnTriRep(thePolygon),
    BRepGraph_RepId::PolygonOnTri(thePolygon.Index));
}

//=================================================================================================

void BRepGraph::BuilderView::applyModificationImpl(
  const BRepGraph_NodeId                 theTarget,
  NCollection_Vector<BRepGraph_NodeId>&& theReplacements,
  const TCollection_AsciiString&         theOpLabel)
{
  myGraph->myData->myHistoryLog.Record(theOpLabel, theTarget, theReplacements);
  myGraph->invalidateSubgraphImpl(theTarget);
}

//=================================================================================================

void BRepGraph::BuilderView::SplitEdge(const BRepGraph_NodeId theEdgeEntity,
                                       const BRepGraph_NodeId theSplitVertex,
                                       const double           theSplitParam,
                                       BRepGraph_NodeId&      theSubA,
                                       BRepGraph_NodeId&      theSubB)
{
  Standard_ASSERT_RETURN(theEdgeEntity.NodeKind == BRepGraph_NodeId::Kind::Edge,
                         "SplitEdge: theEdgeEntity must be an Edge node",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theEdgeEntity.Index >= 0
                           && theEdgeEntity.Index < myGraph->myData->myIncStorage.NbEdges(),
                         "SplitEdge: edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theSplitVertex.NodeKind == BRepGraph_NodeId::Kind::Vertex,
                         "SplitEdge: theSplitVertex must be a Vertex node",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theSplitVertex.Index >= 0
                           && theSplitVertex.Index < myGraph->myData->myIncStorage.NbVertices(),
                         "SplitEdge: split-vertex index is out of range",
                         Standard_VOID_RETURN);

  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraphInc::EdgeDef& anOrig =
    myGraph->myData->myIncStorage.Edge(BRepGraph_EdgeId::FromNodeId(theEdgeEntity));
  Standard_ASSERT_RETURN(!anOrig.IsRemoved, "SplitEdge: source edge is removed", Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(!anOrig.IsDegenerate,
                         "SplitEdge: degenerate edge cannot be split",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(anOrig.ParamFirst < theSplitParam && theSplitParam < anOrig.ParamLast,
                         "SplitEdge: split parameter must be inside open edge range",
                         Standard_VOID_RETURN);

  const BRepGraphInc_Storage&  aConstStorage         = myGraph->myData->myIncStorage;
  const BRepGraph_Curve3DRepId aOrigCurve3DRepId     = anOrig.Curve3DRepId;
  const double                 aOrigTolerance        = anOrig.Tolerance;
  const bool                   aOrigSameParameter    = anOrig.SameParameter;
  const double                 aOrigParamFirst       = anOrig.ParamFirst;
  const double                 aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_VertexRefId  aOrigStartVertexRefId = anOrig.StartVertexRefId;
  const BRepGraph_VertexRefId  aOrigEndVertexRefId   = anOrig.EndVertexRefId;
  const bool                   aOrigSameRange        = anOrig.SameRange;

  // Resolve original vertex def ids through storage ref entries.
  const BRepGraph_VertexId aOrigStartVertexDefId =
    aOrigStartVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigStartVertexRefId).VertexDefId
                                    : BRepGraph_VertexId();
  const BRepGraph_VertexId aOrigEndVertexDefId =
    aOrigEndVertexRefId.IsValid() ? aConstStorage.VertexRef(aOrigEndVertexRefId).VertexDefId
                                  : BRepGraph_VertexId();

  // Copy wire indices: ReverseIdx may be rebuilt below.
  const NCollection_Vector<BRepGraph_WireId>* aOrigWiresPtr =
    myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeEntity.Index));
  const NCollection_Vector<BRepGraph_WireId> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<BRepGraph_WireId>();

  // Allocate SubA slot.
  BRepGraphInc::EdgeDef& aSubADef = myGraph->myData->myIncStorage.AppendEdge();
  const int              aSubAIdx = myGraph->myData->myIncStorage.NbEdges() - 1;
  aSubADef.Id                     = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
  theSubA                         = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference - use index).
  BRepGraphInc::EdgeDef& aSubBDef = myGraph->myData->myIncStorage.AppendEdge();
  const int              aSubBIdx = myGraph->myData->myIncStorage.NbEdges() - 1;
  aSubBDef.Id                     = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
  theSubB                         = aSubBDef.Id;

  // Build vertex ref entries for the split vertex (no Location since split vertex is new).
  const BRepGraph_VertexId aSplitVertexDefId = theSplitVertex.IsValid()
                                                 ? BRepGraph_VertexId::FromNodeId(theSplitVertex)
                                                 : BRepGraph_VertexId();

  // Create start vertex ref entry for SubA (copy from original edge's start vertex ref).
  BRepGraph_VertexRefId aSubAStartRefId;
  if (aOrigStartVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRef& aOrigStartRef =
      myGraph->myData->myIncStorage.VertexRef(aOrigStartVertexRefId);
    const BRepGraph_VertexId aOrigStartVertexId = aOrigStartRef.VertexDefId;
    const TopAbs_Orientation aOrigStartOri      = aOrigStartRef.Orientation;
    const TopLoc_Location    aOrigStartLoc      = aOrigStartRef.LocalLocation;

    BRepGraphInc::VertexRef& aSubAStartRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                aSubAStartRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aSubAStartRef.RefId                       = BRepGraph_RefId::Vertex(aSubAStartRefIdx);
    aSubAStartRef.ParentId      = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
    aSubAStartRef.VertexDefId   = aOrigStartVertexId;
    aSubAStartRef.Orientation   = aOrigStartOri;
    aSubAStartRef.LocalLocation = aOrigStartLoc;
    myGraph->allocateRefUID(aSubAStartRef.RefId);
    aSubAStartRefId = BRepGraph_VertexRefId(aSubAStartRefIdx);
  }

  // Create end vertex ref entry for SubA (split vertex, REVERSED).
  BRepGraph_VertexRefId aSubAEndRefId;
  if (aSplitVertexDefId.IsValid())
  {
    BRepGraphInc::VertexRef& aSubAEndRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                aSubAEndRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aSubAEndRef.RefId                       = BRepGraph_RefId::Vertex(aSubAEndRefIdx);
    aSubAEndRef.ParentId    = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
    aSubAEndRef.VertexDefId = aSplitVertexDefId;
    aSubAEndRef.Orientation = TopAbs_REVERSED;
    myGraph->allocateRefUID(aSubAEndRef.RefId);
    aSubAEndRefId = BRepGraph_VertexRefId(aSubAEndRefIdx);
  }

  // Create start vertex ref entry for SubB (split vertex, FORWARD).
  BRepGraph_VertexRefId aSubBStartRefId;
  if (aSplitVertexDefId.IsValid())
  {
    BRepGraphInc::VertexRef& aSubBStartRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                aSubBStartRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aSubBStartRef.RefId                       = BRepGraph_RefId::Vertex(aSubBStartRefIdx);
    aSubBStartRef.ParentId    = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
    aSubBStartRef.VertexDefId = aSplitVertexDefId;
    aSubBStartRef.Orientation = TopAbs_FORWARD;
    myGraph->allocateRefUID(aSubBStartRef.RefId);
    aSubBStartRefId = BRepGraph_VertexRefId(aSubBStartRefIdx);
  }

  // Create end vertex ref entry for SubB (copy from original edge's end vertex ref).
  BRepGraph_VertexRefId aSubBEndRefId;
  if (aOrigEndVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRef& aOrigEndRef =
      myGraph->myData->myIncStorage.VertexRef(aOrigEndVertexRefId);
    const BRepGraph_VertexId aOrigEndVertexId = aOrigEndRef.VertexDefId;
    const TopAbs_Orientation aOrigEndOri      = aOrigEndRef.Orientation;
    const TopLoc_Location    aOrigEndLoc      = aOrigEndRef.LocalLocation;

    BRepGraphInc::VertexRef& aSubBEndRef    = myGraph->myData->myIncStorage.AppendVertexRef();
    const int                aSubBEndRefIdx = myGraph->myData->myIncStorage.NbVertexRefs() - 1;
    aSubBEndRef.RefId                       = BRepGraph_RefId::Vertex(aSubBEndRefIdx);
    aSubBEndRef.ParentId      = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
    aSubBEndRef.VertexDefId   = aOrigEndVertexId;
    aSubBEndRef.Orientation   = aOrigEndOri;
    aSubBEndRef.LocalLocation = aOrigEndLoc;
    myGraph->allocateRefUID(aSubBEndRef.RefId);
    aSubBEndRefId = BRepGraph_VertexRefId(aSubBEndRefIdx);
  }

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraphInc::EdgeDef& aSubA =
      myGraph->myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubAIdx));
    initSubEdgeEntity(aSubA,
                      aOrigCurve3DRepId,
                      aOrigTolerance,
                      aOrigSameParameter,
                      aSubAStartRefId,
                      aSubAEndRefId,
                      aOrigParamFirst,
                      theSplitParam);
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraphInc::EdgeDef& aSubB =
      myGraph->myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubBIdx));
    initSubEdgeEntity(aSubB,
                      aOrigCurve3DRepId,
                      aOrigTolerance,
                      aOrigSameParameter,
                      aSubBStartRefId,
                      aSubBEndRefId,
                      theSplitParam,
                      aOrigParamLast);
  }

  myGraph->allocateUID(theSubA);
  myGraph->allocateUID(theSubB);

  // Update incidence: wire CoEdgeRef rows and wire CoEdgeRefIds.
  {
    BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

    // Replace original edge's coedge with SubA+SubB coedges for each containing wire.
    const NCollection_Vector<BRepGraph_WireId>* aWireIndices =
      aStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aWireIndices != nullptr)
    {
      for (int aWIdx = 0; aWIdx < aWireIndices->Length(); ++aWIdx)
      {
        const int aWireIdx = aWireIndices->Value(aWIdx).Index;
        if (aWireIdx < 0 || aWireIdx >= aStorage.NbWires())
          continue;
        const NCollection_Vector<BRepGraph_CoEdgeRefId>& aWireRefIds =
          wireCoEdgeRefIds(aStorage, BRepGraph_WireId(aWireIdx));
        for (int aRefOrd = 0; aRefOrd < aWireRefIds.Length(); ++aRefOrd)
        {
          const BRepGraph_CoEdgeRefId    aRefId = aWireRefIds.Value(aRefOrd);
          const BRepGraphInc::CoEdgeRef& aRef   = aStorage.CoEdgeRef(aRefId);
          if (aRef.IsRemoved)
            continue;
          const int aOldCoEdgeIdx = aRef.CoEdgeDefId.Index;
          if (aOldCoEdgeIdx < 0 || aOldCoEdgeIdx >= aStorage.NbCoEdges())
            continue;

          BRepGraphInc::CoEdgeDef& aOldCoEdge =
            aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aOldCoEdgeIdx));
          if (aOldCoEdge.EdgeDefId == theEdgeEntity)
          {
            const TopAbs_Orientation aOrigOri = aOldCoEdge.Sense;
            const BRepGraph_FaceId   aFaceDef = aOldCoEdge.FaceDefId;
            const TopLoc_Location    aRefLoc  = aRef.LocalLocation;

            // Replace in-place: update existing coedge to point to SubA.
            aOldCoEdge.EdgeDefId = BRepGraph_EdgeId(aSubAIdx);

            // Create a new coedge for SubB and insert after SubA.
            BRepGraphInc::CoEdgeDef& aSubBCoEdge    = aStorage.AppendCoEdge();
            const int                aSubBCoEdgeIdx = aStorage.NbCoEdges() - 1;
            aSubBCoEdge.Id                          = BRepGraph_NodeId::CoEdge(aSubBCoEdgeIdx);
            aSubBCoEdge.EdgeDefId                   = BRepGraph_EdgeId(aSubBIdx);
            aSubBCoEdge.Sense                       = aOrigOri;
            aSubBCoEdge.FaceDefId                   = aFaceDef;
            myGraph->allocateUID(aSubBCoEdge.Id);

            // Append ref-entry row for the new coedge under this wire (append-only RefId policy).
            BRepGraphInc::CoEdgeRef& aSubBRef    = aStorage.AppendCoEdgeRef();
            const int                aSubBRefIdx = aStorage.NbCoEdgeRefs() - 1;
            aSubBRef.RefId                       = BRepGraph_RefId::CoEdge(aSubBRefIdx);
            aSubBRef.ParentId                    = BRepGraph_NodeId::Wire(aWireIdx);
            aSubBRef.CoEdgeDefId                 = BRepGraph_CoEdgeId(aSubBCoEdgeIdx);
            aSubBRef.LocalLocation               = aRefLoc;
            myGraph->allocateRefUID(aSubBRef.RefId);

            // Add new CoEdgeRefId to wire entity's RefId vector right after the
            // replaced slot to preserve coedge walk order.
            BRepGraphInc::WireDef& aWireEnt = aStorage.ChangeWire(BRepGraph_WireId(aWireIdx));
            if (aRefOrd >= 0 && aRefOrd < aWireEnt.CoEdgeRefIds.Length())
            {
              aWireEnt.CoEdgeRefIds.InsertAfter(aRefOrd, BRepGraph_CoEdgeRefId(aSubBRefIdx));
            }
            else
            {
              aWireEnt.CoEdgeRefIds.Append(BRepGraph_CoEdgeRefId(aSubBRefIdx));
            }

            // Maintain coedge->wire reverse index for incremental queries.
            aStorage.ChangeReverseIndex().BindCoEdgeToWire(BRepGraph_CoEdgeId(aSubBCoEdgeIdx),
                                                           BRepGraph_WireId(aWireIdx));
            break;
          }
        }
      }
    }

    // Mark original edge as removed in incidence.
    myGraph->myData->myIncStorage.MarkRemoved(theEdgeEntity);
  }

  // Split PCurve entries for each CoEdge referencing the original edge.
  // Copy CoEdge data before mutation (vector may reallocate).
  NCollection_Vector<BRepGraphInc::CoEdgeDef> aOrigCoEdges;
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      myGraph->myData->myIncStorage.ReverseIndex().CoEdgesOfEdge(
        BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aCoEdgeIdxs != nullptr)
    {
      for (int i = 0; i < aCoEdgeIdxs->Length(); ++i)
        aOrigCoEdges.Append(myGraph->myData->myIncStorage.CoEdge(aCoEdgeIdxs->Value(i)));
    }
  }

  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCE = aOrigCoEdges.Value(aCEIdx);

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
    BRepGraphInc::CoEdgeDef& aCoEdgeSubA    = myGraph->myData->myIncStorage.AppendCoEdge();
    const int                aCoEdgeSubAIdx = myGraph->myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubA.Id                          = BRepGraph_NodeId::CoEdge(aCoEdgeSubAIdx);
    initSubCoEdgeEntity(aCoEdgeSubA,
                        BRepGraph_EdgeId(aSubAIdx),
                        aCE.FaceDefId,
                        aCE.Sense,
                        aCE.Curve2DRepId,
                        aCE.ParamFirst,
                        aPCSplit,
                        aCE.Continuity);
    myGraph->allocateUID(aCoEdgeSubA.Id);

    // Create CoEdge for SubB.
    BRepGraphInc::CoEdgeDef& aCoEdgeSubB    = myGraph->myData->myIncStorage.AppendCoEdge();
    const int                aCoEdgeSubBIdx = myGraph->myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubB.Id                          = BRepGraph_NodeId::CoEdge(aCoEdgeSubBIdx);
    initSubCoEdgeEntity(aCoEdgeSubB,
                        BRepGraph_EdgeId(aSubBIdx),
                        aCE.FaceDefId,
                        aCE.Sense,
                        aCE.Curve2DRepId,
                        aPCSplit,
                        aCE.ParamLast,
                        aCE.Continuity);
    myGraph->allocateUID(aCoEdgeSubB.Id);
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurve3DRepId.IsValid())
  {
    const occ::handle<Geom_Curve>& aOrigCurve3d =
      myGraph->myData->myIncStorage.Curve3DRep(aOrigCurve3DRepId).Curve;
    if (!aOrigCurve3d.IsNull())
    {
      BRep_Builder aBB;

      const TopoDS_Shape aStartVShape = aOrigStartVertexDefId.IsValid()
                                          ? myGraph->Shapes().Shape(aOrigStartVertexDefId)
                                          : TopoDS_Shape();
      const TopoDS_Shape aSplitVShape = myGraph->Shapes().Shape(theSplitVertex);
      const TopoDS_Shape aEndVShape   = aOrigEndVertexDefId.IsValid()
                                          ? myGraph->Shapes().Shape(aOrigEndVertexDefId)
                                          : TopoDS_Shape();

      TopoDS_Edge aSubAEdge;
      aBB.MakeEdge(aSubAEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
      if (!aStartVShape.IsNull())
        aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
      myGraph->myData->myIncStorage.BindOriginal(theSubA, aSubAEdge);

      TopoDS_Edge aSubBEdge;
      aBB.MakeEdge(aSubBEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
      if (!aEndVShape.IsNull())
        aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
      myGraph->myData->myIncStorage.BindOriginal(theSubB, aSubBEdge);
    }
  }

  // Update edge-to-wire reverse index incrementally.
  BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ChangeReverseIndex();
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const BRepGraph_WireId aWireId = aOrigWires.Value(aWIdx);
    aRevIdx.UnbindEdgeFromWire(BRepGraph_EdgeId(theEdgeEntity.Index), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubAIdx), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubBIdx), aWireId);
    myGraph->markModified(aWireId);
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc_Storage&  aStorageRef = myGraph->myData->myIncStorage;
    const BRepGraphInc::EdgeDef& aSubAEnt    = aStorageRef.Edge(BRepGraph_EdgeId(aSubAIdx));
    const BRepGraphInc::EdgeDef& aSubBEnt    = aStorageRef.Edge(BRepGraph_EdgeId(aSubBIdx));
    BRepGraphInc_ReverseIndex&   aRevIdxMut  = myGraph->myData->myIncStorage.ChangeReverseIndex();

    // Resolve vertex def ids from the sub-edge ref entries.
    if (aSubAEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRef(aSubAEnt.StartVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubAIdx));
    }
    if (aSubAEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId = aStorageRef.VertexRef(aSubAEnt.EndVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubAIdx));
    }
    if (aSubBEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRef(aSubBEnt.StartVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubBIdx));
    }
    if (aSubBEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId = aStorageRef.VertexRef(aSubBEnt.EndVertexRefId).VertexDefId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubBIdx));
    }

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigStartVertexDefId, BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aOrigEndVertexDefId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigEndVertexDefId, BRepGraph_EdgeId(theEdgeEntity.Index));

    // Edge-to-face: derive from original edge's CoEdges (same faces apply to both sub-edges).
    for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aOrigCoEdges.Value(aCEIdx);
      if (aCE.FaceDefId.IsValid())
      {
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubAIdx), aCE.FaceDefId);
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubBIdx), aCE.FaceDefId);
      }
    }
  }

  myGraph->markModified(theEdgeEntity);
  myGraph->markModified(theSubA);
  myGraph->markModified(theSubB);

  Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                       "SplitEdge: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph::BuilderView::ReplaceEdgeInWire(const BRepGraph_WireId theWireDefId,
                                               const BRepGraph_EdgeId theOldEdgeEntity,
                                               const BRepGraph_EdgeId theNewEdgeEntity,
                                               const bool             theReversed)
{
  Standard_ASSERT_RETURN(
    theWireDefId.Index >= 0 && theWireDefId.Index < myGraph->myData->myIncStorage.NbWires(),
    "ReplaceEdgeInWire: wire index is out of range",
    Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theOldEdgeEntity.Index >= 0
                           && theOldEdgeEntity.Index < myGraph->myData->myIncStorage.NbEdges(),
                         "ReplaceEdgeInWire: old edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(theNewEdgeEntity.Index >= 0
                           && theNewEdgeEntity.Index < myGraph->myData->myIncStorage.NbEdges(),
                         "ReplaceEdgeInWire: new edge index is out of range",
                         Standard_VOID_RETURN);
  Standard_ASSERT_RETURN(
    !myGraph->myData->myIncStorage.Edge(theNewEdgeEntity).IsRemoved,
    "ReplaceEdgeInWire: replacement edge must be active",
    Standard_VOID_RETURN);

  BRepGraphInc_Storage&                            aStorage = myGraph->myData->myIncStorage;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aWireRefIds =
    wireCoEdgeRefIds(aStorage, theWireDefId);

  // Update incidence by scanning wire-owned coedge ref entries.
  for (int aRefIdx = 0; aRefIdx < aWireRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRef& aRef = aStorage.CoEdgeRef(aWireRefIds.Value(aRefIdx));
    if (aRef.IsRemoved)
      continue;
    const int aCoEdgeEntIdx = aRef.CoEdgeDefId.Index;
    if (aCoEdgeEntIdx < 0 || aCoEdgeEntIdx >= aStorage.NbCoEdges())
      continue;

    BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aCoEdgeEntIdx));
    if (aCoEdge.EdgeDefId == theOldEdgeEntity)
    {
      aCoEdge.EdgeDefId = theNewEdgeEntity;
      if (theReversed)
        aCoEdge.Sense = TopAbs::Reverse(aCoEdge.Sense);

      // Update reverse indices incrementally.
      BRepGraphInc_ReverseIndex& aRevIdx = myGraph->myData->myIncStorage.ChangeReverseIndex();
      aRevIdx.ReplaceEdgeInWireMap(theOldEdgeEntity, theNewEdgeEntity, theWireDefId);
      aRevIdx.UnbindEdgeFromCoEdge(theOldEdgeEntity, BRepGraph_CoEdgeId(aCoEdgeEntIdx));
      aRevIdx.BindEdgeToCoEdge(theNewEdgeEntity, BRepGraph_CoEdgeId(aCoEdgeEntIdx));

      // Update edge-to-face: bind new edge, unbind old edge for all faces of this wire.
      // Wire-to-face mappings are built from FaceDef.WireRefs during Build() and are
      // stable across edge mutations - only face-level operations modify them.
      const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(theWireDefId);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_FaceId aFaceId = aFaces->Value(aFIdx);
          aRevIdx.BindEdgeToFace(theNewEdgeEntity, aFaceId);
          aRevIdx.UnbindEdgeFromFace(theOldEdgeEntity, aFaceId);
        }
      }
    }
  }

  myGraph->markModified(BRepGraph_NodeId::Wire(theWireDefId.Index));

  // Validate reverse index only when not in deferred mode.
  // In deferred mode (batch sewing, parallel mutations), intermediate states
  // may have temporarily stale entries; validation runs at CommitMutation().
  if (!myGraph->myData->myDeferredMode)
  {
    Standard_ASSERT_VOID(myGraph->myData->myIncStorage.ValidateReverseIndex(),
                         "ReplaceEdgeInWire: post-mutation reverse index inconsistency");
  }
}

//=================================================================================================

void BRepGraph::BuilderView::CommitMutation() noexcept
{
  const bool isValid = ValidateMutationBoundary();
  Standard_ASSERT_VOID(isValid, "CommitMutation: mutation boundary consistency check failed");
  (void)isValid;
}

//=================================================================================================

bool BRepGraph::BuilderView::ValidateMutationBoundary(
  NCollection_Vector<BoundaryIssue>* const theIssues) const
{
  bool                        isValid  = true;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;

  if (!aStorage.ValidateReverseIndex())
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "Mutation boundary reverse index inconsistency";
      theIssues->Append(anIssue);
    }
  }

  constexpr BRepGraph_NodeId::Kind THE_KINDS[] = {BRepGraph_NodeId::Kind::Vertex,
                                                  BRepGraph_NodeId::Kind::Edge,
                                                  BRepGraph_NodeId::Kind::CoEdge,
                                                  BRepGraph_NodeId::Kind::Wire,
                                                  BRepGraph_NodeId::Kind::Face,
                                                  BRepGraph_NodeId::Kind::Shell,
                                                  BRepGraph_NodeId::Kind::Solid,
                                                  BRepGraph_NodeId::Kind::Compound,
                                                  BRepGraph_NodeId::Kind::CompSolid,
                                                  BRepGraph_NodeId::Kind::Product,
                                                  BRepGraph_NodeId::Kind::Occurrence};
  for (int aKindIdx = 0; aKindIdx < static_cast<int>(sizeof(THE_KINDS) / sizeof(THE_KINDS[0]));
       ++aKindIdx)
  {
    const BRepGraph_NodeId::Kind aKind       = THE_KINDS[aKindIdx];
    const int                    aCachedCnt  = cachedActiveByKind(aStorage, aKind);
    const int                    anActualCnt = countActiveByKind(aStorage, aKind);
    if (aCachedCnt == anActualCnt)
      continue;

    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue           anIssue;
      TCollection_AsciiString aDesc("Mutation boundary active count mismatch for ");
      aDesc += kindName(aKind);
      aDesc += ": cached=";
      aDesc += TCollection_AsciiString(aCachedCnt);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(anActualCnt);
      anIssue.NodeId      = BRepGraph_NodeId(aKind, -1);
      anIssue.Description = aDesc;
      theIssues->Append(anIssue);
    }
  }

  // Validate built-in layer consistency: layers must not have bindings
  // for entity kinds that have zero active entities in the graph.
  if (myGraph->ParamLayer().HasBindings() && aStorage.NbVertices() == 0)
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "ParamLayer has bindings but graph has no vertices";
      theIssues->Append(anIssue);
    }
  }

  if (myGraph->RegularityLayer().HasBindings() && aStorage.NbEdges() == 0)
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "RegularityLayer has bindings but graph has no edges";
      theIssues->Append(anIssue);
    }
  }

  return isValid;
}
