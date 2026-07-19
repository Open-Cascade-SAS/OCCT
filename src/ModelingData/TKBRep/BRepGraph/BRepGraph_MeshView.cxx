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

#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphInc_Storage.hxx>

namespace
{
BRepGraph_CacheMesh& getCacheMesh(BRepGraph* theGraph)
{
  return *theGraph->CacheRegistry().Ensure<BRepGraph_CacheMesh>();
}
} // namespace

//=================================================================================================
// PolyOps
//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbFaceTriangulations() const
{
  return myGraph->myData->myIncStorage.NbFaceTriangulations();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbEdgePolygons3D() const
{
  return myGraph->myData->myIncStorage.NbEdgePolygons3D();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbCoEdgePolygons2D() const
{
  return myGraph->myData->myIncStorage.NbCoEdgePolygons2D();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbCoEdgePolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbCoEdgePolygonsOnTri();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbActiveTriangulations() const
{
  return myGraph->myData->myIncStorage.NbActiveFaceTriangulations();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbActivePolygons3D() const
{
  return myGraph->myData->myIncStorage.NbActiveEdgePolygons3D();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbActivePolygons2D() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdgePolygons2D();
}

//=================================================================================================

uint32_t BRepGraph::MeshView::PolyOps::NbActivePolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbActiveCoEdgePolygonsOnTri();
}

//=================================================================================================
// Cache.FaceOps
//=================================================================================================

bool BRepGraph::MeshView::CacheView::FaceOps::Has(const BRepGraph_FaceId theFace) const
{
  return Entry(theFace) != nullptr;
}

//=================================================================================================

static const occ::handle<Poly_Triangulation> THE_NULL_TRIANGULATION_MV;

const occ::handle<Poly_Triangulation>& BRepGraph::MeshView::CacheView::FaceOps::Triangulation(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraph_CacheMesh::FaceMeshEntry* anEntry = Entry(theFace);
  if (anEntry == nullptr || anEntry->Triangulation.IsNull())
  {
    return THE_NULL_TRIANGULATION_MV;
  }
  return anEntry->Triangulation;
}

//=================================================================================================

const BRepGraph_CacheMesh::FaceMeshEntry* BRepGraph::MeshView::CacheView::FaceOps::Entry(
  const BRepGraph_FaceId theFace) const
{
  return getCacheMesh(myGraph).FindFaceMesh(theFace);
}

//=================================================================================================
// Cache.EdgeOps
//=================================================================================================

bool BRepGraph::MeshView::CacheView::EdgeOps::Has(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_CacheMesh::EdgeMeshEntry* anEntry = Entry(theEdge);
  return anEntry != nullptr && !anEntry->Polygon3D.IsNull();
}

//=================================================================================================

static const occ::handle<Poly_Polygon3D> THE_NULL_POLYGON3D_MV;

const occ::handle<Poly_Polygon3D>& BRepGraph::MeshView::CacheView::EdgeOps::Polygon3D(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_CacheMesh::EdgeMeshEntry* anEntry = Entry(theEdge);
  if (anEntry == nullptr || anEntry->Polygon3D.IsNull())
  {
    return THE_NULL_POLYGON3D_MV;
  }
  return anEntry->Polygon3D;
}

//=================================================================================================

const BRepGraph_CacheMesh::EdgeMeshEntry* BRepGraph::MeshView::CacheView::EdgeOps::Entry(
  const BRepGraph_EdgeId theEdge) const
{
  return getCacheMesh(myGraph).FindEdgeMesh(theEdge);
}

//=================================================================================================
// Cache.CoEdgeOps
//=================================================================================================

bool BRepGraph::MeshView::CacheView::CoEdgeOps::Has(const BRepGraph_CoEdgeId theCoEdge) const
{
  return getCacheMesh(myGraph).HasCoEdgeMesh(theCoEdge);
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph::MeshView::CacheView::CoEdgeOps::
  FindPolygon2D(const BRepGraph_CoEdgeId theCoEdge) const
{
  return getCacheMesh(myGraph).FindCoEdgePolygon2D(theCoEdge);
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph::MeshView::CacheView::CoEdgeOps::
  FindPolygonOnTri(const BRepGraph_CoEdgeId theCoEdge) const
{
  return getCacheMesh(myGraph).FindCoEdgePolygonOnTri(theCoEdge);
}

//=================================================================================================

const BRepGraph_CacheMesh::CoEdgeMeshEntry* BRepGraph::MeshView::CacheView::CoEdgeOps::FindRaw(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  return getCacheMesh(myGraph).findCoEdgeEntryRaw(BRepGraph_CacheMesh::DefaultDisplaySlot,
                                                  theCoEdge);
}

//=================================================================================================
// Persistent.FaceOps
//=================================================================================================

bool BRepGraph::MeshView::PersistentView::FaceOps::Has(const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(theFace))
  {
    return false;
  }
  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  return aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations())
         && !aStorage.IsRemoved(aFace.TriangulationRepId);
}

//=================================================================================================

const occ::handle<Poly_Triangulation>& BRepGraph::MeshView::PersistentView::FaceOps::Triangulation(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()) || aStorage.IsRemoved(theFace))
  {
    return THE_NULL_TRIANGULATION_MV;
  }
  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  if (!aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations())
      || aStorage.IsRemoved(aFace.TriangulationRepId))
  {
    return THE_NULL_TRIANGULATION_MV;
  }
  return aStorage.FaceTriangulationRep(aFace.TriangulationRepId).Triangulation;
}

