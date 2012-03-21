// Created on: 1992-03-13
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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


#define IMP240100	//GG
//			Change RefToPix()/Convert() to Project() method.

#include <Select3D_Projector.ixx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

// formula for derivating a perspective, from Mathematica

//        X'[t]      X[t] Z'[t]
// D1 =  -------- + -------------
//           Z[t]          Z[t] 2
//       1 - ----   f (1 - ----)
//            f             f

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector(const Handle(V3d_View)& aViou)
: myPersp(aViou->Type()==V3d_PERSPECTIVE),
  myFocus(aViou->Focale()),
  myView(aViou),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  Standard_Real Xat,Yat,Zat,XUp,YUp,ZUp,DX,DY,DZ;
  //Standard_Boolean Pers=Standard_False;

  aViou->At(Xat,Yat,Zat);
  aViou->Up(XUp,YUp,ZUp);
  aViou->Proj(DX,DY,DZ);
  gp_Pnt At (Xat,Yat,Zat);
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  myScaledTrsf.SetTransformation(Axe);
  myGTrsf.SetTrsf(myScaledTrsf);
  Scaled();

}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector()
: myPersp(Standard_False),
  myFocus(0),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector (const gp_Ax2& CS)
: myPersp(Standard_False),
  myFocus(0),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  myScaledTrsf.SetTransformation(CS);
  myGTrsf.SetTrsf(myScaledTrsf);
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector (const gp_Ax2& CS,
                                        const Standard_Real Focus)
: myPersp(Standard_True),
  myFocus(Focus),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  myScaledTrsf.SetTransformation(CS);
  myGTrsf.SetTrsf(myScaledTrsf);
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector (const gp_Trsf& T,
                                        const Standard_Boolean Persp,
                                        const Standard_Real Focus)
: myPersp(Persp),
  myFocus(Focus),
  myScaledTrsf(T),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  myGTrsf.SetTrsf(myScaledTrsf);
  Scaled();
}

//=======================================================================
//function : Select3D_Projector
//purpose  :
//=======================================================================

Select3D_Projector::Select3D_Projector (const gp_GTrsf& GT,
                                        const Standard_Boolean Persp,
                                        const Standard_Real Focus)
: myPersp(Persp),
  myFocus(Focus),
  myGTrsf(GT),
  myDepthMin(-Precision::Infinite()),
  myDepthMax( Precision::Infinite())
{
  Scaled();
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================

void Select3D_Projector::Set
  (const gp_Trsf& T,
   const Standard_Boolean Persp,
   const Standard_Real Focus)
{
  myPersp      = Persp;
  myFocus      = Focus;
  myScaledTrsf = T;
  Scaled();
}

//=======================================================================
//function : Scaled
//purpose  :
//=======================================================================

#include <gp_Mat.hxx>

static Standard_Integer TrsfType(const gp_GTrsf& Trsf) {
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

void Select3D_Projector::Scaled (const Standard_Boolean On)
{
  myType=-1;
  if (!On) {
    if (!myPersp) {
      //myGTrsf.SetTranslationPart(gp_XYZ(0.,0.,0.));
      myType=TrsfType(myGTrsf);
    }
  }
  myInvTrsf = myGTrsf;
  myInvTrsf.Invert();
}

//=======================================================================
//function : Project
//purpose  :
//=======================================================================

void Select3D_Projector::Project (const gp_Pnt& P, gp_Pnt2d& Pout) const
{

  if(!myView.IsNull()){
    Standard_Real Xout,Yout;
//    V3d_View
#ifdef IMP240100
    myView->Project(P.X(),P.Y(),P.Z(),Xout,Yout);
#else
    Standard_Integer Xp,Yp;
    myView->RefToPix(P.X(),P.Y(),P.Z(),Xp,Yp);
    myView->Convert(Xp,Yp,Xout,Yout);
#endif
    Pout.SetCoord(Xout,Yout);
  }
  else{
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
void Select3D_Projector::Project (const gp_Pnt& P,
				 Standard_Real& X,
				 Standard_Real& Y,
				 Standard_Real& Z) const
{
  if(!myView.IsNull()){
//    Standard_Real Xout,Yout;
//    V3d_View
#ifdef IMP240100
    myView->Project(P.X(),P.Y(),P.Z(),X,Y);
#else
    Standard_Integer Xp,Yp;
    myView->RefToPix(P.X(),P.Y(),P.Z(),Xp,Yp);
    myView->Convert(Xp,Yp,X,Y);
#endif
  }
  else{
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
}
//=======================================================================
//function : Project
//purpose  :
//=======================================================================

void Select3D_Projector::Project (const gp_Pnt& P,
				 const gp_Vec& D1,
				 gp_Pnt2d& Pout,
				 gp_Vec2d& D1out) const
{
  gp_Pnt PP = P;
  Transform(PP);
  gp_Vec DD1 = D1;
  Transform(DD1);
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

gp_Lin Select3D_Projector::Shoot
  (const Standard_Real X,
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
  Transform(L, myInvTrsf);
  return L;
}


void Select3D_Projector::SetView(const Handle(V3d_View)& aViou)
{
  myView = aViou;
  myPersp = aViou->Type()==V3d_PERSPECTIVE;
  myFocus= aViou->Focale();
  Standard_Real Xat,Yat,Zat,XUp,YUp,ZUp,DX,DY,DZ;
  //Standard_Boolean Pers=Standard_False;

  aViou->At(Xat,Yat,Zat);
  aViou->Up(XUp,YUp,ZUp);
  aViou->Proj(DX,DY,DZ);
  gp_Pnt At (Xat,Yat,Zat);
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  myScaledTrsf.SetTransformation(Axe);
  Scaled();

}

void Select3D_Projector::DepthMinMax (const Standard_Real theDepthMin,
                                      const Standard_Real theDepthMax)
{
  myDepthMin = theDepthMin;
  myDepthMax = theDepthMax;
}
