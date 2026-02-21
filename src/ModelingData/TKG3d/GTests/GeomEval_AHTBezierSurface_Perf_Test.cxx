// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <GeomAdaptor_Surface.hxx>
#include <GeomEval_AHTBezierSurface.hxx>
#include <GeomEval_RepSurfaceDesc.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomConvert.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace
{

using Clock = std::chrono::steady_clock;

struct BenchSummary
{
  const char* Name = "";
  double      Ms   = 0.0;
  double      Sink = 0.0;
};

int envIntOrDefault(const char* theName, const int theDefault, const int theMin)
{
  const char* aRawVal = std::getenv(theName);
  if (aRawVal == nullptr)
  {
    return theDefault;
  }
  return std::max(theMin, std::atoi(aRawVal));
}

template <typename FuncT>
BenchSummary runBench(const char*                      theName,
                      const int                        theRepeats,
                      const NCollection_Array1<gp_XY>& theParams,
                      FuncT&&                          theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = Clock::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i).X(), theParams.Value(i).Y());
    }
  }

  const auto                                      aEnd = Clock::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printRow(const BenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(36) << theResult.Name << std::right << std::setw(12)
            << std::fixed << std::setprecision(3) << theResult.Ms << std::setw(14)
            << std::setprecision(3) << aMcallsPerSec << std::setw(16) << std::setprecision(6)
            << theResult.Sink << '\n';
}

void printHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(36) << "Path" << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s" << std::setw(16) << "sink" << '\n';
}

double vecDiff(const gp_Vec& theV1, const gp_Vec& theV2)
{
  return (theV1 - theV2).Magnitude();
}

// Cubic Bernstein -> power conversion matrix.
// a = C * b, where b are Bernstein poles and a are power coefficients.
void buildEquivalentAhtPoles(const NCollection_Array2<gp_Pnt>& theBezierPoles,
                             NCollection_Array2<gp_Pnt>&       theAhtPoles)
{
  const double C[4][4] = {{1.0, 0.0, 0.0, 0.0},
                          {-3.0, 3.0, 0.0, 0.0},
                          {3.0, -6.0, 3.0, 0.0},
                          {-1.0, 3.0, -3.0, 1.0}};

  for (int pu = 0; pu <= 3; ++pu)
  {
    for (int pv = 0; pv <= 3; ++pv)
    {
      gp_XYZ aCoeff(0.0, 0.0, 0.0);
      for (int i = 0; i <= 3; ++i)
      {
        for (int j = 0; j <= 3; ++j)
        {
          aCoeff += theBezierPoles.Value(i + 1, j + 1).XYZ() * (C[pu][i] * C[pv][j]);
        }
      }
      theAhtPoles.SetValue(pu + 1, pv + 1, gp_Pnt(aCoeff));
    }
  }
}

} // namespace

