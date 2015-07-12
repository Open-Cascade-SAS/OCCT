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
#include <RWStepShape_RWShapeDefinitionRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_Representation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>

//=======================================================================
//function : RWStepShape_RWShapeDefinitionRepresentation
//purpose  : 
//=======================================================================
RWStepShape_RWShapeDefinitionRepresentation::RWStepShape_RWShapeDefinitionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepShape_ShapeDefinitionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"shape_definition_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aPropertyDefinitionRepresentation_Definition;
  data->ReadEntity (num, 1, "property_definition_representation.definition", ach, aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  data->ReadEntity (num, 2, "property_definition_representation.used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Initialize entity
  ent->Init(aPropertyDefinitionRepresentation_Definition,
            aPropertyDefinitionRepresentation_UsedRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepShape_ShapeDefinitionRepresentation) &ent) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  SW.Send (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDefinitionRepresentation::Share (const Handle(StepShape_ShapeDefinitionRepresentation) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (ent->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());
}
