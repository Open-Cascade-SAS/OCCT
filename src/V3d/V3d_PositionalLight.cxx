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
        Classe V3d_PositionalLight :
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      18-06-96 : FMN ; Ajout MyGraphicStructure1 pour sauvegarder snopick
      24-12-97 : FMN ; Remplacement de math par MathGra
      31-12-97 : CAL ; Suppression de MathGra
      21-01-98 : CAL ; Window de Xw et WNT remplacee par Aspect_Window
      23-02-98 : FMN ; Remplacement PI par Standard_PI
      30-03-98 : ZOV ; PRO6774 (reconstruction of the class hierarchy and suppressing useless methods)
************************************************************************/
/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Aspect_Window.hxx>
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
#include <TColStd_Array2OfReal.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_PositionalLight,V3d_PositionLight)

// =======================================================================
// function : V3d_PositionalLight
// purpose  :
// =======================================================================
V3d_PositionalLight::V3d_PositionalLight (const Handle(V3d_Viewer)& theViewer,
                                          const Standard_Real theX,
                                          const Standard_Real theY,
                                          const Standard_Real theZ,
                                          const Quantity_Color& theColor,
                                          const Standard_Real theConstAttenuation,
                                          const Standard_Real theLinearAttenuation)
: V3d_PositionLight (theViewer)
{
  SetType (V3d_POSITIONAL);
  SetColor (theColor);
  SetTarget (0., 0., 0.);
  SetPosition (theX, theY, theZ);
  SetAttenuation (theConstAttenuation, theLinearAttenuation);
}

// =======================================================================
// function : V3d_PositionalLight
// purpose  :
// =======================================================================
V3d_PositionalLight::V3d_PositionalLight (const Handle(V3d_Viewer)& theViewer,
                                          const Standard_Real theXt,
                                          const Standard_Real theYt,
                                          const Standard_Real theZt,
                                          const Standard_Real theXp,
                                          const Standard_Real theYp,
                                          const Standard_Real theZp,
                                          const Quantity_Color& theColor,
                                          const Standard_Real theConstAttenuation,
                                          const Standard_Real theLinearAttenuation)
: V3d_PositionLight (theViewer)
{
  SetType (V3d_POSITIONAL);
  SetColor (theColor);
  SetTarget (theXt, theYt, theZt);
  SetPosition (theXp, theYp, theZp);
  SetAttenuation (theConstAttenuation, theLinearAttenuation);
}

