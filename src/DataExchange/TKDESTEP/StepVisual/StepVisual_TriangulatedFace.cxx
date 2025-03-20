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

#include <StepVisual_TriangulatedFace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TriangulatedFace, StepVisual_TessellatedFace)

//=================================================================================================

StepVisual_TriangulatedFace::StepVisual_TriangulatedFace()
    : StepVisual_TessellatedFace()
{
}

//=================================================================================================

void StepVisual_TriangulatedFace::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theTessellatedFace_Coordinates,
  const Standard_Integer                    theTessellatedFace_Pnmax,
  const Handle(TColStd_HArray2OfReal)&      theTessellatedFace_Normals,
  const Standard_Boolean                    theHasTessellatedFace_GeometricLink,
  const StepVisual_FaceOrSurface&           theTessellatedFace_GeometricLink,
  const Handle(TColStd_HArray1OfInteger)&   thePnindex,
  const Handle(TColStd_HArray2OfInteger)&   theTriangles)
{
  StepVisual_TessellatedFace::Init(theRepresentationItem_Name,
                                   theTessellatedFace_Coordinates,
                                   theTessellatedFace_Pnmax,
                                   theTessellatedFace_Normals,
                                   theHasTessellatedFace_GeometricLink,
                                   theTessellatedFace_GeometricLink);

  myPnindex = thePnindex;

  myTriangles = theTriangles;
}

//=================================================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_TriangulatedFace::Pnindex() const
{
  return myPnindex;
}

//=================================================================================================

void StepVisual_TriangulatedFace::SetPnindex(const Handle(TColStd_HArray1OfInteger)& thePnindex)
{
  myPnindex = thePnindex;
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedFace::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedFace::PnindexValue(const Standard_Integer theNum) const
{
  return myPnindex->Value(theNum);
}

//=================================================================================================

Handle(TColStd_HArray2OfInteger) StepVisual_TriangulatedFace::Triangles() const
{
  return myTriangles;
}

//=================================================================================================

Standard_Integer StepVisual_TriangulatedFace::NbTriangles() const
{
  if (myTriangles.IsNull())
  {
    return 0;
  }
  return myTriangles->ColLength();
}

//=================================================================================================

void StepVisual_TriangulatedFace::SetTriangles(const Handle(TColStd_HArray2OfInteger)& theTriangles)
{
  myTriangles = theTriangles;
}
