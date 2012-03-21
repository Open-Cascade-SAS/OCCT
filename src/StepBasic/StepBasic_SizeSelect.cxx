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

#include <StepBasic_SizeSelect.ixx>
#include <StepBasic_SizeMember.hxx>
#include <Interface_Macros.hxx>


//=======================================================================
//function : StepBasic_SizeSelect
//purpose  : 
//=======================================================================

StepBasic_SizeSelect::StepBasic_SizeSelect()
{
}


//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_SizeSelect::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_SizeMember))) return 1;
  return 0;
}


//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepBasic_SizeSelect::NewMember () const
{
  return new StepBasic_SizeMember;
}


//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_SizeSelect::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
  if (ent.IsNull()) return 0;
//skl  Interface_ParamType type = ent->ParamType();
  // Void : on admet "non defini" (en principe, on ne devrait pas)
//skl  if (type != Interface_ParamVoid && type != Interface_ParamReal) return 0;
  if (ent->Matches("POSITIVE_LENGTH_MEASURE")) return 1;
  return 0;
}


//=======================================================================
//function : SetRealValue
//purpose  : 
//=======================================================================

void StepBasic_SizeSelect::SetRealValue (const Standard_Real aRealValue)
{
  SetReal(aRealValue,"POSITIVE_LENGTH_MEASURE");
}


//=======================================================================
//function : RealValue
//purpose  : 
//=======================================================================

Standard_Real StepBasic_SizeSelect::RealValue () const
{
  return Real();
}

