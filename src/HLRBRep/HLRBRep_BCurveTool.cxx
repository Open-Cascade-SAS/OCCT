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


#include <BRepAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <HLRBRep_BCurveTool.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : NbSamples
//purpose  : 
//=======================================================================
Standard_Integer
HLRBRep_BCurveTool::NbSamples (const BRepAdaptor_Curve& C,
			       const Standard_Real U0,
			       const Standard_Real U1)
{
  GeomAbs_CurveType typC = C.GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + C.NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    nbs = C.NbKnots();
    nbs*= C.Degree();
    nbs*= C.LastParameter()- C.FirstParameter();
    nbs/= U1-U0;
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}

//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void HLRBRep_BCurveTool::Poles(const BRepAdaptor_Curve& C,
			       TColgp_Array1OfPnt& T)
{ 
  if(C.GetType() == GeomAbs_BezierCurve) 
    C.Bezier()->Poles(T);
  else if(C.GetType() == GeomAbs_BSplineCurve) 
    C.BSpline()->Poles(T);
}

//=======================================================================
//function : PolesAndWeights
//purpose  : 
//=======================================================================

void HLRBRep_BCurveTool::PolesAndWeights(const BRepAdaptor_Curve& C, 
					 TColgp_Array1OfPnt& T,
					 TColStd_Array1OfReal& W)
{ 
  if(C.GetType() == GeomAbs_BezierCurve) {
    const Handle(Geom_BezierCurve) HB = C.Bezier();
    HB->Poles(T);
    HB->Weights(W);
  }
  else if(C.GetType() == GeomAbs_BSplineCurve) {
    const Handle(Geom_BSplineCurve) HB = C.BSpline();
    HB->Poles(T);
    HB->Weights(W);
  }
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve)
     HLRBRep_BCurveTool::Bezier (const BRepAdaptor_Curve& C)
{ return(C.Bezier()); }

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)
     HLRBRep_BCurveTool::BSpline (const BRepAdaptor_Curve& C)
{ return(C.BSpline()); }
