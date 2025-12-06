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

#include <gtest/gtest.h>

#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepOffset_MakeOffset.hxx>
#include <BRepOffset_Error.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>

//==================================================================================================
// Helper function to create a circular wire
//==================================================================================================

static TopoDS_Wire MakeCircularWire(const gp_Pnt& theCenter,
                                    const gp_Dir& theNormal,
                                    double        theRadius)
{
  gp_Ax2  anAxis(theCenter, theNormal);
  gp_Circ aCirc(anAxis, theRadius);

  BRepBuilderAPI_MakeEdge anEdgeMaker(aCirc);
  if (!anEdgeMaker.IsDone())
  {
    return TopoDS_Wire();
  }

  BRepBuilderAPI_MakeWire aWireMaker(anEdgeMaker.Edge());
  if (!aWireMaker.IsDone())
  {
    return TopoDS_Wire();
  }

  return aWireMaker.Wire();
}

//==================================================================================================
// Helper function to create a rectangular wire
//==================================================================================================

static TopoDS_Wire MakeRectangularWire(const gp_Pnt& theCenter,
                                       const gp_Dir& theNormal,
                                       double        theWidth,
                                       double        theHeight)
{
  // Create rectangle centered at theCenter in a plane with theNormal
  gp_Ax2 anAxis(theCenter, theNormal);
  gp_Dir aXDir = anAxis.XDirection();
  gp_Dir aYDir = anAxis.YDirection();

  double aHalfW = theWidth / 2.0;
  double aHalfH = theHeight / 2.0;

  gp_Pnt aP1 = theCenter.Translated(gp_Vec(aXDir) * (-aHalfW) + gp_Vec(aYDir) * (-aHalfH));
  gp_Pnt aP2 = theCenter.Translated(gp_Vec(aXDir) * (aHalfW) + gp_Vec(aYDir) * (-aHalfH));
  gp_Pnt aP3 = theCenter.Translated(gp_Vec(aXDir) * (aHalfW) + gp_Vec(aYDir) * (aHalfH));
  gp_Pnt aP4 = theCenter.Translated(gp_Vec(aXDir) * (-aHalfW) + gp_Vec(aYDir) * (aHalfH));

  BRepBuilderAPI_MakeWire aWireMaker;
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aP1, aP2).Edge());
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aP2, aP3).Edge());
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aP3, aP4).Edge());
  aWireMaker.Add(BRepBuilderAPI_MakeEdge(aP4, aP1).Edge());

  if (!aWireMaker.IsDone())
  {
    return TopoDS_Wire();
  }

  return aWireMaker.Wire();
}

//==================================================================================================
// Test: Loft from circle to rectangle - thickness operation
// This test reproduces a regression introduced by commit 44be1230391
// where a "mixed connectivity" error is incorrectly reported for valid loft geometry.
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft)
{
  // Create a circular wire at Z=0
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);
  double aRadius = 50.0;

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, aRadius);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  // Create a rectangular wire at Z=100
  gp_Pnt aRectCenter(0, 0, 100);
  double aWidth  = 80.0;
  double aHeight = 60.0;

  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, aWidth, aHeight);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  // Create loft (thru sections) from circle to rectangle
  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True,   // isSolid
                                        Standard_False); // isRuled
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  // Verify loft shape is valid
  BRepCheck_Analyzer aChecker(aLoftShape);
  EXPECT_TRUE(aChecker.IsValid()) << "Loft shape is not valid";

  // Attempt thickness operation (this is where the regression occurs)
  double                       anOffset = 2.0;
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   anOffset,
                                   1.0e-3, // tolerance
                                   BRepOffset_Skin,
                                   Standard_False, // intersection
                                   Standard_False, // selfInter
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // The operation should succeed - this is valid geometry
  // Regression: commit 44be1230391 causes this to fail with BRepOffset_MixedConnectivity
  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid operation failed. This may be a regression from commit 44be1230391 "
    << "where CheckMixedContinuity incorrectly flags valid loft geometry as having "
    << "mixed connectivity.";

  if (aThickMaker.IsDone())
  {
    TopoDS_Shape aThickShape = aThickMaker.Shape();
    // Note: Shape may be null even if IsDone() is true in some edge cases
    if (!aThickShape.IsNull())
    {
      BRepCheck_Analyzer aThickChecker(aThickShape);
      EXPECT_TRUE(aThickChecker.IsValid()) << "Thick solid shape is not valid";
    }
  }
}

