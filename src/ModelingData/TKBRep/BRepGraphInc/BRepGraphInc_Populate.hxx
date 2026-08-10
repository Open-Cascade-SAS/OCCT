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

#ifndef _BRepGraphInc_Populate_HeaderFile
#define _BRepGraphInc_Populate_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TopoDS_Shape;
class BRepGraph;

//! @brief Backend topology/geometry population for BRepGraph.
//!
//! This class is part of the BRepGraphInc backend and is intended for
//! backend maintenance, tests, and low-level infrastructure only.
//! External code should enter through BRepGraph::ShapesView::Add(), which owns the
//! public lifecycle, cache invalidation, and layer coordination.
//!
//! The builder stores forward child relations only. Reverse relations are rebuilt
//! by BRepGraphInc_Storage after population.
class BRepGraphInc_Populate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Result of a build operation.
  enum class BuildStatus
  {
    Success,             //!< All faces built successfully.
    SuccessWithWarnings, //!< Build completed with diagnostics, e.g. unbounded natural faces.
    Failed               //!< Build failed (e.g., null shape, internal error).
  };

  //! Options controlling population.
  struct Options
  {
  };

  //! Build backend incidence storage from a TopoDS_Shape.
  //! @param[out] theGraph    graph whose storage to populate (cleared first)
  //! @param[in]  theShape    root shape
  //! @param[in]  theParallel if true, face-level extraction runs in parallel
  //! @param[in]  theOptions  optional post-pass controls
  //! @return build status indicating success, warnings, or failure
  [[nodiscard]] static Standard_EXPORT BuildStatus Perform(BRepGraph&          theGraph,
                                                           const TopoDS_Shape& theShape,
                                                           bool                theParallel,
                                                           const Options& theOptions = Options());

  //! Extend existing backend storage with additional shapes (no clear).
  //! Flattens hierarchy containers away; Solid/Shell/Compound/CompSolid inputs
  //! contribute appended face roots instead of container entities.
  //! Recomputes the built-in metadata layers from the populated storage.
  //! @param[in,out] theGraph         graph whose storage to extend
  //! @param[in]     theShape         shape to append
  //! @param[in]     theParallel      if true, face-level extraction runs in parallel
  //! @param[out]    theAppendedRoots collected root NodeIds for non-container shapes
  //! @param[in]     theOptions       optional post-pass controls
  //! @return build status indicating success, warnings, or failure
  [[nodiscard]] static Standard_EXPORT BuildStatus
    AppendFlattened(BRepGraph&                                  theGraph,
                    const TopoDS_Shape&                         theShape,
                    bool                                        theParallel,
                    NCollection_LinearVector<BRepGraph_NodeId>& theAppendedRoots,
                    const Options&                              theOptions = Options());

  //! Extend existing backend storage with additional shapes (no clear).
  //! Preserves the full shape hierarchy: Solid/Shell/Compound/CompSolid nodes
  //! are created alongside Face/Edge/Vertex nodes. Shapes already present in
  //! the storage with the same definition identity (TShape + Location, orientation ignored)
  //! are deduplicated and not re-added.
  //! @param[in,out] theGraph    graph whose storage to extend
  //! @param[in]     theShape    shape to append
  //! @param[in]     theParallel if true, face-level extraction runs in parallel
  //! @param[in]     theOptions  optional post-pass controls
  //! @return build status indicating success, warnings, or failure
  [[nodiscard]] static Standard_EXPORT BuildStatus Append(BRepGraph&          theGraph,
                                                          const TopoDS_Shape& theShape,
                                                          bool                theParallel,
                                                          const Options& theOptions = Options());

  BRepGraphInc_Populate() = delete;
};

#endif // _BRepGraphInc_Populate_HeaderFile
