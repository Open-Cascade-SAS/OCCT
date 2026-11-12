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

#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_List.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
int nbSolids(const TopoDS_Shape& theShape)
{
  int aNb = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    ++aNb;
  }
  return aNb;
}

double volume(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

TopoDS_Shape generalFuse(const NCollection_List<TopoDS_Shape>& theArguments)
{
  BRepAlgoAPI_BuilderAlgo aBuilder;
  aBuilder.SetArguments(theArguments);
  aBuilder.SetRunParallel(false);
  aBuilder.Build();
  EXPECT_TRUE(aBuilder.IsDone());
  EXPECT_FALSE(aBuilder.HasErrors());
  return aBuilder.Shape();
}
} // namespace

// The General Fuse of a solid with many touching solids inside it splits the outer solid with
// the faces of all the inner ones, in both orientations (the faces shared by two inner solids
// bound three regions): the shells of all the regions are built from one block of faces.
TEST(BOPAlgo_ShellSplitterTest, SolidWithManyTouchingSolidsInside)
{
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(BRepPrimAPI_MakeBox(gp_Pnt(0., 0., 0.), 10., 10., 10.).Shape());
  // a slab, with a grid of bricks standing on it and touching each other
  anArguments.Append(BRepPrimAPI_MakeBox(gp_Pnt(1., 1., 1.), 8., 8., 1.).Shape());
  const int aNbBricks = 6;
  for (int i = 0; i < aNbBricks; ++i)
  {
    for (int j = 0; j < aNbBricks; ++j)
    {
      anArguments.Append(
        BRepPrimAPI_MakeBox(gp_Pnt(1. + i, 1. + j, 2.), 1., 1., 1. + 0.5 * ((i + j) % 3)).Shape());
    }
  }

  const TopoDS_Shape aResult = generalFuse(anArguments);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  // the outer region, the slab and the bricks
  EXPECT_EQ(nbSolids(aResult), 1 + 1 + aNbBricks * aNbBricks);
  EXPECT_NEAR(volume(aResult), 1000., 1.e-6);
}

// Same, with seam edges: coaxial cylinders stacked inside a box (each lateral face has its seam
// edge twice, in both orientations).
TEST(BOPAlgo_ShellSplitterTest, SolidWithStackedCylindersInside)
{
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(BRepPrimAPI_MakeBox(gp_Pnt(-5., -5., 0.), 10., 10., 10.).Shape());
  const int aNbCylinders = 4;
  for (int i = 0; i < aNbCylinders; ++i)
  {
    const gp_Ax2 anAxis(gp_Pnt(0., 0., 1. + 2. * i), gp::DZ());
    anArguments.Append(BRepPrimAPI_MakeCylinder(anAxis, 2. + 0.5 * (i % 2), 2.).Shape());
  }

  const TopoDS_Shape aResult = generalFuse(anArguments);
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(nbSolids(aResult), 1 + aNbCylinders);
  EXPECT_NEAR(volume(aResult), 1000., 1.e-6);
}
