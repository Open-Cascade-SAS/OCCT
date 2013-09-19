// Created on: 2013-07-12
// Created by: Anton POLETAEV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 65 (the "License") You may not use the content of this file
// except in compliance with the License Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file
//
// The Initial Developer of the Original Code is Open CASCADE SAS, having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement Please see the License for the specific terms
// and conditions governing the rights and limitations under the License

#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <gp_Pln.hxx>
#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_HANDLE(Graphic3d_ClipPlane, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ClipPlane, Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_CLIP_PLANE_COUNTER = 0;
};

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane()
: myEquation (0.0, 0.0, 1.0, 0.0),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myMaterial (Graphic3d_NOM_DEFAULT),
  myTexture (NULL),
  myHatch (Aspect_HS_HORIZONTAL),
  myHatchOn (Standard_False),
  myId(),
  myEquationMod(0),
  myAspectMod(0)
{
  MakeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Equation& theEquation)
: myEquation (theEquation),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myMaterial (Graphic3d_NOM_DEFAULT),
  myTexture (NULL),
  myHatch (Aspect_HS_HORIZONTAL),
  myHatchOn (Standard_False),
  myId(),
  myEquationMod(0),
  myAspectMod(0)
{
  MakeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Graphic3d_ClipPlane& theOther)
: myEquation (theOther.myEquation),
  myIsOn (theOther.myIsOn),
  myIsCapping (theOther.myIsCapping),
  myMaterial (theOther.myMaterial),
  myTexture (theOther.myTexture),
  myHatch (theOther.myHatch),
  myHatchOn (theOther.myHatchOn),
  myId(),
  myEquationMod (0),
  myAspectMod (0)
{
  MakeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const gp_Pln& thePlane)
: myEquation (),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myMaterial (Graphic3d_NOM_DEFAULT),
  myTexture (NULL),
  myHatch (Aspect_HS_HORIZONTAL),
  myHatchOn (Standard_False),
  myId(),
  myEquationMod(0),
  myAspectMod(0)
{
  MakeId();
  SetEquation (thePlane);
}

// =======================================================================
// function : SetEquation
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetEquation (const Equation& theEquation)
{
  myEquation = theEquation;
  myEquationMod++;
}

// =======================================================================
// function : SetPlane
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetEquation (const gp_Pln& thePlane)
{
  thePlane.Coefficients (myEquation[0],
                         myEquation[1],
                         myEquation[2],
                         myEquation[3]);
  myEquationMod++;
}

// =======================================================================
// function : SetOn
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetOn (const Standard_Boolean theIsOn)
{
  myIsOn = theIsOn;
}

// =======================================================================
// function : SetCapping
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCapping (const Standard_Boolean theIsOn)
{
  myIsCapping = theIsOn;
}

// =======================================================================
// function : ToPlane
// purpose  :
// =======================================================================
gp_Pln Graphic3d_ClipPlane::ToPlane() const
{
  return gp_Pln (myEquation[0],
                 myEquation[1],
                 myEquation[2],
                 myEquation[3]);
}

// =======================================================================
// function : Clone
// purpose  :
// =======================================================================
Handle(Graphic3d_ClipPlane) Graphic3d_ClipPlane::Clone() const
{
  return new Graphic3d_ClipPlane(*this);
}

// =======================================================================
// function : SetCappingMaterial
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingMaterial (const Graphic3d_MaterialAspect& theMat)
{
  myMaterial = theMat;
  myAspectMod++;
}

// =======================================================================
// function : SetCappingTexture
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingTexture (const Handle(Graphic3d_TextureMap)& theTexture)
{
  myTexture = theTexture;
  myAspectMod++;
}

// =======================================================================
// function : SetCappingHatch
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatch (const Aspect_HatchStyle theStyle)
{
  myHatch = theStyle;
  myAspectMod++;
}

// =======================================================================
// function : SetCappingHatchOn
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatchOn()
{
  myHatchOn = Standard_True;
  myAspectMod++;
}

// =======================================================================
// function : SetCappingHatchOff
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatchOff()
{
  myHatchOn = Standard_False;
  myAspectMod++;
}

// =======================================================================
// function : MakeId
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::MakeId()
{
  myId = TCollection_AsciiString ("Graphic3d_ClipPlane_") //DynamicType()->Name()
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_CLIP_PLANE_COUNTER));
}

// =======================================================================
// function : CappingAspect
// purpose  :
// =======================================================================
Handle(Graphic3d_AspectFillArea3d) Graphic3d_ClipPlane::CappingAspect() const
{
  Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
  anAspect->SetDistinguishOff();
  anAspect->SetFrontMaterial (myMaterial);
  anAspect->SetTextureMap (myTexture);
  anAspect->SetHatchStyle (myHatch);
  anAspect->SetInteriorStyle (myHatchOn ? Aspect_IS_HATCH : Aspect_IS_SOLID);
  anAspect->SetInteriorColor (myMaterial.Color());
  if (!myTexture.IsNull())
    anAspect->SetTextureMapOn();
  else
    anAspect->SetTextureMapOff();

  return anAspect;
}
