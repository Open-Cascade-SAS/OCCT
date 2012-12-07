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

#include <Graphic3d_Texture2Dmanual.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Graphic3d_TextureParams.hxx>

// =======================================================================
// function : Graphic3d_Texture2Dmanual
// purpose  :
// =======================================================================
Graphic3d_Texture2Dmanual::Graphic3d_Texture2Dmanual (const TCollection_AsciiString& theFileName)
: Graphic3d_Texture2D (theFileName, Graphic3d_TOT_2D_MIPMAP)
{
  myParams->SetModulate (Standard_True);
  myParams->SetRepeat   (Standard_True);
  myParams->SetFilter   (Graphic3d_TOTF_TRILINEAR);
}

// =======================================================================
// function : Graphic3d_Texture2Dmanual
// purpose  :
// =======================================================================
Graphic3d_Texture2Dmanual::Graphic3d_Texture2Dmanual (const Graphic3d_NameOfTexture2D theNOT)
: Graphic3d_Texture2D (theNOT, Graphic3d_TOT_2D_MIPMAP)
{
  myParams->SetModulate (Standard_True);
  myParams->SetRepeat   (Standard_True);
  myParams->SetFilter   (Graphic3d_TOTF_TRILINEAR);
}
