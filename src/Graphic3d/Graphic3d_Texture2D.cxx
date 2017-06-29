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

#include <Graphic3d_Texture2D.hxx>

#include <Standard_OutOfRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture2D,Graphic3d_TextureMap)

static const char *NameOfTexture2d_to_FileName[] =
{
  "2d_MatraDatavision.rgb",
  "2d_alienskin.rgb",
  "2d_blue_rock.rgb",
  "2d_bluewhite_paper.rgb",
  "2d_brushed.rgb",
  "2d_bubbles.rgb",
  "2d_bumps.rgb",
  "2d_cast.rgb",
  "2d_chipbd.rgb",
  "2d_clouds.rgb",
  "2d_flesh.rgb",
  "2d_floor.rgb",
  "2d_galvnisd.rgb",
  "2d_grass.rgb",
  "2d_aluminum.rgb",
  "2d_rock.rgb",
  "2d_knurl.rgb",
  "2d_maple.rgb",
  "2d_marble.rgb",
  "2d_mottled.rgb",
  "2d_rain.rgb",
  "2d_chess.rgba"
};

// =======================================================================
// function : Graphic3d_Texture2D
// purpose  :
// =======================================================================
Graphic3d_Texture2D::Graphic3d_Texture2D (const TCollection_AsciiString& theFileName,
                                          const Graphic3d_TypeOfTexture  theType)
: Graphic3d_TextureMap (theFileName, theType),
  myName (Graphic3d_NOT_2D_UNKNOWN)
{
}

// =======================================================================
// function : Graphic3d_Texture2D
// purpose  :
// =======================================================================
Graphic3d_Texture2D::Graphic3d_Texture2D (const Graphic3d_NameOfTexture2D theNOT,
                                          const Graphic3d_TypeOfTexture   theType)
: Graphic3d_TextureMap (NameOfTexture2d_to_FileName[theNOT], theType),
  myName (theNOT)
{
  myPath.SetTrek (Graphic3d_TextureRoot::TexturesFolder());
  myTexId = TCollection_AsciiString ("Graphic3d_Texture2D_")
          + NameOfTexture2d_to_FileName[theNOT];
}

// =======================================================================
// function : Graphic3d_Texture2D
// purpose  :
// =======================================================================
Graphic3d_Texture2D::Graphic3d_Texture2D (const Handle(Image_PixMap)&    thePixMap,
                                          const Graphic3d_TypeOfTexture  theType)
: Graphic3d_TextureMap (thePixMap, theType),
  myName (Graphic3d_NOT_2D_UNKNOWN)
{
}

// =======================================================================
// function : NumberOfTextures
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_Texture2D::NumberOfTextures()
{
  return sizeof(NameOfTexture2d_to_FileName)/sizeof(char*);
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
Graphic3d_NameOfTexture2D Graphic3d_Texture2D::Name() const
{
  return myName;
}

// =======================================================================
// function : TextureName
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_Texture2D::TextureName (const Standard_Integer theRank)
{
  if (theRank < 1 || theRank > NumberOfTextures())
  {
    throw Standard_OutOfRange("BAD index of texture");
  }

  TCollection_AsciiString aFileName (NameOfTexture2d_to_FileName[theRank - 1]);
  Standard_Integer i = aFileName.SearchFromEnd (".");
  return aFileName.SubString (4, i - 1);
}

// =======================================================================
// function : SetImage
// purpose  :
// =======================================================================
void Graphic3d_Texture2D::SetImage (const Handle(Image_PixMap)& thePixMap)
{
  myPixMap = thePixMap;
  myPath = OSD_Path();
  myName = Graphic3d_NOT_2D_UNKNOWN;
}
