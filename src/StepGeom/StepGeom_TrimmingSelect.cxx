// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <StepGeom_TrimmingSelect.ixx>
#include <StepGeom_TrimmingMember.hxx>
#include <Interface_Macros.hxx>

StepGeom_TrimmingSelect::StepGeom_TrimmingSelect () {  }

Standard_Integer StepGeom_TrimmingSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepGeom_CartesianPoint))) return 1;
  return 0;
}

Handle(StepData_SelectMember)  StepGeom_TrimmingSelect::NewMember () const
{  return  new StepGeom_TrimmingMember;  }

Standard_Integer  StepGeom_TrimmingSelect::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
  if (ent.IsNull()) return 0;
  Interface_ParamType type = ent->ParamType();
//  Void : on admet "non defini" (en principe, on ne devrait pas)
  if (type != Interface_ParamVoid && type != Interface_ParamReal) return 0;
  if (ent->Matches("PARAMETER_VALUE")) return 1;
  return 0;
}

Handle(StepGeom_CartesianPoint) StepGeom_TrimmingSelect::CartesianPoint () const
{
  return GetCasted(StepGeom_CartesianPoint,Value());
}

void StepGeom_TrimmingSelect::SetParameterValue(const Standard_Real aParameterValue)
{  SetReal (aParameterValue,"PARAMETER_VALUE");  }

Standard_Real StepGeom_TrimmingSelect::ParameterValue () const
{  return Real();  }
