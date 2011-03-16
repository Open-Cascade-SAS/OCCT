// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI

#define G002	//TCL: Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define MAXPOINTS 1023
#define IMPLEMENTED

#include <Graphic2d_CircleMarker.ixx>
#include <Quantity_PlaneAngle.hxx>
#include <TShort_Array1OfShortReal.hxx>

Graphic2d_CircleMarker::Graphic2d_CircleMarker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Quantity_Length aXPosition,
	const Quantity_Length aYPosition,
	const Quantity_Length X,
	const Quantity_Length Y,
	const Quantity_Length Radius)

	:Graphic2d_VectorialMarker (aGraphicObject, aXPosition, aYPosition),
	 myX (Standard_ShortReal (X)),
	 myY (Standard_ShortReal (Y)),
	 myRadius (Standard_ShortReal (Radius)),
	 myFirstAngle (0.0),
	 mySecondAngle (Standard_ShortReal (2.*Standard_PI)) {

	if (myRadius <= RealEpsilon ())
		Graphic2d_CircleDefinitionError::Raise ("The radius = 0.");

#ifdef IMPLEMENTED
	myMinX = myX + Standard_ShortReal (aXPosition) - myRadius;
	myMinY = myY + Standard_ShortReal (aYPosition) - myRadius;
	myMaxX = myX + Standard_ShortReal (aXPosition) + myRadius;
	myMaxY = myY + Standard_ShortReal (aYPosition) + myRadius;
#else
	myMinX = myX;
	myMinY = myY;
	myMaxX = myX;
	myMaxY = myY;
#endif /* NOT IMPLEMENTED */
   myisArc = Standard_False;
   myNumOfElem = MAXPOINTS + 1;
   myNumOfVert = 3;
}

Graphic2d_CircleMarker::Graphic2d_CircleMarker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Quantity_Length aXPosition,
	const Quantity_Length aYPosition,
	const Quantity_Length X,
	const Quantity_Length Y,
	const Quantity_Length Radius,
	const Quantity_PlaneAngle Alpha,
	const Quantity_PlaneAngle Beta)

	:Graphic2d_VectorialMarker (aGraphicObject, aXPosition, aYPosition),
	 myX (Standard_ShortReal (X)),
	 myY (Standard_ShortReal (Y)),
	 myRadius (Standard_ShortReal (Radius)),
	 myFirstAngle (Standard_ShortReal (Alpha)),
	 mySecondAngle (Standard_ShortReal (Beta)) {

	if (myRadius <= RealEpsilon ())
		Graphic2d_CircleDefinitionError::Raise ("The radius = 0.");

#ifdef IMPLEMENTED
	myMinX = myX + Standard_ShortReal (aXPosition) - myRadius;
	myMinY = myY + Standard_ShortReal (aYPosition) - myRadius;
	myMaxX = myX + Standard_ShortReal (aXPosition) + myRadius;
	myMaxY = myY + Standard_ShortReal (aYPosition) + myRadius;
#else
	myMinX = myX;
	myMinY = myY;
	myMaxX = myX;
	myMaxY = myY;
#endif /* NOT IMPLEMENTED */
    myisArc = Standard_True;
    myNumOfElem = MAXPOINTS + 1;
    myNumOfVert = 3;
}

void Graphic2d_CircleMarker::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

    DrawLineAttrib(aDrawer);

    Standard_ShortReal xp,yp;
    aDrawer->GetMapFromTo(Standard_ShortReal (XPosition ()),
			  Standard_ShortReal (YPosition ()),xp,yp);
    xp += myX;
    yp += myY;

    if (myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
      aDrawer->DrawArc(xp,yp,myRadius,myFirstAngle,mySecondAngle);
    } else {
      aDrawer->DrawPolyArc(xp,yp,myRadius,myFirstAngle,mySecondAngle);
    }

}

#ifdef G002

void Graphic2d_CircleMarker::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                          const Standard_Integer anIndex ) {
   
    
 if ( anIndex > 0 && anIndex <= MAXPOINTS + 1 ) { 
     DrawLineAttrib(aDrawer);
     Standard_ShortReal rd = aDrawer->ConvertMapToFrom(myRadius);
     Standard_ShortReal x1 = aDrawer->ConvertMapToFrom(myX) + myXPosition;
     Standard_ShortReal y1 = aDrawer->ConvertMapToFrom(myY) + myYPosition;

     Standard_ShortReal teta = Abs( mySecondAngle - myFirstAngle ) / MAXPOINTS;
     Standard_ShortReal 
               x2 = Standard_ShortReal(x1 + rd * Cos(myFirstAngle + teta*(anIndex-1))),
               y2 = Standard_ShortReal(y1 + rd * Sin(myFirstAngle + teta*(anIndex-1)));
    
     aDrawer->MapSegmentFromTo( x1, y1, x2, y2 );
 }

}

void Graphic2d_CircleMarker::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                         const Standard_Integer anIndex) {

  if ( anIndex > 0 && anIndex < 4 ) {
     DrawMarkerAttrib( aDrawer );
     Standard_ShortReal x,y;
     aDrawer->GetMapFromTo(Standard_ShortReal( XPosition() ),
			  Standard_ShortReal( YPosition() ), x, y );
     x += myX;
     y += myY;
     
     Standard_Real alpha = ( anIndex == 2 ) ? myFirstAngle : mySecondAngle;
       x += Standard_ShortReal( myRadius * Cos( alpha ) );
       y += Standard_ShortReal( myRadius * Sin( alpha ) );
     aDrawer->DrawMarker( VERTEXMARKER, x, y, DEFAULTMARKERSIZE,DEFAULTMARKERSIZE, 0.0 );
  }
}

