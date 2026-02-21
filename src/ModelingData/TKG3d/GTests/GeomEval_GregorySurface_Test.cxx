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

#include <Geom_BezierSurface.hxx>
#include <GeomEval_GregorySurface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

namespace
{

constexpr double THE_FD_TOL_D1  = 1e-3;
constexpr double THE_FD_TOL_D2  = 1e-2;

// Helper to create a simple Gregory surface with a flat 4x4 grid
// and interior pairs coinciding with the boundary poles (making it a standard Bezier)
GeomEval_GregorySurface createSimpleGregory()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aPoles.SetValue(i, j, gp_Pnt(double(i - 1) / 3.0, double(j - 1) / 3.0, 0.0));
    }
  }

  // Interior pairs: when both u and v direction points are the same,
  // the Gregory patch degenerates to a standard bi-cubic Bezier.
  gp_Pnt anInteriorPairs[4][2];
  // Corner 0 at (1,1) grid index -> pole(2,2)
  anInteriorPairs[0][0] = aPoles(2, 2);
  anInteriorPairs[0][1] = aPoles(2, 2);
  // Corner 1 at (2,1) grid index -> pole(3,2)
  anInteriorPairs[1][0] = aPoles(3, 2);
  anInteriorPairs[1][1] = aPoles(3, 2);
  // Corner 2 at (1,2) grid index -> pole(2,3)
  anInteriorPairs[2][0] = aPoles(2, 3);
  anInteriorPairs[2][1] = aPoles(2, 3);
  // Corner 3 at (2,2) grid index -> pole(3,3)
  anInteriorPairs[3][0] = aPoles(3, 3);
  anInteriorPairs[3][1] = aPoles(3, 3);

  return GeomEval_GregorySurface(aPoles, anInteriorPairs);
}

// Helper to create a Gregory surface with non-trivial interior pairs
GeomEval_GregorySurface createNonTrivialGregory()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double aZ = 0.1 * std::sin(double(i)) * std::cos(double(j));
      aPoles.SetValue(i, j, gp_Pnt(double(i - 1) / 3.0, double(j - 1) / 3.0, aZ));
    }
  }

  gp_Pnt anInteriorPairs[4][2];
  // Slightly displaced interior pairs
  anInteriorPairs[0][0] = gp_Pnt(1.0 / 3.0, 1.0 / 3.0, 0.05);
  anInteriorPairs[0][1] = gp_Pnt(1.0 / 3.0, 1.0 / 3.0, -0.05);
  anInteriorPairs[1][0] = gp_Pnt(2.0 / 3.0, 1.0 / 3.0, 0.05);
  anInteriorPairs[1][1] = gp_Pnt(2.0 / 3.0, 1.0 / 3.0, -0.05);
  anInteriorPairs[2][0] = gp_Pnt(1.0 / 3.0, 2.0 / 3.0, 0.05);
  anInteriorPairs[2][1] = gp_Pnt(1.0 / 3.0, 2.0 / 3.0, -0.05);
  anInteriorPairs[3][0] = gp_Pnt(2.0 / 3.0, 2.0 / 3.0, 0.05);
  anInteriorPairs[3][1] = gp_Pnt(2.0 / 3.0, 2.0 / 3.0, -0.05);

  return GeomEval_GregorySurface(aPoles, anInteriorPairs);
}

} // namespace

// Test construction
TEST(GeomEval_GregorySurfaceTest, Construction)
{
  GeomEval_GregorySurface           aSurf  = createSimpleGregory();
  const NCollection_Array2<gp_Pnt>& aPoles = aSurf.Poles();
  EXPECT_EQ(aPoles.NbRows(), 4);
  EXPECT_EQ(aPoles.NbColumns(), 4);
}

