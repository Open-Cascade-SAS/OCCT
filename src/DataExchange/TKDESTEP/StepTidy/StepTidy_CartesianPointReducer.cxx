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
  const Handle(XSControl_WorkSession)& theWS)
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_Axis2Placement3d) aSharing =
    Handle(StepGeom_Axis2Placement3d)::DownCast(theSharing);
  if (aSharing->Location() == theOldEntity)
  {
    aSharing->SetLocation(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceVertexPoint(
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepShape_VertexPoint) aSharing = Handle(StepShape_VertexPoint)::DownCast(theSharing);
  if (aSharing->VertexGeometry() == theOldEntity)
  {
    aSharing->SetVertexGeometry(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceGeometricCurveSet(
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepShape_GeometricSet) aSharing   = Handle(StepShape_GeometricSet)::DownCast(theSharing);
  bool                           isReplaced = false;
  for (auto& anElement : *aSharing->Elements())
  {
    const Handle(StepGeom_Point) aCurrentPoint = anElement.Point();
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepVisual_PresentationLayerAssignment) aSharing =
    Handle(StepVisual_PresentationLayerAssignment)::DownCast(theSharing);
  bool isReplaced = false;
  for (auto& anAssignedItem : *aSharing->AssignedItems())
  {
    const Handle(StepRepr_RepresentationItem) aRepItem = anAssignedItem.RepresentationItem();
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
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

bool StepTidy_CartesianPointReducer::replaceBSplineCurveWithKnots(
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_BSplineCurveWithKnots) aSharing =
    Handle(StepGeom_BSplineCurveWithKnots)::DownCast(theSharing);
  bool                                     isReplaced     = false;
  Handle(StepGeom_HArray1OfCartesianPoint) aControlPoints = aSharing->ControlPointsList();
  for (Standard_Integer anIndex = aControlPoints->Lower(); anIndex <= aControlPoints->Upper();
       ++anIndex)
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_Line) aSharing = Handle(StepGeom_Line)::DownCast(theSharing);
  if (aSharing->Pnt() == theOldEntity)
  {
    aSharing->SetPnt(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceBSplineSurfaceWithKnots(
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_BSplineSurfaceWithKnots) aSharing =
    Handle(StepGeom_BSplineSurfaceWithKnots)::DownCast(theSharing);
  bool                                     isReplaced     = false;
  Handle(StepGeom_HArray2OfCartesianPoint) aControlPoints = aSharing->ControlPointsList();
  for (Standard_Integer anIndexI = aControlPoints->LowerRow();
       anIndexI <= aControlPoints->UpperRow();
       ++anIndexI)
  {
    for (Standard_Integer anIndexJ = aControlPoints->LowerCol();
         anIndexJ <= aControlPoints->UpperCol();
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_Axis1Placement) aSharing = Handle(StepGeom_Axis1Placement)::DownCast(theSharing);
  if (aSharing->Location() == theOldEntity)
  {
    aSharing->SetLocation(theNewEntity);
    return true;
  }
  return false;
}

//==================================================================================================

bool StepTidy_CartesianPointReducer::replaceRepresentation(
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepRepr_Representation) aSharing = Handle(StepRepr_Representation)::DownCast(theSharing);
  bool                            isReplaced           = false;
  Handle(StepRepr_HArray1OfRepresentationItem) anItems = aSharing->Items();
  for (Standard_Integer anIndex = 1; anIndex <= aSharing->NbItems(); ++anIndex)
  {
    const Handle(StepRepr_RepresentationItem) aRepItem = anItems->Value(anIndex);
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve) aSharing =
    Handle(StepGeom_BSplineCurveWithKnotsAndRationalBSplineCurve)::DownCast(theSharing);
  bool                                     isReplaced     = false;
  Handle(StepGeom_HArray1OfCartesianPoint) aControlPoints = aSharing->ControlPointsList();
  for (Standard_Integer anIndex = aControlPoints->Lower(); anIndex <= aControlPoints->Upper();
       ++anIndex)
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
  const Handle(StepGeom_CartesianPoint)& theOldEntity,
  const Handle(StepGeom_CartesianPoint)& theNewEntity,
  Handle(Standard_Transient)             theSharing)
{
  Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) aSharing =
    Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface)::DownCast(theSharing);
  bool                                     isReplaced     = false;
  Handle(StepGeom_HArray2OfCartesianPoint) aControlPoints = aSharing->ControlPointsList();
  for (Standard_Integer anIndexI = aControlPoints->LowerRow();
       anIndexI <= aControlPoints->UpperRow();
       ++anIndexI)
  {
    for (Standard_Integer anIndexJ = aControlPoints->LowerCol();
         anIndexJ <= aControlPoints->UpperCol();
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
