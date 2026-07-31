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

#include <StepTidy_LineReducer.pxx>

#include <StepShape_EdgeCurve.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepRepr_DefinitionalRepresentation.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepRepr_RepresentationContext.hxx>

class StepTidy_LineReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  NCollection_Map<occ::handle<Standard_Transient>> replaceDuplicateLines()
  {
    StepTidy_LineReducer aReducer(myWS);
    for (int anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Lines with different names are not merged.
TEST_F(StepTidy_LineReducerTest, DifferentNames)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine("Line1");
  occ::handle<StepGeom_Line> aLine2 = addLine("Line2");

  // Creating EdgeCurve containing the first Line.
  occ::handle<StepShape_EdgeCurve> aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aLine1,
                        true);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Line.
  occ::handle<StepShape_EdgeCurve> aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aLine2,
                         true);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that equal Lines are merged for StepShape_EdgeCurve.
TEST_F(StepTidy_LineReducerTest, StepShape_EdgeCurve)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine();
  occ::handle<StepGeom_Line> aLine2 = addLine();

  // Creating EdgeCurve containing the first Line.
  occ::handle<StepShape_EdgeCurve> aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aLine1,
                        true);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Line.
  occ::handle<StepShape_EdgeCurve> aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aLine2,
                         true);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_TrimmedCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_TrimmedCurve)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine();
  occ::handle<StepGeom_Line> aLine2 = addLine();

  // Creating TrimmedCurve containing the first Line.
  occ::handle<StepGeom_TrimmedCurve> aFirstTrimmedCurve = new StepGeom_TrimmedCurve;
  aFirstTrimmedCurve->Init(new TCollection_HAsciiString,
                           aLine1,
                           new NCollection_HArray1<StepGeom_TrimmingSelect>,
                           new NCollection_HArray1<StepGeom_TrimmingSelect>,
                           true,
                           StepGeom_tpUnspecified);
  addToModel(aFirstTrimmedCurve);

  // Creating TrimmedCurve containing the second Line.
  occ::handle<StepGeom_TrimmedCurve> aSecondTrimmedCurve = new StepGeom_TrimmedCurve;
  aSecondTrimmedCurve->Init(new TCollection_HAsciiString,
                            aLine2,
                            new NCollection_HArray1<StepGeom_TrimmingSelect>,
                            new NCollection_HArray1<StepGeom_TrimmingSelect>,
                            true,
                            StepGeom_tpUnspecified);
  addToModel(aSecondTrimmedCurve);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_SurfaceCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_SurfaceCurve)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine();
  occ::handle<StepGeom_Line> aLine2 = addLine();

  // Creating SurfaceCurve containing the first Line.
  occ::handle<StepGeom_SurfaceCurve> aFirstSurfaceCurve = new StepGeom_SurfaceCurve;
  aFirstSurfaceCurve->Init(new TCollection_HAsciiString,
                           aLine1,
                           new NCollection_HArray1<StepGeom_PcurveOrSurface>,
                           StepGeom_pscrCurve3d);
  addToModel(aFirstSurfaceCurve);

  // Creating SurfaceCurve containing the second Line.
  occ::handle<StepGeom_SurfaceCurve> aSecondSurfaceCurve = new StepGeom_SurfaceCurve;
  aSecondSurfaceCurve->Init(new TCollection_HAsciiString,
                            aLine2,
                            new NCollection_HArray1<StepGeom_PcurveOrSurface>,
                            StepGeom_pscrCurve3d);
  addToModel(aSecondSurfaceCurve);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepRepr_DefinitionalRepresentation.
TEST_F(StepTidy_LineReducerTest, StepRepr_DefinitionalRepresentation)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine();
  occ::handle<StepGeom_Line> aLine2 = addLine();

  // Creating DefinitionalRepresentation containing the first Line.
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aFirstItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aFirstItems->SetValue(1, aLine1);
  occ::handle<StepRepr_DefinitionalRepresentation> aFirstDefinitionalRepresentation =
    new StepRepr_DefinitionalRepresentation;
  aFirstDefinitionalRepresentation->Init(new TCollection_HAsciiString,
                                         aFirstItems,
                                         new StepRepr_RepresentationContext);
  addToModel(aFirstDefinitionalRepresentation);

  // Creating DefinitionalRepresentation containing the second Line.
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aSecondItems =
    new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, 1);
  aSecondItems->SetValue(1, aLine2);
  occ::handle<StepRepr_DefinitionalRepresentation> aSecondDefinitionalRepresentation =
    new StepRepr_DefinitionalRepresentation;
  aSecondDefinitionalRepresentation->Init(new TCollection_HAsciiString,
                                          aSecondItems,
                                          new StepRepr_RepresentationContext);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_SeamCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_SeamCurve)
{
  // Creating Lines.
  occ::handle<StepGeom_Line> aLine1 = addLine();
  occ::handle<StepGeom_Line> aLine2 = addLine();

  // Creating SeamCurve containing the first Line.
  occ::handle<StepGeom_SeamCurve> aFirstSeamCurve = new StepGeom_SeamCurve;
  aFirstSeamCurve->Init(new TCollection_HAsciiString,
                        aLine1,
                        new NCollection_HArray1<StepGeom_PcurveOrSurface>,
                        StepGeom_pscrCurve3d);
  addToModel(aFirstSeamCurve);

  // Creating SeamCurve containing the second Line.
  occ::handle<StepGeom_SeamCurve> aSecondSeamCurve = new StepGeom_SeamCurve;
  aSecondSeamCurve->Init(new TCollection_HAsciiString,
                         aLine2,
                         new NCollection_HArray1<StepGeom_PcurveOrSurface>,
                         StepGeom_pscrCurve3d);
  addToModel(aSecondSeamCurve);

  // Performing removal of duplicate Lines.
  NCollection_Map<occ::handle<Standard_Transient>> aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}
