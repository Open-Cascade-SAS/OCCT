// Copyright (c) 1995-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtElC2d.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>

#include <stdio.h>
static
  void RefineDir(gp_Dir& aDir);

//=======================================================================
//class    : ExtremaExtElC_TrigonometricRoots
//purpose  : 
//==  Classe Interne (Donne des racines classees d un polynome trigo)
//==  Code duplique avec IntAna_IntQuadQuad.cxx (lbr le 26 mars 98)
//==  Solution fiable aux problemes de coefficients proches de 0 
//==  avec essai de rattrapage si coeff<1.e-10 (jct le 27 avril 98) 
//=======================================================================
class ExtremaExtElC_TrigonometricRoots {
 private:
  Standard_Real Roots[4];
  Standard_Boolean done;
  Standard_Integer NbRoots;
  Standard_Boolean infinite_roots;
 public:
  ExtremaExtElC_TrigonometricRoots(const Standard_Real CC,
				   const Standard_Real SC,
				   const Standard_Real C,
				   const Standard_Real S,
				   const Standard_Real Cte,
				   const Standard_Real Binf,
				   const Standard_Real Bsup);
  //
  Standard_Boolean IsDone() {
    return done; 
  }
  //
  Standard_Boolean IsARoot(Standard_Real u) {
    Standard_Real PIpPI, aEps;
    //
    aEps=RealEpsilon();
    PIpPI = M_PI + M_PI;
    for(Standard_Integer i=0 ; i<NbRoots; i++) {
      if(Abs(u - Roots[i])<=aEps) {
	return Standard_True ;
      }
      if(Abs(u - Roots[i]-PIpPI)<=aEps) {
	return Standard_True;
      }
    }
    return Standard_False;
  }
  //
  Standard_Integer NbSolutions() { 
    if(!done) {
      throw StdFail_NotDone();
    }
    return NbRoots; 
  }
  //
  Standard_Boolean InfiniteRoots() { 
    if(!done) {
      throw StdFail_NotDone();
    }
    return infinite_roots; 
  }
  //
  Standard_Real Value(const Standard_Integer& n) {
    if((!done)||(n>NbRoots)) {
      throw StdFail_NotDone();
    }
    return Roots[n-1];
  }
}; 
//=======================================================================
//function : ExtremaExtElC_TrigonometricRoots
//purpose  : 
//=======================================================================
ExtremaExtElC_TrigonometricRoots::
  ExtremaExtElC_TrigonometricRoots(const Standard_Real CC,
				   const Standard_Real SC,
				   const Standard_Real C,
				   const Standard_Real S,
				   const Standard_Real Cte,
				   const Standard_Real Binf,
				   const Standard_Real Bsup) 
{
  Standard_Integer i, nbessai;
  Standard_Real cc ,sc, c, s, cte;
  //
  nbessai = 1;
  cc = CC;
  sc = SC;
  c = C;
  s = S;
  cte = Cte;
  done=Standard_False;
  while (nbessai<=2 && !done) {
    //-- F= AA*CN*CN+2*BB*CN*SN+CC*CN+DD*SN+EE;
    math_TrigonometricFunctionRoots MTFR(cc,sc,c,s,cte,Binf,Bsup); 
    //
    if(MTFR.IsDone()) {
      done=Standard_True;
      if(MTFR.InfiniteRoots()) {
	infinite_roots=Standard_True;
      }
      else { //else #1
	Standard_Boolean Triee;
	Standard_Integer j, SvNbRoots;
	Standard_Real aTwoPI, aMaxCoef, aPrecision;
	//
	aTwoPI=M_PI+M_PI;
	NbRoots=MTFR.NbSolutions();
	for(i=0;i<NbRoots;++i) {
	  Roots[i]=MTFR.Value(i+1);
	  if(Roots[i]<0.) {
	    Roots[i]=Roots[i]+aTwoPI;
	  }
	  if(Roots[i]>aTwoPI) {
	    Roots[i]=Roots[i]-aTwoPI;
	  }
	}
	//
	//-- La recherche directe donne n importe quoi. 
	aMaxCoef = Max(CC,SC);
	aMaxCoef = Max(aMaxCoef,C);
	aMaxCoef = Max(aMaxCoef,S);
	aMaxCoef = Max(aMaxCoef,Cte);
	aPrecision = Max(1.e-8, 1.e-12*aMaxCoef);
	
	SvNbRoots=NbRoots;
	for(i=0; i<SvNbRoots; ++i) {
	  Standard_Real y;
	  Standard_Real co=cos(Roots[i]);
	  Standard_Real si=sin(Roots[i]);
	  y=co*(CC*co + (SC+SC)*si + C) + S*si + Cte;
	  // modified by OCC  Tue Oct  3 18:43:00 2006
	  if(Abs(y)>aPrecision) {
	    NbRoots--;
	    Roots[i]=1000.0;
	  }
	}
	//
	do {
	  Standard_Real t;
	  //
	  Triee=Standard_True;
	  for(i=1, j=0; i<SvNbRoots; ++i, ++j) {
	    if(Roots[i]<Roots[j]) {
	      Triee=Standard_False;
	      t=Roots[i]; 
	      Roots[i]=Roots[j]; 
	      Roots[j]=t;
	    }
	  }
	}
	while(!Triee);
	//
	infinite_roots=Standard_False;
	if(NbRoots==0) { //--!!!!! Detect case Pol = Cte ( 1e-50 ) !!!!
	  if((Abs(CC) + Abs(SC) + Abs(C) + Abs(S)) < 1e-10) {
	    if(Abs(Cte) < 1e-10)  {
	      infinite_roots=Standard_True;
	    }
	  }
	}
      } // else #1
    } // if(MTFR.IsDone()) {
    else {
	// try to set very small coefficients to ZERO
      if (Abs(CC)<1e-10) {
	cc = 0.0;
      }
      if (Abs(SC)<1e-10) {
	sc = 0.0;
      }
      if (Abs(C)<1e-10) {
	c = 0.0;
      }
      if (Abs(S)<1e-10){
	s = 0.0;
      }
      if (Abs(Cte)<1e-10){
	cte = 0.0;
      }
      nbessai++;
    }
  } // while (nbessai<=2 && !done) {
}

