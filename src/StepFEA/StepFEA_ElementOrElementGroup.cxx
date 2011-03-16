// File:	StepFEA_ElementOrElementGroup.cxx
// Created:	Tue Feb  4 10:39:08 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ElementOrElementGroup.ixx>

//=======================================================================
//function : StepFEA_ElementOrElementGroup
//purpose  : 
//=======================================================================

StepFEA_ElementOrElementGroup::StepFEA_ElementOrElementGroup ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_ElementOrElementGroup::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_ElementRepresentation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_ElementGroup))) return 2;
  return 0;
}

//=======================================================================
//function : ElementRepresentation
//purpose  : 
//=======================================================================

Handle(StepFEA_ElementRepresentation) StepFEA_ElementOrElementGroup::ElementRepresentation () const
{
  return Handle(StepFEA_ElementRepresentation)::DownCast(Value());
}

//=======================================================================
//function : ElementGroup
//purpose  : 
//=======================================================================

Handle(StepFEA_ElementGroup) StepFEA_ElementOrElementGroup::ElementGroup () const
{
  return Handle(StepFEA_ElementGroup)::DownCast(Value());
}
