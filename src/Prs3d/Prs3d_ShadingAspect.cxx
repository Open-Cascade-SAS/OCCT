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


#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

//=======================================================================
//function : Prs3d_ShadingAspect
//purpose  : 
//=======================================================================
Prs3d_ShadingAspect::Prs3d_ShadingAspect () {


  Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
  Quantity_Color Col;
  // Ceci permet de recuperer la couleur associee
  // au materiau defini par defaut.
//POP K4L
  Col = aMat.AmbientColor ();
//  Col = aMat.Color ();
  myAspect = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
					     Col,
					     Col,
					     Aspect_TOL_SOLID,
					     1.0,
					     aMat,
					     aMat);
}

Prs3d_ShadingAspect::Prs3d_ShadingAspect( const Handle( Graphic3d_AspectFillArea3d )& theAspect )
{
  myAspect = theAspect;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void Prs3d_ShadingAspect::SetColor(const Quantity_NameOfColor aColor,
					     const Aspect_TypeOfFacingModel aModel) {

  SetColor(Quantity_Color(aColor),aModel);
}

void Prs3d_ShadingAspect::SetColor(const Quantity_Color &aColor,
					     const Aspect_TypeOfFacingModel aModel) {
  if( aModel != Aspect_TOFM_BOTH_SIDE ) {
    myAspect->SetDistinguishOn();
  }
  if( aModel == Aspect_TOFM_FRONT_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    Graphic3d_MaterialAspect front = myAspect->FrontMaterial();
    front.SetColor(aColor);
    myAspect->SetFrontMaterial(front);
    myAspect->SetInteriorColor( aColor );
  }

  if( aModel == Aspect_TOFM_BACK_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    Graphic3d_MaterialAspect back = myAspect->BackMaterial();
    back.SetColor(aColor);
    myAspect->SetBackMaterial(back);
    myAspect->SetBackInteriorColor( aColor );
  }
}

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

void Prs3d_ShadingAspect::SetMaterial(
                   const Graphic3d_NameOfMaterial aMaterial,
			 const Aspect_TypeOfFacingModel aModel ) {
  SetMaterial(Graphic3d_MaterialAspect(aMaterial),aModel);
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void Prs3d_ShadingAspect::SetMaterial(
                   const Graphic3d_MaterialAspect& aMaterial,
			 const Aspect_TypeOfFacingModel aModel ) {

  if( aModel != Aspect_TOFM_BOTH_SIDE ) {
    myAspect->SetDistinguishOn();
  }
  if( aModel == Aspect_TOFM_FRONT_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    myAspect->SetFrontMaterial(aMaterial);
  }

  if( aModel == Aspect_TOFM_BACK_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    myAspect->SetBackMaterial(aMaterial);
  }
}

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

void Prs3d_ShadingAspect::SetTransparency(const Standard_Real aValue,
						      const Aspect_TypeOfFacingModel aModel ) {

  if( aModel != Aspect_TOFM_BOTH_SIDE ) {
    myAspect->SetDistinguishOn();
  }
  if( aModel == Aspect_TOFM_FRONT_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    Graphic3d_MaterialAspect front = myAspect->FrontMaterial();
    front.SetTransparency(aValue);
    myAspect->SetFrontMaterial(front);
  }

  if( aModel == Aspect_TOFM_BACK_SIDE || aModel == Aspect_TOFM_BOTH_SIDE ) {
    Graphic3d_MaterialAspect back = myAspect->BackMaterial();
    back.SetTransparency(aValue);
    myAspect->SetBackMaterial(back);
  }
}

Standard_Real Prs3d_ShadingAspect::Transparency(const Aspect_TypeOfFacingModel aModel ) const {
Standard_Real aValue(0.);
  switch (aModel) {
    case Aspect_TOFM_BOTH_SIDE:
    case Aspect_TOFM_FRONT_SIDE:
	aValue = myAspect->FrontMaterial().Transparency();
	break;
    case Aspect_TOFM_BACK_SIDE:
	aValue = myAspect->BackMaterial().Transparency();
	break;
  }
  return aValue;
}

//=======================================================================
//function : SetAspect
//purpose  : 
//=======================================================================

void Prs3d_ShadingAspect::SetAspect( const Handle( Graphic3d_AspectFillArea3d )& theAspect )
{
  myAspect = theAspect;
}

Handle (Graphic3d_AspectFillArea3d) Prs3d_ShadingAspect::Aspect () const {
  return myAspect;
}
