// Copyright (c) 1995-1999 Matra Datavision
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

#include <Prs3d_ShadingAspect.hxx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_ShadingAspect, Prs3d_BasicAspect)

//=======================================================================
//function : Prs3d_ShadingAspect
//purpose  :
//=======================================================================
Prs3d_ShadingAspect::Prs3d_ShadingAspect()
{
  const Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
  const Quantity_Color aColor = aMat.AmbientColor();
  myAspect = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
					     aColor,
					     aColor,
					     Aspect_TOL_SOLID,
					     1.0,
					     aMat,
					     aMat);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void Prs3d_ShadingAspect::SetColor (const Quantity_Color& theColor,
                                    const Aspect_TypeOfFacingModel theModel)
{
  if (theModel != Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->SetDistinguishOn();
  }

  if (theModel == Aspect_TOFM_FRONT_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->ChangeFrontMaterial().SetColor (theColor);
    myAspect->SetInteriorColor (theColor);
  }

  if (theModel == Aspect_TOFM_BACK_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->ChangeBackMaterial().SetColor (theColor);
    myAspect->SetBackInteriorColor (theColor);
  }
}

//=======================================================================
//function : Color
//purpose  :
//=======================================================================
const Quantity_Color& Prs3d_ShadingAspect::Color (const Aspect_TypeOfFacingModel theModel) const
{
  switch (theModel)
  {
    default:
    case Aspect_TOFM_BOTH_SIDE:
    case Aspect_TOFM_FRONT_SIDE:
      return myAspect->FrontMaterial().Color();
    case Aspect_TOFM_BACK_SIDE:
      return myAspect->BackMaterial().Color();
  }
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void Prs3d_ShadingAspect::SetMaterial (const Graphic3d_MaterialAspect& theMaterial,
                                       const Aspect_TypeOfFacingModel  theModel)
{
  if (theModel != Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->SetDistinguishOn();
  }
  if (theModel == Aspect_TOFM_FRONT_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->SetFrontMaterial(theMaterial);
  }

  if (theModel == Aspect_TOFM_BACK_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->SetBackMaterial (theMaterial);
  }
}

//=======================================================================
//function : Material
//purpose  :
//=======================================================================
const Graphic3d_MaterialAspect& Prs3d_ShadingAspect::Material (const Aspect_TypeOfFacingModel theModel) const
{
  switch (theModel)
  {
    default:
    case Aspect_TOFM_BOTH_SIDE:
    case Aspect_TOFM_FRONT_SIDE:
      return myAspect->FrontMaterial();
    case Aspect_TOFM_BACK_SIDE:
      return myAspect->BackMaterial();
  }
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void Prs3d_ShadingAspect::SetTransparency (const Standard_Real theValue,
                                           const Aspect_TypeOfFacingModel theModel)
{
  if (theModel != Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->SetDistinguishOn();
  }

  if (theModel == Aspect_TOFM_FRONT_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->ChangeFrontMaterial().SetTransparency (Standard_ShortReal(theValue));
  }

  if (theModel == Aspect_TOFM_BACK_SIDE
   || theModel == Aspect_TOFM_BOTH_SIDE)
  {
    myAspect->ChangeBackMaterial().SetTransparency (Standard_ShortReal(theValue));
  }
}

//=======================================================================
//function : Transparency
//purpose  :
//=======================================================================
Standard_Real Prs3d_ShadingAspect::Transparency (const Aspect_TypeOfFacingModel theModel) const
{
  switch (theModel)
  {
    case Aspect_TOFM_BOTH_SIDE:
    case Aspect_TOFM_FRONT_SIDE:
      return myAspect->FrontMaterial().Transparency();
    case Aspect_TOFM_BACK_SIDE:
      return myAspect->BackMaterial().Transparency();
  }
  return 0.0;
}
