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

#ifndef _StepKinematics_SphericalPairWithRange_HeaderFile_
#define _StepKinematics_SphericalPairWithRange_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_SphericalPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity SphericalPairWithRange
class StepKinematics_SphericalPairWithRange : public StepKinematics_SphericalPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_SphericalPairWithRange();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                                        hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const bool                                        theLowOrderKinematicPair_TX,
    const bool                                        theLowOrderKinematicPair_TY,
    const bool                                        theLowOrderKinematicPair_TZ,
    const bool                                        theLowOrderKinematicPair_RX,
    const bool                                        theLowOrderKinematicPair_RY,
    const bool                                        theLowOrderKinematicPair_RZ,
    const bool                                        hasLowerLimitYaw,
    const double                                      theLowerLimitYaw,
    const bool                                        hasUpperLimitYaw,
    const double                                      theUpperLimitYaw,
    const bool                                        hasLowerLimitPitch,
    const double                                      theLowerLimitPitch,
    const bool                                        hasUpperLimitPitch,
    const double                                      theUpperLimitPitch,
    const bool                                        hasLowerLimitRoll,
    const double                                      theLowerLimitRoll,
    const bool                                        hasUpperLimitRoll,
    const double                                      theUpperLimitRoll);

  //! Returns field LowerLimitYaw
  Standard_EXPORT double LowerLimitYaw() const;
  //! Sets field LowerLimitYaw
  Standard_EXPORT void SetLowerLimitYaw(const double theLowerLimitYaw);
  //! Returns True if optional field LowerLimitYaw is defined
  Standard_EXPORT bool HasLowerLimitYaw() const;

  //! Returns field UpperLimitYaw
  Standard_EXPORT double UpperLimitYaw() const;
  //! Sets field UpperLimitYaw
  Standard_EXPORT void SetUpperLimitYaw(const double theUpperLimitYaw);
  //! Returns True if optional field UpperLimitYaw is defined
  Standard_EXPORT bool HasUpperLimitYaw() const;

  //! Returns field LowerLimitPitch
  Standard_EXPORT double LowerLimitPitch() const;
  //! Sets field LowerLimitPitch
  Standard_EXPORT void SetLowerLimitPitch(const double theLowerLimitPitch);
  //! Returns True if optional field LowerLimitPitch is defined
  Standard_EXPORT bool HasLowerLimitPitch() const;

  //! Returns field UpperLimitPitch
  Standard_EXPORT double UpperLimitPitch() const;
  //! Sets field UpperLimitPitch
  Standard_EXPORT void SetUpperLimitPitch(const double theUpperLimitPitch);
  //! Returns True if optional field UpperLimitPitch is defined
  Standard_EXPORT bool HasUpperLimitPitch() const;

  //! Returns field LowerLimitRoll
  Standard_EXPORT double LowerLimitRoll() const;
  //! Sets field LowerLimitRoll
  Standard_EXPORT void SetLowerLimitRoll(const double theLowerLimitRoll);
  //! Returns True if optional field LowerLimitRoll is defined
  Standard_EXPORT bool HasLowerLimitRoll() const;

  //! Returns field UpperLimitRoll
  Standard_EXPORT double UpperLimitRoll() const;
  //! Sets field UpperLimitRoll
  Standard_EXPORT void SetUpperLimitRoll(const double theUpperLimitRoll);
  //! Returns True if optional field UpperLimitRoll is defined
  Standard_EXPORT bool HasUpperLimitRoll() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_SphericalPairWithRange, StepKinematics_SphericalPair)

private:
  double myLowerLimitYaw;    //!< optional
  double myUpperLimitYaw;    //!< optional
  double myLowerLimitPitch;  //!< optional
  double myUpperLimitPitch;  //!< optional
  double myLowerLimitRoll;   //!< optional
  double myUpperLimitRoll;   //!< optional
  bool   defLowerLimitYaw;   //!< flag "is LowerLimitYaw defined"
  bool   defUpperLimitYaw;   //!< flag "is UpperLimitYaw defined"
  bool   defLowerLimitPitch; //!< flag "is LowerLimitPitch defined"
  bool   defUpperLimitPitch; //!< flag "is UpperLimitPitch defined"
  bool   defLowerLimitRoll;  //!< flag "is LowerLimitRoll defined"
  bool   defUpperLimitRoll;  //!< flag "is UpperLimitRoll defined"
};
#endif // _StepKinematics_SphericalPairWithRange_HeaderFile_
