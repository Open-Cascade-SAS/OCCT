
// File		Aspect_GenId.cxx
// Created	Mai 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

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