//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC () 
{
  myDone = Standard_False; 
}
//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Lin& theC1, 
				const gp_Lin& theC2,
				const Standard_Real)
// Function:
//   Find min distance between 2 straight lines.

// Method:
//   Let D1 and D2, be 2 directions of straight lines C1 and C2.
//   2 cases are considered:
//   1- if Angle(D1,D2) < AngTol, straight lines are parallel.
//      The distance is the distance between a point of C1 and the straight line C2.
//   2- if Angle(D1,D2) > AngTol:
//      Let P1=C1(u1) and P2=C2(u2).
//      Then we must find u1 and u2 such as the distance P1P2 is minimal.
//      Target function is:
//        F(u1, u2) = ((L1x+D1x*u1)-(L2x+D2x*u2))^2 + 
//                    ((L1y+D1y*u1)-(L2y+D2y*u2))^2 +
//                    ((L1z+D1z*u1)-(L2z+D2z*u2))^2 --> min,
//      where L1 and L2 are lines locations, D1 and D2 are lines directions. 
//    Let simplify the function F

//      F(u1, u2) = (D1x*u1-D2x*u2-Lx)^2 + (D1y*u1-D2y*u2-Ly)^2 + (D1z*u1-D2z*u2-Lz)^2,
//    where L is a vector L1L2.

//    In point of minimum, the condition
//      {dF/du1 = 0
//      {dF/du2 = 0

//    must be satisfied.

//      dF/du1 = 2*D1x*(D1x*u1-D2x*u2-Lx) +
//               2*D1y*(D1y*u1-D2y*u2-Ly) +
//               2*D1z*(D1z*u1-D2z*u2-Lz) =
//             = 2*((D1^2)*u1-(D1.D2)*u2-L.D1) =
//             = 2*(u1-(D1.D2)*u2-L.D1)
//      dF/du2 = -2*D2x*(D1x*u1-D2x*u2-Lx) - 
//                2*D2y*(D1y*u1-D2y*u2-Ly) - 
//                2*D2z*(D1z*u1-D2z*u2-Lz)=
//             = -2*((D2.D1)*u1-(D2^2)*u2-(D2.L)) = 
//             = -2*((D2.D1)*u1-u2-(D2.L))

//   Consequently, we have two-equation system with two variables:

//     {u1 - (D1.D2)*u2 = L.D1
//     {(D1.D2)*u1 - u2 = L.D2

//   This system has one solution if (D1.D2)^2 != 1
//   (if straight lines are not parallel).
{
  myDone = Standard_False;
  myNbExt = 0;
  myIsPar = Standard_False;

  const gp_Dir &aD1 = theC1.Position().Direction(),
               &aD2 = theC2.Position().Direction();
  const Standard_Real aCosA = aD1.Dot(aD2);
  const Standard_Real aSqSinA = 1.0 - aCosA * aCosA;
  Standard_Real aU1 = 0.0, aU2 = 0.0;
  if (aSqSinA < gp::Resolution() || aD1.IsParallel(aD2, Precision::Angular()))
  {
    myIsPar = Standard_True;
  }
  else
  {
    const gp_XYZ aL1L2 = theC2.Location().XYZ() - theC1.Location().XYZ();
    const Standard_Real aD1L = aD1.XYZ().Dot(aL1L2),
                        aD2L = aD2.XYZ().Dot(aL1L2);
    aU1 = (aD1L - aCosA * aD2L) / aSqSinA;
    aU2 = (aCosA * aD1L - aD2L) / aSqSinA;

    myIsPar = Precision::IsInfinite(aU1) || Precision::IsInfinite(aU2);
  }

  if (myIsPar)
  {
    mySqDist[0] = mySqDist[1] = theC2.SquareDistance(theC1.Location());
    myDone = Standard_True;
    return;
  }

  // Here myIsPar == Standard_False;
  
  const gp_Pnt aP1(ElCLib::Value(aU1, theC1)),
               aP2(ElCLib::Value(aU2, theC2));
  mySqDist[myNbExt] = aP1.SquareDistance(aP2);
  myPoint[myNbExt][0] = Extrema_POnCurv(aU1, aP1);
  myPoint[myNbExt][1] = Extrema_POnCurv(aU2, aP2);
  myNbExt = 1;
  myDone = Standard_True;
}