#endif

Standard_Boolean Graphic2d_CircleMarker::Pick (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision,
					 const Handle(Graphic2d_Drawer)& aDrawer) {

     Standard_ShortReal SRX = X, SRY = Y;
     Standard_ShortReal rd = aDrawer->ConvertMapToFrom(myRadius);
     Standard_ShortReal x =  aDrawer->ConvertMapToFrom(myX) + myXPosition;
     Standard_ShortReal y =  aDrawer->ConvertMapToFrom(myY) + myYPosition;
     Standard_Boolean found = Standard_False;

     if (myGOPtr->IsTransformed ()) {
       gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
       Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	   aTrsf.Transforms (RX, RY);
	   SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
     }

 #ifdef G002

     if ( Graphic2d_Primitive::IsOn( SRX, SRY, x, y, aPrecision) ) {
        SetPickedIndex(-1);
        return Standard_True;
     } else {
       if ( myisArc ) {
         Standard_ShortReal x1 = Standard_ShortReal(rd * Cos( myFirstAngle )  + x),
                            y1 = Standard_ShortReal(rd * Sin( myFirstAngle )  + y),
                            x2 = Standard_ShortReal(rd * Cos( mySecondAngle ) + x),
                            y2 = Standard_ShortReal(rd * Sin( mySecondAngle ) + y);

        if ( Graphic2d_Primitive::IsOn( SRX, SRY, x1, y1, aPrecision) ) {
            SetPickedIndex(-2);
            return Standard_True;
        } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, x2, y2, aPrecision) ) {
            SetPickedIndex(-3);
            return Standard_True;
        }
       } // end if is Arc
       
       TShort_Array1OfShortReal Xpoint( 1, MAXPOINTS + 1 ); 
       TShort_Array1OfShortReal Ypoint( 1, MAXPOINTS + 1 ); 
          
       Standard_ShortReal teta = Abs( mySecondAngle - myFirstAngle ) / MAXPOINTS;
    
       for ( Standard_Integer i = 1; i <= MAXPOINTS + 1; i++ ) {
	      Xpoint(i) = Standard_ShortReal(x + rd * Cos( myFirstAngle + teta*(i-1) ));
	      Ypoint(i) = Standard_ShortReal(y + rd * Sin( myFirstAngle + teta*(i-1) ));
          if ( Graphic2d_Primitive::IsOn( SRX, SRY, Xpoint(i), Ypoint(i), aPrecision) ) {
            SetPickedIndex(i);
            return Standard_True;
          }
       }

       if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY )
	      found = Abs (Sqrt ((x - SRX)*(x - SRX) + (y - SRY)*(y - SRY))) < rd;
       if ( !found )
	      found  = Abs (Sqrt ((x - SRX)*(x - SRX) + (y - SRY)*(y - SRY)) - rd) < aPrecision;
       if ( found ) {
           SetPickedIndex( 0 );
           return Standard_True;
       }                                                      
     }
 #else	 

     if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY ) 
	   found = Abs (Sqrt ((x - SRX)*(x - SRX) + (y - SRY)*(y - SRY))) < rd;
     if ( !found ) 
	     found  = Abs (Sqrt ((x - SRX)*(x - SRX) + (y - SRY)*(y - SRY)) - rd) < aPrecision;
     if ( found ) return Standard_True;
     
 #endif

  return Standard_False;

}

void Graphic2d_CircleMarker::Center( Quantity_Length& X,Quantity_Length& Y ) const {
     
    X = Quantity_Length( myX );
    Y = Quantity_Length( myY );
}

Quantity_Length Graphic2d_CircleMarker::Radius() const {

    return Quantity_Length( myRadius );
}

Quantity_PlaneAngle Graphic2d_CircleMarker::FirstAngle() const {
 
    return Quantity_PlaneAngle( myFirstAngle );
}

Quantity_PlaneAngle Graphic2d_CircleMarker::SecondAngle() const {

    return Quantity_PlaneAngle( mySecondAngle );
}

void Graphic2d_CircleMarker::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_CircleMarker" << endl;
	*aFStream << myXPosition << ' ' << myYPosition << endl;
	*aFStream << myX << ' ' << myY << endl;
	*aFStream << myRadius << endl;
	*aFStream << myisArc << endl;
	if (myisArc)
		*aFStream << myFirstAngle << ' ' << mySecondAngle << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_CircleMarker::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Quantity_Length XPos, YPos, X, Y, Rad;
	Quantity_PlaneAngle Ang1, Ang2;
	int isArc;
	Handle(Graphic2d_CircleMarker) theCirM;

	*anIFStream >> XPos >> YPos;
	*anIFStream >> X >> Y;
	*anIFStream >> Rad;
	*anIFStream >> isArc;
	if (isArc)
	{
		*anIFStream >> Ang1 >> Ang2;
		theCirM = new Graphic2d_CircleMarker(aGraphicObject, XPos, YPos, X, Y, Rad, Ang1, Ang2);
	}
	else
		theCirM = new Graphic2d_CircleMarker(aGraphicObject, XPos, YPos, X, Y, Rad);
	((Handle (Graphic2d_Line))theCirM)->Retrieve(anIFStream);
}

