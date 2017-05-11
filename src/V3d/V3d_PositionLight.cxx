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

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_PositionLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_PositionLight,V3d_Light)

// =======================================================================
// function : V3d_PositionLight
// purpose  :
// =======================================================================
V3d_PositionLight::V3d_PositionLight (const Handle(V3d_Viewer)& theViewer)
: V3d_Light(theViewer)
{
}

// =======================================================================
// function : SetTarget
// purpose  :
// =======================================================================
void V3d_PositionLight::SetTarget (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
{
  Standard_Real Xc,Yc,Zc, Xp,Yp,Zp;
  
  // Recalculation of the position
  myTarget.Coord(Xc,Yc,Zc);
  Position (Xp,Yp,Zp) ;

  Xp = Xp + (theX - Xc);
  Yp = Yp + (theY - Yc);
  Zp = Zp + (theZ - Zc);

  // Affectation
  myTarget.SetCoord(theX,theY,theZ);
  SetPosition(Xp,Yp,Zp) ;
}

// =======================================================================
// function : SetRadius
// purpose  :
// =======================================================================
void V3d_PositionLight::SetRadius (const Standard_Real theRadius)
{
  V3d_BadValue_Raise_if( theRadius <= 0. , "V3d_PositionLight::SetRadius, bad radius");
  V3d_BadValue_Raise_if( Type() == V3d_DIRECTIONAL , "V3d_PositionLight::SetRadius, bad light type");

  Standard_Real X0,Y0,Z0, Xn,Yn,Zn, Xp,Yp,Zp;
  
  // The target point remains unchanged, only the position of the light is modified
  // by preserving the direction.
	Position (Xp,Yp,Zp);
  Graphic3d_Vector  D(myTarget, Graphic3d_Vertex(Xp, Yp, Zp));
  D.Normalize();
  D.Coord(Xn,Yn,Zn);
  myTarget.Coord(X0,Y0,Z0);
  Xn = X0 + theRadius*Xn;
  Yn = Y0 + theRadius*Yn;
  Zn = Z0 + theRadius*Zn;

  SetPosition(Xn,Yn,Zn) ;
}

// =======================================================================
// function : OnHideFace
// purpose  :
// =======================================================================
void V3d_PositionLight::OnHideFace (const Handle(V3d_View)& theView)
{
  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
  V3d_Light::SymetricPointOnSphere (theView, 
    myTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // This is a visible point
  if ((VX*(X-Xp) < 0.) && (VY*(Y-Yp) < 0.) && (VZ*(Z-Zp) < 0.))
    SetPosition (X,Y,Z);
}

// =======================================================================
// function : OnSeeFace
// purpose  :
// =======================================================================
void V3d_PositionLight::OnSeeFace (const Handle(V3d_View)& theView)
{
  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
  V3d_Light::SymetricPointOnSphere (theView, 
    myTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // This is a hidden point
  if ((VX*(X-Xp) > 0.) && (VY*(Y-Yp) > 0.) && (VZ*(Z-Zp) > 0.))
    SetPosition (X,Y,Z);
}

// =======================================================================
// function : SeeOrHide
// purpose  :
// =======================================================================
Standard_Boolean V3d_PositionLight::SeeOrHide (const Handle(V3d_View)& theView) const
{
  Standard_Real Xp,Yp,Zp, X,Y,Z, VX,VY,VZ;
  
  Position (Xp,Yp,Zp);
  V3d_Light::SymetricPointOnSphere (theView, 
    myTarget, Graphic3d_Vertex(Xp,Yp,Yp), Radius(), X,Y,Z, VX,VY,VZ);

  // Is it a visible or a hidden point
  return ( (VX*(X-Xp) > 0.) || (VY*(Y-Yp) > 0.) || (VZ*(Z-Zp) > 0.) )?
    // the source is on the hidden face 
    Standard_False:
    // the source is on the visible face.
    Standard_True;
}

// =======================================================================
// function : Target
// purpose  :
// =======================================================================
void V3d_PositionLight::Target (Standard_Real& theXp, Standard_Real& theYp, Standard_Real& theZp) const
{
  myTarget.Coord (theXp, theYp, theZp);
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void V3d_PositionLight::Display (const Handle(V3d_View)& theView, const V3d_TypeOfRepresentation theTPres)
{
  Graphic3d_Vertex PText ;
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  V3d_TypeOfRepresentation Pres;

//  Creation of a structure of markable elements (position of the
//  light, and the domain of lighting represented by a circle)
//  Creation of a structure snopick of non-markable elements (target, meridian and 
//  parallel).

  Pres = theTPres;
  Handle(V3d_Viewer) TheViewer = theView->Viewer();
  if (!myGraphicStructure.IsNull()) {
    myGraphicStructure->Disconnect(myGraphicStructure1);
    myGraphicStructure->Clear();
    myGraphicStructure1->Clear();
    if (Pres == V3d_SAMELAST) Pres = myTypeOfRepresentation;
  }
  else {
    if (Pres == V3d_SAMELAST) Pres = V3d_SIMPLE;
    Handle(Graphic3d_Structure) slight = new Graphic3d_Structure(TheViewer->StructureManager());
    myGraphicStructure = slight;
    Handle(Graphic3d_Structure) snopick = new Graphic3d_Structure(TheViewer->StructureManager()); 
    myGraphicStructure1 = snopick;
  }

  Handle(Graphic3d_Group) gradius, gExtArrow, gIntArrow;
  if (Type() != V3d_DIRECTIONAL
   && Pres == V3d_COMPLETE)
  {
    gradius   = myGraphicStructure->NewGroup();
    gExtArrow = myGraphicStructure->NewGroup();
    gIntArrow = myGraphicStructure->NewGroup();
  }
  Handle(Graphic3d_Group) glight = myGraphicStructure->NewGroup();
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE
   || Pres == V3d_PARTIAL)
  {
    gsphere = myGraphicStructure->NewGroup();
  }
  
  Handle(Graphic3d_Group) gnopick = myGraphicStructure1->NewGroup();
  
  X0 = myTarget.X();
  Y0 = myTarget.Y();
  Z0 = myTarget.Z();
  
// Display of the light position.

  const Quantity_Color Col1 = this->Color();
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,theView);

// Display of the marking sphere (limit at the circle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
      
    Rayon = this->Radius();
    theView->Proj(VX,VY,VZ);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);

    if (Type() != V3d_DIRECTIONAL) {

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
    theView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
      
    // Display of the parallel

    // Definition of the axis of circle
    theView->Proj(VX,VY,VZ);
    theView->Up(X1,Y1,Z1);
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

  myGraphicStructure->Connect(myGraphicStructure1,Graphic3d_TOC_DESCENDANT);
  myTypeOfRepresentation = Pres;
  myGraphicStructure->Display();
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void V3d_PositionLight::Tracking (const Handle(V3d_View)& theView,
                                  const V3d_TypeOfPickLight theWhatPick,
                                  const Standard_Integer theXpix,
                                  const Standard_Integer theYpix)
{
//  Quantity_Color   Col ;
  Standard_Real    xPos, yPos, zPos;
  Standard_Real    XPp,YPp,PXT,PYT,X,Y,Z,Rayon,Ylim;
  Standard_Real    XMinTrack,XMaxTrack,YMinTrack,YMaxTrack;
  Standard_Real    XT,YT,ZT,X0,Y0,Z0,XP,YP,ZP,VX,VY,VZ,A,B,C,Delta;
  Standard_Real    DX,DY,PXP,PYP,Xproj,Yproj;
  Standard_Real    A1,A2,B1,B2,Rap,OldRprj,NewRprj;
  Standard_Real    Xi,Yi,Zi,DeltaX,DeltaY,DeltaZ,Lambda;
  Standard_Integer IPX,IPY;
  
  theView->Convert(theXpix,theYpix,XPp,YPp);
  X0 = myTarget.X();
  Y0 = myTarget.Y();
  Z0 = myTarget.Z();
  theView->Project(X0,Y0,Z0,PXT,PYT);
  theView->Convert(PXT,PYT,IPX,IPY);
//      Coord 3d in the plane of projection of the target.
  theView->Convert(IPX,IPY,XT,YT,ZT);
  switch (theWhatPick) {
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
				theView->ProjReferenceAxe(theXpix,theYpix,XP,YP,ZP,VX,VY,VZ);
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

		if (Type() == V3d_SPOT)
	    ((V3d_SpotLight*)this)->SetDirection(X0-X,Y0-Y,Z0-Z);

	  Display(theView,myTypeOfRepresentation);
	  (theView->Viewer())->UpdateLights();
	}
      }
    }
    break;

  case V3d_SPACELIGHT :
    theView->Convert(PXT,PYT,IPX,IPY);
//               In this case Xpix,Ypix correspond to a distance, relative
//               to the translation that is planned to be performed on the sphere. 
    theView->Convert(IPX+theXpix,IPY+theYpix,X,Y,Z);
    X = X+X0-XT;
    Y = Y+Y0-YT; 
    Z = Z+Z0-ZT;
    SetTarget(X,Y,Z);
    Display(theView,myTypeOfRepresentation);
    (theView->Viewer())->UpdateLights();
    break;

  case V3d_ExtRADIUSLIGHT :
		if (Type() == V3d_DIRECTIONAL)
			break;
//             it is attempted to preserve the target direction position of the  
//             source ==> the point is projected on the target source direction.
    this->Position(Xi,Yi,Zi);
    theView->Project(Xi,Yi,Zi,PXP,PYP);
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
      Display(theView,myTypeOfRepresentation);
      (theView->Viewer())->UpdateLights();
    }
    break;

  case V3d_IntRADIUSLIGHT :
		if (Type() == V3d_DIRECTIONAL)
			break;
//             it is attempted to preserve the target direction position of the  
//             source ==> the point is projected on the target source direction.
    Position(Xi,Yi,Zi);
    theView->Project(Xi,Yi,Zi,PXP,PYP);
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
      Graphic3d_Vector  Dir(Graphic3d_Vertex(xPos,yPos,zPos), myTarget);
      Dir.Normalize();
      Dir.Coord(X,Y,Z);
      X = Xi + Rayon*X;
      Y = Yi + Rayon*Y;
      Z = Zi + Rayon*Z;
//                 the source should remain at a fixed position, 
//                 only the target is modified.
      myTarget.SetCoord(X,Y,Z);
      Display(theView,myTypeOfRepresentation);		 
      (theView->Viewer())->UpdateLights();
    }
		break;

  case V3d_RADIUSTEXTLIGHT :
    break;

  case V3d_NOTHING : 
    break;
  }
}

// =======================================================================
// function : Radius
// purpose  :
// =======================================================================
Standard_Real V3d_PositionLight::Radius() const
{
  Standard_Real  Xp,Yp,Zp, Xc,Yc,Zc;
  
  Position (Xp,Yp,Zp);
  myTarget.Coord(Xc,Yc,Zc);

  return Sqrt (Square(Xc - Xp) + Square(Yc - Yp) + Square(Zc - Zp));
}

// =======================================================================
// function : Radius
// purpose  :
// =======================================================================
void V3d_PositionLight::Erase()
{
  if (!myGraphicStructure.IsNull()) myGraphicStructure->Erase();
  if (!myGraphicStructure1.IsNull()) myGraphicStructure1->Erase();
}

