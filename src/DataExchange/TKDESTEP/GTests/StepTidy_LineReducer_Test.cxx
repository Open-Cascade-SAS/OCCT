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
  TColStd_MapOfTransient replaceDuplicateLines()
  {
    StepTidy_LineReducer aReducer(myWS);
    for (Standard_Integer anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    TColStd_MapOfTransient aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Lines with different names are not merged.
TEST_F(StepTidy_LineReducerTest, DifferentNames)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine("Line1");
  Handle(StepGeom_Line) aLine2 = addLine("Line2");

  // Creating EdgeCurve containing the first Line.
  Handle(StepShape_EdgeCurve) aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aLine1,
                        Standard_True);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Line.
  Handle(StepShape_EdgeCurve) aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aLine2,
                         Standard_True);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that equal Lines are merged for StepShape_EdgeCurve.
TEST_F(StepTidy_LineReducerTest, StepShape_EdgeCurve)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine();
  Handle(StepGeom_Line) aLine2 = addLine();

  // Creating EdgeCurve containing the first Line.
  Handle(StepShape_EdgeCurve) aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aLine1,
                        Standard_True);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Line.
  Handle(StepShape_EdgeCurve) aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aLine2,
                         Standard_True);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_TrimmedCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_TrimmedCurve)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine();
  Handle(StepGeom_Line) aLine2 = addLine();

  // Creating TrimmedCurve containing the first Line.
  Handle(StepGeom_TrimmedCurve) aFirstTrimmedCurve = new StepGeom_TrimmedCurve;
  aFirstTrimmedCurve->Init(new TCollection_HAsciiString,
                           aLine1,
                           new StepGeom_HArray1OfTrimmingSelect,
                           new StepGeom_HArray1OfTrimmingSelect,
                           Standard_True,
                           StepGeom_tpUnspecified);
  addToModel(aFirstTrimmedCurve);

  // Creating TrimmedCurve containing the second Line.
  Handle(StepGeom_TrimmedCurve) aSecondTrimmedCurve = new StepGeom_TrimmedCurve;
  aSecondTrimmedCurve->Init(new TCollection_HAsciiString,
                            aLine2,
                            new StepGeom_HArray1OfTrimmingSelect,
                            new StepGeom_HArray1OfTrimmingSelect,
                            Standard_True,
                            StepGeom_tpUnspecified);
  addToModel(aSecondTrimmedCurve);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_SurfaceCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_SurfaceCurve)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine();
  Handle(StepGeom_Line) aLine2 = addLine();

  // Creating SurfaceCurve containing the first Line.
  Handle(StepGeom_SurfaceCurve) aFirstSurfaceCurve = new StepGeom_SurfaceCurve;
  aFirstSurfaceCurve->Init(new TCollection_HAsciiString,
                           aLine1,
                           new StepGeom_HArray1OfPcurveOrSurface,
                           StepGeom_pscrCurve3d);
  addToModel(aFirstSurfaceCurve);

  // Creating SurfaceCurve containing the second Line.
  Handle(StepGeom_SurfaceCurve) aSecondSurfaceCurve = new StepGeom_SurfaceCurve;
  aSecondSurfaceCurve->Init(new TCollection_HAsciiString,
                            aLine2,
                            new StepGeom_HArray1OfPcurveOrSurface,
                            StepGeom_pscrCurve3d);
  addToModel(aSecondSurfaceCurve);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepRepr_DefinitionalRepresentation.
TEST_F(StepTidy_LineReducerTest, StepRepr_DefinitionalRepresentation)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine();
  Handle(StepGeom_Line) aLine2 = addLine();

  // Creating DefinitionalRepresentation containing the first Line.
  Handle(StepRepr_HArray1OfRepresentationItem) aFirstItems =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aFirstItems->SetValue(1, aLine1);
  Handle(StepRepr_DefinitionalRepresentation) aFirstDefinitionalRepresentation =
    new StepRepr_DefinitionalRepresentation;
  aFirstDefinitionalRepresentation->Init(new TCollection_HAsciiString,
                                         aFirstItems,
                                         new StepRepr_RepresentationContext);
  addToModel(aFirstDefinitionalRepresentation);

  // Creating DefinitionalRepresentation containing the second Line.
  Handle(StepRepr_HArray1OfRepresentationItem) aSecondItems =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aSecondItems->SetValue(1, aLine2);
  Handle(StepRepr_DefinitionalRepresentation) aSecondDefinitionalRepresentation =
    new StepRepr_DefinitionalRepresentation;
  aSecondDefinitionalRepresentation->Init(new TCollection_HAsciiString,
                                          aSecondItems,
                                          new StepRepr_RepresentationContext);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}

// Check that equal Lines are merged for StepGeom_SeamCurve.
TEST_F(StepTidy_LineReducerTest, StepGeom_SeamCurve)
{
  // Creating Lines.
  Handle(StepGeom_Line) aLine1 = addLine();
  Handle(StepGeom_Line) aLine2 = addLine();

  // Creating SeamCurve containing the first Line.
  Handle(StepGeom_SeamCurve) aFirstSeamCurve = new StepGeom_SeamCurve;
  aFirstSeamCurve->Init(new TCollection_HAsciiString,
                        aLine1,
                        new StepGeom_HArray1OfPcurveOrSurface,
                        StepGeom_pscrCurve3d);
  addToModel(aFirstSeamCurve);

  // Creating SeamCurve containing the second Line.
  Handle(StepGeom_SeamCurve) aSecondSeamCurve = new StepGeom_SeamCurve;
  aSecondSeamCurve->Init(new TCollection_HAsciiString,
                         aLine2,
                         new StepGeom_HArray1OfPcurveOrSurface,
                         StepGeom_pscrCurve3d);
  addToModel(aSecondSeamCurve);

  // Performing removal of duplicate Lines.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateLines();

  // Check that one Line was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aLine1) || aRemovedEntities.Contains(aLine2));
}
