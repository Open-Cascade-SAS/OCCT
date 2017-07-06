// Created on: 1994-02-23
// Created by: model
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

#ifndef _Adaptor2d_HCurve2d_HeaderFile
#define _Adaptor2d_HCurve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

class Adaptor2d_Curve2d;
class gp_Pnt2d;
class gp_Vec2d;


class Adaptor2d_HCurve2d;
DEFINE_STANDARD_HANDLE(Adaptor2d_HCurve2d, Standard_Transient)

//! Root class for 2D curves manipulated by handles, on
//! which geometric algorithms work.
//! An adapted curve is an interface between the
//! services provided by a curve, and those required of
//! the curve by algorithms, which use it.
//! A derived specific class is provided:
//! Geom2dAdaptor_HCurve for a curve from the Geom2d package.
class Adaptor2d_HCurve2d : public Standard_Transient
{

public:

  
  //! Returns a reference to the Curve2d inside the HCurve2d.
  Standard_EXPORT virtual const Adaptor2d_Curve2d& Curve2d() const = 0;
  
    Standard_Real FirstParameter() const;
  
    Standard_Real LastParameter() const;
  
    GeomAbs_Shape Continuity() const;
  
    Standard_Integer NbIntervals (const GeomAbs_Shape S) const;
  
    void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
  //! If <First> >= <Last>
    Handle(Adaptor2d_HCurve2d) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
    Standard_Boolean IsClosed() const;
  
    Standard_Boolean IsPeriodic() const;
  
    Standard_Real Period() const;
  
    gp_Pnt2d Value (const Standard_Real U) const;
  
    void D0 (const Standard_Real U, gp_Pnt2d& P) const;
  
    void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V) const;
  
    void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const;
  
    void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const;
  
    gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const;
  
    Standard_Real Resolution (const Standard_Real R3d) const;
  
    GeomAbs_CurveType GetType() const;
  
    gp_Lin2d Line() const;
  
    gp_Circ2d Circle() const;
  
    gp_Elips2d Ellipse() const;
  
    gp_Hypr2d Hyperbola() const;
  
    gp_Parab2d Parabola() const;
  
    Standard_Integer Degree() const;
  
    Standard_Boolean IsRational() const;
  
    Standard_Integer NbPoles() const;
  
    Standard_Integer NbKnots() const;
  
    Handle(Geom2d_BezierCurve) Bezier() const;
  
    virtual Handle(Geom2d_BSplineCurve) BSpline() const;


  DEFINE_STANDARD_RTTIEXT(Adaptor2d_HCurve2d,Standard_Transient)
};

#include <Adaptor2d_HCurve2d.lxx>

#endif // _Adaptor2d_HCurve2d_HeaderFile
