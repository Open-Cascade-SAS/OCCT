// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// Temporary performance check test for Eval-representation paths.
// The test is disabled by default; run with --gtest_also_run_disabled_tests.

#include <GeomAdaptor_Surface.hxx>
#include <GeomEval_RepSurfaceDesc.hxx>
#include <GeomEval_TBezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <Geom_SphericalSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Ax3.hxx>
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
using PerfClockTBezierSurface = std::chrono::steady_clock;

struct TBezierSurfaceBenchSummary
{
  const char* Name = "";
  double      Ms   = 0.0;
  double      Sink = 0.0;
};

int tbezierSurfaceEnvIntOrDefault(const char* theName, const int theDefault, const int theMin)
{
  const char* aRawVal = std::getenv(theName);
  if (aRawVal == nullptr)
  {
    return theDefault;
  }
  return std::max(theMin, std::atoi(aRawVal));
}

template <typename FuncT>
TBezierSurfaceBenchSummary runTBezierSurfaceBench(const char*                      theName,
                                                  const int                        theRepeats,
                                                  const NCollection_Array1<gp_XY>& theParams,
                                                  FuncT&&                          theFunc)
{
  volatile double aSink = 0.0;
  const auto      aBeg  = PerfClockTBezierSurface::now();
  for (int aRep = 0; aRep < theRepeats; ++aRep)
  {
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aSink += theFunc(theParams.Value(i).X(), theParams.Value(i).Y());
    }
  }

  const auto aEnd = PerfClockTBezierSurface::now();
  const std::chrono::duration<double, std::milli> aDur = aEnd - aBeg;
  return {theName, aDur.count(), aSink};
}

void printTBezierSurfaceBenchRow(const TBezierSurfaceBenchSummary& theResult, const int theCalls)
{
  const double aMcallsPerSec = double(theCalls) / (theResult.Ms * 1000.0);
  std::cout << std::left << std::setw(36) << theResult.Name
            << std::right << std::setw(12) << std::fixed << std::setprecision(3) << theResult.Ms
            << std::setw(14) << std::setprecision(3) << aMcallsPerSec
            << std::setw(16) << std::setprecision(6) << theResult.Sink << '\n';
}

void printTBezierSurfaceBenchHeader(const char* theTitle)
{
  std::cout << "\n=== " << theTitle << " ===\n";
  std::cout << std::left << std::setw(36) << "Path"
            << std::right << std::setw(12) << "ms"
            << std::setw(14) << "MCalls/s"
            << std::setw(16) << "sink" << '\n';
}

double tbezierSurfaceVecDiff(const gp_Vec& theV1, const gp_Vec& theV2)
{
  return (theV1 - theV2).Magnitude();
}

} // namespace

