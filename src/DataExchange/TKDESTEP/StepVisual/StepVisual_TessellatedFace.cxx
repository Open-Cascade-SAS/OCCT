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

#include <StepVisual_TessellatedFace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedFace, StepVisual_TessellatedStructuredItem)

//=================================================================================================

StepVisual_TessellatedFace::StepVisual_TessellatedFace()
{
  myHasGeometricLink = false;
}

//=================================================================================================

void StepVisual_TessellatedFace::Init(
  const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
  const occ::handle<StepVisual_CoordinatesList>& theCoordinates,
  const int                    thePnmax,
  const occ::handle<NCollection_HArray2<double>>&      theNormals,
  const bool                    theHasGeometricLink,
  const StepVisual_FaceOrSurface&           theGeometricLink)
{
  StepVisual_TessellatedStructuredItem::Init(theRepresentationItem_Name);

  myCoordinates = theCoordinates;

  myPnmax = thePnmax;

  myNormals = theNormals;

  myHasGeometricLink = theHasGeometricLink;
  if (myHasGeometricLink)
  {
    myGeometricLink = theGeometricLink;
  }
  else
  {
    myGeometricLink = StepVisual_FaceOrSurface();
  }
}

//=================================================================================================

occ::handle<StepVisual_CoordinatesList> StepVisual_TessellatedFace::Coordinates() const
{
  return myCoordinates;
}

//=================================================================================================

void StepVisual_TessellatedFace::SetCoordinates(
  const occ::handle<StepVisual_CoordinatesList>& theCoordinates)
{
  myCoordinates = theCoordinates;
}

//=================================================================================================

int StepVisual_TessellatedFace::Pnmax() const
{
  return myPnmax;
}

//=================================================================================================

void StepVisual_TessellatedFace::SetPnmax(const int thePnmax)
{
  myPnmax = thePnmax;
}

//=================================================================================================

occ::handle<NCollection_HArray2<double>> StepVisual_TessellatedFace::Normals() const
{
  return myNormals;
}

//=================================================================================================

void StepVisual_TessellatedFace::SetNormals(const occ::handle<NCollection_HArray2<double>>& theNormals)
{
  myNormals = theNormals;
}

//=================================================================================================

int StepVisual_TessellatedFace::NbNormals() const
{
  if (myNormals.IsNull())
  {
    return 0;
  }
  return myNormals->ColLength();
}

//=================================================================================================

StepVisual_FaceOrSurface StepVisual_TessellatedFace::GeometricLink() const
{
  return myGeometricLink;
}

//=================================================================================================

void StepVisual_TessellatedFace::SetGeometricLink(const StepVisual_FaceOrSurface& theGeometricLink)
{
  myGeometricLink = theGeometricLink;
}

//=================================================================================================

bool StepVisual_TessellatedFace::HasGeometricLink() const
{
  return myHasGeometricLink;
}
