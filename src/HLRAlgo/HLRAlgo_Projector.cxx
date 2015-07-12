// Created on: 1992-03-13
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef No_Exception
// #define No_Exception
#endif


#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>

// formula for derivating a perspective, from Mathematica
//        X'[t]      X[t] Z'[t]                                     
// D1 =  -------- + -------------                                   
//           Z[t]          Z[t] 2                                   
//       1 - ----   f (1 - ----)                                    
//            f             f                                       
//=======================================================================
//function : HLRAlgo_Projector
//purpose  : 
//=======================================================================
HLRAlgo_Projector::HLRAlgo_Projector () :
       myPersp(Standard_False),myFocus(0)
{
  Scaled();
} 

//=======================================================================
//function : HLRAlgo_Projector
//purpose  : 
//=======================================================================

HLRAlgo_Projector::HLRAlgo_Projector (const gp_Ax2& CS) :
myPersp(Standard_False), myFocus(0)
{
  myScaledTrsf.SetTransformation(CS);
  Scaled();
  SetDirection();
}

//=======================================================================
//function : HLRAlgo_Projector
//purpose  : 
//=======================================================================

HLRAlgo_Projector::HLRAlgo_Projector (const gp_Ax2& CS, 
				      const Standard_Real Focus) :
				      myPersp(Standard_True), myFocus(Focus)
{
  myScaledTrsf.SetTransformation(CS);
  Scaled();
  SetDirection();
}

//=======================================================================
//function : HLRAlgo_Projector
//purpose  : 
//=======================================================================

HLRAlgo_Projector::HLRAlgo_Projector (const gp_Trsf& T, 
				      const Standard_Boolean Persp,
				      const Standard_Real Focus) :
				      myPersp(Persp),
				      myFocus(Focus),
				      myScaledTrsf(T)
{
  Scaled();
  SetDirection();
}

//=======================================================================
//function : HLRAlgo_Projector
//purpose  : 
//=======================================================================

