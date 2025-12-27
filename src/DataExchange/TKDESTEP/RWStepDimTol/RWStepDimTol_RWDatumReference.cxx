// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <Interface_EntityIterator.hxx>
#include "RWStepDimTol_RWDatumReference.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumReference.hxx>

//=================================================================================================

RWStepDimTol_RWDatumReference::RWStepDimTol_RWDatumReference() {}

//=================================================================================================

void RWStepDimTol_RWDatumReference::ReadStep(const occ::handle<StepData_StepReaderData>&   data,
                                             const int                   num,
                                             occ::handle<Interface_Check>&                 ach,
                                             const occ::handle<StepDimTol_DatumReference>& ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 2, ach, "datum_reference"))
    return;

  // Own fields of DatumReference

  int aPrecedence;
  data->ReadInteger(num, 1, "precedence", ach, aPrecedence);

  occ::handle<StepDimTol_Datum> aReferencedDatum;
  data->ReadEntity(num,
                   2,
                   "referenced_datum",
                   ach,
                   STANDARD_TYPE(StepDimTol_Datum),
                   aReferencedDatum);

  // Initialize entity
  ent->Init(aPrecedence, aReferencedDatum);
}

//=================================================================================================

void RWStepDimTol_RWDatumReference::WriteStep(StepData_StepWriter&                     SW,
                                              const occ::handle<StepDimTol_DatumReference>& ent) const
{

  // Own fields of DatumReference

  SW.Send(ent->Precedence());

  SW.Send(ent->ReferencedDatum());
}

//=================================================================================================

void RWStepDimTol_RWDatumReference::Share(const occ::handle<StepDimTol_DatumReference>& ent,
                                          Interface_EntityIterator&                iter) const
{

  // Own fields of DatumReference

  iter.AddItem(ent->ReferencedDatum());
}
