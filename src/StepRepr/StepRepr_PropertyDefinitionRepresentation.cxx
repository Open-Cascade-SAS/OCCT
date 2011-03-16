// File:	StepRepr_PropertyDefinitionRepresentation.cxx
// Created:	Thu Dec 12 16:02:14 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepRepr_PropertyDefinitionRepresentation.ixx>

//=======================================================================
//function : StepRepr_PropertyDefinitionRepresentation
//purpose  : 
//=======================================================================

StepRepr_PropertyDefinitionRepresentation::StepRepr_PropertyDefinitionRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::Init (const StepRepr_RepresentedDefinition &aDefinition,
                                                      const Handle(StepRepr_Representation) &aUsedRepresentation)
{

  theDefinition = aDefinition;

  theUsedRepresentation = aUsedRepresentation;
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

StepRepr_RepresentedDefinition StepRepr_PropertyDefinitionRepresentation::Definition () const
{
  return theDefinition;
}

//=======================================================================
//function : SetDefinition
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::SetDefinition (const StepRepr_RepresentedDefinition &aDefinition)
{
  theDefinition = aDefinition;
}

//=======================================================================
//function : UsedRepresentation
//purpose  : 
//=======================================================================

Handle(StepRepr_Representation) StepRepr_PropertyDefinitionRepresentation::UsedRepresentation () const
{
  return theUsedRepresentation;
}

//=======================================================================
//function : SetUsedRepresentation
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRepresentation::SetUsedRepresentation (const Handle(StepRepr_Representation) &aUsedRepresentation)
{
  theUsedRepresentation = aUsedRepresentation;
}
