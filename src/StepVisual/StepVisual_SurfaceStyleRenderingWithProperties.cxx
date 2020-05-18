// Created on : Thu May 14 15:13:19 2020
// Created by: Igor KHOZHANOV
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2020
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

#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleRenderingWithProperties, StepVisual_SurfaceStyleRendering)

//=======================================================================
//function : StepVisual_SurfaceStyleRenderingWithProperties
//purpose  :
//=======================================================================

StepVisual_SurfaceStyleRenderingWithProperties::StepVisual_SurfaceStyleRenderingWithProperties ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleRenderingWithProperties::Init (const StepVisual_ShadingSurfaceMethod theSurfaceStyleRendering_RenderingMethod,
                                                           const Handle(StepVisual_Colour)& theSurfaceStyleRendering_SurfaceColour,
                                                           const Handle(StepVisual_HArray1OfRenderingPropertiesSelect)& theProperties)
{
  StepVisual_SurfaceStyleRendering::Init(theSurfaceStyleRendering_RenderingMethod,
                                         theSurfaceStyleRendering_SurfaceColour);

  myProperties = theProperties;
}

//=======================================================================
//function : Properties
//purpose  :
//=======================================================================

Handle(StepVisual_HArray1OfRenderingPropertiesSelect) StepVisual_SurfaceStyleRenderingWithProperties::Properties () const
{
  return myProperties;
}

//=======================================================================
//function : SetProperties
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleRenderingWithProperties::SetProperties (const Handle(StepVisual_HArray1OfRenderingPropertiesSelect)& theProperties)
{
  myProperties = theProperties;
}
