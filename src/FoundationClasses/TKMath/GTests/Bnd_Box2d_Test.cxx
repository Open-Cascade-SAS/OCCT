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

#include <Bnd_Box2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>

#include <gtest/gtest.h>

TEST(Bnd_Box2dTest, DefaultConstructor)
{
  Bnd_Box2d aBox;
  EXPECT_TRUE(aBox.IsVoid());
  EXPECT_FALSE(aBox.IsWhole());
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 0.0);
}

TEST(Bnd_Box2dTest, SetVoid)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  EXPECT_FALSE(aBox.IsVoid());
  aBox.SetVoid();
  EXPECT_TRUE(aBox.IsVoid());
}

TEST(Bnd_Box2dTest, SetWhole)
{
  Bnd_Box2d aBox;
  aBox.SetWhole();
  EXPECT_TRUE(aBox.IsWhole());
  EXPECT_FALSE(aBox.IsVoid());
}

TEST(Bnd_Box2dTest, Update_Bounds)
{
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 10.0, 20.0);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 1.0);
  EXPECT_DOUBLE_EQ(aYmin, 2.0);
  EXPECT_DOUBLE_EQ(aXmax, 10.0);
  EXPECT_DOUBLE_EQ(aYmax, 20.0);
}

TEST(Bnd_Box2dTest, Update_SinglePoint)
{
  Bnd_Box2d aBox;
  aBox.Update(5.0, 7.0);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 5.0);
  EXPECT_DOUBLE_EQ(aYmin, 7.0);
  EXPECT_DOUBLE_EQ(aXmax, 5.0);
  EXPECT_DOUBLE_EQ(aYmax, 7.0);
}

TEST(Bnd_Box2dTest, Update_Expansion)
{
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 10.0, 20.0);
  aBox.Update(0.0, 0.0, 15.0, 25.0);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 0.0);
  EXPECT_DOUBLE_EQ(aYmin, 0.0);
  EXPECT_DOUBLE_EQ(aXmax, 15.0);
  EXPECT_DOUBLE_EQ(aYmax, 25.0);
}

TEST(Bnd_Box2dTest, GapOperations)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 0.0);
  aBox.SetGap(2.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 2.0);
  aBox.SetGap(-3.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 3.0);
}

TEST(Bnd_Box2dTest, Enlarge)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  aBox.Enlarge(5.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 5.0);
  aBox.Enlarge(3.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 5.0); // max(5, 3) = 5
  aBox.Enlarge(7.0);
  EXPECT_DOUBLE_EQ(aBox.GetGap(), 7.0); // max(5, 7) = 7
}

TEST(Bnd_Box2dTest, Get_WithGap)
{
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 10.0, 20.0);
  aBox.SetGap(0.5);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 0.5);
  EXPECT_DOUBLE_EQ(aYmin, 1.5);
  EXPECT_DOUBLE_EQ(aXmax, 10.5);
  EXPECT_DOUBLE_EQ(aYmax, 20.5);
}

TEST(Bnd_Box2dTest, Get_StructuredBindings)
{
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 10.0, 20.0);
  const auto [aXmin, aXmax, aYmin, aYmax] = aBox.Get();
  EXPECT_DOUBLE_EQ(aXmin, 1.0);
  EXPECT_DOUBLE_EQ(aXmax, 10.0);
  EXPECT_DOUBLE_EQ(aYmin, 2.0);
  EXPECT_DOUBLE_EQ(aYmax, 20.0);
}

TEST(Bnd_Box2dTest, Set_Point)
{
  Bnd_Box2d aBox;
  aBox.Set(gp_Pnt2d(5.0, 7.0));
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 5.0);
  EXPECT_DOUBLE_EQ(aYmin, 7.0);
  EXPECT_DOUBLE_EQ(aXmax, 5.0);
  EXPECT_DOUBLE_EQ(aYmax, 7.0);
}

TEST(Bnd_Box2dTest, Add_Point)
{
  Bnd_Box2d aBox;
  aBox.Add(gp_Pnt2d(1.0, 2.0));
  aBox.Add(gp_Pnt2d(10.0, 20.0));
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 1.0);
  EXPECT_DOUBLE_EQ(aYmin, 2.0);
  EXPECT_DOUBLE_EQ(aXmax, 10.0);
  EXPECT_DOUBLE_EQ(aYmax, 20.0);
}

TEST(Bnd_Box2dTest, Add_Box)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 5.0, 5.0);
  Bnd_Box2d aBox2;
  aBox2.Update(3.0, 3.0, 10.0, 10.0);
  aBox1.Add(aBox2);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox1.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 0.0);
  EXPECT_DOUBLE_EQ(aYmin, 0.0);
  EXPECT_DOUBLE_EQ(aXmax, 10.0);
  EXPECT_DOUBLE_EQ(aYmax, 10.0);
}

