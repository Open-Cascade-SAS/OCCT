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
  myHasGeometricLink = Standard_False;
}

//=================================================================================================

void StepVisual_TessellatedEdge::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theCoordinates,
  const Standard_Boolean                    theHasGeometricLink,
  const StepVisual_EdgeOrCurve&             theGeometricLink,
  const Handle(TColStd_HArray1OfInteger)&   theLineStrip)
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

Handle(StepVisual_CoordinatesList) StepVisual_TessellatedEdge::Coordinates() const
{
  return myCoordinates;
}

//=================================================================================================

void StepVisual_TessellatedEdge::SetCoordinates(
  const Handle(StepVisual_CoordinatesList)& theCoordinates)
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

Standard_Boolean StepVisual_TessellatedEdge::HasGeometricLink() const
{
  return myHasGeometricLink;
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_TessellatedEdge::LineStrip() const
{
  return myLineStrip;
}

//=================================================================================================

void StepVisual_TessellatedEdge::SetLineStrip(const Handle(TColStd_HArray1OfInteger)& theLineStrip)
{
  myLineStrip = theLineStrip;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedEdge::NbLineStrip() const
{
  if (myLineStrip.IsNull())
  {
    return 0;
  }
  return myLineStrip->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedEdge::LineStripValue(const Standard_Integer theNum) const
{
  return myLineStrip->Value(theNum);
}
