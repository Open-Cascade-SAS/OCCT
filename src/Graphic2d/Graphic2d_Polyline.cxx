// WTO0001         GG_140596
//                      Calcul des min-max faux apres transformation.

#define G002	//GG_140400 Use SetPickedIndex() method
//                GG_050500 Add new DrawElement(), DrawVertex() methods
//		   	    Returns a negative picked index when the point
//			    is very closed to a polyline point.

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

#include <Graphic2d_Polyline.ixx>
#include <Graphic2d_Vertex.hxx>

#include <TShort_Array1OfShortReal.hxx>

Graphic2d_Polyline::Graphic2d_Polyline (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Graphic2d_Array1OfVertex& aListVertex)

	: Graphic2d_Line (aGraphicObject),
	myX (1, aListVertex.Length ()),
	myY (1, aListVertex.Length ()) {

	if (aListVertex.Length () < 2)
		Graphic2d_PolylineDefinitionError::Raise
			("A polyline with a length less than 2 points.");

Standard_ShortReal X, Y;
Standard_Integer i, j;

	// Recherche des MinMax
Standard_Integer Lower, Upper;
	Lower	= aListVertex.Lower ();
	Upper	= aListVertex.Upper ();

	for (j=1, i=Lower; i<=Upper; i++, j++) {
		X	= Standard_ShortReal(aListVertex (i).X ());
		Y	= Standard_ShortReal(aListVertex (i).Y ());
		myX (j)	= X;
		myY (j)	= Y;
		if (X > myMaxX) myMaxX = X;
		if (X < myMinX) myMinX = X;
		if (Y > myMaxY) myMaxY = Y;
		if (Y < myMinY) myMinY = Y;
	}

    myNumOfElem = myX.Length()-1;
    myNumOfVert = myX.Length();

}

Graphic2d_Polyline::Graphic2d_Polyline (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const TColStd_Array1OfReal& aListX,
	const TColStd_Array1OfReal& aListY)

	: Graphic2d_Line (aGraphicObject),
	myX (1, aListX.Length ()),
	myY (1, aListY.Length ()) {

	if (aListX.Length () < 2)
		Graphic2d_PolylineDefinitionError::Raise
			("polyline : length < 2.");

	if (aListX.Length () != aListY.Length ())
		Graphic2d_PolylineDefinitionError::Raise
			("polyline : ListX and ListY have different lengths.");

Standard_ShortReal X, Y;
Standard_Integer i, j;

	// Recherche des MinMax
Standard_Integer Lower, Upper;
	Lower	= aListX.Lower ();
	Upper	= aListX.Upper ();

	for (j=1, i=Lower; i<=Upper; i++, j++) {
		X	= Standard_ShortReal(aListX (i));
		Y	= Standard_ShortReal(aListY (i));
		myX (j)	= X;
		myY (j)	= Y;
		if (X > myMaxX) myMaxX = X;
		if (X < myMinX) myMinX = X;
		if (Y > myMaxY) myMaxY = Y;
		if (Y < myMinY) myMinY = Y;
	}
    myNumOfElem = myX.Length()-1;
    myNumOfVert = myX.Length();
}


Standard_Integer Graphic2d_Polyline::Length () const {
   return myX.Length();
}

void Graphic2d_Polyline::Values( const Standard_Integer aRank,
                                 Standard_Real &X,Standard_Real &Y ) const
{
        if( aRank < 1 || aRank > myX.Length() )
                Standard_OutOfRange::Raise
                        ("the point rank is out of bounds in the line");

        X = myX(aRank);
        Y = myY(aRank);

}

void Graphic2d_Polyline::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if (IsIn) {
    DrawLineAttrib (aDrawer);

    if (myGOPtr->IsTransformed ()) {
      Standard_Integer nbpoints = myX.Length ();
      TShort_Array1OfShortReal Xpoint (1, nbpoints);
      TShort_Array1OfShortReal Ypoint (1, nbpoints);
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real A, B;
	  for (Standard_Integer j=1; j<= nbpoints; j++) {
		     A = Standard_Real (myX(j));
		     B = Standard_Real (myY(j));
		     aTrsf.Transforms (A, B);
		     Xpoint(j) = Standard_ShortReal (A);
		     Ypoint(j) = Standard_ShortReal (B);
	  }
      if ( myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY ) {
          aDrawer->MapPolylineFromTo(Xpoint, Ypoint);
      } else {
          aDrawer->MapPolygonFromTo(Xpoint, Ypoint); 
      }
    } else {
       if(myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
          aDrawer->MapPolylineFromTo(myX, myY);
       } else {
          aDrawer->MapPolygonFromTo(myX, myY); 
       }
    }
  }
}