TEST(Bnd_Box2dTest, Add_VoidBox)
{
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 5.0, 6.0);
  Bnd_Box2d aVoidBox;
  aBox.Add(aVoidBox);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 1.0);
  EXPECT_DOUBLE_EQ(aYmin, 2.0);
}

TEST(Bnd_Box2dTest, Add_ToVoidBox)
{
  Bnd_Box2d aVoidBox;
  Bnd_Box2d aBox;
  aBox.Update(1.0, 2.0, 5.0, 6.0);
  aVoidBox.Add(aBox);
  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aVoidBox.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 1.0);
  EXPECT_DOUBLE_EQ(aYmin, 2.0);
  EXPECT_DOUBLE_EQ(aXmax, 5.0);
  EXPECT_DOUBLE_EQ(aYmax, 6.0);
}

TEST(Bnd_Box2dTest, OpenDirections)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  EXPECT_FALSE(aBox.IsOpenXmin());
  EXPECT_FALSE(aBox.IsOpenXmax());
  EXPECT_FALSE(aBox.IsOpenYmin());
  EXPECT_FALSE(aBox.IsOpenYmax());

  aBox.OpenXmin();
  EXPECT_TRUE(aBox.IsOpenXmin());
  aBox.OpenYmax();
  EXPECT_TRUE(aBox.IsOpenYmax());
  EXPECT_FALSE(aBox.IsWhole());
}

TEST(Bnd_Box2dTest, IsOut_Point)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  EXPECT_FALSE(aBox.IsOut(gp_Pnt2d(5.0, 5.0)));
  EXPECT_TRUE(aBox.IsOut(gp_Pnt2d(15.0, 5.0)));
  EXPECT_TRUE(aBox.IsOut(gp_Pnt2d(-1.0, 5.0)));
}

TEST(Bnd_Box2dTest, IsOut_Box_Overlapping)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aBox2;
  aBox2.Update(5.0, 5.0, 15.0, 15.0);
  EXPECT_FALSE(aBox1.IsOut(aBox2));
}

TEST(Bnd_Box2dTest, IsOut_Box_Separated)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 5.0, 5.0);
  Bnd_Box2d aBox2;
  aBox2.Update(10.0, 10.0, 15.0, 15.0);
  EXPECT_TRUE(aBox1.IsOut(aBox2));
}

TEST(Bnd_Box2dTest, IsOut_Box_FastPath)
{
  // Both non-void, non-whole, non-open -> fast path
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aBox2;
  aBox2.Update(5.0, 5.0, 15.0, 15.0);
  EXPECT_FALSE(aBox1.IsOut(aBox2));

  Bnd_Box2d aBox3;
  aBox3.Update(20.0, 20.0, 30.0, 30.0);
  EXPECT_TRUE(aBox1.IsOut(aBox3));
}

TEST(Bnd_Box2dTest, IsOut_Box_FastPath_WithGap)
{
  // Boxes separated by 1.0, but gap bridges it
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 5.0, 5.0);
  aBox1.SetGap(1.0);
  Bnd_Box2d aBox2;
  aBox2.Update(6.0, 0.0, 10.0, 5.0);
  // Gap from box1 = 1.0, gap from box2 = 0.0, total = 1.0
  // Xmin1 - Xmax2 = 0 - 10 = -10 (not > 1.0) -> not out in X
  // Xmin2 - Xmax1 = 6 - 5 = 1 (not > 1.0, equal) -> not out
  EXPECT_FALSE(aBox1.IsOut(aBox2));

  // Now increase separation
  Bnd_Box2d aBox3;
  aBox3.Update(7.0, 0.0, 10.0, 5.0);
  // Xmin3 - Xmax1 = 7 - 5 = 2 > 1.0 -> out
  EXPECT_TRUE(aBox1.IsOut(aBox3));
}

TEST(Bnd_Box2dTest, IsOut_VoidBox)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aVoid;
  EXPECT_TRUE(aBox.IsOut(aVoid));
  EXPECT_TRUE(aVoid.IsOut(aBox));
}

TEST(Bnd_Box2dTest, IsOut_WholeBox)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aWhole;
  aWhole.SetWhole();
  EXPECT_FALSE(aBox.IsOut(aWhole));
  EXPECT_FALSE(aWhole.IsOut(aBox));
}

TEST(Bnd_Box2dTest, IsOut_OpenBox)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 10.0, 10.0);
  aBox1.OpenXmin(); // extends to -infinity in X

  Bnd_Box2d aBox2;
  aBox2.Update(-100.0, 0.0, -50.0, 10.0);

  // aBox1 is open in Xmin, so extends to -infinity: should overlap
  EXPECT_FALSE(aBox1.IsOut(aBox2));
}

TEST(Bnd_Box2dTest, IsOut_Line)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);

  // Line that intersects
  gp_Lin2d aLinInside(gp_Pnt2d(5.0, -5.0), gp_Dir2d(0.0, 1.0));
  EXPECT_FALSE(aBox.IsOut(aLinInside));

  // Line that doesn't intersect
  gp_Lin2d aLinOutside(gp_Pnt2d(20.0, -5.0), gp_Dir2d(0.0, 1.0));
  EXPECT_TRUE(aBox.IsOut(aLinOutside));
}

