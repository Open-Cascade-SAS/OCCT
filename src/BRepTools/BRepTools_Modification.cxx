// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BRepTools_Modification.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_Modification,Standard_Transient)

Standard_Boolean BRepTools_Modification::NewTriangulation(const TopoDS_Face&, Handle(Poly_Triangulation)&)
{
  return Standard_False;
}

Standard_Boolean BRepTools_Modification::NewPolygon(const TopoDS_Edge&, Handle(Poly_Polygon3D)&)
{
  return Standard_False;
}

Standard_Boolean BRepTools_Modification::NewPolygonOnTriangulation(const TopoDS_Edge&, const TopoDS_Face&, Handle(Poly_PolygonOnTriangulation)&)
{
  return Standard_False;
}
