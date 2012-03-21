// Created on: 2000-06-16
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>
#include <gp_Pnt.hxx>

void Graphic3d_Group :: AddPrimitiveArray ( const Handle(Graphic3d_ArrayOfPrimitives)& elem,const Standard_Boolean EvalMinMax )  
{
	if (IsDeleted () ) return;

        if( !elem->IsValid() ) return;

	if( !MyContainsFacet && 
		(elem->Type() != Graphic3d_TOPA_POLYLINES) &&
		(elem->Type() != Graphic3d_TOPA_SEGMENTS) &&
		(elem->Type() != Graphic3d_TOPA_POINTS) ) {
	  MyStructure->GroupsWithFacet (+1);
	  MyContainsFacet = Standard_True;
	}

	MyIsEmpty	= Standard_False;

	MyListOfPArray.Append(elem);

	// Min-Max Update
	if (EvalMinMax) {
	  Standard_Real x,y,z;

	  for (int k=1; k<=elem->VertexNumber() ; k++) {
	    elem->Vertice(k,x,y,z);

	    if (x < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal(x);
	    if (y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal(y);
	    if (z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal(z);
	    if (x > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal(x);
	    if (y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal(y);
	    if (z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal(z);
	  }
	}

	MyGraphicDriver->PrimitiveArray(MyCGroup, elem->Array());

	Update ();
}

void Graphic3d_Group :: RemovePrimitiveArrays () {
  // clear primitives array's visualization data in graphics driver and remove
  // references to it in driver
  if (!IsEmpty() && !MyGraphicDriver.IsNull())
  {
    for (Graphic3d_ListIteratorOfListOfPArray it (MyListOfPArray);
         it.More(); it.Next())
      MyGraphicDriver->RemovePrimitiveArray (MyCGroup, it.Value()->Array());
  }

  // remove references to primitives arrays
  MyListOfPArray.Clear();
}

Standard_Integer Graphic3d_Group :: ArrayNumber () const {

  	return MyListOfPArray.Extent();
}

void Graphic3d_Group :: InitDefinedArray() {
	MyListOfPArrayIterator.Initialize(MyListOfPArray);
}

void Graphic3d_Group :: NextDefinedArray() {
	MyListOfPArrayIterator.Next();
}

Standard_Boolean Graphic3d_Group :: MoreDefinedArray() {
	return MyListOfPArrayIterator.More();
}

Handle(Graphic3d_ArrayOfPrimitives) Graphic3d_Group :: DefinedArray() const {

	return MyListOfPArrayIterator.Value();
}

void Graphic3d_Group :: RemovePrimitiveArray ( const Standard_Integer aRank ) {

	if( aRank < 1 || aRank > ArrayNumber() ) {
	  Standard_OutOfRange::Raise(" BAD ARRAY index");
	}
	Graphic3d_ListIteratorOfListOfPArray it(MyListOfPArray);
	for( Standard_Integer i=1 ; it.More() ; it.Next(),i++ ) {
	  if( aRank == i ) break;
	}

  // clear primitives array's visualization data in graphics driver and remove
  // references to it in driver
  if (!IsEmpty() && !MyGraphicDriver.IsNull())
    MyGraphicDriver->RemovePrimitiveArray (MyCGroup, it.Value()->Array());

  // remove references to primitives array
  MyListOfPArray.Remove (it);
}

void Graphic3d_Group :: UserDraw ( const Standard_Address AnObject,				   
				   const Standard_Boolean EvalMinMax,
				   const Standard_Boolean ContainsFacet )
{
	if (IsDeleted ()) return;

	// Without this modification, the group assumes the primitive contains
	// no polygons and does not require the Z-buffer for display.
	if (!MyContainsFacet && ContainsFacet) {
	  MyStructure->GroupsWithFacet (+1);
	  MyContainsFacet = Standard_True;
	}

	MyIsEmpty	= Standard_False;

        Graphic3d_CUserDraw AUserDraw;
        AUserDraw.Data = AnObject;
        AUserDraw.Bounds = ( EvalMinMax? &MyBounds : NULL );
	MyGraphicDriver->UserDraw (MyCGroup, AUserDraw);

	Update ();
}
