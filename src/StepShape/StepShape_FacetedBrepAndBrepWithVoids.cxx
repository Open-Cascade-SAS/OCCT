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

#include <StepShape_FacetedBrepAndBrepWithVoids.ixx>

#include <StepShape_FacetedBrep.hxx>

#include <StepShape_BrepWithVoids.hxx>


StepShape_FacetedBrepAndBrepWithVoids::StepShape_FacetedBrepAndBrepWithVoids ()  {}

void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter)
{
  
  StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}

void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter,
 const Handle(StepShape_FacetedBrep)& aFacetedBrep,
 const Handle(StepShape_BrepWithVoids)& aBrepWithVoids)
{
  // --- class own fields ---
  facetedBrep = aFacetedBrep;
  brepWithVoids = aBrepWithVoids;
  // --- class inherited fields ---
  StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}


void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter,
 const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	// --- class inherited fields ---

	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
	
	// --- ANDOR componant fields ---
	
	brepWithVoids = new StepShape_BrepWithVoids();
	brepWithVoids->Init(aName, aOuter, aVoids);
	
	// --- ANDOR componant fields ---
	
	facetedBrep = new StepShape_FacetedBrep();
	facetedBrep->Init(aName, aOuter);
}


void StepShape_FacetedBrepAndBrepWithVoids::SetFacetedBrep(const Handle(StepShape_FacetedBrep)& aFacetedBrep)
{
	facetedBrep = aFacetedBrep;
}

Handle(StepShape_FacetedBrep) StepShape_FacetedBrepAndBrepWithVoids::FacetedBrep() const
{
	return facetedBrep;
}

void StepShape_FacetedBrepAndBrepWithVoids::SetBrepWithVoids(const Handle(StepShape_BrepWithVoids)& aBrepWithVoids)
{
	brepWithVoids = aBrepWithVoids;
}

Handle(StepShape_BrepWithVoids) StepShape_FacetedBrepAndBrepWithVoids::BrepWithVoids() const
{
	return brepWithVoids;
}

	//--- Specific Methods for AND class field access ---


void StepShape_FacetedBrepAndBrepWithVoids::SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	brepWithVoids->SetVoids(aVoids);
}

Handle(StepShape_HArray1OfOrientedClosedShell) StepShape_FacetedBrepAndBrepWithVoids::Voids() const
{
	return brepWithVoids->Voids();
}

Handle(StepShape_OrientedClosedShell) StepShape_FacetedBrepAndBrepWithVoids::VoidsValue(const Standard_Integer num) const
{
	return brepWithVoids->VoidsValue(num);
}

Standard_Integer StepShape_FacetedBrepAndBrepWithVoids::NbVoids () const
{
	return brepWithVoids->NbVoids();
}

	//--- Specific Methods for AND class field access ---

