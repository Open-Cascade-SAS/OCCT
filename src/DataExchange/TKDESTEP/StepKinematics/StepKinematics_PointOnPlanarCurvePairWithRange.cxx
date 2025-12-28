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

#include <StepKinematics_PointOnPlanarCurvePairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PointOnPlanarCurvePairWithRange,
                           StepKinematics_PointOnPlanarCurvePair)

//=================================================================================================

StepKinematics_PointOnPlanarCurvePairWithRange::StepKinematics_PointOnPlanarCurvePairWithRange()
{
  defLowerLimitYaw   = false;
  defUpperLimitYaw   = false;
  defLowerLimitPitch = false;
  defUpperLimitPitch = false;
  defLowerLimitRoll  = false;
  defUpperLimitRoll  = false;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::Init(
  const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
  const bool                                        hasItemDefinedTransformation_Description,
  const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
  const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
  const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
  const occ::handle<StepGeom_Curve>&                thePointOnPlanarCurvePair_PairCurve,
  const bool                                        thePointOnPlanarCurvePair_Orientation,
  const occ::handle<StepGeom_TrimmedCurve>&         theRangeOnPairCurve,
  const bool                                        hasLowerLimitYaw,
  const double                                      theLowerLimitYaw,
  const bool                                        hasUpperLimitYaw,
  const double                                      theUpperLimitYaw,
  const bool                                        hasLowerLimitPitch,
  const double                                      theLowerLimitPitch,
  const bool                                        hasUpperLimitPitch,
  const double                                      theUpperLimitPitch,
  const bool                                        hasLowerLimitRoll,
  const double                                      theLowerLimitRoll,
  const bool                                        hasUpperLimitRoll,
  const double                                      theUpperLimitRoll)
{
  StepKinematics_PointOnPlanarCurvePair::Init(theRepresentationItem_Name,
                                              theItemDefinedTransformation_Name,
                                              hasItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_TransformItem1,
                                              theItemDefinedTransformation_TransformItem2,
                                              theKinematicPair_Joint,
                                              thePointOnPlanarCurvePair_PairCurve,
                                              thePointOnPlanarCurvePair_Orientation);

  myRangeOnPairCurve = theRangeOnPairCurve;

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

occ::handle<StepGeom_TrimmedCurve> StepKinematics_PointOnPlanarCurvePairWithRange::
  RangeOnPairCurve() const
{
  return myRangeOnPairCurve;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetRangeOnPairCurve(
  const occ::handle<StepGeom_TrimmedCurve>& theRangeOnPairCurve)
{
  myRangeOnPairCurve = theRangeOnPairCurve;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::LowerLimitYaw() const
{
  return myLowerLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetLowerLimitYaw(const double theLowerLimitYaw)
{
  myLowerLimitYaw = theLowerLimitYaw;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasLowerLimitYaw() const
{
  return defLowerLimitYaw;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::UpperLimitYaw() const
{
  return myUpperLimitYaw;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetUpperLimitYaw(const double theUpperLimitYaw)
{
  myUpperLimitYaw = theUpperLimitYaw;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasUpperLimitYaw() const
{
  return defUpperLimitYaw;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::LowerLimitPitch() const
{
  return myLowerLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetLowerLimitPitch(
  const double theLowerLimitPitch)
{
  myLowerLimitPitch = theLowerLimitPitch;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasLowerLimitPitch() const
{
  return defLowerLimitPitch;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::UpperLimitPitch() const
{
  return myUpperLimitPitch;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetUpperLimitPitch(
  const double theUpperLimitPitch)
{
  myUpperLimitPitch = theUpperLimitPitch;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasUpperLimitPitch() const
{
  return defUpperLimitPitch;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::LowerLimitRoll() const
{
  return myLowerLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetLowerLimitRoll(
  const double theLowerLimitRoll)
{
  myLowerLimitRoll = theLowerLimitRoll;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasLowerLimitRoll() const
{
  return defLowerLimitRoll;
}

//=================================================================================================

double StepKinematics_PointOnPlanarCurvePairWithRange::UpperLimitRoll() const
{
  return myUpperLimitRoll;
}

//=================================================================================================

void StepKinematics_PointOnPlanarCurvePairWithRange::SetUpperLimitRoll(
  const double theUpperLimitRoll)
{
  myUpperLimitRoll = theUpperLimitRoll;
}

//=================================================================================================

bool StepKinematics_PointOnPlanarCurvePairWithRange::HasUpperLimitRoll() const
{
  return defUpperLimitRoll;
}
