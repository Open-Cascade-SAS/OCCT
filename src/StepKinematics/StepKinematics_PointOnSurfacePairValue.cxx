// Created on : Sat May 02 12:41:15 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <StepKinematics_PointOnSurfacePairValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PointOnSurfacePairValue, StepKinematics_PairValue)

//=======================================================================
//function : StepKinematics_PointOnSurfacePairValue
//purpose  :
//=======================================================================
StepKinematics_PointOnSurfacePairValue::StepKinematics_PointOnSurfacePairValue ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_PointOnSurfacePairValue::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                   const Handle(StepKinematics_KinematicPair)& thePairValue_AppliesToPair,
                                                   const Handle(StepGeom_PointOnSurface)& theActualPointOnSurface,
                                                   const StepKinematics_SpatialRotation& theInputOrientation)
{
  StepKinematics_PairValue::Init(theRepresentationItem_Name,
                                 thePairValue_AppliesToPair);

  myActualPointOnSurface = theActualPointOnSurface;

  myInputOrientation = theInputOrientation;
}

//=======================================================================
//function : ActualPointOnSurface
//purpose  :
//=======================================================================
Handle(StepGeom_PointOnSurface) StepKinematics_PointOnSurfacePairValue::ActualPointOnSurface () const
{
  return myActualPointOnSurface;
}

//=======================================================================
//function : SetActualPointOnSurface
//purpose  :
//=======================================================================
void StepKinematics_PointOnSurfacePairValue::SetActualPointOnSurface (const Handle(StepGeom_PointOnSurface)& theActualPointOnSurface)
{
  myActualPointOnSurface = theActualPointOnSurface;
}

//=======================================================================
//function : InputOrientation
//purpose  :
//=======================================================================
StepKinematics_SpatialRotation StepKinematics_PointOnSurfacePairValue::InputOrientation () const
{
  return myInputOrientation;
}

//=======================================================================
//function : SetInputOrientation
//purpose  :
//=======================================================================
void StepKinematics_PointOnSurfacePairValue::SetInputOrientation (const StepKinematics_SpatialRotation& theInputOrientation)
{
  myInputOrientation = theInputOrientation;
}
