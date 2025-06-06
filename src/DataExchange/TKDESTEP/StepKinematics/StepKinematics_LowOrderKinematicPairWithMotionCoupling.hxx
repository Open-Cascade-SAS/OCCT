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

#ifndef _StepKinematics_LowOrderKinematicPairWithMotionCoupling_HeaderFile_
#define _StepKinematics_LowOrderKinematicPairWithMotionCoupling_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_KinematicPair.hxx>

#include <TCollection_HAsciiString.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_LowOrderKinematicPairWithMotionCoupling,
                       StepKinematics_KinematicPair)

//! Representation of STEP entity LowOrderKinematicPairWithMotionCoupling
class StepKinematics_LowOrderKinematicPairWithMotionCoupling : public StepKinematics_KinematicPair
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_LowOrderKinematicPairWithMotionCoupling();

  DEFINE_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairWithMotionCoupling,
                          StepKinematics_KinematicPair)
};
#endif // _StepKinematics_LowOrderKinematicPairWithMotionCoupling_HeaderFile_