//=======================================================================
//function : PlanarLineCircleExtrema
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtElC::PlanarLineCircleExtrema(const gp_Lin& theLin,
                                                         const gp_Circ& theCirc)
{
  const gp_Dir &aDirC = theCirc.Axis().Direction(),
               &aDirL = theLin.Direction();

  if (Abs(aDirC.Dot(aDirL)) > Precision::Angular())
    return Standard_False;
  
  //The line is in the circle-plane completely
  //(or parallel to the circle-plane).
  //Therefore, we are looking for extremas and
  //intersections in 2D-space.

  const gp_XYZ &aCLoc = theCirc.Location().XYZ();
  const gp_XYZ &aDCx = theCirc.Position().XDirection().XYZ(),
               &aDCy = theCirc.Position().YDirection().XYZ();

  const gp_XYZ &aLLoc = theLin.Location().XYZ();
  const gp_XYZ &aLDir = theLin.Direction().XYZ();

  const gp_XYZ aVecCL(aLLoc - aCLoc);

  //Center of 2D-circle
  const gp_Pnt2d aPC(0.0, 0.0);

  gp_Ax22d aCircAxis(aPC, gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0));
  gp_Circ2d aCirc2d(aCircAxis, theCirc.Radius());

  gp_Pnt2d aPL(aVecCL.Dot(aDCx), aVecCL.Dot(aDCy));
  gp_Dir2d aDL(aLDir.Dot(aDCx), aLDir.Dot(aDCy));
  gp_Lin2d aLin2d(aPL, aDL);

  // Extremas
  Extrema_ExtElC2d anExt2d(aLin2d, aCirc2d, Precision::Confusion());
  //Intersections
  IntAna2d_AnaIntersection anInters(aLin2d, aCirc2d);

  myDone = anExt2d.IsDone() || anInters.IsDone();

  if (!myDone)
    return Standard_True;

  const Standard_Integer aNbExtr = anExt2d.NbExt();
  const Standard_Integer aNbSol = anInters.NbPoints();

  const Standard_Integer aNbSum = aNbExtr + aNbSol;
  for (Standard_Integer anExtrID = 1; anExtrID <= aNbSum; anExtrID++)
  {
    const Standard_Integer aDelta = anExtrID - aNbExtr;

    Standard_Real aLinPar = 0.0, aCircPar = 0.0;

    if (aDelta < 1)
    {
      Extrema_POnCurv2d aPLin2d, aPCirc2d;
      anExt2d.Points(anExtrID, aPLin2d, aPCirc2d);
      aLinPar = aPLin2d.Parameter();
      aCircPar = aPCirc2d.Parameter();
    }
    else
    {
      aLinPar = anInters.Point(aDelta).ParamOnFirst();
      aCircPar = anInters.Point(aDelta).ParamOnSecond();
    }

    const gp_Pnt aPOnL(ElCLib::LineValue(aLinPar, theLin.Position())),
                 aPOnC(ElCLib::CircleValue(aCircPar,
                                            theCirc.Position(), theCirc.Radius()));

    mySqDist[myNbExt] = aPOnL.SquareDistance(aPOnC);
    myPoint[myNbExt][0].SetValues(aLinPar, aPOnL);
    myPoint[myNbExt][1].SetValues(aCircPar, aPOnC);
    myNbExt++;
  }

  return Standard_True;
}

