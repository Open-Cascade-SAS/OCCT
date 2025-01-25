// Copyright (c) Open CASCADE 2023
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

#include <StepVisual_TriangulatedSurfaceSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TriangulatedSurfaceSet, StepVisual_TessellatedSurfaceSet)

//=================================================================================================

StepVisual_TriangulatedSurfaceSet::StepVisual_TriangulatedSurfaceSet() {}

//=================================================================================================

void StepVisual_TriangulatedSurfaceSet::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItemName,
  const Handle(StepVisual_CoordinatesList)& theTessellatedSurfaceSetCoordinates,
  const Standard_Integer                    theTessellatedSurfaceSetPnmax,
  const Handle(TColStd_HArray2OfReal)&      theTessellatedSurfaceSetNormals,
  const Handle(TColStd_HArray1OfInteger)&   thePnindex,
  const Handle(TColStd_HArray2OfInteger)&   theTriangles)
{
  StepVisual_TessellatedSurfaceSet::Init(theRepresentationItemName,
                                         theTessellatedSurfaceSetCoordinates,
                                         theTessellatedSurfaceSetPnmax,
                                         theTessellatedSurfaceSetNormals);
  myPnindex   = thePnindex;
  myTriangles = theTriangles;
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedSurfaceSet::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedSurfaceSet::PnindexValue(
  const Standard_Integer theNum) const
{
  return myPnindex->Value(theNum);
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedSurfaceSet::NbTriangles() const
{
  if (myTriangles.IsNull())
  {
    return 0;
  }
  return myTriangles->ColLength();
}
