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

#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_BSplineSurface.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Surface.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

TEST(HLRBRep_SurfaceTest, SplineSamplingAndEdgeOnProjection)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 2);
  const double               aY[4] = {0.0, 0.25, -0.25, 0.0};
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 2; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) / 3.0, aY[i - 1], j - 1.0);
    }
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aUMultiplicities(1, 2), aVMultiplicities(1, 2);
  aUMultiplicities.Init(4);
  aVMultiplicities.Init(2);
  const occ::handle<Geom_BSplineSurface> aSpline =
    new Geom_BSplineSurface(aPoles, aKnots, aKnots, aUMultiplicities, aVMultiplicities, 3, 1);
  BRepBuilderAPI_MakeFace aBuilder(aSpline, Precision::Confusion());
  ASSERT_TRUE(aBuilder.IsDone());
  HLRAlgo_Projector aProjector(gp_Trsf(), false, 1.0);
  HLRBRep_Surface   aSurface;
  aSurface.Surface(aBuilder.Face());
  aSurface.Projector(&aProjector);

  EXPECT_EQ(aSurface.GetType(), GeomAbs_BSplineSurface);
  EXPECT_EQ(HLRBRep_SurfaceTool::NbSamplesU(&aSurface), aSpline->NbUKnots() * aSpline->UDegree());
  EXPECT_EQ(HLRBRep_SurfaceTool::NbSamplesV(&aSurface), aSpline->NbVKnots() * aSpline->VDegree());
  EXPECT_TRUE(aSurface.IsSide(Precision::Confusion(), Precision::Angular()));
}