//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
// Find extreme distances between straight line C1 and circle C2.
//
//Method:
//   Let P1=C1(u1) and P2=C2(u2) be two solution points
//        D the direction of straight line C1
//	T tangent at point P2;
//  Then, ( P1P2.D = 0. (1)
//         ( P1P2.T = 0. (2)
//  Let O1 and O2 be the origins of C1 and C2;
//  Then, (1) <=> (O1P2-u1*D).D = 0.         as O1P1 = u1*D
//	     <=> u1 = O1P2.D                as D.D = 1.
//         (2) <=> P1O2.T = 0.                as O2P2.T = 0.
//             <=> ((P2O1.D)D+O1O2).T = 0.    as P1O1 = -u1*D = (P2O1.D)D
//	     <=> (((P2O2+O2O1).D)D+O1O2).T = 0.
//	     <=> ((P2O2.D)(D.T)+((O2O1.D)D-O2O1).T = 0.
//  We are in the reference of the circle; let:
//         Cos = Cos(u2) and Sin = Sin(u2),
//         P2 (R*Cos,R*Sin,0.),
//         T (-R*Sin,R*Cos,0.),
//	 D (Dx,Dy,Dz),
//	 V (Vx,Vy,Vz) = (O2O1.D)D-O2O1;
//  Then, the equation by Cos and Sin is as follows:
//    -(2*R*R*Dx*Dy)   * Cos**2  +       A1
//   R*R*(Dx**2-Dy**2) * Cos*Sin +    2* A2
//         R*Vy        * Cos     +       A3
//	-R*Vx        * Sin     +       A4
//      R*R*Dx*Dy                = 0.    A5
//Use the algorithm math_TrigonometricFunctionRoots to solve this equation.
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Lin& C1, 
				const gp_Circ& C2,
				const Standard_Real)
{
  Standard_Real Dx,Dy,Dz,aRO2O1, aTolRO2O1;
  Standard_Real R, A1, A2, A3, A4, A5, aTol;
  gp_Dir x2, y2, z2, D, D1;
  //
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

  if (PlanarLineCircleExtrema(C1, C2))
  {
    return;
  }

  // Calculate T1 in the reference of the circle ...
  D = C1.Direction();
  D1 = D;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();
  z2 = C2.Axis().Direction();
  Dx = D.Dot(x2);
  Dy = D.Dot(y2);
  Dz = D.Dot(z2);
  //
  D.SetCoord(Dx, Dy, Dz);
  RefineDir(D);
  D.Coord(Dx, Dy, Dz);
  //
  // Calcul de V dans le repere du cercle:
  gp_Pnt O1 = C1.Location();
  gp_Pnt O2 = C2.Location();
  gp_Vec O2O1 (O2,O1);
  //
  aTolRO2O1=gp::Resolution();
  aRO2O1=O2O1.Magnitude();
  if (aRO2O1 > aTolRO2O1) {
    gp_Dir aDO2O1;
    //
    O2O1.Multiply(1./aRO2O1);
    aDO2O1.SetCoord(O2O1.Dot(x2), O2O1.Dot(y2), O2O1.Dot(z2));
    RefineDir(aDO2O1);
    O2O1.SetXYZ(aRO2O1*aDO2O1.XYZ());
  }
  else {
    O2O1.SetCoord(O2O1.Dot(x2), O2O1.Dot(y2), O2O1.Dot(z2));
  }
  //
  gp_XYZ Vxyz = (D.XYZ()*(O2O1.Dot(D)))-O2O1.XYZ();
  //
  //modified by NIZNHY-PKV Tue Mar 20 10:36:38 2012
  /*
  R = C2.Radius();
  A5 = R*R*Dx*Dy;
  A1 = -2.*A5;
  A2 = R*R*(Dx*Dx-Dy*Dy)/2.;
  A3 = R*Vxyz.Y();
  A4 = -R*Vxyz.X();
  //
  aTol=1.e-12;
  //

  if(fabs(A5) <= aTol) {
    A5 = 0.;
  }
  if(fabs(A1) <= aTol) {
    A1 = 0.;
  }
  if(fabs(A2) <= aTol) {
    A2 = 0.;
  }
  if(fabs(A3) <= aTol) {
    A3 = 0.;
  }
  if(fabs(A4) <= aTol) {
    A4 = 0.;
  }
  */
  //
  aTol=1.e-12;
  // Calculate the coefficients of the equation by Cos and Sin ...
  // [divided by R]
  R = C2.Radius();
  A5 = R*Dx*Dy;
  A1 = -2.*A5;
  A2 = 0.5*R*(Dx*Dx-Dy*Dy);// /2.;
  A3 = Vxyz.Y();
  A4 = -Vxyz.X();
  //
  if (A1>=-aTol && A1<=aTol) {
    A1 = 0.;
  }
  if (A2>=-aTol && A2<=aTol) {
    A2 = 0.;
  }
  if (A3>=-aTol && A3<=aTol) {
    A3 = 0.;
  }
  if (A4>=-aTol && A4<=aTol) {
    A4 = 0.;
  }
  if (A5>=-aTol && A5<=aTol) {
    A5 = 0.;
  }
  //modified by NIZNHY-PKV Tue Mar 20 10:36:40 2012t
  //
  ExtremaExtElC_TrigonometricRoots Sol(A1, A2, A3, A4, A5, 0., M_PI+M_PI);
  if (!Sol.IsDone()) { 
    return; 
  }
  if (Sol.InfiniteRoots()) { 
    myIsPar = Standard_True;
    mySqDist[0] = R*R;
    myDone = Standard_True;
    return; 
  }
  // Storage of solutions ...
  Standard_Integer NoSol, NbSol;
  Standard_Real U1,U2;
  gp_Pnt P1,P2;
  //
  NbSol = Sol.NbSolutions();
  for (NoSol=1; NoSol<=NbSol; ++NoSol) {
    U2 = Sol.Value(NoSol);
    P2 = ElCLib::Value(U2,C2);
    U1 = (gp_Vec(O1,P2)).Dot(D1);
    P1 = ElCLib::Value(U1,C1);
    mySqDist[myNbExt] = P1.SquareDistance(P2);
    //modified by NIZNHY-PKV Wed Mar 21 08:11:33 2012f
    //myPoint[myNbExt][0] = Extrema_POnCurv(U1,P1);
    //myPoint[myNbExt][1] = Extrema_POnCurv(U2,P2);
    myPoint[myNbExt][0].SetValues(U1,P1);
    myPoint[myNbExt][1].SetValues(U2,P2);
    //modified by NIZNHY-PKV Wed Mar 21 08:11:36 2012t
    myNbExt++;
  }
  myDone = Standard_True;
}
//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Lin& C1,
				const gp_Elips& C2)
{
/*-----------------------------------------------------------------------------
Function:
  Find extreme distances between straight line C1 and ellipse C2.

Method:
  Let P1=C1(u1) and P2=C2(u2) two solution points
        D the direction of straight line C1
	T the tangent to point P2;
  Then, ( P1P2.D = 0. (1)
         ( P1P2.T = 0. (2)
  Let O1 and O2 be the origins of C1 and C2;
  Then, (1) <=> (O1P2-u1*D).D = 0.        as O1P1 = u1*D
	     <=> u1 = O1P2.D              as D.D = 1.
         (2) <=> P1O2.T = 0.              as O2P2.T = 0.
             <=> ((P2O1.D)D+O1O2).T = 0.  as P1O1 = -u1*D = (P2O1.D)D
	     <=> (((P2O2+O2O1).D)D+O1O2).T = 0.
	     <=> ((P2O2.D)(D.T)+((O2O1.D)D-O2O1).T = 0.
  We are in the reference of the ellipse; let:
         Cos = Cos(u2) and Sin = Sin(u2),
         P2 (MajR*Cos,MinR*Sin,0.),
         T (-MajR*Sin,MinR*Cos,0.),
	 D (Dx,Dy,Dz),
	 V (Vx,Vy,Vz) = (O2O1.D)D-O2O1;
  Then,  get the following equation by Cos and Sin:
    -(2*MajR*MinR*Dx*Dy)             * Cos**2  +
   (MajR*MajR*Dx**2-MinR*MinR*Dy**2) * Cos*Sin +
         MinR*Vy                     * Cos     +
       - MajR*Vx                     * Sin     +
      MinR*MajR*Dx*Dy                = 0.
  Use algorithm math_TrigonometricFunctionRoots to solve this equation.
-----------------------------------------------------------------------------*/
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calculate T1 the reference of the ellipse ...
  gp_Dir D = C1.Direction();
  gp_Dir D1 = D;
  gp_Dir x2, y2, z2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();
  z2 = C2.Axis().Direction();
  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);
  Standard_Real Dz = D.Dot(z2);
  D.SetCoord(Dx,Dy,Dz);

