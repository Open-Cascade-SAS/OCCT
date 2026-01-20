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

#include "StepTidy_BaseTestFixture.pxx"

#include <StepTidy_CartesianPointReducer.pxx>

#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_RationalBSplineSurface.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_StyledItem.hxx>

class StepTidy_CartesianPointReducerTest : public StepTidy_BaseTestFixture
{
protected:
  // Perform removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> replaceDuplicateCartesianPoints()
  {
    StepTidy_CartesianPointReducer aReducer(myWS);
    for (int anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that points with the same coordinates and different names are not merged.
TEST_F(StepTidy_CartesianPointReducerTest, DifferentNames)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint("FirstPt");
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint("SecondPt");

  // Creating direction.
  occ::handle<NCollection_HArray1<double>> aDirCoords = new NCollection_HArray1<double>(1, 3);
  aDirCoords->SetValue(1, 0.);
  aDirCoords->SetValue(2, 0.);
  aDirCoords->SetValue(3, 1.);
  occ::handle<StepGeom_Direction> aDir = new StepGeom_Direction;
  aDir->Init(new TCollection_HAsciiString, aDirCoords);
  addToModel(aDir);

  // Creating axis containing the first Cartesian point.
  occ::handle<StepGeom_Axis2Placement3d> aFirstAxis = new StepGeom_Axis2Placement3d;
  aFirstAxis
    ->Init(new TCollection_HAsciiString, aPt1, true, aDir, false, nullptr);
  addToModel(aFirstAxis);

  // Creating axis containing the second Cartesian point.
  occ::handle<StepGeom_Axis2Placement3d> aSecondAxis = new StepGeom_Axis2Placement3d;
  aSecondAxis
    ->Init(new TCollection_HAsciiString, aPt2, true, aDir, false, nullptr);
  addToModel(aSecondAxis);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that points with the same coordinates and same names are
// merged for StepGeom_Axis2Placement3d.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_Axis2Placement3d)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating direction.
  occ::handle<NCollection_HArray1<double>> aDirCoords = new NCollection_HArray1<double>(1, 3);
  aDirCoords->SetValue(1, 0.);
  aDirCoords->SetValue(2, 0.);
  aDirCoords->SetValue(3, 1.);
  occ::handle<StepGeom_Direction> aDir = new StepGeom_Direction;
  aDir->Init(new TCollection_HAsciiString, aDirCoords);
  addToModel(aDir);

  // Creating axis containing the first Cartesian point.
  occ::handle<StepGeom_Axis2Placement3d> aFirstAxis = new StepGeom_Axis2Placement3d;
  aFirstAxis
    ->Init(new TCollection_HAsciiString, aPt1, true, aDir, false, nullptr);
  addToModel(aFirstAxis);

  // Creating axis containing the second Cartesian point.
  occ::handle<StepGeom_Axis2Placement3d> aSecondAxis = new StepGeom_Axis2Placement3d;
  aSecondAxis
    ->Init(new TCollection_HAsciiString, aPt2, true, aDir, false, nullptr);
  addToModel(aSecondAxis);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepShape_VertexPoint.
TEST_F(StepTidy_CartesianPointReducerTest, StepShape_VertexPoint)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating vertex containing the first Cartesian point.
  occ::handle<StepShape_VertexPoint> aFirstVertex = new StepShape_VertexPoint;
  aFirstVertex->Init(new TCollection_HAsciiString, aPt1);
  addToModel(aFirstVertex);

  // Creating vertex containing the second Cartesian point.
  occ::handle<StepShape_VertexPoint> aSecondVertex = new StepShape_VertexPoint;
  aSecondVertex->Init(new TCollection_HAsciiString, aPt2);
  addToModel(aSecondVertex);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepShape_GeometricCurveSet.
TEST_F(StepTidy_CartesianPointReducerTest, StepShape_GeometricCurveSet)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating curve set containing the first Cartesian point.
  occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aFirstElements =
    new NCollection_HArray1<StepShape_GeometricSetSelect>(1, 1);
  StepShape_GeometricSetSelect aFirstSelect;
  aFirstSelect.SetValue(aPt1);
  aFirstElements->SetValue(1, aFirstSelect);
  addToModel(aFirstElements);
  occ::handle<StepShape_GeometricCurveSet> aFirstCurveSet = new StepShape_GeometricCurveSet;
  aFirstCurveSet->Init(new TCollection_HAsciiString, aFirstElements);
  addToModel(aFirstCurveSet);

  // Creating curve set containing the second Cartesian point.
  occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> aSecondElements =
    new NCollection_HArray1<StepShape_GeometricSetSelect>(1, 1);
  StepShape_GeometricSetSelect aSecondSelect;
  aSecondSelect.SetValue(aPt2);
  aSecondElements->SetValue(1, aSecondSelect);
  addToModel(aSecondElements);
  occ::handle<StepShape_GeometricCurveSet> aSecondCurveSet = new StepShape_GeometricCurveSet;
  aSecondCurveSet->Init(new TCollection_HAsciiString, aSecondElements);
  addToModel(aSecondCurveSet);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepVisual_PresentationLayerAssignment.
TEST_F(StepTidy_CartesianPointReducerTest, StepVisual_PresentationLayerAssignment)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating presentation layer assignment containing the first Cartesian point.
  occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> aFirstAssignedItems =
    new NCollection_HArray1<StepVisual_LayeredItem>(1, 1);
  StepVisual_LayeredItem aFirstLayeredItem;
  aFirstLayeredItem.SetValue(aPt1);
  aFirstAssignedItems->SetValue(1, aFirstLayeredItem);
  addToModel(aFirstAssignedItems);
  occ::handle<StepVisual_PresentationLayerAssignment> aFirstAssignment =
    new StepVisual_PresentationLayerAssignment;
  aFirstAssignment->Init(new TCollection_HAsciiString,
                         new TCollection_HAsciiString,
                         aFirstAssignedItems);
  addToModel(aFirstAssignment);

  // Creating presentation layer assignment containing the second Cartesian point.
  occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> aSecondAssignedItems =
    new NCollection_HArray1<StepVisual_LayeredItem>(1, 1);
  StepVisual_LayeredItem aSecondLayeredItem;
  aSecondLayeredItem.SetValue(aPt2);
  aSecondAssignedItems->SetValue(1, aSecondLayeredItem);
  addToModel(aSecondAssignedItems);
  occ::handle<StepVisual_PresentationLayerAssignment> aSecondAssignment =
    new StepVisual_PresentationLayerAssignment;
  aSecondAssignment->Init(new TCollection_HAsciiString,
                          new TCollection_HAsciiString,
                          aSecondAssignedItems);
  addToModel(aSecondAssignment);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepVisual_StyledItem.
TEST_F(StepTidy_CartesianPointReducerTest, StepVisual_StyledItem)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating styled item containing the first Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aFirstAssignedItems =
    new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, 1);
  occ::handle<StepVisual_StyledItem> aFirstStyledItem = new StepVisual_StyledItem;
  aFirstStyledItem->Init(new TCollection_HAsciiString, aFirstAssignedItems, aPt1);
  addToModel(aFirstStyledItem);

  // Creating styled item containing the second Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>> aSecondAssignedItems =
    new NCollection_HArray1<occ::handle<StepVisual_PresentationStyleAssignment>>(1, 1);
  occ::handle<StepVisual_StyledItem> aSecondStyledItem = new StepVisual_StyledItem;
  aSecondStyledItem->Init(new TCollection_HAsciiString, aSecondAssignedItems, aPt2);
  addToModel(aSecondStyledItem);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_BSplineCurveWithKnots.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_BSplineCurveWithKnots)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating curve containing the first Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aFirstControlPoints =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 1);
  aFirstControlPoints->SetValue(1, aPt1);
  occ::handle<StepGeom_BSplineCurveWithKnots> aFirstCurve = new StepGeom_BSplineCurveWithKnots;
  aFirstCurve->Init(new TCollection_HAsciiString,
                    1,
                    aFirstControlPoints,
                    StepGeom_bscfUnspecified,
                    StepData_LUnknown,
                    StepData_LUnknown,
                    new NCollection_HArray1<int>,
                    new NCollection_HArray1<double>,
                    StepGeom_ktUnspecified);
  addToModel(aFirstCurve);

  // Creating curve containing the second Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aSecondControlPoints =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 1);
  aSecondControlPoints->SetValue(1, aPt2);
  occ::handle<StepGeom_BSplineCurveWithKnots> aSecondCurve = new StepGeom_BSplineCurveWithKnots;
  aSecondCurve->Init(new TCollection_HAsciiString,
                     1,
                     aSecondControlPoints,
                     StepGeom_bscfUnspecified,
                     StepData_LUnknown,
                     StepData_LUnknown,
                     new NCollection_HArray1<int>,
                     new NCollection_HArray1<double>,
                     StepGeom_ktUnspecified);
  addToModel(aSecondCurve);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_Line.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_Line)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating line containing the first Cartesian point.
  occ::handle<StepGeom_Line> aFirstLine = new StepGeom_Line;
  aFirstLine->Init(new TCollection_HAsciiString, aPt1, new StepGeom_Vector);
  addToModel(aFirstLine);

  // Creating line containing the second Cartesian point.
  occ::handle<StepGeom_Line> aSecondLine = new StepGeom_Line;
  aSecondLine->Init(new TCollection_HAsciiString, aPt2, new StepGeom_Vector);
  addToModel(aSecondLine);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_BSplineSurfaceWithKnots.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_BSplineSurfaceWithKnots)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating surface containing the first Cartesian point.
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aFirstControlPoints =
    new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, 1, 1, 1);
  aFirstControlPoints->SetValue(1, 1, aPt1);
  occ::handle<StepGeom_BSplineSurfaceWithKnots> aFirstSurface = new StepGeom_BSplineSurfaceWithKnots;
  aFirstSurface->Init(new TCollection_HAsciiString,
                      1,
                      1,
                      aFirstControlPoints,
                      StepGeom_bssfUnspecified,
                      StepData_LUnknown,
                      StepData_LUnknown,
                      StepData_LUnknown,
                      new NCollection_HArray1<int>,
                      new NCollection_HArray1<int>,
                      new NCollection_HArray1<double>,
                      new NCollection_HArray1<double>,
                      StepGeom_ktUnspecified);
  addToModel(aFirstSurface);

  // Creating surface containing the second Cartesian point.
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aSecondControlPoints =
    new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, 1, 1, 1);
  aSecondControlPoints->SetValue(1, 1, aPt2);
  occ::handle<StepGeom_BSplineSurfaceWithKnots> aSecondSurface = new StepGeom_BSplineSurfaceWithKnots;
  aSecondSurface->Init(new TCollection_HAsciiString,
                       1,
                       1,
                       aSecondControlPoints,
                       StepGeom_bssfUnspecified,
                       StepData_LUnknown,
                       StepData_LUnknown,
                       StepData_LUnknown,
                       new NCollection_HArray1<int>,
                       new NCollection_HArray1<int>,
                       new NCollection_HArray1<double>,
                       new NCollection_HArray1<double>,
                       StepGeom_ktUnspecified);
  addToModel(aSecondSurface);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_Axis1Placement.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_Axis1Placement)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating axis containing the first Cartesian point.
  occ::handle<StepGeom_Axis1Placement> aFirstAxis = new StepGeom_Axis1Placement;
  aFirstAxis->Init(new TCollection_HAsciiString, aPt1, false, new StepGeom_Direction);
  addToModel(aFirstAxis);

  // Creating axis containing the second Cartesian point.
  occ::handle<StepGeom_Axis1Placement> aSecondAxis = new StepGeom_Axis1Placement;
  aSecondAxis->Init(new TCollection_HAsciiString, aPt2, false, new StepGeom_Direction);
  addToModel(aSecondAxis);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepRepr_Representation.
