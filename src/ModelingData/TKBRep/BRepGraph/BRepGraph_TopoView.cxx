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
#include <BRepGraph_Iterator.hxx>
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
  const BRepGraphInc_Storage&                   theStorage,
  const BRepGraph_FaceId                        theFace,
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  NCollection_Vector<BRepGraph_EdgeId> aResult(THE_TOPOVIEW_FACE_EDGE_BLOCK_SIZE, theAllocator);
  if (!theFace.IsValid(theStorage.NbFaces()))
    return aResult;

  const BRepGraphInc::FaceDef& aFace = theStorage.Face(theFace);
  NCollection_PackedMap<int>   anEdgeSet;
  for (int aWireRefIdx = 0; aWireRefIdx < aFace.WireRefIds.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aFace.WireRefIds.Value(aWireRefIdx));
    if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(theStorage.NbWires()))
      continue;

    const BRepGraphInc::WireDef& aWire = theStorage.Wire(aWireRef.WireDefId);
    for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < aWire.CoEdgeRefIds.Length(); ++aCoEdgeRefIdx)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef =
        theStorage.CoEdgeRef(aWire.CoEdgeRefIds.Value(aCoEdgeRefIdx));
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
        continue;

      const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(theStorage.NbEdges()))
        continue;

      if (anEdgeSet.Add(aCoEdge.EdgeDefId.Index))
        aResult.Append(aCoEdge.EdgeDefId);
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

}

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

  const NCollection_Vector<BRepGraph_EdgeId> aFaceAEdges = collectFaceEdges(aStorage, theFaceA, theAllocator);
  const NCollection_Vector<BRepGraph_EdgeId> aFaceBEdges = collectFaceEdges(aStorage, theFaceB, theAllocator);
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

  const NCollection_Vector<BRepGraph_EdgeId> anEdges = collectFaceEdges(aStorage, theFace, theAllocator);
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

  // Collect vertex IDs from the edge definition inline.
  const BRepGraphInc::EdgeDef& aDef          = aStorage.Edge(theEdge);
  const int                    aNbVertexRefs = aStorage.NbVertexRefs();
  NCollection_Vector<BRepGraph_VertexId> aVertices(THE_TOPOVIEW_EDGE_VERTEX_BLOCK_SIZE, theAllocator);
  NCollection_PackedMap<int>             aSeenRefIds;

  const auto aProcessRefId = [&aVertices, &aSeenRefIds, &aStorage, aNbVertexRefs](
                               const BRepGraph_VertexRefId theRefId) {
    if (!theRefId.IsValid(aNbVertexRefs) || !aSeenRefIds.Add(theRefId.Index))
      return;
    const BRepGraphInc::VertexRef& aVertexRef = aStorage.VertexRef(theRefId);
    if (!aVertexRef.IsRemoved)
      aVertices.Append(aVertexRef.VertexDefId);
  };

  aProcessRefId(aDef.StartVertexRefId);
  aProcessRefId(aDef.EndVertexRefId);
  for (int aRefIdx = 0; aRefIdx < aDef.InternalVertexRefIds.Length(); ++aRefIdx)
    aProcessRefId(aDef.InternalVertexRefIds.Value(aRefIdx));

  // Find adjacent edges via shared vertices.
  const BRepGraphInc_ReverseIndex& aRevIdx = aStorage.ReverseIndex();
  NCollection_PackedMap<int>       anEdgeSet;
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

  const BRepGraphInc::BaseDef* aRoot = myGraph->Topo().Gen().TopoEntity(aShapeRoot);
  if (aRoot == nullptr || aRoot->IsRemoved)
  {
    return BRepGraph_NodeId();
  }
  return aShapeRoot;
}

//=================================================================================================

NCollection_Vector<BRepGraph_ProductId> BRepGraph::TopoView::ProductOps::RootProducts(
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  constexpr int THE_ROOT_PRODUCTS_BLOCK_SIZE = 4;
  NCollection_Vector<BRepGraph_ProductId> aResult(THE_ROOT_PRODUCTS_BLOCK_SIZE, theAllocator);
  const BRepGraphInc_Storage& aStorage    = myGraph->myData->myIncStorage;
  const int                   aNbProducts = aStorage.NbProducts();

  NCollection_Map<int> aReferencedProducts(1, theAllocator);
  for (int anOccIdx = 0; anOccIdx < aStorage.NbOccurrences(); ++anOccIdx)
  {
    const BRepGraphInc::OccurrenceDef& anOcc = aStorage.Occurrence(BRepGraph_OccurrenceId(anOccIdx));
    if (!anOcc.IsRemoved && anOcc.ProductDefId.IsValid())
    {
      aReferencedProducts.Add(anOcc.ProductDefId.Index);
    }
  }

  for (int aProdIdx = 0; aProdIdx < aNbProducts; ++aProdIdx)
  {
    const BRepGraphInc::ProductDef& aProduct = aStorage.Product(BRepGraph_ProductId(aProdIdx));
    if (aProduct.IsRemoved)
    {
      continue;
    }
    if (!aReferencedProducts.Contains(aProdIdx))
    {
      aResult.Append(BRepGraph_ProductId(aProdIdx));
    }
  }
  return aResult;
}

