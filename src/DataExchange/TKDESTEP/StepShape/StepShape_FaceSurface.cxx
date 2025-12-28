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

#include <StepGeom_Surface.hxx>
#include <StepShape_FaceSurface.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_FaceSurface, StepShape_Face)

StepShape_FaceSurface::StepShape_FaceSurface() {}

void StepShape_FaceSurface::Init(const occ::handle<TCollection_HAsciiString>&     aName,
                                 const occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>>& aBounds,
                                 const occ::handle<StepGeom_Surface>&             aFaceGeometry,
                                 const bool                      aSameSense)
{
  // --- classe own fields ---
  faceGeometry = aFaceGeometry;
  sameSense    = aSameSense;
  // --- classe inherited fields ---
  StepShape_Face::Init(aName, aBounds);
}

void StepShape_FaceSurface::SetFaceGeometry(const occ::handle<StepGeom_Surface>& aFaceGeometry)
{
  faceGeometry = aFaceGeometry;
}

occ::handle<StepGeom_Surface> StepShape_FaceSurface::FaceGeometry() const
{
  return faceGeometry;
}

void StepShape_FaceSurface::SetSameSense(const bool aSameSense)
{
  sameSense = aSameSense;
}

bool StepShape_FaceSurface::SameSense() const
{
  return sameSense;
}
