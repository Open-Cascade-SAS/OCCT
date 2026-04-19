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

#ifndef _BRepGraph_MeshCache_HeaderFile
#define _BRepGraph_MeshCache_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RepId.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>

//! @brief Cached mesh data storage for BRepGraph.
//!
//! Stores mesh RepId references (triangulations for faces, polygons for edges
//! and coedges) separately from topology definitions. This cache holds
//! algorithm-derived mesh data written by BRepGraphMesh, as opposed to
//! persistent mesh data stored in definition structs (imported from STEP, etc.).
//!
//! Priority rule: cached mesh takes precedence over persistent mesh in
//! definitions. Persistent mesh is the fallback when no fresh cache exists.
//!
//! Freshness is validated by comparing StoredOwnGen against the entity's
//! current OwnGen. A mismatch means the geometry changed since meshing,
//! so the cached mesh is stale.
//!
//! Writing to the cache does NOT trigger markModified() or mutation tracking.
//!
//! ### Invalidation contract
//! The cache relies on the following invariants upheld by BRepGraph mutations:
//! 1. Any `Editor().Faces().Mut(FaceId)` guard bumps `FaceDef.OwnGen` on scope
//!    exit, invalidating cached face mesh entries.
//! 2. `markRepModified(SurfaceRepId | TriangulationRepId)` iterates every Face
//!    referencing the rep and calls `markModified(FaceId)`, so geometry edits
//!    through `Editor().Reps().MutSurface/MutTriangulation()` also invalidate
//!    cached face meshes.
//! 3. `markRepModified(TriangulationRepId)` additionally scans the cache itself
//!    (not just persistent `FaceDef.TriangulationRepId`) so that cached-only
//!    triangulations are bumped along with their owning Face's `OwnGen`.
//! Edge/CoEdge caches follow the analogous pattern for `EdgeDef`/`CoEdgeDef`
//! and the corresponding `Polygon3D`/`Polygon2D`/`PolygonOnTri` reps.
namespace BRepGraph_MeshCache
{

//! Cached mesh entry for a face: triangulation rep references.
struct FaceMeshEntry
{
  NCollection_Vector<BRepGraph_TriangulationRepId> TriangulationRepIds;
  int                                              ActiveTriangulationIndex = -1;
  uint32_t StoredOwnGen = 0; //!< OwnGen of FaceDef at write time

  //! True if this entry contains mesh data.
  [[nodiscard]] bool IsPresent() const { return !TriangulationRepIds.IsEmpty(); }

  //! Convenience: active triangulation rep id, or invalid.
  [[nodiscard]] BRepGraph_TriangulationRepId ActiveTriangulationRepId() const
  {
    if (ActiveTriangulationIndex >= 0 && ActiveTriangulationIndex < TriangulationRepIds.Length())
      return TriangulationRepIds.Value(ActiveTriangulationIndex);
    return BRepGraph_TriangulationRepId();
  }

  //! Reset all fields to default (absent) state.
  void Reset()
  {
    TriangulationRepIds.Clear();
    ActiveTriangulationIndex = -1;
    StoredOwnGen             = 0;
  }
};

//! Cached mesh entry for a coedge: polygon-on-triangulation and polygon-2D rep references.
struct CoEdgeMeshEntry
{
  BRepGraph_Polygon2DRepId                        Polygon2DRepId;
  NCollection_Vector<BRepGraph_PolygonOnTriRepId> PolygonOnTriRepIds;
  uint32_t StoredOwnGen = 0; //!< OwnGen of CoEdgeDef at write time

  //! True if this entry contains mesh data.
  [[nodiscard]] bool IsPresent() const
  {
    return Polygon2DRepId.IsValid() || !PolygonOnTriRepIds.IsEmpty();
  }

  //! Reset all fields to default (absent) state.
  void Reset()
  {
    Polygon2DRepId = BRepGraph_Polygon2DRepId();
    PolygonOnTriRepIds.Clear();
    StoredOwnGen = 0;
  }
};

//! Cached mesh entry for an edge: polygon-3D rep reference.
struct EdgeMeshEntry
{
  BRepGraph_Polygon3DRepId Polygon3DRepId;
  uint32_t                 StoredOwnGen = 0; //!< OwnGen of EdgeDef at write time

