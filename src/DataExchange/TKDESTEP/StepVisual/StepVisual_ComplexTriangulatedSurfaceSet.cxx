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

#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ComplexTriangulatedSurfaceSet,
                           StepVisual_TessellatedSurfaceSet)

//=================================================================================================

StepVisual_ComplexTriangulatedSurfaceSet::StepVisual_ComplexTriangulatedSurfaceSet() {}

//=================================================================================================

void StepVisual_ComplexTriangulatedSurfaceSet::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theTessellatedSurfaceSet_Coordinates,
  const Standard_Integer                    theTessellatedSurfaceSet_Pnmax,
  const Handle(TColStd_HArray2OfReal)&      theTessellatedSurfaceSet_Normals,
  const Handle(TColStd_HArray1OfInteger)&   thePnindex,
  const Handle(TColStd_HArray1OfTransient)& theTriangleStrips,
  const Handle(TColStd_HArray1OfTransient)& theTriangleFans)
{
  StepVisual_TessellatedSurfaceSet::Init(theRepresentationItem_Name,
                                         theTessellatedSurfaceSet_Coordinates,
                                         theTessellatedSurfaceSet_Pnmax,
                                         theTessellatedSurfaceSet_Normals);

  myPnindex = thePnindex;

  myTriangleStrips = theTriangleStrips;

  myTriangleFans = theTriangleFans;
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_ComplexTriangulatedSurfaceSet::Pnindex() const
{
  return myPnindex;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedSurfaceSet::SetPnindex(
  const Handle(TColStd_HArray1OfInteger)& thePnindex)
{
  myPnindex = thePnindex;
}

//=================================================================================================

Standard_Integer StepVisual_ComplexTriangulatedSurfaceSet::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}

//=================================================================================================

Standard_Integer StepVisual_ComplexTriangulatedSurfaceSet::PnindexValue(
  const Standard_Integer theNum) const
{
  return myPnindex->Value(theNum);
}

//=================================================================================================

Handle(TColStd_HArray1OfTransient) StepVisual_ComplexTriangulatedSurfaceSet::TriangleStrips() const
{
  return myTriangleStrips;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedSurfaceSet::SetTriangleStrips(
  const Handle(TColStd_HArray1OfTransient)& theTriangleStrips)
{
  myTriangleStrips = theTriangleStrips;
}

//=================================================================================================

Standard_Integer StepVisual_ComplexTriangulatedSurfaceSet::NbTriangleStrips() const
{
  if (myTriangleStrips.IsNull())
  {
    return 0;
  }
  return myTriangleStrips->Length();
}

//=================================================================================================

Handle(TColStd_HArray1OfTransient) StepVisual_ComplexTriangulatedSurfaceSet::TriangleFans() const
{
  return myTriangleFans;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedSurfaceSet::SetTriangleFans(
  const Handle(TColStd_HArray1OfTransient)& theTriangleFans)
{
  myTriangleFans = theTriangleFans;
}

//=================================================================================================

Standard_Integer StepVisual_ComplexTriangulatedSurfaceSet::NbTriangleFans() const
{
  if (myTriangleFans.IsNull())
  {
    return 0;
  }
  return myTriangleFans->Length();
}
