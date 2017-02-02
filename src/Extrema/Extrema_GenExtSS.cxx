// Created on: 1995-07-18
// Created by: Modelistation
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


#include <Adaptor3d_Surface.hxx>
#include <Extrema_GenExtSS.hxx>
#include <Extrema_POnSurf.hxx>
#include <math_BFGS.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_Vector.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <StdFail_NotDone.hxx>

//! This class represents distance objective function for surface / surface.
class Extrema_FuncDistSS  : public math_MultipleVarFunctionWithGradient
{
public:
    DEFINE_STANDARD_ALLOC

    Standard_EXPORT Extrema_FuncDistSS(const Adaptor3d_Surface& S1,
                                       const Adaptor3d_Surface& S2)
    : myS1(&S1),
      myS2(&S2)
    {
    }

  Standard_EXPORT Standard_Integer NbVariables() const
  {
    return 4;
  }

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X,Standard_Real& F)
  {
    F = myS1->Value(X(1), X(2)).SquareDistance(myS2->Value(X(3), X(4)));
    return true;
  }

  Standard_EXPORT Standard_Boolean Gradient(const math_Vector& X,math_Vector& G)
  {
    gp_Pnt P1, P2;
    gp_Vec Du1s1, Dv1s1;
    gp_Vec Du2s2, Dv2s2;
    myS1->D1(X(1),X(2),P1,Du1s1,Dv1s1);
    myS2->D1(X(3),X(4),P2,Du2s2,Dv2s2);

    gp_Vec P1P2 (P2,P1);

    G(1) = P1P2.Dot(Du1s1);
    G(2) = P1P2.Dot(Dv1s1);
    G(3) = -P1P2.Dot(Du2s2);
    G(4) = -P1P2.Dot(Dv2s2);

    return true;
  }

  Standard_EXPORT virtual  Standard_Boolean Values(const math_Vector& X,Standard_Real& F,math_Vector& G)
  {
    F = myS1->Value(X(1), X(2)).SquareDistance(myS2->Value(X(3), X(4)));

    gp_Pnt P1, P2;
    gp_Vec Du1s1, Dv1s1;
    gp_Vec Du2s2, Dv2s2;
    myS1->D1(X(1),X(2),P1,Du1s1,Dv1s1);
    myS2->D1(X(3),X(4),P2,Du2s2,Dv2s2);

    gp_Vec P1P2 (P2,P1);

    G(1) = P1P2.Dot(Du1s1);
    G(2) = P1P2.Dot(Dv1s1);
    G(3) = -P1P2.Dot(Du2s2);
    G(4) = -P1P2.Dot(Dv2s2);

    return true;
  }

protected:

private:

  const Adaptor3d_Surface *myS1;
  const Adaptor3d_Surface *myS2;
};

//=======================================================================
//function : Extrema_GenExtSS
//purpose  : 
//=======================================================================
Extrema_GenExtSS::Extrema_GenExtSS()
{
  myDone = Standard_False;
  myInit = Standard_False;
}

//=======================================================================
//function : Extrema_GenExtSS
//purpose  : 
//=======================================================================

Extrema_GenExtSS::Extrema_GenExtSS(const Adaptor3d_Surface& S1, 
				   const Adaptor3d_Surface& S2, 
				   const Standard_Integer NbU, 
				   const Standard_Integer NbV, 
				   const Standard_Real    Tol1, 
				   const Standard_Real    Tol2) : myF(S1,S2)
{
  Initialize(S2, NbU, NbV, Tol2);
  Perform(S1, Tol1);
}

//=======================================================================
//function : Extrema_GenExtSS
//purpose  : 
//=======================================================================

