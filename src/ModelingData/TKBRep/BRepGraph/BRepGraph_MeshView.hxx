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
#include <BRepGraph_RepId.hxx>
#include <BRepGraphInc_Representation.hxx>

namespace BRepGraph_MeshCache
{
struct FaceMeshEntry;
struct CoEdgeMeshEntry;
struct EdgeMeshEntry;
} // namespace BRepGraph_MeshCache

namespace BRepGraphInc
{
struct TriangulationRep;
struct Polygon3DRep;
struct Polygon2DRep;
struct PolygonOnTriRep;
} // namespace BRepGraphInc

//! @brief Read-only view over mesh data with cache-first, persistent-fallback priority.
//!
//! Provides mesh queries that check the mesh cache (algorithm-derived mesh
//! from BRepGraphMesh) first, falling back to persistent mesh stored in
//! topology definitions (imported from STEP, etc.).
//!
//! For mesh cache writes and rep creation, use BRepGraph_Tool::Mesh.
//!
//! Obtained via BRepGraph::Mesh().
class BRepGraph::MeshView
{
public:
  //! @brief Face mesh queries (cache-first, persistent fallback).
  class FaceOps
  {
  public:
    //! Check if face has any mesh data (cached or persistent).
    [[nodiscard]] Standard_EXPORT bool HasTriangulation(const BRepGraph_FaceId theFace) const;

    //! Active triangulation rep id (cached if fresh, else persistent).
    //! @return valid TriangulationRepId, or invalid if no mesh available
    [[nodiscard]] Standard_EXPORT BRepGraph_TriangulationRepId
      ActiveTriangulationRepId(const BRepGraph_FaceId theFace) const;

    //! Direct access to cached face mesh entry (null if absent or stale).
    [[nodiscard]] Standard_EXPORT const BRepGraph_MeshCache::FaceMeshEntry*
      CachedMesh(const BRepGraph_FaceId theFace) const;

  private:
    friend class MeshView;

    explicit FaceOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    [[nodiscard]] bool isFresh(const BRepGraph_FaceId theFace,
                               const uint32_t         theStoredGen) const;

    const BRepGraph* myGraph;
  };

  //! @brief Edge mesh queries (cache-first, persistent fallback).
  class EdgeOps
  {
  public:
    //! Check if edge has polygon-3D mesh data (cached or persistent).
    [[nodiscard]] Standard_EXPORT bool HasPolygon3D(const BRepGraph_EdgeId theEdge) const;

    //! Polygon3D rep id (cached if fresh, else persistent).
    [[nodiscard]] Standard_EXPORT BRepGraph_Polygon3DRepId
      Polygon3DRepId(const BRepGraph_EdgeId theEdge) const;

    //! Direct access to cached edge mesh entry (null if absent or stale).
    [[nodiscard]] Standard_EXPORT const BRepGraph_MeshCache::EdgeMeshEntry*
      CachedMesh(const BRepGraph_EdgeId theEdge) const;

  private:
    friend class MeshView;

    explicit EdgeOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    [[nodiscard]] bool isFresh(const BRepGraph_EdgeId theEdge,
                               const uint32_t         theStoredGen) const;

    const BRepGraph* myGraph;
  };

  //! @brief CoEdge mesh queries (cache-first, persistent fallback).
  class CoEdgeOps
  {
  public:
    //! Check if coedge has cached mesh data (polygon-on-tri or polygon-2D).
    [[nodiscard]] Standard_EXPORT bool HasMesh(const BRepGraph_CoEdgeId theCoEdge) const;

    //! Direct access to cached coedge mesh entry (null if absent or stale).
    [[nodiscard]] Standard_EXPORT const BRepGraph_MeshCache::CoEdgeMeshEntry*
      CachedMesh(const BRepGraph_CoEdgeId theCoEdge) const;

  private:
    friend class MeshView;

    explicit CoEdgeOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    [[nodiscard]] bool isFresh(const BRepGraph_CoEdgeId theCoEdge,
                               const uint32_t           theStoredGen) const;

    const BRepGraph* myGraph;
  };

  //! @brief Polygonal and triangulation representation queries.
  class PolyOps
  {
  public:
    [[nodiscard]] Standard_EXPORT int NbTriangulations() const;
    [[nodiscard]] Standard_EXPORT int NbPolygons3D() const;
    [[nodiscard]] Standard_EXPORT int NbPolygons2D() const;
    [[nodiscard]] Standard_EXPORT int NbPolygonsOnTri() const;

    [[nodiscard]] Standard_EXPORT int NbActiveTriangulations() const;
    [[nodiscard]] Standard_EXPORT int NbActivePolygons3D() const;
    [[nodiscard]] Standard_EXPORT int NbActivePolygons2D() const;
    [[nodiscard]] Standard_EXPORT int NbActivePolygonsOnTri() const;

    [[nodiscard]] Standard_EXPORT const BRepGraphInc::TriangulationRep& TriangulationRep(
      const BRepGraph_TriangulationRepId theRep) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::Polygon3DRep& Polygon3DRep(
      const BRepGraph_Polygon3DRepId theRep) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::Polygon2DRep& Polygon2DRep(
      const BRepGraph_Polygon2DRepId theRep) const;
    [[nodiscard]] Standard_EXPORT const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(
      const BRepGraph_PolygonOnTriRepId theRep) const;

  private:
    friend class MeshView;

    explicit PolyOps(const BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    const BRepGraph* myGraph;
  };

  //! Grouped face mesh queries.
  [[nodiscard]] const FaceOps& Faces() const { return myFaces; }

  //! Grouped edge mesh queries.
  [[nodiscard]] const EdgeOps& Edges() const { return myEdges; }

  //! Grouped coedge mesh queries.
  [[nodiscard]] const CoEdgeOps& CoEdges() const { return myCoEdges; }

  //! Grouped polygonal representation queries.
  [[nodiscard]] const PolyOps& Poly() const { return myPoly; }

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit MeshView(const BRepGraph* theGraph)
      : myGraph(theGraph),
        myFaces(theGraph),
        myEdges(theGraph),
        myCoEdges(theGraph),
        myPoly(theGraph)
  {
  }

  const BRepGraph* myGraph;
  FaceOps          myFaces;
  EdgeOps          myEdges;
  CoEdgeOps        myCoEdges;
  PolyOps          myPoly;
};

#endif // _BRepGraph_MeshView_HeaderFile
