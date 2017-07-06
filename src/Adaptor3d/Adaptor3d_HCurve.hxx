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

#ifndef _Adaptor3d_HCurve_HeaderFile
#define _Adaptor3d_HCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor3d_Curve;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;
class Geom_OffsetCurve;


class Adaptor3d_HCurve;
DEFINE_STANDARD_HANDLE(Adaptor3d_HCurve, Standard_Transient)

//! Root class for 3D curves manipulated by handles, on
//! which geometric algorithms work.
//! An adapted curve is an interface between the
//! services provided by a curve and those required of
//! the curve by algorithms which use it.
//! Two derived concrete classes are provided:
//! - GeomAdaptor_HCurve for a curve from the Geom package
//! - Adaptor3d_HCurveOnSurface for a curve lying
//! on a surface from the Geom package.
class Adaptor3d_HCurve : public Standard_Transient
{

public:

  
  //! Returns a pointer to the Curve inside the HCurve.
  Standard_EXPORT virtual const Adaptor3d_Curve& Curve() const = 0;
  
  //! Returns a pointer to the Curve inside the HCurve.
  Standard_EXPORT virtual Adaptor3d_Curve& GetCurve() = 0;
  
    Standard_Real FirstParameter() const;
  
    Standard_Real LastParameter() const;
  
    GeomAbs_Shape Continuity() const;
  
    Standard_Integer NbIntervals (const GeomAbs_Shape S) const;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
    void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //!
  //! If <First> >= <Last>
    Handle(Adaptor3d_HCurve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
    Standard_Boolean IsClosed() const;
  
    Standard_Boolean IsPeriodic() const;
  
    Standard_Real Period() const;
  
    gp_Pnt Value (const Standard_Real U) const;
  
    void D0 (const Standard_Real U, gp_Pnt& P) const;
  
    void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const;
  
    void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const;
  
    void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const;
  
    gp_Vec DN (const Standard_Real U, const Standard_Integer N) const;
  
    Standard_Real Resolution (const Standard_Real R3d) const;
  
    GeomAbs_CurveType GetType() const;
  
    gp_Lin Line() const;
  
    gp_Circ Circle() const;
  
    gp_Elips Ellipse() const;
  
    gp_Hypr Hyperbola() const;
  
    gp_Parab Parabola() const;
  
    Standard_Integer Degree() const;
  
    Standard_Boolean IsRational() const;
  
    Standard_Integer NbPoles() const;
  
    Standard_Integer NbKnots() const;
  
    Handle(Geom_BezierCurve) Bezier() const;
  
    Handle(Geom_BSplineCurve) BSpline() const;

    Handle(Geom_OffsetCurve) OffsetCurve() const;


  DEFINE_STANDARD_RTTIEXT(Adaptor3d_HCurve,Standard_Transient)

protected:




private:




};


#include <Adaptor3d_HCurve.lxx>





#endif // _Adaptor3d_HCurve_HeaderFile
