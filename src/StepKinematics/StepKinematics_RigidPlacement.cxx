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

#include <StepKinematics_RigidPlacement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_SuParameters.hxx>

//=================================================================================================

StepKinematics_RigidPlacement::StepKinematics_RigidPlacement() {}

//=================================================================================================

Standard_Integer StepKinematics_RigidPlacement::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_Axis2Placement3d)))
    return 1;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_SuParameters)))
    return 2;
  return 0;
}

//=================================================================================================

Handle(StepGeom_Axis2Placement3d) StepKinematics_RigidPlacement::Axis2Placement3d() const
{
  return Handle(StepGeom_Axis2Placement3d)::DownCast(Value());
}

//=================================================================================================

Handle(StepGeom_SuParameters) StepKinematics_RigidPlacement::SuParameters() const
{
  return Handle(StepGeom_SuParameters)::DownCast(Value());
}
