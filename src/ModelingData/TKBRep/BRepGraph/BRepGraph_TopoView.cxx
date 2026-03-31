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
#include <BRepGraph_RefsView.hxx>
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
constexpr int THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE    = 4;
constexpr int THE_TOPOVIEW_EDGE_ADJACENCY_BLOCK_SIZE = 8;
constexpr int THE_TOPOVIEW_VERTEX_FACE_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE    = 4;
constexpr int THE_TOPOVIEW_WIRE_COEDGE_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_WIRE_EDGE_BLOCK_SIZE      = 8;
constexpr int THE_TOPOVIEW_SHELL_FACE_BLOCK_SIZE     = 8;
constexpr int THE_TOPOVIEW_SHELL_CHILD_BLOCK_SIZE    = 2;
constexpr int THE_TOPOVIEW_SOLID_SHELL_BLOCK_SIZE    = 2;
constexpr int THE_TOPOVIEW_SOLID_CHILD_BLOCK_SIZE    = 2;
constexpr int THE_TOPOVIEW_COMPOUND_CHILD_BLOCK_SIZE = 8;
constexpr int THE_TOPOVIEW_COMPSOLID_SOLID_BLOCK_SIZE = 4;
constexpr int THE_TOPOVIEW_PRODUCT_OCCURRENCE_BLOCK_SIZE = 4;

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> collectActiveChildNodes(
  const BRepGraph&                                theGraph,
  const NCollection_Vector<BRepGraph_ChildRefId>& theChildRefIds,
  const int                                       theBlockSize,
  const occ::handle<NCollection_BaseAllocator>&   theAllocator)
{
  NCollection_Vector<BRepGraph_NodeId> aResult(theBlockSize, theAllocator);
  NCollection_PackedMap<int>           aSeenByKind[BRepGraph_NodeId::THE_KIND_COUNT];
  for (int aRefIdx = 0; aRefIdx < theChildRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::ChildRef& aRef = theGraph.Refs().Child(theChildRefIds.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
    {
      continue;
    }

    const BRepGraphInc::BaseDef* aChild = theGraph.Topo().TopoEntity(aRef.ChildDefId);
    if (aChild == nullptr || aChild->IsRemoved)
    {
      continue;
    }

    const int aKindIdx = static_cast<int>(aRef.ChildDefId.NodeKind);
    if (aKindIdx < 0 || aKindIdx >= BRepGraph_NodeId::THE_KIND_COUNT)
    {
      continue;
    }

    if (aSeenByKind[aKindIdx].Add(aRef.ChildDefId.Index))
    {
      aResult.Append(aRef.ChildDefId);
    }
  }
  return aResult;
}

//=================================================================================================

}

// ==========================================================================
// Tool-like grouped helpers.
// ==========================================================================

//=================================================================================================

const BRepGraphInc::FaceDef& BRepGraph::TopoView::FaceOps::Definition(
  const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.Face(theFace);
}

//=================================================================================================

BRepGraph_SurfaceRepId BRepGraph::TopoView::FaceOps::SurfaceRepId(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_SurfaceRepId();
  }

  const BRepGraph_SurfaceRepId aRepId = aStorage.Face(theFace).SurfaceRepId;
  if (!aRepId.IsValid(aStorage.NbSurfaces()) || aStorage.SurfaceRep(aRepId).IsRemoved)
  {
    return BRepGraph_SurfaceRepId();
  }
  return aRepId;
}

//=================================================================================================

