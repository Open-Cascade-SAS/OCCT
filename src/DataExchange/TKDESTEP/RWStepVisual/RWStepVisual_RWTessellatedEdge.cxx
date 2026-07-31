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

#include "RWStepVisual_RWTessellatedEdge.pxx"
#include <StepVisual_TessellatedEdge.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_EdgeOrCurve.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

RWStepVisual_RWTessellatedEdge::RWStepVisual_RWTessellatedEdge() = default;

//=================================================================================================

void RWStepVisual_RWTessellatedEdge::ReadStep(
  const occ::handle<StepData_StepReaderData>&    theData,
  const int                                      theNum,
  occ::handle<Interface_Check>&                  theCheck,
  const occ::handle<StepVisual_TessellatedEdge>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theCheck, "tessellated_edge"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedEdge

  occ::handle<StepVisual_CoordinatesList> aCoordinates;
  theData->ReadEntity(theNum,
                      2,
                      "coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aCoordinates);

  StepVisual_EdgeOrCurve aGeometricLink;
  bool                   hasGeometricLink = true;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum, 3, "geometric_link", theCheck, aGeometricLink);
  }
  else
  {
    hasGeometricLink = false;
    aGeometricLink   = StepVisual_EdgeOrCurve();
  }

  occ::handle<NCollection_HArray1<int>> aLineStrip;
  int                                   sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "line_strip", theCheck, sub4))
  {
    int nb0    = theData->NbParams(sub4);
    aLineStrip = new NCollection_HArray1<int>(1, nb0);
    int num2   = sub4;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aLineStrip->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aCoordinates,
               hasGeometricLink,
               aGeometricLink,
               aLineStrip);
}

//=================================================================================================

void RWStepVisual_RWTessellatedEdge::WriteStep(
  StepData_StepWriter&                           theSW,
  const occ::handle<StepVisual_TessellatedEdge>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedEdge

  theSW.Send(theEnt->Coordinates());

  if (theEnt->HasGeometricLink())
  {
    theSW.Send(theEnt->GeometricLink().Value());
  }
  else
  {
    theSW.SendUndef();
  }

  theSW.OpenSub();
  for (int i3 = 1; i3 <= theEnt->LineStrip()->Length(); i3++)
  {
    int Var0 = theEnt->LineStrip()->Value(i3);
    theSW.Send(Var0);
  }
  theSW.CloseSub();
}

//=================================================================================================

void RWStepVisual_RWTessellatedEdge::Share(const occ::handle<StepVisual_TessellatedEdge>& theEnt,
                                           Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedEdge

  theIter.AddItem(theEnt->Coordinates());

  if (theEnt->HasGeometricLink())
  {
    theIter.AddItem(theEnt->GeometricLink().Value());
  }
}
