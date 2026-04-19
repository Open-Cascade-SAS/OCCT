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
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
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
constexpr int THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE    = 8;
constexpr int THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE    = 4;

//! Collect unique edge IDs reachable from a face through its wire/coedge refs.
NCollection_Vector<BRepGraph_EdgeId> collectFaceEdges(
  const BRepGraph&                              theGraph,
  const BRepGraph_FaceId                        theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_FACE_EDGE_BLOCK_SIZE, theAllocator);
  if (!theFace.IsValid(theGraph.Topo().Faces().Nb()))
    return aResult;

  NCollection_Map<BRepGraph_EdgeId> anEdgeSet;
  for (BRepGraph_DefsWireOfFace aWireIt(theGraph, theFace); aWireIt.More(); aWireIt.Next())
  {
    for (BRepGraph_DefsEdgeOfWire anEdgeIt(theGraph, aWireIt.CurrentId()); anEdgeIt.More();
         anEdgeIt.Next())
    {
      const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
      if (anEdgeSet.Add(anEdgeId))
      {
        aResult.Append(anEdgeId);
      }
    }
  }
  return aResult;
}

//=================================================================================================

template <typename T>
const NCollection_Vector<T>& emptyVector()
{
  static const NCollection_Vector<T> THE_EMPTY_VECTOR;
  return THE_EMPTY_VECTOR;
}

} // namespace

// ==========================================================================
// Tool-like grouped helpers.
// ==========================================================================

//=================================================================================================

int BRepGraph::TopoView::FaceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbFaces();
}

//=================================================================================================

int BRepGraph::TopoView::FaceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveFaces();
}

//=================================================================================================

