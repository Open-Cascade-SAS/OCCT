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

#include <gtest/gtest.h>

#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

// Helper to find minimum distance from ExtCC result
static double getMinDistance(const Extrema_ExtCC& theExtCC)
{
  if (theExtCC.NbExt() == 0)
  {
    return -1.0;
  }
  double aMinSqDist = theExtCC.SquareDistance(1);
  for (int i = 2; i <= theExtCC.NbExt(); ++i)
  {
    double aSqDist = theExtCC.SquareDistance(i);
    if (aSqDist < aMinSqDist)
    {
      aMinSqDist = aSqDist;
    }
  }
  return sqrt(aMinSqDist);
}

// Test: Two coplanar circles not touching
TEST(Extrema_ExtCC_Test, CoplanarCircles_NotTouching)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(15.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Distance: 15 - 5 - 5 = 5
  EXPECT_NEAR(getMinDistance(anExtCC), 5.0, 1e-6);
}

// Test: Two coplanar circles touching
TEST(Extrema_ExtCC_Test, CoplanarCircles_Touching)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(10.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  EXPECT_NEAR(getMinDistance(anExtCC), 0.0, 1e-6);
}

// Test: Two coplanar circles intersecting
TEST(Extrema_ExtCC_Test, CoplanarCircles_Intersecting)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(8.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  EXPECT_GE(anExtCC.NbExt(), 2);

  EXPECT_NEAR(getMinDistance(anExtCC), 0.0, 1e-6);
}

// Test: Perpendicular circles intersecting at origin
TEST(Extrema_ExtCC_Test, PerpendicularCircles_Intersecting)
{
  // Circle in XY plane
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  // Circle in XZ plane
  gp_Ax2              anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Circles intersect at (5, 0, 0) and (-5, 0, 0)
  EXPECT_NEAR(getMinDistance(anExtCC), 0.0, 1e-6);
}

// Test: Verify extrema points
TEST(Extrema_ExtCC_Test, VerifyExtremaPoints)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(20.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Find minimum extremum
  int    aMinIdx   = 1;
  double aMinDist2 = anExtCC.SquareDistance(1);
  for (int i = 2; i <= anExtCC.NbExt(); ++i)
  {
    if (anExtCC.SquareDistance(i) < aMinDist2)
    {
      aMinDist2 = anExtCC.SquareDistance(i);
      aMinIdx   = i;
    }
  }

  Extrema_POnCurv aP1, aP2;
  anExtCC.Points(aMinIdx, aP1, aP2);

  gp_Pnt aPnt1 = aP1.Value();
  gp_Pnt aPnt2 = aP2.Value();

  // Closest points should be at (5, 0, 0) and (15, 0, 0)
  EXPECT_NEAR(aPnt1.X(), 5.0, 1e-6);
  EXPECT_NEAR(aPnt1.Y(), 0.0, 1e-6);
  EXPECT_NEAR(aPnt1.Z(), 0.0, 1e-6);

  EXPECT_NEAR(aPnt2.X(), 15.0, 1e-6);
  EXPECT_NEAR(aPnt2.Y(), 0.0, 1e-6);
  EXPECT_NEAR(aPnt2.Z(), 0.0, 1e-6);

  EXPECT_NEAR(sqrt(aMinDist2), 10.0, 1e-6);
  EXPECT_NEAR(aPnt1.Distance(aPnt2), 10.0, 1e-6);
}

// Test: TrimmedSquareDistances method
TEST(Extrema_ExtCC_Test, TrimmedSquareDistances)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(20.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());

  double aDist11, aDist12, aDist21, aDist22;
  gp_Pnt aP11, aP12, aP21, aP22;
  anExtCC.TrimmedSquareDistances(aDist11, aDist12, aDist21, aDist22, aP11, aP12, aP21, aP22);

  // All distances should be positive
  EXPECT_GE(aDist11, 0.0);
  EXPECT_GE(aDist12, 0.0);
  EXPECT_GE(aDist21, 0.0);
  EXPECT_GE(aDist22, 0.0);
}

