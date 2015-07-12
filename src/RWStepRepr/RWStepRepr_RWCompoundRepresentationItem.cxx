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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWCompoundRepresentationItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepRepr_RWCompoundRepresentationItem::RWStepRepr_RWCompoundRepresentationItem  ()    {  }

void  RWStepRepr_RWCompoundRepresentationItem::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepRepr_CompoundRepresentationItem)& ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num,2,ach,"compound_representation_item")) return;

  // --- inherited field : name ---

  Handle(TCollection_HAsciiString) aName;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString (num,1,"name",ach,aName);

  // --- own field : item_element

  Handle(StepRepr_HArray1OfRepresentationItem) aItems;
  Handle(StepRepr_RepresentationItem) anent2;
  Standard_Integer nsub2;
  if (data->ReadSubList (num,2,"item_element",ach,nsub2)) {
    Standard_Integer nb2 = data->NbParams(nsub2);
    aItems = new StepRepr_HArray1OfRepresentationItem (1, nb2);
    for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
      if (data->ReadEntity(nsub2, i2,"representation_item", ach,
			   STANDARD_TYPE(StepRepr_RepresentationItem), anent2))
	aItems->SetValue(i2, anent2);
    }
  }

  ent->Init (aName,aItems);
}

void  RWStepRepr_RWCompoundRepresentationItem::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepRepr_CompoundRepresentationItem)& ent) const
{
  // --- inherited field : name ---

  SW.Send(ent->Name());

  // --- own field : items ---

  SW.OpenSub();
  for (Standard_Integer i2 = 1;  i2 <= ent->NbItemElement();  i2 ++) {
    SW.Send(ent->ItemElementValue(i2));
  }
  SW.CloseSub();
}

void  RWStepRepr_RWCompoundRepresentationItem::Share
  (const Handle(StepRepr_CompoundRepresentationItem)& ent,
   Interface_EntityIterator& iter) const
{
  Standard_Integer i, nb = ent->NbItemElement();
  for (i = 1; i <= nb; i ++)  iter.AddItem (ent->ItemElementValue(i));
}
