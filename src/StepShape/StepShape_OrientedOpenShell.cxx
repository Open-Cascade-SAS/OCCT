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
#include <StepShape_Face.hxx>
#include <StepShape_OpenShell.hxx>
#include <StepShape_OrientedOpenShell.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedOpenShell,StepShape_OpenShell)

StepShape_OrientedOpenShell::StepShape_OrientedOpenShell ()  {}

void StepShape_OrientedOpenShell::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_OpenShell)& aOpenShellElement,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	openShellElement = aOpenShellElement;
	orientation = aOrientation;
	// --- classe inherited fields ---
	Handle(StepShape_HArray1OfFace) aCfsFaces;
	aCfsFaces.Nullify();
	StepShape_ConnectedFaceSet::Init(aName, aCfsFaces);
}


void StepShape_OrientedOpenShell::SetOpenShellElement(const Handle(StepShape_OpenShell)& aOpenShellElement)
{
	openShellElement = aOpenShellElement;
}

Handle(StepShape_OpenShell) StepShape_OrientedOpenShell::OpenShellElement() const
{
	return openShellElement;
}

void StepShape_OrientedOpenShell::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_OrientedOpenShell::Orientation() const
{
	return orientation;
}

void StepShape_OrientedOpenShell::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& /*aCfsFaces*/)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	cout << "Field is redefined, SetUp Forbidden" << endl;
}

Handle(StepShape_HArray1OfFace) StepShape_OrientedOpenShell::CfsFaces() const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return openShellElement->CfsFaces();
}

Handle(StepShape_Face) StepShape_OrientedOpenShell::CfsFacesValue(const Standard_Integer num) const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return openShellElement->CfsFacesValue(num);
}

Standard_Integer StepShape_OrientedOpenShell::NbCfsFaces () const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return openShellElement->NbCfsFaces();
}
