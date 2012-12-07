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

#include <Graphic3d_Texture1Dsegment.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>
#include <Graphic3d_TextureParams.hxx>

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
