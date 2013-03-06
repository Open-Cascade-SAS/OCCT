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
        Classe V3d_View_3.cxx :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
     00-09-92 : GG  ; Creation.
     24-12-97 : FMN ; Suppression de GEOMLITE
     13-06-98 : FMN ; PRO14896: Correction sur la gestion de la perspective (cf Programming Guide)
     22-12-98 : FMN ; Rename CSF_WALKTHROW en CSF_WALKTHROUGH
     IMP240100: GG -> Activates WalkThrough model.

************************************************************************/

#define IMP020300       //GG Don't use ZFitAll in during Rotation
//                      for perf improvment

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_View.jxx>
#include <V3d_Static.hxx>

#define Zmargin 1.

/*----------------------------------------------------------------------*/

void V3d_View::Move(const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start) {

  Graphic3d_Vertex Prp ;
  Graphic3d_Vector Vpn ;
  Standard_Real XX,XY,XZ,YX,YY,YZ,ZX,ZY,ZZ ;
  Standard_Real Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xeye,Yeye,Zeye ;
  
  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    if (!ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
		MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
	V3d_BadValue::Raise ("V3d_View::Move, alignment of Eye,At,Up");
  }
  MyXscreenAxis.Coord(XX,XY,XZ) ; 
  MyYscreenAxis.Coord(YX,YY,YZ) ; 
  MyZscreenAxis.Coord(ZX,ZY,ZZ) ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Dx*XX + Dy*YX + Dz*ZX ;
  Yeye = Zrp*Ypn + Dx*XY + Dy*YY + Dz*ZY ;
  Zeye = Zrp*Zpn + Dx*XZ + Dy*YZ + Dz*ZZ ;
  Zrp = sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ;
  V3d_BadValue_Raise_if( Zrp <= 0. ,"V3d_View::Move:: Eye,At are Confused");
#ifdef DEB
  Standard_Real focale = 
#endif
    Focale();
  Prp.SetCoord(Xrp,Yrp,Zrp) ;
  MyViewMapping.SetProjectionReferencePoint(Prp) ;
  Xpn = Xeye / Zrp ; Ypn = Yeye / Zrp ; Zpn = Zeye / Zrp ;
  Vpn.SetCoord(Xpn,Ypn,Zpn) ;   
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  MyView->SetViewOrientation(MyViewOrientation) ; 
  
  // Check ZClipping planes
#ifdef IMP020300
  MyView->SetViewMapping(MyViewMapping) ; 
  SetZSize(0.);
#else
  Standard_Real Zmax,Xat,Yat,Zat ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Xeye += Xat ; Yeye += Yat ; Zeye += Zat ;
  Zmax = sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
	MyProjModel == V3d_TPM_SCREEN ) {
    SetZSize(2.*Zmax+Zmax*Zmargin) ;
  } else {
    if( MyType == V3d_PERSPECTIVE ) {
      SetFocale(focale) ;
    }
    MyView->SetViewMapping(MyViewMapping) ; 
  }
#endif
  ImmediateUpdate();
}

void V3d_View::Move(const Standard_Real Length, const Standard_Boolean Start) {
  Graphic3d_Vertex Prp ;
  Graphic3d_Vector Vpn ;
  Standard_Real Vx,Vy,Vz ;
  Standard_Real Xrp,Yrp,Zrp,Xpn,Ypn,Zpn,Xeye,Yeye,Zeye ;
  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint() ;
  }
  MyDefaultViewAxis.Coord(Vx,Vy,Vz) ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Vx*Length ; 
  Yeye = Zrp*Ypn + Vy*Length ; 
  Zeye = Zrp*Zpn + Vz*Length ;
  Zrp = sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ;
  V3d_BadValue_Raise_if( Zrp <= 0. ,"V3d_View::Move:: Eye,At are Confused");
  
#ifdef DEB
  Standard_Real focale = 
#endif
    Focale();
  Prp.SetCoord(Xrp,Yrp,Zrp) ;
  MyViewMapping.SetProjectionReferencePoint(Prp) ;
  Xpn = Xeye / Zrp ; Ypn = Yeye / Zrp ; Zpn = Zeye / Zrp ;
  Vpn.SetCoord(Xpn,Ypn,Zpn) ;
  MyViewOrientation.SetViewReferencePlane(Vpn) ;
  MyView->SetViewOrientation(MyViewOrientation) ; 

#ifdef IMP020300
  MyView->SetViewMapping(MyViewMapping) ; 
  SetZSize(0.);
#else
  // Check ZClipping planes
  Standard_Real Zmax,Xat,Yat,Zat ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Xeye += Xat ; Yeye += Yat ; Zeye += Zat ;
  Zmax = sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
	MyProjModel == V3d_TPM_SCREEN ) {
    SetZSize(2.*Zmax+Zmax*Zmargin) ;
  } else {
    if( MyType == V3d_PERSPECTIVE ) {
      SetFocale(focale) ;
    }
    MyView->SetViewMapping(MyViewMapping) ; 
  }
