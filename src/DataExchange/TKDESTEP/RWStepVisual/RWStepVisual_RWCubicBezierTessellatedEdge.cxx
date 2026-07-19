// Created on : Thu Mar 24 18:30:11 2022
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

#include "RWStepVisual_RWCubicBezierTessellatedEdge.pxx"
#include <StepVisual_CubicBezierTessellatedEdge.hxx>
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

RWStepVisual_RWCubicBezierTessellatedEdge::RWStepVisual_RWCubicBezierTessellatedEdge() = default;

//=================================================================================================

void RWStepVisual_RWCubicBezierTessellatedEdge::ReadStep(
  const occ::handle<StepData_StepReaderData>&               theData,
  const int                                                 theNum,
  occ::handle<Interface_Check>&                             theCheck,
  const occ::handle<StepVisual_CubicBezierTessellatedEdge>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theCheck, "cubic_bezier_tessellated_edge"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Inherited fields of TessellatedEdge

  occ::handle<StepVisual_CoordinatesList> aTessellatedEdge_Coordinates;
  theData->ReadEntity(theNum,
                      2,
                      "tessellated_edge.coordinates",
                      theCheck,
                      STANDARD_TYPE(StepVisual_CoordinatesList),
                      aTessellatedEdge_Coordinates);

  StepVisual_EdgeOrCurve aTessellatedEdge_GeometricLink;
  bool                   hasTessellatedEdge_GeometricLink = true;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum,
                        3,
                        "tessellated_edge.geometric_link",
                        theCheck,
                        aTessellatedEdge_GeometricLink);
  }
  else
  {
    hasTessellatedEdge_GeometricLink = false;
    aTessellatedEdge_GeometricLink   = StepVisual_EdgeOrCurve();
  }

  occ::handle<NCollection_HArray1<int>> aTessellatedEdge_LineStrip;
  int                                   sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_edge.line_strip", theCheck, sub4))
  {
    int nb0                    = theData->NbParams(sub4);
    aTessellatedEdge_LineStrip = new NCollection_HArray1<int>(1, nb0);
    int num2                   = sub4;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      int anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aTessellatedEdge_LineStrip->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name,
               aTessellatedEdge_Coordinates,
               hasTessellatedEdge_GeometricLink,
               aTessellatedEdge_GeometricLink,
               aTessellatedEdge_LineStrip);
}

//=================================================================================================

void RWStepVisual_RWCubicBezierTessellatedEdge::WriteStep(
  StepData_StepWriter&                                      theSW,
  const occ::handle<StepVisual_CubicBezierTessellatedEdge>& theEnt) const
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

void RWStepVisual_RWCubicBezierTessellatedEdge::Share(
  const occ::handle<StepVisual_CubicBezierTessellatedEdge>& theEnt,
  Interface_EntityIterator&                                 theIter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of TessellatedEdge

  theIter.AddItem(theEnt->StepVisual_TessellatedEdge::Coordinates());

  if (theEnt->StepVisual_TessellatedEdge::HasGeometricLink())
  {
    theIter.AddItem(theEnt->StepVisual_TessellatedEdge::GeometricLink().Value());
  }
}
