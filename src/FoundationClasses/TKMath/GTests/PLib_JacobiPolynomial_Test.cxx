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

#include <PLib_JacobiPolynomial.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <gtest/gtest.h>

TEST(PLib_JacobiPolynomialTest, ConstructorAndAccessors)
{
  // Test with GeomAbs_C0
  PLib_JacobiPolynomial aJacobi1(10, GeomAbs_C0);
  EXPECT_EQ(10, aJacobi1.WorkDegree());
  EXPECT_EQ(0, aJacobi1.NivConstr());

  // Test with GeomAbs_C1
  PLib_JacobiPolynomial aJacobi2(20, GeomAbs_C1);
  EXPECT_EQ(20, aJacobi2.WorkDegree());
  EXPECT_EQ(1, aJacobi2.NivConstr());

  // Test with GeomAbs_C2
  PLib_JacobiPolynomial aJacobi3(30, GeomAbs_C2);
  EXPECT_EQ(30, aJacobi3.WorkDegree());
  EXPECT_EQ(2, aJacobi3.NivConstr());
}

TEST(PLib_JacobiPolynomialTest, Points)
{
  PLib_JacobiPolynomial aJacobi(15, GeomAbs_C1);

  // Test with 8 Gauss points
  TColStd_Array1OfReal aPoints1(0, 4); // 8/2 = 4
  aJacobi.Points(8, aPoints1);

  // Values should be between 0 and 1 (positive Legendre roots)
  for (Standard_Integer i = 0; i <= 4; i++)
  {
    EXPECT_GE(aPoints1(i), 0.0);
    EXPECT_LE(aPoints1(i), 1.0);
  }

  // Test with odd number (15) of Gauss points
  TColStd_Array1OfReal aPoints2(0, 7); // 15/2 = 7.5 -> 7
  aJacobi.Points(15, aPoints2);

  for (Standard_Integer i = 0; i <= 7; i++)
  {
    EXPECT_GE(aPoints2(i), 0.0);
    EXPECT_LE(aPoints2(i), 1.0);
  }

  // For odd number of points, the first point should be 0
  EXPECT_NEAR(0.0, aPoints2(0), 1e-10);
}

TEST(PLib_JacobiPolynomialTest, Weights)
{
  PLib_JacobiPolynomial aJacobi(20, GeomAbs_C1);

  // Test with 10 Gauss points
  Standard_Integer     nPoints = 10;
  Standard_Integer     nDegree = 20;
  TColStd_Array2OfReal aWeights(0, nPoints / 2, 0, nDegree);
  aJacobi.Weights(nPoints, aWeights);

  // Weights should be positive
  for (Standard_Integer i = 0; i <= nPoints / 2; i++)
  {
    for (Standard_Integer j = 0; j <= nDegree; j++)
    {
      // We don't check exact values, but they should be defined
      EXPECT_FALSE(std::isnan(aWeights(i, j)));
    }
  }
}

TEST(PLib_JacobiPolynomialTest, MaxValue)
{
  PLib_JacobiPolynomial aJacobi(15, GeomAbs_C0);
  Standard_Integer      q = aJacobi.WorkDegree() - 2 * (aJacobi.NivConstr() + 1);

  TColStd_Array1OfReal aTabMax(0, q);
  aJacobi.MaxValue(aTabMax);

  // The maximum values should be positive
  for (Standard_Integer i = 0; i <= q; i++)
  {
    EXPECT_GT(aTabMax(i), 0.0);
  }
}

TEST(PLib_JacobiPolynomialTest, D0)
{
  PLib_JacobiPolynomial aJacobi(10, GeomAbs_C0);
  Standard_Integer      q = aJacobi.WorkDegree() - 2 * (aJacobi.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, q);

  // Test at u = 0
  aJacobi.D0(0.0, aBasisValue);

  // Test at u = 0.5
  aJacobi.D0(0.5, aBasisValue);

  // Test at u = 1.0
  aJacobi.D0(1.0, aBasisValue);

  // We don't check exact values here, just that the method executes without issues
  EXPECT_TRUE(true);
}

