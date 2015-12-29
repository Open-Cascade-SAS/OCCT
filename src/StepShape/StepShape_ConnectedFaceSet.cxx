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
#include <StepShape_ConnectedFaceSet.hxx>
#include <StepShape_Face.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_ConnectedFaceSet,StepShape_TopologicalRepresentationItem)

StepShape_ConnectedFaceSet::StepShape_ConnectedFaceSet ()  {}

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