// Calculate V ...
  gp_Pnt O1 = C1.Location();
  gp_Pnt O2 = C2.Location();
  gp_Vec O2O1 (O2,O1);
  O2O1.SetCoord(O2O1.Dot(x2), O2O1.Dot(y2), O2O1.Dot(z2));
  gp_XYZ Vxyz = (D.XYZ()*(O2O1.Dot(D)))-O2O1.XYZ();

// Calculate the coefficients of the equation by Cos and Sin ...
  Standard_Real MajR = C2.MajorRadius();
  Standard_Real MinR = C2.MinorRadius();
  Standard_Real A5 = MajR*MinR*Dx*Dy;
  Standard_Real A1 = -2.*A5;
  Standard_Real R2 = MajR*MajR;
  Standard_Real r2 = MinR*MinR;
  Standard_Real A2 =(R2*Dx*Dx -r2*Dy*Dy -R2 +r2)/2.0;
  Standard_Real A3 = MinR*Vxyz.Y();
  Standard_Real A4 = -MajR*Vxyz.X();
  //
  Standard_Real aEps=1.e-12;
  //
  if(fabs(A5) <= aEps) A5 = 0.;
  if(fabs(A1) <= aEps) A1 = 0.;
  if(fabs(A2) <= aEps) A2 = 0.;
  if(fabs(A3) <= aEps) A3 = 0.;
  if(fabs(A4) <= aEps) A4 = 0.;
  //
  ExtremaExtElC_TrigonometricRoots Sol(A1,A2,A3,A4,A5,0.,M_PI+M_PI);
  if (!Sol.IsDone()) { return; }

// Storage of solutions ...
  gp_Pnt P1,P2;
  Standard_Real U1,U2;
  Standard_Integer NbSol = Sol.NbSolutions();
  for (Standard_Integer NoSol = 1; NoSol <= NbSol; NoSol++) {
    U2 = Sol.Value(NoSol);
    P2 = ElCLib::Value(U2,C2);
    U1 = (gp_Vec(O1,P2)).Dot(D1);
    P1 = ElCLib::Value(U1,C1);
    mySqDist[myNbExt] = P1.SquareDistance(P2);
    myPoint[myNbExt][0] = Extrema_POnCurv(U1,P1);
    myPoint[myNbExt][1] = Extrema_POnCurv(U2,P2);
    myNbExt++;
  }
  myDone = Standard_True;
}

