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
#include "RWStepFEA_RWElementRepresentation.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_ElementRepresentation.hxx>
#include <StepFEA_HArray1OfNodeRepresentation.hxx>
#include <StepFEA_NodeRepresentation.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>

//=================================================================================================

RWStepFEA_RWElementRepresentation::RWStepFEA_RWElementRepresentation() {}

//=================================================================================================

void RWStepFEA_RWElementRepresentation::ReadStep(
  const Handle(StepData_StepReaderData)&       data,
  const Standard_Integer                       num,
  Handle(Interface_Check)&                     ach,
  const Handle(StepFEA_ElementRepresentation)& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "element_representation"))
    return;

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  data->ReadString(num, 1, "representation.name", ach, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Standard_Integer                             sub2 = 0;
  if (data->ReadSubList(num, 2, "representation.items", ach, sub2))
  {
    Standard_Integer nb0  = data->NbParams(sub2);
    aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem(1, nb0);
    Standard_Integer num2 = sub2;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Handle(StepRepr_RepresentationItem) anIt0;
      data->ReadEntity(num2,
                       i0,
                       "representation_item",
                       ach,
                       STANDARD_TYPE(StepRepr_RepresentationItem),
                       anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  data->ReadEntity(num,
                   3,
                   "representation.context_of_items",
                   ach,
                   STANDARD_TYPE(StepRepr_RepresentationContext),
                   aRepresentation_ContextOfItems);

  // Own fields of ElementRepresentation

  Handle(StepFEA_HArray1OfNodeRepresentation) aNodeList;
  Standard_Integer                            sub4 = 0;
  if (data->ReadSubList(num, 4, "node_list", ach, sub4))
  {
    Standard_Integer nb0  = data->NbParams(sub4);
    aNodeList             = new StepFEA_HArray1OfNodeRepresentation(1, nb0);
    Standard_Integer num2 = sub4;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Handle(StepFEA_NodeRepresentation) anIt0;
      data->ReadEntity(num2,
                       i0,
                       "node_representation",
                       ach,
                       STANDARD_TYPE(StepFEA_NodeRepresentation),
                       anIt0);
      aNodeList->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aRepresentation_Name, aRepresentation_Items, aRepresentation_ContextOfItems, aNodeList);
}

//=================================================================================================

void RWStepFEA_RWElementRepresentation::WriteStep(
  StepData_StepWriter&                         SW,
  const Handle(StepFEA_ElementRepresentation)& ent) const
{

  // Inherited fields of Representation

  SW.Send(ent->StepRepr_Representation::Name());

  SW.OpenSub();
  for (Standard_Integer i1 = 1; i1 <= ent->StepRepr_Representation::NbItems(); i1++)
  {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    SW.Send(Var0);
  }
  SW.CloseSub();

  SW.Send(ent->StepRepr_Representation::ContextOfItems());

  // Own fields of ElementRepresentation

  SW.OpenSub();
  for (Standard_Integer i3 = 1; i3 <= ent->NodeList()->Length(); i3++)
  {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->NodeList()->Value(i3);
    SW.Send(Var0);
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepFEA_RWElementRepresentation::Share(const Handle(StepFEA_ElementRepresentation)& ent,
                                              Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  for (Standard_Integer i1 = 1; i1 <= ent->StepRepr_Representation::NbItems(); i1++)
  {
    Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem(Var0);
  }

  iter.AddItem(ent->StepRepr_Representation::ContextOfItems());

  // Own fields of ElementRepresentation

  for (Standard_Integer i3 = 1; i3 <= ent->NodeList()->Length(); i3++)
  {
    Handle(StepFEA_NodeRepresentation) Var0 = ent->NodeList()->Value(i3);
    iter.AddItem(Var0);
  }
}
