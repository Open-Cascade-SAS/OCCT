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
#include <StepShape_ClosedShell.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedClosedShell,StepShape_ClosedShell)

StepShape_OrientedClosedShell::StepShape_OrientedClosedShell ()  {}

void StepShape_OrientedClosedShell::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_ClosedShell)& aClosedShellElement,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	closedShellElement = aClosedShellElement;
	orientation = aOrientation;
	// --- classe inherited fields ---
	Handle(StepShape_HArray1OfFace) aCfsFaces;
	aCfsFaces.Nullify();
	StepShape_ConnectedFaceSet::Init(aName, aCfsFaces);
}


void StepShape_OrientedClosedShell::SetClosedShellElement(const Handle(StepShape_ClosedShell)& aClosedShellElement)
{
	closedShellElement = aClosedShellElement;
}

Handle(StepShape_ClosedShell) StepShape_OrientedClosedShell::ClosedShellElement() const
{
	return closedShellElement;
}

void StepShape_OrientedClosedShell::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_OrientedClosedShell::Orientation() const
{
	return orientation;
}

void StepShape_OrientedClosedShell::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& /*aCfsFaces*/)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

Handle(StepShape_HArray1OfFace) StepShape_OrientedClosedShell::CfsFaces() const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation
  return closedShellElement->CfsFaces();
}

Handle(StepShape_Face) StepShape_OrientedClosedShell::CfsFacesValue(const Standard_Integer num) const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return closedShellElement->CfsFacesValue(num);
}

Standard_Integer StepShape_OrientedClosedShell::NbCfsFaces () const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return closedShellElement->NbCfsFaces();
}
