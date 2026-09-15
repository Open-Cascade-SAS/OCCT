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

#include <BRepFeat_MakeCylindricalHole.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Builder.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

// Two 50x50x20 plates stacked on the Z axis: A at z in [-10, 10], B at z in [-50, -30].
TopoDS_Shape twoPlateStack()
{
  TopoDS_Shape    aPlateA = BRepPrimAPI_MakeBox(gp_Pnt(-25, -25, -10), 50, 50, 20).Shape();
  TopoDS_Shape    aPlateB = BRepPrimAPI_MakeBox(gp_Pnt(-25, -25, -50), 50, 50, 20).Shape();
  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aPlateA);
  aBuilder.Add(aCompound, aPlateB);
  return aCompound;
}
} // namespace

// Regression test: PartsOfTool() explores myShape, which only holds the tool split by the object
// after a BOPAlgo_COMMON pass. The four modes that select which part to keep drove a CUT instead,
// so on a stock whose cut result has two solids, the selection loop registered pieces of the CUT
// RESULT as kept tool parts, and PerformResult() built with an empty keep set: no material
// removed, status NoError throughout. A single-solid stock never reached the divergence, which is
// why it was never caught.
TEST(BRepFeat_MakeCylindricalHoleTest, PerformUntilEnd_TwoPlateStack_RemovesBothBores)
{
  const TopoDS_Shape aStock = twoPlateStack();
  const double       aV0    = volumeOf(aStock);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 15), gp_Dir(0, 0, -1));

  BRepFeat_MakeCylindricalHole aHole;
  aHole.Init(aStock, anAxis);
  aHole.PerformUntilEnd(5.0);
  ASSERT_EQ(aHole.Status(), BRepFeat_NoError);
  aHole.Build();
  ASSERT_FALSE(aHole.Shape().IsNull());

  const double aRemoved = aV0 - volumeOf(aHole.Shape());
  // Both plates are 20 thick; each bore removes pi * 5^2 * 20 = 1570.7963.
  EXPECT_NEAR(aRemoved, 2 * M_PI * 25.0 * 20.0, 1.e-3);
}

TEST(BRepFeat_MakeCylindricalHoleTest, PerformBlind_TwoPlateStack_RemovesPartialBore)
{
  const TopoDS_Shape aStock = twoPlateStack();
  const double       aV0    = volumeOf(aStock);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 15), gp_Dir(0, 0, -1));

  BRepFeat_MakeCylindricalHole aHole;
  aHole.Init(aStock, anAxis);
  // The axis origin sits 5 units above plate A's entry face (axis parameter 5); a blind depth of
  // 20 reaches axis parameter 20, boring 15 of plate A's 20 units of thickness.
  aHole.PerformBlind(5.0, 20.0);
  ASSERT_EQ(aHole.Status(), BRepFeat_NoError);
  aHole.Build();
  ASSERT_FALSE(aHole.Shape().IsNull());

  const double aRemoved = aV0 - volumeOf(aHole.Shape());
  EXPECT_NEAR(aRemoved, M_PI * 25.0 * 15.0, 1.e-3);
}

TEST(BRepFeat_MakeCylindricalHoleTest, PerformRanged_TwoPlateStack_RemovesBothBores)
{
  const TopoDS_Shape aStock = twoPlateStack();
  const double       aV0    = volumeOf(aStock);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 15), gp_Dir(0, 0, -1));

  BRepFeat_MakeCylindricalHole aHole;
  aHole.Init(aStock, anAxis);
  aHole.Perform(5.0, 0.0, 70.0);
  ASSERT_EQ(aHole.Status(), BRepFeat_NoError);
  aHole.Build();
  ASSERT_FALSE(aHole.Shape().IsNull());

  const double aRemoved = aV0 - volumeOf(aHole.Shape());
  EXPECT_NEAR(aRemoved, 2 * M_PI * 25.0 * 20.0, 1.e-3);
}

// Perform(Radius) (the infinite-cylinder through-all mode) selects no parts and was already
// unaffected; kept as a control against the same stock.
TEST(BRepFeat_MakeCylindricalHoleTest, PerformThroughAll_TwoPlateStack_RemovesBothBores)
{
  const TopoDS_Shape aStock = twoPlateStack();
  const double       aV0    = volumeOf(aStock);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 15), gp_Dir(0, 0, -1));

  BRepFeat_MakeCylindricalHole aHole;
  aHole.Init(aStock, anAxis);
  aHole.Perform(5.0);
  ASSERT_EQ(aHole.Status(), BRepFeat_NoError);
  aHole.Build();
  ASSERT_FALSE(aHole.Shape().IsNull());

  const double aRemoved = aV0 - volumeOf(aHole.Shape());
  EXPECT_NEAR(aRemoved, 2 * M_PI * 25.0 * 20.0, 1.e-3);
}

// A single plate never reaches the divergence (the cut result stays one solid); unaffected by
// this fix, byte-identical before and after.
TEST(BRepFeat_MakeCylindricalHoleTest, PerformUntilEnd_SinglePlate_Unaffected)
{
  const TopoDS_Shape aStock = BRepPrimAPI_MakeBox(gp_Pnt(-25, -25, -10), 50, 50, 20).Shape();
  const double       aV0    = volumeOf(aStock);
  const gp_Ax1       anAxis(gp_Pnt(0, 0, 15), gp_Dir(0, 0, -1));

  BRepFeat_MakeCylindricalHole aHole;
  aHole.Init(aStock, anAxis);
  aHole.PerformUntilEnd(5.0);
  ASSERT_EQ(aHole.Status(), BRepFeat_NoError);
  aHole.Build();
  ASSERT_FALSE(aHole.Shape().IsNull());

  const double aRemoved = aV0 - volumeOf(aHole.Shape());
  EXPECT_NEAR(aRemoved, M_PI * 25.0 * 20.0, 1.e-3);
}
