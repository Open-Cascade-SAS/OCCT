#include <Prs2d_Radius.ixx>
#include <ElCLib.hxx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define XROTATE(x,y) ((x)*cosa - (y)*sina)
#define YROTATE(x,y) ((y)*cosa + (x)*sina)
#define ADDPIX 17

Prs2d_Radius::Prs2d_Radius( const Handle(Graphic2d_GraphicObject) & aGO,
                            const gp_Pnt2d                        & anAttachPnt,
                            const gp_Circ2d                       & aCircle,
                            const TCollection_ExtendedString      & aText,
                            const Standard_Real                     aTxtScale,
                            const Standard_Real                     anArrAngle,
                            const Standard_Real                     anArrLength,
                            const Prs2d_TypeOfArrow                 anArrType,
                            const Prs2d_ArrowSide                   anArrow,
                            const Standard_Boolean                  IsRevArrow ) 

 : Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                    anArrLength, anArrType, anArrow, IsRevArrow ),

   myPnt( anAttachPnt ),
   myCirc( aCircle )
       
{
   SetSymbolCode( 198 );
   gp_Pnt2d aPnt1 = anAttachPnt, aPnt2 = aCircle.Location();
   Standard_Real U = ElCLib::Parameter( aCircle, aPnt1 );
   gp_Pnt2d PTonCirc = ElCLib::Value( U, aCircle );

//   gp_Vec2d theVec( PTonCirc, aPnt2 );
   Standard_Real theDist = aPnt2.Distance( aPnt1 );
   Standard_Real theRad  = aCircle.Radius();
   Standard_Boolean inside  = Standard_False;

   if ( theDist < theRad ) {
    aPnt1 = PTonCirc;
    theDist = theRad;
    inside = Standard_True;
   }

   Standard_Real X1, Y1, X2, Y2;
   aPnt1.Coord( X1, Y1 );
   aPnt2.Coord( X2, Y2 );  

   myX1 = Standard_ShortReal( X1 );
   myY1 = Standard_ShortReal( Y1 );
   myX2 = Standard_ShortReal( X2 );
   myY2 = Standard_ShortReal( Y2 );

   myMinX	= myX1;
   myMinY	= myY1;
   myMaxX	= myX1;
   myMaxY	= myY1;

   if ( myX2 < myMinX ) myMinX = myX2;
   if ( myY2 < myMinY ) myMinY = myY2;
   if ( myX2 > myMaxX ) myMaxX = myX2;
   if ( myY2 > myMaxY ) myMaxY = myY2;
   
   Standard_Real ArrAngle = PI/180.* anArrAngle, theAngle;
   gp_Pnt2d theOrigine( 0., 0. ), P1, P2, P3;
   gp_Vec2d VX( 1., 0. ), VDir;
 
   if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
     P1 = aPnt1;
    
     P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
     P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
     VDir = gp_Vec2d( aPnt1, aPnt2 );
     if ( IsRevArrow ) VDir.Reverse();

     theAngle = VX.Angle( VDir );

     P2.Rotate( theOrigine, theAngle );
     P3.Rotate( theOrigine, theAngle );
  
     P2.Translate( gp_Vec2d( theOrigine, P1 ) );
     P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
     myXVert1(1) = Standard_ShortReal( P2.X() );
     myYVert1(1) = Standard_ShortReal( P2.Y() );
     myXVert1(2) = Standard_ShortReal( P1.X() );
     myYVert1(2) = Standard_ShortReal( P1.Y() );
     myXVert1(3) = Standard_ShortReal( P3.X() );
     myYVert1(3) = Standard_ShortReal( P3.Y() );

     for ( Standard_Integer i = 1; i <= 3; i++ ) {
	   
	   if ( myXVert1(i) < myMinX ) myMinX = myXVert1(i);
           if ( myYVert1(i) < myMinY ) myMinY = myYVert1(i);
           if ( myXVert1(i) > myMaxX ) myMaxX = myXVert1(i);
	   if ( myYVert1(i) > myMaxY ) myMaxY = myYVert1(i);
	   
     }
  } // end if myArrow is FIRSTAR or BOTHAR
 
  if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
    P1 = aPnt2;

    P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
    P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
    VDir = gp_Vec2d( aPnt2, aPnt1 );
    if ( IsRevArrow ) VDir.Reverse();

    theAngle = VX.Angle( VDir );

    P2.Rotate( theOrigine, theAngle );
    P3.Rotate( theOrigine, theAngle );
  
    P2.Translate( gp_Vec2d( theOrigine, P1 ) );
    P3.Translate( gp_Vec2d( theOrigine, P1 ) );
  
    myXVert2(1) = Standard_ShortReal( P2.X() );
    myYVert2(1) = Standard_ShortReal( P2.Y() );
    myXVert2(2) = Standard_ShortReal( P1.X() );
    myYVert2(2) = Standard_ShortReal( P1.Y() );
    myXVert2(3) = Standard_ShortReal( P3.X() );
    myYVert2(3) = Standard_ShortReal( P3.Y() );

    for ( Standard_Integer i = 1; i <= 3; i++ ) {
	  if ( myXVert2(i) < myMinX ) myMinX = myXVert2(i);
          if ( myYVert2(i) < myMinY ) myMinY = myYVert2(i);
          if ( myXVert2(i) > myMaxX ) myMaxX = myXVert2(i);
	  if ( myYVert2(i) > myMaxY ) myMaxY = myYVert2(i);
	  
    }
 } // end if myArrow is FIRSTAR or BOTHAR
  myNumOfElem = 4;
  myNumOfVert = 2;

}

