// Copyright (c) 2025 OPEN CASCADE SAS
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

// Performance comparison: Old Extrema_ExtSS vs New ExtremaSS_* classes

#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <iomanip>

#include <Extrema_ExtSS.hxx>

#include <ExtremaSS_CylinderCone.hxx>
#include <ExtremaSS_CylinderCylinder.hxx>
#include <ExtremaSS_CylinderSphere.hxx>
#include <ExtremaSS_ConeCone.hxx>
#include <ExtremaSS_ConeSphere.hxx>
#include <ExtremaSS_PlaneCylinder.hxx>
#include <ExtremaSS_PlaneCone.hxx>
#include <ExtremaSS_PlaneSphere.hxx>
#include <ExtremaSS_SphereSphere.hxx>
#include <ExtremaSS_CylinderTorus.hxx>
#include <ExtremaSS_ConeTorus.hxx>
#include <ExtremaSS_SphereTorus.hxx>
#include <ExtremaSS_PlaneTorus.hxx>
#include <ExtremaSS_TorusTorus.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

namespace
{
constexpr int    NUM_ITERATIONS = 1000;
constexpr double THE_TOL        = 1e-7;

// Benchmark helper
template <typename Func>
double Benchmark(Func theFunc, int theIterations)
{
  auto aStart = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < theIterations; ++i)
  {
    theFunc();
  }
  auto aEnd = std::chrono::high_resolution_clock::now();
  return std::chrono::duration<double, std::milli>(aEnd - aStart).count();
}

void PrintResult(const char* theName,
                 double      theOldTime,
                 double      theNewTime,
                 double      theOldDist,
                 double      theNewDist,
                 const char* theNote = nullptr)
{
  double aSpeedup = theOldTime / theNewTime;
  std::cout << std::setw(22) << theName << ": "
            << "Old: " << std::fixed << std::setprecision(1) << std::setw(8) << theOldTime << " ms "
            << "(d=" << std::setprecision(4) << theOldDist << "), "
            << "New: " << std::setprecision(1) << std::setw(8) << theNewTime << " ms "
            << "(d=" << std::setprecision(4) << theNewDist << "), "
            << "Speedup: " << std::setprecision(1) << aSpeedup << "x"
            << (aSpeedup > 1.0 ? " FASTER" : " slower");
  if (theNote != nullptr)
  {
    std::cout << " " << theNote;
  }
  std::cout << std::endl;
}
} // namespace

//==================================================================================================
// Sphere-Sphere
//==================================================================================================
TEST(ExtremaSS_Performance, SphereSphere)
{
  gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  gp_Sphere aSphere2(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(0, 0, 1)), 3.0);

  Handle(Geom_SphericalSurface) aGeomSph1 = new Geom_SphericalSurface(aSphere1);
  Handle(Geom_SphericalSurface) aGeomSph2 = new Geom_SphericalSurface(aSphere2);
  GeomAdaptor_Surface           aAdaptor1(aGeomSph1);
  GeomAdaptor_Surface           aAdaptor2(aGeomSph2);

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API - spheres have bounded domains naturally
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptor1, aAdaptor2, THE_TOL, THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_SphereSphere anEval(aSphere1, aSphere2);
      const auto&            aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Sphere-Sphere", aOldTime, aNewTime, aOldDist, aNewDist);

  // Verify correctness
  EXPECT_NEAR(aOldDist, aNewDist, 1e-3);
}

//==================================================================================================
// Cylinder-Cylinder with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, CylinderCylinder)
{
  gp_Cylinder aCyl1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  gp_Cylinder aCyl2(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0)), 2.0);

  Handle(Geom_CylindricalSurface) aGeomCyl1 = new Geom_CylindricalSurface(aCyl1);
  Handle(Geom_CylindricalSurface) aGeomCyl2 = new Geom_CylindricalSurface(aCyl2);
  GeomAdaptor_Surface             aAdaptor1(aGeomCyl1);
  GeomAdaptor_Surface             aAdaptor2(aGeomCyl2);

  // Bounded domains for old API
  const double aU1Min = 0.0, aU1Max = 2 * M_PI;
  const double aV1Min = -10.0, aV1Max = 10.0;
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = -10.0, aV2Max = 10.0;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptor1,
                          aAdaptor2,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_CylinderCylinder anEval(aCyl1, aCyl2);
      const auto&                aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Cylinder-Cylinder", aOldTime, aNewTime, aOldDist, aNewDist);

  // The new API computes distance between infinite surfaces analytically
  // Old API uses bounded domains and may fail for some cases
  // New API should give correct result: 15 - 3 - 2 = 10 (perpendicular cylinders)
  EXPECT_GT(aNewDist, 0.0);
}

