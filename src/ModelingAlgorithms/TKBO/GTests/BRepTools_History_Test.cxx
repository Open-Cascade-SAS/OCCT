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

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <BRepTools_History.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>

namespace
{
static int CountShapes(const NCollection_List<TopoDS_Shape>& theShapes,
                       const TopAbs_ShapeEnum                theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(theShapes); anIterator.More();
       anIterator.Next())
  {
    TopExp::MapShapes(anIterator.Value(), aMap);
  }

  int aCount = 0;
  for (int anIndex = 1; anIndex <= aMap.Extent(); ++anIndex)
  {
    if (aMap(anIndex).ShapeType() == theType)
    {
      ++aCount;
    }
  }
  return aCount;
}

static void ExpectHistory(const BRepTools_History& theHistory,
                          const TopoDS_Shape&       theShape,
                          const bool                theDeleted,
                          const TopAbs_ShapeEnum    theModifiedType,
                          const int                 theModifiedCount,
                          const TopAbs_ShapeEnum    theGeneratedType,
                          const int                 theGeneratedCount)
{
  EXPECT_EQ(theHistory.IsRemoved(theShape), theDeleted);
  EXPECT_EQ(CountShapes(theHistory.Modified(theShape), theModifiedType), theModifiedCount);
  EXPECT_EQ(CountShapes(theHistory.Generated(theShape), theGeneratedType), theGeneratedCount);
}

static TopoDS_Edge MakeCircleEdge(const gp_Pnt& theCenter, const double theRadius)
{
  return BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(theCenter, gp::DZ()), theRadius)).Edge();
}

static TopoDS_Face MakeRectangleFace(const double theFirstX,
                                     const double theLastX,
                                     const double theFirstY,
                                     const double theLastY)
{
  return BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp::DZ()),
                                 theFirstX,
                                 theLastX,
                                 theFirstY,
                                 theLastY)
    .Face();
}

static TopoDS_Face MakePolygonFace()
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-5.0, 0.0, -5.0));
  aPolygon.Add(gp_Pnt(-5.0, 0.0, 10.0));
  aPolygon.Add(gp_Pnt(5.0, 0.0, 10.0));
  aPolygon.Add(gp_Pnt(5.0, 0.0, -5.0));
  aPolygon.Add(gp_Pnt(2.0, 0.0, -5.0));
  aPolygon.Add(gp_Pnt(2.0, 0.0, 5.0));
  aPolygon.Add(gp_Pnt(-2.0, 0.0, 5.0));
  aPolygon.Add(gp_Pnt(-2.0, 0.0, -5.0));
  aPolygon.Close();
  return BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face();
}

static TopoDS_Shape MakeExpandedFace(const TopoDS_Face& theFace)
{
  double                          aFirstU  = 0.0;
  double                          aLastU   = 0.0;
  double                          aFirstV  = 0.0;
  double                          aLastV   = 0.0;
  const occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(theFace);
  BRepTools::UVBounds(theFace, aFirstU, aLastU, aFirstV, aLastV);
  return BRepBuilderAPI_MakeFace(aSurface,
                                 aFirstU - 100.0,
                                 aLastU + 100.0,
                                 aFirstV - 100.0,
                                 aLastV + 100.0,
                                 Precision::Confusion())
    .Face();
}

