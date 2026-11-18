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
#include <BRepMesh_Circle.hxx>
#include <BRepMesh_CircleInspector.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>

#include <cmath>
#include <array>
#include <cstdlib>
#include <ctime>

namespace
{
class BRepMesh_CountingIncAllocator : public NCollection_IncAllocator
{
public:
  void* AllocateOptimal(size_t theSize) override;

  size_t NbAllocations() const;

private:
  size_t myNbAllocations = 0;
};

//==================================================================================================

void* BRepMesh_CountingIncAllocator::AllocateOptimal(size_t theSize)
{
  ++myNbAllocations;
  return NCollection_IncAllocator::AllocateOptimal(theSize);
}

//==================================================================================================

size_t BRepMesh_CountingIncAllocator::NbAllocations() const
{
  return myNbAllocations;
}

// Returns true if thePoint lies on the circle defined by theCenter and theRadius.
bool isOnCircle(const gp_XY& thePoint, const gp_XY& theCenter, const double theRadius)
{
  static const double aSqPrec = Precision::PConfusion() * Precision::PConfusion();
  const gp_XY         aDiff   = thePoint - theCenter;
  return aDiff.SquareModulus() - theRadius * theRadius < aSqPrec;
}
} // namespace

TEST(BRepMesh_CircleToolTest, CircumcircleKeepsPointsBeyondSecantOutside)
{
  const std::array<gp_XY, 3> aPoints       = {gp_XY(12.463211482361567, 1.9738661904523815e-06),
                                              gp_XY(12.463212204366371, 4.7372788570857157e-06),
                                              gp_XY(12.361816737261172, 0.025871858931497457)};
  const std::array<gp_XY, 3> aTranslations = {gp_XY(0.0, 0.0),
                                              gp_XY(16.0, -8.0),
                                              gp_XY(-32.0, 64.0)};
  BRepMesh_CircleInspector   anInspector(Precision::PConfusion(), 1, new NCollection_IncAllocator);
  for (const double aScale : {0.25, 1.0, 4.0})
  {
    for (const gp_XY& aTranslation : aTranslations)
    {
      SCOPED_TRACE(::testing::Message() << "Scale=" << aScale << " translation=" << aTranslation.X()
                                        << "," << aTranslation.Y());
      const gp_XY aP1 = aPoints[0] * aScale + aTranslation;
      const gp_XY aP2 = aPoints[1] * aScale + aTranslation;
      const gp_XY aP3 = aPoints[2] * aScale + aTranslation;
      gp_XY       aCenter;
      double      aRadius;
      ASSERT_TRUE(BRepMesh_CircleTool::MakeCircle(aP1, aP2, aP3, aCenter, aRadius));
      anInspector.Bind(1, BRepMesh_Circle(aCenter, aRadius));
      for (const gp_XY& aPoint : {aP1, aP2, aP3})
      {
        anInspector.SetPoint(aPoint);
        anInspector.Inspect(1);
        EXPECT_FALSE(anInspector.GetShotCircles().IsEmpty());
      }
      // A point beyond an endpoint of a secant is outside the circle.
      anInspector.SetPoint(aP1 + (aP1 - aP2) * 0.5);
      anInspector.Inspect(1);
      EXPECT_TRUE(anInspector.GetShotCircles().IsEmpty());
    }
  }
}

TEST(BRepMesh_CircleToolTest, TranslatedRightTrianglesHaveAnalyticCircumcircles)
{
  const std::array<gp_XY, 3> aLegs = {gp_XY(1.0, 1.0), gp_XY(1.0, 1.e-6), gp_XY(1.e-3, 1.e-4)};
  const std::array<gp_XY, 3> aTranslations = {gp_XY(0.0, 0.0),
                                              gp_XY(16.0, -8.0),
                                              gp_XY(-32.0, 64.0)};
  for (const gp_XY& aLeg : aLegs)
  {
    for (const double aScale : {0.25, 1.0, 4.0})
    {
      for (const gp_XY& aTranslation : aTranslations)
      {
        const gp_XY  aP2              = aTranslation + gp_XY(aLeg.X() * aScale, 0.0);
        const gp_XY  aP3              = aTranslation + gp_XY(0.0, aLeg.Y() * aScale);
        const gp_XY  anExpectedCenter = aTranslation + aLeg * (0.5 * aScale);
        const double anExpectedRadius = (aLeg * (0.5 * aScale)).Modulus();
        gp_XY        aCenter;
        double       aRadius;
        ASSERT_TRUE(BRepMesh_CircleTool::MakeCircle(aTranslation, aP2, aP3, aCenter, aRadius));
        EXPECT_NEAR(aCenter.X(), anExpectedCenter.X(), Precision::PConfusion());
        EXPECT_NEAR(aCenter.Y(), anExpectedCenter.Y(), Precision::PConfusion());
        EXPECT_NEAR(aRadius, anExpectedRadius, Precision::PConfusion());
      }
    }
  }
}

