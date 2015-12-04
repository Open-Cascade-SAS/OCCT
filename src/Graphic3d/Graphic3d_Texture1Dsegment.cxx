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


#include <Graphic3d_Texture1Dsegment.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Texture1Dsegment,Graphic3d_Texture1D)

// =======================================================================
// function : Graphic3d_Texture1Dsegment
// purpose  :
// =======================================================================
Graphic3d_Texture1Dsegment::Graphic3d_Texture1Dsegment (const TCollection_AsciiString& theFileName)
: Graphic3d_Texture1D (theFileName, Graphic3d_TOT_1D),
  myX1 (0.0f),
  myY1 (0.0f),
  myZ1 (0.0f),
  myX2 (0.0f),
  myY2 (0.0f),
  myZ2 (0.0f)
{
  myParams->SetRepeat (Standard_True);
  myParams->SetGenMode (Graphic3d_TOTM_OBJECT,
                        Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Graphic3d_Texture1Dsegment
// purpose  :
// =======================================================================
Graphic3d_Texture1Dsegment::Graphic3d_Texture1Dsegment (const Graphic3d_NameOfTexture1D theNOT)
: Graphic3d_Texture1D (theNOT, Graphic3d_TOT_1D),
  myX1 (0.0f),
  myY1 (0.0f),
  myZ1 (0.0f),
  myX2 (0.0f),
  myY2 (0.0f),
  myZ2 (0.0f)
{
  myParams->SetRepeat (Standard_True);
  myParams->SetGenMode (Graphic3d_TOTM_OBJECT,
                        Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Graphic3d_Texture1Dsegment
// purpose  :
// =======================================================================
Graphic3d_Texture1Dsegment::Graphic3d_Texture1Dsegment (const Handle(Image_PixMap)& thePixMap)
: Graphic3d_Texture1D (thePixMap, Graphic3d_TOT_1D),
  myX1 (0.0f),
  myY1 (0.0f),
  myZ1 (0.0f),
  myX2 (0.0f),
  myY2 (0.0f),
  myZ2 (0.0f)
{
  myParams->SetRepeat (Standard_True);
  myParams->SetGenMode (Graphic3d_TOTM_OBJECT,
                        Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f),
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : SetSegment
// purpose  :
// =======================================================================
void Graphic3d_Texture1Dsegment::SetSegment (const Standard_ShortReal X1,
                                             const Standard_ShortReal Y1,
                                             const Standard_ShortReal Z1,
                                             const Standard_ShortReal X2,
                                             const Standard_ShortReal Y2,
                                             const Standard_ShortReal Z2)
{
  myX1 = X1;
  myY1 = Y1;
  myZ1 = Z1;
  myX2 = X2;
  myY2 = Y2;
  myZ2 = Z2;
  Graphic3d_Vec4 aPlaneX (X2 - X1, Y2 - Y1, Z2 - Z1, 0.0f);

  Standard_ShortReal aSqNorm = aPlaneX.x() * aPlaneX.x()
                             + aPlaneX.y() * aPlaneX.y()
                             + aPlaneX.z() * aPlaneX.z();
  aPlaneX.x() /= aSqNorm;
  aPlaneX.y() /= aSqNorm;
  aPlaneX.z() /= aSqNorm;
  aPlaneX.w() = -aPlaneX.x() * X1
                -aPlaneX.y() * Y1
			    -aPlaneX.z() * Z1;

  myParams->SetGenMode (Graphic3d_TOTM_OBJECT,
                        aPlaneX,
                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
}

// =======================================================================
// function : Segment
// purpose  :
// =======================================================================
void Graphic3d_Texture1Dsegment::Segment (Standard_ShortReal& X1,
                                          Standard_ShortReal& Y1,
                                          Standard_ShortReal& Z1,
                                          Standard_ShortReal& X2,
                                          Standard_ShortReal& Y2,
                                          Standard_ShortReal& Z2) const
{
  X1 = myX1;
  Y1 = myY1;
  Z1 = myZ1;
  X2 = myX2;
  Y2 = myY2;
  Z2 = myZ2;
}
