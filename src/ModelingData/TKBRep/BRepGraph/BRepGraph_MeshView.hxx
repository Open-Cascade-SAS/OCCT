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

#ifndef _BRepGraph_MeshView_HeaderFile
#define _BRepGraph_MeshView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_CacheMesh.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

//! @brief Read/write view over mesh data.
//!
//! Splits mesh access into three explicit sub-views:
//! - `Cache()`     - reads from the BRepGraphMesh cache only (algorithm-derived,
//!                   freshness-checked against the entity's OwnGen).
//! - `Persistent()` - reads from definition-resident mesh (FaceDef.TriangulationRepId,
//!                   EdgeDef.Polygon3DRepId, CoEdgeDef.Polygon2DRepId,
//!                   CoEdgeDef.PolygonOnTriRepId).
//! - `Editor()`    - cache mutations (append/clear). Persistent rep creation lives on
//!                   `BRepGraph::Editor().Edges()`, `BRepGraph::Editor().CoEdges()`,
//!                   `BRepGraph::Editor().Faces()` since reps back the topology defs.
//! - `Poly()`      - mesh element count queries (shared by all paths).
//!
//! There is no fallback path that mixes cache and persistent - callers pick the
//! source explicitly.
//!
//! Obtained via `BRepGraph::Mesh()` (const) or `BRepGraph::Mesh()` (non-const for Editor).
class BRepGraph::MeshView
{
public:
  //! Cache reads. Each accessor returns data only if a fresh cache entry exists
  //! for the given entity (matched against its current OwnGen).
  class CacheView
  {
  public:
    class FaceOps
    {
    public:
      //! True if a fresh cached triangulation is present.
      //! @param[in] theFace typed face definition identifier
      //! @return true if Entry() would return non-null
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_FaceId theFace) const;

      //! Cached triangulation handle.
      //! @param[in] theFace typed face definition identifier
      //! @return triangulation handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Triangulation>& Triangulation(
        const BRepGraph_FaceId theFace) const;

      //! Raw cached face mesh entry, or nullptr if absent or stale.
      //! @param[in] theFace typed face definition identifier
      //! @return cache entry pointer, or nullptr
      [[nodiscard]] Standard_EXPORT const BRepGraph_CacheMesh::FaceMeshEntry* Entry(
        const BRepGraph_FaceId theFace) const;

    private:
      friend class CacheView;

      explicit FaceOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class EdgeOps
    {
    public:
      //! True if a fresh cached Polygon3D is bound to the edge.
      //! @param[in] theEdge typed edge definition identifier
      //! @return true if a fresh Polygon3D is present in cache
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_EdgeId theEdge) const;

