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

//-Design	Declaration des variables specifiques aux contextes
//		de reperage.

//-Warning	Un contexte de reperage est defini par :
//		- l'ouverture de la fenetre
//		- la profondeur demandee
//		- l'ordre de parcours des structures

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_ContextPick.ixx>

//-Aliases

//-Global data definitions

//	-- ouverture ou taille de la fenetre de reperage
//	MyAperture	:	Standard_Real;

//	-- profondeur ou nombre de structures a reperer
//	MyDepth		:	Standard_Integer;

//	-- ordre de parcours des structures reperees
//	MyOrder		:	TypeOfOrder;

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_ContextPick::Visual3d_ContextPick ():
MyAperture (4.0),
MyDepth (10),
MyOrder (Visual3d_TOO_TOPFIRST) {
}

Visual3d_ContextPick::Visual3d_ContextPick (const Standard_Real Aperture, const Standard_Integer Depth, const Visual3d_TypeOfOrder Order):
MyAperture (Aperture),
MyDepth (Depth),
MyOrder (Order) {

	if (Aperture <= 0.0)
		Visual3d_ContextPickDefinitionError::Raise
			("Bad value for PickAperture");

	if (Depth <= 0)
		Visual3d_ContextPickDefinitionError::Raise
			("Bad value for PickDepth");

}

void Visual3d_ContextPick::SetAperture (const Standard_Real Aperture) {

	if (Aperture <= 0.0)
		Visual3d_ContextPickDefinitionError::Raise
			("Bad value for PickAperture");

	MyAperture	= Aperture;

}

Standard_Real Visual3d_ContextPick::Aperture () const {

	return (MyAperture);

}

void Visual3d_ContextPick::SetDepth (const Standard_Integer Depth) {

	if (Depth <= 0)
		Visual3d_ContextPickDefinitionError::Raise 
			("Bad value for PickDepth");

	MyDepth		= Depth;
	
}

Standard_Integer Visual3d_ContextPick::Depth () const {

	return (MyDepth);

}

void Visual3d_ContextPick::SetOrder (const Visual3d_TypeOfOrder Order) {

	MyOrder		= Order;

}

Visual3d_TypeOfOrder Visual3d_ContextPick::Order () const {

	return (MyOrder);

}
