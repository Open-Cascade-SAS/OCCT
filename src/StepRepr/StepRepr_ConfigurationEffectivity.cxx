// File:	StepRepr_ConfigurationEffectivity.cxx
// Created:	Fri Nov 26 16:26:36 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepRepr_ConfigurationEffectivity.ixx>

//=======================================================================
//function : StepRepr_ConfigurationEffectivity
//purpose  : 
//=======================================================================

StepRepr_ConfigurationEffectivity::StepRepr_ConfigurationEffectivity ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationEffectivity::Init (const Handle(TCollection_HAsciiString) &aEffectivity_Id,
                                              const Handle(StepBasic_ProductDefinitionRelationship) &aProductDefinitionEffectivity_Usage,
                                              const Handle(StepRepr_ConfigurationDesign) &aConfiguration)
{
  StepBasic_ProductDefinitionEffectivity::Init(aEffectivity_Id,
                                               aProductDefinitionEffectivity_Usage);

  theConfiguration = aConfiguration;
}

//=======================================================================
//function : Configuration
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationDesign) StepRepr_ConfigurationEffectivity::Configuration () const
{
  return theConfiguration;
}

//=======================================================================
//function : SetConfiguration
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationEffectivity::SetConfiguration (const Handle(StepRepr_ConfigurationDesign) &aConfiguration)
{
  theConfiguration = aConfiguration;
}
