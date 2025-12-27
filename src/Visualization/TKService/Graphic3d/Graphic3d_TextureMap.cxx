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

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureMap, Graphic3d_TextureRoot)

//=================================================================================================

Graphic3d_TextureMap::Graphic3d_TextureMap(const TCollection_AsciiString& theFileName,
                                           const Graphic3d_TypeOfTexture  theType)
    : Graphic3d_TextureRoot(theFileName, theType)
{
}

//=================================================================================================

Graphic3d_TextureMap::Graphic3d_TextureMap(const occ::handle<Image_PixMap>&   thePixMap,
                                           const Graphic3d_TypeOfTexture theType)
    : Graphic3d_TextureRoot(thePixMap, theType)
{
}

//=================================================================================================

void Graphic3d_TextureMap::EnableSmooth()
{
  myParams->SetFilter(Graphic3d_TOTF_TRILINEAR);
}

//=================================================================================================

void Graphic3d_TextureMap::DisableSmooth()
{
  myParams->SetFilter(Graphic3d_TOTF_NEAREST);
}

//=================================================================================================

bool Graphic3d_TextureMap::IsSmoothed() const
{
  return myParams->Filter() != Graphic3d_TOTF_NEAREST;
}

//=================================================================================================

void Graphic3d_TextureMap::EnableModulate()
{
  myParams->SetModulate(true);
}

//=================================================================================================

void Graphic3d_TextureMap::DisableModulate()
{
  myParams->SetModulate(false);
}

//=================================================================================================

bool Graphic3d_TextureMap::IsModulate() const
{
  return myParams->IsModulate();
}

//=================================================================================================

void Graphic3d_TextureMap::EnableRepeat()
{
  myParams->SetRepeat(true);
}

//=================================================================================================

void Graphic3d_TextureMap::DisableRepeat()
{
  myParams->SetRepeat(false);
}

//=================================================================================================

bool Graphic3d_TextureMap::IsRepeat() const
{
  return myParams->IsRepeat();
}

//=================================================================================================

Graphic3d_LevelOfTextureAnisotropy Graphic3d_TextureMap::AnisoFilter() const
{
  return myParams->AnisoFilter();
}

//=================================================================================================

void Graphic3d_TextureMap::SetAnisoFilter(const Graphic3d_LevelOfTextureAnisotropy theLevel)
{
  myParams->SetAnisoFilter(theLevel);
}
