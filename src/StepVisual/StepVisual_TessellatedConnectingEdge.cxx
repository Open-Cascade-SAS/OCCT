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

#include <StepVisual_TessellatedConnectingEdge.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedConnectingEdge, StepVisual_TessellatedEdge)

//=================================================================================================

StepVisual_TessellatedConnectingEdge::StepVisual_TessellatedConnectingEdge() {}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theTessellatedEdge_Coordinates,
  const Standard_Boolean                    theHasTessellatedEdge_GeometricLink,
  const StepVisual_EdgeOrCurve&             theTessellatedEdge_GeometricLink,
  const Handle(TColStd_HArray1OfInteger)&   theTessellatedEdge_LineStrip,
  const StepData_Logical                    theSmooth,
  const Handle(StepVisual_TessellatedFace)& theFace1,
  const Handle(StepVisual_TessellatedFace)& theFace2,
  const Handle(TColStd_HArray1OfInteger)&   theLineStripFace1,
  const Handle(TColStd_HArray1OfInteger)&   theLineStripFace2)
{
  StepVisual_TessellatedEdge::Init(theRepresentationItem_Name,
                                   theTessellatedEdge_Coordinates,
                                   theHasTessellatedEdge_GeometricLink,
                                   theTessellatedEdge_GeometricLink,
                                   theTessellatedEdge_LineStrip);

  mySmooth = theSmooth;

  myFace1 = theFace1;

  myFace2 = theFace2;

  myLineStripFace1 = theLineStripFace1;

  myLineStripFace2 = theLineStripFace2;
}

//=================================================================================================

StepData_Logical StepVisual_TessellatedConnectingEdge::Smooth() const
{
  return mySmooth;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetSmooth(const StepData_Logical theSmooth)
{
  mySmooth = theSmooth;
}

//=================================================================================================

Handle(StepVisual_TessellatedFace) StepVisual_TessellatedConnectingEdge::Face1() const
{
  return myFace1;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetFace1(
  const Handle(StepVisual_TessellatedFace)& theFace1)
{
  myFace1 = theFace1;
}

//=================================================================================================

Handle(StepVisual_TessellatedFace) StepVisual_TessellatedConnectingEdge::Face2() const
{
  return myFace2;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetFace2(
  const Handle(StepVisual_TessellatedFace)& theFace2)
{
  myFace2 = theFace2;
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_TessellatedConnectingEdge::LineStripFace1() const
{
  return myLineStripFace1;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetLineStripFace1(
  const Handle(TColStd_HArray1OfInteger)& theLineStripFace1)
{
  myLineStripFace1 = theLineStripFace1;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedConnectingEdge::NbLineStripFace1() const
{
  if (myLineStripFace1.IsNull())
  {
    return 0;
  }
  return myLineStripFace1->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedConnectingEdge::LineStripFace1Value(
  const Standard_Integer theNum) const
{
  return myLineStripFace1->Value(theNum);
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_TessellatedConnectingEdge::LineStripFace2() const
{
  return myLineStripFace2;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetLineStripFace2(
  const Handle(TColStd_HArray1OfInteger)& theLineStripFace2)
{
  myLineStripFace2 = theLineStripFace2;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedConnectingEdge::NbLineStripFace2() const
{
  if (myLineStripFace2.IsNull())
  {
    return 0;
  }
  return myLineStripFace2->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedConnectingEdge::LineStripFace2Value(
  const Standard_Integer theNum) const
{
  return myLineStripFace2->Value(theNum);
}
