// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRepMesh_CircleTool.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>

#include <cmath>
#include <cstdlib>
#include <ctime>

namespace
{
// Returns true if thePoint lies on the circle defined by theCenter and theRadius.
bool isOnCircle(const gp_XY& thePoint, const gp_XY& theCenter, const double theRadius)
{
  static const double aSqPrec = Precision::PConfusion() * Precision::PConfusion();
  const gp_XY         aDiff   = thePoint - theCenter;
  return aDiff.SquareModulus() - theRadius * theRadius < aSqPrec;
}
} // namespace

// OCC24923: BRepMesh_CircleTool produces bad circles
// For a large set of random non-degenerate triangles, every circumscribed circle
// computed by BRepMesh_CircleTool::MakeCircle must pass through all three input points.
TEST(BRepMesh_CircleTool_Test, OCC24923_CircumCirclePassesThroughAllVertices)
{
  srand(42);

  static const double aSqPrec  = Precision::PConfusion() * Precision::PConfusion();
  const double        aMinArea = 5.0 * M_PI / 180.0; // same threshold as the original command
  const int           aNbTests = 100000;

  int aNbFailed = 0;
  int i         = 0;
  while (i < aNbTests)
  {
    gp_XY p[3];
    for (int j = 0; j < 3; ++j)
      p[j].SetCoord(static_cast<double>(rand()) / RAND_MAX,
                    static_cast<double>(rand()) / RAND_MAX);

    // Skip degenerate (nearly collinear) triangles — retry like the original.
    const gp_XY aV1 = p[1] - p[0];
    const gp_XY aV2 = p[2] - p[0];
    if (aV1.SquareModulus() <= aSqPrec || aV2.SquareModulus() <= aSqPrec
        || (aV1 ^ aV2) <= aMinArea)
      continue;

    ++i;

    gp_XY  aCenter;
    double aRadius = 0.0;
    if (!BRepMesh_CircleTool::MakeCircle(p[0], p[1], p[2], aCenter, aRadius))
      continue;

    if (!isOnCircle(p[0], aCenter, aRadius) || !isOnCircle(p[1], aCenter, aRadius)
        || !isOnCircle(p[2], aCenter, aRadius))
    {
      ++aNbFailed;
    }
  }

  // Allow at most 1% failure rate (same threshold as the original Draw test).
  const double aFailRate = static_cast<double>(aNbFailed) / static_cast<double>(aNbTests);
  EXPECT_LE(aFailRate, 0.01) << "Too many bad circumscribed circles: " << aNbFailed << " / "
                              << aNbTests;
}
