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

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

//==================================================================================================
// Test fixture for Geom2dAdaptor_Curve degenerated curve handling
//==================================================================================================

class Geom2dAdaptor_Curve_Test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a simple 2D line for testing
    gp_Pnt2d aP1(0.0, 0.0);
    gp_Lin2d aLine(aP1, gp_Dir2d(1.0, 1.0));
    myLine = new Geom2d_Line(aLine);

    // Create a 2D circle for testing
    gp_Pnt2d  aCenter(5.0, 5.0);
    gp_Circ2d aCirc(gp_Ax2d(aCenter, gp_Dir2d(1.0, 0.0)), 3.0);
    myCircle = new Geom2d_Circle(aCirc);
  }

  Handle(Geom2d_Line)   myLine;
  Handle(Geom2d_Circle) myCircle;
};

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_ValidParameters_Success)
{
  // Test loading with valid parameters
  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, 0.0, 10.0));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 10.0);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_EqualParameters_Success)
{
  // Test loading with equal parameters (degenerated curve)
  // This should be allowed as it represents a point
  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, 5.0, 5.0));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 5.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 5.0);

  // Verify it evaluates to a single point
  gp_Pnt2d aP1 = anAdaptor.Value(5.0);
  gp_Pnt2d aP2 = myLine->Value(5.0);
  EXPECT_TRUE(aP1.IsEqual(aP2, Precision::Confusion()));
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_ParametersWithinConfusion_Success)
{
  // Test loading with parameters within Precision::Confusion()
  // This should be allowed (degenerated curve handling)
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 + Precision::Confusion() * 0.5;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_ParametersAtConfusionBoundary_Success)
{
  // Test loading with parameters exactly at the confusion tolerance boundary
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 + Precision::Confusion();

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_FirstGreaterThanLastWithinConfusion_Success)
{
  // Test loading with theUFirst > theULast but within Precision::Confusion()
  // This represents a degenerated curve and should be allowed
  const double aParam1 = 5.0 + Precision::Confusion() * 0.5;
  const double aParam2 = 5.0;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myLine, aParam1, aParam2));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam1);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam2);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_FirstGreaterThanLastBeyondConfusion_ThrowsException)
{
  // Test loading with UFirst > ULast + Precision::Confusion()
  // This should throw Standard_ConstructionError
  const double aParam1 = 10.0;
  const double aParam2 = 5.0;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_FirstSlightlyGreaterThanLast_ThrowsException)
{
  // Test loading with UFirst slightly greater than ULast beyond tolerance
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 - Precision::Confusion() * 2.0;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Constructor_DegeneratedCurve_Success)
{
  // Test constructor with degenerated curve parameters
  EXPECT_NO_THROW(Geom2dAdaptor_Curve anAdaptor(myCircle, 0.0, 0.0));

  Geom2dAdaptor_Curve anAdaptor(myCircle, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), 0.0);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Constructor_InvalidParameters_ThrowsException)
{
  // Test constructor with invalid parameters
  EXPECT_THROW(Geom2dAdaptor_Curve anAdaptor(myCircle, 10.0, 0.0), Standard_ConstructionError);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, Load_NullCurve_ThrowsException)
{
  // Test loading with null curve
  Handle(Geom2d_Curve) aNullCurve;
  Geom2dAdaptor_Curve  anAdaptor;

  EXPECT_THROW(anAdaptor.Load(aNullCurve, 0.0, 10.0), Standard_NullObject);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, DegeneratedCurve_CircleAtZeroLength_Success)
{
  // Test degenerated circle (zero length arc)
  const double aParam = M_PI;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myCircle, aParam, aParam));

  // Verify the curve represents a single point
  gp_Pnt2d aPoint         = anAdaptor.Value(aParam);
  gp_Pnt2d aExpectedPoint = myCircle->Value(aParam);

  EXPECT_TRUE(aPoint.IsEqual(aExpectedPoint, Precision::Confusion()));
  EXPECT_TRUE(anAdaptor.IsClosed() || anAdaptor.FirstParameter() == anAdaptor.LastParameter());
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, DegeneratedCurve_TrimmedCurve_Success)
{
  // Create a trimmed curve and test degenerated case
  Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(myLine, 0.0, 20.0);

  const double        aParam = 10.0;
  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(aTrimmedCurve, aParam, aParam));

  EXPECT_DOUBLE_EQ(anAdaptor.FirstParameter(), aParam);
  EXPECT_DOUBLE_EQ(anAdaptor.LastParameter(), aParam);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, ToleranceBoundary_NegativeCase_ThrowsException)
{
  // Test parameters just beyond the negative tolerance boundary
  const double aParam1 = 5.0;
  const double aParam2 = 5.0 - Precision::Confusion() - 1e-10;

  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_THROW(anAdaptor.Load(myLine, aParam1, aParam2), Standard_ConstructionError);
}

//==================================================================================================

TEST_F(Geom2dAdaptor_Curve_Test, LoadWithoutParameters_Success)
{
  // Test loading curve without specifying parameters (uses curve's own parameters)
  Geom2dAdaptor_Curve anAdaptor;
  EXPECT_NO_THROW(anAdaptor.Load(myCircle));

  EXPECT_NEAR(anAdaptor.FirstParameter(), myCircle->FirstParameter(), Precision::Confusion());
  EXPECT_NEAR(anAdaptor.LastParameter(), myCircle->LastParameter(), Precision::Confusion());
  EXPECT_TRUE(anAdaptor.IsPeriodic());
}
