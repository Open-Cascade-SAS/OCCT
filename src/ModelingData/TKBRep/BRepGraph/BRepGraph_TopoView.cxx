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

#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_PackedMap.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>

namespace
{

constexpr int THE_TOPOVIEW_FACE_ADJACENCY_BLOCK_SIZE = 8;
constexpr int THE_TOPOVIEW_FACE_EDGE_BLOCK_SIZE      = 8;
constexpr int THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE = 4;
constexpr int THE_TOPOVIEW_EDGE_ADJACENCY_BLOCK_SIZE = 8;
constexpr int THE_TOPOVIEW_VERTEX_FACE_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE    = 4;

//=================================================================================================

const occ::handle<NCollection_BaseAllocator>& effectiveAllocator(
  const BRepGraph&                               theGraph,
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  return theAllocator.IsNull() ? theGraph.Allocator() : theAllocator;
}

}

// ==========================================================================
// Count methods read directly from incidence storage.
// Both Build() and BuilderView::Add*() write to incidence, so counts are
// always in sync.
// ==========================================================================

//=================================================================================================

int BRepGraph::TopoView::NbSolids() const
{
  return myGraph->myData->myIncStorage.NbSolids();
}

//=================================================================================================

int BRepGraph::TopoView::NbShells() const
{
  return myGraph->myData->myIncStorage.NbShells();
}

//=================================================================================================

int BRepGraph::TopoView::NbFaces() const
{
  return myGraph->myData->myIncStorage.NbFaces();
}

//=================================================================================================

int BRepGraph::TopoView::NbWires() const
{
  return myGraph->myData->myIncStorage.NbWires();
}

//=================================================================================================

int BRepGraph::TopoView::NbEdges() const
{
  return myGraph->myData->myIncStorage.NbEdges();
}

//=================================================================================================

int BRepGraph::TopoView::NbVertices() const
{
  return myGraph->myData->myIncStorage.NbVertices();
}

//=================================================================================================

int BRepGraph::TopoView::NbCompounds() const
{
  return myGraph->myData->myIncStorage.NbCompounds();
}

//=================================================================================================

int BRepGraph::TopoView::NbCompSolids() const
{
  return myGraph->myData->myIncStorage.NbCompSolids();
}

//=================================================================================================