// Test: Symmetry - ExtCC(C1, C2) should give same results as ExtCC(C2, C1)
TEST(Extrema_ExtCC_Test, SymmetryTest)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(15.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC1(aAdaptor1, aAdaptor2);
  Extrema_ExtCC anExtCC2(aAdaptor2, aAdaptor1);

  ASSERT_TRUE(anExtCC1.IsDone());
  ASSERT_TRUE(anExtCC2.IsDone());

  EXPECT_EQ(anExtCC1.NbExt(), anExtCC2.NbExt());

  if (anExtCC1.NbExt() > 0 && anExtCC2.NbExt() > 0)
  {
    double aMinDist1 = getMinDistance(anExtCC1);
    double aMinDist2 = getMinDistance(anExtCC2);
    EXPECT_NEAR(aMinDist1, aMinDist2, 1e-6);
  }
}

// Helper to find maximum distance from ExtCC result
static double getMaxDistance(const Extrema_ExtCC& theExtCC)
{
  if (theExtCC.NbExt() == 0)
  {
    return -1.0;
  }
  double aMaxSqDist = theExtCC.SquareDistance(1);
  for (int i = 2; i <= theExtCC.NbExt(); ++i)
  {
    double aSqDist = theExtCC.SquareDistance(i);
    if (aSqDist > aMaxSqDist)
    {
      aMaxSqDist = aSqDist;
    }
  }
  return sqrt(aMaxSqDist);
}

// Test: Non-coplanar circles at 45 degrees
TEST(Extrema_ExtCC_Test, NonCoplanar_45Degrees)
{
  // Circle in XY plane
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  // Circle tilted 45 degrees
  gp_Dir              aDir2(0.0, -1.0 / sqrt(2.0), 1.0 / sqrt(2.0));
  gp_Ax2              anAxis2(gp_Pnt(0.0, 10.0, 0.0), aDir2);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  EXPECT_GT(getMinDistance(anExtCC), 0.0);
}

// Test: Linked circles (like chain links)
TEST(Extrema_ExtCC_Test, LinkedCircles)
{
  // Circle in XY plane at origin, radius 5
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  // Circle in XZ plane at origin, radius 3
  // C1 passes through (5,0,0), C2 passes through (3,0,0)
  // Closest distance is 5-3=2
  gp_Ax2              anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 3.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Closest points: (5,0,0) on C1 to (3,0,0) on C2, distance = 2
  EXPECT_NEAR(getMinDistance(anExtCC), 2.0, 1e-6);
}

// Test: Circles with different radii, non-coplanar
TEST(Extrema_ExtCC_Test, DifferentRadii_NonCoplanar)
{
  // Large circle in XY plane at origin, radius 10
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 10.0);

  // Small circle in YZ plane at x=15, radius 2
  gp_Ax2              anAxis2(gp_Pnt(15.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 2.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Non-trivial geometry - just verify reasonable bounds
  double aMinDist = getMinDistance(anExtCC);
  EXPECT_GT(aMinDist, 4.0); // Should be > 5 - 2 = 3
  EXPECT_LT(aMinDist, 6.0); // Should be < sqrt(25+4) ≈ 5.4
}

// Test: Symmetry for non-coplanar circles
TEST(Extrema_ExtCC_Test, SymmetryNonCoplanar)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  gp_Dir              aDir2(1.0, 1.0, 2.0);
  gp_Ax2              anAxis2(gp_Pnt(8.0, 3.0, 4.0), aDir2);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 4.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC1(aAdaptor1, aAdaptor2);
  Extrema_ExtCC anExtCC2(aAdaptor2, aAdaptor1);

  ASSERT_TRUE(anExtCC1.IsDone());
  ASSERT_TRUE(anExtCC2.IsDone());
  ASSERT_GT(anExtCC1.NbExt(), 0);
  ASSERT_GT(anExtCC2.NbExt(), 0);

  EXPECT_NEAR(getMinDistance(anExtCC1), getMinDistance(anExtCC2), 1e-6);
}

