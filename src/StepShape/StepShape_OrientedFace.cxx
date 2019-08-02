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
#include <StepShape_FaceBound.hxx>
#include <StepShape_OrientedFace.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_OrientedFace,StepShape_Face)

StepShape_OrientedFace::StepShape_OrientedFace ()  {}

void StepShape_OrientedFace::Init(
	const Handle(TCollection_HAsciiString)& aName,
	const Handle(StepShape_Face)& aFaceElement,
	const Standard_Boolean aOrientation)
{
	// --- classe own fields ---
	faceElement = aFaceElement;
	orientation = aOrientation;
	// --- classe inherited fields ---
	Handle(StepShape_HArray1OfFaceBound) aBounds;
	aBounds.Nullify();
	StepShape_Face::Init(aName, aBounds);
}


void StepShape_OrientedFace::SetFaceElement(const Handle(StepShape_Face)& aFaceElement)
{
	faceElement = aFaceElement;
}

Handle(StepShape_Face) StepShape_OrientedFace::FaceElement() const
{
	return faceElement;
}

void StepShape_OrientedFace::SetOrientation(const Standard_Boolean aOrientation)
{
	orientation = aOrientation;
}

Standard_Boolean StepShape_OrientedFace::Orientation() const
{
	return orientation;
}

void StepShape_OrientedFace::SetBounds(const Handle(StepShape_HArray1OfFaceBound)& /*aBounds*/)
{
	// WARNING : the field is redefined.
	// field set up forbidden.
	std::cout << "Field is redefined, SetUp Forbidden" << std::endl;
}

Handle(StepShape_HArray1OfFaceBound) StepShape_OrientedFace::Bounds() const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return faceElement->Bounds();

}

Handle(StepShape_FaceBound) StepShape_OrientedFace::BoundsValue(const Standard_Integer num) const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return faceElement->BoundsValue(num);
}

Standard_Integer StepShape_OrientedFace::NbBounds () const
{
  // WARNING : the field is redefined.
  // method body is not yet automaticly wrote
  // Attention, cette modif. est juste pour la compilation  
  return faceElement->NbBounds();
}