//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Lin& C1, 
				const gp_Hypr& C2)
{
/*-----------------------------------------------------------------------------
Function:
  Find extrema between straight line C1 and hyperbola C2.

Method:
  Let P1=C1(u1) and P2=C2(u2) be two solution points
        D the direction of straight line C1
	T the tangent at point P2;
  Then, ( P1P2.D = 0. (1)
        ( P1P2.T = 0. (2)
  Let O1 and O2 be the origins of C1 and C2;
  Then, (1) <=> (O1P2-u1*D).D = 0.         as O1P1 = u1*D
	     <=> u1 = O1P2.D               as D.D = 1.
         (2) <=> (P1O2 + O2P2).T= 0.
             <=> ((P2O1.D)D+O1O2 + O2P2).T = 0.  as P1O1 = -u1*D = (P2O1.D)D
	     <=> (((P2O2+O2O1).D)D+O1O2 + O2P2).T = 0.
	     <=> (P2O2.D)(D.T)+((O2O1.D)D-O2O1).T + O2P2.T= 0.
  We are in the reference of the hyperbola; let:
         by writing P (R* Chu, r* Shu, 0.0)
	 and Chu = (v**2 + 1)/(2*v) ,
	     Shu = (V**2 - 1)/(2*v)

	 T(R*Shu, r*Chu)
	 D (Dx,Dy,Dz),
	 V (Vx,Vy,Vz) = (O2O1.D)D-O2O1;

  Then we obtain the following equation by v:
         (-2*R*r*Dx*Dy - R*R*Dx*Dx-r*r*Dy*Dy + R*R + r*r)     * v**4  +
	 (2*R*Vx + 2*r*Vy)                                    * v**3  +
	 (-2*R*Vx + 2*r*Vy)                                   * v     +
	 (-2*R*r*Dx*Dy - (R*R*Dx*Dx-r*r*Dy*Dy + R*R + r*r))  = 0


  Use the algorithm math_DirectPolynomialRoots to solve this equation.
-----------------------------------------------------------------------------*/
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calculate T1 in the reference of the hyperbola...
  gp_Dir D = C1.Direction();
  gp_Dir D1 = D;
  gp_Dir x2, y2, z2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();
  z2 = C2.Axis().Direction();
  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);
  Standard_Real Dz = D.Dot(z2);
  D.SetCoord(Dx,Dy,Dz);

// Calculate V ...
  gp_Pnt O1 = C1.Location();
  gp_Pnt O2 = C2.Location();
  gp_Vec O2O1 (O2,O1);
  O2O1.SetCoord(O2O1.Dot(x2), O2O1.Dot(y2), O2O1.Dot(z2));
  gp_XYZ Vxyz = (D.XYZ()*(O2O1.Dot(D)))-O2O1.XYZ();
  Standard_Real Vx = Vxyz.X();
  Standard_Real Vy = Vxyz.Y();

// Calculate coefficients of the equation by v
  Standard_Real R = C2.MajorRadius();
  Standard_Real r = C2.MinorRadius();
  Standard_Real a = -2*R*r*Dx*Dy;
  Standard_Real b = -R*R*Dx*Dx - r*r*Dy*Dy + R*R + r*r;
  Standard_Real A1 = a + b;
  Standard_Real A2 = 2*R*Vx + 2*r*Vy;
  Standard_Real A4 = -2*R*Vx + 2*r*Vy;
  Standard_Real A5 = a - b;

  math_DirectPolynomialRoots Sol(A1,A2,0.0,A4, A5);
  if (!Sol.IsDone()) { return; }

// Store solutions ...
  gp_Pnt P1,P2;
  Standard_Real U1,U2, v;
  Standard_Integer NbSol = Sol.NbSolutions();
  for (Standard_Integer NoSol = 1; NoSol <= NbSol; NoSol++) {
    v = Sol.Value(NoSol);
    if (v > 0.0) {
      U2 = Log(v);
      P2 = ElCLib::Value(U2,C2);
      U1 = (gp_Vec(O1,P2)).Dot(D1);
      P1 = ElCLib::Value(U1,C1);
      mySqDist[myNbExt] = P1.SquareDistance(P2);
      myPoint[myNbExt][0] = Extrema_POnCurv(U1,P1);
      myPoint[myNbExt][1] = Extrema_POnCurv(U2,P2);
      myNbExt++;
    }
  }
  myDone = Standard_True;
}
//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Lin& C1, 
				const gp_Parab& C2)
{
/*-----------------------------------------------------------------------------
Function:
  Find extreme distances between straight line C1 and parabole C2.

Method:
   Let P1=C1(u1) and P2=C2(u2) be two solution points
        D the direction of straight line C1
	T the tangent to point P2;
  Then, ( P1P2.D = 0. (1)
        ( P1P2.T = 0. (2)
  Let O1 and O2 be the origins of C1 and C2;
  Then, (1) <=> (O1P2-u1*D).D = 0.         as O1P1 = u1*D
	     <=> u1 = O1P2.D               as D.D = 1.
         (2) <=> (P1O2 + O2P2).T= 0.
             <=> ((P2O1.D)D+O1O2 + O2P2).T = 0.  as P1O1 = -u1*D = (P2O1.D)D
	     <=> (((P2O2+O2O1).D)D+O1O2 + O2P2).T = 0.
	     <=> (P2O2.D)(D.T)+((O2O1.D)D-O2O1).T + O2P2.T = 0.
  We are in the reference of the parabola; let:
         P2 (y*y/(2*p), y, 0)
         T (y/p, 1, 0)
	 D (Dx,Dy,Dz),
	 V (Vx,Vy,Vz) = (O2O1.D)D-O2O1;

  Then, get the following equation by y:
     ((1-Dx*Dx)/(2*p*p))            *  y*y*y  +        A1
     (-3*Dx*Dy/(2*p))               *  y*y    +        A2
     (1-Dy*Dy + Vx/p)               *  y      +        A3 
        Vy                          = 0.               A4

  Use the algorithm math_DirectPolynomialRoots to solve this equation.
-----------------------------------------------------------------------------*/
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calculate T1 in the reference of the parabola...
  gp_Dir D = C1.Direction();
  gp_Dir D1 = D;
  gp_Dir x2, y2, z2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();
  z2 = C2.Axis().Direction();
  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);
  Standard_Real Dz = D.Dot(z2);
  D.SetCoord(Dx,Dy,Dz);

