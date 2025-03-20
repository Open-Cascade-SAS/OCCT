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

#include <StepKinematics_PlanarCurvePairRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PlanarCurvePairRange, StepKinematics_PlanarCurvePair)

//=================================================================================================

StepKinematics_PlanarCurvePairRange::StepKinematics_PlanarCurvePairRange() {}

//=================================================================================================

void StepKinematics_PlanarCurvePairRange::Init(
  const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
  const Standard_Boolean                       hasItemDefinedTransformation_Description,
  const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
  const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
  const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
  const Handle(StepGeom_Curve)&                thePlanarCurvePair_Curve1,
  const Handle(StepGeom_Curve)&                thePlanarCurvePair_Curve2,
  const Standard_Boolean                       thePlanarCurvePair_Orientation,
  const Handle(StepGeom_TrimmedCurve)&         theRangeOnCurve1,
  const Handle(StepGeom_TrimmedCurve)&         theRangeOnCurve2)
{
  StepKinematics_PlanarCurvePair::Init(theRepresentationItem_Name,
                                       theItemDefinedTransformation_Name,
                                       hasItemDefinedTransformation_Description,
                                       theItemDefinedTransformation_Description,
                                       theItemDefinedTransformation_TransformItem1,
                                       theItemDefinedTransformation_TransformItem2,
                                       theKinematicPair_Joint,
                                       thePlanarCurvePair_Curve1,
                                       thePlanarCurvePair_Curve2,
                                       thePlanarCurvePair_Orientation);

  myRangeOnCurve1 = theRangeOnCurve1;

  myRangeOnCurve2 = theRangeOnCurve2;
}

//=================================================================================================

Handle(StepGeom_TrimmedCurve) StepKinematics_PlanarCurvePairRange::RangeOnCurve1() const
{
  return myRangeOnCurve1;
}

//=================================================================================================

void StepKinematics_PlanarCurvePairRange::SetRangeOnCurve1(
  const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve1)
{
  myRangeOnCurve1 = theRangeOnCurve1;
}

//=================================================================================================

Handle(StepGeom_TrimmedCurve) StepKinematics_PlanarCurvePairRange::RangeOnCurve2() const
{
  return myRangeOnCurve2;
}

//=================================================================================================

void StepKinematics_PlanarCurvePairRange::SetRangeOnCurve2(
  const Handle(StepGeom_TrimmedCurve)& theRangeOnCurve2)
{
  myRangeOnCurve2 = theRangeOnCurve2;
}