const BRepGraphInc::FaceDef& BRepGraph::TopoView::FaceOps::Definition(
  const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.Face(theFace);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_ShellId>& BRepGraph::TopoView::FaceOps::Shells(
  const BRepGraph_FaceId theFace) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().ShellsOfFaceRef(theFace);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompoundId>& BRepGraph::TopoView::FaceOps::Compounds(
  const BRepGraph_FaceId theFace) const
{
  const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
    myGraph->myData->myIncStorage.ReverseIndex().CompoundsOfFace(theFace);
  return aCompounds != nullptr ? *aCompounds : emptyVector<BRepGraph_CompoundId>();
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

  const BRepGraph_TriangulationRepId aRepId = aStorage.Face(theFace).TriangulationRepId;
  if (!aRepId.IsValid(aStorage.NbTriangulations()) || aStorage.TriangulationRep(aRepId).IsRemoved)
  {
    return BRepGraph_TriangulationRepId();
  }
  return aRepId;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::FaceOps::SameDomain(
  const BRepGraph_FaceId                        theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_SAME_DOMAIN_BLOCK_SIZE, theAllocator);
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

  for (BRepGraph_FaceIterator aFaceIt(*myGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId       anOtherFaceId = aFaceIt.CurrentId();
    const BRepGraphInc::FaceDef& anOtherFace   = aFaceIt.Current();
    if (anOtherFaceId != theFace && anOtherFace.SurfaceRepId == aFaceDef.SurfaceRepId)
    {
      aResult.Append(anOtherFaceId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::FaceOps::SharedEdges(
  const BRepGraph_FaceId                        theFaceA,
  const BRepGraph_FaceId                        theFaceB,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_SHARED_EDGE_BLOCK_SIZE, theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFaceA.IsValid(aStorage.NbFaces()) || !theFaceB.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const NCollection_Vector<BRepGraph_EdgeId> aFaceAEdges =
    collectFaceEdges(*myGraph, theFaceA, theAllocator);
  const NCollection_Vector<BRepGraph_EdgeId> aFaceBEdges =
    collectFaceEdges(*myGraph, theFaceB, theAllocator);
  NCollection_Map<BRepGraph_EdgeId> aFaceAEdgeSet;
  NCollection_Map<BRepGraph_EdgeId> anAddedEdges;

  for (const BRepGraph_EdgeId& anEdgeId : aFaceAEdges)
  {
    aFaceAEdgeSet.Add(anEdgeId);
  }

  for (const BRepGraph_EdgeId& anEdgeId : aFaceBEdges)
  {
    if (aFaceAEdgeSet.Contains(anEdgeId) && anAddedEdges.Add(anEdgeId))
    {
      aResult.Append(anEdgeId);
    }
  }
  return aResult;
}

//=================================================================================================

NCollection_Vector<BRepGraph_FaceId> BRepGraph::TopoView::FaceOps::Adjacent(
  const BRepGraph_FaceId                        theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_FaceId> aResult(THE_TOPOVIEW_FACE_ADJACENCY_BLOCK_SIZE,
                                               theAllocator);
  NCollection_Map<BRepGraph_FaceId>    aFaceSet;
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return aResult;
  }

  const NCollection_Vector<BRepGraph_EdgeId> anEdges =
    collectFaceEdges(*myGraph, theFace, theAllocator);
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  for (const BRepGraph_EdgeId& anEdgeId : anEdges)
  {
    const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfEdge(anEdgeId);
    if (aFaces == nullptr)
    {
      continue;
    }

    for (const BRepGraph_FaceId& anAdjacentFaceId : *aFaces)
    {
      if (anAdjacentFaceId == theFace)
      {
        continue;
      }

      const BRepGraphInc::FaceDef& anAdjacentFace = aStorage.Face(anAdjacentFaceId);
      if (anAdjacentFace.IsRemoved)
      {
        continue;
      }

      if (aFaceSet.Add(anAdjacentFaceId))
      {
        aResult.Append(anAdjacentFaceId);
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

  for (BRepGraph_RefsWireOfFace aWireIt(*myGraph, theFace); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireRef& aWireRef = myGraph->Refs().Wires().Entry(aWireIt.CurrentId());
    if (!aWireRef.IsRemoved && aWireRef.IsOuter)
    {
      return aWireRef.WireDefId;
    }
  }
  return BRepGraph_WireId();
}

//=================================================================================================

int BRepGraph::TopoView::EdgeOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbEdges();
}

//=================================================================================================

int BRepGraph::TopoView::EdgeOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveEdges();
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

NCollection_Vector<BRepGraph_EdgeId> BRepGraph::TopoView::EdgeOps::Adjacent(
  const BRepGraph_EdgeId                        theEdge,
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_EDGE_ADJACENCY_BLOCK_SIZE,
                                               theAllocator);
  const BRepGraphInc_Storage&          aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return aResult;
  }

  NCollection_Vector<BRepGraph_VertexId> aVertices(THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE,
                                                   theAllocator);
  NCollection_Map<BRepGraph_VertexId>    aSeenVertices;
  for (BRepGraph_DefsVertexOfEdge aVertexIt(*myGraph, theEdge); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertexId = aVertexIt.CurrentId();
    if (aSeenVertices.Add(aVertexId))
    {
      aVertices.Append(aVertexId);
    }
  }

  // Find adjacent edges via shared vertices.
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_Map<BRepGraph_EdgeId> anEdgeSet;
  for (const BRepGraph_VertexId& aVertexId : aVertices)
  {
    const NCollection_Vector<BRepGraph_EdgeId>* anEdges = aRevIdx.EdgesOfVertex(aVertexId);
    if (anEdges == nullptr)
    {
      continue;
    }

    for (const BRepGraph_EdgeId& anAdjacentEdgeId : *anEdges)
    {
      if (anAdjacentEdgeId == theEdge)
      {
        continue;
      }

      const BRepGraphInc::EdgeDef& anAdjacentEdge = aStorage.Edge(anAdjacentEdgeId);
      if (anAdjacentEdge.IsRemoved)
      {
        continue;
      }

      if (anEdgeSet.Add(anAdjacentEdgeId))
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

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
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

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  const BRepGraphInc::CoEdgeDef* aFirstMatch = nullptr;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.EdgeDefId != theEdge || aCoEdge.FaceDefId != theFace)
    {
      continue;
    }
    if (aFirstMatch == nullptr)
    {
      aFirstMatch = &aCoEdge;
    }
    if (aCoEdge.Orientation == theOrientation)
    {
      return &aCoEdge;
    }
  }
  return aFirstMatch;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph::TopoView::EdgeOps::FindCoEdgeId(const BRepGraph_EdgeId theEdge,
                                                              const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || !theFace.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_CoEdgeId();
  }

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.EdgeDefId == theEdge && aCoEdge.FaceDefId == theFace)
    {
      return aCoEdgeId;
    }
  }
  return BRepGraph_CoEdgeId();
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph::TopoView::EdgeOps::FindCoEdgeId(
  const BRepGraph_EdgeId   theEdge,
  const BRepGraph_FaceId   theFace,
  const TopAbs_Orientation theOrientation) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || !theFace.IsValid(aStorage.NbFaces()))
  {
    return BRepGraph_CoEdgeId();
  }

  const NCollection_Vector<BRepGraph_CoEdgeId>& aCoEdges =
    aStorage.ReverseIndex().CoEdgesOfEdgeRef(theEdge);
  BRepGraph_CoEdgeId aFirstMatch;
  for (const BRepGraph_CoEdgeId& aCoEdgeId : aCoEdges)
  {
    const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
    if (aCoEdge.EdgeDefId != theEdge || aCoEdge.FaceDefId != theFace)
    {
      continue;
    }
    if (!aFirstMatch.IsValid())
    {
      aFirstMatch = aCoEdgeId;
    }
    if (aCoEdge.Orientation == theOrientation)
    {
      return aCoEdgeId;
    }
  }
  return aFirstMatch;
}

//=================================================================================================

int BRepGraph::TopoView::VertexOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbVertices();
}

//=================================================================================================

int BRepGraph::TopoView::VertexOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveVertices();
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

int BRepGraph::TopoView::WireOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbWires();
}

