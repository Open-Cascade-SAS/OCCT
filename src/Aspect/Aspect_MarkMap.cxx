// Created on: 1995-01-14
// Created by: GG
// Copyright (c) 1995-1999 Matra Datavision
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

//-Design       Declaration des variables specifiques aux Ensembles
//              de Markers

//-Warning      Une MarkMap est definie par un ensemble de MarkMapEntries

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Aspect_MarkMap.ixx>

//-Aliases

//-Global data definitions

//      mydata       : SequenceOfMarkMapEntry from Aspect is protected

//-Constructors

//-Destructors

//-Methods, in order

Aspect_MarkMap::Aspect_MarkMap( ) {
Aspect_MarkMapEntry theDefaultEntry;

	AddEntry(theDefaultEntry);
}

void Aspect_MarkMap::AddEntry (const Aspect_MarkMapEntry& AnEntry) {
Standard_Integer i,index = AnEntry.Index();
Aspect_MarkMapEntry theEntry;

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

Standard_Integer Aspect_MarkMap::AddEntry (const Aspect_MarkerStyle &aStyle) {
Aspect_MarkMapEntry theEntry ;
Standard_Integer i,maxindex = 0 ;
 
    for( i=1 ; i<=mydata.Length() ; i++ ) {
        theEntry = mydata.Value(i) ;
        maxindex = Max(maxindex,theEntry.Index()) ;
        if( theEntry.Style() == aStyle ) return theEntry.Index() ;
    }
 
    maxindex++ ;
    theEntry.SetValue(maxindex,aStyle) ;
    mydata.Append( theEntry ) ;
    return maxindex ;
}

Standard_Integer     Aspect_MarkMap::Size() const {

	return mydata.Length() ;
}

Standard_Integer     Aspect_MarkMap::Index(const Standard_Integer anIndex) const {

        if( anIndex < 1 || anIndex > Size() ) {
            Aspect_BadAccess::Raise ("Undefined markmap Index");
        }

        Aspect_MarkMapEntry theEntry = mydata.Value(anIndex) ;

        return theEntry.Index() ;
}

Aspect_MarkMapEntry Aspect_MarkMap::Entry (const Standard_Integer AnIndex) const {

	if( AnIndex < 1 || AnIndex > mydata.Length() )
		Aspect_BadAccess::Raise ("Aspect_MarkMap::Entry Bad Index");

	return mydata.Value(AnIndex);
}

void Aspect_MarkMap::Dump () const {

  Standard_Integer i ;

  cout << "Markmap Dump-->\n" ;

  for ( i = 1 ; i <= Size() ; i++ ) (Entry(i)).Dump() ;

  cout << "<--End Markmap Dump\n" ;
}
