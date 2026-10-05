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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FClassifier.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepClass_Intersector.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

namespace
{

TopoDS_Wire makeRectangle(const double theXMin,
                          const double theYMin,
                          const double theXMax,
                          const double theYMax)
{
  const gp_Pnt            aPoints[] = {gp_Pnt(theXMin, theYMin, 0.0),
                                       gp_Pnt(theXMax, theYMin, 0.0),
                                       gp_Pnt(theXMax, theYMax, 0.0),
                                       gp_Pnt(theXMin, theYMax, 0.0)};
  BRepBuilderAPI_MakeWire aWireBuilder;
  for (int anIndex = 0; anIndex < 4; ++anIndex)
  {
    BRepBuilderAPI_MakeEdge anEdgeBuilder(aPoints[anIndex], aPoints[(anIndex + 1) % 4]);
    if (!anEdgeBuilder.IsDone())
    {
      ADD_FAILURE() << "Edge creation failed";
      return TopoDS_Wire();
    }
    aWireBuilder.Add(anEdgeBuilder.Edge());
  }
  if (!aWireBuilder.IsDone())
  {
    ADD_FAILURE() << "Wire creation failed";
    return TopoDS_Wire();
  }
  return aWireBuilder.Wire();
}

TopAbs_State classify(const TopoDS_Face& theFace, const gp_Pnt2d& thePoint, const bool theUseBndBox)
{
  BRepClass_FaceExplorer anExplorer(theFace);
  anExplorer.SetUseBndBox(theUseBndBox);
  BRepClass_FClassifier aClassifier(anExplorer, thePoint, Precision::Confusion());
  return aClassifier.State();
}

TopoDS_Edge referenceNextEdge(const TopoDS_Edge& theEdge, const TopoDS_Wire& theWire)
{
  TopoDS_Vertex aFirstVertex;
  TopoDS_Vertex aLastVertex;
  TopExp::Vertices(theEdge, aFirstVertex, aLastVertex, true);
  if (aLastVertex.IsNull() || aLastVertex.IsSame(aFirstVertex))
  {
    return TopoDS_Edge();
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aVertexEdges;
  TopExp::MapShapesAndAncestors(theWire, TopAbs_VERTEX, TopAbs_EDGE, aVertexEdges);
  const NCollection_List<TopoDS_Shape>* anEdges = aVertexEdges.Seek(aLastVertex);
  if (anEdges == nullptr || anEdges->Extent() != 2)
  {
    return TopoDS_Edge();
  }

  TopoDS_Edge aNextEdge;
  for (NCollection_List<TopoDS_Shape>::Iterator anIt(*anEdges); anIt.More(); anIt.Next())
  {
    if (!anIt.Value().IsNull() && !anIt.Value().IsSame(theEdge))
    {
      aNextEdge = TopoDS::Edge(anIt.Value());
    }
  }
  return aNextEdge;
}

void expectNextEdgesMatchReference(const TopoDS_Face& theFace)
{
  BRepClass_FaceExplorer anExplorer(theFace);
  TopExp_Explorer        aWireExp(theFace, TopAbs_WIRE);
  for (anExplorer.InitWires(); anExplorer.MoreWires(); anExplorer.NextWire(), aWireExp.Next())
  {
    ASSERT_TRUE(aWireExp.More());
    const TopoDS_Wire& aWire = TopoDS::Wire(aWireExp.Current());
    TopExp_Explorer    anEdgeExp(aWire, TopAbs_EDGE);
    for (anExplorer.InitEdges(); anExplorer.MoreEdges(); anExplorer.NextEdge(), anEdgeExp.Next())
    {
      ASSERT_TRUE(anEdgeExp.More());
      BRepClass_Edge     anEdgeData;
      TopAbs_Orientation anOrientation = TopAbs_EXTERNAL;
      anExplorer.CurrentEdge(anEdgeData, anOrientation);
      const TopoDS_Edge& anExpectedEdge = TopoDS::Edge(anEdgeExp.Current());
      ASSERT_TRUE(anEdgeData.Edge().IsEqual(anExpectedEdge));

      const TopoDS_Edge aExpectedNext = referenceNextEdge(anExpectedEdge, aWire);
      if (aExpectedNext.IsNull())
      {
        EXPECT_TRUE(anEdgeData.NextEdge().IsNull());
      }
      else
      {
        EXPECT_TRUE(anEdgeData.NextEdge().IsEqual(aExpectedNext));
      }
    }
    EXPECT_FALSE(anEdgeExp.More());
  }
  EXPECT_FALSE(aWireExp.More());
}

} // namespace

TEST(BRepClassFaceExplorerTest, MultiWireHole_BoundingBoxesPreserveClassification)
{
  const TopoDS_Wire anInnerWire = makeRectangle(3.0, 3.0, 7.0, 7.0);
  ASSERT_FALSE(anInnerWire.IsNull());

  BRepBuilderAPI_MakeFace aFaceBuilder(gp_Pln(gp::XOY()), 0.0, 10.0, 0.0, 10.0);
  ASSERT_TRUE(aFaceBuilder.IsDone());
  aFaceBuilder.Add(TopoDS::Wire(anInnerWire.Reversed()));
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  const gp_Pnt2d     aPoints[]    = {gp_Pnt2d(1.0, 1.0), gp_Pnt2d(5.0, 5.0), gp_Pnt2d(12.0, 5.0)};
  const TopAbs_State anExpected[] = {TopAbs_IN, TopAbs_OUT, TopAbs_OUT};
  for (int anIndex = 0; anIndex < 3; ++anIndex)
  {
    EXPECT_EQ(classify(aFace, aPoints[anIndex], false), anExpected[anIndex]);
    EXPECT_EQ(classify(aFace, aPoints[anIndex], true), anExpected[anIndex]);
  }
}

TEST(BRepClassFaceExplorerTest, CylindricalSeam_PreservesWireOccurrenceOrder)
{
  occ::handle<Geom_CylindricalSurface> aSurface =
    new Geom_CylindricalSurface(gp_Ax3(gp::Origin(), gp::DZ()), 2.0);
  BRepBuilderAPI_MakeFace aFaceBuilder(aSurface, 0.0, 2.0 * M_PI, 0.0, 5.0, Precision::Confusion());
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  NCollection_Sequence<TopoDS_Edge> anExpectedEdges;
  for (TopExp_Explorer aWireExp(aFace, TopAbs_WIRE); aWireExp.More(); aWireExp.Next())
  {
    for (TopExp_Explorer anEdgeExp(aWireExp.Current(), TopAbs_EDGE); anEdgeExp.More();
         anEdgeExp.Next())
    {
      anExpectedEdges.Append(TopoDS::Edge(anEdgeExp.Current()));
    }
  }

  bool hasSeamOccurrences = false;
  for (size_t anIndex = 1; anIndex <= anExpectedEdges.Size(); ++anIndex)
  {
    for (size_t anOther = anIndex + 1; anOther <= anExpectedEdges.Size(); ++anOther)
    {
      if (anExpectedEdges.Value(anIndex).IsSame(anExpectedEdges.Value(anOther)))
      {
        hasSeamOccurrences = true;
      }
    }
  }
  ASSERT_TRUE(hasSeamOccurrences);

  BRepClass_FaceExplorer anExplorer(aFace);
  size_t                 anExpectedIndex = 1;
  for (anExplorer.InitWires(); anExplorer.MoreWires(); anExplorer.NextWire())
  {
    for (anExplorer.InitEdges(); anExplorer.MoreEdges(); anExplorer.NextEdge())
    {
      ASSERT_LE(anExpectedIndex, anExpectedEdges.Size());
      BRepClass_Edge     anEdgeData;
      TopAbs_Orientation anOrientation = TopAbs_EXTERNAL;
      anExplorer.CurrentEdge(anEdgeData, anOrientation);
      const BRepClass_Edge& aConstEdgeData = anEdgeData;
      EXPECT_TRUE(aConstEdgeData.Edge().IsEqual(anExpectedEdges.Value(anExpectedIndex)));
      EXPECT_EQ(anOrientation, anExpectedEdges.Value(anExpectedIndex).Orientation());
      ++anExpectedIndex;
    }
  }
  EXPECT_EQ(anExpectedIndex, anExpectedEdges.Size() + 1);
  expectNextEdgesMatchReference(aFace);
  EXPECT_EQ(classify(aFace, gp_Pnt2d(M_PI, 2.5), false), TopAbs_IN);
  EXPECT_EQ(classify(aFace, gp_Pnt2d(M_PI, 2.5), true), TopAbs_IN);
  EXPECT_EQ(classify(aFace, gp_Pnt2d(M_PI, 6.0), true), TopAbs_OUT);
}

TEST(BRepClassFaceExplorerTest, ReversedWire_NextEdgesMatchReference)
{
  const TopoDS_Wire aWire = makeRectangle(0.0, 0.0, 2.0, 2.0);
  ASSERT_FALSE(aWire.IsNull());
  const TopoDS_Wire       aReversedWire = TopoDS::Wire(aWire.Reversed());
  BRepBuilderAPI_MakeFace aFaceBuilder(aReversedWire, true);
  ASSERT_TRUE(aFaceBuilder.IsDone());

  expectNextEdgesMatchReference(aFaceBuilder.Face());
}

TEST(BRepClassFaceExplorerTest, SingleClosedEdge_HasNoNextEdge)
{
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 2.0);
  BRepBuilderAPI_MakeEdge  anEdgeBuilder(aCircle);
  ASSERT_TRUE(anEdgeBuilder.IsDone());
  BRepBuilderAPI_MakeWire aWireBuilder(anEdgeBuilder.Edge());
  ASSERT_TRUE(aWireBuilder.IsDone());
  BRepBuilderAPI_MakeFace aFaceBuilder(aWireBuilder.Wire(), true);
  ASSERT_TRUE(aFaceBuilder.IsDone());

