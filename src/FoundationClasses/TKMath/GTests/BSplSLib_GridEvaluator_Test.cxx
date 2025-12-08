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

#include <BSplSLib_GridEvaluator.hxx>

#include <BSplSLib.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

namespace
{

//! Creates a quadratic B-spline surface (degree 2x2) with planar geometry z = x + y.
void createQuadraticBSplineSurface(Handle(TColgp_HArray2OfPnt)&   thePoles,
                                   Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                   Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                   int&                           theDegreeU,
                                   int&                           theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  thePoles = new TColgp_HArray2OfPnt(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      const double x = (i - 1) / 3.0;
      const double y = (j - 1) / 3.0;
      const double z = x + y;
      thePoles->SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Clamped knot vector with one internal knot at 0.5
  theUFlatKnots = new TColStd_HArray1OfReal(1, 7);
  theUFlatKnots->SetValue(1, 0.0);
  theUFlatKnots->SetValue(2, 0.0);
  theUFlatKnots->SetValue(3, 0.0);
  theUFlatKnots->SetValue(4, 0.5);
  theUFlatKnots->SetValue(5, 1.0);
  theUFlatKnots->SetValue(6, 1.0);
  theUFlatKnots->SetValue(7, 1.0);

  theVFlatKnots = new TColStd_HArray1OfReal(1, 7);
  theVFlatKnots->SetValue(1, 0.0);
  theVFlatKnots->SetValue(2, 0.0);
  theVFlatKnots->SetValue(3, 0.0);
  theVFlatKnots->SetValue(4, 0.5);
  theVFlatKnots->SetValue(5, 1.0);
  theVFlatKnots->SetValue(6, 1.0);
  theVFlatKnots->SetValue(7, 1.0);
}

//! Creates a rational B-spline surface (degree 2x2) with weights.
void createRationalBSplineSurface(Handle(TColgp_HArray2OfPnt)&   thePoles,
                                  Handle(TColStd_HArray2OfReal)& theWeights,
                                  Handle(TColStd_HArray1OfReal)& theUFlatKnots,
                                  Handle(TColStd_HArray1OfReal)& theVFlatKnots,
                                  int&                           theDegreeU,
                                  int&                           theDegreeV)
{
  theDegreeU = 2;
  theDegreeV = 2;

  thePoles   = new TColgp_HArray2OfPnt(1, 3, 1, 3);
  theWeights = new TColStd_HArray2OfReal(1, 3, 1, 3);

  const double aW = 1.0 / std::sqrt(2.0);

  thePoles->SetValue(1, 1, gp_Pnt(1.0, 0.0, 0.0));
  thePoles->SetValue(1, 2, gp_Pnt(1.0, 1.0, 0.0));
  thePoles->SetValue(1, 3, gp_Pnt(0.0, 1.0, 0.0));
  thePoles->SetValue(2, 1, gp_Pnt(1.0, 0.0, 1.0));
  thePoles->SetValue(2, 2, gp_Pnt(1.0, 1.0, 1.0));
  thePoles->SetValue(2, 3, gp_Pnt(0.0, 1.0, 1.0));
  thePoles->SetValue(3, 1, gp_Pnt(0.0, 0.0, 1.0));
  thePoles->SetValue(3, 2, gp_Pnt(0.0, 0.0, 1.0));
  thePoles->SetValue(3, 3, gp_Pnt(0.0, 0.0, 1.0));

  theWeights->SetValue(1, 1, 1.0);
  theWeights->SetValue(1, 2, aW);
  theWeights->SetValue(1, 3, 1.0);
  theWeights->SetValue(2, 1, aW);
  theWeights->SetValue(2, 2, 0.5);
  theWeights->SetValue(2, 3, aW);
  theWeights->SetValue(3, 1, 1.0);
  theWeights->SetValue(3, 2, aW);
  theWeights->SetValue(3, 3, 1.0);

  // Clamped Bezier-style knots for single span
  theUFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theUFlatKnots->SetValue(1, 0.0);
  theUFlatKnots->SetValue(2, 0.0);
  theUFlatKnots->SetValue(3, 0.0);
  theUFlatKnots->SetValue(4, 1.0);
  theUFlatKnots->SetValue(5, 1.0);
  theUFlatKnots->SetValue(6, 1.0);

  theVFlatKnots = new TColStd_HArray1OfReal(1, 6);
  theVFlatKnots->SetValue(1, 0.0);
  theVFlatKnots->SetValue(2, 0.0);
  theVFlatKnots->SetValue(3, 0.0);
  theVFlatKnots->SetValue(4, 1.0);
  theVFlatKnots->SetValue(5, 1.0);
  theVFlatKnots->SetValue(6, 1.0);
}

//! Creates uniform parameter array.
Handle(TColStd_HArray1OfReal) createUniformParams(double theMin, double theMax, int theNbSamples)
{
  Handle(TColStd_HArray1OfReal) aParams = new TColStd_HArray1OfReal(1, theNbSamples);
  const double                  aStep   = (theMax - theMin) / (theNbSamples - 1);
  for (int i = 1; i <= theNbSamples; ++i)
  {
    aParams->SetValue(i, theMin + (i - 1) * aStep);
  }
  // Ensure exact boundaries
  aParams->SetValue(1, theMin);
  aParams->SetValue(theNbSamples, theMax);
  return aParams;
}

} // namespace

// Verifies that default-constructed evaluator is in uninitialized state.
TEST(BSplSLib_GridEvaluatorTest, DefaultConstructor)
{
  BSplSLib_GridEvaluator anEval;
  EXPECT_FALSE(anEval.IsInitialized());
  EXPECT_EQ(anEval.NbUParams(), 0);
  EXPECT_EQ(anEval.NbVParams(), 0);
}

// Verifies successful initialization with valid B-spline surface data.
TEST(BSplSLib_GridEvaluatorTest, Initialize)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(aDegU,
                                      aDegV,
                                      aPoles,
                                      Handle(TColStd_HArray2OfReal)(),
                                      aUKnots,
                                      aVKnots,
                                      false,
                                      false,
                                      false,
                                      false);

