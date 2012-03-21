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
