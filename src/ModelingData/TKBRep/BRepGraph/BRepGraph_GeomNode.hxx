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

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Poly_Triangulation.hxx>

#include <NCollection_Vector.hxx>

//! Namespace grouping all geometry node types for BRepGraph.
//!
//! Geometry nodes are non-owning references to existing OCCT Handle<>
//! objects.  Deduplication during Build() ensures one node per unique
//! geometry handle.
//!
//! Back-references store definition NodeIds (not usages), since geometry
//! is shared at the definition level.
namespace BRepGraph_GeomNode
{

//! Graph node representing a unique Geom_Surface handle.
//!
//! Multiple Face definitions may reference one Surf node; this is the
//! "same domain" relationship.  FaceDefUsers enables reverse queries.
struct Surf
{
  BRepGraph_NodeId           Id;
  Handle(Geom_Surface)       Surface;        //!< The shared geometry
  Handle(Poly_Triangulation) Triangulation;  //!< Optional cached mesh

  //! Back-references: all Face definitions realized by this surface.
  NCollection_Vector<BRepGraph_NodeId> FaceDefUsers;

  //! True if referenced by Face definitions whose usages have different GlobalLocations.
  bool IsMultiLocated = false;
};

//! Graph node representing a unique Geom_Curve handle.
struct Curve
{
  BRepGraph_NodeId           Id;
  Handle(Geom_Curve)         CurveGeom;

  //! Back-references: all Edge definitions realized by this curve.
  NCollection_Vector<BRepGraph_NodeId> EdgeDefUsers;

  //! True if referenced by Edge definitions whose usages have different GlobalLocations.
  bool IsMultiLocated = false;
};

//! Graph node representing a parametric 2D curve in a specific (Edge, Face)
//! context.  A pcurve is never shared: each (Edge, Face, Orientation) triple produces
//! exactly one PCurve node.  For seam edges, two PCurve nodes exist for the
//! same (Edge, Face) pair, distinguished by edge orientation (FORWARD vs REVERSED).
struct PCurve
{
  BRepGraph_NodeId           Id;
  Handle(Geom2d_Curve)       Curve2d;

  BRepGraph_NodeId           EdgeContext;  //!< The edge def this pcurve belongs to
  BRepGraph_NodeId           FaceContext;  //!< The face def whose UV space it lives in

  double                     ParamFirst = 0.0;
  double                     ParamLast  = 0.0;

  //! Geometric continuity across the edge (from BRep_CurveOnClosedSurface).
  //! GeomAbs_C0 for most edges; higher values encode G1/G2 continuity.
  GeomAbs_Shape              Continuity = GeomAbs_C0;
};

} // namespace BRepGraph_GeomNode

#endif // _BRepGraph_GeomNode_HeaderFile