//=================================================================================================
// Persistent.EdgeOps
//=================================================================================================

bool BRepGraph::MeshView::PersistentView::EdgeOps::Has(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || aStorage.IsRemoved(theEdge))
  {
    return false;
  }
  const BRepGraph_EdgePolygon3DRepId aRepId = aStorage.Edge(theEdge).Polygon3DRepId;
  return aRepId.IsValid(aStorage.NbEdgePolygons3D()) && !aStorage.IsRemoved(aRepId);
}

//=================================================================================================

const occ::handle<Poly_Polygon3D>& BRepGraph::MeshView::PersistentView::EdgeOps::Polygon3D(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()) || aStorage.IsRemoved(theEdge))
  {
    return THE_NULL_POLYGON3D_MV;
  }
  const BRepGraph_EdgePolygon3DRepId aRepId = aStorage.Edge(theEdge).Polygon3DRepId;
  if (!aRepId.IsValid(aStorage.NbEdgePolygons3D()) || aStorage.IsRemoved(aRepId))
  {
    return THE_NULL_POLYGON3D_MV;
  }
  return aStorage.EdgePolygon3DRep(aRepId).Polygon;
}

//=================================================================================================

bool BRepGraph::MeshView::PersistentView::EdgeOps::HasPolygonOnTriangulation(
  const BRepGraph_EdgeId theEdge,
  const BRepGraph_FaceId theFace) const
{
  const BRepGraph_CoEdgeId aCoEdgeId =
    BRepGraph_Tool::Edge::FindCoEdgeId(*myGraph, theEdge, theFace);
  if (!aCoEdgeId.IsValid())
  {
    return false;
  }
  const BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.CoEdge(aCoEdgeId);
  return aCoEdge.PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
         && !aStorage.IsRemoved(aCoEdge.PolygonOnTriRepId);
}

//=================================================================================================

static const occ::handle<Poly_PolygonOnTriangulation> THE_NULL_POLYGON_ON_TRI_MV;

const occ::handle<Poly_PolygonOnTriangulation>& BRepGraph::MeshView::PersistentView::EdgeOps::
  PolygonOnTriangulation(const BRepGraph_EdgeId theEdge, const BRepGraph_FaceId theFace) const
{
  const BRepGraph_CoEdgeId aCoEdgeId =
    BRepGraph_Tool::Edge::FindCoEdgeId(*myGraph, theEdge, theFace);
  if (!aCoEdgeId.IsValid())
  {
    return THE_NULL_POLYGON_ON_TRI_MV;
  }
  const BRepGraphInc_Storage&    aStorage = myGraph->myData->myIncStorage;
  const BRepGraphInc::CoEdgeDef& aCoEdge  = aStorage.CoEdge(aCoEdgeId);
  if (!aCoEdge.PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
      || aStorage.IsRemoved(aCoEdge.PolygonOnTriRepId))
  {
    return THE_NULL_POLYGON_ON_TRI_MV;
  }
  return aStorage.CoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId).Polygon;
}

//=================================================================================================
// Persistent.CoEdgeOps
//=================================================================================================

bool BRepGraph::MeshView::PersistentView::CoEdgeOps::Has(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return false;
  }
  const BRepGraph_CoEdgePolygon2DRepId aRepId = aStorage.CoEdge(theCoEdge).Polygon2DRepId;
  return aRepId.IsValid(aStorage.NbCoEdgePolygons2D()) && !aStorage.IsRemoved(aRepId);
}

//=================================================================================================

static const occ::handle<Poly_Polygon2D> THE_NULL_POLYGON2D_MV;

