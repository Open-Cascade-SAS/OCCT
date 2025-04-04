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

#include <StepTidy_DirectionReducer.pxx>

#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

class StepTidy_DirectionReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  TColStd_MapOfTransient replaceDuplicateDirections()
  {
    StepTidy_DirectionReducer aReducer(myWS);
    for (Standard_Integer anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    TColStd_MapOfTransient aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that directions with the same coordinates and different names are not merged.
TEST_F(StepTidy_DirectionReducerTest, DifferentNames)
{
  // Creating directions.
  Handle(StepGeom_Direction) aDir1 = addDirection("dir1");
  Handle(StepGeom_Direction) aDir2 = addDirection("dir2");

  // Creating vector containing the first direction.
  Handle(StepGeom_Vector) aFirstVector = new StepGeom_Vector;
  aFirstVector->Init(new TCollection_HAsciiString, aDir1, 1.);
  addToModel(aFirstVector);

  // Creating vector containing the second direction.
  Handle(StepGeom_Vector) aSecondVector = new StepGeom_Vector;
  aSecondVector->Init(new TCollection_HAsciiString, aDir2, 1.);
  addToModel(aSecondVector);

  // Performing removal of duplicate directions.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateDirections();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that directions with the same coordinates and same names are
// merged for StepGeom_Axis1Placement.
TEST_F(StepTidy_DirectionReducerTest, StepGeom_Axis1Placement)
{
  // Creating directions.
  Handle(StepGeom_Direction) aDir1 = addDirection();
  Handle(StepGeom_Direction) aDir2 = addDirection();

  // Creating Cartesian point for the location.
  Handle(StepGeom_CartesianPoint) aLocation       = new StepGeom_CartesianPoint;
  Handle(TColStd_HArray1OfReal)   aLocationCoords = new TColStd_HArray1OfReal(1, 3);
  aLocationCoords->SetValue(1, 0.);
  aLocationCoords->SetValue(2, 0.);
  aLocationCoords->SetValue(3, 0.);
  aLocation->Init(new TCollection_HAsciiString, aLocationCoords);
  addToModel(aLocation);

  // Creating axis containing the first direction.
  Handle(StepGeom_Axis1Placement) aFirstAxis = new StepGeom_Axis1Placement;
  aFirstAxis->Init(new TCollection_HAsciiString, aLocation, true, aDir1);
  addToModel(aFirstAxis);

  // Creating axis containing the second direction.
  Handle(StepGeom_Axis1Placement) aSecondAxis = new StepGeom_Axis1Placement;
  aSecondAxis->Init(new TCollection_HAsciiString, aLocation, true, aDir2);
  addToModel(aSecondAxis);

  // Performing removal of duplicate Cartesian points.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateDirections();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aDir1) || aRemovedEntities.Contains(aDir2));
}

// Check that directions with the same coordinates and same names are
// merged for StepGeom_Axis2Placement.
TEST_F(StepTidy_DirectionReducerTest, StepGeom_Axis2Placement)
{
  // Creating directions.
  Handle(StepGeom_Direction) aDir1 = addDirection();
  Handle(StepGeom_Direction) aDir2 = addDirection();

  // Creating Cartesian point for the location.
  Handle(StepGeom_CartesianPoint) aLocation       = new StepGeom_CartesianPoint;
  Handle(TColStd_HArray1OfReal)   aLocationCoords = new TColStd_HArray1OfReal(1, 3);
  aLocationCoords->SetValue(1, 0.);
  aLocationCoords->SetValue(2, 0.);
  aLocationCoords->SetValue(3, 0.);
  aLocation->Init(new TCollection_HAsciiString, aLocationCoords);
  addToModel(aLocation);

  // Creating axis containing the first direction.
  Handle(StepGeom_Axis2Placement3d) aFirstAxis = new StepGeom_Axis2Placement3d;
  aFirstAxis->Init(new TCollection_HAsciiString, aLocation, true, aDir1, false, nullptr);
  addToModel(aFirstAxis);

  // Creating axis containing the second direction.
  Handle(StepGeom_Axis2Placement3d) aSecondAxis = new StepGeom_Axis2Placement3d;
  aSecondAxis->Init(new TCollection_HAsciiString, aLocation, true, aDir2, false, nullptr);
  addToModel(aSecondAxis);

  // Performing removal of duplicate Cartesian points.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateDirections();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aDir1) || aRemovedEntities.Contains(aDir2));
}

// Check that points with the same coordinates and same names are
// merged for StepGeom_Vector.
TEST_F(StepTidy_DirectionReducerTest, StepGeom_Vector)
{
  // Creating directions.
  Handle(StepGeom_Direction) aDir1 = addDirection();
  Handle(StepGeom_Direction) aDir2 = addDirection();

  // Creating vector containing the first direction.
  Handle(StepGeom_Vector) aFirstVector = new StepGeom_Vector;
  aFirstVector->Init(new TCollection_HAsciiString, aDir1, 1.);
  addToModel(aFirstVector);

  // Creating vector containing the second direction.
  Handle(StepGeom_Vector) aSecondVector = new StepGeom_Vector;
  aSecondVector->Init(new TCollection_HAsciiString, aDir2, 1.);
  addToModel(aSecondVector);

  // Performing removal of duplicate Cartesian points.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateDirections();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aDir1) || aRemovedEntities.Contains(aDir2));
}
