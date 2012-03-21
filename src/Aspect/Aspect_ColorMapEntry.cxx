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
