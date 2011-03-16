#include <Extrema_ExtPElC.ixx>
#include <StdFail_NotDone.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <math_TrigonometricFunctionRoots.hxx>
#include <ElCLib.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NotImplemented.hxx>
#include <Precision.hxx>


//=============================================================================

Extrema_ExtPElC::Extrema_ExtPElC () { myDone = Standard_False; }
//=============================================================================

Extrema_ExtPElC::Extrema_ExtPElC (const gp_Pnt&       P, 
				  const gp_Lin&       L,
				  const Standard_Real Tol,
				  const Standard_Real Uinf, 
				  const Standard_Real Usup)
{
  Perform(P, L, Tol, Uinf, Usup);
}

void Extrema_ExtPElC::Perform(const gp_Pnt&       P, 
			      const gp_Lin&       L,
			      const Standard_Real Tol,
			      const Standard_Real Uinf, 
			      const Standard_Real Usup)
{
  myDone = Standard_False;
  myNbExt = 0;
  gp_Vec V1 = gp_Vec(L.Direction());
  gp_Pnt OR = L.Location();
  gp_Vec V(OR, P);
  Standard_Real Mydist = V1.Dot(V);
  if ((Mydist >= Uinf-Tol) && 
      (Mydist <= Usup+Tol)){ 
    
    gp_Pnt MyP = OR.Translated(Mydist*V1);
    Extrema_POnCurv MyPOnCurve(Mydist, MyP);
    mySqDist[0] = P.SquareDistance(MyP);
    myPoint[0] = MyPOnCurve;
    myIsMin[0] = Standard_True;
    myNbExt = 1;
    myDone = Standard_True;
  }

}



Extrema_ExtPElC::Extrema_ExtPElC (const gp_Pnt&       P,
				  const gp_Circ&      C,
				  const Standard_Real Tol,
				  const Standard_Real Uinf,
				  const Standard_Real Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}

void Extrema_ExtPElC::Perform(const gp_Pnt&       P, 
			      const gp_Circ&      C,
			      const Standard_Real Tol,
			      const Standard_Real Uinf,
			      const Standard_Real Usup)
