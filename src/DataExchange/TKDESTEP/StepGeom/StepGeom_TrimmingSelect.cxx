// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepData_SelectMember.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_TrimmingMember.hxx>
#include <StepGeom_TrimmingSelect.hxx>

StepGeom_TrimmingSelect::StepGeom_TrimmingSelect() = default;

int StepGeom_TrimmingSelect::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint)))
    return 1;
  return 0;
}

occ::handle<StepData_SelectMember> StepGeom_TrimmingSelect::NewMember() const
{
  return new StepGeom_TrimmingMember;
}

int StepGeom_TrimmingSelect::CaseMem(const occ::handle<StepData_SelectMember>& ent) const
{
  if (ent.IsNull())
    return 0;
  Interface_ParamType type = ent->ParamType();
  //  Void : on admet "non defini" (en principe, on ne devrait pas)
  if (type != Interface_ParamVoid && type != Interface_ParamReal)
    return 0;
  if (ent->Matches("PARAMETER_VALUE"))
    return 1;
  return 0;
}

occ::handle<StepGeom_CartesianPoint> StepGeom_TrimmingSelect::CartesianPoint() const
{
  return GetCasted(StepGeom_CartesianPoint, Value());
}

void StepGeom_TrimmingSelect::SetParameterValue(const double aParameterValue)
{
  SetReal(aParameterValue, "PARAMETER_VALUE");
}

double StepGeom_TrimmingSelect::ParameterValue() const
{
  return Real();
}
