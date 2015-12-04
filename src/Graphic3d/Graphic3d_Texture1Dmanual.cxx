// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Graphic3d_Texture1Dmanual.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture1Dmanual,Graphic3d_Texture1D)

// =======================================================================
// function : Graphic3d_Texture1Dmanual
// purpose  :
// =======================================================================
Graphic3d_Texture1Dmanual::Graphic3d_Texture1Dmanual (const TCollection_AsciiString& theFileName)
: Graphic3d_Texture1D (theFileName, Graphic3d_TOT_1D)
{
  //
}

// =======================================================================
// function : Graphic3d_Texture1Dmanual
// purpose  :
// =======================================================================
Graphic3d_Texture1Dmanual::Graphic3d_Texture1Dmanual (const Graphic3d_NameOfTexture1D theNOT)
: Graphic3d_Texture1D (theNOT, Graphic3d_TOT_1D)
{
  //
}

// =======================================================================
// function : Graphic3d_Texture1Dmanual
// purpose  :
// =======================================================================
Graphic3d_Texture1Dmanual::Graphic3d_Texture1Dmanual (const Handle(Image_PixMap)& thePixMap)
: Graphic3d_Texture1D (thePixMap, Graphic3d_TOT_1D)
{
  //
}
