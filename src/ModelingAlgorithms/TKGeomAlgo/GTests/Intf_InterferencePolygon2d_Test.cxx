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

#include <Intf_InterferencePolygon2d.hxx>
#include <Intf_Polygon2d.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>

#include <algorithm>
#include <array>

#include <gtest/gtest.h>

namespace
{
//! Exact test polyline, including empty input and closed endpoint handling.
class Intf_TestPolygon2d : public Intf_Polygon2d
{
public:
  //! Copy vertices so temporary array views can be passed safely.
  Intf_TestPolygon2d(const NCollection_Array1<gp_Pnt2d>& thePoints, const bool theClosed = false);
  //! Return the closed-polygon flag.
  bool Closed() const override;
  //! Segments are exact; no deflection allowance is needed.
  double DeflectionOverEstimation() const override;
  //! Empty input has no segments.
  int NbSegments() const override;
  //! Return the endpoints of a segment using one-based indices.
  void Segment(const int theIndex, gp_Pnt2d& theFirst, gp_Pnt2d& theLast) const override;

private:
  NCollection_Array1<gp_Pnt2d> myPoints; //!< Vertices, including the closing vertex.
  bool                         myClosed; //!< Whether the first and last segments are adjacent.
};

//==================================================================================================

Intf_TestPolygon2d::Intf_TestPolygon2d(const NCollection_Array1<gp_Pnt2d>& thePoints,
                                       const bool                          theClosed)
    : myPoints(thePoints),
      myClosed(theClosed)
{
  for (const gp_Pnt2d& aPoint : myPoints)
  {
    myBox.Add(aPoint);
  }
}

//==================================================================================================

bool Intf_TestPolygon2d::Closed() const
{
  return myClosed;
}

//==================================================================================================

double Intf_TestPolygon2d::DeflectionOverEstimation() const
{
  return 0.0;
}

//==================================================================================================

int Intf_TestPolygon2d::NbSegments() const
{
  return std::max(0, static_cast<int>(myPoints.Size()) - 1);
}

//==================================================================================================

void Intf_TestPolygon2d::Segment(const int theIndex, gp_Pnt2d& theFirst, gp_Pnt2d& theLast) const
{
  theFirst = myPoints.At(static_cast<size_t>(theIndex - 1));
  theLast  = myPoints.At(static_cast<size_t>(theIndex));
}
} // namespace

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, CrossingPolylinesPreserveSegmentOrder)
{
  NCollection_Array1<gp_Pnt2d> aZigzagPoints(size_t(4));
  aZigzagPoints.ChangeAt(0) = gp_Pnt2d(-1, -1);
  aZigzagPoints.ChangeAt(1) = gp_Pnt2d(1, 1);
  aZigzagPoints.ChangeAt(2) = gp_Pnt2d(3, -1);
  aZigzagPoints.ChangeAt(3) = gp_Pnt2d(5, 1);
  NCollection_Array1<gp_Pnt2d> aLinePoints(size_t(2));
  aLinePoints.ChangeAt(0) = gp_Pnt2d(-2, 0);
  aLinePoints.ChangeAt(1) = gp_Pnt2d(6, 0);
  const Intf_TestPolygon2d         aZigzag(aZigzagPoints);
  const Intf_TestPolygon2d         aLine(aLinePoints);
  const Intf_InterferencePolygon2d anIntersection(aZigzag, aLine);
  ASSERT_EQ(anIntersection.NbSectionPoints(), 3);
  EXPECT_EQ(anIntersection.NbTangentZones(), 0);
  for (int aPointIndex = 1; aPointIndex <= 3; ++aPointIndex)
  {
    EXPECT_NEAR(anIntersection.Pnt2dValue(aPointIndex).X(), 2.0 * (aPointIndex - 1), 1.e-14);
    EXPECT_NEAR(anIntersection.Pnt2dValue(aPointIndex).Y(), 0.0, 1.e-14);
  }

  const Intf_InterferencePolygon2d aReverse(aLine, aZigzag);
  ASSERT_EQ(aReverse.NbSectionPoints(), 3);
  for (int aPointIndex = 1; aPointIndex <= 3; ++aPointIndex)
  {
    EXPECT_NEAR(aReverse.Pnt2dValue(aPointIndex).Distance(anIntersection.Pnt2dValue(aPointIndex)),
                0.0,
                1.e-14);
  }
}

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, ClosedBowTieHasOneSelfIntersection)
{
  NCollection_Array1<gp_Pnt2d> aPoints(size_t(5));
  aPoints.ChangeAt(0) = gp_Pnt2d(-1, -1);
  aPoints.ChangeAt(1) = gp_Pnt2d(1, 1);
  aPoints.ChangeAt(2) = gp_Pnt2d(-1, 1);
  aPoints.ChangeAt(3) = gp_Pnt2d(1, -1);
  aPoints.ChangeAt(4) = aPoints.At(0);
  const Intf_TestPolygon2d         aPolygon(aPoints, true);
  const Intf_InterferencePolygon2d anIntersection(aPolygon);
  ASSERT_EQ(anIntersection.NbSectionPoints(), 1);
  EXPECT_NEAR(anIntersection.Pnt2dValue(1).Distance(gp_Pnt2d(0, 0)), 0.0, 1.e-14);
  EXPECT_EQ(anIntersection.NbTangentZones(), 0);
}

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, EmptySelfIntersection)
{
  const NCollection_Array1<gp_Pnt2d> aPoints;
  const Intf_TestPolygon2d           aPolygon(aPoints);
  const Intf_InterferencePolygon2d   anIntersection(aPolygon);
  EXPECT_EQ(anIntersection.NbSectionPoints(), 0);
  EXPECT_EQ(anIntersection.NbTangentZones(), 0);
}

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, ClosedPolygonDoesNotReportItsClosureVertex)
{
  NCollection_Array1<gp_Pnt2d> aPoints(size_t(4));
  aPoints.ChangeAt(0) = gp_Pnt2d(0, 0);
  aPoints.ChangeAt(1) = gp_Pnt2d(2, 0);
  aPoints.ChangeAt(2) = gp_Pnt2d(1, 1);
  aPoints.ChangeAt(3) = aPoints.At(0);
  const Intf_TestPolygon2d         aPolygon(aPoints, true);
  const Intf_InterferencePolygon2d anIntersection(aPolygon);
  EXPECT_EQ(anIntersection.NbSectionPoints(), 0);
  EXPECT_EQ(anIntersection.NbTangentZones(), 0);
}

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, ReuseRefreshesClosureSegmentCount)
{
  const std::array<gp_Pnt2d, 5> aBowTie = {
    {gp_Pnt2d(-1, -1), gp_Pnt2d(1, 1), gp_Pnt2d(-1, 1), gp_Pnt2d(1, -1), gp_Pnt2d(-1, -1)}};
  const std::array<gp_Pnt2d, 4> aTriangle = {
    {gp_Pnt2d(0, 0), gp_Pnt2d(2, 0), gp_Pnt2d(1, 1), gp_Pnt2d(0, 0)}};
  const Intf_TestPolygon2d   aCrossing(NCollection_Array1<gp_Pnt2d>(aBowTie[0], 1, 5), true);
  const Intf_TestPolygon2d   aSimple(NCollection_Array1<gp_Pnt2d>(aTriangle[0], 1, 4), true);
  Intf_InterferencePolygon2d anIntersection(aSimple, aCrossing);
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    anIntersection.Perform(aCrossing);
    ASSERT_EQ(anIntersection.NbSectionPoints(), 1);
    EXPECT_NEAR(anIntersection.Pnt2dValue(1).Distance(gp_Pnt2d(0, 0)), 0.0, 1.e-14);
    anIntersection.Perform(aSimple);
    EXPECT_EQ(anIntersection.NbSectionPoints(), 0);
  }
  const NCollection_Array1<gp_Pnt2d> anEmpty;
  anIntersection.Perform(Intf_TestPolygon2d(anEmpty));
  EXPECT_EQ(anIntersection.NbSectionPoints(), 0);
}

//==================================================================================================

TEST(Intf_InterferencePolygon2d_Test, OpenBowTieRetainsFirstSegmentCrossing)
{
  const std::array<gp_Pnt2d, 4> aPoints = {
    {gp_Pnt2d(-1, -1), gp_Pnt2d(1, 1), gp_Pnt2d(-1, 1), gp_Pnt2d(1, -1)}};
  const Intf_TestPolygon2d         aPolygon(NCollection_Array1<gp_Pnt2d>(aPoints[0], 1, 4));
  const Intf_InterferencePolygon2d anIntersection(aPolygon);
  ASSERT_EQ(anIntersection.NbSectionPoints(), 1);
  EXPECT_NEAR(anIntersection.Pnt2dValue(1).Distance(gp_Pnt2d(0, 0)), 0.0, 1.e-14);
}