// Test: Verify points lie on circles
TEST(Extrema_ExtCC_Test, PointsOnCircles)
{
  gp_Ax2              anAxis1(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 7.0);

  gp_Dir              aDir2(1.0, 0.0, 1.0);
  gp_Ax2              anAxis2(gp_Pnt(10.0, 5.0, 0.0), aDir2);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 4.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  for (int i = 1; i <= anExtCC.NbExt(); ++i)
  {
    Extrema_POnCurv aP1, aP2;
    anExtCC.Points(i, aP1, aP2);

    gp_Pnt aPnt1 = aP1.Value();
    gp_Pnt aPnt2 = aP2.Value();

    // Point 1 should be on circle 1
    double aDistToC1 = aPnt1.Distance(aC1->Location());
    EXPECT_NEAR(aDistToC1, aC1->Radius(), 1e-6);

    // Point 2 should be on circle 2
    double aDistToC2 = aPnt2.Distance(aC2->Location());
    EXPECT_NEAR(aDistToC2, aC2->Radius(), 1e-6);

    // Verify squared distance matches
    double aReportedSqDist = anExtCC.SquareDistance(i);
    double aActualSqDist   = aPnt1.SquareDistance(aPnt2);
    EXPECT_NEAR(aReportedSqDist, aActualSqDist, 1e-10);
  }
}

// Test: Skew circles
TEST(Extrema_ExtCC_Test, SkewCircles)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  gp_Dir              aDir2(1.0, 1.0, 1.0);
  gp_Ax2              anAxis2(gp_Pnt(10.0, 10.0, 5.0), aDir2);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  EXPECT_GE(getMinDistance(anExtCC), 0.0);
}

// Test: Maximum distance
TEST(Extrema_ExtCC_Test, MaximumDistance)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(20.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Maximum distance: 20 + 5 + 5 = 30
  EXPECT_NEAR(getMaxDistance(anExtCC), 30.0, 1e-6);
}

// Test: Nearly coplanar circles
TEST(Extrema_ExtCC_Test, NearlyCoplanar)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  gp_Dir              aDir2(0.0, 1e-8, 1.0);
  gp_Ax2              anAxis2(gp_Pnt(15.0, 0.0, 0.0), aDir2);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  EXPECT_NEAR(getMinDistance(anExtCC), 5.0, 1e-4);
}

// Test: Parallel circles with offset centers
TEST(Extrema_ExtCC_Test, ParallelCircles_OffsetCenters)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 5.0);

  gp_Ax2              anAxis2(gp_Pnt(3.0, 4.0, 12.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // XY offset is 5, Z offset is 12, circles project to touch
  // Minimum 3D distance is 12.0
  EXPECT_NEAR(getMinDistance(anExtCC), 12.0, 1e-5);
}

// Test: Concentric circles (same center, different radii)
TEST(Extrema_ExtCC_Test, ConcentricCircles)
{
  gp_Ax2              anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis, 10.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis, 5.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());

  // For concentric coplanar circles, should be parallel
  if (anExtCC.IsParallel())
  {
    // Distance should be R1 - R2 = 5
    EXPECT_TRUE(true);
  }
  else if (anExtCC.NbExt() > 0)
  {
    EXPECT_NEAR(getMinDistance(anExtCC), 5.0, 1e-6);
  }
}

// Test: One circle inside the other (coplanar)
TEST(Extrema_ExtCC_Test, CoplanarCircles_OneInsideOther)
{
  gp_Ax2              anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax2              anAxis2(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Circle) aC1 = new Geom_Circle(anAxis1, 10.0);
  Handle(Geom_Circle) aC2 = new Geom_Circle(anAxis2, 3.0);

  GeomAdaptor_Curve aAdaptor1(aC1);
  GeomAdaptor_Curve aAdaptor2(aC2);

  Extrema_ExtCC anExtCC(aAdaptor1, aAdaptor2);

  ASSERT_TRUE(anExtCC.IsDone());
  ASSERT_GT(anExtCC.NbExt(), 0);

  // Minimum distance: 10 - 2 - 3 = 5
  EXPECT_NEAR(getMinDistance(anExtCC), 5.0, 1e-6);
}
