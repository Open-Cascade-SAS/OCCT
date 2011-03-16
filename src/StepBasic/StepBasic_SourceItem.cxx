// File:	StepBasic_SourceItem.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_SourceItem.ixx>
#include <StepData_SelectNamed.hxx>

//=======================================================================
//function : StepBasic_SourceItem
//purpose  : 
//=======================================================================

StepBasic_SourceItem::StepBasic_SourceItem ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_SourceItem::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepData_SelectNamed))) return 1;
  return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepBasic_SourceItem::NewMember() const
{
  Handle(StepData_SelectNamed) member = new StepData_SelectNamed;
  return member;
}

//=======================================================================
//function : Identifier
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_SourceItem::Identifier () const
{
  return Handle(TCollection_HAsciiString)::DownCast(Value());
}
