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

#ifndef _StepKinematics_CylindricalPairWithRange_HeaderFile_
#define _StepKinematics_CylindricalPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_CylindricalPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity CylindricalPairWithRange
class StepKinematics_CylindricalPairWithRange : public StepKinematics_CylindricalPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_CylindricalPairWithRange();

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
    const bool                       hasLowerLimitActualTranslation,
    const double                          theLowerLimitActualTranslation,
    const bool                       hasUpperLimitActualTranslation,
    const double                          theUpperLimitActualTranslation,
    const bool                       hasLowerLimitActualRotation,
    const double                          theLowerLimitActualRotation,
    const bool                       hasUpperLimitActualRotation,
    const double                          theUpperLimitActualRotation);

  //! Returns field LowerLimitActualTranslation
  Standard_EXPORT double LowerLimitActualTranslation() const;
  //! Sets field LowerLimitActualTranslation
  Standard_EXPORT void SetLowerLimitActualTranslation(
    const double theLowerLimitActualTranslation);
  //! Returns True if optional field LowerLimitActualTranslation is defined
  Standard_EXPORT bool HasLowerLimitActualTranslation() const;

  //! Returns field UpperLimitActualTranslation
  Standard_EXPORT double UpperLimitActualTranslation() const;
  //! Sets field UpperLimitActualTranslation
  Standard_EXPORT void SetUpperLimitActualTranslation(
    const double theUpperLimitActualTranslation);
  //! Returns True if optional field UpperLimitActualTranslation is defined
  Standard_EXPORT bool HasUpperLimitActualTranslation() const;

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

  DEFINE_STANDARD_RTTIEXT(StepKinematics_CylindricalPairWithRange, StepKinematics_CylindricalPair)

private:
  double myLowerLimitActualTranslation;    //!< optional
  double myUpperLimitActualTranslation;    //!< optional
  double myLowerLimitActualRotation;       //!< optional
  double myUpperLimitActualRotation;       //!< optional
                                                  // clang-format off
  bool defLowerLimitActualTranslation; //!< flag "is LowerLimitActualTranslation defined"
  bool defUpperLimitActualTranslation; //!< flag "is UpperLimitActualTranslation defined"
                                                  // clang-format on
  bool defLowerLimitActualRotation;   //!< flag "is LowerLimitActualRotation defined"
  bool defUpperLimitActualRotation;   //!< flag "is UpperLimitActualRotation defined"
};
#endif // _StepKinematics_CylindricalPairWithRange_HeaderFile_