//==================================================================================================
// Test: Loft from circle to rectangle - thickness with intersection mode
// Intersection mode was specifically mentioned in the original bug report (0030055)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft_IntersectionMode)
{
  // Create a circular wire at Z=0
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);
  double aRadius = 50.0;

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, aRadius);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  // Create a rectangular wire at Z=100
  gp_Pnt aRectCenter(0, 0, 100);
  double aWidth  = 80.0;
  double aHeight = 60.0;

  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, aWidth, aHeight);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  // Create loft (thru sections) from circle to rectangle
  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True,   // isSolid
                                        Standard_False); // isRuled
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  // Attempt thickness operation with intersection mode
  double                       anOffset = 2.0;
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   anOffset,
                                   1.0e-3, // tolerance
                                   BRepOffset_Skin,
                                   Standard_True,  // intersection mode
                                   Standard_False, // selfInter
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // The operation should succeed - this is valid geometry
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid operation with intersection mode failed. "
                                    << "This may be a regression from commit 44be1230391.";

  if (aThickMaker.IsDone())
  {
    TopoDS_Shape aThickShape = aThickMaker.Shape();
    // Note: Shape may be null even if IsDone() is true in some edge cases
    if (!aThickShape.IsNull())
    {
      BRepCheck_Analyzer aThickChecker(aThickShape);
      EXPECT_TRUE(aThickChecker.IsValid()) << "Thick solid shape is not valid";
    }
  }
}

//==================================================================================================
// Test: Simple box thickness - should always work (baseline test)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_SimpleBox_Baseline)
{
  // Create a simple rectangular wire
  gp_Pnt aCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aWire = MakeRectangularWire(aCenter, aNormal, 100.0, 100.0);
  ASSERT_FALSE(aWire.IsNull()) << "Failed to create rectangular wire";

  // Extrude to make a box-like solid
  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_True);

  gp_Pnt      aTopCenter(0, 0, 50);
  TopoDS_Wire aTopWire = MakeRectangularWire(aTopCenter, aNormal, 100.0, 100.0);
  ASSERT_FALSE(aTopWire.IsNull()) << "Failed to create top wire";

  aLoftMaker.AddWire(aWire);
  aLoftMaker.AddWire(aTopWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create ruled solid";

  TopoDS_Shape aShape = aLoftMaker.Shape();

  // Thickness operation on ruled surface between two rectangles
  double                       anOffset = 2.0;
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aShape,
                                   aFacesToRemove,
                                   anOffset,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on simple ruled solid failed";

  if (aThickMaker.IsDone())
  {
    TopoDS_Shape aThickShape = aThickMaker.Shape();
    EXPECT_FALSE(aThickShape.IsNull()) << "Thick solid shape is null";
  }
}

//==================================================================================================
// Helper function to create an elliptical wire
//==================================================================================================

static TopoDS_Wire MakeEllipticalWire(const gp_Pnt& theCenter,
                                      const gp_Dir& theNormal,
                                      double        theMajorRadius,
                                      double        theMinorRadius)
{
  gp_Ax2   anAxis(theCenter, theNormal);
  gp_Elips anEllipse(anAxis, theMajorRadius, theMinorRadius);

  BRepBuilderAPI_MakeEdge anEdgeMaker(anEllipse);
  if (!anEdgeMaker.IsDone())
  {
    return TopoDS_Wire();
  }

  BRepBuilderAPI_MakeWire aWireMaker(anEdgeMaker.Edge());
  if (!aWireMaker.IsDone())
  {
    return TopoDS_Wire();
  }

  return aWireMaker.Wire();
}

//==================================================================================================
// Helper function to create a polygonal wire (regular polygon)
//==================================================================================================

static TopoDS_Wire MakePolygonalWire(const gp_Pnt& theCenter,
                                     const gp_Dir& theNormal,
                                     double        theRadius,
                                     int           theNbSides)
{
  gp_Ax2 anAxis(theCenter, theNormal);
  gp_Dir aXDir = anAxis.XDirection();
  gp_Dir aYDir = anAxis.YDirection();

  BRepBuilderAPI_MakePolygon aPolygon;

  for (int i = 0; i <= theNbSides; ++i)
  {
    double anAngle = 2.0 * M_PI * i / theNbSides;
    gp_Pnt aPnt    = theCenter.Translated(gp_Vec(aXDir) * theRadius * cos(anAngle)
                                       + gp_Vec(aYDir) * theRadius * sin(anAngle));
    aPolygon.Add(aPnt);
  }

  aPolygon.Close();

  if (!aPolygon.IsDone())
  {
    return TopoDS_Wire();
  }

  return aPolygon.Wire();
}

//==================================================================================================
// Test: Loft from circle to ellipse - smooth transition, should work
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToEllipseLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      anEllipseCenter(0, 0, 100);
  TopoDS_Wire anEllipseWire = MakeEllipticalWire(anEllipseCenter, aNormal, 60.0, 40.0);
  ASSERT_FALSE(anEllipseWire.IsNull()) << "Failed to create elliptical wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(anEllipseWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Circle to ellipse is a smooth transition - should work
  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid on circle-to-ellipse loft failed. "
    << "This is a smooth transition and should not trigger mixed connectivity.";
}

