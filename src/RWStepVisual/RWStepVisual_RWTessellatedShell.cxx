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

#include <RWStepVisual_RWTessellatedShell.hxx>
#include <StepVisual_TessellatedShell.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_HArray1OfTessellatedStructuredItem.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>
#include <StepShape_ConnectedFaceSet.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedShell
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedShell::RWStepVisual_RWTessellatedShell() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShell::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                const Standard_Integer theNum,
                                                Handle(Interface_Check)& theCheck,
                                                const Handle(StepVisual_TessellatedShell)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theCheck, "tessellated_shell"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedShell

  Handle(StepVisual_HArray1OfTessellatedStructuredItem) aItems;
  Standard_Integer sub2 = 0;
  if (theData->ReadSubList(theNum, 2, "items", theCheck, sub2))
  {
    Standard_Integer nb0 = theData->NbParams(sub2);
    aItems = new StepVisual_HArray1OfTessellatedStructuredItem(1, nb0);
    Standard_Integer num2 = sub2;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Handle(StepVisual_TessellatedStructuredItem) anIt0;
      theData->ReadEntity(num2, i0, "tessellated_structured_item", theCheck,
        STANDARD_TYPE(StepVisual_TessellatedStructuredItem), anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  Handle(StepShape_ConnectedFaceSet) aTopologicalLink;
  Standard_Boolean hasTopologicalLink = Standard_True;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum, 3, "topological_link", theCheck,
      STANDARD_TYPE(StepShape_ConnectedFaceSet), aTopologicalLink);
  }
  else
  {
    hasTopologicalLink = Standard_False;
    aTopologicalLink.Nullify();
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aItems, hasTopologicalLink, aTopologicalLink);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShell::WriteStep (StepData_StepWriter& theSW,
                                                 const Handle(StepVisual_TessellatedShell)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedShell

  theSW.OpenSub();
  for (Standard_Integer i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    Handle(StepVisual_TessellatedStructuredItem) Var0 = theEnt->Items()->Value(i1);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  if (theEnt->HasTopologicalLink())
  {
    theSW.Send(theEnt->TopologicalLink());
  }
  else
  {
    theSW.SendUndef();
  }
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedShell::Share (const Handle(StepVisual_TessellatedShell)&theEnt,
                                             Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedShell

  for (Standard_Integer i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    Handle(StepVisual_TessellatedStructuredItem) Var0 = theEnt->Items()->Value(i1);
    theIter.AddItem(Var0);
  }

  if (theEnt->HasTopologicalLink())
  {
    theIter.AddItem(theEnt->TopologicalLink());
  }
}
