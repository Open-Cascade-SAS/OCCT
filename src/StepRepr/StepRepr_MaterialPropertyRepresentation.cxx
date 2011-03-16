// File:	StepRepr_MaterialPropertyRepresentation.cxx
// Created:	Thu Dec 12 16:07:47 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepRepr_MaterialPropertyRepresentation.ixx>

//=======================================================================
//function : StepRepr_MaterialPropertyRepresentation
//purpose  : 
//=======================================================================

StepRepr_MaterialPropertyRepresentation::StepRepr_MaterialPropertyRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_MaterialPropertyRepresentation::Init (const StepRepr_RepresentedDefinition &aPropertyDefinitionRepresentation_Definition,
                                                    const Handle(StepRepr_Representation) &aPropertyDefinitionRepresentation_UsedRepresentation,
                                                    const Handle(StepRepr_DataEnvironment) &aDependentEnvironment)
{
  StepRepr_PropertyDefinitionRepresentation::Init(aPropertyDefinitionRepresentation_Definition,
                                                  aPropertyDefinitionRepresentation_UsedRepresentation);

  theDependentEnvironment = aDependentEnvironment;
}

//=======================================================================
//function : DependentEnvironment
//purpose  : 
//=======================================================================

Handle(StepRepr_DataEnvironment) StepRepr_MaterialPropertyRepresentation::DependentEnvironment () const
{
  return theDependentEnvironment;
}

//=======================================================================
//function : SetDependentEnvironment
//purpose  : 
//=======================================================================

void StepRepr_MaterialPropertyRepresentation::SetDependentEnvironment (const Handle(StepRepr_DataEnvironment) &aDependentEnvironment)
{
  theDependentEnvironment = aDependentEnvironment;
}
