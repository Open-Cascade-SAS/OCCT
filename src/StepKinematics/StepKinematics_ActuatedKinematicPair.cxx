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

#include <StepKinematics_ActuatedKinematicPair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_ActuatedKinematicPair, StepKinematics_KinematicPair)

//=================================================================================================

StepKinematics_ActuatedKinematicPair::StepKinematics_ActuatedKinematicPair()
{
  defTX = Standard_False;
  defTY = Standard_False;
  defTZ = Standard_False;
  defRX = Standard_False;
  defRY = Standard_False;
  defRZ = Standard_False;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Standard_Boolean                       hasTX,
  const StepKinematics_ActuatedDirection       theTX,
  const Standard_Boolean                       hasTY,
  const StepKinematics_ActuatedDirection       theTY,
  const Standard_Boolean                       hasTZ,
  const StepKinematics_ActuatedDirection       theTZ,
  const Standard_Boolean                       hasRX,
  const StepKinematics_ActuatedDirection       theRX,
  const Standard_Boolean                       hasRY,
  const StepKinematics_ActuatedDirection       theRY,
  const Standard_Boolean                       hasRZ,
  const StepKinematics_ActuatedDirection       theRZ)
{
  StepKinematics_KinematicPair::Init(theRepresentationItem_Name,
                                     theItemDefinedTransformation_Name,
                                     hasItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_Description,
                                     theItemDefinedTransformation_TransformItem1,
                                     theItemDefinedTransformation_TransformItem2,
                                     theKinematicPair_Joint);

  defTX = hasTX;
  if (defTX)
  {
    myTX = theTX;
  }
  else
    myTX = StepKinematics_ActuatedDirection();

  defTY = hasTY;
  if (defTY)
  {
    myTY = theTY;
  }
  else
    myTY = StepKinematics_ActuatedDirection();

  defTZ = hasTZ;
  if (defTZ)
  {
    myTZ = theTZ;
  }
  else
    myTZ = StepKinematics_ActuatedDirection();

  defRX = hasRX;
  if (defRX)
  {
    myRX = theRX;
  }
  else
    myRX = StepKinematics_ActuatedDirection();

  defRY = hasRY;
  if (defRY)
  {
    myRY = theRY;
  }
  else
    myRY = StepKinematics_ActuatedDirection();

  defRZ = hasRZ;
  if (defRZ)
  {
    myRZ = theRZ;
  }
  else
    myRZ = StepKinematics_ActuatedDirection();
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::TX() const
{
  return myTX;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetTX(const StepKinematics_ActuatedDirection theTX)
{
  myTX = theTX;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasTX() const
{
  return defTX;
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::TY() const
{
  return myTY;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetTY(const StepKinematics_ActuatedDirection theTY)
{
  myTY = theTY;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasTY() const
{
  return defTY;
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::TZ() const
{
  return myTZ;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetTZ(const StepKinematics_ActuatedDirection theTZ)
{
  myTZ = theTZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasTZ() const
{
  return defTZ;
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::RX() const
{
  return myRX;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetRX(const StepKinematics_ActuatedDirection theRX)
{
  myRX = theRX;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasRX() const
{
  return defRX;
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::RY() const
{
  return myRY;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetRY(const StepKinematics_ActuatedDirection theRY)
{
  myRY = theRY;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasRY() const
{
  return defRY;
}

//=================================================================================================

StepKinematics_ActuatedDirection StepKinematics_ActuatedKinematicPair::RZ() const
{
  return myRZ;
}

//=================================================================================================

void StepKinematics_ActuatedKinematicPair::SetRZ(const StepKinematics_ActuatedDirection theRZ)
{
  myRZ = theRZ;
}

//=================================================================================================

Standard_Boolean StepKinematics_ActuatedKinematicPair::HasRZ() const
{
  return defRZ;
}
