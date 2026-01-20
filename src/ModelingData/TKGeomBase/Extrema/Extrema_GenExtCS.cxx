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

#include <Extrema_GenExtCS.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Extrema_GlobOptFuncCS.hxx>
#include <Extrema_GlobOptFuncConicS.hxx>
#include <Extrema_GlobOptFuncCQuadric.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_Hyperbola.hxx>
#include <GeomGridEval_Surface.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_PSO.hxx>
#include <math_PSOParticlesPool.hxx>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <ElCLib.hxx>
#include <Extrema_GenLocateExtPS.hxx>

const double MaxParamVal    = 1.0e+10;
const double aBorderDivisor = 1.0e+4;
const double HyperbolaLimit = 23.; // ln(MaxParamVal)

static bool IsQuadric(const GeomAbs_SurfaceType theSType)
{
  if (theSType == GeomAbs_Plane)
    return true;
  if (theSType == GeomAbs_Cylinder)
    return true;
  if (theSType == GeomAbs_Cone)
    return true;
  if (theSType == GeomAbs_Sphere)
    return true;
  if (theSType == GeomAbs_Torus)
    return true;
  return false;
}

static bool IsConic(const GeomAbs_CurveType theCType)
{
  if (theCType == GeomAbs_Line)
    return true;
  if (theCType == GeomAbs_Circle)
    return true;
  if (theCType == GeomAbs_Ellipse)
    return true;
  if (theCType == GeomAbs_Hyperbola)
    return true;
  if (theCType == GeomAbs_Parabola)
    return true;
  return false;
}

// restrict maximal parameter on hyperbola to avoid FPE
static double GetCurvMaxParamVal(const Adaptor3d_Curve& theC)
{
  if (theC.GetType() == GeomAbs_Hyperbola)
  {
    return HyperbolaLimit;
  }
  if (theC.GetType() == GeomAbs_OffsetCurve)
  {
    occ::handle<Geom_Curve>        aBC(theC.OffsetCurve()->BasisCurve());
    occ::handle<Geom_TrimmedCurve> aTC = occ::down_cast<Geom_TrimmedCurve>(aBC);
    if (!aTC.IsNull())
    {
      aBC = aTC->BasisCurve();
    }
    if (aBC->IsKind(STANDARD_TYPE(Geom_Hyperbola)))
      return HyperbolaLimit;
  }
  return MaxParamVal;
}

// restrict maximal parameter on surfaces based on hyperbola to avoid FPE
static void GetSurfMaxParamVals(const Adaptor3d_Surface& theS,
                                double&           theUmax,
                                double&           theVmax)
{
  theUmax = theVmax = MaxParamVal;

  if (theS.GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    theUmax = GetCurvMaxParamVal(*theS.BasisCurve());
  }
  else if (theS.GetType() == GeomAbs_SurfaceOfRevolution)
  {
    theVmax = GetCurvMaxParamVal(*theS.BasisCurve());
  }
  else if (theS.GetType() == GeomAbs_OffsetSurface)
  {
    GetSurfMaxParamVals(*theS.BasisSurface(), theUmax, theVmax);
  }
}

//=================================================================================================

Extrema_GenExtCS::Extrema_GenExtCS()
    : myDone(false),
      mytmin(0.0),
      mytsup(0.0),
      myumin(0.0),
      myusup(0.0),
      myvmin(0.0),
      myvsup(0.0),
      mytsample(0),
      myusample(0),
      myvsample(0),
      mytol1(0.0),
      mytol2(0.0),
      myS(NULL)
{
}

//=================================================================================================

Extrema_GenExtCS::~Extrema_GenExtCS()
{
  //
}

//=================================================================================================

Extrema_GenExtCS::Extrema_GenExtCS(const Adaptor3d_Curve&   C,
                                   const Adaptor3d_Surface& S,
                                   const int   NbT,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      Tol1,
                                   const double      Tol2)
{
  Initialize(S, NbU, NbV, Tol2);
  Perform(C, NbT, Tol1);
}

//=================================================================================================

