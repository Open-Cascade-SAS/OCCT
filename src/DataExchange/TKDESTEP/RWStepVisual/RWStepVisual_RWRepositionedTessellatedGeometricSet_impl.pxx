// Copyright (c) 2022 OPEN CASCADE SAS
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

#include "RWStepVisual_RWRepositionedTessellatedGeometricSet.pxx"

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_RepositionedTessellatedGeometricSet.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

//=================================================================================================

void RWStepVisual_RWRepositionedTessellatedGeometricSet::ReadStep(
  const Handle(StepData_StepReaderData)&                        theData,
  const Standard_Integer                                        theNum,
  Handle(Interface_Check)&                                      theAch,
  const Handle(StepVisual_RepositionedTessellatedGeometricSet)& theEnt) const
{
  Standard_Integer aNum = 0;
  theData->NamedForComplex("REPOSITIONED_TESSELLATED_ITEM", theNum, aNum, theAch);
  if (!theData->CheckNbParams(aNum, 1, theAch, "location"))
    return;
  Handle(StepGeom_Axis2Placement3d) aLocation;
  theData
    ->ReadEntity(aNum, 1, "location", theAch, STANDARD_TYPE(StepGeom_Axis2Placement3d), aLocation);

  theData->NamedForComplex("REPRESENTATION_ITEM", theNum, aNum, theAch);
  if (!theData->CheckNbParams(aNum, 1, theAch, "name"))
    return;
  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(aNum, 1, "name", theAch, aName);

  theData->NamedForComplex("TESSELLATED_GEOMETRIC_SET", theNum, aNum, theAch);
  NCollection_Handle<StepVisual_Array1OfTessellatedItem> anItems;
  Standard_Integer                                       aNSub2;
  if (theData->ReadSubList(aNum, 1, "items", theAch, aNSub2))
  {
    Standard_Integer aNb2 = theData->NbParams(aNSub2);
    anItems               = new StepVisual_Array1OfTessellatedItem(1, aNb2);
    for (Standard_Integer i2 = 1; i2 <= aNb2; i2++)
    {
      Handle(StepVisual_TessellatedItem) anItem;
      if (theData->ReadEntity(aNSub2,
                              i2,
                              "item",
                              theAch,
                              STANDARD_TYPE(StepVisual_TessellatedItem),
                              anItem))
        anItems->SetValue(i2, anItem);
    }
  }
  theEnt->Init(aName, anItems, aLocation);
}

//=================================================================================================

void RWStepVisual_RWRepositionedTessellatedGeometricSet::WriteStep(
  StepData_StepWriter&                                          theSW,
  const Handle(StepVisual_RepositionedTessellatedGeometricSet)& theEnt) const
{
  theSW.StartEntity("GEOMETRIC_REPRESENTATION_ITEM");
  theSW.StartEntity("REPOSITIONED_TESSELLATED_ITEM");
  theSW.Send(theEnt->Location());
  theSW.StartEntity("REPRESENTATION_ITEM");
  theSW.Send(theEnt->Name());
  theSW.StartEntity("TESSELLATED_GEOMETRIC_SET");
  theSW.OpenSub();
  for (StepVisual_Array1OfTessellatedItem::Iterator anIter(*theEnt->Items()); anIter.More();
       anIter.Next())
  {
    theSW.Send(anIter.Value());
  }
  theSW.CloseSub();
  theSW.StartEntity("TESSELLATED_ITEM");
}

//=================================================================================================

void RWStepVisual_RWRepositionedTessellatedGeometricSet::Share(
  const Handle(StepVisual_RepositionedTessellatedGeometricSet)& theEnt,
  Interface_EntityIterator&                                     theIter) const
{
  // Own field : children
  for (Standard_Integer i = 1; i <= theEnt->Items()->Length(); i++)
    theIter.AddItem(theEnt->Items()->Value(i));
  theIter.AddItem(theEnt->Location());
}
