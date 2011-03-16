// Modified	23/02/98 : FMN ; Remplacement PI par Standard_PI

#define G002	//TCL: Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define MAXPOINTS 1023
#define IMPLEMENTED

#include <Graphic2d_EllipsMarker.ixx>
#include <TShort_Array1OfShortReal.hxx>

#define MAXPOINTS 1023

Graphic2d_EllipsMarker::Graphic2d_EllipsMarker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Quantity_Length aXPosition,
	const Quantity_Length aYPosition,
	const Standard_Real X,
	const Standard_Real Y,
	const Standard_Real MajorRadius,
	const Standard_Real MinorRadius,
	const Quantity_PlaneAngle anAngle) 

	:Graphic2d_VectorialMarker (aGraphicObject, aXPosition, aYPosition),
	 myX (Standard_ShortReal (X)),
	 myY (Standard_ShortReal (Y)),
	 myMajorRadius (Standard_ShortReal (MajorRadius)),
	 myMinorRadius (Standard_ShortReal (MinorRadius)),
	 myAngle (Standard_ShortReal(anAngle)) {

	if (myMajorRadius <= RealEpsilon ())
	    Graphic2d_EllipsDefinitionError::Raise ("The major radius = 0.");

	if (myMinorRadius <= RealEpsilon ())
	    Graphic2d_EllipsDefinitionError::Raise ("The minor radius = 0.");

#ifdef IMPLEMENTED
	myMinX = myX + Standard_ShortReal (aXPosition) - myMajorRadius;
	myMinY = myY + Standard_ShortReal (aYPosition) - myMinorRadius;
	myMaxX = myX + Standard_ShortReal (aXPosition) + myMajorRadius;
	myMaxY = myY + Standard_ShortReal (aYPosition) + myMinorRadius;
#else
	myMinX = myX;
	myMinY = myY;
	myMaxX = myX;
	myMaxY = myY;
#endif /* NOT IMPLEMENTED */

  myNumOfElem = 4;
  myNumOfVert = 1;
}

void Graphic2d_EllipsMarker::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

    DrawLineAttrib(aDrawer);

    Standard_ShortReal xp,yp;
    aDrawer->GetMapFromTo(Standard_ShortReal (XPosition ()),
			  Standard_ShortReal (YPosition ()),xp,yp);
    xp+= myX;
    yp+= myY;

    Standard_ShortReal Def;
    Standard_ShortReal Coeff;
    Aspect_TypeOfDeflection Type;
    aDrawer->DrawPrecision(Def,Coeff,Type);
    if (Type == Aspect_TOD_RELATIVE) Def = myMajorRadius * Coeff;

    Standard_Real val;
    if(myMajorRadius > Def) 
      val = Max( 0.0044 , Min (0.7854 , 2. * ACos(1.-Def/myMajorRadius)));
    else
      val = 0.7854;  // = Standard_PI/4.
    Standard_Integer nbpoints = Min(MAXPOINTS,Standard_Integer(2*Standard_PI/val)+2);

//    Standard_Integer nbpoints = Max ( 8 ,Standard_Integer ( myMajorRadius / Def));
    Standard_ShortReal teta = Standard_ShortReal( 2*Standard_PI /nbpoints);
    Standard_ShortReal x1 = Standard_ShortReal( myMajorRadius *Cos (myAngle)),
                       y1 = Standard_ShortReal( myMajorRadius *Sin (myAngle));
    Standard_ShortReal x2,y2;
    Standard_ShortReal cosin = Standard_ShortReal( Cos(teta) );

    x2 = Standard_ShortReal( myMajorRadius * Cos(teta) * Cos(myAngle) -
      myMinorRadius * Sin(teta) * Sin(myAngle));
    y2 = Standard_ShortReal( myMajorRadius * Cos(teta) * Sin(myAngle) +
      myMinorRadius * Sin(teta) * Cos(myAngle));

    TShort_Array1OfShortReal X(1,nbpoints+1); 
    TShort_Array1OfShortReal Y(1,nbpoints+1); 
    X(1) = x1; Y(1) = y1; X(2) = x2; Y(2) = y2;
    for (Standard_Integer i=3; i<= nbpoints+1; i++) {
	X(i) = 2*x2*cosin - x1;
	Y(i) = 2*y2*cosin - y1;
	x1 = x2; y1 = y2; x2 = X(i); y2 = Y(i);
    }

    if (myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
      aDrawer->DrawPolyline(xp,yp,X,Y);
    }
    else {
      aDrawer->DrawPolygon(xp,yp,X,Y);
    }

}

