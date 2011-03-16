// File:	Extrema_ExtElC2d.cxx
// Created:	Tue Jan  4 11:42:51 1994
// Author:	Christophe MARION
//		<cma@sdsun1>


#include <Extrema_ExtElC2d.ixx>

#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>
#include <ElCLib.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NotImplemented.hxx>
#include <Precision.hxx>
#include <Extrema_ExtPElC2d.hxx>

//=============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d () { myDone = Standard_False; }
//=============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Lin2d& C1, 
				    const gp_Lin2d& C2,
				    const Standard_Real)
/*-----------------------------------------------------------------------------
Fonction:
   Recherche de la distance minimale entre 2 droites.

Methode:
  Soit D1 et D2, les 2 directions des droites C1 et C2.
  2 cas sont consideres:
  1- si Angle(D1,D2) < AngTol, les droites sont paralleles.
     La distance est la distance entre un point quelconque de C1 et la droite
     C2.
  2- si Angle(D1,D2) > AngTol:
     Soit P = C1(u1) et P =C2(u2) le point intersection:
     
-----------------------------------------------------------------------------*/
{
  myDone = Standard_False;
  myIsPar = Standard_False;
  myNbExt = 0;

  gp_Dir2d D1 = C1.Direction();
  gp_Dir2d D2 = C2.Direction();
  if (D1.IsParallel(D2, Precision::Angular())) {
    myIsPar = Standard_True;
    mySqDist[0] = C2.SquareDistance(C1.Location());
  }
  else {
    myNbExt = 0;
  }
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Lin2d& C1, 
				    const gp_Circ2d& C2,
				    const Standard_Real)
/*-----------------------------------------------------------------------------
Fonction:
   Recherche des distances extremales entre la droite C1 et le cercle C2.

Methode:
   Soit P1=C1(u1) et P2=C2(u2) deux points solutions
        D la direction de la droite C1
	T la tangente au point P2;
  Alors, ( P1P2.D = 0. (1)
         ( P1P2.T = 0. (2)
-----------------------------------------------------------------------------*/
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calcul de T1 dans le repere du cercle ...
  gp_Dir2d D = C1.Direction();
  gp_Dir2d x2, y2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();

  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);
  Standard_Real U1, teta[2];
  gp_Pnt2d O1=C1.Location();
#ifdef DEB
  gp_Pnt2d O2= C2.Location();
  gp_Pnt2d P1, P2;
#else
  C2.Location();
  gp_Pnt2d P1, P2;
