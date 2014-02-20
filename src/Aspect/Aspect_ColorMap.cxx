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