Extrema_GenExtCS::Extrema_GenExtCS(const Adaptor3d_Curve&   C,
                                   const Adaptor3d_Surface& S,
                                   const int   NbT,
                                   const int   NbU,
                                   const int   NbV,
                                   const double      tmin,
                                   const double      tsup,
                                   const double      Umin,
                                   const double      Usup,
                                   const double      Vmin,
                                   const double      Vsup,
                                   const double      Tol1,
                                   const double      Tol2)
{
  Initialize(S, NbU, NbV, Umin, Usup, Vmin, Vsup, Tol2);
  Perform(C, NbT, tmin, tsup, Tol1);
}

//=================================================================================================

void Extrema_GenExtCS::Initialize(const Adaptor3d_Surface& S,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      Tol2)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S, NbU, NbV, myumin, myusup, myvmin, myvsup, Tol2);
}

//=================================================================================================

void Extrema_GenExtCS::Initialize(const Adaptor3d_Surface& S,
                                  const int   NbU,
                                  const int   NbV,
                                  const double      Umin,
                                  const double      Usup,
                                  const double      Vmin,
                                  const double      Vsup,
                                  const double      Tol2)
{
  myS       = &S;
  myusample = NbU;
  myvsample = NbV;
  myumin    = Umin;
  myusup    = Usup;
  myvmin    = Vmin;
  myvsup    = Vsup;
  mytol2    = Tol2;

  double umaxpar, vmaxpar;
  GetSurfMaxParamVals(*myS, umaxpar, vmaxpar);

  if (Precision::IsInfinite(myusup))
  {
    myusup = umaxpar;
  }
  if (Precision::IsInfinite(myumin))
  {
    myumin = -umaxpar;
  }
  if (Precision::IsInfinite(myvsup))
  {
    myvsup = vmaxpar;
  }
  if (Precision::IsInfinite(myvmin))
  {
    myvmin = -vmaxpar;
  }

  double       du    = (myusup - myumin) / aBorderDivisor;
  double       dv    = (myvsup - myvmin) / aBorderDivisor;
  const double aMinU = myumin + du;
  const double aMinV = myvmin + dv;
  const double aMaxU = myusup - du;
  const double aMaxV = myvsup - dv;

  const double aStepSU = (aMaxU - aMinU) / myusample;
  const double aStepSV = (aMaxV - aMinV) / myvsample;

  mySurfPnts = new NCollection_HArray2<gp_Pnt>(0, myusample, 0, myvsample);

  // Build UV parameter arrays for batch evaluation
  NCollection_Array1<double> aUParams(0, myusample);
  NCollection_Array1<double> aVParams(0, myvsample);

  double aSU = aMinU;
  for (int aSUI = 0; aSUI <= myusample; aSUI++, aSU += aStepSU)
  {
    aUParams.SetValue(aSUI, aSU);
  }
  double aSV = aMinV;
  for (int aSVI = 0; aSVI <= myvsample; aSVI++, aSV += aStepSV)
  {
    aVParams.SetValue(aSVI, aSV);
  }

  // Use batch grid evaluation for optimized surface point computation
  GeomGridEval_Surface anEvaluator;
  anEvaluator.Initialize(*myS);

  const NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(aUParams, aVParams);

  // aGrid is 1-based, mySurfPnts is 0-based - adjust indexing
  for (int aSUI = 0; aSUI <= myusample; aSUI++)
  {
    for (int aSVI = 0; aSVI <= myvsample; aSVI++)
    {
      mySurfPnts->ChangeValue(aSUI, aSVI) = aGrid.Value(aSUI + 1, aSVI + 1);
    }
  }
}

//=================================================================================================

void Extrema_GenExtCS::Perform(const Adaptor3d_Curve& C,
                               const int NbT,
                               const double    Tol1)
{
  mytmin = C.FirstParameter();
  mytsup = C.LastParameter();
  Perform(C, NbT, mytmin, mytsup, Tol1);
}

//=================================================================================================