#endif
  
  if (Abs(Dy) <= RealEpsilon()) {
    teta[0] = PI/2.0;
  }
  else  teta[0] = ATan(-Dx/Dy);
  teta[1] = teta[0]+ PI;
  if (teta[0] < 0.0) teta[0] = teta[0] + 2.0*PI;

  P2 = ElCLib::Value(teta[0], C2);
  U1 = (gp_Vec2d(O1, P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(teta[0],P2);
  myNbExt++;

  P2 = ElCLib::Value(teta[1], C2);
  U1 = (gp_Vec2d(O1, P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(teta[1],P2);
  myNbExt++;
  myDone = Standard_True;
}


// =============================================================================
Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Lin2d& C1, 
				    const gp_Elips2d& C2)
{
  myDone = Standard_True;
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calcul de T1 dans le repere de l ellipse ...
  gp_Dir2d D = C1.Direction();
  gp_Dir2d x2, y2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();

  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);
  Standard_Real U1, teta[2], r1 = C2.MajorRadius(), r2 = C2.MinorRadius();
#ifdef DEB
  gp_Pnt2d O1=C1.Location(), O2= C2.Location(), P1, P2;
#else
  gp_Pnt2d O1=C1.Location(), P1, P2;
#endif
  
  if (Abs(Dy) <= RealEpsilon()) {
    teta[0] = PI/2.0;
  }
  else  teta[0] = ATan(-Dx*r2/(Dy*r1));

  teta[1] = teta[0] + PI;
  if (teta[0] < 0.0) teta[0] += 2.0*PI;
  P2 = ElCLib::Value(teta[0], C2);
  U1 = (gp_Vec2d(O1, P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(teta[0],P2);
  myNbExt++;


  P2 = ElCLib::Value(teta[1], C2);
  U1 = (gp_Vec2d(O1, P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(teta[1],P2);
  myNbExt++;
  myDone = Standard_True;
}



//=============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Lin2d& C1, const gp_Hypr2d& C2)
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calcul de T1 dans le repere de la parabole ...
  gp_Dir2d D = C1.Direction();
  gp_Dir2d x2, y2;
  x2 = C2.XAxis().Direction();
  y2 = C2.YAxis().Direction();
  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);

  Standard_Real U1, v2, U2=0, R = C2.MajorRadius(), r = C2.MinorRadius();
  gp_Pnt2d P1, P2;
  if (Abs(Dy) < RealEpsilon()) { return;}
  if (Abs(R - r*Dx/Dy) < RealEpsilon()) return;

  v2 = (R + r*Dx/Dy)/(R - r*Dx/Dy);
  if (v2 > 0.0) U2 = Log(Sqrt(v2));
  P2 = ElCLib::Value(U2, C2);

  U1 = (gp_Vec2d(C1.Location(), P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(U2,P2);
  myNbExt++;
  myDone = Standard_True;
}



//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Lin2d& C1, const gp_Parab2d& C2)
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

// Calcul de T1 dans le repere de la parabole ...
  gp_Dir2d D = C1.Direction();
  gp_Dir2d x2, y2;
  x2 = C2.MirrorAxis().Direction();
  y2 = C2.Axis().YAxis().Direction();
  Standard_Real Dx = D.Dot(x2);
  Standard_Real Dy = D.Dot(y2);

  Standard_Real U1, U2, P = C2.Parameter();
  gp_Pnt2d P1, P2;
  if (Abs(Dy) < RealEpsilon()) { return; }
  U2 = Dx*P/Dy;
  P2 = ElCLib::Value(U2, C2);

  U1 = (gp_Vec2d(C1.Location(), P2)).Dot(D);
  P1 = ElCLib::Value(U1, C1);
  mySqDist[myNbExt] = P1.SquareDistance(P2);
  myPoint[myNbExt][0] = Extrema_POnCurv2d(U1,P1);
  myPoint[myNbExt][1] = Extrema_POnCurv2d(U2,P2);
  myNbExt++;
  myDone = Standard_True;
}



//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Circ2d& C1, const gp_Circ2d& C2)
{
  myIsPar = Standard_False;
  myDone  = Standard_False;
  myNbExt = 0;
  myDone  = Standard_True;

  gp_Pnt2d O1 = C1.Location();
  gp_Pnt2d O2 = C2.Location();

  gp_Vec2d DO1O2 (O1, O2);
  if (DO1O2.Magnitude() < Precision::Confusion()) { 
    myIsPar = Standard_True;
    return; 
  }

  Standard_Integer NoSol, kk;
  Standard_Real U1, U2;
  Standard_Real r1 = C1.Radius(), r2 = C2.Radius();
  Standard_Real Usol2[2], Usol1[2];
  gp_Pnt2d P1[2], P2[2];
  gp_Dir2d O1O2(DO1O2);

  P1[0] = O1.Translated(r1*O1O2);
  Usol1[0] = ElCLib::Parameter(C1, P1[0]);
  P1[1] = O1.Translated(-r1*O1O2);
  Usol1[1] = ElCLib::Parameter(C1, P1[1]);
  
  P2[0] = O2.Translated(r2*O1O2);
  Usol2[0] = ElCLib::Parameter(C2, P2[0]);
  P2[1] = O2.Translated(-r2*O1O2);
  Usol2[1] = ElCLib::Parameter(C2, P2[1]);
  
  for (NoSol = 0; NoSol <= 1; NoSol++) {
    U1 = Usol1[NoSol];
    for (kk = 0; kk <= 1; kk++) {
      U2 = Usol2[kk];
      mySqDist[myNbExt] = P2[kk].SquareDistance(P1[NoSol]);
      myPoint[myNbExt][0] = Extrema_POnCurv2d(U1, P1[NoSol]);
      myPoint[myNbExt][1] = Extrema_POnCurv2d(U2, P2[kk]);
      myNbExt++;
    }
  }
}
//===========================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Circ2d& C1, const gp_Elips2d& C2)
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

  Standard_Integer i, j;

  Extrema_ExtPElC2d ExtElip(C1.Location(), C2, 
			    Precision::Confusion(), 0.0, 2.0*PI);
  
  if (ExtElip.IsDone()) {
    for (i = 1; i <= ExtElip.NbExt(); i++) {
      Extrema_ExtPElC2d ExtCirc(ExtElip.Point(i).Value(), C1, 
				Precision::Confusion(), 0.0, 2.0*PI);
      if (ExtCirc.IsDone()) {
	for (j = 1; j <= ExtCirc.NbExt(); j++) {
	  mySqDist[myNbExt] = ExtCirc.SquareDistance(j);
	  myPoint[myNbExt][0] = ExtCirc.Point(j);
	  myPoint[myNbExt][1] = ExtElip.Point(i);
	  myNbExt++;
	}
      }
      myDone = Standard_True;
    }
  }
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Circ2d& C1, const gp_Hypr2d& C2)
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

  Standard_Integer i, j;

  Extrema_ExtPElC2d ExtHyp(C1.Location(), C2, Precision::Confusion(), 
			   RealFirst(), RealLast());
  
  if (ExtHyp.IsDone()) {
    for (i = 1; i <= ExtHyp.NbExt(); i++) {
      Extrema_ExtPElC2d ExtCirc(ExtHyp.Point(i).Value(), C1, 
				Precision::Confusion(), 0.0, 2.0*PI);
      if (ExtCirc.IsDone()) {
	for (j = 1; j <= ExtCirc.NbExt(); j++) {
	  mySqDist[myNbExt] = ExtCirc.SquareDistance(j);
	  myPoint[myNbExt][0] = ExtCirc.Point(j);
	  myPoint[myNbExt][1] = ExtHyp.Point(i);
	  myNbExt++;
	}
      }
      myDone = Standard_True;
    }
  }
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Circ2d& C1, const gp_Parab2d& C2)
{
  myIsPar = Standard_False;
  myDone = Standard_False;
  myNbExt = 0;

  Standard_Integer i, j;

  Extrema_ExtPElC2d ExtParab(C1.Location(), C2, Precision::Confusion(),
			     RealFirst(), RealLast());
  
  if (ExtParab.IsDone()) {
    for (i = 1; i <= ExtParab.NbExt(); i++) {
      Extrema_ExtPElC2d ExtCirc(ExtParab.Point(i).Value(), 
				C1, Precision::Confusion(), 0.0, 2.0*PI);
      if (ExtCirc.IsDone()) {
	for (j = 1; j <= ExtCirc.NbExt(); j++) {
	  mySqDist[myNbExt] = ExtCirc.SquareDistance(j);
	  myPoint[myNbExt][0] = ExtCirc.Point(j);
	  myPoint[myNbExt][1] = ExtParab.Point(i);
	  myNbExt++;
	}
      }
      myDone = Standard_True;
    }
  }
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Elips2d&, const gp_Elips2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Elips2d&, const gp_Hypr2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Elips2d&, const gp_Parab2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Hypr2d&, const gp_Hypr2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Hypr2d&, const gp_Parab2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Extrema_ExtElC2d::Extrema_ExtElC2d (const gp_Parab2d&, const gp_Parab2d&)
{
  Standard_NotImplemented::Raise();
}
//============================================================================

Standard_Boolean Extrema_ExtElC2d::IsDone () const { return myDone; }
//============================================================================

Standard_Boolean Extrema_ExtElC2d::IsParallel () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myIsPar;
}
//============================================================================

Standard_Integer Extrema_ExtElC2d::NbExt () const
{
  if (IsParallel()) { StdFail_InfiniteSolutions::Raise(); }
  return myNbExt;
}
//============================================================================

Standard_Real Extrema_ExtElC2d::SquareDistance (const Standard_Integer N) const
{
  if (!(N == 1 && myDone)) {
    if (N < 1 || N > NbExt()) { Standard_OutOfRange::Raise(); }
  }
  return mySqDist[N-1];
}
//============================================================================

void Extrema_ExtElC2d::Points (const Standard_Integer N,
			       Extrema_POnCurv2d& P1, 
			       Extrema_POnCurv2d& P2) const
{
  if (N < 1 || N > NbExt()) { Standard_OutOfRange::Raise(); }
  P1 = myPoint[N-1][0];
  P2 = myPoint[N-1][1];
}
//============================================================================
