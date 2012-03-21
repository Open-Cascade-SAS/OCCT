// Created on: 1995-01-14
// Created by: GG
// Copyright (c) 1995-1999 Matra Datavision
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
