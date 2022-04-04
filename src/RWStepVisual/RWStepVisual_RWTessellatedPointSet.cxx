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

#include <RWStepVisual_RWTessellatedPointSet.hxx>
#include <StepVisual_TessellatedPointSet.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedPointSet
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedPointSet::RWStepVisual_RWTessellatedPointSet() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedPointSet::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                   const Standard_Integer theNum,
                                                   Handle(Interface_Check)& theCheck,
                                                   const Handle(StepVisual_TessellatedPointSet)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theCheck, "tessellated_point_set"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedPointSet

  Handle(StepVisual_CoordinatesList) aCoordinates;
  theData->ReadEntity(theNum, 2, "coordinates", theCheck,
    STANDARD_TYPE(StepVisual_CoordinatesList), aCoordinates);

  Handle(TColStd_HArray1OfInteger) aPointList;
  Standard_Integer sub3 = 0;
  if (theData->ReadSubList(theNum, 3, "point_list", theCheck, sub3))
  {
    Standard_Integer nb0 = theData->NbParams(sub3);
    aPointList = new TColStd_HArray1OfInteger(1, nb0);
    Standard_Integer num2 = sub3;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Standard_Integer anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aPointList->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aCoordinates, aPointList);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedPointSet::WriteStep (StepData_StepWriter& theSW,
                                                    const Handle(StepVisual_TessellatedPointSet)& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedPointSet

  theSW.Send(theEnt->Coordinates());

  theSW.OpenSub();
  for (Standard_Integer i2 = 1; i2 <= theEnt->PointList()->Length(); i2++)
  {
    Standard_Integer Var0 = theEnt->PointList()->Value(i2);
    theSW.Send(Var0);
  }
  theSW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedPointSet::Share (const Handle(StepVisual_TessellatedPointSet)&theEnt,
                                                Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedPointSet

  theIter.AddItem(theEnt->Coordinates());
}
