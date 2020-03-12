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

#include <StepKinematics_PlanarCurvePair.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PlanarCurvePair, StepKinematics_HighOrderKinematicPair)

//=======================================================================
//function : StepKinematics_PlanarCurvePair
//purpose  :
//=======================================================================
StepKinematics_PlanarCurvePair::StepKinematics_PlanarCurvePair ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_PlanarCurvePair::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                           const Standard_Boolean hasItemDefinedTransformation_Description,
                                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                           const Handle(StepGeom_Curve)& theCurve1,
                                           const Handle(StepGeom_Curve)& theCurve2,
                                           const Standard_Boolean theOrientation)
{
  StepKinematics_HighOrderKinematicPair::Init(theRepresentationItem_Name,
                                              theItemDefinedTransformation_Name,
                                              hasItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_Description,
                                              theItemDefinedTransformation_TransformItem1,
                                              theItemDefinedTransformation_TransformItem2,
                                              theKinematicPair_Joint);

  myCurve1 = theCurve1;

  myCurve2 = theCurve2;

  myOrientation = theOrientation;
}

//=======================================================================
//function : Curve1
//purpose  :
//=======================================================================
Handle(StepGeom_Curve) StepKinematics_PlanarCurvePair::Curve1 () const
{
  return myCurve1;
}

//=======================================================================
//function : SetCurve1
//purpose  :
//=======================================================================
void StepKinematics_PlanarCurvePair::SetCurve1 (const Handle(StepGeom_Curve)& theCurve1)
{
  myCurve1 = theCurve1;
}

//=======================================================================
//function : Curve2
//purpose  :
//=======================================================================
Handle(StepGeom_Curve) StepKinematics_PlanarCurvePair::Curve2 () const
{
  return myCurve2;
}

//=======================================================================
//function : SetCurve2
//purpose  :
//=======================================================================
void StepKinematics_PlanarCurvePair::SetCurve2 (const Handle(StepGeom_Curve)& theCurve2)
{
  myCurve2 = theCurve2;
}

//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarCurvePair::Orientation () const
{
  return myOrientation;
}

//=======================================================================
//function : SetOrientation
//purpose  :
//=======================================================================
void StepKinematics_PlanarCurvePair::SetOrientation (const Standard_Boolean theOrientation)
{
  myOrientation = theOrientation;
}