void Extrema_GenExtCS::Perform(const Adaptor3d_Curve& C,
                               const int NbT,
                               const double    tmin,
                               const double    tsup,
                               const double    Tol1)
{
  myDone = false;
  myF.Initialize(C, *myS);
  mytmin    = tmin;
  mytsup    = tsup;
  mytol1    = Tol1;
  mytsample = NbT;
  // Modif de lvt pour trimer la surface non pas aux infinis mais  a +/- 10000

  double trimusup = myusup, trimumin = myumin, trimvsup = myvsup, trimvmin = myvmin;
  double aCMaxVal = GetCurvMaxParamVal(C);
  if (Precision::IsInfinite(mytsup))
  {
    mytsup = aCMaxVal;
  }
  if (Precision::IsInfinite(mytmin))
  {
    mytmin = -aCMaxVal;
  }
  //
  int    aNbVar = 3;
  GeomAbs_SurfaceType aSType = myS->GetType();
  if (IsQuadric(aSType))
  {
    aNbVar = 1;
  }
  else
  {
    GeomAbs_CurveType aCType = C.GetType();
    if (IsConic(aCType))
    {
      aNbVar = 2;
    }
  }

  math_Vector Tol(1, 3), TUV(1, 3), TUVinf(1, 3), TUVsup(1, 3);
  //
  Tol(1) = mytol1;
  Tol(2) = mytol2;
  Tol(3) = mytol2;
  //
  // Number of particles used in PSO algorithm (particle swarm optimization).
  const int aNbParticles = 48;

  int aNbIntC = 1;
  if (C.IsClosed() || C.IsPeriodic())
  {
    double aPeriod = C.Period();
    if (C.LastParameter() - C.FirstParameter() > 2. * aPeriod / 3.)
    {
      aNbIntC = 2;
    }
  }

  int anInt;
  double    dT = (mytsup - mytmin) / aNbIntC;
  for (anInt = 1; anInt <= aNbIntC; anInt++)
  {
    TUVinf(1) = mytmin + (anInt - 1) * dT;
    TUVinf(2) = trimumin;
    TUVinf(3) = trimvmin;
    //
    TUVsup(1) = TUVinf(1) + dT; // mytsup;
    TUVsup(2) = trimusup;
    TUVsup(3) = trimvsup;
    //
    if (aNbVar == 3)
    {
      GlobMinGenCS(C, aNbParticles, TUVinf, TUVsup, TUV);
    }
    else if (aNbVar == 2)
    {
      GlobMinConicS(C, aNbParticles, TUVinf, TUVsup, TUV);
    }
    else
    {
      GlobMinCQuadric(C, aNbParticles, TUVinf, TUVsup, TUV);
    }

    // Find min approximation
    math_FunctionSetRoot anA(myF, Tol);
    anA.Perform(myF, TUV, TUVinf, TUVsup);
  }
  if (aNbIntC > 1 && myF.NbExt() > 1)
  {
    // Try to remove "false" extrema caused by dividing curve interval
    NCollection_Sequence<double>&    aSqDists    = myF.SquareDistances();
    NCollection_Sequence<Extrema_POnCurv>& aPntsOnCrv  = myF.PointsOnCurve();
    NCollection_Sequence<Extrema_POnSurf>& aPntsOnSurf = myF.PointsOnSurf();
    NCollection_Sequence<double>     aSqDists1(aSqDists);
    NCollection_Sequence<Extrema_POnCurv>  aPntsOnCrv1(aPntsOnCrv);
    NCollection_Sequence<Extrema_POnSurf>  aPntsOnSurf1(aPntsOnSurf);

    double    aMinDist = aSqDists(1);
    int i;
    for (i = 2; i <= aSqDists.Length(); ++i)
    {
      double aDist = aSqDists(i);
      if (aDist < aMinDist)
      {
        aMinDist = aDist;
      }
    }
    aSqDists.Clear();
    aPntsOnCrv.Clear();
    aPntsOnSurf.Clear();
    constexpr double aTol = Precision::SquareConfusion();
    for (i = 1; i <= aSqDists1.Length(); ++i)
    {
      double aDist = aSqDists1(i);
      if (std::abs(aDist - aMinDist) <= aTol)
      {
        aSqDists.Append(aDist);
        aPntsOnCrv.Append(aPntsOnCrv1(i));
        aPntsOnSurf.Append(aPntsOnSurf1(i));
      }
    }
  }
  myDone = true;
}

