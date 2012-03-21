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

#include <StepShape_OrientedOpenShell.ixx>


StepShape_OrientedOpenShell::StepShape_OrientedOpenShell ()  {}

void StepShape_OrientedOpenShell::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{

	StepShape_ConnectedFaceSet::Init(aName, aCfsFaces);
}

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

void StepShape_OrientedOpenShell::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& aCfsFaces)
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
