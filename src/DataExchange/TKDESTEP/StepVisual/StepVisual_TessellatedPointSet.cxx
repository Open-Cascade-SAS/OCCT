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

#include <StepVisual_TessellatedPointSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedPointSet, StepVisual_TessellatedItem)

//=================================================================================================

StepVisual_TessellatedPointSet::StepVisual_TessellatedPointSet() {}

//=================================================================================================

void StepVisual_TessellatedPointSet::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theCoordinates,
  const Handle(TColStd_HArray1OfInteger)&   thePointList)
{
  StepVisual_TessellatedItem::Init(theRepresentationItem_Name);

  myCoordinates = theCoordinates;

  myPointList = thePointList;
}

//=================================================================================================

Handle(StepVisual_CoordinatesList) StepVisual_TessellatedPointSet::Coordinates() const
{
  return myCoordinates;
}

//=================================================================================================

void StepVisual_TessellatedPointSet::SetCoordinates(
  const Handle(StepVisual_CoordinatesList)& theCoordinates)
{
  myCoordinates = theCoordinates;
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_TessellatedPointSet::PointList() const
{
  return myPointList;
}

//=================================================================================================

void StepVisual_TessellatedPointSet::SetPointList(
  const Handle(TColStd_HArray1OfInteger)& thePointList)
{
  myPointList = thePointList;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedPointSet::NbPointList() const
{
  if (myPointList.IsNull())
  {
    return 0;
  }
  return myPointList->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedPointSet::PointListValue(const Standard_Integer theNum) const
{
  return myPointList->Value(theNum);
}
