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

#include <StepKinematics_SurfacePairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_SurfacePairWithRange, StepKinematics_SurfacePair)

//=================================================================================================

StepKinematics_SurfacePairWithRange::StepKinematics_SurfacePairWithRange()
{
  defLowerLimitActualRotation = Standard_False;
  defUpperLimitActualRotation = Standard_False;
}

//=================================================================================================

void StepKinematics_SurfacePairWithRange::Init(
  const Handle(TCollection_HAsciiString)&           theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&           theItemDefinedTransformation_Name,
  const Standard_Boolean                            hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&           theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&        theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&        theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)&      theKinematicPair_Joint,
  const Handle(StepGeom_Surface)&                   theSurfacePair_Surface1,
  const Handle(StepGeom_Surface)&                   theSurfacePair_Surface2,
  const Standard_Boolean                            theSurfacePair_Orientation,
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnSurface1,
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnSurface2,
  const Standard_Boolean                            hasLowerLimitActualRotation,
  const Standard_Real                               theLowerLimitActualRotation,
  const Standard_Boolean                            hasUpperLimitActualRotation,
  const Standard_Real                               theUpperLimitActualRotation)
{
  StepKinematics_SurfacePair::Init(theRepresentationItem_Name,
                                   theItemDefinedTransformation_Name,
                                   hasItemDefinedTransformation_Description,
                                   theItemDefinedTransformation_Description,
                                   theItemDefinedTransformation_TransformItem1,
                                   theItemDefinedTransformation_TransformItem2,
                                   theKinematicPair_Joint,
                                   theSurfacePair_Surface1,
                                   theSurfacePair_Surface2,
                                   theSurfacePair_Orientation);

  myRangeOnSurface1 = theRangeOnSurface1;

  myRangeOnSurface2 = theRangeOnSurface2;

  defLowerLimitActualRotation = hasLowerLimitActualRotation;
  if (defLowerLimitActualRotation)
  {
    myLowerLimitActualRotation = theLowerLimitActualRotation;
  }
  else
    myLowerLimitActualRotation = 0;

  defUpperLimitActualRotation = hasUpperLimitActualRotation;
  if (defUpperLimitActualRotation)
  {
    myUpperLimitActualRotation = theUpperLimitActualRotation;
  }
  else
    myUpperLimitActualRotation = 0;
}

//=================================================================================================

Handle(StepGeom_RectangularTrimmedSurface) StepKinematics_SurfacePairWithRange::RangeOnSurface1()
  const
{
  return myRangeOnSurface1;
}

//=================================================================================================

void StepKinematics_SurfacePairWithRange::SetRangeOnSurface1(
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnSurface1)
{
  myRangeOnSurface1 = theRangeOnSurface1;
}

//=================================================================================================

Handle(StepGeom_RectangularTrimmedSurface) StepKinematics_SurfacePairWithRange::RangeOnSurface2()
  const
{
  return myRangeOnSurface2;
}

//=================================================================================================

void StepKinematics_SurfacePairWithRange::SetRangeOnSurface2(
  const Handle(StepGeom_RectangularTrimmedSurface)& theRangeOnSurface2)
{
  myRangeOnSurface2 = theRangeOnSurface2;
}

//=================================================================================================

Standard_Real StepKinematics_SurfacePairWithRange::LowerLimitActualRotation() const
{
  return myLowerLimitActualRotation;
}

//=================================================================================================

void StepKinematics_SurfacePairWithRange::SetLowerLimitActualRotation(
  const Standard_Real theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_SurfacePairWithRange::HasLowerLimitActualRotation() const
{
  return defLowerLimitActualRotation;
}

//=================================================================================================

Standard_Real StepKinematics_SurfacePairWithRange::UpperLimitActualRotation() const
{
  return myUpperLimitActualRotation;
}

//=================================================================================================

void StepKinematics_SurfacePairWithRange::SetUpperLimitActualRotation(
  const Standard_Real theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=================================================================================================

Standard_Boolean StepKinematics_SurfacePairWithRange::HasUpperLimitActualRotation() const
{
  return defUpperLimitActualRotation;
}
