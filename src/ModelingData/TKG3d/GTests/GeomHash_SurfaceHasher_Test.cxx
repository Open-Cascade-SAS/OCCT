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

#include <GeomHash_SurfaceHasher.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>

class GeomHash_SurfaceHasherTest : public ::testing::Test
{
protected:
  GeomHash_SurfaceHasher myHasher;
};

// ============================================================================
// Plane Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, Plane_CopiedPlanes_SameHash)
{
  gp_Ax3             anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(anAxis);
  Handle(Geom_Plane) aPlane2 = Handle(Geom_Plane)::DownCast(aPlane1->Copy());

  EXPECT_EQ(myHasher(aPlane1), myHasher(aPlane2));
  EXPECT_TRUE(myHasher(aPlane1, aPlane2));
}

TEST_F(GeomHash_SurfaceHasherTest, Plane_DifferentPlanes_DifferentHash)
{
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
  Handle(Geom_Plane) aPlane2 = new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 1.0), gp_Dir(0.0, 0.0, 1.0)));

  EXPECT_NE(myHasher(aPlane1), myHasher(aPlane2));
  EXPECT_FALSE(myHasher(aPlane1, aPlane2));
}

// ============================================================================
// Cylindrical Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, CylindricalSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                          anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(anAxis, 5.0);
  Handle(Geom_CylindricalSurface) aCyl2 = Handle(Geom_CylindricalSurface)::DownCast(aCyl1->Copy());

  EXPECT_EQ(myHasher(aCyl1), myHasher(aCyl2));
  EXPECT_TRUE(myHasher(aCyl1, aCyl2));
}

TEST_F(GeomHash_SurfaceHasherTest, CylindricalSurface_DifferentRadius_DifferentHash)
{
  gp_Ax3                          anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(anAxis, 5.0);
  Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(anAxis, 10.0);

  EXPECT_NE(myHasher(aCyl1), myHasher(aCyl2));
  EXPECT_FALSE(myHasher(aCyl1, aCyl2));
}

// ============================================================================
// Conical Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, ConicalSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                      anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_ConicalSurface) aCone1 = new Geom_ConicalSurface(anAxis, M_PI / 6.0, 5.0);
  Handle(Geom_ConicalSurface) aCone2 = Handle(Geom_ConicalSurface)::DownCast(aCone1->Copy());

  EXPECT_EQ(myHasher(aCone1), myHasher(aCone2));
  EXPECT_TRUE(myHasher(aCone1, aCone2));
}

TEST_F(GeomHash_SurfaceHasherTest, ConicalSurface_DifferentAngle_DifferentHash)
{
  gp_Ax3                      anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_ConicalSurface) aCone1 = new Geom_ConicalSurface(anAxis, M_PI / 6.0, 5.0);
  Handle(Geom_ConicalSurface) aCone2 = new Geom_ConicalSurface(anAxis, M_PI / 4.0, 5.0);

  EXPECT_NE(myHasher(aCone1), myHasher(aCone2));
  EXPECT_FALSE(myHasher(aCone1, aCone2));
}

// ============================================================================
// Spherical Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SphericalSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere1 = new Geom_SphericalSurface(anAxis, 5.0);
  Handle(Geom_SphericalSurface) aSphere2 =
    Handle(Geom_SphericalSurface)::DownCast(aSphere1->Copy());

  EXPECT_EQ(myHasher(aSphere1), myHasher(aSphere2));
  EXPECT_TRUE(myHasher(aSphere1, aSphere2));
}

TEST_F(GeomHash_SurfaceHasherTest, SphericalSurface_DifferentRadius_DifferentHash)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere1 = new Geom_SphericalSurface(anAxis, 5.0);
  Handle(Geom_SphericalSurface) aSphere2 = new Geom_SphericalSurface(anAxis, 10.0);

  EXPECT_NE(myHasher(aSphere1), myHasher(aSphere2));
  EXPECT_FALSE(myHasher(aSphere1, aSphere2));
}

