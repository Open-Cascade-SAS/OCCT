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
#include <BRepGraph_Data.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraphInc_Storage.hxx>

//=================================================================================================
// FaceOps
//=================================================================================================

bool BRepGraph::MeshView::FaceOps::isFresh(const BRepGraph_FaceId theFace,
                                           const uint32_t         theStoredGen) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return false;
  }
  return theStoredGen == aStorage.Face(theFace).OwnGen;
}

//=================================================================================================

bool BRepGraph::MeshView::FaceOps::HasTriangulation(const BRepGraph_FaceId theFace) const
{
  const BRepGraph_MeshCacheStorage&         aMeshCache = myGraph->myData->myMeshCache;
  const BRepGraph_MeshCache::FaceMeshEntry* aCached    = aMeshCache.FindFaceMesh(theFace);
  if (aCached != nullptr && isFresh(theFace, aCached->StoredOwnGen))
  {
    const BRepGraph_TriangulationRepId aRepId = aCached->ActiveTriangulationRepId();
    if (aRepId.IsValid())
    {
      return true;
    }
  }
  // Fallback to persistent mesh in definition.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return false;
  }
  const BRepGraph_TriangulationRepId aRepId = aStorage.Face(theFace).TriangulationRepId;
  return aRepId.IsValid(aStorage.NbTriangulations())
         && !aStorage.TriangulationRep(aRepId).IsRemoved;
}

//=================================================================================================

BRepGraph_TriangulationRepId BRepGraph::MeshView::FaceOps::ActiveTriangulationRepId(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraphInc_Storage&       aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_MeshCacheStorage& aMeshCache = myGraph->myData->myMeshCache;

  // Cache-first.
  const BRepGraph_MeshCache::FaceMeshEntry* aCached = aMeshCache.FindFaceMesh(theFace);
  if (aCached != nullptr && isFresh(theFace, aCached->StoredOwnGen))
  {
    const BRepGraph_TriangulationRepId aRepId = aCached->ActiveTriangulationRepId();
    if (aRepId.IsValid(aStorage.NbTriangulations()) && !aStorage.TriangulationRep(aRepId).IsRemoved)
    {
      return aRepId;
    }
  }

  // Fallback to persistent.
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

const BRepGraph_MeshCache::FaceMeshEntry* BRepGraph::MeshView::FaceOps::CachedMesh(
  const BRepGraph_FaceId theFace) const
{
  const BRepGraph_MeshCache::FaceMeshEntry* aCached =
    myGraph->myData->myMeshCache.FindFaceMesh(theFace);
  if (aCached != nullptr && isFresh(theFace, aCached->StoredOwnGen))
  {
    return aCached;
  }
  return nullptr;
}

//=================================================================================================
// EdgeOps
//=================================================================================================

bool BRepGraph::MeshView::EdgeOps::isFresh(const BRepGraph_EdgeId theEdge,
                                           const uint32_t         theStoredGen) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return false;
  }
  return theStoredGen == aStorage.Edge(theEdge).OwnGen;
}

//=================================================================================================

bool BRepGraph::MeshView::EdgeOps::HasPolygon3D(const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_MeshCacheStorage&         aMeshCache = myGraph->myData->myMeshCache;
  const BRepGraph_MeshCache::EdgeMeshEntry* aCached    = aMeshCache.FindEdgeMesh(theEdge);
  if (aCached != nullptr && isFresh(theEdge, aCached->StoredOwnGen))
  {
    if (aCached->Polygon3DRepId.IsValid())
    {
      return true;
    }
  }
  // Fallback to persistent.
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return false;
  }
  const BRepGraph_Polygon3DRepId aRepId = aStorage.Edge(theEdge).Polygon3DRepId;
  return aRepId.IsValid(aStorage.NbPolygons3D()) && !aStorage.Polygon3DRep(aRepId).IsRemoved;
}

//=================================================================================================

