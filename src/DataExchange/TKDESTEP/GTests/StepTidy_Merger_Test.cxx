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

#include <StepTidy_DuplicateCleaner.hxx>

#include <StepGeom_Axis1Placement.hxx>

class StepTidy_DuplicateCleanerTest : public StepTidy_BaseTestFixture
{
protected:
  // Get the number of entities of the specified type.
  // @param theType the type of entities to count.
  // @return the number of entities of the specified type.
  int getEntitiesCount(const Handle(Standard_Type)& theType) const
  {
    int aCount = 0;
    for (Standard_Integer i = 1; i <= myWS->Model()->NbEntities(); i++)
    {
      if (myWS->Model()->Value(i)->IsKind(theType))
      {
        aCount++;
      }
    }
    return aCount;
  }

  //! Perform removal of duplicate entities points.
  void performRemoval()
  {
    StepTidy_DuplicateCleaner aMerger(myWS);
    aMerger.Perform();
  }
};

// Check that entities with the same coordinates and different names are not merged.
TEST_F(StepTidy_DuplicateCleanerTest, DifferentEntities)
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

  const int aDirectionCountBefore = getEntitiesCount(STANDARD_TYPE(StepGeom_Direction));

  // Performing removal of duplicate directions.
  performRemoval();

  const int aDirectionCountAfter = getEntitiesCount(STANDARD_TYPE(StepGeom_Direction));

  // Check that nothing was removed.
  EXPECT_EQ(aDirectionCountBefore, 2);
  EXPECT_EQ(aDirectionCountBefore, aDirectionCountAfter);
}

// Check that entities with the same coordinates and same names are
// merged for StepGeom_Axis1Placement.
TEST_F(StepTidy_DuplicateCleanerTest, EqualEntities)
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

  const int aDirectionCountBefore = getEntitiesCount(STANDARD_TYPE(StepGeom_Direction));

  // Performing removal of duplicate directions.
  performRemoval();

  const int aDirectionCountAfter = getEntitiesCount(STANDARD_TYPE(StepGeom_Direction));

  // Check that one direction was removed.
  EXPECT_EQ(aDirectionCountBefore, 2);
  EXPECT_EQ(aDirectionCountAfter, 1);
}