// ============================================================================
// Toroidal Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, ToroidalSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                       anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_ToroidalSurface) aTorus1 = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);
  Handle(Geom_ToroidalSurface) aTorus2 = Handle(Geom_ToroidalSurface)::DownCast(aTorus1->Copy());

  EXPECT_EQ(myHasher(aTorus1), myHasher(aTorus2));
  EXPECT_TRUE(myHasher(aTorus1, aTorus2));
}

TEST_F(GeomHash_SurfaceHasherTest, ToroidalSurface_DifferentRadii_DifferentHash)
{
  gp_Ax3                       anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_ToroidalSurface) aTorus1 = new Geom_ToroidalSurface(anAxis, 10.0, 3.0);
  Handle(Geom_ToroidalSurface) aTorus2 = new Geom_ToroidalSurface(anAxis, 10.0, 5.0);

  EXPECT_NE(myHasher(aTorus1), myHasher(aTorus2));
  EXPECT_FALSE(myHasher(aTorus1, aTorus2));
}

// ============================================================================
// BezierSurface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BezierSurface_CopiedSurfaces_SameHash)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  Handle(Geom_BezierSurface) aSurf1 = new Geom_BezierSurface(aPoles);
  Handle(Geom_BezierSurface) aSurf2 = Handle(Geom_BezierSurface)::DownCast(aSurf1->Copy());

  EXPECT_EQ(myHasher(aSurf1), myHasher(aSurf2));
  EXPECT_TRUE(myHasher(aSurf1, aSurf2));
}

TEST_F(GeomHash_SurfaceHasherTest, BezierSurface_DifferentPoles_DifferentComparison)
{
  TColgp_Array2OfPnt aPoles1(1, 2, 1, 2);
  aPoles1(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles1(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles1(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles1(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColgp_Array2OfPnt aPoles2(1, 2, 1, 2);
  aPoles2(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles2(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles2(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles2(2, 2) = gp_Pnt(1.0, 1.0, 2.0); // Different

  Handle(Geom_BezierSurface) aSurf1 = new Geom_BezierSurface(aPoles1);
  Handle(Geom_BezierSurface) aSurf2 = new Geom_BezierSurface(aPoles2);

  // Hash may be same (metadata only), but comparison should differ
  EXPECT_FALSE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// BSplineSurface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BSplineSurface_CopiedSurfaces_SameHash)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults(1) = 2;
  aUMults(2) = 2;
  aVMults(1) = 2;
  aVMults(2) = 2;

  Handle(Geom_BSplineSurface) aSurf1 =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
  Handle(Geom_BSplineSurface) aSurf2 = Handle(Geom_BSplineSurface)::DownCast(aSurf1->Copy());

  EXPECT_EQ(myHasher(aSurf1), myHasher(aSurf2));
  EXPECT_TRUE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// SurfaceOfRevolution Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SurfaceOfRevolution_CopiedSurfaces_SameHash)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1            anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SurfaceOfRevolution) aRev1 = new Geom_SurfaceOfRevolution(aLine, anAxis);
  Handle(Geom_SurfaceOfRevolution) aRev2 =
    Handle(Geom_SurfaceOfRevolution)::DownCast(aRev1->Copy());

  EXPECT_EQ(myHasher(aRev1), myHasher(aRev2));
  EXPECT_TRUE(myHasher(aRev1, aRev2));
}

// ============================================================================
// SurfaceOfLinearExtrusion Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SurfaceOfLinearExtrusion_CopiedSurfaces_SameHash)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir            aDir(0.0, 0.0, 1.0);
  Handle(Geom_SurfaceOfLinearExtrusion) anExt1 = new Geom_SurfaceOfLinearExtrusion(aLine, aDir);
  Handle(Geom_SurfaceOfLinearExtrusion) anExt2 =
    Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(anExt1->Copy());

  EXPECT_EQ(myHasher(anExt1), myHasher(anExt2));
  EXPECT_TRUE(myHasher(anExt1, anExt2));
}

// ============================================================================
// RectangularTrimmedSurface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, RectangularTrimmedSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)                     aPlane = new Geom_Plane(anAxis);
  Handle(Geom_RectangularTrimmedSurface) aTrimmed1 =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 10.0, 0.0, 10.0);
  Handle(Geom_RectangularTrimmedSurface) aTrimmed2 =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(aTrimmed1->Copy());

  EXPECT_EQ(myHasher(aTrimmed1), myHasher(aTrimmed2));
  EXPECT_TRUE(myHasher(aTrimmed1, aTrimmed2));
}

TEST_F(GeomHash_SurfaceHasherTest, RectangularTrimmedSurface_DifferentBounds_DifferentHash)
{
  gp_Ax3                                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)                     aPlane = new Geom_Plane(anAxis);
  Handle(Geom_RectangularTrimmedSurface) aTrimmed1 =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 10.0, 0.0, 10.0);
  Handle(Geom_RectangularTrimmedSurface) aTrimmed2 =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 20.0, 0.0, 10.0);

  EXPECT_NE(myHasher(aTrimmed1), myHasher(aTrimmed2));
  EXPECT_FALSE(myHasher(aTrimmed1, aTrimmed2));
}

// ============================================================================
// OffsetSurface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, OffsetSurface_CopiedSurfaces_SameHash)
{
  gp_Ax3                     anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)         aPlane    = new Geom_Plane(anAxis);
  Handle(Geom_OffsetSurface) anOffset1 = new Geom_OffsetSurface(aPlane, 5.0);
  Handle(Geom_OffsetSurface) anOffset2 = Handle(Geom_OffsetSurface)::DownCast(anOffset1->Copy());

  EXPECT_EQ(myHasher(anOffset1), myHasher(anOffset2));
  EXPECT_TRUE(myHasher(anOffset1, anOffset2));
}

