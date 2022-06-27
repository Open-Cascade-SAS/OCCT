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

DEFINE_STANDARD_HANDLE(StepKinematics_LowOrderKinematicPair, StepKinematics_KinematicPair)

//! Representation of STEP entity LowOrderKinematicPair
class StepKinematics_LowOrderKinematicPair : public StepKinematics_KinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_LowOrderKinematicPair();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                           const Standard_Boolean hasItemDefinedTransformation_Description,
                           const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                           const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                           const Handle(StepKinematics_KinematicJoint)& theKinematicPair_Joint,
                           const Standard_Boolean theTX,
                           const Standard_Boolean theTY,
                           const Standard_Boolean theTZ,
                           const Standard_Boolean theRX,
                           const Standard_Boolean theRY,
                           const Standard_Boolean theRZ);

  //! Returns field TX
  Standard_EXPORT Standard_Boolean TX() const;
  //! Sets field TX
  Standard_EXPORT void SetTX (const Standard_Boolean theTX);

  //! Returns field TY
  Standard_EXPORT Standard_Boolean TY() const;
  //! Sets field TY
  Standard_EXPORT void SetTY (const Standard_Boolean theTY);

  //! Returns field TZ
  Standard_EXPORT Standard_Boolean TZ() const;
  //! Sets field TZ
  Standard_EXPORT void SetTZ (const Standard_Boolean theTZ);

  //! Returns field RX
  Standard_EXPORT Standard_Boolean RX() const;
  //! Sets field RX
  Standard_EXPORT void SetRX (const Standard_Boolean theRX);

  //! Returns field RY
  Standard_EXPORT Standard_Boolean RY() const;
  //! Sets field RY
  Standard_EXPORT void SetRY (const Standard_Boolean theRY);

  //! Returns field RZ
  Standard_EXPORT Standard_Boolean RZ() const;
  //! Sets field RZ
  Standard_EXPORT void SetRZ (const Standard_Boolean theRZ);

DEFINE_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPair, StepKinematics_KinematicPair)

private:
  Standard_Boolean myTX;
  Standard_Boolean myTY;
  Standard_Boolean myTZ;
  Standard_Boolean myRX;
  Standard_Boolean myRY;
  Standard_Boolean myRZ;

};
#endif // _StepKinematics_LowOrderKinematicPair_HeaderFile_
