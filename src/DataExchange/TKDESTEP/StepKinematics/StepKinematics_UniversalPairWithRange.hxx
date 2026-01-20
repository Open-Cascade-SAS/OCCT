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

#ifndef _StepKinematics_UniversalPairWithRange_HeaderFile_
#define _StepKinematics_UniversalPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_UniversalPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity UniversalPairWithRange
class StepKinematics_UniversalPairWithRange : public StepKinematics_UniversalPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_UniversalPairWithRange();

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
    const bool                       hasUniversalPair_InputSkewAngle,
    const double                          theUniversalPair_InputSkewAngle,
    const bool                       hasLowerLimitFirstRotation,
    const double                          theLowerLimitFirstRotation,
    const bool                       hasUpperLimitFirstRotation,
    const double                          theUpperLimitFirstRotation,
    const bool                       hasLowerLimitSecondRotation,
    const double                          theLowerLimitSecondRotation,
    const bool                       hasUpperLimitSecondRotation,
    const double                          theUpperLimitSecondRotation);

  //! Returns field LowerLimitFirstRotation
  Standard_EXPORT double LowerLimitFirstRotation() const;
  //! Sets field LowerLimitFirstRotation
  Standard_EXPORT void SetLowerLimitFirstRotation(const double theLowerLimitFirstRotation);
  //! Returns True if optional field LowerLimitFirstRotation is defined
  Standard_EXPORT bool HasLowerLimitFirstRotation() const;

  //! Returns field UpperLimitFirstRotation
  Standard_EXPORT double UpperLimitFirstRotation() const;
  //! Sets field UpperLimitFirstRotation
  Standard_EXPORT void SetUpperLimitFirstRotation(const double theUpperLimitFirstRotation);
  //! Returns True if optional field UpperLimitFirstRotation is defined
  Standard_EXPORT bool HasUpperLimitFirstRotation() const;

  //! Returns field LowerLimitSecondRotation
  Standard_EXPORT double LowerLimitSecondRotation() const;
  //! Sets field LowerLimitSecondRotation
  Standard_EXPORT void SetLowerLimitSecondRotation(const double theLowerLimitSecondRotation);
  //! Returns True if optional field LowerLimitSecondRotation is defined
  Standard_EXPORT bool HasLowerLimitSecondRotation() const;

  //! Returns field UpperLimitSecondRotation
  Standard_EXPORT double UpperLimitSecondRotation() const;
  //! Sets field UpperLimitSecondRotation
  Standard_EXPORT void SetUpperLimitSecondRotation(const double theUpperLimitSecondRotation);
  //! Returns True if optional field UpperLimitSecondRotation is defined
  Standard_EXPORT bool HasUpperLimitSecondRotation() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_UniversalPairWithRange, StepKinematics_UniversalPair)

private:
  double    myLowerLimitFirstRotation;   //!< optional
  double    myUpperLimitFirstRotation;   //!< optional
  double    myLowerLimitSecondRotation;  //!< optional
  double    myUpperLimitSecondRotation;  //!< optional
  bool defLowerLimitFirstRotation;  //!< flag "is LowerLimitFirstRotation defined"
  bool defUpperLimitFirstRotation;  //!< flag "is UpperLimitFirstRotation defined"
  bool defLowerLimitSecondRotation; //!< flag "is LowerLimitSecondRotation defined"
  bool defUpperLimitSecondRotation; //!< flag "is UpperLimitSecondRotation defined"
};
#endif // _StepKinematics_UniversalPairWithRange_HeaderFile_