#ifdef G002

void Graphic2d_EllipsMarker::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                    const Standard_Integer anIndex) {

  if ( anIndex > 0 && anIndex < 5) {
    DrawLineAttrib(aDrawer);

    Standard_ShortReal xp=0.,yp=0., xpp=0., ypp=0.;
    aDrawer->GetMapFromTo( Standard_ShortReal(XPosition() ),
			               Standard_ShortReal(YPosition() ), xp, yp );
    xp += myX;
    yp += myY;

    Standard_ShortReal tSin = Standard_ShortReal( Sin(myAngle) ), 
                       tCos = Standard_ShortReal( Cos(myAngle) );
    if ( anIndex == 1 ) {
      xpp = Standard_ShortReal( xp - myMinorRadius * tSin );
      ypp = Standard_ShortReal( yp + myMinorRadius * tCos );
    } else if ( anIndex == 2 ) {
      xpp = Standard_ShortReal( xp + myMajorRadius * tCos );
      ypp = Standard_ShortReal( yp + myMajorRadius * tSin );
    } else if ( anIndex == 3 ) {
      xpp = Standard_ShortReal( xp - myMajorRadius * tCos );
      ypp = Standard_ShortReal( yp - myMajorRadius * tSin );
    } else if ( anIndex == 4 ) {
      xpp = Standard_ShortReal( xp + myMinorRadius * tSin );
      ypp = Standard_ShortReal( yp - myMinorRadius * tCos );
    }    
    
     aDrawer->DrawSegment( xp, yp, xpp, ypp );
  }

}

void Graphic2d_EllipsMarker::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                         const Standard_Integer anIndex) {
 if ( anIndex == 1 ) {
    DrawMarkerAttrib( aDrawer );
    Standard_ShortReal x =  aDrawer->ConvertMapToFrom(myX) + myXPosition;
    Standard_ShortReal y =  aDrawer->ConvertMapToFrom(myY) + myYPosition;
    aDrawer->MapMarkerFromTo( VERTEXMARKER, x, y, DEFAULTMARKERSIZE,DEFAULTMARKERSIZE, 0.0 );
 }
}

#endif

