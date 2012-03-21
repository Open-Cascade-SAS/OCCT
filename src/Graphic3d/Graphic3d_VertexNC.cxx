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


// Modified:	03-02-98 : FMN ; Add Flag Normal


//-Version	

//-Design	Declaration of variables specific to points

//-Warning	A point is defined by its coordinates, normal 
//		and color

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexNC.ixx>

//-Aliases

//-Global data definitions

//	-- the color
//	MyColor		:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexNC::Graphic3d_VertexNC ():
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0) {

Quantity_Color AColor (Quantity_NOC_WHITE);

	MyColor	= AColor;

}

// :(AX, AY, AZ) as VertexNC inherits VertexN and it is necessary to call
// constructor of VertexN with AX, AY, AZ, ANX, ANY and ANZ to
// update fields.

Graphic3d_VertexNC::Graphic3d_VertexNC (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ, const Quantity_Color& AColor,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (AX, AY, AZ, ANX, ANY, ANZ,FlagNormalise),
MyColor (AColor) {
}

//  :(APoint, AVector) as VertexNC inherits VertexN and it is necessary to call
// constructor of VertexN with APoint and AVector to
// update fields.

Graphic3d_VertexNC::Graphic3d_VertexNC (const Graphic3d_Vertex& APoint, const Graphic3d_Vector& AVector, const Quantity_Color& AColor,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (APoint, AVector,FlagNormalise),
MyColor (AColor) {
}

Quantity_Color Graphic3d_VertexNC::Color () const {

	return (MyColor);

}

void Graphic3d_VertexNC::SetColor (const Quantity_Color& ColorNew) {

	MyColor	= ColorNew;

}
