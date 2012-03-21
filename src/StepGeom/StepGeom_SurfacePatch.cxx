// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepGeom_SurfacePatch.ixx>


StepGeom_SurfacePatch::StepGeom_SurfacePatch ()  {}

void StepGeom_SurfacePatch::Init(
	const Handle(StepGeom_BoundedSurface)& aParentSurface,
	const StepGeom_TransitionCode aUTransition,
	const StepGeom_TransitionCode aVTransition,
	const Standard_Boolean aUSense,
	const Standard_Boolean aVSense)
{
	// --- classe own fields ---
	parentSurface = aParentSurface;
	uTransition = aUTransition;
	vTransition = aVTransition;
	uSense = aUSense;
	vSense = aVSense;
}


void StepGeom_SurfacePatch::SetParentSurface(const Handle(StepGeom_BoundedSurface)& aParentSurface)
{
	parentSurface = aParentSurface;
}

Handle(StepGeom_BoundedSurface) StepGeom_SurfacePatch::ParentSurface() const
{
	return parentSurface;
}

void StepGeom_SurfacePatch::SetUTransition(const StepGeom_TransitionCode aUTransition)
{
	uTransition = aUTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::UTransition() const
{
	return uTransition;
}

void StepGeom_SurfacePatch::SetVTransition(const StepGeom_TransitionCode aVTransition)
{
	vTransition = aVTransition;
}

StepGeom_TransitionCode StepGeom_SurfacePatch::VTransition() const
{
	return vTransition;
}

void StepGeom_SurfacePatch::SetUSense(const Standard_Boolean aUSense)
{
	uSense = aUSense;
}

Standard_Boolean StepGeom_SurfacePatch::USense() const
{
	return uSense;
}

void StepGeom_SurfacePatch::SetVSense(const Standard_Boolean aVSense)
{
	vSense = aVSense;
}

Standard_Boolean StepGeom_SurfacePatch::VSense() const
{
	return vSense;
}