void Prs2d_Radius::Values( gp_Pnt2d& anAttPnt,gp_Circ2d& aCirc ) const {
    anAttPnt = myPnt;
    aCirc    = myCirc;
}

void Prs2d_Radius::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {

  Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {

   DrawLineAttrib( aDrawer );
   
   Standard_ShortReal d1 = aDrawer->Convert(ADDPIX);
   gp_Pnt2d thePnt1(myX1,myY1+d1), thePnt2(myX2,myY2+d1);
   gp_Vec2d VX( 1., 0. ), theVec(thePnt2, thePnt1);
   Standard_Real theTxtAngle = VX.Angle( theVec );
   gp_Pnt2d pntText;
   if ( theTxtAngle >= 0 && theTxtAngle <= PI/2 )
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle > PI/2 && theTxtAngle <= PI ) {
       pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
       theTxtAngle = VX.Angle( theVec.Reversed() );
   } else if ( theTxtAngle >= -PI/2 && theTxtAngle < 0 )  
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle >= -PI && theTxtAngle < -PI/2 ) {
        pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
        theTxtAngle = VX.Angle( theVec.Reversed() );
   }

   Standard_Real theTxtX, theTxtY;
   Standard_Real theSmbX = 0., theSmbY = 0.;
    
   if ( myIsSymbol ) {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
//      TCollection_ExtendedString txtSymb( char( SymbolCode() ) );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      pntText.Coord( theSmbX, theSmbY );
      Standard_ShortReal ws=0., hs=0.;
      aDrawer->GetTextSize( txtSymb, ws, hs );
      Standard_ShortReal ddd = aDrawer->Convert(2);
      theVec = VX.Rotated( theTxtAngle );
      theVec *= ( ws + ddd );
      gp_Pnt2d pntSymb = pntText.Translated( theVec );
      pntSymb.Coord( theTxtX, theTxtY );
   } else {
      pntText.Coord( theTxtX, theTxtY );            
   }

   Standard_ShortReal a = myX1, b = myY1, c = myX2, d = myY2,
                      t1 = Standard_ShortReal(theTxtX), t2 = Standard_ShortReal(theTxtY),
                      ts1 = Standard_ShortReal(theSmbX), ts2 = Standard_ShortReal(theSmbY);
    
   TShort_Array1OfShortReal Xpoint1( 1, 3 ), Ypoint1( 1, 3 ), 
                            Xpoint2( 1, 3 ), Ypoint2( 1, 3 );


    if ( myGOPtr->IsTransformed () ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();

      if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
    
        Standard_Real a1, b1;
        for ( Standard_Integer j = 1; j <= 3; j++ ) {
		     a1 = Standard_Real( myXVert1(j) );
		     b1 = Standard_Real( myYVert1(j) );
                     aTrsf.Transforms (a1, b1);
                     Xpoint1(j) = Standard_ShortReal(a1);
		     Ypoint1(j) = Standard_ShortReal(b1);
         }
      } // end if myArrow is FIRSTAR ot BOTHAR
   
      if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
        
        Standard_Real ac1,ad1;

        for ( Standard_Integer j = 1; j <= 3; j++ ) {
                     ac1 = Standard_Real( myXVert2(j) );
                     ad1 = Standard_Real( myYVert2(j) );
                     aTrsf.Transforms (ac1, ad1);
                     Xpoint2(j) = Standard_ShortReal(ac1);
                     Ypoint2(j) = Standard_ShortReal(ad1);
        }
      } // end if myArrow is LASTAR ot BOTHAR   

      Standard_Real A, B;
      A = Standard_Real( a );
      B = Standard_Real( b ); 
      aTrsf.Transforms( A, B );
      a = Standard_ShortReal( A );
      b = Standard_ShortReal( B );	   
      A = Standard_Real( c );
      B = Standard_Real( d );
      aTrsf.Transforms( A, B );
      c = Standard_ShortReal( A );
      d = Standard_ShortReal( B );
      A = Standard_Real( t1 );
      B = Standard_Real( t2);	   
      aTrsf.Transforms( A, B );
      t1 = Standard_ShortReal( A );
      t2 = Standard_ShortReal( B );
      A = Standard_Real( ts1 );
      B = Standard_Real( ts2 );	   
      aTrsf.Transforms( A, B );
      ts1 = Standard_ShortReal( A );
      ts2 = Standard_ShortReal( B );

   }   else {
    
       if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
  		Xpoint1.Assign( myXVert1 );
                Ypoint1.Assign( myYVert1 );
    }
    if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
        Xpoint2.Assign( myXVert2 );
        Ypoint2.Assign( myYVert2 );
    }
                
   }  // end if GO is transformed

    aDrawer->MapSegmentFromTo( a, b, c, d );
   
    if ( myIsSymbol ) {
//      TCollection_ExtendedString txtSymb( char( SymbolCode() ) );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      Standard_ShortReal dx=0.,dy=0.;
      aDrawer->MapTextFromTo( txtSymb, ts1, ts2, Standard_ShortReal(theTxtAngle), dx, dy, Aspect_TOT_SOLID );
        
    }
    
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                            Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
    
    aDrawer->MapTextFromTo( myText, t1, t2, Standard_ShortReal(theTxtAngle), 0., 0., Aspect_TOT_SOLID );
    
    if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
       if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) {
	    aDrawer->MapPolygonFromTo( Xpoint1, Ypoint1 );
       } else {
        aDrawer->MapPolylineFromTo( Xpoint1, Ypoint1 );
       } 
    }

   if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
       if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) {
	    aDrawer->MapPolygonFromTo( Xpoint2, Ypoint2 );
       } else {
        aDrawer->MapPolylineFromTo( Xpoint2, Ypoint2 );
       } 
   }


  } // end if IsIn is true 

}

