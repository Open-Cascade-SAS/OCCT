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


#define IMP280200	//GG Add Plane(),ScaleT(),ScaleS() methods

#include <Graphic3d_Texture2Dplane.ixx>
#include <Graphic3d_TypeOfTextureMode.hxx>


Graphic3d_Texture2Dplane::Graphic3d_Texture2Dplane(const Handle(Graphic3d_StructureManager)& SM,const Standard_CString FileName)
: Graphic3d_Texture2D(SM, FileName, Graphic3d_TOT_2D_MIPMAP)
{
  MyCInitTexture.doModulate = 1;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_OBJECT;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 1.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 1.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}



Graphic3d_Texture2Dplane::Graphic3d_Texture2Dplane(const Handle(Graphic3d_StructureManager)& SM,const Graphic3d_NameOfTexture2D NOT)
: Graphic3d_Texture2D(SM, NOT, Graphic3d_TOT_2D_MIPMAP)
{
  MyCInitTexture.doModulate = 1;
  MyCInitTexture.doRepeat = 1;
  MyCInitTexture.Mode = (int)Graphic3d_TOTM_OBJECT;
  MyCInitTexture.doLinear = 1;
  MyCInitTexture.sx = 1.0F;
  MyCInitTexture.sy = 1.0F;
  MyCInitTexture.tx = 0.0F;
  MyCInitTexture.ty = 0.0F;
  MyCInitTexture.angle = 0.0F;
  MyCInitTexture.sparams[0] = 1.0F;
  MyCInitTexture.sparams[1] = 0.0F;
  MyCInitTexture.sparams[2] = 0.0F;
  MyCInitTexture.sparams[3] = 0.0F;
  MyCInitTexture.tparams[0] = 0.0F;
  MyCInitTexture.tparams[1] = 1.0F;
  MyCInitTexture.tparams[2] = 0.0F;
  MyCInitTexture.tparams[3] = 0.0F;
  Update();
}


void Graphic3d_Texture2Dplane::SetPlaneS(const Standard_ShortReal A,const Standard_ShortReal B,const Standard_ShortReal C,const Standard_ShortReal D) 
{
  MyCInitTexture.sparams[0] = A;
  MyCInitTexture.sparams[1] = B;
  MyCInitTexture.sparams[2] = C;
  MyCInitTexture.sparams[3] = D;
#ifdef IMP280200
  myPlaneName = Graphic3d_NOTP_UNKNOWN;
#endif
  Update();
}


void Graphic3d_Texture2Dplane::SetPlaneT(const Standard_ShortReal A,const Standard_ShortReal B,const Standard_ShortReal C,const Standard_ShortReal D)
{
  MyCInitTexture.tparams[0] = A;
  MyCInitTexture.tparams[1] = B;
  MyCInitTexture.tparams[2] = C;
  MyCInitTexture.tparams[3] = D;
#ifdef IMP280200
  myPlaneName = Graphic3d_NOTP_UNKNOWN;
#endif
  Update();
}


void Graphic3d_Texture2Dplane::SetPlane(const Graphic3d_NameOfTexturePlane APlane) 
{
  switch (APlane)
    {
    case Graphic3d_NOTP_XY:
      MyCInitTexture.sparams[0] = 1.0F;
      MyCInitTexture.sparams[1] = 0.0F;
      MyCInitTexture.sparams[2] = 0.0F;
      MyCInitTexture.sparams[3] = 0.0F;
      MyCInitTexture.tparams[0] = 0.0F;
      MyCInitTexture.tparams[1] = 1.0F;
      MyCInitTexture.tparams[2] = 0.0F;
      MyCInitTexture.tparams[3] = 0.0F;
      break;

    case Graphic3d_NOTP_YZ:
      MyCInitTexture.sparams[0] = 0.0F;
      MyCInitTexture.sparams[1] = 1.0F;
      MyCInitTexture.sparams[2] = 0.0F;
      MyCInitTexture.sparams[3] = 0.0F;
      MyCInitTexture.tparams[0] = 0.0F;
      MyCInitTexture.tparams[1] = 0.0F;
      MyCInitTexture.tparams[2] = 1.0F;
      MyCInitTexture.tparams[3] = 0.0F;
      break;

    case Graphic3d_NOTP_ZX:
      MyCInitTexture.sparams[0] = 0.0F;
      MyCInitTexture.sparams[1] = 0.0F;
      MyCInitTexture.sparams[2] = 1.0F;
      MyCInitTexture.sparams[3] = 0.0F;
      MyCInitTexture.tparams[0] = 1.0F;
      MyCInitTexture.tparams[1] = 0.0F;
      MyCInitTexture.tparams[2] = 0.0F;
      MyCInitTexture.tparams[3] = 0.0F;
      break;
    default:
      break;
    }

#ifdef IMP280200
    myPlaneName = APlane;
#endif

  Update();
}


void Graphic3d_Texture2Dplane::SetScaleS(const Standard_ShortReal val) 
{
  MyCInitTexture.sx = val;
  Update();
}


void Graphic3d_Texture2Dplane::SetScaleT(const Standard_ShortReal val) 
{
  MyCInitTexture.sy = val;
  Update();
}


void Graphic3d_Texture2Dplane::SetTranslateS(const Standard_ShortReal val)
{
  MyCInitTexture.tx = val;
  Update();
}


void Graphic3d_Texture2Dplane::SetTranslateT(const Standard_ShortReal val) 
{
  MyCInitTexture.ty = val;
  Update();
}


void Graphic3d_Texture2Dplane::SetRotation(const Standard_ShortReal val) 
{
  MyCInitTexture.angle = val;
  Update();
}

void Graphic3d_Texture2Dplane::PlaneS(Standard_ShortReal& A,Standard_ShortReal& B,Standard_ShortReal& C,Standard_ShortReal& D) const
{
  A = MyCInitTexture.sparams[0];
  B = MyCInitTexture.sparams[1];
  C = MyCInitTexture.sparams[2];
  D = MyCInitTexture.sparams[3];
}

void Graphic3d_Texture2Dplane::PlaneT(Standard_ShortReal& A,Standard_ShortReal& B,Standard_ShortReal& C,Standard_ShortReal& D) const
{
  A = MyCInitTexture.tparams[0];
  B = MyCInitTexture.tparams[1];
  C = MyCInitTexture.tparams[2];
  D = MyCInitTexture.tparams[3];
}

void Graphic3d_Texture2Dplane::TranslateS(Standard_ShortReal& val) const
{
  val = MyCInitTexture.tx;
}

void Graphic3d_Texture2Dplane::TranslateT(Standard_ShortReal& val) const
{
  val = MyCInitTexture.ty;
}

void Graphic3d_Texture2Dplane::Rotation(Standard_ShortReal& val) const
{
  val = MyCInitTexture.angle;
}

#ifdef IMP280200
Graphic3d_NameOfTexturePlane Graphic3d_Texture2Dplane::Plane() const {

  return myPlaneName;
}

void Graphic3d_Texture2Dplane::ScaleS(Standard_ShortReal& val) const
{
  val = MyCInitTexture.sx;
}

void Graphic3d_Texture2Dplane::ScaleT(Standard_ShortReal& val) const
{
  val = MyCInitTexture.sy;
}
#endif




