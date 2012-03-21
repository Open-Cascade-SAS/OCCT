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

#include <StepShape_OrientedClosedShell.ixx>


StepShape_OrientedClosedShell::StepShape_OrientedClosedShell ()  {}

void StepShape_OrientedClosedShell::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{

	StepShape_ConnectedFaceSet::Init(aName, aCfsFaces);
}

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

void StepShape_OrientedClosedShell::SetCfsFaces(const Handle(StepShape_HArray1OfFace)& aCfsFaces)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	cout << "Field is redefined, SetUp Forbidden" << endl;
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