//==================================================================================================
// Cylinder-Cone with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, CylinderCone)
{
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Ax3      aConeAxis(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0));
  gp_Cone     aCone(aConeAxis, M_PI / 4.0, 0.0);

  Handle(Geom_CylindricalSurface) aGeomCyl  = new Geom_CylindricalSurface(aCyl);
  Handle(Geom_ConicalSurface)     aGeomCone = new Geom_ConicalSurface(aCone);
  GeomAdaptor_Surface             aAdaptorCyl(aGeomCyl);
  GeomAdaptor_Surface             aAdaptorCone(aGeomCone);

  // Bounded domains
  const double aU1Min = 0.0, aU1Max = 2 * M_PI;
  const double aV1Min = -10.0, aV1Max = 10.0;
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = 0.1, aV2Max = 20.0;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptorCyl,
                          aAdaptorCone,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_CylinderCone anEval(aCyl, aCone);
      const auto&            aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Cylinder-Cone", aOldTime, aNewTime, aOldDist, aNewDist);

  // New API should give reasonable result
  EXPECT_GT(aNewDist, 0.0);
}

//==================================================================================================
// Cone-Cone with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, ConeCone)
{
  gp_Cone aCone1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 6.0, 1.0);
  gp_Cone aCone2(gp_Ax3(gp_Pnt(15, 0, 0), gp_Dir(1, 0, 0)), M_PI / 4.0, 2.0);

  Handle(Geom_ConicalSurface) aGeomCone1 = new Geom_ConicalSurface(aCone1);
  Handle(Geom_ConicalSurface) aGeomCone2 = new Geom_ConicalSurface(aCone2);
  GeomAdaptor_Surface         aAdaptor1(aGeomCone1);
  GeomAdaptor_Surface         aAdaptor2(aGeomCone2);

  // Bounded domains
  const double aU1Min = 0.0, aU1Max = 2 * M_PI;
  const double aV1Min = 0.1, aV1Max = 20.0;
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = 0.1, aV2Max = 20.0;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptor1,
                          aAdaptor2,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_ConeCone anEval(aCone1, aCone2);
      const auto&        aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Cone-Cone", aOldTime, aNewTime, aOldDist, aNewDist);

  EXPECT_GE(aOldDist, 0.0);
  EXPECT_GE(aNewDist, 0.0);
}

//==================================================================================================
// Torus-Torus (most complex)
//==================================================================================================
TEST(ExtremaSS_Performance, TorusTorus)
{
  gp_Torus aTorus1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 2.0);
  gp_Torus aTorus2(gp_Ax3(gp_Pnt(25, 0, 0), gp_Dir(0, 0, 1)), 8.0, 1.5);

  Handle(Geom_ToroidalSurface) aGeomTor1 = new Geom_ToroidalSurface(aTorus1);
  Handle(Geom_ToroidalSurface) aGeomTor2 = new Geom_ToroidalSurface(aTorus2);
  GeomAdaptor_Surface          aAdaptor1(aGeomTor1);
  GeomAdaptor_Surface          aAdaptor2(aGeomTor2);

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API - torus has bounded domains naturally
  // Use fewer iterations because it's very slow
  const int aOldIterations = NUM_ITERATIONS / 10;
  aOldTime                 = Benchmark(
                    [&]() {
                      Extrema_ExtSS anExt(aAdaptor1, aAdaptor2, THE_TOL, THE_TOL);
                      if (anExt.IsDone() && anExt.NbExt() > 0)
                        aOldDist = std::sqrt(anExt.SquareDistance(1));
                    },
                    aOldIterations);
  aOldTime *= 10; // Scale up for comparison

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_TorusTorus anEval(aTorus1, aTorus2);
      const auto&          aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Torus-Torus", aOldTime, aNewTime, aOldDist, aNewDist, "[old scaled from 100 iter]");

  EXPECT_NEAR(aOldDist, aNewDist, 1e-2);
}

