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
 
  V3d_PositionLight.cxx
  Created:    30-03-98  ZOV (ZELENKOV Oleg)

************************************************************************/

#include <V3d.hxx>
#include <V3d_PositionLight.ixx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <V3d_SpotLight.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Visual3d_ContextPick.hxx>
#include <V3d_BadValue.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TCollection_AsciiString.hxx>

V3d_PositionLight::V3d_PositionLight(const Handle(V3d_Viewer)& VM) : V3d_Light(VM) {
}

void V3d_PositionLight::SetTarget(const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  Standard_Real Xc,Yc,Zc, Xp,Yp,Zp;
  
  // Recalculation of the position
  MyTarget.Coord(Xc,Yc,Zc);
  Position (Xp,Yp,Zp) ;

  Xp = Xp + (X - Xc);
  Yp = Yp + (Y - Yc);
  Zp = Zp + (Z - Zc);

  // Affectation
  MyTarget.SetCoord(X,Y,Z);
  SetPosition(Xp,Yp,Zp) ;
}

void V3d_PositionLight::SetRadius(const Standard_Real Radius) {

  V3d_BadValue_Raise_if( Radius <= 0. , "V3d_PositionLight::SetRadius, bad radius");
  V3d_BadValue_Raise_if( MyType == V3d_DIRECTIONAL , "V3d_PositionLight::SetRadius, bad light type");

  Standard_Real X0,Y0,Z0, Xn,Yn,Zn, Xp,Yp,Zp;
  
  // The target point remains unchanged, only the position of the light is modified
  // by preserving the direction.
	Position (Xp,Yp,Zp);
  Graphic3d_Vector  D(MyTarget, Graphic3d_Vertex(Xp, Yp, Zp));
  D.Normalize();
  D.Coord(Xn,Yn,Zn);
  MyTarget.Coord(X0,Y0,Z0);
  Xn = X0 + Radius*Xn;
  Yn = Y0 + Radius*Yn;
  Zn = Z0 + Radius*Zn;

  SetPosition(Xn,Yn,Zn) ;
}

void V3d_PositionLight::OnHideFace(const Handle(V3d_View)& aView) {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // This is a visible point
  if ((VX*(X-Xp) < 0.) && (VY*(Y-Yp) < 0.) && (VZ*(Z-Zp) < 0.))
    SetPosition (X,Y,Z);
}

void V3d_PositionLight::OnSeeFace(const Handle(V3d_View)& aView) {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // This is a hidden point
  if ((VX*(X-Xp) > 0.) && (VY*(Y-Yp) > 0.) && (VZ*(Z-Zp) > 0.))
    SetPosition (X,Y,Z);
}

Standard_Boolean V3d_PositionLight::SeeOrHide(const Handle(V3d_View)& aView) const {

  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
	V3d_Light::SymetricPointOnSphere (aView, 
		MyTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // Is it a visible or a hidden point
  return ( (VX*(X-Xp) > 0.) || (VY*(Y-Yp) > 0.) || (VZ*(Z-Zp) > 0.) )?
    // the source is on the hidden face 
    Standard_False:
    // the source is on the visible face.
    Standard_True;
}

void V3d_PositionLight::Target(Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp)const  {
  
  MyTarget.Coord(Xp,Yp,Zp) ;
}

void V3d_PositionLight::Display( const Handle(V3d_View)& aView,
                                    const V3d_TypeOfRepresentation TPres )
{
  Graphic3d_Vertex PText ;
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  Standard_Real R1,G1,B1;
  V3d_TypeOfRepresentation Pres;
  V3d_TypeOfUpdate UpdSov;

//  Creation of a structure of markable elements (position of the
//  light, and the domain of lighting represented by a circle)
//  Creation of a structure snopick of non-markable elements (target, meridian and 
//  parallel).

  Pres = TPres;
  Handle(V3d_Viewer) TheViewer = aView->Viewer();
  UpdSov = TheViewer->UpdateMode();
  TheViewer->SetUpdateMode(V3d_WAIT);
  if (!MyGraphicStructure.IsNull()) {
    MyGraphicStructure->Disconnect(MyGraphicStructure1);
    MyGraphicStructure->Clear();
    MyGraphicStructure1->Clear();
    if (Pres == V3d_SAMELAST) Pres = MyTypeOfRepresentation;
  }
  else {
    if (Pres == V3d_SAMELAST) Pres = V3d_SIMPLE;
    Handle(Graphic3d_Structure) slight = new Graphic3d_Structure(TheViewer->Viewer());
    MyGraphicStructure = slight;
    Handle(Graphic3d_Structure) snopick = new Graphic3d_Structure(TheViewer->Viewer()); 
    MyGraphicStructure1 = snopick;
  }

  Handle(Graphic3d_Group) gradius, gExtArrow, gIntArrow;
  if (MyType != V3d_DIRECTIONAL
   && Pres == V3d_COMPLETE)
  {
    gradius   = MyGraphicStructure->NewGroup();
    gExtArrow = MyGraphicStructure->NewGroup();
    gIntArrow = MyGraphicStructure->NewGroup();
  }
  Handle(Graphic3d_Group) glight = MyGraphicStructure->NewGroup();
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE
   || Pres == V3d_PARTIAL)
  {
    gsphere = MyGraphicStructure->NewGroup();
  }
  
  Handle(Graphic3d_Group) gnopick = MyGraphicStructure1->NewGroup();
  
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  
// Display of the light position.

  this->Color(Quantity_TOC_RGB,R1,G1,B1);
  Quantity_Color Col1(R1,G1,B1,Quantity_TOC_RGB);
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,aView);

