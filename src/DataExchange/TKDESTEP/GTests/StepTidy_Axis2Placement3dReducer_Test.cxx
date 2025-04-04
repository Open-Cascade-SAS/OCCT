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

#include <StepTidy_Axis2Placement3dReducer.pxx>

#include <StepGeom_Plane.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepGeom_Circle.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepGeom_SphericalSurface.hxx>

class StepTidy_Axis2Placement3dReducerTest : public StepTidy_BaseTestFixture
{
protected:
  //! Perform removal of duplicate entities.
  TColStd_MapOfTransient replaceDuplicateAxis2Placement3ds()
  {
    StepTidy_Axis2Placement3dReducer aReducer(myWS);
    for (Standard_Integer anIndex = 1; anIndex <= myWS->Model()->NbEntities(); ++anIndex)
    {
      aReducer.ProcessEntity(myWS->Model()->Value(anIndex));
    }

    TColStd_MapOfTransient aRemovedEntities;
    aReducer.Perform(aRemovedEntities);
    return aRemovedEntities;
  }
};

// Check that Axis2Placement3ds with the same coordinates and different names are not merged.
TEST_F(StepTidy_Axis2Placement3dReducerTest, DifferentNames)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d("Axis1");
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d("Axis2");

  // Creating a plane containing the first Axis2Placement3d.
  Handle(StepGeom_Plane) aPlane1 = new StepGeom_Plane;
  aPlane1->Init(new TCollection_HAsciiString, anAxis1);
  addToModel(aPlane1);

  // Creating a plane containing the second Axis2Placement3d.
  Handle(StepGeom_Plane) aPlane2 = new StepGeom_Plane;
  aPlane2->Init(new TCollection_HAsciiString, anAxis2);
  addToModel(aPlane2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that nothing was removed.
  EXPECT_TRUE(aRemovedEntities.IsEmpty());
}