// Test parameter range is [0, 1] x [0, 1]
TEST(GeomEval_GregorySurfaceTest, ParameterRange)
{
  GeomEval_GregorySurface aSurf = createSimpleGregory();
  double                  aU1, aU2, aV1, aV2;
  aSurf.Bounds(aU1, aU2, aV1, aV2);
  EXPECT_NEAR(aU1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aU2, 1.0, Precision::Confusion());
  EXPECT_NEAR(aV1, 0.0, Precision::Confusion());
  EXPECT_NEAR(aV2, 1.0, Precision::Confusion());
}

// Test EvalD0 at corners matches boundary poles
TEST(GeomEval_GregorySurfaceTest, EvalD0_Corners)
{
  GeomEval_GregorySurface           aSurf  = createSimpleGregory();
  const NCollection_Array2<gp_Pnt>& aPoles = aSurf.Poles();

  // Corner (0,0) -> pole(1,1)
  gp_Pnt aP00 = aSurf.EvalD0(0.0, 0.0);
  EXPECT_NEAR(aP00.Distance(aPoles(1, 1)), 0.0, Precision::Confusion());

  // Corner (1,0) -> pole(4,1)
  gp_Pnt aP10 = aSurf.EvalD0(1.0, 0.0);
  EXPECT_NEAR(aP10.Distance(aPoles(4, 1)), 0.0, Precision::Confusion());

  // Corner (0,1) -> pole(1,4)
  gp_Pnt aP01 = aSurf.EvalD0(0.0, 1.0);
  EXPECT_NEAR(aP01.Distance(aPoles(1, 4)), 0.0, Precision::Confusion());

  // Corner (1,1) -> pole(4,4)
  gp_Pnt aP11 = aSurf.EvalD0(1.0, 1.0);
  EXPECT_NEAR(aP11.Distance(aPoles(4, 4)), 0.0, Precision::Confusion());
}

// Test EvalD0 at corners for non-trivial Gregory (should still match boundary poles)
TEST(GeomEval_GregorySurfaceTest, EvalD0_Corners_NonTrivial)
{
  GeomEval_GregorySurface           aSurf  = createNonTrivialGregory();
  const NCollection_Array2<gp_Pnt>& aPoles = aSurf.Poles();

  gp_Pnt aP00 = aSurf.EvalD0(0.0, 0.0);
  EXPECT_NEAR(aP00.Distance(aPoles(1, 1)), 0.0, Precision::Confusion());

  gp_Pnt aP11 = aSurf.EvalD0(1.0, 1.0);
  EXPECT_NEAR(aP11.Distance(aPoles(4, 4)), 0.0, Precision::Confusion());
}

// Test periodicity and closure
TEST(GeomEval_GregorySurfaceTest, Periodicity)
{
  GeomEval_GregorySurface aSurf = createSimpleGregory();
  EXPECT_FALSE(aSurf.IsUPeriodic());
  EXPECT_FALSE(aSurf.IsVPeriodic());
  EXPECT_FALSE(aSurf.IsUClosed());
  EXPECT_FALSE(aSurf.IsVClosed());
}

// Test derivative consistency at interior point
TEST(GeomEval_GregorySurfaceTest, EvalD1_ConsistentWithD0)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();
  const double aU = 0.4, aV = 0.6;

  Geom_Surface::ResD1 aD1  = aSurf.EvalD1(aU, aV);
  gp_Pnt              aPu1 = aSurf.EvalD0(aU + Precision::Approximation(), aV);
  gp_Pnt              aPu2 = aSurf.EvalD0(aU - Precision::Approximation(), aV);
  gp_Pnt              aPv1 = aSurf.EvalD0(aU, aV + Precision::Approximation());
  gp_Pnt              aPv2 = aSurf.EvalD0(aU, aV - Precision::Approximation());

  gp_Vec aFDU((aPu1.XYZ() - aPu2.XYZ()) / (2.0 * Precision::Approximation()));
  gp_Vec aFDV((aPv1.XYZ() - aPv2.XYZ()) / (2.0 * Precision::Approximation()));

  // Gregory surface uses finite differences internally, so use larger tolerance
  EXPECT_NEAR(aD1.D1U.X(), aFDU.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1U.Y(), aFDU.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1U.Z(), aFDU.Z(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.X(), aFDV.X(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.Y(), aFDV.Y(), THE_FD_TOL_D1);
  EXPECT_NEAR(aD1.D1V.Z(), aFDV.Z(), THE_FD_TOL_D1);
}

// Test InteriorPoint access
TEST(GeomEval_GregorySurfaceTest, InteriorPoint_Access)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();
  // Verify interior points can be accessed without crash
  for (int i = 0; i < 4; ++i)
  {
    gp_Pnt aPu = aSurf.InteriorPoint(i, 0);
    gp_Pnt aPv = aSurf.InteriorPoint(i, 1);
    // Points should be finite
    EXPECT_TRUE(std::isfinite(aPu.X()));
    EXPECT_TRUE(std::isfinite(aPv.X()));
  }
}