//=================================================================================================

int BRepGraph::TopoView::WireOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveWires();
}

//=================================================================================================

const BRepGraphInc::WireDef& BRepGraph::TopoView::WireOps::Definition(
  const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.Wire(theWire);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_FaceId>& BRepGraph::TopoView::WireOps::Faces(
  const BRepGraph_WireId theWire) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().FacesOfWireRef(theWire);
}

//=================================================================================================

int BRepGraph::TopoView::ShellOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbShells();
}

//=================================================================================================

int BRepGraph::TopoView::ShellOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveShells();
}

//=================================================================================================

const BRepGraphInc::ShellDef& BRepGraph::TopoView::ShellOps::Definition(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.Shell(theShell);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_SolidId>& BRepGraph::TopoView::ShellOps::Solids(
  const BRepGraph_ShellId theShell) const
{
  return myGraph->myData->myIncStorage.ReverseIndex().SolidsOfShellRef(theShell);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompoundId>& BRepGraph::TopoView::ShellOps::Compounds(
  const BRepGraph_ShellId theShell) const
{
  const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
    myGraph->myData->myIncStorage.ReverseIndex().CompoundsOfShell(theShell);
  return aCompounds != nullptr ? *aCompounds : emptyVector<BRepGraph_CompoundId>();
}

//=================================================================================================

int BRepGraph::TopoView::SolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbSolids();
}

//=================================================================================================

int BRepGraph::TopoView::SolidOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveSolids();
}

//=================================================================================================