TEST_F(GeomHash_SurfaceHasherTest, OffsetSurface_DifferentOffset_DifferentHash)
{
  gp_Ax3                     anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)         aPlane    = new Geom_Plane(anAxis);
  Handle(Geom_OffsetSurface) anOffset1 = new Geom_OffsetSurface(aPlane, 5.0);
  Handle(Geom_OffsetSurface) anOffset2 = new Geom_OffsetSurface(aPlane, 10.0);

  EXPECT_NE(myHasher(anOffset1), myHasher(anOffset2));
  EXPECT_FALSE(myHasher(anOffset1, anOffset2));
}

// ============================================================================
// Cross-type Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, DifferentTypes_DifferentComparison)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)            aPlane  = new Geom_Plane(anAxis);
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 5.0);

  EXPECT_FALSE(myHasher(aPlane, aSphere));
}

TEST_F(GeomHash_SurfaceHasherTest, NullSurfaces_HandledCorrectly)
{
  Handle(Geom_Surface) aNullSurface;
  gp_Ax3               anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)   aPlane = new Geom_Plane(anAxis);

  EXPECT_EQ(myHasher(aNullSurface), 0u);
  EXPECT_TRUE(myHasher(aNullSurface, aNullSurface));
  EXPECT_FALSE(myHasher(aPlane, aNullSurface));
}

TEST_F(GeomHash_SurfaceHasherTest, SameObject_Equal)
{
  gp_Ax3             anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane = new Geom_Plane(anAxis);
  EXPECT_TRUE(myHasher(aPlane, aPlane));
}

// ============================================================================
// Weighted BSpline Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BSplineSurface_Weighted_CopiedSurfaces_SameHash)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColStd_Array2OfReal aWeights(1, 2, 1, 2);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 1.0;
  aWeights(2, 1) = 1.0;
  aWeights(2, 2) = 2.0;

  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults(1) = 2;
  aUMults(2) = 2;
  aVMults(1) = 2;
  aVMults(2) = 2;

  Handle(Geom_BSplineSurface) aSurf1 =
    new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
  Handle(Geom_BSplineSurface) aSurf2 = Handle(Geom_BSplineSurface)::DownCast(aSurf1->Copy());

  EXPECT_EQ(myHasher(aSurf1), myHasher(aSurf2));
  EXPECT_TRUE(myHasher(aSurf1, aSurf2));
}