// Display of the marking sphere (limit at the circle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
      
    Rayon = this->Radius();
    aView->Proj(VX,VY,VZ);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);

    if (MyType != V3d_DIRECTIONAL) {

      //Display of the radius of the sphere (line + text)

      if (Pres == V3d_COMPLETE) {
        this->Position(X,Y,Z);
        Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(X0,Y0,Z0);
        aPrims->AddVertex(X,Y,Z);
        gnopick->AddPrimitiveArray(aPrims);
        V3d::ArrowOfRadius(gExtArrow,X-.1*(X-X0),Y-.1*(Y-Y0),Z-.1*(Z-Z0),X-X0,Y-Y0,Z-Z0,M_PI/15.,Rayon/20.);
        V3d::ArrowOfRadius(gIntArrow,X0,Y0,Z0,X0-X,Y0-Y,Z0-Z,M_PI/15.,Rayon/20.);
        TCollection_AsciiString ValOfRadius(Rayon);
        PText.SetCoord( .5*(X0+X), .5*(Y0+Y), .5*(Z0+Z) );
        gradius->Text(ValOfRadius.ToCString(),PText,0.01);
      }
    }

    // Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d(Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
    // Definition of the axis of circle
    aView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
      
    // Display of the parallel

    // Definition of the axis of circle
    aView->Proj(VX,VY,VZ);
    aView->Up(X1,Y1,Z1);
    DXRef = VY * Z1 - VZ * Y1;
    DYRef = VZ * X1 - VX * Z1;
    DZRef = VX * Y1 - VY * X1;
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
  }

  MyGraphicStructure->Connect(MyGraphicStructure1,Graphic3d_TOC_DESCENDANT);
  MyTypeOfRepresentation = Pres;
  MyGraphicStructure->Display();
  TheViewer->SetUpdateMode(UpdSov);
}