const BRepGraphInc::SolidDef& BRepGraph::TopoView::SolidOps::Definition(
  const BRepGraph_SolidId theSolid) const
{
  return myGraph->myData->myIncStorage.Solid(theSolid);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompSolidId>& BRepGraph::TopoView::SolidOps::CompSolids(
  const BRepGraph_SolidId theSolid) const
{
  const NCollection_Vector<BRepGraph_CompSolidId>* aCompSolids =
    myGraph->myData->myIncStorage.ReverseIndex().CompSolidsOfSolid(theSolid);
  return aCompSolids != nullptr ? *aCompSolids : emptyVector<BRepGraph_CompSolidId>();
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompoundId>& BRepGraph::TopoView::SolidOps::Compounds(
  const BRepGraph_SolidId theSolid) const
{
  const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
    myGraph->myData->myIncStorage.ReverseIndex().CompoundsOfSolid(theSolid);
  return aCompounds != nullptr ? *aCompounds : emptyVector<BRepGraph_CompoundId>();
}

//=================================================================================================

int BRepGraph::TopoView::CoEdgeOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCoEdges();
}

//=================================================================================================

int BRepGraph::TopoView::CoEdgeOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdges();
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef& BRepGraph::TopoView::CoEdgeOps::Definition(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return myGraph->myData->myIncStorage.CoEdge(theCoEdge);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_WireId>& BRepGraph::TopoView::CoEdgeOps::Wires(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const NCollection_Vector<BRepGraph_WireId>* aWires =
    myGraph->myData->myIncStorage.ReverseIndex().WiresOfCoEdge(theCoEdge);
  return aWires != nullptr ? *aWires : emptyVector<BRepGraph_WireId>();
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

int BRepGraph::TopoView::CompoundOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCompounds();
}

//=================================================================================================

int BRepGraph::TopoView::CompoundOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveCompounds();
}

//=================================================================================================

const BRepGraphInc::CompoundDef& BRepGraph::TopoView::CompoundOps::Definition(
  const BRepGraph_CompoundId theCompound) const
{
  return myGraph->myData->myIncStorage.Compound(theCompound);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompoundId>& BRepGraph::TopoView::CompoundOps::ParentCompounds(
  const BRepGraph_CompoundId theCompound) const
{
  const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
    myGraph->myData->myIncStorage.ReverseIndex().CompoundsOfCompound(theCompound);
  return aCompounds != nullptr ? *aCompounds : emptyVector<BRepGraph_CompoundId>();
}

//=================================================================================================

int BRepGraph::TopoView::CompSolidOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbCompSolids();
}

//=================================================================================================

int BRepGraph::TopoView::CompSolidOps::NbActive() const
{
  return myGraph->incStorage().NbActiveCompSolids();
}

//=================================================================================================

const BRepGraphInc::CompSolidDef& BRepGraph::TopoView::CompSolidOps::Definition(
  const BRepGraph_CompSolidId theCompSolid) const
{
  return myGraph->myData->myIncStorage.CompSolid(theCompSolid);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_CompoundId>& BRepGraph::TopoView::CompSolidOps::Compounds(
  const BRepGraph_CompSolidId theCompSolid) const
{
  const NCollection_Vector<BRepGraph_CompoundId>* aCompounds =
    myGraph->myData->myIncStorage.ReverseIndex().CompoundsOfCompSolid(theCompSolid);
  return aCompounds != nullptr ? *aCompounds : emptyVector<BRepGraph_CompoundId>();
}

//=================================================================================================

int BRepGraph::TopoView::ProductOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbProducts();
}

//=================================================================================================

int BRepGraph::TopoView::ProductOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveProducts();
}

//=================================================================================================

const BRepGraphInc::ProductDef& BRepGraph::TopoView::ProductOps::Definition(
  const BRepGraph_ProductId theProduct) const
{
  return myGraph->myData->myIncStorage.Product(theProduct);
}

//=================================================================================================

const NCollection_Vector<BRepGraph_OccurrenceId>& BRepGraph::TopoView::ProductOps::Instances(
  const BRepGraph_ProductId theProduct) const
{
  const NCollection_Vector<BRepGraph_OccurrenceId>* anOccurrences =
    myGraph->myData->myIncStorage.ReverseIndex().OccurrencesOfProduct(theProduct);
  return anOccurrences != nullptr ? *anOccurrences : emptyVector<BRepGraph_OccurrenceId>();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ProductOps::ShapeRoot(
  const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_NodeId();
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return BRepGraph_NodeId();
  }

  // Scan occurrences to find the first with a topology ChildDefId.
  for (int i = 0; i < aProductDef.OccurrenceRefIds.Length(); ++i)
  {
    const BRepGraph_OccurrenceRefId       aRefId = aProductDef.OccurrenceRefIds.Value(i);
    const BRepGraphInc::OccurrenceRef&    aRef   = aStorage.OccurrenceRef(aRefId);
    if (aRef.IsRemoved)
      continue;
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (anOccDef.IsRemoved)
      continue;
    if (!anOccDef.ChildDefId.IsValid()
        || BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildDefId.NodeKind))
      continue;

    const BRepGraphInc::BaseDef* aRoot = myGraph->Topo().Gen().TopoEntity(anOccDef.ChildDefId);
    if (aRoot != nullptr && !aRoot->IsRemoved)
    {
      return anOccDef.ChildDefId;
    }
  }
  return BRepGraph_NodeId();
}

