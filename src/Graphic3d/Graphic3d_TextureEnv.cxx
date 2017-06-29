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


#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureEnv,Graphic3d_TextureRoot)

static const char *NameOfTextureEnv_to_FileName[] =
{
  "env_clouds.rgb",
  "env_cv.rgb",
  "env_medit.rgb",
  "env_pearl.rgb",
  "env_sky1.rgb",
  "env_sky2.rgb",
  "env_lines.rgb",
  "env_road.rgb"
};

// =======================================================================
// function : Graphic3d_TextureEnv
// purpose  :
// =======================================================================
Graphic3d_TextureEnv::Graphic3d_TextureEnv (const TCollection_AsciiString& theFileName)
: Graphic3d_TextureRoot (theFileName, Graphic3d_TOT_2D_MIPMAP),
  myName (Graphic3d_NOT_ENV_UNKNOWN)
{
  myParams->SetFilter  (Graphic3d_TOTF_TRILINEAR);
  myParams->SetGenMode (Graphic3d_TOTM_SPHERE,
                        Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Graphic3d_TextureEnv
// purpose  :
// =======================================================================
Graphic3d_TextureEnv::Graphic3d_TextureEnv (const Graphic3d_NameOfTextureEnv theNOT)
: Graphic3d_TextureRoot (NameOfTextureEnv_to_FileName[theNOT], Graphic3d_TOT_2D_MIPMAP),
  myName (theNOT)
{
  myPath.SetTrek (Graphic3d_TextureRoot::TexturesFolder());
  myTexId = TCollection_AsciiString ("Graphic3d_TextureEnv_")
          + NameOfTextureEnv_to_FileName[theNOT];

  myParams->SetFilter  (Graphic3d_TOTF_TRILINEAR);
  myParams->SetGenMode (Graphic3d_TOTM_SPHERE,
                        Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Graphic3d_TextureEnv
// purpose  :
// =======================================================================
Graphic3d_TextureEnv::Graphic3d_TextureEnv (const Handle(Image_PixMap)& thePixMap)
: Graphic3d_TextureRoot (thePixMap, Graphic3d_TOT_2D_MIPMAP),
  myName (Graphic3d_NOT_ENV_UNKNOWN)
{
  myParams->SetFilter  (Graphic3d_TOTF_TRILINEAR);
  myParams->SetGenMode (Graphic3d_TOTM_SPHERE,
                        Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Name
// purpose  :
// =======================================================================
Graphic3d_NameOfTextureEnv Graphic3d_TextureEnv::Name() const
{
  return myName;
}

// =======================================================================
// function : NumberOfTextures
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_TextureEnv::NumberOfTextures()
{
  return sizeof(NameOfTextureEnv_to_FileName)/sizeof(char*);
}

// =======================================================================
// function : TextureName
// purpose  :
// =======================================================================
TCollection_AsciiString Graphic3d_TextureEnv::TextureName (const Standard_Integer theRank)
{
  if(theRank < 1 || theRank > NumberOfTextures())
  {
	throw Standard_OutOfRange("BAD index of texture");
  }

  TCollection_AsciiString aFileName (NameOfTextureEnv_to_FileName[theRank - 1]);
  Standard_Integer i = aFileName.SearchFromEnd(".");
  return aFileName.SubString (5, i - 1);
}
