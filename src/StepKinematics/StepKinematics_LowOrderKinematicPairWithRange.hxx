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

#ifndef _StepKinematics_LowOrderKinematicPairWithRange_HeaderFile_
#define _StepKinematics_LowOrderKinematicPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_LowOrderKinematicPairWithRange,
                       StepKinematics_LowOrderKinematicPair)

//! Representation of STEP entity LowOrderKinematicPairWithRange
class StepKinematics_LowOrderKinematicPairWithRange : public StepKinematics_LowOrderKinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_LowOrderKinematicPairWithRange();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&      theRepresentationItem_Name,
    const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Name,
    const Standard_Boolean                       hasItemDefinedTransformation_Description,
    const Handle(TCollection_HAsciiString)&      theItemDefinedTransformation_Description,
    const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem1,
    const Handle(StepRepr_RepresentationItem)&   theItemDefinedTransformation_TransformItem2,
    const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
    const Standard_Boolean                       theLowOrderKinematicPair_TX,
    const Standard_Boolean                       theLowOrderKinematicPair_TY,
    const Standard_Boolean                       theLowOrderKinematicPair_TZ,
    const Standard_Boolean                       theLowOrderKinematicPair_RX,
    const Standard_Boolean                       theLowOrderKinematicPair_RY,
    const Standard_Boolean                       theLowOrderKinematicPair_RZ,
    const Standard_Boolean                       hasLowerLimitActualRotationX,
    const Standard_Real                          theLowerLimitActualRotationX,
    const Standard_Boolean                       hasUpperLimitActualRotationX,
    const Standard_Real                          theUpperLimitActualRotationX,
    const Standard_Boolean                       hasLowerLimitActualRotationY,
    const Standard_Real                          theLowerLimitActualRotationY,
    const Standard_Boolean                       hasUpperLimitActualRotationY,
    const Standard_Real                          theUpperLimitActualRotationY,
    const Standard_Boolean                       hasLowerLimitActualRotationZ,
    const Standard_Real                          theLowerLimitActualRotationZ,
    const Standard_Boolean                       hasUpperLimitActualRotationZ,
    const Standard_Real                          theUpperLimitActualRotationZ,
    const Standard_Boolean                       hasLowerLimitActualTranslationX,
    const Standard_Real                          theLowerLimitActualTranslationX,
    const Standard_Boolean                       hasUpperLimitActualTranslationX,
    const Standard_Real                          theUpperLimitActualTranslationX,
    const Standard_Boolean                       hasLowerLimitActualTranslationY,
    const Standard_Real                          theLowerLimitActualTranslationY,
    const Standard_Boolean                       hasUpperLimitActualTranslationY,
    const Standard_Real                          theUpperLimitActualTranslationY,
    const Standard_Boolean                       hasLowerLimitActualTranslationZ,
    const Standard_Real                          theLowerLimitActualTranslationZ,
    const Standard_Boolean                       hasUpperLimitActualTranslationZ,
    const Standard_Real                          theUpperLimitActualTranslationZ);

  //! Returns field LowerLimitActualRotationX
  Standard_EXPORT Standard_Real LowerLimitActualRotationX() const;
  //! Sets field LowerLimitActualRotationX
  Standard_EXPORT void SetLowerLimitActualRotationX(
    const Standard_Real theLowerLimitActualRotationX);
  //! Returns True if optional field LowerLimitActualRotationX is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualRotationX() const;

  //! Returns field UpperLimitActualRotationX
  Standard_EXPORT Standard_Real UpperLimitActualRotationX() const;
  //! Sets field UpperLimitActualRotationX
  Standard_EXPORT void SetUpperLimitActualRotationX(
    const Standard_Real theUpperLimitActualRotationX);
  //! Returns True if optional field UpperLimitActualRotationX is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualRotationX() const;

  //! Returns field LowerLimitActualRotationY
  Standard_EXPORT Standard_Real LowerLimitActualRotationY() const;
  //! Sets field LowerLimitActualRotationY
  Standard_EXPORT void SetLowerLimitActualRotationY(
    const Standard_Real theLowerLimitActualRotationY);
  //! Returns True if optional field LowerLimitActualRotationY is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualRotationY() const;

  //! Returns field UpperLimitActualRotationY
  Standard_EXPORT Standard_Real UpperLimitActualRotationY() const;
  //! Sets field UpperLimitActualRotationY
  Standard_EXPORT void SetUpperLimitActualRotationY(
    const Standard_Real theUpperLimitActualRotationY);
  //! Returns True if optional field UpperLimitActualRotationY is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualRotationY() const;

  //! Returns field LowerLimitActualRotationZ
  Standard_EXPORT Standard_Real LowerLimitActualRotationZ() const;
  //! Sets field LowerLimitActualRotationZ
  Standard_EXPORT void SetLowerLimitActualRotationZ(
    const Standard_Real theLowerLimitActualRotationZ);
  //! Returns True if optional field LowerLimitActualRotationZ is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualRotationZ() const;

  //! Returns field UpperLimitActualRotationZ
  Standard_EXPORT Standard_Real UpperLimitActualRotationZ() const;
  //! Sets field UpperLimitActualRotationZ
  Standard_EXPORT void SetUpperLimitActualRotationZ(
    const Standard_Real theUpperLimitActualRotationZ);
  //! Returns True if optional field UpperLimitActualRotationZ is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualRotationZ() const;

  //! Returns field LowerLimitActualTranslationX
  Standard_EXPORT Standard_Real LowerLimitActualTranslationX() const;
  //! Sets field LowerLimitActualTranslationX
  Standard_EXPORT void SetLowerLimitActualTranslationX(
    const Standard_Real theLowerLimitActualTranslationX);
  //! Returns True if optional field LowerLimitActualTranslationX is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualTranslationX() const;

  //! Returns field UpperLimitActualTranslationX
  Standard_EXPORT Standard_Real UpperLimitActualTranslationX() const;
  //! Sets field UpperLimitActualTranslationX
  Standard_EXPORT void SetUpperLimitActualTranslationX(
    const Standard_Real theUpperLimitActualTranslationX);
  //! Returns True if optional field UpperLimitActualTranslationX is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualTranslationX() const;

  //! Returns field LowerLimitActualTranslationY
  Standard_EXPORT Standard_Real LowerLimitActualTranslationY() const;
  //! Sets field LowerLimitActualTranslationY
  Standard_EXPORT void SetLowerLimitActualTranslationY(
    const Standard_Real theLowerLimitActualTranslationY);
  //! Returns True if optional field LowerLimitActualTranslationY is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualTranslationY() const;

  //! Returns field UpperLimitActualTranslationY
  Standard_EXPORT Standard_Real UpperLimitActualTranslationY() const;
  //! Sets field UpperLimitActualTranslationY
  Standard_EXPORT void SetUpperLimitActualTranslationY(
    const Standard_Real theUpperLimitActualTranslationY);
  //! Returns True if optional field UpperLimitActualTranslationY is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualTranslationY() const;

  //! Returns field LowerLimitActualTranslationZ
  Standard_EXPORT Standard_Real LowerLimitActualTranslationZ() const;
  //! Sets field LowerLimitActualTranslationZ
  Standard_EXPORT void SetLowerLimitActualTranslationZ(
    const Standard_Real theLowerLimitActualTranslationZ);
  //! Returns True if optional field LowerLimitActualTranslationZ is defined
  Standard_EXPORT Standard_Boolean HasLowerLimitActualTranslationZ() const;

  //! Returns field UpperLimitActualTranslationZ
  Standard_EXPORT Standard_Real UpperLimitActualTranslationZ() const;
  //! Sets field UpperLimitActualTranslationZ
  Standard_EXPORT void SetUpperLimitActualTranslationZ(
    const Standard_Real theUpperLimitActualTranslationZ);
  //! Returns True if optional field UpperLimitActualTranslationZ is defined
  Standard_EXPORT Standard_Boolean HasUpperLimitActualTranslationZ() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairWithRange,
                          StepKinematics_LowOrderKinematicPair)

