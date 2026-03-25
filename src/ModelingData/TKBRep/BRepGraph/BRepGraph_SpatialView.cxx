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

#include <BRepGraph_SpatialView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_PackedMap.hxx>

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SameDomainFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::FacesOfEdge(
  const BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return aResult;

  const BRepGraph_EdgeId                      anEdgeDefId(theEdgeDef.Index);
  const NCollection_Vector<BRepGraph_FaceId>* aFaces = aStorage.ReverseIndex().FacesOfEdge(anEdgeDefId);
  if (aFaces != nullptr)
  {
    for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
      aResult.Append(aFaces->Value(aFIdx));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::SharedEdges(
  const BRepGraph_NodeId theFaceA,
  const BRepGraph_NodeId theFaceB) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theFaceA.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceA.IsValid(aStorage.NbFaces()) || theFaceB.NodeKind != BRepGraph_NodeId::Kind::Face
      || !theFaceB.IsValid(aStorage.NbFaces()))
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentFaces(
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  NCollection_PackedMap<int>           aFaceSet;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::EdgesOfFace(
  const BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::EdgesOfVertex(
  const BRepGraph_NodeId theVertexDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::VerticesOfEdge(
  const BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SpatialView::AdjacentEdges(
  const BRepGraph_NodeId theEdgeDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
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

int BRepGraph::SpatialView::FaceCountOfEdge(const BRepGraph_NodeId theEdgeDef) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (theEdgeDef.NodeKind != BRepGraph_NodeId::Kind::Edge
      || !theEdgeDef.IsValid(aStorage.NbEdges()))
    return 0;
  const BRepGraph_EdgeId aEdgeDefId(theEdgeDef.Index);
  return aStorage.ReverseIndex().FaceCountOfEdge(aEdgeDefId);
}

//=================================================================================================

bool BRepGraph::SpatialView::IsBoundaryEdge(const BRepGraph_NodeId theEdgeDef) const
{
  return FaceCountOfEdge(theEdgeDef) == 1;
}

//=================================================================================================

bool BRepGraph::SpatialView::IsManifoldEdge(const BRepGraph_NodeId theEdgeDef) const
{
  return FaceCountOfEdge(theEdgeDef) == 2;
}
