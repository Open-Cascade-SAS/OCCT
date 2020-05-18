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

#include <StepVisual_SurfaceStyleReflectanceAmbient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleReflectanceAmbient, Standard_Transient)

//=======================================================================
//function : StepVisual_SurfaceStyleReflectanceAmbient
//purpose  :
//=======================================================================

StepVisual_SurfaceStyleReflectanceAmbient::StepVisual_SurfaceStyleReflectanceAmbient ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleReflectanceAmbient::Init (const Standard_Real theAmbientReflectance)
{

  myAmbientReflectance = theAmbientReflectance;
}

//=======================================================================
//function : AmbientReflectance
//purpose  :
//=======================================================================

Standard_Real StepVisual_SurfaceStyleReflectanceAmbient::AmbientReflectance () const
{
  return myAmbientReflectance;
}

//=======================================================================
//function : SetAmbientReflectance
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleReflectanceAmbient::SetAmbientReflectance (const Standard_Real theAmbientReflectance)
{
  myAmbientReflectance = theAmbientReflectance;
}
