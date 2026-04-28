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

#ifndef _BRepGraph_Transform_HeaderFile
#define _BRepGraph_Transform_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_Builder.hxx>

#include <Standard_DefineAlloc.hxx>
#include <gp_Trsf.hxx>

//! @brief Graph-to-graph transformation.
//!
//! Produces a new BRepGraph by copying and then applying a geometric
//! transformation to vertex points and geometry node locations.
//!
//! Two geometry modes (matching BRepBuilderAPI_Transform semantics):
//! - theCopyGeom = true  (geometry-level): deep-copy geometry, transform handles
//!   in-place via Geom_Surface::Transform() etc., reset locations to identity.
//! - theCopyGeom = false (root-level): light-copy with shared geometry, apply
//!   transform via location modification only.
//!
//! Mesh handling (theCopyMesh parameter):
//! - theCopyMesh = false (default): triangulations and polygons are discarded
//!   after a geometry-level transform and must be recomputed.
//! - theCopyMesh = true: all mesh data (Poly_Triangulation on FaceDefs and the
//!   MeshLayer cache, Poly_Polygon3D on edges, Poly_PolygonOnTriangulation on
//!   coedges) is copied and transformed in sync with the geometry.
//!   In location-only mode the mesh data is copied as-is (nodes stay in the
//!   graph coordinate system, which is unaffected by a pure location compose).
//!
//! @note Returns BRepGraph directly (not a Result struct) because this is an
//! immutable operation producing a new graph. Check IsDone() for success.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   BRepGraph_Builder::Add(aGraph, myShape);
//!   gp_Trsf aTrsf;
//!   aTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
//!   BRepGraph aTransformed = BRepGraph_Transform::Perform(aGraph, aTrsf);
//!   TopoDS_Shape aShape = aTransformed.Shapes().Shape();
//! @endcode
class BRepGraph_Transform
{
public:
  DEFINE_STANDARD_ALLOC

  //! Transform the entire graph.
  //! @param[in] theGraph    a pre-built BRepGraph (must have IsDone() == true)
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming;
  //!                        if false, light-copy then transform locations/points only
  //! @param[in] theCopyMesh if true, mesh data (triangulations, polygons) is copied and
  //!                        transformed; if false, meshes are discarded after transform
  //! @return a new BRepGraph with the transformation applied
  [[nodiscard]] Standard_EXPORT static BRepGraph Perform(const BRepGraph& theGraph,
                                                         const gp_Trsf&   theTrsf,
                                                         const bool       theCopyGeom = true,
                                                         const bool       theCopyMesh = false);

  //! Transform a single node sub-graph of any kind (Face, Shell, Solid, Wire, Edge, Vertex).
  //! Produces a new BRepGraph containing only the specified node and its referenced sub-graph.
  //! The transform is applied to all copied geometry (same rules as Perform()).
  //! @param[in] theGraph    a pre-built BRepGraph
  //! @param[in] theNodeId   node identifier (any kind: Face, Shell, Solid, Wire, Edge, Vertex,
  //!                        Compound, CompSolid, Product, Occurrence)
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming
  //! @param[in] theCopyMesh if true, mesh data is copied and transformed
  //! @return a new BRepGraph containing only the specified sub-graph, transformed
  [[nodiscard]] Standard_EXPORT static BRepGraph TransformNode(const BRepGraph&       theGraph,
                                                               const BRepGraph_NodeId theNodeId,
                                                               const gp_Trsf&         theTrsf,
                                                               const bool theCopyGeom = true,
                                                               const bool theCopyMesh = false);

  //! Apply an in-place location-only transform to a single reference.
  //! Composes theTrsf into the reference's LocalLocation field without copying
  //! any geometry. This is O(1) and equivalent to TopoDS_Shape::Moved(trsf).
  //! Cached mesh data on entities downstream of the moved ref is stored in the
  //! entity's local frame and is unaffected; callers that bake a world transform
  //! into a cache key own the invalidation responsibility.
  //! @note Only pure rotation/translation transforms (scale == 1) are supported.
  //!       The method is a no-op and returns false if |scaleFactor| != 1.
  //! @param[in] theGraph  the graph containing the reference
  //! @param[in] theRefId  reference to move (any ref kind)
  //! @param[in] theTrsf   the transformation to compose into the location
  //! @return true on success; false if theTrsf has a non-unit scale factor
  Standard_EXPORT static bool MoveRef(BRepGraph&             theGraph,
                                      const BRepGraph_RefId& theRefId,
                                      const gp_Trsf&         theTrsf);

private:
  //! Apply location-only transform by storing per-node locations.
  static void applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf);

  BRepGraph_Transform() = delete;
};

#endif // _BRepGraph_Transform_HeaderFile
