
// File		Visual3d_PickDescriptor.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration of variables specific to the class
//		describing marking.

//-Warning	A return of marking is defined by :
//		- sequence of (Elem_Num, Pick_Id, Structure)
//		- depth

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Visual3d_PickDescriptor.ixx>

//-Aliases

//-Global data definitions

//	-- sequence reperee
//	MyPickPathSequence	:	HSequenceOfPickPath;

//	-- contexte picking de reperage associe
//	MyContext		:	ContextPick;

//-Constructors

//-Destructors

//-Methods, in order

Visual3d_PickDescriptor::Visual3d_PickDescriptor (const Visual3d_ContextPick& CTXP):
MyContext (CTXP) {

	MyPickPathSequence	= new Visual3d_HSequenceOfPickPath ();

}

void Visual3d_PickDescriptor::AddPickPath (const Visual3d_PickPath& APickPath) {

	if (MyContext.Depth () > MyPickPathSequence->Length ())
		MyPickPathSequence->Append (APickPath);

}

void Visual3d_PickDescriptor::Clear () {

	MyPickPathSequence->Clear ();

}

Standard_Integer Visual3d_PickDescriptor::Depth () const {

	return (MyPickPathSequence->Length ());

}

Handle(Graphic3d_Structure) Visual3d_PickDescriptor::TopStructure () const {

Visual3d_PickPath Result;

	if (MyPickPathSequence->IsEmpty ())
		Visual3d_PickError::Raise ("PickDescriptor empty");

	switch (MyContext.Order ()) {

		case Visual3d_TOO_TOPFIRST :
			// MyPickPathSequence->First returns the first
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().First ();
		break;

		case Visual3d_TOO_BOTTOMFIRST :
			// MyPickPathSequence->Last returns the last
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().Last ();
		break;

	}
	return (Result.StructIdentifier ());
}

Standard_Integer Visual3d_PickDescriptor::TopPickId () const {

Visual3d_PickPath Result;

	if (MyPickPathSequence->IsEmpty ())
		Visual3d_PickError::Raise ("PickDescriptor empty");

	switch (MyContext.Order ()) {

		case Visual3d_TOO_TOPFIRST :
			// MyPickPathSequence->First returns the first
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().First ();
		break;

		case Visual3d_TOO_BOTTOMFIRST :
			// MyPickPathSequence->Last returns the last
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().Last ();
		break;

	}
	return (Result.PickIdentifier ());
}

Standard_Integer Visual3d_PickDescriptor::TopElementNumber () const {

Visual3d_PickPath Result;

	if (MyPickPathSequence->IsEmpty ())
		Visual3d_PickError::Raise ("PickDescriptor empty");

	switch (MyContext.Order ()) {

		case Visual3d_TOO_TOPFIRST :
			// MyPickPathSequence->First returns the first
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().First ();
		break;

		case Visual3d_TOO_BOTTOMFIRST :
			// MyPickPathSequence->Last returns the last
			// PickPath found in the sequence.
			Result	= MyPickPathSequence->Sequence().Last ();
		break;

	}
	return (Result.ElementNumber ());
}

Handle(Visual3d_HSequenceOfPickPath) Visual3d_PickDescriptor::PickPath () const {

	return (Handle(Visual3d_HSequenceOfPickPath)::DownCast
					(MyPickPathSequence->ShallowCopy ()));

}
