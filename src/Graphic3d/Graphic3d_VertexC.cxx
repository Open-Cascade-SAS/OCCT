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

//-Design	Declaration of variables specific to points

//-Warning	A point is defined by its coordinates and color

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexC.ixx>

//-Aliases

//-Global data definitions

//	-- point color
//	MyColor		:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexC::Graphic3d_VertexC () {
}

// :(AX, AY, AZ) as VertexC inherits Vertex and it is necessary to call
// constructor of Vertex with AX, AY and AZ to update fields MyX, MyY, MyZ.

Graphic3d_VertexC::Graphic3d_VertexC (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Quantity_Color& AColor):
Graphic3d_Vertex (AX, AY, AZ),
MyColor (AColor) {
}

// :(APoint) as VertexC inherits Vertex and it is necessary to call
// constructor of Vertex with APoint to update fields MyX, MyY, MyZ.

Graphic3d_VertexC::Graphic3d_VertexC (const Graphic3d_Vertex& APoint, const Quantity_Color& AColor):
Graphic3d_Vertex (APoint),
MyColor (AColor) {
}

Quantity_Color Graphic3d_VertexC::Color () const {

	return (MyColor);

}

void Graphic3d_VertexC::SetColor (const Quantity_Color& ColorNew) {

	MyColor	= ColorNew;

}
