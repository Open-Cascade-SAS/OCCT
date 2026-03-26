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

const BRepGraph_TopoNode::SolidDef& BRepGraph::TopoView::Solid(
  const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.Solid(theSolid);
}

//=================================================================================================

const BRepGraph_TopoNode::ShellDef& BRepGraph::TopoView::Shell(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.Shell(theShell);
}

//=================================================================================================

const BRepGraph_TopoNode::FaceDef& BRepGraph::TopoView::Face(const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.Face(theFace);
}

//=================================================================================================

const BRepGraph_TopoNode::WireDef& BRepGraph::TopoView::Wire(const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.Wire(theWire);
}

//=================================================================================================

const BRepGraph_TopoNode::EdgeDef& BRepGraph::TopoView::Edge(const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.Edge(theEdge);
}

//=================================================================================================

const BRepGraph_TopoNode::VertexDef& BRepGraph::TopoView::Vertex(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.Vertex(theVertex);
}

//=================================================================================================

const BRepGraph_TopoNode::CompoundDef& BRepGraph::TopoView::Compound(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.Compound(theCompound);
}

//=================================================================================================

const BRepGraph_TopoNode::CompSolidDef& BRepGraph::TopoView::CompSolid(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid);
}

//=================================================================================================

const BRepGraph_TopoNode::CoEdgeDef& BRepGraph::TopoView::CoEdge(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdge);
}

//=================================================================================================

