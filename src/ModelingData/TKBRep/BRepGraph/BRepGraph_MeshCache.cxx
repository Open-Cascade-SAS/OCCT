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

#include <BRepGraph_MeshCache.hxx>

//=================================================================================================

template <typename T>
void BRepGraph_MeshCacheStorage::ensureSize(NCollection_Vector<T>& theVec, const int theIndex)
{
  while (theVec.Length() <= theIndex)
  {
    theVec.Append(T());
  }
}

//=================================================================================================

bool BRepGraph_MeshCacheStorage::HasFaceMesh(const BRepGraph_FaceId theFace) const
{
  if (!theFace.IsValidIn(myFaceMeshes))
    return false;
  return myFaceMeshes.Value(theFace.Index).IsPresent();
}

//=================================================================================================

const BRepGraph_MeshCache::FaceMeshEntry* BRepGraph_MeshCacheStorage::FindFaceMesh(
  const BRepGraph_FaceId theFace) const
{
  if (!theFace.IsValidIn(myFaceMeshes))
    return nullptr;
  const BRepGraph_MeshCache::FaceMeshEntry& anEntry = myFaceMeshes.Value(theFace.Index);
  if (!anEntry.IsPresent())
    return nullptr;
  return &anEntry;
}

//=================================================================================================

BRepGraph_MeshCache::FaceMeshEntry& BRepGraph_MeshCacheStorage::ChangeFaceMesh(
  const BRepGraph_FaceId theFace)
{
  ensureSize(myFaceMeshes, theFace.Index);
  return myFaceMeshes.ChangeValue(theFace.Index);
}

//=================================================================================================

void BRepGraph_MeshCacheStorage::ClearFaceMesh(const BRepGraph_FaceId theFace)
{
  if (theFace.IsValidIn(myFaceMeshes))
  {
    myFaceMeshes.ChangeValue(theFace.Index).Reset();
  }
}

//=================================================================================================

bool BRepGraph_MeshCacheStorage::HasCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge) const
{
  if (!theCoEdge.IsValidIn(myCoEdgeMeshes))
    return false;
  return myCoEdgeMeshes.Value(theCoEdge.Index).IsPresent();
}

//=================================================================================================

const BRepGraph_MeshCache::CoEdgeMeshEntry* BRepGraph_MeshCacheStorage::FindCoEdgeMesh(
  const BRepGraph_CoEdgeId theCoEdge) const
{
  if (!theCoEdge.IsValidIn(myCoEdgeMeshes))
    return nullptr;
  const BRepGraph_MeshCache::CoEdgeMeshEntry& anEntry = myCoEdgeMeshes.Value(theCoEdge.Index);
  if (!anEntry.IsPresent())
    return nullptr;
  return &anEntry;
}

//=================================================================================================

BRepGraph_MeshCache::CoEdgeMeshEntry& BRepGraph_MeshCacheStorage::ChangeCoEdgeMesh(
  const BRepGraph_CoEdgeId theCoEdge)
{
  ensureSize(myCoEdgeMeshes, theCoEdge.Index);
  return myCoEdgeMeshes.ChangeValue(theCoEdge.Index);
}

//=================================================================================================

void BRepGraph_MeshCacheStorage::ClearCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge)
{
  if (theCoEdge.IsValidIn(myCoEdgeMeshes))
  {
    myCoEdgeMeshes.ChangeValue(theCoEdge.Index).Reset();
  }
}

//=================================================================================================

bool BRepGraph_MeshCacheStorage::HasEdgeMesh(const BRepGraph_EdgeId theEdge) const
{
  if (!theEdge.IsValidIn(myEdgeMeshes))
    return false;
  return myEdgeMeshes.Value(theEdge.Index).IsPresent();
}

//=================================================================================================

const BRepGraph_MeshCache::EdgeMeshEntry* BRepGraph_MeshCacheStorage::FindEdgeMesh(
  const BRepGraph_EdgeId theEdge) const
{
  if (!theEdge.IsValidIn(myEdgeMeshes))
    return nullptr;
  const BRepGraph_MeshCache::EdgeMeshEntry& anEntry = myEdgeMeshes.Value(theEdge.Index);
  if (!anEntry.IsPresent())
    return nullptr;
  return &anEntry;
}

