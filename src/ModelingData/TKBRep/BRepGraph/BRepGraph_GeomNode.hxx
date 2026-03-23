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

//! Namespace grouping all geometry node types for BRepGraph.
//!
//! Geometry nodes are pure geometry references — they hold only the
//! Handle<> and an Id.  All intrinsic data (location, triangulations,
//! pcurves) lives on the topology definitions (FaceDef, EdgeDef).
//! Deduplication during Build() ensures one node per unique geometry handle.
namespace BRepGraph_GeomNode
{

//! Graph node representing a unique Geom_Surface handle (pure geometry).
struct Surf
{
  BRepGraph_NodeId     Id;
  Handle(Geom_Surface) Surface;
};

//! Graph node representing a unique Geom_Curve handle (pure geometry).
struct Curve
{
  BRepGraph_NodeId   Id;
  Handle(Geom_Curve) CurveGeom;
};

} // namespace BRepGraph_GeomNode

#endif // _BRepGraph_GeomNode_HeaderFile
