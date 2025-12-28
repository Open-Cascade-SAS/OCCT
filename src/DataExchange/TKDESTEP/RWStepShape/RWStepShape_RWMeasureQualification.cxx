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
#include "RWStepShape_RWMeasureQualification.pxx"
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_MeasureQualification.hxx>
#include <StepShape_ValueQualifier.hxx>
#include <TCollection_HAsciiString.hxx>

RWStepShape_RWMeasureQualification::RWStepShape_RWMeasureQualification() {}

void RWStepShape_RWMeasureQualification::ReadStep(
  const occ::handle<StepData_StepReaderData>&        data,
  const int                                          num,
  occ::handle<Interface_Check>&                      ach,
  const occ::handle<StepShape_MeasureQualification>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "measure_qualification"))
    return;

  // --- own field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : description ---

  occ::handle<TCollection_HAsciiString> aDescr;
  data->ReadString(num, 2, "description", ach, aDescr);

  // --- own field : qualified_measure ---

  occ::handle<Standard_Transient> aQM;
  data->ReadEntity(num, 3, "qualified_measure", ach, STANDARD_TYPE(Standard_Transient), aQM);

  // --- own field : qualifiers ---

  occ::handle<NCollection_HArray1<StepShape_ValueQualifier>> quals;
  int                                                        nsub4;
  if (data->ReadSubList(num, 4, "qualifiers", ach, nsub4))
  {
    int nb4 = data->NbParams(nsub4);
    quals   = new NCollection_HArray1<StepShape_ValueQualifier>(1, nb4);
    for (int i4 = 1; i4 <= nb4; i4++)
    {
      StepShape_ValueQualifier VQ;
      if (data->ReadEntity(nsub4, i4, "qualifier", ach, VQ))
        quals->SetValue(i4, VQ);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aDescr, aQM, quals);
}

void RWStepShape_RWMeasureQualification::WriteStep(
  StepData_StepWriter&                               SW,
  const occ::handle<StepShape_MeasureQualification>& ent) const
{
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->QualifiedMeasure());
  int i, nbq = ent->NbQualifiers();
  SW.OpenSub();
  for (i = 1; i <= nbq; i++)
    SW.Send(ent->QualifiersValue(i).Value());
  SW.CloseSub();
}

void RWStepShape_RWMeasureQualification::Share(
  const occ::handle<StepShape_MeasureQualification>& ent,
  Interface_EntityIterator&                          iter) const
{
  int i, nbq = ent->NbQualifiers();
  for (i = 1; i <= nbq; i++)
    iter.AddItem(ent->QualifiersValue(i).Value());
}
