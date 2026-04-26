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
#include <BRepGraph_Builder.hxx>

#include <Standard_DefineAlloc.hxx>

//! @brief Graph-to-graph deep copy.
//!
//! Produces a new BRepGraph from an existing one in a single bottom-up pass,
//! avoiding the 5-7 traversals of BRepTools_Modifier used by BRepBuilderAPI_Copy.
//!
//! Two modes:
//! - theCopyGeom = true  (deep): geometry handles are cloned, result is fully independent.
//! - theCopyGeom = false (light): geometry handles are shared, only topology is duplicated.
//!
//! @note Unlike in-place mutation algorithms (Sewing, Deduplicate) which return a
//! Result struct with diagnostics, Copy and Transform return a BRepGraph directly
//! because they produce new graphs. Check IsDone() on the returned graph for success.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   BRepGraph_Builder::Add(aGraph, myShape);
//!   BRepGraph aCopy = BRepGraph_Copy::Perform(aGraph);
//!   TopoDS_Shape aShape = aCopy.Shapes().Shape();
//! @endcode
class BRepGraph_Copy
{
public:
  DEFINE_STANDARD_ALLOC

  //! Copy the entire graph.
  //! @param[in] theGraph    a pre-built BRepGraph (must have IsDone() == true)
  //! @param[in] theCopyGeom if true (default), geometry handles are deep-copied;
  //!                        if false, geometry is shared (only topology is duplicated)
  //! @return a new BRepGraph with IsDone() == true on success,
  //!         or an empty graph with IsDone() == false on failure
  [[nodiscard]] Standard_EXPORT static BRepGraph Perform(const BRepGraph& theGraph,
                                                         const bool       theCopyGeom = true);

  //! Copy a single face sub-graph.
  //! @param[in] theGraph        a pre-built BRepGraph
  //! @param[in] theFace         face definition identifier in the graph
  //! @param[in] theCopyGeom     if true, geometry is deep-copied
  //! @param[in] theReserveCache if true (default), pre-allocates transient cache;
  //!                            pass false for short-lived temporary graphs
  //! @return a new BRepGraph containing only the specified face and its dependencies
  [[nodiscard]] Standard_EXPORT static BRepGraph CopyFace(const BRepGraph&       theGraph,
                                                          const BRepGraph_FaceId theFace,
                                                          const bool             theCopyGeom = true,
                                                          const bool theReserveCache = true);

  //! Copy a single node sub-graph of any kind (Face, Shell, Solid, Wire, Edge, Vertex, etc.).
  //! The new graph contains only the specified node and all entities it references.
  //! @param[in] theGraph        a pre-built BRepGraph
  //! @param[in] theNodeId       node identifier (any kind)
  //! @param[in] theCopyGeom     if true, geometry handles are deep-copied
  //! @param[in] theReserveCache if true, pre-allocates transient cache
  //! @return a new BRepGraph containing only the specified sub-graph
  [[nodiscard]] Standard_EXPORT static BRepGraph CopyNode(const BRepGraph&       theGraph,
                                                          const BRepGraph_NodeId theNodeId,
                                                          const bool             theCopyGeom = true,
                                                          const bool theReserveCache = false);

private:
  //! Pre-allocate transient cache for lock-free parallel access.
  Standard_EXPORT static void reserveTransientCache(BRepGraph& theGraph);

  BRepGraph_Copy() = delete;
};

#endif // _BRepGraph_Copy_HeaderFile
