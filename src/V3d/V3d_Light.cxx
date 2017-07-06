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

/***********************************************************************
     FONCTION :
     ----------
        Classe V3d_SpotLight :
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
************************************************************************/
/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Graphic3d_Structure.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Light,Standard_Transient)

// =======================================================================
// function : V3d_Light
// purpose  :
// =======================================================================
V3d_Light::V3d_Light (const Handle(V3d_Viewer)& theViewer)
{
  SetType (V3d_AMBIENT);
  theViewer->AddLight (this);
}

// =======================================================================
// function : SetType
// purpose  :
// =======================================================================
void V3d_Light::SetType (const V3d_TypeOfLight theType)
{
  myLight.Type = (Graphic3d_TypeOfLightSource)theType;
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void V3d_Light::SetColor (const Quantity_Color& theColor)
{
  myLight.Color.r() = static_cast<Standard_ShortReal> (theColor.Red());
  myLight.Color.g() = static_cast<Standard_ShortReal> (theColor.Green());
  myLight.Color.b() = static_cast<Standard_ShortReal> (theColor.Blue());
}

// =======================================================================
// function : Type
// purpose  :
// =======================================================================
V3d_TypeOfLight V3d_Light::Type() const
{
  return (V3d_TypeOfLight)myLight.Type;
}

// =======================================================================
// function : SetIntensity
// purpose  :
// =======================================================================
void V3d_Light::SetIntensity (const Standard_Real theValue)
{
  Standard_ASSERT_RAISE (theValue > 0.,
                         "V3d_Light::SetIntensity, "
                         "Negative value for intensity");

  myLight.Intensity = static_cast<Standard_ShortReal> (theValue);
}

// =======================================================================
// function : Intensity
// purpose  :
// =======================================================================
Standard_Real V3d_Light::Intensity() const
{
  return myLight.Intensity;
}

// =======================================================================
// function : Smoothness
// purpose  :
// =======================================================================
Standard_Real V3d_Light::Smoothness() const
{
  return myLight.Smoothness;
}

// =======================================================================
// function : Headlight
// purpose  :
// =======================================================================
Standard_Boolean V3d_Light::Headlight() const
{
  return myLight.IsHeadlight;
}

// =======================================================================
// function : SetHeadlight
// purpose  :
// =======================================================================
void V3d_Light::SetHeadlight (const Standard_Boolean theValue)
{
  myLight.IsHeadlight = theValue;
}

// =======================================================================
// function : SymetricPointOnSphere
// purpose  :
// =======================================================================
void V3d_Light::SymetricPointOnSphere (const Handle(V3d_View)& aView, const Graphic3d_Vertex &Center, const Graphic3d_Vertex &aPoint, const Standard_Real Rayon, Standard_Real& X, Standard_Real& Y, Standard_Real& Z, Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ ) {

  Standard_Real X0,Y0,Z0,XP,YP,ZP;
  Standard_Real PXP,PYP,DeltaX,DeltaY,DeltaZ;
  Standard_Real A,B,C,Delta,Lambda;
  Standard_Integer IPX,IPY;

  Center.Coord(X0,Y0,Z0);
  aPoint.Coord(XP,YP,ZP);
  aView->Project(XP,YP,ZP,PXP,PYP);
  aView->Convert(PXP,PYP,IPX,IPY);
  aView->ProjReferenceAxe(IPX,IPY,X,Y,Z,VX,VY,VZ);
  DeltaX = X0 - XP;
  DeltaY = Y0 - YP;
  DeltaZ = Z0 - ZP;

//      The point of intersection of straight lines defined by :
//      - Straight line passing by the point of projection and the eye
//        if this is a perspective, parralel to the normal of the view 
//        if this is an axonometric view.
//        position in the view is parallel to the normal of the view
//      - The distance position of the target camera is equal to the radius.

  A = VX*VX + VY*VY + VZ*VZ ;
  B = -2. * (VX*DeltaX + VY*DeltaY + VZ*DeltaZ);
  C = DeltaX*DeltaX + DeltaY*DeltaY + DeltaZ*DeltaZ 
    - Rayon*Rayon ;
  Delta = B*B - 4.*A*C;
  if ( Delta >= 0 ) {
    Lambda = (-B + Sqrt(Delta))/(2.*A);
    if ( Lambda >= -0.0001 && Lambda <= 0.0001 ) 
      Lambda = (-B - Sqrt(Delta))/(2.*A);
    X = XP + Lambda*VX;
    Y = YP + Lambda*VY;
    Z = ZP + Lambda*VZ;
  }
  else {
    X = XP; Y = YP; Z = ZP;
  }
}

// =======================================================================
// function : IsDisplayed
// purpose  :
// =======================================================================
Standard_Boolean V3d_Light::IsDisplayed() const
{
  if (myGraphicStructure.IsNull())
  {
    return Standard_False;
  }

  return myGraphicStructure->IsDisplayed();
}
