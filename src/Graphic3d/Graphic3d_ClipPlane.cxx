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

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ClipPlane,Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_CLIP_PLANE_COUNTER = 0;

  static Handle(Graphic3d_AspectFillArea3d) defaultAspect()
  {
    const Graphic3d_MaterialAspect aMaterial (Graphic3d_NOM_DEFAULT);
    Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
    anAspect->SetDistinguishOff();
    anAspect->SetFrontMaterial (aMaterial);
    anAspect->SetHatchStyle (Aspect_HS_HORIZONTAL);
    anAspect->SetInteriorStyle (Aspect_IS_SOLID);
    anAspect->SetInteriorColor (aMaterial.Color());
    anAspect->SetSuppressBackFaces (false);
    return anAspect;
  }
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane()
: myAspect (defaultAspect()),
  myEquation (0.0, 0.0, 1.0, 0.0),
  myFlags (Graphic3d_CappingFlags_None),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myEquationMod(0),
  myAspectMod(0)
{
  makeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Equation& theEquation)
: myAspect (defaultAspect()),
  myEquation (theEquation),
  myFlags (Graphic3d_CappingFlags_None),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myEquationMod(0),
  myAspectMod(0)
{
  makeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Graphic3d_ClipPlane& theOther)
: Standard_Transient(theOther),
  myAspect (defaultAspect()),
  myEquation (theOther.myEquation),
  myFlags (theOther.myFlags),
  myIsOn (theOther.myIsOn),
  myIsCapping (theOther.myIsCapping),
  myEquationMod (0),
  myAspectMod (0)
{
  makeId();
  *myAspect = *theOther.CappingAspect();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const gp_Pln& thePlane)
: myAspect (defaultAspect()),
  myEquation (),
  myFlags (Graphic3d_CappingFlags_None),
  myIsOn (Standard_True),
  myIsCapping (Standard_False),
  myEquationMod(0),
  myAspectMod(0)
{
  makeId();
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
  myAspect->SetFrontMaterial (theMat);
  myAspect->SetInteriorColor (theMat.Color());
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingTexture
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingTexture (const Handle(Graphic3d_TextureMap)& theTexture)
{
  myAspect->SetTextureMap (theTexture);
  if (!theTexture.IsNull())
  {
    myAspect->SetTextureMapOn();
  }
  else
  {
    myAspect->SetTextureMapOff();
  }
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingHatch
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatch (const Aspect_HatchStyle theStyle)
{
  myAspect->SetHatchStyle (theStyle);
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingHatchOn
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatchOn()
{
  myAspect->SetInteriorStyle (Aspect_IS_HATCH);
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingHatchOff
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingHatchOff()
{
  myAspect->SetInteriorStyle (Aspect_IS_SOLID);
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingAspect
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect)
{
  myAspect = theAspect;
  ++myAspectMod;
}

// =======================================================================
// function : setCappingFlag
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::setCappingFlag (bool theToUse, int theFlag)
{
  if (theToUse)
  {
    myFlags |= theFlag;
  }
  else
  {
    myFlags &= ~(theFlag);
  }
  ++myAspectMod;
}

// =======================================================================
// function : makeId
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::makeId()
{
  myId = TCollection_AsciiString ("Graphic3d_ClipPlane_") //DynamicType()->Name()
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_CLIP_PLANE_COUNTER));
}