const occ::handle<Poly_Polygon2D>& BRepGraph::MeshView::PersistentView::CoEdgeOps::PolygonOnSurface(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return THE_NULL_POLYGON2D_MV;
  }
  const BRepGraph_CoEdgePolygon2DRepId aRepId = aStorage.CoEdge(theCoEdge).Polygon2DRepId;
  if (!aRepId.IsValid(aStorage.NbCoEdgePolygons2D()) || aStorage.IsRemoved(aRepId))
  {
    return THE_NULL_POLYGON2D_MV;
  }
  return aStorage.CoEdgePolygon2DRep(aRepId).Polygon;
}

//=================================================================================================

bool BRepGraph::MeshView::PersistentView::CoEdgeOps::HasPolygonOnTriangulation(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return false;
  }
  const BRepGraph_CoEdgePolygonOnTriRepId aRepId = aStorage.CoEdge(theCoEdge).PolygonOnTriRepId;
  return aRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri()) && !aStorage.IsRemoved(aRepId);
}

//=================================================================================================

const occ::handle<Poly_PolygonOnTriangulation>& BRepGraph::MeshView::PersistentView::CoEdgeOps::
  PolygonOnTriangulation(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return THE_NULL_POLYGON_ON_TRI_MV;
  }
  const BRepGraph_CoEdgePolygonOnTriRepId aRepId = aStorage.CoEdge(theCoEdge).PolygonOnTriRepId;
  if (!aRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri()) || aStorage.IsRemoved(aRepId))
  {
    return THE_NULL_POLYGON_ON_TRI_MV;
  }
  return aStorage.CoEdgePolygonOnTriRep(aRepId).Polygon;
}

//=================================================================================================
// Effective.FaceOps (cache-first, persistent fallback)
//=================================================================================================

bool BRepGraph::MeshView::EffectiveView::FaceOps::Has(const BRepGraph_FaceId theFace) const
{
  const BRepGraph_CacheMesh::SlotId aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  return getCacheMesh(myGraph).FindFaceMesh(aSlot, theFace) != nullptr
         || myGraph->Mesh().Persistent().Faces().Has(theFace);
}

//=================================================================================================

const occ::handle<Poly_Triangulation>& BRepGraph::MeshView::EffectiveView::FaceOps::Triangulation(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraph_CacheMesh::SlotId         aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::FaceMeshEntry* anEntry =
    getCacheMesh(myGraph).FindFaceMesh(aSlot, theFace);
  if (anEntry != nullptr && !anEntry->Triangulation.IsNull())
  {
    return anEntry->Triangulation;
  }
  return myGraph->Mesh().Persistent().Faces().Triangulation(theFace);
}

//=================================================================================================
// Effective.EdgeOps (cache-first, persistent fallback)
//=================================================================================================

bool BRepGraph::MeshView::EffectiveView::EdgeOps::Has(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_CacheMesh::SlotId aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  return getCacheMesh(myGraph).FindEdgeMesh(aSlot, theEdge) != nullptr
         || myGraph->Mesh().Persistent().Edges().Has(theEdge);
}

//=================================================================================================

const occ::handle<Poly_Polygon3D>& BRepGraph::MeshView::EffectiveView::EdgeOps::Polygon3D(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_CacheMesh::SlotId         aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::EdgeMeshEntry* anEntry =
    getCacheMesh(myGraph).FindEdgeMesh(aSlot, theEdge);
  if (anEntry != nullptr && !anEntry->Polygon3D.IsNull())
  {
    return anEntry->Polygon3D;
  }
  return myGraph->Mesh().Persistent().Edges().Polygon3D(theEdge);
}

//=================================================================================================
// Effective.CoEdgeOps (cache-first, persistent fallback)
//=================================================================================================

bool BRepGraph::MeshView::EffectiveView::CoEdgeOps::Has(const BRepGraph_CoEdgeId theCoEdge) const
{
  return HasPolygonOnSurface(theCoEdge) || HasPolygonOnTriangulation(theCoEdge);
}

//=================================================================================================

bool BRepGraph::MeshView::EffectiveView::CoEdgeOps::HasPolygonOnSurface(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_CacheMesh::SlotId           aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::CoEdgeMeshEntry* anEntry =
    getCacheMesh(myGraph).FindCoEdgePolygon2D(aSlot, theCoEdge);
  if (anEntry != nullptr && !anEntry->Polygon2D.IsNull())
  {
    return true;
  }
  return myGraph->Mesh().Persistent().CoEdges().Has(theCoEdge);
}

//=================================================================================================

