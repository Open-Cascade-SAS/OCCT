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

#include <RWStepVisual_RWTessellatedConnectingEdge.hxx>
#include <StepVisual_TessellatedConnectingEdge.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_EdgeOrCurve.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <StepData_Logical.hxx>
#include <StepVisual_TessellatedFace.hxx>

//=======================================================================
//function : RWStepVisual_RWTessellatedConnectingEdge
//purpose  : 
//=======================================================================

RWStepVisual_RWTessellatedConnectingEdge::RWStepVisual_RWTessellatedConnectingEdge() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedConnectingEdge::ReadStep(
  const Handle(StepData_StepReaderData)& theData,
  const Standard_Integer theNum,
  Handle(Interface_Check)& theCheck,
  const Handle(StepVisual_TessellatedConnectingEdge)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 9, theCheck, "tessellated_connecting_edge"))
  {
    return;
  }

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theCheck, aRepresentationItem_Name);

  // Inherited fields of TessellatedEdge

  Handle(StepVisual_CoordinatesList) aTessellatedEdge_Coordinates;
  theData->ReadEntity(theNum, 2, "tessellated_edge.coordinates", theCheck,
    STANDARD_TYPE(StepVisual_CoordinatesList), aTessellatedEdge_Coordinates);

  StepVisual_EdgeOrCurve aTessellatedEdge_GeometricLink;
  Standard_Boolean hasTessellatedEdge_GeometricLink = Standard_True;
  if (theData->IsParamDefined(theNum, 3))
  {
    theData->ReadEntity(theNum, 3, "tessellated_edge.geometric_link", theCheck, aTessellatedEdge_GeometricLink);
  }
  else
  {
    hasTessellatedEdge_GeometricLink = Standard_False;
    aTessellatedEdge_GeometricLink = StepVisual_EdgeOrCurve();
  }

  Handle(TColStd_HArray1OfInteger) aTessellatedEdge_LineStrip;
  Standard_Integer sub4 = 0;
  if (theData->ReadSubList(theNum, 4, "tessellated_edge.line_strip", theCheck, sub4))
  {
    Standard_Integer nb0 = theData->NbParams(sub4);
    aTessellatedEdge_LineStrip = new TColStd_HArray1OfInteger(1, nb0);
    Standard_Integer num2 = sub4;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Standard_Integer anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aTessellatedEdge_LineStrip->SetValue(i0, anIt0);
    }
  }

  // Own fields of TessellatedConnectingEdge

  StepData_Logical aSmooth;
  theData->ReadLogical(theNum, 5, "smooth", theCheck, aSmooth);

  Handle(StepVisual_TessellatedFace) aFace1;
  theData->ReadEntity(theNum, 6, "face1", theCheck,
    STANDARD_TYPE(StepVisual_TessellatedFace), aFace1);

  Handle(StepVisual_TessellatedFace) aFace2;
  theData->ReadEntity(theNum, 7, "face2", theCheck,
    STANDARD_TYPE(StepVisual_TessellatedFace), aFace2);

  Handle(TColStd_HArray1OfInteger) aLineStripFace1;
  Standard_Integer sub8 = 0;
  if (theData->ReadSubList(theNum, 8, "line_strip_face1", theCheck, sub8))
  {
    Standard_Integer nb0 = theData->NbParams(sub8);
    aLineStripFace1 = new TColStd_HArray1OfInteger(1, nb0);
    Standard_Integer num2 = sub8;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Standard_Integer anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aLineStripFace1->SetValue(i0, anIt0);
    }
  }

  Handle(TColStd_HArray1OfInteger) aLineStripFace2;
  Standard_Integer sub9 = 0;
  if (theData->ReadSubList(theNum, 9, "line_strip_face2", theCheck, sub9))
  {
    Standard_Integer nb0 = theData->NbParams(sub9);
    aLineStripFace2 = new TColStd_HArray1OfInteger(1, nb0);
    Standard_Integer num2 = sub9;
    for (Standard_Integer i0 = 1; i0 <= nb0; i0++)
    {
      Standard_Integer anIt0;
      theData->ReadInteger(num2, i0, "integer", theCheck, anIt0);
      aLineStripFace2->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aTessellatedEdge_Coordinates, hasTessellatedEdge_GeometricLink, aTessellatedEdge_GeometricLink, aTessellatedEdge_LineStrip, aSmooth, aFace1, aFace2, aLineStripFace1, aLineStripFace2);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedConnectingEdge::WriteStep(
  StepData_StepWriter& theSW,
  const Handle(StepVisual_TessellatedConnectingEdge)& theEnt) const
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
  for (Standard_Integer i3 = 1; i3 <= theEnt->LineStrip()->Length(); i3++)
  {
    Standard_Integer Var0 = theEnt->LineStrip()->Value(i3);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  // Own fields of TessellatedConnectingEdge

  theSW.SendLogical(theEnt->Smooth());

  theSW.Send(theEnt->Face1());

  theSW.Send(theEnt->Face2());

  theSW.OpenSub();
  for (Standard_Integer i7 = 1; i7 <= theEnt->LineStripFace1()->Length(); i7++)
  {
    Standard_Integer Var0 = theEnt->LineStripFace1()->Value(i7);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  theSW.OpenSub();
  for (Standard_Integer i8 = 1; i8 <= theEnt->LineStripFace2()->Length(); i8++)
  {
    Standard_Integer Var0 = theEnt->LineStripFace2()->Value(i8);
    theSW.Send(Var0);
  }
  theSW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepVisual_RWTessellatedConnectingEdge::Share(
  const Handle(StepVisual_TessellatedConnectingEdge)&theEnt,
Interface_EntityIterator& theIter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of TessellatedEdge

  theIter.AddItem(theEnt->StepVisual_TessellatedEdge::Coordinates());

  if (theEnt->StepVisual_TessellatedEdge::HasGeometricLink())
  {
    theIter.AddItem(theEnt->StepVisual_TessellatedEdge::GeometricLink().Value());
  }

  // Own fields of TessellatedConnectingEdge

  theIter.AddItem(theEnt->Face1());

  theIter.AddItem(theEnt->Face2());
}
