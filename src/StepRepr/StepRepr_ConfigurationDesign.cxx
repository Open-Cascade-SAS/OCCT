// File:	StepRepr_ConfigurationDesign.cxx
// Created:	Fri Nov 26 16:26:36 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepRepr_ConfigurationDesign.ixx>

//=======================================================================
//function : StepRepr_ConfigurationDesign
//purpose  : 
//=======================================================================

StepRepr_ConfigurationDesign::StepRepr_ConfigurationDesign ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::Init (const Handle(StepRepr_ConfigurationItem) &aConfiguration,
                                         const StepRepr_ConfigurationDesignItem &aDesign)
{

  theConfiguration = aConfiguration;

  theDesign = aDesign;
}

//=======================================================================
//function : Configuration
//purpose  : 
//=======================================================================

Handle(StepRepr_ConfigurationItem) StepRepr_ConfigurationDesign::Configuration () const
{
  return theConfiguration;
}

//=======================================================================
//function : SetConfiguration
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::SetConfiguration (const Handle(StepRepr_ConfigurationItem) &aConfiguration)
{
  theConfiguration = aConfiguration;
}

//=======================================================================
//function : Design
//purpose  : 
//=======================================================================

StepRepr_ConfigurationDesignItem StepRepr_ConfigurationDesign::Design () const
{
  return theDesign;
}

//=======================================================================
//function : SetDesign
//purpose  : 
//=======================================================================

void StepRepr_ConfigurationDesign::SetDesign (const StepRepr_ConfigurationDesignItem &aDesign)
{
  theDesign = aDesign;
}
