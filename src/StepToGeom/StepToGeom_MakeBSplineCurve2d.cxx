// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