int BRepGraph::TopoView::NbShellFaces(const BRepGraph_ShellId theShell) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theShell.IsValid(aStorage.NbShells()))
    return 0;
  return aStorage.Shell(theShell).FaceRefs.Length();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ShellFaceDef(const BRepGraph_ShellId theShell,
                                                    const int               theFaceIndex) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theShell.IsValid(aStorage.NbShells()))
    return BRepGraph_NodeId();
  const BRepGraphInc::ShellEntity& aShellEnt = aStorage.Shell(theShell);
  if (theFaceIndex < 0 || theFaceIndex >= aShellEnt.FaceRefs.Length())
    return BRepGraph_NodeId();
  return aShellEnt.FaceRefs.Value(theFaceIndex).FaceDefId;
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::TopoView::TopoDef(const BRepGraph_NodeId theId) const
{
  return myGraph->TopoDef(theId);
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

const BRepGraphInc::CoEdgeEntity* BRepGraph::TopoView::FindPCurve(
  const BRepGraph_NodeId theEdgeDef,
  const BRepGraph_NodeId theFaceDef) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return nullptr;

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(BRepGraph_EdgeId(theEdgeDef.Index));
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeDefId == theEdgeDef && aCoEdge.FaceDefId == theFaceDef)
      return &aCoEdge;
  }
  return nullptr;
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph::TopoView::FindPCurve(
  const BRepGraph_NodeId   theEdgeDef,
  const BRepGraph_NodeId   theFaceDef,
  const TopAbs_Orientation theEdgeOrientation) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return nullptr;

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdgeIdxs =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(BRepGraph_EdgeId(theEdgeDef.Index));
  // For non-seam edges (1 CoEdge per face), return the only match.
  // For seam edges (2 CoEdges per face), prefer exact orientation match.
  const BRepGraphInc::CoEdgeEntity* aFirstMatch = nullptr;
  for (int anIter = 0; anIter < aCoEdgeIdxs.Length(); ++anIter)
  {
    const BRepGraphInc::CoEdgeEntity& aCoEdge = aStorage.CoEdge(aCoEdgeIdxs.Value(anIter));
    if (aCoEdge.EdgeDefId != theEdgeDef || aCoEdge.FaceDefId != theFaceDef)
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
  const BRepGraph_TopoNode::BaseDef* aDef = myGraph->TopoDef(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

// ==========================================================================
// Spatial adjacency queries (formerly SpatialView)
// ==========================================================================

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::SameDomainFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  // Collect faces sharing the same surface representation.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceDef.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraph_FaceId             aFaceDefId(theFaceDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aStorage.Face(aFaceDefId);
  if (!aFaceDef.SurfaceRepId.IsValid())
    return aResult;

  for (int aFaceIdx = 0; aFaceIdx < aStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraph_TopoNode::FaceDef& aOtherFace = aStorage.Face(BRepGraph_FaceId(aFaceIdx));
    if (aOtherFace.SurfaceRepId == aFaceDef.SurfaceRepId && aOtherFace.Id != theFaceDef)
      aResult.Append(aOtherFace.Id);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::FacesOfEdge(
  const BRepGraph_NodeId theEdgeDef) const
{
  // Walk CoEdgesOfEdge to collect distinct face definitions.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId                      anEdgeDefId(theEdgeDef.Index);
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

  const BRepGraph_FaceId             aFaceDefAId(theFaceA.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDefA = aStorage.Face(aFaceDefAId);

  NCollection_PackedMap<int> aEdgesA;
  for (int aWIdx = 0; aWIdx < aFaceDefA.WireRefs.Length(); ++aWIdx)
  {
    const BRepGraph_WireId             aWireDefId = aFaceDefA.WireRefs.Value(aWIdx).WireDefId;
    const BRepGraph_TopoNode::WireDef& aWireDef   = aStorage.Wire(aWireDefId);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        aStorage.CoEdge(BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
      aEdgesA.Add(aCoEdge.EdgeDefId.Index);
    }
  }

  const BRepGraph_FaceId             aFaceDefBId(theFaceB.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDefB = aStorage.Face(aFaceDefBId);

  NCollection_PackedMap<int> aAdded;
  for (int aWIdx = 0; aWIdx < aFaceDefB.WireRefs.Length(); ++aWIdx)
  {
    const BRepGraph_WireId             aWireDefId = aFaceDefB.WireRefs.Value(aWIdx).WireDefId;
    const BRepGraph_TopoNode::WireDef& aWireDef   = aStorage.Wire(aWireDefId);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        aStorage.CoEdge(BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
      const BRepGraph_EdgeId anEdgeDefId = aCoEdge.EdgeDefId;
      if (aEdgesA.Contains(anEdgeDefId.Index) && aAdded.Add(anEdgeDefId.Index))
        aResult.Append(aStorage.Edge(anEdgeDefId).Id);
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::AdjacentFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  // Collect faces sharing at least one edge with this face.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceDef.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraph_FaceId             aFaceDefId(theFaceDef.Index);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = aStorage.Face(aFaceDefId);

  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  for (int aWireRefIdx = 0; aWireRefIdx < aFaceDef.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraph_WireId             aWireDefId = aFaceDef.WireRefs.Value(aWireRefIdx).WireDefId;
    const BRepGraph_TopoNode::WireDef& aWireDef   = aStorage.Wire(aWireDefId);
    for (int aCoEdgeIdx = 0; aCoEdgeIdx < aWireDef.CoEdgeRefs.Length(); ++aCoEdgeIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        aStorage.CoEdge(BRepGraph_CoEdgeId(aWireDef.CoEdgeRefs.Value(aCoEdgeIdx).CoEdgeDefId));
      const BRepGraph_EdgeId                      anEdgeDefId = aCoEdge.EdgeDefId;
      const NCollection_Vector<BRepGraph_FaceId>* aFaces      = aRevIdx.FacesOfEdge(anEdgeDefId);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const int aFaceIdx = aFaces->Value(aFIdx).Index;
          if (aFaceIdx != theFaceDef.Index && aFaceSet.Add(aFaceIdx))
            aResult.Append(aFaces->Value(aFIdx));
        }
      }
    }
  }

  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::EdgesOfFace(
  const BRepGraph_NodeId theFaceDef) const
{
  // Traverse all wires and coedges to collect unique edge definitions.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theFaceDef.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceDef.IsValid(aStorage.NbFaces()))
    return aResult;

  const BRepGraph_FaceId          aFaceDefId(theFaceDef.Index);
  const BRepGraphInc::FaceEntity& aFaceDef = aStorage.Face(aFaceDefId);
  NCollection_PackedMap<int>      anEdgeSet;

  for (int aWIdx = 0; aWIdx < aFaceDef.WireRefs.Length(); ++aWIdx)
  {
    const BRepGraph_WireId          aWireDefId = aFaceDef.WireRefs.Value(aWIdx).WireDefId;
    const BRepGraphInc::WireEntity& aWire      = aStorage.Wire(aWireDefId);
    for (int aCEIdx = 0; aCEIdx < aWire.CoEdgeRefs.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        aStorage.CoEdge(BRepGraph_CoEdgeId(aWire.CoEdgeRefs.Value(aCEIdx).CoEdgeDefId));
      if (anEdgeSet.Add(aCoEdge.EdgeDefId.Index))
        aResult.Append(aCoEdge.EdgeDefId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::EdgesOfVertex(
  const BRepGraph_NodeId theVertexDef) const
{
  // Delegate to reverse index for edges incident to vertex.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theVertexDef.NodeKind != BRepGraph_NodeId::Kind::Vertex
      || !theVertexDef.IsValid(aStorage.NbVertices()))
    return aResult;

  const BRepGraph_VertexId                    aVertexDefId(theVertexDef.Index);
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
  const BRepGraph_NodeId theEdgeDef) const
{
  // Collect start, end, and internal vertices of the edge.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId          aEdgeDefId(theEdgeDef.Index);
  const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edge(aEdgeDefId);
  if (anEdge.StartVertex.VertexDefId.IsValid())
    aResult.Append(anEdge.StartVertex.VertexDefId);
  if (anEdge.EndVertex.VertexDefId.IsValid())
    aResult.Append(anEdge.EndVertex.VertexDefId);
  for (int i = 0; i < anEdge.InternalVertices.Length(); ++i)
    aResult.Append(anEdge.InternalVertices.Value(i).VertexDefId);
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::AdjacentEdges(
  const BRepGraph_NodeId theEdgeDef) const
{
  // Find edges sharing a vertex with this edge.
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId           aEdgeDefId(theEdgeDef.Index);
  const BRepGraphInc::EdgeEntity&  anEdge  = aStorage.Edge(aEdgeDefId);
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_PackedMap<int>       anEdgeSet;

  // Collect all vertices of this edge, then all edges of those vertices.
  NCollection_Vector<BRepGraph_VertexId> aVertices;
  if (anEdge.StartVertex.VertexDefId.IsValid())
    aVertices.Append(anEdge.StartVertex.VertexDefId);
  if (anEdge.EndVertex.VertexDefId.IsValid())
    aVertices.Append(anEdge.EndVertex.VertexDefId);
  for (int i = 0; i < anEdge.InternalVertices.Length(); ++i)
    aVertices.Append(anEdge.InternalVertices.Value(i).VertexDefId);

  for (int v = 0; v < aVertices.Length(); ++v)
  {
    const NCollection_Vector<BRepGraph_EdgeId>* aEdges = aRevIdx.EdgesOfVertex(aVertices.Value(v));
    if (aEdges == nullptr)
      continue;
    for (int e = 0; e < aEdges->Length(); ++e)
    {
      const int anEdgeIdx = aEdges->Value(e).Index;
      if (anEdgeIdx != theEdgeDef.Index && anEdgeSet.Add(anEdgeIdx))
        aResult.Append(aEdges->Value(e));
    }
  }
  return aResult;
}

//=================================================================================================

int BRepGraph::TopoView::FaceCountOfEdge(const BRepGraph_NodeId theEdgeDef) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return 0;
  const BRepGraph_EdgeId aEdgeDefId(theEdgeDef.Index);
  return aStorage.ReverseIndex().FaceCountOfEdge(aEdgeDefId);
}

//=================================================================================================

bool BRepGraph::TopoView::IsBoundaryEdge(const BRepGraph_NodeId theEdgeDef) const
{
  return FaceCountOfEdge(theEdgeDef) == 1;
}

//=================================================================================================

bool BRepGraph::TopoView::IsManifoldEdge(const BRepGraph_NodeId theEdgeDef) const
{
  return FaceCountOfEdge(theEdgeDef) == 2;
}
