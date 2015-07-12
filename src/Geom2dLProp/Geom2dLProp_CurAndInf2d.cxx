// Created on: 1994-09-06
// Created by: Yves FRICAUD
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


#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dLProp_CurAndInf2d.hxx>
#include <Geom2dLProp_NumericCurInf2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <LProp_AnalyticCurInf.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : Geom2dLProp_CurAndInf2d
//purpose  : 
//=======================================================================
Geom2dLProp_CurAndInf2d::Geom2dLProp_CurAndInf2d()
{
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::Perform(const Handle(Geom2d_Curve)& C)
{
  PerformCurExt(C);
  PerformInf   (C);
}


//=======================================================================
//function : PerformCurExt
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::PerformCurExt(const Handle(Geom2d_Curve)& C)
{
  isDone = Standard_True;

  Geom2dAdaptor_Curve         CC(C);
  LProp_AnalyticCurInf        AC;
  Geom2dLProp_NumericCurInf2d NC;
  GeomAbs_CurveType           CType = CC.GetType();

  switch (CType) {
  case GeomAbs_Line: 
    break;
  case GeomAbs_Circle:
    break;
  case GeomAbs_Ellipse:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_Hyperbola:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_Parabola:
    AC.Perform(CType,CC.FirstParameter(),CC.LastParameter(),*this);
    break;
  case GeomAbs_BSplineCurve:
    if (CC.Continuity() >= GeomAbs_C3 ) {
      NC.PerformCurExt(C,*this);
      isDone = NC.IsDone();
    }
    else {
      // Decoupage en intervalles C3.
      isDone = Standard_True;
      Standard_Integer NbInt = CC.NbIntervals(GeomAbs_C3);
      TColStd_Array1OfReal Param(1,NbInt+1);
      CC.Intervals(Param,GeomAbs_C3);
      for (Standard_Integer i = 1; i <= NbInt; i++) {
	NC.PerformCurExt(C,Param(i),Param(i+1),*this);
	if (!NC.IsDone()) {isDone = Standard_False;}
      }
    }
    break;

    default : {
      NC.PerformCurExt(C,*this);
      isDone = NC.IsDone();
    }
    break;
  }
  
}


//=======================================================================
//function : PerformInf
//purpose  : 
//=======================================================================

void Geom2dLProp_CurAndInf2d::PerformInf(const Handle(Geom2d_Curve)& C)
{
  isDone = Standard_True;

  Geom2dAdaptor_Curve  CC(C);
  GeomAbs_CurveType    CType = CC.GetType();
  Geom2dLProp_NumericCurInf2d NC;

  switch (CType) {
  case GeomAbs_Line:
    break;
  case GeomAbs_Circle:
    break;
  case GeomAbs_Ellipse:
      break;
  case GeomAbs_Hyperbola:
    break;
  case GeomAbs_Parabola:
    break;
  case GeomAbs_BSplineCurve:
    if (CC.Continuity() >= GeomAbs_C3 ) {
      NC.PerformInf(C,*this);
      isDone = NC.IsDone();
    }
    else {
      // Decoupage en intervalles C3.
      isDone = Standard_True;
      Standard_Integer NbInt = CC.NbIntervals(GeomAbs_C3);
      TColStd_Array1OfReal Param(1,NbInt+1);
      CC.Intervals(Param,GeomAbs_C3);

      for (Standard_Integer i = 1; i <= NbInt; i++) {
	NC.PerformInf(C,Param(i),Param(i+1),*this);
	if (!NC.IsDone()) {isDone = Standard_False;}
      }
    }
    break;

    default : {
      NC.PerformInf(C,*this);
      isDone = NC.IsDone();
    }
    break;
  }  
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dLProp_CurAndInf2d::IsDone() const 
{
  return isDone;
}


