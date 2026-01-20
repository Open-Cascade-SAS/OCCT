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
#include "RWStepAP214_RWAutoDesignDocumentReference.pxx"
#include <StepAP214_AutoDesignDocumentReference.hxx>
#include <StepAP214_AutoDesignReferencingItem.hxx>
#include <StepAP214_AutoDesignReferencingItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_Document.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepAP214_RWAutoDesignDocumentReference::RWStepAP214_RWAutoDesignDocumentReference() {}

void RWStepAP214_RWAutoDesignDocumentReference::ReadStep(
  const occ::handle<StepData_StepReaderData>&               data,
  const int                               num,
  occ::handle<Interface_Check>&                             ach,
  const occ::handle<StepAP214_AutoDesignDocumentReference>& ent) const
{
  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "auto_deesign_document_reference"))
    return;

  // --- inherited field : assigned_document

  occ::handle<StepBasic_Document> adoc;
  data->ReadEntity(num, 1, "assigned_document", ach, STANDARD_TYPE(StepBasic_Document), adoc);

  // --- inherited field : source ---

  occ::handle<TCollection_HAsciiString> asource;
  data->ReadString(num, 2, "source", ach, asource);

  // --- own field : items ---

  occ::handle<NCollection_HArray1<StepAP214_AutoDesignReferencingItem>> aItems;
  StepAP214_AutoDesignReferencingItem                  anItem;
  int                                     nsub3;
  if (data->ReadSubList(num, 3, "items", ach, nsub3))
  {
    int nb3 = data->NbParams(nsub3);
    aItems               = new NCollection_HArray1<StepAP214_AutoDesignReferencingItem>(1, nb3);
    for (int i3 = 1; i3 <= nb3; i3++)
    {
      bool stat3 = data->ReadEntity(nsub3, i3, "item", ach, anItem);
      if (stat3)
        aItems->SetValue(i3, anItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(adoc, asource, aItems);
}

void RWStepAP214_RWAutoDesignDocumentReference::WriteStep(
  StepData_StepWriter&                                 SW,
  const occ::handle<StepAP214_AutoDesignDocumentReference>& ent) const
{

  // --- inherited field : assigned_document ---

  SW.Send(ent->AssignedDocument());

  // --- inherited field : source ---

  SW.Send(ent->Source());

  // --- own field : items ---

  SW.OpenSub();
  for (int i3 = 1; i3 <= ent->NbItems(); i3++)
  {
    SW.Send(ent->ItemsValue(i3).Value());
  }
  SW.CloseSub();
}

void RWStepAP214_RWAutoDesignDocumentReference::Share(
  const occ::handle<StepAP214_AutoDesignDocumentReference>& ent,
  Interface_EntityIterator&                            iter) const
{
  iter.AddItem(ent->AssignedDocument());
  for (int i3 = 1; i3 <= ent->NbItems(); i3++)
    iter.AddItem(ent->ItemsValue(i3).Value());
}
