// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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



#include <Extrema_GenExtCS.ixx>

#include <math_Vector.hxx>
#include <math_FunctionSetRoot.hxx>
#include <Precision.hxx>

#include <Geom_Line.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_ExtPS.hxx>

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

Extrema_GenExtCS::Extrema_GenExtCS()
{
  myDone = Standard_False;
  myInit = Standard_False;
}

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

 Extrema_GenExtCS::Extrema_GenExtCS(const Adaptor3d_Curve& C, 
				    const Adaptor3d_Surface& S, 
				    const Standard_Integer NbT, 
				    const Standard_Integer NbU, 
				    const Standard_Integer NbV, 
				    const Standard_Real Tol1, 
				    const Standard_Real Tol2)
{
  Initialize(S, NbU, NbV, Tol2);
  Perform(C, NbT, Tol1);
}

//=======================================================================
//function : Extrema_GenExtCS
//purpose  : 
//=======================================================================

 Extrema_GenExtCS::Extrema_GenExtCS(const Adaptor3d_Curve& C, 
				    const Adaptor3d_Surface& S, 
				    const Standard_Integer NbT, 
				    const Standard_Integer NbU, 
				    const Standard_Integer NbV, 
				    const Standard_Real tmin, 
				    const Standard_Real tsup, 
				    const Standard_Real Umin, 
				    const Standard_Real Usup,
				    const Standard_Real Vmin, 
				    const Standard_Real Vsup, 
				    const Standard_Real Tol1, 
				    const Standard_Real Tol2)
{
  Initialize(S, NbU, NbV, Umin,Usup,Vmin,Vsup,Tol2);
  Perform(C, NbT, tmin, tsup, Tol1);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Initialize(const Adaptor3d_Surface& S, 
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV, 
				  const Standard_Real Tol2)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S,NbU,NbV,myumin,myusup,myvmin,myvsup,Tol2);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Initialize(const Adaptor3d_Surface& S, 
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV, 
				  const Standard_Real Umin, 
				  const Standard_Real Usup, 
				  const Standard_Real Vmin, 
				  const Standard_Real Vsup, 
				  const Standard_Real Tol2)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  mypoints2 = new TColgp_HArray2OfPnt(0,NbU+1,0,NbV+1);
  myusample = NbU;
  myvsample = NbV;
  myumin = Umin;
  myusup = Usup;
  myvmin = Vmin;
  myvsup = Vsup;
  mytol2 = Tol2;

// Parametrage de l echantillon sur S

  Standard_Real PasU = myusup - myumin;
  Standard_Real PasV = myvsup - myvmin;
  Standard_Real U0 = PasU / myusample / 100.;
  Standard_Real V0 = PasV / myvsample / 100.;
  gp_Pnt P1;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = myumin + U0/2.;
  V0 = myvmin + V0/2.;

