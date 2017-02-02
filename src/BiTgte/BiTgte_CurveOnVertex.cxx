// Created on: 1997-03-21
// Created by: Bruno DUMORTIER
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <BiTgte_CurveOnVertex.hxx>
#include <BiTgte_HCurveOnVertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : BiTgte_CurveOnVertex
//purpose  : 
//======================================================================
BiTgte_CurveOnVertex::BiTgte_CurveOnVertex()
: myFirst(0.0), myLast(0.0)
{
}


//=======================================================================
//function : BiTgte_CurveOnVertex
//purpose  : 
//=======================================================================

BiTgte_CurveOnVertex::BiTgte_CurveOnVertex(const TopoDS_Edge&   theEonF,
                                           const TopoDS_Vertex& theVertex)
: myFirst(0.0), myLast(0.0)
{
  Init(theEonF, theVertex);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::Init(const TopoDS_Edge&   EonF,
                                const TopoDS_Vertex& V)
{
  BRep_Tool::Range(EonF,myFirst,myLast);
  myPnt = BRep_Tool::Pnt(V);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::FirstParameter() const
{
  return myFirst;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::LastParameter() const
{
  return myLast;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

GeomAbs_Shape BiTgte_CurveOnVertex::Continuity() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbIntervals(const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::Intervals(TColStd_Array1OfReal&,
                                     const GeomAbs_Shape) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) BiTgte_CurveOnVertex::Trim(const Standard_Real,
                                                    const Standard_Real,
                                                    const Standard_Real) 
const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsClosed() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsPeriodic() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::Period() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Pnt BiTgte_CurveOnVertex::Value(const Standard_Real) const
{
  return myPnt;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D0(const Standard_Real /*U*/,gp_Pnt& P) const
{
  P = myPnt;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D1(const Standard_Real ,gp_Pnt& ,gp_Vec&) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D2(const Standard_Real,gp_Pnt&,
                              gp_Vec& ,gp_Vec& ) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void BiTgte_CurveOnVertex::D3(const Standard_Real,
                              gp_Pnt& ,
                              gp_Vec& ,
                              gp_Vec& ,
                              gp_Vec& ) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Vec BiTgte_CurveOnVertex::DN(const Standard_Real,
                                const Standard_Integer) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Real BiTgte_CurveOnVertex::Resolution(const Standard_Real) const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

GeomAbs_CurveType BiTgte_CurveOnVertex::GetType() const
{
  return GeomAbs_OtherCurve;
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Lin BiTgte_CurveOnVertex::Line() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Circ BiTgte_CurveOnVertex::Circle() const
{
  throw Standard_NoSuchObject("BiTgte_CurveOnVertex::Circle");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Elips BiTgte_CurveOnVertex::Ellipse() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Hypr BiTgte_CurveOnVertex::Hyperbola() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

gp_Parab BiTgte_CurveOnVertex::Parabola() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::Degree() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BiTgte_CurveOnVertex::IsRational() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbPoles() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Integer BiTgte_CurveOnVertex::NbKnots() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) BiTgte_CurveOnVertex::Bezier() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) BiTgte_CurveOnVertex::BSpline() const
{
  throw Standard_NotImplemented("BiTgte_CurveOnVertex");
}


