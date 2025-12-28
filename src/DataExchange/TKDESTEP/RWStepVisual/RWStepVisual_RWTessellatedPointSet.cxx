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

#include "RWStepVisual_RWTessellatedPointSet.pxx"
#include <StepVisual_TessellatedPointSet.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>

//=================================================================================================

RWStepVisual_RWTessellatedPointSet::RWStepVisual_RWTessellatedPointSet() {}

//=================================================================================================

void RWStepVisual_RWTessellatedPointSet::ReadStep(
  const occ::handle<StepData_StepReaderData>&        theData,
  const int                        theNum,
  occ::handle<Interface_Check>&                      theCheck,
  const occ::handle<StepVisual_TessellatedPointSet>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theCheck, "tessellated_point_set"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedPointSet

  occ::handle<StepVisual_CoordinatesList> aCoordinates;
  theData->ReadEntity(theNum,
                      2,
                      "coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aCoordinates);

  occ::handle<NCollection_HArray1<int>> aPointList;
  int                 sub3 = 0;
  if (theData->ReadSubList(theNum, 3, "point_list", theCheck, sub3))
  {
    int nb0  = theData->NbParams(sub3);
    aPointList            = new NCollection_HArray1<int>(1, nb0);
    int num2 = sub3;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aPointList->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aCoordinates, aPointList);
}

//=================================================================================================

void RWStepVisual_RWTessellatedPointSet::WriteStep(
  StepData_StepWriter&                          theSW,
  const occ::handle<StepVisual_TessellatedPointSet>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedPointSet

  theSW.Send(theEnt->Coordinates());

  theSW.OpenSub();
  for (int i2 = 1; i2 <= theEnt->PointList()->Length(); i2++)
  {
    int Var0 = theEnt->PointList()->Value(i2);
    theSW.Send(Var0);
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWTessellatedPointSet::Share(const occ::handle<StepVisual_TessellatedPointSet>& theEnt,
                                               Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedPointSet

  theIter.AddItem(theEnt->Coordinates());
}
