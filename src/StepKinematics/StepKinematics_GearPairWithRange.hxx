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

#ifndef _StepKinematics_GearPairWithRange_HeaderFile_
#define _StepKinematics_GearPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_GearPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_GearPairWithRange, StepKinematics_GearPair)

//! Representation of STEP entity GearPairWithRange
class StepKinematics_GearPairWithRange : public StepKinematics_GearPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_GearPairWithRange();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Real theGearPair_RadiusFirstLink,
                           const Standard_Real theGearPair_RadiusSecondLink,
                           const Standard_Real theGearPair_Bevel,
                           const Standard_Real theGearPair_HelicalAngle,
                           const Standard_Real theGearPair_GearRatio,
                           const Standard_Boolean hasLowerLimitActualRotation1,
                           const Standard_Real theLowerLimitActualRotation1,
                           const Standard_Boolean hasUpperLimitActualRotation1,
                           const Standard_Real theUpperLimitActualRotation1);

  //! Returns field LowerLimitActualRotation1
  Standard_EXPORT Standard_Real LowerLimitActualRotation1() const;
  //! Sets field LowerLimitActualRotation1
  Standard_EXPORT void SetLowerLimitActualRotation1 (const Standard_Real theLowerLimitActualRotation1);
  //! Returns True if optional field LowerLimitActualRotation1 is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualRotation1() const;

  //! Returns field UpperLimitActualRotation1
  Standard_EXPORT Standard_Real UpperLimitActualRotation1() const;
  //! Sets field UpperLimitActualRotation1
  Standard_EXPORT void SetUpperLimitActualRotation1 (const Standard_Real theUpperLimitActualRotation1);
  //! Returns True if optional field UpperLimitActualRotation1 is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualRotation1() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_GearPairWithRange, StepKinematics_GearPair)

private:
  Standard_Real myLowerLimitActualRotation1; //!< optional
  Standard_Real myUpperLimitActualRotation1; //!< optional
  Standard_Boolean defLowerLimitActualRotation1; //!< flag "is LowerLimitActualRotation1 defined"
  Standard_Boolean defUpperLimitActualRotation1; //!< flag "is UpperLimitActualRotation1 defined"

};
#endif // _StepKinematics_GearPairWithRange_HeaderFile_