HLRAlgo_Projector::HLRAlgo_Projector (const gp_Trsf& T, 
				      const Standard_Boolean Persp,
				      const Standard_Real Focus,
				      const gp_Vec2d& v1,
				      const gp_Vec2d& v2,
				      const gp_Vec2d& v3) :
				      myPersp(Persp),
				      myFocus(Focus),
				      myScaledTrsf(T),
				      myD1(v1),
				      myD2(v2),
				      myD3(v3)
{
  Scaled();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRAlgo_Projector::Set (const gp_Trsf& T, 
			     const Standard_Boolean Persp,
			     const Standard_Real Focus) 
{
  myPersp      = Persp;
  myFocus      = Focus;
  myScaledTrsf = T;
  Scaled();
  SetDirection();
}

//=======================================================================
//function : Scaled
//purpose  : 
//=======================================================================

#include <gp_Mat.hxx>

static Standard_Integer TrsfType(const gp_Trsf& Trsf) { 
  const gp_Mat& Mat = Trsf.VectorialPart();
  if(   (Abs(Mat.Value(1,1)-1.0) < 1e-15) 
     && (Abs(Mat.Value(2,2)-1.0) < 1e-15)
     && (Abs(Mat.Value(3,3)-1.0) < 1e-15)) { 
    return(1); //-- top
  }
  else if(   (Abs(Mat.Value(1,1)-0.7071067811865476) < 1e-15)
	  && (Abs(Mat.Value(1,2)+0.5) < 1e-15)
	  && (Abs(Mat.Value(1,3)-0.5) < 1e-15)
	  
	  && (Abs(Mat.Value(2,1)-0.7071067811865476) < 1e-15)
	  && (Abs(Mat.Value(2,2)-0.5) < 1e-15)
	  && (Abs(Mat.Value(2,3)+0.5) < 1e-15)
	  
	  && (Abs(Mat.Value(3,1)) < 1e-15)  
	  && (Abs(Mat.Value(3,2)-0.7071067811865476) < 1e-15)
	  && (Abs(Mat.Value(3,3)-0.7071067811865476) < 1e-15)) { 
    return(0); //-- 
  }
  else if(   (Abs(Mat.Value(1,1)-1.0) < 1e-15) 
	  && (Abs(Mat.Value(2,3)-1.0) < 1e-15)
	  && (Abs(Mat.Value(3,2)+1.0) < 1e-15)) { 
    return(2); //-- front
  }
  else if(   (Abs(Mat.Value(1,1)-0.7071067811865476) < 1e-15)
	  && (Abs(Mat.Value(1,2)-0.7071067811865476) < 1e-15)
	  && (Abs(Mat.Value(1,3)) < 1e-15)
	  
	  && (Abs(Mat.Value(2,1)+0.5) < 1e-15)
	  && (Abs(Mat.Value(2,2)-0.5) < 1e-15)
	  && (Abs(Mat.Value(2,3)-0.7071067811865476) < 1e-15)
	  
	  && (Abs(Mat.Value(3,1)-0.5) < 1e-15)  
	  && (Abs(Mat.Value(3,2)+0.5) < 1e-15)
	  && (Abs(Mat.Value(3,3)-0.7071067811865476) < 1e-15)) { 
    return(3); //-- axo
  }
  return(-1);
}

void HLRAlgo_Projector::Scaled (const Standard_Boolean On)
{ 
  myType=-1;
  myTrsf = myScaledTrsf;
  if (!On) {
    myTrsf.SetScaleFactor(1.);
    if (!myPersp) { 
      myTrsf.SetTranslationPart(gp_Vec(0.,0.,0.));
      myType=TrsfType(myTrsf); 
    }
  }
  myInvTrsf = myTrsf;
  myInvTrsf.Invert();
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void HLRAlgo_Projector::Project (const gp_Pnt& P, gp_Pnt2d& Pout) const
{
  if(myType!=-1) { 
    Standard_Real X,Y;
    switch (myType) { 
    case 0: {  //-- axono standard
      Standard_Real x07 = P.X()*0.7071067811865475;
      Standard_Real y05 = P.Y()*0.5;
      Standard_Real z05 = P.Z()*0.5;
      X=x07-y05+z05;
      Y=x07+y05-z05;
      //-- Z=0.7071067811865475*(P.Y()+P.Z());
      break;
    }
    case 1: { //-- top
      X=P.X(); Y=P.Y(); //-- Z=P.Z();
      Pout.SetCoord(X,Y);
      break;
    }
    case 2: { 
      X=P.X(); Y=P.Z(); //-- Z=-P.Y();
      Pout.SetCoord(X,Y);
      break;
    }
    case 3: { 
      Standard_Real xmy05 = (P.X()-P.Y())*0.5;
      Standard_Real z07 = P.Z()*0.7071067811865476;
      X=0.7071067811865476*(P.X()+P.Y());
      Y=-xmy05+z07;
      Pout.SetCoord(X,Y);
      //-- Z= xmy05+z07;
      break;
    }
    default: { 
      gp_Pnt P2 = P;
      Transform(P2);
      if (myPersp) {
	Standard_Real R = 1.-P2.Z()/myFocus;
	Pout.SetCoord(P2.X()/R,P2.Y()/R);
      }
      else 
	Pout.SetCoord(P2.X(),P2.Y());    
      break;
    }
    }
  }
  else { 
    gp_Pnt P2 = P;
    Transform(P2);
    if (myPersp) {
      Standard_Real R = 1.-P2.Z()/myFocus;
      Pout.SetCoord(P2.X()/R,P2.Y()/R);
    }
    else 
      Pout.SetCoord(P2.X(),P2.Y()); 
  }
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================
/*  ====== TYPE 0  (??)
   (0.7071067811865476, -0.5               ,  0.4999999999999999)
   (0.7071067811865475,  0.5000000000000001, -0.5              )
   (0.0,                 0.7071067811865475,  0.7071067811865476)

  ====== TYPE 1 (top)
(1.0, 0.0, 0.0)
(0.0, 1.0, 0.0)
(0.0, 0.0, 1.0)

 ======= TYPE 2 (front)
(1.0,  0.0                   , 0.0)
(0.0,  1.110223024625157e-16 , 1.0)
(0.0, -1.0                   , 1.110223024625157e-16)

 ======= TYPE 3 
( 0.7071067811865476, 0.7071067811865475, 0.0)
(-0.5               , 0.5000000000000001, 0.7071067811865475)
( 0.4999999999999999, -0.5              , 0.7071067811865476)
*/
void HLRAlgo_Projector::Project (const gp_Pnt& P,
				 Standard_Real& X,
				 Standard_Real& Y,
				 Standard_Real& Z) const
{
  if(myType!=-1) { 
    switch (myType) { 
    case 0: {  //-- axono standard
      Standard_Real x07 = P.X()*0.7071067811865475;
      Standard_Real y05 = P.Y()*0.5;
      Standard_Real z05 = P.Z()*0.5;
      X=x07-y05+z05;
      Y=x07+y05-z05;
      Z=0.7071067811865475*(P.Y()+P.Z());
      break;
    }
    case 1: { //-- top
      X=P.X(); Y=P.Y(); Z=P.Z();
      break;
    }
    case 2: { 
      X=P.X(); Y=P.Z(); Z=-P.Y();
      break;
    }
    case 3: { 
      Standard_Real xmy05 = (P.X()-P.Y())*0.5;
      Standard_Real z07 = P.Z()*0.7071067811865476;
      X=0.7071067811865476*(P.X()+P.Y());
      Y=-xmy05+z07;
      Z= xmy05+z07;
      break;
    }
    default: { 
      gp_Pnt P2 = P;
      Transform(P2);
      P2.Coord(X,Y,Z);
      break;
    }
    }
  }
  else { 
    gp_Pnt P2 = P;
    Transform(P2);
    P2.Coord(X,Y,Z);
    if (myPersp) {
      Standard_Real R = 1 - Z / myFocus;
      X = X / R;
      Y = Y / R;
    }
  }
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void HLRAlgo_Projector::Project (const gp_Pnt& P,
				 const gp_Vec& D1,
				 gp_Pnt2d& Pout,
				 gp_Vec2d& D1out) const
{
  gp_Pnt PP = P;
  PP.Transform(myTrsf);
  gp_Vec DD1 = D1;
  DD1.Transform(myTrsf);
  if (myPersp) {
    Standard_Real R = 1. - PP.Z() / myFocus;
    Pout .SetCoord(PP .X()/R , PP.Y()/R);
    D1out.SetCoord(DD1.X()/R + PP.X()*DD1.Z()/(myFocus * R*R),
		   DD1.Y()/R + PP.Y()*DD1.Z()/(myFocus * R*R));
  }
  else {
    Pout .SetCoord(PP .X(),PP .Y());
    D1out.SetCoord(DD1.X(),DD1.Y());
  }
}

//=======================================================================
//function : Shoot
//purpose  : 
//=======================================================================

gp_Lin HLRAlgo_Projector::Shoot (const Standard_Real X,
				 const Standard_Real Y) const
{
  gp_Lin L;
  if (myPersp) {
    L = gp_Lin(gp_Pnt(0,0, myFocus),
	       gp_Dir(X,Y,-myFocus));
  }
  else {
    L = gp_Lin(gp_Pnt(X,Y,0),
	       gp_Dir(0,0,-1));
  }
  L.Transform(myInvTrsf);
  return L;
}

//=======================================================================
//function : SetDirection
//purpose  : 
//=======================================================================

void HLRAlgo_Projector::SetDirection () 
{
  gp_Vec V1(1,0,0);
  V1.Transform(myTrsf);
  if ((Abs(V1.X()) + Abs(V1.Y())) < Precision::Angular()) V1.SetCoord(1,1,0);
  gp_Vec2d D1(V1.X(),V1.Y());
  myD1.SetCoord(-D1.Y(),D1.X());
  gp_Vec V2(0,1,0);
  V2.Transform(myTrsf);
  if ((Abs(V2.X()) + Abs(V2.Y())) < Precision::Angular()) V2.SetCoord(1,1,0);
  gp_Vec2d D2(V2.X(),V2.Y());
  myD2.SetCoord(-D2.Y(),D2.X());
  gp_Vec V3(0,0,1);
  V3.Transform(myTrsf);
  if ((Abs(V3.X()) + Abs(V3.Y())) < Precision::Angular()) V3.SetCoord(1,1,0);
  gp_Vec2d D3(V3.X(),V3.Y());
  myD3.SetCoord(-D3.Y(),D3.X());
}

//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================

const gp_Trsf & HLRAlgo_Projector::Transformation() const
{ return myTrsf; }