private:
  Standard_Real    myLowerLimitActualRotationX;    //!< optional
  Standard_Real    myUpperLimitActualRotationX;    //!< optional
  Standard_Real    myLowerLimitActualRotationY;    //!< optional
  Standard_Real    myUpperLimitActualRotationY;    //!< optional
  Standard_Real    myLowerLimitActualRotationZ;    //!< optional
  Standard_Real    myUpperLimitActualRotationZ;    //!< optional
  Standard_Real    myLowerLimitActualTranslationX; //!< optional
  Standard_Real    myUpperLimitActualTranslationX; //!< optional
  Standard_Real    myLowerLimitActualTranslationY; //!< optional
  Standard_Real    myUpperLimitActualTranslationY; //!< optional
  Standard_Real    myLowerLimitActualTranslationZ; //!< optional
  Standard_Real    myUpperLimitActualTranslationZ; //!< optional
  Standard_Boolean defLowerLimitActualRotationX;   //!< flag "is LowerLimitActualRotationX defined"
  Standard_Boolean defUpperLimitActualRotationX;   //!< flag "is UpperLimitActualRotationX defined"
  Standard_Boolean defLowerLimitActualRotationY;   //!< flag "is LowerLimitActualRotationY defined"
  Standard_Boolean defUpperLimitActualRotationY;   //!< flag "is UpperLimitActualRotationY defined"
  Standard_Boolean defLowerLimitActualRotationZ;   //!< flag "is LowerLimitActualRotationZ defined"
  Standard_Boolean defUpperLimitActualRotationZ;   //!< flag "is UpperLimitActualRotationZ defined"
  // clang-format off
  Standard_Boolean defLowerLimitActualTranslationX; //!< flag "is LowerLimitActualTranslationX defined"
  Standard_Boolean defUpperLimitActualTranslationX; //!< flag "is UpperLimitActualTranslationX defined"
  Standard_Boolean defLowerLimitActualTranslationY; //!< flag "is LowerLimitActualTranslationY defined"
  Standard_Boolean defUpperLimitActualTranslationY; //!< flag "is UpperLimitActualTranslationY defined"
  Standard_Boolean defLowerLimitActualTranslationZ; //!< flag "is LowerLimitActualTranslationZ defined"
  Standard_Boolean defUpperLimitActualTranslationZ; //!< flag "is UpperLimitActualTranslationZ defined"
  // clang-format on
};
#endif // _StepKinematics_LowOrderKinematicPairWithRange_HeaderFile_
