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

#include <StepTidy_CartesianPointReducer.pxx>

#include <Interface_Graph.hxx>
#include <StepGeom_Axis1Placement.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_Line.hxx>
#include <StepRepr_Representation.hxx>
#include <StepShape_GeometricCurveSet.hxx>
#include <StepShape_VertexPoint.hxx>
#include <StepVisual_PresentationLayerAssignment.hxx>
#include <StepVisual_StyledItem.hxx>

//==================================================================================================

StepTidy_CartesianPointReducer::StepTidy_CartesianPointReducer(
  const occ::handle<XSControl_WorkSession>& theWS)
    : StepTidy_EntityReducer<StepGeom_CartesianPoint, StepTidy_CartesianPointHasher>(theWS)
{
  registerReplacer(STANDARD_TYPE(StepGeom_Axis1Placement), replaceAxis1Placement);
  registerReplacer(STANDARD_TYPE(StepGeom_Axis2Placement3d), replaceAxis2Placement3d);
  registerReplacer(STANDARD_TYPE(StepShape_VertexPoint), replaceVertexPoint);
  registerReplacer(STANDARD_TYPE(StepShape_GeometricCurveSet), replaceGeometricCurveSet);
  registerReplacer(STANDARD_TYPE(StepVisual_PresentationLayerAssignment),
                   replacePresentationLayerAssignment);
  registerReplacer(STANDARD_TYPE(StepVisual_StyledItem), replaceStyledItem);
  registerReplacer(STANDARD_TYPE(StepGeom_BSplineCurveWithKnots), replaceBSplineCurveWithKnots);
  registerReplacer(STANDARD_TYPE(StepGeom_Line), replaceLine);
  registerReplacer(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnots), replaceBSplineSurfaceWithKnots);
  registerReplacer(STANDARD_TYPE(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve),
                   replaceBSplineCurveWithKnotsAndRationalBSplineCurve);
  registerReplacer(STANDARD_TYPE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface),
                   replaceBSplineSurfaceWithKnotsAndRationalBSplineSurface);
  registerReplacer(STANDARD_TYPE(StepRepr_Representation), replaceRepresentation);
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceAxis2Placement3d(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_Axis2Placement3d> aSharing =
    occ::down_cast<StepGeom_Axis2Placement3d>(theSharing);
  if (aSharing->Location() == theOldEntity)
  {
    aSharing->SetLocation(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceVertexPoint(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepShape_VertexPoint> aSharing = occ::down_cast<StepShape_VertexPoint>(theSharing);
  if (aSharing->VertexGeometry() == theOldEntity)
  {
    aSharing->SetVertexGeometry(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceGeometricCurveSet(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepShape_GeometricSet> aSharing = occ::down_cast<StepShape_GeometricSet>(theSharing);
  bool                                isReplaced = false;
  for (auto& anElement : *aSharing->Elements())
  {
    const occ::handle<StepGeom_Point> aCurrentPoint = anElement.Point();
    if (aCurrentPoint && aCurrentPoint == theOldEntity)
    {
      anElement.SetValue(theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replacePresentationLayerAssignment(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepVisual_PresentationLayerAssignment> aSharing =
    occ::down_cast<StepVisual_PresentationLayerAssignment>(theSharing);
  bool isReplaced = false;
  for (auto& anAssignedItem : *aSharing->AssignedItems())
  {
    const occ::handle<StepRepr_RepresentationItem> aRepItem = anAssignedItem.RepresentationItem();
    if (aRepItem == theOldEntity)
    {
      anAssignedItem.SetValue(theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceStyledItem(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
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

bool StepTidy_CartesianPointReducer::replaceBSplineCurveWithKnots(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_BSplineCurveWithKnots> aSharing =
    occ::down_cast<StepGeom_BSplineCurveWithKnots>(theSharing);
  bool                                                                   isReplaced = false;
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aControlPoints =
    aSharing->ControlPointsList();
  for (int anIndex = aControlPoints->Lower(); anIndex <= aControlPoints->Upper(); ++anIndex)
  {
    if (aControlPoints->Value(anIndex) == theOldEntity)
    {
      aControlPoints->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceLine(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_Line> aSharing = occ::down_cast<StepGeom_Line>(theSharing);
  if (aSharing->Pnt() == theOldEntity)
  {
    aSharing->SetPnt(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceBSplineSurfaceWithKnots(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_BSplineSurfaceWithKnots> aSharing =
    occ::down_cast<StepGeom_BSplineSurfaceWithKnots>(theSharing);
  bool                                                                   isReplaced = false;
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aControlPoints =
    aSharing->ControlPointsList();
  for (int anIndexI = aControlPoints->LowerRow(); anIndexI <= aControlPoints->UpperRow();
       ++anIndexI)
  {
    for (int anIndexJ = aControlPoints->LowerCol(); anIndexJ <= aControlPoints->UpperCol();
         ++anIndexJ)
    {
      if (aControlPoints->Value(anIndexI, anIndexJ) == theOldEntity)
      {
        aControlPoints->SetValue(anIndexI, anIndexJ, theNewEntity);
        isReplaced = true;
      }
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceAxis1Placement(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_Axis1Placement> aSharing =
    occ::down_cast<StepGeom_Axis1Placement>(theSharing);
  if (aSharing->Location() == theOldEntity)
  {
    aSharing->SetLocation(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceRepresentation(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepRepr_Representation> aSharing =
    occ::down_cast<StepRepr_Representation>(theSharing);
  bool                                                                       isReplaced = false;
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> anItems =
    aSharing->Items();
  for (int anIndex = 1; anIndex <= aSharing->NbItems(); ++anIndex)
  {
    const occ::handle<StepRepr_RepresentationItem> aRepItem = anItems->Value(anIndex);
    if (aRepItem == theOldEntity)
    {
      anItems->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceBSplineCurveWithKnotsAndRationalBSplineCurve(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve> aSharing =
    occ::down_cast<StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve>(theSharing);
  bool                                                                   isReplaced = false;
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aControlPoints =
    aSharing->ControlPointsList();
  for (int anIndex = aControlPoints->Lower(); anIndex <= aControlPoints->Upper(); ++anIndex)
  {
    if (aControlPoints->Value(anIndex) == theOldEntity)
    {
      aControlPoints->SetValue(anIndex, theNewEntity);
      isReplaced = true;
    }
  }
  return isReplaced;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceBSplineSurfaceWithKnotsAndRationalBSplineSurface(
  const occ::handle<StepGeom_CartesianPoint>& theOldEntity,
  const occ::handle<StepGeom_CartesianPoint>& theNewEntity,
  const occ::handle<Standard_Transient>&      theSharing)
{
  occ::handle<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface> aSharing =
    occ::down_cast<StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface>(theSharing);
  bool                                                                   isReplaced = false;
  occ::handle<NCollection_HArray2<occ::handle<StepGeom_CartesianPoint>>> aControlPoints =
    aSharing->ControlPointsList();
  for (int anIndexI = aControlPoints->LowerRow(); anIndexI <= aControlPoints->UpperRow();
       ++anIndexI)
  {
    for (int anIndexJ = aControlPoints->LowerCol(); anIndexJ <= aControlPoints->UpperCol();
         ++anIndexJ)
    {
      if (aControlPoints->Value(anIndexI, anIndexJ) == theOldEntity)
      {
        aControlPoints->SetValue(anIndexI, anIndexJ, theNewEntity);
        isReplaced = true;
      }
    }
  }
  return isReplaced;
}
