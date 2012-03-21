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
// Modified:	15-09-98 : CAL ; Modification champ definissant la normale
//		pour permettre l'entrelacement des coordonnees dans Optimizer.


//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees et sa normale

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexN.ixx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexN::Graphic3d_VertexN ():
Graphic3d_Vertex (0.0, 0.0, 0.0) {
	MyDX = 0.0,
	MyDY = 0.0,
	MyDZ = 1.0;
}

Graphic3d_VertexN::Graphic3d_VertexN (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ, const Standard_Boolean FlagNormalise):
Graphic3d_Vertex (AX, AY, AZ) {
	MyDX = (Standard_ShortReal ) ANX,
	MyDY = (Standard_ShortReal ) ANY,
	MyDZ = (Standard_ShortReal ) ANZ;
}

Graphic3d_VertexN::Graphic3d_VertexN (const Graphic3d_Vertex& APoint, const Graphic3d_Vector& AVector, const Standard_Boolean FlagNormalise):
Graphic3d_Vertex (APoint) {
Standard_Real ANX, ANY, ANZ;
	AVector.Coord (ANX, ANY, ANZ);
	MyDX = (Standard_ShortReal ) ANX,
	MyDY = (Standard_ShortReal ) ANY,
	MyDZ = (Standard_ShortReal ) ANZ;
}

void Graphic3d_VertexN::SetNormal (const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ, const Standard_Boolean FlagNormalise) {
	MyDX = (Standard_ShortReal ) ANX,
	MyDY = (Standard_ShortReal ) ANY,
	MyDZ = (Standard_ShortReal ) ANZ;
}

void Graphic3d_VertexN::Normal (Standard_Real& ANX, Standard_Real& ANY, Standard_Real& ANZ) const {
	ANX = MyDX,
	ANY = MyDY,
	ANZ = MyDZ;
}
