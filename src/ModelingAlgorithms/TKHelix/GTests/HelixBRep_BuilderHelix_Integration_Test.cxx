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

#include <HelixBRep_BuilderHelix.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <gp_Ax3.hxx>
#include <GProp_GProps.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_ConstructionError.hxx>

class HelixBRepTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Standard axis aligned with Z
    myAxis      = gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
    myTolerance = 1.e-4;
  }

  // Helper method to count shapes
  Standard_Integer CountShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType) const
  {
    Standard_Integer aCount = 0;
    for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
    {
      aCount++;
    }
    return aCount;
  }

  // Helper method to compute wire length
  Standard_Real ComputeWireLength(const TopoDS_Shape& theWire) const
  {
    GProp_GProps aLProps;
    BRepGProp::LinearProperties(theWire, aLProps);
    return aLProps.Mass();
  }

  // Helper method to validate helix wire properties
  void ValidateHelixWire(const TopoDS_Wire& theWire,
                         Standard_Real      theExpectedLength,
                         Standard_Real      theTolerance = 0.05) const
  {
    EXPECT_FALSE(theWire.IsNull());
    EXPECT_EQ(theWire.ShapeType(), TopAbs_WIRE);

    // Check that wire has edges
    Standard_Integer aNbEdges = CountShapes(theWire, TopAbs_EDGE);
    EXPECT_GT(aNbEdges, 0);

    // Compute wire length
    Standard_Real aWireLength = ComputeWireLength(theWire);
    if (theExpectedLength > 0)
    {
      EXPECT_NEAR(aWireLength, theExpectedLength, theExpectedLength * theTolerance);
    }
  }

  gp_Ax3        myAxis;
  Standard_Real myTolerance;
};

// Test Case A1 - Pure cylindrical helix (TCL test A1)
TEST_F(HelixBRepTest, TCL_Test_A1_PureCylindricalHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // Parameters from TCL test A1: helix, D1 = 100, 1 part, pitch = 5, number of turns, PFi=0
  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0; // H1 = 100

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 5.0; // P1 = 5 (number of turns)

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_False; // PF1 = 0 means number of turns

  aBuilder.SetParameters(myAxis, 100.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());
  EXPECT_EQ(aHelixWire.ShapeType(), TopAbs_WIRE);

  // Calculate expected helix length
  Standard_Real aCircumference = M_PI * 100.0; // D1 = 100
  Standard_Real aTurns         = 100.0 / 20.0; // Height/Pitch for number of turns mode (20 = 100/5)
  Standard_Real aExpectedLength = aTurns * sqrt(aCircumference * aCircumference + 20.0 * 20.0);

  ValidateHelixWire(TopoDS::Wire(aHelixWire), aExpectedLength);
}

// Test Case B1 - Pure cylindrical helix with comphelix (TCL test B1)
TEST_F(HelixBRepTest, TCL_Test_B1_CompositeCylindricalHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // Parameters from TCL test B1: comphelix, D1 = D2 = 100, 1 part, pitch = 20, PFi=1
  TColStd_Array1OfReal aDiams(1, 2);
  aDiams(1) = 100.0; // D1
  aDiams(2) = 100.0; // D2

  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0; // H1

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 20.0; // P1

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True; // PF1 = 1 means pitch

  aBuilder.SetParameters(myAxis, aDiams, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());

  // Calculate expected helix length - same as A1 since geometry is identical
  Standard_Real aCircumference  = M_PI * 100.0;
  Standard_Real aTurns          = 100.0 / 20.0; // Height/Pitch
  Standard_Real aExpectedLength = aTurns * sqrt(aCircumference * aCircumference + 20.0 * 20.0);

  ValidateHelixWire(TopoDS::Wire(aHelixWire), aExpectedLength);
}

// Test Case C1 - Spiral helix (TCL test C1)
TEST_F(HelixBRepTest, TCL_Test_C1_SpiralHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // Parameters from TCL test C1: spiral, D1 = 100, D2 = 20, 3 parts
  TColStd_Array1OfReal aHeights(1, 3);
  aHeights(1) = 20.0; // H1
  aHeights(2) = 60.0; // H2
  aHeights(3) = 20.0; // H3

  TColStd_Array1OfReal aPitches(1, 3);
  aPitches(1) = 2.0; // P1 (number of turns)
  aPitches(2) = 6.0; // P2 (number of turns)
  aPitches(3) = 2.0; // P3 (number of turns)

  TColStd_Array1OfBoolean aIsPitches(1, 3);
  aIsPitches(1) = Standard_False; // PF1 = 0
  aIsPitches(2) = Standard_False; // PF2 = 0
  aIsPitches(3) = Standard_False; // PF3 = 0

  aBuilder.SetParameters(myAxis, 100.0, 20.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());

  // For spiral helix, length calculation is more complex due to changing diameter
  // Just validate basic properties
  ValidateHelixWire(TopoDS::Wire(aHelixWire), 0); // Don't check exact length for complex spiral
}