static void ExpectBooleanHistory(const TopoDS_Shape&     theFirst,
                                 const TopoDS_Shape&     theSecond,
                                 const BOPAlgo_Operation theOperation,
                                 const bool              theFirstDeleted,
                                 const int               theFirstModified,
                                 const int               theFirstGenerated,
                                 const bool              theSecondDeleted,
                                 const int               theSecondModified,
                                 const int               theSecondGenerated)
{
  BOPAlgo_BOP*    anBOP       = nullptr;
  BOPAlgo_BOP     aBOP;
  BOPAlgo_Section aSection;
  BOPAlgo_Builder* anAlgorithm = nullptr;
  switch (theOperation)
  {
    case BOPAlgo_COMMON:
      anBOP = &aBOP;
      break;
    case BOPAlgo_FUSE:
      anBOP = &aBOP;
      break;
    case BOPAlgo_CUT:
      anBOP = &aBOP;
      break;
    case BOPAlgo_SECTION:
      anAlgorithm = &aSection;
      break;
    default:
      ADD_FAILURE() << "Unsupported history operation";
      return;
  }

  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(theFirst);
  anArguments.Append(theSecond);

  BOPAlgo_PaveFiller aFiller;
  aFiller.SetArguments(anArguments);
  aFiller.SetFuzzyValue(0.0);
  aFiller.SetRunParallel(false);
  aFiller.SetNonDestructive(false);
  aFiller.Perform();
  ASSERT_FALSE(aFiller.HasErrors());

  const NCollection_List<TopoDS_Shape>& aFillerArguments = aFiller.Arguments();
  if (anBOP != nullptr)
  {
    anBOP->AddArgument(aFillerArguments.First());
    anBOP->AddTool(aFillerArguments.Last());
    anBOP->SetOperation(theOperation);
    anAlgorithm = anBOP;
  }
  else
  {
    anAlgorithm->AddArgument(aFillerArguments.First());
    anAlgorithm->AddArgument(aFillerArguments.Last());
  }
  anAlgorithm->SetRunParallel(false);
  anAlgorithm->SetToFillHistory(true);
  anAlgorithm->PerformWithFiller(aFiller);
  if (anAlgorithm->HasErrors())
  {
    Standard_SStream anErrors;
    anAlgorithm->DumpErrors(anErrors);
    ADD_FAILURE() << anErrors.str();
    return;
  }

  const occ::handle<BRepTools_History> aHistory = anAlgorithm->History();
  ASSERT_FALSE(aHistory.IsNull());

  const TopAbs_ShapeEnum aFirstGeneratedType =
    theFirst.ShapeType() == TopAbs_FACE ? TopAbs_EDGE : TopAbs_VERTEX;
  const TopAbs_ShapeEnum aSecondGeneratedType =
    theSecond.ShapeType() == TopAbs_FACE ? TopAbs_EDGE : TopAbs_VERTEX;
  ExpectHistory(*aHistory,
                theFirst,
                theFirstDeleted,
                theFirst.ShapeType(),
                theFirstModified,
                aFirstGeneratedType,
                theFirstGenerated);
  ExpectHistory(*aHistory,
                theSecond,
                theSecondDeleted,
                theSecond.ShapeType(),
                theSecondModified,
                aSecondGeneratedType,
                theSecondGenerated);
}
} // namespace

// Equivalent to tests/boolean/history/A1.
TEST(BRepTools_HistoryTest, A1_IntersectingCircleEdges)
{
  const TopoDS_Shape anEdge1 = MakeCircleEdge(gp_Pnt(0.0, 0.0, 0.0), 10.0);
  const TopoDS_Shape anEdge2 = MakeCircleEdge(gp_Pnt(10.0, 0.0, 0.0), 10.0);

  ExpectBooleanHistory(anEdge1, anEdge2, BOPAlgo_COMMON, true, 0, 0, true, 0, 0);
  ExpectBooleanHistory(anEdge1, anEdge2, BOPAlgo_FUSE, false, 3, 2, false, 3, 2);
  ExpectBooleanHistory(anEdge1, anEdge2, BOPAlgo_CUT, false, 3, 2, true, 0, 2);
  ExpectBooleanHistory(anEdge2, anEdge1, BOPAlgo_CUT, false, 3, 2, true, 0, 2);
  ExpectBooleanHistory(anEdge1, anEdge2, BOPAlgo_SECTION, true, 0, 2, true, 0, 2);
}

// Equivalent to tests/boolean/history/A2.
TEST(BRepTools_HistoryTest, A2_IntersectingPlanarFaces)
{
  const TopoDS_Shape aFace1 = MakeRectangleFace(-10.0, 10.0, -10.0, 10.0);
  const TopoDS_Shape aFace2 = MakePolygonFace();

  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_COMMON, true, 0, 0, true, 0, 0);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_FUSE, false, 1, 2, false, 3, 2);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_CUT, false, 1, 2, true, 0, 2);
  ExpectBooleanHistory(aFace2, aFace1, BOPAlgo_CUT, false, 3, 2, true, 0, 2);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_SECTION, true, 0, 2, true, 0, 2);
}

// Equivalent to tests/boolean/history/A3.
TEST(BRepTools_HistoryTest, A3_OverlappingPlanarFaces)
{
  const TopoDS_Shape aFace1 = MakeRectangleFace(-10.0, 10.0, -10.0, 10.0);
  const TopoDS_Shape aFace2 = MakeRectangleFace(0.0, 20.0, -10.0, 10.0);

  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_COMMON, false, 1, 0, false, 1, 0);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_FUSE, false, 2, 0, false, 2, 0);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_CUT, false, 1, 0, true, 0, 0);
  ExpectBooleanHistory(aFace2, aFace1, BOPAlgo_CUT, false, 1, 0, true, 0, 0);
  ExpectBooleanHistory(aFace1, aFace2, BOPAlgo_SECTION, true, 0, 0, true, 0, 0);
}