bool BRepGraph::TopoView::ProductOps::IsAssembly(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return false;
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return false;
  }

  // Assembly if any active occurrence references a product child.
  for (int i = 0; i < aProductDef.OccurrenceRefIds.Length(); ++i)
  {
    const BRepGraph_OccurrenceRefId    aRefId = aProductDef.OccurrenceRefIds.Value(i);
    const BRepGraphInc::OccurrenceRef& aRef   = aStorage.OccurrenceRef(aRefId);
    if (aRef.IsRemoved)
      continue;
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (anOccDef.IsRemoved)
      continue;
    if (anOccDef.ChildDefId.NodeKind == BRepGraph_NodeId::Kind::Product)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraph::TopoView::ProductOps::IsPart(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return false;
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return false;
  }

  // Part if any active occurrence references a topology child.
  for (int i = 0; i < aProductDef.OccurrenceRefIds.Length(); ++i)
  {
    const BRepGraph_OccurrenceRefId    aRefId = aProductDef.OccurrenceRefIds.Value(i);
    const BRepGraphInc::OccurrenceRef& aRef   = aStorage.OccurrenceRef(aRefId);
    if (aRef.IsRemoved)
      continue;
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (anOccDef.IsRemoved)
      continue;
    if (anOccDef.ChildDefId.IsValid()
        && !BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildDefId.NodeKind))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::TopoView::ProductOps::ShapeRootNode(
  const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return BRepGraph_NodeId();
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return BRepGraph_NodeId();
  }

  // Find the first occurrence with a topology ChildDefId.
  for (int i = 0; i < aProductDef.OccurrenceRefIds.Length(); ++i)
  {
    const BRepGraph_OccurrenceRefId    aRefId = aProductDef.OccurrenceRefIds.Value(i);
    const BRepGraphInc::OccurrenceRef& aRef   = aStorage.OccurrenceRef(aRefId);
    if (aRef.IsRemoved)
      continue;
    const BRepGraphInc::OccurrenceDef& anOccDef = aStorage.Occurrence(aRef.OccurrenceDefId);
    if (anOccDef.IsRemoved)
      continue;
    if (anOccDef.ChildDefId.IsValid()
        && !BRepGraph_NodeId::IsAssemblyKind(anOccDef.ChildDefId.NodeKind))
    {
      return anOccDef.ChildDefId;
    }
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

int BRepGraph::TopoView::ProductOps::NbComponents(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return 0;
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return 0;
  }

  int aCount = 0;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    if (!aStorage.Occurrence(anOccRef.OccurrenceDefId).IsRemoved)
    {
      ++aCount;
    }
  }
  return aCount;
}

//=================================================================================================

BRepGraph_OccurrenceId BRepGraph::TopoView::ProductOps::Component(
  const BRepGraph_ProductId theProduct,
  const int                 theComponentIdx) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()) || theComponentIdx < 0)
  {
    return BRepGraph_OccurrenceId();
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  if (aProductDef.IsRemoved)
  {
    return BRepGraph_OccurrenceId();
  }

  int anActiveIndex = 0;
  for (BRepGraph_RefsOccurrenceOfProduct anOccIt(*myGraph, theProduct); anOccIt.More();
       anOccIt.Next())
  {
    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccIt.CurrentId());
    if (aStorage.Occurrence(anOccRef.OccurrenceDefId).IsRemoved)
    {
      continue;
    }

    if (anActiveIndex == theComponentIdx)
    {
      return anOccRef.OccurrenceDefId;
    }
    ++anActiveIndex;
  }
  return BRepGraph_OccurrenceId();
}

//=================================================================================================

int BRepGraph::TopoView::OccurrenceOps::Nb() const
{
  return myGraph->myData->myIncStorage.NbOccurrences();
}

//=================================================================================================

