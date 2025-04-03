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

#include <StepTidy_VectorReducer.pxx>

#include <StepGeom_Line.hxx>

class StepTidy_VectorReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  TColStd_MapOfTransient replaceDuplicateVectors()
  {
    StepTidy_VectorReducer aReducer(myWS);
    for (Standard_Integer anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    TColStd_MapOfTransient aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Vectors with the same coordinates and different names are not merged.
TEST_F(StepTidy_VectorReducerTest, DifferentNames)
{
  // Creating Vectors.
  Handle(StepGeom_Vector) aVec1 = addVector("vec1");
  Handle(StepGeom_Vector) aVec2 = addVector("vec2");

  // Creating a cartesian point for the lines.
  Handle(StepGeom_CartesianPoint) aPnt = addCartesianPoint(nullptr, {0., 0., 0.});

  // Creating aLine containing the first Vector.
  Handle(StepGeom_Line) aLine1 = new StepGeom_Line;
  aLine1->Init(new TCollection_HAsciiString, aPnt, aVec1);
  addToModel(aLine1);

  // Creating aLine containing the second Vector.
  Handle(StepGeom_Line) aLine2 = new StepGeom_Line;
  aLine2->Init(new TCollection_HAsciiString, aPnt, aVec2);
  addToModel(aLine2);

  // Performing removal of duplicate Vectors.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateVectors();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that Vectors with the same coordinates and same names are
// merged for StepGeom_Axis1Placement.
TEST_F(StepTidy_VectorReducerTest, StepGeom_Line)
{
  // Creating Vectors.
  Handle(StepGeom_Vector) aVec1 = addVector();
  Handle(StepGeom_Vector) aVec2 = addVector();

  // Creating a cartesian point for the lines.
  Handle(StepGeom_CartesianPoint) aPnt = addCartesianPoint(nullptr, {0., 0., 0.});

  // Creating aLine containing the first Vector.
  Handle(StepGeom_Line) aLine1 = new StepGeom_Line;
  aLine1->Init(new TCollection_HAsciiString, aPnt, aVec1);
  addToModel(aLine1);

  // Creating aLine containing the second Vector.
  Handle(StepGeom_Line) aLine2 = new StepGeom_Line;
  aLine2->Init(new TCollection_HAsciiString, aPnt, aVec2);
  addToModel(aLine2);

  // Performing removal of duplicate Vectors.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateVectors();

  // Check that duplicate was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aVec1) || aRemovedEntities.Contains(aVec2));
}