//==================================================================================================
// Test: Loft from circle to hexagon - sharp corners transition
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToHexagonLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aHexCenter(0, 0, 100);
  TopoDS_Wire aHexWire = MakePolygonalWire(aHexCenter, aNormal, 50.0, 6);
  ASSERT_FALSE(aHexWire.IsNull()) << "Failed to create hexagonal wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aHexWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Circle to hexagon creates transitional geometry
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on circle-to-hexagon loft failed. "
                                    << "This may be affected by the mixed connectivity regression.";
}

//==================================================================================================
// Test: Loft from circle to square - 4 corners, most problematic case
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToSquareLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aSquareCenter(0, 0, 100);
  TopoDS_Wire aSquareWire = MakeRectangularWire(aSquareCenter, aNormal, 80.0, 80.0);
  ASSERT_FALSE(aSquareWire.IsNull()) << "Failed to create square wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aSquareWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Circle to square is similar to circle to rectangle
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on circle-to-square loft failed. "
                                    << "This is a regression from commit 44be1230391.";
}

//==================================================================================================
// Test: Loft from ellipse to rectangle - both have different aspect ratios
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_EllipseToRectangleLoft)
{
  gp_Dir aNormal(0, 0, 1);

  gp_Pnt      anEllipseCenter(0, 0, 0);
  TopoDS_Wire anEllipseWire = MakeEllipticalWire(anEllipseCenter, aNormal, 60.0, 30.0);
  ASSERT_FALSE(anEllipseWire.IsNull()) << "Failed to create elliptical wire";

  gp_Pnt      aRectCenter(0, 0, 100);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 100.0, 50.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(anEllipseWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on ellipse-to-rectangle loft failed. "
                                    << "This may be affected by the mixed connectivity regression.";
}

//==================================================================================================
// Test: Loft with three sections - circle, ellipse, rectangle
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_ThreeSectionLoft)
{
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(gp_Pnt(0, 0, 0), aNormal, 40.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  TopoDS_Wire anEllipseWire = MakeEllipticalWire(gp_Pnt(0, 0, 50), aNormal, 50.0, 35.0);
  ASSERT_FALSE(anEllipseWire.IsNull()) << "Failed to create elliptical wire";

  TopoDS_Wire aRectWire = MakeRectangularWire(gp_Pnt(0, 0, 100), aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(anEllipseWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid on three-section loft failed. "
    << "Complex multi-section lofts are prone to mixed connectivity issues.";
}

//==================================================================================================
// Test: Ruled loft from circle to rectangle (should be less problematic)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft_Ruled)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aRectCenter(0, 0, 100);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  // Use ruled mode (isRuled = true)
  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_True);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create ruled loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();
  ASSERT_FALSE(aLoftShape.IsNull()) << "Loft shape is null";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Ruled lofts produce different surface types and may behave differently
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on ruled circle-to-rectangle loft failed.";
}

//==================================================================================================
// Test: Small offset value - edge case for tolerance
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft_SmallOffset)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aRectCenter(0, 0, 100);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  // Try with a very small offset
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   0.5, // small offset
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid with small offset failed on circle-to-rectangle loft.";
}

//==================================================================================================
// Test: Large offset value - stress test
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft_LargeOffset)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aRectCenter(0, 0, 100);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  // Try with a larger offset
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   10.0, // large offset
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid with large offset failed on circle-to-rectangle loft.";
}

//==================================================================================================
// Test: Negative offset (inward thickness)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangleLoft_NegativeOffset)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aRectCenter(0, 0, 100);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  // Try with negative offset (inward)
  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   -2.0, // negative offset
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone())
    << "ThickSolid with negative offset failed on circle-to-rectangle loft.";
}

//==================================================================================================
// Test: Cylinder thickness - baseline for analytic surfaces (should always work)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_Cylinder_Baseline)
{
  BRepPrimAPI_MakeCylinder aCylMaker(50.0, 100.0);
  TopoDS_Shape             aCylinder = aCylMaker.Shape();
  ASSERT_FALSE(aCylinder.IsNull()) << "Failed to create cylinder";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aCylinder,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Analytic surfaces should always work
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on cylinder failed - this is unexpected.";

  if (aThickMaker.IsDone())
  {
    BRepCheck_Analyzer aChecker(aThickMaker.Shape());
    EXPECT_TRUE(aChecker.IsValid()) << "Thick cylinder is not valid";
  }
}

//==================================================================================================
// Test: Sphere thickness - baseline for analytic surfaces
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_Sphere_Baseline)
{
  BRepPrimAPI_MakeSphere aSphereMaker(50.0);
  TopoDS_Shape           aSphere = aSphereMaker.Shape();
  ASSERT_FALSE(aSphere.IsNull()) << "Failed to create sphere";

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aSphere,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on sphere failed - this is unexpected.";

  if (aThickMaker.IsDone())
  {
    BRepCheck_Analyzer aChecker(aThickMaker.Shape());
    EXPECT_TRUE(aChecker.IsValid()) << "Thick sphere is not valid";
  }
}

