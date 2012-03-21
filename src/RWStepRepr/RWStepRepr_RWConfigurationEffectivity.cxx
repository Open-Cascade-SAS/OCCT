// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <RWStepRepr_RWConfigurationEffectivity.ixx>

//=======================================================================
//function : RWStepRepr_RWConfigurationEffectivity
//purpose  : 
//=======================================================================

RWStepRepr_RWConfigurationEffectivity::RWStepRepr_RWConfigurationEffectivity ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationEffectivity::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepRepr_ConfigurationEffectivity) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"configuration_effectivity") ) return;

  // Inherited fields of Effectivity

  Handle(TCollection_HAsciiString) aEffectivity_Id;
  data->ReadString (num, 1, "effectivity.id", ach, aEffectivity_Id);

  // Inherited fields of ProductDefinitionEffectivity

  Handle(StepBasic_ProductDefinitionRelationship) aProductDefinitionEffectivity_Usage;
  data->ReadEntity (num, 2, "product_definition_effectivity.usage", ach, STANDARD_TYPE(StepBasic_ProductDefinitionRelationship), aProductDefinitionEffectivity_Usage);

  // Own fields of ConfigurationEffectivity

  Handle(StepRepr_ConfigurationDesign) aConfiguration;
  data->ReadEntity (num, 3, "configuration", ach, STANDARD_TYPE(StepRepr_ConfigurationDesign), aConfiguration);

  // Initialize entity
  ent->Init(aEffectivity_Id,
            aProductDefinitionEffectivity_Usage,
            aConfiguration);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationEffectivity::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepRepr_ConfigurationEffectivity) &ent) const
{

  // Inherited fields of Effectivity

  SW.Send (ent->StepBasic_Effectivity::Id());

  // Inherited fields of ProductDefinitionEffectivity

  SW.Send (ent->StepBasic_ProductDefinitionEffectivity::Usage());

  // Own fields of ConfigurationEffectivity

  SW.Send (ent->Configuration());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationEffectivity::Share (const Handle(StepRepr_ConfigurationEffectivity) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of Effectivity

  // Inherited fields of ProductDefinitionEffectivity

  iter.AddItem (ent->StepBasic_ProductDefinitionEffectivity::Usage());

  // Own fields of ConfigurationEffectivity

  iter.AddItem (ent->Configuration());
}