void Prs2d_Radius::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                const Standard_Integer anIndex ) {
 
  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {

    DrawLineAttrib( aDrawer );
  //  aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, myTextScale*aDrawer->Scale(), 
  //                          myTextScale*aDrawer->Scale(), Standard_False );

   Standard_ShortReal d1 = aDrawer->Convert(ADDPIX);
   gp_Pnt2d thePnt1(myX1,myY1+d1), thePnt2(myX2,myY2+d1);
   gp_Vec2d VX( 1., 0. ), theVec(thePnt2, thePnt1);
   Standard_Real theTxtAngle = VX.Angle( theVec );
   gp_Pnt2d pntText;
   if ( theTxtAngle >= 0 && theTxtAngle <= PI/2 )
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle > PI/2 && theTxtAngle <= PI ) {
       pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
       theTxtAngle = VX.Angle( theVec.Reversed() );
   } else if ( theTxtAngle >= -PI/2 && theTxtAngle < 0 )  
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle >= -PI && theTxtAngle < -PI/2 ) {
        pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
        theTxtAngle = VX.Angle( theVec.Reversed() );
   }
   Standard_Real theTxtX, theTxtY;
   pntText.Coord( theTxtX, theTxtY );

   Standard_ShortReal a = myX1, b = myY1, c = myX2, d = myY2,
                      t1 = Standard_ShortReal(theTxtX), t2 = Standard_ShortReal(theTxtY);
    TShort_Array1OfShortReal Xpoint( 1, 3 ), Ypoint( 1, 3 ); 

    if ( anIndex == 1 ) {
     Xpoint.Assign( myXVert1 );
     Ypoint.Assign( myYVert1 );
    } else if ( anIndex == 2 ) {
     Xpoint.Assign( myXVert2 );
     Ypoint.Assign( myYVert2 );    
    } 

    if ( myGOPtr->IsTransformed () ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real a1, b1, c1, ad1;
      
      if ( anIndex == 1 ) {      
        for ( Standard_Integer j = 1; j <= 3; j++ ) {
		     a1 = Standard_Real( Xpoint(j) );
		     b1 = Standard_Real( Xpoint(j) );
                     aTrsf.Transforms( a1, b1 );
                     Xpoint(j) = Standard_ShortReal( a1 );
		     Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 2 ) {

        for ( Standard_Integer j = 1; j <= 3; j++ ) {
                     a1 = Standard_Real( Xpoint(j) );
		     b1 = Standard_Real( Xpoint(j) );
                     aTrsf.Transforms( a1, b1 );
                     Xpoint(j) = Standard_ShortReal( a1 );
		     Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 4 ) {

        a1 = Standard_Real( a );
        b1 = Standard_Real( b );
        c1 = Standard_Real( c );
        ad1 = Standard_Real( d );
        aTrsf.Transforms( a1, b1 );
        aTrsf.Transforms( c1, ad1 );
        a = Standard_ShortReal( a1 );
        b = Standard_ShortReal( b1 );
        c = Standard_ShortReal( c1 );
        d = Standard_ShortReal( ad1 );
	          
      } else if ( anIndex == 3 ) {
        a1 = Standard_Real( t1 );
        b1 = Standard_Real( t2);
        aTrsf.Transforms( a1, b1 );
        t1 = Standard_ShortReal( a1 );
        t2 = Standard_ShortReal( b1);
      }
    } 

    if ( anIndex == 1 || anIndex == 2 ) 
      if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) {
	    aDrawer->MapPolygonFromTo( Xpoint, Ypoint );
      } else {
        aDrawer->MapPolylineFromTo( Xpoint, Ypoint );
      } 
    else if ( anIndex == 4 )
        aDrawer->MapSegmentFromTo( a, b, c, d );
    else if ( anIndex == 3 )
        aDrawer->MapTextFromTo( myText, t1, t2, Standard_ShortReal(theTxtAngle), 0., 0., Aspect_TOT_SOLID );
  
  } // end if IsIn is true 

}

