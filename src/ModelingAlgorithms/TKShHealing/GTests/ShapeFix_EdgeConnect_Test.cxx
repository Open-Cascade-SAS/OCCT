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

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Curve.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <ShapeFix_EdgeConnect.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

namespace
{
//=================================================================================================

TopoDS_Edge makeEdge(const gp_Pnt& theFirst, const gp_Pnt& theLast)
{
  return BRepBuilderAPI_MakeEdge(theFirst, theLast).Edge();
}

//=================================================================================================

gp_Pnt orientedFirstPoint(const TopoDS_Edge& theEdge)
{
  return BRep_Tool::Pnt(TopExp::FirstVertex(theEdge, true));
}

//=================================================================================================

gp_Pnt orientedLastPoint(const TopoDS_Edge& theEdge)
{
  return BRep_Tool::Pnt(TopExp::LastVertex(theEdge, true));
}

//=================================================================================================

double totalLength(const TopoDS_Shape& theShape)
{
  GProp_GProps aProperties;
  BRepGProp::LinearProperties(theShape, aProperties);
  return aProperties.Mass();
}
} // namespace

//=================================================================================================

TEST(ShapeFix_EdgeConnectTest, ConnectsPairAtBoundingBoxMidpoint)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  const TopoDS_Vertex aJoint1 = TopExp::LastVertex(anEdge1, true);
  const TopoDS_Vertex aJoint2 = TopExp::FirstVertex(anEdge2, true);
  ASSERT_TRUE(aJoint1.IsSame(aJoint2));
  EXPECT_NEAR(BRep_Tool::Pnt(aJoint1).X(), 1.1, Precision::Confusion());
  EXPECT_NEAR(BRep_Tool::Tolerance(aJoint1), 0.10001, Precision::Confusion());
  EXPECT_NEAR(orientedFirstPoint(anEdge1).X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(orientedLastPoint(anEdge2).X(), 2.0, Precision::Confusion());
}

TEST(ShapeFix_EdgeConnectTest, HonorsEdgeOrientation)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(2.0, 0.0, 0.0), gp_Pnt(3.0, 0.0, 0.0));
  anEdge1.Reverse();
  anEdge2.Reverse();
  const gp_Pnt anExpected =
    gp_Pnt((orientedLastPoint(anEdge1).XYZ() + orientedFirstPoint(anEdge2).XYZ()) * 0.5);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  const TopoDS_Vertex aJoint1 = TopExp::LastVertex(anEdge1, true);
  const TopoDS_Vertex aJoint2 = TopExp::FirstVertex(anEdge2, true);
  ASSERT_TRUE(aJoint1.IsSame(aJoint2));
  EXPECT_TRUE(BRep_Tool::Pnt(aJoint1).IsEqual(anExpected, Precision::Confusion()));
}

TEST(ShapeFix_EdgeConnectTest, MergesExistingConnectivityGroups)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge anEdge3 = makeEdge(gp_Pnt(3.0, 0.0, 0.0), gp_Pnt(4.0, 0.0, 0.0));
  TopoDS_Edge anEdge4 = makeEdge(gp_Pnt(4.1, 0.0, 0.0), gp_Pnt(5.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Add(anEdge3, anEdge4);
  aConnector.Add(TopoDS::Edge(anEdge2.Reversed()), TopoDS::Edge(anEdge3.Reversed()));
  aConnector.Build();

  const TopoDS_Vertex aJoint = TopExp::LastVertex(anEdge1, true);
  EXPECT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(aJoint.IsSame(TopExp::LastVertex(anEdge3, true)));
  EXPECT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge4, true)));
  EXPECT_NEAR(BRep_Tool::Pnt(aJoint).X(), 2.55, Precision::Confusion());
}

TEST(ShapeFix_EdgeConnectTest, AddsAllConsecutiveEdgesOfOpenWire)
{
  TopoDS_Edge  anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge  anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge  anEdge3 = makeEdge(gp_Pnt(2.1, 0.0, 0.0), gp_Pnt(3.0, 0.0, 0.0));
  BRep_Builder aBuilder;
  TopoDS_Wire  aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, anEdge1);
  aBuilder.Add(aWire, anEdge2);
  aBuilder.Add(aWire, anEdge3);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(aWire);
  aConnector.Build();

  EXPECT_TRUE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge2, true).IsSame(TopExp::FirstVertex(anEdge3, true)));
  EXPECT_FALSE(TopExp::FirstVertex(anEdge1, true).IsSame(TopExp::LastVertex(anEdge3, true)));
}

