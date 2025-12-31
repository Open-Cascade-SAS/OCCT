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

// Performance comparison: Old Extrema_ExtPS vs New ExtremaPS_Surface

#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include <ExtremaPS_Surface.hxx>
#include <Extrema_ExtPS.hxx>

#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>

const double THE_TOL = 1.0e-6;

// Larger values for analytical surfaces (very fast)
const int NUM_POINTS_ANALYTICAL     = 100000;
const int NUM_ITERATIONS_ANALYTICAL = 100;

// Smaller values for numerical surfaces (slower)
const int NUM_POINTS_NUMERICAL     = 10000;
const int NUM_ITERATIONS_NUMERICAL = 20;

std::vector<gp_Pnt> GenerateTestPoints(int count, double range)
{
  std::vector<gp_Pnt> points;
  points.reserve(count);
  srand(42);
  for (int i = 0; i < count; ++i)
  {
    double x = (double(rand()) / RAND_MAX - 0.5) * 2.0 * range;
    double y = (double(rand()) / RAND_MAX - 0.5) * 2.0 * range;
    double z = (double(rand()) / RAND_MAX - 0.5) * 2.0 * range;
    points.emplace_back(x, y, z);
  }
  return points;
}

void RunPerfTest(const char*                      name,
                 const occ::handle<Geom_Surface>& surf,
                 double                           uMin,
                 double                           uMax,
                 double                           vMin,
                 double                           vMax,
                 int                              numPoints,
                 int                              numIterations)
{
  GeomAdaptor_Surface adaptor(surf);
  auto                points = GenerateTestPoints(numPoints, 20.0);

  const ExtremaPS::Domain2D aDomain(uMin, uMax, vMin, vMax);

  // Warm up - both with Initialize+Perform pattern
  Extrema_ExtPS oldExtWarmup;
  oldExtWarmup.Initialize(adaptor, uMin, uMax, vMin, vMax, THE_TOL, THE_TOL);
  ExtremaPS_Surface newExtWarmup(adaptor, aDomain);
  for (int i = 0; i < 10; ++i)
  {
    (void)oldExtWarmup.Perform(points[i]);
    (void)newExtWarmup.Perform(points[i], THE_TOL);
  }

  // Old implementation - fair comparison with Initialize+Perform pattern
  Extrema_ExtPS oldExt;
  oldExt.Initialize(adaptor, uMin, uMax, vMin, vMax, THE_TOL, THE_TOL);
  auto oldStart = std::chrono::high_resolution_clock::now();
  for (int iter = 0; iter < numIterations; ++iter)
  {
    for (const auto& p : points)
    {
      oldExt.Perform(p);
    }
  }
  auto   oldEnd = std::chrono::high_resolution_clock::now();
  double oldTime =
    std::chrono::duration<double, std::milli>(oldEnd - oldStart).count() / numIterations;

  // New implementation - use Perform (interior only, no boundary) to match old behavior
  // Grid is built eagerly at construction time, so no warm-up needed
  ExtremaPS_Surface newExt(adaptor, aDomain);
  auto              newStart = std::chrono::high_resolution_clock::now();
  for (int iter = 0; iter < numIterations; ++iter)
  {
    for (const auto& p : points)
    {
      (void)newExt.Perform(p, THE_TOL);
    }
  }
  auto   newEnd = std::chrono::high_resolution_clock::now();
  double newTime =
    std::chrono::duration<double, std::milli>(newEnd - newStart).count() / numIterations;

  double speedup = oldTime / newTime;
  std::cout << std::setw(20) << name << ": "
            << "Old: " << std::fixed << std::setprecision(1) << std::setw(7) << oldTime << " ms, "
            << "New: " << std::setw(7) << newTime << " ms, "
            << "Speedup: " << std::setprecision(2) << speedup << "x"
            << (speedup > 1.0 ? " FASTER" : " slower") << std::endl;
}

