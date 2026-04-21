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

#include <Bnd_Box.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Flipper.hxx>
#include <NCollection_Mat4.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

namespace
{

static bool MatricesNear(const NCollection_Mat4<double>& theLhs,
                         const NCollection_Mat4<double>& theRhs,
                         const double                    theTol)
{
  for (int aCol = 0; aCol < 4; ++aCol)
  {
    for (int aRow = 0; aRow < 4; ++aRow)
    {
      if (std::abs(theLhs.GetValue(aRow, aCol) - theRhs.GetValue(aRow, aCol)) > theTol)
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace

// Identity world-view keeps the reference system aligned with the world axes, so no axis is
// detected as reversed and Compute() must return identity (pass-through).
TEST(Graphic3d_FlipperTest, Compute_IdentityWorldView_ReturnsIdentity)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  NCollection_Mat4<double> aWorldView; // identity
  const NCollection_Mat4<double> aResult = aFlipper.Compute(aWorldView);
  EXPECT_TRUE(aResult.IsIdentity()) << "Aligned ref system under identity WV should yield no flip";
}

// A 180 degree rotation around Z in the world view inverts both X and Y of the reference system,
// triggering the (isReversedX || isReversedY) && !isReversedZ branch. The resulting flip is a
// 180 degree rotation, which is self-inverse: applying it twice must return the identity.
TEST(Graphic3d_FlipperTest, Compute_180AroundZ_IsSelfInverse)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  NCollection_Mat4<double> aWorldView; // identity
  aWorldView.SetValue(0, 0, -1.0);
  aWorldView.SetValue(1, 1, -1.0);

  const NCollection_Mat4<double> aFlip = aFlipper.Compute(aWorldView);
  ASSERT_FALSE(aFlip.IsIdentity()) << "180 around Z must trigger a flip";

  const NCollection_Mat4<double> aTwice = aFlip * aFlip;
  NCollection_Mat4<double>       aIdentity;
  EXPECT_TRUE(MatricesNear(aTwice, aIdentity, Precision::Confusion()))
    << "Flip matrix must be self-inverse";
}

// Apply() on a box with no detected flip must leave the box untouched.
TEST(Graphic3d_FlipperTest, Apply_NoFlip_BoxUnchanged)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  Bnd_Box aBox;
  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

  NCollection_Mat4<double> aWorldView;
  aFlipper.Apply(aWorldView, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  EXPECT_NEAR(aXmin, 1.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 2.0, Precision::Confusion());
  EXPECT_NEAR(aZmin, 3.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, 4.0, Precision::Confusion());
  EXPECT_NEAR(aYmax, 5.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 6.0, Precision::Confusion());
}

// Apply() on a void box is a no-op.
TEST(Graphic3d_FlipperTest, Apply_VoidBox_RemainsVoid)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  Bnd_Box aBox;
  EXPECT_TRUE(aBox.IsVoid());

  NCollection_Mat4<double> aWorldView;
  aWorldView.SetValue(0, 0, -1.0);
  aWorldView.SetValue(1, 1, -1.0);

  aFlipper.Apply(aWorldView, aBox);
  EXPECT_TRUE(aBox.IsVoid()) << "Flipper must not materialize a void box";
}

// When flip is active, applying it twice to a bounding box must recover the original bounds
// (mirroring the self-inverse property at the box level).
TEST(Graphic3d_FlipperTest, Apply_180AroundZ_Involution)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  Bnd_Box aBox;
  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

  NCollection_Mat4<double> aWorldView;
  aWorldView.SetValue(0, 0, -1.0);
  aWorldView.SetValue(1, 1, -1.0);

  aFlipper.Apply(aWorldView, aBox);
  aFlipper.Apply(aWorldView, aBox);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  EXPECT_NEAR(aXmin, 1.0, Precision::Confusion());
  EXPECT_NEAR(aYmin, 2.0, Precision::Confusion());
  EXPECT_NEAR(aZmin, 3.0, Precision::Confusion());
  EXPECT_NEAR(aXmax, 4.0, Precision::Confusion());
  EXPECT_NEAR(aYmax, 5.0, Precision::Confusion());
  EXPECT_NEAR(aZmax, 6.0, Precision::Confusion());
}

// Regression for the MV-contract: the matrix passed to Compute() must include the
// object's ModelWorld, not just the camera's WorldView. Here the raw WorldView is
// identity, so no reversal is detected; but once we fold in a 180 deg Z-rotation
// (the object's transformation), Compute() returns a non-identity flip. This locks
// in that selection callers must pass WorldView * ModelWorld to match OpenGl_Flipper.
TEST(Graphic3d_FlipperTest, Compute_ObjectTransformChangesDecision)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  NCollection_Mat4<double> aWorldView; // identity
  EXPECT_TRUE(aFlipper.Compute(aWorldView).IsIdentity())
    << "Identity WV keeps ref system aligned, no flip";

  // Object rotated 180 deg about Z — composing this into MV inverts X and Y,
  // which is exactly the trigger for the (isReversedX || isReversedY) && !isReversedZ
  // branch in Compute(). The resulting matrix must not be identity.
  NCollection_Mat4<double> anObjTrsf;
  anObjTrsf.SetValue(0, 0, -1.0);
  anObjTrsf.SetValue(1, 1, -1.0);

  const NCollection_Mat4<double> aMV   = aWorldView * anObjTrsf;
  const NCollection_Mat4<double> aFlip = aFlipper.Compute(aMV);
  EXPECT_FALSE(aFlip.IsIdentity())
    << "Folding object transform must surface a flip the raw WV misses";
}

// RefPlane() round-trip via SetRefPlane() preserves the axis.
TEST(Graphic3d_FlipperTest, SetRefPlane_RoundTrip)
{
  Graphic3d_Flipper aFlipper(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)));

  const gp_Ax2 aNewPlane(gp_Pnt(10, 20, 30), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0));
  aFlipper.SetRefPlane(aNewPlane);

  const gp_Ax2 aRead = aFlipper.RefPlane();
  EXPECT_NEAR(aRead.Location().X(), 10.0, Precision::Confusion());
  EXPECT_NEAR(aRead.Location().Y(), 20.0, Precision::Confusion());
  EXPECT_NEAR(aRead.Location().Z(), 30.0, Precision::Confusion());
}
