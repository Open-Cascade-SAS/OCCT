// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
