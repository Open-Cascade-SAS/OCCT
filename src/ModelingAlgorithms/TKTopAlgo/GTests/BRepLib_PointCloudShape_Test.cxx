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

#include <BRepLib_PointCloudShape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

namespace
{
//! Minimal concrete subclass: the base class is abstract on addPoint alone.
class PointCounter : public BRepLib_PointCloudShape
{
public:
  PointCounter(const TopoDS_Shape& theShape)
      : BRepLib_PointCloudShape(theShape),
        myCount(0)
  {
  }

  int Count() const { return myCount; }

protected:
  void addPoint(const gp_Pnt&, const gp_Vec&, const gp_Pnt2d&, const TopoDS_Shape&) override
  {
    ++myCount;
  }

private:
  int myCount;
};
} // namespace

// Density 0 asks for the density to be computed. NbPointsByDensity then sized every face from the
// caller's 0 rather than from the computed value, so anArea / 0 saturated the per-face count and
// the total ran to billions of points.
TEST(BRepLib_PointCloudShapeTest, NbPointsByDensityUsesTheComputedDensity)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();
  PointCounter aCloud(aBox);

  const int aNbAuto = aCloud.NbPointsByDensity(0.);
  EXPECT_GT(aNbAuto, 0);
  // 10 points per minimal unreduced face area, per the header: a box of six equal faces asks for
  // tens of points, not billions. The unfixed code answered INT_MAX per face.
  EXPECT_LT(aNbAuto, 100000);
}

// The count must not depend on how the same density was arrived at.
TEST(BRepLib_PointCloudShapeTest, AutoDensityMatchesTheEquivalentExplicitDensity)
{
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();

  PointCounter aAuto(aBox);
  const int    aNbAuto = aAuto.NbPointsByDensity(0.);

  // Six 10x10 faces, minimal area 100, ten points per face area: the same density, given.
  PointCounter aExplicit(aBox);
  const int    aNbExplicit = aExplicit.NbPointsByDensity(10.);

  EXPECT_EQ(aNbAuto, aNbExplicit);
}
