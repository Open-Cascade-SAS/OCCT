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

#include <Graphic3d_Texture2Dplane.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture2Dplane, Graphic3d_Texture2D)

//=================================================================================================

Graphic3d_Texture2Dplane::Graphic3d_Texture2Dplane(const TCollection_AsciiString& theFileName)
    : Graphic3d_Texture2D(theFileName)
{
  myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                       Graphic3d_Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                       Graphic3d_Vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

//=================================================================================================

Graphic3d_Texture2Dplane::Graphic3d_Texture2Dplane(const Graphic3d_NameOfTexture2D theNOT)
    : Graphic3d_Texture2D(theNOT)
{
  myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                       Graphic3d_Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                       Graphic3d_Vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

//=================================================================================================

Graphic3d_Texture2Dplane::Graphic3d_Texture2Dplane(const Handle(Image_PixMap)& thePixMap)
    : Graphic3d_Texture2D(thePixMap)
{
  myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                       Graphic3d_Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                       Graphic3d_Vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetPlaneS(const Standard_ShortReal theA,
                                         const Standard_ShortReal theB,
                                         const Standard_ShortReal theC,
                                         const Standard_ShortReal theD)
{
  const Graphic3d_Vec4 aPlaneS(theA, theB, theC, theD);
  const Graphic3d_Vec4 aPlaneT = myParams->GenPlaneT();
  myParams->SetGenMode(Graphic3d_TOTM_OBJECT, aPlaneS, aPlaneT);
  myPlaneName = Graphic3d_NOTP_UNKNOWN;
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetPlaneT(const Standard_ShortReal theA,
                                         const Standard_ShortReal theB,
                                         const Standard_ShortReal theC,
                                         const Standard_ShortReal theD)
{
  const Graphic3d_Vec4 aPlaneS = myParams->GenPlaneS();
  const Graphic3d_Vec4 aPlaneT(theA, theB, theC, theD);
  myParams->SetGenMode(Graphic3d_TOTM_OBJECT, aPlaneS, aPlaneT);
  myPlaneName = Graphic3d_NOTP_UNKNOWN;
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetPlane(const Graphic3d_NameOfTexturePlane thePlane)
{
  switch (thePlane)
  {
    case Graphic3d_NOTP_XY: {
      myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                           Graphic3d_Vec4(1.0f, 0.0f, 0.0f, 0.0f),
                           Graphic3d_Vec4(0.0f, 1.0f, 0.0f, 0.0f));
      break;
    }
    case Graphic3d_NOTP_YZ: {
      myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                           Graphic3d_Vec4(0.0f, 1.0f, 0.0f, 0.0f),
                           Graphic3d_Vec4(0.0f, 0.0f, 1.0f, 0.0f));
      break;
    }
    case Graphic3d_NOTP_ZX: {
      myParams->SetGenMode(Graphic3d_TOTM_OBJECT,
                           Graphic3d_Vec4(0.0f, 0.0f, 1.0f, 0.0f),
                           Graphic3d_Vec4(1.0f, 0.0f, 0.0f, 0.0f));
      break;
    }
    default:
      break;
  }

  myPlaneName = thePlane;
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetScaleS(const Standard_ShortReal theVal)
{
  Graphic3d_Vec2 aScale = myParams->Scale();
  aScale.x()            = theVal;
  myParams->SetScale(aScale);
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetScaleT(const Standard_ShortReal theVal)
{
  Graphic3d_Vec2 aScale = myParams->Scale();
  aScale.y()            = theVal;
  myParams->SetScale(aScale);
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetTranslateS(const Standard_ShortReal theVal)
{
  Graphic3d_Vec2 aVec = myParams->Translation();
  aVec.x()            = theVal;
  myParams->SetTranslation(aVec);
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetTranslateT(const Standard_ShortReal theVal)
{
  Graphic3d_Vec2 aVec = myParams->Translation();
  aVec.y()            = theVal;
  myParams->SetTranslation(aVec);
}

//=================================================================================================

void Graphic3d_Texture2Dplane::SetRotation(const Standard_ShortReal theAngleDegrees)
{
  myParams->SetRotation(theAngleDegrees);
}

//=================================================================================================

void Graphic3d_Texture2Dplane::PlaneS(Standard_ShortReal& theA,
                                      Standard_ShortReal& theB,
                                      Standard_ShortReal& theC,
                                      Standard_ShortReal& theD) const
{
  const Graphic3d_Vec4& aPlaneS = myParams->GenPlaneS();
  theA                          = aPlaneS.x();
  theB                          = aPlaneS.y();
  theC                          = aPlaneS.z();
  theD                          = aPlaneS.w();
}

//=================================================================================================

void Graphic3d_Texture2Dplane::PlaneT(Standard_ShortReal& theA,
                                      Standard_ShortReal& theB,
                                      Standard_ShortReal& theC,
                                      Standard_ShortReal& theD) const
{
  const Graphic3d_Vec4& aPlaneT = myParams->GenPlaneT();
  theA                          = aPlaneT.x();
  theB                          = aPlaneT.y();
  theC                          = aPlaneT.z();
  theD                          = aPlaneT.w();
}

//=================================================================================================

void Graphic3d_Texture2Dplane::TranslateS(Standard_ShortReal& theVal) const
{
  theVal = myParams->Translation().x();
}

//=================================================================================================

void Graphic3d_Texture2Dplane::TranslateT(Standard_ShortReal& theVal) const
{
  theVal = myParams->Translation().y();
}

//=================================================================================================

void Graphic3d_Texture2Dplane::Rotation(Standard_ShortReal& theVal) const
{
  theVal = myParams->Rotation();
}

//=================================================================================================

Graphic3d_NameOfTexturePlane Graphic3d_Texture2Dplane::Plane() const
{
  return myPlaneName;
}

//=================================================================================================

void Graphic3d_Texture2Dplane::ScaleS(Standard_ShortReal& theVal) const
{
  theVal = myParams->Scale().x();
}

//=================================================================================================

void Graphic3d_Texture2Dplane::ScaleT(Standard_ShortReal& theVal) const
{
  theVal = myParams->Scale().y();
}
