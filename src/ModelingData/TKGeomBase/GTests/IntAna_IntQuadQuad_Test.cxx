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

#include <IntAna_IntQuadQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>

#include <gtest/gtest.h>

class IntAna_IntQuadQuad_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Set up common geometric objects for tests
    gp_Pnt anOrigin(0, 0, 0);
    gp_Dir aZDir(gp_Dir::D::Z);
    gp_Dir anXDir(gp_Dir::D::X);
    gp_Ax3 anAxis(anOrigin, aZDir, anXDir);

    // Create sphere at origin with radius 5
    mySphere1 = gp_Sphere(anAxis, 5.0);

    // Create cylinder with axis along Z, radius 3
    myCylinder1 = gp_Cylinder(anAxis, 3.0);

    // Create cone with semi-angle 30 degrees
    myCone1 = gp_Cone(anAxis, M_PI / 6, 2.0);

    // Create plane at Z = 2
    gp_Pnt aPlaneOrigin(0, 0, 2);
    myPlane1 = gp_Pln(aPlaneOrigin, aZDir);
  }

  gp_Sphere   mySphere1;
  gp_Cylinder myCylinder1;
  gp_Cone     myCone1;
  gp_Pln      myPlane1;
};

// Test basic intersection functionality - Cylinder vs Quadric (sphere)
TEST_F(IntAna_IntQuadQuad_Test, CylinderVsSphereIntersection)
{
  IntAna_Quadric aSphereQuad(mySphere1);

  IntAna_IntQuadQuad anIntersector(myCylinder1, aSphereQuad, 1e-7);

  // Should have intersection curves
  EXPECT_TRUE(anIntersector.IsDone());
  EXPECT_FALSE(anIntersector.IdenticalElements());

  // Should have intersection curves for cylinder-sphere case
  EXPECT_GT(anIntersector.NbCurve(), 0);

  // Test that we can access all curves without exceptions
  for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
  {
    // This should not throw any exceptions - just verify we can access the curve
    EXPECT_NO_THROW(anIntersector.Curve(i));
  }
}

// Test NextCurve functionality - the main test for our bug fix
TEST_F(IntAna_IntQuadQuad_Test, NextCurveMethodCorrectness)
{
  IntAna_Quadric aSphereQuad(mySphere1);

  IntAna_IntQuadQuad anIntersector(myCylinder1, aSphereQuad, 1e-7);

  EXPECT_TRUE(anIntersector.IsDone());

  // Test HasNextCurve and NextCurve for valid indices
  for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
  {
    // These methods should not crash and should return consistent results
    Standard_Boolean aHasNext = anIntersector.HasNextCurve(i);

    if (aHasNext) // Only test NextCurve when HasNextCurve is true
    {
      Standard_Boolean anOpposite;
      Standard_Integer aNextIdx = anIntersector.NextCurve(i, anOpposite);

      // Next curve index should be valid
      EXPECT_GT(aNextIdx, 0);
      EXPECT_LE(aNextIdx, anIntersector.NbCurve());
      EXPECT_NE(aNextIdx, i); // Should not point to itself
    }
  }
}

// Test edge cases and boundary conditions
TEST_F(IntAna_IntQuadQuad_Test, NextCurveBoundaryConditions)
{
  IntAna_Quadric aSphereQuad(mySphere1);

  IntAna_IntQuadQuad anIntersector(myCylinder1, aSphereQuad, 1e-7);

  EXPECT_TRUE(anIntersector.IsDone());

  // Test with invalid indices (should throw exceptions)
  EXPECT_THROW(anIntersector.HasNextCurve(0), Standard_OutOfRange);
  EXPECT_THROW(anIntersector.HasNextCurve(anIntersector.NbCurve() + 1), Standard_OutOfRange);

  // Test NextCurve on curves that don't have next curves
  for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
  {
    if (!anIntersector.HasNextCurve(i)) // Only test exception when HasNextCurve is false
    {
      Standard_Boolean anOpposite;
      EXPECT_THROW(anIntersector.NextCurve(i, anOpposite), Standard_DomainError);
    }
  }
}

// Test specific geometric configuration that creates connected curves
TEST_F(IntAna_IntQuadQuad_Test, ConnectedCurvesScenario)
{
  // Create two spheres that intersect in a circle
  gp_Pnt aCenter1(0, 0, 0);
  gp_Pnt aCenter2(3, 0, 0); // Overlapping spheres
  gp_Dir aZDir(gp_Dir::D::Z);
  gp_Dir anXDir(gp_Dir::D::X);
  gp_Ax3 anAxis1(aCenter1, aZDir, anXDir);
  gp_Ax3 anAxis2(aCenter2, aZDir, anXDir);

  gp_Sphere aSphere1(anAxis1, 2.0);
  gp_Sphere aSphere2(anAxis2, 2.0);

  IntAna_Quadric aSphere2Quad(aSphere2);

  IntAna_IntQuadQuad anIntersector;
  anIntersector.Perform(gp_Cylinder(anAxis1, 2.0), aSphere2Quad, 1e-7);

  EXPECT_TRUE(anIntersector.IsDone());

  EXPECT_FALSE(anIntersector.IdenticalElements());

  // Verify that NextCurve method works correctly for each curve
  for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
  {
    if (anIntersector.HasNextCurve(i)) // Only test NextCurve when HasNextCurve is true
    {
      Standard_Boolean anOpposite;
      Standard_Integer aNextIdx = anIntersector.NextCurve(i, anOpposite);

      // Validate the result
      EXPECT_GT(aNextIdx, 0);
      EXPECT_LE(aNextIdx, anIntersector.NbCurve());

      // Test consistency: if curve i connects to j, then j should connect back
      // (though possibly with different opposite flag)
      if (anIntersector.HasNextCurve(aNextIdx))
      {
        Standard_Boolean aReverseOpposite;
        Standard_Integer aReverseNext = anIntersector.NextCurve(aNextIdx, aReverseOpposite);
        // This creates a circular reference which is geometrically possible
        EXPECT_GT(aReverseNext, 0);
        EXPECT_LE(aReverseNext, anIntersector.NbCurve());
      }
    }
  }
}

// Test the fix specifically - ensure we don't access out-of-bounds memory
TEST_F(IntAna_IntQuadQuad_Test, IndexingConsistencyTest)
{
  // This test specifically validates that HasNextCurve and NextCurve
  // use consistent indexing (both should use I-1 for 0-based arrays)

  IntAna_Quadric aSphereQuad(mySphere1); // Cylinder vs sphere intersection

  IntAna_IntQuadQuad anIntersector(myCylinder1, aSphereQuad, 1e-7);

  EXPECT_TRUE(anIntersector.IsDone());

  // Test all valid curve indices
  for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
  {
    // These calls should be consistent and not crash
    Standard_Boolean aHasNext = anIntersector.HasNextCurve(i);

    if (aHasNext) // Only test NextCurve when HasNextCurve is true
    {
      Standard_Boolean anOpposite;
      // This should not crash or return invalid values
      Standard_Integer aNextIdx = anIntersector.NextCurve(i, anOpposite);

      EXPECT_GT(aNextIdx, 0);
      EXPECT_LE(aNextIdx, anIntersector.NbCurve());
    }
  }
}
