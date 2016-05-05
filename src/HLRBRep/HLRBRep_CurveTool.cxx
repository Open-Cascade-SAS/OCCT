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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <HLRBRep_Curve.hxx>
#include <HLRBRep_CurveTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================
Standard_Integer
HLRBRep_CurveTool::NbSamples (const Standard_Address C)
{ 
  GeomAbs_CurveType typC = ((HLRBRep_Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + ((HLRBRep_Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    nbs = ((HLRBRep_Curve *)C)->NbKnots();
    nbs*= ((HLRBRep_Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_CurveTool::NbSamples (const Standard_Address C,
                              const Standard_Real u1,
                              const Standard_Real u2) 
{ 
  GeomAbs_CurveType typC = ((HLRBRep_Curve *)C)->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + ((HLRBRep_Curve *)C)->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    Handle(Geom_Curve) aCurve = ((HLRBRep_Curve *)C)->Curve().Curve().Curve();
    GeomAdaptor_Curve GAcurve(aCurve, u1, u2);
    nbs = GAcurve.NbIntervals(GeomAbs_CN) + 1;
    nbs*= ((HLRBRep_Curve *)C)->Degree();
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}
