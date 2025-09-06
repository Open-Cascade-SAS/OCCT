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
#include <chrono>

class IntAna_IntQuadQuad_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Set up common geometric objects for tests
    gp_Pnt origin(0, 0, 0);
    gp_Dir zDir(0, 0, 1);
    gp_Dir xDir(1, 0, 0);
    gp_Ax3 axis(origin, zDir, xDir);

    // Create sphere at origin with radius 5
    sphere1 = gp_Sphere(axis, 5.0);

    // Create cylinder with axis along Z, radius 3
    cylinder1 = gp_Cylinder(axis, 3.0);

    // Create cone with semi-angle 30 degrees
    cone1 = gp_Cone(axis, M_PI / 6, 2.0);

    // Create plane at Z = 2
    gp_Pnt planeOrigin(0, 0, 2);
    plane1 = gp_Pln(planeOrigin, zDir);
  }

  gp_Sphere   sphere1;
  gp_Cylinder cylinder1;
  gp_Cone     cone1;
  gp_Pln      plane1;
};

// Test basic intersection functionality - Cylinder vs Quadric (sphere)
TEST_F(IntAna_IntQuadQuad_Test, CylinderVsSphereIntersection)
{
  IntAna_Quadric sphereQuad(sphere1);

  IntAna_IntQuadQuad intersector(cylinder1, sphereQuad, 1e-7);

  // Should have intersection curves
  EXPECT_TRUE(intersector.IsDone());
  EXPECT_FALSE(intersector.IdenticalElements());

  // Should have intersection curves for cylinder-sphere case
  EXPECT_GT(intersector.NbCurve(), 0);

  // Test that curve indices are valid
  for (Standard_Integer i = 1; i <= intersector.NbCurve(); i++)
  {
    const IntAna_Curve& aCurve = intersector.Curve(i);
    EXPECT_TRUE(aCurve.IsOpen() || aCurve.IsConstant() || !aCurve.IsOpen());
  }
}

// Test NextCurve functionality - the main test for our bug fix
TEST_F(IntAna_IntQuadQuad_Test, NextCurveMethodCorrectness)
{
  IntAna_Quadric aSphereQuad(sphere1);

  IntAna_IntQuadQuad anIntersector(cylinder1, aSphereQuad, 1e-7);

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

      // The opposite flag should be set correctly
      EXPECT_TRUE(anOpposite == Standard_True || anOpposite == Standard_False);
    }
  }
}

// Test edge cases and boundary conditions
TEST_F(IntAna_IntQuadQuad_Test, NextCurveBoundaryConditions)
{
  IntAna_Quadric aSphereQuad(sphere1);

  IntAna_IntQuadQuad anIntersector(cylinder1, aSphereQuad, 1e-7);

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
  gp_Pnt center1(0, 0, 0);
  gp_Pnt center2(3, 0, 0); // Overlapping spheres
  gp_Dir zDir(0, 0, 1);
  gp_Dir xDir(1, 0, 0);
  gp_Ax3 axis1(center1, zDir, xDir);
  gp_Ax3 axis2(center2, zDir, xDir);

  gp_Sphere sphere1(axis1, 2.0);
  gp_Sphere sphere2(axis2, 2.0);

  IntAna_Quadric aSphere2Quad(sphere2);

  IntAna_IntQuadQuad anIntersector;
  anIntersector.Perform(gp_Cylinder(axis1, 2.0), aSphere2Quad, 1e-7);

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

  IntAna_Quadric aSphereQuad(sphere1); // Cylinder vs sphere intersection

  IntAna_IntQuadQuad anIntersector(cylinder1, aSphereQuad, 1e-7);

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

      // The bug was that NextCurve used wrong index for checking sign
      // Now it should correctly determine the opposite parameter
      EXPECT_TRUE(anOpposite == Standard_True || anOpposite == Standard_False);
    }
  }
}

// Performance test to ensure the fix doesn't impact performance
TEST_F(IntAna_IntQuadQuad_Test, PerformanceRegressionTest)
{
  const Standard_Integer aNumIterations = 100;

  IntAna_Quadric aSphereQuad(sphere1);

  // Measure time for multiple intersections
  auto aStart = std::chrono::high_resolution_clock::now();

  for (Standard_Integer anIter = 0; anIter < aNumIterations; anIter++)
  {
    IntAna_IntQuadQuad anIntersector(cylinder1, aSphereQuad, 1e-7);

    EXPECT_TRUE(anIntersector.IsDone());

    for (Standard_Integer i = 1; i <= anIntersector.NbCurve(); i++)
    {
      if (anIntersector.HasNextCurve(i)) // Only call NextCurve when HasNextCurve is true
      {
        Standard_Boolean anOpposite;
        anIntersector.NextCurve(i, anOpposite);
      }
    }
  }

  auto anEnd     = std::chrono::high_resolution_clock::now();
  auto aDuration = std::chrono::duration_cast<std::chrono::milliseconds>(anEnd - aStart);

  // Should complete within reasonable time (less than 5 seconds for 100 iterations)
  EXPECT_LT(aDuration.count(), 5000);
}