//=================================================================================================

void Extrema_GenExtCS::GlobMinGenCS(const Adaptor3d_Curve& theC,
                                    const int theNbParticles,
                                    const math_Vector&     theTUVinf,
                                    const math_Vector&     theTUVsup,
                                    math_Vector&           theTUV)
{
  math_PSOParticlesPool aParticles(theNbParticles, 3);

  math_Vector aMinTUV(1, 3);
  aMinTUV = theTUVinf + (theTUVsup - theTUVinf) / aBorderDivisor;

  math_Vector aMaxTUV(1, 3);
  aMaxTUV = theTUVsup - (theTUVsup - theTUVinf) / aBorderDivisor;

  double aStepCU = (aMaxTUV(1) - aMinTUV(1)) / mytsample;
  double aStepSU = (aMaxTUV(2) - aMinTUV(2)) / myusample;
  double aStepSV = (aMaxTUV(3) - aMinTUV(3)) / myvsample;

  // Correct number of curve samples in case of low resolution
  int aNewCsample   = mytsample;
  double    aScaleFactor  = 5.0;
  double    aResolutionCU = aStepCU / theC.Resolution(1.0);

  double aMinResolution =
    aScaleFactor
    * std::min(aResolutionCU,
               std::min(aStepSU / myS->UResolution(1.0), aStepSV / myS->VResolution(1.0)));

  if (aMinResolution > Epsilon(1.0))
  {
    if (aResolutionCU > aMinResolution)
    {
      const int aMaxNbNodes = 50;

      aNewCsample = std::min(aMaxNbNodes, RealToInt(mytsample * aResolutionCU / aMinResolution));

      aStepCU = (aMaxTUV(1) - aMinTUV(1)) / aNewCsample;
    }
  }

  // Pre-compute curve sample points.
  NCollection_Array1<gp_Pnt> aCurvPnts(0, aNewCsample);

  double aCU1 = aMinTUV(1);
  for (int aCUI = 0; aCUI <= aNewCsample; aCUI++, aCU1 += aStepCU)
    aCurvPnts.SetValue(aCUI, theC.Value(aCU1));

  PSO_Particle* aParticle = aParticles.GetWorstParticle();
  // Select specified number of particles from pre-computed set of samples
  double aSU = aMinTUV(2);
  for (int aSUI = 0; aSUI <= myusample; aSUI++, aSU += aStepSU)
  {
    double aSV = aMinTUV(3);
    for (int aSVI = 0; aSVI <= myvsample; aSVI++, aSV += aStepSV)
    {
      double aCU2 = aMinTUV(1);
      for (int aCUI = 0; aCUI <= aNewCsample; aCUI++, aCU2 += aStepCU)
      {
        double aSqDist = mySurfPnts->Value(aSUI, aSVI).SquareDistance(aCurvPnts.Value(aCUI));

        if (aSqDist < aParticle->Distance)
        {
          aParticle->Position[0] = aCU2;
          aParticle->Position[1] = aSU;
          aParticle->Position[2] = aSV;

          aParticle->BestPosition[0] = aCU2;
          aParticle->BestPosition[1] = aSU;
          aParticle->BestPosition[2] = aSV;

          aParticle->Distance     = aSqDist;
          aParticle->BestDistance = aSqDist;

          aParticle = aParticles.GetWorstParticle();
        }
      }
    }
  }

  math_Vector aStep(1, 3);
  aStep(1) = aStepCU;
  aStep(2) = aStepSU;
  aStep(3) = aStepSV;

  // Find min approximation
  double         aValue;
  Extrema_GlobOptFuncCS aFunc(&theC, myS);
  math_PSO              aPSO(&aFunc, theTUVinf, theTUVsup, aStep);
  aPSO.Perform(aParticles, theNbParticles, aValue, theTUV);
}

