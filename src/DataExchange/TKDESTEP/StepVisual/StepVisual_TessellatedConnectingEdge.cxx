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
  const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
  const occ::handle<StepVisual_CoordinatesList>& theTessellatedEdge_Coordinates,
  const bool                    theHasTessellatedEdge_GeometricLink,
  const StepVisual_EdgeOrCurve&             theTessellatedEdge_GeometricLink,
  const occ::handle<NCollection_HArray1<int>>&   theTessellatedEdge_LineStrip,
  const StepData_Logical                    theSmooth,
  const occ::handle<StepVisual_TessellatedFace>& theFace1,
  const occ::handle<StepVisual_TessellatedFace>& theFace2,
  const occ::handle<NCollection_HArray1<int>>&   theLineStripFace1,
  const occ::handle<NCollection_HArray1<int>>&   theLineStripFace2)
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

occ::handle<StepVisual_TessellatedFace> StepVisual_TessellatedConnectingEdge::Face1() const
{
  return myFace1;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetFace1(
  const occ::handle<StepVisual_TessellatedFace>& theFace1)
{
  myFace1 = theFace1;
}

//=================================================================================================

occ::handle<StepVisual_TessellatedFace> StepVisual_TessellatedConnectingEdge::Face2() const
{
  return myFace2;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetFace2(
  const occ::handle<StepVisual_TessellatedFace>& theFace2)
{
  myFace2 = theFace2;
}

//=================================================================================================

occ::handle<NCollection_HArray1<int>> StepVisual_TessellatedConnectingEdge::LineStripFace1() const
{
  return myLineStripFace1;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetLineStripFace1(
  const occ::handle<NCollection_HArray1<int>>& theLineStripFace1)
{
  myLineStripFace1 = theLineStripFace1;
}

//=================================================================================================

int StepVisual_TessellatedConnectingEdge::NbLineStripFace1() const
{
  if (myLineStripFace1.IsNull())
  {
    return 0;
  }
  return myLineStripFace1->Length();
}

//=================================================================================================

int StepVisual_TessellatedConnectingEdge::LineStripFace1Value(
  const int theNum) const
{
  return myLineStripFace1->Value(theNum);
}

//=================================================================================================

occ::handle<NCollection_HArray1<int>> StepVisual_TessellatedConnectingEdge::LineStripFace2() const
{
  return myLineStripFace2;
}

//=================================================================================================

void StepVisual_TessellatedConnectingEdge::SetLineStripFace2(
  const occ::handle<NCollection_HArray1<int>>& theLineStripFace2)
{
  myLineStripFace2 = theLineStripFace2;
}

//=================================================================================================

int StepVisual_TessellatedConnectingEdge::NbLineStripFace2() const
{
  if (myLineStripFace2.IsNull())
  {
    return 0;
  }
  return myLineStripFace2->Length();
}

//=================================================================================================

int StepVisual_TessellatedConnectingEdge::LineStripFace2Value(
  const int theNum) const
{
  return myLineStripFace2->Value(theNum);
}
