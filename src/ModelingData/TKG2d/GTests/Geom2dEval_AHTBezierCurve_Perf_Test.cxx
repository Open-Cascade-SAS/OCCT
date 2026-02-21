// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dEval_AHTBezierCurve.hxx>
#include <Geom2dEval_RepCurveDesc.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dConvert.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace
{
using PerfClockGeom2dAHT = std::chrono::steady_clock;

struct Geom2dAHTBenchSummary
{
  const char* Name = "";
  double      Ms   = 0.0;
  double      Sink = 0.0;
};

int geom2dPerfEnvIntOrDefault(const char* theName, const int theDefault, const int theMin)
{
  const char* aRawVal = std::getenv(theName);
  if (aRawVal == nullptr)
  {
    return theDefault;
  }
  return std::max(theMin, std::atoi(aRawVal));
}

template <typename FuncT>
Geom2dAHTBenchSummary runGeom2dBench(const char*                        theName,
                                     const int                          theRepeats,
                                     const NCollection_Array1<double>& theParams,
                                     FuncT&&                            theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = PerfClockGeom2dAHT::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i));
    }
  }
  const auto aEnd = PerfClockGeom2dAHT::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printGeom2dBenchHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(40) << "Path"
            << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s"
            << std::setw(16) << "sink" << '\n';
}

void printGeom2dBenchRow(const Geom2dAHTBenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(40) << theResult.Name
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << theResult.Ms
            << std::setw(14) << std::setprecision(3) << aMcallsPerSec
            << std::setw(16) << std::setprecision(6) << theResult.Sink << '\n';
}

double vec2dDiff(const gp_Vec2d& theV1, const gp_Vec2d& theV2)
{
  return (theV1 - theV2).Magnitude();
}
} // namespace

