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

#include <BOPAlgo_CellsBuilder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>

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

static TopoDS_Face MakeCircleFace(const gp_Pnt& theCenter, const double theRadius)
{
  const TopoDS_Edge anEdge =
    BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(theCenter, gp::DZ()), theRadius)).Edge();
  return BRepBuilderAPI_MakeFace(BRepBuilderAPI_MakeWire(anEdge).Wire()).Face();
}

static void ExpectModified(const BOPAlgo_CellsBuilder& theBuilder,
                           const TopoDS_Shape&         theShape,
                           const TopAbs_ShapeEnum      theType,
                           const int                   theCount)
{
  BOPAlgo_CellsBuilder& aBuilder = const_cast<BOPAlgo_CellsBuilder&>(theBuilder);
  EXPECT_EQ(CountShapes(aBuilder.Modified(theShape), theType), theCount);
}

static void AddToResult(BOPAlgo_CellsBuilder& theBuilder,
                        const TopoDS_Shape&   theTake1,
                        const TopoDS_Shape&   theTake2,
                        const TopoDS_Shape&   theAvoid,
                        const int             theMaterial)
{
  NCollection_List<TopoDS_Shape> aTake;
  aTake.Append(theTake1);
  if (!theTake2.IsNull())
  {
    aTake.Append(theTake2);
  }
  NCollection_List<TopoDS_Shape> anAvoid;
  if (!theAvoid.IsNull())
  {
    anAvoid.Append(theAvoid);
  }
  theBuilder.AddToResult(aTake, anAvoid, theMaterial, false);
}
} // namespace

// Equivalent to tests/boolean/history/A5.
TEST(BOPAlgo_CellsBuilderTest, A5_CircleCellsHistory)
{
  const TopoDS_Shape aFace1 = MakeCircleFace(gp_Pnt(0.0, 0.0, 0.0), 10.0);
  const TopoDS_Shape aFace2 = MakeCircleFace(gp_Pnt(10.0, 0.0, 0.0), 10.0);
  const TopoDS_Shape aFace3 = MakeCircleFace(gp_Pnt(10.0, 10.0, 0.0), 10.0);

  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aFace1);
  anArguments.Append(aFace2);
  anArguments.Append(aFace3);
  BOPAlgo_CellsBuilder aBuilder;
  aBuilder.SetArguments(anArguments);
  aBuilder.SetRunParallel(false);
  aBuilder.Perform();
  ASSERT_FALSE(aBuilder.HasErrors());

  AddToResult(aBuilder, aFace1, TopoDS_Shape(), TopoDS_Shape(), 1);
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 4);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 2);
  ExpectModified(aBuilder, aFace3, TopAbs_FACE, 2);
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 1);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 1);
  ExpectModified(aBuilder, aFace3, TopAbs_FACE, 1);

  aBuilder.RemoveAllFromResult();
  AddToResult(aBuilder, aFace1, TopoDS_Shape(), aFace3, 1);
  AddToResult(aBuilder, aFace2, TopoDS_Shape(), aFace3, 1);
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 2);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 2);
  EXPECT_TRUE(aBuilder.IsDeleted(aFace3));
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 1);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 1);

  aBuilder.RemoveAllFromResult();
  AddToResult(aBuilder, aFace1, TopoDS_Shape(), TopoDS_Shape(), 1);
  AddToResult(aBuilder, aFace2, TopoDS_Shape(), aFace1, 2);
  AddToResult(aBuilder, aFace3, TopoDS_Shape(), aFace1, 2);
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 4);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 4);
  ExpectModified(aBuilder, aFace3, TopAbs_FACE, 4);
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aFace1, TopAbs_FACE, 1);
  ExpectModified(aBuilder, aFace2, TopAbs_FACE, 2);
  ExpectModified(aBuilder, aFace3, TopAbs_FACE, 2);
}

// Equivalent to tests/boolean/history/A6.
TEST(BOPAlgo_CellsBuilderTest, A6_BoxCellsHistory)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(gp_Pnt(2.5, 0.0, 5.0), 10.0, 10.0, 10.0).Shape();

  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox1);
  anArguments.Append(aBox2);
  anArguments.Append(aBox3);
  BOPAlgo_CellsBuilder aBuilder;
  aBuilder.SetArguments(anArguments);
  aBuilder.SetRunParallel(false);
  aBuilder.Perform();
  ASSERT_FALSE(aBuilder.HasErrors());

  AddToResult(aBuilder, aBox1, TopoDS_Shape(), TopoDS_Shape(), 1);
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 4);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 2);
  ExpectModified(aBuilder, aBox3, TopAbs_SOLID, 2);
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 1);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 1);
  ExpectModified(aBuilder, aBox3, TopAbs_SOLID, 1);

  aBuilder.RemoveAllFromResult();
  AddToResult(aBuilder, aBox1, TopoDS_Shape(), aBox3, 1);
  AddToResult(aBuilder, aBox2, TopoDS_Shape(), aBox3, 1);
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 2);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 2);
  EXPECT_TRUE(aBuilder.IsDeleted(aBox3));
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 1);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 1);

  aBuilder.RemoveAllFromResult();
  AddToResult(aBuilder, aBox1, TopoDS_Shape(), TopoDS_Shape(), 1);
  AddToResult(aBuilder, aBox2, TopoDS_Shape(), aBox1, 2);
  AddToResult(aBuilder, aBox3, TopoDS_Shape(), aBox1, 2);
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 4);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 4);
  ExpectModified(aBuilder, aBox3, TopAbs_SOLID, 4);
  aBuilder.RemoveInternalBoundaries();
  ExpectModified(aBuilder, aBox1, TopAbs_SOLID, 1);
  ExpectModified(aBuilder, aBox2, TopAbs_SOLID, 2);
  ExpectModified(aBuilder, aBox3, TopAbs_SOLID, 2);
}
