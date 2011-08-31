// File		Graphic3d_Graoup_13.cxx
// Created	16/06/2000 : ATS,SPK,GG : G005: Calculate bounding box coordinates for ARRAY primitives

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

	// SAMTECH contribution 5 July 2007 -- BEGIN
	// Without this modification, the group assumes the primitive contains
	// no polygons and does not require the Z-buffer for display.
	if (!MyContainsFacet && ContainsFacet) {
	  MyStructure->GroupsWithFacet (+1);
	  MyContainsFacet = Standard_True;
	}
	// SAMTECH contribution 5 July 2007 -- END

	MyIsEmpty	= Standard_False;

        Graphic3d_CUserDraw AUserDraw;
        AUserDraw.Data = AnObject;
        AUserDraw.Bounds = ( EvalMinMax? &MyBounds : NULL );
	MyGraphicDriver->UserDraw (MyCGroup, AUserDraw);

	Update ();
}
