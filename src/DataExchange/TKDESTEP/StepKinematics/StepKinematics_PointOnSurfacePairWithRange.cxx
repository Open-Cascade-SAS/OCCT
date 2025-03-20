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

#include <StepKinematics_PointOnSurfacePairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PointOnSurfacePairWithRange,
                           StepKinematics_PointOnSurfacePair)

//=================================================================================================

StepKinematics_PointOnSurfacePairWithRange::StepKinematics_PointOnSurfacePairWithRange()
{
  defLowerLimitYaw   = Standard_False;
  defUpperLimitYaw   = Standard_False;
  defLowerLimitPitch = Standard_False;
  defUpperLimitPitch = Standard_False;
  defLowerLimitRoll  = Standard_False;
  defUpperLimitRoll  = Standard_False;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::Init(
  const Handle(TCollection_HAsciiString)&           theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&           theItemDefinedTransformation_Name,
  const Standard_Boolean                            hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&           theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&        theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&        theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)&      theKinematicPair_Joint,
  const Handle(StepGeom_Surface)&                   thePointOnSurfacePair_PairSurface,
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnPairSurface,
  const Standard_Boolean                            hasLowerLimitYaw,
  const Standard_Real                               theLowerLimitYaw,
  const Standard_Boolean                            hasUpperLimitYaw,
  const Standard_Real                               theUpperLimitYaw,
  const Standard_Boolean                            hasLowerLimitPitch,
  const Standard_Real                               theLowerLimitPitch,
  const Standard_Boolean                            hasUpperLimitPitch,
  const Standard_Real                               theUpperLimitPitch,
  const Standard_Boolean                            hasLowerLimitRoll,
  const Standard_Real                               theLowerLimitRoll,
  const Standard_Boolean                            hasUpperLimitRoll,
  const Standard_Real                               theUpperLimitRoll)
{
  StepKinematics_PointOnSurfacePair::Init(theRepresentationItem_Name,
                                          theItemDefinedTransformation_Name,
                                          hasItemDefinedTransformation_Description,
                                          theItemDefinedTransformation_Description,
                                          theItemDefinedTransformation_TransformItem1,
                                          theItemDefinedTransformation_TransformItem2,
                                          theKinematicPair_Joint,
                                          thePointOnSurfacePair_PairSurface);

  myRangeOnPairSurface = theRangeOnPairSurface;

  defLowerLimitYaw = hasLowerLimitYaw;
  if (defLowerLimitYaw)
  {
    myLowerLimitYaw = theLowerLimitYaw;
  }
  else
    myLowerLimitYaw = 0;

  defUpperLimitYaw = hasUpperLimitYaw;
  if (defUpperLimitYaw)
  {
    myUpperLimitYaw = theUpperLimitYaw;
  }
  else
    myUpperLimitYaw = 0;

  defLowerLimitPitch = hasLowerLimitPitch;
  if (defLowerLimitPitch)
  {
    myLowerLimitPitch = theLowerLimitPitch;
  }
  else
    myLowerLimitPitch = 0;

  defUpperLimitPitch = hasUpperLimitPitch;
  if (defUpperLimitPitch)
  {
    myUpperLimitPitch = theUpperLimitPitch;
  }
  else
    myUpperLimitPitch = 0;

  defLowerLimitRoll = hasLowerLimitRoll;
  if (defLowerLimitRoll)
  {
    myLowerLimitRoll = theLowerLimitRoll;
  }
  else
    myLowerLimitRoll = 0;

  defUpperLimitRoll = hasUpperLimitRoll;
  if (defUpperLimitRoll)
  {
    myUpperLimitRoll = theUpperLimitRoll;
  }
  else
    myUpperLimitRoll = 0;
}

//=================================================================================================

Handle(StepGeom_RectangularTrimmedSurface) StepKinematics_PointOnSurfacePairWithRange::
  RangeOnPairSurface() const
{
  return myRangeOnPairSurface;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetRangeOnPairSurface(
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnPairSurface)
{
  myRangeOnPairSurface = theRangeOnPairSurface;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::LowerLimitYaw() const
{
  return myLowerLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitYaw(
  const Standard_Real theLowerLimitYaw)
{
  myLowerLimitYaw = theLowerLimitYaw;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitYaw() const
{
  return defLowerLimitYaw;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::UpperLimitYaw() const
{
  return myUpperLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitYaw(
  const Standard_Real theUpperLimitYaw)
{
  myUpperLimitYaw = theUpperLimitYaw;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitYaw() const
{
  return defUpperLimitYaw;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::LowerLimitPitch() const
{
  return myLowerLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitPitch(
  const Standard_Real theLowerLimitPitch)
{
  myLowerLimitPitch = theLowerLimitPitch;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitPitch() const
{
  return defLowerLimitPitch;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::UpperLimitPitch() const
{
  return myUpperLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitPitch(
  const Standard_Real theUpperLimitPitch)
{
  myUpperLimitPitch = theUpperLimitPitch;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitPitch() const
{
  return defUpperLimitPitch;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::LowerLimitRoll() const
{
  return myLowerLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitRoll(
  const Standard_Real theLowerLimitRoll)
{
  myLowerLimitRoll = theLowerLimitRoll;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitRoll() const
{
  return defLowerLimitRoll;
}

//=================================================================================================

Standard_Real StepKinematics_PointOnSurfacePairWithRange::UpperLimitRoll() const
{
  return myUpperLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitRoll(
  const Standard_Real theUpperLimitRoll)
{
  myUpperLimitRoll = theUpperLimitRoll;
}

//=================================================================================================

Standard_Boolean StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitRoll() const
{
  return defUpperLimitRoll;
}
