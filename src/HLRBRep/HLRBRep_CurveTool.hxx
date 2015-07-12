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

#ifndef _HLRBRep_CurveTool_HeaderFile
#define _HLRBRep_CurveTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
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
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;



class HLRBRep_CurveTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
    static Standard_Real FirstParameter (const Standard_Address C);
  
    static Standard_Real LastParameter (const Standard_Address C);
  
    static GeomAbs_Shape Continuity (const Standard_Address C);
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
    static Standard_Integer NbIntervals (const Standard_Address C);
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
    static void Intervals (const Standard_Address C, TColStd_Array1OfReal& T);
  
  //! output the bounds of interval of index <Index>
  //! used if Type == Composite.
    static void GetInterval (const Standard_Address C, const Standard_Integer Index, const TColStd_Array1OfReal& Tab, Standard_Real& U1, Standard_Real& U2);
  
    static Standard_Boolean IsClosed (const Standard_Address C);
  
    static Standard_Boolean IsPeriodic (const Standard_Address C);
  
    static Standard_Real Period (const Standard_Address C);
  
  //! Computes the point of parameter U on the curve.
    static gp_Pnt2d Value (const Standard_Address C, const Standard_Real U);
  
  //! Computes the point of parameter U on the curve.
    static void D0 (const Standard_Address C, const Standard_Real U, gp_Pnt2d& P);
  
  //! Computes the point  of parameter U on the curve
  //! with its first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
    static void D1 (const Standard_Address C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V);
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
    static void D2 (const Standard_Address C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
    static void D3 (const Standard_Address C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
    static gp_Vec2d DN (const Standard_Address C, const Standard_Real U, const Standard_Integer N);
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
    static Standard_Real Resolution (const Standard_Address C, const Standard_Real R3d);
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
    static GeomAbs_CurveType GetType (const Standard_Address C);
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
    static GeomAbs_CurveType TheType (const Standard_Address C);
  
    static gp_Lin2d Line (const Standard_Address C);
  
    static gp_Circ2d Circle (const Standard_Address C);
  
    static gp_Elips2d Ellipse (const Standard_Address C);
  
    static gp_Hypr2d Hyperbola (const Standard_Address C);
  
    static gp_Parab2d Parabola (const Standard_Address C);
  
    static Handle(Geom2d_BezierCurve) Bezier (const Standard_Address C);
  
    static Handle(Geom2d_BSplineCurve) BSpline (const Standard_Address C);
  
    static Standard_Real EpsX (const Standard_Address C);
  
  Standard_EXPORT static Standard_Integer NbSamples (const Standard_Address C, const Standard_Real U0, const Standard_Real U1);
  
  Standard_EXPORT static Standard_Integer NbSamples (const Standard_Address C);
  
    static Standard_Integer Degree (const Standard_Address C);




protected:





private:





};


#include <HLRBRep_CurveTool.lxx>





#endif // _HLRBRep_CurveTool_HeaderFile
