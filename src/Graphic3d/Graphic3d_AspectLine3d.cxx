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

//-Design	Declaration of variables specific to the context
//		of tracing of lines 3d

//-Warning	Context of tracing of lines 3d inherits the context
//		defined by :
//		- the color
//		- the type of trait
//		- the thickness

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectLine3d.ixx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_AspectLine3d::Graphic3d_AspectLine3d () {
}

// (AColor, AType, AWidth)
// because  AspectLine3d inherits AspectLine and it is necessary to call
// initialisation of AspectLine with AColor, AType, AWidth.

Graphic3d_AspectLine3d::Graphic3d_AspectLine3d (const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth):Aspect_AspectLine (AColor, AType, AWidth) {}
