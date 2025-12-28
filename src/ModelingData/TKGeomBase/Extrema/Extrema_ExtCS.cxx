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

//  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_AddSurface.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtCS.hxx>
#include <Extrema_GenExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <Extrema_ExtPS.hxx>

Extrema_ExtCS::Extrema_ExtCS()
    : myS(nullptr),
      myDone(false),
      myIsPar(false),
      myuinf(0.0),
      myusup(0.0),
      myvinf(0.0),
      myvsup(0.0),
      mytolC(0.0),
      mytolS(0.0),
      myucinf(0.0),
      myucsup(0.0),
      myStype(GeomAbs_OtherSurface)
{
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
                             const Adaptor3d_Surface& S,
                             const double             TolC,
                             const double             TolS)

{
  Initialize(S, TolC, TolS);
  Perform(C, C.FirstParameter(), C.LastParameter());
}

Extrema_ExtCS::Extrema_ExtCS(const Adaptor3d_Curve&   C,
                             const Adaptor3d_Surface& S,
                             const double             UCinf,
                             const double             UCsup,
                             const double             Uinf,
                             const double             Usup,
                             const double             Vinf,
                             const double             Vsup,
                             const double             TolC,
                             const double             TolS)

{
  Initialize(S, Uinf, Usup, Vinf, Vsup, TolC, TolS);
  Perform(C, UCinf, UCsup);
}

void Extrema_ExtCS::Initialize(const Adaptor3d_Surface& S, const double TolC, const double TolS)
{
  Initialize(S,
             S.FirstUParameter(),
             S.LastUParameter(),
             S.FirstVParameter(),
             S.LastVParameter(),
             TolC,
             TolS);
}

void Extrema_ExtCS::Initialize(const Adaptor3d_Surface& S,
                               const double             Uinf,
                               const double             Usup,
                               const double             Vinf,
                               const double             Vsup,
                               const double             TolC,
                               const double             TolS)
{
  myS     = &S;
  myIsPar = false;
  myuinf  = Uinf;
  myusup  = Usup;
  myvinf  = Vinf;
  myvsup  = Vsup;
  mytolC  = TolC;
  mytolS  = TolS;
  myStype = myS->GetType();
}

