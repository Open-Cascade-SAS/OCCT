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

#ifndef _StepKinematics_UniversalPair_HeaderFile_
#define _StepKinematics_UniversalPair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_LowOrderKinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity UniversalPair
class StepKinematics_UniversalPair : public StepKinematics_LowOrderKinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_UniversalPair();

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
    const bool                       hasInputSkewAngle,
    const double                          theInputSkewAngle);

  //! Returns field InputSkewAngle
  Standard_EXPORT double InputSkewAngle() const;
  //! Sets field InputSkewAngle
  Standard_EXPORT void SetInputSkewAngle(const double theInputSkewAngle);
  //! Returns True if optional field InputSkewAngle is defined
  Standard_EXPORT bool HasInputSkewAngle() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_UniversalPair, StepKinematics_LowOrderKinematicPair)

private:
  double    myInputSkewAngle;  //!< optional
  bool defInputSkewAngle; //!< flag "is InputSkewAngle defined"
};
#endif // _StepKinematics_UniversalPair_HeaderFile_
