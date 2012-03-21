// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <RWStepRepr_RWCompoundRepresentationItem.ixx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>


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