// Check that equal Axis2Placement3ds are merged for StepShape_GeometricCurveSet.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_Plane)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a plane containing the first Axis2Placement3d.
  Handle(StepGeom_Plane) aPlane1 = new StepGeom_Plane;
  aPlane1->Init(new TCollection_HAsciiString, anAxis1);
  addToModel(aPlane1);

  // Creating a plane containing the second Axis2Placement3d.
  Handle(StepGeom_Plane) aPlane2 = new StepGeom_Plane;
  aPlane2->Init(new TCollection_HAsciiString, anAxis2);
  addToModel(aPlane2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepRepr_ItemDefinedTransformation.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepRepr_ItemDefinedTransformation)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis3 = addAxis2Placement3d(nullptr, gp_XYZ(1., 1., 1.));

  // Creating ItemDefinedTransformation containing the first Axis2Placement3d.
  Handle(StepRepr_ItemDefinedTransformation) aItem1 = new StepRepr_ItemDefinedTransformation;
  aItem1->Init(new TCollection_HAsciiString, new TCollection_HAsciiString, anAxis1, anAxis3);
  addToModel(aItem1);

  // Creating ItemDefinedTransformation containing the second Axis2Placement3d.
  Handle(StepRepr_ItemDefinedTransformation) aItem2 = new StepRepr_ItemDefinedTransformation;
  aItem1->Init(new TCollection_HAsciiString, new TCollection_HAsciiString, anAxis2, anAxis3);
  addToModel(aItem2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_CylindricalSurface.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_CylindricalSurface)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a cylindrical surface containing the first Axis2Placement3d.
  Handle(StepGeom_CylindricalSurface) aCylindricalSurface1 = new StepGeom_CylindricalSurface;
  aCylindricalSurface1->Init(new TCollection_HAsciiString, anAxis1, 1.0);
  addToModel(aCylindricalSurface1);

  // Creating a cylindrical surface containing the second Axis2Placement3d.
  Handle(StepGeom_CylindricalSurface) aCylindricalSurface2 = new StepGeom_CylindricalSurface;
  aCylindricalSurface2->Init(new TCollection_HAsciiString, anAxis2, 1.0);
  addToModel(aCylindricalSurface2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepShape_ShapeRepresentation.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepShape_ShapeRepresentation)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a shape representation containing the first Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems1 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems1->SetValue(1, anAxis1);
  Handle(StepShape_ShapeRepresentation) aShapeRepresentation1 = new StepShape_ShapeRepresentation;
  aShapeRepresentation1->Init(new TCollection_HAsciiString,
                              aItems1,
                              new StepRepr_RepresentationContext);
  addToModel(aShapeRepresentation1);

  // Creating a shape representation containing the second Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems2 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems2->SetValue(1, anAxis2);
  Handle(StepShape_ShapeRepresentation) aShapeRepresentation2 = new StepShape_ShapeRepresentation;
  aShapeRepresentation2->Init(new TCollection_HAsciiString,
                              aItems2,
                              new StepRepr_RepresentationContext);
  addToModel(aShapeRepresentation2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepRepr_ConstructiveGeometryRepresentation.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepRepr_ConstructiveGeometryRepresentation)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a constructive geometry representation containing the first Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems1 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems1->SetValue(1, anAxis1);
  Handle(StepRepr_ConstructiveGeometryRepresentation) aConstructiveGeometryRepresentation1 =
    new StepRepr_ConstructiveGeometryRepresentation;
  aConstructiveGeometryRepresentation1->Init(new TCollection_HAsciiString,
                                             aItems1,
                                             new StepRepr_RepresentationContext);
  addToModel(aConstructiveGeometryRepresentation1);

  // Creating a constructive geometry representation containing the second Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems2 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems2->SetValue(1, anAxis2);
  Handle(StepRepr_ConstructiveGeometryRepresentation) aConstructiveGeometryRepresentation2 =
    new StepRepr_ConstructiveGeometryRepresentation;
  aConstructiveGeometryRepresentation2->Init(new TCollection_HAsciiString,
                                             aItems2,
                                             new StepRepr_RepresentationContext);
  addToModel(aConstructiveGeometryRepresentation2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_Circle.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_Circle)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a circle containing the first Axis2Placement3d.
  StepGeom_Axis2Placement aSelector1;
  aSelector1.SetValue(anAxis1);
  Handle(StepGeom_Circle) aCircle1 = new StepGeom_Circle;
  aCircle1->Init(new TCollection_HAsciiString, aSelector1, 1.0);
  addToModel(aCircle1);

  // Creating a circle containing the second Axis2Placement3d.
  StepGeom_Axis2Placement aSelector2;
  aSelector2.SetValue(anAxis2);
  Handle(StepGeom_Circle) aCircle2 = new StepGeom_Circle;
  aCircle2->Init(new TCollection_HAsciiString, aSelector2, 1.0);
  addToModel(aCircle2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepVisual_PresentationLayerAssignment.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepVisual_PresentationLayerAssignment)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a presentation layer assignment containing the first Axis2Placement3d.
  Handle(StepVisual_HArray1OfLayeredItem) aAssignedItems1 =
    new StepVisual_HArray1OfLayeredItem(1, 1);
  StepVisual_LayeredItem aLayeredItem1;
  aLayeredItem1.SetValue(anAxis1);
  aAssignedItems1->SetValue(1, aLayeredItem1);
  Handle(StepVisual_PresentationLayerAssignment) aPresentationLayerAssignment1 =
    new StepVisual_PresentationLayerAssignment;
  aPresentationLayerAssignment1->Init(new TCollection_HAsciiString,
                                      new TCollection_HAsciiString,
                                      aAssignedItems1);
  addToModel(aPresentationLayerAssignment1);

  // Creating a presentation layer assignment containing the second Axis2Placement3d.
  Handle(StepVisual_HArray1OfLayeredItem) aAssignedItems2 =
    new StepVisual_HArray1OfLayeredItem(1, 1);
  StepVisual_LayeredItem aLayeredItem2;
  aLayeredItem2.SetValue(anAxis2);
  aAssignedItems2->SetValue(1, aLayeredItem2);
  Handle(StepVisual_PresentationLayerAssignment) aPresentationLayerAssignment2 =
    new StepVisual_PresentationLayerAssignment;
  aPresentationLayerAssignment2->Init(new TCollection_HAsciiString,
                                      new TCollection_HAsciiString,
                                      aAssignedItems2);
  addToModel(aPresentationLayerAssignment2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepVisual_StyledItem.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepVisual_StyledItem)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a styled item containing the first Axis2Placement3d.
  Handle(StepVisual_StyledItem) aStiledItem1 = new StepVisual_StyledItem;
  aStiledItem1->Init(new TCollection_HAsciiString,
                     new StepVisual_HArray1OfPresentationStyleAssignment(1, 1),
                     anAxis1);
  addToModel(aStiledItem1);

  // Creating a styled item containing the second Axis2Placement3d.
  Handle(StepVisual_StyledItem) aStiledItem2 = new StepVisual_StyledItem;
  aStiledItem2->Init(new TCollection_HAsciiString,
                     new StepVisual_HArray1OfPresentationStyleAssignment(1, 1),
                     anAxis2);
  addToModel(aStiledItem2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_Ellipse.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_Ellipse)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating an ellipse containing the first Axis2Placement3d.
  StepGeom_Axis2Placement aSelector1;
  aSelector1.SetValue(anAxis1);
  Handle(StepGeom_Ellipse) aEllipse1 = new StepGeom_Ellipse;
  aEllipse1->Init(new TCollection_HAsciiString, aSelector1, 1.0, 2.0);
  addToModel(aEllipse1);

  // Creating an ellipse containing the second Axis2Placement3d.
  StepGeom_Axis2Placement aSelector2;
  aSelector2.SetValue(anAxis2);
  Handle(StepGeom_Ellipse) aEllipse2 = new StepGeom_Ellipse;
  aEllipse2->Init(new TCollection_HAsciiString, aSelector2, 1.0, 2.0);
  addToModel(aEllipse2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_ConicalSurface.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_ConicalSurface)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a conical surface containing the first Axis2Placement3d.
  Handle(StepGeom_ConicalSurface) aConicalSurface1 = new StepGeom_ConicalSurface;
  aConicalSurface1->Init(new TCollection_HAsciiString, anAxis1, 1.0, 1.0);
  addToModel(aConicalSurface1);

  // Creating a conical surface containing the second Axis2Placement3d.
  Handle(StepGeom_ConicalSurface) aConicalSurface2 = new StepGeom_ConicalSurface;
  aConicalSurface2->Init(new TCollection_HAsciiString, anAxis2, 1.0, 1.0);
  addToModel(aConicalSurface2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_ToroidalSurface.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_ToroidalSurface)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a toroidal surface containing the first Axis2Placement3d.
  Handle(StepGeom_ToroidalSurface) aToroidalSurface1 = new StepGeom_ToroidalSurface;
  aToroidalSurface1->Init(new TCollection_HAsciiString, anAxis1, 1.0, 1.0);
  addToModel(aToroidalSurface1);

  // Creating a toroidal surface containing the second Axis2Placement3d.
  Handle(StepGeom_ToroidalSurface) aToroidalSurface2 = new StepGeom_ToroidalSurface;
  aToroidalSurface2->Init(new TCollection_HAsciiString, anAxis2, 1.0, 1.0);
  addToModel(aToroidalSurface2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepShape_AdvancedBrepShapeRepresentation.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepShape_AdvancedBrepShapeRepresentation)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a shape representation containing the first Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems1 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems1->SetValue(1, anAxis1);
  Handle(StepShape_AdvancedBrepShapeRepresentation) aShapeRepresentation1 =
    new StepShape_AdvancedBrepShapeRepresentation;
  aShapeRepresentation1->Init(new TCollection_HAsciiString,
                              aItems1,
                              new StepRepr_RepresentationContext);
  addToModel(aShapeRepresentation1);

  // Creating a shape representation containing the second Axis2Placement3d.
  Handle(StepRepr_HArray1OfRepresentationItem) aItems2 =
    new StepRepr_HArray1OfRepresentationItem(1, 1);
  aItems2->SetValue(1, anAxis2);
  Handle(StepShape_AdvancedBrepShapeRepresentation) aShapeRepresentation2 =
    new StepShape_AdvancedBrepShapeRepresentation;
  aShapeRepresentation2->Init(new TCollection_HAsciiString,
                              aItems2,
                              new StepRepr_RepresentationContext);
  addToModel(aShapeRepresentation2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}