// Test Case F1 - Using helix2 interface (TCL test F1)
TEST_F(HelixBRepTest, TCL_Test_F1_Helix2Interface)
{
  HelixBRep_BuilderHelix aBuilder;

  // Parameters from TCL test F1: comphelix2 with turns interface
  TColStd_Array1OfReal aPitches(1, 3);
  aPitches(1) = 10.0; // P1
  aPitches(2) = 10.0; // P2
  aPitches(3) = 10.0; // P3

  TColStd_Array1OfReal aNbTurns(1, 3);
  aNbTurns(1) = 2.0; // N1
  aNbTurns(2) = 6.0; // N2
  aNbTurns(3) = 2.0; // N3

  aBuilder.SetParameters(myAxis, 100.0, aPitches, aNbTurns);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());

  // Calculate expected length for cylindrical helix with multiple parts
  Standard_Real aCircumference  = M_PI * 100.0;
  Standard_Real aTotalTurns     = 2.0 + 6.0 + 2.0; // Sum of turns
  Standard_Real aExpectedLength = aTotalTurns * sqrt(aCircumference * aCircumference + 10.0 * 10.0);

  ValidateHelixWire(TopoDS::Wire(aHelixWire), aExpectedLength);
}

// Test Case E1 - Complex customer example (TCL test E1)
TEST_F(HelixBRepTest, TCL_Test_E1_CustomerExample)
{
  HelixBRep_BuilderHelix aBuilder;

  // Parameters from TCL test E1: Customer example with 5 parts
  TColStd_Array1OfReal aDiams(1, 6);
  aDiams(1) = 150.0; // D1
  aDiams(2) = 150.0; // D2
  aDiams(3) = 150.0; // D3
  aDiams(4) = 123.0; // D4
  aDiams(5) = 123.0; // D5
  aDiams(6) = 123.0; // D6

  // Calculate heights from pitches and turns
  TColStd_Array1OfReal aHeights(1, 5);
  aHeights(1) = 0.75 * 13.0; // N1 * P1
  aHeights(2) = 2.1 * 64.0;  // N2 * P2
  aHeights(3) = 2.25 * 50.0; // N3 * P3
  aHeights(4) = 2.5 * 45.0;  // N4 * P4
  aHeights(5) = 0.75 * 13.0; // N5 * P5

  TColStd_Array1OfReal aPitches(1, 5);
  aPitches(1) = 0.75; // N1 (number of turns)
  aPitches(2) = 2.1;  // N2
  aPitches(3) = 2.25; // N3
  aPitches(4) = 2.5;  // N4
  aPitches(5) = 0.75; // N5

  TColStd_Array1OfBoolean aIsPitches(1, 5);
  aIsPitches(1) = Standard_False; // Number of turns mode
  aIsPitches(2) = Standard_False;
  aIsPitches(3) = Standard_False;
  aIsPitches(4) = Standard_False;
  aIsPitches(5) = Standard_False;

  aBuilder.SetParameters(myAxis, aDiams, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());

  // Complex customer example - just validate basic properties
  ValidateHelixWire(TopoDS::Wire(aHelixWire),
                    0); // Don't check exact length for complex multi-part helix
}

// Test error conditions
TEST_F(HelixBRepTest, ErrorConditions_InvalidDimensions)
{
  HelixBRep_BuilderHelix aBuilder;

  // Test mismatched array dimensions
  TColStd_Array1OfReal    aDiams(1, 3);     // 3 elements = 2 parts
  TColStd_Array1OfReal    aHeights(1, 1);   // 1 element - should cause error (need 2 for 2 parts)
  TColStd_Array1OfReal    aPitches(1, 1);   // 1 element - should cause error (need 2 for 2 parts)
  TColStd_Array1OfBoolean aIsPitches(1, 1); // 1 element - should cause error (need 2 for 2 parts)

  aDiams(1)     = 100.0;
  aDiams(2)     = 100.0;
  aDiams(3)     = 100.0;
  aHeights(1)   = 50.0;
  aPitches(1)   = 10.0;
  aIsPitches(1) = Standard_True;

  EXPECT_THROW(aBuilder.SetParameters(myAxis, aDiams, aHeights, aPitches, aIsPitches),
               Standard_ConstructionError);
}