// =======================================================================
// function : SetSmoothRadius
// purpose  :
// =======================================================================
void V3d_PositionalLight::SetSmoothRadius (const Standard_Real theValue)
{
  V3d_BadValue_Raise_if (theValue < 0.0,
    "V3d_PositionalLight::SetSmoothRadius,"
    "Bad value for smoothing radius");

  myLight.Smoothness = static_cast<Standard_ShortReal> (theValue);
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void V3d_PositionalLight::SetPosition (const Standard_Real theXp,
                                       const Standard_Real theYp,
                                       const Standard_Real theZp)
{
  myLight.Position.x() = theXp;
  myLight.Position.y() = theYp;
  myLight.Position.z() = theZp;
}

// =======================================================================
// function : SetAttenuation
// purpose  :
// =======================================================================
void V3d_PositionalLight::SetAttenuation (const Standard_Real theConstAttenuation,
                                          const Standard_Real theLinearAttenuation)
{
  V3d_BadValue_Raise_if (theConstAttenuation < 0.
                      || theConstAttenuation > 1.
                      || theLinearAttenuation < 0.
                      || theLinearAttenuation > 1.,
    "V3d_PositionalLight::SetAttenuation, bad coefficients");

  myLight.ChangeConstAttenuation()  = static_cast<Standard_ShortReal> (theConstAttenuation);
  myLight.ChangeLinearAttenuation() = static_cast<Standard_ShortReal> (theLinearAttenuation);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void V3d_PositionalLight::Position (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ) const
{
  theX = myLight.Position.x();
  theY = myLight.Position.y();
  theZ = myLight.Position.z();
}

// =======================================================================
// function : Attenuation
// purpose  :
// =======================================================================
void V3d_PositionalLight::Attenuation (Standard_Real& theConstAttenuation,
                                       Standard_Real& theLinearAttenuation) const
{
  theConstAttenuation  = myLight.ConstAttenuation();
  theLinearAttenuation = myLight.LinearAttenuation();
}

// =======================================================================
// function : Symbol
// purpose  :
// =======================================================================
void V3d_PositionalLight::Symbol (const Handle(Graphic3d_Group)& theSymbol, const Handle(V3d_View)& theView) const
{
  Standard_Real Xi,Yi,Zi,Xf,Yf,Zf,Rayon,PXT,PYT,X,Y,Z,XT,YT,ZT;
  Standard_Real A,B,C,Dist,Beta,CosBeta,SinBeta,Coef,X1,Y1,Z1;
  Standard_Real VX,VY,VZ;
  Standard_Integer IXP,IYP,j;
  TColStd_Array2OfReal MatRot(0,2,0,2);

  theView->Proj(VX,VY,VZ);
  this->Position(Xi,Yi,Zi);
  Rayon = this->Radius();
  theView->Project(Xi,Yi,Zi,PXT,PYT); 
  theView->Convert(PXT,PYT,IXP,IYP);
//  3D Coordinate in the plane of projection of the source.
  theView->Convert(IXP,IYP,XT,YT,ZT);
  theView->Convert(PXT,PYT+Rayon,IXP,IYP);
  theView->Convert(IXP,IYP,X,Y,Z);
  X = X+Xi-XT; Y = Y+Yi-YT; Z = Z+Zi-ZT;
  Dist = Sqrt( Square(X-Xi) + Square(Y-Yi) + Square(Z-Zi) );
//  Axis of rotation.
  A = (X-Xi)/Dist;
  B = (Y-Yi)/Dist;
  C = (Z-Zi)/Dist;

//  A sphere is drawn
  V3d::CircleInPlane(theSymbol,Xi,Yi,Zi,VX,VY,VZ,Rayon/40.);
  for( j=1 ; j<=3 ; j++ ) {
    Beta = j * M_PI / 4.;
    CosBeta = Cos(Beta);
    SinBeta = Sin(Beta);
    Coef = 1. - CosBeta;
    MatRot(0,0) =  A * A + (1. - A * A) * CosBeta;
    MatRot(0,1) = -C * SinBeta + Coef * A * B;
    MatRot(0,2) =  B * SinBeta + Coef * A * C;
    MatRot(1,0) =  C * SinBeta + Coef * A * B; 
    MatRot(1,1) =  B * B + (1. - B * B) * CosBeta;
    MatRot(1,2) = -A * SinBeta + Coef * B * C;
    MatRot(2,0) = -B * SinBeta + Coef * A * C;
    MatRot(2,1) =  A * SinBeta + Coef * B * C;
    MatRot(2,2) =  C * C + (1. - C * C) * CosBeta;
    Xf = Xi * MatRot(0,0) + Yi * MatRot(0,1) + Zi * MatRot(0,2);
    Yf = Xi * MatRot(1,0) + Yi * MatRot(1,1) + Zi * MatRot(1,2);
    Zf = Xi * MatRot(2,0) + Yi * MatRot(2,1) + Zi * MatRot(2,2);
//    Rotation of the normal
    X1 = VX * MatRot(0,0) + VY * MatRot(0,1) + VZ * MatRot(0,2);
    Y1 = VX * MatRot(1,0) + VY * MatRot(1,1) + VZ * MatRot(1,2);
    Z1 = VX * MatRot(2,0) + VY * MatRot(2,1) + VZ * MatRot(2,2);
    VX = X1 + Xi - Xf ; VY = Y1 + Yi - Yf ; VZ = Z1 + Zi - Zf;
    V3d::CircleInPlane(theSymbol,Xi,Yi,Zi,VX,VY,VZ,Rayon/40.);
  }
}

// =======================================================================
// function : Display
// purpose  :
// =======================================================================
void V3d_PositionalLight::Display (const Handle(V3d_View)& theView,
                                   const V3d_TypeOfRepresentation theRepresentation)
{
  Graphic3d_Vertex PText ;
  Standard_Real X,Y,Z,Rayon;
  Standard_Real X0,Y0,Z0,VX,VY,VZ;
  Standard_Real X1,Y1,Z1;
  Standard_Real DXRef,DYRef,DZRef,DXini,DYini,DZini;
  V3d_TypeOfRepresentation Pres;

//  Creation of a structure slight of markable elements (position of the
//  light, and the domain of lighting represented by a circle)
//  Creation of a structure snopick of non-markable elements (target, meridian and 
//  parallel).

  Pres = theRepresentation;
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
  if (Pres == V3d_COMPLETE)
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
  
// Display of the position of the light.

  const Quantity_Color Col1 = this->Color();
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,theView);

// Display of the markable sphere (limit at the cercle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
      
    Rayon = this->Radius();
    theView->Proj(VX,VY,VZ);
    V3d::CircleInPlane(gsphere,X0,Y0,Z0,VX,VY,VZ,Rayon);

// Display of the radius of the sphere (line + text)

    if (Pres == V3d_COMPLETE) {
      this->Position(X,Y,Z);
      Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
      aPrims->AddVertex(X0,Y0,Z0);
      aPrims->AddVertex(X,Y,Z);
      gnopick->AddPrimitiveArray(aPrims);
      V3d::ArrowOfRadius(gExtArrow,X-.1*(X-X0),Y-.1*(Y-Y0),Z-.1*(Z-Z0),X-X0,Y-Y0,Z-Z0,M_PI/15.,Rayon/20.);
      V3d::ArrowOfRadius(gIntArrow, X0, Y0, Z0, X0-X, Y0-Y, Z0-Z, M_PI / 15., Rayon / 20.);
      TCollection_AsciiString ValOfRadius(Rayon);
      PText.SetCoord( 0.5*(X0+X), 0.5*(Y0+Y), 0.5*(Z0+Z) );
      gradius->Text(ValOfRadius.ToCString(),PText,0.01);
    }
    
// Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d(Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
//    Definition of the axis of circle
    theView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
      
//  Display of the parallel

//  Definition of the axis of circle
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
