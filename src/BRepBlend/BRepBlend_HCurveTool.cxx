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


#include <Adaptor3d_HCurve.hxx>
#include <BRepBlend_HCurveTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

//============================================================
Standard_Integer BRepBlend_HCurveTool::NbSamples (const Handle(Adaptor3d_HCurve)& C,
						       const Standard_Real U0,
						       const Standard_Real U1) {
  GeomAbs_CurveType typC = C->GetType();
  static Standard_Real nbsOther = 10.0;
  Standard_Real nbs = nbsOther;
  
  if(typC == GeomAbs_Line) 
    nbs = 2;
  else if(typC == GeomAbs_BezierCurve) 
    nbs = 3 + C->Bezier()->NbPoles();
  else if(typC == GeomAbs_BSplineCurve) { 
    Handle(Geom_BSplineCurve) BSC = C->BSpline();
    nbs = BSC->NbKnots();
    nbs*= BSC->Degree();
    nbs*= BSC->LastParameter()- BSC->FirstParameter();
    nbs/= U1-U0;
    if(nbs < 2.0) nbs=2;
  }
  if(nbs>50)
    nbs = 50;
  return((Standard_Integer)nbs);
}
