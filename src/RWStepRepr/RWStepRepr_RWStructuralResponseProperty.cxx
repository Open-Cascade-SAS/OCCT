// Created on: 2002-12-15
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWStructuralResponseProperty.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_StructuralResponseProperty.hxx>

//#include <StepRepr_RepresentedDefinition.hxx>
//=======================================================================
//function : RWStepRepr_RWStructuralResponseProperty
//purpose  : 
//=======================================================================
RWStepRepr_RWStructuralResponseProperty::RWStepRepr_RWStructuralResponseProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                        const Standard_Integer num,
                                                        Handle(Interface_Check)& ach,
                                                        const Handle(StepRepr_StructuralResponseProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"structural_response_property") ) return;

  // Inherited fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aPropertyDefinition_Name;
  data->ReadString (num, 1, "property_definition.name", ach, aPropertyDefinition_Name);

  Handle(TCollection_HAsciiString) aPropertyDefinition_Description;
  data->ReadString (num, 2, "property_definition.description", ach, aPropertyDefinition_Description);

  StepRepr_CharacterizedDefinition aPropertyDefinition_Definition;
  data->ReadEntity (num, 3, "property_definition.definition", ach, aPropertyDefinition_Definition);

  // Initialize entity
  ent->Init(aPropertyDefinition_Name, Standard_True,
            aPropertyDefinition_Description,
            aPropertyDefinition_Definition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::WriteStep (StepData_StepWriter& SW,
                                                         const Handle(StepRepr_StructuralResponseProperty) &ent) const
{

  // Inherited fields of PropertyDefinition

  SW.Send (ent->StepRepr_PropertyDefinition::Name());

  SW.Send (ent->StepRepr_PropertyDefinition::Description());

  SW.Send (ent->StepRepr_PropertyDefinition::Definition().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWStructuralResponseProperty::Share (const Handle(StepRepr_StructuralResponseProperty) &ent,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (ent->StepRepr_PropertyDefinition::Definition().Value());
}
