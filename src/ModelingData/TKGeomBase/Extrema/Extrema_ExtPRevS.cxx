// Created on: 1999-09-21
// Created by: Edward AGAPOV
// Copyright (c) 1999-1999 Matra Datavision
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

#include <Adaptor3d_Curve.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_ExtPRevS.hxx>
#include <Extrema_GenExtPS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <StdFail_NotDone.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Extrema_ExtPRevS, Standard_Transient)

static gp_Ax2 GetPosition(
  const GeomAdaptor_SurfaceOfRevolution& S) // const occ::handle<Adaptor_HCurve>& C)
{
  occ::handle<Adaptor3d_Curve> C = S.BasisCurve();

  switch (C->GetType())
  {

    case GeomAbs_Line: {
      gp_Lin L = C->Line();
      gp_Dir N = S.AxeOfRevolution().Direction();
      if (N.IsParallel(L.Direction(), Precision::Angular()))
      {
        gp_Vec OO(L.Location(), S.AxeOfRevolution().Location());
        if (OO.Magnitude() <= gp::Resolution())
        {
          OO = gp_Vec(L.Location(), ElCLib::Value(100, L));
          if (N.IsParallel(OO, Precision::Angular()))
            return gp_Ax2(); // Line and axe of revolution coincide
        }
        N ^= OO;
      }
      else
      {
        N ^= L.Direction();
      }
      return gp_Ax2(L.Location(), N, L.Direction());
    }
    case GeomAbs_Circle:
      return C->Circle().Position();
    case GeomAbs_Ellipse:
      return C->Ellipse().Position();
    case GeomAbs_Hyperbola:
      return C->Hyperbola().Position();
    case GeomAbs_Parabola:
      return C->Parabola().Position();
    default:
      return gp_Ax2();
  }
}

//=================================================================================================

static bool HasSingularity(const GeomAdaptor_SurfaceOfRevolution& S)
{

  const occ::handle<Adaptor3d_Curve> C = S.BasisCurve();
  gp_Dir                             N = S.AxeOfRevolution().Direction();
  gp_Pnt                             P = S.AxeOfRevolution().Location();

  gp_Lin L(P, N);

  P = C->Value(C->FirstParameter());

  if (L.SquareDistance(P) < Precision::SquareConfusion())
    return true;

  P = C->Value(C->LastParameter());

  if (L.SquareDistance(P) < Precision::SquareConfusion())
    return true;

  return false;
}

//=============================================================================

static void PerformExtPElC(Extrema_ExtPElC&                    E,
                           const gp_Pnt&                       P,
                           const occ::handle<Adaptor3d_Curve>& C,
                           const double                        Tol)
{
  switch (C->GetType())
  {
    case GeomAbs_Hyperbola:
      E.Perform(P, C->Hyperbola(), Tol, -Precision::Infinite(), Precision::Infinite());
      return;
    case GeomAbs_Line:
      E.Perform(P, C->Line(), Tol, -Precision::Infinite(), Precision::Infinite());
      return;
    case GeomAbs_Circle:
      E.Perform(P, C->Circle(), Tol, 0.0, 2.0 * M_PI);
      return;
    case GeomAbs_Ellipse:
      E.Perform(P, C->Ellipse(), Tol, 0.0, 2.0 * M_PI);
      return;
    case GeomAbs_Parabola:
      E.Perform(P, C->Parabola(), Tol, -Precision::Infinite(), Precision::Infinite());
      return;
    default:
      return;
  }
}

//=================================================================================================