// Calcul des distances

  Standard_Integer NoU, NoV;
  Standard_Real U, V;
  for ( NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU) {
    for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV) {
      P1 = myS->Value(U, V);
      mypoints2->SetValue(NoU,NoV,P1);
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Perform(const Adaptor3d_Curve& C, 
			       const Standard_Integer NbT,
			       const Standard_Real Tol1)
{
  mytmin = C.FirstParameter();
  mytsup = C.LastParameter();
  Perform(C, NbT, mytmin, mytsup,Tol1);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_GenExtCS::Perform(const Adaptor3d_Curve& C, 
			       const Standard_Integer NbT,
			       const Standard_Real tmin, 
			       const Standard_Real tsup, 
			       const Standard_Real Tol1)
{
  myDone = Standard_False;
  myF.Initialize(C,*myS);
  mytmin = tmin;
  mytsup = tsup;
  mytol1 = Tol1;
  mytsample = NbT;
  mypoints1 = new TColgp_HArray1OfPnt(0,NbT);

  Standard_Real t1, U, V;
  Standard_Integer NoT, NoU, NoV;
  gp_Pnt P1, P2;

// Modif de lvt pour trimer la surface non pas aux infinis mais  a +/- 10000

  Standard_Real trimusup = myusup, trimumin = myumin,trimvsup = myvsup,trimvmin = myvmin;
  if (Precision::IsInfinite(trimusup)){
    trimusup = 1.0e+10;
  }
  if (Precision::IsInfinite(trimvsup)){
    trimvsup = 1.0e+10;
  }
  if (Precision::IsInfinite(trimumin)){
    trimumin = -1.0e+10;
  }
  if (Precision::IsInfinite(trimvmin)){
    trimvmin = -1.0e+10;
  }
  

// Parametrage de l echantillon sur C

  Standard_Real PasT = mytsup - mytmin;
  Standard_Real t0 = PasT / mytsample / 100.;
  PasT = (PasT - t0) / (mytsample - 1);
  t0 = mytmin + t0/2.;

// Parametrage de l echantillon sur S

  Standard_Real PasU = trimusup - trimumin;
  Standard_Real PasV = trimvsup - trimvmin;
  Standard_Real U0 = PasU / myusample / 100.;
  Standard_Real V0 = PasV / myvsample / 100.;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = trimumin + U0/2.;
  V0 = trimvmin + V0/2.;

// Calcul des distances

  for ( NoT = 1, t1 = t0; NoT <= mytsample; NoT++, t1 += PasT) {
    P1 = C.Value(t1);
    mypoints1->SetValue(NoT,P1);
  }
  

/*
b- Calcul des minima:
   -----------------
   b.a) Initialisations:
*/

  math_Vector Tol(1, 3);
  Tol(1) = mytol1;
  Tol(2) = mytol2;
  Tol(3) = mytol2;
  math_Vector UV(1,3), UVinf(1,3), UVsup(1,3);
  UVinf(1) = mytmin;
  UVinf(2) = trimumin;
  UVinf(3) = trimvmin;

  UVsup(1) = mytsup;
  UVsup(2) = trimusup;
  UVsup(3) = trimvsup;

  // 18/02/02 akm vvv : (OCC163) bad extremas - extrusion surface versus the line.
  //                    Try to preset the initial solution as extrema between
  //                    extrusion direction and the curve.
  if (myS->GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    gp_Dir aDir = myS->Direction();
    Handle(Adaptor3d_HCurve) aCurve = myS->BasisCurve();
    Standard_Real dfUFirst = aCurve->FirstParameter();
    // Create iso line of U=U0
    GeomAdaptor_Curve anAx(new Geom_Line(aCurve->Value(dfUFirst), aDir),
                           trimvmin, trimvsup);
    Extrema_ExtCC aLocator(C, anAx);
    if (aLocator.IsDone() && aLocator.NbExt()>0)
    {
      Standard_Integer iExt;
      // Try to find all extremas
      Extrema_POnCurv aP1, aP2;
      for (iExt=1; iExt<=aLocator.NbExt(); iExt++)
      {
	aLocator.Points (iExt, aP1, aP2);
	// Parameter on curve
	UV(1) = aP1.Parameter();
	// To find parameters on surf, try ExtPS
	Extrema_ExtPS aPreciser (aP1.Value(), *myS, mytol2, mytol2);
	if (aPreciser.IsDone())
	{
	  // Managed to find extremas between point and surface
	  Standard_Integer iPExt;
	  for (iPExt=1; iPExt<=aPreciser.NbExt(); iPExt++)
	  {
	    aPreciser.Point(iPExt).Parameter(UV(2),UV(3));
	    math_FunctionSetRoot S1 (myF,UV,Tol,UVinf,UVsup);
	  }
	}
	else
	{
	  // Failed... try the point on iso line
	  UV(2) = dfUFirst;
	  UV(3) = aP2.Parameter();
	  math_FunctionSetRoot S1 (myF,UV,Tol,UVinf,UVsup);
	}
      } // for (iExt=1; iExt<=aLocator.NbExt(); iExt++)
    } // if (aLocator.IsDone() && aLocator.NbExt()>0)
  } // if (myS.Type() == GeomAbs_ExtrusionSurface)
  else 
  // 18/02/02 akm ^^^
  {
    Standard_Real distmin = RealLast(), distmax = 0.0, TheDist;

    Standard_Integer NTmin=0,NUmin=0,NVmin=0;
    gp_Pnt PP1min, PP2min;
    Standard_Integer NTmax=0,NUmax=0,NVmax=0;
    gp_Pnt PP1max, PP2max;
    for ( NoT = 1, t1 = t0; NoT <= mytsample; NoT++, t1 += PasT) {
      P1 = mypoints1->Value(NoT);
      for ( NoU = 1, U = U0; NoU <= myusample; NoU++, U += PasU) {
	for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV) {
	  P2 = mypoints2->Value(NoU, NoV);
	  TheDist = P1.SquareDistance(P2);
	  if (TheDist < distmin) {
	    distmin = TheDist;
	    NTmin = NoT;
	    NUmin = NoU;
	    NVmin = NoV;
	    PP1min = P1;
	    PP2min = P2;
	  }
	  if (TheDist > distmax) {
	    distmax = TheDist;
	    NTmax = NoT;
	    NUmax = NoU;
	    NVmax = NoV;
	    PP1max = P1;
	    PP2max = P2;
	  }
	}
      }
    }
  
    // minimum:
    UV(1) = t0 + (NTmin - 1) * PasT;
    UV(2) = U0 + (NUmin - 1) * PasU;
    UV(3) = V0 + (NVmin - 1) * PasV;

    math_FunctionSetRoot S1 (myF,UV,Tol,UVinf,UVsup);

    // maximum:
    UV(1) = t0 + (NTmax - 1) * PasT;
    UV(2) = U0 + (NUmax - 1) * PasU;
    UV(3) = V0 + (NVmax - 1) * PasV;

    math_FunctionSetRoot S2 (myF,UV,Tol,UVinf,UVsup);

  }

  myDone = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_GenExtCS::IsDone() const 
{
  return myDone;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_GenExtCS::NbExt() const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.NbExt();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_GenExtCS::SquareDistance(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.SquareDistance(N);
}

//=======================================================================
//function : PointOnCurve
//purpose  : 
//=======================================================================

const Extrema_POnCurv& Extrema_GenExtCS::PointOnCurve(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.PointOnCurve(N);
}

//=======================================================================
//function : PointOnSurface
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_GenExtCS::PointOnSurface(const Standard_Integer N) const 
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.PointOnSurface(N);
}

//=======================================================================
//function : BidonSurface
//purpose  : 
//=======================================================================

Adaptor3d_SurfacePtr Extrema_GenExtCS::BidonSurface() const 
{
  return (Adaptor3d_SurfacePtr)0L;
}

//=======================================================================
//function : BidonCurve
//purpose  : 
//=======================================================================

Adaptor3d_CurvePtr Extrema_GenExtCS::BidonCurve() const 
{
  return (Adaptor3d_CurvePtr)0L;
}

