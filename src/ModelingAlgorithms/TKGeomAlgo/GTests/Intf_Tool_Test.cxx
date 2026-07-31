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

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <Intf_Tool.hxx>
#include <Precision.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>

#include <gtest/gtest.h>

// Regression test for bugs #75/#76: Hypr2dBox must produce valid segments
// with finite begin/end parameters and not overflow fixed-size arrays.
TEST(Intf_Tool, Hypr2dBox_ProducesValidSegments)
{
  gp_Ax2d   anAxis(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  gp_Hypr2d aHypr(anAxis, 2.0, 1.0);
  Bnd_Box2d aDomain;
  aDomain.Update(-5., -5., 5., 5.);

  Bnd_Box2d aResult;
  Intf_Tool aTool;
  aTool.Hypr2dBox(aHypr, aDomain, aResult);

  const int aNbSeg = aTool.NbSegments();
  EXPECT_GE(aNbSeg, 0);
  EXPECT_LE(aNbSeg, 6);

  for (int i = 1; i <= aNbSeg; ++i)
  {
    double aBegin = aTool.BeginParam(i);
    double aEnd   = aTool.EndParam(i);
    EXPECT_TRUE(std::isfinite(aBegin) || aBegin == -Precision::Infinite())
      << "Segment " << i << " begin=" << aBegin;
    EXPECT_TRUE(std::isfinite(aEnd) || aEnd == Precision::Infinite())
      << "Segment " << i << " end=" << aEnd;
    EXPECT_LE(aBegin, aEnd) << "Segment " << i << " begin > end";
  }
}

// Regression test for bugs #75/#76: Parab2dBox must produce valid segments.
TEST(Intf_Tool, Parab2dBox_ProducesValidSegments)
{
  gp_Ax2d    anAxis(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  gp_Parab2d aParab(anAxis, 1.0);
  Bnd_Box2d  aDomain;
  aDomain.Update(-5., -5., 5., 5.);

  Bnd_Box2d aResult;
  Intf_Tool aTool;
  aTool.Parab2dBox(aParab, aDomain, aResult);

  const int aNbSeg = aTool.NbSegments();
  EXPECT_GE(aNbSeg, 0);
  EXPECT_LE(aNbSeg, 6);

  for (int i = 1; i <= aNbSeg; ++i)
  {
    double aBegin = aTool.BeginParam(i);
    double aEnd   = aTool.EndParam(i);
    EXPECT_TRUE(std::isfinite(aBegin) || aBegin == -Precision::Infinite())
      << "Segment " << i << " begin=" << aBegin;
    EXPECT_TRUE(std::isfinite(aEnd) || aEnd == Precision::Infinite())
      << "Segment " << i << " end=" << aEnd;
    EXPECT_LE(aBegin, aEnd) << "Segment " << i << " begin > end";
  }
}

// Regression test for bugs #75/#76: ParabBox (3D) must produce valid segments.
TEST(Intf_Tool, ParabBox_ProducesValidSegments)
{
  gp_Parab aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1.0);
  Bnd_Box  aDomain;
  aDomain.Update(-5., -5., -5., 5., 5., 5.);

  Bnd_Box   aResult;
  Intf_Tool aTool;
  aTool.ParabBox(aParab, aDomain, aResult);

  const int aNbSeg = aTool.NbSegments();
  EXPECT_GE(aNbSeg, 0);
  EXPECT_LE(aNbSeg, 6);

  for (int i = 1; i <= aNbSeg; ++i)
  {
    double aBegin = aTool.BeginParam(i);
    double aEnd   = aTool.EndParam(i);
    EXPECT_LE(aBegin, aEnd) << "Segment " << i << " begin > end";
  }
}

// HyprBox (3D) produces valid segments.
TEST(Intf_Tool, HyprBox_ProducesValidSegments)
{
  gp_Hypr aHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0, 1.0);
  Bnd_Box aDomain;
  aDomain.Update(-5., -5., -5., 5., 5., 5.);

  Bnd_Box   aResult;
  Intf_Tool aTool;
  aTool.HyprBox(aHypr, aDomain, aResult);

  const int aNbSeg = aTool.NbSegments();
  EXPECT_GE(aNbSeg, 0);
  EXPECT_LE(aNbSeg, 6);
}

// Small domain that doesn't intersect the curve should produce zero segments.
TEST(Intf_Tool, Hypr2dBox_NoIntersection_ZeroSegments)
{
  gp_Ax2d   anAxis(gp_Pnt2d(100, 100), gp_Dir2d(1, 0));
  gp_Hypr2d aHypr(anAxis, 0.1, 0.05);
  Bnd_Box2d aDomain;
  aDomain.Update(-1., -1., 1., 1.);

  Bnd_Box2d aResult;
  Intf_Tool aTool;
  aTool.Hypr2dBox(aHypr, aDomain, aResult);

  EXPECT_EQ(aTool.NbSegments(), 0);
}
