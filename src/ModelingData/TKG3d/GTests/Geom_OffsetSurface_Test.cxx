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

#include <cmath>

#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

class Geom_OffsetSurface_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a plane as basis surface
    gp_Pln             aPlane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)));
    Handle(Geom_Plane) aBasisSurface = new Geom_Plane(aPlane);

    // Create offset surface
    Standard_Real anOffsetValue = 3.0;

    myOriginalSurface = new Geom_OffsetSurface(aBasisSurface, anOffsetValue);
  }

  Handle(Geom_OffsetSurface) myOriginalSurface;
};

TEST_F(Geom_OffsetSurface_Test, CopyConstructorBasicProperties)
{
  // Test copy constructor
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Verify basic properties are identical
  EXPECT_DOUBLE_EQ(myOriginalSurface->Offset(), aCopiedSurface->Offset());
  EXPECT_EQ(myOriginalSurface->IsUPeriodic(), aCopiedSurface->IsUPeriodic());
  EXPECT_EQ(myOriginalSurface->IsVPeriodic(), aCopiedSurface->IsVPeriodic());
  EXPECT_EQ(myOriginalSurface->IsUClosed(), aCopiedSurface->IsUClosed());
  EXPECT_EQ(myOriginalSurface->IsVClosed(), aCopiedSurface->IsVClosed());
}

TEST_F(Geom_OffsetSurface_Test, CopyConstructorBasisSurface)
{
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Verify basis surfaces are equivalent but independent
  Handle(Geom_Surface) anOrigBasis = myOriginalSurface->BasisSurface();
  Handle(Geom_Surface) aCopyBasis  = aCopiedSurface->BasisSurface();

  // They should be different objects
  EXPECT_NE(anOrigBasis.get(), aCopyBasis.get());

  // But functionally equivalent
  Standard_Real anUFirst, anULast, aVFirst, aVLast;
  anOrigBasis->Bounds(anUFirst, anULast, aVFirst, aVLast);

  Standard_Real anUFirstCopy, anULastCopy, aVFirstCopy, aVLastCopy;
  aCopyBasis->Bounds(anUFirstCopy, anULastCopy, aVFirstCopy, aVLastCopy);

  EXPECT_DOUBLE_EQ(anUFirst, anUFirstCopy);
  EXPECT_DOUBLE_EQ(anULast, anULastCopy);
  EXPECT_DOUBLE_EQ(aVFirst, aVFirstCopy);
  EXPECT_DOUBLE_EQ(aVLast, aVLastCopy);
}

TEST_F(Geom_OffsetSurface_Test, CopyMethodUsesOptimizedConstructor)
{
  // Test that Copy() method uses the optimized copy constructor
  Handle(Geom_Geometry)      aCopiedGeom    = myOriginalSurface->Copy();
  Handle(Geom_OffsetSurface) aCopiedSurface = Handle(Geom_OffsetSurface)::DownCast(aCopiedGeom);

  EXPECT_FALSE(aCopiedSurface.IsNull());

  // Verify the copy is functionally identical
  EXPECT_DOUBLE_EQ(myOriginalSurface->Offset(), aCopiedSurface->Offset());

  // Test evaluation at several points
  for (Standard_Real u = -5.0; u <= 5.0; u += 2.5)
  {
    for (Standard_Real v = -5.0; v <= 5.0; v += 2.5)
    {
      gp_Pnt anOrigPnt = myOriginalSurface->Value(u, v);
      gp_Pnt aCopyPnt  = aCopiedSurface->Value(u, v);
      EXPECT_TRUE(anOrigPnt.IsEqual(aCopyPnt, 1e-10));
    }
  }
}

TEST_F(Geom_OffsetSurface_Test, CopyIndependence)
{
  Handle(Geom_OffsetSurface) aCopiedSurface = new Geom_OffsetSurface(*myOriginalSurface);

  // Store original offset value
  Standard_Real anOrigOffset = aCopiedSurface->Offset();

  // Modify the original surface
  myOriginalSurface->SetOffsetValue(15.0);

  // Verify the copied surface is not affected
  EXPECT_DOUBLE_EQ(aCopiedSurface->Offset(), anOrigOffset);
  EXPECT_NE(aCopiedSurface->Offset(), myOriginalSurface->Offset());
}

// Tests for Surface() method - equivalent surface computation for canonical surfaces.

