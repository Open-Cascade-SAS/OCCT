// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_BSplineSurfaceForm_HeaderFile
#define _StepGeom_BSplineSurfaceForm_HeaderFile

enum StepGeom_BSplineSurfaceForm
{
  StepGeom_bssfPlaneSurf,
  StepGeom_bssfCylindricalSurf,
  StepGeom_bssfConicalSurf,
  StepGeom_bssfSphericalSurf,
  StepGeom_bssfToroidalSurf,
  StepGeom_bssfSurfOfRevolution,
  StepGeom_bssfRuledSurf,
  StepGeom_bssfGeneralisedCone,
  StepGeom_bssfQuadricSurf,
  StepGeom_bssfSurfOfLinearExtrusion,
  StepGeom_bssfUnspecified
};

#endif // _StepGeom_BSplineSurfaceForm_HeaderFile
