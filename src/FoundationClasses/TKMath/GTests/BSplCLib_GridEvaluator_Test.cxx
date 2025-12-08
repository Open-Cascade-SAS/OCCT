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

#include <BSplCLib.hxx>
#include <BSplCLib_GridEvaluator.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

// Helper function to create a simple cubic B-spline curve.
static void CreateSimpleBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                     Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                     int&                           theDegree)
{
  theDegree = 3;

  // 5 control points
  thePoles = new TColgp_HArray1OfPnt(1, 5);
  thePoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(2.0, 2.0, 1.0));
  thePoles->SetValue(4, gp_Pnt(3.0, 1.0, 1.0));
  thePoles->SetValue(5, gp_Pnt(4.0, 0.0, 0.0));

  // Flat knots for degree 3 with 5 control points
  // n + p + 1 = 5 + 3 + 1 = 9 knots
  // Clamped knot vector: [0, 0, 0, 0, 0.5, 1, 1, 1, 1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 9);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 0.0);
  theFlatKnots->SetValue(5, 0.5);
  theFlatKnots->SetValue(6, 1.0);
  theFlatKnots->SetValue(7, 1.0);
  theFlatKnots->SetValue(8, 1.0);
  theFlatKnots->SetValue(9, 1.0);
}

// Helper function to create a rational B-spline curve (circular arc).
static void CreateRationalBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                       Handle(TColStd_HArray1OfReal)& theWeights,
                                       Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                       int&                           theDegree)
{
  theDegree = 2;

  // 3 control points for a 90-degree circular arc
  thePoles = new TColgp_HArray1OfPnt(1, 3);
  thePoles->SetValue(1, gp_Pnt(1.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 1.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(0.0, 1.0, 0.0));

  // Weights for circular arc
  theWeights = new TColStd_HArray1OfReal(1, 3);
  theWeights->SetValue(1, 1.0);
  theWeights->SetValue(2, 1.0 / std::sqrt(2.0));
  theWeights->SetValue(3, 1.0);

  // Flat knots: [0,0,0,1,1,1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 1.0);
  theFlatKnots->SetValue(5, 1.0);
  theFlatKnots->SetValue(6, 1.0);
}

// Helper to create a multi-span B-spline curve with 3 spans.
static void CreateMultiSpanBSplineCurve(Handle(TColgp_HArray1OfPnt)&   thePoles,
                                        Handle(TColStd_HArray1OfReal)& theFlatKnots,
                                        int&                           theDegree)
{
  theDegree = 3;

  // 7 control points for 3 spans
  thePoles = new TColgp_HArray1OfPnt(1, 7);
  thePoles->SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  thePoles->SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  thePoles->SetValue(3, gp_Pnt(2.0, 3.0, 1.0));
  thePoles->SetValue(4, gp_Pnt(3.0, 2.5, 1.5));
  thePoles->SetValue(5, gp_Pnt(4.0, 1.5, 1.0));
  thePoles->SetValue(6, gp_Pnt(5.0, 0.5, 0.5));
  thePoles->SetValue(7, gp_Pnt(6.0, 0.0, 0.0));

  // Flat knots for degree 3 with 7 control points: n+p+1 = 7+3+1 = 11 knots
  // Clamped knot vector with 3 internal knots: [0,0,0,0, 0.33, 0.5, 0.67, 1,1,1,1]
  theFlatKnots = new TColStd_HArray1OfReal(1, 11);
  theFlatKnots->SetValue(1, 0.0);
  theFlatKnots->SetValue(2, 0.0);
  theFlatKnots->SetValue(3, 0.0);
  theFlatKnots->SetValue(4, 0.0);
  theFlatKnots->SetValue(5, 0.33);
  theFlatKnots->SetValue(6, 0.5);
  theFlatKnots->SetValue(7, 0.67);
  theFlatKnots->SetValue(8, 1.0);
  theFlatKnots->SetValue(9, 1.0);
  theFlatKnots->SetValue(10, 1.0);
  theFlatKnots->SetValue(11, 1.0);
}

TEST(BSplCLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplCLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbParams(), 0);
}