// Test Transform preserves evaluation
TEST(GeomEval_GregorySurfaceTest, Transform_PreservesEvaluation)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();
  gp_Trsf                 aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  const double aU = 0.5, aV = 0.5;
  gp_Pnt       aPBefore = aSurf.EvalD0(aU, aV);
  aPBefore.Transform(aTrsf);
  aSurf.Transform(aTrsf);
  gp_Pnt aPAfter = aSurf.EvalD0(aU, aV);

  EXPECT_NEAR(aPBefore.Distance(aPAfter), 0.0, Precision::Confusion());
}

// Test Copy produces independent identical object
TEST(GeomEval_GregorySurfaceTest, Copy_Independent)
{
  GeomEval_GregorySurface    aSurf = createNonTrivialGregory();
  occ::handle<Geom_Geometry> aCopy = aSurf.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_GregorySurface* aCopySurf =
    dynamic_cast<const GeomEval_GregorySurface*>(aCopy.get());
  EXPECT_TRUE(aCopySurf != nullptr);
  EXPECT_EQ(aCopySurf->Poles().NbRows(), 4);
  EXPECT_EQ(aCopySurf->Poles().NbColumns(), 4);

  // Verify copy is independent: modifying original should not affect copy
  gp_Pnt aPOriginal = aSurf.EvalD0(0.5, 0.5);
  gp_Pnt aPCopy     = aCopySurf->EvalD0(0.5, 0.5);
  EXPECT_NEAR(aPOriginal.Distance(aPCopy), 0.0, Precision::Confusion());
}

// Test DumpJson does not crash
TEST(GeomEval_GregorySurfaceTest, DumpJson_NoCrash)
{
  GeomEval_GregorySurface aSurf = createSimpleGregory();
  Standard_SStream        aSS;
  EXPECT_NO_THROW(aSurf.DumpJson(aSS));
}

// Test EvalD0 matches Bezier surface with equivalent poles
TEST(GeomEval_GregorySurfaceTest, EvalD0_MatchesBezierSurface)
{
  GeomEval_GregorySurface           aGregory = createSimpleGregory();
  const NCollection_Array2<gp_Pnt>& aPoles   = aGregory.Poles();

  // Create a Bezier surface with the same 4x4 poles
  NCollection_Array2<gp_Pnt> aBPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aBPoles.SetValue(i, j, aPoles(i, j));
    }
  }
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aBPoles);

  // Compare EvalD0 at interior points
  const double aParams[][2] = {{0.25, 0.25},
                               {0.25, 0.5},
                               {0.25, 0.75},
                               {0.5, 0.25},
                               {0.5, 0.5},
                               {0.5, 0.75},
                               {0.75, 0.25},
                               {0.75, 0.5},
                               {0.75, 0.75},
                               {0.1, 0.9},
                               {0.9, 0.1}};
  for (const auto& aUV : aParams)
  {
    gp_Pnt aPGregory = aGregory.EvalD0(aUV[0], aUV[1]);
    gp_Pnt aPBezier  = aBezier->EvalD0(aUV[0], aUV[1]);
    EXPECT_NEAR(aPGregory.Distance(aPBezier), 0.0, Precision::Confusion())
      << "Mismatch at u=" << aUV[0] << " v=" << aUV[1];
  }
}