//=================================================================================================

void Extrema_GenExtCS::GlobMinConicS(const Adaptor3d_Curve& theC,
                                     const int theNbParticles,
                                     const math_Vector&     theTUVinf,
                                     const math_Vector&     theTUVsup,
                                     math_Vector&           theTUV)
{
  int aNbVar = 2;
  math_Vector      anUVinf(1, aNbVar), anUVsup(1, aNbVar), anUV(1, aNbVar);
  int i;
  for (i = 1; i <= aNbVar; ++i)
  {
    anUVinf(i) = theTUVinf(i + 1);
    anUVsup(i) = theTUVsup(i + 1);
  }
  //
  //
  math_PSOParticlesPool aParticles(theNbParticles, aNbVar);

  math_Vector aMinUV(1, aNbVar);
  aMinUV = anUVinf + (anUVsup - anUVinf) / aBorderDivisor;

  math_Vector aMaxUV(1, aNbVar);
  aMaxUV = anUVsup - (anUVsup - anUVinf) / aBorderDivisor;

  // Increase numbers of UV samples to improve searching global minimum
  int anAddsample = std::max(mytsample / 2, 3);
  int anUsample   = myusample + anAddsample;
  int aVsample    = myvsample + anAddsample;
  //
  double aStepSU = (aMaxUV(1) - aMinUV(1)) / anUsample;
  double aStepSV = (aMaxUV(2) - aMinUV(2)) / aVsample;
  //
  Extrema_GlobOptFuncConicS aFunc(myS, anUVinf(1), anUVsup(1), anUVinf(2), anUVsup(2));
  aFunc.LoadConic(&theC, theTUVinf(1), theTUVsup(1));

  PSO_Particle* aParticle = aParticles.GetWorstParticle();
  // Select specified number of particles from pre-computed set of samples
  double aSU = aMinUV(1);

  for (int aSUI = 0; aSUI <= anUsample; aSUI++, aSU += aStepSU)
  {
    anUV(1)           = aSU;
    double aSV = aMinUV(2);
    for (int aSVI = 0; aSVI <= aVsample; aSVI++, aSV += aStepSV)
    {
      anUV(2) = aSV;
      double aSqDist;
      if (!aFunc.Value(anUV, aSqDist))
      {
        aSqDist = Precision::Infinite();
      }

      if (aSqDist < aParticle->Distance)
      {
        aParticle->Position[0] = aSU;
        aParticle->Position[1] = aSV;

        aParticle->BestPosition[0] = aSU;
        aParticle->BestPosition[1] = aSV;

        aParticle->Distance     = aSqDist;
        aParticle->BestDistance = aSqDist;

        aParticle = aParticles.GetWorstParticle();
      }
    }
  }

  math_Vector aStep(1, aNbVar);
  aStep(1) = aStepSU;
  aStep(2) = aStepSV;

  // Find min approximation
  double aValue;
  math_PSO      aPSO(&aFunc, anUVinf, anUVsup, aStep);
  aPSO.Perform(aParticles, theNbParticles, aValue, anUV);
  //
  double aCT = aFunc.ConicParameter(anUV);
  if (theC.IsPeriodic())
  {
    if (aCT < theTUVinf(1) - Precision::PConfusion()
        || aCT > theTUVsup(1) + Precision::PConfusion())
    {
      aCT = ElCLib::InPeriod(aCT, theTUVinf(1), theTUVinf(1) + 2. * M_PI);
    }
  }

  theTUV(1) = aCT;
  theTUV(2) = anUV(1);
  theTUV(3) = anUV(2);

  bool isBadSol = false;
  gp_Vec           aDU, aDV, aDT;
  gp_Pnt           aPOnS, aPOnC;
  myS->D1(anUV(1), anUV(2), aPOnS, aDU, aDV);
  theC.D1(aCT, aPOnC, aDT);
  double aSqDist = aPOnC.SquareDistance(aPOnS);
  if (aSqDist <= Precision::SquareConfusion())
    return;

  gp_Vec aN = aDU.Crossed(aDV);
  if (aN.SquareMagnitude() < Precision::SquareConfusion())
    return;

  gp_Vec PcPs(aPOnC, aPOnS);

  double anAngMin = M_PI_2 - M_PI_2 / 10.;
  double anAngMax = M_PI_2 + M_PI_2 / 10.;

  double anAngN = PcPs.Angle(aN);
  if (anAngN >= anAngMin && anAngN <= anAngMax)
  {
    // PcPs is perpendicular to surface normal, it means that
    // aPOnC can be on surface, but far from aPOnS
    isBadSol = true;
    int iu, iv;
    for (iu = -1; iu <= 1; ++iu)
    {
      double u = anUV(1) + iu * aStepSU;
      u               = std::max(anUVinf(1), u);
      u               = std::min(anUVsup(1), u);
      for (iv = -1; iv <= 1; ++iv)
      {
        double v = anUV(2) + iv * aStepSV;
        v               = std::max(anUVinf(2), v);
        v               = std::min(anUVsup(2), v);
        myS->D1(u, v, aPOnS, aDU, aDV);
        if (aPOnC.SquareDistance(aPOnS) < Precision::SquareConfusion())
        {
          isBadSol = false;
          break;
        }
        aN = aDU.Crossed(aDV);
        if (aN.SquareMagnitude() < Precision::SquareConfusion())
        {
          isBadSol = false;
          break;
        }
        PcPs.SetXYZ(aPOnS.XYZ() - aPOnC.XYZ());
        anAngN = PcPs.Angle(aN);
        if (anAngN < anAngMin || anAngN > anAngMax)
        {
          isBadSol = false;
          break;
        }
      }
      if (!isBadSol)
      {
        break;
      }
    }
  }

  if (isBadSol)
  {
    // Try to precise solution with help of Extrema PS

    math_Vector aF(1, 3);
    aF(1)             = PcPs.Dot(aDT);
    aF(2)             = PcPs.Dot(aDU);
    aF(3)             = PcPs.Dot(aDV);
    double aFF = aF.Norm2();

    Extrema_GenLocateExtPS anExtPS(*myS, mytol2, mytol2);
    anExtPS.Perform(aPOnC, anUV(1), anUV(2), false);
    if (anExtPS.IsDone())
    {
      const Extrema_POnSurf& aPmin = anExtPS.Point();
      aPmin.Parameter(anUV(1), anUV(2));
      math_Vector aTUV = theTUV;
      aTUV(2)          = anUV(1);
      aTUV(3)          = anUV(2);
      myF.Value(aTUV, aF);
      double aFF1 = aF.Norm2();

      if (anExtPS.SquareDistance() < aSqDist && aFF1 <= 1.1 * aFF)
      {
        theTUV(2) = aTUV(2);
        theTUV(3) = aTUV(3);
      }
    }
  }
}

