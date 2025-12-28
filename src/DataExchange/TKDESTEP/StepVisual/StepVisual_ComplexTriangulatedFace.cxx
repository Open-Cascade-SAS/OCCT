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

#include <StepVisual_ComplexTriangulatedFace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ComplexTriangulatedFace, StepVisual_TessellatedFace)

//=================================================================================================

StepVisual_ComplexTriangulatedFace::StepVisual_ComplexTriangulatedFace() {}

//=================================================================================================

void StepVisual_ComplexTriangulatedFace::Init(
  const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
  const occ::handle<StepVisual_CoordinatesList>& theTessellatedFace_Coordinates,
  const int                    theTessellatedFace_Pnmax,
  const occ::handle<NCollection_HArray2<double>>&      theTessellatedFace_Normals,
  const bool                    theHasTessellatedFace_GeometricLink,
  const StepVisual_FaceOrSurface&           theTessellatedFace_GeometricLink,
  const occ::handle<NCollection_HArray1<int>>&   thePnindex,
  const occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>>& theTriangleStrips,
  const occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>>& theTriangleFans)
{
  StepVisual_TessellatedFace::Init(theRepresentationItem_Name,
                                   theTessellatedFace_Coordinates,
                                   theTessellatedFace_Pnmax,
                                   theTessellatedFace_Normals,
                                   theHasTessellatedFace_GeometricLink,
                                   theTessellatedFace_GeometricLink);

  myPnindex = thePnindex;

  myTriangleStrips = theTriangleStrips;

  myTriangleFans = theTriangleFans;
}

//=================================================================================================

occ::handle<NCollection_HArray1<int>> StepVisual_ComplexTriangulatedFace::Pnindex() const
{
  return myPnindex;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedFace::SetPnindex(
  const occ::handle<NCollection_HArray1<int>>& thePnindex)
{
  myPnindex = thePnindex;
}

//=================================================================================================

int StepVisual_ComplexTriangulatedFace::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}

//=================================================================================================

int StepVisual_ComplexTriangulatedFace::PnindexValue(
  const int theNum) const
{
  return myPnindex->Value(theNum);
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> StepVisual_ComplexTriangulatedFace::TriangleStrips() const
{
  return myTriangleStrips;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedFace::SetTriangleStrips(
  const occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>>& theTriangleStrips)
{
  myTriangleStrips = theTriangleStrips;
}

//=================================================================================================

int StepVisual_ComplexTriangulatedFace::NbTriangleStrips() const
{
  if (myTriangleStrips.IsNull())
  {
    return 0;
  }
  return myTriangleStrips->Length();
}

//=================================================================================================

occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> StepVisual_ComplexTriangulatedFace::TriangleFans() const
{
  return myTriangleFans;
}

//=================================================================================================

void StepVisual_ComplexTriangulatedFace::SetTriangleFans(
  const occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>>& theTriangleFans)
{
  myTriangleFans = theTriangleFans;
}

//=================================================================================================

int StepVisual_ComplexTriangulatedFace::NbTriangleFans() const
{
  if (myTriangleFans.IsNull())
  {
    return 0;
  }
  return myTriangleFans->Length();
}
