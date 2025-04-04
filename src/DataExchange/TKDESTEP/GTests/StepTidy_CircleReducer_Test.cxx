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

#include <StepTidy_CircleReducer.pxx>

#include <StepShape_EdgeCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepGeom_SeamCurve.hxx>

class StepTidy_CircleReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  TColStd_MapOfTransient replaceDuplicateCircles()
  {
    StepTidy_CircleReducer aReducer(myWS);
    for (Standard_Integer anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    TColStd_MapOfTransient aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Circles with the same coordinates and different names are not merged.
TEST_F(StepTidy_CircleReducerTest, DifferentNames)
{
  // Creating Circles.
  Handle(StepGeom_Circle) aCircle1 = addCircle("Circle1");
  Handle(StepGeom_Circle) aCircle2 = addCircle("Circle2");

  // Creating EdgeCurve containing the first Circle.
  Handle(StepShape_EdgeCurve) aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aCircle1,
                        Standard_True);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Circle.
  Handle(StepShape_EdgeCurve) aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aCircle2,
                         Standard_True);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Circles.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateCircles();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that equal Circles are merged for StepShape_EdgeCurve.
TEST_F(StepTidy_CircleReducerTest, StepShape_EdgeCurve)
{
  // Creating Circles.
  Handle(StepGeom_Circle) aCircle1 = addCircle();
  Handle(StepGeom_Circle) aCircle2 = addCircle();

  // Creating EdgeCurve containing the first Circle.
  Handle(StepShape_EdgeCurve) aFirstEdgeCurve = new StepShape_EdgeCurve;
  aFirstEdgeCurve->Init(new TCollection_HAsciiString,
                        new StepShape_Vertex,
                        new StepShape_Vertex,
                        aCircle1,
                        Standard_True);
  addToModel(aFirstEdgeCurve);

  // Creating EdgeCurve containing the second Circle.
  Handle(StepShape_EdgeCurve) aSecondEdgeCurve = new StepShape_EdgeCurve;
  aSecondEdgeCurve->Init(new TCollection_HAsciiString,
                         new StepShape_Vertex,
                         new StepShape_Vertex,
                         aCircle2,
                         Standard_True);
  addToModel(aSecondEdgeCurve);

  // Performing removal of duplicate Circles.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateCircles();

  // Check that one Circle was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aCircle1) || aRemovedEntities.Contains(aCircle2));
}

// Check that equal Circles are merged for StepGeom_SurfaceCurve.
TEST_F(StepTidy_CircleReducerTest, StepGeom_SurfaceCurve)
{
  // Creating Circles.
  Handle(StepGeom_Circle) aCircle1 = addCircle();
  Handle(StepGeom_Circle) aCircle2 = addCircle();

  // Creating SurfaceCurve containing the first Circle.
  Handle(StepGeom_SurfaceCurve) aFirstSurfaceCurve = new StepGeom_SurfaceCurve;
  aFirstSurfaceCurve->Init(new TCollection_HAsciiString,
                           aCircle1,
                           new StepGeom_HArray1OfPcurveOrSurface,
                           StepGeom_pscrCurve3d);
  addToModel(aFirstSurfaceCurve);

  // Creating SurfaceCurve containing the second Circle.
  Handle(StepGeom_SurfaceCurve) aSecondSurfaceCurve = new StepGeom_SurfaceCurve;
  aSecondSurfaceCurve->Init(new TCollection_HAsciiString,
                            aCircle2,
                            new StepGeom_HArray1OfPcurveOrSurface,
                            StepGeom_pscrCurve3d);
  addToModel(aSecondSurfaceCurve);

  // Performing removal of duplicate Circles.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateCircles();

  // Check that one Circle was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aCircle1) || aRemovedEntities.Contains(aCircle2));
}

// Check that equal Circles are merged for StepGeom_SeamCurve.
TEST_F(StepTidy_CircleReducerTest, StepGeom_SeamCurve)
{
  // Creating Circles.
  Handle(StepGeom_Circle) aCircle1 = addCircle();
  Handle(StepGeom_Circle) aCircle2 = addCircle();

  // Creating SeamCurve containing the first Circle.
  Handle(StepGeom_SeamCurve) aFirstSeamCurve = new StepGeom_SeamCurve;
  aFirstSeamCurve->Init(new TCollection_HAsciiString,
                        aCircle1,
                        new StepGeom_HArray1OfPcurveOrSurface,
                        StepGeom_pscrCurve3d);
  addToModel(aFirstSeamCurve);

  // Creating SeamCurve containing the second Circle.
  Handle(StepGeom_SeamCurve) aSecondSeamCurve = new StepGeom_SeamCurve;
  aSecondSeamCurve->Init(new TCollection_HAsciiString,
                         aCircle2,
                         new StepGeom_HArray1OfPcurveOrSurface,
                         StepGeom_pscrCurve3d);
  addToModel(aSecondSeamCurve);

  // Performing removal of duplicate Circles.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateCircles();

  // Check that one Circle was removed.
  EXPECT_EQ(aRemovedEntities.Size(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(aCircle1) || aRemovedEntities.Contains(aCircle2));
}
