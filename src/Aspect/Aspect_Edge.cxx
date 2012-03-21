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