occ::handle<Geom_BSplineSurface> MakeDomeBSpline()
{
  // Complex BSpline with 20x20 poles and multiple knot spans
  const int aNbPoles = 20;
  const int aDegree  = 3;
  const int aNbKnots = aNbPoles - aDegree + 1; // 18 knots for degree 3

  NCollection_Array2<gp_Pnt> poles(1, aNbPoles, 1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      double u = double(i - 1) / (aNbPoles - 1);
      double v = double(j - 1) / (aNbPoles - 1);
      double x = u * 10.0;
      double y = v * 10.0;
      // Complex wavy surface
      double z = 3.0 * std::sin(u * M_PI * 2) * std::cos(v * M_PI * 2)
                 + 2.0 * std::cos(u * M_PI * 3) * std::sin(v * M_PI * 3)
                 + 5.0 * (1.0 - 2.0 * ((u - 0.5) * (u - 0.5) + (v - 0.5) * (v - 0.5)));
      poles.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Create uniform knot vector
  NCollection_Array1<double> uKnots(1, aNbKnots), vKnots(1, aNbKnots);
  NCollection_Array1<int>    uMults(1, aNbKnots), vMults(1, aNbKnots);

  for (int i = 1; i <= aNbKnots; ++i)
  {
    double knotVal = double(i - 1) / (aNbKnots - 1);
    uKnots(i)      = knotVal;
    vKnots(i)      = knotVal;
    // End knots have multiplicity = degree + 1, interior knots have multiplicity 1
    uMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
    vMults(i) = (i == 1 || i == aNbKnots) ? aDegree + 1 : 1;
  }

  return new Geom_BSplineSurface(poles, uKnots, vKnots, uMults, vMults, aDegree, aDegree);
}

occ::handle<Geom_BezierSurface> MakeDomeBezier()
{
  NCollection_Array2<gp_Pnt> poles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double u = (i - 1) / 3.0;
      double v = (j - 1) / 3.0;
      double x = u * 10.0;
      double y = v * 10.0;
      double z = 5.0 * (1.0 - 4.0 * ((u - 0.5) * (u - 0.5) + (v - 0.5) * (v - 0.5)));
      poles.SetValue(i, j, gp_Pnt(x, y, std::max(0.0, z)));
    }
  }
  return new Geom_BezierSurface(poles);
}

TEST(ExtremaPS_Performance, AllSurfaces)
{
  std::cout << "\n=== ExtremaPS Performance Comparison ===" << std::endl;

  std::cout << "\n--- Analytical Surfaces ---" << std::endl;
  std::cout << "Testing " << NUM_POINTS_ANALYTICAL << " points, " << NUM_ITERATIONS_ANALYTICAL
            << " iterations each" << std::endl;

  // Plane
  {
    occ::handle<Geom_Plane> surf = new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1))));
    RunPerfTest("Plane",
                surf,
                -100,
                100,
                -100,
                100,
                NUM_POINTS_ANALYTICAL,
                NUM_ITERATIONS_ANALYTICAL);
  }

  // Cylinder
  {
    gp_Cylinder                          cyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
    occ::handle<Geom_CylindricalSurface> surf = new Geom_CylindricalSurface(cyl);
    RunPerfTest("Cylinder",
                surf,
                0,
                2 * M_PI,
                -10,
                10,
                NUM_POINTS_ANALYTICAL,
                NUM_ITERATIONS_ANALYTICAL);
  }

  // Sphere
  {
    gp_Sphere                          sph(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
    occ::handle<Geom_SphericalSurface> surf = new Geom_SphericalSurface(sph);
    RunPerfTest("Sphere",
                surf,
                0,
                2 * M_PI,
                -M_PI / 2,
                M_PI / 2,
                NUM_POINTS_ANALYTICAL,
                NUM_ITERATIONS_ANALYTICAL);
  }

  // Cone
  {
    gp_Cone                          cone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 6, 5.0);
    occ::handle<Geom_ConicalSurface> surf = new Geom_ConicalSurface(cone);
    RunPerfTest("Cone",
                surf,
                0,
                2 * M_PI,
                -5,
                20,
                NUM_POINTS_ANALYTICAL,
                NUM_ITERATIONS_ANALYTICAL);
  }

  // Torus
  {
    gp_Torus                          tor(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0);
    occ::handle<Geom_ToroidalSurface> surf = new Geom_ToroidalSurface(tor);
    RunPerfTest("Torus",
                surf,
                0,
                2 * M_PI,
                0,
                2 * M_PI,
                NUM_POINTS_ANALYTICAL,
                NUM_ITERATIONS_ANALYTICAL);
  }

  std::cout << "\n--- Numerical Surfaces ---" << std::endl;
  std::cout << "Testing " << NUM_POINTS_NUMERICAL << " points, " << NUM_ITERATIONS_NUMERICAL
            << " iterations each" << std::endl;

  // BSpline
  {
    occ::handle<Geom_BSplineSurface> surf = MakeDomeBSpline();
    RunPerfTest("BSpline (dome)", surf, 0, 1, 0, 1, NUM_POINTS_NUMERICAL, NUM_ITERATIONS_NUMERICAL);
  }

  // Bezier
  {
    occ::handle<Geom_BezierSurface> surf = MakeDomeBezier();
    RunPerfTest("Bezier (dome)", surf, 0, 1, 0, 1, NUM_POINTS_NUMERICAL, NUM_ITERATIONS_NUMERICAL);
  }

  std::cout << "\n=== Done ===" << std::endl;
}

