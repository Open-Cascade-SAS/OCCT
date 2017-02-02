// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Extrema_Curve2dTool_HeaderFile
#define _Extrema_Curve2dTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
class Adaptor2d_Curve2d;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;



class Extrema_Curve2dTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Real FirstParameter (const Adaptor2d_Curve2d& C);
  
    static Standard_Real LastParameter (const Adaptor2d_Curve2d& C);
  
    static GeomAbs_Shape Continuity (const Adaptor2d_Curve2d& C);
  
  //! If necessary,   breaks the curve  in  intervals of
  //! continuity <S>.     And   returns  the  number  of
  //! intervals.
    static Standard_Integer NbIntervals (const Adaptor2d_Curve2d& C, const GeomAbs_Shape S);
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
    static void Intervals (const Adaptor2d_Curve2d& C, TColStd_Array1OfReal& T, const GeomAbs_Shape S);

  //! Returns the parameters bounding the intervals of subdivision of curve
  //! according to Curvature deflection. Value of deflection is defined in method.
  //!
    Standard_EXPORT static Handle(TColStd_HArray1OfReal) DeflCurvIntervals(const Adaptor2d_Curve2d& C);

    static Standard_Boolean IsClosed (const Adaptor2d_Curve2d& C);
  
    static Standard_Boolean IsPeriodic (const Adaptor2d_Curve2d& C);
  
    static Standard_Real Period (const Adaptor2d_Curve2d& C);
  
  //! Computes the point of parameter U on the curve.
    static gp_Pnt2d Value (const Adaptor2d_Curve2d& C, const Standard_Real U);
  
  //! Computes the point of parameter U on the curve.
    static void D0 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P);
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
    static void D1 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V);
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
    static void D2 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
    static void D3 (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
    static gp_Vec2d DN (const Adaptor2d_Curve2d& C, const Standard_Real U, const Standard_Integer N);
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
    static Standard_Real Resolution (const Adaptor2d_Curve2d& C, const Standard_Real R3d);
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
    static GeomAbs_CurveType GetType (const Adaptor2d_Curve2d& C);
  
    static gp_Lin2d Line (const Adaptor2d_Curve2d& C);
  
    static gp_Circ2d Circle (const Adaptor2d_Curve2d& C);
  
    static gp_Elips2d Ellipse (const Adaptor2d_Curve2d& C);
  
    static gp_Hypr2d Hyperbola (const Adaptor2d_Curve2d& C);
  
    static gp_Parab2d Parabola (const Adaptor2d_Curve2d& C);
  
    static Standard_Integer Degree (const Adaptor2d_Curve2d& C);
  
    static Standard_Boolean IsRational (const Adaptor2d_Curve2d& C);
  
    static Standard_Integer NbPoles (const Adaptor2d_Curve2d& C);
  
    static Standard_Integer NbKnots (const Adaptor2d_Curve2d& C);
  
    static Handle(Geom2d_BezierCurve) Bezier (const Adaptor2d_Curve2d& C);
  
    static Handle(Geom2d_BSplineCurve) BSpline (const Adaptor2d_Curve2d& C);




protected:





private:





};


#include <Extrema_Curve2dTool.lxx>





#endif // _Extrema_Curve2dTool_HeaderFile
