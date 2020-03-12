// Created on: 2020-05-26
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _StepKinematics_ActuatedKinPairAndOrderKinPair_HeaderFile_
#define _StepKinematics_ActuatedKinPairAndOrderKinPair_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepKinematics_KinematicPair.hxx>
class StepRepr_RepresentationItem;
class StepKinematics_ActuatedKinematicPair;
class StepKinematics_ActuatedKinPairAndOrderKinPair;

DEFINE_STANDARD_HANDLE(StepKinematics_ActuatedKinPairAndOrderKinPair, StepKinematics_KinematicPair)

//! Representation of STEP entity ActuatedKinPairAndOrderKinPair
class StepKinematics_ActuatedKinPairAndOrderKinPair : public StepKinematics_KinematicPair
{
public :

  //! default constructor
  Standard_EXPORT StepKinematics_ActuatedKinPairAndOrderKinPair();


  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Name,
                            const Standard_Boolean hasItemDefinedTransformation_Description,
                            const Handle(TCollection_HAsciiString)& theItemDefinedTransformation_Description,
                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem1,
                            const Handle(StepRepr_RepresentationItem)& theItemDefinedTransformation_TransformItem2,
                            const Handle(StepKinematics_KinematicJoint)& theJoint,
                            const Handle(StepKinematics_ActuatedKinematicPair)& theActuatedKinematicPair,
                            const Handle(StepKinematics_KinematicPair)& theOrderKinematicPair);

  inline void SetActuatedKinematicPair(const Handle(StepKinematics_ActuatedKinematicPair)& aKP) {
    myActuatedKinematicPair = aKP;
  }

  inline Handle(StepKinematics_ActuatedKinematicPair) GetActuatedKinematicPair() const {
    return myActuatedKinematicPair;
  }
  inline void SetOrderKinematicPair(const Handle(StepKinematics_KinematicPair)& aKP) {
    myOrderKinematicPair = aKP;
  }

  inline Handle(StepKinematics_KinematicPair) GetOrderKinematicPair() const {
    return myOrderKinematicPair;
  }


DEFINE_STANDARD_RTTIEXT(StepKinematics_ActuatedKinPairAndOrderKinPair, StepKinematics_KinematicPair)

private:
  Handle(StepKinematics_ActuatedKinematicPair) myActuatedKinematicPair;
  Handle(StepKinematics_KinematicPair) myOrderKinematicPair;
};
#endif // StepKinematics_ActuatedKinPairAndOrderKinPair
