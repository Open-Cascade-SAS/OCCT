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
#include <HelixGeom_BuilderHelix.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_Tools.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

class TKHelixTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Default axis setup
    myAxis      = gp_Ax3(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
    myTolerance = 1.e-4;
  }

  // Helper method to count shapes in a TopoDS_Shape
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

  gp_Ax3        myAxis;
  Standard_Real myTolerance;
};

// Test HelixGeom_HelixCurve analytical implementation
TEST_F(TKHelixTest, HelixGeomHelixCurve_Basic)
{
  HelixGeom_HelixCurve aHelix;

  // Test default parameters
  EXPECT_DOUBLE_EQ(aHelix.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aHelix.LastParameter(), 2.0 * M_PI);

  // Test point calculation
  gp_Pnt aP0 = aHelix.Value(0.0);
  EXPECT_DOUBLE_EQ(aP0.X(), 1.0); // RStart = 1.0 by default
  EXPECT_DOUBLE_EQ(aP0.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aP0.Z(), 0.0);

  gp_Pnt aP1 = aHelix.Value(M_PI);
  EXPECT_DOUBLE_EQ(aP1.X(), -1.0);
  EXPECT_NEAR(aP1.Y(), 0.0, 1e-15);
  EXPECT_DOUBLE_EQ(aP1.Z(), M_PI / (2.0 * M_PI)); // Pitch / (2*PI) * t
}

TEST_F(TKHelixTest, HelixGeomHelixCurve_CustomParameters)
{
  HelixGeom_HelixCurve aHelix;
  aHelix.Load(0.0, 4.0 * M_PI, 10.0, 5.0, 0.0, Standard_True);

  EXPECT_DOUBLE_EQ(aHelix.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aHelix.LastParameter(), 4.0 * M_PI);

  // Test point at start
  gp_Pnt aP0 = aHelix.Value(0.0);
  EXPECT_DOUBLE_EQ(aP0.X(), 5.0); // RStart = 5.0
  EXPECT_DOUBLE_EQ(aP0.Y(), 0.0);
  EXPECT_DOUBLE_EQ(aP0.Z(), 0.0);

  // Test point at end (2 full turns)
  gp_Pnt aP1 = aHelix.Value(4.0 * M_PI);
  EXPECT_DOUBLE_EQ(aP1.X(), 5.0);
  EXPECT_NEAR(aP1.Y(), 0.0, 1e-14);
  EXPECT_DOUBLE_EQ(aP1.Z(), 20.0); // 2 turns * pitch(10)
}

TEST_F(TKHelixTest, HelixGeomHelixCurve_TaperedHelix)
{
  HelixGeom_HelixCurve aHelix;
  Standard_Real        aTaperAngle = 0.1; // Small taper angle
  aHelix.Load(0.0, 2.0 * M_PI, 5.0, 2.0, aTaperAngle, Standard_True);

  // At start
  gp_Pnt aP0 = aHelix.Value(0.0);
  EXPECT_DOUBLE_EQ(aP0.X(), 2.0);

  // At end - radius should be larger due to taper
  gp_Pnt        aP1             = aHelix.Value(2.0 * M_PI);
  Standard_Real aExpectedRadius = 2.0 + (5.0 / (2.0 * M_PI)) * tan(aTaperAngle) * (2.0 * M_PI);
  EXPECT_DOUBLE_EQ(aP1.X(), aExpectedRadius);
}

// Test HelixGeom_BuilderHelixCoil
TEST_F(TKHelixTest, HelixGeomBuilderHelixCoil_Basic)
{
  HelixGeom_BuilderHelixCoil aBuilder;

  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(0.0, 2.0 * M_PI, 5.0, 2.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);

  Handle(Geom_Curve) aCurve = aCurves(1);
  EXPECT_FALSE(aCurve.IsNull());

  // Test curve endpoints
  gp_Pnt aP1, aP2;
  aCurve->D0(aCurve->FirstParameter(), aP1);
  aCurve->D0(aCurve->LastParameter(), aP2);

  EXPECT_NEAR(aP1.X(), 2.0, myTolerance);
  EXPECT_NEAR(aP1.Y(), 0.0, myTolerance);
  EXPECT_NEAR(aP1.Z(), 0.0, myTolerance);

  EXPECT_NEAR(aP2.X(), 2.0, myTolerance);
  EXPECT_NEAR(aP2.Y(), 0.0, myTolerance);
  EXPECT_NEAR(aP2.Z(), 5.0, myTolerance);
}

