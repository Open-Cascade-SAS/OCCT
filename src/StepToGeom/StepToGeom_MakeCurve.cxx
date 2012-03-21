// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//:n7 abv 16.02.99: S4132: adding translation of curve_replica
//:o2 abv 17.02.99: S4132: adding translation of offset_curve_3d
//:o5 abv 17.02.99: bm4_sd_seal_c.stp #58720: translate surface_curve (3d only)
//:p0 abv 19.02.99: management of 'done' flag improved

#include <StepToGeom_MakeCurve.ixx>

#include <Geom_OffsetCurve.hxx>
#include <Geom_Direction.hxx>

#include <StepGeom_Curve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Conic.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_CurveReplica.hxx>
#include <StepGeom_OffsetCurve3d.hxx>
#include <StepGeom_SurfaceCurve.hxx>

#include <StepToGeom_MakeDirection.hxx>
#include <StepToGeom_MakeTrimmedCurve.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <StepToGeom_MakeLine.hxx>
#include <StepToGeom_MakeConic.hxx>
#include <StepToGeom_MakeBoundedCurve.hxx>
#include <StepToGeom_MakeTransformation3d.hxx>

//=============================================================================
// Creation d' une Curve de Geom a partir d' une Curve de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCurve::Convert (const Handle(StepGeom_Curve)& SC, Handle(Geom_Curve)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    const Handle(StepGeom_Line) L = Handle(StepGeom_Line)::DownCast(SC);
    return StepToGeom_MakeLine::Convert(L,*((Handle(Geom_Line)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve))) {
    const Handle(StepGeom_TrimmedCurve) TC = Handle(StepGeom_TrimmedCurve)::DownCast(SC);
    return StepToGeom_MakeTrimmedCurve::Convert(TC,*((Handle(Geom_TrimmedCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Conic))) {
    const Handle(StepGeom_Conic) CO = Handle(StepGeom_Conic)::DownCast(SC);
    return StepToGeom_MakeConic::Convert(CO,*((Handle(Geom_Conic)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BoundedCurve))) {
    const Handle(StepGeom_BoundedCurve) BC = Handle(StepGeom_BoundedCurve)::DownCast(SC);
    return StepToGeom_MakeBoundedCurve::Convert(BC,*((Handle(Geom_BoundedCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_CurveReplica))) { //:n7 abv 16 Feb 99
    const Handle(StepGeom_CurveReplica) CR = Handle(StepGeom_CurveReplica)::DownCast(SC);
    const Handle(StepGeom_Curve) PC = CR->ParentCurve();
    const Handle(StepGeom_CartesianTransformationOperator3d) T = 
      Handle(StepGeom_CartesianTransformationOperator3d)::DownCast(CR->Transformation());
    // protect against cyclic references and wrong type of cartop
    if ( !T.IsNull() && PC != SC )
    {
      Handle(Geom_Curve) C1;
      if (StepToGeom_MakeCurve::Convert(PC,C1))
      {
        gp_Trsf T1;
		if (StepToGeom_MakeTransformation3d::Convert(T,T1))
        {
          C1->Transform ( T1 );
          CC = C1;
          return Standard_True;
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_OffsetCurve3d))) { //:o2 abv 17 Feb 99
    const Handle(StepGeom_OffsetCurve3d) OC = Handle(StepGeom_OffsetCurve3d)::DownCast(SC);
    const Handle(StepGeom_Curve) BC = OC->BasisCurve();
    if ( BC != SC ) { // protect against loop
      Handle(Geom_Curve) C1;
      if (StepToGeom_MakeCurve::Convert(BC,C1))
      {
        Handle(Geom_Direction) RD;
        if (StepToGeom_MakeDirection::Convert(OC->RefDirection(),RD))
        {
          CC = new Geom_OffsetCurve ( C1, -OC->Distance(), RD->Dir() );
          return Standard_True;
        }
      }
    }
  }
  else if (SC->IsKind(STANDARD_TYPE(StepGeom_SurfaceCurve))) { //:o5 abv 17 Feb 99
    const Handle(StepGeom_SurfaceCurve) SurfC = Handle(StepGeom_SurfaceCurve)::DownCast(SC);
    return StepToGeom_MakeCurve::Convert(SurfC->Curve3d(),CC);
  }
  return Standard_False;
}	 