TEST(Bnd_Box2dTest, IsOut_Segment)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);

  // Segment that crosses
  EXPECT_FALSE(aBox.IsOut(gp_Pnt2d(5.0, -5.0), gp_Pnt2d(5.0, 15.0)));

  // Segment fully outside
  EXPECT_TRUE(aBox.IsOut(gp_Pnt2d(20.0, 0.0), gp_Pnt2d(20.0, 10.0)));
}

TEST(Bnd_Box2dTest, Transformed)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);

  gp_Trsf2d aTrsf;
  aTrsf.SetTranslation(gp_Pnt2d(0.0, 0.0), gp_Pnt2d(5.0, 5.0));
  Bnd_Box2d aTransformed = aBox.Transformed(aTrsf);

  double aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  aTransformed.Get(aXmin, aYmin, aXmax, aYmax);
  EXPECT_DOUBLE_EQ(aXmin, 5.0);
  EXPECT_DOUBLE_EQ(aYmin, 5.0);
  EXPECT_DOUBLE_EQ(aXmax, 15.0);
  EXPECT_DOUBLE_EQ(aYmax, 15.0);
}

TEST(Bnd_Box2dTest, SquareExtent)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 3.0, 4.0);
  // Diagonal^2 = 3^2 + 4^2 = 25
  EXPECT_DOUBLE_EQ(aBox.SquareExtent(), 25.0);
}

TEST(Bnd_Box2dTest, SquareExtent_Void)
{
  Bnd_Box2d aBox;
  EXPECT_DOUBLE_EQ(aBox.SquareExtent(), 0.0);
}

TEST(Bnd_Box2dTest, Add_Direction)
{
  Bnd_Box2d aBox;
  aBox.Set(gp_Pnt2d(5.0, 5.0));
  aBox.Add(gp_Dir2d(1.0, 0.0));
  EXPECT_TRUE(aBox.IsOpenXmax());
  EXPECT_FALSE(aBox.IsOpenXmin());
}

TEST(Bnd_Box2dTest, Contains_Point)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 10.0);
  EXPECT_TRUE(aBox.Contains(gp_Pnt2d(5.0, 5.0)));
  EXPECT_TRUE(aBox.Contains(gp_Pnt2d(0.0, 0.0)));
  EXPECT_FALSE(aBox.Contains(gp_Pnt2d(-1.0, 5.0)));
  EXPECT_FALSE(aBox.Contains(gp_Pnt2d(5.0, 11.0)));
}

TEST(Bnd_Box2dTest, Intersects_Box)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aBox2;
  aBox2.Update(5.0, 5.0, 15.0, 15.0);
  Bnd_Box2d aBox3;
  aBox3.Update(20.0, 20.0, 30.0, 30.0);
  EXPECT_TRUE(aBox1.Intersects(aBox2));
  EXPECT_FALSE(aBox1.Intersects(aBox3));
}

TEST(Bnd_Box2dTest, Distance_Separated)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 1.0, 1.0);
  Bnd_Box2d aBox2;
  aBox2.Update(4.0, 0.0, 5.0, 1.0);
  EXPECT_NEAR(aBox1.Distance(aBox2), 3.0, 1e-10);
}

TEST(Bnd_Box2dTest, Distance_Overlapping)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 10.0, 10.0);
  Bnd_Box2d aBox2;
  aBox2.Update(5.0, 5.0, 15.0, 15.0);
  EXPECT_DOUBLE_EQ(aBox1.Distance(aBox2), 0.0);
}

TEST(Bnd_Box2dTest, Distance_Diagonal)
{
  Bnd_Box2d aBox1;
  aBox1.Update(0.0, 0.0, 1.0, 1.0);
  Bnd_Box2d aBox2;
  aBox2.Update(4.0, 4.0, 5.0, 5.0);
  EXPECT_NEAR(aBox1.Distance(aBox2), std::sqrt(18.0), 1e-10);
}

TEST(Bnd_Box2dTest, Distance_Void)
{
  Bnd_Box2d aBox1;
  Bnd_Box2d aBox2;
  aBox2.Update(0.0, 0.0, 1.0, 1.0);
  EXPECT_DOUBLE_EQ(aBox1.Distance(aBox2), 0.0);
}

TEST(Bnd_Box2dTest, Center)
{
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 10.0, 20.0);
  auto aCenter = aBox.Center();
  ASSERT_TRUE(aCenter.has_value());
  EXPECT_DOUBLE_EQ(aCenter->X(), 5.0);
  EXPECT_DOUBLE_EQ(aCenter->Y(), 10.0);
}

TEST(Bnd_Box2dTest, Center_VoidNullopt)
{
  Bnd_Box2d aVoid;
  EXPECT_FALSE(aVoid.Center().has_value());
}