// Generate points along a line (simulating coherent scanning)
std::vector<gp_Pnt> GenerateCoherentPoints(int           count,
                                           const gp_Pnt& start,
                                           const gp_Vec& direction,
                                           double        step)
{
  std::vector<gp_Pnt> points;
  points.reserve(count);
  for (int i = 0; i < count; ++i)
  {
    gp_Pnt p = start.Translated(direction.Multiplied(i * step));
    points.push_back(p);
  }
  return points;
}

void RunCoherentPerfTest(const char*                      name,
                         const occ::handle<Geom_Surface>& surf,
                         double                           uMin,
                         double                           uMax,
                         double                           vMin,
                         double                           vMax,
                         int                              numPoints,
                         int                              numIterations)
{
  GeomAdaptor_Surface adaptor(surf);

  // Generate points along a line above the surface (coherent trajectory)
  gp_Pnt start(0.0, 0.0, 10.0);
  gp_Vec direction(0.1, 0.1, 0.0); // Diagonal scan with small step
  auto   points = GenerateCoherentPoints(numPoints, start, direction, 0.01);

  const ExtremaPS::Domain2D aDomain(uMin, uMax, vMin, vMax);

  // Old implementation
  Extrema_ExtPS oldExt;
  oldExt.Initialize(adaptor, uMin, uMax, vMin, vMax, THE_TOL, THE_TOL);
  auto oldStart = std::chrono::high_resolution_clock::now();
  for (int iter = 0; iter < numIterations; ++iter)
  {
    for (const auto& p : points)
    {
      oldExt.Perform(p);
    }
  }
  auto   oldEnd = std::chrono::high_resolution_clock::now();
  double oldTime =
    std::chrono::duration<double, std::milli>(oldEnd - oldStart).count() / numIterations;

  // New implementation with trajectory prediction
  ExtremaPS_Surface newExt(adaptor, aDomain);
  auto              newStart = std::chrono::high_resolution_clock::now();
  for (int iter = 0; iter < numIterations; ++iter)
  {
    for (const auto& p : points)
    {
      (void)newExt.Perform(p, THE_TOL);
    }
  }
  auto   newEnd = std::chrono::high_resolution_clock::now();
  double newTime =
    std::chrono::duration<double, std::milli>(newEnd - newStart).count() / numIterations;

  double speedup = oldTime / newTime;
  std::cout << std::setw(20) << name << ": "
            << "Old: " << std::fixed << std::setprecision(1) << std::setw(7) << oldTime << " ms, "
            << "New: " << std::setw(7) << newTime << " ms, "
            << "Speedup: " << std::setprecision(2) << speedup << "x"
            << (speedup > 1.0 ? " FASTER" : " slower") << std::endl;
}

TEST(ExtremaPS_Performance, CoherentScanning)
{
  std::cout << "\n=== Coherent Scanning Performance (Trajectory Prediction) ===" << std::endl;
  std::cout << "Testing 1000 sequential points, 100 iterations each" << std::endl;

  // BSpline with coherent scanning
  {
    occ::handle<Geom_BSplineSurface> surf = MakeDomeBSpline();
    RunCoherentPerfTest("BSpline (coherent)", surf, 0, 1, 0, 1, 1000, 100);
  }

  // Bezier with coherent scanning
  {
    occ::handle<Geom_BezierSurface> surf = MakeDomeBezier();
    RunCoherentPerfTest("Bezier (coherent)", surf, 0, 1, 0, 1, 1000, 100);
  }

  // Sphere with coherent scanning
  {
    gp_Sphere                          sph(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
    occ::handle<Geom_SphericalSurface> surf = new Geom_SphericalSurface(sph);
    RunCoherentPerfTest("Sphere (coherent)", surf, 0, 2 * M_PI, -M_PI / 2, M_PI / 2, 1000, 100);
  }

  std::cout << "\n=== Done ===" << std::endl;
}
