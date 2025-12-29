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
  const occ::handle<XSControl_WorkSession>& theWS)
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_Plane> aSharing = occ::down_cast<StepGeom_Plane>(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceItemDefinedTransformation(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepRepr_ItemDefinedTransformation> aSharing =
    occ::down_cast<StepRepr_ItemDefinedTransformation>(theSharing);
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_CylindricalSurface> aSharing =
    occ::down_cast<StepGeom_CylindricalSurface>(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceShapeRepresentation(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepShape_ShapeRepresentation> aSharing =
    occ::down_cast<StepShape_ShapeRepresentation>(theSharing);
  bool isReplaced = false;
  for (int anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper(); ++anIndex)
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepRepr_ConstructiveGeometryRepresentation> aSharing =
    occ::down_cast<StepRepr_ConstructiveGeometryRepresentation>(theSharing);
  bool isReplaced = false;
  for (int anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper(); ++anIndex)
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_Circle> aSharing  = occ::down_cast<StepGeom_Circle>(theSharing);
  StepGeom_Axis2Placement      aSelector = aSharing->Position();
  if (aSelector.Axis2Placement3d() == theOldEntity)
  {
    aSelector.SetValue(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replacePresentationLayerAssignment(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepVisual_PresentationLayerAssignment> aSharing =
    occ::down_cast<StepVisual_PresentationLayerAssignment>(theSharing);
  bool                                                     isReplaced = false;
  occ::handle<NCollection_HArray1<StepVisual_LayeredItem>> anItems    = aSharing->AssignedItems();
  for (int anIndex = anItems->Lower(); anIndex <= anItems->Upper(); ++anIndex)
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepVisual_StyledItem> aSharing = occ::down_cast<StepVisual_StyledItem>(theSharing);
  if (aSharing->Item() == theOldEntity)
  {
    aSharing->SetItem(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceEllipse(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_Ellipse> aSharing  = occ::down_cast<StepGeom_Ellipse>(theSharing);
  StepGeom_Axis2Placement       aSelector = aSharing->Position();
  if (aSelector.Axis2Placement3d() == theOldEntity)
  {
    aSelector.SetValue(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceConicalSurface(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_ConicalSurface> aSharing =
    occ::down_cast<StepGeom_ConicalSurface>(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceToroidalSurface(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_ToroidalSurface> aSharing =
    occ::down_cast<StepGeom_ToroidalSurface>(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_Axis2Placement3dReducer::replaceAdvancedBrepShapeRepresentation(
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepShape_AdvancedBrepShapeRepresentation> aSharing =
    occ::down_cast<StepShape_AdvancedBrepShapeRepresentation>(theSharing);
  bool isReplaced = false;
  for (int anIndex = aSharing->Items()->Lower(); anIndex <= aSharing->Items()->Upper(); ++anIndex)
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
  const occ::handle<StepGeom_Axis2Placement3d>& theOldEntity,
  const occ::handle<StepGeom_Axis2Placement3d>& theNewEntity,
  const occ::handle<Standard_Transient>&        theSharing)
{
  occ::handle<StepGeom_SphericalSurface> aSharing =
    occ::down_cast<StepGeom_SphericalSurface>(theSharing);
  if (aSharing->Position() == theOldEntity)
  {
    aSharing->SetPosition(theNewEntity);
    return true;
  }
  return false;
}
