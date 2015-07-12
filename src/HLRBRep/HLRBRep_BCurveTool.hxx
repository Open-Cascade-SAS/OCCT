// Created on: 1995-07-17
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

#ifndef _HLRBRep_BCurveTool_HeaderFile
#define _HLRBRep_BCurveTool_HeaderFile

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
#include <TColgp_Array1OfPnt.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class BRepAdaptor_Curve;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;



class HLRBRep_BCurveTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Real FirstParameter (const BRepAdaptor_Curve& C);
  
    static Standard_Real LastParameter (const BRepAdaptor_Curve& C);
  
    static GeomAbs_Shape Continuity (const BRepAdaptor_Curve& C);
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
    static Standard_Integer NbIntervals (const BRepAdaptor_Curve& C, const GeomAbs_Shape S);
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
    static void Intervals (const BRepAdaptor_Curve& C, TColStd_Array1OfReal& T, const GeomAbs_Shape S);
  
    static Standard_Boolean IsClosed (const BRepAdaptor_Curve& C);
  
    static Standard_Boolean IsPeriodic (const BRepAdaptor_Curve& C);
  
    static Standard_Real Period (const BRepAdaptor_Curve& C);
  
  //! Computes the point of parameter U on the curve.
    static gp_Pnt Value (const BRepAdaptor_Curve& C, const Standard_Real U);
  
  //! Computes the point of parameter U on the curve.
    static void D0 (const BRepAdaptor_Curve& C, const Standard_Real U, gp_Pnt& P);
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
    static void D1 (const BRepAdaptor_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V);
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
    static void D2 (const BRepAdaptor_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
    static void D3 (const BRepAdaptor_Curve& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
    static gp_Vec DN (const BRepAdaptor_Curve& C, const Standard_Real U, const Standard_Integer N);
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
    static Standard_Real Resolution (const BRepAdaptor_Curve& C, const Standard_Real R3d);
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
    static GeomAbs_CurveType GetType (const BRepAdaptor_Curve& C);
  
    static gp_Lin Line (const BRepAdaptor_Curve& C);
  
    static gp_Circ Circle (const BRepAdaptor_Curve& C);
  
    static gp_Elips Ellipse (const BRepAdaptor_Curve& C);
  
    static gp_Hypr Hyperbola (const BRepAdaptor_Curve& C);
  
    static gp_Parab Parabola (const BRepAdaptor_Curve& C);
  
  Standard_EXPORT static Handle(Geom_BezierCurve) Bezier (const BRepAdaptor_Curve& C);
  
  Standard_EXPORT static Handle(Geom_BSplineCurve) BSpline (const BRepAdaptor_Curve& C);
  
    static Standard_Integer Degree (const BRepAdaptor_Curve& C);
  
    static Standard_Boolean IsRational (const BRepAdaptor_Curve& C);
  
    static Standard_Integer NbPoles (const BRepAdaptor_Curve& C);
  
    static Standard_Integer NbKnots (const BRepAdaptor_Curve& C);
  
  Standard_EXPORT static void Poles (const BRepAdaptor_Curve& C, TColgp_Array1OfPnt& T);
  
  Standard_EXPORT static void PolesAndWeights (const BRepAdaptor_Curve& C, TColgp_Array1OfPnt& T, TColStd_Array1OfReal& W);
  
  Standard_EXPORT static Standard_Integer NbSamples (const BRepAdaptor_Curve& C, const Standard_Real U0, const Standard_Real U1);




protected:





private:





};


#include <HLRBRep_BCurveTool.lxx>





#endif // _HLRBRep_BCurveTool_HeaderFile
