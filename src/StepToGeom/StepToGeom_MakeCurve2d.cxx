// Created on: 1993-08-04
// Created by: Martine LANGLOIS
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

//:n7 abv 15.02.99: S4132: adding translation of curve_replica
//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeCurve2d.ixx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <StepToGeom_MakeLine2d.hxx>
#include <StepToGeom_MakeConic2d.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepToGeom_MakeBoundedCurve2d.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dConvert.hxx>
#include <StepGeom_CurveReplica.hxx>
#include <StepGeom_CartesianTransformationOperator2d.hxx>
#include <StepToGeom_MakeTransformation2d.hxx>
#include <gp_Trsf2d.hxx>

//=============================================================================
// Creation d' une Curve de Geom2d a partir d' une Curve de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCurve2d::Convert (const Handle(StepGeom_Curve)& SC, Handle(Geom2d_Curve)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    const Handle(StepGeom_Line) L = Handle(StepGeom_Line)::DownCast(SC);
	return StepToGeom_MakeLine2d::Convert(L,*((Handle(Geom2d_Line)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic))) {
    const Handle(StepGeom_Conic) L = Handle(StepGeom_Conic)::DownCast(SC);
	return StepToGeom_MakeConic2d::Convert(L,*((Handle(Geom2d_Conic)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve))) {
    const Handle(StepGeom_BoundedCurve) L = Handle(StepGeom_BoundedCurve)::DownCast(SC);
	return StepToGeom_MakeBoundedCurve2d::Convert(L,*((Handle(Geom2d_BoundedCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica))) { //:n7 abv 16 Feb 99
    const Handle(StepGeom_CurveReplica) CR = Handle(StepGeom_CurveReplica)::DownCast(SC);
    const Handle(StepGeom_Curve) PC = CR->ParentCurve();
    const Handle(StepGeom_CartesianTransformationOperator2d) T =
      Handle(StepGeom_CartesianTransformationOperator2d)::DownCast(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if ( !T.IsNull() && PC != SC )
    {
      Handle(Geom2d_Curve) C1;
      if (StepToGeom_MakeCurve2d::Convert(PC,C1))
      {
        gp_Trsf2d T1;
        if (StepToGeom_MakeTransformation2d::Convert(T,T1))
        {
          C1->Transform ( T1 );
          CC = C1;
          return Standard_True;
		}
      }
    }
  }
  return Standard_False;
}	 
