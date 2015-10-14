// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_TheHCurveTool_HeaderFile
#define _IntCurveSurface_TheHCurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

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
#include <TColStd_HArray1OfReal.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor3d_HCurve;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;



class IntCurveSurface_TheHCurveTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Real FirstParameter (const Handle(Adaptor3d_HCurve)& C);
  
    static Standard_Real LastParameter (const Handle(Adaptor3d_HCurve)& C);
  
    static GeomAbs_Shape Continuity (const Handle(Adaptor3d_HCurve)& C);
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
    static Standard_Integer NbIntervals (const Handle(Adaptor3d_HCurve)& C, const GeomAbs_Shape S);
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
    static void Intervals (const Handle(Adaptor3d_HCurve)& C, TColStd_Array1OfReal& T, const GeomAbs_Shape S);
  
    static Standard_Boolean IsClosed (const Handle(Adaptor3d_HCurve)& C);
  
    static Standard_Boolean IsPeriodic (const Handle(Adaptor3d_HCurve)& C);
  
    static Standard_Real Period (const Handle(Adaptor3d_HCurve)& C);
  
  //! Computes the point of parameter U on the curve.
    static gp_Pnt Value (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U);
  
  //! Computes the point of parameter U on the curve.
    static void D0 (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U, gp_Pnt& P);
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
    static void D1 (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V);
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
    static void D2 (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
    static void D3 (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
    static gp_Vec DN (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U, const Standard_Integer N);
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
    static Standard_Real Resolution (const Handle(Adaptor3d_HCurve)& C, const Standard_Real R3d);
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
    static GeomAbs_CurveType GetType (const Handle(Adaptor3d_HCurve)& C);
  
    static gp_Lin Line (const Handle(Adaptor3d_HCurve)& C);
  
    static gp_Circ Circle (const Handle(Adaptor3d_HCurve)& C);
  
    static gp_Elips Ellipse (const Handle(Adaptor3d_HCurve)& C);
  
    static gp_Hypr Hyperbola (const Handle(Adaptor3d_HCurve)& C);
  
    static gp_Parab Parabola (const Handle(Adaptor3d_HCurve)& C);
  
    static Handle(Geom_BezierCurve) Bezier (const Handle(Adaptor3d_HCurve)& C);
  
    static Handle(Geom_BSplineCurve) BSpline (const Handle(Adaptor3d_HCurve)& C);
  
  Standard_EXPORT static Standard_Integer NbSamples (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U0, const Standard_Real U1);
  
  Standard_EXPORT static void SamplePars (const Handle(Adaptor3d_HCurve)& C, const Standard_Real U0, const Standard_Real U1, const Standard_Real Defl, const Standard_Integer NbMin, Handle(TColStd_HArray1OfReal)& Pars);




protected:





private:





};

#define CurveGen Handle(Adaptor3d_HCurve)
#define CurveGen_hxx <Adaptor3d_HCurve.hxx>
#define IntCurveSurface_HCurveTool IntCurveSurface_TheHCurveTool
#define IntCurveSurface_HCurveTool_hxx <IntCurveSurface_TheHCurveTool.hxx>

#include <IntCurveSurface_HCurveTool.lxx>

#undef CurveGen
#undef CurveGen_hxx
#undef IntCurveSurface_HCurveTool
#undef IntCurveSurface_HCurveTool_hxx




#endif // _IntCurveSurface_TheHCurveTool_HeaderFile
