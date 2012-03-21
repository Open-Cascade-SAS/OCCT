// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepBasic_ProductOrFormationOrDefinition.ixx>

//=======================================================================
//function : StepBasic_ProductOrFormationOrDefinition
//purpose  : 
//=======================================================================

StepBasic_ProductOrFormationOrDefinition::StepBasic_ProductOrFormationOrDefinition ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_ProductOrFormationOrDefinition::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_Product))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinitionFormation))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) return 3;
  return 0;
}

//=======================================================================
//function : Product
//purpose  : 
//=======================================================================

Handle(StepBasic_Product) StepBasic_ProductOrFormationOrDefinition::Product () const
{
  return Handle(StepBasic_Product)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductOrFormationOrDefinition::ProductDefinitionFormation () const
{
  return Handle(StepBasic_ProductDefinitionFormation)::DownCast(Value());
}

//=======================================================================
//function : ProductDefinition
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinition) StepBasic_ProductOrFormationOrDefinition::ProductDefinition () const
{
  return Handle(StepBasic_ProductDefinition)::DownCast(Value());
}
