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

#ifndef _BRepGraph_Copy_HeaderFile
#define _BRepGraph_Copy_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Graph-to-graph deep copy.
//!
//! Produces a new BRepGraph from an existing one in a single bottom-up pass,
//! avoiding the 5-7 traversals of BRepTools_Modifier used by BRepBuilderAPI_Copy.
//!
//! Two copy modes:
//! - External: source and target are different graphs. Target receives the copied data.
//! - Self-copy: source and target are the same graph. The specified sub-graph is
//!   duplicated with new entity IDs; shared dependencies (geometry, vertices referenced
//!   from outside the sub-graph) are preserved.
//!
//! Geometry and mesh policies are controlled by the GeomPolicy and MeshPolicy enums.
//!
//! @note Check the return value for success: Perform returns bool,
//! CopyNode returns the mapped root NodeId (invalid on failure).
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Shapes().Add(myShape);
//!   BRepGraph aCopy;
//!   BRepGraph_Copy::Perform(aGraph, aCopy);
//!   TopoDS_Shape aShape = aCopy.Shapes().Shape();
//! @endcode
class BRepGraph_Copy
{
public:
  DEFINE_STANDARD_ALLOC

  //! Policy for handling geometry handles (Geom_Curve, Geom_Surface, Geom2d_Curve).
  enum class GeomPolicy
  {
    Copy,  //!< Deep-clone geometry handles; result is fully independent
    Share, //!< Reuse source geometry handles; only topology is duplicated
    Drop   //!< Pure topology: edges carry no curves, faces carry no surfaces
  };

  //! Policy for handling mesh data (Poly_Triangulation, Poly_Polygon3D,
  //! Poly_PolygonOnTriangulation).
  enum class MeshPolicy
  {
    Copy,  //!< Deep-clone mesh data; independent result
    Share, //!< Reuse source mesh handle references; no cloning
    Drop   //!< Discard all mesh data on copied entities
  };

  //! Policy for handling transient runtime cache services.
  enum class CachePolicy
  {
    Drop,     //!< Do not copy runtime cache services or entries.
    CopyFresh //!< Copy fresh, remappable runtime cache entries.
  };

  //! Copy the entire source graph into the target graph.
  //!
  //! Self-copy (theSourceGraph == theTargetGraph):
  //! Identity no-op, returns true immediately.
  //!
  //! External copy to empty target (theTargetGraph.IsEmpty()):
  //! Uses identity-mapped fast path (old index == new index).
  //!
  //! External copy to non-empty target:
  //! Uses explicit mapping; IDs in theTargetGraph will differ from theSourceGraph.
  //! Entities from theSourceGraph are appended to theTargetGraph.
  //!
  //! @param[in] theSourceGraph a pre-built BRepGraph (must not be empty)
  //! @param[in,out] theTargetGraph destination graph (may already contain data)
  //! @param[in] theGeomPolicy geometry handle policy (default: Copy)
  //! @param[in] theMeshPolicy mesh data policy (default: Copy)
  //! @return true on success, false on failure (empty source)
  Standard_EXPORT static bool Perform(const BRepGraph& theSourceGraph,
                                      BRepGraph&       theTargetGraph,
                                      GeomPolicy       theGeomPolicy  = GeomPolicy::Copy,
                                      MeshPolicy       theMeshPolicy  = MeshPolicy::Copy,
                                      CachePolicy      theCachePolicy = CachePolicy::Drop);

  //! Copy a single node sub-graph of any kind (Face, Shell, Solid, Wire, Edge, Vertex, etc.).
  //! The target graph receives the specified node and all entities it references.
  //!
  //! External copy (theSourceGraph != theTargetGraph):
  //! New entities are appended to theTargetGraph. Entities already present
  //! in theTargetGraph are reused (not duplicated).
  //!
  //! Self-copy (theSourceGraph == theTargetGraph):
  //! The specified sub-graph is duplicated with new entity IDs within the same graph.
  //! Shared dependencies (vertices, edges referenced from outside the sub-graph)
  //! are preserved as-is.
  //!
  //! @param[in] theSourceGraph a pre-built BRepGraph
  //! @param[in,out] theTargetGraph destination graph (may already contain data)
  //! @param[in] theNodeId node identifier (any kind)
  //! @param[in] theGeomPolicy geometry handle policy (default: Copy)
  //! @param[in] theMeshPolicy mesh data policy (default: Copy)
  //! @return the mapped root NodeId in theTargetGraph, or invalid NodeId on failure
  [[nodiscard]] Standard_EXPORT static BRepGraph_NodeId CopyNode(
    const BRepGraph&       theSourceGraph,
    BRepGraph&             theTargetGraph,
    const BRepGraph_NodeId theNodeId,
    GeomPolicy             theGeomPolicy  = GeomPolicy::Copy,
    MeshPolicy             theMeshPolicy  = MeshPolicy::Copy,
    CachePolicy            theCachePolicy = CachePolicy::Drop);

  BRepGraph_Copy() = delete;
};

#endif // _BRepGraph_Copy_HeaderFile