const occ::handle<Poly_Polygon2D>& BRepGraph::MeshView::EffectiveView::CoEdgeOps::PolygonOnSurface(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_CacheMesh::SlotId           aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::CoEdgeMeshEntry* anEntry =
    getCacheMesh(myGraph).FindCoEdgePolygon2D(aSlot, theCoEdge);
  if (anEntry != nullptr && !anEntry->Polygon2D.IsNull())
  {
    return anEntry->Polygon2D;
  }
  return myGraph->Mesh().Persistent().CoEdges().PolygonOnSurface(theCoEdge);
}

//=================================================================================================

bool BRepGraph::MeshView::EffectiveView::CoEdgeOps::HasPolygonOnTriangulation(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_CacheMesh::SlotId           aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::CoEdgeMeshEntry* anEntry =
    getCacheMesh(myGraph).FindCoEdgePolygonOnTri(aSlot, theCoEdge);
  if (anEntry != nullptr && !anEntry->PolygonsOnTri.IsEmpty())
  {
    return true;
  }
  return myGraph->Mesh().Persistent().CoEdges().HasPolygonOnTriangulation(theCoEdge);
}

//=================================================================================================

const occ::handle<Poly_PolygonOnTriangulation>& BRepGraph::MeshView::EffectiveView::CoEdgeOps::
  PolygonOnTriangulation(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_CacheMesh::SlotId           aSlot = getCacheMesh(myGraph).ActiveDisplaySlot();
  const BRepGraph_CacheMesh::CoEdgeMeshEntry* anEntry =
    getCacheMesh(myGraph).FindCoEdgePolygonOnTri(aSlot, theCoEdge);
  if (anEntry != nullptr && !anEntry->PolygonsOnTri.IsEmpty())
  {
    const occ::handle<Poly_PolygonOnTriangulation>& aPoly = anEntry->PolygonsOnTri.Value(0);
    if (!aPoly.IsNull())
    {
      return aPoly;
    }
  }
  return myGraph->Mesh().Persistent().CoEdges().PolygonOnTriangulation(theCoEdge);
}

//=================================================================================================
// Editor.FaceOps (cache writes)
//=================================================================================================

void BRepGraph::MeshView::EditorView::FaceOps::SetCachedTriangulation(
  const BRepGraph_FaceId                 theFace,
  const occ::handle<Poly_Triangulation>& theTriangulation)
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return;
  }
  BRepGraph_CacheMesh&                aCacheMesh = getCacheMesh(myGraph);
  BRepGraph_CacheMesh::FaceMeshEntry& anEntry    = aCacheMesh.ChangeFaceMesh(theFace);
  anEntry.Triangulation                          = theTriangulation;
  aCacheMesh.BindFresh(anEntry, theFace);
  aCacheMesh.BumpFaceMeshGeneration(theFace);
}

//=================================================================================================

void BRepGraph::MeshView::EditorView::FaceOps::Clear(const BRepGraph_FaceId theFace)
{
  BRepGraph_CacheMesh& aCacheMesh = getCacheMesh(myGraph);

  BRepGraph_CacheMesh::FaceMeshEntry& anEntry = aCacheMesh.ChangeFaceMesh(theFace);
  anEntry.ClearRepresentation();
  aCacheMesh.BindFresh(anEntry, theFace);
  aCacheMesh.BumpFaceMeshGeneration(theFace);

  // Do NOT touch coedge entries. Polygon2D stays valid.
  // Coedge PolygonOnTri staleness happens naturally via FaceMeshGeneration mismatch.
}

//=================================================================================================
// Editor.EdgeOps (cache writes)
//=================================================================================================

void BRepGraph::MeshView::EditorView::EdgeOps::SetCachedPolygon3D(
  const BRepGraph_EdgeId             theEdge,
  const occ::handle<Poly_Polygon3D>& thePolygon3D)
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return;
  }
  BRepGraph_CacheMesh&                aCacheMesh = getCacheMesh(myGraph);
  BRepGraph_CacheMesh::EdgeMeshEntry& anEntry    = aCacheMesh.ChangeEdgeMesh(theEdge);
  anEntry.Polygon3D                              = thePolygon3D;
  aCacheMesh.BindFresh(anEntry, theEdge);
}

//=================================================================================================

void BRepGraph::MeshView::EditorView::EdgeOps::Clear(const BRepGraph_EdgeId theEdge)
{
  getCacheMesh(myGraph).ClearEdgeMesh(theEdge);
}

//=================================================================================================
// Editor.CoEdgeOps (cache writes)
//=================================================================================================

