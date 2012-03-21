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

// Modified : 	GG 09/11/00 smooth,modulate,repeat texture attributes
//		must be available both for textures 1D & 2D.


#include <Graphic3d_TextureMap.ixx>

Graphic3d_TextureMap::Graphic3d_TextureMap(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString Path,const Standard_CString FileName,const Graphic3d_TypeOfTexture Type)
: Graphic3d_TextureRoot(SM, Path, FileName, Type)
{
}

void Graphic3d_TextureMap::EnableSmooth() {
  MyCInitTexture.doLinear = 1;
  Update();
}

void Graphic3d_TextureMap::DisableSmooth() {
  MyCInitTexture.doLinear = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsSmoothed() const {
  return (MyCInitTexture.doLinear != 0);
}

void Graphic3d_TextureMap::EnableModulate() {
  MyCInitTexture.doModulate = 1;
  Update();
}

void Graphic3d_TextureMap::DisableModulate() {
  MyCInitTexture.doModulate = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsModulate() const {
  return (MyCInitTexture.doModulate != 0);
}

void Graphic3d_TextureMap::EnableRepeat() {
  MyCInitTexture.doRepeat = 1;
  Update();
}

void Graphic3d_TextureMap::DisableRepeat() {
  MyCInitTexture.doRepeat = 0;
  Update();
}

Standard_Boolean Graphic3d_TextureMap::IsRepeat() const {
  return (MyCInitTexture.doRepeat != 0);
}

