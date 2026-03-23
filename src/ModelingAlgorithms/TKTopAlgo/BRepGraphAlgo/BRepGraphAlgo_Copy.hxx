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

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>

//! @brief Graph-based deep copy of a TopoDS_Shape.
//!
//! Performs a complete deep copy of a shape using pre-built BRepGraph data
//! in a single bottom-up pass, avoiding the 5-7 traversals of
//! BRepTools_Modifier used by BRepBuilderAPI_Copy.
//!
//! ## Algorithm
//! Phase 1: Copy unique geometry handles (surfaces, 3D curves, 2D curves).
//! Phase 2: Bottom-up topology rebuild: Vertex -> Edge -> Wire -> Face -> Shell -> Solid.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   TopoDS_Shape aCopy = BRepGraphAlgo_Copy::Perform(aGraph);
//! @endcode
class BRepGraphAlgo_Copy
{
public:
  DEFINE_STANDARD_ALLOC

  //! Deep-copy the entire shape represented by the graph.
  //! @param[in] theGraph   a pre-built BRepGraph (must have IsDone() == true)
  //! @param[in] theCopyGeom if true (default), geometry handles are deep-copied;
  //!                        if false, geometry is shared (only topology is duplicated)
  //! @return the deep-copied shape
  Standard_EXPORT static TopoDS_Shape Perform(const BRepGraph& theGraph,
                                              bool             theCopyGeom = true);

  //! Deep-copy a single face from the graph.
  //! @param[in] theGraph   a pre-built BRepGraph
  //! @param[in] theFaceIdx face index in the graph
  //! @param[in] theCopyGeom if true, geometry is deep-copied
  //! @return the deep-copied face
  Standard_EXPORT static TopoDS_Shape CopyFace(const BRepGraph& theGraph,
                                               int              theFaceIdx,
                                               bool             theCopyGeom = true);

private:
  BRepGraphAlgo_Copy() = delete;
};

#endif // _BRepGraphAlgo_Copy_HeaderFile