TEST_F(GeomHash_SurfaceHasherTest, BSplineSurface_DifferentWeights_DifferentComparison)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColStd_Array2OfReal aWeights1(1, 2, 1, 2);
  aWeights1(1, 1) = 1.0;
  aWeights1(1, 2) = 1.0;
  aWeights1(2, 1) = 1.0;
  aWeights1(2, 2) = 2.0;

  TColStd_Array2OfReal aWeights2(1, 2, 1, 2);
  aWeights2(1, 1) = 1.0;
  aWeights2(1, 2) = 1.0;
  aWeights2(2, 1) = 1.0;
  aWeights2(2, 2) = 3.0; // Different

  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults(1) = 2;
  aUMults(2) = 2;
  aVMults(1) = 2;
  aVMults(2) = 2;

  Handle(Geom_BSplineSurface) aSurf1 =
    new Geom_BSplineSurface(aPoles, aWeights1, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
  Handle(Geom_BSplineSurface) aSurf2 =
    new Geom_BSplineSurface(aPoles, aWeights2, aUKnots, aVKnots, aUMults, aVMults, 1, 1);

  EXPECT_FALSE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// Weighted Bezier Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BezierSurface_Weighted_CopiedSurfaces_SameHash)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColStd_Array2OfReal aWeights(1, 2, 1, 2);
  aWeights(1, 1) = 1.0;
  aWeights(1, 2) = 1.0;
  aWeights(2, 1) = 1.0;
  aWeights(2, 2) = 2.0;

  Handle(Geom_BezierSurface) aSurf1 = new Geom_BezierSurface(aPoles, aWeights);
  Handle(Geom_BezierSurface) aSurf2 = Handle(Geom_BezierSurface)::DownCast(aSurf1->Copy());

  EXPECT_EQ(myHasher(aSurf1), myHasher(aSurf2));
  EXPECT_TRUE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// Axis Orientation Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, Plane_DifferentNormal_DifferentHash)
{
  gp_Ax3             anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax3             anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(anAxis1);
  Handle(Geom_Plane) aPlane2 = new Geom_Plane(anAxis2);

  EXPECT_NE(myHasher(aPlane1), myHasher(aPlane2));
  EXPECT_FALSE(myHasher(aPlane1, aPlane2));
}

TEST_F(GeomHash_SurfaceHasherTest, CylindricalSurface_DifferentAxis_DifferentHash)
{
  gp_Ax3                          anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax3                          anAxis2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(anAxis1, 5.0);
  Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(anAxis2, 5.0);

  EXPECT_NE(myHasher(aCyl1), myHasher(aCyl2));
  EXPECT_FALSE(myHasher(aCyl1, aCyl2));
}

// ============================================================================
// Revolution with Different Basis Curve Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SurfaceOfRevolution_DifferentBasisCurve_DifferentComparison)
{
  Handle(Geom_Line) aLine1 = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Line) aLine2 = new Geom_Line(gp_Pnt(2.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax1            anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SurfaceOfRevolution) aRev1 = new Geom_SurfaceOfRevolution(aLine1, anAxis);
  Handle(Geom_SurfaceOfRevolution) aRev2 = new Geom_SurfaceOfRevolution(aLine2, anAxis);

  EXPECT_FALSE(myHasher(aRev1, aRev2));
}

// ============================================================================
// Extrusion with Different Direction Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SurfaceOfLinearExtrusion_DifferentDirection_DifferentHash)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  gp_Dir            aDir1(0.0, 0.0, 1.0);
  gp_Dir            aDir2(0.0, 1.0, 0.0);
  Handle(Geom_SurfaceOfLinearExtrusion) anExt1 = new Geom_SurfaceOfLinearExtrusion(aLine, aDir1);
  Handle(Geom_SurfaceOfLinearExtrusion) anExt2 = new Geom_SurfaceOfLinearExtrusion(aLine, aDir2);

  EXPECT_NE(myHasher(anExt1), myHasher(anExt2));
  EXPECT_FALSE(myHasher(anExt1, anExt2));
}

