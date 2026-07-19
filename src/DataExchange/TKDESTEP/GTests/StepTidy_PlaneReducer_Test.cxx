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

#include <StepTidy_PlaneReducer.pxx>

#include <StepShape_AdvancedFace.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>

class StepTidy_PlaneReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  NCollection_Map<occ::handle<Standard_Transient>> replaceDuplicatePlanes()
  {
    StepTidy_PlaneReducer aReducer(myWS);
    for (int anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Planes with different names are not merged.
TEST_F(StepTidy_PlaneReducerTest, DifferentNames)
{
  // Creating Planes.
  occ::handle<StepGeom_Plane> aPlane1 = addPlane("Plane1");
  occ::handle<StepGeom_Plane> aPlane2 = addPlane("Plane2");

  // Creating StepShape_AdvancedFace containing the first Plane.
  occ::handle<StepShape_AdvancedFace> aFirstAdvancedFace = new StepShape_AdvancedFace;
  aFirstAdvancedFace->Init(new TCollection_HAsciiString,
                           new NCollection_HArray1<occ::handle<StepShape_FaceBound>>,
                           aPlane1,
                           true);
  addToModel(aFirstAdvancedFace);

  // Creating StepShape_AdvancedFace containing the second Plane.
  occ::handle<StepShape_AdvancedFace> aSecondAdvancedFace = new StepShape_AdvancedFace;
  aSecondAdvancedFace->Init(new TCollection_HAsciiString,
                            new NCollection_HArray1<occ::handle<StepShape_FaceBound>>,
                            aPlane2,
                            true);
  addToModel(aSecondAdvancedFace);

  // Performing removal of duplicate Planes.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicatePlanes();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that equal Planes are merged for StepShape_AdvancedFace.
TEST_F(StepTidy_PlaneReducerTest, StepShape_AdvancedFace)
{
  // Creating Planes.
  occ::handle<StepGeom_Plane> aPlane1 = addPlane();
  occ::handle<StepGeom_Plane> aPlane2 = addPlane();

  // Creating StepShape_AdvancedFace containing the first Plane.
  occ::handle<StepShape_AdvancedFace> aFirstAdvancedFace = new StepShape_AdvancedFace;
  aFirstAdvancedFace->Init(new TCollection_HAsciiString,
                           new NCollection_HArray1<occ::handle<StepShape_FaceBound>>,
                           aPlane1,
                           true);
  addToModel(aFirstAdvancedFace);

  // Creating StepShape_AdvancedFace containing the second Plane.
  occ::handle<StepShape_AdvancedFace> aSecondAdvancedFace = new StepShape_AdvancedFace;
  aSecondAdvancedFace->Init(new TCollection_HAsciiString,
                            new NCollection_HArray1<occ::handle<StepShape_FaceBound>>,
                            aPlane2,
                            true);
  addToModel(aSecondAdvancedFace);

  // Performing removal of duplicate Planes.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicatePlanes();

  // Check that one Plane was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPlane1) || aRemovedEntities.Contains(aPlane2));
}

// Check that equal Planes are merged for StepGeom_Pcurve.
TEST_F(StepTidy_PlaneReducerTest, StepGeom_Pcurve)
{
  // Creating Planes.
  occ::handle<StepGeom_Plane> aPlane1 = addPlane();
  occ::handle<StepGeom_Plane> aPlane2 = addPlane();

  // Creating StepGeom_Pcurve containing the first Plane.
  occ::handle<StepGeom_Pcurve> aFirstPcurve = new StepGeom_Pcurve;
  aFirstPcurve->Init(new TCollection_HAsciiString,
                     aPlane1,
                     new StepRepr_DefinitionalRepresentation);
  addToModel(aFirstPcurve);

  // Creating StepGeom_Pcurve containing the second Plane.
  occ::handle<StepGeom_Pcurve> aSecondPcurve = new StepGeom_Pcurve;
  aSecondPcurve->Init(new TCollection_HAsciiString,
                      aPlane2,
                      new StepRepr_DefinitionalRepresentation);
  addToModel(aSecondPcurve);

  // Performing removal of duplicate Planes.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicatePlanes();

  // Check that one Plane was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aPlane1) || aRemovedEntities.Contains(aPlane2));
}
