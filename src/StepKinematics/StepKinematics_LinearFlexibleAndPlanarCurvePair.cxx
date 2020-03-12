// Created on : Fri May 08 19:02:07 2020 
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

#include <StepKinematics_LinearFlexibleAndPlanarCurvePair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LinearFlexibleAndPlanarCurvePair, StepKinematics_HighOrderKinematicPair)

//=======================================================================
//function : StepKinematics_LinearFlexibleAndPlanarCurvePair
//purpose  :
//=======================================================================
StepKinematics_LinearFlexibleAndPlanarCurvePair::StepKinematics_LinearFlexibleAndPlanarCurvePair ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_LinearFlexibleAndPlanarCurvePair::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                                            const Standard_Boolean hasItemDefinedTransformation_Description,
                                                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                                            const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                                            const Handle(StepGeom_Curve)& thePairCurve,
                                                            const Standard_Boolean theOrientation)
{
  StepKinematics_HighOrderKinematicPair::Init(theRepresentationItem_Name,
                                              theItemDefinedTransformation_Name,
                                              hasItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_TransformItem1,
                                              theItemDefinedTransformation_TransformItem2,
                                              theKinematicPair_Joint);

  myPairCurve = thePairCurve;

  myOrientation = theOrientation;
}

//=======================================================================
//function : PairCurve
//purpose  :
//=======================================================================
Handle(StepGeom_Curve) StepKinematics_LinearFlexibleAndPlanarCurvePair::PairCurve () const
{
  return myPairCurve;
}

//=======================================================================
//function : SetPairCurve
//purpose  :
//=======================================================================
void StepKinematics_LinearFlexibleAndPlanarCurvePair::SetPairCurve (const Handle(StepGeom_Curve)& thePairCurve)
{
  myPairCurve = thePairCurve;
}

//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LinearFlexibleAndPlanarCurvePair::Orientation () const
{
  return myOrientation;
}

//=======================================================================
//function : SetOrientation
//purpose  :
//=======================================================================
void StepKinematics_LinearFlexibleAndPlanarCurvePair::SetOrientation (const Standard_Boolean theOrientation)
{
  myOrientation = theOrientation;
}
