// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Vec.hxx>

#include <cmath>

#include <gtest/gtest.h>

TEST(BRepOffsetAPI_MakeThickSolidTest, HollowBox)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Get the first face to remove.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFaceToRemove = TopoDS::Face(anExp.Current());

  NCollection_List<TopoDS_Shape> aFacesToRemove;
  aFacesToRemove.Append(aFaceToRemove);

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidByJoin(aBox, aFacesToRemove, -1.0, Precision::Confusion());
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  EXPECT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

TEST(BRepOffsetAPI_MakeThickSolidTest, HollowBoxVolume)
{
  BRepPrimAPI_MakeBox aBoxMaker(20.0, 20.0, 20.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  GProp_GProps aOrigProps;
  BRepGProp::VolumeProperties(aBox, aOrigProps);
  const double anOrigVolume = aOrigProps.Mass();

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFaceToRemove = TopoDS::Face(anExp.Current());

  NCollection_List<TopoDS_Shape> aFacesToRemove;
  aFacesToRemove.Append(aFaceToRemove);

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidByJoin(aBox, aFacesToRemove, -1.0, Precision::Confusion());
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aHollowProps;
  BRepGProp::VolumeProperties(aResult, aHollowProps);
  const double aHollowVolume = aHollowProps.Mass();

  // Hollowed box must have less volume than the original solid.
  EXPECT_LT(aHollowVolume, anOrigVolume);
}

TEST(BRepOffsetAPI_MakeThickSolidTest, MakeThickSolidBySimple)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Extract a single face (non-closed shell) for MakeThickSolidBySimple.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidBySimple(aFace, 2.0);
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  EXPECT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer aChecker(aResult);
  EXPECT_TRUE(aChecker.IsValid());
}

TEST(BRepOffsetAPI_MakeThickSolidTest, ThickSolidLargerVolume)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aBoxMaker.Shape();
  ASSERT_TRUE(aBoxMaker.IsDone());

  // Extract a single face for MakeThickSolidBySimple.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  TopoDS_Face aFace = TopoDS::Face(anExp.Current());

  // Compute the area of the original face to estimate expected volume.
  GProp_GProps aFaceProps;
  BRepGProp::SurfaceProperties(aFace, aFaceProps);
  const double aFaceArea = aFaceProps.Mass();

  const double                 anOffset = 2.0;
  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidBySimple(aFace, anOffset);
  aMaker.Build();

  ASSERT_TRUE(aMaker.IsDone());

  const TopoDS_Shape& aResult = aMaker.Shape();
  ASSERT_FALSE(aResult.IsNull());

  GProp_GProps aResultProps;
  BRepGProp::VolumeProperties(aResult, aResultProps);
  const double aResultVolume = std::abs(aResultProps.Mass());

  // The thickened face should produce a solid with volume > 0.
  // A planar face of area A thickened by offset d produces volume approximately A*d.
  EXPECT_GT(aResultVolume, aFaceArea * anOffset * 0.5);
}

namespace
{
// Bosses of radius 18 at the corners of a triangle with circumradius 24,
// joined by their tangent lines, 25 high, with a through hole of radius 13 in each.
const double THE_BOSS_RADIUS = 18.0;
const double THE_HOLE_RADIUS = 13.0;
const double THE_HEIGHT      = 25.0;

const gp_Pnt THE_CENTERS[3] = {gp_Pnt(-12.0 * std::sqrt(3.0), -12.0, 0.0),
                               gp_Pnt(12.0 * std::sqrt(3.0), -12.0, 0.0),
                               gp_Pnt(0.0, 24.0, 0.0)};

// Prism of the triangle of centres offset by theRadius (rounded at the corners), height THE_HEIGHT.
TopoDS_Shape makeRoundedTriangle(const double theRadius)
{
  BRepBuilderAPI_MakePolygon aPolygon(THE_CENTERS[0], THE_CENTERS[1], THE_CENTERS[2], true);
  const TopoDS_Face          aTriangle = BRepBuilderAPI_MakeFace(aPolygon.Wire(), true);
  BRepOffsetAPI_MakeOffset   anOffset(aTriangle, GeomAbs_Arc);
  anOffset.Perform(theRadius);
  const TopoDS_Face aProfile = BRepBuilderAPI_MakeFace(TopoDS::Wire(anOffset.Shape()), true);
  return BRepPrimAPI_MakePrism(aProfile, gp_Vec(0.0, 0.0, THE_HEIGHT)).Shape();
}

// theShape minus a cylinder of radius theRadius around each centre, through its height.
TopoDS_Shape cutCylinders(const TopoDS_Shape& theShape, const double theRadius)
{
  TopoDS_Shape aResult = theShape;
  for (const gp_Pnt& aCenter : THE_CENTERS)
  {
    const gp_Ax2 anAxis(aCenter.Translated(gp_Vec(0.0, 0.0, -1.0)), gp::DZ());
    aResult = BRepAlgoAPI_Cut(aResult,
                              BRepPrimAPI_MakeCylinder(anAxis, theRadius, THE_HEIGHT + 2.0).Shape())
                .Shape();
  }
  return aResult;
}

double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

int nbSolids(const TopoDS_Shape& theShape)
{
  int aNb = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_SOLID); anExp.More(); anExp.Next())
  {
    ++aNb;
  }
  return aNb;
}

