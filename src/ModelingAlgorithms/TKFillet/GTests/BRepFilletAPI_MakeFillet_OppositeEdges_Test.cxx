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

// Fillets on the two opposite edges of a face. Related reports:
// - https://github.com/Open-Cascade-SAS/OCCT/issues/1568
// - https://github.com/Open-Cascade-SAS/OCCT/issues/1177

#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <thread>

namespace
{
//! Returns the edge of the shape whose middle is at the given point.
TopoDS_Edge findEdgeAt(const TopoDS_Shape& theShape, const gp_Pnt& theMiddle)
{
  for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    BRepAdaptor_Curve  aCurve(anEdge);
    const gp_Pnt aMiddle = aCurve.Value(0.5 * (aCurve.FirstParameter() + aCurve.LastParameter()));
    if (aMiddle.Distance(theMiddle) < 1.e-7)
    {
      return anEdge;
    }
  }
  return TopoDS_Edge();
}

//! Volume of the shape.
double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}
} // namespace

// Two fillets on the opposite edges of a 4 wide face whose radii add up to more than the width:
// their arcs cross. The builder must not report a solid that is not one (the volume was more
// than the box's) and, when it does report one, it must be valid.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1568_OverlappingOppositeFilletsAreNotAnInvalidSolid)
{
  const TopoDS_Shape aBox         = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  const double       aBoxVol      = volumeOf(aBox);
  const double       aRadii[9][2] = {{2.001, 2.001},
                                     {2.2, 2.2},
                                     {2.5, 2.5},
                                     {3.0, 3.0},
                                     {3.9, 3.9},
                                     {2.0, 2.5},
                                     {1.0, 3.5},
                                     {2.5, 3.0},
                                     {0.5, 3.9}};
  for (const auto& aPair : aRadii)
  {
    BRepFilletAPI_MakeFillet aFillet(aBox);
    aFillet.Add(aPair[0], findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 6.0)));
    aFillet.Add(aPair[1], findEdgeAt(aBox, gp_Pnt(4.0, 5.0, 6.0)));
    ASSERT_NO_THROW(aFillet.Build());
    if (!aFillet.IsDone())
    {
      continue;
    }
    ASSERT_FALSE(aFillet.Shape().IsNull());
    EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid())
      << "radii " << aPair[0] << " and " << aPair[1];
    EXPECT_LT(volumeOf(aFillet.Shape()), aBoxVol) << "radii " << aPair[0] << " and " << aPair[1];
  }
}

// Radii that exceed the half width by less than the tolerance: the arcs meet within it. The result
// is either refused or a valid solid, never a solid that is not one.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1568_OppositeFilletsMeetingWithinToleranceAreNotInvalid)
{
  const TopoDS_Shape aBox    = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  const double       aBoxVol = volumeOf(aBox);
  for (const double anExcess : {1.e-12, 1.e-9, 1.e-7, 1.e-5})
  {
    BRepFilletAPI_MakeFillet aFillet(aBox);
    aFillet.Add(2.0 + anExcess, findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 6.0)));
    aFillet.Add(2.0 + anExcess, findEdgeAt(aBox, gp_Pnt(4.0, 5.0, 6.0)));
    ASSERT_NO_THROW(aFillet.Build());
    if (!aFillet.IsDone())
    {
      continue;
    }
    EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid()) << "excess " << anExcess;
    EXPECT_LT(volumeOf(aFillet.Shape()), aBoxVol) << "excess " << anExcess;
  }
}

// Radii that add up to exactly the width: the arcs touch and the result is a valid solid.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1568_OppositeFilletsThatTouchAreValid)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(1.5, findEdgeAt(aBox, gp_Pnt(0.0, 5.0, 6.0)));
  aFillet.Add(2.5, findEdgeAt(aBox, gp_Pnt(4.0, 5.0, 6.0)));
  ASSERT_NO_THROW(aFillet.Build());
  ASSERT_TRUE(aFillet.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
  // The area removed from the section is (1 - pi / 4) (r1^2 + r2^2).
  const double aRemoved = (1.0 - M_PI / 4.0) * (1.5 * 1.5 + 2.5 * 2.5) * 10.0;
  EXPECT_NEAR(volumeOf(aFillet.Shape()), 240.0 - aRemoved, 1.e-6);
}

// A fillet that meets a finished fillet exactly (r = 5 twice on a 10 wide face): either refused or
// a valid solid.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1177_SecondFilletThatMeetsTheFirstExactlyIsValid)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  BRepFilletAPI_MakeFillet aFirst(aBox);
  aFirst.Add(5.0, findEdgeAt(aBox, gp_Pnt(2.0, 0.0, 6.0)));
  ASSERT_NO_THROW(aFirst.Build());
  ASSERT_TRUE(aFirst.IsDone());
  const TopoDS_Shape aRounded = aFirst.Shape();

  BRepFilletAPI_MakeFillet aSecond(aRounded);
  aSecond.Add(5.0, findEdgeAt(aRounded, gp_Pnt(2.0, 10.0, 6.0)));
  ASSERT_NO_THROW(aSecond.Build());
  if (aSecond.IsDone())
  {
    EXPECT_TRUE(BRepCheck_Analyzer(aSecond.Shape()).IsValid());
  }
}

// Two fillets on adjacent edges of a 4 wide face with r = w: never a solid that is not one.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1177_AdjacentFilletsAtTheFaceWidthAreNotAnInvalidSolid)
{
  const TopoDS_Shape       aBox = BRepPrimAPI_MakeBox(4.0, 10.0, 6.0).Shape();
  BRepFilletAPI_MakeFillet aFillet(aBox);
  aFillet.Add(4.0, findEdgeAt(aBox, gp_Pnt(4.0, 5.0, 6.0)));
  aFillet.Add(4.0, findEdgeAt(aBox, gp_Pnt(2.0, 10.0, 6.0)));
  ASSERT_NO_THROW(aFillet.Build());
  if (aFillet.IsDone())
  {
    EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
  }
}

// Four fillets of r = w / 2 round the four edges of a 4 x 4 face of a cube: the arcs meet on the
// centre lines and the face between them is a point. Build and the validity check must return, and
// a reported solid must be valid. Both run on a thread under a deadline.
TEST(BRepFilletAPI_MakeFilletTest, OCCT1568_FourFilletsThatConsumeTheirFaceReturn)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(4.0, 4.0, 4.0).Shape();
  std::atomic<int>   aState(-1); // -1 running, 0 refused, 1 built and valid, 2 built and not valid
  std::thread        aThread([&]() {
    BRepFilletAPI_MakeFillet aFillet(aBox);
    aFillet.Add(2.0, findEdgeAt(aBox, gp_Pnt(0.0, 2.0, 4.0)));
    aFillet.Add(2.0, findEdgeAt(aBox, gp_Pnt(4.0, 2.0, 4.0)));
    aFillet.Add(2.0, findEdgeAt(aBox, gp_Pnt(2.0, 0.0, 4.0)));
    aFillet.Add(2.0, findEdgeAt(aBox, gp_Pnt(2.0, 4.0, 4.0)));
    aFillet.Build();
    if (!aFillet.IsDone() || aFillet.Shape().IsNull())
    {
      aState = 0;
      return;
    }
    aState = BRepCheck_Analyzer(aFillet.Shape()).IsValid() ? 1 : 2;
  });
  for (int i = 0; i < 200 && aState.load() < 0; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  if (aState.load() < 0)
  {
    aThread.detach();
    FAIL() << "the fillet did not return within 10 s";
  }
  aThread.join();
  EXPECT_NE(aState.load(), 2) << "the builder reported a solid that is not valid";
}
