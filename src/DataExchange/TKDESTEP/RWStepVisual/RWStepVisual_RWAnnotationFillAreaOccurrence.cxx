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
#include "RWStepVisual_RWAnnotationFillAreaOccurrence.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepVisual_AnnotationFillAreaOccurrence.hxx>

//=================================================================================================

RWStepVisual_RWAnnotationFillAreaOccurrence::RWStepVisual_RWAnnotationFillAreaOccurrence() {}

//=================================================================================================

void RWStepVisual_RWAnnotationFillAreaOccurrence::ReadStep(
  const occ::handle<StepData_StepReaderData>&                 data,
  const int                                 num,
  occ::handle<Interface_Check>&                               ach,
  const occ::handle<StepVisual_AnnotationFillAreaOccurrence>& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 4, ach, "annotation_fill_area_occurrence"))
    return;

  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  // Inherited field : styles
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aStyles;
  occ::handle<StepVisual_PresentationStyleAssignment>          anent;
  int                                        nsub;
  if (data->ReadSubList(num, 2, "styles", ach, nsub))
  {
    int nb = data->NbParams(nsub);
    aStyles             = new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      if (data->ReadEntity(nsub,
                           i,
                           "presentation_style_assignment",
                           ach,
                           STANDARD_TYPE(StepVisual_PresentationStyleAssignment),
                           anent))
        aStyles->SetValue(i, anent);
    }
  }

  // Inherited field : item
  occ::handle<Standard_Transient> aItem;
  data->ReadEntity(num, 3, "item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  // Own field : fill_style_target
  occ::handle<StepGeom_GeometricRepresentationItem> aFillStyleTarget;
  data->ReadEntity(num,
                   4,
                   "item",
                   ach,
                   STANDARD_TYPE(StepGeom_GeometricRepresentationItem),
                   aFillStyleTarget);

  // Initialisation of the read entity
  ent->Init(aName, aStyles, aItem, aFillStyleTarget);
}

//=================================================================================================

void RWStepVisual_RWAnnotationFillAreaOccurrence::WriteStep(
  StepData_StepWriter&                                   SW,
  const occ::handle<StepVisual_AnnotationFillAreaOccurrence>& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Inherited field : styles
  SW.OpenSub();
  for (int i = 1; i <= ent->NbStyles(); i++)
  {
    SW.Send(ent->StylesValue(i));
  }
  SW.CloseSub();

  // Inherited field : item
  SW.Send(ent->Item());

  // Own field: fill_area_target
  SW.Send(ent->FillStyleTarget());
}

//=================================================================================================

void RWStepVisual_RWAnnotationFillAreaOccurrence::Share(
  const occ::handle<StepVisual_AnnotationFillAreaOccurrence>& ent,
  Interface_EntityIterator&                              iter) const
{
  int nbElem1 = ent->NbStyles();
  for (int i = 1; i <= nbElem1; i++)
  {
    iter.GetOneItem(ent->StylesValue(i));
  }
  iter.GetOneItem(ent->Item());
  iter.GetOneItem(ent->FillStyleTarget());
}
