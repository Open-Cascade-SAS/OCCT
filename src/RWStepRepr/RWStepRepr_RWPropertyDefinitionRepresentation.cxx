// Created on: 2002-12-12
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
#include <RWStepRepr_RWPropertyDefinitionRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>

//=======================================================================
//function : RWStepRepr_RWPropertyDefinitionRepresentation
//purpose  : 
//=======================================================================
RWStepRepr_RWPropertyDefinitionRepresentation::RWStepRepr_RWPropertyDefinitionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepRepr_PropertyDefinitionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"property_definition_representation") ) return;

  // Own fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aDefinition;
  data->ReadEntity (num, 1, "definition", ach, aDefinition);

  Handle(StepRepr_Representation) aUsedRepresentation;
  data->ReadEntity (num, 2, "used_representation", ach, STANDARD_TYPE(StepRepr_Representation), aUsedRepresentation);

  // Initialize entity
  ent->Init(aDefinition,
            aUsedRepresentation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepRepr_PropertyDefinitionRepresentation) &ent) const
{

  // Own fields of PropertyDefinitionRepresentation

  SW.Send (ent->Definition().Value());

  SW.Send (ent->UsedRepresentation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWPropertyDefinitionRepresentation::Share (const Handle(StepRepr_PropertyDefinitionRepresentation) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Own fields of PropertyDefinitionRepresentation

  iter.AddItem (ent->Definition().Value());

  iter.AddItem (ent->UsedRepresentation());
}
