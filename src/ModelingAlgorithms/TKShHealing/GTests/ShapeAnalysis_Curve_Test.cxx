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

#include <ShapeAnalysis_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <gtest/gtest.h>

TEST(ShapeAnalysis_CurveTest, SeamDirectionRoundoff)
{
  enum class Axis
  {
    U,
    V
  };
  enum class Form
  {
    Line,
    Trimmed
  };

  ShapeAnalysis_Curve anAnalyzer;
  for (const Axis anAxis : {Axis::U, Axis::V})
  {
    for (const double aSign : {-1.0, 1.0})
    {
      for (const double aNoise : {0.0, -1.e-14, 1.e-14})
      {
        for (const Form aForm : {Form::Line, Form::Trimmed})
        {
          SCOPED_TRACE(testing::Message()
                       << "axis=" << static_cast<int>(anAxis) << ", sign=" << aSign
                       << ", noise=" << aNoise << ", form=" << static_cast<int>(aForm));
          const gp_Dir2d aDirection =
            anAxis == Axis::U ? gp_Dir2d(aSign, aNoise) : gp_Dir2d(aNoise, aSign);
          const gp_Pnt2d aLocation =
            anAxis == Axis::U ? gp_Pnt2d(0.0, 2.0 * M_PI) : gp_Pnt2d(2.0 * M_PI, 0.0);
          occ::handle<Geom2d_Curve> aFirst  = new Geom2d_Line(aLocation, aDirection);
          occ::handle<Geom2d_Curve> aSecond = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), aDirection);
          if (aForm == Form::Trimmed)
          {
            aFirst  = new Geom2d_TrimmedCurve(aFirst, -1.0, 1.0);
            aSecond = new Geom2d_TrimmedCurve(aSecond, -1.0, 1.0);
          }

          const int anExpected = (anAxis == Axis::V) == (aSign > 0.0) ? 1 : 2;
          EXPECT_EQ(anAnalyzer.SelectForwardSeam(aFirst, aSecond), anExpected);
          EXPECT_EQ(anAnalyzer.SelectForwardSeam(aSecond, aFirst), 3 - anExpected);
        }
      }
    }
  }
}

TEST(ShapeAnalysis_CurveTest, ObliqueSeams)
{
  ShapeAnalysis_Curve anAnalyzer;
  const double        aPeriod = 2.0 * M_PI;

  const gp_Dir2d                 aUDir(2.0, 1.0);
  const occ::handle<Geom2d_Line> aU1 = new Geom2d_Line(gp_Pnt2d(aPeriod, 0.0), aUDir);
  const occ::handle<Geom2d_Line> aU2 = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), aUDir);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aU1, aU2), 1);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aU2, aU1), 2);

  const gp_Dir2d                 aVDir(1.0, 2.0);
  const occ::handle<Geom2d_Line> aV1 = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), aVDir);
  const occ::handle<Geom2d_Line> aV2 = new Geom2d_Line(gp_Pnt2d(0.0, aPeriod), aVDir);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aV1, aV2), 1);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aV2, aV1), 2);

  const occ::handle<Geom2d_Line> aV3 = new Geom2d_Line(gp_Pnt2d(1.0, 2.0), aVDir);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aV1, aV3), 0);
  EXPECT_EQ(anAnalyzer.SelectForwardSeam(aV3, aV1), 0);
}
