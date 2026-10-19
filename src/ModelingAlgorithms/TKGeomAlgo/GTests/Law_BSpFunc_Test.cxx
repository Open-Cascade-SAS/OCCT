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

#include <Law_BSpFunc.hxx>
#include <Law_BSpline.hxx>
#include <Law_Function.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(Law_BSplineTest, InputArrayBoundsDoNotChangePoleAndKnotIndices)
{
  for (const int aLower : {-3, 0, 1, 5})
  {
    NCollection_Array1<double> aPoles(aLower, aLower + 2);
    NCollection_Array1<double> aKnots(aLower + 2, aLower + 3);
    NCollection_Array1<int>    aMults(aLower - 2, aLower - 1);
    aPoles.ChangeAt(0) = 1;
    aPoles.ChangeAt(1) = 1;
    aPoles.ChangeAt(2) = 2;
    aKnots.ChangeAt(0) = 0;
    aKnots.ChangeAt(1) = 1;
    aMults.Init(3);
    NCollection_Array1<double> aWeights(aLower - 1, aLower + 1);
    aWeights.ChangeAt(0) = 1;
    aWeights.ChangeAt(1) = 2;
    aWeights.ChangeAt(2) = 1;
    for (const bool isRational : {false, true})
    {
      const occ::handle<Law_BSpline> aCurve =
        isRational ? new Law_BSpline(aPoles, aWeights, aKnots, aMults, 2)
                   : new Law_BSpline(aPoles, aKnots, aMults, 2);
      for (int i = 1; i <= 3; ++i)
      {
        EXPECT_EQ(aCurve->Pole(i), aPoles.At(size_t(i - 1)));
        EXPECT_EQ(aCurve->Weight(i), isRational ? aWeights.At(size_t(i - 1)) : 1);
      }
      EXPECT_EQ(aCurve->Knot(1), 0);
      EXPECT_EQ(aCurve->Knot(2), 1);
      EXPECT_EQ(aCurve->Multiplicity(1), 3);
      EXPECT_EQ(aCurve->Multiplicity(2), 3);
      EXPECT_EQ(aCurve->StartPoint(), 1);
      EXPECT_EQ(aCurve->EndPoint(), 2);
      Law_BSpFunc aLaw(aCurve, 0, 1);
      double      aValue, aD1, aD2;
      aLaw.D2(.5, aValue, aD1, aD2);
      EXPECT_DOUBLE_EQ(aValue, isRational ? 7. / 6 : 1.25);
      EXPECT_DOUBLE_EQ(aD1, isRational ? 2. / 3 : 1);
      EXPECT_NEAR(aD2, isRational ? 16. / 9 : 2, 1e-15);
      // A new physical chart must keep the same one-based knot storage.
      aKnots.ChangeAt(0) = 2;
      aKnots.ChangeAt(1) = 4;
      aCurve->SetKnots(aKnots);
      EXPECT_EQ(aCurve->Knot(1), 2);
      EXPECT_EQ(aCurve->Knot(2), 4);
      EXPECT_DOUBLE_EQ(aCurve->Value(3), isRational ? 7. / 6 : 1.25);
      aKnots.ChangeAt(0) = 0;
      aKnots.ChangeAt(1) = 1;
    }
  }
  // A common weight scale must not turn unequal weights into a polynomial law.
  NCollection_Array1<double> aPoles(1, 3), aWeights(1, 3), aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aPoles.ChangeAt(0) = 1;
  aPoles.ChangeAt(1) = 1;
  aPoles.ChangeAt(2) = 2;
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1;
  aMults.Init(3);
  for (const double aScale : {1.0, 1.e-308})
  {
    aWeights.ChangeAt(0) = 3 * aScale;
    aWeights.ChangeAt(1) = 4.5 * aScale;
    aWeights.ChangeAt(2) = 3 * aScale;
    const Law_BSpline aCurve(aPoles, aWeights, aKnots, aMults, 2);
    EXPECT_TRUE(aCurve.IsRational());
    EXPECT_NEAR(aCurve.Value(0.5), 1.2, 1.e-14);
  }
}
