// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Poly_ArrayOfNodes.hxx>
#include <Poly_ArrayOfUVNodes.hxx>

#include <gtest/gtest.h>

TEST(Poly_ArrayOfNodesTest, AssignBetweenPrecisions)
{
  NCollection_Vec3<float> aFloatNodes[] = {NCollection_Vec3<float>(1.0f, 2.0f, 3.0f),
                                           NCollection_Vec3<float>(4.0f, 5.0f, 6.0f)};
  Poly_ArrayOfNodes       aFloatSource(aFloatNodes[0], 2);
  Poly_ArrayOfNodes       aDoubleTarget(2);

  aDoubleTarget.Assign(aFloatSource);
  EXPECT_TRUE(aDoubleTarget.Value(size_t{0}).IsEqual(gp_Pnt(1.0, 2.0, 3.0), 0.0));
  EXPECT_TRUE(aDoubleTarget.Value(size_t{1}).IsEqual(gp_Pnt(4.0, 5.0, 6.0), 0.0));

  gp_Pnt            aDoubleNodes[] = {{7.0, 8.0, 9.0}, {10.0, 11.0, 12.0}};
  Poly_ArrayOfNodes aDoubleSource(aDoubleNodes[0], 2);
  Poly_ArrayOfNodes aFloatTarget;
  aFloatTarget.SetDoublePrecision(false);
  aFloatTarget.Resize(size_t{2}, false);

  aFloatTarget.Assign(aDoubleSource);
  EXPECT_TRUE(aFloatTarget.Value(size_t{0}).IsEqual(gp_Pnt(7.0, 8.0, 9.0), 0.0));
  EXPECT_TRUE(aFloatTarget.Value(size_t{1}).IsEqual(gp_Pnt(10.0, 11.0, 12.0), 0.0));
}

TEST(Poly_ArrayOfUVNodesTest, AssignBetweenPrecisions)
{
  NCollection_Vec2<float> aFloatNodes[] = {NCollection_Vec2<float>(1.0f, 2.0f),
                                           NCollection_Vec2<float>(3.0f, 4.0f)};
  Poly_ArrayOfUVNodes     aFloatSource(aFloatNodes[0], 2);
  Poly_ArrayOfUVNodes     aDoubleTarget(2);

  aDoubleTarget.Assign(aFloatSource);
  EXPECT_TRUE(aDoubleTarget.Value(size_t{0}).IsEqual(gp_Pnt2d(1.0, 2.0), 0.0));
  EXPECT_TRUE(aDoubleTarget.Value(size_t{1}).IsEqual(gp_Pnt2d(3.0, 4.0), 0.0));

  gp_Pnt2d            aDoubleNodes[] = {{5.0, 6.0}, {7.0, 8.0}};
  Poly_ArrayOfUVNodes aDoubleSource(aDoubleNodes[0], 2);
  Poly_ArrayOfUVNodes aFloatTarget;
  aFloatTarget.SetDoublePrecision(false);
  aFloatTarget.Resize(size_t{2}, false);

  aFloatTarget.Assign(aDoubleSource);
  EXPECT_TRUE(aFloatTarget.Value(size_t{0}).IsEqual(gp_Pnt2d(5.0, 6.0), 0.0));
  EXPECT_TRUE(aFloatTarget.Value(size_t{1}).IsEqual(gp_Pnt2d(7.0, 8.0), 0.0));
}
