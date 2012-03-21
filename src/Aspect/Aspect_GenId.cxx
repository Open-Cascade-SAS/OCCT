// Created on: 1992-05-14
// Created by: NW,JPB,CAL
// Copyright (c) 1992-1999 Matra Datavision
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

//-Design	Declaration des variables specifiques aux identificateurs

//-Warning	Un identificateur est un entier.

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_GenId.ixx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Aspect_GenId::Aspect_GenId ():

	MyCount (INT_MAX/2 + 1),
	MyLength (INT_MAX/2 + 1),
	MyLowerBound (0),
	MyUpperBound (INT_MAX/2),
	MyFreeIds () {

}

Aspect_GenId::Aspect_GenId (const Standard_Integer Low, const Standard_Integer Up):MyFreeIds () {

	if (Low <= Up) {
		MyLowerBound	= Low;
		MyUpperBound	= Up;
		MyLength	= MyUpperBound - MyLowerBound + 1;
		MyCount		= MyLength;
	}
	else
		Aspect_IdentDefinitionError::Raise
			("GenId Create Error: Low > Up");

}

Standard_Integer Aspect_GenId::Available () const {

	return (MyCount);

}

void Aspect_GenId::Free () {

	MyCount	= MyLength;
	MyFreeIds.Clear ();

}

void Aspect_GenId::Free (const Standard_Integer Id) {

	if ( (Id >= MyLowerBound) && (Id <= MyUpperBound) )
		MyFreeIds.Prepend (Id);

}

Standard_Integer Aspect_GenId::Lower () const {

	return (MyLowerBound);

}

Standard_Integer Aspect_GenId::Next () {

	if (MyCount == 0)
		Aspect_IdentDefinitionError::Raise
			("GenId Next Error: Available == 0");

Standard_Integer Id;

	if (! MyFreeIds.IsEmpty ()) {
		Id	= MyFreeIds.First ();
		MyFreeIds.RemoveFirst ();
	}
	else {
		MyCount	--;
		Id	= MyLowerBound + MyLength - MyCount - 1;
	}

	return Id;

}

Standard_Integer Aspect_GenId::Upper () const {

	return (MyUpperBound);

}

//void Aspect_GenId::Assign (const Aspect_GenId& Other) {
//
//	MyLowerBound	= Other.Lower ();
//	MyUpperBound	= Other.Upper ();
//
//}
