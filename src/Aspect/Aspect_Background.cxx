// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//-Version	
//-Design	Declaration des variables specifiques aux fonds d'ecran.
//-Warning	Un fond d'ecran est defini par une couleur.
//-References	
//-Language	C++ 2.0
//-Declarations
// for the class

#include <Aspect_Background.hxx>
#include <Quantity_Color.hxx>

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
