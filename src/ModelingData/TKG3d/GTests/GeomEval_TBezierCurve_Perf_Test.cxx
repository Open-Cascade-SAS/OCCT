// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <GeomEval_RepCurveDesc.hxx>
#include <GeomEval_TBezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Ax2.hxx>
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
using PerfClockTBezierCurve = std::chrono::steady_clock;

struct TBezierCurveBenchSummary
{
  const char* Name = "";
  double      Ms   = 0.0;
  double      Sink = 0.0;
};

int tbezierCurveEnvIntOrDefault(const char* theName, const int theDefault, const int theMin)
{
  const char* aRawVal = std::getenv(theName);
  if (aRawVal == nullptr)
  {
    return theDefault;
  }
  return std::max(theMin, std::atoi(aRawVal));
}

template <typename FuncT>
TBezierCurveBenchSummary runTBezierCurveBench(const char*                        theName,
                                              const int                          theRepeats,
                                              const NCollection_Array1<double>& theParams,
                                              FuncT&&                            theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = PerfClockTBezierCurve::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i));
    }
  }

  const auto aEnd = PerfClockTBezierCurve::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printTBezierCurveBenchRow(const TBezierCurveBenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(34) << theResult.Name
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << theResult.Ms
            << std::setw(14) << std::setprecision(3) << aMcallsPerSec
            << std::setw(16) << std::setprecision(6) << theResult.Sink << '\n';
}

void printTBezierCurveBenchHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(34) << "Path"
            << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s"
            << std::setw(16) << "sink" << '\n';
}

double tbezierCurveVecDiff(const gp_Vec& theV1, const gp_Vec& theV2)
{
  return (theV1 - theV2).Magnitude();
}

} // namespace