Standard_Boolean Graphic2d_EllipsMarker::Pick (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision,
					 const Handle(Graphic2d_Drawer)& aDrawer)
{
     Standard_ShortReal xf1,yf1,xf2,yf2; //Les foyers.
     Standard_ShortReal Xp, Yp;
     Standard_ShortReal SRX = X, SRY = Y;

     Standard_ShortReal Majord = aDrawer->ConvertMapToFrom(myMajorRadius);
     Standard_ShortReal Minord = aDrawer->ConvertMapToFrom(myMinorRadius);
     Standard_ShortReal x =  aDrawer->ConvertMapToFrom(myX) + myXPosition;
     Standard_ShortReal y =  aDrawer->ConvertMapToFrom(myY) + myYPosition;
     Standard_ShortReal maxord = (Minord < Majord ? Majord : Minord);
     Standard_Boolean found = Standard_False;

     Standard_ShortReal q = Standard_ShortReal( 
         Sqrt(Abs (Majord * Majord - Minord * Minord)));

     Standard_ShortReal cos = Standard_ShortReal( Cos(myAngle) );
     Standard_ShortReal sin = Standard_ShortReal( Sin(myAngle) );

     //Focus 1 et 2 calcules pour l' ellipse dont le centre 
     // est ramene a l' origine du repere(0,0)
     xf1 = Standard_ShortReal( q * cos ); 
     yf1 = Standard_ShortReal( q * sin );
     xf2 = Standard_ShortReal( - q * cos ); 
     yf2 = Standard_ShortReal( - q * sin );

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
        Standard_ShortReal tSin = Standard_ShortReal( Sin(myAngle) ), 
                           tCos = Standard_ShortReal( Cos(myAngle) );
        Standard_ShortReal xpp = Standard_ShortReal( x - Minord * tSin ),
                           ypp = Standard_ShortReal( y + Minord * tCos );
        if ( IsOn( SRX, SRY, x, y, xpp, ypp,  aPrecision) ) {
            SetPickedIndex(1);
            return Standard_True;
        } else {
            xpp = x + Majord * tCos;
            ypp = y + Majord * tSin;
            if ( IsOn( SRX, SRY, x, y, xpp, ypp,  aPrecision) ) {
              SetPickedIndex(2);
              return Standard_True;        
            } else {
              xpp = x - Majord * tCos;
              ypp = y - Majord * tSin;
              if ( IsOn( SRX, SRY, x, y, xpp, ypp,  aPrecision) ) {
                SetPickedIndex(3);
                return Standard_True;        
              } else {
                xpp = x + Minord * tSin,
                ypp = y - Minord * tCos;
                if ( IsOn( SRX, SRY, x, y, xpp, ypp,  aPrecision) ) {
                  SetPickedIndex(4);
                  return Standard_True;        
                }
              }
            }   
       }
     }
#endif
     
     Xp = SRX - x; Yp = SRY - y;

     Standard_ShortReal d1,d2;
     d1 =  Standard_ShortReal( 
         Sqrt( (xf1 - Xp) * (xf1 - Xp) + (yf1 - Yp) * (yf1 - Yp)));
     d2 =  Standard_ShortReal( 
         Sqrt( (xf2 - Xp) * (xf2 - Xp) + (yf2 - Yp) * (yf2 - Yp)));

     if (myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY)
       found = Abs (d1 + d2) < 2 * maxord;
     if (!found) {
	 found = Abs (d1 + d2 - 2 * maxord) < aPrecision;
     }

     return found;
}

void Graphic2d_EllipsMarker::Center(Quantity_Length& X,Quantity_Length& Y) const {

    X = Quantity_Length( myX );
    Y = Quantity_Length( myY );
}

Quantity_Length Graphic2d_EllipsMarker::MajorRadius() const {

    return Quantity_Length( myMajorRadius );
}

Quantity_Length Graphic2d_EllipsMarker::MinorRadius() const {

    return Quantity_Length( myMinorRadius );
}

Quantity_PlaneAngle Graphic2d_EllipsMarker::Angle() const {

    return Quantity_PlaneAngle( myAngle );
}

void Graphic2d_EllipsMarker::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_EllipsMarker" << endl;
	*aFStream << myXPosition << ' ' << myYPosition << endl;
	*aFStream << myX << ' ' << myY << endl;
	*aFStream << myMajorRadius << ' ' << myMinorRadius << endl;
	*aFStream << myAngle << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_EllipsMarker::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Standard_ShortReal XPos, YPos, X, Y, radMaj, radMin, Ang;

	*anIFStream >> XPos >> YPos;
	*anIFStream >> X >> Y;
	*anIFStream >> radMaj >> radMin;
	*anIFStream >> Ang;
	Handle(Graphic2d_EllipsMarker)
		theEllM = new Graphic2d_EllipsMarker(aGraphicObject, XPos, YPos, X, Y,
							radMaj, radMin, Ang);
	((Handle (Graphic2d_Line))theEllM)->Retrieve(anIFStream);
}

