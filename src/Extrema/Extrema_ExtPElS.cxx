// modified by MPS (june 96)  gestion du cas ou le plan de reference du cone 
//                            passe par le sommet ( O et M sont alors confondus)
//
// 
#include <Extrema_ExtPElS.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_NotImplemented.hxx>
#include <ElSLib.hxx>
//=============================================================================

Extrema_ExtPElS::Extrema_ExtPElS () { myDone = Standard_False; }
//=============================================================================

Extrema_ExtPElS::Extrema_ExtPElS (const gp_Pnt& P, 
				  const gp_Cylinder& S,
				  const Standard_Real Tol)
{

 Perform(P, S, Tol);
}
/*-----------------------------------------------------------------------------
Fonction:
   Recherche des 2 distances extremales entre le point P et le cylindre S.

Methode:
   Soit Pp la projection de P dans le plan XOY du cylindre;
  2 cas sont consideres:
  1- distance(Pp,O) < Tol:
     Il y a une infinite de solutions; IsDone() = Standard_False.
  2- distance(Pp,O) > Tol:
     Soit V = OP.OZ,
          U1 = angle(OX,OPp) avec 0 < U1 < 2.*PI
	  U2 = U1 + PI avec 0 < U2 < 2.*PI;
     alors (U1,V) correspond a la distance minimale
     et    (U2,V) correspond a la distance maximale.
-----------------------------------------------------------------------------*/

