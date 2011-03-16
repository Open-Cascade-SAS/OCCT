// File:	ProjLib_Torus.cxx
// Created:	Tue Aug 24 18:50:08 1993
// Author:	Bruno DUMORTIER
//		<dub@topsn3>


#include <ProjLib_Torus.ixx>

#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>

#include <Precision.hxx>

//=======================================================================
//function : ProjLib_Torus
//purpose  : 
//=======================================================================

ProjLib_Torus::ProjLib_Torus()
{
}


//=======================================================================
//function : ProjLib_Torus
//purpose  : 
//=======================================================================

ProjLib_Torus::ProjLib_Torus(const gp_Torus& To)
{
  Init(To);
}


//=======================================================================
//function : ProjLib_Torus
//purpose  : 
//=======================================================================

ProjLib_Torus::ProjLib_Torus(const gp_Torus& To, const gp_Circ& C)
{
  Init(To);
  Project(C);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  ProjLib_Torus::Init(const gp_Torus& To)
{
  myType  = GeomAbs_OtherCurve;
  myTorus = To;
  myIsPeriodic = Standard_False;
  isDone = Standard_False;
}


//=======================================================================
//function : EvalPnt2d / EvalDir2d
//purpose  : returns the Projected Pnt / Dir in the parametrization range
//           of myPlane.
//           P is a point on a torus with the same Position as To,
//           but with a major an minor radius equal to 1. 
//           ( in order to avoid to divide by Radius)
//                / X = (1+cosV)*cosU        U = Atan(Y/X)
//            P = | Y = (1+cosV)*sinU   ==>
//                \ Z = sinV                 V = ASin( Z)
//=======================================================================

static gp_Pnt2d EvalPnt2d( const gp_Vec& Ve, const gp_Torus& To)
{
  Standard_Real X = Ve.Dot(gp_Vec(To.Position().XDirection()));
  Standard_Real Y = Ve.Dot(gp_Vec(To.Position().YDirection()));
  Standard_Real U,V;

  if ( Abs(X) > Precision::PConfusion() ||
       Abs(Y) > Precision::PConfusion() ) {
    U = ATan2(Y,X);
  }
  else {
    U = 0.;
  }

  V = 0.;

  return gp_Pnt2d( U, V);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void  ProjLib_Torus::Project(const gp_Circ& C)
{
  myType = GeomAbs_Line;

  gp_Vec Xc( C.Position().XDirection());
  gp_Vec Yc( C.Position().YDirection());
  gp_Vec Xt( myTorus.Position().XDirection());
  gp_Vec Yt( myTorus.Position().YDirection());
  gp_Vec Zt( myTorus.Position().Direction());
  gp_Vec OC( myTorus.Location(), C.Location());
  
//  if (OC.Magnitude() < Precision::Confusion()      ||
//      OC.IsParallel(myTorus.Position().Direction(),
//		    Precision::Angular())) {

  if (OC.Magnitude() < Precision::Confusion()      ||
      C.Position().Direction().IsParallel(myTorus.Position().Direction(),
					  Precision::Angular())) {
    // Iso V
    gp_Pnt2d P1 = EvalPnt2d( Xc, myTorus);  // evaluate U1
    gp_Pnt2d P2 = EvalPnt2d( Yc, myTorus);  // evaluate U2
    Standard_Real Z = OC.Dot(myTorus.Position().Direction());
    Z /= myTorus.MinorRadius();
    
    Standard_Real V;

    if ( Z > 1.) {         
      V = PI/2.;          // protection stupide 
    }                     // contre les erreurs de calcul
    else if ( Z < -1.) {  // il arrive que Z soit legerement 
      V = -PI/2;          // superieur a 1.
    }
    else {
      V = ASin(Z);
    }

    if (C.Radius() < myTorus.MajorRadius()) {
      V = PI - V;
    }
    else if ( V < 0.) {
      V += 2*PI;
    }
    P1.SetY(V);
    P2.SetY(V);
    gp_Vec2d V2d ( P1, P2);
    // Normalement Abs( P1.X() - P2.X()) = PI/2
    // Si != PI/2, on a traverse la periode => On reverse la Direction
    if ( Abs( P1.X() - P2.X()) > PI) V2d.Reverse();
    
    gp_Dir2d D2( V2d);
    if ( P1.X() < 0) 
      P1.SetX( 2*PI + P1.X());
    myLin = gp_Lin2d( P1, D2);
  }
  else {
    // Iso U  -> U = angle( Xt, OC)
    Standard_Real U = Xt.AngleWithRef( OC, Xt^Yt);
    if ( U < 0.) 
      U += 2*PI;
    
    // Origine de la droite
    Standard_Real V1 = OC.AngleWithRef(Xc, OC^Zt);
    if ( V1 < 0.)
      V1 += 2*PI;

    gp_Pnt2d P1( U, V1);
    
    // Direction de la droite
    gp_Dir2d D2 = gp::DY2d();
    if ( ((OC^Zt)*(Xc^Yc)) < 0.) {
      D2.Reverse();
    }
    
    myLin = gp_Lin2d( P1, D2);
  }
  isDone = Standard_True;
}

void  ProjLib_Torus::Project(const gp_Lin& L)
{
 ProjLib_Projector::Project(L);
}

void  ProjLib_Torus::Project(const gp_Elips& E)
{
 ProjLib_Projector::Project(E);
}

void  ProjLib_Torus::Project(const gp_Parab& P)
{
 ProjLib_Projector::Project(P);
}

void  ProjLib_Torus::Project(const gp_Hypr& H)
{
 ProjLib_Projector::Project(H);
}

