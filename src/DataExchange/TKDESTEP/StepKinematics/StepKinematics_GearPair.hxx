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

#ifndef _StepKinematics_GearPair_HeaderFile_
#define _StepKinematics_GearPair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_LowOrderKinematicPairWithMotionCoupling.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity GearPair
class StepKinematics_GearPair : public StepKinematics_LowOrderKinematicPairWithMotionCoupling
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_GearPair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                       hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const double                          theRadiusFirstLink,
    const double                          theRadiusSecondLink,
    const double                          theBevel,
    const double                          theHelicalAngle,
    const double                          theGearRatio);

  //! Returns field RadiusFirstLink
  Standard_EXPORT double RadiusFirstLink() const;
  //! Sets field RadiusFirstLink
  Standard_EXPORT void SetRadiusFirstLink(const double theRadiusFirstLink);

  //! Returns field RadiusSecondLink
  Standard_EXPORT double RadiusSecondLink() const;
  //! Sets field RadiusSecondLink
  Standard_EXPORT void SetRadiusSecondLink(const double theRadiusSecondLink);

  //! Returns field Bevel
  Standard_EXPORT double Bevel() const;
  //! Sets field Bevel
  Standard_EXPORT void SetBevel(const double theBevel);

  //! Returns field HelicalAngle
  Standard_EXPORT double HelicalAngle() const;
  //! Sets field HelicalAngle
  Standard_EXPORT void SetHelicalAngle(const double theHelicalAngle);

  //! Returns field GearRatio
  Standard_EXPORT double GearRatio() const;
  //! Sets field GearRatio
  Standard_EXPORT void SetGearRatio(const double theGearRatio);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_GearPair,
                          StepKinematics_LowOrderKinematicPairWithMotionCoupling)

private:
  double myRadiusFirstLink;
  double myRadiusSecondLink;
  double myBevel;
  double myHelicalAngle;
  double myGearRatio;
};
#endif // _StepKinematics_GearPair_HeaderFile_