// ============================================================================
// BSpline Surface with Different Knots Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BSplineSurface_DifferentKnots_DifferentComparison)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  TColStd_Array1OfReal aUKnots1(1, 2), aUKnots2(1, 2), aVKnots(1, 2);
  aUKnots1(1) = 0.0;
  aUKnots1(2) = 1.0;
  aUKnots2(1) = 0.0;
  aUKnots2(2) = 2.0; // Different
  aVKnots(1)  = 0.0;
  aVKnots(2)  = 1.0;

  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults(1) = 2;
  aUMults(2) = 2;
  aVMults(1) = 2;
  aVMults(2) = 2;

  Handle(Geom_BSplineSurface) aSurf1 =
    new Geom_BSplineSurface(aPoles, aUKnots1, aVKnots, aUMults, aVMults, 1, 1);
  Handle(Geom_BSplineSurface) aSurf2 =
    new Geom_BSplineSurface(aPoles, aUKnots2, aVKnots, aUMults, aVMults, 1, 1);

  EXPECT_FALSE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// Transformed Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, Plane_Translated_DifferentHash)
{
  gp_Ax3             anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(anAxis);
  Handle(Geom_Plane) aPlane2 = Handle(Geom_Plane)::DownCast(aPlane1->Copy());
  aPlane2->Translate(gp_Vec(0.0, 0.0, 1.0));

  EXPECT_NE(myHasher(aPlane1), myHasher(aPlane2));
  EXPECT_FALSE(myHasher(aPlane1, aPlane2));
}

TEST_F(GeomHash_SurfaceHasherTest, Sphere_Scaled_DifferentHash)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere1 = new Geom_SphericalSurface(anAxis, 5.0);
  Handle(Geom_SphericalSurface) aSphere2 =
    Handle(Geom_SphericalSurface)::DownCast(aSphere1->Copy());
  aSphere2->Scale(gp_Pnt(0.0, 0.0, 0.0), 2.0);

  EXPECT_NE(myHasher(aSphere1), myHasher(aSphere2));
  EXPECT_FALSE(myHasher(aSphere1, aSphere2));
}

// ============================================================================
// UReversed/VReversed Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BezierSurface_UReversed_DifferentComparison)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  Handle(Geom_BezierSurface) aSurf1 = new Geom_BezierSurface(aPoles);
  Handle(Geom_BezierSurface) aSurf2 = Handle(Geom_BezierSurface)::DownCast(aSurf1->UReversed());

  EXPECT_FALSE(myHasher(aSurf1, aSurf2));
}

TEST_F(GeomHash_SurfaceHasherTest, BezierSurface_VReversed_DifferentComparison)
{
  TColgp_Array2OfPnt aPoles(1, 2, 1, 2);
  aPoles(1, 1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(1, 2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2, 1) = gp_Pnt(0.0, 1.0, 0.0);
  aPoles(2, 2) = gp_Pnt(1.0, 1.0, 1.0);

  Handle(Geom_BezierSurface) aSurf1 = new Geom_BezierSurface(aPoles);
  Handle(Geom_BezierSurface) aSurf2 = Handle(Geom_BezierSurface)::DownCast(aSurf1->VReversed());

  EXPECT_FALSE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// Higher Degree BSpline Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, BSplineSurface_HigherDegree_CopiedSurfaces_SameHash)
{
  TColgp_Array2OfPnt aPoles(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; ++i)
  {
    for (Standard_Integer j = 1; j <= 4; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 1.0, (j - 1) * 1.0, (i + j) * 0.1);
    }
  }

  TColStd_Array1OfReal aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUMults(1) = 4;
  aUMults(2) = 4;
  aVMults(1) = 4;
  aVMults(2) = 4;

  Handle(Geom_BSplineSurface) aSurf1 =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
  Handle(Geom_BSplineSurface) aSurf2 = Handle(Geom_BSplineSurface)::DownCast(aSurf1->Copy());

  EXPECT_EQ(myHasher(aSurf1), myHasher(aSurf2));
  EXPECT_TRUE(myHasher(aSurf1, aSurf2));
}

// ============================================================================
// Cross-type Elementary Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, Cylinder_vs_Cone_DifferentComparison)
{
  gp_Ax3                          anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_CylindricalSurface) aCylinder = new Geom_CylindricalSurface(anAxis, 5.0);
  Handle(Geom_ConicalSurface)     aCone     = new Geom_ConicalSurface(anAxis, M_PI / 6.0, 5.0);

  EXPECT_FALSE(myHasher(aCylinder, aCone));
}

