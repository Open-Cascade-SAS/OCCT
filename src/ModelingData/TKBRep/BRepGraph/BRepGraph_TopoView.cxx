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
  return myGraph->myData->myIncStorage.NbActiveCompSolids();
}

//=================================================================================================

int BRepGraph::TopoView::FaceCountOfEdge(const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().FaceCountOfEdge(theEdge);
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

int BRepGraph::TopoView::NbShellFaces(const BRepGraph_ShellId theShell) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theShell.IsValid(aStorage.NbShells()))
    return 0;

  const BRepGraphInc::ShellDef& aShell   = aStorage.Shell(theShell);
  int                           aNbFaces = 0;
  for (int i = 0; i < aShell.FaceRefIds.Length(); ++i)
  {
    if (!aStorage.FaceRef(aShell.FaceRefIds.Value(i)).IsRemoved)
      ++aNbFaces;
  }
  return aNbFaces;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ShellFaceEntity(const BRepGraph_ShellId theShell,
                                                      const int               theFaceIndex) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theShell.IsValid(aStorage.NbShells()))
    return BRepGraph_NodeId();
  if (theFaceIndex < 0)
    return BRepGraph_NodeId();

  const BRepGraphInc::ShellDef& aShell    = aStorage.Shell(theShell);
  int                           anOrdinal = 0;
  for (int i = 0; i < aShell.FaceRefIds.Length(); ++i)
  {
    const BRepGraphInc::FaceRef& aRef = aStorage.FaceRef(aShell.FaceRefIds.Value(i));
    if (aRef.IsRemoved)
      continue;
    if (anOrdinal == theFaceIndex)
      return aRef.FaceDefId;
    ++anOrdinal;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

const BRepGraphInc::BaseDef* BRepGraph::TopoView::TopoEntity(const BRepGraph_NodeId theId) const
{
  return myGraph->TopoEntity(theId);
}

//=================================================================================================

size_t BRepGraph::TopoView::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return static_cast<size_t>(aS.NbSolids()) + static_cast<size_t>(aS.NbShells())
         + static_cast<size_t>(aS.NbFaces()) + static_cast<size_t>(aS.NbWires())
         + static_cast<size_t>(aS.NbCoEdges()) + static_cast<size_t>(aS.NbEdges())
         + static_cast<size_t>(aS.NbVertices()) + static_cast<size_t>(aS.NbCompounds())
         + static_cast<size_t>(aS.NbCompSolids()) + static_cast<size_t>(aS.NbProducts())
         + static_cast<size_t>(aS.NbOccurrences());
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
  const BRepGraphInc::BaseDef* aDef = myGraph->TopoEntity(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

// ==========================================================================
// Spatial adjacency queries (formerly SpatialView)
// ==========================================================================

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::SameDomainFaces(
  const BRepGraph_NodeId theFaceEntity) const
{
  // Collect faces sharing the same surface representation.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceEntity.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceEntity.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraph_FaceId       aFaceDefId(theFaceEntity.Index);
  const BRepGraphInc::FaceDef& aFaceDef = aStorage.Face(aFaceDefId);
  if (!aFaceDef.SurfaceRepId.IsValid())
    return aResult;

  for (int aFaceIdx = 0; aFaceIdx < aStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aOtherFace = aStorage.Face(BRepGraph_FaceId(aFaceIdx));
    if (aOtherFace.SurfaceRepId == aFaceDef.SurfaceRepId && aOtherFace.Id != theFaceEntity)
      aResult.Append(aOtherFace.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::FacesOfEdge(
  const BRepGraph_NodeId theEdgeEntity) const
{
  // Walk CoEdgesOfEdge to collect distinct face definitions.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId                      anEdgeDefId(theEdgeEntity.Index);
  const NCollection_Vector<BRepGraph_FaceId>* aFaces =
    aStorage.ReverseIndex().FacesOfEdge(anEdgeDefId);
  if (aFaces != nullptr)
  {
    for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      aResult.Append(aFaces->Value(aFIdx));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::SharedEdges(
  const BRepGraph_NodeId theFaceA,
  const BRepGraph_NodeId theFaceB) const
{
  // Intersect edge sets of both faces to find common edges.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceA.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceA.IsValid(aStorage.NbFaces())
      || theFaceB.NodeKind != BRepGraph_NodeId::Kind::Face || !theFaceB.IsValid(aStorage.NbFaces()))
    return aResult;

  const NCollection_Vector<BRepGraph_NodeId> aFaceAEdges = EdgesOfFace(theFaceA);
  const NCollection_Vector<BRepGraph_NodeId> aFaceBEdges = EdgesOfFace(theFaceB);
  NCollection_PackedMap<int>                 aFaceAEdgeSet;
  for (int aEdgeIdx = 0; aEdgeIdx < aFaceAEdges.Length(); ++aEdgeIdx)
  {
    if (aFaceAEdges.Value(aEdgeIdx).NodeKind == BRepGraph_NodeId::Kind::Edge)
      aFaceAEdgeSet.Add(aFaceAEdges.Value(aEdgeIdx).Index);
  }

  NCollection_PackedMap<int> aAdded;
  for (int aEdgeIdx = 0; aEdgeIdx < aFaceBEdges.Length(); ++aEdgeIdx)
  {
    const BRepGraph_NodeId anEdgeId = aFaceBEdges.Value(aEdgeIdx);
    if (anEdgeId.NodeKind != BRepGraph_NodeId::Kind::Edge)
      continue;
    if (aFaceAEdgeSet.Contains(anEdgeId.Index) && aAdded.Add(anEdgeId.Index))
      aResult.Append(anEdgeId);
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::AdjacentFaces(
  const BRepGraph_NodeId theFaceEntity) const
{
  // Collect faces sharing at least one edge with this face.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceEntity.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceEntity.IsValid(aStorage.NbFaces()))
    return aResult;

  const NCollection_Vector<BRepGraph_NodeId> anEdgeNodes = EdgesOfFace(theFaceEntity);
  const BRepGraphInc_ReverseIndex&           aRevIdx     = aStorage.ReverseIndex();
  for (int anEdgeNodeIdx = 0; anEdgeNodeIdx < anEdgeNodes.Length(); ++anEdgeNodeIdx)
  {
    const BRepGraph_NodeId anEdgeNode = anEdgeNodes.Value(anEdgeNodeIdx);
    if (anEdgeNode.NodeKind != BRepGraph_NodeId::Kind::Edge)
      continue;
    const BRepGraph_EdgeId                      anEdgeDefId(anEdgeNode.Index);
    const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfEdge(anEdgeDefId);
    if (aFaces != nullptr)
    {
      for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      {
        const int aFaceIdx = aFaces->Value(aFIdx).Index;
        if (aFaceIdx != theFaceEntity.Index && aFaceSet.Add(aFaceIdx))
          aResult.Append(aFaces->Value(aFIdx));
      }
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::EdgesOfFace(
  const BRepGraph_NodeId theFaceEntity) const
{
  // Traverse wire/coedge ref entries to collect unique edge definitions.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceEntity.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceEntity.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(BRepGraph_FaceId(theFaceEntity.Index));
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
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef =
        aStorage.CoEdgeRef(aWire.CoEdgeRefIds.Value(ce));
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
    const BRepGraphInc::WireRef& aWireRef =
      aStorage.WireRef(aFace.WireRefIds.Value(aRefIdx));
    if (!aWireRef.IsRemoved && aWireRef.IsOuter)
      return aWireRef.WireDefId;
  }
  return BRepGraph_WireId();
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::EdgesOfVertex(
  const BRepGraph_NodeId theVertexEntity) const
{
  // Delegate to reverse index for edges incident to vertex.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theVertexEntity.NodeKind != BRepGraph_NodeId::Kind::Vertex
      || !theVertexEntity.IsValid(aStorage.NbVertices()))
    return aResult;

  const BRepGraph_VertexId                    aVertexDefId(theVertexEntity.Index);
  const NCollection_Vector<BRepGraph_EdgeId>* aEdges =
    myGraph->myData->myIncStorage.ReverseIndex().EdgesOfVertex(aVertexDefId);
  if (aEdges != nullptr)
  {
    for (int i = 0; i < aEdges->Length(); ++i)
      aResult.Append(aEdges->Value(i));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::VerticesOfEdge(
  const BRepGraph_NodeId theEdgeEntity) const
{
  // Collect start, end, and internal vertices of the edge.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId       aEdgeDefId(theEdgeEntity.Index);
  const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(aEdgeDefId);
  if (anEdge.StartVertexRefId.IsValid())
    aResult.Append(aStorage.VertexRef(anEdge.StartVertexRefId).VertexDefId);
  if (anEdge.EndVertexRefId.IsValid())
    aResult.Append(aStorage.VertexRef(anEdge.EndVertexRefId).VertexDefId);
  for (int i = 0; i < anEdge.InternalVertexRefIds.Length(); ++i)
    aResult.Append(aStorage.VertexRef(anEdge.InternalVertexRefIds.Value(i)).VertexDefId);
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::AdjacentEdges(
  const BRepGraph_NodeId theEdgeEntity) const
{
  // Find edges sharing a vertex with this edge.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId           aEdgeDefId(theEdgeEntity.Index);
  const BRepGraphInc::EdgeDef&     anEdge  = aStorage.Edge(aEdgeDefId);
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_PackedMap<int>       anEdgeSet;

  // Collect all vertices of this edge, then all edges of those vertices.
  NCollection_Vector<BRepGraph_VertexId> aVertices;
  if (anEdge.StartVertexRefId.IsValid())
    aVertices.Append(aStorage.VertexRef(anEdge.StartVertexRefId).VertexDefId);
  if (anEdge.EndVertexRefId.IsValid())
    aVertices.Append(aStorage.VertexRef(anEdge.EndVertexRefId).VertexDefId);
  for (int i = 0; i < anEdge.InternalVertexRefIds.Length(); ++i)
    aVertices.Append(aStorage.VertexRef(anEdge.InternalVertexRefIds.Value(i)).VertexDefId);

  for (int v = 0; v < aVertices.Length(); ++v)
  {
    const NCollection_Vector<BRepGraph_EdgeId>* aEdges = aRevIdx.EdgesOfVertex(aVertices.Value(v));
    if (aEdges == nullptr)
      continue;
    for (int e = 0; e < aEdges->Length(); ++e)
    {
      const int anEdgeIdx = aEdges->Value(e).Index;
      if (anEdgeIdx != theEdgeEntity.Index && anEdgeSet.Add(anEdgeIdx))
        aResult.Append(aEdges->Value(e));
    }
  }
  return aResult;
}

//=================================================================================================

int BRepGraph::TopoView::FaceCountOfEdge(const BRepGraph_NodeId theEdgeEntity) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeEntity.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeEntity.IsValid(aStorage.NbEdges()))
    return 0;
  const BRepGraph_EdgeId aEdgeDefId(theEdgeEntity.Index);
  return aStorage.ReverseIndex().FaceCountOfEdge(aEdgeDefId);
}

//=================================================================================================

bool BRepGraph::TopoView::IsBoundaryEdge(const BRepGraph_NodeId theEdgeEntity) const
{
  return FaceCountOfEdge(theEdgeEntity) == 1;
}

//=================================================================================================

bool BRepGraph::TopoView::IsManifoldEdge(const BRepGraph_NodeId theEdgeEntity) const
{
  return FaceCountOfEdge(theEdgeEntity) == 2;
}
