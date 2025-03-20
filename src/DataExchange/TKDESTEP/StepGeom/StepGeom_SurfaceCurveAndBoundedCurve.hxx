// Created on: 1999-02-15
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepGeom_SurfaceCurveAndBoundedCurve_HeaderFile
#define _StepGeom_SurfaceCurveAndBoundedCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_SurfaceCurve.hxx>
class StepGeom_BoundedCurve;

class StepGeom_SurfaceCurveAndBoundedCurve;
DEFINE_STANDARD_HANDLE(StepGeom_SurfaceCurveAndBoundedCurve, StepGeom_SurfaceCurve)

//! complex type: bounded_curve + surface_curve
//! needed for curve_bounded_surfaces (S4132)
class StepGeom_SurfaceCurveAndBoundedCurve : public StepGeom_SurfaceCurve
{

public:
  //! creates empty object
  Standard_EXPORT StepGeom_SurfaceCurveAndBoundedCurve();

  //! returns field BoundedCurve
  Standard_EXPORT Handle(StepGeom_BoundedCurve)& BoundedCurve();

  DEFINE_STANDARD_RTTIEXT(StepGeom_SurfaceCurveAndBoundedCurve, StepGeom_SurfaceCurve)

protected:
private:
  Handle(StepGeom_BoundedCurve) myBoundedCurve;
};

#endif // _StepGeom_SurfaceCurveAndBoundedCurve_HeaderFile
