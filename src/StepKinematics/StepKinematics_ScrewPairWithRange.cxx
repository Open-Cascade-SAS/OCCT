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

#include <StepKinematics_ScrewPairWithRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_ScrewPairWithRange, StepKinematics_ScrewPair)

//=======================================================================
//function : StepKinematics_ScrewPairWithRange
//purpose  :
//=======================================================================
StepKinematics_ScrewPairWithRange::StepKinematics_ScrewPairWithRange ()
{
  defLowerLimitActualRotation = Standard_False;
  defUpperLimitActualRotation = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_ScrewPairWithRange::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                              const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                                              const Standard_Boolean hasItemDefinedTransformation_Description,
                                              const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                                              const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                                              const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                                              const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                                              const Standard_Real theScrewPair_Pitch,
                                              const Standard_Boolean hasLowerLimitActualRotation,
                                              const Standard_Real theLowerLimitActualRotation,
                                              const Standard_Boolean hasUpperLimitActualRotation,
                                              const Standard_Real theUpperLimitActualRotation)
{
  StepKinematics_ScrewPair::Init(theRepresentationItem_Name,
                                 theItemDefinedTransformation_Name,
                                 hasItemDefinedTransformation_Description,
                                 theItemDefinedTransformation_Description,
                                 theItemDefinedTransformation_TransformItem1,
                                 theItemDefinedTransformation_TransformItem2,
                                 theKinematicPair_Joint,
                                 theScrewPair_Pitch);

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
Standard_Real StepKinematics_ScrewPairWithRange::LowerLimitActualRotation () const
{
  return myLowerLimitActualRotation;
}

//=======================================================================
//function : SetLowerLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_ScrewPairWithRange::SetLowerLimitActualRotation (const Standard_Real theLowerLimitActualRotation)
{
  myLowerLimitActualRotation = theLowerLimitActualRotation;
}

//=======================================================================
//function : HasLowerLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_ScrewPairWithRange::HasLowerLimitActualRotation () const
{
  return defLowerLimitActualRotation;
}

//=======================================================================
//function : UpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Real StepKinematics_ScrewPairWithRange::UpperLimitActualRotation () const
{
  return myUpperLimitActualRotation;
}

//=======================================================================
//function : SetUpperLimitActualRotation
//purpose  :
//=======================================================================
void StepKinematics_ScrewPairWithRange::SetUpperLimitActualRotation (const Standard_Real theUpperLimitActualRotation)
{
  myUpperLimitActualRotation = theUpperLimitActualRotation;
}

//=======================================================================
//function : HasUpperLimitActualRotation
//purpose  :
//=======================================================================
Standard_Boolean StepKinematics_ScrewPairWithRange::HasUpperLimitActualRotation () const
{
  return defUpperLimitActualRotation;
}