void Extrema_ExtCS::Perform(const Adaptor3d_Curve& C, const double Uinf, const double Usup)
{
  myucinf = Uinf;
  myucsup = Usup;
  myPOnS.Clear();
  myPOnC.Clear();
  mySqDist.Clear();
  int i, j;
  int NbT, NbU, NbV;
  NbT = 12;
  NbU = NbV                 = 10;
  GeomAbs_CurveType myCtype = C.GetType();

  myDone = false;
  // Try analytic computation of extrema
  bool isComputeAnalytic = true;

  switch (myCtype)
  {

    case GeomAbs_Line: {

      switch (myStype)
      {
        case GeomAbs_Sphere:
          myExtElCS.Perform(C.Line(), myS->Sphere());
          break;
        case GeomAbs_Cylinder:
          myExtElCS.Perform(C.Line(), myS->Cylinder());
          break;
        case GeomAbs_Plane:
          myExtElCS.Perform(C.Line(), myS->Plane());
          if (myExtElCS.IsParallel())
            break;
          [[fallthrough]];

        case GeomAbs_Torus:
        case GeomAbs_Cone:
        case GeomAbs_BezierSurface:
        case GeomAbs_BSplineSurface:
        case GeomAbs_SurfaceOfRevolution:
        case GeomAbs_SurfaceOfExtrusion:
        case GeomAbs_OffsetSurface:
        case GeomAbs_OtherSurface: {
          double cfirst = myucinf, clast = myucsup;
          double ufirst = myS->FirstUParameter(), ulast = myS->LastUParameter(),
                 vfirst = myS->FirstVParameter(), vlast = myS->LastVParameter();

          if (!(Precision::IsInfinite(ufirst) || Precision::IsInfinite(ulast)
                || Precision::IsInfinite(vfirst) || Precision::IsInfinite(vlast)))
          {
            double  tmin = Precision::Infinite(), tmax = -tmin;
            double  xmin, ymin, zmin, xmax, ymax, zmax;
            Bnd_Box aSurfBox;
            BndLib_AddSurface::Add(*myS,
                                   ufirst,
                                   ulast,
                                   vfirst,
                                   vlast,
                                   Precision::Confusion(),
                                   aSurfBox);
            aSurfBox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            gp_Lin aLin = C.Line();
            double aParOnLin;
            gp_Pnt aLimPntArray[8];

            aLimPntArray[0].SetCoord(xmin, ymin, zmin);
            aLimPntArray[1].SetCoord(xmax, ymin, zmin);
            aLimPntArray[2].SetCoord(xmin, ymax, zmin);
            aLimPntArray[3].SetCoord(xmax, ymax, zmin);
            aLimPntArray[4].SetCoord(xmin, ymin, zmax);
            aLimPntArray[5].SetCoord(xmax, ymin, zmax);
            aLimPntArray[6].SetCoord(xmin, ymax, zmax);
            aLimPntArray[7].SetCoord(xmax, ymax, zmax);

            for (i = 0; i <= 7; i++)
            {
              aParOnLin = ElCLib::Parameter(aLin, aLimPntArray[i]);
              tmin      = std::min(aParOnLin, tmin);
              tmax      = std::max(aParOnLin, tmax);
            }
            cfirst = std::max(cfirst, tmin);
            clast  = std::min(clast, tmax);
          }

          if (myS->IsUPeriodic())
            NbU = 13;
          if (myS->IsVPeriodic())
            NbV = 13;

          if (clast - cfirst <= Precision::Confusion())
          {
            double aCPar = (cfirst + clast) / 2.;
            gp_Pnt aPm   = C.Value(aCPar);
            Extrema_ExtPS
              anExtPS(aPm, *myS, ufirst, ulast, vfirst, vlast, mytolS, mytolS, Extrema_ExtFlag_MIN);
            myDone = anExtPS.IsDone();
            if (myDone)
            {
              int             NbExt = anExtPS.NbExt();
              double          T     = aCPar, U, V;
              Extrema_POnCurv PC;
              Extrema_POnSurf PS;
              for (i = 1; i <= NbExt; i++)
              {
                PS = anExtPS.Point(i);
                PS.Parameter(U, V);
                AddSolution(C, T, U, V, PC.Value(), PS.Value(), anExtPS.SquareDistance(i));
              }
            }
            return;
          }

          Extrema_GenExtCS Ext(C,
                               *myS,
                               NbT,
                               NbU,
                               NbV,
                               cfirst,
                               clast,
                               ufirst,
                               ulast,
                               vfirst,
                               vlast,
                               mytolC,
                               mytolS);

          myDone = Ext.IsDone();
          if (myDone)
          {
            int             NbExt = Ext.NbExt();
            double          T, U, V;
            Extrema_POnCurv PC;
            Extrema_POnSurf PS;
            for (i = 1; i <= NbExt; i++)
            {
              PC = Ext.PointOnCurve(i);
              PS = Ext.PointOnSurface(i);
              T  = PC.Parameter();
              PS.Parameter(U, V);
              AddSolution(C, T, U, V, PC.Value(), PS.Value(), Ext.SquareDistance(i));
            }
          }
          return;
        }
      }
      break;
    }
      //  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 Begin
    case GeomAbs_Circle: {
      if (myStype == GeomAbs_Cylinder)
      {
        myExtElCS.Perform(C.Circle(), myS->Cylinder());
        break;
      }
      else if (myStype == GeomAbs_Plane)
      {
        myExtElCS.Perform(C.Circle(), myS->Plane());
        break;
      }
      else if (myStype == GeomAbs_Sphere)
      {
        myExtElCS.Perform(C.Circle(), myS->Sphere());
        break;
      }
    }
      [[fallthrough]];
    case GeomAbs_Hyperbola: {
      if (myCtype == GeomAbs_Hyperbola && myStype == GeomAbs_Plane)
      {
        //  Modified by skv - Thu Jul  7 12:29:34 2005 OCC9134 End
        myExtElCS.Perform(C.Hyperbola(), myS->Plane());
        break;
      }
    }
      [[fallthrough]];
    default: {
      isComputeAnalytic = false;
      break;
    }
  }

  if (isComputeAnalytic)
  {
    if (myExtElCS.IsDone())
    {
      myDone  = true;
      myIsPar = myExtElCS.IsParallel();
      if (myIsPar)
      {
        mySqDist.Append(myExtElCS.SquareDistance(1));
      }
      else
      {
        int NbExt = myExtElCS.NbExt();
        for (i = 1; i <= NbExt; i++)
        {
          Extrema_POnCurv PC;
          Extrema_POnSurf PS;
          myExtElCS.Points(i, PC, PS);
          double Ucurve = PC.Parameter();
          double U, V;
          PS.Parameter(U, V);
          AddSolution(C, Ucurve, U, V, PC.Value(), PS.Value(), myExtElCS.SquareDistance(i));
        }

        if (mySqDist.Length() == 0 && NbExt > 0)
        {
          // Analytical extrema seem to be out of curve/surface boundaries.
          // Try extremity points of curve.
          gp_Pnt aPOnC[2], aPOnS[2];
          double aT[2]    = {myucinf, myucsup}, U[2], V[2];
          double aDist[2] = {-1, -1};
          for (i = 0; i < 2; ++i)
          {
            if (Precision::IsInfinite(aT[i]))
              continue;

            aPOnC[i] = C.Value(aT[i]);
            switch (myStype)
            {
              case GeomAbs_Plane: {
                ElSLib::Parameters(myS->Plane(), aPOnC[i], U[i], V[i]);
                aPOnS[i] = ElSLib::Value(U[i], V[i], myS->Plane());
                break;
              }
              case GeomAbs_Sphere: {
                ElSLib::Parameters(myS->Sphere(), aPOnC[i], U[i], V[i]);
                aPOnS[i] = ElSLib::Value(U[i], V[i], myS->Sphere());
                break;
              }
              case GeomAbs_Cylinder: {
                ElSLib::Parameters(myS->Cylinder(), aPOnC[i], U[i], V[i]);
                aPOnS[i] = ElSLib::Value(U[i], V[i], myS->Cylinder());
                break;
              }
              case GeomAbs_Torus: {
                ElSLib::Parameters(myS->Torus(), aPOnC[i], U[i], V[i]);
                aPOnS[i] = ElSLib::Value(U[i], V[i], myS->Torus());
                break;
              }
              case GeomAbs_Cone: {
                ElSLib::Parameters(myS->Cone(), aPOnC[i], U[i], V[i]);
                aPOnS[i] = ElSLib::Value(U[i], V[i], myS->Cone());
                break;
              }
              default:
                continue;
            }

            aDist[i] = aPOnC[i].SquareDistance(aPOnS[i]);
          }

          bool bAdd[2] = {false, false};

          // Choose solution to add
          if (aDist[0] >= 0. && aDist[1] >= 0.)
          {
            double aDiff = aDist[0] - aDist[1];
            // Both computed -> take only minimal
            if (std::abs(aDiff) < Precision::Confusion())
              // Add both
              bAdd[0] = bAdd[1] = true;
            else if (aDiff < 0)
              // Add first
              bAdd[0] = true;
            else
              // Add second
              bAdd[1] = true;
          }
          else if (aDist[0] >= 0.)
            // Add first
            bAdd[0] = true;
          else if (aDist[1] >= 0.)
            // Add second
            bAdd[1] = true;

          for (i = 0; i < 2; ++i)
          {
            if (bAdd[i])
              AddSolution(C, aT[i], U[i], V[i], aPOnC[i], aPOnS[i], aDist[i]);
          }
        }
      }
      return;
    }
  }

  // Elementary extrema is not done, try generic solution
  Extrema_GenExtCS Ext;
  Ext.Initialize(*myS, NbU, NbV, mytolS);
  if (myCtype == GeomAbs_Hyperbola)
  {
    double tmin = std::max(-20., C.FirstParameter());
    double tmax = std::min(20., C.LastParameter());
    Ext.Perform(C, NbT, tmin, tmax, mytolC); // to avoid overflow
  }
  else
  {
    if ((myCtype == GeomAbs_Circle && NbT < 13) || (myCtype == GeomAbs_BSplineCurve && NbT < 13))
    {
      NbT = 13;
    }
    Ext.Perform(C, NbT, mytolC);
  }

  myDone = Ext.IsDone();
  if (myDone)
  {
    int             NbExt = Ext.NbExt();
    double          T, U, V;
    Extrema_POnCurv PC;
    Extrema_POnSurf PS;
    for (i = 1; i <= NbExt; i++)
    {
      PC = Ext.PointOnCurve(i);
      PS = Ext.PointOnSurface(i);
      T  = PC.Parameter();
      PS.Parameter(U, V);
      AddSolution(C, T, U, V, PC.Value(), PS.Value(), Ext.SquareDistance(i));
    }

    // Add sharp points
    int                        SolNumber   = mySqDist.Length();
    void*                      CopyC       = (void*)&C;
    Adaptor3d_Curve&           aC          = *(Adaptor3d_Curve*)CopyC;
    int                        NbIntervals = aC.NbIntervals(GeomAbs_C1);
    NCollection_Array1<double> SharpPoints(1, NbIntervals + 1);
    aC.Intervals(SharpPoints, GeomAbs_C1);

    Extrema_ExtPS aProjPS;
    aProjPS.Initialize(*myS,
                       myS->FirstUParameter(),
                       myS->LastUParameter(),
                       myS->FirstVParameter(),
                       myS->LastVParameter(),
                       mytolS,
                       mytolS);

    for (i = 2; i < SharpPoints.Upper(); ++i)
    {
      T           = SharpPoints(i);
      gp_Pnt aPnt = C.Value(T);
      aProjPS.Perform(aPnt);
      if (!aProjPS.IsDone())
        continue;
      int    NbProj = aProjPS.NbExt(), jmin = 0;
      double MinSqDist = RealLast();
      for (j = 1; j <= NbProj; j++)
      {
        double aSqDist = aProjPS.SquareDistance(j);
        if (aSqDist < MinSqDist)
        {
          MinSqDist = aSqDist;
          jmin      = j;
        }
      }
      if (jmin != 0)
      {
        aProjPS.Point(jmin).Parameter(U, V);
        AddSolution(C, T, U, V, aPnt, aProjPS.Point(jmin).Value(), MinSqDist);
      }
    }
    // Cut sharp solutions to keep only minimum and maximum
    int imin = SolNumber + 1, imax = mySqDist.Length();
    for (i = SolNumber + 1; i <= mySqDist.Length(); i++)
    {
      if (mySqDist(i) < mySqDist(imin))
        imin = i;
      if (mySqDist(i) > mySqDist(imax))
        imax = i;
    }
    if (mySqDist.Length() > SolNumber + 2)
    {
      double          MinSqDist = mySqDist(imin);
      double          MaxSqDist = mySqDist(imax);
      Extrema_POnCurv MinPC     = myPOnC(imin);
      Extrema_POnCurv MaxPC     = myPOnC(imax);
      Extrema_POnSurf MinPS     = myPOnS(imin);
      Extrema_POnSurf MaxPS     = myPOnS(imax);

      mySqDist.Remove(SolNumber + 1, mySqDist.Length());
      myPOnC.Remove(SolNumber + 1, myPOnC.Length());
      myPOnS.Remove(SolNumber + 1, myPOnS.Length());

      mySqDist.Append(MinSqDist);
      myPOnC.Append(MinPC);
      myPOnS.Append(MinPS);
      mySqDist.Append(MaxSqDist);
      myPOnC.Append(MaxPC);
      myPOnS.Append(MaxPS);
    }
  }
}