void V3d_PositionLight::Tracking( const Handle(V3d_View)& aView,
			  const V3d_TypeOfPickLight WhatPick,
			  const Standard_Integer Xpix,
			  const Standard_Integer Ypix) {

//  Quantity_Color   Col ;
  Standard_Real    xPos, yPos, zPos;
  Standard_Real    XPp,YPp,PXT,PYT,X,Y,Z,Rayon,Ylim;
  Standard_Real    XMinTrack,XMaxTrack,YMinTrack,YMaxTrack;
  Standard_Real    XT,YT,ZT,X0,Y0,Z0,XP,YP,ZP,VX,VY,VZ,A,B,C,Delta;
  Standard_Real    DX,DY,PXP,PYP,Xproj,Yproj;
  Standard_Real    A1,A2,B1,B2,Rap,OldRprj,NewRprj;
  Standard_Real    Xi,Yi,Zi,DeltaX,DeltaY,DeltaZ,Lambda;
  Standard_Integer IPX,IPY;
  
  aView->Convert(Xpix,Ypix,XPp,YPp);
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  aView->Project(X0,Y0,Z0,PXT,PYT);
  aView->Convert(PXT,PYT,IPX,IPY);
//      Coord 3d in the plane of projection of the target.
  aView->Convert(IPX,IPY,XT,YT,ZT);
  switch (WhatPick) {
  case V3d_POSITIONLIGHT :
	  // The Coordinates should remain inside of the sphere
    Rayon = Radius();
    XMinTrack = PXT - Rayon;
    XMaxTrack = PXT + Rayon;
    Ylim = Sqrt( Square(Rayon) - Square(XPp - PXT) );
    YMinTrack = PYT - Ylim;
    YMaxTrack = PYT + Ylim;
    if (XPp >= XMinTrack && XPp <= XMaxTrack) {
      if (YPp >= YMinTrack && YPp <= YMaxTrack) {
				aView->ProjReferenceAxe(Xpix,Ypix,XP,YP,ZP,VX,VY,VZ);
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
	  X = XP + Lambda*VX;
	  Y = YP + Lambda*VY;
	  Z = ZP + Lambda*VZ;
	  SetPosition(X,Y,Z);

		if (MyType == V3d_SPOT)
	    ((V3d_SpotLight*)this)->SetDirection(X0-X,Y0-Y,Z0-Z);

	  Display(aView,MyTypeOfRepresentation);
	  (aView->Viewer())->UpdateLights();
	}
      }
    }
    break;

  case V3d_SPACELIGHT :
    aView->Convert(PXT,PYT,IPX,IPY);
//               In this case Xpix,Ypix correspond to a distance, relative
//               to the translation that is planned to be performed on the sphere. 
    aView->Convert(IPX+Xpix,IPY+Ypix,X,Y,Z);
    X = X+X0-XT;
    Y = Y+Y0-YT; 
    Z = Z+Z0-ZT;
    SetTarget(X,Y,Z);
    Display(aView,MyTypeOfRepresentation);
    (aView->Viewer())->UpdateLights();
    break;

  case V3d_ExtRADIUSLIGHT :
		if (MyType == V3d_DIRECTIONAL)
			break;
//             it is attempted to preserve the target direction position of the  
//             source ==> the point is projected on the target source direction.
    this->Position(Xi,Yi,Zi);
    aView->Project(Xi,Yi,Zi,PXP,PYP);
    DX = PXP - PXT;
    DY = PYP - PYT;
    A1 = DY/DX ; B1 = PYT - A1*PXT;
    A2 = -DX/DY; B2 = YPp - A2*XPp;
    Xproj = (B2 - B1) / (A1 - A2);
    Yproj = A1*Xproj + B1;
    if ( (DX*(Xproj-PXT) > 0.) && (DY*(Yproj-PYT) > 0.) ) {
      OldRprj = Sqrt ( Square (PXP-PXT) + Square (PYP-PYT) );
      NewRprj = Sqrt ( Square (Xproj-PXT) + Square (Yproj-PYT) );
      Rap = NewRprj/OldRprj;
      Rayon = Radius();
      Rayon = Rayon * Rap;
      SetRadius(Rayon);
      Display(aView,MyTypeOfRepresentation);
      (aView->Viewer())->UpdateLights();
    }
    break;

  case V3d_IntRADIUSLIGHT :
		if (MyType == V3d_DIRECTIONAL)
			break;
//             it is attempted to preserve the target direction position of the  
//             source ==> the point is projected on the target source direction.
    Position(Xi,Yi,Zi);
    aView->Project(Xi,Yi,Zi,PXP,PYP);
    DX = PXP - PXT;
    DY = PYP - PYT;
    A1 = DY/DX ; B1 = PYT - A1*PXT;
    A2 = -DX/DY; B2 = YPp - A2*XPp;
    Xproj = (B2 - B1) / (A1 - A2);
    Yproj = A1*Xproj + B1;
    if ( (DX*(Xproj-PXP) < 0.) && (DY*(Yproj-PYP) < 0.) ) {
      OldRprj = Sqrt ( Square (PXP-PXT) + Square (PYP-PYT) );
      NewRprj = Sqrt ( Square (Xproj-PXP) + Square (Yproj-PYP) );
      Rap = NewRprj/OldRprj;
      Rayon = Radius();
      Rayon = Rayon * Rap;
//                 the source should remain at a fixed position, 
//                 only the target is modified.
      Position (xPos, yPos, zPos);
      Graphic3d_Vector  Dir(Graphic3d_Vertex(xPos,yPos,zPos), MyTarget);
      Dir.Normalize();
      Dir.Coord(X,Y,Z);
      X = Xi + Rayon*X;
      Y = Yi + Rayon*Y;
      Z = Zi + Rayon*Z;
//                 the source should remain at a fixed position, 
//                 only the target is modified.
      MyTarget.SetCoord(X,Y,Z);
      Display(aView,MyTypeOfRepresentation);		 
      (aView->Viewer())->UpdateLights();
    }
		break;

  case V3d_RADIUSTEXTLIGHT :
    break;

  case V3d_NOTHING : 
    break;
  }
}

Standard_Real V3d_PositionLight::Radius() const {

  Standard_Real  Xp,Yp,Zp, Xc,Yc,Zc;
  
  Position (Xp,Yp,Zp);
  MyTarget.Coord(Xc,Yc,Zc);

  return Sqrt (Square(Xc - Xp) + Square(Yc - Yp) + Square(Zc - Zp));
}

void V3d_PositionLight::Erase() {

  if (!MyGraphicStructure.IsNull()) MyGraphicStructure->Erase();
  if (!MyGraphicStructure1.IsNull()) MyGraphicStructure1->Erase();
}

