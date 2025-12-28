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

#ifndef _StepKinematics_PlanarPairWithRange_HeaderFile_
#define _StepKinematics_PlanarPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PlanarPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity PlanarPairWithRange
class StepKinematics_PlanarPairWithRange : public StepKinematics_PlanarPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_PlanarPairWithRange();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                       hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const bool                       theLowOrderKinematicPair_TX,
    const bool                       theLowOrderKinematicPair_TY,
    const bool                       theLowOrderKinematicPair_TZ,
    const bool                       theLowOrderKinematicPair_RX,
    const bool                       theLowOrderKinematicPair_RY,
    const bool                       theLowOrderKinematicPair_RZ,
    const bool                       hasLowerLimitActualRotation,
    const double                          theLowerLimitActualRotation,
    const bool                       hasUpperLimitActualRotation,
    const double                          theUpperLimitActualRotation,
    const bool                       hasLowerLimitActualTranslationX,
    const double                          theLowerLimitActualTranslationX,
    const bool                       hasUpperLimitActualTranslationX,
    const double                          theUpperLimitActualTranslationX,
    const bool                       hasLowerLimitActualTranslationY,
    const double                          theLowerLimitActualTranslationY,
    const bool                       hasUpperLimitActualTranslationY,
    const double                          theUpperLimitActualTranslationY);

  //! Returns field LowerLimitActualRotation
  Standard_EXPORT double LowerLimitActualRotation() const;
  //! Sets field LowerLimitActualRotation
  Standard_EXPORT void SetLowerLimitActualRotation(const double theLowerLimitActualRotation);
  //! Returns True if optional field LowerLimitActualRotation is defined
  Standard_EXPORT bool HasLowerLimitActualRotation() const;

  //! Returns field UpperLimitActualRotation
  Standard_EXPORT double UpperLimitActualRotation() const;
  //! Sets field UpperLimitActualRotation
  Standard_EXPORT void SetUpperLimitActualRotation(const double theUpperLimitActualRotation);
  //! Returns True if optional field UpperLimitActualRotation is defined
  Standard_EXPORT bool HasUpperLimitActualRotation() const;

  //! Returns field LowerLimitActualTranslationX
  Standard_EXPORT double LowerLimitActualTranslationX() const;
  //! Sets field LowerLimitActualTranslationX
  Standard_EXPORT void SetLowerLimitActualTranslationX(
    const double theLowerLimitActualTranslationX);
  //! Returns True if optional field LowerLimitActualTranslationX is defined
  Standard_EXPORT bool HasLowerLimitActualTranslationX() const;

  //! Returns field UpperLimitActualTranslationX
  Standard_EXPORT double UpperLimitActualTranslationX() const;
  //! Sets field UpperLimitActualTranslationX
  Standard_EXPORT void SetUpperLimitActualTranslationX(
    const double theUpperLimitActualTranslationX);
  //! Returns True if optional field UpperLimitActualTranslationX is defined
  Standard_EXPORT bool HasUpperLimitActualTranslationX() const;

  //! Returns field LowerLimitActualTranslationY
  Standard_EXPORT double LowerLimitActualTranslationY() const;
  //! Sets field LowerLimitActualTranslationY
  Standard_EXPORT void SetLowerLimitActualTranslationY(
    const double theLowerLimitActualTranslationY);
  //! Returns True if optional field LowerLimitActualTranslationY is defined
  Standard_EXPORT bool HasLowerLimitActualTranslationY() const;

  //! Returns field UpperLimitActualTranslationY
  Standard_EXPORT double UpperLimitActualTranslationY() const;
  //! Sets field UpperLimitActualTranslationY
  Standard_EXPORT void SetUpperLimitActualTranslationY(
    const double theUpperLimitActualTranslationY);
  //! Returns True if optional field UpperLimitActualTranslationY is defined
  Standard_EXPORT bool HasUpperLimitActualTranslationY() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_PlanarPairWithRange, StepKinematics_PlanarPair)

private:
  double    myLowerLimitActualRotation;     //!< optional
  double    myUpperLimitActualRotation;     //!< optional
  double    myLowerLimitActualTranslationX; //!< optional
  double    myUpperLimitActualTranslationX; //!< optional
  double    myLowerLimitActualTranslationY; //!< optional
  double    myUpperLimitActualTranslationY; //!< optional
  bool defLowerLimitActualRotation;    //!< flag "is LowerLimitActualRotation defined"
  bool defUpperLimitActualRotation;    //!< flag "is UpperLimitActualRotation defined"
  // clang-format off
  bool defLowerLimitActualTranslationX; //!< flag "is LowerLimitActualTranslationX defined"
  bool defUpperLimitActualTranslationX; //!< flag "is UpperLimitActualTranslationX defined"
  bool defLowerLimitActualTranslationY; //!< flag "is LowerLimitActualTranslationY defined"
  bool defUpperLimitActualTranslationY; //!< flag "is UpperLimitActualTranslationY defined"
  // clang-format on
};
#endif // _StepKinematics_PlanarPairWithRange_HeaderFile_
