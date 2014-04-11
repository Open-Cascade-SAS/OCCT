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
      18-06-96 : FMN ; Ajout MyGraphicStructure1 pour sauvegarder snopick
      30-03-98 : ZOV ; PRO6774 (reconstruction of the class hierarchy and suppressing useless methods)
      02.15.100 : JR : Clutter

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d.hxx>
#include <V3d_SpotLight.ixx>
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
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TCollection_AsciiString.hxx>

V3d_SpotLight::V3d_SpotLight(const Handle(V3d_Viewer)& VM, const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const V3d_TypeOfOrientation Direction, const Quantity_NameOfColor Name, const Standard_Real A1, const Standard_Real A2, const Standard_Real CN, const Standard_Real AN):V3d_PositionLight(VM) {

  V3d_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1  
			|| AN < 0. || AN > M_PI, "V3d_SpotLight, bad coefficient or angle");

  Quantity_Color C(Name) ;
  Graphic3d_Vector D = V3d::GetProjAxis(Direction) ;
  Graphic3d_Vertex P(X,Y,Z) ;
  Graphic3d_Vertex T;

  MyType = V3d_SPOT ;
  MyLight = new Visual3d_Light(C,P,D,CN,A1,A2,AN) ;
  // The target is fixed, starting from the light position and the direction.
  T.SetCoord(X + D.X(), Y + D.Y(), Z + D.Z());
  MyTarget = T;
} 

V3d_SpotLight::V3d_SpotLight(const Handle(V3d_Viewer)& VM, const Standard_Real Xt, const Standard_Real Yt, const Standard_Real Zt, const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp, const Quantity_NameOfColor Name, const Standard_Real A1, const Standard_Real A2, const Standard_Real CN, const Standard_Real AN):V3d_PositionLight(VM) {

  V3d_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1  
			|| AN < 0. || AN > M_PI, "V3d_SpotLight, bad coefficient or angle");

  Quantity_Color C(Name) ;
  Graphic3d_Vertex T(Xt,Yt,Zt) ;
  Graphic3d_Vertex P(Xp,Yp,Zp) ;
  Graphic3d_Vector D(P,T);

  MyType = V3d_SPOT ;
  D.Normalize();
  MyLight = new Visual3d_Light(C,P,D,CN,A1,A2,AN) ;
  MyTarget = T;
  // La Structure graphique sera initialisee lors de l'affichage.
}


//-Methods, in order

void V3d_SpotLight::SetPosition(const Standard_Real Xp, const Standard_Real Yp, const Standard_Real Zp) {
  MyLight->SetPosition (Graphic3d_Vertex (Xp,Yp,Zp));
}

void V3d_SpotLight::SetDirection(const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz) {

  Graphic3d_Vector D ;
  D.SetCoord(Vx,Vy,Vz) ; D.Normalize() ;
  MyLight->SetDirection(D) ;
}

void V3d_SpotLight::SetDirection(const V3d_TypeOfOrientation Direction) {

  Graphic3d_Vector D = V3d::GetProjAxis(Direction) ;
  MyLight->SetDirection(D) ;
}

void V3d_SpotLight::SetAttenuation(const Standard_Real A1, const Standard_Real A2) {

  V3d_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1 ,
			"V3d_SpotLight::SetAttenuation, bad coefficients");

  MyLight->SetAttenuation1(A1) ;
  MyLight->SetAttenuation2(A2) ;
}

void V3d_SpotLight::SetConcentration(const Standard_Real C) {

  
  V3d_BadValue_Raise_if( C < 0 || C > 1.,
			"V3d_SpotLight::SetConcentration, bad coefficient");

  MyLight->SetConcentration(C) ;
}

void V3d_SpotLight::SetAngle(const Standard_Real Angle) {

  V3d_BadValue_Raise_if( Angle <= 0. || Angle >= M_PI,
			"V3d_SpotLight::SetAngle, bad angle");
  MyLight->SetAngle(Angle) ;

}

void V3d_SpotLight::Direction(Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz)const  {

  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real CN,A1,A2,AN ;

  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  D.Coord(Vx,Vy,Vz) ;
}

void V3d_SpotLight::Position(Standard_Real& Xp, Standard_Real& Yp, Standard_Real& Zp)const  {
  
  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real CN,A1,A2,AN ;

  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  P.Coord(Xp,Yp,Zp) ;
}

void V3d_SpotLight::Attenuation(Standard_Real& A1, Standard_Real& A2) const  {
    Quantity_Color C ;
    Graphic3d_Vector D ;
    Graphic3d_Vertex P ;
    Standard_Real CN,AN ;
    
    MyLight->Values(C,P,D,CN,A1,A2,AN) ;
}

Standard_Real V3d_SpotLight::Concentration()const
{
  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real AN,A1,A2,CN ;
  
  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  return CN ;
}

Standard_Real V3d_SpotLight::Angle()const
{
  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real CN,A1,A2,AN ;
  
  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  return AN ;
}

void V3d_SpotLight::Symbol (const Handle(Graphic3d_Group)& gsymbol,
                            const Handle(V3d_View)& ) const
{
  Standard_Real X,Y,Z;
  Standard_Real DX,DY,DZ;
  this->Position(X,Y,Z);
  this->Direction(DX,DY,DZ);

  V3d::ArrowOfRadius(gsymbol,X,Y,Z,-DX,-DY,-DZ,M_PI/8.,this->Radius()/15.);
}
    
void V3d_SpotLight::Display( const Handle(V3d_View)& aView,
                             const V3d_TypeOfRepresentation TPres)
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
//  parallel).// 

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
  if (Pres == V3d_COMPLETE)
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
  MyGraphicStructure1->SetPick(Standard_False);
  
  X0 = MyTarget.X();
  Y0 = MyTarget.Y();
  Z0 = MyTarget.Z();
  
//Display of the position of the light.

  this->Color(Quantity_TOC_RGB,R1,G1,B1);
  Quantity_Color Col1(R1,G1,B1,Quantity_TOC_RGB);
  Handle(Graphic3d_AspectLine3d) Asp1 = new Graphic3d_AspectLine3d();
  Asp1->SetColor(Col1);
  glight->SetPrimitivesAspect(Asp1);
  this->Symbol(glight,aView);
  
// Display of the reference sphere (limited by circle).

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
      V3d::ArrowOfRadius(gIntArrow,X0,Y0,Z0,X0-X,Y0-Y,Z0-Z,M_PI/15.,Rayon/20.);
      TCollection_AsciiString ValOfRadius(Rayon);
      PText.SetCoord( .5*(X0+X), .5*(Y0+Y), .5*(Z0+Z) );
      gradius->Text(ValOfRadius.ToCString(),PText,0.01);
    }
    
// Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d(Col2,Aspect_TOL_SOLID,1.);
    gnopick->SetPrimitivesAspect(Asp2);
    
//    Definition of the axis of the circle
    aView->Up(DXRef,DYRef,DZRef);
    this->Position(X,Y,Z);
    DXini = X-X0; DYini = Y-Y0; DZini = Z-Z0;
    VX = DYRef*DZini - DZRef*DYini;
    VY = DZRef*DXini - DXRef*DZini;
    VZ = DXRef*DYini - DYRef*DXini;
    
    V3d::CircleInPlane(gnopick,X0,Y0,Z0,VX,VY,VZ,Rayon);

//    Display of the parallel

//    Definition of the axis of the circle
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