int BRepGraph::TopoView::OccurrenceOps::NbActive() const
{
  return myGraph->myData->myIncStorage.NbActiveOccurrences();
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
  if (anOccurrence.IsRemoved || !anOccurrence.ChildDefId.IsValid()
      || anOccurrence.ChildDefId.NodeKind != BRepGraph_NodeId::Kind::Product)
  {
    return BRepGraph_ProductId();
  }
  const BRepGraph_ProductId aProductId = BRepGraph_ProductId::FromNodeId(anOccurrence.ChildDefId);
  if (!aProductId.IsValid(aStorage.NbProducts()) || aStorage.Product(aProductId).IsRemoved)
  {
    return BRepGraph_ProductId();
  }
  return aProductId;
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
  if (anOccurrence.IsRemoved)
  {
    return BRepGraph_ProductId();
  }

  // Find the OccurrenceRef that owns this OccurrenceDef to get ParentId.
  const int aNbOccRefs = aStorage.NbOccurrenceRefs();
  for (int i = 0; i < aNbOccRefs; ++i)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(BRepGraph_OccurrenceRefId(i));
    if (aRef.IsRemoved || aRef.OccurrenceDefId != theOccurrence)
      continue;

    if (aRef.ParentId.NodeKind != BRepGraph_NodeId::Kind::Product)
      return BRepGraph_ProductId();

    const BRepGraph_ProductId aParentProduct = BRepGraph_ProductId::FromNodeId(aRef.ParentId);
    if (!aParentProduct.IsValid(aStorage.NbProducts())
        || aStorage.Product(aParentProduct).IsRemoved)
    {
      return BRepGraph_ProductId();
    }
    return aParentProduct;
  }
  return BRepGraph_ProductId();
}

//=================================================================================================

TopLoc_Location BRepGraph::TopoView::OccurrenceOps::OccurrenceLocation(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
    return TopLoc_Location();

  // Placement is now on OccurrenceRef::LocalLocation.
  // Find the OccurrenceRef that owns this OccurrenceDef.
  const int aNbOccRefs = aStorage.NbOccurrenceRefs();
  for (int i = 0; i < aNbOccRefs; ++i)
  {
    const BRepGraphInc::OccurrenceRef& aRef = aStorage.OccurrenceRef(BRepGraph_OccurrenceRefId(i));
    if (!aRef.IsRemoved && aRef.OccurrenceDefId == theOccurrence)
    {
      return aRef.LocalLocation;
    }
  }
  return TopLoc_Location();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbSurfaces() const
{
  return myGraph->myData->myIncStorage.NbSurfaces();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbCurves3D() const
{
  return myGraph->myData->myIncStorage.NbCurves3D();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbCurves2D() const
{
  return myGraph->myData->myIncStorage.NbCurves2D();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbActiveSurfaces() const
{
  return myGraph->myData->myIncStorage.NbActiveSurfaces();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbActiveCurves3D() const
{
  return myGraph->myData->myIncStorage.NbActiveCurves3D();
}

//=================================================================================================

int BRepGraph::TopoView::GeometryOps::NbActiveCurves2D() const
{
  return myGraph->myData->myIncStorage.NbActiveCurves2D();
}

//=================================================================================================

const BRepGraphInc::SurfaceRep& BRepGraph::TopoView::GeometryOps::SurfaceRep(
  const BRepGraph_SurfaceRepId theRep) const
{
  return myGraph->myData->myIncStorage.SurfaceRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Curve3DRep& BRepGraph::TopoView::GeometryOps::Curve3DRep(
  const BRepGraph_Curve3DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Curve3DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Curve2DRep& BRepGraph::TopoView::GeometryOps::Curve2DRep(
  const BRepGraph_Curve2DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Curve2DRep(theRep);
}

const BRepGraphInc::BaseDef* BRepGraph::TopoView::GenOps::TopoEntity(
  const BRepGraph_NodeId theId) const
{
  return myGraph->topoEntity(theId);
}

//=================================================================================================

int BRepGraph::TopoView::GenOps::NbNodes() const
{
  const BRepGraphInc_Storage& aS = myGraph->myData->myIncStorage;
  return aS.NbSolids() + aS.NbShells() + aS.NbFaces() + aS.NbWires() + aS.NbCoEdges() + aS.NbEdges()
         + aS.NbVertices() + aS.NbCompounds() + aS.NbCompSolids() + aS.NbProducts()
         + aS.NbOccurrences();
}

//=================================================================================================
bool BRepGraph::TopoView::GenOps::IsRemoved(const BRepGraph_NodeId theNode) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

