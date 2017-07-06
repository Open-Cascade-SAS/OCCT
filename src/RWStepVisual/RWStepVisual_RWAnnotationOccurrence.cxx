// Created on: 2015-10-29
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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
#include <RWStepVisual_RWAnnotationOccurrence.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_AnnotationOccurrence.hxx>

//=======================================================================
//function : RWStepVisual_RWAnnotationOccurrence
//purpose  : 
//=======================================================================
RWStepVisual_RWAnnotationOccurrence::RWStepVisual_RWAnnotationOccurrence () {}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationOccurrence::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num,
   Handle(Interface_Check)& ach,
   const Handle(StepVisual_AnnotationOccurrence)& ent) const
{
  // Number of Parameter Control
  if (!data->CheckNbParams(num, 3, ach, "annotation_occurrence")) return;

  // Inherited field : name
  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);
  
  // Inherited field : styles
  Handle(StepVisual_HArray1OfPresentationStyleAssignment) aStyles;
  Handle(StepVisual_PresentationStyleAssignment) anent2;
  Standard_Integer nsub2;
  if (data->ReadSubList (num,2,"styles",ach,nsub2)) {
    Standard_Integer nb2 = data->NbParams(nsub2);
    aStyles = new StepVisual_HArray1OfPresentationStyleAssignment (1, nb2);
    for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
      if (data->ReadEntity (nsub2, i2,"presentation_style_assignment", ach,
          STANDARD_TYPE(StepVisual_PresentationStyleAssignment), anent2))
        aStyles->SetValue(i2, anent2);
    }
  }

  // Inherited field : item
  Handle(Standard_Transient) aItem;
  data->ReadEntity(num, 3,"item", ach, STANDARD_TYPE(Standard_Transient), aItem);

  // Initialisation of the read entity
  ent->Init(aName, aStyles, aItem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationOccurrence::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepVisual_AnnotationOccurrence)& ent) const
{
  // Inherited field : name
  SW.Send(ent->Name());

  // Inherited field : styles

  SW.OpenSub();
  for (Standard_Integer i2 = 1;  i2 <= ent->NbStyles();  i2 ++) {
    SW.Send(ent->StylesValue(i2));
  }
  SW.CloseSub();

  // Inherited field : item

  SW.Send(ent->Item());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================
void RWStepVisual_RWAnnotationOccurrence::Share(const Handle(StepVisual_AnnotationOccurrence)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer nbElem1 = ent->NbStyles();
  for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
    iter.GetOneItem(ent->StylesValue(is1));
  }
  iter.GetOneItem(ent->Item());
}
