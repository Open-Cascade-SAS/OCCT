/*=====================================================================

     FONCTION :
     ----------
        Classe Graphic2d_Ellips

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

=====================================================================*/
#define G002	//Add new DrawElement(), DrawVertex() methods

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

#include <Graphic2d_Ellips.ixx>
#include <TShort_Array1OfShortReal.hxx>

#define MAXPOINTS 1023

Graphic2d_Ellips::Graphic2d_Ellips
  (const Handle(Graphic2d_GraphicObject)& aGraphicObject,
   const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length MajorRadius,
   const Quantity_Length MinorRadius,
   const Quantity_PlaneAngle anAngle) 
    : Graphic2d_Line(aGraphicObject) {

      myX = Standard_ShortReal(X); 
      myY = Standard_ShortReal(Y); 
      myMajorRadius = Standard_ShortReal(MajorRadius);
      myMinorRadius = Standard_ShortReal(MinorRadius);

      if (myMajorRadius <= RealEpsilon ())
	Graphic2d_EllipsDefinitionError::Raise ("The major radius = 0.");

      if (myMinorRadius <= RealEpsilon ())
	Graphic2d_EllipsDefinitionError::Raise ("The minor radius = 0.");

      myAngle =Standard_ShortReal(anAngle); 

      // Cas de l'ellipse horizontale
      if (Abs (anAngle) <= RealEpsilon ()) {
          myMinX = myX - myMajorRadius;
          myMaxX = myX + myMajorRadius;
          myMinY = myY - myMinorRadius;
          myMaxY = myY + myMinorRadius;
      }
      else {
          Standard_ShortReal X1, Y1, X2, Y2;
          Standard_Integer nbpoints = 360; // fobenselitmiter
          Standard_ShortReal teta = Standard_ShortReal(2 * Standard_PI / nbpoints);
          Standard_ShortReal x1 = Standard_ShortReal(myMajorRadius * Cos (myAngle));
          Standard_ShortReal y1 = Standard_ShortReal(myMajorRadius * Sin (myAngle));
          Standard_ShortReal x2, y2;
          Standard_ShortReal costeta = Standard_ShortReal(Cos(teta));
          Standard_ShortReal sinteta = Standard_ShortReal(Sin(teta));
    
          x2 = Standard_ShortReal(myMajorRadius * costeta * Cos (myAngle) -
            myMinorRadius * sinteta * Sin (myAngle));
          y2 = Standard_ShortReal(myMajorRadius * costeta * Sin (myAngle) +
            myMinorRadius * sinteta * Cos (myAngle));
    
          X1 = myX + x1; Y1 = myY + y1; 
          X2 = myX + x2; Y2 = myY + y2; 
    
          myMinX = ( X1 < X2 ? X1 : X2 );
          myMaxX = ( X1 < X2 ? X2 : X1 );
          myMinY = ( Y1 < Y2 ? Y1 : Y2 );
          myMaxY = ( Y1 < Y2 ? Y2 : Y1 );
    
          nbpoints++;
          for (Standard_Integer i=3; i<= nbpoints; i++) {
    	  X2 = 2*x2*costeta - x1;
    	  Y2 = 2*y2*costeta - y1;
    	  x1 = x2; y1 = y2; x2 = X2; y2 = Y2; 
    	  X2 += myX; Y2 += myY;
              myMinX = ( myMinX < X2 ? myMinX : X2 );
              myMaxX = ( myMaxX < X2 ? X2 : myMaxX );
              myMinY = ( myMinY < Y2 ? myMinY : Y2 );
              myMaxY = ( myMaxY < Y2 ? Y2 : myMaxY );
          }
      }
    myNumOfElem = 4;
    myNumOfVert = 1;
}

