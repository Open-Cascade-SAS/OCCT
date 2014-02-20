// Created on: 1994-11-04
// Created by: Frederic MAUPAS
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

//:p0 abv 19.02.99: management of 'done' flag improved
//:p3 abv 23.02.99: bm4_id_punch_d.stp #1313: shift of parameters on ellipse with R1 < R2

#include <StepToGeom_MakeTrimmedCurve2d.ixx>
#include <gp_Pnt2d.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepToGeom_MakeTrimmedCurve2d.hxx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <StepToGeom_MakeCurve2d.hxx>

#include <UnitsMethods.hxx>

//=============================================================================
// Creation d'une Trimmed Curve de Geom2d a partir d' une Trimmed Curve de Step
//=============================================================================
// Shall be completed to treat trimming with points

Standard_Boolean StepToGeom_MakeTrimmedCurve2d::Convert (const Handle(StepGeom_TrimmedCurve)& SC, Handle(Geom2d_BSplineCurve)& CC)
{
  const Handle(StepGeom_Curve) BasisCurve = SC->BasisCurve();
  Handle(Geom2d_Curve) theGeomBasis;
  if (!StepToGeom_MakeCurve2d::Convert(BasisCurve,theGeomBasis))
    return Standard_False;

  if (theGeomBasis->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    CC = Handle(Geom2d_BSplineCurve)::DownCast(theGeomBasis);
    return Standard_True;
  }

  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel1 = SC->Trim1();
  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel2 = SC->Trim2();
  const Standard_Integer nbSel1 = SC->NbTrim1();
  const Standard_Integer nbSel2 = SC->NbTrim2();
  if ((nbSel1 == 1) && (nbSel2 == 1) &&
      (theTrimSel1->Value(1).CaseMember() > 0) &&
      (theTrimSel2->Value(1).CaseMember() > 0))
  {
    const Standard_Real u1 = theTrimSel1->Value(1).ParameterValue();
    const Standard_Real u2 = theTrimSel2->Value(1).ParameterValue();
    Standard_Real fact = 1., shift = 0.;

    if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Line))) {
      const Handle(StepGeom_Line) theLine = Handle(StepGeom_Line)::DownCast(BasisCurve);
      fact = theLine->Dir()->Magnitude();
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Circle)) ||
             BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
//      if (u1 > 2.1*M_PI || u2 > 2.1*M_PI) fact = M_PI / 180.;
      fact = UnitsMethods::PlaneAngleFactor();
      //:p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
      const Handle(StepGeom_Ellipse) ellipse = Handle(StepGeom_Ellipse)::DownCast(BasisCurve);
      if ( !ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0. )
        shift = 0.5 * M_PI;
    }
    else if (BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Parabola)) ||
             BasisCurve->IsKind(STANDARD_TYPE(StepGeom_Hyperbola))) {
      // LATER !!!
    }
//    CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
//      mais des systemes ecrivent en radians. Exploiter UnitsMethods

    const Standard_Real newU1 = shift + u1 * fact;
    const Standard_Real newU2 = shift + u2 * fact;

    const Handle(Geom2d_TrimmedCurve) theTrimmed =
      new Geom2d_TrimmedCurve(theGeomBasis, newU1, newU2, SC->SenseAgreement());
    CC = Geom2dConvert::CurveToBSplineCurve(theTrimmed);
    return Standard_True;
  }
//#ifdef DEB
//  else cout << "Warning: TrimmedCurve2d not translated" << endl;
//#endif
  return Standard_False;
}
