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

#include <StepKinematics_LowOrderKinematicPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairWithRange, StepKinematics_LowOrderKinematicPair)

//=======================================================================
//function : StepKinematics_LowOrderKinematicPairWithRange
//purpose  :
//=======================================================================
StepKinematics_LowOrderKinematicPairWithRange::StepKinematics_LowOrderKinematicPairWithRange ()
{
  defLowerLimitActualRotationX = Standard_False;
  defUpperLimitActualRotationX = Standard_False;
  defLowerLimitActualRotationY = Standard_False;
  defUpperLimitActualRotationY = Standard_False;
  defLowerLimitActualRotationZ = Standard_False;
  defUpperLimitActualRotationZ = Standard_False;
  defLowerLimitActualTranslationX = Standard_False;
  defUpperLimitActualTranslationX = Standard_False;
  defLowerLimitActualTranslationY = Standard_False;
  defUpperLimitActualTranslationY = Standard_False;
  defLowerLimitActualTranslationZ = Standard_False;
  defUpperLimitActualTranslationZ = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
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
                                                          const Standard_Boolean hasLowerLimitActualRotationX,
                                                          const Standard_Real theLowerLimitActualRotationX,
                                                          const Standard_Boolean hasUpperLimitActualRotationX,
                                                          const Standard_Real theUpperLimitActualRotationX,
                                                          const Standard_Boolean hasLowerLimitActualRotationY,
                                                          const Standard_Real theLowerLimitActualRotationY,
                                                          const Standard_Boolean hasUpperLimitActualRotationY,
                                                          const Standard_Real theUpperLimitActualRotationY,
                                                          const Standard_Boolean hasLowerLimitActualRotationZ,
                                                          const Standard_Real theLowerLimitActualRotationZ,
                                                          const Standard_Boolean hasUpperLimitActualRotationZ,
                                                          const Standard_Real theUpperLimitActualRotationZ,
                                                          const Standard_Boolean hasLowerLimitActualTranslationX,
                                                          const Standard_Real theLowerLimitActualTranslationX,
                                                          const Standard_Boolean hasUpperLimitActualTranslationX,
                                                          const Standard_Real theUpperLimitActualTranslationX,
                                                          const Standard_Boolean hasLowerLimitActualTranslationY,
                                                          const Standard_Real theLowerLimitActualTranslationY,
                                                          const Standard_Boolean hasUpperLimitActualTranslationY,
                                                          const Standard_Real theUpperLimitActualTranslationY,
                                                          const Standard_Boolean hasLowerLimitActualTranslationZ,
                                                          const Standard_Real theLowerLimitActualTranslationZ,
                                                          const Standard_Boolean hasUpperLimitActualTranslationZ,
                                                          const Standard_Real theUpperLimitActualTranslationZ)
{
  StepKinematics_LowOrderKinematicPair::Init(theRepresentationItem_Name,
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

  defLowerLimitActualRotationX = hasLowerLimitActualRotationX;
  if (defLowerLimitActualRotationX) {
    myLowerLimitActualRotationX = theLowerLimitActualRotationX;
  }
  else myLowerLimitActualRotationX = 0;

  defUpperLimitActualRotationX = hasUpperLimitActualRotationX;
  if (defUpperLimitActualRotationX) {
    myUpperLimitActualRotationX = theUpperLimitActualRotationX;
  }
  else myUpperLimitActualRotationX = 0;

  defLowerLimitActualRotationY = hasLowerLimitActualRotationY;
  if (defLowerLimitActualRotationY) {
    myLowerLimitActualRotationY = theLowerLimitActualRotationY;
  }
  else myLowerLimitActualRotationY = 0;

  defUpperLimitActualRotationY = hasUpperLimitActualRotationY;
  if (defUpperLimitActualRotationY) {
    myUpperLimitActualRotationY = theUpperLimitActualRotationY;
  }
  else myUpperLimitActualRotationY = 0;

  defLowerLimitActualRotationZ = hasLowerLimitActualRotationZ;
  if (defLowerLimitActualRotationZ) {
    myLowerLimitActualRotationZ = theLowerLimitActualRotationZ;
  }
  else myLowerLimitActualRotationZ = 0;

  defUpperLimitActualRotationZ = hasUpperLimitActualRotationZ;
  if (defUpperLimitActualRotationZ) {
    myUpperLimitActualRotationZ = theUpperLimitActualRotationZ;
  }
  else myUpperLimitActualRotationZ = 0;

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

  defLowerLimitActualTranslationZ = hasLowerLimitActualTranslationZ;
  if (defLowerLimitActualTranslationZ) {
    myLowerLimitActualTranslationZ = theLowerLimitActualTranslationZ;
  }
  else myLowerLimitActualTranslationZ = 0;

  defUpperLimitActualTranslationZ = hasUpperLimitActualTranslationZ;
  if (defUpperLimitActualTranslationZ) {
    myUpperLimitActualTranslationZ = theUpperLimitActualTranslationZ;
  }
  else myUpperLimitActualTranslationZ = 0;
}

//=======================================================================
//function : LowerLimitActualRotationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationX () const
{
  return myLowerLimitActualRotationX;
}

//=======================================================================
//function : SetLowerLimitActualRotationX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationX (const Standard_Real theLowerLimitActualRotationX)
{
  myLowerLimitActualRotationX = theLowerLimitActualRotationX;
}

//=======================================================================
//function : HasLowerLimitActualRotationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationX () const
{
  return defLowerLimitActualRotationX;
}

//=======================================================================
//function : UpperLimitActualRotationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationX () const
{
  return myUpperLimitActualRotationX;
}

//=======================================================================
//function : SetUpperLimitActualRotationX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationX (const Standard_Real theUpperLimitActualRotationX)
{
  myUpperLimitActualRotationX = theUpperLimitActualRotationX;
}

//=======================================================================
//function : HasUpperLimitActualRotationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationX () const
{
  return defUpperLimitActualRotationX;
}

//=======================================================================
//function : LowerLimitActualRotationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationY () const
{
  return myLowerLimitActualRotationY;
}

//=======================================================================
//function : SetLowerLimitActualRotationY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationY (const Standard_Real theLowerLimitActualRotationY)
{
  myLowerLimitActualRotationY = theLowerLimitActualRotationY;
}

//=======================================================================
//function : HasLowerLimitActualRotationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationY () const
{
  return defLowerLimitActualRotationY;
}

//=======================================================================
//function : UpperLimitActualRotationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationY () const
{
  return myUpperLimitActualRotationY;
}

//=======================================================================
//function : SetUpperLimitActualRotationY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationY (const Standard_Real theUpperLimitActualRotationY)
{
  myUpperLimitActualRotationY = theUpperLimitActualRotationY;
}

//=======================================================================
//function : HasUpperLimitActualRotationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationY () const
{
  return defUpperLimitActualRotationY;
}

//=======================================================================
//function : LowerLimitActualRotationZ
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualRotationZ () const
{
  return myLowerLimitActualRotationZ;
}

//=======================================================================
//function : SetLowerLimitActualRotationZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualRotationZ (const Standard_Real theLowerLimitActualRotationZ)
{
  myLowerLimitActualRotationZ = theLowerLimitActualRotationZ;
}

//=======================================================================
//function : HasLowerLimitActualRotationZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualRotationZ () const
{
  return defLowerLimitActualRotationZ;
}

//=======================================================================
//function : UpperLimitActualRotationZ
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualRotationZ () const
{
  return myUpperLimitActualRotationZ;
}

//=======================================================================
//function : SetUpperLimitActualRotationZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualRotationZ (const Standard_Real theUpperLimitActualRotationZ)
{
  myUpperLimitActualRotationZ = theUpperLimitActualRotationZ;
}

//=======================================================================
//function : HasUpperLimitActualRotationZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualRotationZ () const
{
  return defUpperLimitActualRotationZ;
}

//=======================================================================
//function : LowerLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationX () const
{
  return myLowerLimitActualTranslationX;
}

//=======================================================================
//function : SetLowerLimitActualTranslationX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationX (const Standard_Real theLowerLimitActualTranslationX)
{
  myLowerLimitActualTranslationX = theLowerLimitActualTranslationX;
}

//=======================================================================
//function : HasLowerLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationX () const
{
  return defLowerLimitActualTranslationX;
}

//=======================================================================
//function : UpperLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationX () const
{
  return myUpperLimitActualTranslationX;
}

//=======================================================================
//function : SetUpperLimitActualTranslationX
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationX (const Standard_Real theUpperLimitActualTranslationX)
{
  myUpperLimitActualTranslationX = theUpperLimitActualTranslationX;
}

//=======================================================================
//function : HasUpperLimitActualTranslationX
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationX () const
{
  return defUpperLimitActualTranslationX;
}

//=======================================================================
//function : LowerLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationY () const
{
  return myLowerLimitActualTranslationY;
}

//=======================================================================
//function : SetLowerLimitActualTranslationY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationY (const Standard_Real theLowerLimitActualTranslationY)
{
  myLowerLimitActualTranslationY = theLowerLimitActualTranslationY;
}

//=======================================================================
//function : HasLowerLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationY () const
{
  return defLowerLimitActualTranslationY;
}

//=======================================================================
//function : UpperLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationY () const
{
  return myUpperLimitActualTranslationY;
}

//=======================================================================
//function : SetUpperLimitActualTranslationY
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationY (const Standard_Real theUpperLimitActualTranslationY)
{
  myUpperLimitActualTranslationY = theUpperLimitActualTranslationY;
}

//=======================================================================
//function : HasUpperLimitActualTranslationY
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationY () const
{
  return defUpperLimitActualTranslationY;
}

//=======================================================================
//function : LowerLimitActualTranslationZ
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::LowerLimitActualTranslationZ () const
{
  return myLowerLimitActualTranslationZ;
}

//=======================================================================
//function : SetLowerLimitActualTranslationZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetLowerLimitActualTranslationZ (const Standard_Real theLowerLimitActualTranslationZ)
{
  myLowerLimitActualTranslationZ = theLowerLimitActualTranslationZ;
}

//=======================================================================
//function : HasLowerLimitActualTranslationZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasLowerLimitActualTranslationZ () const
{
  return defLowerLimitActualTranslationZ;
}

//=======================================================================
//function : UpperLimitActualTranslationZ
//purpose  :
//=======================================================================
Standard_Real StepKinematics_LowOrderKinematicPairWithRange::UpperLimitActualTranslationZ () const
{
  return myUpperLimitActualTranslationZ;
}

//=======================================================================
//function : SetUpperLimitActualTranslationZ
//purpose  :
//=======================================================================
void StepKinematics_LowOrderKinematicPairWithRange::SetUpperLimitActualTranslationZ (const Standard_Real theUpperLimitActualTranslationZ)
{
  myUpperLimitActualTranslationZ = theUpperLimitActualTranslationZ;
}

//=======================================================================
//function : HasUpperLimitActualTranslationZ
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_LowOrderKinematicPairWithRange::HasUpperLimitActualTranslationZ () const
{
  return defUpperLimitActualTranslationZ;
}
