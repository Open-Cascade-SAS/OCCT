// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Graphic3d_TextureParams.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_TextureParams,Standard_Transient)

// =======================================================================
// function : Graphic3d_TextureParams
// purpose  :
// =======================================================================
Graphic3d_TextureParams::Graphic3d_TextureParams()
: myGenPlaneS  (0.0f, 0.0f, 0.0f, 0.0f),
  myGenPlaneT  (0.0f, 0.0f, 0.0f, 0.0f),
  myScale      (1.0f, 1.0f),
  myTranslation(0.0f, 0.0f),
  mySamplerRevision (0),
  myTextureUnit(Graphic3d_TextureUnit_BaseColor),
  myFilter     (Graphic3d_TOTF_NEAREST),
  myAnisoLevel (Graphic3d_LOTA_OFF),
  myGenMode    (Graphic3d_TOTM_MANUAL),
  myRotAngle   (0.0f),
  myToModulate (Standard_False),
  myToRepeat   (Standard_False)
{
  //
}

// =======================================================================
// function : ~Graphic3d_TextureParams
// purpose  :
// =======================================================================
Graphic3d_TextureParams::~Graphic3d_TextureParams()
{
  //
}

// =======================================================================
// function : SetModulate
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetModulate (const Standard_Boolean theToModulate)
{
  myToModulate = theToModulate;
}

// =======================================================================
// function : SetRepeat
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetRepeat (const Standard_Boolean theToRepeat)
{
  if (myToRepeat != theToRepeat)
  {
    myToRepeat = theToRepeat;
    updateSamplerRevision();
  }
}

// =======================================================================
// function : SetFilter
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetFilter (const Graphic3d_TypeOfTextureFilter theFilter)
{
  if (myFilter != theFilter)
  {
    myFilter = theFilter;
    updateSamplerRevision();
  }
}

// =======================================================================
// function : SetAnisoFilter
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel)
{
  if (myAnisoLevel != theLevel)
  {
    myAnisoLevel = theLevel;
    updateSamplerRevision();
  }
}

// =======================================================================
// function : SetRotation
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetRotation (const Standard_ShortReal theAngleDegrees)
{
  myRotAngle = theAngleDegrees;
}

// =======================================================================
// function : SetScale
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetScale (const Graphic3d_Vec2 theScale)
{
  myScale = theScale;
}

// =======================================================================
// function : SetTranslation
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetTranslation (const Graphic3d_Vec2 theVec)
{
  myTranslation = theVec;
}

// =======================================================================
// function : SetGenMode
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetGenMode (const Graphic3d_TypeOfTextureMode theMode,
                                          const Graphic3d_Vec4              thePlaneS,
                                          const Graphic3d_Vec4              thePlaneT)
{
  myGenMode   = theMode;
  myGenPlaneS = thePlaneS;
  myGenPlaneT = thePlaneT;
}