TEST(GeomEval_TBezierSurfacePerfTest, DISABLED_D0ToD2_EquivalentSphereAndAdaptorPaths)
{
  const int aRepeats   = tbezierSurfaceEnvIntOrDefault("OCCT_GEOMEVAL_PERF_REPEATS", 300, 1);
  const int aUSamples  = tbezierSurfaceEnvIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES_U", 48, 4);
  const int aVSamples  = tbezierSurfaceEnvIntOrDefault("OCCT_GEOMEVAL_PERF_SAMPLES_V", 48, 4);
  const int aNbSamples = aUSamples * aVSamples;

  NCollection_Array2<gp_Pnt> aTBezPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aTBezPoles.SetValue(i, j, gp_Pnt(0.0, 0.0, 0.0));
    }
  }
  aTBezPoles.SetValue(1, 2, gp_Pnt(0.0, 0.0, 1.0));
  aTBezPoles.SetValue(2, 3, gp_Pnt(0.0, 1.0, 0.0));
  aTBezPoles.SetValue(3, 3, gp_Pnt(1.0, 0.0, 0.0));
  const Handle(GeomEval_TBezierSurface) aTBezierSurface =
    new GeomEval_TBezierSurface(aTBezPoles, 1.0, 1.0);

  const Handle(Geom_SphericalSurface) aSphereSurface = new Geom_SphericalSurface(gp_Ax3(), 1.0);
  const Handle(Geom_BSplineSurface) aBSplineSurface = GeomConvert::SurfaceToBSplineSurface(aSphereSurface);

  const Handle(Geom_BSplineSurface) aBSplineWithRep =
    Handle(Geom_BSplineSurface)::DownCast(aBSplineSurface->Copy());
  ASSERT_FALSE(aBSplineWithRep.IsNull());

  const Handle(GeomEval_RepSurfaceDesc::Full) aBspDesc = new GeomEval_RepSurfaceDesc::Full();
  aBspDesc->Representation = aTBezierSurface;
  aBSplineWithRep->SetEvalRepresentation(aBspDesc);

  GeomAdaptor_Surface anAdaptorSphere(aSphereSurface);
  GeomAdaptor_Surface anAdaptorBSpline(aBSplineSurface);
  GeomAdaptor_Surface anAdaptorBSplineRep(aBSplineWithRep);

  NCollection_Array1<gp_XY> aParams(1, aNbSamples);
  int anIdx = aParams.Lower();
  for (int iu = 0; iu < aUSamples; ++iu)
  {
    const double aU = (aUSamples == 1) ? 0.0 : double(iu) / double(aUSamples - 1);
    for (int iv = 0; iv < aVSamples; ++iv)
    {
      const double aV = (aVSamples == 1) ? 0.0 : double(iv) / double(aVSamples - 1);
      aParams.SetValue(anIdx++, gp_XY(aU * M_PI, aV * M_PI));
    }
  }

  double aMaxD0Sphere = 0.0;
  double aMaxD0Bsp    = 0.0;
  double aMaxD1Sphere = 0.0;
  double aMaxD1Bsp    = 0.0;
  for (int i = aParams.Lower(); i <= aParams.Upper(); ++i)
  {
    const double aU = aParams.Value(i).X();
    const double aV = aParams.Value(i).Y();

    gp_Pnt aPTBez;
    gp_Pnt aPSphere;
    gp_Pnt aPBsp;
    gp_Vec aTBezDU;
    gp_Vec aTBezDV;
    gp_Vec aSphereDU;
    gp_Vec aSphereDV;
    gp_Vec aBspDU;
    gp_Vec aBspDV;

    aTBezierSurface->D0(aU, aV, aPTBez);
    aSphereSurface->D0(aU, aV, aPSphere);
    aBSplineSurface->D0(aU, aV, aPBsp);

    aTBezierSurface->D1(aU, aV, aPTBez, aTBezDU, aTBezDV);
    aSphereSurface->D1(aU, aV, aPSphere, aSphereDU, aSphereDV);
    aBSplineSurface->D1(aU, aV, aPBsp, aBspDU, aBspDV);

    aMaxD0Sphere = std::max(aMaxD0Sphere, aPTBez.Distance(aPSphere));
    aMaxD0Bsp    = std::max(aMaxD0Bsp, aPTBez.Distance(aPBsp));
    aMaxD1Sphere = std::max(aMaxD1Sphere, tbezierSurfaceVecDiff(aTBezDU, aSphereDU)
                                              + tbezierSurfaceVecDiff(aTBezDV, aSphereDV));
    aMaxD1Bsp    = std::max(aMaxD1Bsp, tbezierSurfaceVecDiff(aTBezDU, aBspDU)
                                           + tbezierSurfaceVecDiff(aTBezDV, aBspDV));
  }

  EXPECT_LE(aMaxD0Sphere, 1e-12);
  EXPECT_LE(aMaxD1Sphere, 1e-10);
  EXPECT_TRUE(std::isfinite(aMaxD0Bsp));
  EXPECT_TRUE(std::isfinite(aMaxD1Bsp));

  const int aCalls = aRepeats * aNbSamples;

  std::cout << "GeomEval TBezier surface temporary perf check\n";
  std::cout << "Repeats: " << aRepeats << ", UxV Samples: " << aUSamples << 'x' << aVSamples
            << ", Total calls/path: " << aCalls << '\n';
  std::cout << "Equivalence check (TBezier surface vs Sphere/BSpline)\n";
  std::cout << "  max |D0(TBezier)-D0(Sphere)|  = " << std::setprecision(12) << aMaxD0Sphere << '\n';
  std::cout << "  max |D0(TBezier)-D0(BSpline)| = " << std::setprecision(12) << aMaxD0Bsp << '\n';
  std::cout << "  max |D1(TBezier)-D1(Sphere)|  = " << std::setprecision(12) << aMaxD1Sphere << '\n';
  std::cout << "  max |D1(TBezier)-D1(BSpline)| = " << std::setprecision(12) << aMaxD1Bsp << '\n';

  printTBezierSurfaceBenchHeader("D0");
  const TBezierSurfaceBenchSummary aDirectTBezD0 = runTBezierSurfaceBench("Direct_TBezierSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    aTBezierSurface->D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierSurfaceBenchSummary aDirectSphereD0 = runTBezierSurfaceBench("Direct_SphereSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    aSphereSurface->D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierSurfaceBenchSummary aDirectBspD0 = runTBezierSurfaceBench("Direct_BSplineSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    aBSplineSurface->D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorSphereD0 = runTBezierSurfaceBench("Adaptor_SphereSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    anAdaptorSphere.D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD0 = runTBezierSurfaceBench("Adaptor_BSplineSurface_D0", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    anAdaptorBSpline.D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD0Rep = runTBezierSurfaceBench("Adaptor_BSplineSurface_D0_WithEvalRep", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    anAdaptorBSplineRep.D0(theU, theV, aP);
    return aP.X() + aP.Y() + aP.Z();
  });
  printTBezierSurfaceBenchRow(aDirectTBezD0, aCalls);
  printTBezierSurfaceBenchRow(aDirectSphereD0, aCalls);
  printTBezierSurfaceBenchRow(aDirectBspD0, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorSphereD0, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD0, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD0Rep, aCalls);

  printTBezierSurfaceBenchHeader("D1");
  const TBezierSurfaceBenchSummary aDirectTBezD1 = runTBezierSurfaceBench("Direct_TBezierSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    aTBezierSurface->D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  const TBezierSurfaceBenchSummary aDirectSphereD1 = runTBezierSurfaceBench("Direct_SphereSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    aSphereSurface->D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  const TBezierSurfaceBenchSummary aDirectBspD1 = runTBezierSurfaceBench("Direct_BSplineSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    aBSplineSurface->D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorSphereD1 = runTBezierSurfaceBench("Adaptor_SphereSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    anAdaptorSphere.D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD1 = runTBezierSurfaceBench("Adaptor_BSplineSurface_D1", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    anAdaptorBSpline.D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD1Rep = runTBezierSurfaceBench("Adaptor_BSplineSurface_D1_WithEvalRep", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    anAdaptorBSplineRep.D1(theU, theV, aP, aDU, aDV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z();
  });
  printTBezierSurfaceBenchRow(aDirectTBezD1, aCalls);
  printTBezierSurfaceBenchRow(aDirectSphereD1, aCalls);
  printTBezierSurfaceBenchRow(aDirectBspD1, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorSphereD1, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD1, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD1Rep, aCalls);

  printTBezierSurfaceBenchHeader("D2");
  const TBezierSurfaceBenchSummary aDirectTBezD2 = runTBezierSurfaceBench("Direct_TBezierSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    aTBezierSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  const TBezierSurfaceBenchSummary aDirectSphereD2 = runTBezierSurfaceBench("Direct_SphereSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    aSphereSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  const TBezierSurfaceBenchSummary aDirectBspD2 = runTBezierSurfaceBench("Direct_BSplineSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    aBSplineSurface->D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorSphereD2 = runTBezierSurfaceBench("Adaptor_SphereSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    anAdaptorSphere.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD2 = runTBezierSurfaceBench("Adaptor_BSplineSurface_D2", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    anAdaptorBSpline.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  const TBezierSurfaceBenchSummary anAdaptorBspD2Rep = runTBezierSurfaceBench("Adaptor_BSplineSurface_D2_WithEvalRep", aRepeats, aParams, [&](double theU, double theV) {
    gp_Pnt aP;
    gp_Vec aDU;
    gp_Vec aDV;
    gp_Vec aD2U;
    gp_Vec aD2V;
    gp_Vec aD2UV;
    anAdaptorBSplineRep.D2(theU, theV, aP, aDU, aDV, aD2U, aD2V, aD2UV);
    return aP.X() + aP.Y() + aP.Z() + aDU.X() + aDU.Y() + aDU.Z() + aDV.X() + aDV.Y() + aDV.Z()
         + aD2U.X() + aD2U.Y() + aD2U.Z() + aD2V.X() + aD2V.Y() + aD2V.Z() + aD2UV.X() + aD2UV.Y() + aD2UV.Z();
  });
  printTBezierSurfaceBenchRow(aDirectTBezD2, aCalls);
  printTBezierSurfaceBenchRow(aDirectSphereD2, aCalls);
  printTBezierSurfaceBenchRow(aDirectBspD2, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorSphereD2, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD2, aCalls);
  printTBezierSurfaceBenchRow(anAdaptorBspD2Rep, aCalls);

  EXPECT_TRUE(std::isfinite(aDirectTBezD0.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD1.Sink));
  EXPECT_TRUE(std::isfinite(aDirectTBezD2.Sink));
}