  //! True if this entry contains mesh data.
  [[nodiscard]] bool IsPresent() const { return Polygon3DRepId.IsValid(); }

  //! Reset all fields to default (absent) state.
  void Reset()
  {
    Polygon3DRepId = BRepGraph_Polygon3DRepId();
    StoredOwnGen   = 0;
  }
};

} // namespace BRepGraph_MeshCache

//! @brief Storage backend for cached mesh data.
//!
//! Dense vectors indexed by per-kind entity index (same pattern as DefStore).
//! Entries with StoredOwnGen == 0 are absent (no cached mesh data).
//! Thread safety: parallel writes to different indices are safe (no contention).
class BRepGraph_MeshCacheStorage
{
public:
  //! Check if a face has a cached mesh entry (StoredOwnGen != 0).
  [[nodiscard]] bool HasFaceMesh(const BRepGraph_FaceId theFace) const;

  //! Find face mesh entry, or nullptr if absent.
  [[nodiscard]] const BRepGraph_MeshCache::FaceMeshEntry* FindFaceMesh(
    const BRepGraph_FaceId theFace) const;

  //! Get or create a face mesh entry. Creates with default values if absent.
  [[nodiscard]] BRepGraph_MeshCache::FaceMeshEntry& ChangeFaceMesh(const BRepGraph_FaceId theFace);

  //! Clear the face mesh entry (reset to absent).
  void ClearFaceMesh(const BRepGraph_FaceId theFace);

  //! Check if a coedge has a cached mesh entry.
  [[nodiscard]] bool HasCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge) const;

  //! Find coedge mesh entry, or nullptr if absent.
  [[nodiscard]] const BRepGraph_MeshCache::CoEdgeMeshEntry* FindCoEdgeMesh(
    const BRepGraph_CoEdgeId theCoEdge) const;

  //! Get or create a coedge mesh entry.
  [[nodiscard]] BRepGraph_MeshCache::CoEdgeMeshEntry& ChangeCoEdgeMesh(
    const BRepGraph_CoEdgeId theCoEdge);

  //! Clear the coedge mesh entry.
  void ClearCoEdgeMesh(const BRepGraph_CoEdgeId theCoEdge);

  //! Check if an edge has a cached mesh entry.
  [[nodiscard]] bool HasEdgeMesh(const BRepGraph_EdgeId theEdge) const;

  //! Find edge mesh entry, or nullptr if absent.
  [[nodiscard]] const BRepGraph_MeshCache::EdgeMeshEntry* FindEdgeMesh(
    const BRepGraph_EdgeId theEdge) const;

  //! Get or create an edge mesh entry.
  [[nodiscard]] BRepGraph_MeshCache::EdgeMeshEntry& ChangeEdgeMesh(const BRepGraph_EdgeId theEdge);

  //! Clear the edge mesh entry.
  void ClearEdgeMesh(const BRepGraph_EdgeId theEdge);

  //! Clear all cached mesh data.
  void Clear();

  //! Remap cache entries after compaction.
  //! @param[in] theNodeRemapMap old NodeId -> new NodeId mapping
  void OnCompact(const NCollection_DataMap<BRepGraph_NodeId, BRepGraph_NodeId>& theNodeRemapMap);

private:
  //! Ensure vector has at least theIndex+1 elements.
  template <typename T>
  static void ensureSize(NCollection_Vector<T>& theVec, const int theIndex);

  NCollection_Vector<BRepGraph_MeshCache::FaceMeshEntry>   myFaceMeshes;
  NCollection_Vector<BRepGraph_MeshCache::CoEdgeMeshEntry> myCoEdgeMeshes;
  NCollection_Vector<BRepGraph_MeshCache::EdgeMeshEntry>   myEdgeMeshes;
};

#endif // _BRepGraph_MeshCache_HeaderFile
