#include <Graphic2d_PolylineMarker.ixx>
#include <Graphic2d_Vertex.hxx>

#define G002	//TCL : Use SetPickedIndex() method
//                TCL : Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

Graphic2d_PolylineMarker::Graphic2d_PolylineMarker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Standard_Real aXPosition,
	const Standard_Real aYPosition,
	const Graphic2d_Array1OfVertex& aListVertex)

	: Graphic2d_VectorialMarker (aGraphicObject, aXPosition, aYPosition),
	  myX (1, aListVertex.Length ()),
	  myY (1, aListVertex.Length ()) {

	if (aListVertex.Length () < 2)
		Graphic2d_PolylineDefinitionError::Raise
			("A polyline with a length less than 2 points.");

	Standard_Integer i, j;

	Standard_Integer Lower = aListVertex.Lower ();
	Standard_Integer Upper = aListVertex.Upper ();

	// Recopie de tableaux avec recherche du minmax
	myMinX = Standard_ShortReal( aListVertex (Lower).X () );
	myMinY = Standard_ShortReal( aListVertex (Lower).Y () );
	myMaxX = Standard_ShortReal( aListVertex (Lower).X () );
	myMaxY = Standard_ShortReal( aListVertex (Lower).Y () );

	for (j=1, i=Lower; i<=Upper; i++, j++) {
		myX (j)	= Standard_ShortReal( aListVertex (i).X () );
		myY (j)	= Standard_ShortReal( aListVertex (i).Y () );
		if (myX (j) < myMinX) myMinX = myX (j);
		if (myY (j) < myMinY) myMinY = myY (j);
		if (myX (j) > myMaxX) myMaxX = myX (j);
		if (myY (j) > myMaxY) myMaxY = myY (j);
	}

#ifdef IMPLEMENTED
	myMinX += aXPosition;
	myMinY += aYPosition;
	myMaxX += aXPosition;
	myMaxY += aYPosition;
#else
#endif /* NOT IMPLEMENTED */

    myNumOfElem = myX.Length()-1;
    myNumOfVert = myX.Length();

}

Graphic2d_PolylineMarker::Graphic2d_PolylineMarker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Standard_Real aXPosition, const Standard_Real aYPosition,
    const TColStd_Array1OfReal& aListX,
	const TColStd_Array1OfReal& aListY)

	: Graphic2d_VectorialMarker (aGraphicObject,aXPosition,aYPosition),
	myX (1, aListX.Length ()),
	myY (1, aListY.Length ()) {

	if (aListX.Length () < 2)
		Graphic2d_PolylineDefinitionError::Raise
			("polyline : length < 2.");

	if (aListX.Length () != aListY.Length ())
		Graphic2d_PolylineDefinitionError::Raise
			("polyline : ListX and ListY have different lengths.");

	Standard_Integer i, j;

	Standard_Integer Lower = aListX.Lower ();
	Standard_Integer Upper = aListX.Upper ();

	// Recopie de tableaux avec recherche du minmax
	myMinX = Standard_ShortReal( aListX (Lower) );
	myMinY = Standard_ShortReal( aListY (Lower) );
	myMaxX = Standard_ShortReal( aListX (Lower) );
	myMaxY = Standard_ShortReal( aListY (Lower) );

	for (j=1, i=Lower; i<=Upper; i++, j++) {
		myX (j)  = Standard_ShortReal( aListX (i) );
		myY (j)  = Standard_ShortReal( aListY (i) );
		if (myX (j) < myMinX) myMinX = myX (j);
		if (myY (j) < myMinY) myMinY = myY (j);
		if (myX (j) > myMaxX) myMaxX = myX (j);
		if (myY (j) > myMaxY) myMaxY = myY (j);
	}

#ifdef IMPLEMENTED
	myMinX += aXPosition;
	myMinY += aYPosition;
	myMaxX += aXPosition;
	myMaxY += aYPosition;
#else
#endif /* NOT IMPLEMENTED */
   
    myNumOfElem = myX.Length()-1;
    myNumOfVert = myX.Length();

}

Standard_Integer Graphic2d_PolylineMarker::Length () const {
   return myX.Length();
}

void Graphic2d_PolylineMarker::Values( const Standard_Integer aRank,
                                 Standard_Real &X,Standard_Real &Y ) const {

    if ( aRank < 1 || aRank > myX.Length() )
     Standard_OutOfRange::Raise( "The renk of point is out of bounds in the line");

    X = myX(aRank);
    Y = myY(aRank);

}

void Graphic2d_PolylineMarker::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

    Standard_ShortReal xp,yp;
    aDrawer->GetMapFromTo(Standard_ShortReal (XPosition ()),
			  Standard_ShortReal (YPosition ()),xp,yp);
    DrawLineAttrib (aDrawer);
    if(myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
      aDrawer->DrawPolyline(xp,yp,myX, myY);
    } else {
      aDrawer->DrawPolygon(xp,yp,myX, myY); 
    }
}

