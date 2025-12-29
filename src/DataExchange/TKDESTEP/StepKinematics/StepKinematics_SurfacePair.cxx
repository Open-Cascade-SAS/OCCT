// Created on : Sat May 02 12:41:16 2020
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

#include <StepKinematics_SurfacePair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_SurfacePair, StepKinematics_HighOrderKinematicPair)

//=================================================================================================

StepKinematics_SurfacePair::StepKinematics_SurfacePair() = default;

//=================================================================================================

void StepKinematics_SurfacePair::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                                        hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const occ::handle<StepGeom_Surface>&              theSurface1,
  const occ::handle<StepGeom_Surface>&              theSurface2,
  const bool                                        theOrientation)
{
  StepKinematics_HighOrderKinematicPair::Init(theRepresentationItem_Name,
                                              theItemDefinedTransformation_Name,
                                              hasItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_TransformItem1,
                                              theItemDefinedTransformation_TransformItem2,
                                              theKinematicPair_Joint);

  mySurface1 = theSurface1;

  mySurface2 = theSurface2;

  myOrientation = theOrientation;
}

//=================================================================================================

occ::handle<StepGeom_Surface> StepKinematics_SurfacePair::Surface1() const
{
  return mySurface1;
}

//=================================================================================================

void StepKinematics_SurfacePair::SetSurface1(const occ::handle<StepGeom_Surface>& theSurface1)
{
  mySurface1 = theSurface1;
}

//=================================================================================================

occ::handle<StepGeom_Surface> StepKinematics_SurfacePair::Surface2() const
{
  return mySurface2;
}

//=================================================================================================

void StepKinematics_SurfacePair::SetSurface2(const occ::handle<StepGeom_Surface>& theSurface2)
{
  mySurface2 = theSurface2;
}

//=================================================================================================

bool StepKinematics_SurfacePair::Orientation() const
{
  return myOrientation;
}

//=================================================================================================

void StepKinematics_SurfacePair::SetOrientation(const bool theOrientation)
{
  myOrientation = theOrientation;
}
