#include <gtest/gtest.h>

#include <Convert_CompPolynomialToPoles.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>

TEST(Convert_CompPolynomialToPolesTest, SingleLinearPolynomial)
{
  // Convert f(x) = 2*x + 1 on [-1,1] to BSpline
  // Polynomial domain [-1,1], True interval [-1,1]
  const int aDim = 1;
  const int aMaxDeg = 1;
  const int aDeg = 1;

  NCollection_Array1<double> aCoeffs(1, 2);
  aCoeffs(1) = 1.0; // constant term
  aCoeffs(2) = 2.0; // linear term

  NCollection_Array1<double> aPolyIntervals(1, 2);
  aPolyIntervals(1) = -1.0;
  aPolyIntervals(2) = 1.0;

  NCollection_Array1<double> aTrueIntervals(1, 2);
  aTrueIntervals(1) = -1.0;
  aTrueIntervals(2) = 1.0;

  Convert_CompPolynomialToPoles aConv(aDim, aMaxDeg, aDeg, aCoeffs, aPolyIntervals, aTrueIntervals);

  EXPECT_TRUE(aConv.IsDone());
  EXPECT_EQ(aConv.Degree(), 1);
  EXPECT_EQ(aConv.NbKnots(), 2);
  EXPECT_GT(aConv.NbPoles(), 0);
}

TEST(Convert_CompPolynomialToPolesTest, SingleQuadraticPolynomial)
{
  // Convert f(x) = x^2 on [0,1] to BSpline
  const int aDim = 1;
  const int aMaxDeg = 2;
  const int aDeg = 2;

  NCollection_Array1<double> aCoeffs(1, 3);
  aCoeffs(1) = 0.0; // x^0
  aCoeffs(2) = 0.0; // x^1
  aCoeffs(3) = 1.0; // x^2

  NCollection_Array1<double> aPolyIntervals(1, 2);
  aPolyIntervals(1) = 0.0;
  aPolyIntervals(2) = 1.0;

  NCollection_Array1<double> aTrueIntervals(1, 2);
  aTrueIntervals(1) = 0.0;
  aTrueIntervals(2) = 1.0;

  Convert_CompPolynomialToPoles aConv(aDim, aMaxDeg, aDeg, aCoeffs, aPolyIntervals, aTrueIntervals);

  EXPECT_TRUE(aConv.IsDone());
  EXPECT_EQ(aConv.Degree(), 2);
}

TEST(Convert_CompPolynomialToPolesTest, TwoSpansUniformContinuity)
{
  // Two linear polynomials with C0 continuity
  const int aNumCurves = 2;
  const int aContinuity = 0;
  const int aDim = 1;
  const int aMaxDeg = 1;

  occ::handle<NCollection_HArray1<int>> aNumCoeff = new NCollection_HArray1<int>(1, 2);
  aNumCoeff->SetValue(1, 2); // linear = 2 coefficients
  aNumCoeff->SetValue(2, 2);

  // Coefficients: [curve1: c0, c1] [curve2: c0, c1]
  occ::handle<NCollection_HArray1<double>> aCoeffs = new NCollection_HArray1<double>(1, 4);
  aCoeffs->SetValue(1, 0.0);
  aCoeffs->SetValue(2, 1.0);
  aCoeffs->SetValue(3, 1.0);
  aCoeffs->SetValue(4, -1.0);

  occ::handle<NCollection_HArray2<double>> aPolyIntervals = new NCollection_HArray2<double>(1, 2, 1, 2);
  aPolyIntervals->SetValue(1, 1, 0.0);
  aPolyIntervals->SetValue(1, 2, 1.0);
  aPolyIntervals->SetValue(2, 1, 0.0);
  aPolyIntervals->SetValue(2, 2, 1.0);

  occ::handle<NCollection_HArray1<double>> aTrueIntervals = new NCollection_HArray1<double>(1, 3);
  aTrueIntervals->SetValue(1, 0.0);
  aTrueIntervals->SetValue(2, 0.5);
  aTrueIntervals->SetValue(3, 1.0);

  Convert_CompPolynomialToPoles aConv(aNumCurves, aContinuity, aDim, aMaxDeg,
                                      aNumCoeff, aCoeffs, aPolyIntervals, aTrueIntervals);

  EXPECT_TRUE(aConv.IsDone());
  EXPECT_EQ(aConv.Degree(), 1);
  EXPECT_EQ(aConv.NbKnots(), 3);
}

TEST(Convert_CompPolynomialToPolesTest, ThreeDimensional)
{
  // 3D linear curve
  const int aDim = 3;
  const int aMaxDeg = 1;
  const int aDeg = 1;

  // Coefficients for 3D: [x0, y0, z0, x1, y1, z1]
  NCollection_Array1<double> aCoeffs(1, 6);
  aCoeffs(1) = 0.0; aCoeffs(2) = 0.0; aCoeffs(3) = 0.0; // constant
  aCoeffs(4) = 1.0; aCoeffs(5) = 2.0; aCoeffs(6) = 3.0; // linear

  NCollection_Array1<double> aPolyIntervals(1, 2);
  aPolyIntervals(1) = 0.0;
  aPolyIntervals(2) = 1.0;

  NCollection_Array1<double> aTrueIntervals(1, 2);
  aTrueIntervals(1) = 0.0;
  aTrueIntervals(2) = 1.0;

  Convert_CompPolynomialToPoles aConv(aDim, aMaxDeg, aDeg, aCoeffs, aPolyIntervals, aTrueIntervals);

  EXPECT_TRUE(aConv.IsDone());
  EXPECT_EQ(aConv.Degree(), 1);

  const NCollection_Array2<double>& aPoles = aConv.Poles();
  // Poles should be [1..NbPoles][1..3]
  EXPECT_EQ(aPoles.RowLength(), 3);
}