// Calculate V ...
  gp_Pnt O1 = C1.Location();
  gp_Pnt O2 = C2.Location();
  gp_Vec O2O1 (O2,O1);
  O2O1.SetCoord(O2O1.Dot(x2), O2O1.Dot(y2), O2O1.Dot(z2));
  gp_XYZ Vxyz = (D.XYZ()*(O2O1.Dot(D)))-O2O1.XYZ();

// Calculate coefficients of the equation by y
  Standard_Real P = C2.Parameter();
  Standard_Real A1 = (1-Dx*Dx)/(2.0*P*P);
  Standard_Real A2 = (-3.0*Dx*Dy/(2.0*P));
  Standard_Real A3 = (1 - Dy*Dy + Vxyz.X()/P);
  Standard_Real A4 = Vxyz.Y();

  math_DirectPolynomialRoots Sol(A1,A2,A3,A4);
  if (!Sol.IsDone()) { return; }

// Storage of solutions ...
  gp_Pnt P1,P2;
  Standard_Real U1,U2;
  Standard_Integer NbSol = Sol.NbSolutions();
  for (Standard_Integer NoSol = 1; NoSol <= NbSol; NoSol++) {
    U2 = Sol.Value(NoSol);
    P2 = ElCLib::Value(U2,C2);
    U1 = (gp_Vec(O1,P2)).Dot(D1);
    P1 = ElCLib::Value(U1,C1);
    mySqDist[myNbExt] = P1.SquareDistance(P2);
    myPoint[myNbExt][0] = Extrema_POnCurv(U1,P1);
    myPoint[myNbExt][1] = Extrema_POnCurv(U2,P2);
    myNbExt++;
  }
  myDone = Standard_True;
}
//=======================================================================
//function : Extrema_ExtElC
//purpose  : 
//=======================================================================
Extrema_ExtElC::Extrema_ExtElC (const gp_Circ& C1, 
				const gp_Circ& C2)
{
  Standard_Boolean bIsSamePlane, bIsSameAxe;
  Standard_Real aTolD, aTolD2, aTolA, aD2, aDC2;
  gp_Pnt aPc1, aPc2;
  gp_Dir aDc1, aDc2;
  //
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;
  //
  aTolA=Precision::Angular();
  aTolD=Precision::Confusion();
  aTolD2=aTolD*aTolD;
  //
  aPc1=C1.Location();
  aDc1=C1.Axis().Direction();
    
  aPc2=C2.Location();
  aDc2=C2.Axis().Direction();
  gp_Pln aPlc1(aPc1, aDc1);
  //
  aD2=aPlc1.SquareDistance(aPc2);
  bIsSamePlane=aDc1.IsParallel(aDc2, aTolA) && aD2<aTolD2;
  if (!bIsSamePlane) {
    return;
  }
  //
  aDC2=aPc1.SquareDistance(aPc2);
  bIsSameAxe=aDC2<aTolD2;
  //
  if(bIsSameAxe) {
    myIsPar = Standard_True;
    Standard_Real dR = C1.Radius() - C2.Radius();
    Standard_Real dC = C1.Location().Distance(C2.Location());
    mySqDist[0] = dR*dR + dC*dC;
    dR = C1.Radius() + C2.Radius();
    mySqDist[1] = dR*dR + dC*dC;
    myDone = Standard_True; 
  }
  else {
    Standard_Boolean bIn, bOut;
    Standard_Integer j1, j2;
    Standard_Real aR1, aR2, aD12, aT11, aT12, aT21, aT22;
    gp_Circ aC1, aC2;
    gp_Pnt aP11, aP12, aP21, aP22;
    //
    myDone = Standard_True; 
    //
    aR1=C1.Radius();
    aR2=C2.Radius();
    //
    j1=0;
    j2=1;
    aC1=C1;
    aC2=C2;
    if (aR2>aR1) {
      j1=1;
      j2=0;
      aC1=C2;
      aC2=C1;
    }
    //
    aR1=aC1.Radius(); // max radius
    aR2=aC2.Radius(); // min radius
    //
    aPc1=aC1.Location();
    aPc2=aC2.Location();
    //
    aD12=aPc1.Distance(aPc2);
    gp_Vec aVec12(aPc1, aPc2);
    gp_Dir aDir12(aVec12);
    //
    // 1. Four common solutions
    myNbExt=4;
    //
    aP11.SetXYZ(aPc1.XYZ()-aR1*aDir12.XYZ());
    aP12.SetXYZ(aPc1.XYZ()+aR1*aDir12.XYZ());
    aP21.SetXYZ(aPc2.XYZ()-aR2*aDir12.XYZ());
    aP22.SetXYZ(aPc2.XYZ()+aR2*aDir12.XYZ());
    //
    aT11=ElCLib::Parameter(aC1, aP11);
    aT12=ElCLib::Parameter(aC1, aP12);
    aT21=ElCLib::Parameter(aC2, aP21);
    aT22=ElCLib::Parameter(aC2, aP22);
    //
    // P11, P21
    myPoint[0][j1].SetValues(aT11, aP11);
    myPoint[0][j2].SetValues(aT21, aP21);
    mySqDist[0]=aP11.SquareDistance(aP21);
    // P11, P22
    myPoint[1][j1].SetValues(aT11, aP11);
    myPoint[1][j2].SetValues(aT22, aP22);
    mySqDist[1]=aP11.SquareDistance(aP22);
    //
    // P12, P21
    myPoint[2][j1].SetValues(aT12, aP12);
    myPoint[2][j2].SetValues(aT21, aP21);
    mySqDist[2]=aP12.SquareDistance(aP21);
    //
    // P12, P22
    myPoint[3][j1].SetValues(aT12, aP12);
    myPoint[3][j2].SetValues(aT22, aP22);
    mySqDist[3]=aP12.SquareDistance(aP22);
    //
    // 2. Check for intersections
    bOut=aD12>(aR1+aR2+aTolD);
    bIn =aD12<(aR1-aR2-aTolD);
    if (!bOut && !bIn) {
      Standard_Boolean bNbExt6;
      Standard_Real aAlpha, aBeta, aT[2], aVal, aDist2;
      gp_Pnt aPt, aPL1, aPL2;
      gp_Dir aDLt;
      //
      aAlpha=0.5*(aR1*aR1-aR2*aR2+aD12*aD12)/aD12;
      aVal=aR1*aR1-aAlpha*aAlpha;
      if (aVal<0.) {// see pkv/900/L4 for details
	aVal=-aVal;
      }
      aBeta=Sqrt(aVal);
      //aBeta=Sqrt(aR1*aR1-aAlpha*aAlpha);
      //--
      aPt.SetXYZ(aPc1.XYZ()+aAlpha*aDir12.XYZ());
      //
      aDLt=aDc1^aDir12;
      aPL1.SetXYZ(aPt.XYZ()+aBeta*aDLt.XYZ());
      aPL2.SetXYZ(aPt.XYZ()-aBeta*aDLt.XYZ());
      //
      aDist2=aPL1.SquareDistance(aPL2);
      bNbExt6=aDist2>aTolD2;
      //
      myNbExt=5;// just in case. see pkv/900/L4 for details
      aT[j1]=ElCLib::Parameter(aC1, aPL1);
      aT[j2]=ElCLib::Parameter(aC2, aPL1);
      myPoint[4][j1].SetValues(aT[j1], aPL1);
      myPoint[4][j2].SetValues(aT[j2], aPL1);
      mySqDist[4]=0.;
      //
      if (bNbExt6) {
	myNbExt=6;
	aT[j1]=ElCLib::Parameter(aC1, aPL2);
	aT[j2]=ElCLib::Parameter(aC2, aPL2);
	myPoint[5][j1].SetValues(aT[j1], aPL2);
	myPoint[5][j2].SetValues(aT[j2], aPL2);
	mySqDist[5]=0.;
      }
      //
    }// if (!bOut || !bIn) {
  }// else
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtElC::IsDone () const {
  return myDone; 
}
//=======================================================================
//function : IsParallel
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtElC::IsParallel () const
{
  if (!IsDone()) { 
    throw StdFail_NotDone();
  }
  return myIsPar;
}
//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================
Standard_Integer Extrema_ExtElC::NbExt () const
{
  if (IsParallel()) {
    throw StdFail_InfiniteSolutions();
  }
  return myNbExt;
}
//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================
Standard_Real Extrema_ExtElC::SquareDistance (const Standard_Integer N) const
{
  if (!myDone) { 
    throw StdFail_NotDone();
  }
  if (myIsPar) {
    if (N < 1 || N > 2) { 
      throw Standard_OutOfRange();
    }
  }
  else {  
    if (N < 1 || N > NbExt()) { 
      throw Standard_OutOfRange();
    }
  }
  return mySqDist[N-1];
}
//=======================================================================
//function : Points
//purpose  : 
//=======================================================================
void Extrema_ExtElC::Points (const Standard_Integer N,
			     Extrema_POnCurv& P1, 
			     Extrema_POnCurv& P2) const
{
  if (N < 1 || N > NbExt()) { 
    throw Standard_OutOfRange();
  }
  P1 = myPoint[N-1][0];
  P2 = myPoint[N-1][1];
}


//=======================================================================
//function : RefineDir
//purpose  : 
//=======================================================================
void RefineDir(gp_Dir& aDir)
{
  Standard_Integer i, j, k, iK;
  Standard_Real aCx[3], aEps, aX1, aX2, aOne;
  //
  iK=3;
  aEps=RealEpsilon();
  aDir.Coord(aCx[0], aCx[1], aCx[2]);
  //
  for (i=0; i<iK; ++i) {
    aOne=(aCx[i]>0.) ? 1. : -1.;
    aX1=aOne-aEps;
    aX2=aOne+aEps;
    //
    if (aCx[i]>aX1 && aCx[i]<aX2) {
      j=(i+1)%iK;
      k=(i+2)%iK;
      aCx[i]=aOne;
      aCx[j]=0.;
      aCx[k]=0.;
      aDir.SetCoord(aCx[0], aCx[1], aCx[2]);
      return;
    }
  }
}
