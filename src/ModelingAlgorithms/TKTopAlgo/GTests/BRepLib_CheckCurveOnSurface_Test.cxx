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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <cmath>
#include <limits>

TEST(BRepLib_CheckCurveOnSurfaceTest, BothSeamPCurvesAndReuse)
{
  constexpr double                           aLargeAngle = 0.2;
  constexpr double                           aSmallAngle = 0.1;
  const BRep_Builder                         aBuilder;
  const occ::handle<Geom_CylindricalSurface> aSurface = new Geom_CylindricalSurface(gp_Ax3(), 1.0);
  const occ::handle<Geom_Line> aCurve = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  TopoDS_Face                  aFace;
  aBuilder.MakeFace(aFace, aSurface, Precision::Confusion());
  TopoDS_Edge anOpenEdge;
  aBuilder.MakeEdge(anOpenEdge, aCurve, Precision::Confusion());
  aBuilder.UpdateEdge(anOpenEdge,
                      new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0)),
                      aFace,
                      Precision::Confusion());
  aBuilder.Range(anOpenEdge, 0.0, 1.0);
  for (bool isLargeFirst : {false, true})
  {
    for (bool isParallel : {false, true})
    {
      SCOPED_TRACE(::testing::Message() << isLargeFirst << ' ' << isParallel);
      TopoDS_Edge aSeam;
      aBuilder.MakeEdge(aSeam, aCurve, Precision::Confusion());
      aBuilder.UpdateEdge(
        aSeam,
        new Geom2d_Line(gp_Pnt2d(isLargeFirst ? aLargeAngle : aSmallAngle, 0.0),
                        gp_Dir2d(0.0, 1.0)),
        new Geom2d_Line(gp_Pnt2d(2.0 * M_PI + (isLargeFirst ? aSmallAngle : aLargeAngle), 0.0),
                        gp_Dir2d(0.0, 1.0)),
        aFace,
        Precision::Confusion());
      aBuilder.Range(aSeam, 0.0, 1.0);
      ASSERT_TRUE(BRep_Tool::IsClosed(aSeam, aFace));
      BRepLib_CheckCurveOnSurface aCheck(aSeam, aFace);
      aCheck.SetParallel(isParallel);
      aCheck.Perform();
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_NEAR(aCheck.MaxDistance(), 2.0 * std::sin(0.5 * aLargeAngle), Precision::Confusion());
      EXPECT_GE(aCheck.MaxParameter(), 0.0);
      EXPECT_LE(aCheck.MaxParameter(), 1.0);

      aCheck.Init(anOpenEdge, aFace);
      aCheck.Perform();
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_LE(aCheck.MaxDistance(), Precision::Confusion());

      aBuilder.UpdateEdge(aSeam,
                          new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0)),
                          new Geom2d_Line(gp_Pnt2d(std::numeric_limits<double>::quiet_NaN(), 0.0),
                                          gp_Dir2d(0.0, 1.0)),
                          aFace,
                          Precision::Confusion());
      aCheck.Init(aSeam, aFace);
      aCheck.Perform();
      EXPECT_FALSE(aCheck.IsDone());
      EXPECT_EQ(aCheck.ErrorStatus(), 3);

      aCheck.Init(anOpenEdge, aFace);
      aCheck.Perform();
      ASSERT_TRUE(aCheck.IsDone());
      EXPECT_LE(aCheck.MaxDistance(), Precision::Confusion());
    }
  }
}
