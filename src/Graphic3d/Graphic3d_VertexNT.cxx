// Created on: 1997-08-27
// Created by: PCT
// Copyright (c) 1997-1999 Matra Datavision
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

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees et sa normale et des coordonnees texture

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexNT.ixx>

//-Aliases

//-Global data definitions

//	-- la normale au point
//      MyTX, MyTY      :       Standard_Real;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexNT::Graphic3d_VertexNT ():
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0),
MyTX(0.0),
MyTY(0.0) {
}

Graphic3d_VertexNT::Graphic3d_VertexNT (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ,const Standard_Real ATX,const Standard_Real ATY,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0,FlagNormalise),
MyTX(ATX),
MyTY(ATY) {
}

Graphic3d_VertexNT::Graphic3d_VertexNT (const Graphic3d_Vertex& APoint, const Graphic3d_Vector& AVector,const Standard_Real ATX,const Standard_Real ATY,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (APoint, AVector,FlagNormalise),
MyTX(ATX),
MyTY(ATY) {
}

void Graphic3d_VertexNT::SetTextureCoordinate(const Standard_Real ATX,const Standard_Real ATY) 
{
  MyTX = ATX;
  MyTY = ATY;
}

void Graphic3d_VertexNT::TextureCoordinate(Standard_Real& ATX,Standard_Real& ATY) const
{
  ATX = MyTX;
  ATY = MyTY;
}
