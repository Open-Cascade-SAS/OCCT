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

#include <AdvApp2Var_Context.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>

TEST(AdvApp2Var_ContextTest, Tolerances_AreAggregatedAndRescaledForBorderConstraints)
{
  const occ::handle<NCollection_HArray1<double>> aTol1D = new NCollection_HArray1<double>(1, 1);
  const occ::handle<NCollection_HArray1<double>> aTol2D = new NCollection_HArray1<double>(1, 1);
  const occ::handle<NCollection_HArray1<double>> aTol3D = new NCollection_HArray1<double>(1, 1);
  aTol1D->SetValue(1, 6.0);
  aTol2D->SetValue(1, 12.0);
  aTol3D->SetValue(1, 18.0);

  const occ::handle<NCollection_HArray2<double>> aTof1D = new NCollection_HArray2<double>(1, 1, 1, 4);
  const occ::handle<NCollection_HArray2<double>> aTof2D = new NCollection_HArray2<double>(1, 1, 1, 4);
  const occ::handle<NCollection_HArray2<double>> aTof3D = new NCollection_HArray2<double>(1, 1, 1, 4);
  aTof1D->Init(100.0);
  aTof2D->Init(100.0);
  aTof3D->Init(100.0);

  const AdvApp2Var_Context aContext(0,
                                    0,
                                    0,
                                    2,
                                    2,
                                    0,
                                    1,
                                    1,
                                    1,
                                    aTol1D,
                                    aTol2D,
                                    aTol3D,
                                    aTof1D,
                                    aTof2D,
                                    aTof3D);

  EXPECT_EQ(aContext.TotalNumberSSP(), 3);
  EXPECT_EQ(aContext.TotalDimension(), 6);

  const occ::handle<NCollection_HArray1<double>> anITol = aContext.IToler();
  ASSERT_FALSE(anITol.IsNull());
  EXPECT_DOUBLE_EQ(anITol->Value(1), 3.0);
  EXPECT_DOUBLE_EQ(anITol->Value(2), 6.0);
  EXPECT_DOUBLE_EQ(anITol->Value(3), 9.0);

  const occ::handle<NCollection_HArray2<double>> aFTol = aContext.FToler();
  const occ::handle<NCollection_HArray2<double>> aCTol = aContext.CToler();
  ASSERT_FALSE(aFTol.IsNull());
  ASSERT_FALSE(aCTol.IsNull());

  for (int aTolIndex = 1; aTolIndex <= 4; ++aTolIndex)
  {
    EXPECT_DOUBLE_EQ(aFTol->Value(1, aTolIndex), 1.0);
    EXPECT_DOUBLE_EQ(aFTol->Value(2, aTolIndex), 2.0);
    EXPECT_DOUBLE_EQ(aFTol->Value(3, aTolIndex), 3.0);

    EXPECT_DOUBLE_EQ(aCTol->Value(1, aTolIndex), 1.0);
    EXPECT_DOUBLE_EQ(aCTol->Value(2, aTolIndex), 2.0);
    EXPECT_DOUBLE_EQ(aCTol->Value(3, aTolIndex), 3.0);
  }
}
