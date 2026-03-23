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

#ifndef _BRepGraph_GeomNode_HeaderFile
#define _BRepGraph_GeomNode_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_UID.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Poly_Triangulation.hxx>

#include <NCollection_Vector.hxx>

//! Namespace grouping all geometry node types for BRepGraph.
//!
//! Geometry nodes are non-owning references to existing OCCT Handle<>
//! objects.  Deduplication during Build() ensures one node per unique
//! geometry handle.
namespace BRepGraph_GeomNode
{

//! Graph node representing a unique Geom_Surface handle.
//!
//! Multiple Face nodes may reference one Surf node; this is the
//! "same domain" relationship.  FaceUsers enables reverse queries.
struct Surf
{
  BRepGraph_NodeId           Id;
  BRepGraph_UID              UID;
  Handle(Geom_Surface)       Surface;        //!< The shared geometry
  Handle(Poly_Triangulation) Triangulation;  //!< Optional cached mesh

  //! Back-references: all Face nodes realized by this surface.
  NCollection_Vector<BRepGraph_NodeId> FaceUsers;

  //! True if referenced by Face nodes with different GlobalLocations.
  bool IsMultiLocated = false;
};

//! Graph node representing a unique Geom_Curve handle.
struct Curve
{
  BRepGraph_NodeId           Id;
  BRepGraph_UID              UID;
  Handle(Geom_Curve)         CurveGeom;

  //! Back-references: all Edge nodes realized by this curve.
  NCollection_Vector<BRepGraph_NodeId> EdgeUsers;

  //! True if referenced by Edge nodes with different GlobalLocations.
  bool IsMultiLocated = false;
};

//! Graph node representing a parametric 2D curve in a specific (Edge, Face)
//! context.  A pcurve is never shared: each (Edge, Face) pair produces
//! exactly one PCurve node.
struct PCurve
{
  BRepGraph_NodeId           Id;
  BRepGraph_UID              UID;
  Handle(Geom2d_Curve)       Curve2d;

  BRepGraph_NodeId           EdgeContext;  //!< The edge this pcurve belongs to
  BRepGraph_NodeId           FaceContext;  //!< The face whose UV space it lives in

  double                     ParamFirst = 0.0;
  double                     ParamLast  = 0.0;
};

} // namespace BRepGraph_GeomNode

#endif // _BRepGraph_GeomNode_HeaderFile
