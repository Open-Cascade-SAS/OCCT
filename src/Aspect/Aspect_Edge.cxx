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

//-Design	Declaration des variables specifiques aux aretes

//-Warning	Une arete est definie par :
//		- les 2 sommets references par des indices
//		- la visibilite

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_Edge.ixx>

//-Aliases

//-Global data definitions

//	-- les indices des sommets extremite
//	MyBegin		:	Standard_Integer;
//	MyEnd		:	Standard_Integer;

//	-- la visibilite
//	MyVisibility	:	TypeOfEdge;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_Edge::Aspect_Edge () {

	MyBegin		= 0;
	MyEnd		= 0;
	MyVisibility	= Aspect_TOE_INVISIBLE;

}

Aspect_Edge::Aspect_Edge (const Standard_Integer AIndex1, const Standard_Integer AIndex2, const Aspect_TypeOfEdge AType) {

	if (AIndex1 == AIndex2)
		Aspect_EdgeDefinitionError::Raise ("Bad index for the edge");

	MyBegin		= AIndex1;
	MyEnd		= AIndex2;
	MyVisibility	= AType;

}

void Aspect_Edge::SetValues (const Standard_Integer AIndex1, const Standard_Integer AIndex2, const Aspect_TypeOfEdge AType) {

	if (AIndex1 == AIndex2)
		Aspect_EdgeDefinitionError::Raise ("Bad index for the edge");

	MyBegin		= AIndex1;
	MyEnd		= AIndex2;
	MyVisibility	= AType;

}

void Aspect_Edge::Values (Standard_Integer& AIndex1, Standard_Integer& AIndex2, Aspect_TypeOfEdge& AType) const {

	AIndex1	= MyBegin;
	AIndex2	= MyEnd;
	AType	= MyVisibility;

}

Standard_Integer Aspect_Edge::FirstIndex () const {

	return (MyBegin);

}

Standard_Integer Aspect_Edge::LastIndex () const {

	return (MyEnd);

}

Aspect_TypeOfEdge Aspect_Edge::Type () const {

	return (MyVisibility);

}
