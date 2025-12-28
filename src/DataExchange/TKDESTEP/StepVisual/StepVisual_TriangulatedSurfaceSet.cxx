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
  const occ::handle<TCollection_HAsciiString>&    theRepresentationItemName,
  const occ::handle<StepVisual_CoordinatesList>&  theTessellatedSurfaceSetCoordinates,
  const int                                       theTessellatedSurfaceSetPnmax,
  const occ::handle<NCollection_HArray2<double>>& theTessellatedSurfaceSetNormals,
  const occ::handle<NCollection_HArray1<int>>&    thePnindex,
  const occ::handle<NCollection_HArray2<int>>&    theTriangles)
{
  StepVisual_TessellatedSurfaceSet::Init(theRepresentationItemName,
                                         theTessellatedSurfaceSetCoordinates,
                                         theTessellatedSurfaceSetPnmax,
                                         theTessellatedSurfaceSetNormals);
  myPnindex   = thePnindex;
  myTriangles = theTriangles;
}

//=================================================================================================

int StepVisual_TriangulatedSurfaceSet::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}

//=================================================================================================

int StepVisual_TriangulatedSurfaceSet::PnindexValue(const int theNum) const
{
  return myPnindex->Value(theNum);
}

//=================================================================================================

int StepVisual_TriangulatedSurfaceSet::NbTriangles() const
{
  if (myTriangles.IsNull())
  {
    return 0;
  }
  return myTriangles->ColLength();
}
