// Created on: 1999-03-09
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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


#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <ShapeCustom_TrsfModification.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_TrsfModification,BRepTools_TrsfModification)

//=======================================================================
//function : ShapeCustom_TrsfModification
//purpose  : 
//=======================================================================
ShapeCustom_TrsfModification::ShapeCustom_TrsfModification(const gp_Trsf& T):
       BRepTools_TrsfModification(T)
{
}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewSurface(const TopoDS_Face& F,
							   Handle(Geom_Surface)& S,
							   TopLoc_Location& L,
							   Standard_Real& Tol,
							   Standard_Boolean& RevWires,
							   Standard_Boolean& RevFace) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewSurface(F, S, L, Tol, RevWires, RevFace);
  Tol = (*((Handle(BRep_TFace)*)&F.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewCurve(const TopoDS_Edge& E,
							 Handle(Geom_Curve)& C,
							 TopLoc_Location& L,
							 Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewCurve (E, C, L, Tol);
  Tol = (*((Handle(BRep_TEdge)*)&E.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewPoint(const TopoDS_Vertex& V,
							 gp_Pnt& P,
							 Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewPoint (V, P, Tol);
  Tol = (*((Handle(BRep_TVertex)*)&V.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewCurve2d(const TopoDS_Edge& E,
							   const TopoDS_Face& F,
							   const TopoDS_Edge& NewE,
							   const TopoDS_Face& NewF,
							   Handle(Geom2d_Curve)& C,
							   Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewCurve2d (E, F, NewE, NewF, C, Tol);
  Tol = (*((Handle(BRep_TEdge)*)&E.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeCustom_TrsfModification::NewParameter(const TopoDS_Vertex& V,
							     const TopoDS_Edge& E,
							     Standard_Real& P,
							     Standard_Real& Tol) 
{
  Standard_Boolean result = BRepTools_TrsfModification::NewParameter (V, E, P, Tol);
  Tol = (*((Handle(BRep_TVertex)*)&V.TShape()))->Tolerance() * Abs(Trsf().ScaleFactor());
  return result;
}

