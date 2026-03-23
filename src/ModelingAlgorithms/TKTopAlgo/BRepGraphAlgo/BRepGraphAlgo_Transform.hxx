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

#ifndef _BRepGraphAlgo_Transform_HeaderFile
#define _BRepGraphAlgo_Transform_HeaderFile

#include <BRepGraph.hxx>

#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Trsf.hxx>

//! @brief Graph-based shape transformation.
//!
//! Applies a geometric transformation to a shape using pre-built BRepGraph
//! data. Two modes:
//! - theCopyGeom = false: applies transform as TopLoc_Location change (cheapest)
//! - theCopyGeom = true: deep-copies geometry first, then transforms in-place
//!
//! This avoids the 5-7 traversals of BRepTools_Modifier used by
//! BRepBuilderAPI_Transform.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   gp_Trsf aTrsf;
//!   aTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
//!   TopoDS_Shape aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf);
//! @endcode
class BRepGraphAlgo_Transform
{
public:
  DEFINE_STANDARD_ALLOC

  //! Transform the entire shape represented by the graph.
  //! @param[in] theGraph    a pre-built BRepGraph (must have IsDone() == true)
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming;
  //!                        if false, transform is applied via TopLoc_Location only
  //! @return the transformed shape
  Standard_EXPORT static TopoDS_Shape Perform(const BRepGraph& theGraph,
                                              const gp_Trsf&   theTrsf,
                                              bool             theCopyGeom = true);

  //! Transform a single face from the graph.
  //! @param[in] theGraph    a pre-built BRepGraph
  //! @param[in] theFaceIdx  face index in the graph
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming
  //! @return the transformed face
  Standard_EXPORT static TopoDS_Shape TransformFace(const BRepGraph& theGraph,
                                                    int              theFaceIdx,
                                                    const gp_Trsf&   theTrsf,
                                                    bool             theCopyGeom = true);

private:
  BRepGraphAlgo_Transform() = delete;
};

#endif // _BRepGraphAlgo_Transform_HeaderFile