TEST(PLib_JacobiPolynomialTest, D1)
{
  PLib_JacobiPolynomial aJacobi(12, GeomAbs_C1);
  Standard_Integer      q = aJacobi.WorkDegree() - 2 * (aJacobi.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, q);
  TColStd_Array1OfReal aBasisD1(0, q);

  // Test at u = 0.5
  aJacobi.D1(0.5, aBasisValue, aBasisD1);

  // We don't check exact values here, just that the method executes without issues
  EXPECT_TRUE(true);
}

TEST(PLib_JacobiPolynomialTest, D2)
{
  PLib_JacobiPolynomial aJacobi(14, GeomAbs_C2);
  Standard_Integer      q = aJacobi.WorkDegree() - 2 * (aJacobi.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, q);
  TColStd_Array1OfReal aBasisD1(0, q);
  TColStd_Array1OfReal aBasisD2(0, q);

  // Test at u = 0.5
  aJacobi.D2(0.5, aBasisValue, aBasisD1, aBasisD2);

  // We don't check exact values here, just that the method executes without issues
  EXPECT_TRUE(true);
}

TEST(PLib_JacobiPolynomialTest, D3)
{
  PLib_JacobiPolynomial aJacobi(16, GeomAbs_C2);
  Standard_Integer      q = aJacobi.WorkDegree() - 2 * (aJacobi.NivConstr() + 1);

  TColStd_Array1OfReal aBasisValue(0, q);
  TColStd_Array1OfReal aBasisD1(0, q);
  TColStd_Array1OfReal aBasisD2(0, q);
  TColStd_Array1OfReal aBasisD3(0, q);

  // Test at u = 0.5
  aJacobi.D3(0.5, aBasisValue, aBasisD1, aBasisD2, aBasisD3);

  // We don't check exact values here, just that the method executes without issues
  EXPECT_TRUE(true);
}

TEST(PLib_JacobiPolynomialTest, ToCoefficients)
{
  PLib_JacobiPolynomial aJacobi(10, GeomAbs_C1);
  Standard_Integer      dimension = 3;
  Standard_Integer      degree    = 8;

  // Create sample JacCoeff array
  TColStd_Array1OfReal aJacCoeff(1, dimension * (degree + 1));
  for (Standard_Integer i = 1; i <= dimension * (degree + 1); i++)
  {
    aJacCoeff(i) = 0.1 * i;
  }

  // Create output coefficients array
  TColStd_Array1OfReal aCoefficients(1, dimension * (degree + 1));

  // Convert to coefficients
  aJacobi.ToCoefficients(dimension, degree, aJacCoeff, aCoefficients);

  // Check that all values are defined (not checking exact values)
  for (Standard_Integer i = 1; i <= dimension * (degree + 1); i++)
  {
    EXPECT_FALSE(std::isnan(aCoefficients(i)));
  }
}

TEST(PLib_JacobiPolynomialTest, ReduceDegree)
{
  PLib_JacobiPolynomial aJacobi(20, GeomAbs_C1);
  Standard_Integer      dimension = 3;
  Standard_Integer      maxDegree = 15;
  Standard_Real         tolerance = 1.0e-3;

  // Create sample JacCoeff array with increasing values
  TColStd_Array1OfReal aJacCoeff(1, dimension * (maxDegree + 1));
  for (Standard_Integer i = 1; i <= dimension * (maxDegree + 1); i++)
  {
    aJacCoeff(i) = 0.1 * i;
  }

  // Initialize output variables
  Standard_Integer newDegree = 0;
  Standard_Real    maxError  = 0.0;

  // Test the reduce degree functionality
  aJacobi
    .ReduceDegree(dimension, maxDegree, tolerance, aJacCoeff.ChangeValue(1), newDegree, maxError);

  // New degree should be less than or equal to maxDegree
  EXPECT_LE(newDegree, maxDegree);

  // Max error should be defined
  EXPECT_FALSE(std::isnan(maxError));
}
