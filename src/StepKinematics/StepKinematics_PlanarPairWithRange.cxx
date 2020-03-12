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

#include <StepKinematics_PlanarPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_PlanarPairWithRange, StepKinematics_PlanarPair)

//=======================================================================
//function : StepKinematics_PlanarPairWithRange
//purpose  :
//=======================================================================
StepKinematics_PlanarPairWithRange::StepKinematics_PlanarPairWithRange ()
{
  defLowerLimitActualRotation = Standard_False;
  defUpperLimitActualRotation = Standard_False;
  defLowerLimitActualTranslationX = Standard_False;
  defUpperLimitActualTranslationX = Standard_False;
  defLowerLimitActualTranslationY = Standard_False;
  defUpperLimitActualTranslationY = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                               const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                               const Standard_Boolean hasItemDefinedTransformation_Description,
                                               const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                               const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                               const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                               const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                               const Standard_Boolean theLowOrderKinematicPair_TX,
                                               const Standard_Boolean theLowOrderKinematicPair_TY,
                                               const Standard_Boolean theLowOrderKinematicPair_TZ,
                                               const Standard_Boolean theLowOrderKinematicPair_RX,
                                               const Standard_Boolean theLowOrderKinematicPair_RY,
                                               const Standard_Boolean theLowOrderKinematicPair_RZ,
                                               const Standard_Boolean hasLowerLimitActualRotation,
                                               const Standard_Real theLowerLimitActualRotation,
                                               const Standard_Boolean hasUpperLimitActualRotation,
                                               const Standard_Real theUpperLimitActualRotation,
                                               const Standard_Boolean hasLowerLimitActualTranslationX,
                                               const Standard_Real theLowerLimitActualTranslationX,
                                               const Standard_Boolean hasUpperLimitActualTranslationX,
                                               const Standard_Real theUpperLimitActualTranslationX,
                                               const Standard_Boolean hasLowerLimitActualTranslationY,
                                               const Standard_Real theLowerLimitActualTranslationY,
                                               const Standard_Boolean hasUpperLimitActualTranslationY,
                                               const Standard_Real theUpperLimitActualTranslationY)
{
  StepKinematics_PlanarPair::Init(theRepresentationItem_Name,
                                  theItemDefinedTransformation_Name,
                                  hasItemDefinedTransformation_Description,
                                  theItemDefinedTransformation_Description,
                                  theItemDefinedTransformation_TransformItem1,
                                  theItemDefinedTransformation_TransformItem2,
                                  theKinematicPair_Joint,
                                  theLowOrderKinematicPair_TX,
                                  theLowOrderKinematicPair_TY,
                                  theLowOrderKinematicPair_TZ,
                                  theLowOrderKinematicPair_RX,
                                  theLowOrderKinematicPair_RY,
                                  theLowOrderKinematicPair_RZ);

  defLowerLimitActualRotation = hasLowerLimitActualRotation;
  if (defLowerLimitActualRotation) {
    myLowerLimitActualRotation = theLowerLimitActualRotation;
  }
  else myLowerLimitActualRotation = 0;

  defUpperLimitActualRotation = hasUpperLimitActualRotation;
  if (defUpperLimitActualRotation) {
    myUpperLimitActualRotation = theUpperLimitActualRotation;
  }
  else myUpperLimitActualRotation = 0;

  defLowerLimitActualTranslationX = hasLowerLimitActualTranslationX;
  if (defLowerLimitActualTranslationX) {
    myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
  }
  else myLowerLimitActualTranslationX = 0;

  defUpperLimitActualTranslationX = hasUpperLimitActualTranslationX;
  if (defUpperLimitActualTranslationX) {
    myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
  }
  else myUpperLimitActualTranslationX = 0;

  defLowerLimitActualTranslationY = hasLowerLimitActualTranslationY;
  if (defLowerLimitActualTranslationY) {
    myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
  }
  else myLowerLimitActualTranslationY = 0;

  defUpperLimitActualTranslationY = hasUpperLimitActualTranslationY;
  if (defUpperLimitActualTranslationY) {
    myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
  }
  else myUpperLimitActualTranslationY = 0;
}

//=======================================================================
//function : LowerLimitActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::LowerLimitActualRotation () const
{
  return myLowerLimitActualRotation;
}

//=======================================================================
//function : SetLowerLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetLowerLimitActualRotation (const Standard_Real theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=======================================================================
//function : HasLowerLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasLowerLimitActualRotation () const
{
  return defLowerLimitActualRotation;
}

//=======================================================================
//function : UpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::UpperLimitActualRotation () const
{
  return myUpperLimitActualRotation;
}

//=======================================================================
//function : SetUpperLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetUpperLimitActualRotation (const Standard_Real theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=======================================================================
//function : HasUpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasUpperLimitActualRotation () const
{
  return defUpperLimitActualRotation;
}

//=======================================================================
//function : LowerLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::LowerLimitActualTranslationX () const
{
  return myLowerLimitActualTranslationX;
}

//=======================================================================
//function : SetLowerLimitActualTranslationX
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetLowerLimitActualTranslationX (const Standard_Real theLowerLimitActualTranslationX)
{
  myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
}

//=======================================================================
//function : HasLowerLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasLowerLimitActualTranslationX () const
{
  return defLowerLimitActualTranslationX;
}

//=======================================================================
//function : UpperLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::UpperLimitActualTranslationX () const
{
  return myUpperLimitActualTranslationX;
}

//=======================================================================
//function : SetUpperLimitActualTranslationX
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetUpperLimitActualTranslationX (const Standard_Real theUpperLimitActualTranslationX)
{
  myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
}

//=======================================================================
//function : HasUpperLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasUpperLimitActualTranslationX () const
{
  return defUpperLimitActualTranslationX;
}

//=======================================================================
//function : LowerLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::LowerLimitActualTranslationY () const
{
  return myLowerLimitActualTranslationY;
}

//=======================================================================
//function : SetLowerLimitActualTranslationY
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetLowerLimitActualTranslationY (const Standard_Real theLowerLimitActualTranslationY)
{
  myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
}

//=======================================================================
//function : HasLowerLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasLowerLimitActualTranslationY () const
{
  return defLowerLimitActualTranslationY;
}

//=======================================================================
//function : UpperLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_PlanarPairWithRange::UpperLimitActualTranslationY () const
{
  return myUpperLimitActualTranslationY;
}

//=======================================================================
//function : SetUpperLimitActualTranslationY
//purpose  :
//=======================================================================
void StepKinematics_PlanarPairWithRange::SetUpperLimitActualTranslationY (const Standard_Real theUpperLimitActualTranslationY)
{
  myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
}

//=======================================================================
//function : HasUpperLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_PlanarPairWithRange::HasUpperLimitActualTranslationY () const
{
  return defUpperLimitActualTranslationY;
}
