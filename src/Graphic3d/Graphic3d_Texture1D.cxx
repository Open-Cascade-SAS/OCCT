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


#include <Graphic3d_Texture1D.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture1D,Graphic3d_TextureMap)

static const char *NameOfTexture1d_to_FileName[] =
{
  "1d_elevation.rgb"
};

// =======================================================================
// function : Graphic3d_Texture1D
// purpose  :
// =======================================================================
Graphic3d_Texture1D::Graphic3d_Texture1D (const TCollection_AsciiString& theFileName,
                                          const Graphic3d_TypeOfTexture  theType)
: Graphic3d_TextureMap (theFileName, theType),
  myName (Graphic3d_NOT_1D_UNKNOWN)
{
}

// =======================================================================
// function : Graphic3d_Texture1D
// purpose  :
// =======================================================================
Graphic3d_Texture1D::Graphic3d_Texture1D (const Graphic3d_NameOfTexture1D theNOT,
                                          const Graphic3d_TypeOfTexture   theType)
: Graphic3d_TextureMap (NameOfTexture1d_to_FileName[theNOT], theType),
  myName (theNOT)
{
  myPath.SetTrek (Graphic3d_TextureRoot::TexturesFolder());
  myTexId = TCollection_AsciiString ("Graphic3d_Texture1D_")
          + NameOfTexture1d_to_FileName[theNOT];
}

// =======================================================================
// function : Graphic3d_Texture1D
// purpose  :
// =======================================================================
Graphic3d_Texture1D::Graphic3d_Texture1D (const Handle(Image_PixMap)&   thePixMap,
                                          const Graphic3d_TypeOfTexture theType)
: Graphic3d_TextureMap (thePixMap, theType),
  myName (Graphic3d_NOT_1D_UNKNOWN)
{
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
Graphic3d_NameOfTexture1D Graphic3d_Texture1D::Name() const
{
  return myName;
}

// =======================================================================
// function : NumberOfTextures
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_Texture1D::NumberOfTextures()
{
  return sizeof(NameOfTexture1d_to_FileName)/sizeof(char*);
}

// =======================================================================
// function : TextureName
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_Texture1D::TextureName (const Standard_Integer theRank)
{
  if (theRank < 1 || theRank > NumberOfTextures())
  {
    throw Standard_OutOfRange("BAD index of texture");
  }

  TCollection_AsciiString aFileName (NameOfTexture1d_to_FileName[theRank - 1]);
  Standard_Integer i = aFileName.SearchFromEnd (".");
  return aFileName.SubString (4, i - 1);
}
