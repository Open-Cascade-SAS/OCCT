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

#include <StepShape_ConnectedFaceSet.ixx>


StepShape_ConnectedFaceSet::StepShape_ConnectedFaceSet ()  {}

void StepShape_ConnectedFaceSet::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_ConnectedFaceSet::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{
	// --- classe own fields ---
	cfsFaces = aCfsFaces;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_ConnectedFaceSet::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{
	cfsFaces = aCfsFaces;
}

Handle(StepShape_HArray1OfFace) StepShape_ConnectedFaceSet::CfsFaces() const
{
	return cfsFaces;
}

Handle(StepShape_Face) StepShape_ConnectedFaceSet::CfsFacesValue(const Standard_Integer num) const
{
	return cfsFaces->Value(num);
}

Standard_Integer StepShape_ConnectedFaceSet::NbCfsFaces () const
{
	if (cfsFaces.IsNull()) return 0;
	return cfsFaces->Length();
}
