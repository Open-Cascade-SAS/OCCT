// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Aspect_ColorMapEntry.ixx>

Aspect_ColorMapEntry::Aspect_ColorMapEntry() {

	myColorIsDef	= Standard_True;
	myIndexIsDef	= Standard_True;
	allocated	= myColorIsDef && myIndexIsDef;
	myindex		= 0;
	mycolor.SetValues (0., 0., 0., Quantity_TOC_RGB);

}

Aspect_ColorMapEntry::Aspect_ColorMapEntry (const Standard_Integer index, const Quantity_Color &color) {

	myColorIsDef	= Standard_True;
	myIndexIsDef	= Standard_True;
	allocated	= myColorIsDef && myIndexIsDef;
	myindex		= index;
	mycolor		= color;

}

Aspect_ColorMapEntry::Aspect_ColorMapEntry (const Aspect_ColorMapEntry& entry) {

	if (entry.allocated == Standard_False) {
		Aspect_BadAccess::Raise
("Aspect_ColorMapEntry::Aspect_ColorMapEntry Unallocated ColorMapEntry");
	}
	else {
		myColorIsDef	= Standard_True;
		myIndexIsDef	= Standard_True;
		allocated	= myColorIsDef && myIndexIsDef;
		myindex		= entry.myindex;
		mycolor		= entry.mycolor;
	}
}

void Aspect_ColorMapEntry::SetValue (const Standard_Integer index, const Quantity_Color &color) {

	myColorIsDef	= Standard_True;
	myIndexIsDef	= Standard_True;
	allocated	= myColorIsDef && myIndexIsDef;
	myindex		= index;
	mycolor		= color;

}

void Aspect_ColorMapEntry::SetValue (const Aspect_ColorMapEntry& entry) {

	if (entry.allocated == Standard_False) {
		Aspect_BadAccess::Raise
("Aspect_ColorMapEntry::Aspect_ColorMapEntry Unallocated ColorMapEntry");
	}
	else {
		myColorIsDef	= Standard_True;
		myIndexIsDef	= Standard_True;
		allocated	= myColorIsDef && myIndexIsDef;
		myindex		= entry.myindex;
		mycolor		= entry.mycolor;
	}

}

void Aspect_ColorMapEntry::SetColor (const Quantity_Color &color) {

	myColorIsDef	= Standard_True;
	allocated	= myColorIsDef && myIndexIsDef;
	mycolor		= color;

}

const Quantity_Color& Aspect_ColorMapEntry::Color () const {

	if (allocated == Standard_False)
		Aspect_BadAccess::Raise
		("Aspect_ColorMapEntry::Color Unallocated ColorMapEntry");

	return mycolor;

}

void Aspect_ColorMapEntry::SetIndex (const Standard_Integer index) {

	myColorIsDef	= Standard_True;
	allocated	= myColorIsDef && myIndexIsDef;
	myindex		= index;

}

Standard_Integer Aspect_ColorMapEntry::Index () const {

	if (allocated == Standard_False)
		Aspect_BadAccess::Raise
		("Aspect_ColorMapEntry::Index Unallocated ColorMapEntry");

	return myindex;

}

Standard_Boolean Aspect_ColorMapEntry::IsAllocated () const {

	return allocated;

}

void Aspect_ColorMapEntry::Free () {

	myColorIsDef	= Standard_False;
	myIndexIsDef	= Standard_False;
	allocated	= myColorIsDef && myIndexIsDef;

}

void Aspect_ColorMapEntry::Dump () const {

  Standard_Real r,g,b ;

  mycolor.Values( r,g,b, Quantity_TOC_RGB ) ;

  cout	<< flush;
  cout	<< "myColorIsDef : " << (myColorIsDef ? "True " : "False") << " , "
	<< "myIndexIsDef : " << (myIndexIsDef ? "True " : "False") << " , "
	<< "allocated : "    << (allocated ? "True " : "False") << "\n";
  cout	<< "myindex : " << myindex << " myColor : ( "
	<< r << ", " << g << ", " << g << " )\n";
  cout	<< flush;
}