void Extrema_ExtPElS::Perform(const gp_Pnt&       P, 
			      const gp_Cylinder&  S,
			      const Standard_Real Tol)
{
  myDone = Standard_False;
  myNbExt = 0;

// Projection du point P dans le plan XOY du cylindre ...
  gp_Ax3 Pos = S.Position();
  gp_Pnt O = Pos.Location();
  gp_Vec OZ (Pos.Direction());
  Standard_Real V = gp_Vec(O,P).Dot(OZ);
  gp_Pnt Pp = P.Translated(OZ.Multiplied(-V));

// Calcul des extrema
  gp_Vec OPp (O,Pp);
  if (OPp.Magnitude() < Tol) { return; }
  gp_Vec myZ = Pos.XDirection()^Pos.YDirection();
  Standard_Real U1 = gp_Vec(Pos.XDirection()).AngleWithRef(OPp,myZ); //-PI<U1<PI
  Standard_Real U2 = U1 + PI;
  if (U1 < 0.) { U1 += 2. * PI; }

  gp_Pnt Ps;
  Ps = ElSLib::Value(U1,V,S);
  mySqDist[0] = Ps.SquareDistance(P);
  myPoint[0] = Extrema_POnSurf(U1,V,Ps);
  Ps = ElSLib::Value(U2,V,S);
  mySqDist[1] = Ps.SquareDistance(P);
  myPoint[1] = Extrema_POnSurf(U2,V,Ps);

  myNbExt = 2;
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElS::Extrema_ExtPElS (const gp_Pnt&       P, 
				  const gp_Cone&      S,
				  const Standard_Real Tol)
{
  Perform(P, S, Tol);
}
/*-----------------------------------------------------------------------------
Fonction:
   Recherche des 2 distances extremales entre le point P et le cone S.

Methode:
   Soit M le sommet du cone.
  2 cas sont consideres:
  1- distance(P,M) < Tol:
     Il y a un minimum en M.
  2- distance(P,M) > Tol:
     Soit Pp la projection de P dans le plan XOY du cone;
     2 cas sont consideres:
     1- distance(Pp,O) < Tol:
        Il y a une infinite de solutions; IsDone() = Standard_False.
     2- distance(Pp,O) > Tol:
        Il existe 2 extrema:
        Soit Vm = valeur de v pour le point M,
             Vp = valeur de v pour le point P,
             U1 = angle(OX,OPp) si Vp > Vm )
	         -angle(OX,OPp) sinon      ) avec 0. < U1 < 2*PI,
             U2 = U1 + PI avec 0. < U2 < 2*PI;
        On se place dans le plan PpOZ.
        Soit A l'angle du cone,
             B = angle(MP,MO) avec 0. < B < PI,
	     L = longueur(MP),
	     V1 = (L * cos(B-A)) + Vm,
	     V2 = (L * cos(B+A)) + Vm;
        alors (U1,V1) et (U2,V2) correspondent aux distances minimales.
-----------------------------------------------------------------------------*/

void Extrema_ExtPElS::Perform(const gp_Pnt&       P, 
			      const gp_Cone&      S,
			      const Standard_Real Tol)
{
  myDone = Standard_False;
  myNbExt = 0;

  gp_Pnt M = S.Apex();
  gp_Ax3 Pos = S.Position();
  gp_Pnt O = Pos.Location();
  Standard_Real A = S.SemiAngle();
  gp_Vec OZ (Pos.Direction());
  gp_Vec myZ = Pos.XDirection()^Pos.YDirection();
  gp_Vec MP (M,P);

  Standard_Real L2 = MP.SquareMagnitude();
  Standard_Real Vm = -(S.RefRadius() / Sin(A));
#ifdef DEB
  Standard_Real Zm = gp_Vec(O, M).Dot(OZ);
#else
  gp_Vec(O, M).Dot(OZ);
#endif

// Cas ou P est confondu avec S ...
  if (L2 < Tol * Tol) {
    mySqDist[0] = L2;
    myPoint[0] = Extrema_POnSurf(0.,Vm,M);
    myNbExt = 1;
    myDone = Standard_True;
    return;
  }
    gp_Vec DirZ;
    if (M.SquareDistance(O)<Tol * Tol) 
    { DirZ=OZ;
     if( A<0) DirZ.Multiplied(-1.);
    }
    else 
     DirZ=gp_Vec(M,O); 
// Projection de P dans le plan de reference du cone ...
  Standard_Real Zp = gp_Vec(O, P).Dot(OZ);

  gp_Pnt Pp = P.Translated(OZ.Multiplied(-Zp));
  gp_Vec OPp(O, Pp);
  if (OPp.SquareMagnitude() < Tol * Tol) return;
  Standard_Real B, U1, V1, U2, V2;
  Standard_Boolean Same = DirZ.Dot(MP) >= 0.0;
  U1 = gp_Vec(Pos.XDirection()).AngleWithRef(OPp,myZ); //-PI<U1<PI
  B = MP.Angle(DirZ);
  if (!Same) { U1 += PI; }
  U2 = U1 + PI;
  if (U1 < 0.) { U1 += 2. * PI; }
  if (U2 > 2.*PI) { U2 -= 2. * PI; }
  B = MP.Angle(DirZ);
  A = Abs(A);
  Standard_Real L = sqrt(L2);
  if (!Same) {
    B = PI-B;
    V1 = -L*cos(B-A);
    V2 = -L*cos(B+A);
  }
  else {
    V1 = L * cos(B-A);
    V2 = L * cos(B+A);
  }
  Standard_Real Sense = OZ.Dot(gp_Dir(DirZ));
  V1 *= Sense;   V2 *= Sense;
  V1 += Vm;   V2 += Vm;

  gp_Pnt Ps;
  Ps = ElSLib::Value(U1,V1,S);
  mySqDist[0] = Ps.SquareDistance(P);
  myPoint[0] = Extrema_POnSurf(U1,V1,Ps);
  Ps = ElSLib::Value(U2,V2,S);
  mySqDist[1] = Ps.SquareDistance(P);
  myPoint[1] = Extrema_POnSurf(U2,V2,Ps);

  myNbExt = 2;
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElS::Extrema_ExtPElS (const gp_Pnt&       P, 
				  const gp_Sphere&    S,
				  const Standard_Real Tol)
{
  Perform(P, S, Tol);
}
/*-----------------------------------------------------------------------------
Fonction:
   Recherche des 2 distances extremales entre le point P et la sphere S.

Methode:
   Soit O l'origine de la sphere.
  2 cas sont consideres:
  1- distance(P,O) < Tol:
     Il y a une infinite de solutions; IsDone() = Standard_False
  2- distance(P,O) > Tol:
     Soit Pp la projection du point P dans le plan XOY de la sphere;
     2 cas sont consideres:
     1- distance(Pp,O) < Tol:
        Les 2 solutions sont: (0,-PI/2.) et (0.,PI/2.)
     2- distance(Pp,O) > Tol:
        Soit U1 = angle(OX,OPp) avec 0. < U1 < 2.*PI,
	     U2 = U1 + PI avec 0. < U2 < 2*PI,
	     V1 = angle(OPp,OP) avec -PI/2. < V1 < PI/2. ,
	alors (U1, V1) correspond a la distance minimale
	et    (U2,-V1) correspond a la distance maximale.
-----------------------------------------------------------------------------*/

void Extrema_ExtPElS::Perform(const gp_Pnt&       P, 
			      const gp_Sphere&    S,
			      const Standard_Real Tol)
{
  myDone = Standard_False;
  myNbExt = 0;

  gp_Ax3 Pos = S.Position();
  gp_Vec OP (Pos.Location(),P);

// Cas ou P est confondu avec O ...
  if (OP.SquareMagnitude() < Tol * Tol) { return; }

// Projection de P dans le plan XOY de la sphere ...
  gp_Pnt O = Pos.Location();
  gp_Vec OZ (Pos.Direction());
  Standard_Real Zp = OP.Dot(OZ);
  gp_Pnt Pp = P.Translated(OZ.Multiplied(-Zp));

// Calcul des extrema ...
  gp_Vec OPp (O,Pp);
  Standard_Real U1, U2, V;
  if (OPp.SquareMagnitude() < Tol * Tol) {
    U1 = 0.;
    U2 = 0.;
    if (Zp < 0.) { V = -PI / 2.; }
    else { V = PI / 2.; }
  }
  else {
    gp_Vec myZ = Pos.XDirection()^Pos.YDirection();
    U1 = gp_Vec(Pos.XDirection()).AngleWithRef(OPp,myZ);
    U2 = U1 + PI;
    if (U1 < 0.) { U1 += 2. * PI; }
    V = OP.Angle(OPp);
    if (Zp < 0.) { V = -V; }
  }

  gp_Pnt Ps;
  Ps = ElSLib::Value(U1,V,S);
  mySqDist[0] = Ps.SquareDistance(P);
  myPoint[0] = Extrema_POnSurf(U1,V,Ps);
  Ps = ElSLib::Value(U2,-V,S);
  mySqDist[1] = Ps.SquareDistance(P);
  myPoint[1] = Extrema_POnSurf(U2,-V,Ps);

  myNbExt = 2;
  myDone = Standard_True;
}
//=============================================================================

Extrema_ExtPElS::Extrema_ExtPElS (const gp_Pnt&       P, 
				  const gp_Torus&     S,
				  const Standard_Real Tol)
{
  Perform(P, S, Tol);
}
/*-----------------------------------------------------------------------------
Fonction:
   Recherche des 2 distances extremales entre le point P et le tore S.

Methode:
   Soit Pp la projection du point P dans le plan XOY du tore;
  2 cas sont consideres:
  1- distance(Pp,O) < Tol:
     Il y a une infinite de solutions; IsDone() = Standard_False.
  2- distance(Pp,O) > Tol:
     On se place dans le plan PpOZ;
     Soit V1 = angle(OX,OPp) avec 0. < V1 < 2.*PI,
	  V2 = V1 + PI avec 0. < V2 < 2.*PI,
	  O1 et O2 les centres des cercles (O1 sur coord. posit.)
          U1 = angle(OPp,O1P),
	  U2 = angle(OPp,PO2);
     alors (U1,V1) correspond a la distance minimale
     et    (U2,V2) correspond a la distance maximale.
-----------------------------------------------------------------------------*/
void Extrema_ExtPElS::Perform(const gp_Pnt&       P, 
			      const gp_Torus&     S,
			      const Standard_Real Tol)
{
  myDone = Standard_False;
  myNbExt = 0;

// Projection de P dans le plan XOY ...
  gp_Ax3 Pos = S.Position();
  gp_Pnt O = Pos.Location();
  gp_Vec OZ (Pos.Direction());
  gp_Pnt Pp = P.Translated(OZ.Multiplied(-(gp_Vec(O,P).Dot(Pos.Direction()))));
					 
// Calcul des extrema ...
  gp_Vec OPp (O,Pp);
  Standard_Real R2 = OPp.SquareMagnitude();
  if (R2 < Tol * Tol) { return; }
 
  gp_Vec myZ = Pos.XDirection()^Pos.YDirection();
  Standard_Real U1 = gp_Vec(Pos.XDirection()).AngleWithRef(OPp,myZ);
  Standard_Real U2 = U1 + PI;
  if (U1 < 0.) { U1 += 2. * PI; }
  Standard_Real R = sqrt(R2);
  gp_Vec OO1 = OPp.Divided(R).Multiplied(S.MajorRadius());
  gp_Vec OO2 = OO1.Multiplied(-1.);
  gp_Pnt O1 = O.Translated(OO1);
  gp_Pnt O2 = O.Translated(OO2);

  if(O1.SquareDistance(P) < Tol) { return; }
  if(O2.SquareDistance(P) < Tol) { return; }

  Standard_Real V1 = OO1.AngleWithRef(gp_Vec(O1,P),OO1.Crossed(OZ));
  Standard_Real V2 = OO2.AngleWithRef(gp_Vec(P,O2),OO2.Crossed(OZ));
  if (V1 < 0.) { V1 += 2. * PI; }
  if (V2 < 0.) { V2 += 2. * PI; }

  gp_Pnt Ps;
  Ps = ElSLib::Value(U1,V1,S);
  mySqDist[0] = Ps.SquareDistance(P);
  myPoint[0] = Extrema_POnSurf(U1,V1,Ps);

  Ps = ElSLib::Value(U1,V1+PI,S);
  mySqDist[1] = Ps.SquareDistance(P);
  myPoint[1] = Extrema_POnSurf(U1,V1+PI,Ps);

  Ps = ElSLib::Value(U2,V2,S);
  mySqDist[2] = Ps.SquareDistance(P);
  myPoint[2] = Extrema_POnSurf(U2,V2,Ps);

  Ps = ElSLib::Value(U2,V2+PI,S);
  mySqDist[3] = Ps.SquareDistance(P);
  myPoint[3] = Extrema_POnSurf(U2,V2+PI,Ps);

  myNbExt = 4;
  myDone = Standard_True;
}


Extrema_ExtPElS::Extrema_ExtPElS (const gp_Pnt&       P, 
				  const gp_Pln&       S,
				  const Standard_Real Tol)
{
  Perform(P, S, Tol);
}

void Extrema_ExtPElS::Perform (const gp_Pnt&       P, 
			       const gp_Pln&       S,
//			       const Standard_Real Tol)
			       const Standard_Real )
{
  myDone = Standard_False;
  myNbExt = 0;

// Projection du point P dans le plan XOY du cylindre ...
  gp_Pnt O = S.Location();
  gp_Vec OZ (S.Axis().Direction());
  Standard_Real U, V = gp_Vec(O,P).Dot(OZ);
  gp_Pnt Pp = P.Translated(OZ.Multiplied(-V));

  ElSLib::Parameters(S, P, U, V);
  mySqDist[0] = Pp.SquareDistance(P);
  myPoint[0] = Extrema_POnSurf(U,V,Pp);
  myNbExt = 1;
  myDone = Standard_True;
}


//=============================================================================

Standard_Boolean Extrema_ExtPElS::IsDone () const { return myDone; }
//=============================================================================

Standard_Integer Extrema_ExtPElS::NbExt () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myNbExt;
}
//=============================================================================

Standard_Real Extrema_ExtPElS::SquareDistance (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  if ((N < 1) || (N > myNbExt)) { Standard_OutOfRange::Raise(); }
  return mySqDist[N-1];
}
//=============================================================================

Extrema_POnSurf Extrema_ExtPElS::Point (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  if ((N < 1) || (N > myNbExt)) { Standard_OutOfRange::Raise(); }
  return myPoint[N-1];
}
//=============================================================================