  EXPECT_TRUE(isOk);
  EXPECT_TRUE(anEval.IsInitialized());
}

// Verifies that initialization fails when degree is zero.
TEST(BSplSLib_GridEvaluatorTest, InitializeValidation_InvalidDegree)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  const bool             isOk = anEval.Initialize(0, // Invalid degree
                                      aDegV,
                                      aPoles,
                                      Handle(TColStd_HArray2OfReal)(),
                                      aUKnots,
                                      aVKnots,
                                      false,
                                      false,
                                      false,
                                      false);

  EXPECT_FALSE(isOk);
  EXPECT_FALSE(anEval.IsInitialized());
}

// Verifies that EvaluateGrid produces correct results matching BSplSLib::D0.
TEST(BSplSLib_GridEvaluatorTest, EvaluateGrid_MatchesBSplSLib)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  Handle(TColStd_HArray1OfReal) aUParams = createUniformParams(0.0, 1.0, 5);
  Handle(TColStd_HArray1OfReal) aVParams = createUniformParams(0.0, 1.0, 5);

  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

  EXPECT_EQ(anEval.NbUParams(), 5);
  EXPECT_EQ(anEval.NbVParams(), 5);

  NCollection_Array2<gp_Pnt> aGridPoints = anEval.EvaluateGrid();

  EXPECT_EQ(aGridPoints.RowLength(), 5);
  EXPECT_EQ(aGridPoints.ColLength(), 5);

  // Compare with direct BSplSLib::D0 evaluation
  for (int iu = 1; iu <= 5; ++iu)
  {
    for (int iv = 1; iv <= 5; ++iv)
    {
      const gp_Pnt& aPtGrid = aGridPoints.Value(iu, iv);

      gp_Pnt aPtDirect;
      int    aUSpan = 0, aVSpan = 0;
      double aUParam = aUParams->Value(iu);
      double aVParam = aVParams->Value(iv);

      BSplCLib::LocateParameter(aDegU,
                                aUKnots->Array1(),
                                BSplCLib::NoMults(),
                                aUParam,
                                false,
                                aUSpan,
                                aUParam);
      BSplCLib::LocateParameter(aDegV,
                                aVKnots->Array1(),
                                BSplCLib::NoMults(),
                                aVParam,
                                false,
                                aVSpan,
                                aVParam);

      BSplSLib::D0(aUParam,
                   aVParam,
                   aUSpan,
                   aVSpan,
                   aPoles->Array2(),
                   nullptr,
                   aUKnots->Array1(),
                   aVKnots->Array1(),
                   nullptr,
                   nullptr,
                   aDegU,
                   aDegV,
                   false,
                   false,
                   false,
                   false,
                   aPtDirect);

      EXPECT_NEAR(aPtGrid.X(), aPtDirect.X(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Y(), aPtDirect.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Z(), aPtDirect.Z(), Precision::Confusion());
    }
  }
}