BRepGraph_TriangulationRepId BRepGraph::TopoView::FaceOps::ActiveTriangulationRepId(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_TriangulationRepId();
  }

  const BRepGraph_TriangulationRepId aRepId = aStorage.Face(theFace).ActiveTriangulationRepId();
  if (!aRepId.IsValid(aStorage.NbTriangulations()) || aStorage.TriangulationRep(aRepId).IsRemoved)
  {
    return BRepGraph_TriangulationRepId();
  }
  return aRepId;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::FaceOps::SameDomain(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE,
                                               theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const BRepGraphInc::FaceDef& aFaceDef = aStorage.Face(theFace);
  if (!aFaceDef.SurfaceRepId.IsValid())
  {
    return aResult;
  }

  for (int aFaceIdx = 0; aFaceIdx < aStorage.NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& anOtherFace = aStorage.Face(BRepGraph_FaceId(aFaceIdx));
    if (anOtherFace.IsRemoved)
    {
      continue;
    }
    if (aFaceIdx != theFace.Index && anOtherFace.SurfaceRepId == aFaceDef.SurfaceRepId)
    {
      aResult.Append(BRepGraph_FaceId(aFaceIdx));
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::FaceOps::SharedEdges(
  const BRepGraph_FaceId                          theFaceA,
  const BRepGraph_FaceId                          theFaceB,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE,
                                               theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFaceA.IsValid(aStorage.NbFaces()) || !theFaceB.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const NCollection_Vector<BRepGraph_EdgeId> aFaceAEdges = Edges(theFaceA, theAllocator);
  const NCollection_Vector<BRepGraph_EdgeId> aFaceBEdges = Edges(theFaceB, theAllocator);
  NCollection_PackedMap<int>                 aFaceAEdgeSet;
  NCollection_PackedMap<int>                 anAddedEdges;

  for (int aFaceAIdx = 0; aFaceAIdx < aFaceAEdges.Length(); ++aFaceAIdx)
  {
    aFaceAEdgeSet.Add(aFaceAEdges.Value(aFaceAIdx).Index);
  }

  for (int aFaceBIdx = 0; aFaceBIdx < aFaceBEdges.Length(); ++aFaceBIdx)
  {
    const BRepGraph_EdgeId anEdgeId = aFaceBEdges.Value(aFaceBIdx);
    if (aFaceAEdgeSet.Contains(anEdgeId.Index) && anAddedEdges.Add(anEdgeId.Index))
    {
      aResult.Append(anEdgeId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::FaceOps::Adjacent(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_FACE_ADJACENCY_BLOCK_SIZE,
                                               theAllocator);
  NCollection_PackedMap<int>           aFaceSet;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const NCollection_Vector<BRepGraph_EdgeId> anEdges = Edges(theFace, theAllocator);
  const BRepGraphInc_ReverseIndex&           aRevIdx = aStorage.ReverseIndex();
  for (int anEdgeIdx = 0; anEdgeIdx < anEdges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfEdge(anEdges.Value(anEdgeIdx));
    if (aFaces == nullptr)
    {
      continue;
    }

    for (int aFaceIdx = 0; aFaceIdx < aFaces->Length(); ++aFaceIdx)
    {
      const BRepGraph_FaceId anAdjacentFaceId = aFaces->Value(aFaceIdx);
      if (anAdjacentFaceId == theFace)
      {
        continue;
      }

      const BRepGraphInc::FaceDef& anAdjacentFace = aStorage.Face(anAdjacentFaceId);
      if (anAdjacentFace.IsRemoved)
      {
        continue;
      }

      if (aFaceSet.Add(anAdjacentFaceId.Index))
      {
        aResult.Append(anAdjacentFaceId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::FaceOps::Edges(
  const BRepGraph_FaceId                          theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_FACE_EDGE_BLOCK_SIZE,
                                               theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  NCollection_PackedMap<int>   anEdgeSet;
  for (int aWireRefIdx = 0; aWireRefIdx < aFace.WireRefIds.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aFace.WireRefIds.Value(aWireRefIdx));
    if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(aStorage.NbWires()))
    {
      continue;
    }

    const BRepGraphInc::WireDef& aWire = aStorage.Wire(aWireRef.WireDefId);
    for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < aWire.CoEdgeRefIds.Length(); ++aCoEdgeRefIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = aStorage.CoEdgeRef(aWire.CoEdgeRefIds.Value(aCoEdgeRefIdx));
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(aStorage.NbCoEdges()))
      {
        continue;
      }

      const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(aStorage.NbEdges()))
      {
        continue;
      }

      if (anEdgeSet.Add(aCoEdge.EdgeDefId.Index))
      {
        aResult.Append(aCoEdge.EdgeDefId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

BRepGraph_WireId BRepGraph::TopoView::FaceOps::OuterWire(const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_WireId();
  }

  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  for (int aRefIdx = 0; aRefIdx < aFace.WireRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aFace.WireRefIds.Value(aRefIdx));
    if (!aWireRef.IsRemoved && aWireRef.IsOuter)
    {
      return aWireRef.WireDefId;
    }
  }
  return BRepGraph_WireId();
}

//=================================================================================================

const BRepGraphInc::EdgeDef& BRepGraph::TopoView::EdgeOps::Definition(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.Edge(theEdge);
}

//=================================================================================================

int BRepGraph::TopoView::EdgeOps::NbFaces(const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().NbFacesOfEdge(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_WireId>& BRepGraph::TopoView::EdgeOps::Wires(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().WiresOfEdgeRef(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CoEdgeId>& BRepGraph::TopoView::EdgeOps::CoEdges(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_FaceId>& BRepGraph::TopoView::EdgeOps::Faces(
  const BRepGraph_EdgeId theEdge) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdgeRef(theEdge);
}

//=================================================================================================

BRepGraph_Curve3DRepId BRepGraph::TopoView::EdgeOps::Curve3DRepId(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return BRepGraph_Curve3DRepId();
  }

  const BRepGraph_Curve3DRepId aRepId = aStorage.Edge(theEdge).Curve3DRepId;
  if (!aRepId.IsValid(aStorage.NbCurves3D()) || aStorage.Curve3DRep(aRepId).IsRemoved)
  {
    return BRepGraph_Curve3DRepId();
  }
  return aRepId;
}

//=================================================================================================

NCollection_Vector<BRepGraph_VertexId> BRepGraph::TopoView::EdgeOps::Vertices(
  const BRepGraph_EdgeId                          theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_VertexId> aResult(THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE,
                                                 theAllocator);
  const BRepGraphInc_Storage&            aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return aResult;
  }

  const BRepGraphInc::EdgeDef& aDef          = aStorage.Edge(theEdge);
  const int                    aNbVertexRefs = aStorage.NbVertexRefs();
  NCollection_PackedMap<int>   aSeenRefIds;

  const auto aProcessRefId = [&aResult, &aSeenRefIds, &aStorage, aNbVertexRefs](
                               const BRepGraph_VertexRefId theRefId) {
    if (!theRefId.IsValid(aNbVertexRefs) || !aSeenRefIds.Add(theRefId.Index))
    {
      return;
    }

    const BRepGraphInc::VertexRef& aVertexRef = aStorage.VertexRef(theRefId);
    if (aVertexRef.IsRemoved)
    {
      return;
    }
    aResult.Append(aVertexRef.VertexDefId);
  };

  aProcessRefId(aDef.StartVertexRefId);
  aProcessRefId(aDef.EndVertexRefId);
  for (int aRefIdx = 0; aRefIdx < aDef.InternalVertexRefIds.Length(); ++aRefIdx)
  {
    aProcessRefId(aDef.InternalVertexRefIds.Value(aRefIdx));
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::EdgeOps::Adjacent(
  const BRepGraph_EdgeId                          theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_EDGE_ADJACENCY_BLOCK_SIZE,
                                               theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return aResult;
  }

  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_PackedMap<int>       anEdgeSet;
  const NCollection_Vector<BRepGraph_VertexId> aVertices = Vertices(theEdge, theAllocator);
  for (int aVertexIdx = 0; aVertexIdx < aVertices.Length(); ++aVertexIdx)
  {
    const NCollection_Vector<BRepGraph_EdgeId>* anEdges = aRevIdx.EdgesOfVertex(aVertices.Value(aVertexIdx));
    if (anEdges == nullptr)
    {
      continue;
    }

    for (int anEdgeIdx = 0; anEdgeIdx < anEdges->Length(); ++anEdgeIdx)
    {
      const BRepGraph_EdgeId anAdjacentEdgeId = anEdges->Value(anEdgeIdx);
      if (anAdjacentEdgeId == theEdge)
      {
        continue;
      }

      const BRepGraphInc::EdgeDef& anAdjacentEdge = aStorage.Edge(anAdjacentEdgeId);
      if (anAdjacentEdge.IsRemoved)
      {
        continue;
      }

      if (anEdgeSet.Add(anAdjacentEdgeId.Index))
      {
        aResult.Append(anAdjacentEdgeId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

bool BRepGraph::TopoView::EdgeOps::IsBoundary(const BRepGraph_EdgeId theEdge) const
{
  return NbFaces(theEdge) == 1;
}

//=================================================================================================

bool BRepGraph::TopoView::EdgeOps::IsManifold(const BRepGraph_EdgeId theEdge) const
{
  return NbFaces(theEdge) == 2;
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph::TopoView::EdgeOps::FindPCurve(
  const BRepGraph_EdgeId theEdge,
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || !theFace.IsValid(aStorage.NbFaces()))
  {
    return nullptr;
  }

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aCoEdges.Length(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdges.Value(aCoEdgeIdx));
    if (aCoEdge.EdgeDefId == theEdge && aCoEdge.FaceDefId == theFace)
    {
      return &aCoEdge;
    }
  }
  return nullptr;
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph::TopoView::EdgeOps::FindPCurve(
  const BRepGraph_EdgeId   theEdge,
  const BRepGraph_FaceId   theFace,
  const TopAbs_Orientation theOrientation) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || !theFace.IsValid(aStorage.NbFaces()))
  {
    return nullptr;
  }

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges = aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  const BRepGraphInc::CoEdgeDef*                aFirstMatch = nullptr;
  for (int aCoEdgeIdx = 0; aCoEdgeIdx < aCoEdges.Length(); ++aCoEdgeIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdges.Value(aCoEdgeIdx));
    if (aCoEdge.EdgeDefId != theEdge || aCoEdge.FaceDefId != theFace)
    {
      continue;
    }
    if (aFirstMatch == nullptr)
    {
      aFirstMatch = &aCoEdge;
    }
    if (aCoEdge.Sense == theOrientation)
    {
      return &aCoEdge;
    }
  }
  return aFirstMatch;
}

//=================================================================================================

const BRepGraphInc::VertexDef& BRepGraph::TopoView::VertexOps::Definition(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.Vertex(theVertex);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_EdgeId>& BRepGraph::TopoView::VertexOps::Edges(
  const BRepGraph_VertexId theVertex) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().EdgesOfVertexRef(theVertex);
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::VertexOps::Faces(
  const BRepGraph_VertexId                        theVertex,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_VERTEX_FACE_BLOCK_SIZE,
                                               theAllocator);
  const NCollection_Vector<BRepGraph_EdgeId>& anEdges = Edges(theVertex);
  NCollection_Map<int>                        aSeenFaces;
  for (int anEdgeIdx = 0; anEdgeIdx < anEdges.Length(); ++anEdgeIdx)
  {
    const NCollection_Vector<BRepGraph_FaceId>& aFaces = myGraph->myData->myIncStorage.ReverseIndex().FacesOfEdgeRef(anEdges.Value(anEdgeIdx));
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

//=================================================================================================

const BRepGraphInc::WireDef& BRepGraph::TopoView::WireOps::Definition(
  const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.Wire(theWire);
}

//=================================================================================================

NCollection_Vector<BRepGraph_CoEdgeId> BRepGraph::TopoView::WireOps::CoEdges(
  const BRepGraph_WireId                          theWire,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_CoEdgeId> aResult(THE_TOPOVIEW_WIRE_COEDGE_BLOCK_SIZE,
                                                 theAllocator);
  const BRepGraphInc_Storage&            aStorage = myGraph->myData->myIncStorage;
  if (!theWire.IsValid(myGraph->Topo().NbWires()))
  {
    return aResult;
  }

  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefs = myGraph->Refs().CoEdgeRefIdsOf(theWire);
  for (int aRefIdx = 0; aRefIdx < aCoEdgeRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRef& aRef = myGraph->Refs().CoEdge(aCoEdgeRefs.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(myGraph->Topo().NbCoEdges()))
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aRef.CoEdgeDefId);
    if (aCoEdge.IsRemoved)
    {
      continue;
    }
    aResult.Append(aRef.CoEdgeDefId);
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::WireOps::Edges(
  const BRepGraph_WireId                          theWire,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_WIRE_EDGE_BLOCK_SIZE, theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theWire.IsValid(myGraph->Topo().NbWires()))
  {
    return aResult;
  }

  NCollection_PackedMap<int> aSeenEdges;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aCoEdgeRefs = myGraph->Refs().CoEdgeRefIdsOf(theWire);
  for (int aRefIdx = 0; aRefIdx < aCoEdgeRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRef& aRef = myGraph->Refs().CoEdge(aCoEdgeRefs.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.CoEdgeDefId.IsValid(myGraph->Topo().NbCoEdges()))
    {
      continue;
    }

    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aRef.CoEdgeDefId);
    if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(myGraph->Topo().NbEdges()))
    {
      continue;
    }

    const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(aCoEdge.EdgeDefId);
    if (anEdge.IsRemoved)
    {
      continue;
    }

    if (aSeenEdges.Add(aCoEdge.EdgeDefId.Index))
    {
      aResult.Append(aCoEdge.EdgeDefId);
    }
  }
  return aResult;
}

//=================================================================================================

const BRepGraphInc::ShellDef& BRepGraph::TopoView::ShellOps::Definition(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.Shell(theShell);
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::ShellOps::Faces(
  const BRepGraph_ShellId                         theShell,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_SHELL_FACE_BLOCK_SIZE, theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theShell.IsValid(myGraph->Topo().NbShells()))
  {
    return aResult;
  }

  NCollection_PackedMap<int> aSeenFaces;
  const NCollection_Vector<BRepGraph_FaceRefId>& aFaceRefs = myGraph->Refs().FaceRefIdsOf(theShell);
  for (int aRefIdx = 0; aRefIdx < aFaceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::FaceRef& aRef = myGraph->Refs().Face(aFaceRefs.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(myGraph->Topo().NbFaces()))
    {
      continue;
    }

    const BRepGraphInc::FaceDef& aFace = aStorage.Face(aRef.FaceDefId);
    if (aFace.IsRemoved)
    {
      continue;
    }

    if (aSeenFaces.Add(aRef.FaceDefId.Index))
    {
      aResult.Append(aRef.FaceDefId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::ShellOps::FreeChildren(
  const BRepGraph_ShellId                         theShell,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  if (!theShell.IsValid(myGraph->Topo().NbShells()))
  {
    return NCollection_Vector<BRepGraph_NodeId>(THE_TOPOVIEW_SHELL_CHILD_BLOCK_SIZE, theAllocator);
  }

  return collectActiveChildNodes(*myGraph,
                                 myGraph->myData->myIncStorage.Shell(theShell).FreeChildRefIds,
                                 THE_TOPOVIEW_SHELL_CHILD_BLOCK_SIZE,
                                 theAllocator);
}

//=================================================================================================

const BRepGraphInc::SolidDef& BRepGraph::TopoView::SolidOps::Definition(
  const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.Solid(theSolid);
}

//=================================================================================================

NCollection_Vector<BRepGraph_ShellId> BRepGraph::TopoView::SolidOps::Shells(
  const BRepGraph_SolidId                         theSolid,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_ShellId> aResult(THE_TOPOVIEW_SOLID_SHELL_BLOCK_SIZE, theAllocator);
  const BRepGraphInc_Storage&           aStorage = myGraph->myData->myIncStorage;
  if (!theSolid.IsValid(myGraph->Topo().NbSolids()))
  {
    return aResult;
  }

  NCollection_PackedMap<int> aSeenShells;
  const NCollection_Vector<BRepGraph_ShellRefId>& aShellRefs = myGraph->Refs().ShellRefIdsOf(theSolid);
  for (int aRefIdx = 0; aRefIdx < aShellRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::ShellRef& aRef = myGraph->Refs().Shell(aShellRefs.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.ShellDefId.IsValid(myGraph->Topo().NbShells()))
    {
      continue;
    }

    const BRepGraphInc::ShellDef& aShell = aStorage.Shell(aRef.ShellDefId);
    if (aShell.IsRemoved)
    {
      continue;
    }

    if (aSeenShells.Add(aRef.ShellDefId.Index))
    {
      aResult.Append(aRef.ShellDefId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::SolidOps::FreeChildren(
  const BRepGraph_SolidId                         theSolid,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  if (!theSolid.IsValid(myGraph->Topo().NbSolids()))
  {
    return NCollection_Vector<BRepGraph_NodeId>(THE_TOPOVIEW_SOLID_CHILD_BLOCK_SIZE, theAllocator);
  }

  return collectActiveChildNodes(*myGraph,
                                 myGraph->myData->myIncStorage.Solid(theSolid).FreeChildRefIds,
                                 THE_TOPOVIEW_SOLID_CHILD_BLOCK_SIZE,
                                 theAllocator);
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef& BRepGraph::TopoView::CoEdgeOps::Definition(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdge);
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph::TopoView::CoEdgeOps::Edge(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_EdgeId();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(aStorage.NbEdges()))
  {
    return BRepGraph_EdgeId();
  }
  if (aStorage.Edge(aCoEdge.EdgeDefId).IsRemoved)
  {
    return BRepGraph_EdgeId();
  }
  return aCoEdge.EdgeDefId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph::TopoView::CoEdgeOps::Face(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_FaceId();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (aCoEdge.IsRemoved || !aCoEdge.FaceDefId.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_FaceId();
  }
  if (aStorage.Face(aCoEdge.FaceDefId).IsRemoved)
  {
    return BRepGraph_FaceId();
  }
  return aCoEdge.FaceDefId;
}

//=================================================================================================

BRepGraph_Curve2DRepId BRepGraph::TopoView::CoEdgeOps::Curve2DRepId(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_Curve2DRepId();
  }

  const BRepGraph_Curve2DRepId aRepId = aStorage.CoEdge(theCoEdge).Curve2DRepId;
  if (!aRepId.IsValid(aStorage.NbCurves2D()) || aStorage.Curve2DRep(aRepId).IsRemoved)
  {
    return BRepGraph_Curve2DRepId();
  }
  return aRepId;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph::TopoView::CoEdgeOps::SeamPair(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_CoEdgeId();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdge);
  if (aCoEdge.IsRemoved || !aCoEdge.SeamPairId.IsValid(aStorage.NbCoEdges()))
  {
    return BRepGraph_CoEdgeId();
  }
  if (aStorage.CoEdge(aCoEdge.SeamPairId).IsRemoved)
  {
    return BRepGraph_CoEdgeId();
  }
  return aCoEdge.SeamPairId;
}

//=================================================================================================

const BRepGraphInc::CompoundDef& BRepGraph::TopoView::CompoundOps::Definition(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.Compound(theCompound);
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::TopoView::CompoundOps::Children(
  const BRepGraph_CompoundId                      theCompound,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  if (!theCompound.IsValid(myGraph->Topo().NbCompounds()))
  {
    return NCollection_Vector<BRepGraph_NodeId>(THE_TOPOVIEW_COMPOUND_CHILD_BLOCK_SIZE, theAllocator);
  }

  return collectActiveChildNodes(*myGraph,
                                 myGraph->myData->myIncStorage.Compound(theCompound).ChildRefIds,
                                 THE_TOPOVIEW_COMPOUND_CHILD_BLOCK_SIZE,
                                 theAllocator);
}

//=================================================================================================

const BRepGraphInc::CompSolidDef& BRepGraph::TopoView::CompSolidOps::Definition(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid);
}

//=================================================================================================

NCollection_Vector<BRepGraph_SolidId> BRepGraph::TopoView::CompSolidOps::Solids(
  const BRepGraph_CompSolidId                     theCompSolid,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  const BRepGraphInc_Storage&           aStorage = myGraph->myData->myIncStorage;
  NCollection_Vector<BRepGraph_SolidId> aResult(THE_TOPOVIEW_COMPSOLID_SOLID_BLOCK_SIZE,
                                                theAllocator);
  if (!theCompSolid.IsValid(aStorage.NbCompSolids()))
  {
    return aResult;
  }

  NCollection_PackedMap<int> aSeenSolidIds;
  const BRepGraphInc::CompSolidDef& aDef = aStorage.CompSolid(theCompSolid);
  const int aNbSolids = aStorage.NbSolids();
  for (int aRefIdx = 0; aRefIdx < aDef.SolidRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::SolidRef& aRef = aStorage.SolidRef(aDef.SolidRefIds.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.SolidDefId.IsValid(aNbSolids))
    {
      continue;
    }

    if (aStorage.Solid(aRef.SolidDefId).IsRemoved)
    {
      continue;
    }

    if (aSeenSolidIds.Add(aRef.SolidDefId.Index))
    {
      aResult.Append(aRef.SolidDefId);
    }
  }
  return aResult;
}

//=================================================================================================

const BRepGraphInc::ProductDef& BRepGraph::TopoView::ProductOps::Definition(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.Product(theProduct);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ProductOps::ShapeRoot(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_NodeId();
  }

  const BRepGraph_NodeId aShapeRoot = aStorage.Product(theProduct).ShapeRootId;
  if (!aShapeRoot.IsValid())
  {
    return BRepGraph_NodeId();
  }

  const BRepGraphInc::BaseDef* aRoot = myGraph->Topo().TopoEntity(aShapeRoot);
  if (aRoot == nullptr || aRoot->IsRemoved)
  {
    return BRepGraph_NodeId();
  }
  return aShapeRoot;
}

//=================================================================================================

NCollection_Vector<BRepGraph_OccurrenceId> BRepGraph::TopoView::ProductOps::Occurrences(
  const BRepGraph_ProductId                       theProduct,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_OccurrenceId> aResult(THE_TOPOVIEW_PRODUCT_OCCURRENCE_BLOCK_SIZE,
                                                     theAllocator);
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return aResult;
  }

  NCollection_PackedMap<int> aSeenOccurrences;
  const NCollection_Vector<BRepGraph_OccurrenceRefId>& aOccurrenceRefs =
    myGraph->Refs().OccurrenceRefIdsOf(theProduct);
  for (int aRefIdx = 0; aRefIdx < aOccurrenceRefs.Length(); ++aRefIdx)
  {
    const BRepGraphInc::OccurrenceRef& aRef = myGraph->Refs().Occurrence(aOccurrenceRefs.Value(aRefIdx));
    if (aRef.IsRemoved || !aRef.OccurrenceDefId.IsValid(myGraph->Topo().NbOccurrences()))
    {
      continue;
    }

    const BRepGraphInc::OccurrenceDef& aOccurrence = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (aOccurrence.IsRemoved)
    {
      continue;
    }

    if (aSeenOccurrences.Add(aRef.OccurrenceDefId.Index))
    {
      aResult.Append(aRef.OccurrenceDefId);
    }
  }
  return aResult;
}

//=================================================================================================

const BRepGraphInc::OccurrenceDef& BRepGraph::TopoView::OccurrenceOps::Definition(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  return myGraph->myData->myIncStorage.Occurrence(theOccurrence);
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::TopoView::OccurrenceOps::Product(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_ProductId();
  }

  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (anOccurrence.IsRemoved || !anOccurrence.ProductDefId.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_ProductId();
  }
  if (aStorage.Product(anOccurrence.ProductDefId).IsRemoved)
  {
    return BRepGraph_ProductId();
  }
  return anOccurrence.ProductDefId;
}

//=================================================================================================

BRepGraph_ProductId BRepGraph::TopoView::OccurrenceOps::ParentProduct(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_ProductId();
  }

  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (anOccurrence.IsRemoved || !anOccurrence.ParentProductDefId.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_ProductId();
  }
  if (aStorage.Product(anOccurrence.ParentProductDefId).IsRemoved)
  {
    return BRepGraph_ProductId();
  }
  return anOccurrence.ParentProductDefId;
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::TopoView::OccurrenceOps::ParentOccurrence(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_OccurrenceId();
  }

  const BRepGraphInc::OccurrenceDef& anOccurrence = aStorage.Occurrence(theOccurrence);
  if (anOccurrence.IsRemoved || !anOccurrence.ParentOccurrenceDefId.IsValid(aStorage.NbOccurrences()))
  {
    return BRepGraph_OccurrenceId();
  }
  if (aStorage.Occurrence(anOccurrence.ParentOccurrenceDefId).IsRemoved)
  {
    return BRepGraph_OccurrenceId();
  }
  return anOccurrence.ParentOccurrenceDefId;
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

bool BRepGraph::TopoView::IsRemoved(const BRepGraph_NodeId theNode) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
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
