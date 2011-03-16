/*=====================================================================

     FONCTION :
     ----------
        Classe Graphic2d_SetOfSegments

     TEST :
     ------

        Voir TestG2D/TestG21

     REMARQUES:
     ----------

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------

      27-01-98 : GG ; OPTIMISATION LOADER
                    Transformer les variables static globales en static
                   locales.

=======================================================================*/

#define G002    //GG_140400 Use SetPickedIndex method.
                // Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0


#include <Graphic2d_SetOfSegments.ixx>

#include <TShort_Array1OfShortReal.hxx>

Graphic2d_SetOfSegments::Graphic2d_SetOfSegments (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject)

  : Graphic2d_Line (aGraphicObject) {

}

void Graphic2d_SetOfSegments::Add (
	const Standard_Real X1, const Standard_Real Y1,
	const Standard_Real X2, const Standard_Real Y2)
{
Standard_ShortReal x1 = Standard_ShortReal( X1 );
Standard_ShortReal y1 = Standard_ShortReal( Y1 );
Standard_ShortReal x2 = Standard_ShortReal( X2 );
Standard_ShortReal y2 = Standard_ShortReal( Y2 );

	if ((x1 == x2) && (y1 == y2)) return;

	myMinX	= Min(myMinX,Min(x1,x2));
	myMinY	= Min(myMinY,Min(y1,y2));
	myMaxX	= Max(myMaxX,Max(x1,x2));
	myMaxY	= Max(myMaxY,Max(y1,y2));

	myX1.Append(x1); 
    myY1.Append(y1); 
    myX2.Append(x2); 
    myY2.Append(y2);
    
}

Standard_Integer Graphic2d_SetOfSegments::Length () const {
   return myX1.Length();
}

void Graphic2d_SetOfSegments::Values (const Standard_Integer aRank,
				      Standard_Real &X1,Standard_Real &Y1,
				      Standard_Real &X2,Standard_Real &Y2) const {

	if( aRank < 1 || aRank > myX1.Length() ) 
                Standard_OutOfRange::Raise
			("the segment rank is out of bounds in the set");

	X1 = myX1(aRank);
	Y1 = myY1(aRank);
	X2 = myX2(aRank);
	Y2 = myY2(aRank);

}

void Graphic2d_SetOfSegments::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {
  Standard_Integer i,ns = Length(),bufferize = 0;
  Standard_Boolean IsIn = Standard_False,transform = Standard_False;

  if( ns <= 0 ) return;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    transform = Standard_True;
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {
    static gp_GTrsf2d theTrsf;
    DrawLineAttrib(aDrawer);
    if( transform ) theTrsf = myGOPtr->Transform ();

    if( ns > 2 ) bufferize = 1; 
    Standard_Real A,B;
    Standard_ShortReal x1,y1,x2,y2;
    A = myX1(1); B = myY1(1);
    if( transform ) theTrsf.Transforms(A,B);
    x1 = Standard_ShortReal( A ); 
    y1 = Standard_ShortReal( B );
    A = myX2(1); B = myY2(1);
    if( transform ) theTrsf.Transforms(A,B);
    x2 = Standard_ShortReal( A ); 
    y2 = Standard_ShortReal( B );
    aDrawer->MapSegmentFromTo(x1,y1,x2,y2,bufferize);
    for( i=2 ; i<ns ; i++ ) {
      A = myX1(i); B = myY1(i);
      if( transform ) theTrsf.Transforms(A,B);
      x1 = Standard_ShortReal( A ); 
      y1 = Standard_ShortReal( B );
      A = myX2(i); B = myY2(i);
      if( transform ) theTrsf.Transforms(A,B);
      x2 = Standard_ShortReal( A ); 
      y2 = Standard_ShortReal( B );
      aDrawer->MapSegmentFromTo(x1,y1,x2,y2,0);
    }
    A = myX1(ns); B = myY1(ns);
    if( transform ) theTrsf.Transforms(A,B);
    x1 = Standard_ShortReal( A ); 
    y1 = Standard_ShortReal( B );
    A = myX2(ns); B = myY2(ns);
    if( transform ) theTrsf.Transforms(A,B);
    x2 = Standard_ShortReal( A ); 
    y2 = Standard_ShortReal( B );
    aDrawer->MapSegmentFromTo(x1,y1,x2,y2,-bufferize);
  }
  myNumOfElem = ns;
  myNumOfVert = 2*ns;
}

#ifdef G002

