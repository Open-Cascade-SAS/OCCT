// Created on: 1993-07-02
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

//:n6 abv 15.02.99: S4132: adding translation of polyline
//:p0 abv 19.02.99: management of 'done' flag improved; trimmed_curve treated

#include <StepToGeom_MakeBoundedCurve.ixx>

#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BezierCurve.hxx>
#include <StepGeom_UniformCurve.hxx>
#include <StepGeom_UniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_QuasiUniformCurve.hxx>
#include <StepGeom_QuasiUniformCurveAndRationalBSplineCurve.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_KnotType.hxx>
#include <StepToGeom_MakeBSplineCurve.hxx>
#include <StepGeom_Polyline.hxx>
#include <StepToGeom_MakePolyline.hxx>
#include <StepToGeom_MakeTrimmedCurve.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=============================================================================
// Creation d' une BoundedCurve de Geom a partir d' une BoundedCurve de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeBoundedCurve::Convert
    (const Handle(StepGeom_BoundedCurve)& SC,
     Handle(Geom_BoundedCurve)& CC)
{
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve))) {
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)
      Bspli = Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(SC);
	return StepToGeom_MakeBSplineCurve::Convert(Bspli,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots))) {
    const Handle(StepGeom_BSplineCurveWithKnots)
      Bspli = Handle(StepGeom_BSplineCurveWithKnots)::DownCast(SC);
	return StepToGeom_MakeBSplineCurve::Convert(Bspli,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_TrimmedCurve))) {
    const Handle(StepGeom_TrimmedCurve) L = Handle(StepGeom_TrimmedCurve)::DownCast(SC);
	return StepToGeom_MakeTrimmedCurve::Convert(L,*((Handle(Geom_TrimmedCurve)*)&CC));
  }
  // STEP BezierCurve, UniformCurve and QuasiUniformCurve are transformed into
  // STEP BSplineCurve before being mapped onto CAS.CADE/SF
  if (SC->IsKind(STANDARD_TYPE(StepGeom_BezierCurve))) {
    const Handle(StepGeom_BezierCurve) BzC = Handle(StepGeom_BezierCurve)::DownCast(SC);
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(BzC->Degree());
    BSPL->SetControlPointsList(BzC->ControlPointsList());
    BSPL->SetCurveForm(BzC->CurveForm());
    BSPL->SetClosedCurve(BzC->ClosedCurve());
    BSPL->SetSelfIntersect(BzC->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,2);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,2);
    Kmult->SetValue(1, BzC->Degree() + 1);
    Kmult->SetValue(2, BzC->Degree() + 1);
    Knots->SetValue(1, 0.);
    Knots->SetValue(2, 1.);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);
	return StepToGeom_MakeBSplineCurve::Convert(BSPL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurve))) {
    const Handle(StepGeom_UniformCurve) UC = Handle(StepGeom_UniformCurve)::DownCast(SC);
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(UC->Degree());
    BSPL->SetControlPointsList(UC->ControlPointsList());
    BSPL->SetCurveForm(UC->CurveForm());
    BSPL->SetClosedCurve(UC->ClosedCurve());
    BSPL->SetSelfIntersect(UC->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = BSPL->NbControlPointsList() + BSPL->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iUC = 1 ; iUC <= nbK ; iUC ++) {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);
	return StepToGeom_MakeBSplineCurve::Convert(BSPL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurve))) {
    const Handle(StepGeom_QuasiUniformCurve) QUC = 
      Handle(StepGeom_QuasiUniformCurve)::DownCast(SC);
    const Handle(StepGeom_BSplineCurveWithKnots) BSPL = new StepGeom_BSplineCurveWithKnots;
    BSPL->SetDegree(QUC->Degree());
    BSPL->SetControlPointsList(QUC->ControlPointsList());
    BSPL->SetCurveForm(QUC->CurveForm());
    BSPL->SetClosedCurve(QUC->ClosedCurve());
    BSPL->SetSelfIntersect(QUC->SelfIntersect());
    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = BSPL->NbControlPointsList() - BSPL->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iQUC = 1 ; iQUC <= nbK ; iQUC ++) {
      Kmult->SetValue(iQUC, 1);
      Knots->SetValue(iQUC, iQUC - 1.);
    }
    Kmult->SetValue(1, BSPL->Degree() + 1);
    Kmult->SetValue(nbK, BSPL->Degree() + 1);
    BSPL->SetKnotMultiplicities(Kmult);
    BSPL->SetKnots(Knots);
	return StepToGeom_MakeBSplineCurve::Convert(BSPL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_UniformCurveAndRationalBSplineCurve))) {
    const Handle(StepGeom_UniformCurveAndRationalBSplineCurve) RUC = 
      Handle(StepGeom_UniformCurveAndRationalBSplineCurve)::DownCast(SC);
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) RBSPL = 
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = RUC->NbControlPointsList() + RUC->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iUC = 1 ; iUC <= nbK ; iUC ++) {
      Kmult->SetValue(iUC, 1);
      Knots->SetValue(iUC, iUC - 1.);
    }
    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RUC->Name(), RUC->Degree(), RUC->ControlPointsList(), RUC->CurveForm(),
		RUC->ClosedCurve(), RUC->SelfIntersect(), Kmult, Knots, StepGeom_ktUnspecified,
		RUC->WeightsData());
	return StepToGeom_MakeBSplineCurve::Convert(RBSPL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_QuasiUniformCurveAndRationalBSplineCurve))) {
    const Handle(StepGeom_QuasiUniformCurveAndRationalBSplineCurve) RQUC = 
      Handle(StepGeom_QuasiUniformCurveAndRationalBSplineCurve)::DownCast(SC);
    const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) RBSPL = 
      new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
    // Compute Knots and KnotsMultiplicity
    const Standard_Integer nbK = RQUC->NbControlPointsList() - RQUC->Degree() + 1;
    const Handle(TColStd_HArray1OfInteger) Kmult = new TColStd_HArray1OfInteger(1,nbK);
    const Handle(TColStd_HArray1OfReal) Knots = new TColStd_HArray1OfReal(1,nbK);
    for (Standard_Integer iRQUC = 1 ; iRQUC <= nbK ; iRQUC ++) {
      Kmult->SetValue(iRQUC, 1);
      Knots->SetValue(iRQUC, iRQUC - 1.);
    }
    Kmult->SetValue(1, RQUC->Degree() + 1);
    Kmult->SetValue(nbK, RQUC->Degree() + 1);
    // Initialize the BSplineCurveWithKnotsAndRationalBSplineCurve
    RBSPL->Init(RQUC->Name(), RQUC->Degree(), RQUC->ControlPointsList(), RQUC->CurveForm(),
		RQUC->ClosedCurve(), RQUC->SelfIntersect(), Kmult, Knots, StepGeom_ktUnspecified,
		RQUC->WeightsData());
	return StepToGeom_MakeBSplineCurve::Convert(RBSPL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  if (SC->IsKind(STANDARD_TYPE(StepGeom_Polyline))) { //:n6 abv 15 Feb 99
    const Handle(StepGeom_Polyline) PL = Handle(StepGeom_Polyline)::DownCast (SC);
    return StepToGeom_MakePolyline::Convert(PL,*((Handle(Geom_BSplineCurve)*)&CC));
  }
  return Standard_False;
}
