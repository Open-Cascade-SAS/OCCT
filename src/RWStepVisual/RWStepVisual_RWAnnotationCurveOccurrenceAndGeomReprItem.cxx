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
#include <RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_AnnotationCurveOccurrenceAndGeomReprItem.hxx>

//=======================================================================
//function : RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem
//purpose  : 
//=======================================================================
RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::
  RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem() {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::ReadStep
(const Handle(StepData_StepReaderData)& data,
const Standard_Integer num0,
Handle(Interface_Check)& ach,
const Handle(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem)& ent) const
{
  Standard_Integer num = 0;
  data->NamedForComplex("REPRESENTATION_ITEM", "RPRITM", num0, num, ach);
  // Inherited field : name 
  Handle(TCollection_HAsciiString) aName;
  data->ReadString(num, 1, "name", ach, aName);

  data->NamedForComplex("STYLED_ITEM", "STYITM", num0, num, ach);
  // Inherited field : styles
  Handle(StepVisual_HArray1OfPresentationStyleAssignment) aStyles;
  Handle(StepVisual_PresentationStyleAssignment) anEnt;
  Standard_Integer nsub;
  if (data->ReadSubList(num, 1, "styles", ach, nsub)) {
    Standard_Integer nb = data->NbParams(nsub);
    aStyles = new StepVisual_HArray1OfPresentationStyleAssignment(1, nb);
    for (Standard_Integer i = 1; i <= nb; i++) {
      if (data->ReadEntity(nsub, i, "presentation_style_assignment", ach,
        STANDARD_TYPE(StepVisual_PresentationStyleAssignment), anEnt))
        aStyles->SetValue(i, anEnt);
    }
  }

  // Inherited field : item
  Handle(Standard_Transient) aItem;
  data->ReadEntity(num, 2, "item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  // Initialization of the read entity
  ent->Init(aName, aStyles, aItem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::WriteStep
(StepData_StepWriter& SW,
const Handle(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem)& ent) const
{
  SW.StartEntity("ANNOTATION_CURVE_OCCURRENCE");
  SW.StartEntity("ANNOTATION_OCCURRENCE");
  SW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");
  SW.StartEntity("REPRESENTATION_ITEM");
  //Inherited field : name
  SW.Send(ent->Name());

  SW.StartEntity("STYLED_ITEM");
  // Inherited field : styles
  SW.OpenSub();
  for (Standard_Integer i = 1; i <= ent->NbStyles(); i++) {
    SW.Send(ent->StylesValue(i));
  }
  SW.CloseSub();

  // Inherited field : item
  SW.Send(ent->Item());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationCurveOccurrenceAndGeomReprItem::Share(
  const Handle(StepVisual_AnnotationCurveOccurrenceAndGeomReprItem)& ent,
  Interface_EntityIterator& iter) const
{

  Standard_Integer nbElem = ent->NbStyles();
  for (Standard_Integer i = 1; i <= nbElem; i++) {
    iter.GetOneItem(ent->StylesValue(i));
  }

  iter.GetOneItem(ent->Item());
}
