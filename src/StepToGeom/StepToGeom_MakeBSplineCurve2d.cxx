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


#include <StepToGeom_MakeBSplineCurve2d.ixx>
#include <gp_Pnt2d.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <Geom2d_CartesianPoint.hxx>

//=============================================================================
// Creation d' une BSplineCurve de Geom2d a partir d' une
// BSplineCurveWithKnotsAndRationalBSplineCurve de Step
//=============================================================================

#define Array1OfPnt_gen                  TColgp_Array1OfPnt2d
#define Pnt_gen                          gp_Pnt2d
#define CartesianPoint_gen               Handle(Geom2d_CartesianPoint)
#define StepToGeom_MakeCartesianPoint_gen StepToGeom_MakeCartesianPoint2d
#define Pnt_fonc                         Pnt2d
#define BSplineCurve_gen                 Geom2d_BSplineCurve
#define BSplineCurve_retour              Handle(Geom2d_BSplineCurve)
#define StepToGeom_MakeBSplineCurve_gen   StepToGeom_MakeBSplineCurve2d

Standard_Boolean StepToGeom_MakeBSplineCurve2d::Convert
    (const Handle(StepGeom_BSplineCurve)& SC,
     Handle(Geom2d_BSplineCurve)& CC)

#include <StepToGeom_MakeBSplineCurve.pxx>

#undef Array1OfPnt_gen
#undef Pnt_gen
#undef CartesianPoint_gen
#undef StepToGeom_MakeCartesianPoint_gen
#undef Pnt_fonc
#undef BSplineCurve_gen
#undef StepToGeom_MakeBSplineCurve_gen
#undef BSplineCurve_retour
