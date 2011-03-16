
// File         Aspect_MarkMapEntry.cxx
// Created      Janvier 1995
// Author       GG

//-Copyright    MatraDatavision 1991,1992,1993,1994

//-Version

//-Design       Declaration des variables specifiques aux Entries de 
//		Markers 

//-Warning      Une entry est definie par un index associee a un Marker style

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Aspect_MarkMapEntry.ixx>
#include <TColQuantity_Array1OfLength.hxx>
//-Aliases

//-Global data definitions

//      MyStyle      		: MarkerStyle from Aspect
//      MyIndex   		: Integer from Standard
//      MyStyleIsDef		: Boolean from Standard
//      MyIndexIsDef		: Boolean from Standard

//-Constructors

//-Destructors

//-Methods, in order

Aspect_MarkMapEntry::Aspect_MarkMapEntry() : 
MyStyle(),MyIndex(0),MyStyleIsDef(Standard_True),MyIndexIsDef(Standard_True) {
}

Aspect_MarkMapEntry::Aspect_MarkMapEntry (const Standard_Integer index, const Aspect_MarkerStyle &style) :
MyStyle(style),MyIndex(index),MyStyleIsDef(Standard_True),MyIndexIsDef(Standard_True) {
}

Aspect_MarkMapEntry::Aspect_MarkMapEntry (const Aspect_MarkMapEntry& entry) :
MyStyle(entry.MyStyle),MyIndex(entry.MyIndex),MyStyleIsDef(Standard_True),MyIndexIsDef(Standard_True) {

	if ( !entry.MyStyleIsDef || !entry.MyIndexIsDef ) {
		Aspect_BadAccess::Raise("Unallocated MarkMapEntry") ;
	}
}

void Aspect_MarkMapEntry::SetValue (const Standard_Integer index, const Aspect_MarkerStyle &style) {

	MyStyleIsDef	= Standard_True;
	MyIndexIsDef	= Standard_True;
	MyIndex		= index;
	MyStyle		= style;

}

void Aspect_MarkMapEntry::SetValue (const Aspect_MarkMapEntry& entry) {

	if ( !entry.MyStyleIsDef || !entry.MyIndexIsDef ) {
		Aspect_BadAccess::Raise("Unallocated MarkMapEntry") ;
	}
	else {
		MyStyleIsDef	= Standard_True;
		MyIndexIsDef	= Standard_True;
		MyIndex		= entry.MyIndex;
		MyStyle		= entry.MyStyle;
	}

}

void Aspect_MarkMapEntry::SetStyle (const Aspect_MarkerStyle &style) {

	MyStyleIsDef	= Standard_True;
	MyStyle		= style;

}

const Aspect_MarkerStyle& Aspect_MarkMapEntry::Style () const {

	if ( !MyStyleIsDef || !MyIndexIsDef ) 
		Aspect_BadAccess::Raise("Unallocated MarkMapEntry") ;

	return MyStyle;

}

void Aspect_MarkMapEntry::SetIndex (const Standard_Integer index) {

	MyStyleIsDef	= Standard_True;
	MyIndex		= index;

}

Standard_Integer Aspect_MarkMapEntry::Index () const {

	if ( !MyStyleIsDef || !MyIndexIsDef ) 
		Aspect_BadAccess::Raise("Unallocated MarkMapEntry");

	return MyIndex;

}

Standard_Boolean Aspect_MarkMapEntry::IsAllocated () const {

	return ( MyStyleIsDef && MyIndexIsDef ) ; 

}

void Aspect_MarkMapEntry::Free () {

	MyStyleIsDef	= Standard_False;
	MyIndexIsDef	= Standard_False;

}

void Aspect_MarkMapEntry::Dump () const {

  Aspect_TypeOfMarker type = MyStyle.Type() ;
  Standard_Integer i,length = MyStyle.Length() ;
  Standard_Boolean draw = Standard_False ;
  Standard_Real X,Y;

  cout << " Aspect_MarkMapEntry::Dump ()\n";
  cout << "      MyStyleIsDef : " << (MyStyleIsDef) ? "True\n" : "False\n";
  cout << "      MyIndexIsDef : " << (MyIndexIsDef) ? "True\n" : "False\n";
  cout << "      MarkerStyle : " << (Standard_Integer) type << " Length : " << length << "\n";//WNT
  if( length ) {
    for( i=1 ; i<=length ; i++ ) 
	draw = MyStyle.Values(i,X,Y);
	cout << "		value("<< i << ") : " << X << "," << Y << "," << draw << "\n";
  }
  cout << flush;
}
