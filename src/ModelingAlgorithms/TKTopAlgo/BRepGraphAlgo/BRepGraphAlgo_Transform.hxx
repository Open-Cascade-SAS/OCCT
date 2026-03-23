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
#include <gp_Trsf.hxx>

//! @brief Graph-to-graph transformation.
//!
//! Produces a new BRepGraph by copying and then applying a geometric
//! transformation to vertex points and geometry node locations.
//!
//! Two modes (matching BRepBuilderAPI_Transform semantics):
//! - theCopyGeom = true  (geometry-level): deep-copy geometry, transform handles
//!   in-place via Geom_Surface::Transform() etc., reset locations to identity.
//!   Triangulations are invalidated.
//! - theCopyGeom = false (root-level): light-copy with shared geometry, apply
//!   transform via location modification only. Triangulations remain valid.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   gp_Trsf aTrsf;
//!   aTrsf.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
//!   BRepGraph aTransformed = BRepGraphAlgo_Transform::Perform(aGraph, aTrsf);
//!   TopoDS_Shape aShape = aTransformed.Shapes().Shape();
//! @endcode
class BRepGraphAlgo_Transform
{
public:
  DEFINE_STANDARD_ALLOC

  //! Transform the entire graph.
  //! @param[in] theGraph    a pre-built BRepGraph (must have IsDone() == true)
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming;
  //!                        if false, light-copy then transform locations/points only
  //! @return a new BRepGraph with the transformation applied
  Standard_EXPORT static BRepGraph Perform(const BRepGraph& theGraph,
                                           const gp_Trsf&   theTrsf,
                                           bool             theCopyGeom = true);

  //! Transform a single face sub-graph.
  //! @param[in] theGraph    a pre-built BRepGraph
  //! @param[in] theFaceIdx  face definition index in the graph
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming
  //! @return a new BRepGraph containing only the specified face, transformed
  Standard_EXPORT static BRepGraph TransformFace(const BRepGraph& theGraph,
                                                 int              theFaceIdx,
                                                 const gp_Trsf&   theTrsf,
                                                 bool             theCopyGeom = true);

private:
  //! Apply location-only transform by storing per-node locations.
  static void applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf);

  BRepGraphAlgo_Transform() = delete;
};

#endif // _BRepGraphAlgo_Transform_HeaderFile
