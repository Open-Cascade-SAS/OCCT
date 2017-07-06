// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_ViewVolume.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ViewVolume,Standard_Transient)

StepVisual_ViewVolume::StepVisual_ViewVolume ()  {}

void StepVisual_ViewVolume::Init(
	const StepVisual_CentralOrParallel aProjectionType,
	const Handle(StepGeom_CartesianPoint)& aProjectionPoint,
	const Standard_Real aViewPlaneDistance,
	const Standard_Real aFrontPlaneDistance,
	const Standard_Boolean aFrontPlaneClipping,
	const Standard_Real aBackPlaneDistance,
	const Standard_Boolean aBackPlaneClipping,
	const Standard_Boolean aViewVolumeSidesClipping,
	const Handle(StepVisual_PlanarBox)& aViewWindow)
{
	// --- classe own fields ---
	projectionType = aProjectionType;
	projectionPoint = aProjectionPoint;
	viewPlaneDistance = aViewPlaneDistance;
	frontPlaneDistance = aFrontPlaneDistance;
	frontPlaneClipping = aFrontPlaneClipping;
	backPlaneDistance = aBackPlaneDistance;
	backPlaneClipping = aBackPlaneClipping;
	viewVolumeSidesClipping = aViewVolumeSidesClipping;
	viewWindow = aViewWindow;
}


void StepVisual_ViewVolume::SetProjectionType(const StepVisual_CentralOrParallel aProjectionType)
{
	projectionType = aProjectionType;
}

StepVisual_CentralOrParallel StepVisual_ViewVolume::ProjectionType() const
{
	return projectionType;
}

void StepVisual_ViewVolume::SetProjectionPoint(const Handle(StepGeom_CartesianPoint)& aProjectionPoint)
{
	projectionPoint = aProjectionPoint;
}

Handle(StepGeom_CartesianPoint) StepVisual_ViewVolume::ProjectionPoint() const
{
	return projectionPoint;
}

void StepVisual_ViewVolume::SetViewPlaneDistance(const Standard_Real aViewPlaneDistance)
{
	viewPlaneDistance = aViewPlaneDistance;
}

Standard_Real StepVisual_ViewVolume::ViewPlaneDistance() const
{
	return viewPlaneDistance;
}

void StepVisual_ViewVolume::SetFrontPlaneDistance(const Standard_Real aFrontPlaneDistance)
{
	frontPlaneDistance = aFrontPlaneDistance;
}

Standard_Real StepVisual_ViewVolume::FrontPlaneDistance() const
{
	return frontPlaneDistance;
}

void StepVisual_ViewVolume::SetFrontPlaneClipping(const Standard_Boolean aFrontPlaneClipping)
{
	frontPlaneClipping = aFrontPlaneClipping;
}

Standard_Boolean StepVisual_ViewVolume::FrontPlaneClipping() const
{
	return frontPlaneClipping;
}

void StepVisual_ViewVolume::SetBackPlaneDistance(const Standard_Real aBackPlaneDistance)
{
	backPlaneDistance = aBackPlaneDistance;
}

Standard_Real StepVisual_ViewVolume::BackPlaneDistance() const
{
	return backPlaneDistance;
}

void StepVisual_ViewVolume::SetBackPlaneClipping(const Standard_Boolean aBackPlaneClipping)
{
	backPlaneClipping = aBackPlaneClipping;
}

Standard_Boolean StepVisual_ViewVolume::BackPlaneClipping() const
{
	return backPlaneClipping;
}

void StepVisual_ViewVolume::SetViewVolumeSidesClipping(const Standard_Boolean aViewVolumeSidesClipping)
{
	viewVolumeSidesClipping = aViewVolumeSidesClipping;
}

Standard_Boolean StepVisual_ViewVolume::ViewVolumeSidesClipping() const
{
	return viewVolumeSidesClipping;
}

void StepVisual_ViewVolume::SetViewWindow(const Handle(StepVisual_PlanarBox)& aViewWindow)
{
	viewWindow = aViewWindow;
}

Handle(StepVisual_PlanarBox) StepVisual_ViewVolume::ViewWindow() const
{
	return viewWindow;
}