static bool IsCaseAnalyticallyComputable(const GeomAbs_CurveType& theType,
                                         const gp_Ax2&            theCurvePos,
                                         const gp_Ax1&            AxeOfRevolution)
{
  // check type
  switch (theType)
  {
    case GeomAbs_Line:
    case GeomAbs_Circle:
    case GeomAbs_Ellipse:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
      break;
    default:
      return false;
  }
  //  the axe of revolution must be in the plane of the curve.
  gp_Pln pl(theCurvePos.Location(), theCurvePos.Direction());
  gp_Pnt p1   = AxeOfRevolution.Location();
  double dist = 100., dist2 = dist * dist;
  double aThreshold = Precision::Angular() * Precision::Angular() * dist2;
  gp_Pnt p2         = AxeOfRevolution.Location().XYZ() + dist * AxeOfRevolution.Direction().XYZ();

  if ((pl.SquareDistance(p1) < aThreshold) && (pl.SquareDistance(p2) < aThreshold))
    return true;
  return false;
  //   gp_Vec V (AxeOfRevolution.Location(),theCurvePos.Location());
  //   if (std::abs( V * theCurvePos.Direction()) <= gp::Resolution())
  //     return true;
  //   else
  //     return false;
}

//=================================================================================================

static bool IsOriginalPnt(const gp_Pnt& P, const Extrema_POnSurf* Points, const int NbPoints)
{
  for (int i = 1; i <= NbPoints; i++)
  {
    if (Points[i - 1].Value().IsEqual(P, Precision::Confusion()))
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

static bool IsExtremum(const double             U,
                       const double             V,
                       const gp_Pnt&            P,
                       const Adaptor3d_Surface* S,
                       gp_Pnt&                  E,
                       double&                  Dist2,
                       const bool               IsVSup,
                       const bool               IsMin)
{
  E     = S->Value(U, V);
  Dist2 = P.SquareDistance(E);
  if (IsMin)
    return (Dist2 < P.SquareDistance(S->Value(U + 1, V))
            && Dist2 < P.SquareDistance(S->Value(U - 1, V))
            && Dist2 < P.SquareDistance(S->Value(U, IsVSup ? V - 1 : V + 1)));
  else
    return (Dist2 > P.SquareDistance(S->Value(U + 1, V))
            && Dist2 > P.SquareDistance(S->Value(U - 1, V))
            && Dist2 > P.SquareDistance(S->Value(U, IsVSup ? V - 1 : V + 1)));
}

//=================================================================================================

Extrema_ExtPRevS::Extrema_ExtPRevS()
{
  myvinf = myvsup            = 0.0;
  mytolv                     = Precision::Confusion();
  myDone                     = false;
  myNbExt                    = 0;
  myIsAnalyticallyComputable = false;

  for (int i = 0; i < 8; i++)
  {
    mySqDist[i] = RealLast();
  }
}

//=================================================================================================

Extrema_ExtPRevS::Extrema_ExtPRevS(const gp_Pnt&                                       theP,
                                   const occ::handle<GeomAdaptor_SurfaceOfRevolution>& theS,
                                   const double                                        theUmin,
                                   const double                                        theUsup,
                                   const double                                        theVmin,
                                   const double                                        theVsup,
                                   const double                                        theTolU,
                                   const double                                        theTolV)
{
  Initialize(theS, theUmin, theUsup, theVmin, theVsup, theTolU, theTolV);

  Perform(theP);
}

//=================================================================================================

Extrema_ExtPRevS::Extrema_ExtPRevS(const gp_Pnt&                                       theP,
                                   const occ::handle<GeomAdaptor_SurfaceOfRevolution>& theS,
                                   const double                                        theTolU,
                                   const double                                        theTolV)
{
  Initialize(theS,
             theS->FirstUParameter(),
             theS->LastUParameter(),
             theS->FirstVParameter(),
             theS->LastVParameter(),
             theTolU,
             theTolV);

  Perform(theP);
}

//=================================================================================================

void Extrema_ExtPRevS::Initialize(const occ::handle<GeomAdaptor_SurfaceOfRevolution>& theS,
                                  const double                                        theUmin,
                                  const double                                        theUsup,
                                  const double                                        theVmin,
                                  const double                                        theVsup,
                                  const double                                        theTolU,
                                  const double                                        theTolV)
{
  myvinf = theVmin;
  myvsup = theVsup;
  mytolv = theTolV;

  myDone                     = false;
  myNbExt                    = 0;
  myIsAnalyticallyComputable = false;

  occ::handle<Adaptor3d_Curve> anACurve = theS->BasisCurve();

  if (myS != theS)
  {
    myS        = theS;
    myPosition = GetPosition(*theS);
    myIsAnalyticallyComputable =
      IsCaseAnalyticallyComputable(anACurve->GetType(), myPosition, theS->AxeOfRevolution());
  }

  if (!myIsAnalyticallyComputable)
  {
    int aNbu = 32, aNbv = 32;

    if (HasSingularity(*theS))
    {
      aNbv = 100;
    }

    myExtPS.Initialize(*theS, aNbu, aNbv, theUmin, theUsup, theVmin, theVsup, theTolU, theTolV);
  }
}

//=================================================================================================

void Extrema_ExtPRevS::Perform(const gp_Pnt& P)
{
  myDone  = false;
  myNbExt = 0;

  if (!myIsAnalyticallyComputable)
  {

    myExtPS.Perform(P);
    myDone  = myExtPS.IsDone();
    myNbExt = myExtPS.NbExt();
    return;
  }

  occ::handle<Adaptor3d_Curve> anACurve = myS->BasisCurve();

  gp_Ax1 Ax  = myS->AxeOfRevolution();
  gp_Vec Dir = Ax.Direction(), Z = myPosition.Direction();
  gp_Pnt O = Ax.Location();

  double OPdir = gp_Vec(O, P).Dot(Dir);
  gp_Pnt Pp    = P.Translated(Dir.Multiplied(-OPdir));
  if (O.IsEqual(Pp, Precision::Confusion())) // P is on the AxeOfRevolution
    return;

  double U, V;
  gp_Pnt P1, Ppp;
  double OPpz = gp_Vec(O, Pp).Dot(Z);
  if (std::abs(OPpz) <= gp::Resolution())
  {
    Ppp = Pp;
    U   = 0;
  }
  else
  {
    Ppp = Pp.Translated(Z.Multiplied(-OPpz));
    if (O.IsEqual(Ppp, Precision::Confusion()))
      U = M_PI / 2;
    else
    {
      U = gp_Vec(O, Ppp).AngleWithRef(gp_Vec(O, Pp), Dir);
    }
  }

  gp_Vec OPpp(O, Ppp), OPq(O, myS->Value(M_PI / 2, 0));
  if (U != M_PI / 2)
  {
    if (std::abs(OPq.Magnitude()) <= gp::Resolution())
      OPq = gp_Vec(O, myS->Value(M_PI / 2, anACurve->LastParameter() / 10));
    if (OPpp.AngleWithRef(OPq, Dir) < 0)
      U += M_PI;
  }

  gp_Trsf T;
  T.SetRotation(Ax, -U);
  P1 = P.Transformed(T);

  gp_Pnt E;
  double Dist2;
  int    i;

  Extrema_ExtPElC anExt;
  PerformExtPElC(anExt, P1, anACurve, mytolv);

  if (anExt.IsDone())
  {
    myDone = true;
    for (i = 1; i <= anExt.NbExt(); i++)
    {
      Extrema_POnCurv POC = anExt.Point(i);
      V                   = POC.Parameter();
      if (V > myvsup)
      {
        // 	 if ( !IsExtremum (U, V = myvsup, P, myS, E, Dist2,
        // 			   true, anExt.IsMin(i))) continue;
        double newV = myvsup;

        if ((anACurve->GetType() == GeomAbs_Circle) || (anACurve->GetType() == GeomAbs_Ellipse))
        {
          newV = ElCLib::InPeriod(V, myvinf, myvinf + 2. * M_PI);

          if (newV > myvsup)
          {
            newV -= 2. * M_PI;

            if (newV + mytolv < myvinf)
            {
              newV = myvsup;
            }
            else if (newV < myvinf)
            {
              newV = myvinf;
            }
          }
        }
        V = newV;

        if (!IsExtremum(U, V, P, myS.get(), E, Dist2, true, anExt.IsMin(i)))
        {
          continue;
        }
      }
      else if (V < myvinf)
      {
        // 	if ( !IsExtremum (U, V = myvinf, P, myS, E, Dist2,
        // 			  false, anExt.IsMin(i))) continue;

        double newV = myvinf;

        if ((anACurve->GetType() == GeomAbs_Circle) || (anACurve->GetType() == GeomAbs_Ellipse))
        {
          newV = ElCLib::InPeriod(V, myvsup - 2. * M_PI, myvsup);

          if (newV < myvinf)
          {
            newV += 2. * M_PI;

            if (newV - mytolv > myvsup)
            {
              newV = myvinf;
            }
            else if (newV > myvsup)
            {
              newV = myvsup;
            }
          }
        }
        V = newV;

        if (!IsExtremum(U, V, P, myS.get(), E, Dist2, false, anExt.IsMin(i)))
          continue;
      }
      else
      {
        E     = myS->Value(U, V);
        Dist2 = P.SquareDistance(E);
      }
      if (IsOriginalPnt(E, myPoint, myNbExt))
      {
        myPoint[myNbExt]  = Extrema_POnSurf(U, V, E);
        mySqDist[myNbExt] = Dist2;
        myNbExt++;
      }
    }
  }
  T.SetRotation(Ax, M_PI);
  P1.Transform(T);

  PerformExtPElC(anExt, P1, anACurve, mytolv);
  if (anExt.IsDone())
  {
    myDone = true;

    U += M_PI;

    for (i = 1; i <= anExt.NbExt(); i++)
    {
      Extrema_POnCurv POC = anExt.Point(i);
      V                   = POC.Parameter();
      if (V > myvsup)
      {
        // 	if ( !IsExtremum (U, V = myvsup, P, myS, E, Dist2,
        // 			   true, anExt.IsMin(i))) continue;

        double newV = myvsup;

        if ((anACurve->GetType() == GeomAbs_Circle) || (anACurve->GetType() == GeomAbs_Ellipse))
        {
          newV = ElCLib::InPeriod(V, myvinf, myvinf + 2. * M_PI);

          if (newV > myvsup)
          {
            newV -= 2. * M_PI;

            if (newV + mytolv < myvinf)
            {
              newV = myvsup;
            }
            else if (newV < myvinf)
            {
              newV = myvinf;
            }
          }
        }
        V = newV;

        if (!IsExtremum(U, V, P, myS.get(), E, Dist2, true, anExt.IsMin(i)))
          continue;
      }
      else if (V < myvinf)
      {
        // 	if ( !IsExtremum (U, V = myvinf, P, myS, E, Dist2,
        // 			  false, anExt.IsMin(i))) continue;
        double newV = myvinf;

        if ((anACurve->GetType() == GeomAbs_Circle) || (anACurve->GetType() == GeomAbs_Ellipse))
        {
          newV = ElCLib::InPeriod(V, myvsup - 2. * M_PI, myvsup);

          if (newV < myvinf)
          {
            newV += 2. * M_PI;

            if (newV - mytolv > myvsup)
            {
              newV = myvinf;
            }
            else if (newV > myvsup)
            {
              newV = myvsup;
            }
          }
        }
        V = newV;

        if (!IsExtremum(U, V, P, myS.get(), E, Dist2, false, anExt.IsMin(i)))
          continue;
      }
      else
      {
        E     = myS->Value(U, V);
        Dist2 = P.SquareDistance(E);
      }
      if (IsOriginalPnt(E, myPoint, myNbExt))
      {
        myPoint[myNbExt]  = Extrema_POnSurf(U, V, E);
        mySqDist[myNbExt] = Dist2;
        myNbExt++;
      }
    }
  }
}

//=================================================================================================

bool Extrema_ExtPRevS::IsDone() const
{
  return myDone;
}

//=================================================================================================

int Extrema_ExtPRevS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myNbExt;
}

//=================================================================================================

double Extrema_ExtPRevS::SquareDistance(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  if (myIsAnalyticallyComputable)
    return mySqDist[N - 1];
  else
    return myExtPS.SquareDistance(N);
}

//=================================================================================================

const Extrema_POnSurf& Extrema_ExtPRevS::Point(const int N) const
{
  if ((N < 1) || (N > NbExt()))
  {
    throw Standard_OutOfRange();
  }
  if (myIsAnalyticallyComputable)
    return myPoint[N - 1];
  else
    return myExtPS.Point(N);
}
