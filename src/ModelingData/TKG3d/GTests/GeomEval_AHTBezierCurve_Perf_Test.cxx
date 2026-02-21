// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <GeomAdaptor_Curve.hxx>
#include <GeomEval_AHTBezierCurve.hxx>
#include <GeomEval_RepCurveDesc.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomConvert.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

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
BenchSummary runBench(const char*                        theName,
                      const int                          theRepeats,
                      const NCollection_Array1<double>& theParams,
                      FuncT&&                            theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = Clock::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i));
    }
  }

  const auto aEnd = Clock::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printRow(const BenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(34) << theResult.Name
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << theResult.Ms
            << std::setw(14) << std::setprecision(3) << aMcallsPerSec
            << std::setw(16) << std::setprecision(6) << theResult.Sink << '\n';
}

void printHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(34) << "Path"
            << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s"
            << std::setw(16) << "sink" << '\n';
}

double vecDiff(const gp_Vec& theV1, const gp_Vec& theV2)
{
  return (theV1 - theV2).Magnitude();
}

} // namespace

TEST(GeomEval_AHTBezierCurvePerfTest, DISABLED_D0ToD3_EquivalentCubicAndAdaptorPaths)
{
  const int aRepeats   = envIntOrDefault("OCCT_GEOMEVAL_PERF_REPEATS", 1000, 1);
  const int aNbSamples = envIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES", 1024, 8);

  // Build equivalent cubic curves on [0, 1]:
  // C(t) = A0 + A1*t + A2*t^2 + A3*t^3.
  const gp_Pnt aA0(1.25, -0.50, 0.10);
  const gp_Pnt aA1(4.00, 1.75, -2.20);
  const gp_Pnt aA2(-1.20, 3.10, 0.65);
  const gp_Pnt aA3(0.80, -2.40, 1.35);

  NCollection_Array1<gp_Pnt> aAhtPoles(1, 4);
  aAhtPoles.SetValue(1, aA0);
  aAhtPoles.SetValue(2, aA1);
  aAhtPoles.SetValue(3, aA2);
  aAhtPoles.SetValue(4, aA3);

  const Handle(GeomEval_AHTBezierCurve) anAhtCurve =
    new GeomEval_AHTBezierCurve(aAhtPoles, 3, 0.0, 0.0);

  // Power -> Bernstein conversion for cubic (exact).
  NCollection_Array1<gp_Pnt> aBezPoles(1, 4);
  aBezPoles.SetValue(1, aA0);
  aBezPoles.SetValue(2, gp_Pnt(aA0.XYZ() + aA1.XYZ() / 3.0));
  aBezPoles.SetValue(3, gp_Pnt(aA0.XYZ() + (2.0 / 3.0) * aA1.XYZ() + aA2.XYZ() / 3.0));
  aBezPoles.SetValue(4, gp_Pnt(aA0.XYZ() + aA1.XYZ() + aA2.XYZ() + aA3.XYZ()));

  const Handle(Geom_BezierCurve)  aBezierCurve  = new Geom_BezierCurve(aBezPoles);
  const Handle(Geom_BSplineCurve) aBSplineCurve = GeomConvert::CurveToBSplineCurve(aBezierCurve);

  const Handle(Geom_BezierCurve) aBezierWithRep =
    Handle(Geom_BezierCurve)::DownCast(aBezierCurve->Copy());
  const Handle(Geom_BSplineCurve) aBSplineWithRep =
    Handle(Geom_BSplineCurve)::DownCast(aBSplineCurve->Copy());
  ASSERT_FALSE(aBezierWithRep.IsNull());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(GeomEval_RepCurveDesc::Full) aBezDesc = new GeomEval_RepCurveDesc::Full();
  aBezDesc->Representation = anAhtCurve;
  aBezierWithRep->SetEvalRepresentation(aBezDesc);

  const Handle(GeomEval_RepCurveDesc::Full) aBspDesc = new GeomEval_RepCurveDesc::Full();
  aBspDesc->Representation = anAhtCurve;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  GeomAdaptor_Curve anAdaptorBezier(aBezierCurve);
  GeomAdaptor_Curve anAdaptorBSpline(aBSplineCurve);
  GeomAdaptor_Curve anAdaptorBezierRep(aBezierWithRep);
  GeomAdaptor_Curve anAdaptorBSplineRep(aBSplineWithRep);

  NCollection_Array1<double> aParams(1, aNbSamples);
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aT = double(i - aParams.Lower()) / double(aParams.Size() - 1);
    aParams.SetValue(i, aT);
  }

  // Basic equivalence check for D0 and D1 between direct Eval and exact cubic Bezier/BSpline.
  double aMaxD0Bez = 0.0;
  double aMaxD0Bsp = 0.0;
  double aMaxD1Bez = 0.0;
  double aMaxD1Bsp = 0.0;
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aU = aParams.Value(i);

    gp_Pnt aPAht;
    gp_Pnt aPBez;
    gp_Pnt aPBsp;
    gp_Vec aVAht;
    gp_Vec aVBez;
    gp_Vec aVBsp;

    anAhtCurve->D0(aU, aPAht);
    aBezierCurve->D0(aU, aPBez);
    aBSplineCurve->D0(aU, aPBsp);

    anAhtCurve->D1(aU, aPAht, aVAht);
    aBezierCurve->D1(aU, aPBez, aVBez);
    aBSplineCurve->D1(aU, aPBsp, aVBsp);

    aMaxD0Bez = std::max(aMaxD0Bez, aPAht.Distance(aPBez));
    aMaxD0Bsp = std::max(aMaxD0Bsp, aPAht.Distance(aPBsp));
    aMaxD1Bez = std::max(aMaxD1Bez, vecDiff(aVAht, aVBez));
    aMaxD1Bsp = std::max(aMaxD1Bsp, vecDiff(aVAht, aVBsp));
  }

  EXPECT_LE(aMaxD0Bez, 1e-12);
  EXPECT_LE(aMaxD0Bsp, 1e-12);
  EXPECT_LE(aMaxD1Bez, 1e-11);
  EXPECT_LE(aMaxD1Bsp, 1e-11);

  const int aCalls = aRepeats * aNbSamples;

  std::cout << "GeomEval temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", Samples: " << aNbSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (AHT vs exact cubic Bezier/BSpline)\n";
  std::cout << "  max |D0(AHT)-D0(Bezier)|   = " << std::setprecision(12) << aMaxD0Bez << '\n';
  std::cout << "  max |D0(AHT)-D0(BSpline)|  = " << std::setprecision(12) << aMaxD0Bsp << '\n';
  std::cout << "  max |D1(AHT)-D1(Bezier)|   = " << std::setprecision(12) << aMaxD1Bez << '\n';
  std::cout << "  max |D1(AHT)-D1(BSpline)|  = " << std::setprecision(12) << aMaxD1Bsp << '\n';

  printHeader("D0");
  const BenchSummary aDirectAhtD0 = runBench("Direct_AHT_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    anAhtCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const BenchSummary aDirectBezD0 = runBench("Direct_Bezier_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    aBezierCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const BenchSummary aDirectBspD0 = runBench("Direct_BSpline_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    aBSplineCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const BenchSummary anAdaptorBezD0 = runBench("Adaptor_Bezier_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    anAdaptorBezier.D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const BenchSummary anAdaptorBspD0 = runBench("Adaptor_BSpline_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    anAdaptorBSpline.D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const BenchSummary anAdaptorBezD0Rep =
    runBench("Adaptor_Bezier_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      anAdaptorBezierRep.D0(theU, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  const BenchSummary anAdaptorBspD0Rep =
    runBench("Adaptor_BSpline_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      anAdaptorBSplineRep.D0(theU, aP);
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
  const BenchSummary aDirectAhtD1 = runBench("Direct_AHT_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    anAhtCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const BenchSummary aDirectBezD1 = runBench("Direct_Bezier_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    aBezierCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const BenchSummary aDirectBspD1 = runBench("Direct_BSpline_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    aBSplineCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const BenchSummary anAdaptorBezD1 = runBench("Adaptor_Bezier_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    anAdaptorBezier.D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const BenchSummary anAdaptorBspD1 = runBench("Adaptor_BSpline_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    anAdaptorBSpline.D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const BenchSummary anAdaptorBezD1Rep =
    runBench("Adaptor_Bezier_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV;
      anAdaptorBezierRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
    });
  const BenchSummary anAdaptorBspD1Rep =
    runBench("Adaptor_BSpline_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV;
      anAdaptorBSplineRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
    });
  printRow(aDirectAhtD1, aCalls);
  printRow(aDirectBezD1, aCalls);
  printRow(aDirectBspD1, aCalls);
  printRow(anAdaptorBezD1, aCalls);
  printRow(anAdaptorBspD1, aCalls);
  printRow(anAdaptorBezD1Rep, aCalls);
  printRow(anAdaptorBspD1Rep, aCalls);

  printHeader("D2");
  const BenchSummary aDirectAhtD2 = runBench("Direct_AHT_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    anAhtCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const BenchSummary aDirectBezD2 = runBench("Direct_Bezier_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    aBezierCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const BenchSummary aDirectBspD2 = runBench("Direct_BSpline_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    aBSplineCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const BenchSummary anAdaptorBezD2 = runBench("Adaptor_Bezier_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    anAdaptorBezier.D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const BenchSummary anAdaptorBspD2 = runBench("Adaptor_BSpline_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    anAdaptorBSpline.D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const BenchSummary anAdaptorBezD2Rep =
    runBench("Adaptor_Bezier_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV1;
      gp_Vec aV2;
      anAdaptorBezierRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
    });
  const BenchSummary anAdaptorBspD2Rep =
    runBench("Adaptor_BSpline_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV1;
      gp_Vec aV2;
      anAdaptorBSplineRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
    });
  printRow(aDirectAhtD2, aCalls);
  printRow(aDirectBezD2, aCalls);
  printRow(aDirectBspD2, aCalls);
  printRow(anAdaptorBezD2, aCalls);
  printRow(anAdaptorBspD2, aCalls);
  printRow(anAdaptorBezD2Rep, aCalls);
  printRow(anAdaptorBspD2Rep, aCalls);

  printHeader("D3");
  const BenchSummary aDirectAhtD3 = runBench("Direct_AHT_D3", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    gp_Vec aV3;
    anAhtCurve->D3(theU, aP, aV1, aV2, aV3);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
           + aV3.X() + aV3.Y() + aV3.Z();
  });
  const BenchSummary aDirectBezD3 = runBench("Direct_Bezier_D3", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    gp_Vec aV3;
    aBezierCurve->D3(theU, aP, aV1, aV2, aV3);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
           + aV3.X() + aV3.Y() + aV3.Z();
  });
  const BenchSummary aDirectBspD3 = runBench("Direct_BSpline_D3", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    gp_Vec aV3;
    aBSplineCurve->D3(theU, aP, aV1, aV2, aV3);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
           + aV3.X() + aV3.Y() + aV3.Z();
  });
  const BenchSummary anAdaptorBezD3 = runBench("Adaptor_Bezier_D3", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    gp_Vec aV3;
    anAdaptorBezier.D3(theU, aP, aV1, aV2, aV3);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
           + aV3.X() + aV3.Y() + aV3.Z();
  });
  const BenchSummary anAdaptorBspD3 = runBench("Adaptor_BSpline_D3", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    gp_Vec aV3;
    anAdaptorBSpline.D3(theU, aP, aV1, aV2, aV3);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
           + aV3.X() + aV3.Y() + aV3.Z();
  });
  const BenchSummary anAdaptorBezD3Rep =
    runBench("Adaptor_Bezier_D3_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV1;
      gp_Vec aV2;
      gp_Vec aV3;
      anAdaptorBezierRep.D3(theU, aP, aV1, aV2, aV3);
      return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
             + aV3.X() + aV3.Y() + aV3.Z();
    });
  const BenchSummary anAdaptorBspD3Rep =
    runBench("Adaptor_BSpline_D3_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV1;
      gp_Vec aV2;
      gp_Vec aV3;
      anAdaptorBSplineRep.D3(theU, aP, aV1, aV2, aV3);
      return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z()
             + aV3.X() + aV3.Y() + aV3.Z();
    });
  printRow(aDirectAhtD3, aCalls);
  printRow(aDirectBezD3, aCalls);
  printRow(aDirectBspD3, aCalls);
  printRow(anAdaptorBezD3, aCalls);
  printRow(anAdaptorBspD3, aCalls);
  printRow(anAdaptorBezD3Rep, aCalls);
  printRow(anAdaptorBspD3Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectAhtD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD2.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD3.Sink));
}
