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
  defLowerLimitYaw   = false;
  defUpperLimitYaw   = false;
  defLowerLimitPitch = false;
  defUpperLimitPitch = false;
  defLowerLimitRoll  = false;
  defUpperLimitRoll  = false;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&           theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&           theItemDefinedTransformation_Name,
  const bool                            hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&           theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&        theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&        theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>&      theKinematicPair_Joint,
  const occ::handle<StepGeom_Surface>&                   thePointOnSurfacePair_PairSurface,
  const occ::handle<StepGeom_RectangularTrimmedSurface>& theRangeOnPairSurface,
  const bool                            hasLowerLimitYaw,
  const double                               theLowerLimitYaw,
  const bool                            hasUpperLimitYaw,
  const double                               theUpperLimitYaw,
  const bool                            hasLowerLimitPitch,
  const double                               theLowerLimitPitch,
  const bool                            hasUpperLimitPitch,
  const double                               theUpperLimitPitch,
  const bool                            hasLowerLimitRoll,
  const double                               theLowerLimitRoll,
  const bool                            hasUpperLimitRoll,
  const double                               theUpperLimitRoll)
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

occ::handle<StepGeom_RectangularTrimmedSurface> StepKinematics_PointOnSurfacePairWithRange::
  RangeOnPairSurface() const
{
  return myRangeOnPairSurface;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetRangeOnPairSurface(
  const occ::handle<StepGeom_RectangularTrimmedSurface>& theRangeOnPairSurface)
{
  myRangeOnPairSurface = theRangeOnPairSurface;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::LowerLimitYaw() const
{
  return myLowerLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitYaw(
  const double theLowerLimitYaw)
{
  myLowerLimitYaw = theLowerLimitYaw;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitYaw() const
{
  return defLowerLimitYaw;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::UpperLimitYaw() const
{
  return myUpperLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitYaw(
  const double theUpperLimitYaw)
{
  myUpperLimitYaw = theUpperLimitYaw;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitYaw() const
{
  return defUpperLimitYaw;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::LowerLimitPitch() const
{
  return myLowerLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitPitch(
  const double theLowerLimitPitch)
{
  myLowerLimitPitch = theLowerLimitPitch;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitPitch() const
{
  return defLowerLimitPitch;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::UpperLimitPitch() const
{
  return myUpperLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitPitch(
  const double theUpperLimitPitch)
{
  myUpperLimitPitch = theUpperLimitPitch;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitPitch() const
{
  return defUpperLimitPitch;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::LowerLimitRoll() const
{
  return myLowerLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetLowerLimitRoll(
  const double theLowerLimitRoll)
{
  myLowerLimitRoll = theLowerLimitRoll;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasLowerLimitRoll() const
{
  return defLowerLimitRoll;
}

//=================================================================================================

double StepKinematics_PointOnSurfacePairWithRange::UpperLimitRoll() const
{
  return myUpperLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnSurfacePairWithRange::SetUpperLimitRoll(
  const double theUpperLimitRoll)
{
  myUpperLimitRoll = theUpperLimitRoll;
}

//=================================================================================================

bool StepKinematics_PointOnSurfacePairWithRange::HasUpperLimitRoll() const
{
  return defUpperLimitRoll;
}
