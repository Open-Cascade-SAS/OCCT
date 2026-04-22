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

#include <Aspect_CircularGrid.hxx>
#include <Aspect_RectangularGrid.hxx>

#include <gtest/gtest.h>

namespace
{
// Concrete shims: Aspect_RectangularGrid / Aspect_CircularGrid are abstract
// because Display/Erase/IsDisplayed/UpdateDisplay are pure virtual on the
// base. V3d_RectangularGrid supplies them in production, but pulling V3d into
// this test would need a live viewer/window. Since we only exercise the new
// bounds accessors, a minimal stub is enough.
class RectGridStub : public Aspect_RectangularGrid
{
public:
  RectGridStub(const double theXStep, const double theYStep)
      : Aspect_RectangularGrid(theXStep, theYStep)
  {
  }
  void Display() override {}
  void Erase() const override {}
  bool IsDisplayed() const override { return false; }

protected:
  void UpdateDisplay() override {}
};

class CircGridStub : public Aspect_CircularGrid
{
public:
  CircGridStub(const double theRadiusStep, const int theDivisions)
      : Aspect_CircularGrid(theRadiusStep, theDivisions)
  {
  }
  void Display() override {}
  void Erase() const override {}
  bool IsDisplayed() const override { return false; }

protected:
  void UpdateDisplay() override {}
};
} // namespace

TEST(Aspect_GridBoundsTest, Rectangular_Defaults)
{
  RectGridStub aGrid(1.0, 1.0);
  EXPECT_DOUBLE_EQ(0.0, aGrid.SizeX());
  EXPECT_DOUBLE_EQ(0.0, aGrid.SizeY());
  EXPECT_DOUBLE_EQ(0.0, aGrid.ZOffset());
}

TEST(Aspect_GridBoundsTest, Rectangular_RoundTrip)
{
  RectGridStub aGrid(1.0, 1.0);
  aGrid.SetSizeX(8.0);
  aGrid.SetSizeY(4.0);
  aGrid.SetZOffset(-0.005);
  EXPECT_DOUBLE_EQ(8.0, aGrid.SizeX());
  EXPECT_DOUBLE_EQ(4.0, aGrid.SizeY());
  EXPECT_DOUBLE_EQ(-0.005, aGrid.ZOffset());
}

TEST(Aspect_GridBoundsTest, Circular_Defaults)
{
  CircGridStub aGrid(1.0, 8);
  EXPECT_DOUBLE_EQ(0.0, aGrid.Radius());
  EXPECT_DOUBLE_EQ(0.0, aGrid.ZOffset());
  EXPECT_DOUBLE_EQ(0.0, aGrid.AngleStart());
  EXPECT_DOUBLE_EQ(0.0, aGrid.AngleEnd());
  EXPECT_FALSE(aGrid.IsArc());
}

TEST(Aspect_GridBoundsTest, Circular_RoundTrip)
{
  CircGridStub aGrid(1.0, 8);
  aGrid.SetRadius(2.5);
  aGrid.SetZOffset(-0.005);
  EXPECT_DOUBLE_EQ(2.5, aGrid.Radius());
  EXPECT_DOUBLE_EQ(-0.005, aGrid.ZOffset());
}

TEST(Aspect_GridBoundsTest, Circular_ArcRange)
{
  CircGridStub aGrid(1.0, 8);
  EXPECT_FALSE(aGrid.IsArc());

  aGrid.SetArcRange(0.0, M_PI);
  EXPECT_TRUE(aGrid.IsArc());
  EXPECT_DOUBLE_EQ(0.0, aGrid.AngleStart());
  EXPECT_DOUBLE_EQ(M_PI, aGrid.AngleEnd());

  aGrid.SetArcRange(0.0, 0.0);
  EXPECT_FALSE(aGrid.IsArc()) << "equal start/end is the full-circle sentinel";

  aGrid.SetArcRange(2.356, -2.356);
  EXPECT_TRUE(aGrid.IsArc()) << "wraparound (Start > End) is valid";
}