void Graphic2d_SetOfSegments::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                      const Standard_Integer anIndex) {
  Standard_Integer ns = Length(),bufferize = 0;
  Standard_Boolean IsIn = Standard_False,transform = Standard_False;

  if ( ns <= 0 ) return;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    transform = Standard_True;
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {
   if ( anIndex > 0 && anIndex <= ns ) { //Draw element
    static gp_GTrsf2d theTrsf;
    DrawLineAttrib(aDrawer);
    if( transform ) theTrsf = myGOPtr->Transform ();

    if( ns > 2 ) bufferize = 1; 
    Standard_Real A,B;
    Standard_ShortReal x1,y1,x2,y2;
    if ( anIndex == 1) {
      A = myX1(1); B = myY1(1);
      if( transform ) theTrsf.Transforms(A,B);
      x1 = Standard_ShortReal( A ); 
      y1 = Standard_ShortReal( B );
      A = myX2(1); B = myY2(1);
      if( transform ) theTrsf.Transforms(A,B);
      x2 = Standard_ShortReal( A ); 
      y2 = Standard_ShortReal( B );
      aDrawer->MapSegmentFromTo(x1,y1,x2,y2,bufferize);
    } else if ( anIndex == ns ) {
      A = myX1(ns); B = myY1(ns);
      if( transform ) theTrsf.Transforms(A,B);
      x1 = Standard_ShortReal( A ); 
      y1 = Standard_ShortReal( B );
      A = myX2(ns); B = myY2(ns);
      if( transform ) theTrsf.Transforms(A,B);
      x2 = Standard_ShortReal( A ); 
      y2 = Standard_ShortReal( B );
      aDrawer->MapSegmentFromTo(x1,y1,x2,y2,-bufferize);
    } else {
      A = myX1(anIndex); B = myY1(anIndex);
      if( transform ) theTrsf.Transforms(A,B);
      x1 = Standard_ShortReal( A ); 
      y1 = Standard_ShortReal( B );
      A = myX2(anIndex); B = myY2(anIndex);
      if( transform ) theTrsf.Transforms(A,B);
      x2 = Standard_ShortReal( A ); 
      y2 = Standard_ShortReal( B );
      aDrawer->MapSegmentFromTo(x1,y1,x2,y2,0);
    }
   }
  }

}

void Graphic2d_SetOfSegments::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                          const Standard_Integer anIndex) {
    Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  Standard_Integer nbpoints = myX1.Length ();
  if (IsIn ) {
    if( anIndex > 0 && anIndex <= nbpoints ) { 
      Standard_ShortReal X,Y;
      DrawMarkerAttrib (aDrawer);
      if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real (myX1(anIndex));
        B = Standard_Real (myY1(anIndex));
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal (A);
        Y = Standard_ShortReal (B);
      } else {
        X = myX1(anIndex);
        Y = myY1(anIndex);
      }
      aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
    } else if ( anIndex > 0 && anIndex > nbpoints &&  anIndex <= 2*nbpoints) {
      Standard_ShortReal X,Y;
      DrawMarkerAttrib (aDrawer);
      
      if (myGOPtr->IsTransformed ()) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real (myX2(anIndex-nbpoints));
        B = Standard_Real (myY2(anIndex-nbpoints));
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal (A);
        Y = Standard_ShortReal (B);
      } else {
        X = myX2(anIndex-nbpoints);
        Y = myY2(anIndex-nbpoints);
      }
      aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
     }
  }
}
#endif

Standard_Boolean Graphic2d_SetOfSegments::Pick (const Standard_ShortReal X,
					  const Standard_ShortReal Y,
					  const Standard_ShortReal aPrecision,
					  const Handle(Graphic2d_Drawer)& /*aDrawer*/) {

   Standard_Integer i,ns = Length();
   Standard_ShortReal SRX = X, SRY = Y;
 
   if ( (ns > 0) && IsInMinMax (X, Y, aPrecision)) {
	if (myGOPtr->IsTransformed ()) {
     gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
     Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	 aTrsf.Transforms (RX, RY);
	 SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
	}
 
#ifdef G002
    for ( i = 1; i <= myX1.Length(); i++ ) {
       if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX1(i), myY1(i), aPrecision) ) {
           SetPickedIndex(-i);
           return Standard_True;
       } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX2(i), myY2(i), aPrecision) ) {
           SetPickedIndex(-ns-i);
           return Standard_True;
       } else if (IsOn( SRX, SRY, myX1(i), myY1(i), myX2(i), myY2(i), aPrecision) )  {
           SetPickedIndex(i);
           return Standard_True;
       }
    }
#else

    for (i=1; i<=myX1.Length(); i++) {
        if (IsOn (SRX, SRY, myX1 (i), myY1 (i), 
				myX2 (i), myY2 (i), aPrecision) ) {
                myPickedIndex = i;                       
                return Standard_True;
        }
    }

#endif
   
    return Standard_False;
  } else 
   return Standard_False;

}


void Graphic2d_SetOfSegments::Save(Aspect_FStream& aFStream) const
{
}
