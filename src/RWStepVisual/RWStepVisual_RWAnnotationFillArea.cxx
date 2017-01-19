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
#include <RWStepVisual_RWAnnotationFillArea.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_AnnotationFillArea.hxx>
#include <StepShape_GeometricSetSelect.hxx>
#include <StepShape_HArray1OfGeometricSetSelect.hxx>

//=======================================================================
//function : RWStepVisual_RWAnnotationFillArea
//purpose  : 
//=======================================================================
RWStepVisual_RWAnnotationFillArea::RWStepVisual_RWAnnotationFillArea () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationFillArea::ReadStep
(const Handle(StepData_StepReaderData)& data,
const Standard_Integer num,
Handle(Interface_Check)& ach,
const Handle(StepVisual_AnnotationFillArea)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 2, ach, "annotation_fill_area"))
    return;

  // Inherited field : name

  Handle(TCollection_HAsciiString) aName;
  data->ReadString(num, 1, "name", ach, aName);

  // Own field : boundaries
  Handle(StepShape_HArray1OfGeometricSetSelect) aElements;
  StepShape_GeometricSetSelect aElementsItem;
  Standard_Integer nsub;
  if (data->ReadSubList(num, 2, "boundaries", ach, nsub)) {
    Standard_Integer nb = data->NbParams(nsub);
    aElements = new StepShape_HArray1OfGeometricSetSelect(1, nb);
    for (Standard_Integer i = 1; i <= nb; i++) {
      if (data->ReadEntity(nsub, i, "boundaries", ach, aElementsItem))
        aElements->SetValue(i, aElementsItem);
    }
  }

  //Initialization of the read entity
  ent->Init(aName, aElements);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationFillArea::WriteStep
(StepData_StepWriter& SW,
const Handle(StepVisual_AnnotationFillArea)& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Own field : elements
  SW.OpenSub();
  for (Standard_Integer i = 1; i <= ent->NbElements(); i++) {
    SW.Send(ent->ElementsValue(i).Value());
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationFillArea::Share(const Handle(StepVisual_AnnotationFillArea)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nbBound = ent->NbElements();
  for (Standard_Integer i = 1; i <= nbBound; i++) {
    iter.GetOneItem(ent->ElementsValue(i).Value());
  }
}

