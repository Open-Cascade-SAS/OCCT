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
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Trsf.hxx>

//! @brief Graph-to-graph transformation.
//!
//! Applies a geometric transformation to vertex points and geometry node
//! locations by copying into a target graph, then transforming in-place.
//!
//! Two geometry modes (matching BRepBuilderAPI_Transform semantics):
//! - GeomPolicy::Copy (geometry-level): deep-copy geometry, create new
//!   transformed handles via Geom_Geometry::Transformed(), reset locations
//!   to identity.
//! - GeomPolicy::Share (root-level): light-copy with shared geometry, apply
//!   transform via location modification only.
//!
//! Mesh handling (MeshPolicy parameter):
//! - MeshPolicy::Drop (default for Transform): triangulations and polygons are
//!   discarded after a geometry-level transform and must be recomputed.
//! - MeshPolicy::Copy: all mesh data (Poly_Triangulation on FaceDefs and the
//!   MeshLayer cache, Poly_Polygon3D on edges, Poly_PolygonOnTriangulation on
//!   coedges) is copied and transformed in sync with the geometry.
//!   In location-only mode the mesh data is copied as-is (nodes stay in the
//!   graph coordinate system, which is unaffected by a pure location compose).
//!
//! @note Check the return value for success: Perform returns bool,
//! TransformNode returns the mapped root NodeId (invalid on failure).
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Shapes().Add(myShape);
//!   gp_Trsf aTrsf;
//!   aTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
//!   BRepGraph aTransformed;
//!   BRepGraph_Transform::Perform(aGraph, aTransformed, aTrsf);
//!   TopoDS_Shape aShape = aTransformed.Shapes().Shape();
//! @endcode
class BRepGraph_Transform
{
public:
  DEFINE_STANDARD_ALLOC

  //! Transform the entire graph into a target graph.
  //!
  //! Self-transform (theSourceGraph == theTargetGraph):
  //! Applies transform in-place on theTargetGraph.
  //!
  //! External transform to empty target (theTargetGraph.IsEmpty()):
  //! Copies source into target, then transforms.
  //!
  //! External transform to non-empty target:
  //! Appends source entities into target with explicit mapping, then transforms.
  //!
  //! @param[in] theSourceGraph a pre-built BRepGraph (must not be empty)
  //! @param[in,out] theTargetGraph destination graph (may already contain data)
  //! @param[in] theTrsf       the transformation to apply
  //! @param[in] theGeomPolicy geometry handle policy (default: Copy)
  //! @param[in] theMeshPolicy mesh data policy (default: Drop)
  //! @return true on success, false on failure (empty source, or Drop +
  //! geometry-modification-required)
  Standard_EXPORT static bool Perform(
    const BRepGraph&                 theSourceGraph,
    BRepGraph&                       theTargetGraph,
    const gp_Trsf&                   theTrsf,
    const BRepGraph_Copy::GeomPolicy theGeomPolicy = BRepGraph_Copy::GeomPolicy::Copy,
    const BRepGraph_Copy::MeshPolicy theMeshPolicy = BRepGraph_Copy::MeshPolicy::Drop);

  //! Transform a single node sub-graph of any kind.
  //! Topology nodes are copied and transformed by baking the transform into their definitions.
  //!
  //! Self-transform (theSourceGraph == theTargetGraph):
  //! Duplicates the sub-graph with new entity IDs, then transforms the copy.
  //!
  //! External transform:
  //! Copies the sub-graph into theTargetGraph, then transforms.
  //!
  //! @param[in] theSourceGraph a pre-built BRepGraph
  //! @param[in,out] theTargetGraph destination graph (may already contain data)
  //! @param[in] theNodeId     node identifier (any kind)
  //! @param[in] theTrsf       the transformation to apply
  //! @param[in] theGeomPolicy geometry handle policy (default: Copy; Drop is invalid for topology)
  //! @param[in] theMeshPolicy mesh data policy (default: Drop)
  //! @return the mapped root NodeId in theTargetGraph, or invalid NodeId on failure
  [[nodiscard]] Standard_EXPORT static BRepGraph_NodeId TransformNode(
    const BRepGraph&                 theSourceGraph,
    BRepGraph&                       theTargetGraph,
    const BRepGraph_NodeId           theNodeId,
    const gp_Trsf&                   theTrsf,
    const BRepGraph_Copy::GeomPolicy theGeomPolicy = BRepGraph_Copy::GeomPolicy::Copy,
    const BRepGraph_Copy::MeshPolicy theMeshPolicy = BRepGraph_Copy::MeshPolicy::Drop);

  //! Apply an in-place location-only transform to a child reference.
  //! Composes theTrsf into ChildRef placement without copying any geometry.
  //! Cached mesh data on entities downstream of the moved ref is stored in the
  //! entity's local frame and is unaffected; callers that bake a world transform
  //! into a cache key own the invalidation responsibility.
  //! @note Only pure rotation/translation transforms (scale == 1) are supported.
  //!       The method returns false if |scaleFactor| != 1.
  //! @param[in] theGraph  the graph containing the reference
  //! @param[in] theRefId  child reference to move
  //! @param[in] theTrsf   the transformation to compose into the location
  //! @return true on success; false if the ref is invalid/removed or theTrsf has non-unit scale
  Standard_EXPORT static bool MoveRef(BRepGraph&                 theGraph,
                                      const BRepGraph_ChildRefId theRefId,
                                      const gp_Trsf&             theTrsf);

  //! Apply an in-place location-only transform to an occurrence reference.
  //! Composes theTrsf into OccurrenceRef placement without copying any geometry.
  //! @note Only pure rotation/translation transforms (scale == 1) are supported.
  //! @return true on success; false if the ref is invalid/removed or theTrsf has non-unit scale
  Standard_EXPORT static bool MoveRef(BRepGraph&                      theGraph,
                                      const BRepGraph_OccurrenceRefId theRefId,
                                      const gp_Trsf&                  theTrsf);

  BRepGraph_Transform() = delete;

private:
  //! Apply location-only transform by storing per-node locations.
  static void applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf);
};

#endif // _BRepGraph_Transform_HeaderFile
