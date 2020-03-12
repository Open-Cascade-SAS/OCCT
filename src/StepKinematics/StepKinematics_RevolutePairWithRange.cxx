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

#include <StepKinematics_RevolutePairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_RevolutePairWithRange, StepKinematics_RevolutePair)

//=======================================================================
//function : StepKinematics_RevolutePairWithRange
//purpose  :
//=======================================================================
StepKinematics_RevolutePairWithRange::StepKinematics_RevolutePairWithRange ()
{
  defLowerLimitActualRotation = Standard_False;
  defUpperLimitActualRotation = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_RevolutePairWithRange::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
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
                                                 const Standard_Real theUpperLimitActualRotation)
{
  StepKinematics_RevolutePair::Init(theRepresentationItem_Name,
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
}

//=======================================================================
//function : LowerLimitActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_RevolutePairWithRange::LowerLimitActualRotation () const
{
  return myLowerLimitActualRotation;
}

//=======================================================================
//function : SetLowerLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_RevolutePairWithRange::SetLowerLimitActualRotation (const Standard_Real theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=======================================================================
//function : HasLowerLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_RevolutePairWithRange::HasLowerLimitActualRotation () const
{
  return defLowerLimitActualRotation;
}

//=======================================================================
//function : UpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_RevolutePairWithRange::UpperLimitActualRotation () const
{
  return myUpperLimitActualRotation;
}

//=======================================================================
//function : SetUpperLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_RevolutePairWithRange::SetUpperLimitActualRotation (const Standard_Real theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=======================================================================
//function : HasUpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_RevolutePairWithRange::HasUpperLimitActualRotation () const
{
  return defUpperLimitActualRotation;
}
