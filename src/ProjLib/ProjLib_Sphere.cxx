// Created on: 1993-08-24
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

//  Modified by skv - Tue Aug  1 16:29:59 2006 OCC13116

#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <ProjLib_Sphere.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : ProjLib_Sphere
//purpose  : 
//=======================================================================
ProjLib_Sphere::ProjLib_Sphere()
{
}


//=======================================================================
//function : ProjLib_Sphere
//purpose  : 
//=======================================================================

ProjLib_Sphere::ProjLib_Sphere(const gp_Sphere& Sp)
{
  Init(Sp);
}


//=======================================================================
//function : ProjLib_Sphere
//purpose  : 
//=======================================================================

ProjLib_Sphere::ProjLib_Sphere(const gp_Sphere& Sp, const gp_Circ& C)
{
  Init(Sp);
  Project(C);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  ProjLib_Sphere::Init(const gp_Sphere& Sp)
{
  myType = GeomAbs_OtherCurve;
  mySphere = Sp;
  myIsPeriodic = Standard_False;
  isDone = Standard_False;
}


//=======================================================================
//function : EvalPnt2d / EvalDir2d
//purpose  : returns the Projected Pnt / Dir in the parametrization range
//           of mySphere.
//           P is a point on a sphere with the same Position as Sp,
//           but with a radius equal to 1. ( in order to avoid to divide
//           by Radius)
//                / X = cosV cosU        U = Atan(Y/X)
//            P = | Y = cosV sinU   ==>
//                \ Z = sinV             V = ASin( Z)
//=======================================================================

static gp_Pnt2d EvalPnt2d( const gp_Vec P, const gp_Sphere& Sp)
{
  Standard_Real X = P.Dot(gp_Vec(Sp.Position().XDirection()));
  Standard_Real Y = P.Dot(gp_Vec(Sp.Position().YDirection()));
  Standard_Real Z = P.Dot(gp_Vec(Sp.Position().Direction()));
  Standard_Real U,V;

  if ( Abs(X) > Precision::PConfusion() ||
       Abs(Y) > Precision::PConfusion() ) {
    Standard_Real UU = ATan2(Y,X);
    U = ElCLib::InPeriod(UU, 0., 2*M_PI);
  }
  else {
    U = 0.;
  }

  if ( Z > 1.) 
    Z = 1.;
  else if ( Z < -1.) 
    Z = -1.;
  V = ASin ( Z);

  return gp_Pnt2d( U, V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Sphere::Project(const gp_Circ& C)
{
  gp_Pnt O;           // O Location of Sp;
  gp_Dir Xc, Yc, Zc;  // X Y Z Direction of C; 
  gp_Dir Xs, Ys, Zs;  // X Y Z Direction of Sp;
  
  //Check the validity :
  //                     Xc & Yc must be perpendicular to Zs ->IsoV;
  //                     O,Zs is in the Plane O,Xc,Yc;       ->IsoU;
  
  O  = mySphere.Location();
  Xc = C.Position().XDirection();
  Yc = C.Position().YDirection();
  Zc = Xc ^ Yc;
  Xs = mySphere.Position().XDirection();
  Ys = mySphere.Position().YDirection();
  Zs = mySphere.Position().Direction();
  
  Standard_Boolean isIsoU, isIsoV;
  Standard_Real Tol = Precision::Confusion();

  isIsoU = Zc.IsNormal(Zs,Tol) && O.IsEqual(C.Location(),Tol);
  isIsoV = Xc.IsNormal(Zs,Tol) && Yc.IsNormal(Zs,Tol); 
  
  gp_Pnt2d P2d1, P2d2;
  gp_Dir2d D2d;

  if ( isIsoU) {  
    myType = GeomAbs_Line;
    
    P2d1 = EvalPnt2d(gp_Vec(Xc),mySphere);
    P2d2 = EvalPnt2d(gp_Vec(Yc),mySphere);
    
    if (isIsoU && (Abs(P2d1.Y()-M_PI/2.) < Precision::PConfusion() ||
		   Abs(P2d1.Y()+M_PI/2.) < Precision::PConfusion()   )) {
      // then P1 is on the apex of the sphere and U is undefined
      // The value of U is given by P2d2.Y() .
      P2d1.SetX(P2d2.X());
    }
    else if ( Abs( Abs(P2d1.X()-P2d2.X()) - M_PI) < Precision::PConfusion()) {
      // then we have U2 = U1 + PI; V2;
      // we have to assume that U1 = U2 
      //   so V2 = PI - V2;
      P2d2.SetX( P2d1.X());
      if (P2d2.Y() < 0.) 
	P2d2.SetY( -M_PI - P2d2.Y());
      else
	P2d2.SetY(  M_PI - P2d2.Y());
    }
    else {
      P2d2.SetX( P2d1.X());
    }
    
    D2d = gp_Dir2d(gp_Vec2d(P2d1,P2d2));
    isDone = Standard_True;
  }
  else if ( isIsoV) {
    myType = GeomAbs_Line;
    
    //P2d(U,V) :first point of the PCurve.
    Standard_Real U = Xs.AngleWithRef(Xc, Xs^Ys);
    if (U<0) 
      U += 2*M_PI;
    Standard_Real Z = gp_Vec(O,C.Location()).Dot(Zs);
    Standard_Real V = ASin(Z/mySphere.Radius());
    P2d1 = gp_Pnt2d(U,V);
    D2d = gp_Dir2d((Xc^Yc).Dot(Xs^Ys) ,0.);
    isDone = Standard_True;
  }
  myLin = gp_Lin2d(P2d1, D2d);
}


void  ProjLib_Sphere::Project(const gp_Lin& L)
{
 ProjLib_Projector::Project(L);
}

void  ProjLib_Sphere::Project(const gp_Elips& E)
{
 ProjLib_Projector::Project(E);
}

void  ProjLib_Sphere::Project(const gp_Parab& P)
{
 ProjLib_Projector::Project(P);
}

void  ProjLib_Sphere::Project(const gp_Hypr& H)
{
 ProjLib_Projector::Project(H);
}


//=======================================================================
//function : SetInBounds
//purpose  : 
//=======================================================================

void ProjLib_Sphere::SetInBounds(const Standard_Real U) 
{
  StdFail_NotDone_Raise_if( !isDone, "ProjLib_Sphere:SetInBounds");
  
  // first set the y of the first point in -pi/2 pi/2
  Standard_Real newY, Y = ElCLib::Value(U,myLin).Y();
  newY = ElCLib::InPeriod( Y, -M_PI, M_PI);
  
  myLin.Translate(gp_Vec2d(0.,newY-Y));

  gp_Pnt2d P = ElCLib::Value(U,myLin);
  gp_Trsf2d Trsf;
  gp_Ax2d   Axis;
  Standard_Real Tol = 1.e-7;
  gp_Dir2d D2d = myLin.Direction();
//  Modified by skv - Tue Aug  1 16:29:59 2006 OCC13116 Begin
//   if ((P.Y() > M_PI/2) || 
  if ((P.Y() - M_PI/2 > Tol) || 
//  Modified by skv - Tue Aug  1 16:29:59 2006 OCC13116 End
      (Abs(P.Y()-M_PI/2)<Tol && D2d.IsEqual(gp::DY2d(),Tol))) {
    Axis = gp_Ax2d( gp_Pnt2d( 0., M_PI/2.), gp::DX2d());
  }
//  Modified by skv - Tue Aug  1 16:29:59 2006 OCC13116 Begin
//   else if ((P.Y() < -M_PI/2) || 
  else if ((P.Y() + M_PI/2 < -Tol) || 
//  Modified by skv - Tue Aug  1 16:29:59 2006 OCC13116 End
	   (Abs(P.Y()+M_PI/2)<Tol && D2d.IsOpposite(gp::DY2d(),Tol))) {
    Axis = gp_Ax2d( gp_Pnt2d( 0., -M_PI/2.), gp::DX2d());
  }
  else 
    return;

  Trsf.SetMirror(Axis);
  myLin.Transform(Trsf);

  myLin.Translate(gp_Vec2d(M_PI,0.));

  // il faut maintenant recadrer en U
  Standard_Real newX, X = ElCLib::Value(U,myLin).X();
  newX = ElCLib::InPeriod( X, 0., 2.*M_PI);
  myLin.Translate(gp_Vec2d(newX-X,0.));
}