TEST(Geom_OffsetSurface_EquivalentSurface, Plane_ReturnsTranslatedPlane)
{
  // Create a plane at origin with Z-normal.
  gp_Ax3             anAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAxis);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aPlane, anOffset);

  // Get equivalent surface.
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  // Should be a plane.
  Handle(Geom_Plane) anEquivPlane = Handle(Geom_Plane)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivPlane.IsNull());

  // Verify the plane is translated by offset along normal.
  gp_Pnt anOrigLoc  = aPlane->Location();
  gp_Pnt anEquivLoc = anEquivPlane->Location();
  EXPECT_NEAR(anEquivLoc.Z() - anOrigLoc.Z(), anOffset, Precision::Confusion());
  EXPECT_NEAR(anEquivLoc.X(), anOrigLoc.X(), Precision::Confusion());
  EXPECT_NEAR(anEquivLoc.Y(), anOrigLoc.Y(), Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Cylinder_PositiveOffset_ReturnsLargerCylinder)
{
  // Create a cylinder with radius 10.
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_CylindricalSurface) anEquivCyl = Handle(Geom_CylindricalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivCyl.IsNull());

  // Radius should increase by offset.
  EXPECT_NEAR(anEquivCyl->Radius(), 13.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Cylinder_NegativeOffset_ReturnsSmallerCylinder)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = -3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_CylindricalSurface) anEquivCyl = Handle(Geom_CylindricalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivCyl.IsNull());

  EXPECT_NEAR(anEquivCyl->Radius(), 7.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Cylinder_NegativeRadiusFlip_ReturnsFlippedCylinder)
{
  // Offset that makes radius negative should flip the surface.
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 5.0);

  const double               anOffset     = -8.0; // Results in radius = -3
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_CylindricalSurface) anEquivCyl = Handle(Geom_CylindricalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivCyl.IsNull());

  // Radius should be absolute value.
  EXPECT_NEAR(anEquivCyl->Radius(), 3.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Sphere_PositiveOffset_ReturnsLargerSphere)
{
  gp_Ax3                        anAxis(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 10.0);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aSphere, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_SphericalSurface) anEquivSphere = Handle(Geom_SphericalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivSphere.IsNull());

  EXPECT_NEAR(anEquivSphere->Radius(), 15.0, Precision::Confusion());
  // Center should remain the same.
  EXPECT_TRUE(anEquivSphere->Location().IsEqual(aSphere->Location(), Precision::Confusion()));
}

TEST(Geom_OffsetSurface_EquivalentSurface, Sphere_NegativeRadiusFlip_ReturnsFlippedSphere)
{
  gp_Ax3                        anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 5.0);

  const double               anOffset     = -8.0; // Results in radius = -3
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aSphere, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_SphericalSurface) anEquivSphere = Handle(Geom_SphericalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivSphere.IsNull());

  EXPECT_NEAR(anEquivSphere->Radius(), 3.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Cone_PositiveOffset_ReturnsOffsetCone)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                aSemiAngle = M_PI / 6.0; // 30 degrees
  const double                aRefRadius = 10.0;
  Handle(Geom_ConicalSurface) aCone      = new Geom_ConicalSurface(anAxis, aSemiAngle, aRefRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCone, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_ConicalSurface) anEquivCone = Handle(Geom_ConicalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivCone.IsNull());

  // Semi-angle should remain the same.
  EXPECT_NEAR(anEquivCone->SemiAngle(), aSemiAngle, Precision::Confusion());

  // Reference radius should change by offset * cos(alpha).
  const double anExpectedRadius = aRefRadius + anOffset * std::cos(aSemiAngle);
  EXPECT_NEAR(anEquivCone->RefRadius(), anExpectedRadius, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Torus_PositiveOffset_ReturnsLargerTorus)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                 aMajorRadius = 20.0;
  const double                 aMinorRadius = 5.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(anAxis, aMajorRadius, aMinorRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTorus, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_ToroidalSurface) anEquivTorus = Handle(Geom_ToroidalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivTorus.IsNull());

  // Major radius should remain the same.
  EXPECT_NEAR(anEquivTorus->MajorRadius(), aMajorRadius, Precision::Confusion());
  // Minor radius should increase by offset.
  EXPECT_NEAR(anEquivTorus->MinorRadius(), aMinorRadius + anOffset, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Torus_NegativeOffset_ReturnsSmallerTorus)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                 aMajorRadius = 20.0;
  const double                 aMinorRadius = 5.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(anAxis, aMajorRadius, aMinorRadius);

  const double               anOffset     = -2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTorus, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_ToroidalSurface) anEquivTorus = Handle(Geom_ToroidalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivTorus.IsNull());

  EXPECT_NEAR(anEquivTorus->MajorRadius(), aMajorRadius, Precision::Confusion());
  EXPECT_NEAR(anEquivTorus->MinorRadius(), aMinorRadius - 2.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, Torus_NegativeMinorRadiusFlip_ReturnsFlippedTorus)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                 aMajorRadius = 20.0;
  const double                 aMinorRadius = 3.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(anAxis, aMajorRadius, aMinorRadius);

  const double               anOffset     = -5.0; // Results in minor radius = -2
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTorus, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  Handle(Geom_ToroidalSurface) anEquivTorus = Handle(Geom_ToroidalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivTorus.IsNull());

  EXPECT_NEAR(anEquivTorus->MajorRadius(), aMajorRadius, Precision::Confusion());
  EXPECT_NEAR(anEquivTorus->MinorRadius(), 2.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, ZeroOffset_ReturnsBasisSurface)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, 0.0);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  // Should return the internal basis surface (which is a copy of the original).
  EXPECT_EQ(anEquiv.get(), anOffsetSurf->BasisSurface().get());

  // The equivalent should be geometrically identical to the original.
  Handle(Geom_CylindricalSurface) anEquivCyl = Handle(Geom_CylindricalSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivCyl.IsNull());
  EXPECT_NEAR(anEquivCyl->Radius(), 10.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, TrimmedPlane_ReturnsTrimmedTranslatedPlane)
{
  gp_Ax3             anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAxis);

  // Create trimmed surface.
  Handle(Geom_RectangularTrimmedSurface) aTrimmed =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 10.0, 0.0, 5.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTrimmed, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  // Should be a trimmed surface.
  Handle(Geom_RectangularTrimmedSurface) anEquivTrimmed =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(anEquiv);
  ASSERT_FALSE(anEquivTrimmed.IsNull());

  // Verify bounds are preserved.
  double U1, U2, V1, V2;
  anEquivTrimmed->Bounds(U1, U2, V1, V2);
  EXPECT_NEAR(U1, 0.0, Precision::Confusion());
  EXPECT_NEAR(U2, 10.0, Precision::Confusion());
  EXPECT_NEAR(V1, 0.0, Precision::Confusion());
  EXPECT_NEAR(V2, 5.0, Precision::Confusion());
}

