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

// OCCT headers
#include <BRep_Builder.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <STEPControl_Reader.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <Standard_NullObject.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>

// GoogleTest headers
#include <gtest/gtest.h>

// System headers
#include <fstream>

//! Test fixture for shape healing operations on tessellated geometry.
//! This fixture loads a tessellated tetrahedron from a STEP file
//! and provides it for testing shape healing algorithms.
//! Related to bug 33894: unifysamedom crashes on tessellated geometry without surfaces.
class ShapeHealing_TessellatedTest : public ::testing::Test
{
protected:
  TopoDS_Shape myShape;

  void SetUp() override
  {
    // Load the tessellated tetrahedron STEP file
    STEPControl_Reader aReader;

    // Use relative path from test execution directory
    const char* aFileName = "data/step/tessellated_tetrahedron_ap242.step";

    // Check if file exists
    std::ifstream aFile(aFileName);
    if (!aFile.good())
    {
      GTEST_SKIP() << "Test file not found: " << aFileName;
      return;
    }
    aFile.close();

    IFSelect_ReturnStatus aStatus = aReader.ReadFile(aFileName);
    ASSERT_EQ(aStatus, IFSelect_RetDone) << "Failed to read STEP file";

    Standard_Integer aNbRoots = aReader.NbRootsForTransfer();
    ASSERT_GT(aNbRoots, 0) << "No roots found in STEP file";

    aReader.TransferRoots();

    // Get the shape
    myShape = aReader.OneShape();
    ASSERT_FALSE(myShape.IsNull()) << "Loaded shape is null";
  }
};

//! Test that the shape loads successfully and has expected structure.
//! Verifies that the tessellated tetrahedron has 4 faces, 1 shell, and 1 solid.
//! Also checks that the shape is considered invalid due to missing surfaces.
TEST_F(ShapeHealing_TessellatedTest, LoadAndValidateShape)
{
  // Count sub-shapes
  Standard_Integer aNbFaces  = 0;
  Standard_Integer aNbShells = 0;
  Standard_Integer aNbSolids = 0;

  for (TopExp_Explorer anExp(myShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aNbFaces++;
  }

  for (TopExp_Explorer anExp(myShape, TopAbs_SHELL); anExp.More(); anExp.Next())
  {
    aNbShells++;
  }

  for (TopExp_Explorer anExp(myShape, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    aNbSolids++;
  }

  // Expected: 4 faces (tetrahedron), 1 shell, 1 solid
  EXPECT_EQ(aNbFaces, 4) << "Tetrahedron should have 4 faces";
  EXPECT_EQ(aNbShells, 1) << "Should have 1 shell";
  EXPECT_EQ(aNbSolids, 1) << "Should have 1 solid";

  // Check shape validity
  BRepCheck_Analyzer aChecker(myShape);
  EXPECT_FALSE(aChecker.IsValid()) << "Tessellated shape without surfaces should not be valid";
}

//! Test that ShapeUpgrade_UnifySameDomain throws exception on tessellated geometry.
//! Verifies that attempting to unify same domains on a tessellated shape without surfaces
//! throws a Standard_NullObject exception instead of crashing.
//! This test reproduces bug 33894.
TEST_F(ShapeHealing_TessellatedTest, UnifySameDomainThrowsException)
{
  ShapeUpgrade_UnifySameDomain aUnifier(myShape);
  aUnifier.SetLinearTolerance(1.0e-6);
  aUnifier.SetAngularTolerance(1.0e-6);

  // This should throw Standard_NullObject exception instead of crashing
  EXPECT_THROW({ aUnifier.Build(); }, Standard_NullObject)
    << "ShapeUpgrade_UnifySameDomain should throw exception for tessellated geometry";
}

//! Test that ShapeUpgrade_UnifySameDomain works when tessellated faces are excluded via KeepShapes.
//! This test verifies that excluding all faces from unification prevents the null surface crash.
TEST_F(ShapeHealing_TessellatedTest, UnifySameDomainWithKeepShapes)
{
  ShapeUpgrade_UnifySameDomain aUnifier(myShape);
  aUnifier.SetLinearTolerance(1.0e-6);
  aUnifier.SetAngularTolerance(1.0e-6);

  // Exclude all faces from unification by adding them to KeepShapes
  TopTools_MapOfShape aKeepShapes;
  for (TopExp_Explorer anExp(myShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aKeepShapes.Add(anExp.Current());
  }
  aUnifier.KeepShapes(aKeepShapes);

  // The algorithm processes ALL faces first (to get their surfaces)
  // BEFORE checking KeepShapes. So even with KeepShapes, tessellated
  // faces without surfaces will cause an exception during initial processing.
  //
  // This is a design limitation: KeepShapes only prevents unification,
  // not the initial face analysis that requires surfaces.
  EXPECT_THROW({ aUnifier.Build(); }, Standard_NullObject)
    << "UnifySameDomain still needs to access surfaces even for kept faces";
}