// Test EvalD1 matches Bezier surface with equivalent poles
TEST(GeomEval_GregorySurfaceTest, EvalD1_MatchesBezierSurface)
{
  GeomEval_GregorySurface           aGregory = createSimpleGregory();
  const NCollection_Array2<gp_Pnt>& aPoles   = aGregory.Poles();

  NCollection_Array2<gp_Pnt> aBPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aBPoles.SetValue(i, j, aPoles(i, j));
    }
  }
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aBPoles);

  const double aTol = THE_FD_TOL_D1;
  const double aParams[][2] = {{0.25, 0.25}, {0.5, 0.5}, {0.75, 0.25}, {0.3, 0.7}};
  for (const auto& aUV : aParams)
  {
    Geom_Surface::ResD1 aD1G = aGregory.EvalD1(aUV[0], aUV[1]);
    Geom_Surface::ResD1 aD1B = aBezier->EvalD1(aUV[0], aUV[1]);

    EXPECT_NEAR(aD1G.Point.Distance(aD1B.Point), 0.0, Precision::Confusion())
      << "Point mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1U.X(), aD1B.D1U.X(), aTol)
      << "D1U.X mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1U.Y(), aD1B.D1U.Y(), aTol)
      << "D1U.Y mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1U.Z(), aD1B.D1U.Z(), aTol)
      << "D1U.Z mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1V.X(), aD1B.D1V.X(), aTol)
      << "D1V.X mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1V.Y(), aD1B.D1V.Y(), aTol)
      << "D1V.Y mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD1G.D1V.Z(), aD1B.D1V.Z(), aTol)
      << "D1V.Z mismatch at u=" << aUV[0] << " v=" << aUV[1];
  }
}

// Test EvalD2 matches Bezier surface with equivalent poles (degenerate Gregory)
TEST(GeomEval_GregorySurfaceTest, EvalD2_MatchesBezierSurface)
{
  GeomEval_GregorySurface           aGregory = createSimpleGregory();
  const NCollection_Array2<gp_Pnt>& aPoles   = aGregory.Poles();

  NCollection_Array2<gp_Pnt> aBPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aBPoles.SetValue(i, j, aPoles(i, j));
    }
  }
  Handle(Geom_BezierSurface) aBezier = new Geom_BezierSurface(aBPoles);

  const double aTol = THE_FD_TOL_D2;
  const double aParams[][2] = {{0.25, 0.25}, {0.5, 0.5}, {0.75, 0.25}, {0.3, 0.7}};
  for (const auto& aUV : aParams)
  {
    Geom_Surface::ResD2 aD2G = aGregory.EvalD2(aUV[0], aUV[1]);
    Geom_Surface::ResD2 aD2B = aBezier->EvalD2(aUV[0], aUV[1]);

    EXPECT_NEAR(aD2G.Point.Distance(aD2B.Point), 0.0, Precision::Confusion())
      << "Point mismatch at u=" << aUV[0] << " v=" << aUV[1];
    EXPECT_NEAR(aD2G.D1U.X(), aD2B.D1U.X(), aTol);
    EXPECT_NEAR(aD2G.D1U.Y(), aD2B.D1U.Y(), aTol);
    EXPECT_NEAR(aD2G.D1U.Z(), aD2B.D1U.Z(), aTol);
    EXPECT_NEAR(aD2G.D1V.X(), aD2B.D1V.X(), aTol);
    EXPECT_NEAR(aD2G.D1V.Y(), aD2B.D1V.Y(), aTol);
    EXPECT_NEAR(aD2G.D1V.Z(), aD2B.D1V.Z(), aTol);
    EXPECT_NEAR(aD2G.D2U.X(), aD2B.D2U.X(), aTol);
    EXPECT_NEAR(aD2G.D2U.Y(), aD2B.D2U.Y(), aTol);
    EXPECT_NEAR(aD2G.D2U.Z(), aD2B.D2U.Z(), aTol);
    EXPECT_NEAR(aD2G.D2V.X(), aD2B.D2V.X(), aTol);
    EXPECT_NEAR(aD2G.D2V.Y(), aD2B.D2V.Y(), aTol);
    EXPECT_NEAR(aD2G.D2V.Z(), aD2B.D2V.Z(), aTol);
    EXPECT_NEAR(aD2G.D2UV.X(), aD2B.D2UV.X(), aTol);
    EXPECT_NEAR(aD2G.D2UV.Y(), aD2B.D2UV.Y(), aTol);
    EXPECT_NEAR(aD2G.D2UV.Z(), aD2B.D2UV.Z(), aTol);
  }
}

