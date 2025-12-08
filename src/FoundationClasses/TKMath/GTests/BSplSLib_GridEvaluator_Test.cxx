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
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Chronometer.hxx>
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

// Performance benchmark comparing GridEvaluator::EvaluateGrid with GeomAdaptor_Surface.
TEST(BSplSLib_GridEvaluatorTest, PerformanceComparison_GridEvaluatorVsAdaptorVsGeom)
{
  // Test parameters
  const int    aNbSamplesU   = 50;
  const int    aNbSamplesV   = 50;
  const int    aNbIterations = 1000;
  const double aUMin = 0.0, aUMax = 1.0;
  const double aVMin = 0.0, aVMax = 1.0;

  // B-spline surface parameters - large surface with 100+ poles
  const int aDegU     = 3;
  const int aDegV     = 3;
  const int aNbPolesU = 103; // 100 internal knots + degree + 1
  const int aNbPolesV = 103;
  const int aNbKnotsU = 101; // 100 internal + 2 boundary
  const int aNbKnotsV = 101;

  // Pre-compute parameter arrays
  Handle(TColStd_HArray1OfReal) aUParams = createUniformParams(aUMin, aUMax, aNbSamplesU);
  Handle(TColStd_HArray1OfReal) aVParams = createUniformParams(aVMin, aVMax, aNbSamplesV);

  // Create B-spline surface with 103x103 poles
  TColgp_Array2OfPnt aPoles2D(1, aNbPolesU, 1, aNbPolesV);
  for (int i = 1; i <= aNbPolesU; ++i)
  {
    for (int j = 1; j <= aNbPolesV; ++j)
    {
      const double x = (i - 1.0) / (aNbPolesU - 1);
      const double y = (j - 1.0) / (aNbPolesV - 1);
      const double z = 0.1 * std::sin(x * 6.0) * std::cos(y * 6.0);
      aPoles2D.SetValue(i, j, gp_Pnt(x, y, z));
    }
  }

  // Create knot vectors with 100 internal knots (uniform distribution)
  TColStd_Array1OfReal    aUKnots(1, aNbKnotsU);
  TColStd_Array1OfInteger aUMults(1, aNbKnotsU);
  TColStd_Array1OfReal    aVKnots(1, aNbKnotsV);
  TColStd_Array1OfInteger aVMults(1, aNbKnotsV);

  for (int i = 1; i <= aNbKnotsU; ++i)
  {
    aUKnots(i) = (i - 1.0) / (aNbKnotsU - 1);
    aVKnots(i) = (i - 1.0) / (aNbKnotsV - 1);
    // Clamped: multiplicity = degree+1 at ends, 1 at internal knots
    if (i == 1 || i == aNbKnotsU)
    {
      aUMults(i) = aDegU + 1;
      aVMults(i) = aDegV + 1;
    }
    else
    {
      aUMults(i) = 1;
      aVMults(i) = 1;
    }
  }

  Handle(Geom_BSplineSurface) aBSplineSurf =
    new Geom_BSplineSurface(aPoles2D, aUKnots, aVKnots, aUMults, aVMults, aDegU, aDegV);

  // ============================================================================
  // Method 1: GeomAdaptor_Surface - sequential access
  // ============================================================================
  double aAdaptorSeqTime = 0.0;
  {
    OSD_Chronometer aTimer;
    aTimer.Start();

    for (int iter = 0; iter < aNbIterations; ++iter)
    {
      GeomAdaptor_Surface anAdaptor(aBSplineSurf);
      for (int iu = 1; iu <= aNbSamplesU; ++iu)
      {
        for (int iv = 1; iv <= aNbSamplesV; ++iv)
        {
          gp_Pnt aPt;
          anAdaptor.D0(aUParams->Value(iu), aVParams->Value(iv), aPt);
        }
      }
    }

    aTimer.Stop();
    aTimer.Show(aAdaptorSeqTime);
  }

  // ============================================================================
  // Method 2: Geom_BSplineSurface::D0 - sequential access
  // ============================================================================
  double aGeomSeqTime = 0.0;
  {
    OSD_Chronometer aTimer;
    aTimer.Start();

    for (int iter = 0; iter < aNbIterations; ++iter)
    {
      for (int iu = 1; iu <= aNbSamplesU; ++iu)
      {
        for (int iv = 1; iv <= aNbSamplesV; ++iv)
        {
          gp_Pnt aPt;
          aBSplineSurf->D0(aUParams->Value(iu), aVParams->Value(iv), aPt);
        }
      }
    }

    aTimer.Stop();
    aTimer.Show(aGeomSeqTime);
  }

  // ============================================================================
  // Method 3: BSplSLib_GridEvaluator::EvaluateGrid - batch evaluation
  // ============================================================================
  double aGridEvalBatchTime = 0.0;
  {

    OSD_Chronometer aTimer;
    aTimer.Start();

    for (int iter = 0; iter < aNbIterations; ++iter)
    {
      BSplSLib_GridEvaluator aGridEval;
      aGridEval.Initialize(aDegU,
                           aDegV,
                           aBSplineSurf->HArrayPoles(),
                           aBSplineSurf->HArrayWeights(),
                           aBSplineSurf->HArrayUFlatKnots(),
                           aBSplineSurf->HArrayVFlatKnots(),
                           aBSplineSurf->IsURational(),
                           aBSplineSurf->IsVRational(),
                           aBSplineSurf->IsUPeriodic(),
                           aBSplineSurf->IsVPeriodic());
      aGridEval.SetUParams(aUParams);
      aGridEval.SetVParams(aVParams);

      {
        NCollection_Array2<gp_Pnt> aPoints = aGridEval.EvaluateGrid();
        (void)aPoints; // Prevent optimization
      }
    }
    aTimer.Stop();
    aTimer.Show(aGridEvalBatchTime);
  }

  // ============================================================================
  // Output timing results
  // ============================================================================
  const int aTotalPoints = aNbSamplesU * aNbSamplesV * aNbIterations;
  std::cout << "\n=== BSplSLib_GridEvaluator Performance Comparison ===\n";
  std::cout << "Surface: " << aNbPolesU << "x" << aNbPolesV << " poles, degree " << aDegU << "x"
            << aDegV << "\n";
  std::cout << "Grid: " << aNbSamplesU << "x" << aNbSamplesV << " samples, " << aNbIterations
            << " iterations\n";
  std::cout << "Total evaluations: " << aTotalPoints << "\n\n";

  std::cout << "--- Sequential Access (row-major iteration) ---\n";
  std::cout << "GeomAdaptor_Surface::D0():        " << aAdaptorSeqTime * 1000.0 << " ms ("
            << aTotalPoints / aAdaptorSeqTime / 1000000.0 << " M pts/sec)\n";
  std::cout << "Geom_BSplineSurface::D0():        " << aGeomSeqTime * 1000.0 << " ms ("
            << aTotalPoints / aGeomSeqTime / 1000000.0 << " M pts/sec)\n";
  std::cout << "GridEvaluator::EvaluateGrid():    " << aGridEvalBatchTime * 1000.0 << " ms ("
            << aTotalPoints / aGridEvalBatchTime / 1000000.0 << " M pts/sec)\n";

  std::cout << "\n--- Speedup Summary ---\n";
  if (aAdaptorSeqTime > 0.0)
  {
    std::cout << "vs GeomAdaptor:    " << aAdaptorSeqTime / aGridEvalBatchTime << "x\n";
  }
  if (aGeomSeqTime > 0.0)
  {
    std::cout << "vs Geom_BSpline:   " << aGeomSeqTime / aGridEvalBatchTime << "x\n";
  }
  std::cout << "=====================================================\n";

  // ============================================================================
  // Verify correctness
  // ============================================================================
  GeomAdaptor_Surface aVerifyAdaptor(aBSplineSurf);

  BSplSLib_GridEvaluator aVerifyGridEval;
  aVerifyGridEval.Initialize(aDegU,
                             aDegV,
                             aBSplineSurf->HArrayPoles(),
                             aBSplineSurf->HArrayWeights(),
                             aBSplineSurf->HArrayUFlatKnots(),
                             aBSplineSurf->HArrayVFlatKnots(),
                             aBSplineSurf->IsURational(),
                             aBSplineSurf->IsVRational(),
                             aBSplineSurf->IsUPeriodic(),
                             aBSplineSurf->IsVPeriodic());
  aVerifyGridEval.SetUParams(aUParams);
  aVerifyGridEval.SetVParams(aVParams);

  NCollection_Array2<gp_Pnt> aGridPoints = aVerifyGridEval.EvaluateGrid();

  for (int iu = 1; iu <= aNbSamplesU; iu += 10)
  {
    for (int iv = 1; iv <= aNbSamplesV; iv += 10)
    {
      gp_Pnt aPtAdaptor, aPtGeom;
      aVerifyAdaptor.D0(aUParams->Value(iu), aVParams->Value(iv), aPtAdaptor);
      aBSplineSurf->D0(aUParams->Value(iu), aVParams->Value(iv), aPtGeom);
      const gp_Pnt& aPtGrid = aGridPoints.Value(iu, iv);

      EXPECT_NEAR(aPtAdaptor.X(), aPtGrid.X(), Precision::Confusion());
      EXPECT_NEAR(aPtAdaptor.Y(), aPtGrid.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtAdaptor.Z(), aPtGrid.Z(), Precision::Confusion());

      EXPECT_NEAR(aPtGeom.X(), aPtGrid.X(), Precision::Confusion());
      EXPECT_NEAR(aPtGeom.Y(), aPtGrid.Y(), Precision::Confusion());
      EXPECT_NEAR(aPtGeom.Z(), aPtGrid.Z(), Precision::Confusion());
    }
  }

  // The grid evaluator should be comparable to or faster than GeomAdaptor
  // (allow 50% margin for Debug build variability)
  EXPECT_LE(aGridEvalBatchTime, aAdaptorSeqTime * 1.5);
}
