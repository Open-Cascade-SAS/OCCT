// Created on: 1993-06-14
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

#ifndef _GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile
#define _GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToStep_Root.hxx>

class StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
class Geom_BSplineCurve;
class Geom2d_BSplineCurve;

//! This class implements the mapping between classes
//! BSplineCurve from Geom, Geom2d and the class
//! BSplineCurveWithKnotsAndRationalBSplineCurve from StepGeom
//! which describes a rational_bspline_curve_with_knots from
//! Prostep
class GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve : public GeomToStep_Root
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve(
    const Handle(Geom_BSplineCurve)& Bsplin,
    const StepData_Factors&          theLocalFactors = StepData_Factors());

  Standard_EXPORT GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve(
    const Handle(Geom2d_BSplineCurve)& Bsplin,
    const StepData_Factors&            theLocalFactors = StepData_Factors());

  Standard_EXPORT const Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)& Value()
    const;

protected:
private:
  Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)
    theBSplineCurveWithKnotsAndRationalBSplineCurve;
};

#endif // _GeomToStep_MakeBSplineCurveWithKnotsAndRationalBSplineCurve_HeaderFile