Extrema_GenExtSS::Extrema_GenExtSS(const Adaptor3d_Surface& S1, 
				   const Adaptor3d_Surface& S2, 
				   const Standard_Integer NbU, 
				   const Standard_Integer NbV, 
				   const Standard_Real U1min, 
				   const Standard_Real U1sup, 
				   const Standard_Real V1min, 
				   const Standard_Real V1sup, 
				   const Standard_Real U2min, 
				   const Standard_Real U2sup, 
				   const Standard_Real V2min, 
				   const Standard_Real V2sup, 
				   const Standard_Real Tol1, 
				   const Standard_Real Tol2): myF(S1, S2)
{
  Initialize(S2, NbU, NbV, U2min,U2sup,V2min,V2sup,Tol2);
  Perform(S1, U1min,U1sup,V1min,V1sup,Tol1);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtSS::Initialize(const Adaptor3d_Surface& S2, 
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV, 
				  const Standard_Real Tol2)
{
  myu2min = S2.FirstUParameter();
  myu2sup = S2.LastUParameter();
  myv2min = S2.FirstVParameter();
  myv2sup = S2.LastVParameter();
  Initialize(S2,NbU,NbV,myu2min,myu2sup,myv2min,myv2sup,Tol2);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtSS::Initialize(const Adaptor3d_Surface& S2, 
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV, 
				  const Standard_Real U2min, 
				  const Standard_Real U2sup, 
				  const Standard_Real V2min, 
				  const Standard_Real V2sup, 
				  const Standard_Real Tol2)
{
  myS2 = (Adaptor3d_SurfacePtr)&S2;
  mypoints1 = new TColgp_HArray2OfPnt(0,NbU+1,0,NbV+1);
  mypoints2 = new TColgp_HArray2OfPnt(0,NbU+1,0,NbV+1);
  myusample = NbU;
  myvsample = NbV;
  myu2min = U2min;
  myu2sup = U2sup;
  myv2min = V2min;
  myv2sup = V2sup;
  mytol2 = Tol2;

// Parametrage de l echantillon sur S2

  Standard_Real PasU = myu2sup - myu2min;
  Standard_Real PasV = myv2sup - myv2min;
  Standard_Real U0 = PasU / myusample / 100.;
  Standard_Real V0 = PasV / myvsample / 100.;
  gp_Pnt P1;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = myu2min + U0/2.;
  V0 = myv2min + V0/2.;

// Calcul des distances

  Standard_Integer NoU, NoV;
  Standard_Real U, V;
  for ( NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU) {
    for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV) {
      P1 = myS2->Value(U, V);
      mypoints2->SetValue(NoU,NoV,P1);
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtSS::Perform(const Adaptor3d_Surface& S1,
			       const Standard_Real    Tol1)
{
  myu1min = S1.FirstUParameter();
  myu1sup = S1.LastUParameter();
  myv1min = S1.FirstVParameter();
  myv1sup = S1.LastVParameter();
  Perform(S1, myu1min, myu1sup,myv1min,myv1sup,Tol1);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtSS::Perform(const Adaptor3d_Surface& S1,
			       const Standard_Real U1min, 
			       const Standard_Real U1sup, 
			       const Standard_Real V1min, 
			       const Standard_Real V1sup, 
			       const Standard_Real Tol1)
{
  myF.Initialize(S1,*myS2);
  myu1min = U1min;
  myu1sup = U1sup;
  myv1min = V1min;
  myv1sup = V1sup;
  mytol1 = Tol1;

  Standard_Real U1, V1, U2, V2;
  Standard_Integer NoU1, NoV1, NoU2, NoV2;
  gp_Pnt P1, P2;

// Parametrage de l echantillon sur S1

  Standard_Real PasU1 = myu1sup - myu1min;
  Standard_Real PasV1 = myv1sup - myv1min;
  Standard_Real U10 = PasU1 / myusample / 100.;
  Standard_Real V10 = PasV1 / myvsample / 100.;
  PasU1 = (PasU1 - U10) / (myusample - 1);
  PasV1 = (PasV1 - V10) / (myvsample - 1);
  U10 = myu1min + U10/2.;
  V10 = myv1min + V10/2.;

  Standard_Real PasU2 = myu2sup - myu2min;
  Standard_Real PasV2 = myv2sup - myv2min;
  Standard_Real U20 = PasU2 / myusample / 100.;
  Standard_Real V20 = PasV2 / myvsample / 100.;
  PasU2 = (PasU2 - U20) / (myusample - 1);
  PasV2 = (PasV2 - V20) / (myvsample - 1);
  U20 = myu2min + U20/2.;
  V20 = myv2min + V20/2.;

// Calcul des distances

  for ( NoU1 = 1, U1 = U10; NoU1 <= myusample; NoU1++, U1 += PasU1) {
    for ( NoV1 = 1, V1 = V10; NoV1 <= myvsample; NoV1++, V1 += PasV1) {
      P1 = S1.Value(U1, V1);
      mypoints1->SetValue(NoU1,NoV1,P1);
    }
  }
  
/*
b- Calcul des minima:
   -----------------
   b.a) Initialisations:
*/

  math_Vector Tol(1, 4);
  Tol(1) = mytol1;
  Tol(2) = mytol1;
  Tol(3) = mytol2;
  Tol(4) = mytol2;
  math_Vector UV(1,4), UVinf(1,4), UVsup(1,4);
  UVinf(1) = myu1min;
  UVinf(2) = myv1min;
  UVinf(3) = myu2min;
  UVinf(4) = myv2min;
  UVsup(1) = myu1sup;
  UVsup(2) = myv1sup;
  UVsup(3) = myu2sup;
  UVsup(4) = myv2sup;
  

  Standard_Real distmin = RealLast(), distmax = 0.0, TheDist;

  Standard_Integer N1Umin=0,N1Vmin=0,N2Umin=0,N2Vmin=0;
  gp_Pnt PP1min, PP2min;
  Standard_Integer N1Umax=0,N1Vmax=0,N2Umax=0,N2Vmax=0;
  gp_Pnt PP1max, PP2max;

  for ( NoU1 = 1, U1 = U10; NoU1 <= myusample; NoU1++, U1 += PasU1) {
    for ( NoV1 = 1, V1 = V10; NoV1 <= myvsample; NoV1++, V1 += PasV1) {
      P1 = mypoints1->Value(NoU1, NoV1);
      for ( NoU2 = 1, U2 = U20; NoU2 <= myusample; NoU2++, U2 += PasU2) {
	for ( NoV2 = 1, V2 = V20; NoV2 <= myvsample; NoV2++, V2 += PasV2) {
	  P2 = mypoints2->Value(NoU2, NoV2);
	  TheDist = P1.SquareDistance(P2);
	  if (TheDist < distmin) {
	    distmin = TheDist;
	    N1Umin = NoU1;
	    N1Vmin = NoV1;
	    N2Umin = NoU2;
	    N2Vmin = NoV2;
	    PP1min = P1;
	    PP2min = P2;
	  }
	  if (TheDist > distmax) {
	    distmax = TheDist;
	    N1Umax = NoU1;
	    N1Vmax = NoV1;
	    N2Umax = NoU2;
	    N2Vmax = NoV2;
	    PP1max = P1;
	    PP2max = P2;
	  }
	}
      }
    }
  }
  
  UV(1) = U10 + (N1Umin - 1) * PasU1;
  UV(2) = V10 + (N1Vmin - 1) * PasV1;
  UV(3) = U20 + (N2Umin - 1) * PasU2;
  UV(4) = V20 + (N2Vmin - 1) * PasV2;

  Extrema_FuncDistSS aGFSS(S1, *myS2);
  math_BFGS aBFGSSolver(4);
  aBFGSSolver.Perform(aGFSS, UV);
  if (aBFGSSolver.IsDone())
  {
    aBFGSSolver.Location(UV);

    //  Store result in myF.
    myF.Value(UV , UV);
    myF.GetStateNumber();
  }
  else
  {
    // If optimum is not computed successfully then compute by old approach.

    // Restore initial point.
    UV(1) = U10 + (N1Umin - 1) * PasU1;
    UV(2) = V10 + (N1Vmin - 1) * PasV1;
    UV(3) = U20 + (N2Umin - 1) * PasU2;
    UV(4) = V20 + (N2Vmin - 1) * PasV2;

    math_FunctionSetRoot SR1(myF, Tol);
    SR1.Perform(myF, UV, UVinf, UVsup);
  }

  //math_FunctionSetRoot SR1(myF, Tol);
  //SR1.Perform(myF, UV, UVinf, UVsup);

  UV(1) = U10 + (N1Umax - 1) * PasU1;
  UV(2) = V10 + (N1Vmax - 1) * PasV1;
  UV(3) = U20 + (N2Umax - 1) * PasU2;
  UV(4) = V20 + (N2Vmax - 1) * PasV2;

  // It is impossible to compute max distance in the same manner,
  // since for the distance functional for max have bad definition.
  // So, for max computation old approach is used.
  math_FunctionSetRoot SR2(myF, Tol);
  SR2.Perform(myF, UV, UVinf, UVsup);

  myDone = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_GenExtSS::IsDone() const 
{
  return myDone;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_GenExtSS::NbExt() const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myF.NbExt();

}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_GenExtSS::SquareDistance(const Standard_Integer N) const 
{  
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myF.SquareDistance(N);
}

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenExtSS::PointOnS1(const Standard_Integer N) const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myF.PointOnS1(N);
}

//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenExtSS::PointOnS2(const Standard_Integer N) const 
{
  if (!IsDone()) { throw StdFail_NotDone(); }
  return myF.PointOnS2(N);
}

//=======================================================================
//function : Bidon
//purpose  : 
//=======================================================================

Adaptor3d_SurfacePtr Extrema_GenExtSS::Bidon() const 
{
  return (Adaptor3d_SurfacePtr)0L;
}

