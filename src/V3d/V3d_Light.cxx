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

#include <V3d.hxx>
#include <V3d_Light.ixx>

//-Declarations


//-Aliases

//-Global data definitions

//-Local data definitions

//-Constructors

V3d_Light::V3d_Light(const Handle(V3d_Viewer)& VM) {

	MyType = V3d_AMBIENT ;
	VM->AddLight(this) ;
}

//-Methods, in order

Handle(Visual3d_Light) V3d_Light::Light() const {

	return MyLight ;
}

void V3d_Light::SetColor(const Quantity_TypeOfColor Type, const Standard_Real v1, const Standard_Real v2, const Standard_Real v3) {
  Standard_Real V1 = v1 ;
  Standard_Real V2 = v2 ;
  Standard_Real V3 = v3 ;

  if( V1 < 0. ) V1 = 0. ; else if( V1 > 1. ) V1 = 1. ;
  if( V2 < 0. ) V2 = 0. ; else if( V2 > 1. ) V2 = 1. ;
  if( V3 < 0. ) V3 = 0. ; else if( V3 > 1. ) V3 = 1. ;


  Quantity_Color C(V1,V2,V3,Type) ;
  MyLight->SetColor(C) ;
}

void V3d_Light::SetColor(const Quantity_NameOfColor Name) {
  Quantity_Color C(Name) ;
  MyLight->SetColor(C) ;
}

void V3d_Light::SetColor(const Quantity_Color& aColor) {
  MyLight->SetColor(aColor) ;
}

void V3d_Light::Color(const Quantity_TypeOfColor Type, Standard_Real& V1, Standard_Real& V2, Standard_Real& V3) const {

  Quantity_Color C ;
  C = MyLight->Color() ;
  C.Values(V1,V2,V3,Type) ;
}

void V3d_Light::Color(Quantity_NameOfColor& Name) const{

  Quantity_Color C ;
  C = MyLight->Color() ;
  Name = C.Name();
}

Quantity_Color V3d_Light::Color() const{

  return MyLight->Color();
}

V3d_TypeOfLight V3d_Light::Type() const {

  return MyType;
}


Standard_Boolean V3d_Light::Headlight() const {
  return MyLight->Headlight();
}

void V3d_Light::SetHeadlight (const Standard_Boolean theValue)
{
  MyLight->SetHeadlight (theValue);
}

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

Standard_Boolean V3d_Light::IsDisplayed() const {
  if( MyGraphicStructure.IsNull() ) return Standard_False;
  return MyGraphicStructure->IsDisplayed();
}
