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
#include "RWStepShape_RWGeometricSet.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_GeometricSet.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

RWStepShape_RWGeometricSet::RWStepShape_RWGeometricSet() {}

void RWStepShape_RWGeometricSet::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                          const int                 num,
                                          occ::handle<Interface_Check>&               ach,
                                          const occ::handle<StepShape_GeometricSet>&  ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "geometric_set"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : elements ---

  occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aElements;
  StepShape_GeometricSetSelect                  aElementsItem;
  int                              nsub2;
  if (data->ReadSubList(num, 2, "elements", ach, nsub2))
  {
    int nb2 = data->NbParams(nsub2);
    aElements            = new NCollection_HArray1<StepShape_GeometricSetSelect>(1, nb2);
    for (int i2 = 1; i2 <= nb2; i2++)
    {
      // szv#4:S4163:12Mar99 `bool stat2 =` not needed
      if (data->ReadEntity(nsub2, i2, "elements", ach, aElementsItem))
        aElements->SetValue(i2, aElementsItem);
    }
  }

  //--- Initialisation of the read entity ---

  ent->Init(aName, aElements);
}

void RWStepShape_RWGeometricSet::WriteStep(StepData_StepWriter&                  SW,
                                           const occ::handle<StepShape_GeometricSet>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : elements ---

  SW.OpenSub();
  for (int i2 = 1; i2 <= ent->NbElements(); i2++)
  {
    SW.Send(ent->ElementsValue(i2).Value());
  }
  SW.CloseSub();
}

void RWStepShape_RWGeometricSet::Share(const occ::handle<StepShape_GeometricSet>& ent,
                                       Interface_EntityIterator&             iter) const
{

  int nbElem1 = ent->NbElements();
  for (int is1 = 1; is1 <= nbElem1; is1++)
  {
    iter.GetOneItem(ent->ElementsValue(is1).Value());
  }
}
