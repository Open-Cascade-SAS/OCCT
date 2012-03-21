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

//-Design	Declaration des variables specifiques aux fonds d'ecran.

//-Warning	Un fond d'ecran est defini par une couleur.

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_Background.ixx>

//-Aliases

//-Global data definitions

//	-- la couleur associee au fond d'ecran
//	MyColor	:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_Background::Aspect_Background () {

Quantity_Color MatraGray (Quantity_NOC_MATRAGRAY);

	MyColor	= MatraGray;

}

Aspect_Background::Aspect_Background (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

void Aspect_Background::SetColor (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

Quantity_Color Aspect_Background::Color () const {

	return (MyColor);

}