TEST(Geom2dEval_AHTBezierCurvePerfTest, DISABLED_D0ToD2_EquivalentBezierAndAdaptorPaths)
{
  const int aRepeats =
    geom2dPerfEnvIntOrDefault("OCCT_GEOM2DEVAL_PERF_REPEATS", 2000, 1);
  const int aNbSamples =
    geom2dPerfEnvIntOrDefault("OCCT_GEOM2DEVAL_PERF_SAMPLES", 2048, 8);

  // AHT polynomial-only basis with degree 2:
  // C(u) = P0 + P1*u + P2*u^2
  const gp_Pnt2d aP0(0.5, -0.25);
  const gp_Pnt2d aP1(1.25, 2.0);
  const gp_Pnt2d aP2(-0.75, 0.4);

  NCollection_Array1<gp_Pnt2d> anAhtPoles(1, 3);
  anAhtPoles.SetValue(1, aP0);
  anAhtPoles.SetValue(2, aP1);
  anAhtPoles.SetValue(3, aP2);
  const Handle(Geom2dEval_AHTBezierCurve) anAhtCurve =
    new Geom2dEval_AHTBezierCurve(anAhtPoles, 2, 0.0, 0.0);

  // Equivalent quadratic Bezier coefficients:
  // Q0 = P0
  // Q1 = P0 + P1/2
  // Q2 = P0 + P1 + P2
  NCollection_Array1<gp_Pnt2d> aBezPoles(1, 3);
  aBezPoles.SetValue(1, aP0);
  aBezPoles.SetValue(2, gp_Pnt2d(aP0.XY() + 0.5 * aP1.XY()));
  aBezPoles.SetValue(3, gp_Pnt2d(aP0.XY() + aP1.XY() + aP2.XY()));
  const Handle(Geom2d_BezierCurve) aBezierCurve = new Geom2d_BezierCurve(aBezPoles);
  const Handle(Geom2d_BSplineCurve) aBSplineCurve =
    Geom2dConvert::CurveToBSplineCurve(aBezierCurve);

  const Handle(Geom2d_BezierCurve) aBezierWithRep =
    Handle(Geom2d_BezierCurve)::DownCast(aBezierCurve->Copy());
  ASSERT_FALSE(aBezierWithRep.IsNull());
  const Handle(Geom2d_BSplineCurve) aBSplineWithRep =
    Handle(Geom2d_BSplineCurve)::DownCast(aBSplineCurve->Copy());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(Geom2dEval_RepCurveDesc::Full) aBezDesc = new Geom2dEval_RepCurveDesc::Full();
  aBezDesc->Representation = anAhtCurve;
  aBezierWithRep->SetEvalRepresentation(aBezDesc);
  const Handle(Geom2dEval_RepCurveDesc::Full) aBspDesc = new Geom2dEval_RepCurveDesc::Full();
  aBspDesc->Representation = anAhtCurve;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  Geom2dAdaptor_Curve anAdaptorBezier(aBezierCurve);
  Geom2dAdaptor_Curve anAdaptorBSpline(aBSplineCurve);
  Geom2dAdaptor_Curve anAdaptorBezierRep(aBezierWithRep);
  Geom2dAdaptor_Curve anAdaptorBSplineRep(aBSplineWithRep);

  NCollection_Array1<double> aParams(1, aNbSamples);
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aT = double(i - aParams.Lower()) / double(aParams.Size() - 1);
    aParams.SetValue(i, aT);
  }

  double aMaxD0Bez = 0.0;
  double aMaxD0Bsp = 0.0;
  double aMaxD1Bez = 0.0;
  double aMaxD1Bsp = 0.0;
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aU = aParams.Value(i);
    gp_Pnt2d     aPAht;
    gp_Pnt2d     aPBez;
    gp_Pnt2d     aPBsp;
    gp_Vec2d     aVAht;
    gp_Vec2d     aVBez;
    gp_Vec2d     aVBsp;

    anAhtCurve->D0(aU, aPAht);
    aBezierCurve->D0(aU, aPBez);
    aBSplineCurve->D0(aU, aPBsp);

    anAhtCurve->D1(aU, aPAht, aVAht);
    aBezierCurve->D1(aU, aPBez, aVBez);
    aBSplineCurve->D1(aU, aPBsp, aVBsp);

    aMaxD0Bez = std::max(aMaxD0Bez, aPAht.Distance(aPBez));
    aMaxD0Bsp = std::max(aMaxD0Bsp, aPAht.Distance(aPBsp));
    aMaxD1Bez = std::max(aMaxD1Bez, vec2dDiff(aVAht, aVBez));
    aMaxD1Bsp = std::max(aMaxD1Bsp, vec2dDiff(aVAht, aVBsp));
  }

  EXPECT_LE(aMaxD0Bez, 1e-12);
  EXPECT_LE(aMaxD0Bsp, 1e-12);
  EXPECT_LE(aMaxD1Bez, 1e-11);
  EXPECT_LE(aMaxD1Bsp, 1e-11);

  const int aCalls = aRepeats * aNbSamples;
  std::cout << "Geom2dEval AHT temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", Samples: " << aNbSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (AHT vs equivalent Bezier/BSpline)\n";
  std::cout << "  max |D0(AHT)-D0(Bezier)|   = " << std::setprecision(12) << aMaxD0Bez << '\n';
  std::cout << "  max |D0(AHT)-D0(BSpline)|  = " << std::setprecision(12) << aMaxD0Bsp << '\n';
  std::cout << "  max |D1(AHT)-D1(Bezier)|   = " << std::setprecision(12) << aMaxD1Bez << '\n';
  std::cout << "  max |D1(AHT)-D1(BSpline)|  = " << std::setprecision(12) << aMaxD1Bsp << '\n';

  printGeom2dBenchHeader("D0");
  const Geom2dAHTBenchSummary aDirectAhtD0 =
    runGeom2dBench("Direct_AHT_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAhtCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary aDirectBezD0 =
    runGeom2dBench("Direct_Bezier_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      aBezierCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary aDirectBspD0 =
    runGeom2dBench("Direct_BSpline_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      aBSplineCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD0 =
    runGeom2dBench("Adaptor_Bezier_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBezier.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD0 =
    runGeom2dBench("Adaptor_BSpline_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBSpline.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD0Rep =
    runGeom2dBench("Adaptor_Bezier_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBezierRep.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD0Rep =
    runGeom2dBench("Adaptor_BSpline_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBSplineRep.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  printGeom2dBenchRow(aDirectAhtD0, aCalls);
  printGeom2dBenchRow(aDirectBezD0, aCalls);
  printGeom2dBenchRow(aDirectBspD0, aCalls);
  printGeom2dBenchRow(anAdaptorBezD0, aCalls);
  printGeom2dBenchRow(anAdaptorBspD0, aCalls);
  printGeom2dBenchRow(anAdaptorBezD0Rep, aCalls);
  printGeom2dBenchRow(anAdaptorBspD0Rep, aCalls);

  printGeom2dBenchHeader("D1");
  const Geom2dAHTBenchSummary aDirectAhtD1 =
    runGeom2dBench("Direct_AHT_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAhtCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary aDirectBezD1 =
    runGeom2dBench("Direct_Bezier_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      aBezierCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary aDirectBspD1 =
    runGeom2dBench("Direct_BSpline_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      aBSplineCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD1 =
    runGeom2dBench("Adaptor_Bezier_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBezier.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD1 =
    runGeom2dBench("Adaptor_BSpline_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBSpline.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD1Rep =
    runGeom2dBench("Adaptor_Bezier_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBezierRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD1Rep =
    runGeom2dBench("Adaptor_BSpline_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBSplineRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  printGeom2dBenchRow(aDirectAhtD1, aCalls);
  printGeom2dBenchRow(aDirectBezD1, aCalls);
  printGeom2dBenchRow(aDirectBspD1, aCalls);
  printGeom2dBenchRow(anAdaptorBezD1, aCalls);
  printGeom2dBenchRow(anAdaptorBspD1, aCalls);
  printGeom2dBenchRow(anAdaptorBezD1Rep, aCalls);
  printGeom2dBenchRow(anAdaptorBspD1Rep, aCalls);

  printGeom2dBenchHeader("D2");
  const Geom2dAHTBenchSummary aDirectAhtD2 =
    runGeom2dBench("Direct_AHT_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAhtCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary aDirectBezD2 =
    runGeom2dBench("Direct_Bezier_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      aBezierCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary aDirectBspD2 =
    runGeom2dBench("Direct_BSpline_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      aBSplineCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD2 =
    runGeom2dBench("Adaptor_Bezier_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBezier.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD2 =
    runGeom2dBench("Adaptor_BSpline_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBSpline.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBezD2Rep =
    runGeom2dBench("Adaptor_Bezier_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBezierRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dAHTBenchSummary anAdaptorBspD2Rep =
    runGeom2dBench("Adaptor_BSpline_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBSplineRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  printGeom2dBenchRow(aDirectAhtD2, aCalls);
  printGeom2dBenchRow(aDirectBezD2, aCalls);
  printGeom2dBenchRow(aDirectBspD2, aCalls);
  printGeom2dBenchRow(anAdaptorBezD2, aCalls);
  printGeom2dBenchRow(anAdaptorBspD2, aCalls);
  printGeom2dBenchRow(anAdaptorBezD2Rep, aCalls);
  printGeom2dBenchRow(anAdaptorBspD2Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectAhtD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectAhtD2.Sink));
}