// Test HelixGeom_BuilderHelix
TEST_F(TKHelixTest, HelixGeomBuilderHelix_SingleCoil)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);
  aBuilder.SetCurveParameters(0.0, 2.0 * M_PI, 10.0, 5.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 1);
}

TEST_F(TKHelixTest, HelixGeomBuilderHelix_MultipleCoils)
{
  HelixGeom_BuilderHelix aBuilder;

  gp_Ax2 aPosition(gp_Pnt(0., 0., 0.), gp_Dir(gp_Dir::D::Z), gp_Dir(gp_Dir::D::X));
  aBuilder.SetPosition(aPosition);
  aBuilder.SetTolerance(myTolerance);

  // 3 full turns
  aBuilder.SetCurveParameters(0.0, 6.0 * M_PI, 10.0, 5.0, 0.0, Standard_True);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TColGeom_SequenceOfCurve& aCurves = aBuilder.Curves();
  EXPECT_EQ(aCurves.Length(), 3); // Should split into 3 coils
}

// Test HelixBRep_BuilderHelix - Pure Cylindrical Helix
TEST_F(TKHelixTest, HelixBRepBuilder_PureCylindricalHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // Single part helix with diameter 100, height 100, pitch 20
  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0;

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 20.0;

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True; // Pitch mode

  aBuilder.SetParameters(myAxis, 100.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aResult = aBuilder.Shape();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult.ShapeType(), TopAbs_WIRE);

  // Check number of edges
  Standard_Integer aNbEdges = CountShapes(aResult, TopAbs_EDGE);
  EXPECT_GT(aNbEdges, 0);

  // Compute wire length - should be approximately sqrt(circumference^2 + height^2) * turns
  Standard_Real aWireLength     = ComputeWireLength(aResult);
  Standard_Real aCircumference  = M_PI * 100.0;
  Standard_Real aTurns          = 100.0 / 20.0; // height/pitch
  Standard_Real aExpectedLength = aTurns * sqrt(aCircumference * aCircumference + 20.0 * 20.0);

  EXPECT_NEAR(aWireLength, aExpectedLength, aExpectedLength * 0.01); // 1% tolerance
}

// Test HelixBRep_BuilderHelix - Spiral Helix
TEST_F(TKHelixTest, HelixBRepBuilder_SpiralHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // Single part spiral with diameter changing from 100 to 20
  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0;

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 10.0;

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True;

  aBuilder.SetParameters(myAxis, 100.0, 20.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aResult = aBuilder.Shape();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult.ShapeType(), TopAbs_WIRE);

  Standard_Integer aNbEdges = CountShapes(aResult, TopAbs_EDGE);
  EXPECT_GT(aNbEdges, 0);
}