TEST(GeomEval_AHTBezierSurfacePerfTest, DISABLED_D0ToD3_EquivalentCubicAndAdaptorPaths)
{
  const int aRepeats   = envIntOrDefault("OCCT_GEOMEVAL_PERF_REPEATS", 300, 1);
  const int aUSamples  = envIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES_U", 48, 4);
  const int aVSamples  = envIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES_V", 48, 4);
  const int aNbSamples = aUSamples * aVSamples;

  // Build an exact cubic Bezier surface and equivalent cubic power-form AHT surface.
  NCollection_Array2<gp_Pnt> aBezPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double aX = 0.8 * i - 0.25 * j + 0.15 * i * j;
      const double aY = -0.4 * i + 1.1 * j - 0.10 * i * j;
      const double aZ = 0.6 * i * i - 0.35 * j * j + 0.22 * i * j;
      aBezPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  const Handle(Geom_BezierSurface)  aBezierSurface = new Geom_BezierSurface(aBezPoles);
  const Handle(Geom_BSplineSurface) aBSplineSurface =
    GeomConvert::SurfaceToBSplineSurface(aBezierSurface);

  NCollection_Array2<gp_Pnt> aAhtPoles(1, 4, 1, 4);
  buildEquivalentAhtPoles(aBezPoles, aAhtPoles);
  const Handle(GeomEval_AHTBezierSurface) anAhtSurface =
    new GeomEval_AHTBezierSurface(aAhtPoles, 3, 3, 0.0, 0.0, 0.0, 0.0);

  const Handle(Geom_BezierSurface) aBezierWithRep =
    Handle(Geom_BezierSurface)::DownCast(aBezierSurface->Copy());
  const Handle(Geom_BSplineSurface) aBSplineWithRep =
    Handle(Geom_BSplineSurface)::DownCast(aBSplineSurface->Copy());
  ASSERT_FALSE(aBezierWithRep.IsNull());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(GeomEval_RepSurfaceDesc::Full) aBezDesc = new GeomEval_RepSurfaceDesc::Full();
  aBezDesc->Representation                             = anAhtSurface;
  aBezierWithRep->SetEvalRepresentation(aBezDesc);

  const Handle(GeomEval_RepSurfaceDesc::Full) aBspDesc = new GeomEval_RepSurfaceDesc::Full();
  aBspDesc->Representation                             = anAhtSurface;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  GeomAdaptor_Surface anAdaptorBezier(aBezierSurface);
  GeomAdaptor_Surface anAdaptorBSpline(aBSplineSurface);
  GeomAdaptor_Surface anAdaptorBezierRep(aBezierWithRep);
  GeomAdaptor_Surface anAdaptorBSplineRep(aBSplineWithRep);

  NCollection_Array1<gp_XY> aParams(1, aNbSamples);
  int                       anIdx = aParams.Lower();
  for (int iu = 0; iu < aUSamples; ++iu)
  {
    const double aU = (aUSamples == 1) ? 0.0 : double(iu) / double(aUSamples - 1);
    for (int iv = 0; iv < aVSamples; ++iv)
    {
      const double aV = (aVSamples == 1) ? 0.0 : double(iv) / double(aVSamples - 1);
      aParams.SetValue(anIdx++, gp_XY(aU, aV));
    }
  }

  double aMaxD0Bez = 0.0;
  double aMaxD0Bsp = 0.0;
  double aMaxD1Bez = 0.0;
  double aMaxD1Bsp = 0.0;
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aU = aParams.Value(i).X();
    const double aV = aParams.Value(i).Y();

    gp_Pnt aPAht;
    gp_Pnt aPBez;
    gp_Pnt aPBsp;
    gp_Vec aAhtDU;
    gp_Vec aAhtDV;
    gp_Vec aBezDU;
    gp_Vec aBezDV;
    gp_Vec aBspDU;
    gp_Vec aBspDV;

    anAhtSurface->D0(aU, aV, aPAht);
    aBezierSurface->D0(aU, aV, aPBez);
    aBSplineSurface->D0(aU, aV, aPBsp);

    anAhtSurface->D1(aU, aV, aPAht, aAhtDU, aAhtDV);
    aBezierSurface->D1(aU, aV, aPBez, aBezDU, aBezDV);
    aBSplineSurface->D1(aU, aV, aPBsp, aBspDU, aBspDV);

    aMaxD0Bez = std::max(aMaxD0Bez, aPAht.Distance(aPBez));
    aMaxD0Bsp = std::max(aMaxD0Bsp, aPAht.Distance(aPBsp));
    aMaxD1Bez = std::max(aMaxD1Bez, vecDiff(aAhtDU, aBezDU) + vecDiff(aAhtDV, aBezDV));
    aMaxD1Bsp = std::max(aMaxD1Bsp, vecDiff(aAhtDU, aBspDU) + vecDiff(aAhtDV, aBspDV));
  }

  EXPECT_LE(aMaxD0Bez, 1e-12);
  EXPECT_LE(aMaxD0Bsp, 1e-12);
  EXPECT_LE(aMaxD1Bez, 1e-10);
  EXPECT_LE(aMaxD1Bsp, 1e-10);

  const int aCalls = aRepeats * aNbSamples;

  std::cout << "GeomEval surface temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", UxV Samples: " << aUSamples << 'x' << aVSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (AHT surface vs exact cubic Bezier/BSpline)\n";
  std::cout << "  max |D0(AHT)-D0(Bezier)|  = " << std::setprecision(12) << aMaxD0Bez << '\n';
  std::cout << "  max |D0(AHT)-D0(BSpline)| = " << std::setprecision(12) << aMaxD0Bsp << '\n';
  std::cout << "  max |D1(AHT)-D1(Bezier)|  = " << std::setprecision(12) << aMaxD1Bez << '\n';
  std::cout << "  max |D1(AHT)-D1(BSpline)| = " << std::setprecision(12) << aMaxD1Bsp << '\n';

  printHeader("D0");
  const BenchSummary aDirectAhtD0 =
    runBench("Direct_AHTSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      anAhtSurface->D0(theU, theV, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary aDirectBezD0 =
    runBench("Direct_BezierSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      aBezierSurface->D0(theU, theV, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary aDirectBspD0 =
    runBench("Direct_BSplineSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      aBSplineSurface->D0(theU, theV, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary anAdaptorBezD0 =
    runBench("Adaptor_BezierSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      anAdaptorBezier.D0(theU, theV, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary anAdaptorBspD0 =
    runBench("Adaptor_BSplineSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      anAdaptorBSpline.D0(theU, theV, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary anAdaptorBezD0Rep = runBench("Adaptor_BezierSurface_D0_WithEvalRep",
                                                  aRepeats,
                                                  aParams,
                                                  [&](double theU, double theV) {
                                                    gp_Pnt aP;
                                                    anAdaptorBezierRep.D0(theU, theV, aP);
                                                    return aP.X() + aP.Y() + aP.Z();
                                                  });
  const BenchSummary anAdaptorBspD0Rep = runBench("Adaptor_BSplineSurface_D0_WithEvalRep",
                                                  aRepeats,
                                                  aParams,
                                                  [&](double theU, double theV) {
                                                    gp_Pnt aP;
                                                    anAdaptorBSplineRep.D0(theU, theV, aP);
                                                    return aP.X() + aP.Y() + aP.Z();
                                                  });
  printRow(aDirectAhtD0, aCalls);
  printRow(aDirectBezD0, aCalls);
  printRow(aDirectBspD0, aCalls);
  printRow(anAdaptorBezD0, aCalls);
  printRow(anAdaptorBspD0, aCalls);
  printRow(anAdaptorBezD0Rep, aCalls);
  printRow(anAdaptorBspD0Rep, aCalls);

  printHeader("D1");
  const BenchSummary aDirectAhtD1 =
    runBench("Direct_AHTSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      anAhtSurface->D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary aDirectBezD1 =
    runBench("Direct_BezierSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      aBezierSurface->D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary aDirectBspD1 =
    runBench("Direct_BSplineSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      aBSplineSurface->D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary anAdaptorBezD1 =
    runBench("Adaptor_BezierSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      anAdaptorBezier.D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary anAdaptorBspD1 =
    runBench("Adaptor_BSplineSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      anAdaptorBSpline.D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary anAdaptorBezD1Rep = runBench(
    "Adaptor_BezierSurface_D1_WithEvalRep",
    aRepeats,
    aParams,
    [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      anAdaptorBezierRep.D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  const BenchSummary anAdaptorBspD1Rep = runBench(
    "Adaptor_BSplineSurface_D1_WithEvalRep",
    aRepeats,
    aParams,
    [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      anAdaptorBSplineRep.D1(theU, theV, aP, aDU, aDV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
    });
  printRow(aDirectAhtD1, aCalls);
  printRow(aDirectBezD1, aCalls);
  printRow(aDirectBspD1, aCalls);
  printRow(anAdaptorBezD1, aCalls);
  printRow(anAdaptorBspD1, aCalls);
  printRow(anAdaptorBezD1Rep, aCalls);
  printRow(anAdaptorBspD1Rep, aCalls);

  printHeader("D2");
  const BenchSummary aDirectAhtD2 =
    runBench("Direct_AHTSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aD2U;
      gp_Vec aD2V;
      gp_Vec aD2UV;
      anAhtSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
             + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X()
             + aD2UV.Y() + aD2UV.Z();
    });
  const BenchSummary aDirectBezD2 =
    runBench("Direct_BezierSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aD2U;
      gp_Vec aD2V;
      gp_Vec aD2UV;
      aBezierSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
             + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X()
             + aD2UV.Y() + aD2UV.Z();
    });
  const BenchSummary aDirectBspD2 =
    runBench("Direct_BSplineSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aD2U;
      gp_Vec aD2V;
      gp_Vec aD2UV;
      aBSplineSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
             + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X()
             + aD2UV.Y() + aD2UV.Z();
    });
  const BenchSummary anAdaptorBezD2 =
    runBench("Adaptor_BezierSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aD2U;
      gp_Vec aD2V;
      gp_Vec aD2UV;
      anAdaptorBezier.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
             + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X()
             + aD2UV.Y() + aD2UV.Z();
    });
  const BenchSummary anAdaptorBspD2 =
    runBench("Adaptor_BSplineSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
      gp_Pnt aP;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aD2U;
      gp_Vec aD2V;
      gp_Vec aD2UV;
      anAdaptorBSpline.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
      return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
             + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X()
             + aD2UV.Y() + aD2UV.Z();
    });
  const BenchSummary anAdaptorBezD2Rep =
    runBench("Adaptor_BezierSurface_D2_WithEvalRep",
             aRepeats,
             aParams,
             [&](double theU, double theV) {
               gp_Pnt aP;
               gp_Vec aDU;
               gp_Vec aDV;
               gp_Vec aD2U;
               gp_Vec aD2V;
               gp_Vec aD2UV;
               anAdaptorBezierRep.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
               return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y()
                      + aDV.Z() + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z()
                      + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
             });
  const BenchSummary anAdaptorBspD2Rep =
    runBench("Adaptor_BSplineSurface_D2_WithEvalRep",
             aRepeats,
             aParams,
             [&](double theU, double theV) {
               gp_Pnt aP;
               gp_Vec aDU;
               gp_Vec aDV;
               gp_Vec aD2U;
               gp_Vec aD2V;
               gp_Vec aD2UV;
               anAdaptorBSplineRep.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
               return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y()
                      + aDV.Z() + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z()
                      + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
             });
  printRow(aDirectAhtD2, aCalls);
  printRow(aDirectBezD2, aCalls);
  printRow(aDirectBspD2, aCalls);
  printRow(anAdaptorBezD2, aCalls);
  printRow(anAdaptorBspD2, aCalls);
  printRow(anAdaptorBezD2Rep, aCalls);
  printRow(anAdaptorBspD2Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectAhtD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD2.Sink));
}
