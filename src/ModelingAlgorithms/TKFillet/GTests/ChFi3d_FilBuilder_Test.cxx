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

#include <ChFi3d_FilBuilder.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBlend_Line.hxx>
#include <Blend_Point.hxx>
#include <Geom_BezierSurface.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <gtest/gtest.h>

namespace
{
class ChFi3d_TestFilBuilder : public ChFi3d_FilBuilder
{
public:
  ChFi3d_TestFilBuilder();

  using ChFi3d_FilBuilder::SplitSurf;

  TopOpeBRepDS_DataStructure& Data();
};

//==================================================================================================

ChFi3d_TestFilBuilder::ChFi3d_TestFilBuilder()
    : ChFi3d_FilBuilder(BRepPrimAPI_MakeBox(1, 1, 1).Shape())
{
}

//==================================================================================================

TopOpeBRepDS_DataStructure& ChFi3d_TestFilBuilder::Data()
{
  return myDS->ChangeDS();
}
} // namespace

TEST(ChFi3d_FilBuilder, SplitOnlyZeroWidthSections)
{
  for (double aMinimumWidth : {0.0, 5.e-5, 0.002})
  {
    SCOPED_TRACE(aMinimumWidth);
    ChFi3d_TestFilBuilder       aBuilder;
    TopOpeBRepDS_DataStructure& aDS = aBuilder.Data();
    // width(v) = minimum + (v - 1/2)^2: every case has a stationary width.
    NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
    for (int v = 1; v <= 3; ++v)
    {
      aPoles(1, v) = gp_Pnt(0, 0.5 * (v - 1), 0);
      aPoles(2, v) = gp_Pnt(aMinimumWidth + (v == 2 ? -0.25 : 0.25), 0.5 * (v - 1), 0);
    }
    const occ::handle<Geom_Surface>    aSurface = new Geom_BezierSurface(aPoles);
    const occ::handle<ChFiDS_SurfData> aData    = new ChFiDS_SurfData;
    aData->ChangeSurf(aDS.AddSurface(TopOpeBRepDS_Surface(aSurface, 1.e-7)));
    for (int side = 1; side <= 2; ++side)
    {
      ChFiDS_FaceInterference& anInterference = aData->ChangeInterference(side);
      anInterference.SetFirstParameter(0.0);
      anInterference.SetLastParameter(1.0);
      anInterference.SetLineIndex(
        aDS.AddCurve(TopOpeBRepDS_Curve(aSurface->UIso(side - 1.0), 1.e-7)));
    }
    aData->FirstSpineParam(0.0);
    aData->LastSpineParam(1.0);
    const occ::handle<BRepBlend_Line> aLine = new BRepBlend_Line;
    for (int i = 0; i <= 4; ++i)
    {
      const double v = 0.25 * i;
      Blend_Point  aPoint;
      aPoint.SetValue(aSurface->EvalD0(0, v), aSurface->EvalD0(1, v), v, 0, v, 1, v);
      aLine->Append(aPoint);
    }
    NCollection_Sequence<occ::handle<ChFiDS_SurfData>> aSequence;
    aSequence.Append(aData);
    aBuilder.SplitSurf(aSequence, aLine);
    EXPECT_EQ(aSequence.Length(), aMinimumWidth < 1.e-4 ? 2 : 1);
  }
}
