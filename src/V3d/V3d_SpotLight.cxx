// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Visual3d_ContextPick.hxx>
#include <Visual3d_PickDescriptor.hxx>
#include <Visual3d_HSequenceOfPickPath.hxx>
#include <Visual3d_PickPath.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <TCollection_AsciiString.hxx>

#ifdef WNT
# include <WNT_Window.hxx>
#else
# include <Xw_Window.hxx>
#endif


V3d_SpotLight::V3d_SpotLight(const Handle(V3d_Viewer)& VM, const Standard_Real X, const Standard_Real Y, const Standard_Real Z, const V3d_TypeOfOrientation Direction, const Quantity_NameOfColor Name, const Standard_Real A1, const Standard_Real A2, const Standard_Real CN, const Standard_Real AN):V3d_PositionLight(VM) {

  Viewer_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1  
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

  Viewer_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1  
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

  Viewer_BadValue_Raise_if( A1 < 0 || A1 > 1. || A2 < 0 || A2 > 1 ,
			"V3d_SpotLight::SetAttenuation, bad coefficients");

  MyLight->SetAttenuation1(A1) ;
  MyLight->SetAttenuation2(A2) ;
}

void V3d_SpotLight::SetConcentration(const Standard_Real C) {

  
  Viewer_BadValue_Raise_if( C < 0 || C > 1.,
			"V3d_SpotLight::SetConcentration, bad coefficient");

  MyLight->SetConcentration(C) ;
}

void V3d_SpotLight::SetAngle(const Standard_Real Angle) {

  Viewer_BadValue_Raise_if( Angle <= 0. || Angle >= M_PI,
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

Standard_Real V3d_SpotLight::Concentration()const  {

  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real AN,A1,A2,CN ;
  
  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  return CN ;
}

Standard_Real V3d_SpotLight::Angle()const  {

  Quantity_Color C ;
  Graphic3d_Vector D ;
  Graphic3d_Vertex P ;
  Standard_Real CN,A1,A2,AN ;
  
  MyLight->Values(C,P,D,CN,A1,A2,AN) ;
  return AN ;
}

void V3d_SpotLight::Symbol (const Handle(Graphic3d_Group)& gsymbol,
//                            const Handle(V3d_View)& aView) const {
                            const Handle(V3d_View)& ) const {

  Standard_Real X,Y,Z,Rayon;
  Standard_Real DX,DY,DZ;

  this->Position(X,Y,Z);
  this->Direction(DX,DY,DZ);
  Rayon = this->Radius();
  V3d::ArrowOfRadius(gsymbol,X,Y,Z,-DX,-DY,-DZ,M_PI/8.,Rayon/15.);
}
    
void V3d_SpotLight::Display( const Handle(V3d_View)& aView,
                                    const V3d_TypeOfRepresentation TPres) {

  Graphic3d_Array1OfVertex PRadius(0,1);
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
      PRadius(0).SetCoord(X0,Y0,Z0);
      this->Position(X,Y,Z);
      PRadius(1).SetCoord(X,Y,Z);
      gnopick->Polyline(PRadius);
      V3d::ArrowOfRadius(gExtArrow,X-(X-X0)/10.,
			 Y-(Y-Y0)/10.,
			 Z-(Z-Z0) / 10., X-X0, Y-Y0, Z-Z0, M_PI / 15., Rayon / 20.);
      V3d::ArrowOfRadius(gIntArrow, X0, Y0, Z0, X0-X, Y0-Y, Z0-Z, M_PI / 15., Rayon / 20.);
      TCollection_AsciiString ValOfRadius(Rayon);
      PText.SetCoord( (X0+X)/2., (Y0+Y)/2. , (Z0+Z)/2. );
      gradius->Text(ValOfRadius.ToCString(),PText,0.01);
    }
    
// Display of the meridian

    Quantity_Color Col2(Quantity_NOC_GREEN);
    Handle(Graphic3d_AspectLine3d) Asp2 = new Graphic3d_AspectLine3d
      (Col2,Aspect_TOL_SOLID,1.);
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
