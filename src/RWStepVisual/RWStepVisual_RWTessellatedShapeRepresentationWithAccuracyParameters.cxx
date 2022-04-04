// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters.hxx>
#include <StepVisual_TessellatedShapeRepresentationWithAccuracyParameters.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_RepresentationContext.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters::RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters::ReadStep(
  const Handle(StepData_StepReaderData)& theData,
  const Standard_Integer theNum,
  Handle(Interface_Check)& theCheck,
  const Handle(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theCheck, "tessellated_shape_representation_with_accuracy_parameters"))
  {
    return;
  }

  // Inherited fields of Representation

  Handle(TCollection_HAsciiString) aRepresentation_Name;
  theData->ReadString(theNum, 1, "representation.name", theCheck, aRepresentation_Name);

  Handle(StepRepr_HArray1OfRepresentationItem) aRepresentation_Items;
  Standard_Integer sub2 = 0;
  if (theData->ReadSubList(theNum, 2, "representation.items", theCheck, sub2))
  {
    Standard_Integer nb0 = theData->NbParams(sub2);
    aRepresentation_Items = new StepRepr_HArray1OfRepresentationItem(1, nb0);
    Standard_Integer num2 = sub2;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Handle(StepRepr_RepresentationItem) anIt0;
      theData->ReadEntity(num2, i0, "representation_item", theCheck,
        STANDARD_TYPE(StepRepr_RepresentationItem), anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  Handle(StepRepr_RepresentationContext) aRepresentation_ContextOfItems;
  theData->ReadEntity(theNum, 3, "representation.context_of_items", theCheck,
    STANDARD_TYPE(StepRepr_RepresentationContext), aRepresentation_ContextOfItems);

  // Own fields of TessellatedShapeRepresentationWithAccuracyParameters

  Handle(TColStd_HArray1OfReal) aTessellationAccuracyParameters;
  Standard_Integer sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellation_accuracy_parameters", theCheck, sub4))
  {
    Standard_Integer nb0 = theData->NbParams(sub4);
    aTessellationAccuracyParameters = new TColStd_HArray1OfReal(1, nb0);
    Standard_Integer num2 = sub4;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Standard_Real anIt0;
      theData->ReadReal(num2, i0, "tessellation_accuracy_parameter_item", theCheck, anIt0);
      aTessellationAccuracyParameters->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentation_Name, aRepresentation_Items, aRepresentation_ContextOfItems, aTessellationAccuracyParameters);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters::WriteStep(
  StepData_StepWriter& theSW,
  const Handle(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters)& theEnt) const
{

  // Own fields of Representation

  theSW.Send(theEnt->Name());

  theSW.OpenSub();
  for (Standard_Integer i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    Handle(StepRepr_RepresentationItem) Var0 = theEnt->Items()->Value(i1);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  theSW.Send(theEnt->ContextOfItems());

  // Own fields of TessellatedShapeRepresentationWithAccuracyParameters

  theSW.OpenSub();
  for (Standard_Integer i3 = 1; i3 <= theEnt->TessellationAccuracyParameters()->Length(); i3++)
  {
    Standard_Real Var0 = theEnt->TessellationAccuracyParameters()->Value(i3);
    theSW.Send(Var0);
  }
  theSW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShapeRepresentationWithAccuracyParameters::Share(
  const Handle(StepVisual_TessellatedShapeRepresentationWithAccuracyParameters)&theEnt,
Interface_EntityIterator& theIter) const
{

  // Inherited fields of Representation

  for (Standard_Integer i1 = 1; i1 <= theEnt->StepRepr_Representation::Items()->Length(); i1++)
  {
    Handle(StepRepr_RepresentationItem) Var0 = theEnt->StepRepr_Representation::Items()->Value(i1);
    theIter.AddItem(Var0);
  }

  theIter.AddItem(theEnt->StepRepr_Representation::ContextOfItems());
}
