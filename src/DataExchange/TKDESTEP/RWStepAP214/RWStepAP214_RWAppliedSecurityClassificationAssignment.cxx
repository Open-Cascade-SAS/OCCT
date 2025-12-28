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
#include "RWStepAP214_RWAppliedSecurityClassificationAssignment.pxx"
#include <StepAP214_AppliedSecurityClassificationAssignment.hxx>
#include <StepAP214_SecurityClassificationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepAP214_SecurityClassificationItem.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepAP214_RWAppliedSecurityClassificationAssignment::
  RWStepAP214_RWAppliedSecurityClassificationAssignment()
{
}

void RWStepAP214_RWAppliedSecurityClassificationAssignment::ReadStep(
  const occ::handle<StepData_StepReaderData>&                           data,
  const int                                           num,
  occ::handle<Interface_Check>&                                         ach,
  const occ::handle<StepAP214_AppliedSecurityClassificationAssignment>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "applied_security_classification_assignment"))
    return;

  // --- inherited field : assignedSecurityClassification ---

  occ::handle<StepBasic_SecurityClassification> aAssignedSecurityClassification;
  data->ReadEntity(num,
                   1,
                   "assigned_security_classification",
                   ach,
                   STANDARD_TYPE(StepBasic_SecurityClassification),
                   aAssignedSecurityClassification);

  // --- own field : items ---

  occ::handle<NCollection_HArray1<StepAP214_SecurityClassificationItem>> aItems;
  StepAP214_SecurityClassificationItem                  anent2;
  int                                      nsub2;
  if (data->ReadSubList(num, 2, "items", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aItems               = new NCollection_HArray1<StepAP214_SecurityClassificationItem>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      bool stat2 = data->ReadEntity(nsub2, i2, "items", ach, anent2);
      if (stat2)
        aItems->SetValue(i2, anent2);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aAssignedSecurityClassification, aItems);
}

void RWStepAP214_RWAppliedSecurityClassificationAssignment::WriteStep(
  StepData_StepWriter&                                             SW,
  const occ::handle<StepAP214_AppliedSecurityClassificationAssignment>& ent) const
{

  // --- inherited field assignedSecurityClassification ---

  SW.Send(ent->AssignedSecurityClassification());

  // --- own field : items ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbItems(); i2++)
  {
    SW.Send(ent->ItemsValue(i2).Value());
  }
  SW.CloseSub();
}

void RWStepAP214_RWAppliedSecurityClassificationAssignment::Share(
  const occ::handle<StepAP214_AppliedSecurityClassificationAssignment>& ent,
  Interface_EntityIterator&                                        iter) const
{

  iter.GetOneItem(ent->AssignedSecurityClassification());

  int nbElem2 = ent->NbItems();
  for (int is2 = 1; is2 <= nbElem2; is2++)
  {
    iter.GetOneItem(ent->ItemsValue(is2).Value());
  }
}
