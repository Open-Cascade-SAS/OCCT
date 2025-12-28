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

#include <StepKinematics_LowOrderKinematicPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPair, StepKinematics_KinematicPair)

//=================================================================================================

StepKinematics_LowOrderKinematicPair::StepKinematics_LowOrderKinematicPair() = default;

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                                        hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const bool                                        theTX,
  const bool                                        theTY,
  const bool                                        theTZ,
  const bool                                        theRX,
  const bool                                        theRY,
  const bool                                        theRZ)
{
  StepKinematics_KinematicPair::Init(theRepresentationItem_Name,
                                     theItemDefinedTransformation_Name,
                                     hasItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_TransformItem1,
                                     theItemDefinedTransformation_TransformItem2,
                                     theKinematicPair_Joint);

  myTX = theTX;

  myTY = theTY;

  myTZ = theTZ;

  myRX = theRX;

  myRY = theRY;

  myRZ = theRZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::TX() const
{
  return myTX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetTX(const bool theTX)
{
  myTX = theTX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::TY() const
{
  return myTY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetTY(const bool theTY)
{
  myTY = theTY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::TZ() const
{
  return myTZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetTZ(const bool theTZ)
{
  myTZ = theTZ;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::RX() const
{
  return myRX;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetRX(const bool theRX)
{
  myRX = theRX;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::RY() const
{
  return myRY;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetRY(const bool theRY)
{
  myRY = theRY;
}

//=================================================================================================

bool StepKinematics_LowOrderKinematicPair::RZ() const
{
  return myRZ;
}

//=================================================================================================

void StepKinematics_LowOrderKinematicPair::SetRZ(const bool theRZ)
{
  myRZ = theRZ;
}