TEST_F(StepTidy_CartesianPointReducerTest, StepRepr_Representation)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating representation containing the first Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aFirstItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aFirstItems->SetValue(1, aPt1);
  occ::handle<StepRepr_Representation> aFirstRepresentation = new StepRepr_Representation;
  aFirstRepresentation->Init(new TCollection_HAsciiString,
                             aFirstItems,
                             new StepRepr_RepresentationContext);
  addToModel(aFirstRepresentation);

  // Creating representation containing the second Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aSecondItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aSecondItems->SetValue(1, aPt2);
  occ::handle<StepRepr_Representation> aSecondRepresentation = new StepRepr_Representation;
  aSecondRepresentation->Init(new TCollection_HAsciiString,
                              aSecondItems,
                              new StepRepr_RepresentationContext);
  addToModel(aSecondRepresentation);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.
TEST_F(StepTidy_CartesianPointReducerTest, StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating curve containing the first Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aFirstControlPoints =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 1);
  aFirstControlPoints->SetValue(1, aPt1);
  occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve> aFirstCurve =
    new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
  aFirstCurve->Init(new TCollection_HAsciiString,
                    1,
                    aFirstControlPoints,
                    StepGeom_bscfUnspecified,
                    StepData_LUnknown,
                    StepData_LUnknown,
                    new NCollection_HArray1<int>,
                    new NCollection_HArray1<double>,
                    StepGeom_ktUnspecified,
                    new NCollection_HArray1<double>);
  addToModel(aFirstCurve);

  // Creating curve containing the second Cartesian point.
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aSecondControlPoints =
    new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, 1);
  aSecondControlPoints->SetValue(1, aPt2);
  occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve> aSecondCurve =
    new StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve;
  aSecondCurve->Init(new TCollection_HAsciiString,
                     1,
                     aSecondControlPoints,
                     StepGeom_bscfUnspecified,
                     StepData_LUnknown,
                     StepData_LUnknown,
                     new NCollection_HArray1<int>,
                     new NCollection_HArray1<double>,
                     StepGeom_ktUnspecified,
                     new NCollection_HArray1<double>);
  addToModel(aSecondCurve);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}