TEST(BSplCLib_GridEvaluatorTest, Initialize)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDeg,
                                      aPoles,
                                      Handle(TColStd_HArray1OfReal)(), // non-rational
                                      aKnots,
                                      false,  // not rational
                                      false); // not periodic

  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, InitializeValidation_InvalidDegree)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;

  // Degree 0 should fail
  const bool isOk =
    anEval.Initialize(0, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, InitializeValidation_RationalWithoutWeights)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;

  // Rational curve without weights should fail
  const bool isOk = anEval.Initialize(aDeg,
                                      aPoles,
                                      Handle(TColStd_HArray1OfReal)(), // No weights
                                      aKnots,
                                      true, // Rational flag set
                                      false);
  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

TEST(BSplCLib_GridEvaluatorTest, SetParams)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create parameters array
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aParams->SetValue(i, (i - 1) / 9.0);
  }

  anEval.SetParams(aParams);

  EXPECT_EQ(anEval.NbParams(), 10);
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_NonRational)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create parameters array
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aParams->SetValue(i, (i - 1) / 9.0);
  }
  anEval.SetParams(aParams);

  // Evaluate grid
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  // Check array size matches number of parameters
  EXPECT_EQ(aPoints.Length(), 10);
  EXPECT_EQ(aPoints.Lower(), 1);
  EXPECT_EQ(aPoints.Upper(), 10);

  // First and last should be at curve endpoints
  EXPECT_NEAR(aPoints.Value(1).X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPoints.Value(1).Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aPoints.Value(10).X(), 4.0, Precision::Confusion());
  EXPECT_NEAR(aPoints.Value(10).Y(), 0.0, Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_Rational)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateRationalBSplineCurve(aPoles, aWeights, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDeg, aPoles, aWeights, aKnots, true, false);

  ASSERT_TRUE(isOk);

  // Create parameters array
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aParams->SetValue(i, (i - 1) / 9.0);
  }
  anEval.SetParams(aParams);

  // Evaluate grid
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  // For a circular arc, all points should be at distance 1 from origin
  for (int i = 1; i <= aPoints.Length(); ++i)
  {
    const gp_Pnt& aPt     = aPoints.Value(i);
    const double  aRadius = std::sqrt(aPt.X() * aPt.X() + aPt.Y() * aPt.Y());
    EXPECT_NEAR(aRadius, 1.0, 1e-10);
  }
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_NotInitialized)
{
  BSplCLib_GridEvaluator anEval;

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aPoints.IsEmpty());
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_NoParams)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  // Note: SetParams not called

  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aPoints.IsEmpty());
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_CompareWithBSplCLib)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create parameters array
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aParams->SetValue(i, (i - 1) / 9.0);
  }
  anEval.SetParams(aParams);

  // Evaluate grid
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  // Compare with direct BSplCLib evaluation
  for (int i = 1; i <= aParams->Length(); ++i)
  {
    const double aParam = aParams->Value(i);

    // Find span index for direct evaluation
    int    aSpanIndex = 0;
    double aNewParam  = aParam;
    BSplCLib::LocateParameter(aDeg,
                              aKnots->Array1(),
                              BSplCLib::NoMults(),
                              aParam,
                              false, // not periodic
                              aSpanIndex,
                              aNewParam);

    // Direct evaluation using BSplCLib
    gp_Pnt aPtDirect;
    BSplCLib::D0(aParam,
                 aSpanIndex,
                 aDeg,
                 false, // not periodic
                 aPoles->Array1(),
                 nullptr,
                 aKnots->Array1(),
                 nullptr, // NoMults for flat knots
                 aPtDirect);

    const gp_Pnt& aPtGrid = aPoints.Value(i);
    EXPECT_NEAR(aPtGrid.X(), aPtDirect.X(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Y(), aPtDirect.Y(), Precision::Confusion());
    EXPECT_NEAR(aPtGrid.Z(), aPtDirect.Z(), Precision::Confusion());
  }
}

