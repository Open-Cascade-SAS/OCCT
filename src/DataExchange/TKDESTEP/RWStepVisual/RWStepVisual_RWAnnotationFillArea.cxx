// Created on: 2016-12-28
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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
#include "RWStepVisual_RWAnnotationFillArea.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepVisual_RWAnnotationFillArea::RWStepVisual_RWAnnotationFillArea() = default;

//=================================================================================================

void RWStepVisual_RWAnnotationFillArea::ReadStep(
  const occ::handle<StepData_StepReaderData>&       data,
  const int                                         num,
  occ::handle<Interface_Check>&                     ach,
  const occ::handle<StepVisual_AnnotationFillArea>& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 2, ach, "annotation_fill_area"))
    return;

  // Inherited field : name

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  // Own field : boundaries
  occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aElements;
  StepShape_GeometricSetSelect                                   aElementsItem;
  int                                                            nsub;
  if (data->ReadSubList(num, 2, "boundaries", ach, nsub))
  {
    int nb    = data->NbParams(nsub);
    aElements = new NCollection_HArray1<StepShape_GeometricSetSelect>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      if (data->ReadEntity(nsub, i, "boundaries", ach, aElementsItem))
        aElements->SetValue(i, aElementsItem);
    }
  }

  // Initialization of the read entity
  ent->Init(aName, aElements);
}

//=================================================================================================

void RWStepVisual_RWAnnotationFillArea::WriteStep(
  StepData_StepWriter&                              SW,
  const occ::handle<StepVisual_AnnotationFillArea>& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own field : elements
  SW.OpenSub();
  for (int i = 1; i <= ent->NbElements(); i++)
  {
    SW.Send(ent->ElementsValue(i).Value());
  }
  SW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWAnnotationFillArea::Share(const occ::handle<StepVisual_AnnotationFillArea>& ent,
                                              Interface_EntityIterator& iter) const
{
  int nbBound = ent->NbElements();
  for (int i = 1; i <= nbBound; i++)
  {
    iter.GetOneItem(ent->ElementsValue(i).Value());
  }
}
