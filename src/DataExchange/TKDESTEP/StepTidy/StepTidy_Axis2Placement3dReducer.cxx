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

#include <StepTidy_Axis2Placement3dReducer.pxx>

#include <StepGeom_Plane.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepGeom_CylindricalSurface.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepRepr_ConstructiveGeometryRepresentation.hxx>
#include <StepGeom_Circle.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_ConicalSurface.hxx>
#include <StepGeom_ToroidalSurface.hxx>
#include <StepShape_AdvancedBrepShapeRepresentation.hxx>
#include <StepGeom_SphericalSurface.hxx>

//==================================================================================================

StepTidy_Axis2Placement3dReducer::StepTidy_Axis2Placement3dReducer(
  const Handle(XSControl_WorkSession)& theWS)
    : StepTidy_EntityReducer<StepGeom_Axis2Placement3d, StepTidy_Axis2Placement3dHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepGeom_Plane), replacePlane);
  registerReplacer(STANDARD_TYPE(StepRepr_ItemDefinedTransformation),
                   replaceItemDefinedTransformation);
  registerReplacer(STANDARD_TYPE(StepGeom_CylindricalSurface), replaceCylindricalSurface);
  registerReplacer(STANDARD_TYPE(StepShape_ShapeRepresentation), replaceShapeRepresentation);
  registerReplacer(STANDARD_TYPE(StepRepr_ConstructiveGeometryRepresentation),
                   replaceConstructiveGeometryRepresentation);
  registerReplacer(STANDARD_TYPE(StepGeom_Circle), replaceCircle);
  registerReplacer(STANDARD_TYPE(StepVisual_PresentationLayerAssignment),
                   replacePresentationLayerAssignment);
  registerReplacer(STANDARD_TYPE(StepVisual_StyledItem), replaceStyledItem);
  registerReplacer(STANDARD_TYPE(StepGeom_Ellipse), replaceEllipse);
  registerReplacer(STANDARD_TYPE(StepGeom_ConicalSurface), replaceConicalSurface);
  registerReplacer(STANDARD_TYPE(StepGeom_ToroidalSurface), replaceToroidalSurface);
  registerReplacer(STANDARD_TYPE(StepShape_AdvancedBrepShapeRepresentation),
                   replaceAdvancedBrepShapeRepresentation);
  registerReplacer(STANDARD_TYPE(StepGeom_SphericalSurface), replaceSphericalSurface);
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replacePlane(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_Plane) aSharing = Handle(StepGeom_Plane)::DownCast(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceItemDefinedTransformation(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepRepr_ItemDefinedTransformation) aSharing =
    Handle(StepRepr_ItemDefinedTransformation)::DownCast(theSharing);
  bool isReplaced = false;
  if (aSharing->TransformItem1() == theOldEntity)
  {
    aSharing->SetTransformItem1(theNewEntity);
    isReplaced = true;
  }
  if (aSharing->TransformItem2() == theOldEntity)
  {
    aSharing->SetTransformItem2(theNewEntity);
    isReplaced = true;
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceCylindricalSurface(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_CylindricalSurface) aSharing =
    Handle(StepGeom_CylindricalSurface)::DownCast(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceShapeRepresentation(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepShape_ShapeRepresentation) aSharing =
    Handle(StepShape_ShapeRepresentation)::DownCast(theSharing);
  bool isReplaced = false;
  for (Standard_Integer anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper();
       ++anIndex)
  {
    if (aSharing->Items()->Value(anIndex) == theOldEntity)
    {
      aSharing->Items()->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceConstructiveGeometryRepresentation(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepRepr_ConstructiveGeometryRepresentation) aSharing =
    Handle(StepRepr_ConstructiveGeometryRepresentation)::DownCast(theSharing);
  bool isReplaced = false;
  for (Standard_Integer anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper();
       ++anIndex)
  {
    if (aSharing->Items()->Value(anIndex) == theOldEntity)
    {
      aSharing->Items()->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceCircle(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_Circle) aSharing  = Handle(StepGeom_Circle)::DownCast(theSharing);
  StepGeom_Axis2Placement aSelector = aSharing->Position();
  if (aSelector.Axis2Placement3d() == theOldEntity)
  {
    aSelector.SetValue(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replacePresentationLayerAssignment(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepVisual_PresentationLayerAssignment) aSharing =
    Handle(StepVisual_PresentationLayerAssignment)::DownCast(theSharing);
  bool                                    isReplaced = false;
  Handle(StepVisual_HArray1OfLayeredItem) anItems    = aSharing->AssignedItems();
  for (Standard_Integer anIndex = anItems->Lower(); anIndex <= anItems->Upper(); ++anIndex)
  {
    StepVisual_LayeredItem& aLayeredItem = anItems->ChangeValue(anIndex);
    if (aLayeredItem.RepresentationItem() == theOldEntity)
    {
      aLayeredItem.SetValue(theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceStyledItem(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepVisual_StyledItem) aSharing = Handle(StepVisual_StyledItem)::DownCast(theSharing);
  if (aSharing->Item() == theOldEntity)
  {
    aSharing->SetItem(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceEllipse(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_Ellipse) aSharing  = Handle(StepGeom_Ellipse)::DownCast(theSharing);
  StepGeom_Axis2Placement  aSelector = aSharing->Position();
  if (aSelector.Axis2Placement3d() == theOldEntity)
  {
    aSelector.SetValue(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceConicalSurface(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_ConicalSurface) aSharing = Handle(StepGeom_ConicalSurface)::DownCast(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceToroidalSurface(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_ToroidalSurface) aSharing =
    Handle(StepGeom_ToroidalSurface)::DownCast(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceAdvancedBrepShapeRepresentation(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepShape_AdvancedBrepShapeRepresentation) aSharing =
    Handle(StepShape_AdvancedBrepShapeRepresentation)::DownCast(theSharing);
  bool isReplaced = false;
  for (Standard_Integer anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper();
       ++anIndex)
  {
    if (aSharing->Items()->Value(anIndex) == theOldEntity)
    {
      aSharing->Items()->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceSphericalSurface(
  const Handle(StepGeom_Axis2Placement3d)& theOldEntity,
  const Handle(StepGeom_Axis2Placement3d)& theNewEntity,
  Handle(Standard_Transient)               theSharing)
{
  Handle(StepGeom_SphericalSurface) aSharing =
    Handle(StepGeom_SphericalSurface)::DownCast(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}