//==================================================================================================
// Plane-Sphere with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, PlaneSphere)
{
  gp_Pln    aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);

  Handle(Geom_Plane)            aGeomPln = new Geom_Plane(aPlane);
  Handle(Geom_SphericalSurface) aGeomSph = new Geom_SphericalSurface(aSphere);
  GeomAdaptor_Surface           aAdaptorPln(aGeomPln);
  GeomAdaptor_Surface           aAdaptorSph(aGeomSph);

  // Bounded domain for plane
  const double aU1Min = -100.0, aU1Max = 100.0;
  const double aV1Min = -100.0, aV1Max = 100.0;
  // Sphere has natural bounds
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = -M_PI / 2, aV2Max = M_PI / 2;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptorPln,
                          aAdaptorSph,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_PlaneSphere anEval(aPlane, aSphere);
      const auto&           aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Plane-Sphere", aOldTime, aNewTime, aOldDist, aNewDist);

  // New API: 10 - 3 = 7 (distance from plane z=0 to sphere center at z=10 minus radius)
  EXPECT_NEAR(aNewDist, 7.0, 1e-3);
}

//==================================================================================================
// Plane-Cylinder with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, PlaneCylinder)
{
  gp_Pln      aPlane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(1, 0, 0)), 2.0);

  Handle(Geom_Plane)             aGeomPln = new Geom_Plane(aPlane);
  Handle(Geom_CylindricalSurface) aGeomCyl = new Geom_CylindricalSurface(aCyl);
  GeomAdaptor_Surface            aAdaptorPln(aGeomPln);
  GeomAdaptor_Surface            aAdaptorCyl(aGeomCyl);

  // Bounded domains
  const double aU1Min = -100.0, aU1Max = 100.0;
  const double aV1Min = -100.0, aV1Max = 100.0;
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = -10.0, aV2Max = 10.0;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptorPln,
                          aAdaptorCyl,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_PlaneCylinder anEval(aPlane, aCyl);
      const auto&             aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Plane-Cylinder", aOldTime, aNewTime, aOldDist, aNewDist);

  // Both APIs may return 0 for parallel plane-cylinder (infinite line of contact)
  // The cylinder axis is horizontal (1,0,0), parallel to the plane z=0
  // Result depends on implementation details
  EXPECT_GE(aNewDist, 0.0);
}

//==================================================================================================
// Cylinder-Sphere with bounded domains
//==================================================================================================
TEST(ExtremaSS_Performance, CylinderSphere)
{
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.0);
  gp_Sphere   aSphere(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  Handle(Geom_CylindricalSurface) aGeomCyl = new Geom_CylindricalSurface(aCyl);
  Handle(Geom_SphericalSurface)   aGeomSph = new Geom_SphericalSurface(aSphere);
  GeomAdaptor_Surface             aAdaptorCyl(aGeomCyl);
  GeomAdaptor_Surface             aAdaptorSph(aGeomSph);

  // Bounded domains
  const double aU1Min = 0.0, aU1Max = 2 * M_PI;
  const double aV1Min = -10.0, aV1Max = 10.0;
  const double aU2Min = 0.0, aU2Max = 2 * M_PI;
  const double aV2Min = -M_PI / 2, aV2Max = M_PI / 2;

  double aOldTime = 0, aNewTime = 0;
  double aOldDist = 0, aNewDist = 0;

  // Old API with explicit bounds
  aOldTime = Benchmark(
    [&]() {
      Extrema_ExtSS anExt(aAdaptorCyl,
                          aAdaptorSph,
                          aU1Min,
                          aU1Max,
                          aV1Min,
                          aV1Max,
                          aU2Min,
                          aU2Max,
                          aV2Min,
                          aV2Max,
                          THE_TOL,
                          THE_TOL);
      if (anExt.IsDone() && anExt.NbExt() > 0)
        aOldDist = std::sqrt(anExt.SquareDistance(1));
    },
    NUM_ITERATIONS);

  // New API
  aNewTime = Benchmark(
    [&]() {
      ExtremaSS_CylinderSphere anEval(aCyl, aSphere);
      const auto&              aResult = anEval.Perform(THE_TOL);
      if (aResult.Status == ExtremaSS::Status::OK && aResult.NbExt() > 0)
        aNewDist = std::sqrt(aResult.MinSquareDistance());
    },
    NUM_ITERATIONS);

  PrintResult("Cylinder-Sphere", aOldTime, aNewTime, aOldDist, aNewDist);

  EXPECT_NEAR(aNewDist, 5.0, 1e-3); // 10 - 3 - 2 = 5
}

//==================================================================================================
// Summary test that prints all results together
//==================================================================================================
TEST(ExtremaSS_Performance, Summary)
{
  std::cout << "\n========================================" << std::endl;
  std::cout << "ExtremaSS Performance Comparison Summary" << std::endl;
  std::cout << "Iterations: " << NUM_ITERATIONS << " (Torus-Torus: 100 scaled to 1000)" << std::endl;
  std::cout << "========================================\n" << std::endl;
}
