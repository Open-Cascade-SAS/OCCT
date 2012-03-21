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

#include <StepShape_FaceSurface.ixx>


StepShape_FaceSurface::StepShape_FaceSurface ()  {}

void StepShape_FaceSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds)
{

	StepShape_Face::Init(aName, aBounds);
}

void StepShape_FaceSurface::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFaceBound)& aBounds,
	const Handle(StepGeom_Surface)& aFaceGeometry,
	const Standard_Boolean aSameSense)
{
	// --- classe own fields ---
	faceGeometry = aFaceGeometry;
	sameSense = aSameSense;
	// --- classe inherited fields ---
	StepShape_Face::Init(aName, aBounds);
}


void StepShape_FaceSurface::SetFaceGeometry(const Handle(StepGeom_Surface)& aFaceGeometry)
{
	faceGeometry = aFaceGeometry;
}

Handle(StepGeom_Surface) StepShape_FaceSurface::FaceGeometry() const
{
	return faceGeometry;
}

void StepShape_FaceSurface::SetSameSense(const Standard_Boolean aSameSense)
{
	sameSense = aSameSense;
}

Standard_Boolean StepShape_FaceSurface::SameSense() const
{
	return sameSense;
}
