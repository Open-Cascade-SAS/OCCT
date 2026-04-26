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
//! Two modes (matching BRepBuilderAPI_Transform semantics):
//! - theCopyGeom = true  (geometry-level): deep-copy geometry, transform handles
//!   in-place via Geom_Surface::Transform() etc., reset locations to identity.
//!   Triangulations are invalidated.
//! - theCopyGeom = false (root-level): light-copy with shared geometry, apply
//!   transform via location modification only. Triangulations remain valid.
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
  //! @return a new BRepGraph with the transformation applied
  [[nodiscard]] Standard_EXPORT static BRepGraph Perform(const BRepGraph& theGraph,
                                                         const gp_Trsf&   theTrsf,
                                                         const bool       theCopyGeom = true);

  //! Transform a single face sub-graph.
  //! @param[in] theGraph    a pre-built BRepGraph
  //! @param[in] theFace     face definition identifier in the graph
  //! @param[in] theTrsf     the transformation to apply
  //! @param[in] theCopyGeom if true, geometry is deep-copied before transforming
  //! @return a new BRepGraph containing only the specified face, transformed
  [[nodiscard]] Standard_EXPORT static BRepGraph TransformFace(const BRepGraph&       theGraph,
                                                               const BRepGraph_FaceId theFace,
                                                               const gp_Trsf&         theTrsf,
                                                               const bool theCopyGeom = true);

private:
  //! Apply location-only transform by storing per-node locations.
  static void applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf);

  BRepGraph_Transform() = delete;
};

#endif // _BRepGraph_Transform_HeaderFile
