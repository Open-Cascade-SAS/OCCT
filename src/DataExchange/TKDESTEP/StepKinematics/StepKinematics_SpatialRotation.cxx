// Created on : Sat May 02 12:41:14 2020
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

#include <StepKinematics_SpatialRotation.hxx>
#include <StepKinematics_RotationAboutDirection.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

StepKinematics_SpatialRotation::StepKinematics_SpatialRotation() {}

//=================================================================================================

int StepKinematics_SpatialRotation::CaseNum(
  const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepKinematics_RotationAboutDirection)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(NCollection_HArray1<double>)))
    return 2;
  return 0;
}

//=================================================================================================

occ::handle<StepKinematics_RotationAboutDirection> StepKinematics_SpatialRotation::
  RotationAboutDirection() const
{
  return occ::down_cast<StepKinematics_RotationAboutDirection>(Value());
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> StepKinematics_SpatialRotation::YprRotation() const
{
  return occ::down_cast<NCollection_HArray1<double>>(Value());
}
