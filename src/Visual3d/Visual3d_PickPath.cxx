// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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




//-Version	

//-Design	Declaration of variables specific to the classe
//		describing a marking.

//-Warning	Marking is defined by :
//		- vector (Elem_Num, Pick_Id, Structure)

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_PickPath.ixx>

//-Aliases

//-Global data definitions

//	-- le vecteur
//	MyElementNumber		:	Standard_Integer;
//	MyPickId		:	Standard_Integer;
//	MyStructure		:	Structure;

//	-- les flags pour definition
//	MyElementNumberIsDef	:	Standard_Boolean;
//	MyPickIdIsDef		:	Standard_Boolean;
//	MyStructureIsDef	:	Standard_Boolean;

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_PickPath::Visual3d_PickPath ():
MyElementNumberIsDef (Standard_False),
MyPickIdIsDef (Standard_False),
MyStructureIsDef (Standard_False) {
}

Visual3d_PickPath::Visual3d_PickPath (const Standard_Integer AnElemNumber, const Standard_Integer APickId, const Handle(Graphic3d_Structure)& AStructure):
MyElementNumber (AnElemNumber),
MyPickId (APickId),
MyStructure (AStructure),
MyElementNumberIsDef (Standard_True),
MyPickIdIsDef (Standard_True),
MyStructureIsDef (Standard_True) {
}

void Visual3d_PickPath::SetElementNumber (const Standard_Integer AnElemNumber) {

	MyElementNumber		= AnElemNumber;
	MyElementNumberIsDef	= Standard_True;

}

void Visual3d_PickPath::SetPickIdentifier (const Standard_Integer APickId) {

	MyPickId		= APickId;
	MyPickIdIsDef		= Standard_True;

}

void Visual3d_PickPath::SetStructIdentifier (const Handle(Graphic3d_Structure)& AStructure) {

	MyStructure		= AStructure;
	MyStructureIsDef	= Standard_True;

}

Standard_Integer Visual3d_PickPath::ElementNumber () const {

	if (!MyElementNumberIsDef)
		Visual3d_PickError::Raise ("No defined ElementNumber");

	return (MyElementNumber);

}

Standard_Integer Visual3d_PickPath::PickIdentifier () const {

	if (!MyPickIdIsDef)
		Visual3d_PickError::Raise ("No defined PickIdentifier");

	return (MyPickId);

}

Handle(Graphic3d_Structure) Visual3d_PickPath::StructIdentifier () const {

	if (!MyStructureIsDef)
		Visual3d_PickError::Raise ("No defined StructIdentifier");

	return (MyStructure);

}