int BRepGraph::TopoView::NbCoEdges() const
{
  return myGraph->myData->myIncStorage.NbCoEdges();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveVertices() const
{
  return myGraph->myData->myIncStorage.NbActiveVertices();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveEdges() const
{
  return myGraph->myData->myIncStorage.NbActiveEdges();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveCoEdges() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdges();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveWires() const
{
  return myGraph->myData->myIncStorage.NbActiveWires();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveFaces() const
{
  return myGraph->myData->myIncStorage.NbActiveFaces();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveShells() const
{
  return myGraph->myData->myIncStorage.NbActiveShells();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveSolids() const
{
  return myGraph->myData->myIncStorage.NbActiveSolids();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveCompounds() const
{
  return myGraph->myData->myIncStorage.NbActiveCompounds();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveCompSolids() const
{
  return myGraph->incStorage().NbActiveCompSolids();
}

//=================================================================================================

NCollection_Vector<BRepGraph_VertexId> BRepGraph::TopoView::ActiveVertexIds() const
{
  NCollection_Vector<BRepGraph_VertexId> aResult;
  const BRepGraphInc_Storage&            aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbVertices(); ++anIdx)
  {
    const BRepGraph_VertexId aId(anIdx);
    if (!aStorage.Vertex(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::ActiveEdgeIds() const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbEdges(); ++anIdx)
  {
    const BRepGraph_EdgeId aId(anIdx);
    if (!aStorage.Edge(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_CoEdgeId> BRepGraph::TopoView::ActiveCoEdgeIds() const
{
  NCollection_Vector<BRepGraph_CoEdgeId> aResult;
  const BRepGraphInc_Storage&            aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbCoEdges(); ++anIdx)
  {
    const BRepGraph_CoEdgeId aId(anIdx);
    if (!aStorage.CoEdge(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_WireId> BRepGraph::TopoView::ActiveWireIds() const
{
  NCollection_Vector<BRepGraph_WireId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbWires(); ++anIdx)
  {
    const BRepGraph_WireId aId(anIdx);
    if (!aStorage.Wire(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::ActiveFaceIds() const
{
  NCollection_Vector<BRepGraph_FaceId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbFaces(); ++anIdx)
  {
    const BRepGraph_FaceId aId(anIdx);
    if (!aStorage.Face(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_ShellId> BRepGraph::TopoView::ActiveShellIds() const
{
  NCollection_Vector<BRepGraph_ShellId> aResult;
  const BRepGraphInc_Storage&           aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbShells(); ++anIdx)
  {
    const BRepGraph_ShellId aId(anIdx);
    if (!aStorage.Shell(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_SolidId> BRepGraph::TopoView::ActiveSolidIds() const
{
  NCollection_Vector<BRepGraph_SolidId> aResult;
  const BRepGraphInc_Storage&           aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbSolids(); ++anIdx)
  {
    const BRepGraph_SolidId aId(anIdx);
    if (!aStorage.Solid(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_CompoundId> BRepGraph::TopoView::ActiveCompoundIds() const
{
  NCollection_Vector<BRepGraph_CompoundId> aResult;
  const BRepGraphInc_Storage&              aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbCompounds(); ++anIdx)
  {
    const BRepGraph_CompoundId aId(anIdx);
    if (!aStorage.Compound(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_CompSolidId> BRepGraph::TopoView::ActiveCompSolidIds() const
{
  NCollection_Vector<BRepGraph_CompSolidId> aResult;
  const BRepGraphInc_Storage&               aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbCompSolids(); ++anIdx)
  {
    const BRepGraph_CompSolidId aId(anIdx);
    if (!aStorage.CompSolid(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

int BRepGraph::TopoView::NbFacesOfEdge(const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().NbFacesOfEdge(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_WireId>& BRepGraph::TopoView::WiresOfEdge(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdgeRef(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CoEdgeId>& BRepGraph::TopoView::CoEdgesOfEdge(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_FaceId>& BRepGraph::TopoView::FacesOfEdge(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdgeRef(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_EdgeId>& BRepGraph::TopoView::EdgesOfVertex(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().EdgesOfVertexRef(theVertex);
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::ChildEntitiesOfCompound(
  const BRepGraph_CompoundId theCompound) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theCompound.IsValid(aStorage.NbCompounds()))
    return aResult;

  NCollection_PackedMap<int> aSeenByKind[BRepGraph_NodeId::THE_KIND_COUNT];
  const BRepGraphInc::CompoundDef& aDef = aStorage.Compound(theCompound);
  for (int i = 0; i < aDef.ChildRefIds.Length(); ++i)
  {
    const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(aDef.ChildRefIds.Value(i));
    if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
      continue;

    const BRepGraphInc::BaseDef* aChild = myGraph->topoEntity(aRef.ChildDefId);
    if (aChild == nullptr || aChild->IsRemoved)
      continue;

    const int aKindIdx = static_cast<int>(aRef.ChildDefId.NodeKind);
    if (aKindIdx < 0 || aKindIdx >= BRepGraph_NodeId::THE_KIND_COUNT)
      continue;

    if (aSeenByKind[aKindIdx].Add(aRef.ChildDefId.Index))
      aResult.Append(aRef.ChildDefId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_SolidId> BRepGraph::TopoView::SolidsOfCompSolid(
  const BRepGraph_CompSolidId theCompSolid) const
{
  NCollection_Vector<BRepGraph_SolidId> aResult;
  const BRepGraphInc_Storage&           aStorage = myGraph->myData->myIncStorage;
  if (!theCompSolid.IsValid(aStorage.NbCompSolids()))
    return aResult;

  NCollection_PackedMap<int> aSeen;
  const BRepGraphInc::CompSolidDef& aDef = aStorage.CompSolid(theCompSolid);
  const int aNbSolids = aStorage.NbSolids();
  for (int i = 0; i < aDef.SolidRefIds.Length(); ++i)
  {
    const BRepGraphInc::SolidRef& aRef = aStorage.SolidRef(aDef.SolidRefIds.Value(i));
    if (aRef.IsRemoved || !aRef.SolidDefId.IsValid(aNbSolids))
      continue;

    if (aStorage.Solid(aRef.SolidDefId).IsRemoved)
      continue;

    if (aSeen.Add(aRef.SolidDefId.Index))
      aResult.Append(aRef.SolidDefId);
  }
  return aResult;
}

//=================================================================================================

const BRepGraphInc::SolidDef& BRepGraph::TopoView::Solid(const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.Solid(theSolid);
}

//=================================================================================================

const BRepGraphInc::ShellDef& BRepGraph::TopoView::Shell(const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.Shell(theShell);
}

//=================================================================================================

const BRepGraphInc::FaceDef& BRepGraph::TopoView::Face(const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.Face(theFace);
}

//=================================================================================================

const BRepGraphInc::WireDef& BRepGraph::TopoView::Wire(const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.Wire(theWire);
}

//=================================================================================================

const BRepGraphInc::EdgeDef& BRepGraph::TopoView::Edge(const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.Edge(theEdge);
}

//=================================================================================================

const BRepGraphInc::VertexDef& BRepGraph::TopoView::Vertex(const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.Vertex(theVertex);
}

//=================================================================================================

const BRepGraphInc::CompoundDef& BRepGraph::TopoView::Compound(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.Compound(theCompound);
}

//=================================================================================================

const BRepGraphInc::CompSolidDef& BRepGraph::TopoView::CompSolid(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid);
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef& BRepGraph::TopoView::CoEdge(const BRepGraph_CoEdgeId theCoEdge) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdge);
}

//=================================================================================================

BRepGraph_SurfaceRepId BRepGraph::TopoView::SurfaceRepIdOfFace(const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return BRepGraph_SurfaceRepId();
  const BRepGraph_SurfaceRepId aRepId = aStorage.Face(theFace).SurfaceRepId;
  if (!aRepId.IsValid(aStorage.NbSurfaces()) || aStorage.SurfaceRep(aRepId).IsRemoved)
    return BRepGraph_SurfaceRepId();
  return aRepId;
}

//=================================================================================================

BRepGraph_TriangulationRepId BRepGraph::TopoView::ActiveTriangulationRepIdOfFace(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return BRepGraph_TriangulationRepId();
  const BRepGraph_TriangulationRepId aRepId = aStorage.Face(theFace).ActiveTriangulationRepId();
  if (!aRepId.IsValid(aStorage.NbTriangulations()) || aStorage.TriangulationRep(aRepId).IsRemoved)
    return BRepGraph_TriangulationRepId();
  return aRepId;
}

//=================================================================================================

BRepGraph_Curve3DRepId BRepGraph::TopoView::Curve3DRepIdOfEdge(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
    return BRepGraph_Curve3DRepId();
  const BRepGraph_Curve3DRepId aRepId = aStorage.Edge(theEdge).Curve3DRepId;
  if (!aRepId.IsValid(aStorage.NbCurves3D()) || aStorage.Curve3DRep(aRepId).IsRemoved)
    return BRepGraph_Curve3DRepId();
  return aRepId;
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::TopoView::Curve2DRepIdOfCoEdge(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
    return BRepGraph_Curve2DRepId();
  const BRepGraph_Curve2DRepId aRepId = aStorage.CoEdge(theCoEdge).Curve2DRepId;
  if (!aRepId.IsValid(aStorage.NbCurves2D()) || aStorage.Curve2DRep(aRepId).IsRemoved)
    return BRepGraph_Curve2DRepId();
  return aRepId;
}

//=================================================================================================

const BRepGraphInc::BaseDef* BRepGraph::TopoView::TopoEntity(const BRepGraph_NodeId theId) const
{
  return myGraph->topoEntity(theId);
}

//=================================================================================================

int BRepGraph::TopoView::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return aS.NbSolids() + aS.NbShells() + aS.NbFaces() + aS.NbWires() + aS.NbCoEdges()
         + aS.NbEdges() + aS.NbVertices() + aS.NbCompounds() + aS.NbCompSolids()
         + aS.NbProducts() + aS.NbOccurrences();
}

//=================================================================================================

int BRepGraph::TopoView::NbSurfaces() const
{
  return myGraph->myData->myIncStorage.NbSurfaces();
}

//=================================================================================================

int BRepGraph::TopoView::NbCurves3D() const
{
  return myGraph->myData->myIncStorage.NbCurves3D();
}

//=================================================================================================

int BRepGraph::TopoView::NbCurves2D() const
{
  return myGraph->myData->myIncStorage.NbCurves2D();
}

//=================================================================================================

int BRepGraph::TopoView::NbTriangulations() const
{
  return myGraph->myData->myIncStorage.NbTriangulations();
}

//=================================================================================================

int BRepGraph::TopoView::NbPolygons3D() const
{
  return myGraph->myData->myIncStorage.NbPolygons3D();
}

//=================================================================================================

const BRepGraphInc::SurfaceRep& BRepGraph::TopoView::SurfaceRep(
  const BRepGraph_SurfaceRepId theRep) const
{
  return myGraph->myData->myIncStorage.SurfaceRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Curve3DRep& BRepGraph::TopoView::Curve3DRep(
  const BRepGraph_Curve3DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Curve3DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Curve2DRep& BRepGraph::TopoView::Curve2DRep(
  const BRepGraph_Curve2DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Curve2DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::TriangulationRep& BRepGraph::TopoView::TriangulationRep(
  const BRepGraph_TriangulationRepId theRep) const
{
  return myGraph->myData->myIncStorage.TriangulationRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Polygon3DRep& BRepGraph::TopoView::Polygon3DRep(
  const BRepGraph_Polygon3DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon3DRep(theRep);
}

//=================================================================================================

int BRepGraph::TopoView::NbPolygons2D() const
{
  return myGraph->myData->myIncStorage.NbPolygons2D();
}

//=================================================================================================

int BRepGraph::TopoView::NbPolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbPolygonsOnTri();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveSurfaces() const
{
  return myGraph->myData->myIncStorage.NbActiveSurfaces();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveCurves3D() const
{
  return myGraph->myData->myIncStorage.NbActiveCurves3D();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveCurves2D() const
{
  return myGraph->myData->myIncStorage.NbActiveCurves2D();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveTriangulations() const
{
  return myGraph->myData->myIncStorage.NbActiveTriangulations();
}

//=================================================================================================

int BRepGraph::TopoView::NbActivePolygons3D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons3D();
}

//=================================================================================================

int BRepGraph::TopoView::NbActivePolygons2D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons2D();
}

//=================================================================================================

int BRepGraph::TopoView::NbActivePolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbActivePolygonsOnTri();
}

//=================================================================================================

const BRepGraphInc::Polygon2DRep& BRepGraph::TopoView::Polygon2DRep(
  const BRepGraph_Polygon2DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon2DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::PolygonOnTriRep& BRepGraph::TopoView::PolygonOnTriRep(
  const BRepGraph_PolygonOnTriRepId theRep) const
{
  return myGraph->myData->myIncStorage.PolygonOnTriRep(theRep);
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph::TopoView::FindPCurve(
  const BRepGraph_NodeId theEdgeEntity,
  const BRepGraph_NodeId theFaceEntity) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return nullptr;

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(BRepGraph_EdgeId(theEdgeEntity.Index));
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeDefId == theEdgeEntity && aCoEdge.FaceDefId == theFaceEntity)
      return &aCoEdge;
  }
  return nullptr;
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph::TopoView::FindPCurve(
  const BRepGraph_NodeId   theEdgeEntity,
  const BRepGraph_NodeId   theFaceEntity,
  const TopAbs_Orientation theEdgeOrientation) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return nullptr;

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(BRepGraph_EdgeId(theEdgeEntity.Index));
  // For non-seam edges (1 CoEdge per face), return the only match.
  // For seam edges (2 CoEdges per face), prefer exact orientation match.
  const BRepGraphInc::CoEdgeDef* aFirstMatch = nullptr;
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeDefId != theEdgeEntity || aCoEdge.FaceDefId != theFaceEntity)
      continue;
    if (aFirstMatch == nullptr)
      aFirstMatch = &aCoEdge;
    if (aCoEdge.Sense == theEdgeOrientation)
      return &aCoEdge;
  }
  return aFirstMatch;
}

//=================================================================================================

bool BRepGraph::TopoView::IsRemoved(const BRepGraph_NodeId theNode) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

// ==========================================================================
// Topology adjacency queries
// ==========================================================================

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::SameDomainFaces(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  // Collect faces sharing the same surface representation.
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraphInc::FaceDef& aFaceDef = aStorage.Face(theFace);
  if (!aFaceDef.SurfaceRepId.IsValid())
    return aResult;

  for (int aFaceIdx = 0; aFaceIdx < aStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aOtherFace = aStorage.Face(BRepGraph_FaceId(aFaceIdx));
    if (aOtherFace.IsRemoved)
      continue;
    if (aOtherFace.SurfaceRepId == aFaceDef.SurfaceRepId && aFaceIdx != theFace.Index)
      aResult.Append(BRepGraph_FaceId(aFaceIdx));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::SharedEdges(
  const BRepGraph_FaceId                          theFaceA,
  const BRepGraph_FaceId                          theFaceB,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  // Intersect edge sets of both faces to find common edges.
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFaceA.IsValid(aStorage.NbFaces()) || !theFaceB.IsValid(aStorage.NbFaces()))
    return aResult;

  const NCollection_Vector<BRepGraph_EdgeId> aFaceAEdges = EdgesOfFace(theFaceA, myGraph->Allocator());
  const NCollection_Vector<BRepGraph_EdgeId> aFaceBEdges = EdgesOfFace(theFaceB, myGraph->Allocator());
  NCollection_PackedMap<int>                 aFaceAEdgeSet;
  for (int aEdgeIdx = 0; aEdgeIdx < aFaceAEdges.Length(); ++aEdgeIdx)
  {
    aFaceAEdgeSet.Add(aFaceAEdges.Value(aEdgeIdx).Index);
  }

  NCollection_PackedMap<int> aAdded;
  for (int aEdgeIdx = 0; aEdgeIdx < aFaceBEdges.Length(); ++aEdgeIdx)
  {
    const BRepGraph_EdgeId anEdgeId = aFaceBEdges.Value(aEdgeIdx);
    if (aFaceAEdgeSet.Contains(anEdgeId.Index) && aAdded.Add(anEdgeId.Index))
      aResult.Append(anEdgeId);
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::AdjacentFaces(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_FACE_ADJACENCY_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  NCollection_PackedMap<int>           aFaceSet;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return aResult;

  const NCollection_Vector<BRepGraph_EdgeId> anEdgeNodes = EdgesOfFace(theFace, myGraph->Allocator());
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  for (int anEdgeNodeIdx = 0; anEdgeNodeIdx < anEdgeNodes.Length(); ++anEdgeNodeIdx)
  {
    const BRepGraph_EdgeId                      anEdgeDefId = anEdgeNodes.Value(anEdgeNodeIdx);
    const NCollection_Vector<BRepGraph_FaceId>* aFaces      = aRevIdx.FacesOfEdge(anEdgeDefId);
    if (aFaces != nullptr)
    {
      for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      {
        const BRepGraph_FaceId anAdjFaceId = aFaces->Value(aFIdx);
        if (anAdjFaceId.Index == theFace.Index)
          continue;
        const BRepGraphInc::FaceDef& anAdjFace = aStorage.Face(anAdjFaceId);
        if (anAdjFace.IsRemoved)
          continue;
        if (aFaceSet.Add(anAdjFaceId.Index))
          aResult.Append(anAdjFaceId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::EdgesOfFace(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_FACE_EDGE_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  NCollection_PackedMap<int>   anEdgeSet;

  for (int w = 0; w < aFace.WireRefIds.Length(); ++w)
  {
    const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aFace.WireRefIds.Value(w));
    if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid())
      continue;
    if (!aWireRef.WireDefId.IsValid(aStorage.NbWires()))
      continue;

    const BRepGraphInc::WireDef& aWire = aStorage.Wire(aWireRef.WireDefId);
    for (int ce = 0; ce < aWire.CoEdgeRefIds.Length(); ++ce)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = aStorage.CoEdgeRef(aWire.CoEdgeRefIds.Value(ce));
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid())
        continue;
      if (!aCoEdgeRef.CoEdgeDefId.IsValid(aStorage.NbCoEdges()))
        continue;

      const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
      if (!aCoEdge.EdgeDefId.IsValid(aStorage.NbEdges()) || aCoEdge.IsRemoved)
        continue;
      if (anEdgeSet.Add(aCoEdge.EdgeDefId.Index))
        aResult.Append(aCoEdge.EdgeDefId);
    }
  }
  return aResult;
}

//=================================================================================================

BRepGraph_WireId BRepGraph::TopoView::OuterWireOfFace(const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
    return BRepGraph_WireId();

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  for (int aRefIdx = 0; aRefIdx < aFace.WireRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aFace.WireRefIds.Value(aRefIdx));
    if (!aWireRef.IsRemoved && aWireRef.IsOuter)
      return aWireRef.WireDefId;
  }
  return BRepGraph_WireId();
}

//=================================================================================================

NCollection_Vector<BRepGraph_VertexId> BRepGraph::TopoView::VerticesOfEdge(
  const BRepGraph_EdgeId                          theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_VertexId> aResult(THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE,
                                                 theAllocator.IsNull() ? myGraph->Allocator()
                                                                       : theAllocator);
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraphInc::EdgeDef& aDef          = aStorage.Edge(theEdge);
  const int                    aNbVertexRefs = aStorage.NbVertexRefs();
  NCollection_PackedMap<int>   aSeenRefIds;

  const auto aProcessRefId = [&aResult, &aSeenRefIds, &aStorage, aNbVertexRefs](
                               const BRepGraph_VertexRefId theRefId) {
    if (!theRefId.IsValid(aNbVertexRefs))
      return;
    if (!aSeenRefIds.Add(theRefId.Index))
      return;

    const BRepGraphInc::VertexRef& aVertexRef = aStorage.VertexRef(theRefId);
    if (aVertexRef.IsRemoved)
      return;
    aResult.Append(aVertexRef.VertexDefId);
  };

  aProcessRefId(aDef.StartVertexRefId);
  aProcessRefId(aDef.EndVertexRefId);
  for (int i = 0; i < aDef.InternalVertexRefIds.Length(); ++i)
  {
    aProcessRefId(aDef.InternalVertexRefIds.Value(i));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::AdjacentEdges(
  const BRepGraph_EdgeId                          theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_EDGE_ADJACENCY_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_PackedMap<int>       anEdgeSet;

  // Collect all vertices of this edge, then all edges of those vertices.
  const NCollection_Vector<BRepGraph_VertexId> aVertices = VerticesOfEdge(theEdge, myGraph->Allocator());

  for (int v = 0; v < aVertices.Length(); ++v)
  {
    const NCollection_Vector<BRepGraph_EdgeId>* aEdges = aRevIdx.EdgesOfVertex(aVertices.Value(v));
    if (aEdges == nullptr)
      continue;
    for (int e = 0; e < aEdges->Length(); ++e)
    {
      const BRepGraph_EdgeId anAdjEdgeId = aEdges->Value(e);
      if (anAdjEdgeId.Index == theEdge.Index)
        continue;
      const BRepGraphInc::EdgeDef& anAdjEdge = aStorage.Edge(anAdjEdgeId);
      if (anAdjEdge.IsRemoved)
        continue;
      if (anEdgeSet.Add(anAdjEdgeId.Index))
        aResult.Append(anAdjEdgeId);
    }
  }
  return aResult;
}

//=================================================================================================

bool BRepGraph::TopoView::IsBoundaryEdge(const BRepGraph_EdgeId theEdge) const
{
  return NbFacesOfEdge(theEdge) == 1;
}

//=================================================================================================

bool BRepGraph::TopoView::IsManifoldEdge(const BRepGraph_EdgeId theEdge) const
{
  return NbFacesOfEdge(theEdge) == 2;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::FacesOfVertex(
  const BRepGraph_VertexId                        theVertex,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_VERTEX_FACE_BLOCK_SIZE,
                                               effectiveAllocator(*myGraph, theAllocator));
  const NCollection_Vector<BRepGraph_EdgeId>& anEdges = EdgesOfVertex(theVertex);
  NCollection_Map<int>                        aSeenFaces;
  for (int anEdgeIdx = 0; anEdgeIdx < anEdges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_FaceId>& aFaces = FacesOfEdge(anEdges.Value(anEdgeIdx));
    for (int aFaceIdx = 0; aFaceIdx < aFaces.Length(); ++aFaceIdx)
    {
      if (aSeenFaces.Add(aFaces.Value(aFaceIdx).Index))
      {
        aResult.Append(aFaces.Value(aFaceIdx));
      }
    }
  }
  return aResult;
}

// ==========================================================================
// Assembly definition accessors
// ==========================================================================

//=================================================================================================

int BRepGraph::TopoView::NbProducts() const
{
  return myGraph->myData->myIncStorage.NbProducts();
}

//=================================================================================================

int BRepGraph::TopoView::NbOccurrences() const
{
  return myGraph->myData->myIncStorage.NbOccurrences();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveProducts() const
{
  return myGraph->myData->myIncStorage.NbActiveProducts();
}

//=================================================================================================

int BRepGraph::TopoView::NbActiveOccurrences() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrences();
}

//=================================================================================================

NCollection_Vector<BRepGraph_ProductId> BRepGraph::TopoView::ActiveProductIds() const
{
  NCollection_Vector<BRepGraph_ProductId> aResult;
  const BRepGraphInc_Storage&             aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbProducts(); ++anIdx)
  {
    const BRepGraph_ProductId aId(anIdx);
    if (!aStorage.Product(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_OccurrenceId> BRepGraph::TopoView::ActiveOccurrenceIds() const
{
  NCollection_Vector<BRepGraph_OccurrenceId> aResult;
  const BRepGraphInc_Storage&                aStorage = myGraph->myData->myIncStorage;
  for (int anIdx = 0; anIdx < aStorage.NbOccurrences(); ++anIdx)
  {
    const BRepGraph_OccurrenceId aId(anIdx);
    if (!aStorage.Occurrence(aId).IsRemoved)
      aResult.Append(aId);
  }
  return aResult;
}

//=================================================================================================

const BRepGraphInc::ProductDef& BRepGraph::TopoView::Product(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.Product(theProduct);
}

//=================================================================================================

const BRepGraphInc::OccurrenceDef& BRepGraph::TopoView::Occurrence(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  return myGraph->myData->myIncStorage.Occurrence(theOccurrence);
}
