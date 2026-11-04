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

#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <Geom_BSplineSurface.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_List.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <cmath>
#include <sstream>
#include <string>

namespace
{
//! Arguments for a General Fuse whose Edge/Face intersections run on multi-span B-spline faces
//! with edges of different tolerances: a wavy bicubic B-spline sheet of 4x4 spans, crossed by
//! a grid of boxes whose edges and vertices have tolerances from 1e-7 to 5e-4.
NCollection_List<TopoDS_Shape> multiSpanMixedToleranceArguments(const gp_Vec& theShift)
{
  const int                  aNbPoles = 7;
  NCollection_Array2<gp_Pnt> aPoles(1, aNbPoles, 1, aNbPoles);
  for (int i = 1; i <= aNbPoles; ++i)
  {
    for (int j = 1; j <= aNbPoles; ++j)
    {
      const double aX = 10. * (i - 1) / (aNbPoles - 1);
      const double aY = 10. * (j - 1) / (aNbPoles - 1);
      aPoles(i, j)    = gp_Pnt(aX, aY, 0.8 * std::sin(1.3 * i) * std::cos(0.9 * j));
    }
  }
  NCollection_Array1<double> aKnots(1, 5);
  NCollection_Array1<int>    aMults(1, 5);
  for (int k = 1; k <= 5; ++k)
  {
    aKnots(k) = 0.25 * (k - 1);
    aMults(k) = (k == 1 || k == 5) ? 4 : 1;
  }
  occ::handle<Geom_BSplineSurface> aSurface =
    new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 3, 3);

  NCollection_List<TopoDS_Shape> anArgs;
  anArgs.Append(BRepBuilderAPI_MakeFace(aSurface, Precision::Confusion()).Face());

  const double aTolerances[] = {1.e-7, 5.e-4, 1.e-6, 1.e-4, 1.e-5};
  BRep_Builder aBuilder;
  int          aBox = 0;
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j, ++aBox)
    {
      const TopoDS_Shape aShape =
        BRepPrimAPI_MakeBox(gp_Pnt(0.6 + 2.3 * i, 0.4 + 2.4 * j, -2.), 1.7, 1.9, 4.).Shape();
      const double aTol = aTolerances[aBox % 5];
      for (TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next())
      {
        aBuilder.UpdateEdge(TopoDS::Edge(anExp.Current()), aTol);
      }
      for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
      {
        aBuilder.UpdateVertex(TopoDS::Vertex(anExp.Current()), aTol);
      }
      anArgs.Append(aShape);
    }
  }

  if (theShift.SquareMagnitude() > 0.)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(theShift);
    NCollection_List<TopoDS_Shape> aMoved;
    for (const TopoDS_Shape& anArg : anArgs)
    {
      aMoved.Append(BRepBuilderAPI_Transform(anArg, aTrsf, true).Shape());
    }
    return aMoved;
  }
  return anArgs;
}

//! Performs the General Fuse of the arguments and returns the result in the BREP format
std::string generalFuse(const NCollection_List<TopoDS_Shape>& theArgs, const bool theRunParallel)
{
  BRepAlgoAPI_BuilderAlgo aBuilder;
  aBuilder.SetArguments(theArgs);
  aBuilder.SetRunParallel(theRunParallel);
  aBuilder.Build();
  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.HasErrors());
  EXPECT_TRUE(BRepCheck_Analyzer(aBuilder.Shape()).IsValid());
  std::ostringstream aStream;
  BRepTools::Write(aBuilder.Shape(), aStream);
  return aStream.str();
}

bool hasThreads()
{
  return OSD_Parallel::NbLogicalProcessors() > 1
         && (!OSD_Parallel::ToUseOcctThreads() || OSD_ThreadPool::DefaultPool()->NbThreads() > 1);
}
} // namespace

//=================================================================================================

// The parallel General Fuse groups the intersections by face and releases the data cached for
// each face once it is done. Multi-span B-spline faces intersected by edges of mixed tolerances
// are the case where the data cached for a face depends on the intersection that built it.
// On this model the parallel result equals the serial one, in every run.
TEST(BOPAlgo_BuilderParallelTest, MultiSpanFacesMixedTolerances_ParallelEqualsSerial)
{
  if (!hasThreads())
  {
    GTEST_SKIP() << "At least two threads are needed to run in parallel.";
  }
  const NCollection_List<TopoDS_Shape> anArgs  = multiSpanMixedToleranceArguments(gp_Vec());
  const std::string                    aSerial = generalFuse(anArgs, false);
  ASSERT_FALSE(aSerial.empty());
  EXPECT_EQ(generalFuse(anArgs, true), aSerial);
  EXPECT_EQ(generalFuse(anArgs, true), aSerial);
}

//=================================================================================================

// Far from the origin, the Edge/Face intersections are computed on moved copies of the shapes
// (see BOPAlgo_Tools::TrsfToPoint()). The data cached for a moved face cannot be used again,
// so it is released at once instead of being kept in the context until its destruction.
TEST(BOPAlgo_BuilderParallelTest, FarFromOrigin_MovedFacesNotCached)
{
  const NCollection_List<TopoDS_Shape> anArgs =
    multiSpanMixedToleranceArguments(gp_Vec(1.e7, -2.e7, 3.e6));
  int aNbFaces = 0;
  for (const TopoDS_Shape& anArg : anArgs)
  {
    for (TopExp_Explorer anExp(anArg, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      ++aNbFaces;
    }
  }

  // Serially, all intersections use the context of the algorithm, which keeps
  // at most one projector per face of the arguments
  BOPAlgo_PaveFiller aPaveFiller;
  aPaveFiller.SetArguments(anArgs);
  aPaveFiller.SetRunParallel(false);
  aPaveFiller.Perform();
  ASSERT_FALSE(aPaveFiller.HasErrors());
  EXPECT_LE(aPaveFiller.Context()->NbPOnSProjectors(), aNbFaces);

  if (hasThreads())
  {
    const std::string aSerial = generalFuse(anArgs, false);
    ASSERT_FALSE(aSerial.empty());
    EXPECT_EQ(generalFuse(anArgs, true), aSerial);
  }
}
