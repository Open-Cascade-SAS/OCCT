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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_SizeMember.hxx>
#include <StepBasic_SizeSelect.hxx>
#include <StepData_SelectMember.hxx>

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