BRepGraph_Polygon3DRepId BRepGraph::MeshView::EdgeOps::Polygon3DRepId(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraphInc_Storage&       aStorage   = myGraph->myData->myIncStorage;
  const BRepGraph_MeshCacheStorage& aMeshCache = myGraph->myData->myMeshCache;

  // Cache-first.
  const BRepGraph_MeshCache::EdgeMeshEntry* aCached = aMeshCache.FindEdgeMesh(theEdge);
  if (aCached != nullptr && isFresh(theEdge, aCached->StoredOwnGen))
  {
    if (aCached->Polygon3DRepId.IsValid(aStorage.NbPolygons3D())
        && !aStorage.Polygon3DRep(aCached->Polygon3DRepId).IsRemoved)
    {
      return aCached->Polygon3DRepId;
    }
  }

  // Fallback to persistent.
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return BRepGraph_Polygon3DRepId();
  }
  const BRepGraph_Polygon3DRepId aRepId = aStorage.Edge(theEdge).Polygon3DRepId;
  if (!aRepId.IsValid(aStorage.NbPolygons3D()) || aStorage.Polygon3DRep(aRepId).IsRemoved)
  {
    return BRepGraph_Polygon3DRepId();
  }
  return aRepId;
}

//=================================================================================================

const BRepGraph_MeshCache::EdgeMeshEntry* BRepGraph::MeshView::EdgeOps::CachedMesh(
  const BRepGraph_EdgeId theEdge) const
{
  const BRepGraph_MeshCache::EdgeMeshEntry* aCached =
    myGraph->myData->myMeshCache.FindEdgeMesh(theEdge);
  if (aCached != nullptr && isFresh(theEdge, aCached->StoredOwnGen))
  {
    return aCached;
  }
  return nullptr;
}

//=================================================================================================
// CoEdgeOps
//=================================================================================================

bool BRepGraph::MeshView::CoEdgeOps::isFresh(const BRepGraph_CoEdgeId theCoEdge,
                                             const uint32_t           theStoredGen) const
{
  const BRepGraphInc_Storage& aStorage = myGraph->myData->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return false;
  }
  return theStoredGen == aStorage.CoEdge(theCoEdge).OwnGen;
}

//=================================================================================================

bool BRepGraph::MeshView::CoEdgeOps::HasMesh(const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_MeshCache::CoEdgeMeshEntry* aCached =
    myGraph->myData->myMeshCache.FindCoEdgeMesh(theCoEdge);
  if (aCached != nullptr && isFresh(theCoEdge, aCached->StoredOwnGen))
  {
    return true;
  }
  return false;
}

//=================================================================================================

const BRepGraph_MeshCache::CoEdgeMeshEntry* BRepGraph::MeshView::CoEdgeOps::CachedMesh(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  const BRepGraph_MeshCache::CoEdgeMeshEntry* aCached =
    myGraph->myData->myMeshCache.FindCoEdgeMesh(theCoEdge);
  if (aCached != nullptr && isFresh(theCoEdge, aCached->StoredOwnGen))
  {
    return aCached;
  }
  return nullptr;
}

//=================================================================================================
// PolyOps
//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbTriangulations() const
{
  return myGraph->myData->myIncStorage.NbTriangulations();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbPolygons3D() const
{
  return myGraph->myData->myIncStorage.NbPolygons3D();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbPolygons2D() const
{
  return myGraph->myData->myIncStorage.NbPolygons2D();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbPolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbPolygonsOnTri();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbActiveTriangulations() const
{
  return myGraph->myData->myIncStorage.NbActiveTriangulations();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbActivePolygons3D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons3D();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbActivePolygons2D() const
{
  return myGraph->myData->myIncStorage.NbActivePolygons2D();
}

//=================================================================================================

int BRepGraph::MeshView::PolyOps::NbActivePolygonsOnTri() const
{
  return myGraph->myData->myIncStorage.NbActivePolygonsOnTri();
}

//=================================================================================================

const BRepGraphInc::TriangulationRep& BRepGraph::MeshView::PolyOps::TriangulationRep(
  const BRepGraph_TriangulationRepId theRep) const
{
  return myGraph->myData->myIncStorage.TriangulationRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Polygon3DRep& BRepGraph::MeshView::PolyOps::Polygon3DRep(
  const BRepGraph_Polygon3DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon3DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::Polygon2DRep& BRepGraph::MeshView::PolyOps::Polygon2DRep(
  const BRepGraph_Polygon2DRepId theRep) const
{
  return myGraph->myData->myIncStorage.Polygon2DRep(theRep);
}

//=================================================================================================

const BRepGraphInc::PolygonOnTriRep& BRepGraph::MeshView::PolyOps::PolygonOnTriRep(
  const BRepGraph_PolygonOnTriRepId theRep) const
{
  return myGraph->myData->myIncStorage.PolygonOnTriRep(theRep);
}
