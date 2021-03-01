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
    Graphic3d_MaterialAspect aMaterial (Graphic3d_NameOfMaterial_DEFAULT);
    Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
    anAspect->SetDistinguishOff();
    anAspect->SetFrontMaterial (aMaterial);
    anAspect->SetHatchStyle (Aspect_HS_HORIZONTAL);
    anAspect->SetInteriorStyle (Aspect_IS_SOLID);
    anAspect->SetInteriorColor (Quantity_NOC_GRAY20);
    anAspect->SetFaceCulling (Graphic3d_TypeOfBackfacingModel_DoubleSided);
    return anAspect;
  }
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane()
: myAspect     (defaultAspect()),
  myPrevInChain(NULL),
  myPlane      (0.0, 0.0, 1.0, 0.0),
  myEquation   (0.0, 0.0, 1.0, 0.0),
  myEquationRev(0.0, 0.0,-1.0, 0.0),
  myChainLenFwd(1),
  myFlags      (Graphic3d_CappingFlags_None),
  myEquationMod(0),
  myAspectMod  (0),
  myIsOn       (Standard_True),
  myIsCapping  (Standard_False)
{
  makeId();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane (const Graphic3d_Vec4d& theEquation)
: myAspect     (defaultAspect()),
  myPrevInChain(NULL),
  myPlane      (theEquation.x(), theEquation.y(), theEquation.z(), theEquation.w()),
  myEquation   (theEquation),
  myEquationRev(0.0, 0.0,-1.0, 0.0),
  myChainLenFwd(1),
  myFlags      (Graphic3d_CappingFlags_None),
  myEquationMod(0),
  myAspectMod  (0),
  myIsOn       (Standard_True),
  myIsCapping  (Standard_False)
{
  makeId();
  updateInversedPlane();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const Graphic3d_ClipPlane& theOther)
: Standard_Transient(theOther),
  myAspect     (defaultAspect()),
  myPrevInChain(NULL),
  myPlane      (theOther.myPlane),
  myEquation   (theOther.myEquation),
  myEquationRev(theOther.myEquationRev),
  myChainLenFwd(1),
  myFlags      (theOther.myFlags),
  myEquationMod(0),
  myAspectMod  (0),
  myIsOn       (theOther.myIsOn),
  myIsCapping  (theOther.myIsCapping)
{
  makeId();
  *myAspect = *theOther.CappingAspect();
}

// =======================================================================
// function : Graphic3d_ClipPlane
// purpose  :
// =======================================================================
Graphic3d_ClipPlane::Graphic3d_ClipPlane(const gp_Pln& thePlane)
: myAspect     (defaultAspect()),
  myPrevInChain(NULL),
  myPlane      (thePlane),
  myChainLenFwd(1),
  myFlags      (Graphic3d_CappingFlags_None),
  myEquationMod(0),
  myAspectMod  (0),
  myIsOn       (Standard_True),
  myIsCapping  (Standard_False)
{
  thePlane.Coefficients (myEquation[0], myEquation[1], myEquation[2], myEquation[3]);
  updateInversedPlane();
  makeId();
}

// =======================================================================
// function : SetEquation
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetEquation (const Graphic3d_Vec4d& theEquation)
{
  myPlane = gp_Pln (theEquation.x(), theEquation.y(), theEquation.z(), theEquation.w());
  myEquation = theEquation;
  updateInversedPlane();
  myEquationMod++;
}

// =======================================================================
// function : SetPlane
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetEquation (const gp_Pln& thePlane)
{
  myPlane = thePlane;
  thePlane.Coefficients (myEquation[0], myEquation[1], myEquation[2], myEquation[3]);
  updateInversedPlane();
  myEquationMod++;
}

// =======================================================================
// function : SetOn
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetOn (const Standard_Boolean theIsOn)
{
  if (myPrevInChain != NULL)
  {
    throw Standard_ProgramError ("Graphic3d_ClipPlane::SetOn() - undefined operation for a plane in Union");
  }
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
// function : Clone
// purpose  :
// =======================================================================
Handle(Graphic3d_ClipPlane) Graphic3d_ClipPlane::Clone() const
{
  return new Graphic3d_ClipPlane(*this);
}

// =======================================================================
// function : SetCappingColor
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingColor (const Quantity_Color& theColor)
{
  myAspect->SetInteriorColor (theColor);
  myAspect->ChangeFrontMaterial().SetColor (theColor);
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingMaterial
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingMaterial (const Graphic3d_MaterialAspect& theMat)
{
  myAspect->SetFrontMaterial (theMat);
  if (myAspect->FrontMaterial().MaterialType() != Graphic3d_MATERIAL_ASPECT)
  {
    myAspect->SetInteriorColor (theMat.Color());
  }
  ++myAspectMod;
}

// =======================================================================
// function : SetCappingTexture
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingTexture (const Handle(Graphic3d_TextureMap)& theTexture)
{
  if (!theTexture.IsNull())
  {
    myAspect->SetTextureMapOn();
    Handle(Graphic3d_TextureSet) aTextureSet = myAspect->TextureSet();
    if (aTextureSet.IsNull() || aTextureSet->Size() != 1)
    {
      aTextureSet = new Graphic3d_TextureSet (theTexture);
    }
    else
    {
      aTextureSet->SetFirst (theTexture);
    }
    myAspect->SetTextureSet (aTextureSet);
  }
  else
  {
    myAspect->SetTextureMapOff();
    myAspect->SetTextureSet (Handle(Graphic3d_TextureSet)());
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
// function : SetCappingCustomHatch
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetCappingCustomHatch (const Handle(Graphic3d_HatchStyle)& theStyle)
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

// =======================================================================
// function : updateChainLen
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::updateChainLen()
{
  myChainLenFwd = !myNextInChain.IsNull() ? (myNextInChain->myChainLenFwd + 1) : 1;
  if (myPrevInChain != NULL)
  {
    myPrevInChain->updateChainLen();
  }
}

// =======================================================================
// function : SetChainNextPlane
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::SetChainNextPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  ++myEquationMod;
  if (!myNextInChain.IsNull())
  {
    myNextInChain->myPrevInChain = NULL;
  }
  myNextInChain = thePlane;
  if (!myNextInChain.IsNull())
  {
    myNextInChain->myPrevInChain = this;
  }
  updateChainLen();
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_ClipPlane::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, this)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myAspect.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myNextInChain.get())
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myPrevInChain)

  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myId)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPlane)

  OCCT_DUMP_FIELD_VALUES_NUMERICAL (theOStream, "Equation", 4, myEquation.x(), myEquation.y(), myEquation.z(), myEquation.w())
  OCCT_DUMP_FIELD_VALUES_NUMERICAL (theOStream, "EquationRev", 4, myEquationRev.x(), myEquationRev.y(), myEquationRev.z(), myEquationRev.w())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myChainLenFwd)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFlags)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myEquationMod)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAspectMod)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsOn)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsCapping)
}
