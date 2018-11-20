// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <RWStepShape_RWShapeDimensionRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_HArray1OfShapeDimensionRepresentationItem.hxx>
#include <StepShape_ShapeDimensionRepresentation.hxx>

//=======================================================================
//function : RWStepShape_RWShapeDimensionRepresentation
//purpose  : 
//=======================================================================
RWStepShape_RWShapeDimensionRepresentation::RWStepShape_RWShapeDimensionRepresentation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDimensionRepresentation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                           const Standard_Integer num,
                                                           Handle(Interface_Check)& ach,
                                                           const Handle(StepShape_ShapeDimensionRepresentation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"shape_dimension_representation") ) return;

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  data->ReadString (num, 1, "representation.name", ach, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Handle(StepShape_HArray1OfShapeDimensionRepresentationItem) anItems;
  Standard_Integer sub2 = 0;
  if ( data->ReadSubList (num, 2, "representation.items", ach, sub2) ) {
    Standard_Integer num2 = sub2;
    Standard_Integer nb0 = data->NbParams(num2);
    Handle(StepRepr_RepresentationItem) anIt0;
    StepShape_ShapeDimensionRepresentationItem anIt0AP242;
    if (data->ReadEntity (num2, 1, "representation.items", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anIt0)) {
      aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem (1, nb0);
      for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
        data->ReadEntity (num2, i0, "representation.items", ach, STANDARD_TYPE(StepRepr_RepresentationItem), anIt0);
        aRepresentation_Items->SetValue(i0, anIt0);
      }
    }
    else {
      anItems = new StepShape_HArray1OfShapeDimensionRepresentationItem (1, nb0);
      for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
        data->ReadEntity (num2, i0, "representation.items", ach, anIt0AP242);
        anItems->SetValue(i0, anIt0AP242);
      }
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  data->ReadEntity (num, 3, "representation.context_of_items", ach, STANDARD_TYPE(StepRepr_RepresentationContext), aRepresentation_ContextOfItems);

  // Initialize entity
  if (anItems.IsNull()) {
    ent->Init(aRepresentation_Name,
              aRepresentation_Items,
              aRepresentation_ContextOfItems);
  }
  else {
    ent->Init(aRepresentation_Name,
              anItems,
              aRepresentation_ContextOfItems);
  }
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDimensionRepresentation::WriteStep (StepData_StepWriter& SW,
                                                            const Handle(StepShape_ShapeDimensionRepresentation) &ent) const
{

  // Inherited fields of Representation

  SW.Send (ent->StepRepr_Representation::Name());

  SW.OpenSub();
  if (ent->ItemsAP242().IsNull()) {
    for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::NbItems(); i1++ ) {
      Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
      SW.Send (Var0);
    }
  }
  else {
    for (Standard_Integer i1=1; i1 <= ent->ItemsAP242()->Length(); i1++ ) {
      StepShape_ShapeDimensionRepresentationItem Var0 = ent->ItemsAP242()->Value(i1);
      SW.Send (Var0.Value());
    }
  }
  SW.CloseSub();

  SW.Send (ent->StepRepr_Representation::ContextOfItems());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWShapeDimensionRepresentation::Share (const Handle(StepShape_ShapeDimensionRepresentation) &ent,
                                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of Representation

  if (ent->ItemsAP242().IsNull()) {
    for (Standard_Integer i1=1; i1 <= ent->StepRepr_Representation::NbItems(); i1++ ) {
      Handle(StepRepr_RepresentationItem) Var0 = ent->StepRepr_Representation::Items()->Value(i1);
      iter.AddItem (Var0);
    }
  }
  else {
    for (Standard_Integer i1=1; i1 <= ent->ItemsAP242()->Length(); i1++ ) {
      StepShape_ShapeDimensionRepresentationItem Var0 = ent->ItemsAP242()->Value(i1);
      iter.AddItem (Var0.Value());
    }
  }

  iter.AddItem (ent->StepRepr_Representation::ContextOfItems());
}
