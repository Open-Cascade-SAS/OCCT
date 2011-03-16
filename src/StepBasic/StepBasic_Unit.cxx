// File:	StepBasic_Unit.cxx
// Created:	Wed Sep  8 14:20:46 1999
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <StepBasic_Unit.ixx>
#include <Interface_Macros.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_DerivedUnit.hxx>

//=======================================================================
//function : StepBasic_Unit
//purpose  : 
//=======================================================================

StepBasic_Unit::StepBasic_Unit () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_Unit::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_NamedUnit))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DerivedUnit))) return 2;
  return 0;
}

//=======================================================================
//function : NamedUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_NamedUnit) StepBasic_Unit::NamedUnit () const
{
  return GetCasted(StepBasic_NamedUnit,Value());
}

//=======================================================================
//function : DerivedUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_DerivedUnit) StepBasic_Unit::DerivedUnit () const
{
  return GetCasted(StepBasic_DerivedUnit,Value());
}

