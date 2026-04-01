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

#ifndef _BRepGraphAlgo_Copy_HeaderFile
#define _BRepGraphAlgo_Copy_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>

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
//!   aGraph.Build(myShape);
//!   BRepGraph aCopy = BRepGraphAlgo_Copy::Perform(aGraph);
//!   TopoDS_Shape aShape = aCopy.Shapes().Shape();
//! @endcode
class BRepGraphAlgo_Copy
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
                                                         bool             theCopyGeom = true);

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

private:
  Standard_EXPORT static void transferCacheValues(const BRepGraph& theSrcGraph,
                                                  BRepGraph_NodeId theSrcNode,
                                                  BRepGraph&       theDstGraph,
                                                  BRepGraph_NodeId theDstNode);

  BRepGraphAlgo_Copy() = delete;
};

#endif // _BRepGraphAlgo_Copy_HeaderFile
