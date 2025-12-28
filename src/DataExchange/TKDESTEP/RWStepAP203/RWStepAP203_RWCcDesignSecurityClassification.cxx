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
#include "RWStepAP203_RWCcDesignSecurityClassification.pxx"
#include <StepAP203_CcDesignSecurityClassification.hxx>
#include <StepAP203_ClassifiedItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepBasic_SecurityClassification.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepAP203_RWCcDesignSecurityClassification::RWStepAP203_RWCcDesignSecurityClassification() {}

//=================================================================================================

void RWStepAP203_RWCcDesignSecurityClassification::ReadStep(
  const occ::handle<StepData_StepReaderData>&                  data,
  const int                                                    num,
  occ::handle<Interface_Check>&                                ach,
  const occ::handle<StepAP203_CcDesignSecurityClassification>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "cc_design_security_classification"))
    return;

  // Inherited fields of SecurityClassificationAssignment

  occ::handle<StepBasic_SecurityClassification>
    aSecurityClassificationAssignment_AssignedSecurityClassification;
  data->ReadEntity(num,
                   1,
                   "security_classification_assignment.assigned_security_classification",
                   ach,
                   STANDARD_TYPE(StepBasic_SecurityClassification),
                   aSecurityClassificationAssignment_AssignedSecurityClassification);

  // Own fields of CcDesignSecurityClassification

  occ::handle<NCollection_HArray1<StepAP203_ClassifiedItem>> aItems;
  int                                                        sub2 = 0;
  if (data->ReadSubList(num, 2, "items", ach, sub2))
  {
    int num2 = sub2;
    int nb0  = data->NbParams(num2);
    aItems   = new NCollection_HArray1<StepAP203_ClassifiedItem>(1, nb0);
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepAP203_ClassifiedItem anIt0;
      data->ReadEntity(num2, i0, "items", ach, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aSecurityClassificationAssignment_AssignedSecurityClassification, aItems);
}

//=================================================================================================

void RWStepAP203_RWCcDesignSecurityClassification::WriteStep(
  StepData_StepWriter&                                         SW,
  const occ::handle<StepAP203_CcDesignSecurityClassification>& ent) const
{

  // Inherited fields of SecurityClassificationAssignment

  SW.Send(ent->StepBasic_SecurityClassificationAssignment::AssignedSecurityClassification());

  // Own fields of CcDesignSecurityClassification

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->Items()->Length(); i1++)
  {
    StepAP203_ClassifiedItem Var0 = ent->Items()->Value(i1);
    SW.Send(Var0.Value());
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepAP203_RWCcDesignSecurityClassification::Share(
  const occ::handle<StepAP203_CcDesignSecurityClassification>& ent,
  Interface_EntityIterator&                                    iter) const
{

  // Inherited fields of SecurityClassificationAssignment

  iter.AddItem(ent->StepBasic_SecurityClassificationAssignment::AssignedSecurityClassification());

  // Own fields of CcDesignSecurityClassification

  for (int i2 = 1; i2 <= ent->Items()->Length(); i2++)
  {
    StepAP203_ClassifiedItem Var0 = ent->Items()->Value(i2);
    iter.AddItem(Var0.Value());
  }
}