// Test HelixBRep_BuilderHelix - Multi-part Helix
TEST_F(TKHelixTest, HelixBRepBuilder_MultiPartHelix)
{
  HelixBRep_BuilderHelix aBuilder;

  // 3-part helix with different diameters
  TColStd_Array1OfReal aDiams(1, 4);
  aDiams(1) = 100.0;
  aDiams(2) = 80.0;
  aDiams(3) = 60.0;
  aDiams(4) = 40.0;

  TColStd_Array1OfReal aHeights(1, 3);
  aHeights(1) = 30.0;
  aHeights(2) = 40.0;
  aHeights(3) = 30.0;

  TColStd_Array1OfReal aPitches(1, 3);
  aPitches(1) = 10.0;
  aPitches(2) = 15.0;
  aPitches(3) = 10.0;

  TColStd_Array1OfBoolean aIsPitches(1, 3);
  aIsPitches(1) = Standard_True;
  aIsPitches(2) = Standard_True;
  aIsPitches(3) = Standard_True;

  aBuilder.SetParameters(myAxis, aDiams, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aResult = aBuilder.Shape();
  EXPECT_FALSE(aResult.IsNull());
  EXPECT_EQ(aResult.ShapeType(), TopAbs_WIRE);

  Standard_Integer aNbEdges = CountShapes(aResult, TopAbs_EDGE);
  EXPECT_GT(aNbEdges, 2); // Should have multiple edges for multiple parts
}

// Test HelixBRep_BuilderHelix - Number of Turns Interface
TEST_F(TKHelixTest, HelixBRepBuilder_NumberOfTurnsInterface)
{
  HelixBRep_BuilderHelix aBuilder;

  // Single part helix using number of turns
  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 20.0;

  TColStd_Array1OfReal aNbTurns(1, 1);
  aNbTurns(1) = 5.0; // 5 turns

  aBuilder.SetParameters(myAxis, 100.0, aPitches, aNbTurns);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_EQ(aBuilder.ErrorStatus(), 0);

  const TopoDS_Shape& aResult = aBuilder.Shape();
  EXPECT_FALSE(aResult.IsNull());

  // Wire length should correspond to 5 turns
  Standard_Real aWireLength     = ComputeWireLength(aResult);
  Standard_Real aCircumference  = M_PI * 100.0;
  Standard_Real aExpectedLength = 5.0 * sqrt(aCircumference * aCircumference + 20.0 * 20.0);

  EXPECT_NEAR(aWireLength, aExpectedLength, aExpectedLength * 0.01);
}

// Test HelixGeom_Tools static methods
TEST_F(TKHelixTest, HelixGeomTools_ApprHelix)
{
  Handle(Geom_BSplineCurve) aBSpline;
  Standard_Real             aMaxError;

  Standard_Integer aResult = HelixGeom_Tools::ApprHelix(0.0,           // T1
                                                        2.0 * M_PI,    // T2
                                                        10.0,          // Pitch
                                                        5.0,           // Start radius
                                                        0.0,           // Taper angle
                                                        Standard_True, // Clockwise
                                                        myTolerance,   // Tolerance
                                                        aBSpline,      // Result
                                                        aMaxError      // Max error
  );

  EXPECT_EQ(aResult, 0); // Success
  EXPECT_FALSE(aBSpline.IsNull());
  EXPECT_LE(aMaxError, myTolerance);

  // Test curve properties
  EXPECT_GT(aBSpline->Degree(), 0);
  EXPECT_GT(aBSpline->NbPoles(), 0);
}

// Test Error Conditions
TEST_F(TKHelixTest, HelixBRepBuilder_ErrorConditions)
{
  HelixBRep_BuilderHelix aBuilder;

  // Test with invalid height (too small)
  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 1.e-6; // Very small height

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 10.0;

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True;

  aBuilder.SetParameters(myAxis, 100.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_NE(aBuilder.ErrorStatus(), 0); // Should have error
}

TEST_F(TKHelixTest, HelixBRepBuilder_ZeroPitch)
{
  HelixBRep_BuilderHelix aBuilder;

  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0;

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 0.0; // Zero pitch

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True;

  aBuilder.SetParameters(myAxis, 100.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(myTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  EXPECT_NE(aBuilder.ErrorStatus(), 0); // Should have error
}

// Test Tolerance Reached
TEST_F(TKHelixTest, HelixBRepBuilder_ToleranceReached)
{
  HelixBRep_BuilderHelix aBuilder;

  TColStd_Array1OfReal aHeights(1, 1);
  aHeights(1) = 100.0;

  TColStd_Array1OfReal aPitches(1, 1);
  aPitches(1) = 20.0;

  TColStd_Array1OfBoolean aIsPitches(1, 1);
  aIsPitches(1) = Standard_True;

  Standard_Real aVeryTightTolerance = 1.e-8;
  aBuilder.SetParameters(myAxis, 100.0, aHeights, aPitches, aIsPitches);
  aBuilder.SetApproxParameters(aVeryTightTolerance, 8, GeomAbs_C1);

  aBuilder.Perform();

  if (aBuilder.ErrorStatus() == 0)
  {
    Standard_Real aToleranceReached = aBuilder.ToleranceReached();
    EXPECT_GT(aToleranceReached, 0.0);
  }
}