  BRepClass_FaceExplorer anExplorer(aFaceBuilder.Face());
  anExplorer.InitWires();
  ASSERT_TRUE(anExplorer.MoreWires());
  anExplorer.InitEdges();
  ASSERT_TRUE(anExplorer.MoreEdges());
  BRepClass_Edge     anEdgeData;
  TopAbs_Orientation anOrientation = TopAbs_EXTERNAL;
  anExplorer.CurrentEdge(anEdgeData, anOrientation);
  EXPECT_TRUE(anEdgeData.NextEdge().IsNull());
  anExplorer.NextEdge();
  EXPECT_FALSE(anExplorer.MoreEdges());
  expectNextEdgesMatchReference(aFaceBuilder.Face());
}

TEST(BRepClassFaceExplorerTest, DegenerateEdges_HaveNoNextEdge)
{
  BRepPrimAPI_MakeSphere aSphereBuilder(2.0);
  aSphereBuilder.Build();
  ASSERT_TRUE(aSphereBuilder.IsDone());
  const TopoDS_Shape aSphere = aSphereBuilder.Shape();
  ASSERT_FALSE(aSphere.IsNull());

  bool hasDegenerateEdge = false;
  for (TopExp_Explorer aFaceExp(aSphere, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
  {
    const TopoDS_Face&     aFace = TopoDS::Face(aFaceExp.Current());
    BRepClass_FaceExplorer anExplorer(aFace);
    for (anExplorer.InitWires(); anExplorer.MoreWires(); anExplorer.NextWire())
    {
      for (anExplorer.InitEdges(); anExplorer.MoreEdges(); anExplorer.NextEdge())
      {
        BRepClass_Edge     anEdgeData;
        TopAbs_Orientation anOrientation = TopAbs_EXTERNAL;
        anExplorer.CurrentEdge(anEdgeData, anOrientation);
        if (BRep_Tool::Degenerated(anEdgeData.Edge()))
        {
          hasDegenerateEdge = true;
          EXPECT_TRUE(anEdgeData.NextEdge().IsNull());
        }
      }
    }
    expectNextEdgesMatchReference(aFace);
  }
  EXPECT_TRUE(hasDegenerateEdge);
}

TEST(BRepClassEdgeTest, SetTopology_InvalidatesDerivedData)
{
  const TopoDS_Wire aWire = makeRectangle(0.0, 0.0, 2.0, 2.0);
  ASSERT_FALSE(aWire.IsNull());
  BRepBuilderAPI_MakeFace aFaceBuilder(aWire, true);
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  const TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
  anEdgeExp.Next();
  ASSERT_TRUE(anEdgeExp.More());
  const TopoDS_Edge aNextEdge = TopoDS::Edge(anEdgeExp.Current());

  double                           aFirst = 0.0;
  double                           aLast  = 0.0;
  const occ::handle<Geom2d_Curve>& aCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
  ASSERT_FALSE(aCurve.IsNull());

  BRepClass_Edge anEdgeData(anEdge, aFace);
  anEdgeData.SetGeometry(aCurve, aFirst, aLast);
  Bnd_Box2d aBox;
  aBox.Update(0.0, 0.0, 2.0, 2.0);
  anEdgeData.SetBoundingBox(aBox);
  anEdgeData.SetNextEdge(aNextEdge);

  anEdgeData.SetEdge(TopoDS::Edge(anEdge.Reversed()));
  EXPECT_TRUE(anEdgeData.Curve().IsNull());
  EXPECT_TRUE(anEdgeData.NextEdge().IsNull());
  EXPECT_TRUE(anEdgeData.BoundingBox().IsVoid());
  EXPECT_EQ(anEdgeData.BoundingBoxState(), BRepClass_Edge::BndBoxState::NotBuilt);
  EXPECT_EQ(anEdgeData.FirstParameter(), 0.0);
  EXPECT_EQ(anEdgeData.LastParameter(), 0.0);

  anEdgeData.SetGeometry(aCurve, aFirst, aLast);
  anEdgeData.SetBoundingBox(aBox);
  anEdgeData.SetFace(TopoDS::Face(aFace.Reversed()));
  EXPECT_TRUE(anEdgeData.Curve().IsNull());
  EXPECT_TRUE(anEdgeData.BoundingBox().IsVoid());
  EXPECT_EQ(anEdgeData.BoundingBoxState(), BRepClass_Edge::BndBoxState::NotBuilt);
}

TEST(BRepClassEdgeTest, VoidBoundingBox_BecomesUnavailable)
{
  BRepClass_Edge anEdgeData;
  anEdgeData.SetBoundingBox(Bnd_Box2d());
  EXPECT_TRUE(anEdgeData.BoundingBox().IsVoid());
  EXPECT_EQ(anEdgeData.BoundingBoxState(), BRepClass_Edge::BndBoxState::Unavailable);
}

TEST(BRepClassIntersectorTest, NotBuiltAndUnavailableBoundingBoxes_IntersectWithoutCacheWrites)
{
  const TopoDS_Wire aWire = makeRectangle(0.0, 0.0, 2.0, 2.0);
  ASSERT_FALSE(aWire.IsNull());
  BRepBuilderAPI_MakeFace aFaceBuilder(aWire, true);
  ASSERT_TRUE(aFaceBuilder.IsDone());
  const TopoDS_Face aFace = aFaceBuilder.Face();

  TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExp.More());
  const TopoDS_Edge                anEdge = TopoDS::Edge(anEdgeExp.Current());
  double                           aFirst = 0.0;
  double                           aLast  = 0.0;
  const occ::handle<Geom2d_Curve>& aCurve = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
  ASSERT_FALSE(aCurve.IsNull());

  BRepClass_Edge anEdgeData(anEdge, aFace);
  anEdgeData.SetGeometry(aCurve, aFirst, aLast);
  anEdgeData.SetUseBndBox(true);

  gp_Pnt2d aPoint;
  gp_Vec2d aTangent;
  aCurve->D1(0.5 * (aFirst + aLast), aPoint, aTangent);
  ASSERT_GT(aTangent.SquareMagnitude(), Precision::SquarePConfusion());
  const gp_Dir2d aNormal(-aTangent.Y(), aTangent.X());

  BRepClass_Intersector aNotBuiltIntersector;
  aNotBuiltIntersector.Perform(gp_Lin2d(aPoint, aNormal),
                               RealLast(),
                               Precision::Confusion(),
                               anEdgeData);
  EXPECT_TRUE(aNotBuiltIntersector.IsDone());
  EXPECT_EQ(anEdgeData.BoundingBoxState(), BRepClass_Edge::BndBoxState::NotBuilt);

  anEdgeData.SetBoundingBoxUnavailable();
  BRepClass_Intersector anUnavailableIntersector;
  anUnavailableIntersector.Perform(gp_Lin2d(aPoint, aNormal),
                                   RealLast(),
                                   Precision::Confusion(),
                                   anEdgeData);
  EXPECT_TRUE(anUnavailableIntersector.IsDone());
  EXPECT_EQ(anEdgeData.BoundingBoxState(), BRepClass_Edge::BndBoxState::Unavailable);
}