void Prs2d_Radius::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                               const Standard_Integer anIndex ) {
  
 Standard_Boolean IsIn = Standard_False;

 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
 else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
 }
 if ( IsIn ) {
  if ( anIndex == 1 || anIndex == 2 ) {
   Standard_ShortReal X=0., Y=0.;
   if ( anIndex == 1 ) {
     X = myX1; Y = myY1;
   } else if ( anIndex == 2 ) {
     X = myX2; Y = myY2;
   }
   DrawMarkerAttrib( aDrawer );
   if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform();
     Standard_Real A = Standard_Real( X ), B = Standard_Real( Y );
     aTrsf.Transforms( A, B );
     X = Standard_ShortReal( A );
     Y = Standard_ShortReal( B );
   } 
     aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y, DEFAULTMARKERSIZE,DEFAULTMARKERSIZE,0.0);
  }
 }
}

Standard_Boolean Prs2d_Radius::Pick( const Standard_ShortReal X,
                                     const Standard_ShortReal Y,
                                     const Standard_ShortReal aPrecision,
                                     const Handle(Graphic2d_Drawer)& aDrawer )  {

   Standard_ShortReal SRX = X, SRY = Y;
   Standard_Boolean Result = Standard_False;
   
   if ( IsInMinMax( X, Y, aPrecision ) ) {
      if ( myGOPtr->IsTransformed () ) {
         gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted();
         Standard_Real RX = Standard_Real(SRX), RY = Standard_Real(SRY);
         aTrsf.Transforms(RX, RY);
         SRX = Standard_ShortReal(RX);
         SRY = Standard_ShortReal(RY);
      }
 
      if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX1, myY1, aPrecision ) ) {
         SetPickedIndex(-1);    
         return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX2, myY2, aPrecision ) ) {
         SetPickedIndex(-2);    
         return Standard_True;
      }

      if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
       for ( Standard_Integer i = 1; i < 3 ; i++)
	    if ( IsOn( SRX, SRY, myXVert1(i), myYVert1(i), myXVert1(i+1), myYVert1( i+1 ), aPrecision ) ) {
	      SetPickedIndex(1);
	      return Standard_True;
        }
	
 	   if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
	    if ( IsIn( SRX, SRY,  myXVert1, myYVert1, aPrecision ) ) {
	       SetPickedIndex(1);
	       return Standard_True;
        }
      } // end if myArrow == FIRSTAR or BOTHAR

    if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
      for ( Standard_Integer i = 1; i < 3 ; i++)
	    if ( IsOn( SRX, SRY, myXVert2(i), myYVert2(i), myXVert2(i+1), myYVert2( i+1 ), aPrecision ) ) {
	       SetPickedIndex(2);
	       return Standard_True;
	    }
	
	  if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
       if ( IsIn( SRX, SRY, myXVert2, myYVert2, aPrecision ) ) {
	      SetPickedIndex(2);
	      return Standard_True;
       }
    } // end if myArrow == LASTAR or BOTHAR

    Standard_ShortReal width,height,xoffset,yoffset;
    Standard_ShortReal hscale = Standard_ShortReal(myTextScale*aDrawer->Scale()),
                       wscale = Standard_ShortReal(myTextScale*aDrawer->Scale());
    Standard_ShortReal TX = X, TY = Y;
    
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, hscale, wscale, Standard_False );
    if ( !aDrawer->GetTextSize( myText, width, height, xoffset, yoffset ) ) {
      width = height = xoffset = yoffset = 0.;
    }
   Standard_ShortReal d1 = aDrawer->Convert(ADDPIX);
   gp_Pnt2d thePnt1(myX1,myY1+d1), thePnt2(myX2,myY2+d1);
   gp_Vec2d VX( 1., 0. ), theVec(thePnt2, thePnt1);
   Standard_Real theTxtAngle = VX.Angle( theVec );
   gp_Pnt2d pntText;
   if ( theTxtAngle >= 0 && theTxtAngle <= PI/2 )
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle > PI/2 && theTxtAngle <= PI ) {
       pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
       theTxtAngle = VX.Angle( theVec.Reversed() );
   } else if ( theTxtAngle >= -PI/2 && theTxtAngle < 0 )  
       pntText = thePnt2.Translated( (myArrowLen+d1)*theVec/(thePnt1.Distance(thePnt2)) );
   else if ( theTxtAngle >= -PI && theTxtAngle < -PI/2 ) {
        pntText = thePnt1.Translated( (myArrowLen+d1)*theVec.Reversed()/(thePnt1.Distance(thePnt2)) );
        theTxtAngle = VX.Angle( theVec.Reversed() );
   }
    Standard_Real theTxtX, theTxtY;
    pntText.Coord( theTxtX, theTxtY );

    Standard_ShortReal cosa = Standard_ShortReal(Cos( -theTxtAngle )),
                       sina = Standard_ShortReal(Sin( -theTxtAngle )),
                       dx = TX-Standard_ShortReal(theTxtX),
                       dy = TY-Standard_ShortReal(theTxtY), 
                       x = Standard_ShortReal(XROTATE(dx,dy)),
                       y = Standard_ShortReal(YROTATE(dx,dy));

    Result   =   (x >= -aPrecision + xoffset)            
            &&   (x <= width + xoffset + aPrecision)
            &&   (y >= -yoffset - aPrecision) 
            &&   (y <= height - yoffset + aPrecision);
  
    if ( Result ) {
      SetPickedIndex(3);
      return Standard_True;
    }

    if ( IsOn( SRX, SRY, myX1, myY1, myX2, myY2, aPrecision ) ) {
      SetPickedIndex( 4 );
      return Standard_True;
    }

 }// end if IsInMinMax is true

    return Standard_False;

}

void Prs2d_Radius::Save(Aspect_FStream& aFStream) const
{
}