void Graphic2d_Ellips::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if (IsIn) {
    static  TShort_Array1OfShortReal Xpoint(1,MAXPOINTS+1); 
    static  TShort_Array1OfShortReal Ypoint(1,MAXPOINTS+1); 

    DrawLineAttrib(aDrawer);
    Standard_ShortReal Def;
    Standard_ShortReal Coeff;
    Aspect_TypeOfDeflection Type;
    aDrawer->DrawPrecision(Def,Coeff,Type);

    if (Type == Aspect_TOD_RELATIVE) Def = myMajorRadius * Coeff;

    Standard_Real val;
    if(myMajorRadius > Def) 
      val = Max( 0.0044 , Min (0.7854 , 2. * ACos(1.-Def/myMajorRadius)));
    else
      val = 0.7854;  // = PI/4.
    Standard_Integer nbpoints = Min(MAXPOINTS,Standard_Integer(2*Standard_PI/val)+2);

//    Standard_Integer nbpoints = Max ( 8 ,Standard_Integer ( myMajorRadius / Def));
    Standard_ShortReal teta = Standard_ShortReal(2*Standard_PI /nbpoints);
    Standard_ShortReal x1 = Standard_ShortReal(myMajorRadius *Cos (myAngle)),
    y1 = Standard_ShortReal(myMajorRadius *Sin (myAngle));
    Standard_ShortReal x2,y2;
    Standard_ShortReal cosin = Standard_ShortReal(Cos(teta));

    x2 = Standard_ShortReal(myMajorRadius * Cos(teta) * Cos(myAngle) -
      myMinorRadius * Sin(teta) * Sin(myAngle));
    y2 = Standard_ShortReal(myMajorRadius * Cos(teta) * Sin(myAngle) +
      myMinorRadius * Sin(teta) * Cos(myAngle));

    Xpoint(1) = myX + x1; Ypoint(1) = myY + y1; 
    Xpoint(2) = myX + x2; Ypoint(2) = myY + y2;

    nbpoints++;
    for (Standard_Integer i=3; i<= nbpoints; i++) {
	 Xpoint(i) = 2*x2*cosin - x1;
	 Ypoint(i) = 2*y2*cosin - y1;
	 x1 = x2; y1 = y2; x2 = Xpoint(i); y2 = Ypoint(i); 
	 Xpoint(i) += myX; Ypoint(i) += myY;
    }

    if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
     Standard_Real A, B;
	 for (Standard_Integer j=1; j<= nbpoints; j++) {
		A = Standard_Real (Xpoint(j));
		B = Standard_Real (Ypoint(j));
		aTrsf.Transforms (A, B);
		Xpoint(j) = Standard_ShortReal (A);
		Ypoint(j) = Standard_ShortReal (B);
     }
    }

    if (myTypeOfPolygonFilling == Graphic2d_TOPF_EMPTY) {
      aDrawer->MapPolylineFromTo(Xpoint,Ypoint,nbpoints);
    } else {
      aDrawer->MapPolygonFromTo(Xpoint,Ypoint,nbpoints);
    }
  }

}

#ifdef G002

void Graphic2d_Ellips::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                    const Standard_Integer anIndex) {

  Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if (IsIn) {
   if ( anIndex > 0 && anIndex < 5 ) {
    DrawLineAttrib(aDrawer);
    Standard_ShortReal Xp=0., Yp=0.;
    Standard_ShortReal tSin = Standard_ShortReal(Sin(myAngle)), 
                       tCos = Standard_ShortReal(Cos(myAngle));
    if ( anIndex == 1 ) {
      Xp = myX - myMinorRadius * tSin;
      Yp = myY + myMinorRadius * tCos;
    } else if ( anIndex == 2 ) {
      Xp = myX + myMajorRadius * tCos;
      Yp = myY + myMajorRadius * tSin;
    } else if ( anIndex == 3 ) {
      Xp = myX - myMajorRadius * tCos;
      Yp = myY - myMajorRadius * tSin;
    } else if ( anIndex == 4 ) {
      Xp = myX + myMinorRadius * tSin;
      Yp = myY - myMinorRadius * tCos;
    }    

    Standard_Real A, B, C, D;
    Standard_ShortReal a = myX, b = myY, c = Xp, d = Yp;
    if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
	 A = Standard_Real(a);
	 B = Standard_Real(b);
     C = Standard_Real(c);
	 D = Standard_Real(d);
     aTrsf.Transforms (A, B);
     aTrsf.Transforms (C, D);
     a = Standard_ShortReal(A);
	 b = Standard_ShortReal(B);
     c = Standard_ShortReal(C);
	 d = Standard_ShortReal(D);
    }

    aDrawer->MapSegmentFromTo( a, b, c, d );
   }
  }   // end if IsIn
 
}

void Graphic2d_Ellips::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                                   const Standard_Integer anIndex) {

  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }
  if ( IsIn  ) {
    if ( anIndex == 1 ) {
      Standard_ShortReal X,Y;
      DrawMarkerAttrib( aDrawer );
      if ( myGOPtr->IsTransformed() ) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real( myX );
        B = Standard_Real( myY );
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal( A );
        Y = Standard_ShortReal( B );
      } else {
        X = Standard_ShortReal( myX );
        Y = Standard_ShortReal( myY );
      }
      aDrawer->MapMarkerFromTo( VERTEXMARKER, X, Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE, 0.0 );
   } 
  }  // end if IsIn is true         
}

#endif

