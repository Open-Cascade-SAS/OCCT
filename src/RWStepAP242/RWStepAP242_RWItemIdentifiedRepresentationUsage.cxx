// Created on: 2015-07-07
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <RWStepAP242_RWItemIdentifiedRepresentationUsage.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepAP242_ItemIdentifiedRepresentationUsage.hxx>
#include <StepAP242_ItemIdentifiedRepresentationUsageDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationItem.hxx>


RWStepAP242_RWItemIdentifiedRepresentationUsage::RWStepAP242_RWItemIdentifiedRepresentationUsage () {}

void RWStepAP242_RWItemIdentifiedRepresentationUsage::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent) const
{
  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,5,ach,"item_identified_representation_usage")) return;

  // --- own field : name ---

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num,1,"name",ach,aName);

  // --- own field : description ---

  Handle(TCollection_HAsciiString) aDescription;
  if (data->IsParamDefined (num,2)) {
    data->ReadString (num,2,"description",ach,aDescription);
  }
  // --- own field : definition ---

  StepAP242_ItemIdentifiedRepresentationUsageDefinition aDefinition;
  data->ReadEntity(num,3,"definition",ach,aDefinition);

  // --- own field : used_representation ---

  Handle(StepRepr_Representation) aRepresentation;
  data->ReadEntity (num,4,"used_representation",ach,STANDARD_TYPE(StepRepr_Representation), aRepresentation);

  // --- own field : identified_item

  Handle(StepRepr_HArray1OfRepresentationItem) anItems;
  Handle(StepRepr_RepresentationItem) anEnt;
  Standard_Integer nbSub;
  Interface_ParamType aType = data->ParamType(num, 5);
  if (aType == Interface_ParamIdent) {
    data->ReadEntity(num, 5,"item_identified_representation_usage.identified_item", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anEnt);
    anItems = new StepRepr_HArray1OfRepresentationItem (1, 1);
    anItems->SetValue(1, anEnt);
  }
  else if (data->ReadSubList (num,5,"item_identified_representation_usage.identified_item",ach,nbSub)) {
    Standard_Integer nbElements = data->NbParams(nbSub);
    anItems = new StepRepr_HArray1OfRepresentationItem (1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      if (data->ReadEntity(nbSub, i,"representation_item", ach,
         STANDARD_TYPE(StepRepr_RepresentationItem), anEnt))
        anItems->SetValue(i, anEnt);
    }
  }

  //--- Initialisation of the read entity ---


  ent->Init(aName, aDescription, aDefinition, aRepresentation, anItems);
}


void RWStepAP242_RWItemIdentifiedRepresentationUsage::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent) const
{

  // --- own field : name ---

  SW.Send(ent->Name());

  // --- own field : description ---

  SW.Send(ent->Description());

  // --- own field : definition ---

  SW.Send(ent->Definition().Value());

  // --- own field : used_representation ---

  SW.Send(ent->UsedRepresentation());

  // --- own field : identified_item ---

  if (ent->NbIdentifiedItem() == 1)
    SW.Send(ent->IdentifiedItemValue(1));
  else {
    SW.OpenSub();
    for (Standard_Integer i = 1;  i <= ent->NbIdentifiedItem();  i++) {
      SW.Send(ent->IdentifiedItemValue(i));
    }
    SW.CloseSub();
  }
}


void RWStepAP242_RWItemIdentifiedRepresentationUsage::Share(
  const Handle(StepAP242_ItemIdentifiedRepresentationUsage)& ent, 
  Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->Definition().Value());
  Standard_Integer i, nb = ent->NbIdentifiedItem();
  for (i = 1; i <= nb; i++)  
    iter.AddItem (ent->IdentifiedItemValue(i));
}

