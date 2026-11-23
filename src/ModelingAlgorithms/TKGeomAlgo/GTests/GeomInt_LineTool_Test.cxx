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

#include <Adaptor3d_TopolTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <GeomInt_LineTool.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>
#include <array>
#include <cmath>

TEST(GeomInt_LineToolTest, DecompositionAtPeriodicSurfaceCorner)
{
  const double                           aHeight   = 20.0;
  const occ::handle<GeomAdaptor_Surface> aCylinder = new GeomAdaptor_Surface(
    new Geom_RectangularTrimmedSurface(new Geom_CylindricalSurface(gp_Ax3(), 10.0),
                                       0.0,
                                       2.0 * M_PI,
                                       aHeight,
                                       50.0));
  const occ::handle<GeomAdaptor_Surface> aPlane =
    new GeomAdaptor_Surface(new Geom_RectangularTrimmedSurface(
      new Geom_Plane(gp_Pnt(0.0, 0.0, aHeight), gp_Dir(0.0, 0.0, 1.0)),
      -20.0,
      20.0,
      -20.0,
      20.0));
  for (double aFirstV : {aHeight, std::nextafter(aHeight, 50.0)})
  {
    SCOPED_TRACE(aFirstV);
    const occ::handle<IntSurf_LineOn2S> aPoints = new IntSurf_LineOn2S();
    const std::array<double, 3> aParameters = {2.0 * M_PI - 0.002, 2.0 * M_PI - 0.001, 2.0 * M_PI};
    for (size_t anIndex = 0; anIndex < aParameters.size(); ++anIndex)
    {
      const double    aV     = anIndex == 0 ? aFirstV : aHeight;
      const gp_Pnt    aPoint = aCylinder->Value(aParameters[anIndex], aV);
      IntSurf_PntOn2S aPointOnSurfaces;
      aPointOnSurfaces.SetValue(aPoint, aParameters[anIndex], aV, aPoint.X(), aPoint.Y());
      aPoints->Add(aPointOnSurfaces);
    }
    const occ::handle<IntPatch_WLine> aLine = new IntPatch_WLine(aPoints, false);
    for (int anIndex : {1, 3})
    {
      IntPatch_Point aVertex;
      aVertex.SetValue(aPoints->Value(anIndex));
      aVertex.SetParameter(anIndex);
      aLine->AddVertex(aVertex);
    }
    GeomInt_LineConstructor aConstructor;
    aConstructor.Load(new Adaptor3d_TopolTool(aCylinder),
                      new Adaptor3d_TopolTool(aPlane),
                      aCylinder,
                      aPlane);
    aConstructor.Perform(aLine);
    ASSERT_TRUE(aConstructor.IsDone());
    ASSERT_EQ(aConstructor.NbParts(), 1);
    NCollection_Sequence<occ::handle<IntPatch_Line>> aParts;
    ASSERT_TRUE(GeomInt_LineTool::DecompositionOfWLine(aLine,
                                                       aCylinder,
                                                       aPlane,
                                                       Precision::Confusion(),
                                                       aConstructor,
                                                       aParts));
    ASSERT_EQ(aParts.Length(), 1);
    const auto aResult = occ::down_cast<IntPatch_WLine>(aParts.First());
    ASSERT_FALSE(aResult.IsNull());
    ASSERT_EQ(aResult->NbPnts(), 3);
    for (int anIndex = 1; anIndex <= 3; ++anIndex)
    {
      EXPECT_LE(aResult->Point(anIndex).Value().Distance(aPoints->Value(anIndex).Value()),
                Precision::Confusion());
    }
  }
}