bool Extrema_ExtCS::IsDone() const
{
  return myDone;
}

bool Extrema_ExtCS::IsParallel() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }

  return myIsPar;
}

double Extrema_ExtCS::SquareDistance(const int N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return mySqDist.Value(N);
}

int Extrema_ExtCS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }

  return mySqDist.Length();
}

void Extrema_ExtCS::Points(const int N, Extrema_POnCurv& P1, Extrema_POnSurf& P2) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  P1 = myPOnC.Value(N);
  P2 = myPOnS.Value(N);
}

bool Extrema_ExtCS::AddSolution(const Adaptor3d_Curve& theCurve,
                                const double           aT,
                                const double           aU,
                                const double           aV,
                                const gp_Pnt&          PointOnCurve,
                                const gp_Pnt&          PointOnSurf,
                                const double           SquareDist)
{
  bool Added = false;

  double T = aT, U = aU, V = aV;

  if (theCurve.IsPeriodic())
    T = ElCLib::InPeriod(T, myucinf, myucinf + theCurve.Period());
  if (myS->IsUPeriodic())
    U = ElCLib::InPeriod(U, myuinf, myuinf + myS->UPeriod());
  if (myS->IsVPeriodic())
    V = ElCLib::InPeriod(V, myvinf, myvinf + myS->VPeriod());

  Extrema_POnCurv aPC;
  Extrema_POnSurf aPS;
  if ((myucinf - T) <= mytolC && (T - myucsup) <= mytolC && (myuinf - U) <= mytolS
      && (U - myusup) <= mytolS && (myvinf - V) <= mytolS && (V - myvsup) <= mytolS)
  {
    bool IsNewSolution = true;
    for (int j = 1; j <= mySqDist.Length(); j++)
    {
      aPC       = myPOnC(j);
      aPS       = myPOnS(j);
      double Tj = aPC.Parameter();
      double Uj, Vj;
      aPS.Parameter(Uj, Vj);
      if (std::abs(T - Tj) <= mytolC && std::abs(U - Uj) <= mytolS && std::abs(V - Vj) <= mytolS)
      {
        IsNewSolution = false;
        break;
      }
    }
    if (IsNewSolution)
    {
      mySqDist.Append(SquareDist);
      aPC.SetValues(T, PointOnCurve);
      myPOnC.Append(aPC);
      myPOnS.Append(Extrema_POnSurf(U, V, PointOnSurf));
      Added = true;
    }
  }
  return Added;
}