TEST_F(HelixBRepTest, ErrorConditions_TurnsInterface)
{
  HelixBRep_BuilderHelix aBuilder;

  // Test mismatched dimensions in turns interface
  TColStd_Array1OfReal aPitches(1, 2);
  TColStd_Array1OfReal aNbTurns(1, 1); // Mismatched size

  aPitches(1) = 10.0;
  aPitches(2) = 10.0;
  aNbTurns(1) = 5.0;

  EXPECT_THROW(aBuilder.SetParameters(myAxis, 100.0, aPitches, aNbTurns),
               Standard_ConstructionError);
}

// Test tolerance and approximation quality
TEST_F(HelixBRepTest, ApproximationQuality)
{
  HelixBRep_BuilderHelix aBuilder;

  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 50.0;

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 10.0;

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True;

  aBuilder.SetParameters(myAxis, 50.0, aHeights, aPitches, aIsPitches);

  // Test different tolerance levels
  Standard_Real aTightTolerance = 1.e-6;
  aBuilder.SetApproxParameters(aTightTolerance, 8, GeomAbs_C2);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  Standard_Real aToleranceReached = aBuilder.ToleranceReached();
  EXPECT_GT(aToleranceReached, 0.0);

  // For complex geometry, tolerance reached might be higher than requested
  // but should be reasonable
  EXPECT_LT(aToleranceReached, aTightTolerance * 1000);
}

// Test parameter validation
TEST_F(HelixBRepTest, ParameterValidation)
{
  HelixBRep_BuilderHelix aBuilder;

  TColStd_Array1OfReal    aHeights(1, 1);
  TColStd_Array1OfReal    aPitches(1, 1);
  TColStd_Array1OfBoolean aIsPitches(1, 1);

  // Test very small height - should cause error during Perform()
  aHeights(1)   = 1.e-8;
  aPitches(1)   = 10.0;
  aIsPitches(1) = Standard_True;

  aBuilder.SetParameters(myAxis, 50.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_NE(aBuilder.ErrorStatus(), 0); // Should have error status 12 (invalid height)

  // Test zero pitch - should cause error
  aHeights(1) = 50.0;
  aPitches(1) = 0.0;

  aBuilder.SetParameters(myAxis, 50.0, aHeights, aPitches, aIsPitches);

  aBuilder.Perform();

  EXPECT_NE(aBuilder.ErrorStatus(), 0); // Should have error status 11 (invalid pitch)
}

// Test wire continuity across multiple parts
TEST_F(HelixBRepTest, MultiPartContinuity)
{
  HelixBRep_BuilderHelix aBuilder;

  // 2-part helix with different parameters
  TColStd_Array1OfReal aDiams(1, 3);
  aDiams(1) = 80.0;
  aDiams(2) = 60.0;
  aDiams(3) = 40.0;

  TColStd_Array1OfReal aHeights(1, 2);
  aHeights(1) = 40.0;
  aHeights(2) = 40.0;

  TColStd_Array1OfReal aPitches(1, 2);
  aPitches(1) = 8.0;
  aPitches(2) = 12.0;

  TColStd_Array1OfBoolean aIsPitches(1, 2);
  aIsPitches(1) = Standard_True;
  aIsPitches(2) = Standard_True;

  aBuilder.SetParameters(myAxis, aDiams, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aHelixWire = aBuilder.Shape();
  EXPECT_FALSE(aHelixWire.IsNull());
  EXPECT_EQ(aHelixWire.ShapeType(), TopAbs_WIRE);

  // Check that wire has multiple edges
  Standard_Integer aNbEdges = CountShapes(aHelixWire, TopAbs_EDGE);
  EXPECT_GT(aNbEdges, 1); // Should have multiple edges for multiple parts

  // Verify wire continuity by checking connectivity
  TopoDS_Wire        aWire = TopoDS::Wire(aHelixWire);
  BRepCheck_Analyzer aCheck(aWire);
  EXPECT_TRUE(aCheck.IsValid());
}