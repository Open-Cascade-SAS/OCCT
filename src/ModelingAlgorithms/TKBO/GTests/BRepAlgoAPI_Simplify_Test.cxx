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

#include "BOPTest_Utilities.pxx"

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>

#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>

namespace
{
int CountShapes(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapes(theShape, aMap);

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

TopoDS_Face MakePlaneFace(const gp_Pln& thePlane,
                          const double  theFirstU,
                          const double  theLastU,
                          const double  theFirstV,
                          const double  theLastV)
{
  return BRepBuilderAPI_MakeFace(thePlane, theFirstU, theLastU, theFirstV, theLastV).Face();
}

TopoDS_Shape MakeThreeFaceShell()
{
  const gp_Pln          aPlane(gp_Pnt(0.0, 0.0, 0.0), gp::DZ());
  BRepBuilderAPI_Sewing aSewing;
  aSewing.Add(MakePlaneFace(aPlane, -10.0, 10.0, -10.0, 10.0));
  aSewing.Add(MakePlaneFace(aPlane, 10.0, 30.0, -10.0, 10.0));
  aSewing.Add(MakePlaneFace(aPlane, 30.0, 50.0, -10.0, 10.0));
  aSewing.Perform();
  return aSewing.SewedShape();
}

double GetLength(const TopoDS_Shape& theShape)
{
  GProp_GProps aProperties;
  BRepGProp::LinearProperties(theShape, aProperties);
  return aProperties.Mass();
}
} // namespace

// Equivalent to tests/boolean/simplify/A1.
TEST(BRepAlgoAPI_SimplifyTest, A1_FuseCoplanarBoxes_SimplifiesFaces)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 15.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(3.0, 7.0, 0.0), 10.0, 10.0, 15.0).Shape();

  BRepAlgoAPI_Fuse               aFuse;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aBox1);
  aFuse.SetArguments(anArguments);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aBox2);
  aFuse.SetTools(aTools);
  aFuse.SetRunParallel(false);
  aFuse.Build();

  ASSERT_FALSE(aFuse.HasErrors());
  const TopoDS_Shape& aResult = aFuse.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(CountShapes(aResult, TopAbs_VERTEX), 20);
  EXPECT_EQ(CountShapes(aResult, TopAbs_EDGE), 32);
  EXPECT_EQ(CountShapes(aResult, TopAbs_WIRE), 14);
  EXPECT_EQ(CountShapes(aResult, TopAbs_FACE), 14);

  aFuse.SimplifyResult(true, true);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(CountShapes(aResult, TopAbs_VERTEX), 16);
  EXPECT_EQ(CountShapes(aResult, TopAbs_EDGE), 24);
  EXPECT_EQ(CountShapes(aResult, TopAbs_WIRE), 10);
  EXPECT_EQ(CountShapes(aResult, TopAbs_FACE), 10);
}

// Equivalent to tests/boolean/simplify/A2.
TEST(BRepAlgoAPI_SimplifyTest, A2_SectionThroughSewnShell_SimplifiesEdges)
{
  const TopoDS_Shape aShell = MakeThreeFaceShell();
  const gp_Pln       aCuttingPlane(gp_Pnt(0.0, 5.0, 0.0), gp::DY());
  const TopoDS_Shape aCuttingFace = MakePlaneFace(aCuttingPlane, -100.0, 100.0, -100.0, 100.0);

  BRepAlgoAPI_Section            aSection;
  NCollection_List<TopoDS_Shape> anArguments;
  anArguments.Append(aShell);
  aSection.SetArguments(anArguments);
  NCollection_List<TopoDS_Shape> aTools;
  aTools.Append(aCuttingFace);
  aSection.SetTools(aTools);
  aSection.SetRunParallel(false);
  aSection.Build();

  ASSERT_FALSE(aSection.HasErrors());
  const TopoDS_Shape& aResult = aSection.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(CountShapes(aResult, TopAbs_VERTEX), 4);
  EXPECT_EQ(CountShapes(aResult, TopAbs_EDGE), 3);
  EXPECT_NEAR(GetLength(aResult), 60.0, Precision::Confusion());

  aSection.SimplifyResult(true, true);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(CountShapes(aResult, TopAbs_VERTEX), 2);
  EXPECT_EQ(CountShapes(aResult, TopAbs_EDGE), 1);
  EXPECT_NEAR(GetLength(aResult), 60.0, Precision::Confusion());
}
