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

#include <StepRepr_RepresentationMap.ixx>


StepRepr_RepresentationMap::StepRepr_RepresentationMap ()  {}

void StepRepr_RepresentationMap::Init(
	const Handle(StepRepr_RepresentationItem)& aMappingOrigin,
	const Handle(StepRepr_Representation)& aMappedRepresentation)
{
	// --- classe own fields ---
	mappingOrigin = aMappingOrigin;
	mappedRepresentation = aMappedRepresentation;
}


void StepRepr_RepresentationMap::SetMappingOrigin(const Handle(StepRepr_RepresentationItem)& aMappingOrigin)
{
	mappingOrigin = aMappingOrigin;
}

Handle(StepRepr_RepresentationItem) StepRepr_RepresentationMap::MappingOrigin() const
{
	return mappingOrigin;
}

void StepRepr_RepresentationMap::SetMappedRepresentation(const Handle(StepRepr_Representation)& aMappedRepresentation)
{
	mappedRepresentation = aMappedRepresentation;
}

Handle(StepRepr_Representation) StepRepr_RepresentationMap::MappedRepresentation() const
{
	return mappedRepresentation;
}