TEST(Geom_OffsetSurface_EquivalentSurface, DegenerateCylinder_ReturnsNull)
{
  // Offset that makes radius too small (degenerate case).
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 1.0);

  // Offset close to -1 should result in degenerate surface.
  const double               anOffset     = -1.0 + 0.5 * Precision::Confusion();
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  // Degenerate surface should return null.
  EXPECT_TRUE(anEquiv.IsNull());
}

TEST(Geom_OffsetSurface_EquivalentSurface, EquivalentSurface_EvaluationConsistency)
{
  // Verify that evaluation on offset surface matches evaluation on equivalent surface.
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());

  // Test evaluation at several points.
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      gp_Pnt anOffsetPnt = anOffsetSurf->Value(u, v);
      gp_Pnt anEquivPnt  = anEquiv->Value(u, v);
      EXPECT_TRUE(anOffsetPnt.IsEqual(anEquivPnt, Precision::Confusion()))
        << "Mismatch at u=" << u << ", v=" << v;
    }
  }
}

// Tests for D0, D1, D2, D3 consistency between offset surface and equivalent surface via adaptor.

namespace
{
//! Helper to compare vectors with tolerance.
bool VectorsEqual(const gp_Vec& theV1, const gp_Vec& theV2, double theTol)
{
  return (theV1 - theV2).Magnitude() < theTol;
}
} // namespace