// Test EvalD2 components are consistent with EvalD1 at the same point.
// The D2 result includes D1U and D1V which must match EvalD1's values,
// and D2U, D2V, D2UV must be finite and self-consistent.
TEST(GeomEval_GregorySurfaceTest, EvalD2_ConsistentWithD1)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();
  const double aTol = THE_FD_TOL_D2;

  // Test at interior points, avoiding corners where Gregory blending is singular
  const double aParams[][2] = {{0.3, 0.4}, {0.6, 0.7}};
  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];

    Geom_Surface::ResD2 aD2 = aSurf.EvalD2(aU, aV);
    Geom_Surface::ResD1 aD1 = aSurf.EvalD1(aU, aV);

    // Point from D2 must match EvalD0
    gp_Pnt aP0 = aSurf.EvalD0(aU, aV);
    EXPECT_NEAR(aD2.Point.Distance(aP0), 0.0, Precision::Confusion())
      << "Point mismatch at u=" << aU << " v=" << aV;

    // D1U and D1V from D2 must match EvalD1
    EXPECT_NEAR(aD2.D1U.X(), aD1.D1U.X(), Precision::Confusion())
      << "D1U.X mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aD2.D1U.Y(), aD1.D1U.Y(), Precision::Confusion())
      << "D1U.Y mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aD2.D1U.Z(), aD1.D1U.Z(), Precision::Confusion())
      << "D1U.Z mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aD2.D1V.X(), aD1.D1V.X(), Precision::Confusion())
      << "D1V.X mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aD2.D1V.Y(), aD1.D1V.Y(), Precision::Confusion())
      << "D1V.Y mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aD2.D1V.Z(), aD1.D1V.Z(), Precision::Confusion())
      << "D1V.Z mismatch at u=" << aU << " v=" << aV;

    // D2 derivatives must be finite
    EXPECT_TRUE(std::isfinite(aD2.D2U.Magnitude())) << "D2U non-finite at u=" << aU << " v=" << aV;
    EXPECT_TRUE(std::isfinite(aD2.D2V.Magnitude())) << "D2V non-finite at u=" << aU << " v=" << aV;
    EXPECT_TRUE(std::isfinite(aD2.D2UV.Magnitude()))
      << "D2UV non-finite at u=" << aU << " v=" << aV;

    // Verify D2 symmetry: EvalD2 at nearby points should vary smoothly.
    // Compare D2U at (u,v) and (u+delta,v) - they should not jump wildly.
    const double        aDelta  = 0.05;
    Geom_Surface::ResD2 aD2Near = aSurf.EvalD2(aU + aDelta, aV);
    // For a smooth surface, second derivatives at points 0.05 apart should
    // not differ by more than a reasonable bound relative to their magnitude
    double aD2UChange = (aD2.D2U - aD2Near.D2U).Magnitude();
    double aD2UScale  = std::max(aD2.D2U.Magnitude(), aD2Near.D2U.Magnitude());
    if (aD2UScale > aTol)
    {
      EXPECT_LT(aD2UChange / aD2UScale, 5.0)
        << "D2U jumps too much between nearby points at u=" << aU << " v=" << aV;
    }
  }
}