//=================================================================================================

void Extrema_GenExtCS::GlobMinCQuadric(const Adaptor3d_Curve& theC,
                                       const int theNbParticles,
                                       const math_Vector&     theTUVinf,
                                       const math_Vector&     theTUVsup,
                                       math_Vector&           theTUV)
{
  int aNbVar = 1;
  math_Vector      aTinf(1, aNbVar), aTsup(1, aNbVar), aT(1, aNbVar);
  aTinf(1) = theTUVinf(1);
  aTsup(1) = theTUVsup(1);
  //
  math_PSOParticlesPool aParticles(theNbParticles, aNbVar);

  math_Vector aMinT(1, aNbVar);
  aMinT = aTinf + (aTsup - aTinf) / aBorderDivisor;

  math_Vector aMaxT(1, aNbVar);
  aMaxT = aTsup - (aTsup - aTinf) / aBorderDivisor;
  //

  // Increase numbers of curve samples to improve searching global minimum
  // because dimension of optimisation task is reduced
  const int aMaxNbNodes = 50;
  int       aNewCsample = mytsample;
  int       anAddsample = std::max(myusample / 2, 3);
  aNewCsample += anAddsample;
  aNewCsample = std::min(aNewCsample, aMaxNbNodes);
  //
  // Correct number of curve samples in case of low resolution
  double aStepCT       = (aMaxT(1) - aMinT(1)) / aNewCsample;
  double aStepSU       = (theTUVsup(2) - theTUVinf(2)) / myusample;
  double aStepSV       = (theTUVsup(3) - theTUVinf(3)) / myvsample;
  double aScaleFactor  = 5.0;
  double aResolutionCU = aStepCT / theC.Resolution(1.0);

  double aMinResolution =
    aScaleFactor
    * std::min(aResolutionCU,
               std::min(aStepSU / myS->UResolution(1.0), aStepSV / myS->VResolution(1.0)));

  if (aMinResolution > Epsilon(1.0))
  {
    if (aResolutionCU > aMinResolution)
    {

      aNewCsample = std::min(aMaxNbNodes, RealToInt(aNewCsample * aResolutionCU / aMinResolution));

      aStepCT = (aMaxT(1) - aMinT(1)) / aNewCsample;
    }
  }

  //
  Extrema_GlobOptFuncCQuadric aFunc(&theC, aTinf(1), aTsup(1));
  aFunc.LoadQuad(myS, theTUVinf(2), theTUVsup(2), theTUVinf(3), theTUVsup(3));

  PSO_Particle* aParticle = aParticles.GetWorstParticle();
  // Select specified number of particles from pre-computed set of samples
  double aCT = aMinT(1);
  for (int aCUI = 0; aCUI <= aNewCsample; aCUI++, aCT += aStepCT)
  {
    aT(1) = aCT;
    double aSqDist;
    if (!aFunc.Value(aT, aSqDist))
    {
      aSqDist = Precision::Infinite();
    }

    if (aSqDist < aParticle->Distance)
    {
      aParticle->Position[0] = aCT;

      aParticle->BestPosition[0] = aCT;

      aParticle->Distance     = aSqDist;
      aParticle->BestDistance = aSqDist;

      aParticle = aParticles.GetWorstParticle();
    }
  }
  //
  math_Vector aStep(1, aNbVar);
  aStep(1) = aStepCT;

  // Find min approximation
  double aValue;
  math_PSO      aPSO(&aFunc, aTinf, aTsup, aStep);
  aPSO.Perform(aParticles, theNbParticles, aValue, aT);
  //
  math_Vector anUV(1, 2);
  aFunc.QuadricParameters(aT, anUV);
  if (myS->IsUPeriodic())
  {
    if (anUV(1) < theTUVinf(2) - Precision::PConfusion()
        || anUV(1) > theTUVsup(2) + Precision::PConfusion())
    {
      anUV(1) = ElCLib::InPeriod(anUV(1), theTUVinf(2), theTUVinf(2) + 2. * M_PI);
    }
  }
  //
  if (myS->IsVPeriodic())
  {
    if (anUV(2) < theTUVinf(3) - Precision::PConfusion()
        || anUV(2) > theTUVsup(3) + Precision::PConfusion())
    {
      anUV(2) = ElCLib::InPeriod(anUV(2), theTUVinf(3), theTUVinf(3) + 2. * M_PI);
    }
  }
  //
  theTUV(1) = aT(1);
  theTUV(2) = anUV(1);
  theTUV(3) = anUV(2);
}

//=================================================================================================

bool Extrema_GenExtCS::IsDone() const
{
  return myDone;
}

//=================================================================================================

int Extrema_GenExtCS::NbExt() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myF.NbExt();
}

//=================================================================================================

double Extrema_GenExtCS::SquareDistance(const int N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.SquareDistance(N);
}

//=================================================================================================

const Extrema_POnCurv& Extrema_GenExtCS::PointOnCurve(const int N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.PointOnCurve(N);
}

//=================================================================================================

const Extrema_POnSurf& Extrema_GenExtCS::PointOnSurface(const int N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return myF.PointOnSurface(N);
}
