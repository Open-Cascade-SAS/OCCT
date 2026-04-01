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

#include <IntSurf_LineOn2S.hxx>

#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

static IntSurf_PntOn2S buildPoint(const gp_Pnt& thePoint,
                                  const double  theU1,
                                  const double  theV1,
                                  const double  theU2,
                                  const double  theV2)
{
  IntSurf_PntOn2S aPnt;
  aPnt.SetValue(thePoint, theU1, theV1, theU2, theV2);
  return aPnt;
}

TEST(IntSurf_LineOn2S_Test, EmptyLineBoxesAreNotOut)
{
  occ::handle<IntSurf_LineOn2S> aLine = new IntSurf_LineOn2S();
  EXPECT_FALSE(aLine->IsOutBox(gp_Pnt(10.0, 10.0, 10.0)));
  EXPECT_FALSE(aLine->IsOutSurf1Box(gp_Pnt2d(10.0, 10.0)));
  EXPECT_FALSE(aLine->IsOutSurf2Box(gp_Pnt2d(10.0, 10.0)));
}

TEST(IntSurf_LineOn2S_Test, PointReplacementInvalidatesCachedBoxes)
{
  occ::handle<IntSurf_LineOn2S> aLine = new IntSurf_LineOn2S();
  aLine->Add(buildPoint(gp_Pnt(0.0, 0.0, 0.0), 0.0, 0.0, 0.0, 0.0));
  aLine->Add(buildPoint(gp_Pnt(1.0, 1.0, 1.0), 1.0, 1.0, 1.0, 1.0));

  EXPECT_TRUE(aLine->IsOutBox(gp_Pnt(100.0, 100.0, 100.0)));
  EXPECT_TRUE(aLine->IsOutSurf1Box(gp_Pnt2d(50.0, 50.0)));

  aLine->SetPoint(2, gp_Pnt(100.0, 100.0, 100.0));
  EXPECT_FALSE(aLine->IsOutBox(gp_Pnt(100.0, 100.0, 100.0)));

  aLine->Value(2, buildPoint(gp_Pnt(100.0, 100.0, 100.0), 50.0, 50.0, 60.0, 60.0));
  EXPECT_FALSE(aLine->IsOutSurf1Box(gp_Pnt2d(50.0, 50.0)));
  EXPECT_FALSE(aLine->IsOutSurf2Box(gp_Pnt2d(60.0, 60.0)));
}
