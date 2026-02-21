// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dEval_RepCurveDesc.hxx>
#include <Geom2dEval_TBezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dConvert.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Ax2d.hxx>
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
using PerfClockGeom2dTBezier = std::chrono::steady_clock;

struct Geom2dTBezierBenchSummary
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
Geom2dTBezierBenchSummary runGeom2dBench(const char*                        theName,
                                         const int                          theRepeats,
                                         const NCollection_Array1<double>& theParams,
                                         FuncT&&                            theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = PerfClockGeom2dTBezier::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i));
    }
  }
  const auto aEnd = PerfClockGeom2dTBezier::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printGeom2dBenchHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(36) << "Path"
            << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s"
            << std::setw(16) << "sink" << '\n';
}

void printGeom2dBenchRow(const Geom2dTBezierBenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(36) << theResult.Name
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << theResult.Ms
            << std::setw(14) << std::setprecision(3) << aMcallsPerSec
            << std::setw(16) << std::setprecision(6) << theResult.Sink << '\n';
}

double vec2dDiff(const gp_Vec2d& theV1, const gp_Vec2d& theV2)
{
  return (theV1 - theV2).Magnitude();
}
} // namespace

TEST(Geom2dEval_TBezierCurvePerfTest, DISABLED_D0ToD2_EquivalentCircleAndAdaptorPaths)
{
  const int aRepeats =
    geom2dPerfEnvIntOrDefault("OCCT_GEOM2DEVAL_PERF_REPEATS", 2000, 1);
  const int aNbSamples =
    geom2dPerfEnvIntOrDefault("OCCT_GEOM2DEVAL_PERF_SAMPLES", 2048, 8);

  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(0.0, 1.0));
  aPoles.SetValue(3, gp_Pnt2d(1.0, 0.0));
  const Handle(Geom2dEval_TBezierCurve) aTBezierCurve =
    new Geom2dEval_TBezierCurve(aPoles, 1.0);

  const Handle(Geom2d_Circle) aCircleCurve = new Geom2d_Circle(gp_Ax2d(), 1.0);
  const Handle(Geom2d_BSplineCurve) aBSplineCurve =
    Geom2dConvert::CurveToBSplineCurve(aCircleCurve);

  const Handle(Geom2d_BSplineCurve) aBSplineWithRep =
    Handle(Geom2d_BSplineCurve)::DownCast(aBSplineCurve->Copy());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(Geom2dEval_RepCurveDesc::Full) aBspDesc = new Geom2dEval_RepCurveDesc::Full();
  aBspDesc->Representation = aTBezierCurve;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  Geom2dAdaptor_Curve anAdaptorCircle(aCircleCurve);
  Geom2dAdaptor_Curve anAdaptorBSpline(aBSplineCurve);
  Geom2dAdaptor_Curve anAdaptorBSplineRep(aBSplineWithRep);

  NCollection_Array1<double> aParams(1, aNbSamples);
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aT = double(i - aParams.Lower()) / double(aParams.Size() - 1);
    aParams.SetValue(i, aT * M_PI);
  }

  double aMaxD0Circle = 0.0;
  double aMaxD0Bsp    = 0.0;
  double aMaxD1Circle = 0.0;
  double aMaxD1Bsp    = 0.0;
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aU = aParams.Value(i);
    gp_Pnt2d     aPTBez;
    gp_Pnt2d     aPCircle;
    gp_Pnt2d     aPBsp;
    gp_Vec2d     aVTBez;
    gp_Vec2d     aVCircle;
    gp_Vec2d     aVBsp;

    aTBezierCurve->D0(aU, aPTBez);
    aCircleCurve->D0(aU, aPCircle);
    aBSplineCurve->D0(aU, aPBsp);

    aTBezierCurve->D1(aU, aPTBez, aVTBez);
    aCircleCurve->D1(aU, aPCircle, aVCircle);
    aBSplineCurve->D1(aU, aPBsp, aVBsp);

    aMaxD0Circle = std::max(aMaxD0Circle, aPTBez.Distance(aPCircle));
    aMaxD0Bsp    = std::max(aMaxD0Bsp, aPTBez.Distance(aPBsp));
    aMaxD1Circle = std::max(aMaxD1Circle, vec2dDiff(aVTBez, aVCircle));
    aMaxD1Bsp    = std::max(aMaxD1Bsp, vec2dDiff(aVTBez, aVBsp));
  }

  EXPECT_LE(aMaxD0Circle, 1e-12);
  EXPECT_LE(aMaxD1Circle, 1e-11);
  EXPECT_TRUE(std::isfinite(aMaxD0Bsp));
  EXPECT_TRUE(std::isfinite(aMaxD1Bsp));

  const int aCalls = aRepeats * aNbSamples;
  std::cout << "Geom2dEval TBezier temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", Samples: " << aNbSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (TBezier vs Circle/BSpline)\n";
  std::cout << "  max |D0(TBezier)-D0(Circle)|  = " << std::setprecision(12) << aMaxD0Circle
            << '\n';
  std::cout << "  max |D0(TBezier)-D0(BSpline)| = " << std::setprecision(12) << aMaxD0Bsp
            << '\n';
  std::cout << "  max |D1(TBezier)-D1(Circle)|  = " << std::setprecision(12) << aMaxD1Circle
            << '\n';
  std::cout << "  max |D1(TBezier)-D1(BSpline)| = " << std::setprecision(12) << aMaxD1Bsp
            << '\n';

  printGeom2dBenchHeader("D0");
  const Geom2dTBezierBenchSummary aDirectTBezD0 =
    runGeom2dBench("Direct_TBezier_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      aTBezierCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dTBezierBenchSummary aDirectCircleD0 =
    runGeom2dBench("Direct_Circle_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      aCircleCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dTBezierBenchSummary aDirectBspD0 =
    runGeom2dBench("Direct_BSpline_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      aBSplineCurve->D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorCircleD0 =
    runGeom2dBench("Adaptor_Circle_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorCircle.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD0 =
    runGeom2dBench("Adaptor_BSpline_D0", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBSpline.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD0Rep =
    runGeom2dBench("Adaptor_BSpline_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      anAdaptorBSplineRep.D0(theU, aP);
      return aP.X() + aP.Y();
    });
  printGeom2dBenchRow(aDirectTBezD0, aCalls);
  printGeom2dBenchRow(aDirectCircleD0, aCalls);
  printGeom2dBenchRow(aDirectBspD0, aCalls);
  printGeom2dBenchRow(anAdaptorCircleD0, aCalls);
  printGeom2dBenchRow(anAdaptorBspD0, aCalls);
  printGeom2dBenchRow(anAdaptorBspD0Rep, aCalls);

  printGeom2dBenchHeader("D1");
  const Geom2dTBezierBenchSummary aDirectTBezD1 =
    runGeom2dBench("Direct_TBezier_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      aTBezierCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dTBezierBenchSummary aDirectCircleD1 =
    runGeom2dBench("Direct_Circle_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      aCircleCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dTBezierBenchSummary aDirectBspD1 =
    runGeom2dBench("Direct_BSpline_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      aBSplineCurve->D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorCircleD1 =
    runGeom2dBench("Adaptor_Circle_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorCircle.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD1 =
    runGeom2dBench("Adaptor_BSpline_D1", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBSpline.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD1Rep =
    runGeom2dBench("Adaptor_BSpline_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV;
      anAdaptorBSplineRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aV.X() + aV.Y();
    });
  printGeom2dBenchRow(aDirectTBezD1, aCalls);
  printGeom2dBenchRow(aDirectCircleD1, aCalls);
  printGeom2dBenchRow(aDirectBspD1, aCalls);
  printGeom2dBenchRow(anAdaptorCircleD1, aCalls);
  printGeom2dBenchRow(anAdaptorBspD1, aCalls);
  printGeom2dBenchRow(anAdaptorBspD1Rep, aCalls);

  printGeom2dBenchHeader("D2");
  const Geom2dTBezierBenchSummary aDirectTBezD2 =
    runGeom2dBench("Direct_TBezier_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      aTBezierCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dTBezierBenchSummary aDirectCircleD2 =
    runGeom2dBench("Direct_Circle_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      aCircleCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dTBezierBenchSummary aDirectBspD2 =
    runGeom2dBench("Direct_BSpline_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      aBSplineCurve->D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorCircleD2 =
    runGeom2dBench("Adaptor_Circle_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorCircle.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD2 =
    runGeom2dBench("Adaptor_BSpline_D2", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBSpline.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  const Geom2dTBezierBenchSummary anAdaptorBspD2Rep =
    runGeom2dBench("Adaptor_BSpline_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt2d aP;
      gp_Vec2d aV1;
      gp_Vec2d aV2;
      anAdaptorBSplineRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aV1.X() + aV1.Y() + aV2.X() + aV2.Y();
    });
  printGeom2dBenchRow(aDirectTBezD2, aCalls);
  printGeom2dBenchRow(aDirectCircleD2, aCalls);
  printGeom2dBenchRow(aDirectBspD2, aCalls);
  printGeom2dBenchRow(anAdaptorCircleD2, aCalls);
  printGeom2dBenchRow(anAdaptorBspD2, aCalls);
  printGeom2dBenchRow(anAdaptorBspD2Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectTBezD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD2.Sink));
}

