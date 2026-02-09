// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Convert_GridPolynomialToPoles.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray1.hxx>

TEST(Convert_GridPolynomialToPolesTest, SinglePlanarPatch)
{
  // A single bilinear patch: z = 0, x = u, y = v
  // MaxUDegree = 1, MaxVDegree = 1
  // Coefficients as "C array" [MaxUDegree+1][MaxVDegree+1][3]
  // For P(u,v) = (u, v, 0): coefficients in polynomial form
  const int aMaxUDeg = 1;
  const int aMaxVDeg = 1;

  occ::handle<NCollection_HArray1<int>> aNumCoeff = new NCollection_HArray1<int>(1, 2);
  aNumCoeff->SetValue(1, 2); // U degree + 1
  aNumCoeff->SetValue(2, 2); // V degree + 1

  // Coefficients [2][2][3] = 12 values
  // Layout: for u_i, v_j: coeff(u_i, v_j) = {x, y, z}
  // P(u,v) = sum c_{ij} * u^i * v^j
  // c_{00} = (0,0,0), c_{10} = (1,0,0), c_{01} = (0,1,0), c_{11} = (0,0,0)
  occ::handle<NCollection_HArray1<double>> aCoeffs = new NCollection_HArray1<double>(1, 12);
  // c_{00}: x=0,y=0,z=0
  aCoeffs->SetValue(1, 0.0);
  aCoeffs->SetValue(2, 0.0);
  aCoeffs->SetValue(3, 0.0);
  // c_{01}: x=0,y=1,z=0
  aCoeffs->SetValue(4, 0.0);
  aCoeffs->SetValue(5, 1.0);
  aCoeffs->SetValue(6, 0.0);
  // c_{10}: x=1,y=0,z=0
  aCoeffs->SetValue(7, 1.0);
  aCoeffs->SetValue(8, 0.0);
  aCoeffs->SetValue(9, 0.0);
  // c_{11}: x=0,y=0,z=0
  aCoeffs->SetValue(10, 0.0);
  aCoeffs->SetValue(11, 0.0);
  aCoeffs->SetValue(12, 0.0);

  occ::handle<NCollection_HArray1<double>> aPolyU = new NCollection_HArray1<double>(1, 2);
  aPolyU->SetValue(1, 0.0);
  aPolyU->SetValue(2, 1.0);

  occ::handle<NCollection_HArray1<double>> aPolyV = new NCollection_HArray1<double>(1, 2);
  aPolyV->SetValue(1, 0.0);
  aPolyV->SetValue(2, 1.0);

  Convert_GridPolynomialToPoles aConv(aMaxUDeg, aMaxVDeg, aNumCoeff, aCoeffs, aPolyU, aPolyV);

  EXPECT_TRUE(aConv.IsDone());
  EXPECT_GT(aConv.NbUPoles(), 0);
  EXPECT_GT(aConv.NbVPoles(), 0);
  EXPECT_EQ(aConv.UDegree(), 1);
  EXPECT_EQ(aConv.VDegree(), 1);
  EXPECT_GT(aConv.NbUKnots(), 0);
  EXPECT_GT(aConv.NbVKnots(), 0);

  // Check poles are accessible
  const NCollection_Array2<gp_Pnt>& aPoles = aConv.Poles();
  EXPECT_GT(aPoles.Size(), 0);
}

TEST(Convert_GridPolynomialToPolesTest, QueryMethods)
{
  const int aMaxUDeg = 1;
  const int aMaxVDeg = 1;

  occ::handle<NCollection_HArray1<int>> aNumCoeff = new NCollection_HArray1<int>(1, 2);
  aNumCoeff->SetValue(1, 2);
  aNumCoeff->SetValue(2, 2);

  occ::handle<NCollection_HArray1<double>> aCoeffs = new NCollection_HArray1<double>(1, 12);
  for (int i = 1; i <= 12; ++i)
  {
    aCoeffs->SetValue(i, 0.0);
  }
  // Just set x = u coefficient
  aCoeffs->SetValue(7, 1.0);
  // y = v coefficient
  aCoeffs->SetValue(5, 1.0);

  occ::handle<NCollection_HArray1<double>> aPolyU = new NCollection_HArray1<double>(1, 2);
  aPolyU->SetValue(1, 0.0);
  aPolyU->SetValue(2, 1.0);

  occ::handle<NCollection_HArray1<double>> aPolyV = new NCollection_HArray1<double>(1, 2);
  aPolyV->SetValue(1, 0.0);
  aPolyV->SetValue(2, 1.0);

  Convert_GridPolynomialToPoles aConv(aMaxUDeg, aMaxVDeg, aNumCoeff, aCoeffs, aPolyU, aPolyV);

  ASSERT_TRUE(aConv.IsDone());

  // Verify knot data is accessible
  const NCollection_Array1<double>& aUKnots = aConv.UKnots();
  const NCollection_Array1<double>& aVKnots = aConv.VKnots();
  const NCollection_Array1<int>&    aUMults = aConv.UMultiplicities();
  const NCollection_Array1<int>&    aVMults = aConv.VMultiplicities();

  EXPECT_GT(aUKnots.Size(), 0);
  EXPECT_GT(aVKnots.Size(), 0);
  EXPECT_GT(aUMults.Size(), 0);
  EXPECT_GT(aVMults.Size(), 0);

  EXPECT_EQ(aUKnots.Length(), aConv.NbUKnots());
  EXPECT_EQ(aVKnots.Length(), aConv.NbVKnots());
}
