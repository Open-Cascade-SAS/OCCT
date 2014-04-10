// Created on: 1992-10-22
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Geom2dInt_Geom2dCurveTool.ixx>

#include <GeomAbs_CurveType.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#include <Adaptor2d_Curve2d.hxx>

//============================================================
Standard_Integer Geom2dInt_Geom2dCurveTool::NbSamples (const Adaptor2d_Curve2d& C,
  const Standard_Real U0,
  const Standard_Real U1) 
{
  GeomAbs_CurveType typC = C.GetType();
  Standard_Integer nbs = C.NbSamples();

  if(typC == GeomAbs_BSplineCurve) { 
    Standard_Real t=C.LastParameter()-C.FirstParameter();
    Standard_Real t1=U1-U0;
    if(t1<0.0) t1=-t1;
    nbs = C.NbKnots();
    nbs*= C.Degree();
    Standard_Real anb = t1/t * nbs;
    nbs = (Standard_Integer)anb;
    if(nbs < 4.0) nbs=4;
  }

  if(nbs>300)
    nbs = 300;
  return nbs;

}
//============================================================
Standard_Integer Geom2dInt_Geom2dCurveTool::NbSamples (const Adaptor2d_Curve2d& C) { 
    return C.NbSamples();
 }