TEST_F(GeomHash_SurfaceHasherTest, Sphere_vs_Torus_DifferentComparison)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(anAxis, 5.0);
  Handle(Geom_ToroidalSurface)  aTorus  = new Geom_ToroidalSurface(anAxis, 5.0, 1.0);

  EXPECT_FALSE(myHasher(aSphere, aTorus));
}

// ============================================================================
// Trimmed vs Base Surface Tests
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, RectangularTrimmedSurface_vs_BaseSurface_DifferentComparison)
{
  gp_Ax3                                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)                     aPlane = new Geom_Plane(anAxis);
  Handle(Geom_RectangularTrimmedSurface) aTrimmed =
    new Geom_RectangularTrimmedSurface(aPlane, 0.0, 10.0, 0.0, 10.0);

  EXPECT_FALSE(myHasher(aPlane, aTrimmed));
}

// ============================================================================
// Edge Cases - Degenerate/Special Values
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, Sphere_VerySmallRadius_CopiedSpheres_SameHash)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere1 = new Geom_SphericalSurface(anAxis, 1e-10);
  Handle(Geom_SphericalSurface) aSphere2 =
    Handle(Geom_SphericalSurface)::DownCast(aSphere1->Copy());

  EXPECT_EQ(myHasher(aSphere1), myHasher(aSphere2));
  EXPECT_TRUE(myHasher(aSphere1, aSphere2));
}

TEST_F(GeomHash_SurfaceHasherTest, Sphere_VeryLargeRadius_CopiedSpheres_SameHash)
{
  gp_Ax3                        anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SphericalSurface) aSphere1 = new Geom_SphericalSurface(anAxis, 1e10);
  Handle(Geom_SphericalSurface) aSphere2 =
    Handle(Geom_SphericalSurface)::DownCast(aSphere1->Copy());

  EXPECT_EQ(myHasher(aSphere1), myHasher(aSphere2));
  EXPECT_TRUE(myHasher(aSphere1, aSphere2));
}

TEST_F(GeomHash_SurfaceHasherTest, Plane_AtOrigin_vs_FarFromOrigin_DifferentHash)
{
  gp_Ax3             anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax3             anAxis2(gp_Pnt(1e10, 1e10, 1e10), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane) aPlane1 = new Geom_Plane(anAxis1);
  Handle(Geom_Plane) aPlane2 = new Geom_Plane(anAxis2);

  EXPECT_NE(myHasher(aPlane1), myHasher(aPlane2));
  EXPECT_FALSE(myHasher(aPlane1, aPlane2));
}

// ============================================================================
// Revolution with Circle Basis (forms Torus-like)
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, SurfaceOfRevolution_CircleBasis_CopiedSurfaces_SameHash)
{
  gp_Ax2                           aCircleAxis(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  Handle(Geom_Circle)              aCircle = new Geom_Circle(aCircleAxis, 1.0);
  gp_Ax1                           aRevAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_SurfaceOfRevolution) aRev1 = new Geom_SurfaceOfRevolution(aCircle, aRevAxis);
  Handle(Geom_SurfaceOfRevolution) aRev2 =
    Handle(Geom_SurfaceOfRevolution)::DownCast(aRev1->Copy());

  EXPECT_EQ(myHasher(aRev1), myHasher(aRev2));
  EXPECT_TRUE(myHasher(aRev1, aRev2));
}

// ============================================================================
// Offset Surface with Different Base
// ============================================================================

TEST_F(GeomHash_SurfaceHasherTest, OffsetSurface_DifferentBaseSurface_DifferentComparison)
{
  gp_Ax3                     anAxis1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Ax3                     anAxis2(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  Handle(Geom_Plane)         aPlane1   = new Geom_Plane(anAxis1);
  Handle(Geom_Plane)         aPlane2   = new Geom_Plane(anAxis2);
  Handle(Geom_OffsetSurface) anOffset1 = new Geom_OffsetSurface(aPlane1, 5.0);
  Handle(Geom_OffsetSurface) anOffset2 = new Geom_OffsetSurface(aPlane2, 5.0);

  EXPECT_FALSE(myHasher(anOffset1, anOffset2));
}
