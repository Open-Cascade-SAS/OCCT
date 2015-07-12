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
#include <RWStepRepr_RWConfigurationItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ConfigurationItem.hxx>
#include <StepRepr_ProductConcept.hxx>

//=======================================================================
//function : RWStepRepr_RWConfigurationItem
//purpose  : 
//=======================================================================
RWStepRepr_RWConfigurationItem::RWStepRepr_RWConfigurationItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                               const Standard_Integer num,
                                               Handle(Interface_Check)& ach,
                                               const Handle(StepRepr_ConfigurationItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"configuration_item") ) return;

  // Own fields of ConfigurationItem

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

  Handle(StepRepr_ProductConcept) aItemConcept;
  data->ReadEntity (num, 4, "item_concept", ach, STANDARD_TYPE(StepRepr_ProductConcept), aItemConcept);

  Handle(TCollection_HAsciiString) aPurpose;
  Standard_Boolean hasPurpose = Standard_True;
  if ( data->IsParamDefined (num,5) ) {
    data->ReadString (num, 5, "purpose", ach, aPurpose);
  }
  else {
    hasPurpose = Standard_False;
  }

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription,
            aItemConcept,
            hasPurpose,
            aPurpose);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::WriteStep (StepData_StepWriter& SW,
                                                const Handle(StepRepr_ConfigurationItem) &ent) const
{

  // Own fields of ConfigurationItem

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->ItemConcept());

  if ( ent->HasPurpose() ) {
    SW.Send (ent->Purpose());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWConfigurationItem::Share (const Handle(StepRepr_ConfigurationItem) &ent,
                                            Interface_EntityIterator& iter) const
{

  // Own fields of ConfigurationItem

  iter.AddItem (ent->ItemConcept());
}
