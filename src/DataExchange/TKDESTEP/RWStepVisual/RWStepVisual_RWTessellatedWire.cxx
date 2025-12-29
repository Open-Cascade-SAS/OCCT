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

#include "RWStepVisual_RWTessellatedWire.pxx"
#include <StepVisual_TessellatedWire.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_TessellatedEdgeOrVertex.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_PathOrCompositeCurve.hxx>

//=================================================================================================

RWStepVisual_RWTessellatedWire::RWStepVisual_RWTessellatedWire() = default;

//=================================================================================================

void RWStepVisual_RWTessellatedWire::ReadStep(
  const occ::handle<StepData_StepReaderData>&    theData,
  const int                                      theNum,
  occ::handle<Interface_Check>&                  theCheck,
  const occ::handle<StepVisual_TessellatedWire>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theCheck, "tessellated_wire"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Own fields of TessellatedWire

  occ::handle<NCollection_HArray1<StepVisual_TessellatedEdgeOrVertex>> aItems;
  int                                                                  sub2 = 0;
  if (theData->ReadSubList(theNum, 2, "items", theCheck, sub2))
  {
    int nb0  = theData->NbParams(sub2);
    aItems   = new NCollection_HArray1<StepVisual_TessellatedEdgeOrVertex>(1, nb0);
    int num2 = sub2;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      StepVisual_TessellatedEdgeOrVertex anIt0;
      theData->ReadEntity(num2, i0, "tessellated_edge_or_vertex", theCheck, anIt0);
      aItems->SetValue(i0, anIt0);
    }
  }

  StepVisual_PathOrCompositeCurve aGeometricModelLink;
  bool                            hasGeometricModelLink = true;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum, 3, "geometric_model_link", theCheck, aGeometricModelLink);
  }
  else
  {
    hasGeometricModelLink = false;
    aGeometricModelLink   = StepVisual_PathOrCompositeCurve();
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aItems, hasGeometricModelLink, aGeometricModelLink);
}

//=================================================================================================

void RWStepVisual_RWTessellatedWire::WriteStep(
  StepData_StepWriter&                           theSW,
  const occ::handle<StepVisual_TessellatedWire>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of TessellatedWire

  theSW.OpenSub();
  for (int i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    StepVisual_TessellatedEdgeOrVertex Var0 = theEnt->Items()->Value(i1);
    theSW.Send(Var0.Value());
  }
  theSW.CloseSub();

  if (theEnt->HasGeometricModelLink())
  {
    theSW.Send(theEnt->GeometricModelLink().Value());
  }
  else
  {
    theSW.SendUndef();
  }
}

//=================================================================================================

void RWStepVisual_RWTessellatedWire::Share(const occ::handle<StepVisual_TessellatedWire>& theEnt,
                                           Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of TessellatedWire

  for (int i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    StepVisual_TessellatedEdgeOrVertex Var0 = theEnt->Items()->Value(i1);
    theIter.AddItem(Var0.Value());
  }

  if (theEnt->HasGeometricModelLink())
  {
    theIter.AddItem(theEnt->GeometricModelLink().Value());
  }
}