void BRepGraph::MeshView::EditorView::CoEdgeOps::AppendCachedPolygonOnTri(
  const BRepGraph_CoEdgeId                        theCoEdge,
  const occ::handle<Poly_PolygonOnTriangulation>& thePolygonOnTri)
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || thePolygonOnTri.IsNull())
  {
    return;
  }
  BRepGraph_CacheMesh&                  aCacheMesh = getCacheMesh(myGraph);
  BRepGraph_CacheMesh::CoEdgeMeshEntry& anEntry    = aCacheMesh.ChangeCoEdgeMesh(theCoEdge);
  anEntry.PolygonsOnTri.Append(thePolygonOnTri);
  aCacheMesh.BindFresh(anEntry, theCoEdge);
}

//=================================================================================================

void BRepGraph::MeshView::EditorView::CoEdgeOps::SetCachedPolygon2D(
  const BRepGraph_CoEdgeId           theCoEdge,
  const occ::handle<Poly_Polygon2D>& thePolygon2D)
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return;
  }
  BRepGraph_CacheMesh&                  aCacheMesh = getCacheMesh(myGraph);
  BRepGraph_CacheMesh::CoEdgeMeshEntry& anEntry    = aCacheMesh.ChangeCoEdgeMesh(theCoEdge);
  anEntry.Polygon2D                                = thePolygon2D;
  aCacheMesh.BindFresh(anEntry, theCoEdge);
}

//=================================================================================================

void BRepGraph::MeshView::EditorView::CoEdgeOps::Clear(const BRepGraph_CoEdgeId theCoEdge)
{
  getCacheMesh(myGraph).ClearCoEdgeMesh(theCoEdge);
}

//=================================================================================================

void BRepGraph::MeshView::EditorView::PromoteToPersistent()
{
  for (BRepGraph_FaceId aFaceId = myGraph->Topo().Faces().StartId();
       aFaceId < myGraph->Topo().Faces().EndId();
       ++aFaceId)
  {
    if (aFaceId.IsRemoved(*myGraph))
    {
      continue;
    }
    const BRepGraph_CacheMesh::FaceMeshEntry* anEntry =
      myGraph->Mesh().Cache().Faces().Entry(aFaceId);
    if (anEntry == nullptr)
    {
      continue;
    }
    const occ::handle<Poly_Triangulation>& aTri = anEntry->Triangulation;
    if (!aTri.IsNull())
    {
      myGraph->Editor().Faces().SetPersistentTriangulation(aFaceId, aTri);
    }
  }

  for (BRepGraph_EdgeId anEdgeId = myGraph->Topo().Edges().StartId();
       anEdgeId < myGraph->Topo().Edges().EndId();
       ++anEdgeId)
  {
    if (anEdgeId.IsRemoved(*myGraph))
    {
      continue;
    }
    const BRepGraph_CacheMesh::EdgeMeshEntry* anEntry =
      myGraph->Mesh().Cache().Edges().Entry(anEdgeId);
    if (anEntry != nullptr && !anEntry->Polygon3D.IsNull())
    {
      myGraph->Editor().Edges().SetPersistentPolygon3D(anEdgeId, anEntry->Polygon3D);
    }
  }

  for (BRepGraph_CoEdgeId aCoEdgeId = myGraph->Topo().CoEdges().StartId();
       aCoEdgeId < myGraph->Topo().CoEdges().EndId();
       ++aCoEdgeId)
  {
    if (aCoEdgeId.IsRemoved(*myGraph))
    {
      continue;
    }
    const BRepGraph_CacheMesh::CoEdgeMeshEntry* aPoly2DEntry =
      getCacheMesh(myGraph).FindCoEdgePolygon2D(aCoEdgeId);
    if (aPoly2DEntry != nullptr && !aPoly2DEntry->Polygon2D.IsNull())
    {
      myGraph->Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId, aPoly2DEntry->Polygon2D);
    }
    const BRepGraph_CacheMesh::CoEdgeMeshEntry* aPolyOnTriEntry =
      getCacheMesh(myGraph).FindCoEdgePolygonOnTri(aCoEdgeId);
    if (aPolyOnTriEntry != nullptr && !aPolyOnTriEntry->PolygonsOnTri.IsEmpty())
    {
      const occ::handle<Poly_PolygonOnTriangulation>& aPoly =
        aPolyOnTriEntry->PolygonsOnTri.Value(0);
      if (!aPoly.IsNull())
      {
        myGraph->Editor().CoEdges().SetPersistentPolygonOnTri(aCoEdgeId, aPoly);
      }
    }
  }
}
