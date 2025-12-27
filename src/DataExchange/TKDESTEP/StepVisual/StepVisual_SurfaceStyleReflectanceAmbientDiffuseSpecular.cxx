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
  const double              theAmbientReflectance,
  const double              theDiffuseReflectance,
  const double              theSpecularReflectance,
  const double              theSpecularExponent,
  const occ::handle<StepVisual_Colour>& theSpecularColour)
{
  StepVisual_SurfaceStyleReflectanceAmbientDiffuse::Init(theAmbientReflectance,
                                                         theDiffuseReflectance);
  mySpecularReflectance = theSpecularReflectance;
  mySpecularExponent    = theSpecularExponent;
  mySpecularColour      = theSpecularColour;
}

//=================================================================================================

double StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularReflectance() const
{
  return mySpecularReflectance;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularReflectance(
  const double theSpecularReflectance)
{
  mySpecularReflectance = theSpecularReflectance;
}

//=================================================================================================

double StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularExponent() const
{
  return mySpecularExponent;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularExponent(
  const double theSpecularExponent)
{
  mySpecularExponent = theSpecularExponent;
}

//=================================================================================================

occ::handle<StepVisual_Colour> StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SpecularColour()
  const
{
  return mySpecularColour;
}

//=================================================================================================

void StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular::SetSpecularColour(
  const occ::handle<StepVisual_Colour>& theSpecularColour)
{
  mySpecularColour = theSpecularColour;
}