// Check that equal Axis2Placement3ds are merged for StepGeom_SphericalSurface.
TEST_F(StepTidy_Axis2Placement3dReducerTest, StepGeom_SphericalSurface)
{
  // Creating Axis2Placement3ds.
  Handle(StepGeom_Axis2Placement3d) anAxis1 = addAxis2Placement3d();
  Handle(StepGeom_Axis2Placement3d) anAxis2 = addAxis2Placement3d();

  // Creating a spherical surface containing the first Axis2Placement3d.
  Handle(StepGeom_SphericalSurface) aSphericalSurface1 = new StepGeom_SphericalSurface;
  aSphericalSurface1->Init(new TCollection_HAsciiString, anAxis1, 1.0);
  addToModel(aSphericalSurface1);

  // Creating a spherical surface containing the second Axis2Placement3d.
  Handle(StepGeom_SphericalSurface) aSphericalSurface2 = new StepGeom_SphericalSurface;
  aSphericalSurface2->Init(new TCollection_HAsciiString, anAxis2, 1.0);
  addToModel(aSphericalSurface2);

  // Performing removal of duplicate Axis2Placement3ds.
  TColStd_MapOfTransient aRemovedEntities = replaceDuplicateAxis2Placement3ds();

  // Check that one Axis2Placement3d was removed.
  EXPECT_EQ(aRemovedEntities.Extent(), 1);
  EXPECT_TRUE(aRemovedEntities.Contains(anAxis1) || aRemovedEntities.Contains(anAxis2));
}