//=================================================================================================

bool BRepGraph::TopoView::ProductOps::IsAssembly(const BRepGraph_ProductId theProduct) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theProduct.IsValid(aStorage.NbProducts()))
  {
    return false;
  }

  const BRepGraphInc::ProductDef& aProductDef = aStorage.Product(theProduct);
  return !aProductDef.IsRemoved && !aProductDef.ShapeRootId.IsValid();
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
  return !aProductDef.IsRemoved && aProductDef.ShapeRootId.IsValid();
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
  return aProductDef.ShapeRootId;
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
  for (int anIdx = 0; anIdx < aProductDef.OccurrenceRefIds.Length(); ++anIdx)
  {
    const BRepGraph_OccurrenceRefId anOccRefId = aProductDef.OccurrenceRefIds.Value(anIdx);
    if (!anOccRefId.IsValid(aStorage.NbOccurrences()))
    {
      continue;
    }

    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccRefId);
    if (anOccRef.IsRemoved || !anOccRef.OccurrenceDefId.IsValid(aStorage.NbOccurrences()))
    {
      continue;
    }

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
  const int                theComponentIdx) const
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
  for (int anIdx = 0; anIdx < aProductDef.OccurrenceRefIds.Length(); ++anIdx)
  {
    const BRepGraph_OccurrenceRefId anOccRefId = aProductDef.OccurrenceRefIds.Value(anIdx);
    if (!anOccRefId.IsValid(aStorage.NbOccurrences()))
    {
      continue;
    }

    const BRepGraphInc::OccurrenceRef& anOccRef = aStorage.OccurrenceRef(anOccRefId);
    if (anOccRef.IsRemoved || !anOccRef.OccurrenceDefId.IsValid(aStorage.NbOccurrences()))
    {
      continue;
    }

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

//=================================================================================================

TopLoc_Location BRepGraph::TopoView::OccurrenceOps::OccurrenceLocation(
  const BRepGraph_OccurrenceId theOccurrence) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theOccurrence.IsValid(aStorage.NbOccurrences()))
    return TopLoc_Location();

  TopLoc_Location        aGlobal      = aStorage.Occurrence(theOccurrence).Placement;
  BRepGraph_OccurrenceId aParentOccId = aStorage.Occurrence(theOccurrence).ParentOccurrenceDefId;

  const int aNbOccurrences = aStorage.NbOccurrences();
  int       aSteps         = 0;

  while (aParentOccId.IsValid(aNbOccurrences) && aSteps < aNbOccurrences)
  {
    ++aSteps;
    const BRepGraphInc::OccurrenceDef& aParentOcc = aStorage.Occurrence(aParentOccId);
    if (aParentOcc.IsRemoved)
      break;
    aGlobal      = aParentOcc.Placement * aGlobal;
    aParentOccId = aParentOcc.ParentOccurrenceDefId;
  }

  return aGlobal;
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

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbTriangulations() const
{
  return myGraph->myData->myIncStorage.NbTriangulations();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbPolygons3D() const
{
  return myGraph->myData->myIncStorage.NbPolygons3D();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbPolygons2D() const
{
  return myGraph->myData->myIncStorage.NbPolygons2D();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbPolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbPolygonsOnTri();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbActiveTriangulations() const
{
  return myGraph->myData->myIncStorage.NbActiveTriangulations();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbActivePolygons3D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons3D();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbActivePolygons2D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons2D();
}

//=================================================================================================

int BRepGraph::TopoView::PolyOps::NbActivePolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbActivePolygonsOnTri();
}

//=================================================================================================

const BRepGraphInc::TriangulationRep& BRepGraph::TopoView::PolyOps::TriangulationRep(
  const BRepGraph_TriangulationRepId theRep) const
{
  return myGraph->myData->myIncStorage.TriangulationRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Polygon3DRep& BRepGraph::TopoView::PolyOps::Polygon3DRep(
  const BRepGraph_Polygon3DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon3DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Polygon2DRep& BRepGraph::TopoView::PolyOps::Polygon2DRep(
  const BRepGraph_Polygon2DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon2DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::PolygonOnTriRep& BRepGraph::TopoView::PolyOps::PolygonOnTriRep(
  const BRepGraph_PolygonOnTriRepId theRep) const
{
  return myGraph->myData->myIncStorage.PolygonOnTriRep(theRep);
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
  return aS.NbSolids() + aS.NbShells() + aS.NbFaces() + aS.NbWires() + aS.NbCoEdges()
         + aS.NbEdges() + aS.NbVertices() + aS.NbCompounds() + aS.NbCompSolids()
         + aS.NbProducts() + aS.NbOccurrences();
}

//=================================================================================================
bool BRepGraph::TopoView::GenOps::IsRemoved(const BRepGraph_NodeId theNode) const
{
  const BRepGraphInc::BaseDef* aDef = myGraph->topoEntity(theNode);
  if (aDef == nullptr)
    return false;
  return aDef->IsRemoved;
}