// The bosses minus the pocket the shell leaves: the triangle offset by the boss radius
// minus the thickness, outside the hole walls thickened by the thickness.
double expectedShellVolume(const TopoDS_Shape& theSolid, const double theThickness)
{
  const TopoDS_Shape aPocket = cutCylinders(makeRoundedTriangle(THE_BOSS_RADIUS - theThickness),
                                            THE_HOLE_RADIUS + theThickness);
  return volumeOf(theSolid) - volumeOf(aPocket);
}

// Shells the bosses with their top and bottom faces removed, and checks the result. Without
// those faces, the outer wall and the wall of each hole share no edge.
void checkShellOpenAtBothEnds(const TopoDS_Shape& theSolid,
                              const double        theThickness,
                              const bool          theIntersection,
                              const int           theNbSolids)
{
  SCOPED_TRACE(testing::Message() << "thickness " << theThickness << ", intersection "
                                  << theIntersection);
  NCollection_List<TopoDS_Shape> aFacesToRemove;
  for (TopExp_Explorer anExp(theSolid, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face&        aFace = TopoDS::Face(anExp.Current());
    const BRepAdaptor_Surface aSurf(aFace);
    if (aSurf.GetType() == GeomAbs_Plane
        && aSurf.Plane().Axis().Direction().IsParallel(gp::DZ(), Precision::Angular()))
    {
      aFacesToRemove.Append(aFace);
    }
  }
  ASSERT_EQ(aFacesToRemove.Extent(), 2);

  BRepOffsetAPI_MakeThickSolid aMaker;
  aMaker.MakeThickSolidByJoin(theSolid,
                              aFacesToRemove,
                              -theThickness,
                              1.e-3,
                              BRepOffset_Skin,
                              theIntersection);
  ASSERT_TRUE(aMaker.IsDone());
  const TopoDS_Shape& aResult = aMaker.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
  EXPECT_EQ(nbSolids(aResult), theNbSolids);
  EXPECT_NEAR(volumeOf(aResult), expectedShellVolume(theSolid, theThickness), 1.e-3);
}
} // namespace

// Below half the wall between a hole and the outside (5), the wall of each hole
// and the outer wall stay apart: the result is four solids.
TEST(BRepOffsetAPI_MakeThickSolidTest, OpenAtBothEnds_WallsApart)
{
  const TopoDS_Shape aSolid = cutCylinders(makeRoundedTriangle(THE_BOSS_RADIUS), THE_HOLE_RADIUS);
  ASSERT_TRUE(BRepCheck_Analyzer(aSolid).IsValid());

  for (const double aThickness : {1.0, 2.0})
  {
    for (const bool anIntersection : {false, true})
    {
      checkShellOpenAtBothEnds(aSolid, aThickness, anIntersection, 4);
    }
  }
}

// Above it, each hole's wall merges with the outer wall: one solid, with three
// pockets between the holes. The offset walls cross, so this needs the intersection mode.
TEST(BRepOffsetAPI_MakeThickSolidTest, OpenAtBothEnds_WallsMerge)
{
  const TopoDS_Shape aSolid = cutCylinders(makeRoundedTriangle(THE_BOSS_RADIUS), THE_HOLE_RADIUS);
  ASSERT_TRUE(BRepCheck_Analyzer(aSolid).IsValid());

  checkShellOpenAtBothEnds(aSolid, 3.0, true, 1);
}
