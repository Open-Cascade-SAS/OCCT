// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWConfigurationEffectivity.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ConfigurationDesign.hxx>
#include <StepRepr_ConfigurationEffectivity.hxx>

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
