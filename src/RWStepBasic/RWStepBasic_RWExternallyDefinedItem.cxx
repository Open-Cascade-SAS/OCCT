// Created on: 2000-05-10
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
#include <RWStepBasic_RWExternallyDefinedItem.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWExternallyDefinedItem
//purpose  : 
//=======================================================================
RWStepBasic_RWExternallyDefinedItem::RWStepBasic_RWExternallyDefinedItem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_ExternallyDefinedItem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"externally_defined_item") ) return;

  // Own fields of ExternallyDefinedItem

  StepBasic_SourceItem aItemId;
  data->ReadEntity (num, 1, "item_id", ach, aItemId);

  Handle(StepBasic_ExternalSource) aSource;
  data->ReadEntity (num, 2, "source", ach, STANDARD_TYPE(StepBasic_ExternalSource), aSource);

  // Initialize entity
  ent->Init(aItemId,
            aSource);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_ExternallyDefinedItem) &ent) const
{

  // Own fields of ExternallyDefinedItem

  SW.Send (ent->ItemId().Value());

  SW.Send (ent->Source());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWExternallyDefinedItem::Share (const Handle(StepBasic_ExternallyDefinedItem) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Own fields of ExternallyDefinedItem

  iter.AddItem (ent->ItemId().Value());

  iter.AddItem (ent->Source());
}
