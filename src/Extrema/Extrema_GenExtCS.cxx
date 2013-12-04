// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
  myusample = NbU;
  myvsample = NbV;
  myumin = Umin;
  myusup = Usup;
  myvmin = Vmin;
  myvsup = Vsup;
  mytol2 = Tol2;
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
  //
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
  {
    Standard_Real aCUAdd = (mytsup - mytmin) / mytsample;
    Standard_Real aSUAdd = (myusup - myumin) / myusample;
    Standard_Real aSVAdd = (myvsup - myvmin) / myvsample;
    TColgp_HArray1OfPnt aCPs(1, mytsample);
    TColgp_HArray2OfPnt aSPs(1, myusample, 1, myvsample);
    Standard_Integer aRestIterCount = 3;
      // The value is calculated by the bug CR23830.
    Standard_Integer aCUDen = 2, aSUDen = 2, aSVDen = 2;
    Standard_Boolean anAreAvSqsInited = Standard_False;
    Standard_Real aCUSq = 0, aSUSq = 0, aSVSq = 0;
    while (aRestIterCount--)
    {
      Standard_Real aMinCU = 0., aMinSU = 0., aMinSV = 0., aMaxCU = 0., aMaxSU = 0., aMaxSV = 0.;
      Standard_Real aMinSqDist = DBL_MAX, aMaxSqDist = -DBL_MAX;
      for (Standard_Integer aSUNom = 1; aSUNom < aSUDen; aSUNom += 2)
      {
        Standard_Real aSU0 = myumin + (aSUNom * aSUAdd) / aSUDen;
        for (Standard_Integer aSVNom = 1; aSVNom < aSVDen; aSVNom += 2)
        {
          Standard_Real aSV0 = myvmin + (aSVNom * aSVAdd) / aSVDen;
          for (Standard_Integer aCUNom = 1; aCUNom < aCUDen; aCUNom += 2)
          {
            Standard_Real aCU0 = mytmin + (aCUNom * aCUAdd) / aCUDen;
            Standard_Real aCU = aCU0;
            for (Standard_Integer aCUI = 1; aCUI <= mytsample;
              aCUI++, aCU += aCUAdd)
            {
              aCPs.SetValue(aCUI, C.Value(aCU));
            }
            //
            aCU = aCU0;
            Standard_Real aSU = aSU0;
            for (Standard_Integer aSUI = 1; aSUI <= myusample;
              aSUI++, aSU += aSUAdd)
            {
              Standard_Real aSV = aSV0;
              for (Standard_Integer aSVI = 1; aSVI <= myvsample;
                aSVI++, aSV += aSVAdd)
              {
                gp_Pnt aSP = myS->Value(aSU, aSV);
                aSPs.ChangeValue(aSUI, aSVI) = aSP;
                Standard_Real aCU = aCU0;
                for (Standard_Integer aCUI = 1; aCUI <= mytsample;
                  aCUI++, aCU += aCUAdd)
                {
                  Standard_Real aSqDist = aSP.SquareDistance(aCPs.Value(aCUI));
                  if (aSqDist < aMinSqDist)
                  {
                    aMinSqDist = aSqDist;
                    aMinCU = aCU;
                    aMinSU = aSU;
                    aMinSV = aSV;
                  }
                  if (aMaxSqDist < aSqDist)
                  {
                    aMaxSqDist = aSqDist;
                    aMaxCU = aCU;
                    aMaxSU = aSU;
                    aMaxSV = aSV;
                  }
                }
              }
            }
          }
        }
      }
      // Find min approximation.
      UV(1) = aMinCU;
      UV(2) = aMinSU;
      UV(3) = aMinSV;
      math_FunctionSetRoot anA(myF, UV, Tol, UVinf, UVsup, 100, aRestIterCount);
      // Find max approximation.
      if (!anA.IsDivergent() || !aRestIterCount)
      {
        UV(1) = aMaxCU;
        UV(2) = aMaxSU;
        UV(3) = aMaxSV;
        math_FunctionSetRoot(myF, UV, Tol, UVinf, UVsup);
        break;
      }
      //
      if (!anAreAvSqsInited)
      {
        anAreAvSqsInited = Standard_True;
        //
        const gp_Pnt * aCP1 = &aCPs.Value(1);
        for (Standard_Integer aCUI = 2; aCUI <= mytsample; aCUI++)
        {
          const gp_Pnt & aCP2 = aCPs.Value(aCUI);
          aCUSq += aCP1->SquareDistance(aCP2);
          aCP1 = &aCP2;
        }
        aCUSq /= mytsample - 1;
        //
        for (Standard_Integer aSVI = 1; aSVI <= myvsample; aSVI++)
        {
          const gp_Pnt * aSP1 = &aSPs.Value(1, aSVI);
          for (Standard_Integer aSUI = 2; aSUI <= myusample; aSUI++)
          {
            const gp_Pnt & aSP2 = aSPs.Value(aSUI, aSVI);
            aSUSq += aSP1->SquareDistance(aSP2);
            aSP1 = &aSP2;
          }
        }
        aSUSq /= myvsample * (myusample - 1);
        //
        for (Standard_Integer aSUI = 1; aSUI <= myusample; aSUI++)
        {
          const gp_Pnt * aSP1 = &aSPs.Value(aSUI, 1);
          for (Standard_Integer aSVI = 2; aSVI <= myvsample; aSVI++)
          {
            const gp_Pnt & aSP2 = aSPs.Value(aSUI, aSVI);
            aSVSq += aSP1->SquareDistance(aSP2);
            aSP1 = &aSP2;
          }
        }
        aSVSq /= myusample * (myvsample - 1);
      }
      //
      if ((aSUSq <= aCUSq) && (aSVSq <= aCUSq))
      {
        aCUDen += aCUDen;
        aCUSq /= 4;
      }
      else if ((aCUSq <= aSUSq) && (aSVSq <= aSUSq))
      {
        aSUDen += aSUDen;
        aSUSq /= 4;
      }
      else
      {
        aSVDen += aSVDen;
        aSVSq /= 4;
      }
    }
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