TEST(BSplCLib_GridEvaluatorTest, EvaluateGrid_MultiSpan)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateMultiSpanBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create many parameters to span multiple knot spans
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 100);
  for (int i = 1; i <= 100; ++i)
  {
    aParams->SetValue(i, (i - 1) / 99.0);
  }
  anEval.SetParams(aParams);

  // Evaluate grid
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();

  EXPECT_EQ(aPoints.Length(), 100);

  // Verify all points are valid
  gp_Pnt aPrevPt;
  for (int i = 1; i <= aPoints.Length(); ++i)
  {
    const gp_Pnt& aPt = aPoints.Value(i);
    EXPECT_FALSE(std::isnan(aPt.X()));
    EXPECT_FALSE(std::isnan(aPt.Y()));
    EXPECT_FALSE(std::isnan(aPt.Z()));

    // Points should form a continuous curve
    if (i > 1)
    {
      const double aDist = aPt.Distance(aPrevPt);
      EXPECT_LT(aDist, 0.25); // Reasonable distance for dense sampling
    }
    aPrevPt = aPt;
  }
}

TEST(BSplCLib_GridEvaluatorTest, SetParams_NullHandle)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Call with null handle - should not crash
  anEval.SetParams(Handle(TColStd_HArray1OfReal)());

  // Parameters should remain empty
  EXPECT_EQ(anEval.NbParams(), 0);
}

TEST(BSplCLib_GridEvaluatorTest, ReinitializationResetsState)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create and set parameters
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aParams->SetValue(i, (i - 1) / 9.0);
  }
  anEval.SetParams(aParams);

  // Evaluate to populate cache
  NCollection_Array1<gp_Pnt> aPoints1 = anEval.EvaluateGrid();
  EXPECT_EQ(aPoints1.Length(), 10);

  // Create a different curve
  Handle(TColgp_HArray1OfPnt) aNewPoles = new TColgp_HArray1OfPnt(1, 5);
  aNewPoles->SetValue(1, gp_Pnt(10.0, 0.0, 0.0));
  aNewPoles->SetValue(2, gp_Pnt(11.0, 2.0, 0.0));
  aNewPoles->SetValue(3, gp_Pnt(12.0, 2.0, 1.0));
  aNewPoles->SetValue(4, gp_Pnt(13.0, 1.0, 1.0));
  aNewPoles->SetValue(5, gp_Pnt(14.0, 0.0, 0.0));

  // Reinitialize with new poles
  anEval.Initialize(aDeg, aNewPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);
  anEval.SetParams(aParams);

  // Evaluate - should get points from new curve
  NCollection_Array1<gp_Pnt> aPoints2 = anEval.EvaluateGrid();
  EXPECT_EQ(aPoints2.Length(), 10);
  EXPECT_NEAR(aPoints2.Value(1).X(), 10.0, Precision::Confusion());
}

TEST(BSplCLib_GridEvaluatorTest, SingleParameter)
{
  Handle(TColgp_HArray1OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aKnots;
  int                           aDeg;

  CreateSimpleBSplineCurve(aPoles, aKnots, aDeg);

  BSplCLib_GridEvaluator anEval;
  anEval.Initialize(aDeg, aPoles, Handle(TColStd_HArray1OfReal)(), aKnots, false, false);

  // Create single parameter
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, 1);
  aParams->SetValue(1, 0.5);
  anEval.SetParams(aParams);

  EXPECT_EQ(anEval.NbParams(), 1);

  // Evaluate grid
  NCollection_Array1<gp_Pnt> aPoints = anEval.EvaluateGrid();
  EXPECT_EQ(aPoints.Length(), 1);
  EXPECT_FALSE(std::isnan(aPoints.Value(1).X()));
}
