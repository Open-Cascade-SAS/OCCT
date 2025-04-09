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

#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular,
                           StepVisual_SurfaceStyleReflectanceAmbientDiffuse)

//=================================================================================================

StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::
  StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular()
{
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::Init(
  const Standard_Real              theAmbientReflectance,
  const Standard_Real              theDiffuseReflectance,
  const Standard_Real              theSpecularReflectance,
  const Standard_Real              theSpecularExponent,
  const Handle(StepVisual_Colour)& theSpecularColour)
{
  StepVisual_SurfaceStyleReflectanceAmbientDiffuse::Init(theAmbientReflectance,
                                                         theDiffuseReflectance);
  mySpecularReflectance = theSpecularReflectance;
  mySpecularExponent    = theSpecularExponent;
  mySpecularColour      = theSpecularColour;
}

//=================================================================================================

Standard_Real StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularReflectance() const
{
  return mySpecularReflectance;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularReflectance(
  const Standard_Real theSpecularReflectance)
{
  mySpecularReflectance = theSpecularReflectance;
}

//=================================================================================================

Standard_Real StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularExponent() const
{
  return mySpecularExponent;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularExponent(
  const Standard_Real theSpecularExponent)
{
  mySpecularExponent = theSpecularExponent;
}

//=================================================================================================

Handle(StepVisual_Colour) StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularColour()
  const
{
  return mySpecularColour;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularColour(
  const Handle(StepVisual_Colour)& theSpecularColour)
{
  mySpecularColour = theSpecularColour;
}