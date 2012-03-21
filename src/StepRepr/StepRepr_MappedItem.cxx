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

#include <StepRepr_MappedItem.ixx>


StepRepr_MappedItem::StepRepr_MappedItem ()  {}

void StepRepr_MappedItem::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepRepr_MappedItem::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepRepr_RepresentationMap)& aMappingSource,
	const Handle(StepRepr_RepresentationItem)& aMappingTarget)
{
	// --- classe own fields ---
	mappingSource = aMappingSource;
	mappingTarget = aMappingTarget;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepRepr_MappedItem::SetMappingSource(const Handle(StepRepr_RepresentationMap)& aMappingSource)
{
	mappingSource = aMappingSource;
}

Handle(StepRepr_RepresentationMap) StepRepr_MappedItem::MappingSource() const
{
	return mappingSource;
}

void StepRepr_MappedItem::SetMappingTarget(const Handle(StepRepr_RepresentationItem)& aMappingTarget)
{
	mappingTarget = aMappingTarget;
}

Handle(StepRepr_RepresentationItem) StepRepr_MappedItem::MappingTarget() const
{
	return mappingTarget;
}