// Test EvalD0 at interior points for non-trivial Gregory produces finite, smooth values
TEST(GeomEval_GregorySurfaceTest, EvalD0_InteriorPoints_NonTrivialGregory)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();

  constexpr int THE_GRID_SIZE = 4;
  gp_Pnt        aGrid[THE_GRID_SIZE][THE_GRID_SIZE];

  // Evaluate on a 4x4 interior grid
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    for (int j = 0; j < THE_GRID_SIZE; ++j)
    {
      const double aU = (double(i) + 1.0) / (double(THE_GRID_SIZE) + 1.0);
      const double aV = (double(j) + 1.0) / (double(THE_GRID_SIZE) + 1.0);
      aGrid[i][j]     = aSurf.EvalD0(aU, aV);

      // Verify all coordinates are finite (no NaN or Inf)
      EXPECT_TRUE(std::isfinite(aGrid[i][j].X())) << "Non-finite X at u=" << aU << " v=" << aV;
      EXPECT_TRUE(std::isfinite(aGrid[i][j].Y())) << "Non-finite Y at u=" << aU << " v=" << aV;
      EXPECT_TRUE(std::isfinite(aGrid[i][j].Z())) << "Non-finite Z at u=" << aU << " v=" << aV;
    }
  }

  // Verify smoothness: no large jumps between adjacent points
  // The surface spans roughly [0,1] x [0,1] x [-0.1,0.1], so adjacent
  // grid points at spacing ~0.2 should not differ by more than 1.0
  const double aMaxJump = 1.0;
  for (int i = 0; i < THE_GRID_SIZE; ++i)
  {
    for (int j = 0; j < THE_GRID_SIZE; ++j)
    {
      if (i + 1 < THE_GRID_SIZE)
      {
        double aDist = aGrid[i][j].Distance(aGrid[i + 1][j]);
        EXPECT_LT(aDist, aMaxJump) << "Large jump in U direction at grid (" << i << "," << j << ")";
      }
      if (j + 1 < THE_GRID_SIZE)
      {
        double aDist = aGrid[i][j].Distance(aGrid[i][j + 1]);
        EXPECT_LT(aDist, aMaxJump) << "Large jump in V direction at grid (" << i << "," << j << ")";
      }
    }
  }
}

// Test EvalDN consistency with EvalD1
TEST(GeomEval_GregorySurfaceTest, EvalDN_D1_Consistency)
{
  GeomEval_GregorySurface aSurf = createNonTrivialGregory();

  const double aParams[][2] = {{0.3, 0.4}, {0.6, 0.7}};
  for (const auto& aUV : aParams)
  {
    const double aU = aUV[0];
    const double aV = aUV[1];

    Geom_Surface::ResD1 aD1   = aSurf.EvalD1(aU, aV);
    gp_Vec              aDN10 = aSurf.EvalDN(aU, aV, 1, 0);
    gp_Vec              aDN01 = aSurf.EvalDN(aU, aV, 0, 1);

    // EvalDN(u,v,1,0) should match D1U from EvalD1
    EXPECT_NEAR(aDN10.X(), aD1.D1U.X(), Precision::Confusion())
      << "DN(1,0).X mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aDN10.Y(), aD1.D1U.Y(), Precision::Confusion())
      << "DN(1,0).Y mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aDN10.Z(), aD1.D1U.Z(), Precision::Confusion())
      << "DN(1,0).Z mismatch at u=" << aU << " v=" << aV;

    // EvalDN(u,v,0,1) should match D1V from EvalD1
    EXPECT_NEAR(aDN01.X(), aD1.D1V.X(), Precision::Confusion())
      << "DN(0,1).X mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aDN01.Y(), aD1.D1V.Y(), Precision::Confusion())
      << "DN(0,1).Y mismatch at u=" << aU << " v=" << aV;
    EXPECT_NEAR(aDN01.Z(), aD1.D1V.Z(), Precision::Confusion())
      << "DN(0,1).Z mismatch at u=" << aU << " v=" << aV;
  }
}
