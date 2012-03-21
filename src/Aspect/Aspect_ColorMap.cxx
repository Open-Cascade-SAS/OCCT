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


#include <Aspect_ColorMap.ixx>

Aspect_ColorMap::Aspect_ColorMap( const Aspect_TypeOfColorMap type ) {

	mytype		= type;
}

Aspect_TypeOfColorMap Aspect_ColorMap::Type() const {

	return mytype ;
}

Standard_Integer     Aspect_ColorMap::Size() const {

	return mydata.Length() ;
}

Standard_Integer     Aspect_ColorMap::Index(const Standard_Integer anIndex) const {

	if( anIndex < 1 || anIndex > Size() ) {
            Aspect_BadAccess::Raise ("Undefined colormap Index");
	}

	Aspect_ColorMapEntry theEntry = mydata.Value(anIndex) ;

	return theEntry.Index() ;
}

const Aspect_ColorMapEntry& Aspect_ColorMap::Entry (const Standard_Integer AnIndex) const {

		return mydata.Value(AnIndex);
}

void Aspect_ColorMap::Dump () const {

  Standard_Integer i ;

  cout << "Colormap Dump-->\n" ;

  for ( i = 1 ; i <= Size() ; i++ ) (Entry(i)).Dump() ;

  cout << "<--End Colormap Dump\n" ;
}
