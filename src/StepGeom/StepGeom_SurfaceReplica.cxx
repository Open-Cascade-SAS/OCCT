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
#include <StepGeom_CartesianTransformationOperator3d.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceReplica.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_SurfaceReplica,StepGeom_Surface)

StepGeom_SurfaceReplica::StepGeom_SurfaceReplica ()  {}

void StepGeom_SurfaceReplica::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_Surface)& aParentSurface,
	const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	// --- classe own fields ---
	parentSurface = aParentSurface;
	transformation = aTransformation;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_SurfaceReplica::SetParentSurface(const Handle(StepGeom_Surface)& aParentSurface)
{
	parentSurface = aParentSurface;
}

Handle(StepGeom_Surface) StepGeom_SurfaceReplica::ParentSurface() const
{
	return parentSurface;
}

void StepGeom_SurfaceReplica::SetTransformation(const Handle(StepGeom_CartesianTransformationOperator3d)& aTransformation)
{
	transformation = aTransformation;
}

Handle(StepGeom_CartesianTransformationOperator3d) StepGeom_SurfaceReplica::Transformation() const
{
	return transformation;
}