/*-----------------------------------------------------------------------------
Fonction:
  Recherche des valeurs de parametre u telle que:
   - dist(P,C(u)) passe par un extremum,
   - Uinf <= u <= Usup.

Methode:
  On procede en 3 temps:
  1- Projection du point P dans le plan du cercle,
  2- Calculs des u solutions dans [0.,2.*PI]:
      Soit Pp, le point projete et
           O, le centre du cercle;
     2 cas:
     - si Pp est confondu avec O, il y a une infinite de solutions;
       IsDone() renvoie Standard_False.
     - sinon, 2 points sont solutions pour le cercle complet:
       . Us1 = angle(OPp,OX) correspond au minimum,
       . soit Us2 = ( Us1 + PI si Us1 < PI,
                    ( Us1 - PI sinon;
         Us2 correspond au maximum.
  3- Calcul des extrema dans [Uinf,Usup].
-----------------------------------------------------------------------------*/
{
  myDone = Standard_False;
  myNbExt = 0;

// 1- Projection du point P dans le plan du cercle -> Pp ...

  gp_Pnt O = C.Location();
  gp_Vec Axe (C.Axis().Direction());
  gp_Vec Trsl = Axe.Multiplied(-(gp_Vec(O,P).Dot(Axe)));
  gp_Pnt Pp = P.Translated(Trsl);

// 2- Calcul des u solutions dans [0.,2.*PI] ...

  gp_Vec OPp (O,Pp);
  if (OPp.Magnitude() < Tol) { return; }
  Standard_Real Usol[2];
  Usol[0] = C.XAxis().Direction().AngleWithRef(OPp,Axe); // -PI<U1<PI
  Usol[1] = Usol[0] + PI;

  Standard_Real myuinf = Uinf;
  //modified by NIZNHY-PKV Fri Apr 20 15:03:28 2001 f
  //Standard_Real TolU = Tol*C.Radius();
  Standard_Real TolU, aR;
  aR=C.Radius();
  TolU=Precision::Infinite();
  if (aR > gp::Resolution()) {
    TolU= Tol/aR;
  }
  //modified by NIZNHY-PKV Fri Apr 20 15:03:32 2001 t
  ElCLib::AdjustPeriodic(Uinf, Uinf+2*PI, Precision::PConfusion(), myuinf, Usol[0]);
  ElCLib::AdjustPeriodic(Uinf, Uinf+2*PI, Precision::PConfusion(), myuinf, Usol[1]);
  if (((Usol[0]-2*PI-Uinf) < TolU) && ((Usol[0]-2*PI-Uinf) > -TolU)) Usol[0] = Uinf;
  if (((Usol[1]-2*PI-Uinf) < TolU) && ((Usol[1]-2*PI-Uinf) > -TolU)) Usol[1] = Uinf;


// 3- Calcul des extrema dans [Umin,Umax] ...

  gp_Pnt Cu;
  Standard_Real Us;
  for (Standard_Integer NoSol = 0; NoSol <= 1; NoSol++) {
    Us = Usol[NoSol];
    if (((Uinf-Us) < TolU) && ((Us-Usup) < TolU)) {
      Cu = ElCLib::Value(Us,C);
      mySqDist[myNbExt] = Cu.SquareDistance(P);
      myIsMin[myNbExt] = (NoSol == 0);
      myPoint[myNbExt] = Extrema_POnCurv(Us,Cu);
      myNbExt++;
    }
  }
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElC::Extrema_ExtPElC (const gp_Pnt&       P,
				  const gp_Elips&     C,
				  const Standard_Real Tol,
				  const Standard_Real Uinf, 
				  const Standard_Real Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}



void Extrema_ExtPElC::Perform (const gp_Pnt&       P, 
			       const gp_Elips&     C,
			       const Standard_Real Tol,
			       const Standard_Real Uinf, 
			       const Standard_Real Usup)
/*-----------------------------------------------------------------------------
Fonction:
  Recherche des valeurs de parametre u telle que:
   - dist(P,C(u)) passe par un extremum,
   - Uinf <= u <= Usup.

Methode:
  On procede en 2 temps:
  1- Projection du point P dans le plan de l'ellipse,
  2- Calculs des solutions:
      Soit Pp, le point projete; on recherche les valeurs u telles que:
       (C(u)-Pp).C'(u) = 0. (1)
     Soit Cos = cos(u) et Sin = sin(u),
          C(u) = (A*Cos,B*Sin) et Pp = (X,Y);
     Alors, (1) <=> (A*Cos-X,B*Sin-Y).(-A*Sin,B*Cos) = 0.
                    (B**2-A**2)*Cos*Sin - B*Y*Cos + A*X*Sin = 0.
     On utilise l'algorithme math_TrigonometricFunctionRoots pour resoudre
    cette equation.
-----------------------------------------------------------------------------*/
{
  myDone = Standard_False;
  myNbExt = 0;

// 1- Projection du point P dans le plan de l ellipse -> Pp ...

  gp_Pnt O = C.Location();
  gp_Vec Axe (C.Axis().Direction());
  gp_Vec Trsl = Axe.Multiplied(-(gp_Vec(O,P).Dot(Axe)));
  gp_Pnt Pp = P.Translated(Trsl);

// 2- Calculs des solutions ...

  Standard_Integer NoSol, NbSol;
  Standard_Real A = C.MajorRadius();
  Standard_Real B = C.MinorRadius();
  gp_Vec OPp (O,Pp);
  Standard_Real OPpMagn = OPp.Magnitude();
  if (OPpMagn < Tol) { if (Abs(A-B) < Tol) { return; } }
  Standard_Real X = OPp.Dot(gp_Vec(C.XAxis().Direction()));
  Standard_Real Y = OPp.Dot(gp_Vec(C.YAxis().Direction()));
  //  Standard_Real Y = Sqrt(OPpMagn*OPpMagn-X*X);

  Standard_Real ko2 = (B*B-A*A)/2., ko3 = -B*Y, ko4 = A*X;
  if(Abs(ko3) < 1.e-16*Max(Abs(ko2), Abs(ko3))) ko3 = 0.0;

//  math_TrigonometricFunctionRoots Sol(0.,(B*B-A*A)/2.,-B*Y,A*X,0.,Uinf,Usup);
  math_TrigonometricFunctionRoots Sol(0.,ko2, ko3, ko4, 0.,Uinf,Usup);

  if (!Sol.IsDone()) { return; }
  gp_Pnt Cu;
  Standard_Real Us;
  NbSol = Sol.NbSolutions();
  for (NoSol = 1; NoSol <= NbSol; NoSol++) {
    Us = Sol.Value(NoSol);
    Cu = ElCLib::Value(Us,C);
    mySqDist[myNbExt] = Cu.SquareDistance(P);
    myIsMin[myNbExt] = (NoSol == 1);
    myPoint[myNbExt] = Extrema_POnCurv(Us,Cu);
    myNbExt++;
  }
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElC::Extrema_ExtPElC (const gp_Pnt&       P, 
				  const gp_Hypr&      C,
				  const Standard_Real Tol,
				  const Standard_Real Uinf,
				  const Standard_Real Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}


void Extrema_ExtPElC::Perform(const gp_Pnt&       P, 
			      const gp_Hypr&      C,
			      const Standard_Real Tol,
			      const Standard_Real Uinf,
			      const Standard_Real Usup)
/*-----------------------------------------------------------------------------
Fonction:
  Recherche des valeurs de parametre u telle que:
   - dist(P,C(u)) passe par un extremum,
   - Uinf <= u <= Usup.

Methode:
  On procede en 2 temps:
  1- Projection du point P dans le plan de l'hyperbole,
  2- Calculs des solutions:
      Soit Pp, le point projete; on recherche les valeurs u telles que:
       (C(u)-Pp).C'(u) = 0. (1)
     Soit R et r les rayons de l'hyperbole,
          Chu = Cosh(u) et Shu = Sinh(u),
          C(u) = (R*Chu,r*Shu) et Pp = (X,Y);
     Alors, (1) <=> (R*Chu-X,r*Shu-Y).(R*Shu,r*Chu) = 0.
                    (R**2+r**2)*Chu*Shu - X*R*Shu - Y*r*Chu = 0. (2)
     Soit v = e**u;
     Alors, en utilisant Chu = (e**u+e**(-u))/2. et Sh = (e**u-e**(-u)))/2.
            (2) <=> ((R**2+r**2)/4.) * (v**2-v**(-2)) -
	            ((X*R+Y*r)/2.) * v +
	            ((X*R-Y*r)/2.) * v**(-1) = 0.
      (2)* v**2	<=> ((R**2+r**2)/4.) * v**4 -
                     ((X*R+Y*r)/2.)  * v**3 +
		     ((X*R-Y*r)/2.)  * v    -
		    ((R**2+r**2)/4.)        = 0.
     On utilise l'algorithme math_DirectPolynomialRoots pour resoudre
    cette equation en v.
-----------------------------------------------------------------------------*/
{
  myDone = Standard_False;
  myNbExt = 0;

// 1- Projection du point P dans le plan de l hyperbole -> Pp ...

  gp_Pnt O = C.Location();
  gp_Vec Axe (C.Axis().Direction());
  gp_Vec Trsl = Axe.Multiplied(-(gp_Vec(O,P).Dot(Axe)));
  gp_Pnt Pp = P.Translated(Trsl);

// 2- Calculs des solutions ...

  Standard_Real Tol2 = Tol * Tol;
  Standard_Real R = C.MajorRadius();
  Standard_Real r = C.MinorRadius();
  gp_Vec OPp (O,Pp);
#ifdef DEB
  Standard_Real OPpMagn = OPp.Magnitude();
#else
  OPp.Magnitude();
#endif
  Standard_Real X = OPp.Dot(gp_Vec(C.XAxis().Direction()));
  Standard_Real Y = OPp.Dot(gp_Vec(C.YAxis().Direction()));

  Standard_Real C1 = (R*R+r*r)/4.;
  math_DirectPolynomialRoots Sol(C1,-(X*R+Y*r)/2.,0.,(X*R-Y*r)/2.,-C1);
  if (!Sol.IsDone()) { return; }
  gp_Pnt Cu;
  Standard_Real Us, Vs;
  Standard_Integer NbSol = Sol.NbSolutions();
  Standard_Boolean DejaEnr;
  Standard_Integer NoExt;
  gp_Pnt TbExt[4];
  for (Standard_Integer NoSol = 1; NoSol <= NbSol; NoSol++) {
    Vs = Sol.Value(NoSol);
    if (Vs > 0.) {
      Us = Log(Vs);
      if ((Us >= Uinf) && (Us <= Usup)) {
	Cu = ElCLib::Value(Us,C);
	DejaEnr = Standard_False;
	for (NoExt = 0; NoExt < myNbExt; NoExt++) {
	  if (TbExt[NoExt].SquareDistance(Cu) < Tol2) {
	    DejaEnr = Standard_True;
	    break;
	  }
	}
	if (!DejaEnr) {
	  TbExt[myNbExt] = Cu;
	  mySqDist[myNbExt] = Cu.SquareDistance(P);
	  myIsMin[myNbExt] = mySqDist[myNbExt] < P.SquareDistance(ElCLib::Value(Us+1,C));
	  myPoint[myNbExt] = Extrema_POnCurv(Us,Cu);
	  myNbExt++;
	}
      } // if ((Us >= Uinf) && (Us <= Usup))
    } // if (Vs > 0.)
  } // for (Standard_Integer NoSol = 1; ...
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElC::Extrema_ExtPElC (const gp_Pnt&       P,
				  const gp_Parab&     C,
				  const Standard_Real Tol,
				  const Standard_Real Uinf,
				  const Standard_Real Usup)
{
  Perform(P, C, Tol, Uinf, Usup);
}


void Extrema_ExtPElC::Perform(const gp_Pnt&       P, 
			      const gp_Parab&     C,
//			      const Standard_Real Tol,
			      const Standard_Real ,
			      const Standard_Real Uinf,
			      const Standard_Real Usup)
/*-----------------------------------------------------------------------------
Fonction:
  Recherche des valeurs de parametre u telle que:
   - dist(P,C(u)) passe par un extremum,
   - Uinf <= u <= Usup.

Methode:
  On procede en 2 temps:
  1- Projection du point P dans le plan de la parabole,
  2- Calculs des solutions:
      Soit Pp, le point projete; on recherche les valeurs u telles que:
       (C(u)-Pp).C'(u) = 0. (1)
     Soit F la focale de la parabole,
          C(u) = ((u*u)/(4.*F),u) et Pp = (X,Y);
     Alors, (1) <=> ((u*u)/(4.*F)-X,u-Y).(u/(2.*F),1) = 0.
                    (1./(4.*F)) * U**3 + (2.*F-X) * U - 2*F*Y = 0.
     On utilise l'algorithme math_DirectPolynomialRoots pour resoudre
    cette equation en U.
-----------------------------------------------------------------------------*/
{
  myDone = Standard_False;
  myNbExt = 0;

// 1- Projection du point P dans le plan de la parabole -> Pp ...

  gp_Pnt O = C.Location();
  gp_Vec Axe (C.Axis().Direction());
  gp_Vec Trsl = Axe.Multiplied(-(gp_Vec(O,P).Dot(Axe)));
  gp_Pnt Pp = P.Translated(Trsl);

// 2- Calculs des solutions ...

  Standard_Real F = C.Focal();
  gp_Vec OPp (O,Pp);
#ifdef DEB
  Standard_Real OPpMagn = OPp.Magnitude();
#else
  OPp.Magnitude();
#endif
  Standard_Real X = OPp.Dot(gp_Vec(C.XAxis().Direction()));
//  Standard_Real Y = Sqrt(OPpMagn*OPpMagn-X*X);
  Standard_Real Y = OPp.Dot(gp_Vec(C.YAxis().Direction()));
  math_DirectPolynomialRoots Sol(1./(4.*F),0.,2.*F-X,-2.*F*Y);
  if (!Sol.IsDone()) { return; }
  gp_Pnt Cu;
  Standard_Real Us;
  Standard_Integer NbSol = Sol.NbSolutions();
  Standard_Boolean DejaEnr;
  Standard_Integer NoExt;
  gp_Pnt TbExt[3];
  for (Standard_Integer NoSol = 1; NoSol <= NbSol; NoSol++) {
    Us = Sol.Value(NoSol);
    if ((Us >= Uinf) && (Us <= Usup)) {
      Cu = ElCLib::Value(Us,C);
      DejaEnr = Standard_False;
      for (NoExt = 0; NoExt < myNbExt; NoExt++) {
	if (TbExt[NoExt].SquareDistance(Cu) < Precision::Confusion() * Precision::Confusion()) {
	  DejaEnr = Standard_True;
	  break;
	}
      }
      if (!DejaEnr) {
	TbExt[myNbExt] = Cu;
	mySqDist[myNbExt] = Cu.SquareDistance(P);
	myIsMin[myNbExt] = mySqDist[myNbExt] < P.SquareDistance(ElCLib::Value(Us+1,C));
	myPoint[myNbExt] = Extrema_POnCurv(Us,Cu);
	myNbExt++;
      }
    } // if ((Us >= Uinf) && (Us <= Usup))
  } // for (Standard_Integer NoSol = 1; ...
  myDone = Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_ExtPElC::IsDone () const { return myDone; }
//=============================================================================

Standard_Integer Extrema_ExtPElC::NbExt () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myNbExt;
}
//=============================================================================

Standard_Real Extrema_ExtPElC::SquareDistance (const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt())) { Standard_OutOfRange::Raise(); }
  return mySqDist[N-1];
}
//=============================================================================

Standard_Boolean Extrema_ExtPElC::IsMin (const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt())) { Standard_OutOfRange::Raise(); }
  return myIsMin[N-1];
}
//=============================================================================

Extrema_POnCurv Extrema_ExtPElC::Point (const Standard_Integer N) const
{
  if ((N < 1) || (N > NbExt())) { Standard_OutOfRange::Raise(); }
  return myPoint[N-1];
}
//=============================================================================
