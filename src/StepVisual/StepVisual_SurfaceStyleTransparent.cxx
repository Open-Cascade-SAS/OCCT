// Created on : Tue May 12 14:11:46 2020
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

#include <StepVisual_SurfaceStyleTransparent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_SurfaceStyleTransparent, Standard_Transient)

//=======================================================================
//function : StepVisual_SurfaceStyleTransparent
//purpose  :
//=======================================================================

StepVisual_SurfaceStyleTransparent::StepVisual_SurfaceStyleTransparent ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleTransparent::Init (const Standard_Real theTransparency)
{

  myTransparency = theTransparency;
}

//=======================================================================
//function : Transparency
//purpose  :
//=======================================================================

Standard_Real StepVisual_SurfaceStyleTransparent::Transparency () const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================

void StepVisual_SurfaceStyleTransparent::SetTransparency (const Standard_Real theTransparency)
{
  myTransparency = theTransparency;
}