//==================================================================================================
// Test: Fused box and cylinder - mixed analytic surfaces
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_FusedBoxCylinder)
{
  BRepPrimAPI_MakeBox aBoxMaker(100.0, 100.0, 50.0);
  TopoDS_Shape        aBox = aBoxMaker.Shape();
  ASSERT_FALSE(aBox.IsNull()) << "Failed to create box";

  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(50, 50, 50), gp_Dir(0, 0, 1)), 30.0, 50.0);
  TopoDS_Shape             aCyl = aCylMaker.Shape();
  ASSERT_FALSE(aCyl.IsNull()) << "Failed to create cylinder";

  BRepAlgoAPI_Fuse aFuser(aBox, aCyl);
  ASSERT_TRUE(aFuser.IsDone()) << "Failed to fuse shapes";

  TopoDS_Shape aFusedShape = aFuser.Shape();

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aFusedShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Fused analytic surfaces should work
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on fused box+cylinder failed.";
}

//==================================================================================================
// Test: Very thin loft (small height) - edge case
// This tests that the algorithm doesn't crash on very thin lofts where edges may not
// have valid PCurves on all faces.
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangle_ThinLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  // Very small height - tests edge case behavior
  gp_Pnt      aRectCenter(0, 0, 10);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create thin loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  // The operation may or may not succeed on very thin geometry,
  // but it must not crash
  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   1.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // We don't require IsDone() to be true for very thin lofts,
  // but the algorithm must not crash. Shape validity is not guaranteed
  // for extreme edge cases like very thin geometry.
  // The primary goal of this test is to verify no crash occurs.
  (void)aThickMaker.IsDone(); // Suppress unused warning
}

//==================================================================================================
// Test: Tall loft (large height) - edge case
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToRectangle_TallLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  // Very large height
  gp_Pnt      aRectCenter(0, 0, 500);
  TopoDS_Wire aRectWire = MakeRectangularWire(aRectCenter, aNormal, 80.0, 60.0);
  ASSERT_FALSE(aRectWire.IsNull()) << "Failed to create rectangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aRectWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create tall loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on tall circle-to-rectangle loft failed.";
}

//==================================================================================================
// Test: Circle to triangle - most extreme corner case (3 corners)
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToTriangleLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      aTriCenter(0, 0, 100);
  TopoDS_Wire aTriWire = MakePolygonalWire(aTriCenter, aNormal, 60.0, 3);
  ASSERT_FALSE(aTriWire.IsNull()) << "Failed to create triangular wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(aTriWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Triangle has sharp 60-degree corners
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on circle-to-triangle loft failed. "
                                    << "Triangle corners create extreme curvature transitions.";
}

//==================================================================================================
// Test: Circle to octagon - many corners but more gradual
//==================================================================================================

TEST(BRepOffset_MakeOffsetTest, ThickSolid_CircleToOctagonLoft)
{
  gp_Pnt aCircleCenter(0, 0, 0);
  gp_Dir aNormal(0, 0, 1);

  TopoDS_Wire aCircleWire = MakeCircularWire(aCircleCenter, aNormal, 50.0);
  ASSERT_FALSE(aCircleWire.IsNull()) << "Failed to create circular wire";

  gp_Pnt      anOctCenter(0, 0, 100);
  TopoDS_Wire anOctWire = MakePolygonalWire(anOctCenter, aNormal, 55.0, 8);
  ASSERT_FALSE(anOctWire.IsNull()) << "Failed to create octagonal wire";

  BRepOffsetAPI_ThruSections aLoftMaker(Standard_True, Standard_False);
  aLoftMaker.AddWire(aCircleWire);
  aLoftMaker.AddWire(anOctWire);
  aLoftMaker.Build();

  ASSERT_TRUE(aLoftMaker.IsDone()) << "Failed to create loft";

  TopoDS_Shape aLoftShape = aLoftMaker.Shape();

  TopTools_ListOfShape         aFacesToRemove;
  BRepOffsetAPI_MakeThickSolid aThickMaker;

  aThickMaker.MakeThickSolidByJoin(aLoftShape,
                                   aFacesToRemove,
                                   2.0,
                                   1.0e-3,
                                   BRepOffset_Skin,
                                   Standard_False,
                                   Standard_False,
                                   GeomAbs_Intersection);
  aThickMaker.Build();

  // Octagon is closer to a circle, corners are less sharp
  EXPECT_TRUE(aThickMaker.IsDone()) << "ThickSolid on circle-to-octagon loft failed.";
}
