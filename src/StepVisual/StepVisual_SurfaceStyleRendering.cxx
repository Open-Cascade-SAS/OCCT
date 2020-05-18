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

#include <StepVisual_SurfaceStyleRendering.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleRendering, Standard_Transient)

//=======================================================================
//function : StepVisual_SurfaceStyleRendering
//purpose  :
//=======================================================================

StepVisual_SurfaceStyleRendering::StepVisual_SurfaceStyleRendering ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleRendering::Init (const StepVisual_ShadingSurfaceMethod theRenderingMethod,
                                             const Handle(StepVisual_Colour)& theSurfaceColour)
{

  myRenderingMethod = theRenderingMethod;

  mySurfaceColour = theSurfaceColour;
}

//=======================================================================
//function : RenderingMethod
//purpose  :
//=======================================================================

StepVisual_ShadingSurfaceMethod StepVisual_SurfaceStyleRendering::RenderingMethod () const
{
  return myRenderingMethod;
}

//=======================================================================
//function : SetRenderingMethod
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleRendering::SetRenderingMethod (const StepVisual_ShadingSurfaceMethod theRenderingMethod)
{
  myRenderingMethod = theRenderingMethod;
}

//=======================================================================
//function : SurfaceColour
//purpose  :
//=======================================================================

Handle(StepVisual_Colour) StepVisual_SurfaceStyleRendering::SurfaceColour () const
{
  return mySurfaceColour;
}

//=======================================================================
//function : SetSurfaceColour
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleRendering::SetSurfaceColour (const Handle(StepVisual_Colour)& theSurfaceColour)
{
  mySurfaceColour = theSurfaceColour;
}
