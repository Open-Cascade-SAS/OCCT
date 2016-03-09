// Created on: 2000-07-03
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWProductDefinitionRelationship.hxx>
#include <StepBasic_ProductDefinitionOrReference.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWProductDefinitionRelationship
//purpose  : 
//=======================================================================
RWStepBasic_RWProductDefinitionRelationship::RWStepBasic_RWProductDefinitionRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                            const Standard_Integer num,
                                                            Handle(Interface_Check)& ach,
                                                            const Handle(StepBasic_ProductDefinitionRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"product_definition_relationship") ) return;

  // Own fields of ProductDefinitionRelationship

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  StepBasic_ProductDefinitionOrReference aRelatingProductDefinition;
  data->ReadEntity (num, 4, "relating_product_definition", ach, aRelatingProductDefinition);

  StepBasic_ProductDefinitionOrReference aRelatedProductDefinition;
  data->ReadEntity (num, 5, "related_product_definition", ach, aRelatedProductDefinition);

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aRelatingProductDefinition,
            aRelatedProductDefinition);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::WriteStep (StepData_StepWriter& SW,
                                                             const Handle(StepBasic_ProductDefinitionRelationship) &ent) const
{

  // Own fields of ProductDefinitionRelationship

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->RelatingProductDefinitionAP242().Value());

  SW.Send (ent->RelatedProductDefinitionAP242().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductDefinitionRelationship::Share (const Handle(StepBasic_ProductDefinitionRelationship) &ent,
                                                         Interface_EntityIterator& iter) const
{

  // Own fields of ProductDefinitionRelationship

  iter.AddItem (ent->RelatingProductDefinitionAP242().Value());

  iter.AddItem (ent->RelatedProductDefinitionAP242().Value());
}