// Verifies that EvaluateGrid works with rational surfaces.
TEST(BSplSLib_GridEvaluatorTest, EvaluateGrid_RationalSurface)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray2OfReal) aWeights;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createRationalBSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU, aDegV, aPoles, aWeights, aUKnots, aVKnots, true, true, false, false);

  Handle(TColStd_HArray1OfReal) aUParams = createUniformParams(0.0, 1.0, 5);
  Handle(TColStd_HArray1OfReal) aVParams = createUniformParams(0.0, 1.0, 5);

  anEval.SetUParams(aUParams);
  anEval.SetVParams(aVParams);

  NCollection_Array2<gp_Pnt> aGridPoints = anEval.EvaluateGrid();

  EXPECT_EQ(aGridPoints.RowLength(), 5);
  EXPECT_EQ(aGridPoints.ColLength(), 5);

  // Compare with direct BSplSLib::D0 evaluation
  for (int iu = 1; iu <= 5; ++iu)
  {
    for (int iv = 1; iv <= 5; ++iv)
    {
      const gp_Pnt& aPtGrid = aGridPoints.Value(iu, iv);

      gp_Pnt aPtDirect;
      int    aUSpan = 0, aVSpan = 0;
      double aUParam = aUParams->Value(iu);
      double aVParam = aVParams->Value(iv);

      BSplCLib::LocateParameter(aDegU,
                                aUKnots->Array1(),
                                BSplCLib::NoMults(),
                                aUParam,
                                false,
                                aUSpan,
                                aUParam);
      BSplCLib::LocateParameter(aDegV,
                                aVKnots->Array1(),
                                BSplCLib::NoMults(),
                                aVParam,
                                false,
                                aVSpan,
                                aVParam);

      BSplSLib::D0(aUParam,
                   aVParam,
                   aUSpan,
                   aVSpan,
                   aPoles->Array2(),
                   &aWeights->Array2(),
                   aUKnots->Array1(),
                   aVKnots->Array1(),
                   nullptr,
                   nullptr,
                   aDegU,
                   aDegV,
                   true,
                   true,
                   false,
                   false,
                   aPtDirect);

      EXPECT_NEAR(aPtGrid.X(), aPtDirect.X(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Y(), aPtDirect.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtGrid.Z(), aPtDirect.Z(), Precision::Confusion());
    }
  }
}

// Verifies that EvaluateGrid returns empty array when not initialized.
TEST(BSplSLib_GridEvaluatorTest, EvaluateGrid_NotInitialized)
{
  BSplSLib_GridEvaluator anEval;

  NCollection_Array2<gp_Pnt> aGridPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aGridPoints.IsEmpty());
}

// Verifies that EvaluateGrid returns empty array when no parameters set.
TEST(BSplSLib_GridEvaluatorTest, EvaluateGrid_NoParams)
{
  Handle(TColgp_HArray2OfPnt)   aPoles;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  int                           aDegU, aDegV;

  createQuadraticBSplineSurface(aPoles, aUKnots, aVKnots, aDegU, aDegV);

  BSplSLib_GridEvaluator anEval;
  anEval.Initialize(aDegU,
                    aDegV,
                    aPoles,
                    Handle(TColStd_HArray2OfReal)(),
                    aUKnots,
                    aVKnots,
                    false,
                    false,
                    false,
                    false);

  NCollection_Array2<gp_Pnt> aGridPoints = anEval.EvaluateGrid();

  EXPECT_TRUE(aGridPoints.IsEmpty());
}
