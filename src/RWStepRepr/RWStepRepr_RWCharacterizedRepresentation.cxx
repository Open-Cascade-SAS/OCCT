// Created on: 2016-08-25
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWCharacterizedRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_CharacterizedRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>

//=======================================================================
//function : RWStepRepr_RWCharacterizedRepresentation
//purpose  : 
//=======================================================================
RWStepRepr_RWCharacterizedRepresentation::RWStepRepr_RWCharacterizedRepresentation () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepRepr_RWCharacterizedRepresentation::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepRepr_CharacterizedRepresentation)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 4, ach, "characterized_representation")) return;

  // name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);
  
  // description
  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  // items
  Handle(StepRepr_HArray1OfRepresentationItem) anItems;
  Handle(StepRepr_RepresentationItem) anItem;
  Standard_Integer nsub;
  if (data->ReadSubList (num, 3, "items", ach, nsub)) {
    Standard_Integer nb = data->NbParams(nsub);
    anItems = new StepRepr_HArray1OfRepresentationItem (1, nb);
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (data->ReadEntity (nsub, i,"representation_item", ach,
        STANDARD_TYPE(StepRepr_RepresentationItem), anItem))
        anItems->SetValue(i, anItem);
    }
  }

  // context_of_items
  Handle(StepRepr_RepresentationContext) aContextOfItems;
  data->ReadEntity(num, 4, "context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aContextOfItems);

  // Initialization of the read entity
  ent->Init(aName, aDescription, anItems, aContextOfItems);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepRepr_RWCharacterizedRepresentation::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepRepr_CharacterizedRepresentation)& ent) const
{
  // name
  SW.Send(ent->Name());

  // description
  if (!ent->Description().IsNull())
    SW.Send(ent->Description());
  else
    SW.SendUndef();

  // items 
  SW.OpenSub();
  for (Standard_Integer i = 1;  i <= ent->NbItems();  i ++) {
    SW.Send(ent->ItemsValue(i));
  }
  SW.CloseSub();

  // context_of_items
  SW.Send(ent->ContextOfItems());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepRepr_RWCharacterizedRepresentation::Share
  (const Handle(StepRepr_CharacterizedRepresentation)& ent,
   Interface_EntityIterator& iter) const
{
  Standard_Integer nbElem = ent->NbItems();
  for (Standard_Integer i=1; i <= nbElem; i++) {
    iter.GetOneItem(ent->ItemsValue(i));
  }
  iter.GetOneItem(ent->ContextOfItems());
}

