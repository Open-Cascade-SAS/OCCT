#include <Aspect_WidthMapEntry.ixx>
#include <Aspect_Units.hxx>

Aspect_WidthMapEntry::Aspect_WidthMapEntry() {

	SetPredefinedStyle(Aspect_WOL_THIN);
	MyIndexIsDef	= Standard_True;
	MyIndex		= 0;

}

Aspect_WidthMapEntry::Aspect_WidthMapEntry (const Standard_Integer index, const Aspect_WidthOfLine style) {

	SetPredefinedStyle(style);
	MyIndexIsDef	= Standard_True;
	MyIndex		= index;

}

Aspect_WidthMapEntry::Aspect_WidthMapEntry (const Standard_Integer index, const Quantity_Length width) {

	if( width < 0. )
		Aspect_BadAccess::Raise("Bad Line Width") ;

        MyType 		= Aspect_WOL_USERDEFINED ;
	MyTypeIsDef 	= Standard_True;
	MyIndexIsDef	= Standard_True;
	MyIndex		= index;
	MyWidth		= width;

}

Aspect_WidthMapEntry::Aspect_WidthMapEntry (const Aspect_WidthMapEntry& entry) {

	if ( !entry.MyTypeIsDef || !entry.MyIndexIsDef ) {
		Aspect_BadAccess::Raise("Unallocated WidthMapEntry") ;
	}
	else {
		MyTypeIsDef	= Standard_True;
		MyIndexIsDef	= Standard_True;
		MyIndex		= entry.MyIndex;
		MyType		= entry.MyType;
		MyWidth		= entry.MyWidth;
	}
}

void Aspect_WidthMapEntry::SetValue (const Standard_Integer index, const Aspect_WidthOfLine style) {

	SetPredefinedStyle(style);
	MyIndexIsDef	= Standard_True;
	MyIndex		= index;

}

void Aspect_WidthMapEntry::SetValue (const Standard_Integer index, const Quantity_Length width) {

	if( width < 0. )
		Aspect_BadAccess::Raise("Bad Line Width") ;

        MyType 		= Aspect_WOL_USERDEFINED ;
	MyTypeIsDef 	= Standard_True;
	MyWidth		= width ;
	MyIndexIsDef	= Standard_True;
	MyIndex		= index;

}

void Aspect_WidthMapEntry::SetValue (const Aspect_WidthMapEntry& entry) {

	if ( !entry.MyTypeIsDef || !entry.MyIndexIsDef ) {
		Aspect_BadAccess::Raise("Unallocated WidthMapEntry") ;
	}
	else {
		MyTypeIsDef	= Standard_True;
		MyIndexIsDef	= Standard_True;
		MyIndex		= entry.MyIndex;
		MyType		= entry.MyType;
		MyWidth		= entry.MyWidth;
	}

}

void Aspect_WidthMapEntry::SetType (const Aspect_WidthOfLine style) {

	SetPredefinedStyle(style);

}

void Aspect_WidthMapEntry::SetWidth (const Quantity_Length width) {

	if( width < 0. )
		Aspect_BadAccess::Raise("Bad Line Width") ;

        MyType 		= Aspect_WOL_USERDEFINED ;
	MyTypeIsDef 	= Standard_True;
	MyWidth		= width ;

}

Aspect_WidthOfLine Aspect_WidthMapEntry::Type () const {

	if ( !MyTypeIsDef || !MyIndexIsDef ) 
		Aspect_BadAccess::Raise("Unallocated WidthMapEntry") ;

	return MyType;

}

Standard_Real Aspect_WidthMapEntry::Width () const {

	if ( !MyTypeIsDef || !MyIndexIsDef ) 
		Aspect_BadAccess::Raise("Unallocated WidthMapEntry") ;

	return MyWidth;

}

void Aspect_WidthMapEntry::SetIndex (const Standard_Integer index) {

	MyTypeIsDef	= Standard_True;
	MyIndex		= index;

}

Standard_Integer Aspect_WidthMapEntry::Index () const {

	if ( !MyTypeIsDef || !MyIndexIsDef ) 
		Aspect_BadAccess::Raise("Unallocated WidthMapEntry");

	return MyIndex;

}

Standard_Boolean Aspect_WidthMapEntry::IsAllocated () const {

	return ( MyTypeIsDef && MyIndexIsDef ) ; 

}

void Aspect_WidthMapEntry::Free () {

	MyTypeIsDef	= Standard_False;
	MyIndexIsDef	= Standard_False;

}

void Aspect_WidthMapEntry::Dump () const {

  cout << " Aspect_WidthMapEntry::Dump ()\n";
  cout << "      MyTypeIsDef : " << (MyTypeIsDef ? "True\n" : "False\n");
  cout << "      MyIndexIsDef : " << (MyIndexIsDef ? "True\n" : "False\n");
  cout << "      TypeOfWidth : " << (Standard_Integer) MyType << "\n";//WNT
  cout << "      LineWidth : " << MyWidth << "\n";
  cout << flush;
}

void Aspect_WidthMapEntry::SetPredefinedStyle(const Aspect_WidthOfLine Style) {

        MyType = Style ;
	MyTypeIsDef = Standard_True;
        switch ( Style ) {
            case Aspect_WOL_USERDEFINED :
		Aspect_BadAccess::Raise("Bad Predefined Line Width Style") ;
            case Aspect_WOL_THIN:
		MyWidth = 0.25 MILLIMETER ;
                break ;
            case Aspect_WOL_MEDIUM :
		MyWidth = 0.5 MILLIMETER ;
                break ;
            case Aspect_WOL_THICK :
		MyWidth = 0.7 MILLIMETER ;
                break ;
            case Aspect_WOL_VERYTHICK :
		MyWidth = 1.5 MILLIMETER ;
                break ;
        }
}
