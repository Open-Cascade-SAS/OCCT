// Created on: 2017-02-06
// Created by: Irina KRYLOVA
// Copyright (c) 2017 OPEN CASCADE SAS
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
#include "RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_AnnotationCurveOccurrenceAndGeomReprItem.hxx>

//=================================================================================================

RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::
  RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem() = default;

//=================================================================================================

void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::ReadStep(
  const occ::handle<StepData_StepReaderData>&                             data,
  const int                                                               num0,
  occ::handle<Interface_Check>&                                           ach,
  const occ::handle<StepVisual_AnnotationCurveOccurrenceAndGeomReprItem>& ent) const
{
  int num = 0;
  data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM", num0, num, ach);
  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 1, "name", ach, aName);

  data->NamedForComplex("STYLED_ITEM", "STYITM", num0, num, ach);
  // Inherited field : styles
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aStyles;
  occ::handle<StepVisual_PresentationStyleAssignment>                                   anEnt;
  int                                                                                   nsub;
  if (data->ReadSubList(num, 1, "styles", ach, nsub))
  {
    int nb  = data->NbParams(nsub);
    aStyles = new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      if (data->ReadEntity(nsub,
                           i,
                           "presentation_style_assignment",
                           ach,
                           STANDARD_TYPE(StepVisual_PresentationStyleAssignment),
                           anEnt))
        aStyles->SetValue(i, anEnt);
    }
  }

  // Inherited field : item
  occ::handle<Standard_Transient> aItem;
  data->ReadEntity(num, 2, "item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  // Initialization of the read entity
  ent->Init(aName, aStyles, aItem);
}

//=================================================================================================

void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::WriteStep(
  StepData_StepWriter&                                                    SW,
  const occ::handle<StepVisual_AnnotationCurveOccurrenceAndGeomReprItem>& ent) const
{
  SW.StartEntity("ANNOTATION_CURVE_OCCURRENCE");
  SW.StartEntity("ANNOTATION_OCCURRENCE");
  SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");
  SW.StartEntity("REPRESENTATION_ITEM");
  // Inherited field : name
  SW.Send(ent->Name());

  SW.StartEntity("STYLED_ITEM");
  // Inherited field : styles
  SW.OpenSub();
  for (int i = 1; i <= ent->NbStyles(); i++)
  {
    SW.Send(ent->StylesValue(i));
  }
  SW.CloseSub();

  // Inherited field : item
  SW.Send(ent->Item());
}

//=================================================================================================

void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::Share(
  const occ::handle<StepVisual_AnnotationCurveOccurrenceAndGeomReprItem>& ent,
  Interface_EntityIterator&                                               iter) const
{

  int nbElem = ent->NbStyles();
  for (int i = 1; i <= nbElem; i++)
  {
    iter.GetOneItem(ent->StylesValue(i));
  }

  iter.GetOneItem(ent->Item());
}