// Equivalent to tests/boolean/history/A4.
TEST(BRepTools_HistoryTest, A4_GeneralFuseEdgeAndFace)
{
  const TopoDS_Shape aFace = MakeRectangleFace(0.0, 10.0, 0.0, 10.0);
  const TopoDS_Shape anEdge =
    BRepBuilderAPI_MakeEdge(gp_Pnt(5.0, 5.0, -5.0), gp_Pnt(5.0, 5.0, 5.0)).Edge();

  BOPAlgo_Builder aBuilder;
  aBuilder.AddArgument(aFace);
  aBuilder.AddArgument(anEdge);
  aBuilder.SetRunParallel(false);
  aBuilder.Perform();
  ASSERT_FALSE(aBuilder.HasErrors());
  EXPECT_FALSE(aBuilder.IsDeleted(aFace));
  EXPECT_FALSE(aBuilder.IsDeleted(anEdge));
  EXPECT_EQ(CountShapes(aBuilder.Modified(aFace), TopAbs_FACE), 1);
  EXPECT_EQ(CountShapes(aBuilder.Modified(anEdge), TopAbs_EDGE), 2);
  EXPECT_EQ(CountShapes(aBuilder.Generated(aFace), TopAbs_VERTEX), 1);
  EXPECT_EQ(CountShapes(aBuilder.Generated(anEdge), TopAbs_VERTEX), 1);
}

// Equivalent to tests/boolean/history/A8.
TEST(BRepTools_HistoryTest, A8_FuseAndSimplifyHistory)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse   aFuse(aBox1, aBox2);
  aFuse.SetRunParallel(false);
  aFuse.Build();
  ASSERT_FALSE(aFuse.HasErrors());

  int anIndex = 0;
  for (TopExp_Explorer anExplorer(aBox1, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (anIndex++ < 2)
    {
      continue;
    }
    const NCollection_List<TopoDS_Shape>& aModified = aFuse.Modified(anExplorer.Current());
    EXPECT_EQ(CountShapes(aModified, TopAbs_FACE), 2);
  }

  aFuse.SimplifyResult(true, true);
  anIndex = 0;
  for (TopExp_Explorer anExplorer(aBox1, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (anIndex++ < 2)
    {
      continue;
    }
    const NCollection_List<TopoDS_Shape>& aModified = aFuse.Modified(anExplorer.Current());
    EXPECT_EQ(CountShapes(aModified, TopAbs_FACE), 1);
    if (!aModified.IsEmpty())
    {
      GProp_GProps aProperties;
      BRepGProp::SurfaceProperties(aModified.First(), aProperties);
      EXPECT_NEAR(aProperties.Mass(), 150.0, Precision::Confusion());
    }
  }
}

// Equivalent to tests/boolean/history/A9.
TEST(BRepTools_HistoryTest, A9_SectionExpandedBoxFacesHistory)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  NCollection_List<TopoDS_Shape> anInputFaces;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Shape anExpandedFace = MakeExpandedFace(TopoDS::Face(anExplorer.Current()));
    anInputFaces.Append(anExpandedFace);
  }

  BOPAlgo_PaveFiller aFiller;
  aFiller.SetArguments(anInputFaces);
  aFiller.SetFuzzyValue(0.0);
  aFiller.SetRunParallel(false);
  aFiller.SetNonDestructive(false);
  aFiller.Perform();
  ASSERT_FALSE(aFiller.HasErrors());

  BOPAlgo_Section aSection;
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aFiller.Arguments());
       anIterator.More();
       anIterator.Next())
  {
    aSection.AddArgument(anIterator.Value());
  }
  aSection.SetToFillHistory(true);
  aSection.SetRunParallel(false);
  aSection.PerformWithFiller(aFiller);
  ASSERT_FALSE(aSection.HasErrors());
  ASSERT_FALSE(aSection.Shape().IsNull());
  const occ::handle<BRepTools_History> aHistory = aSection.History();
  ASSERT_FALSE(aHistory.IsNull());
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(anInputFaces); anIterator.More();
       anIterator.Next())
  {
    EXPECT_TRUE(aHistory->IsRemoved(anIterator.Value()));
    EXPECT_EQ(CountShapes(aHistory->Modified(anIterator.Value()), TopAbs_FACE), 0);
    EXPECT_EQ(CountShapes(aHistory->Generated(anIterator.Value()), TopAbs_EDGE), 12);
  }
}