//=================================================================================================

BRepGraph_MeshCache::EdgeMeshEntry& BRepGraph_MeshCacheStorage::ChangeEdgeMesh(
  const BRepGraph_EdgeId theEdge)
{
  ensureSize(myEdgeMeshes, theEdge.Index);
  return myEdgeMeshes.ChangeValue(theEdge.Index);
}

//=================================================================================================

void BRepGraph_MeshCacheStorage::ClearEdgeMesh(const BRepGraph_EdgeId theEdge)
{
  if (theEdge.IsValidIn(myEdgeMeshes))
  {
    myEdgeMeshes.ChangeValue(theEdge.Index).Reset();
  }
}

//=================================================================================================

void BRepGraph_MeshCacheStorage::Clear()
{
  myFaceMeshes.Clear();
  myCoEdgeMeshes.Clear();
  myEdgeMeshes.Clear();
}

//=================================================================================================

void BRepGraph_MeshCacheStorage::OnCompact(
  const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeRemapMap)
{
  // Remap face mesh entries.
  {
    NCollection_Vector<BRepGraph_MeshCache::FaceMeshEntry> aNewFaces;
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIter(theNodeRemapMap);
         anIter.More();
         anIter.Next())
    {
      const BRepGraph_NodeId& anOldId = anIter.Key();
      const BRepGraph_NodeId& aNewId  = anIter.Value();
      if (anOldId.NodeKind != BRepGraph_NodeId::Kind::Face)
        continue;
      const BRepGraph_FaceId anOldFaceId(anOldId);
      if (!anOldFaceId.IsValidIn(myFaceMeshes))
        continue;
      const BRepGraph_MeshCache::FaceMeshEntry& anOldEntry = myFaceMeshes.Value(anOldFaceId.Index);
      if (!anOldEntry.IsPresent())
        continue;
      ensureSize(aNewFaces, aNewId.Index);
      aNewFaces.ChangeValue(aNewId.Index) = anOldEntry;
    }
    myFaceMeshes = std::move(aNewFaces);
  }

  // Remap coedge mesh entries.
  {
    NCollection_Vector<BRepGraph_MeshCache::CoEdgeMeshEntry> aNewCoEdges;
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIter(theNodeRemapMap);
         anIter.More();
         anIter.Next())
    {
      const BRepGraph_NodeId& anOldId = anIter.Key();
      const BRepGraph_NodeId& aNewId  = anIter.Value();
      if (anOldId.NodeKind != BRepGraph_NodeId::Kind::CoEdge)
        continue;
      const BRepGraph_CoEdgeId anOldCoEdgeId(anOldId);
      if (!anOldCoEdgeId.IsValidIn(myCoEdgeMeshes))
        continue;
      const BRepGraph_MeshCache::CoEdgeMeshEntry& anOldEntry =
        myCoEdgeMeshes.Value(anOldCoEdgeId.Index);
      if (!anOldEntry.IsPresent())
        continue;
      ensureSize(aNewCoEdges, aNewId.Index);
      aNewCoEdges.ChangeValue(aNewId.Index) = anOldEntry;
    }
    myCoEdgeMeshes = std::move(aNewCoEdges);
  }

  // Remap edge mesh entries.
  {
    NCollection_Vector<BRepGraph_MeshCache::EdgeMeshEntry> aNewEdges;
    for (NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>::Iterator anIter(theNodeRemapMap);
         anIter.More();
         anIter.Next())
    {
      const BRepGraph_NodeId& anOldId = anIter.Key();
      const BRepGraph_NodeId& aNewId  = anIter.Value();
      if (anOldId.NodeKind != BRepGraph_NodeId::Kind::Edge)
        continue;
      const BRepGraph_EdgeId anOldEdgeId(anOldId);
      if (!anOldEdgeId.IsValidIn(myEdgeMeshes))
        continue;
      const BRepGraph_MeshCache::EdgeMeshEntry& anOldEntry = myEdgeMeshes.Value(anOldEdgeId.Index);
      if (!anOldEntry.IsPresent())
        continue;
      ensureSize(aNewEdges, aNewId.Index);
      aNewEdges.ChangeValue(aNewId.Index) = anOldEntry;
    }
    myEdgeMeshes = std::move(aNewEdges);
  }
}
