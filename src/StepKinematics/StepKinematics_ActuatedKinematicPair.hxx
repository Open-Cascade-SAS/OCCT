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

DEFINE_STANDARD_HANDLE(StepKinematics_ActuatedKinematicPair, StepKinematics_KinematicPair)

//! Representation of STEP entity ActuatedKinematicPair
class StepKinematics_ActuatedKinematicPair : public StepKinematics_KinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_ActuatedKinematicPair();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Boolean hasTX,
                           const StepKinematics_ActuatedDirection theTX,
                           const Standard_Boolean hasTY,
                           const StepKinematics_ActuatedDirection theTY,
                           const Standard_Boolean hasTZ,
                           const StepKinematics_ActuatedDirection theTZ,
                           const Standard_Boolean hasRX,
                           const StepKinematics_ActuatedDirection theRX,
                           const Standard_Boolean hasRY,
                           const StepKinematics_ActuatedDirection theRY,
                           const Standard_Boolean hasRZ,
                           const StepKinematics_ActuatedDirection theRZ);

  //! Returns field TX
  Standard_EXPORT StepKinematics_ActuatedDirection TX() const;
  //! Sets field TX
  Standard_EXPORT void SetTX (const StepKinematics_ActuatedDirection theTX);
  //! Returns True if optional field TX is defined
  Standard_EXPORT Standard_Boolean HasTX() const;

  //! Returns field TY
  Standard_EXPORT StepKinematics_ActuatedDirection TY() const;
  //! Sets field TY
  Standard_EXPORT void SetTY (const StepKinematics_ActuatedDirection theTY);
  //! Returns True if optional field TY is defined
  Standard_EXPORT Standard_Boolean HasTY() const;

  //! Returns field TZ
  Standard_EXPORT StepKinematics_ActuatedDirection TZ() const;
  //! Sets field TZ
  Standard_EXPORT void SetTZ (const StepKinematics_ActuatedDirection theTZ);
  //! Returns True if optional field TZ is defined
  Standard_EXPORT Standard_Boolean HasTZ() const;

  //! Returns field RX
  Standard_EXPORT StepKinematics_ActuatedDirection RX() const;
  //! Sets field RX
  Standard_EXPORT void SetRX (const StepKinematics_ActuatedDirection theRX);
  //! Returns True if optional field RX is defined
  Standard_EXPORT Standard_Boolean HasRX() const;

  //! Returns field RY
  Standard_EXPORT StepKinematics_ActuatedDirection RY() const;
  //! Sets field RY
  Standard_EXPORT void SetRY (const StepKinematics_ActuatedDirection theRY);
  //! Returns True if optional field RY is defined
  Standard_EXPORT Standard_Boolean HasRY() const;

  //! Returns field RZ
  Standard_EXPORT StepKinematics_ActuatedDirection RZ() const;
  //! Sets field RZ
  Standard_EXPORT void SetRZ (const StepKinematics_ActuatedDirection theRZ);
  //! Returns True if optional field RZ is defined
  Standard_EXPORT Standard_Boolean HasRZ() const;

DEFINE_STANDARD_RTTIEXT(StepKinematics_ActuatedKinematicPair, StepKinematics_KinematicPair)

private:
  StepKinematics_ActuatedDirection myTX; //!< optional
  StepKinematics_ActuatedDirection myTY; //!< optional
  StepKinematics_ActuatedDirection myTZ; //!< optional
  StepKinematics_ActuatedDirection myRX; //!< optional
  StepKinematics_ActuatedDirection myRY; //!< optional
  StepKinematics_ActuatedDirection myRZ; //!< optional
  Standard_Boolean defTX; //!< flag "is TX defined"
  Standard_Boolean defTY; //!< flag "is TY defined"
  Standard_Boolean defTZ; //!< flag "is TZ defined"
  Standard_Boolean defRX; //!< flag "is RX defined"
  Standard_Boolean defRY; //!< flag "is RY defined"
  Standard_Boolean defRZ; //!< flag "is RZ defined"

};
#endif // _StepKinematics_ActuatedKinematicPair_HeaderFile_
