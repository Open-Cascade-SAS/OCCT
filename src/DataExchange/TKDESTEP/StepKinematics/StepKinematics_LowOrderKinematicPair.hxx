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

#ifndef _StepKinematics_LowOrderKinematicPair_HeaderFile_
#define _StepKinematics_LowOrderKinematicPair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_KinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>

//! Representation of STEP entity LowOrderKinematicPair
class StepKinematics_LowOrderKinematicPair : public StepKinematics_KinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_LowOrderKinematicPair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                       hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const bool                       theTX,
    const bool                       theTY,
    const bool                       theTZ,
    const bool                       theRX,
    const bool                       theRY,
    const bool                       theRZ);

  //! Returns field TX
  Standard_EXPORT bool TX() const;
  //! Sets field TX
  Standard_EXPORT void SetTX(const bool theTX);

  //! Returns field TY
  Standard_EXPORT bool TY() const;
  //! Sets field TY
  Standard_EXPORT void SetTY(const bool theTY);

  //! Returns field TZ
  Standard_EXPORT bool TZ() const;
  //! Sets field TZ
  Standard_EXPORT void SetTZ(const bool theTZ);

  //! Returns field RX
  Standard_EXPORT bool RX() const;
  //! Sets field RX
  Standard_EXPORT void SetRX(const bool theRX);

  //! Returns field RY
  Standard_EXPORT bool RY() const;
  //! Sets field RY
  Standard_EXPORT void SetRY(const bool theRY);

  //! Returns field RZ
  Standard_EXPORT bool RZ() const;
  //! Sets field RZ
  Standard_EXPORT void SetRZ(const bool theRZ);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPair, StepKinematics_KinematicPair)

private:
  bool myTX;
  bool myTY;
  bool myTZ;
  bool myRX;
  bool myRY;
  bool myRZ;
};
#endif // _StepKinematics_LowOrderKinematicPair_HeaderFile_
