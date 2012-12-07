// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Graphic3d_TextureMap.ixx>
#include <Graphic3d_TextureParams.hxx>

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
