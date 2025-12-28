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

#ifndef _StepKinematics_ActuatedKinematicPair_HeaderFile_
#define _StepKinematics_ActuatedKinematicPair_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_KinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepKinematics_ActuatedDirection.hxx>

//! Representation of STEP entity ActuatedKinematicPair
class StepKinematics_ActuatedKinematicPair : public StepKinematics_KinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_ActuatedKinematicPair();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&      theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Name,
    const bool                                        hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&      theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&   theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>& theKinematicPair_Joint,
    const bool                                        hasTX,
    const StepKinematics_ActuatedDirection            theTX,
    const bool                                        hasTY,
    const StepKinematics_ActuatedDirection            theTY,
    const bool                                        hasTZ,
    const StepKinematics_ActuatedDirection            theTZ,
    const bool                                        hasRX,
    const StepKinematics_ActuatedDirection            theRX,
    const bool                                        hasRY,
    const StepKinematics_ActuatedDirection            theRY,
    const bool                                        hasRZ,
    const StepKinematics_ActuatedDirection            theRZ);

  //! Returns field TX
  Standard_EXPORT StepKinematics_ActuatedDirection TX() const;
  //! Sets field TX
  Standard_EXPORT void SetTX(const StepKinematics_ActuatedDirection theTX);
  //! Returns True if optional field TX is defined
  Standard_EXPORT bool HasTX() const;

  //! Returns field TY
  Standard_EXPORT StepKinematics_ActuatedDirection TY() const;
  //! Sets field TY
  Standard_EXPORT void SetTY(const StepKinematics_ActuatedDirection theTY);
  //! Returns True if optional field TY is defined
  Standard_EXPORT bool HasTY() const;

  //! Returns field TZ
  Standard_EXPORT StepKinematics_ActuatedDirection TZ() const;
  //! Sets field TZ
  Standard_EXPORT void SetTZ(const StepKinematics_ActuatedDirection theTZ);
  //! Returns True if optional field TZ is defined
  Standard_EXPORT bool HasTZ() const;

  //! Returns field RX
  Standard_EXPORT StepKinematics_ActuatedDirection RX() const;
  //! Sets field RX
  Standard_EXPORT void SetRX(const StepKinematics_ActuatedDirection theRX);
  //! Returns True if optional field RX is defined
  Standard_EXPORT bool HasRX() const;

  //! Returns field RY
  Standard_EXPORT StepKinematics_ActuatedDirection RY() const;
  //! Sets field RY
  Standard_EXPORT void SetRY(const StepKinematics_ActuatedDirection theRY);
  //! Returns True if optional field RY is defined
  Standard_EXPORT bool HasRY() const;

  //! Returns field RZ
  Standard_EXPORT StepKinematics_ActuatedDirection RZ() const;
  //! Sets field RZ
  Standard_EXPORT void SetRZ(const StepKinematics_ActuatedDirection theRZ);
  //! Returns True if optional field RZ is defined
  Standard_EXPORT bool HasRZ() const;

  DEFINE_STANDARD_RTTIEXT(StepKinematics_ActuatedKinematicPair, StepKinematics_KinematicPair)

private:
  StepKinematics_ActuatedDirection myTX;  //!< optional
  StepKinematics_ActuatedDirection myTY;  //!< optional
  StepKinematics_ActuatedDirection myTZ;  //!< optional
  StepKinematics_ActuatedDirection myRX;  //!< optional
  StepKinematics_ActuatedDirection myRY;  //!< optional
  StepKinematics_ActuatedDirection myRZ;  //!< optional
  bool                             defTX; //!< flag "is TX defined"
  bool                             defTY; //!< flag "is TY defined"
  bool                             defTZ; //!< flag "is TZ defined"
  bool                             defRX; //!< flag "is RX defined"
  bool                             defRY; //!< flag "is RY defined"
  bool                             defRZ; //!< flag "is RZ defined"
};
#endif // _StepKinematics_ActuatedKinematicPair_HeaderFile_