TEST(ShapeFix_EdgeConnectTest, ConnectsLastAndFirstEdgesOfClosedWire)
{
  TopoDS_Edge  anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge  anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(0.5, 1.0, 0.0));
  TopoDS_Edge  anEdge3 = makeEdge(gp_Pnt(0.4, 1.0, 0.0), gp_Pnt(0.0, 0.1, 0.0));
  BRep_Builder aBuilder;
  TopoDS_Wire  aWire;
  aBuilder.MakeWire(aWire);
  aBuilder.Add(aWire, anEdge1);
  aBuilder.Add(aWire, anEdge2);
  aBuilder.Add(aWire, anEdge3);
  aWire.Closed(true);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(aWire);
  aConnector.Build();

  EXPECT_TRUE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge2, true).IsSame(TopExp::FirstVertex(anEdge3, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge3, true).IsSame(TopExp::FirstVertex(anEdge1, true)));
}

TEST(ShapeFix_EdgeConnectTest, RepeatedAddIsIdempotent)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  EXPECT_TRUE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_NEAR(BRep_Tool::Pnt(TopExp::LastVertex(anEdge1, true)).X(), 1.1, Precision::Confusion());
}

TEST(ShapeFix_EdgeConnectTest, ClearDiscardsPendingConnections)
{
  TopoDS_Edge         anEdge1     = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge         anEdge2     = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  const TopoDS_Vertex anOriginal1 = TopExp::LastVertex(anEdge1, true);
  const TopoDS_Vertex anOriginal2 = TopExp::FirstVertex(anEdge2, true);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Clear();
  aConnector.Build();

  EXPECT_TRUE(anOriginal1.IsSame(TopExp::LastVertex(anEdge1, true)));
  EXPECT_TRUE(anOriginal2.IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_FALSE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
}

TEST(ShapeFix_EdgeConnectTest, BuildClearsCompletedConnections)
{
  TopoDS_Edge          anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge          anEdge2 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  TopoDS_Edge anEdge3 = makeEdge(gp_Pnt(3.0, 0.0, 0.0), gp_Pnt(4.0, 0.0, 0.0));
  TopoDS_Edge anEdge4 = makeEdge(gp_Pnt(4.2, 0.0, 0.0), gp_Pnt(5.0, 0.0, 0.0));
  aConnector.Build();

  EXPECT_FALSE(TopExp::LastVertex(anEdge3, true).IsSame(TopExp::FirstVertex(anEdge4, true)));
}

TEST(ShapeFix_EdgeConnectTest, UsesConfusionAsMinimumTolerance)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  const TopoDS_Vertex aJoint = TopExp::LastVertex(anEdge1, true);
  ASSERT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_NEAR(BRep_Tool::Tolerance(aJoint), Precision::Confusion(), Precision::PConfusion());
}

TEST(ShapeFix_EdgeConnectTest, ComputesThreeDimensionalBoundingBoxCenter)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(-1.0, -2.0, -3.0), gp_Pnt(1.0, 2.0, 3.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(5.0, 8.0, 11.0), gp_Pnt(6.0, 9.0, 12.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  const TopoDS_Vertex aJoint = TopExp::LastVertex(anEdge1, true);
  const gp_Pnt        aPoint = BRep_Tool::Pnt(aJoint);
  EXPECT_TRUE(aPoint.IsEqual(gp_Pnt(3.0, 5.0, 7.0), Precision::Confusion()));
  EXPECT_NEAR(BRep_Tool::Tolerance(aJoint),
              gp_Pnt(1.0, 2.0, 3.0).Distance(gp_Pnt(3.0, 5.0, 7.0)) * 1.0001,
              Precision::Confusion());
}

TEST(ShapeFix_EdgeConnectTest, AppendsUnboundVertexToExistingGroup)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge anEdge3 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(3.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Add(TopoDS::Edge(anEdge2.Reversed()), anEdge3);
  aConnector.Build();

  const TopoDS_Vertex aJoint = TopExp::LastVertex(anEdge1, true);
  EXPECT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge3, true)));
}

