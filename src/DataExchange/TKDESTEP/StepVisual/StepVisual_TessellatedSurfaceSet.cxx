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

#include <StepVisual_TessellatedSurfaceSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedSurfaceSet, StepVisual_TessellatedItem)

//=================================================================================================

StepVisual_TessellatedSurfaceSet::StepVisual_TessellatedSurfaceSet() {}

//=================================================================================================

void StepVisual_TessellatedSurfaceSet::Init(
  const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
  const Handle(StepVisual_CoordinatesList)& theCoordinates,
  const Standard_Integer                    thePnmax,
  const Handle(TColStd_HArray2OfReal)&      theNormals)
{
  StepVisual_TessellatedItem::Init(theRepresentationItem_Name);

  myCoordinates = theCoordinates;

  myPnmax = thePnmax;

  myNormals = theNormals;
}

//=================================================================================================

Handle(StepVisual_CoordinatesList) StepVisual_TessellatedSurfaceSet::Coordinates() const
{
  return myCoordinates;
}

//=================================================================================================

void StepVisual_TessellatedSurfaceSet::SetCoordinates(
  const Handle(StepVisual_CoordinatesList)& theCoordinates)
{
  myCoordinates = theCoordinates;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedSurfaceSet::Pnmax() const
{
  return myPnmax;
}

//=================================================================================================

void StepVisual_TessellatedSurfaceSet::SetPnmax(const Standard_Integer thePnmax)
{
  myPnmax = thePnmax;
}

//=================================================================================================

Handle(TColStd_HArray2OfReal) StepVisual_TessellatedSurfaceSet::Normals() const
{
  return myNormals;
}

//=================================================================================================

void StepVisual_TessellatedSurfaceSet::SetNormals(const Handle(TColStd_HArray2OfReal)& theNormals)
{
  myNormals = theNormals;
}

//=================================================================================================

Standard_Integer StepVisual_TessellatedSurfaceSet::NbNormals() const
{
  if (myNormals.IsNull())
  {
    return 0;
  }
  return myNormals->ColLength();
}