      //! Cached Polygon3D handle.
      //! @param[in] theEdge typed edge definition identifier
      //! @return polygon-3D handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Polygon3D>& Polygon3D(
        const BRepGraph_EdgeId theEdge) const;

      //! Raw cached edge mesh entry, or nullptr if absent or stale.
      //! @param[in] theEdge typed edge definition identifier
      //! @return cache entry pointer, or nullptr
      [[nodiscard]] Standard_EXPORT const BRepGraph_CacheMesh::EdgeMeshEntry* Entry(
        const BRepGraph_EdgeId theEdge) const;

    private:
      friend class CacheView;

      explicit EdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class CoEdgeOps
    {
    public:
      //! True if a fresh cached entry exists (any of polygon-2D / polygon-on-tri).
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if a fresh cache entry exists
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_CoEdgeId theCoEdge) const;

      //! Return coedge entry if Polygon2D is fresh, nullptr otherwise.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return cache entry pointer, or nullptr
      [[nodiscard]] Standard_EXPORT const BRepGraph_CacheMesh::CoEdgeMeshEntry* FindPolygon2D(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! Return coedge entry if PolygonsOnTri is fresh, nullptr otherwise.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return cache entry pointer, or nullptr
      [[nodiscard]] Standard_EXPORT const BRepGraph_CacheMesh::CoEdgeMeshEntry* FindPolygonOnTri(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! Raw coedge entry access (no freshness filtering). Returns nullptr if
      //! the entry has no representation. For internal/testing use.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return cache entry pointer, or nullptr if absent
      [[nodiscard]] Standard_EXPORT const BRepGraph_CacheMesh::CoEdgeMeshEntry* FindRaw(
        const BRepGraph_CoEdgeId theCoEdge) const;

    private:
      friend class CacheView;

      explicit CoEdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    //! Grouped face cache queries.
    [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

    //! Grouped edge cache queries.
    [[nodiscard]] const EdgeOps& Edges() const { return myEdges; }

    //! Grouped coedge cache queries.
    [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  private:
    friend class MeshView;

    explicit CacheView(BRepGraph* theGraph)
        : myFaces(theGraph),
          myEdges(theGraph),
          myCoEdges(theGraph)
    {
    }

    FaceOps   myFaces;
    EdgeOps   myEdges;
    CoEdgeOps myCoEdges;
  };

  //! Persistent reads. Resolves data through the rep id stored on the entity's
  //! definition (FaceDef / EdgeDef / CoEdgeDef). Independent of the cache.
  class PersistentView
  {
  public:
    class FaceOps
    {
    public:
      //! True if FaceDef.TriangulationRepId is valid and the rep is not removed.
      //! @param[in] theFace typed face definition identifier
      //! @return true if a persistent triangulation is bound
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_FaceId theFace) const;

      //! Persistent triangulation handle.
      //! @param[in] theFace typed face definition identifier
      //! @return triangulation handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Triangulation>& Triangulation(
        const BRepGraph_FaceId theFace) const;

    private:
      friend class PersistentView;

      explicit FaceOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class EdgeOps
    {
    public:
      //! True if EdgeDef.Polygon3DRepId is bound (dominant kind on edges).
      //! @param[in] theEdge typed edge definition identifier
      //! @return true if a persistent Polygon3D is bound
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_EdgeId theEdge) const;

      //! Persistent Polygon3D handle.
      //! @param[in] theEdge typed edge definition identifier
      //! @return polygon-3D handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Polygon3D>& Polygon3D(
        const BRepGraph_EdgeId theEdge) const;

      //! True if the (edge, face) coedge has a polygon-on-triangulation.
      //! @param[in] theEdge typed edge definition identifier
      //! @param[in] theFace typed face definition identifier
      //! @return true if persistent polygon-on-triangulation is bound
      [[nodiscard]] Standard_EXPORT bool HasPolygonOnTriangulation(
        const BRepGraph_EdgeId theEdge,
        const BRepGraph_FaceId theFace) const;

      //! Polygon-on-triangulation for the (edge, face) coedge.
      //! @param[in] theEdge typed edge definition identifier
      //! @param[in] theFace typed face definition identifier
      //! @return polygon-on-triangulation handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_PolygonOnTriangulation>&
                                          PolygonOnTriangulation(const BRepGraph_EdgeId theEdge,
                                                                 const BRepGraph_FaceId theFace) const;

    private:
      friend class PersistentView;

      explicit EdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class CoEdgeOps
    {
    public:
      //! True if CoEdgeDef.Polygon2DRepId is bound (dominant kind on coedges).
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if a persistent polygon-on-surface is bound
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_CoEdgeId theCoEdge) const;

      //! Persistent polygon-on-surface (2D polygon) bound to the coedge.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return polygon-2D handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Polygon2D>& PolygonOnSurface(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! True if CoEdgeDef.PolygonOnTriRepId is bound.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if persistent polygon-on-triangulation is bound
      [[nodiscard]] Standard_EXPORT bool HasPolygonOnTriangulation(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! Persistent polygon-on-triangulation bound to the coedge.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return polygon-on-triangulation handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_PolygonOnTriangulation>&
        PolygonOnTriangulation(const BRepGraph_CoEdgeId theCoEdge) const;

    private:
      friend class PersistentView;

      explicit CoEdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    //! Grouped face persistent queries.
    [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

    //! Grouped edge persistent queries.
    [[nodiscard]] const EdgeOps& Edges() const { return myEdges; }

    //! Grouped coedge persistent queries.
    [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  private:
    friend class MeshView;

    explicit PersistentView(BRepGraph* theGraph)
        : myFaces(theGraph),
          myEdges(theGraph),
          myCoEdges(theGraph)
    {
    }

    FaceOps   myFaces;
    EdgeOps   myEdges;
    CoEdgeOps myCoEdges;
  };

  //! Resolves mesh data by checking the cache first and the persistent (def-resident)
  //! source second. Callers that do not care which source supplies the data go
  //! through this view; callers that do care use Cache() or Persistent() directly.
  class EffectiveView
  {
  public:
    class FaceOps
    {
    public:
      //! True if the face has a triangulation in either cache or persistent storage.
      //! @param[in] theFace typed face definition identifier
      //! @return true if any triangulation is reachable
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_FaceId theFace) const;

      //! Cached triangulation handle (cache first, persistent fallback).
      //! @param[in] theFace typed face definition identifier
      //! @return triangulation handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Triangulation>& Triangulation(
        const BRepGraph_FaceId theFace) const;

    private:
      friend class EffectiveView;

      explicit FaceOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class EdgeOps
    {
    public:
      //! True if the edge has a Polygon3D in either cache or persistent storage.
      //! @param[in] theEdge typed edge definition identifier
      //! @return true if any Polygon3D is reachable
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_EdgeId theEdge) const;

      //! Polygon3D handle (cache first, persistent fallback).
      //! @param[in] theEdge typed edge definition identifier
      //! @return polygon-3D handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Polygon3D>& Polygon3D(
        const BRepGraph_EdgeId theEdge) const;

    private:
      friend class EffectiveView;

      explicit EdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class CoEdgeOps
    {
    public:
      //! True if the coedge has any polygon-2D / polygon-on-tri in either source.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if any coedge mesh data is reachable
      [[nodiscard]] Standard_EXPORT bool Has(const BRepGraph_CoEdgeId theCoEdge) const;

      //! True if a polygon-on-surface (2D) is reachable in either source.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if a polygon-2D is bound on either side
      [[nodiscard]] Standard_EXPORT bool HasPolygonOnSurface(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! Polygon-on-surface (2D) handle (cache first, persistent fallback).
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return polygon-2D handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_Polygon2D>& PolygonOnSurface(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! True if a polygon-on-triangulation is reachable in either source.
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return true if a polygon-on-tri is bound on either side
      [[nodiscard]] Standard_EXPORT bool HasPolygonOnTriangulation(
        const BRepGraph_CoEdgeId theCoEdge) const;

      //! Polygon-on-triangulation handle (cache first, persistent fallback).
      //! @param[in] theCoEdge typed coedge definition identifier
      //! @return polygon-on-tri handle, or null handle if absent
      [[nodiscard]] Standard_EXPORT const occ::handle<Poly_PolygonOnTriangulation>&
        PolygonOnTriangulation(const BRepGraph_CoEdgeId theCoEdge) const;

    private:
      friend class EffectiveView;

      explicit CoEdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    //! Grouped face effective queries.
    [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

    //! Grouped edge effective queries.
    [[nodiscard]] const EdgeOps& Edges() const { return myEdges; }

    //! Grouped coedge effective queries.
    [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  private:
    friend class MeshView;

    explicit EffectiveView(BRepGraph* theGraph)
        : myFaces(theGraph),
          myEdges(theGraph),
          myCoEdges(theGraph)
    {
    }

    FaceOps   myFaces;
    EdgeOps   myEdges;
    CoEdgeOps myCoEdges;
  };

  //! Cache mutation surface. Mutates the BRepGraphMesh cache only - does not
  //! touch persistent definition data. Persistent rep creation/edit lives on
  //! `BRepGraph::Editor().Edges()`, `BRepGraph::Editor().CoEdges()`,
  //! `BRepGraph::Editor().Faces()`.
  class EditorView
  {
  public:
    class FaceOps
    {
    public:
      //! Set the cached triangulation for a face.
      //! @param[in] theFace         typed face definition identifier
      //! @param[in] theTriangulation triangulation to store (null clears)
      Standard_EXPORT void SetCachedTriangulation(
        const BRepGraph_FaceId                 theFace,
        const occ::handle<Poly_Triangulation>& theTriangulation);

      //! Clear the face's cached mesh entry (no effect if absent).
      //! @param[in] theFace typed face definition identifier
      Standard_EXPORT void Clear(const BRepGraph_FaceId theFace);

    private:
      friend class EditorView;

      explicit FaceOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class EdgeOps
    {
    public:
      //! Bind a Polygon3D to the edge's cached entry.
      //! @param[in] theEdge     typed edge definition identifier
      //! @param[in] thePolygon3D polygon-3D handle (null clears the cached binding)
      Standard_EXPORT void SetCachedPolygon3D(const BRepGraph_EdgeId             theEdge,
                                              const occ::handle<Poly_Polygon3D>& thePolygon3D);

      //! Clear the edge's cached mesh entry.
      //! @param[in] theEdge typed edge definition identifier
      Standard_EXPORT void Clear(const BRepGraph_EdgeId theEdge);

    private:
      friend class EditorView;

      explicit EdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    class CoEdgeOps
    {
    public:
      //! Append a polygon-on-triangulation to the coedge's cached list.
      //! @param[in] theCoEdge    typed coedge definition identifier
      //! @param[in] thePolygonOnTri polygon-on-tri to append
      Standard_EXPORT void AppendCachedPolygonOnTri(
        const BRepGraph_CoEdgeId                        theCoEdge,
        const occ::handle<Poly_PolygonOnTriangulation>& thePolygonOnTri);

      //! Bind a polygon-2D to the coedge's cached entry.
      //! @param[in] theCoEdge   typed coedge definition identifier
      //! @param[in] thePolygon2D polygon-2D handle (null clears the cached binding)
      Standard_EXPORT void SetCachedPolygon2D(const BRepGraph_CoEdgeId           theCoEdge,
                                              const occ::handle<Poly_Polygon2D>& thePolygon2D);

      //! Clear the coedge's cached mesh entry.
      //! @param[in] theCoEdge typed coedge definition identifier
      Standard_EXPORT void Clear(const BRepGraph_CoEdgeId theCoEdge);

    private:
      friend class EditorView;

      explicit CoEdgeOps(BRepGraph* theGraph)
          : myGraph(theGraph)
      {
      }

      BRepGraph* myGraph;
    };

    //! Grouped face cache mutations.
    [[nodiscard]] FaceOps& Faces() { return myFaces; }

    //! Grouped edge cache mutations.
    [[nodiscard]] EdgeOps& Edges() { return myEdges; }

    //! Grouped coedge cache mutations.
    [[nodiscard]] CoEdgeOps& CoEdges() { return myCoEdges; }

    //! Promote all currently fresh default-slot cache mesh entries to persistent mesh reps.
    Standard_EXPORT void PromoteToPersistent();

  private:
    friend class MeshView;

    explicit EditorView(BRepGraph* theGraph)
        : myGraph(theGraph),
          myFaces(theGraph),
          myEdges(theGraph),
          myCoEdges(theGraph)
    {
    }

    BRepGraph* myGraph;
    FaceOps    myFaces;
    EdgeOps    myEdges;
    CoEdgeOps  myCoEdges;
  };

  //! @brief Polygonal and triangulation count queries.
  class PolyOps
  {
  public:
    //! Total number of face triangulation slots (including removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbFaceTriangulations() const;
    //! Total number of edge polygon-3D slots (including removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbEdgePolygons3D() const;
    //! Total number of coedge polygon-2D slots (including removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbCoEdgePolygons2D() const;
    //! Total number of coedge polygon-on-triangulation slots (including removed).
    [[nodiscard]] Standard_EXPORT uint32_t NbCoEdgePolygonsOnTri() const;

    //! Number of non-removed face triangulation entries.
    [[nodiscard]] Standard_EXPORT uint32_t NbActiveTriangulations() const;
    //! Number of non-removed edge polygon-3D entries.
    [[nodiscard]] Standard_EXPORT uint32_t NbActivePolygons3D() const;
    //! Number of non-removed coedge polygon-2D entries.
    [[nodiscard]] Standard_EXPORT uint32_t NbActivePolygons2D() const;
    //! Number of non-removed coedge polygon-on-triangulation entries.
    [[nodiscard]] Standard_EXPORT uint32_t NbActivePolygonsOnTri() const;

  private:
    friend class MeshView;

    explicit PolyOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! Cache-only reads.
  [[nodiscard]] const CacheView& Cache() const { return myCache; }

  //! Persistent (definition-resident) reads.
  [[nodiscard]] const PersistentView& Persistent() const { return myPersistent; }

  //! Effective reads - cache first, persistent fallback. Use when source is irrelevant.
  [[nodiscard]] const EffectiveView& Effective() const { return myEffective; }

  //! Cache mutations.
  [[nodiscard]] EditorView& Editor() { return myEditor; }

  //! Polygon/triangulation count queries.
  [[nodiscard]] const PolyOps& Poly() const { return myPoly; }

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit MeshView(BRepGraph* theGraph)
      : myGraph(theGraph),
        myPoly(theGraph),
        myCache(theGraph),
        myPersistent(theGraph),
        myEffective(theGraph),
        myEditor(theGraph)
  {
  }

  BRepGraph*     myGraph;
  PolyOps        myPoly;
  CacheView      myCache;
  PersistentView myPersistent;
  EffectiveView  myEffective;
  EditorView     myEditor;
};

#endif // _BRepGraph_MeshView_HeaderFile