TEST(ShapeFix_EdgeConnectTest, PrependsUnboundVertexToExistingGroup)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.9, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.0, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge anEdge3 = makeEdge(gp_Pnt(2.1, 0.0, 0.0), gp_Pnt(3.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge2, anEdge3);
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  const TopoDS_Vertex aJoint = TopExp::LastVertex(anEdge1, true);
  EXPECT_TRUE(aJoint.IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_FALSE(aJoint.IsSame(TopExp::LastVertex(anEdge2, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge2, true).IsSame(TopExp::FirstVertex(anEdge3, true)));
}

TEST(ShapeFix_EdgeConnectTest, ProcessesMultipleWiresIndependently)
{
  TopoDS_Edge  anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge  anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge  anEdge3 = makeEdge(gp_Pnt(10.0, 0.0, 0.0), gp_Pnt(11.0, 0.0, 0.0));
  TopoDS_Edge  anEdge4 = makeEdge(gp_Pnt(11.1, 0.0, 0.0), gp_Pnt(12.0, 0.0, 0.0));
  BRep_Builder aBuilder;
  TopoDS_Wire  aWire1;
  TopoDS_Wire  aWire2;
  aBuilder.MakeWire(aWire1);
  aBuilder.MakeWire(aWire2);
  aBuilder.Add(aWire1, anEdge1);
  aBuilder.Add(aWire1, anEdge2);
  aBuilder.Add(aWire2, anEdge3);
  aBuilder.Add(aWire2, anEdge4);
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aWire1);
  aBuilder.Add(aCompound, aWire2);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(aCompound);
  aConnector.Build();

  EXPECT_TRUE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge3, true).IsSame(TopExp::FirstVertex(anEdge4, true)));
  EXPECT_FALSE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge4, true)));
}

TEST(ShapeFix_EdgeConnectTest, IgnoresEdgesOutsideWiresInShapeOverload)
{
  TopoDS_Edge     anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge     anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anEdge1);
  aBuilder.Add(aCompound, anEdge2);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(aCompound);
  aConnector.Build();

  EXPECT_FALSE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
}

TEST(ShapeFix_EdgeConnectTest, AcceptsEmptyWire)
{
  BRep_Builder aBuilder;
  TopoDS_Wire  anEmptyWire;
  aBuilder.MakeWire(anEmptyWire);

  ShapeFix_EdgeConnect aConnector;
  EXPECT_NO_THROW(aConnector.Add(anEmptyWire));
  EXPECT_NO_THROW(aConnector.Build());
}

TEST(ShapeFix_EdgeConnectTest, CanBeReusedAfterBuild)
{
  TopoDS_Edge anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge anEdge2 = makeEdge(gp_Pnt(1.1, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  TopoDS_Edge anEdge3 = makeEdge(gp_Pnt(3.0, 0.0, 0.0), gp_Pnt(4.0, 0.0, 0.0));
  TopoDS_Edge anEdge4 = makeEdge(gp_Pnt(4.1, 0.0, 0.0), gp_Pnt(5.0, 0.0, 0.0));

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();
  aConnector.Add(anEdge3, anEdge4);
  aConnector.Build();

  EXPECT_TRUE(TopExp::LastVertex(anEdge1, true).IsSame(TopExp::FirstVertex(anEdge2, true)));
  EXPECT_TRUE(TopExp::LastVertex(anEdge3, true).IsSame(TopExp::FirstVertex(anEdge4, true)));
}

TEST(ShapeFix_EdgeConnectTest, PreservesUnderlyingCurves)
{
  TopoDS_Edge                   anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge                   anEdge2 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  double                        aFirst1 = 0.0;
  double                        aLast1  = 0.0;
  const occ::handle<Geom_Curve> aCurve1 = BRep_Tool::Curve(anEdge1, aFirst1, aLast1);
  double                        aFirst2 = 0.0;
  double                        aLast2  = 0.0;
  const occ::handle<Geom_Curve> aCurve2 = BRep_Tool::Curve(anEdge2, aFirst2, aLast2);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();

  double aNewFirst = 0.0;
  double aNewLast  = 0.0;
  EXPECT_TRUE(BRep_Tool::Curve(anEdge1, aNewFirst, aNewLast) == aCurve1);
  EXPECT_DOUBLE_EQ(aNewFirst, aFirst1);
  EXPECT_DOUBLE_EQ(aNewLast, aLast1);
  EXPECT_TRUE(BRep_Tool::Curve(anEdge2, aNewFirst, aNewLast) == aCurve2);
  EXPECT_DOUBLE_EQ(aNewFirst, aFirst2);
  EXPECT_DOUBLE_EQ(aNewLast, aLast2);
}

TEST(ShapeFix_EdgeConnectTest, PreservesTotalLengthAgainstReference)
{
  TopoDS_Edge     anEdge1 = makeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  TopoDS_Edge     anEdge2 = makeEdge(gp_Pnt(1.2, 0.0, 0.0), gp_Pnt(2.0, 0.0, 0.0));
  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, anEdge1);
  aBuilder.Add(aCompound, anEdge2);
  const double anInputLength = totalLength(aCompound);

  ShapeFix_EdgeConnect aConnector;
  aConnector.Add(anEdge1, anEdge2);
  aConnector.Build();
  const double aResultLength = totalLength(aCompound);

  EXPECT_NEAR(anInputLength, 1.8, Precision::Confusion());
  EXPECT_NEAR(aResultLength, anInputLength, Precision::Confusion());
  EXPECT_NEAR(aResultLength, 1.8, Precision::Confusion());
}