#ifdef G002
void Graphic2d_Polyline::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                      const Standard_Integer anIndex) {
Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  Standard_Integer nbpoints = myX.Length ();
  if (IsIn ) {
    if( anIndex > 0 && anIndex < nbpoints ) { //Draw edge
      Standard_ShortReal X1,Y1,X2,Y2;
      DrawLineAttrib (aDrawer);
      if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real (myX(anIndex));
        B = Standard_Real (myY(anIndex));
        aTrsf.Transforms (A, B);
        X1 = Standard_ShortReal (A);
        Y1 = Standard_ShortReal (B);
        A = Standard_Real (myX(anIndex+1));
        B = Standard_Real (myY(anIndex+1));
        aTrsf.Transforms (A, B);
        X2 = Standard_ShortReal (A);
        Y2 = Standard_ShortReal (B);
      } else {
        X1 = Standard_ShortReal (myX(anIndex));
        Y1 = Standard_ShortReal (myY(anIndex));
        X2 = Standard_ShortReal (myX(anIndex+1));
        Y2 = Standard_ShortReal (myY(anIndex+1));
      }
      aDrawer->MapSegmentFromTo(X1,Y1,X2,Y2);
    }
  }
}

void Graphic2d_Polyline::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                     const Standard_Integer anIndex) {
 Standard_Boolean IsIn = Standard_False;

 if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
 else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
 }

 Standard_Integer nbpoints = myX.Length ();
 if (IsIn ) {
    if( anIndex > 0 && anIndex <= nbpoints ) { 
      Standard_ShortReal X,Y;
      DrawMarkerAttrib (aDrawer);
      if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real (myX(anIndex));
        B = Standard_Real (myY(anIndex));
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal (A);
        Y = Standard_ShortReal (B);
      } else {
        X = Standard_ShortReal (myX(anIndex));
        Y = Standard_ShortReal (myY(anIndex));
      }
      aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
    }
  }
}
#endif

Standard_Boolean Graphic2d_Polyline::Pick (const Standard_ShortReal X,
			const Standard_ShortReal Y,
			const Standard_ShortReal aPrecision,
			const Handle(Graphic2d_Drawer)& /*aDrawer*/)
{

  Standard_ShortReal SRX = X, SRY = Y;

  Standard_Integer i;

  Standard_Integer Lower, Upper;
  Lower	= myX.Lower ();
  Upper	= myX.Upper ();

   if (IsInMinMax (X, Y, aPrecision)) {

	if (myGOPtr->IsTransformed ()) {
      gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
      Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	  aTrsf.Transforms (RX, RY);
	  SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
	}

#ifdef G002
   for ( i = Lower; i <= Upper; i++ ) {
     if( Graphic2d_Primitive::IsOn ( SRX, SRY, myX (i), myY (i), aPrecision) ) {
        SetPickedIndex(-i);
        return Standard_True;
     } else if( (i < Upper) && IsOn (SRX, SRY, myX (i), myY (i),
                                myX (i+1), myY (i+1), aPrecision) ) {
           SetPickedIndex(i);
           return Standard_True;
     }
    }
#else
	for (i=Lower; i<Upper ; i++) {
	    if( IsOn (SRX, SRY, myX (i), myY (i), 
				myX (i+1), myY (i+1), aPrecision) ) {
	      myPickedIndex = i;
	      return Standard_True;
	    }
	}
#endif

	if (myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY) {
	  if ( IsIn (SRX  , SRY ,  myX , myY , aPrecision) ) {
#ifdef G002
	      SetPickedIndex(0);
#else
	      myPickedIndex = 0;
#endif
	      return Standard_True;
	  }
	}
   }
   return Standard_False;

}

void Graphic2d_Polyline::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_Polyline" << endl;
	*aFStream << myNumOfVert << endl;
	for (Standard_Integer i=1; i<=myNumOfVert; i++)
		*aFStream << myX(i) << ' ' << myY(i) << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_Polyline::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Standard_Integer numOfVert;
	Standard_ShortReal X, Y;

	*anIFStream >> numOfVert;
	Graphic2d_Array1OfVertex listVertex(1, numOfVert);
	for (Standard_Integer i=1; i<=numOfVert; i++)
	{
		*anIFStream >> X >> Y;
		listVertex(i).SetCoord(X, Y);
	}
	Handle(Graphic2d_Polyline)
		thePLin = new Graphic2d_Polyline(aGraphicObject, listVertex);
	((Handle (Graphic2d_Line))thePLin)->Retrieve(anIFStream);
}

