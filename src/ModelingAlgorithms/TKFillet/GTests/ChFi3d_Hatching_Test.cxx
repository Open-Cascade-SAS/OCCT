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

#include <ChFi3d_Builder_0.hxx>

#include <GC_MakeSegment2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2d_Line.hxx>
#include <HatchGen_Domain.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

namespace
{

void AddContour(Geom2dHatch_Hatcher& theHatcher, const NCollection_Array1<gp_Pnt2d>& thePoints)
{
  for (int anIndex = thePoints.Lower(); anIndex <= thePoints.Upper(); ++anIndex)
  {
    const int aNextIndex = anIndex == thePoints.Upper() ? thePoints.Lower() : anIndex + 1;
    theHatcher.AddElement(GC_MakeSegment2d(thePoints(anIndex), thePoints(aNextIndex)).Value(),
                          TopAbs_FORWARD);
  }
}

int AddHorizontalHatching(Geom2dHatch_Hatcher& theHatcher)
{
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
  return theHatcher.Trim(Geom2dAdaptor_Curve(aLine));
}

} // namespace

TEST(ChFi3d_HatchingTest, CoincidentSegmentFormsLimitingDomain)
{
  constexpr double        aTolerance = 1.0e-7;
  Geom2dHatch_Intersector anIntersector(aTolerance, aTolerance);
  Geom2dHatch_Hatcher     aHatcher(anIntersector, aTolerance, aTolerance);

  NCollection_Array1<gp_Pnt2d> aContour(1, 4);
  aContour(1) = gp_Pnt2d(0.0, 0.0);
  aContour(2) = gp_Pnt2d(10.0, 0.0);
  aContour(3) = gp_Pnt2d(10.0, 10.0);
  aContour(4) = gp_Pnt2d(0.0, 10.0);
  AddContour(aHatcher, aContour);

  const int aHatchingIndex = AddHorizontalHatching(aHatcher);
  ASSERT_TRUE(ChFi3d_ComputeHatchingDomains(aHatcher, aHatchingIndex));
  ASSERT_EQ(aHatcher.NbDomains(aHatchingIndex), 1);

  const HatchGen_Domain& aDomain = aHatcher.Domain(aHatchingIndex, 1);
  ASSERT_TRUE(aDomain.HasFirstPoint());
  ASSERT_TRUE(aDomain.HasSecondPoint());
  EXPECT_NEAR(aDomain.FirstPoint().Parameter(), 0.0, Precision::PConfusion());
  EXPECT_NEAR(aDomain.SecondPoint().Parameter(), 10.0, Precision::PConfusion());
}

TEST(ChFi3d_HatchingTest, BoundaryOverlapDoesNotDuplicateInteriorDomain)
{
  constexpr double        aTolerance = 1.0e-7;
  Geom2dHatch_Intersector anIntersector(aTolerance, aTolerance);
  Geom2dHatch_Hatcher     aHatcher(anIntersector, aTolerance, aTolerance);

  NCollection_Array1<gp_Pnt2d> aContour(1, 6);
  aContour(1) = gp_Pnt2d(-10.0, -10.0);
  aContour(2) = gp_Pnt2d(0.0, -10.0);
  aContour(3) = gp_Pnt2d(0.0, 0.0);
  aContour(4) = gp_Pnt2d(10.0, 0.0);
  aContour(5) = gp_Pnt2d(10.0, 10.0);
  aContour(6) = gp_Pnt2d(-10.0, 10.0);
  AddContour(aHatcher, aContour);

  const int aHatchingIndex = AddHorizontalHatching(aHatcher);
  ASSERT_TRUE(ChFi3d_ComputeHatchingDomains(aHatcher, aHatchingIndex));
  ASSERT_EQ(aHatcher.NbDomains(aHatchingIndex), 1);

  const HatchGen_Domain& aDomain = aHatcher.Domain(aHatchingIndex, 1);
  ASSERT_TRUE(aDomain.HasFirstPoint());
  ASSERT_TRUE(aDomain.HasSecondPoint());
  EXPECT_NEAR(aDomain.FirstPoint().Parameter(), -10.0, Precision::PConfusion());
  EXPECT_NEAR(aDomain.SecondPoint().Parameter(), 0.0, Precision::PConfusion());
}
