// Created on: 2013-07-12
// Created by: Anton POLETAEV
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

#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <gp_Pln.hxx>
#include <Standard_Atomic.hxx>


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
: Standard_Transient(theOther),
  myEquation (theOther.myEquation),
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
