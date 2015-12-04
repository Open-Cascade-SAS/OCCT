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


#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureMap,Graphic3d_TextureRoot)

// =======================================================================
// function : Graphic3d_TextureMap
// purpose  :
// =======================================================================
Graphic3d_TextureMap::Graphic3d_TextureMap (const TCollection_AsciiString& theFileName,
                                            const Graphic3d_TypeOfTexture  theType)
: Graphic3d_TextureRoot (theFileName, theType)
{
}

// =======================================================================
// function : Graphic3d_TextureMap
// purpose  :
// =======================================================================
Graphic3d_TextureMap::Graphic3d_TextureMap (const Handle(Image_PixMap)&   thePixMap,
                                            const Graphic3d_TypeOfTexture theType)
: Graphic3d_TextureRoot (thePixMap, theType)
{
}

// =======================================================================
// function : EnableSmooth
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::EnableSmooth()
{
  myParams->SetFilter (Graphic3d_TOTF_TRILINEAR);
}

// =======================================================================
// function : DisableSmooth
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::DisableSmooth()
{
  myParams->SetFilter (Graphic3d_TOTF_NEAREST);
}

// =======================================================================
// function : IsSmoothed
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureMap::IsSmoothed() const
{
  return myParams->Filter() != Graphic3d_TOTF_NEAREST;
}

// =======================================================================
// function : EnableModulate
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::EnableModulate()
{
  myParams->SetModulate (Standard_True);
}

// =======================================================================
// function : DisableModulate
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::DisableModulate()
{
  myParams->SetModulate (Standard_False);
}

// =======================================================================
// function : IsModulate
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureMap::IsModulate() const
{
  return myParams->IsModulate();
}

// =======================================================================
// function : EnableRepeat
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::EnableRepeat()
{
  myParams->SetRepeat (Standard_True);
}

// =======================================================================
// function : DisableRepeat
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::DisableRepeat()
{
  myParams->SetRepeat (Standard_False);
}

// =======================================================================
// function : IsRepeat
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureMap::IsRepeat() const
{
  return myParams->IsRepeat();
}

// =======================================================================
// function : AnisoFilter
// purpose  :
// =======================================================================
Graphic3d_LevelOfTextureAnisotropy Graphic3d_TextureMap::AnisoFilter() const
{
  return myParams->AnisoFilter();
}

// =======================================================================
// function : SetAnisoFilter
// purpose  :
// =======================================================================
void Graphic3d_TextureMap::SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel)
{
  myParams->SetAnisoFilter (theLevel);
}
