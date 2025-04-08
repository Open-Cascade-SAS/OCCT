// Copyright (c) Open CASCADE 2025
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

#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuse.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbientDiffuse,
                           StepVisual_SurfaceStyleReflectanceAmbient)

//=================================================================================================

StepVisual_SurfaceStyleReflectanceAmbientDiffuse::StepVisual_SurfaceStyleReflectanceAmbientDiffuse()
{
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuse::Init(
  const Standard_Real theAmbientReflectance,
  const Standard_Real theDiffuseReflectance)
{
  StepVisual_SurfaceStyleReflectanceAmbient::Init(theAmbientReflectance);
  myDiffuseReflectance = theDiffuseReflectance;
}

//=================================================================================================

Standard_Real StepVisual_SurfaceStyleReflectanceAmbientDiffuse::DiffuseReflectance() const
{
  return myDiffuseReflectance;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuse::SetDiffuseReflectance(
  const Standard_Real theDiffuseReflectance)
{
  myDiffuseReflectance = theDiffuseReflectance;
}
