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

#include <StepShape_LoopAndPath.ixx>

#include <StepShape_Loop.hxx>

#include <StepShape_Path.hxx>


StepShape_LoopAndPath::StepShape_LoopAndPath ()  {}

void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName)
{

	StepRepr_RepresentationItem::Init(aName);
}

void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Loop)& aLoop,
	const Handle(StepShape_Path)& aPath)
{
	// --- classe own fields ---
	loop = aLoop;
	path = aPath;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepShape_LoopAndPath::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	// --- classe inherited fields ---

	StepRepr_RepresentationItem::Init(aName);

	// --- ANDOR componant fields ---

	loop = new StepShape_Loop();
	loop->Init(aName);

	// --- ANDOR componant fields ---

	path = new StepShape_Path();
	path->Init(aName, aEdgeList);
}


void StepShape_LoopAndPath::SetLoop(const Handle(StepShape_Loop)& aLoop)
{
	loop = aLoop;
}

Handle(StepShape_Loop) StepShape_LoopAndPath::Loop() const
{
	return loop;
}

void StepShape_LoopAndPath::SetPath(const Handle(StepShape_Path)& aPath)
{
	path = aPath;
}

Handle(StepShape_Path) StepShape_LoopAndPath::Path() const
{
	return path;
}

	//--- Specific Methods for AND classe field access ---


	//--- Specific Methods for AND classe field access ---


void StepShape_LoopAndPath::SetEdgeList(const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList)
{
	path->SetEdgeList(aEdgeList);
}

Handle(StepShape_HArray1OfOrientedEdge) StepShape_LoopAndPath::EdgeList() const
{
	return path->EdgeList();
}

Handle(StepShape_OrientedEdge) StepShape_LoopAndPath::EdgeListValue(const Standard_Integer num) const
{
	return path->EdgeListValue(num);
}

Standard_Integer StepShape_LoopAndPath::NbEdgeList () const
{
	return path->NbEdgeList();
}
