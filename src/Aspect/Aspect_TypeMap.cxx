// File         Aspect_TypeMap.cxx
// Created      Septembre 1993
// Author       GG

//-Copyright    MatraDatavision 1991,1992

//-Version

//-Design       Declaration des variables specifiques aux Ensembles
//              de Type de traits

//-Warning      Une TypeMap est definie par un ensemble de TypeMapEntries

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Aspect_TypeMap.ixx>

//-Aliases

//-Global data definitions

//      mydata       : SequenceOfTypeMapEntry from Aspect is protected

//-Constructors

//-Destructors

//-Methods, in order

Aspect_TypeMap::Aspect_TypeMap( ) {
Aspect_TypeMapEntry theDefaultEntry;

	AddEntry(theDefaultEntry);
}

void Aspect_TypeMap::AddEntry (const Aspect_TypeMapEntry& AnEntry) {
Standard_Integer i,index = AnEntry.Index();
Aspect_TypeMapEntry theEntry;

        for( i=1 ; i<=mydata.Length() ; i++ ) {
            theEntry = mydata.Value(i);
            if( index == theEntry.Index() ) break;
        }

        if( i > mydata.Length() ) {
            mydata.Append( AnEntry );
        } else {
            mydata.SetValue(i,AnEntry);
        }
}

Standard_Integer Aspect_TypeMap::AddEntry (const Aspect_LineStyle &aStyle) {
Aspect_TypeMapEntry theEntry ;
Standard_Integer i,maxindex = 0 ;
 
    for( i=1 ; i<=mydata.Length() ; i++ ) {
        theEntry = mydata.Value(i) ;
        maxindex = Max(maxindex,theEntry.Index()) ;
        if( theEntry.Type() == aStyle ) return theEntry.Index() ;
    }
 
    maxindex++ ;
    theEntry.SetValue(maxindex,aStyle) ;
    mydata.Append( theEntry ) ;
    return maxindex ;
}

Standard_Integer     Aspect_TypeMap::Size() const {

	return mydata.Length() ;
}

Standard_Integer     Aspect_TypeMap::Index(const Standard_Integer anIndex) const {

        if( anIndex < 1 || anIndex > Size() ) {
            Aspect_BadAccess::Raise ("Undefined typemap Index");
        }

        Aspect_TypeMapEntry theEntry = mydata.Value(anIndex) ;

        return theEntry.Index() ;
}

const Aspect_TypeMapEntry& Aspect_TypeMap::Entry (const Standard_Integer AnIndex) const {

	if( AnIndex < 1 || AnIndex > mydata.Length() )
		Aspect_BadAccess::Raise ("Aspect_TypeMap::Entry Bad Index");

	return mydata.Value(AnIndex);
}

void Aspect_TypeMap::Dump () const {

  Standard_Integer i ;

  cout << "Typemap Dump-->\n" ;

  for ( i = 1 ; i <= Size() ; i++ ) (Entry(i)).Dump() ;

  cout << "<--End Typemap Dump\n" ;
}