// Check that points with the same coordinates and same names are merged
// for StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.
TEST_F(StepTidy_CartesianPointReducerTest,
       StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)
{
  // Creating Cartesian points.
  occ::handle<StepGeom_CartesianPoint> aPt1 = addCartesianPoint();
  occ::handle<StepGeom_CartesianPoint> aPt2 = addCartesianPoint();

  // Creating rational BSpline surface to use.
  occ::handle<StepGeom_RationalBSplineSurface> aRationalBSplineSurface =
    new StepGeom_RationalBSplineSurface;
  aRationalBSplineSurface->Init(new TCollection_HAsciiString,
                                1,
                                1,
                                new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, 1, 1, 1),
                                StepGeom_bssfUnspecified,
                                StepData_LUnknown,
                                StepData_LUnknown,
                                StepData_LUnknown,
                                new NCollection_HArray2<double>(1, 1, 1, 1));

  // Creating surface containing the first Cartesian point.
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aFirstControlPoints =
    new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, 1, 1, 1);
  aFirstControlPoints->SetValue(1, 1, aPt1);
  occ::handle<StepGeom_BSplineSurfaceWithKnots> aFirstBSSWN = new StepGeom_BSplineSurfaceWithKnots;
  aFirstBSSWN->Init(new TCollection_HAsciiString,
                    1,
                    1,
                    aFirstControlPoints,
                    StepGeom_bssfUnspecified,
                    StepData_LUnknown,
                    StepData_LUnknown,
                    StepData_LUnknown,
                    new NCollection_HArray1<int>,
                    new NCollection_HArray1<int>,
                    new NCollection_HArray1<double>,
                    new NCollection_HArray1<double>,
                    StepGeom_ktUnspecified);
  addToModel(aFirstBSSWN);
  occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> aFirstSurface =
    new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
  aFirstSurface->Init(new TCollection_HAsciiString,
                      1,
                      1,
                      aFirstControlPoints,
                      StepGeom_bssfUnspecified,
                      StepData_LUnknown,
                      StepData_LUnknown,
                      StepData_LUnknown,
                      aFirstBSSWN,
                      aRationalBSplineSurface);
  addToModel(aFirstSurface);

  // Creating surface containing the second Cartesian point.
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aSecondControlPoints =
    new NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>(1, 1, 1, 1);
  aSecondControlPoints->SetValue(1, 1, aPt2);
  occ::handle<StepGeom_BSplineSurfaceWithKnots> aSecondBSSWN = new StepGeom_BSplineSurfaceWithKnots;
  aSecondBSSWN->Init(new TCollection_HAsciiString,
                     1,
                     1,
                     aSecondControlPoints,
                     StepGeom_bssfUnspecified,
                     StepData_LUnknown,
                     StepData_LUnknown,
                     StepData_LUnknown,
                     new NCollection_HArray1<int>,
                     new NCollection_HArray1<int>,
                     new NCollection_HArray1<double>,
                     new NCollection_HArray1<double>,
                     StepGeom_ktUnspecified);
  addToModel(aSecondBSSWN);
  occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> aSecondSurface =
    new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
  aSecondSurface->Init(new TCollection_HAsciiString,
                       1,
                       1,
                       aSecondControlPoints,
                       StepGeom_bssfUnspecified,
                       StepData_LUnknown,
                       StepData_LUnknown,
                       StepData_LUnknown,
                       aSecondBSSWN,
                       aRationalBSplineSurface);

  // Performing removal of duplicate Cartesian points.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateCartesianPoints();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPt1) || aRemovedEntities.Contains(aPt2));
}
