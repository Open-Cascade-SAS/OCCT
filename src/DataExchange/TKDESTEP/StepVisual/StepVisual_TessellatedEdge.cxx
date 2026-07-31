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

#include <StepVisual_TessellatedEdge.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedEdge, StepVisual_TessellatedStructuredItem)

//=================================================================================================

StepVisual_TessellatedEdge::StepVisual_TessellatedEdge()
{
  myHasGeometricLink = false;
}

//=================================================================================================

void StepVisual_TessellatedEdge::Init(
  const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
  const occ::handle<StepVisual_CoordinatesList>& theCoordinates,
  const bool                                     theHasGeometricLink,
  const StepVisual_EdgeOrCurve&                  theGeometricLink,
  const occ::handle<NCollection_HArray1<int>>&   theLineStrip)
{
  StepVisual_TessellatedStructuredItem::Init(theRepresentationItem_Name);

  myCoordinates = theCoordinates;

  myHasGeometricLink = theHasGeometricLink;
  if (myHasGeometricLink)
  {
    myGeometricLink = theGeometricLink;
  }
  else
  {
    myGeometricLink = StepVisual_EdgeOrCurve();
  }

  myLineStrip = theLineStrip;
}

//=================================================================================================

occ::handle<StepVisual_CoordinatesList> StepVisual_TessellatedEdge::Coordinates() const
{
  return myCoordinates;
}

//=================================================================================================

void StepVisual_TessellatedEdge::SetCoordinates(
  const occ::handle<StepVisual_CoordinatesList>& theCoordinates)
{
  myCoordinates = theCoordinates;
}

//=================================================================================================

StepVisual_EdgeOrCurve StepVisual_TessellatedEdge::GeometricLink() const
{
  return myGeometricLink;
}

//=================================================================================================

void StepVisual_TessellatedEdge::SetGeometricLink(const StepVisual_EdgeOrCurve& theGeometricLink)
{
  myGeometricLink = theGeometricLink;
}

//=================================================================================================

bool StepVisual_TessellatedEdge::HasGeometricLink() const
{
  return myHasGeometricLink;
}

//=================================================================================================

occ::handle<NCollection_HArray1<int>> StepVisual_TessellatedEdge::LineStrip() const
{
  return myLineStrip;
}

//=================================================================================================

void StepVisual_TessellatedEdge::SetLineStrip(
  const occ::handle<NCollection_HArray1<int>>& theLineStrip)
{
  myLineStrip = theLineStrip;
}

//=================================================================================================

int StepVisual_TessellatedEdge::NbLineStrip() const
{
  if (myLineStrip.IsNull())
  {
    return 0;
  }
  return myLineStrip->Length();
}

//=================================================================================================

int StepVisual_TessellatedEdge::LineStripValue(const int theNum) const
{
  return myLineStrip->Value(theNum);
}