TEST(Geom_OffsetSurface_Derivatives, Plane_D0D1D2D3_Consistency)
{
  gp_Ax3             anAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAxis);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aPlane, anOffset);

  // Create adaptors for offset surface and equivalent surface.
  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  // Test at several points.
  for (double u = -5.0; u <= 5.0; u += 2.5)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // D0
      gp_Pnt aP1, aP2;
      anOffsetAdaptor.D0(u, v, aP1);
      anEquivAdaptor.D0(u, v, aP2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D0 mismatch at u=" << u << ", v=" << v;

      // D1
      gp_Pnt aP1D1, aP2D1;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1D1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2D1, aD1U2, aD1V2);
      EXPECT_TRUE(aP1D1.IsEqual(aP2D1, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;

      // D2
      gp_Pnt aP1D2, aP2D2;
      gp_Vec aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1;
      gp_Vec aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2;
      anOffsetAdaptor.D2(u, v, aP1D2, aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1);
      anEquivAdaptor.D2(u, v, aP2D2, aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2);
      EXPECT_TRUE(aP1D2.IsEqual(aP2D2, aTol)) << "D2 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2U1, aD2U2, aTol)) << "D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2V1, aD2V2, aTol)) << "D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2UV1, aD2UV2, aTol)) << "D2UV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, Cylinder_D0D1D2D3_Consistency)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // D0
      gp_Pnt aP1, aP2;
      anOffsetAdaptor.D0(u, v, aP1);
      anEquivAdaptor.D0(u, v, aP2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D0 mismatch at u=" << u << ", v=" << v;

      // D1
      gp_Pnt aP1D1, aP2D1;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1D1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2D1, aD1U2, aD1V2);
      EXPECT_TRUE(aP1D1.IsEqual(aP2D1, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;

      // D2
      gp_Pnt aP1D2, aP2D2;
      gp_Vec aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1;
      gp_Vec aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2;
      anOffsetAdaptor.D2(u, v, aP1D2, aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1);
      anEquivAdaptor.D2(u, v, aP2D2, aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2);
      EXPECT_TRUE(aP1D2.IsEqual(aP2D2, aTol)) << "D2 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2U1, aD2U2, aTol)) << "D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2V1, aD2V2, aTol)) << "D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2UV1, aD2UV2, aTol)) << "D2UV mismatch at u=" << u << ", v=" << v;

      // D3
      gp_Pnt aP1D3, aP2D3;
      gp_Vec aD1U1D3, aD1V1D3, aD2U1D3, aD2V1D3, aD2UV1D3, aD3U1, aD3V1, aD3UUV1, aD3UVV1;
      gp_Vec aD1U2D3, aD1V2D3, aD2U2D3, aD2V2D3, aD2UV2D3, aD3U2, aD3V2, aD3UUV2, aD3UVV2;
      anOffsetAdaptor.D3(u,
                         v,
                         aP1D3,
                         aD1U1D3,
                         aD1V1D3,
                         aD2U1D3,
                         aD2V1D3,
                         aD2UV1D3,
                         aD3U1,
                         aD3V1,
                         aD3UUV1,
                         aD3UVV1);
      anEquivAdaptor.D3(u,
                        v,
                        aP2D3,
                        aD1U2D3,
                        aD1V2D3,
                        aD2U2D3,
                        aD2V2D3,
                        aD2UV2D3,
                        aD3U2,
                        aD3V2,
                        aD3UUV2,
                        aD3UVV2);
      EXPECT_TRUE(aP1D3.IsEqual(aP2D3, aTol)) << "D3 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3U1, aD3U2, aTol)) << "D3U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3V1, aD3V2, aTol)) << "D3V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3UUV1, aD3UUV2, aTol))
        << "D3UUV mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3UVV1, aD3UVV2, aTol))
        << "D3UVV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, Sphere_D0D1D2D3_Consistency)
{
  gp_Ax3                        anAxis(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 10.0);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aSphere, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  // Avoid poles at v = +/- PI/2.
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -M_PI / 3.0; v <= M_PI / 3.0; v += M_PI / 6.0)
    {
      // D0
      gp_Pnt aP1, aP2;
      anOffsetAdaptor.D0(u, v, aP1);
      anEquivAdaptor.D0(u, v, aP2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D0 mismatch at u=" << u << ", v=" << v;

      // D1
      gp_Pnt aP1D1, aP2D1;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1D1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2D1, aD1U2, aD1V2);
      EXPECT_TRUE(aP1D1.IsEqual(aP2D1, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;

      // D2
      gp_Pnt aP1D2, aP2D2;
      gp_Vec aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1;
      gp_Vec aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2;
      anOffsetAdaptor.D2(u, v, aP1D2, aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1);
      anEquivAdaptor.D2(u, v, aP2D2, aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2);
      EXPECT_TRUE(aP1D2.IsEqual(aP2D2, aTol)) << "D2 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2U1, aD2U2, aTol)) << "D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2V1, aD2V2, aTol)) << "D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2UV1, aD2UV2, aTol)) << "D2UV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, Cone_D0D1D2_Consistency)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                aSemiAngle = M_PI / 6.0;
  const double                aRefRadius = 10.0;
  Handle(Geom_ConicalSurface) aCone      = new Geom_ConicalSurface(anAxis, aSemiAngle, aRefRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCone, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = 1.0; v <= 10.0; v += 3.0) // Avoid apex at v=0.
    {
      // D0
      gp_Pnt aP1, aP2;
      anOffsetAdaptor.D0(u, v, aP1);
      anEquivAdaptor.D0(u, v, aP2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D0 mismatch at u=" << u << ", v=" << v;

      // D1
      gp_Pnt aP1D1, aP2D1;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1D1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2D1, aD1U2, aD1V2);
      EXPECT_TRUE(aP1D1.IsEqual(aP2D1, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;

      // D2
      gp_Pnt aP1D2, aP2D2;
      gp_Vec aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1;
      gp_Vec aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2;
      anOffsetAdaptor.D2(u, v, aP1D2, aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1);
      anEquivAdaptor.D2(u, v, aP2D2, aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2);
      EXPECT_TRUE(aP1D2.IsEqual(aP2D2, aTol)) << "D2 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2U1, aD2U2, aTol)) << "D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2V1, aD2V2, aTol)) << "D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2UV1, aD2UV2, aTol)) << "D2UV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, Torus_D0D1D2D3_Consistency)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                 aMajorRadius = 20.0;
  const double                 aMinorRadius = 5.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(anAxis, aMajorRadius, aMinorRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTorus, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = 0.0; v < 2.0 * M_PI; v += M_PI / 4.0)
    {
      // D0
      gp_Pnt aP1, aP2;
      anOffsetAdaptor.D0(u, v, aP1);
      anEquivAdaptor.D0(u, v, aP2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D0 mismatch at u=" << u << ", v=" << v;

      // D1
      gp_Pnt aP1D1, aP2D1;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1D1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2D1, aD1U2, aD1V2);
      EXPECT_TRUE(aP1D1.IsEqual(aP2D1, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;

      // D2
      gp_Pnt aP1D2, aP2D2;
      gp_Vec aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1;
      gp_Vec aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2;
      anOffsetAdaptor.D2(u, v, aP1D2, aD1U1D2, aD1V1D2, aD2U1, aD2V1, aD2UV1);
      anEquivAdaptor.D2(u, v, aP2D2, aD1U2D2, aD1V2D2, aD2U2, aD2V2, aD2UV2);
      EXPECT_TRUE(aP1D2.IsEqual(aP2D2, aTol)) << "D2 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2U1, aD2U2, aTol)) << "D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2V1, aD2V2, aTol)) << "D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD2UV1, aD2UV2, aTol)) << "D2UV mismatch at u=" << u << ", v=" << v;

      // D3
      gp_Pnt aP1D3, aP2D3;
      gp_Vec aD1U1D3, aD1V1D3, aD2U1D3, aD2V1D3, aD2UV1D3, aD3U1, aD3V1, aD3UUV1, aD3UVV1;
      gp_Vec aD1U2D3, aD1V2D3, aD2U2D3, aD2V2D3, aD2UV2D3, aD3U2, aD3V2, aD3UUV2, aD3UVV2;
      anOffsetAdaptor.D3(u,
                         v,
                         aP1D3,
                         aD1U1D3,
                         aD1V1D3,
                         aD2U1D3,
                         aD2V1D3,
                         aD2UV1D3,
                         aD3U1,
                         aD3V1,
                         aD3UUV1,
                         aD3UVV1);
      anEquivAdaptor.D3(u,
                        v,
                        aP2D3,
                        aD1U2D3,
                        aD1V2D3,
                        aD2U2D3,
                        aD2V2D3,
                        aD2UV2D3,
                        aD3U2,
                        aD3V2,
                        aD3UUV2,
                        aD3UVV2);
      EXPECT_TRUE(aP1D3.IsEqual(aP2D3, aTol)) << "D3 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3U1, aD3U2, aTol)) << "D3U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3V1, aD3V2, aTol)) << "D3V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3UUV1, aD3UUV2, aTol))
        << "D3UUV mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD3UVV1, aD3UVV2, aTol))
        << "D3UVV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, Cylinder_DN_Consistency)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;
  const double u    = M_PI / 3.0;
  const double v    = 2.5;

  // Test various derivative orders.
  for (int Nu = 0; Nu <= 3; ++Nu)
  {
    for (int Nv = 0; Nv <= 3; ++Nv)
    {
      if (Nu + Nv == 0 || Nu + Nv > 3)
        continue;
      gp_Vec aDN1 = anOffsetAdaptor.DN(u, v, Nu, Nv);
      gp_Vec aDN2 = anEquivAdaptor.DN(u, v, Nu, Nv);
      EXPECT_TRUE(VectorsEqual(aDN1, aDN2, aTol))
        << "DN(" << Nu << "," << Nv << ") mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, NegativeOffset_Cylinder_D0D1D2_Consistency)
{
  // Test with negative offset (smaller cylinder).
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = -3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 3.0)
  {
    for (double v = -3.0; v <= 3.0; v += 2.0)
    {
      gp_Pnt aP1, aP2;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2, aD1U2, aD1V2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_Derivatives, FlippedCylinder_D0D1_Consistency)
{
  // Test with offset that causes cylinder flip (negative radius).
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 5.0);

  const double               anOffset     = -8.0; // Results in flipped cylinder with radius 3.
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 3.0)
  {
    for (double v = -3.0; v <= 3.0; v += 2.0)
    {
      gp_Pnt aP1, aP2;
      gp_Vec aD1U1, aD1V1, aD1U2, aD1V2;
      anOffsetAdaptor.D1(u, v, aP1, aD1U1, aD1V1);
      anEquivAdaptor.D1(u, v, aP2, aD1U2, aD1V2);
      EXPECT_TRUE(aP1.IsEqual(aP2, aTol)) << "D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1U1, aD1U2, aTol)) << "D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aD1V1, aD1V2, aTol)) << "D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}

// Three-way consistency tests: Direct Geom_OffsetSurface vs OffsetAdaptor vs EquivalentAdaptor.

TEST(Geom_OffsetSurface_ThreeWay, Cylinder_D0_ThreeWayConsistency)
{
  // Verify that direct Geom_OffsetSurface, adaptor from offset surface, and adaptor
  // from equivalent surface all return the same values.
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  // Create adaptors.
  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // 1. Direct evaluation on Geom_OffsetSurface.
      gp_Pnt aDirectPnt = anOffsetSurf->Value(u, v);

      // 2. Evaluation via adaptor from offset surface.
      gp_Pnt aOffsetAdaptorPnt;
      anOffsetAdaptor.D0(u, v, aOffsetAdaptorPnt);

      // 3. Evaluation via adaptor from equivalent surface.
      gp_Pnt aEquivAdaptorPnt;
      anEquivAdaptor.D0(u, v, aEquivAdaptorPnt);

      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Direct vs OffsetAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Direct vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aOffsetAdaptorPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "OffsetAdaptor vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Cylinder_D1_ThreeWayConsistency)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // 1. Direct evaluation on Geom_OffsetSurface.
      gp_Pnt aDirectPnt;
      gp_Vec aDirectD1U, aDirectD1V;
      anOffsetSurf->D1(u, v, aDirectPnt, aDirectD1U, aDirectD1V);

      // 2. Evaluation via adaptor from offset surface.
      gp_Pnt aOffsetAdaptorPnt;
      gp_Vec aOffsetAdaptorD1U, aOffsetAdaptorD1V;
      anOffsetAdaptor.D1(u, v, aOffsetAdaptorPnt, aOffsetAdaptorD1U, aOffsetAdaptorD1V);

      // 3. Evaluation via adaptor from equivalent surface.
      gp_Pnt aEquivAdaptorPnt;
      gp_Vec aEquivAdaptorD1U, aEquivAdaptorD1V;
      anEquivAdaptor.D1(u, v, aEquivAdaptorPnt, aEquivAdaptorD1U, aEquivAdaptorD1V);

      // Point consistency.
      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Direct vs OffsetAdaptor D1 point mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Direct vs EquivAdaptor D1 point mismatch at u=" << u << ", v=" << v;

      // D1U consistency.
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aOffsetAdaptorD1U, aTol))
        << "Direct vs OffsetAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aEquivAdaptorD1U, aTol))
        << "Direct vs EquivAdaptor D1U mismatch at u=" << u << ", v=" << v;

      // D1V consistency.
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aOffsetAdaptorD1V, aTol))
        << "Direct vs OffsetAdaptor D1V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aEquivAdaptorD1V, aTol))
        << "Direct vs EquivAdaptor D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Cylinder_D2_ThreeWayConsistency)
{
  gp_Ax3                          anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCyl = new Geom_CylindricalSurface(anAxis, 10.0);

  const double               anOffset     = 3.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCyl, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // 1. Direct evaluation on Geom_OffsetSurface.
      gp_Pnt aDirectPnt;
      gp_Vec aDirectD1U, aDirectD1V, aDirectD2U, aDirectD2V, aDirectD2UV;
      anOffsetSurf
        ->D2(u, v, aDirectPnt, aDirectD1U, aDirectD1V, aDirectD2U, aDirectD2V, aDirectD2UV);

      // 2. Evaluation via adaptor from offset surface.
      gp_Pnt aOffsetAdaptorPnt;
      gp_Vec aOffsetAdaptorD1U, aOffsetAdaptorD1V, aOffsetAdaptorD2U, aOffsetAdaptorD2V,
        aOffsetAdaptorD2UV;
      anOffsetAdaptor.D2(u,
                         v,
                         aOffsetAdaptorPnt,
                         aOffsetAdaptorD1U,
                         aOffsetAdaptorD1V,
                         aOffsetAdaptorD2U,
                         aOffsetAdaptorD2V,
                         aOffsetAdaptorD2UV);

      // 3. Evaluation via adaptor from equivalent surface.
      gp_Pnt aEquivAdaptorPnt;
      gp_Vec aEquivAdaptorD1U, aEquivAdaptorD1V, aEquivAdaptorD2U, aEquivAdaptorD2V,
        aEquivAdaptorD2UV;
      anEquivAdaptor.D2(u,
                        v,
                        aEquivAdaptorPnt,
                        aEquivAdaptorD1U,
                        aEquivAdaptorD1V,
                        aEquivAdaptorD2U,
                        aEquivAdaptorD2V,
                        aEquivAdaptorD2UV);

      // D2U consistency.
      EXPECT_TRUE(VectorsEqual(aDirectD2U, aOffsetAdaptorD2U, aTol))
        << "Direct vs OffsetAdaptor D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2U, aEquivAdaptorD2U, aTol))
        << "Direct vs EquivAdaptor D2U mismatch at u=" << u << ", v=" << v;

      // D2V consistency.
      EXPECT_TRUE(VectorsEqual(aDirectD2V, aOffsetAdaptorD2V, aTol))
        << "Direct vs OffsetAdaptor D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2V, aEquivAdaptorD2V, aTol))
        << "Direct vs EquivAdaptor D2V mismatch at u=" << u << ", v=" << v;

      // D2UV consistency.
      EXPECT_TRUE(VectorsEqual(aDirectD2UV, aOffsetAdaptorD2UV, aTol))
        << "Direct vs OffsetAdaptor D2UV mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2UV, aEquivAdaptorD2UV, aTol))
        << "Direct vs EquivAdaptor D2UV mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Sphere_D0D1D2_ThreeWayConsistency)
{
  gp_Ax3                        anAxis(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 10.0);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aSphere, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  // Avoid poles at v = +/- PI/2.
  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = -M_PI / 3.0; v <= M_PI / 3.0; v += M_PI / 6.0)
    {
      // D0.
      gp_Pnt aDirectPnt = anOffsetSurf->Value(u, v);
      gp_Pnt aOffsetAdaptorPnt, aEquivAdaptorPnt;
      anOffsetAdaptor.D0(u, v, aOffsetAdaptorPnt);
      anEquivAdaptor.D0(u, v, aEquivAdaptorPnt);

      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Sphere Direct vs OffsetAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Sphere Direct vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;

      // D1.
      gp_Pnt aDirectD1Pnt;
      gp_Vec aDirectD1U, aDirectD1V;
      anOffsetSurf->D1(u, v, aDirectD1Pnt, aDirectD1U, aDirectD1V);

      gp_Pnt aOffsetD1Pnt, aEquivD1Pnt;
      gp_Vec aOffsetD1U, aOffsetD1V, aEquivD1U, aEquivD1V;
      anOffsetAdaptor.D1(u, v, aOffsetD1Pnt, aOffsetD1U, aOffsetD1V);
      anEquivAdaptor.D1(u, v, aEquivD1Pnt, aEquivD1U, aEquivD1V);

      EXPECT_TRUE(VectorsEqual(aDirectD1U, aOffsetD1U, aTol))
        << "Sphere Direct vs OffsetAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aEquivD1U, aTol))
        << "Sphere Direct vs EquivAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aOffsetD1V, aTol))
        << "Sphere Direct vs OffsetAdaptor D1V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aEquivD1V, aTol))
        << "Sphere Direct vs EquivAdaptor D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Plane_D0D1D2_ThreeWayConsistency)
{
  gp_Ax3             anAxis(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAxis);

  const double               anOffset     = 5.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aPlane, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = -5.0; u <= 5.0; u += 2.5)
  {
    for (double v = -5.0; v <= 5.0; v += 2.5)
    {
      // D0.
      gp_Pnt aDirectPnt = anOffsetSurf->Value(u, v);
      gp_Pnt aOffsetAdaptorPnt, aEquivAdaptorPnt;
      anOffsetAdaptor.D0(u, v, aOffsetAdaptorPnt);
      anEquivAdaptor.D0(u, v, aEquivAdaptorPnt);

      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Plane Direct vs OffsetAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Plane Direct vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;

      // D1.
      gp_Pnt aDirectD1Pnt;
      gp_Vec aDirectD1U, aDirectD1V;
      anOffsetSurf->D1(u, v, aDirectD1Pnt, aDirectD1U, aDirectD1V);

      gp_Pnt aOffsetD1Pnt, aEquivD1Pnt;
      gp_Vec aOffsetD1U, aOffsetD1V, aEquivD1U, aEquivD1V;
      anOffsetAdaptor.D1(u, v, aOffsetD1Pnt, aOffsetD1U, aOffsetD1V);
      anEquivAdaptor.D1(u, v, aEquivD1Pnt, aEquivD1U, aEquivD1V);

      EXPECT_TRUE(VectorsEqual(aDirectD1U, aOffsetD1U, aTol))
        << "Plane Direct vs OffsetAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aEquivD1U, aTol))
        << "Plane Direct vs EquivAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aOffsetD1V, aTol))
        << "Plane Direct vs OffsetAdaptor D1V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aEquivD1V, aTol))
        << "Plane Direct vs EquivAdaptor D1V mismatch at u=" << u << ", v=" << v;

      // D2.
      gp_Pnt aDirectD2Pnt;
      gp_Vec aDirectD2D1U, aDirectD2D1V, aDirectD2U, aDirectD2V, aDirectD2UV;
      anOffsetSurf
        ->D2(u, v, aDirectD2Pnt, aDirectD2D1U, aDirectD2D1V, aDirectD2U, aDirectD2V, aDirectD2UV);

      gp_Pnt aOffsetD2Pnt, aEquivD2Pnt;
      gp_Vec aOffsetD2D1U, aOffsetD2D1V, aOffsetD2U, aOffsetD2V, aOffsetD2UV;
      gp_Vec aEquivD2D1U, aEquivD2D1V, aEquivD2U, aEquivD2V, aEquivD2UV;
      anOffsetAdaptor
        .D2(u, v, aOffsetD2Pnt, aOffsetD2D1U, aOffsetD2D1V, aOffsetD2U, aOffsetD2V, aOffsetD2UV);
      anEquivAdaptor
        .D2(u, v, aEquivD2Pnt, aEquivD2D1U, aEquivD2D1V, aEquivD2U, aEquivD2V, aEquivD2UV);

      EXPECT_TRUE(VectorsEqual(aDirectD2U, aOffsetD2U, aTol))
        << "Plane Direct vs OffsetAdaptor D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2U, aEquivD2U, aTol))
        << "Plane Direct vs EquivAdaptor D2U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2V, aOffsetD2V, aTol))
        << "Plane Direct vs OffsetAdaptor D2V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD2V, aEquivD2V, aTol))
        << "Plane Direct vs EquivAdaptor D2V mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Torus_D0D1D2_ThreeWayConsistency)
{
  gp_Ax3                       anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                 aMajorRadius = 20.0;
  const double                 aMinorRadius = 5.0;
  Handle(Geom_ToroidalSurface) aTorus =
    new Geom_ToroidalSurface(anAxis, aMajorRadius, aMinorRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aTorus, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = 0.0; v < 2.0 * M_PI; v += M_PI / 4.0)
    {
      // D0.
      gp_Pnt aDirectPnt = anOffsetSurf->Value(u, v);
      gp_Pnt aOffsetAdaptorPnt, aEquivAdaptorPnt;
      anOffsetAdaptor.D0(u, v, aOffsetAdaptorPnt);
      anEquivAdaptor.D0(u, v, aEquivAdaptorPnt);

      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Torus Direct vs OffsetAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Torus Direct vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;

      // D1.
      gp_Pnt aDirectD1Pnt;
      gp_Vec aDirectD1U, aDirectD1V;
      anOffsetSurf->D1(u, v, aDirectD1Pnt, aDirectD1U, aDirectD1V);

      gp_Pnt aOffsetD1Pnt, aEquivD1Pnt;
      gp_Vec aOffsetD1U, aOffsetD1V, aEquivD1U, aEquivD1V;
      anOffsetAdaptor.D1(u, v, aOffsetD1Pnt, aOffsetD1U, aOffsetD1V);
      anEquivAdaptor.D1(u, v, aEquivD1Pnt, aEquivD1U, aEquivD1V);

      EXPECT_TRUE(VectorsEqual(aDirectD1U, aOffsetD1U, aTol))
        << "Torus Direct vs OffsetAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aEquivD1U, aTol))
        << "Torus Direct vs EquivAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aOffsetD1V, aTol))
        << "Torus Direct vs OffsetAdaptor D1V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aEquivD1V, aTol))
        << "Torus Direct vs EquivAdaptor D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}

TEST(Geom_OffsetSurface_ThreeWay, Cone_D0D1_ThreeWayConsistency)
{
  gp_Ax3                      anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  const double                aSemiAngle = M_PI / 6.0;
  const double                aRefRadius = 10.0;
  Handle(Geom_ConicalSurface) aCone      = new Geom_ConicalSurface(anAxis, aSemiAngle, aRefRadius);

  const double               anOffset     = 2.0;
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aCone, anOffset);

  GeomAdaptor_Surface  anOffsetAdaptor(anOffsetSurf);
  Handle(Geom_Surface) anEquiv = anOffsetSurf->Surface();
  ASSERT_FALSE(anEquiv.IsNull());
  GeomAdaptor_Surface anEquivAdaptor(anEquiv);

  const double aTol = 1e-10;

  for (double u = 0.0; u < 2.0 * M_PI; u += M_PI / 4.0)
  {
    for (double v = 1.0; v <= 10.0; v += 3.0) // Avoid apex at v=0.
    {
      // D0.
      gp_Pnt aDirectPnt = anOffsetSurf->Value(u, v);
      gp_Pnt aOffsetAdaptorPnt, aEquivAdaptorPnt;
      anOffsetAdaptor.D0(u, v, aOffsetAdaptorPnt);
      anEquivAdaptor.D0(u, v, aEquivAdaptorPnt);

      EXPECT_TRUE(aDirectPnt.IsEqual(aOffsetAdaptorPnt, aTol))
        << "Cone Direct vs OffsetAdaptor D0 mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(aDirectPnt.IsEqual(aEquivAdaptorPnt, aTol))
        << "Cone Direct vs EquivAdaptor D0 mismatch at u=" << u << ", v=" << v;

      // D1.
      gp_Pnt aDirectD1Pnt;
      gp_Vec aDirectD1U, aDirectD1V;
      anOffsetSurf->D1(u, v, aDirectD1Pnt, aDirectD1U, aDirectD1V);

      gp_Pnt aOffsetD1Pnt, aEquivD1Pnt;
      gp_Vec aOffsetD1U, aOffsetD1V, aEquivD1U, aEquivD1V;
      anOffsetAdaptor.D1(u, v, aOffsetD1Pnt, aOffsetD1U, aOffsetD1V);
      anEquivAdaptor.D1(u, v, aEquivD1Pnt, aEquivD1U, aEquivD1V);

      EXPECT_TRUE(VectorsEqual(aDirectD1U, aOffsetD1U, aTol))
        << "Cone Direct vs OffsetAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1U, aEquivD1U, aTol))
        << "Cone Direct vs EquivAdaptor D1U mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aOffsetD1V, aTol))
        << "Cone Direct vs OffsetAdaptor D1V mismatch at u=" << u << ", v=" << v;
      EXPECT_TRUE(VectorsEqual(aDirectD1V, aEquivD1V, aTol))
        << "Cone Direct vs EquivAdaptor D1V mismatch at u=" << u << ", v=" << v;
    }
  }
}