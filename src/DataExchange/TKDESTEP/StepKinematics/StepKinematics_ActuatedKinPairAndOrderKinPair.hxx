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
#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepKinematics_KinematicPair.hxx>
class StepRepr_RepresentationItem;
class StepKinematics_ActuatedKinematicPair;
//! Representation of STEP entity ActuatedKinPairAndOrderKinPair
class StepKinematics_ActuatedKinPairAndOrderKinPair : public StepKinematics_KinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_ActuatedKinPairAndOrderKinPair();

  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&             theRepresentationItem_Name,
    const occ::handle<TCollection_HAsciiString>&             theItemDefinedTransformation_Name,
    const bool                              hasItemDefinedTransformation_Description,
    const occ::handle<TCollection_HAsciiString>&             theItemDefinedTransformation_Description,
    const occ::handle<StepRepr_RepresentationItem>&          theItemDefinedTransformation_TransformItem1,
    const occ::handle<StepRepr_RepresentationItem>&          theItemDefinedTransformation_TransformItem2,
    const occ::handle<StepKinematics_KinematicJoint>&        theJoint,
    const occ::handle<StepKinematics_ActuatedKinematicPair>& theActuatedKinematicPair,
    const occ::handle<StepKinematics_KinematicPair>&         theOrderKinematicPair);

  inline void SetActuatedKinematicPair(const occ::handle<StepKinematics_ActuatedKinematicPair>& aKP)
  {
    myActuatedKinematicPair = aKP;
  }

  inline occ::handle<StepKinematics_ActuatedKinematicPair> GetActuatedKinematicPair() const
  {
    return myActuatedKinematicPair;
  }

  inline void SetOrderKinematicPair(const occ::handle<StepKinematics_KinematicPair>& aKP)
  {
    myOrderKinematicPair = aKP;
  }

  inline occ::handle<StepKinematics_KinematicPair> GetOrderKinematicPair() const
  {
    return myOrderKinematicPair;
  }

  DEFINE_STANDARD_RTTIEXT(StepKinematics_ActuatedKinPairAndOrderKinPair,
                          StepKinematics_KinematicPair)

private:
  occ::handle<StepKinematics_ActuatedKinematicPair> myActuatedKinematicPair;
  occ::handle<StepKinematics_KinematicPair>         myOrderKinematicPair;
};
#endif // StepKinematics_ActuatedKinPairAndOrderKinPair