TEST(BRepMesh_CircleToolTest, CircumcircleRejectsDegenerateEdges)
{
  const std::array<gp_XY, 3> aPoints   = {gp_XY(0.0, 0.0), gp_XY(1.0, 0.0), gp_XY(0.0, 1.0)};
  std::array<gp_XY, 3>       aTriangle = aPoints;
  for (size_t anIndex = 0; anIndex < aPoints.size(); ++anIndex)
  {
    aTriangle = aPoints;
    aTriangle[(anIndex + 1) % aTriangle.size()] =
      aTriangle[anIndex] + gp_XY(0.5 * Precision::PConfusion(), 0.0);
    gp_XY  aCenter;
    double aRadius;
    EXPECT_FALSE(
      BRepMesh_CircleTool::MakeCircle(aTriangle[0], aTriangle[1], aTriangle[2], aCenter, aRadius));
  }
  gp_XY  aCenter;
  double aRadius;
  EXPECT_FALSE(BRepMesh_CircleTool::MakeCircle(gp_XY(0.0, 0.0),
                                               gp_XY(1.0, 0.0),
                                               gp_XY(2.0, 0.0),
                                               aCenter,
                                               aRadius));
}

TEST(BRepMesh_CircleInspectorTest, RepeatedSelectionsRetainCircleStorage)
{
  const occ::handle<BRepMesh_CountingIncAllocator> anAllocator = new BRepMesh_CountingIncAllocator;
  BRepMesh_CircleInspector anInspector(Precision::PConfusion(), 3, anAllocator);
  anInspector.Bind(1, BRepMesh_Circle(gp_XY(0.0, 0.0), 1.0));
  anInspector.Bind(2, BRepMesh_Circle(gp_XY(4.0, 0.0), 1.0));
  anInspector.Bind(3, BRepMesh_Circle(gp_XY(0.0, 0.0), -1.0));
  const size_t aNbAllocations = anAllocator->NbAllocations();

  for (int aRepeat = 0; aRepeat < 2; ++aRepeat)
  {
    for (int anIndex = 1; anIndex <= 2; ++anIndex)
    {
      anInspector.SetPoint(gp_XY(4.0 * (anIndex - 1), 0.0));
      EXPECT_EQ(anInspector.Inspect(1), CellFilter_Keep);
      EXPECT_EQ(anInspector.Inspect(2), CellFilter_Keep);
      EXPECT_EQ(anInspector.Inspect(3), CellFilter_Purge);
      ASSERT_EQ(anInspector.GetShotCircles().Size(), 1);
      EXPECT_EQ(anInspector.GetShotCircles().First(), anIndex);
    }
    anInspector.SetPoint(gp_XY(8.0, 0.0));
    anInspector.Inspect(1);
    anInspector.Inspect(2);
    EXPECT_TRUE(anInspector.GetShotCircles().IsEmpty());
  }

  EXPECT_EQ(anAllocator->NbAllocations(), aNbAllocations);
  EXPECT_EQ(anInspector.Circle(1).Radius(), 1.0);
  EXPECT_EQ(anInspector.Circle(2).Location().X(), 4.0);

  anInspector.SetPoint(gp_XY(0.0, 0.0));
  anInspector.Inspect(1);
  BRepMesh_CircleInspector aCopy(anInspector);
  anInspector.SetPoint(gp_XY(4.0, 0.0));
  anInspector.Inspect(1);
  anInspector.Inspect(2);
  ASSERT_EQ(aCopy.GetShotCircles().Size(), 1);
  EXPECT_EQ(aCopy.GetShotCircles().First(), 1);
  aCopy.SetPoint(gp_XY(4.0, 0.0));
  aCopy.Inspect(1);
  aCopy.Inspect(2);
  ASSERT_EQ(aCopy.GetShotCircles().Size(), 1);
  EXPECT_EQ(aCopy.GetShotCircles().First(), 2);
}

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
    {
      p[j].SetCoord(static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX);
    }

    // Skip degenerate (nearly collinear) triangles - retry like the original.
    const gp_XY aV1 = p[1] - p[0];
    const gp_XY aV2 = p[2] - p[0];
    if (aV1.SquareModulus() <= aSqPrec || aV2.SquareModulus() <= aSqPrec || (aV1 ^ aV2) <= aMinArea)
    {
      continue;
    }

    ++i;

    gp_XY  aCenter;
    double aRadius = 0.0;
    if (!BRepMesh_CircleTool::MakeCircle(p[0], p[1], p[2], aCenter, aRadius))
    {
      continue;
    }

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
