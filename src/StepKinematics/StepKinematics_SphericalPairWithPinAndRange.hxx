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

#ifndef _StepKinematics_SphericalPairWithPinAndRange_HeaderFile_
#define _StepKinematics_SphericalPairWithPinAndRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_SphericalPairWithPin.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_SphericalPairWithPinAndRange, StepKinematics_SphericalPairWithPin)

//! Representation of STEP entity SphericalPairWithPinAndRange
class StepKinematics_SphericalPairWithPinAndRange : public StepKinematics_SphericalPairWithPin
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_SphericalPairWithPinAndRange();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
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
                           const Standard_Boolean hasLowerLimitYaw,
                           const Standard_Real theLowerLimitYaw,
                           const Standard_Boolean hasUpperLimitYaw,
                           const Standard_Real theUpperLimitYaw,
                           const Standard_Boolean hasLowerLimitRoll,
                           const Standard_Real theLowerLimitRoll,
                           const Standard_Boolean hasUpperLimitRoll,
                           const Standard_Real theUpperLimitRoll);

  //! Returns field LowerLimitYaw
  Standard_EXPORT Standard_Real LowerLimitYaw() const;
  //! Sets field LowerLimitYaw
  Standard_EXPORT void SetLowerLimitYaw (const Standard_Real theLowerLimitYaw);
  //! Returns True if optional field LowerLimitYaw is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitYaw() const;

  //! Returns field UpperLimitYaw
  Standard_EXPORT Standard_Real UpperLimitYaw() const;
  //! Sets field UpperLimitYaw
  Standard_EXPORT void SetUpperLimitYaw (const Standard_Real theUpperLimitYaw);
  //! Returns True if optional field UpperLimitYaw is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitYaw() const;

  //! Returns field LowerLimitRoll
  Standard_EXPORT Standard_Real LowerLimitRoll() const;
  //! Sets field LowerLimitRoll
  Standard_EXPORT void SetLowerLimitRoll (const Standard_Real theLowerLimitRoll);
  //! Returns True if optional field LowerLimitRoll is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitRoll() const;

  //! Returns field UpperLimitRoll
  Standard_EXPORT Standard_Real UpperLimitRoll() const;
  //! Sets field UpperLimitRoll
  Standard_EXPORT void SetUpperLimitRoll (const Standard_Real theUpperLimitRoll);
  //! Returns True if optional field UpperLimitRoll is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitRoll() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_SphericalPairWithPinAndRange, StepKinematics_SphericalPairWithPin)

private:
  Standard_Real myLowerLimitYaw; //!< optional
  Standard_Real myUpperLimitYaw; //!< optional
  Standard_Real myLowerLimitRoll; //!< optional
  Standard_Real myUpperLimitRoll; //!< optional
  Standard_Boolean defLowerLimitYaw; //!< flag "is LowerLimitYaw defined"
  Standard_Boolean defUpperLimitYaw; //!< flag "is UpperLimitYaw defined"
  Standard_Boolean defLowerLimitRoll; //!< flag "is LowerLimitRoll defined"
  Standard_Boolean defUpperLimitRoll; //!< flag "is UpperLimitRoll defined"

};
#endif // _StepKinematics_SphericalPairWithPinAndRange_HeaderFile_