TEST(GeomEval_TBezierCurvePerfTest, DISABLED_D0ToD2_EquivalentCircleAndAdaptorPaths)
{
  const int aRepeats   = tbezierCurveEnvIntOrDefault("OCCT_GEOMEVAL_PERF_REPEATS", 1000, 1);
  const int aNbSamples = tbezierCurveEnvIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES", 1024, 8);

  // Exact semicircle on [0, Pi]: C(u) = (cos(u), sin(u), 0).
  NCollection_Array1<gp_Pnt> aTBezPoles(1, 3);
  aTBezPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aTBezPoles.SetValue(2, gp_Pnt(0.0, 1.0, 0.0));
  aTBezPoles.SetValue(3, gp_Pnt(1.0, 0.0, 0.0));
  const Handle(GeomEval_TBezierCurve) aTBezierCurve = new GeomEval_TBezierCurve(aTBezPoles, 1.0);

  const Handle(Geom_Circle) aCircleCurve = new Geom_Circle(gp_Ax2(), 1.0);
  const Handle(Geom_BSplineCurve) aBSplineCurve = GeomConvert::CurveToBSplineCurve(aCircleCurve);

  const Handle(Geom_BSplineCurve) aBSplineWithRep =
    Handle(Geom_BSplineCurve)::DownCast(aBSplineCurve->Copy());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(GeomEval_RepCurveDesc::Full) aBspDesc = new GeomEval_RepCurveDesc::Full();
  aBspDesc->Representation = aTBezierCurve;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  GeomAdaptor_Curve anAdaptorCircle(aCircleCurve);
  GeomAdaptor_Curve anAdaptorBSpline(aBSplineCurve);
  GeomAdaptor_Curve anAdaptorBSplineRep(aBSplineWithRep);

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

    gp_Pnt aPTBez;
    gp_Pnt aPCircle;
    gp_Pnt aPBsp;
    gp_Vec aVTBez;
    gp_Vec aVCircle;
    gp_Vec aVBsp;

    aTBezierCurve->D0(aU, aPTBez);
    aCircleCurve->D0(aU, aPCircle);
    aBSplineCurve->D0(aU, aPBsp);

    aTBezierCurve->D1(aU, aPTBez, aVTBez);
    aCircleCurve->D1(aU, aPCircle, aVCircle);
    aBSplineCurve->D1(aU, aPBsp, aVBsp);

    aMaxD0Circle = std::max(aMaxD0Circle, aPTBez.Distance(aPCircle));
    aMaxD0Bsp    = std::max(aMaxD0Bsp, aPTBez.Distance(aPBsp));
    aMaxD1Circle = std::max(aMaxD1Circle, tbezierCurveVecDiff(aVTBez, aVCircle));
    aMaxD1Bsp    = std::max(aMaxD1Bsp, tbezierCurveVecDiff(aVTBez, aVBsp));
  }

  EXPECT_LE(aMaxD0Circle, 1e-12);
  EXPECT_LE(aMaxD1Circle, 1e-11);
  EXPECT_TRUE(std::isfinite(aMaxD0Bsp));
  EXPECT_TRUE(std::isfinite(aMaxD1Bsp));

  const int aCalls = aRepeats * aNbSamples;

  std::cout << "GeomEval TBezier curve temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", Samples: " << aNbSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (TBezier vs Circle/BSpline)\n";
  std::cout << "  max |D0(TBezier)-D0(Circle)|  = " << std::setprecision(12) << aMaxD0Circle << '\n';
  std::cout << "  max |D0(TBezier)-D0(BSpline)| = " << std::setprecision(12) << aMaxD0Bsp << '\n';
  std::cout << "  max |D1(TBezier)-D1(Circle)|  = " << std::setprecision(12) << aMaxD1Circle << '\n';
  std::cout << "  max |D1(TBezier)-D1(BSpline)| = " << std::setprecision(12) << aMaxD1Bsp << '\n';

  printTBezierCurveBenchHeader("D0");
  const TBezierCurveBenchSummary aDirectTBezD0 = runTBezierCurveBench("Direct_TBezier_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    aTBezierCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierCurveBenchSummary aDirectCircleD0 = runTBezierCurveBench("Direct_Circle_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    aCircleCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierCurveBenchSummary aDirectBspD0 = runTBezierCurveBench("Direct_BSpline_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    aBSplineCurve->D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierCurveBenchSummary anAdaptorCircleD0 = runTBezierCurveBench("Adaptor_Circle_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    anAdaptorCircle.D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD0 = runTBezierCurveBench("Adaptor_BSpline_D0", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    anAdaptorBSpline.D0(theU, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD0Rep =
    runTBezierCurveBench("Adaptor_BSpline_D0_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      anAdaptorBSplineRep.D0(theU, aP);
      return aP.X() + aP.Y() + aP.Z();
    });
  printTBezierCurveBenchRow(aDirectTBezD0, aCalls);
  printTBezierCurveBenchRow(aDirectCircleD0, aCalls);
  printTBezierCurveBenchRow(aDirectBspD0, aCalls);
  printTBezierCurveBenchRow(anAdaptorCircleD0, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD0, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD0Rep, aCalls);

  printTBezierCurveBenchHeader("D1");
  const TBezierCurveBenchSummary aDirectTBezD1 = runTBezierCurveBench("Direct_TBezier_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    aTBezierCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const TBezierCurveBenchSummary aDirectCircleD1 = runTBezierCurveBench("Direct_Circle_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    aCircleCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const TBezierCurveBenchSummary aDirectBspD1 = runTBezierCurveBench("Direct_BSpline_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    aBSplineCurve->D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const TBezierCurveBenchSummary anAdaptorCircleD1 = runTBezierCurveBench("Adaptor_Circle_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    anAdaptorCircle.D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD1 = runTBezierCurveBench("Adaptor_BSpline_D1", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV;
    anAdaptorBSpline.D1(theU, aP, aV);
    return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD1Rep =
    runTBezierCurveBench("Adaptor_BSpline_D1_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV;
      anAdaptorBSplineRep.D1(theU, aP, aV);
      return aP.X() + aP.Y() + aP.Z() + aV.X() + aV.Y() + aV.Z();
    });
  printTBezierCurveBenchRow(aDirectTBezD1, aCalls);
  printTBezierCurveBenchRow(aDirectCircleD1, aCalls);
  printTBezierCurveBenchRow(aDirectBspD1, aCalls);
  printTBezierCurveBenchRow(anAdaptorCircleD1, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD1, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD1Rep, aCalls);

  printTBezierCurveBenchHeader("D2");
  const TBezierCurveBenchSummary aDirectTBezD2 = runTBezierCurveBench("Direct_TBezier_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    aTBezierCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const TBezierCurveBenchSummary aDirectCircleD2 = runTBezierCurveBench("Direct_Circle_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    aCircleCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const TBezierCurveBenchSummary aDirectBspD2 = runTBezierCurveBench("Direct_BSpline_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    aBSplineCurve->D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const TBezierCurveBenchSummary anAdaptorCircleD2 = runTBezierCurveBench("Adaptor_Circle_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    anAdaptorCircle.D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD2 = runTBezierCurveBench("Adaptor_BSpline_D2", aRepeats, aParams, [&](double theU) {
    gp_Pnt aP;
    gp_Vec aV1;
    gp_Vec aV2;
    anAdaptorBSpline.D2(theU, aP, aV1, aV2);
    return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
  });
  const TBezierCurveBenchSummary anAdaptorBspD2Rep =
    runTBezierCurveBench("Adaptor_BSpline_D2_WithEvalRep", aRepeats, aParams, [&](double theU) {
      gp_Pnt aP;
      gp_Vec aV1;
      gp_Vec aV2;
      anAdaptorBSplineRep.D2(theU, aP, aV1, aV2);
      return aP.X() + aP.Y() + aP.Z() + aV1.X() + aV1.Y() + aV1.Z() + aV2.X() + aV2.Y() + aV2.Z();
    });
  printTBezierCurveBenchRow(aDirectTBezD2, aCalls);
  printTBezierCurveBenchRow(aDirectCircleD2, aCalls);
  printTBezierCurveBenchRow(aDirectBspD2, aCalls);
  printTBezierCurveBenchRow(anAdaptorCircleD2, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD2, aCalls);
  printTBezierCurveBenchRow(anAdaptorBspD2Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectTBezD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD2.Sink));
}
