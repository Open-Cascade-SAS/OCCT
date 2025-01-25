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

#include <StepKinematics_GearPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_GearPair,
                           StepKinematics_LowOrderKinematicPairWithMotionCoupling)

//=================================================================================================

StepKinematics_GearPair::StepKinematics_GearPair() {}

//=================================================================================================

void StepKinematics_GearPair::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Standard_Real                          theRadiusFirstLink,
  const Standard_Real                          theRadiusSecondLink,
  const Standard_Real                          theBevel,
  const Standard_Real                          theHelicalAngle,
  const Standard_Real                          theGearRatio)
{
  StepKinematics_LowOrderKinematicPairWithMotionCoupling::Init(
    theRepresentationItem_Name,
    theItemDefinedTransformation_Name,
    hasItemDefinedTransformation_Description,
    theItemDefinedTransformation_Description,
    theItemDefinedTransformation_TransformItem1,
    theItemDefinedTransformation_TransformItem2,
    theKinematicPair_Joint);

  myRadiusFirstLink = theRadiusFirstLink;

  myRadiusSecondLink = theRadiusSecondLink;

  myBevel = theBevel;

  myHelicalAngle = theHelicalAngle;

  myGearRatio = theGearRatio;
}

//=================================================================================================

Standard_Real StepKinematics_GearPair::RadiusFirstLink() const
{
  return myRadiusFirstLink;
}

//=================================================================================================

void StepKinematics_GearPair::SetRadiusFirstLink(const Standard_Real theRadiusFirstLink)
{
  myRadiusFirstLink = theRadiusFirstLink;
}

//=================================================================================================

Standard_Real StepKinematics_GearPair::RadiusSecondLink() const
{
  return myRadiusSecondLink;
}

//=================================================================================================

void StepKinematics_GearPair::SetRadiusSecondLink(const Standard_Real theRadiusSecondLink)
{
  myRadiusSecondLink = theRadiusSecondLink;
}

//=================================================================================================

Standard_Real StepKinematics_GearPair::Bevel() const
{
  return myBevel;
}

//=================================================================================================

void StepKinematics_GearPair::SetBevel(const Standard_Real theBevel)
{
  myBevel = theBevel;
}

//=================================================================================================

Standard_Real StepKinematics_GearPair::HelicalAngle() const
{
  return myHelicalAngle;
}

//=================================================================================================

void StepKinematics_GearPair::SetHelicalAngle(const Standard_Real theHelicalAngle)
{
  myHelicalAngle = theHelicalAngle;
}

//=================================================================================================

Standard_Real StepKinematics_GearPair::GearRatio() const
{
  return myGearRatio;
}

//=================================================================================================

void StepKinematics_GearPair::SetGearRatio(const Standard_Real theGearRatio)
{
  myGearRatio = theGearRatio;
}
