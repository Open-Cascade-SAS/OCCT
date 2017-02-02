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

#ifndef _Extrema_CurveTool_HeaderFile
#define _Extrema_CurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
class Adaptor3d_Curve;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;



class Extrema_CurveTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Real FirstParameter (const Adaptor3d_Curve& C);
  
    static Standard_Real LastParameter (const Adaptor3d_Curve& C);
  
    static GeomAbs_Shape Continuity (const Adaptor3d_Curve& C);
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
    static Standard_Integer NbIntervals (Adaptor3d_Curve& C, const GeomAbs_Shape S);
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
    static void Intervals (Adaptor3d_Curve& C, TColStd_Array1OfReal& T, const GeomAbs_Shape S);

  //! Returns the parameters bounding the intervals of subdivision of curve
  //! according to Curvature deflection. Value of deflection is defined in method.
  //!
    Standard_EXPORT static Handle(TColStd_HArray1OfReal) DeflCurvIntervals(const Adaptor3d_Curve& C);

    Standard_EXPORT static Standard_Boolean IsPeriodic (const Adaptor3d_Curve& C);
  
    static Standard_Real Period (const Adaptor3d_Curve& C);
  
    static Standard_Real Resolution (const Adaptor3d_Curve& C, const Standard_Real R3d);
  
    static GeomAbs_CurveType GetType (const Adaptor3d_Curve& C);
  
    static gp_Pnt Value (const Adaptor3d_Curve& C, const Standard_Real U);
  
    static void D0 (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P);
  
    static void D1 (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V);
  
    static void D2 (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
    static void D3 (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  
    static gp_Vec DN (const Adaptor3d_Curve& C, const Standard_Real U, const Standard_Integer N);
  
    static gp_Lin Line (const Adaptor3d_Curve& C);
  
    static gp_Circ Circle (const Adaptor3d_Curve& C);
  
    static gp_Elips Ellipse (const Adaptor3d_Curve& C);
  
    static gp_Hypr Hyperbola (const Adaptor3d_Curve& C);
  
    static gp_Parab Parabola (const Adaptor3d_Curve& C);
  
    static Standard_Integer Degree (const Adaptor3d_Curve& C);
  
    static Standard_Boolean IsRational (const Adaptor3d_Curve& C);
  
    static Standard_Integer NbPoles (const Adaptor3d_Curve& C);
  
    static Standard_Integer NbKnots (const Adaptor3d_Curve& C);
  
    static Handle(Geom_BezierCurve) Bezier (const Adaptor3d_Curve& C);
  
    static Handle(Geom_BSplineCurve) BSpline (const Adaptor3d_Curve& C);




protected:





private:





};


#include <Extrema_CurveTool.lxx>





#endif // _Extrema_CurveTool_HeaderFile