#endif
  ImmediateUpdate();
}

void V3d_View::Move(const V3d_TypeOfAxe Axe , const Standard_Real Length, const Standard_Boolean Start) {


  switch (Axe) {
  case V3d_X :
    Move(Length,0.,0.,Start);
    break ;
  case V3d_Y :
    Move(0.,Length,0.,Start);
    break ;
  case V3d_Z :
    Move(0.,0.,Length,Start);
    break ;
  }
}

void V3d_View::Translate(const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start) {

  Graphic3d_Vertex Vrp ;
  Standard_Real XX,XY,XZ,YX,YY,YZ,ZX,ZY,ZZ ;
  Standard_Real Xat,Yat,Zat ;
  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
    MyViewReferencePlane = MyViewOrientation.ViewReferencePlane() ;
    MyViewReferenceUp = MyViewOrientation.ViewReferenceUp() ;
    MyProjReferencePoint = MyViewMapping.ProjectionReferencePoint();
    if (!ScreenAxis(MyViewReferencePlane,MyViewReferenceUp,
		MyXscreenAxis,MyYscreenAxis,MyZscreenAxis))
	V3d_BadValue::Raise ("V3d_View::Translate, alignment of Eye,At,Up");
  }
  MyXscreenAxis.Coord(XX,XY,XZ) ; 
  MyYscreenAxis.Coord(YX,YY,YZ) ; 
  MyZscreenAxis.Coord(ZX,ZY,ZZ) ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Xat -= Dx*XX + Dy*YX + Dz*ZX ;
  Yat -= Dx*XY + Dy*YY + Dz*ZY ;
  Zat -= Dx*XZ + Dy*YZ + Dz*ZZ ;
  Vrp.SetCoord(Xat,Yat,Zat) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  MyView->SetViewOrientation(MyViewOrientation) ; 
#ifdef IMP020300
  SetZSize(0.);
#else
  // Check ZClipping planes
  Standard_Real Xpn,Ypn,Zpn,Xrp,Yrp,Zrp,Xeye,Yeye,Zeye,Zmax ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat ;
  Zmax = Max(sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ,
	     sqrt( Xat*Xat + Yat*Yat + Zat*Zat )) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
	MyProjModel == V3d_TPM_SCREEN ) {
    SetZSize(2.*Zmax+Zmax*Zmargin) ;
  }
#endif
  ImmediateUpdate();
}

void V3d_View::Translate(const V3d_TypeOfAxe Axe, const Standard_Real Length,const Standard_Boolean Start) {

  switch (Axe) {
  case V3d_X :
    Translate(Length,0.,0., Start);
    break ;
  case V3d_Y :
    Translate(0.,Length,0., Start);
    break ;
  case V3d_Z :
    Translate(0.,0.,Length, Start);
    break ;
  }
}
void V3d_View::Place (const Standard_Integer ix, const Standard_Integer iy,
		      const Quantity_Factor aZoomFactor) {
  Standard_Real xpos,ypos;
  Standard_Integer xc,yc;
  Center(xpos,ypos);
  Convert(xpos,ypos,xc,yc);
  Pan(xc-ix,iy-yc,aZoomFactor/Scale());
}
void V3d_View::Translate(const Standard_Real Length,const Standard_Boolean Start) {
  Graphic3d_Vertex Vrp ;
  Standard_Real Xat,Yat,Zat,Vx,Vy,Vz ;
  if( Start ) {
    MyViewReferencePoint = MyViewOrientation.ViewReferencePoint() ;
  }
  MyDefaultViewAxis.Coord(Vx,Vy,Vz) ;
  MyViewReferencePoint.Coord(Xat,Yat,Zat) ;
  Xat -= Vx*Length ; Yat -= Vy*Length ; Zat -= Vz*Length ;
  Vrp.SetCoord(Xat,Yat,Zat) ;
  MyViewOrientation.SetViewReferencePoint(Vrp) ;
  MyView->SetViewOrientation(MyViewOrientation) ; 
  
#ifdef IMP020300
  SetZSize(0.);
#else
  // Check ZClipping planes
  Standard_Real Xpn,Ypn,Zpn,Xrp,Yrp,Zrp,Xeye,Yeye,Zeye,Zmax ;
  MyProjReferencePoint.Coord(Xrp,Yrp,Zrp) ;
  MyViewReferencePlane.Coord(Xpn,Ypn,Zpn) ;
  Xeye = Zrp*Xpn + Xat ; Yeye = Zrp*Ypn + Yat ; Zeye = Zrp*Zpn + Zat ;
  Zmax = Max(sqrt( Xeye*Xeye + Yeye*Yeye + Zeye*Zeye ) ,
	     sqrt( Xat*Xat + Yat*Yat + Zat*Zat )) ;
  if( Zmax > MyViewMapping.FrontPlaneDistance() &&
	MyProjModel == V3d_TPM_SCREEN ) {
    SetZSize(2.*Zmax+Zmax*Zmargin) ;
  }
#endif
  ImmediateUpdate();
}