Standard_Boolean Graphic2d_Ellips::Pick (const Standard_ShortReal X,
					 const Standard_ShortReal Y,
					 const Standard_ShortReal aPrecision,
					 const Handle(Graphic2d_Drawer)& /*aDrawer*/) {

  Standard_ShortReal SRX = X, SRY = Y;
  Standard_ShortReal maxRadius =
	(myMinorRadius < myMajorRadius ? myMajorRadius : myMinorRadius);

  Standard_Boolean found = Standard_False;
  if ( IsInMinMax (X, Y, aPrecision) ) {
     if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
      Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	  aTrsf.Transforms (RX, RY);
	  SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
     }
     #ifdef G002
     if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX, myY, aPrecision) ) {
        SetPickedIndex(-1);
        found = Standard_True;
      } else {
        Standard_ShortReal tSin = Standard_ShortReal(Sin(myAngle)), 
                           tCos = Standard_ShortReal(Cos(myAngle));
        Standard_ShortReal  Xp = myX - myMinorRadius * tSin,
                            Yp = myY + myMinorRadius * tCos;
        if ( IsOn( SRX, SRY, myX, myY, Xp, Yp,  aPrecision) ) {
            SetPickedIndex(1);
            return Standard_True;
        } else {
            Xp = myX + myMajorRadius * tCos;
            Yp = myY + myMajorRadius * tSin;
            if ( IsOn( SRX, SRY, myX, myY, Xp, Yp,  aPrecision) ) {
              SetPickedIndex(2);
              return Standard_True;        
            } else {
              Xp = myX - myMajorRadius * tCos;
              Yp = myY - myMajorRadius * tSin;
              if ( IsOn( SRX, SRY, myX, myY, Xp, Yp,  aPrecision) ) {
                SetPickedIndex(3);
                return Standard_True;        
              } else {
                Xp = myX + myMinorRadius * tSin,
                Yp = myY - myMinorRadius * tCos;
                if ( IsOn( SRX, SRY, myX, myY, Xp, Yp,  aPrecision) ) {
                  SetPickedIndex(4);
                  return Standard_True;        
                }
              }
            }   
        }
        Standard_ShortReal xf1,yf1,xf2,yf2; //Les foyers.
        Standard_ShortReal q = 
           Standard_ShortReal(Sqrt( Abs( myMajorRadius*myMajorRadius - myMinorRadius*myMinorRadius) ));

        Standard_ShortReal cos = Standard_ShortReal(Cos(myAngle));
        Standard_ShortReal sin = Standard_ShortReal(Sin(myAngle));

     //Focus 1 et 2 calcules pour l' ellipse dont le centre 
     // est ramene a l' origine du repere(0,0)
        xf1 = q*cos; 
        yf1 = q*sin;
        xf2 = -q*cos; 
        yf2 = -q*sin;

        Xp = SRX - myX; Yp = SRY - myY;

        Standard_ShortReal d1,d2;
        d1 =  Standard_ShortReal(Sqrt( (xf1 - Xp) * (xf1 - Xp) + (yf1 - Yp) * (yf1 - Yp)));
        d2 =  Standard_ShortReal(Sqrt( (xf2 - Xp) * (xf2 - Xp) + (yf2 - Yp) * (yf2 - Yp)));
        if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY )
         found = Abs (d1 + d2) < 2 * maxRadius;
        if ( !found ) 
	     found = Abs(d1 + d2 - 2 * maxRadius) < aPrecision;
        if ( found ) 
          SetPickedIndex( 0 );
     }
 #else	 
	  Standard_ShortReal xf1,yf1,xf2,yf2; //Les foyers.
      Standard_ShortReal Xp, Yp; 
      Standard_ShortReal q = 
         Sqrt( Abs( myMajorRadius*myMajorRadius - myMinorRadius*myMinorRadius) );

      Standard_ShortReal cos = Cos (myAngle);
      Standard_ShortReal sin = Sin (myAngle);

     //Focus 1 et 2 calcules pour l' ellipse dont le centre 
     // est ramene a l' origine du repere(0,0)
      xf1 = q*cos; 
      yf1 = q*sin;
      xf2 = -q*cos; 
      yf2 = -q*sin;

      Xp = SRX - myX; Yp = SRY - myY;

      Standard_ShortReal d1,d2;
      d1 =  Sqrt ( (xf1 - Xp) * (xf1 - Xp) + (yf1 - Yp) * (yf1 - Yp));
      d2 =  Sqrt ( (xf2 - Xp) * (xf2 - Xp) + (yf2 - Yp) * (yf2 - Yp));
      if ( myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY )
        found = Abs (d1 + d2) < 2 * maxRadius;
      if ( !found ) 
	    found = Abs(d1 + d2 - 2 * maxRadius) < aPrecision;
    
 #endif

  } // end if it's in MinMax

  return found;
}

void Graphic2d_Ellips::Center(Quantity_Length& X,Quantity_Length& Y) const {
    X = Quantity_Length( myX );
    Y = Quantity_Length( myY );
}

Quantity_Length Graphic2d_Ellips::MajorRadius() const {
    return Quantity_Length( myMajorRadius );
}

Quantity_Length Graphic2d_Ellips::MinorRadius() const {
    return Quantity_Length( myMinorRadius );
}

Quantity_PlaneAngle Graphic2d_Ellips::Angle() const {
   return Quantity_PlaneAngle( myAngle );
}

void Graphic2d_Ellips::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_Ellips" << endl;
	*aFStream << myX << ' ' << myY << endl;
	*aFStream << myMajorRadius << ' ' << myMinorRadius << endl;
	*aFStream << myAngle << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_Ellips::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Standard_ShortReal X, Y, radMaj, radMin, Ang;
	
	*anIFStream >> X >> Y;
	*anIFStream >> radMaj >> radMin;
	*anIFStream >> Ang;
	Handle(Graphic2d_Ellips)
		theEll = new Graphic2d_Ellips(aGraphicObject, X, Y, radMaj, radMin, Ang);
	((Handle (Graphic2d_Line))theEll)->Retrieve(anIFStream);
}