#ifdef G002

void Graphic2d_PolylineMarker::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                            const Standard_Integer anIndex) {

  if ( anIndex > 0 && anIndex < myX.Length() ) { 
    Standard_ShortReal xp, yp;
    DrawLineAttrib( aDrawer );
    aDrawer->GetMapFromTo(Standard_ShortReal(XPosition()),
			  Standard_ShortReal(YPosition()), xp, yp );
    aDrawer->DrawSegment( xp + myX(anIndex), yp + myY(anIndex), 
                          xp + myX(anIndex+1), yp + myY(anIndex+1));
  }

}

void Graphic2d_PolylineMarker::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                           const Standard_Integer anIndex) {
 
  if( anIndex > 0 && anIndex <= myX.Length() ) { 
    Standard_ShortReal xp, yp;
    aDrawer->GetMapFromTo(Standard_ShortReal(XPosition()),
			  Standard_ShortReal(YPosition()), xp, yp );
    DrawMarkerAttrib( aDrawer );
    aDrawer->DrawMarker( VERTEXMARKER, myX(anIndex) + xp, myY(anIndex) + yp,
	       		DEFAULTMARKERSIZE, DEFAULTMARKERSIZE, 0.0 );
  }
}

#endif

Standard_Boolean Graphic2d_PolylineMarker::Pick( const Standard_ShortReal X,
					                             const Standard_ShortReal Y,
					                             const Standard_ShortReal aPrecision,
					                             const Handle(Graphic2d_Drawer)& aDrawer) {

  Standard_Integer i, Lower, Upper;

  Standard_ShortReal SRX = X, SRY = Y;

  Lower	= myX.Lower();
  Upper	= myX.Upper();

  TShort_Array1OfShortReal x(Lower,Upper);
  TShort_Array1OfShortReal y(Lower,Upper);
  for ( i = Lower; i <= Upper; i++ ) {
	x(i) = aDrawer->ConvertMapToFrom( myX(i) );
	y(i) = aDrawer->ConvertMapToFrom( myY(i) ); 
  }	

  if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted();
     Standard_Real RX = Standard_Real( SRX ), RY = Standard_Real( SRY );
	 aTrsf.Transforms( RX, RY );
	 SRX = Standard_ShortReal( RX ); SRY = Standard_ShortReal( RY );
  }

#ifdef G002

  for ( i = Lower; i <= Upper; i++ )   {
      if ( Graphic2d_Primitive::IsOn ( SRX, SRY, myXPosition + x(i), myYPosition + y(i), aPrecision ) ) {
          SetPickedIndex(-i);
          return Standard_True;
      } else if ( ( i < Upper ) && IsOn( SRX, SRY, myXPosition + x(i), myYPosition + y(i), 
	                                     myXPosition + x(i+1), myYPosition + y(i+1), aPrecision ) ) {
          SetPickedIndex(i);
          return Standard_True;
      }
  } 
#else

  for ( i = Lower; i < Upper; i++ ) {
      if (  IsOn( SRX, SRY, myXPosition + x(i), myYPosition + y(i), 
	              myXPosition + x(i+1), myYPosition + y(i+1), aPrecision ) ) {
	      myPickedIndex = i;
          return Standard_True;
      }
  }
#endif  

  if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY )
    if ( IsIn( SRX - myXPosition, SRY - myYPosition, x, y, aPrecision ) ) {
#ifdef G002
       SetPickedIndex(0);
#else
       myPickedIndex = 0;
#endif
       return Standard_True; 
    }

    return Standard_False;

}
void Graphic2d_PolylineMarker::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_PolylineMarker" << endl;
	*aFStream << myNumOfVert << endl;
	*aFStream << myXPosition << ' ' << myYPosition << endl;
	for (Standard_Integer i=1; i<=myNumOfVert; i++)
		*aFStream << myX(i) << ' ' << myY(i) << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_PolylineMarker::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Standard_Integer numOfVert;
	Standard_ShortReal XPos, YPos, X, Y;

	*anIFStream >> numOfVert;
	*anIFStream >> XPos >> YPos;
	Graphic2d_Array1OfVertex listVertex(1, numOfVert);
	for (Standard_Integer i=1; i<=numOfVert; i++)
	{
		*anIFStream >> X >> Y;
		listVertex(i).SetCoord(X, Y);
	}
	Handle(Graphic2d_PolylineMarker)
		thePLinM = new Graphic2d_PolylineMarker(aGraphicObject, XPos, YPos, listVertex);
	((Handle (Graphic2d_Line))thePLinM)->Retrieve(anIFStream);
}


