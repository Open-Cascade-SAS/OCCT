// Created on: 1996-04-09
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _STEPControl_StepModelType_HeaderFile
#define _STEPControl_StepModelType_HeaderFile

//! Gives you the choice of translation mode for an Open
//! CASCADE shape that is being translated to STEP.
//! - STEPControl_AsIs translates an Open CASCADE shape to its
//! highest possible STEP representation.
//! - STEPControl_ManifoldSolidBrep translates an Open CASCADE shape
//! to a STEP manifold_solid_brep or brep_with_voids entity.
//! - STEPControl_FacetedBrep translates an Open CASCADE shape
//! into a STEP faceted_brep entity.
//! -  STEPControl_ShellBasedSurfaceModel translates an Open CASCADE shape
//! into a STEP shell_based_surface_model entity.
//! - STEPControl_GeometricCurveSet
//! translates an Open CASCADE shape into a STEP geometric_curve_set entity.
enum STEPControl_StepModelType
{
  STEPControl_AsIs,
  STEPControl_ManifoldSolidBrep,
  STEPControl_BrepWithVoids,
  STEPControl_FacetedBrep,
  STEPControl_FacetedBrepAndBrepWithVoids,
  STEPControl_ShellBasedSurfaceModel,
  STEPControl_GeometricCurveSet,
  STEPControl_Hybrid
};

#endif // _STEPControl_StepModelType_HeaderFile
