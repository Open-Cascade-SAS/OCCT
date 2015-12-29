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
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Line,StepGeom_Curve)

StepGeom_Line::StepGeom_Line ()  {}

void StepGeom_Line::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepGeom_CartesianPoint)& aPnt,
	const Handle(StepGeom_Vector)& aDir)
{
	// --- classe own fields ---
	pnt = aPnt;
	dir = aDir;
	// --- classe inherited fields ---
	StepRepr_RepresentationItem::Init(aName);
}


void StepGeom_Line::SetPnt(const Handle(StepGeom_CartesianPoint)& aPnt)
{
	pnt = aPnt;
}

Handle(StepGeom_CartesianPoint) StepGeom_Line::Pnt() const
{
	return pnt;
}

void StepGeom_Line::SetDir(const Handle(StepGeom_Vector)& aDir)
{
	dir = aDir;
}

Handle(StepGeom_Vector) StepGeom_Line::Dir() const
{
	return dir;
}
