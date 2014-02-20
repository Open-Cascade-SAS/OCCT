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

#include <V3d.hxx>
#include <V3d_PositionalLight.ixx>
#include <Graphic3d_Vector.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Visual3d_ContextPick.hxx>
#include <Visual3d_PickDescriptor.hxx>
#include <Visual3d_HSequenceOfPickPath.hxx>
#include <Visual3d_PickPath.hxx>
#include <V3d_BadValue.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TCollection_AsciiString.hxx>
#include <Aspect_Window.hxx>

//-Declarations

//-Constructors

V3d_PositionalLight::V3d_PositionalLight(const Handle(V3d_Viewer)& VM, const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const Quantity_NameOfColor Name, const Standard_Real A1, const Standard_Real A2):V3d_PositionLight(VM) {

  Quantity_Color C(Name) ;
  Graphic3d_Vertex P(X,Y,Z) ;
  Graphic3d_Vertex T(0.,0.,0.);

  V3d_BadValue_Raise_if ( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1,
			 "V3d_PositionalLight, bad coefficients");
  
  MyType = V3d_POSITIONAL ;
  MyLight = new Visual3d_Light(C,P,A1,A2) ;
  MyTarget = T;

}

V3d_PositionalLight::V3d_PositionalLight(const Handle(V3d_Viewer)& VM, const Standard_Real Xt, const Standard_Real Yt, const Standard_Real Zt, const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp, const Quantity_NameOfColor Name, const Standard_Real A1, const Standard_Real A2):V3d_PositionLight(VM) {

  Quantity_Color C(Name) ;
  Graphic3d_Vertex T(Xt,Yt,Zt) ;
  Graphic3d_Vertex P(Xp,Yp,Zp) ;
  
  V3d_BadValue_Raise_if ( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1,
			 "V3d_PositionalLight, bad coefficients");
  
  MyType = V3d_POSITIONAL ;
  MyLight = new Visual3d_Light(C,P,A1,A2) ;
  MyTarget = T;
  // Graphic structure is initialized during the display.

}

//-Methods, in order

void V3d_PositionalLight::SetPosition(const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp) {
  MyLight->SetPosition (Graphic3d_Vertex (Xp,Yp,Zp));
}

void V3d_PositionalLight::SetAttenuation(const Standard_Real A1, const Standard_Real A2) {


  V3d_BadValue_Raise_if ( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1,
			 "V3d_PositionalLight::SetAttenuation, bad coefficients");

  MyLight->SetAttenuation1(A1) ;
  MyLight->SetAttenuation2(A2) ;
}

void V3d_PositionalLight::Position(Standard_Real& X, Standard_Real& Y, Standard_Real& Z)const  {
  Quantity_Color C ;
  Graphic3d_Vertex P ;
  Standard_Real A1,A2 ;

  MyLight->Values(C,P,A1,A2) ;
  P.Coord(X,Y,Z) ;
}

void V3d_PositionalLight::Attenuation(Standard_Real& A1, Standard_Real& A2)const  {
  Quantity_Color C ;
  Graphic3d_Vertex P ;
  
  MyLight->Values(C,P,A1,A2) ;
}

void V3d_PositionalLight::Symbol (const Handle(Graphic3d_Group)& gsymbol, const Handle(V3d_View)& aView) const {

  Standard_Real Xi,Yi,Zi,Xf,Yf,Zf,Rayon,PXT,PYT,X,Y,Z,XT,YT,ZT;
  Standard_Real A,B,C,Dist,Beta,CosBeta,SinBeta,Coef,X1,Y1,Z1;
  Standard_Real VX,VY,VZ;
  Standard_Integer IXP,IYP,j;
  TColStd_Array2OfReal MatRot(0,2,0,2);

  aView->Proj(VX,VY,VZ);
  this->Position(Xi,Yi,Zi);
  Rayon = this->Radius();
  aView->Project(Xi,Yi,Zi,PXT,PYT); 
  aView->Convert(PXT,PYT,IXP,IYP);
//  3D Coordinate in the plane of projection of the source.
  aView->Convert(IXP,IYP,XT,YT,ZT);
  aView->Convert(PXT,PYT+Rayon,IXP,IYP);
  aView->Convert(IXP,IYP,X,Y,Z);
  X = X+Xi-XT; Y = Y+Yi-YT; Z = Z+Zi-ZT;
  Dist = Sqrt( Square(X-Xi) + Square(Y-Yi) + Square(Z-Zi) );
//  Axis of rotation.
  A = (X-Xi)/Dist;
  B = (Y-Yi)/Dist;
  C = (Z-Zi)/Dist;

//  A sphere is drawn
  V3d::CircleInPlane(gsymbol,Xi,Yi,Zi,VX,VY,VZ,Rayon/40.);
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
    V3d::CircleInPlane(gsymbol,Xi,Yi,Zi,VX,VY,VZ,Rayon/40.);
  }
}


void V3d_PositionalLight::Display( const Handle(V3d_View)& aView,
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

//  Creation of a structure slight of markable elements (position of the
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

  Handle(Graphic3d_Group) gradius;
  Handle(Graphic3d_Group) gExtArrow;
  Handle(Graphic3d_Group) gIntArrow;
  if (Pres == V3d_COMPLETE) {
    gradius = new Graphic3d_Group(MyGraphicStructure);
    gExtArrow = new Graphic3d_Group(MyGraphicStructure);
    gIntArrow = new Graphic3d_Group(MyGraphicStructure);
  }
  Handle(Graphic3d_Group) glight  = new Graphic3d_Group(MyGraphicStructure);
  Handle(Graphic3d_Group) gsphere;
  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) gsphere = new Graphic3d_Group(MyGraphicStructure);
  
  Handle(Graphic3d_Group) gnopick = new Graphic3d_Group(MyGraphicStructure1);
  MyGraphicStructure1->SetPick(Standard_False);
  
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  
// Display of the position of the light.

  this->Color(Quantity_TOC_RGB,R1,G1,B1);
  Quantity_Color Col1(R1,G1,B1,Quantity_TOC_RGB);
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,aView);

// Display of the markable sphere (limit at the cercle).

  if (Pres == V3d_COMPLETE || Pres == V3d_PARTIAL) {
      
    Rayon = this->Radius();
    aView->Proj(VX,VY,VZ);
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
    aView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);
      
//  Display of the parallel

//  Definition of the axis of circle
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
