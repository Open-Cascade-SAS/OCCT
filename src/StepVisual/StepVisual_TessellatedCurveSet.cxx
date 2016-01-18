// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_TessellatedItem.hxx>


IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedCurveSet,StepVisual_TessellatedItem)

StepVisual_TessellatedCurveSet::StepVisual_TessellatedCurveSet ()  {}

void StepVisual_TessellatedCurveSet::Init(const Handle(TCollection_HAsciiString)& theName,
  const Handle(StepVisual_CoordinatesList)& theCoordList,
  const NCollection_Handle<StepVisual_VectorOfHSequenceOfInteger>& theCurves)
{
  StepRepr_RepresentationItem::Init(theName);
  myCoordList = theCoordList;
  myCurves = theCurves;
}


NCollection_Handle<StepVisual_VectorOfHSequenceOfInteger> StepVisual_TessellatedCurveSet::Curves() const
{
  return myCurves;
}

Handle(StepVisual_CoordinatesList) StepVisual_TessellatedCurveSet::CoordList() const
{
  return myCoordList;
}
