#include <Prs2d_Length.ixx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define XROTATE(x,y) ((x)*cosa - (y)*sina)
#define YROTATE(x,y) ((y)*cosa + (x)*sina)
#define ADDPIX 17

Prs2d_Length::Prs2d_Length( const Handle(Graphic2d_GraphicObject)& aGO,
                            const gp_Pnt2d&                        anAttachPnt1,
                            const gp_Pnt2d&                        anAttachPnt2,
                            const TCollection_ExtendedString&      aText,
                            const Standard_Real                    aTxtScale,
                            const Standard_Real                    aLength,
                            const Prs2d_TypeOfDist                 aTypeDist,
                            const Standard_Real                    anArrAngle,
                            const Standard_Real                    anArrLength,
                            const Prs2d_TypeOfArrow                anArrType,
                            const Prs2d_ArrowSide                  anArrow,
                            const Standard_Boolean                 IsRevArrow ) 

 : Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                    anArrLength, anArrType, anArrow, IsRevArrow ),
  
   myTypeDist( aTypeDist )
     
{
   SetSymbolCode( 198 );
   myAX1 = Standard_ShortReal( anAttachPnt1.X() );
   myAY1 = Standard_ShortReal( anAttachPnt1.Y() ); 
   myAX2 = Standard_ShortReal( anAttachPnt2.X() );
   myAY2 = Standard_ShortReal( anAttachPnt2.Y() );
   GetPntProject( aLength );
   ComputeArrows( Standard_False );

}

Prs2d_Length::Prs2d_Length( const Handle(Graphic2d_GraphicObject)& aGO,
                            const gp_Pnt2d&                        anAttachPnt,
                            const gp_Lin2d&                        anAttachLin,
                            const TCollection_ExtendedString&      aText,
                            const Standard_Real                    aTxtScale,
                            const Standard_Real                    /*aLenAttachLin*/,
                            const Standard_Real                    anArrAngle,
                            const Standard_Real                    anArrLength,
                            const Prs2d_TypeOfArrow                anArrType,
                            const Prs2d_ArrowSide                  anArrow,
                            const Standard_Boolean                 IsRevArrow ) 

 : Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                    anArrLength, anArrType, anArrow, IsRevArrow ),
    
   myTypeDist( Prs2d_TOD_AUTOMATIC )

{
    SetSymbolCode( 198 );
    gp_Pnt2d aPntProj1 = anAttachPnt, aPntProj2;
    Handle(Geom2d_Line) theLine = new Geom2d_Line( anAttachLin );
    Geom2dAPI_ProjectPointOnCurve theProj( aPntProj1, theLine );
    aPntProj2 = theProj.Point(1);

    myX1 = Standard_ShortReal( aPntProj1.X() );
    myY1 = Standard_ShortReal( aPntProj1.Y() );
    myX2 = Standard_ShortReal( aPntProj2.X() );
    myY2 = Standard_ShortReal( aPntProj2.Y() );
	
    myAX1 = myX1;
    myAY1 = myY1; 
    myAX2 = myX2;
    myAY2 = myY2;
    
    myMinX = myX1;
    myMinY = myY1;
    myMaxX = myX1;
    myMaxY = myY1;

    if ( myX2 < myMinX ) myMinX	= myX2;
    if ( myY2 < myMinY ) myMinY	= myY2;
    if ( myX2 > myMaxX ) myMaxX	= myX2;
    if ( myY2 > myMaxY ) myMaxY	= myY2;
    
    ComputeArrows( Standard_True );
}

Prs2d_Length::Prs2d_Length( const Handle(Graphic2d_GraphicObject)& aGO,
                            const gp_Lin2d&                        anAttachLin1,
                            const gp_Lin2d&                        anAttachLin2,
                            const TCollection_ExtendedString&      aText,
                            const Standard_Real                    aTxtScale,
                            const Standard_Real                    /*aLenAttachLin*/,
                            const Standard_Real                    anArrAngle,
                            const Standard_Real                    anArrLength,
                            const Prs2d_TypeOfArrow                anArrType,
                            const Prs2d_ArrowSide                  anArrow,
                            const Standard_Boolean                 IsRevArrow ) 
 : Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                    anArrLength, anArrType, anArrow, IsRevArrow ),
    
   myTypeDist( Prs2d_TOD_AUTOMATIC )

{
    SetSymbolCode( 198 );
    gp_Pnt2d aPntProj1, aPntProj2;
    aPntProj1 = anAttachLin1.Location();
    Handle(Geom2d_Line) theLine = new Geom2d_Line( anAttachLin2 );
    Geom2dAPI_ProjectPointOnCurve theProj( aPntProj1, theLine );
    aPntProj2 = theProj.Point(1);

    myX1 = Standard_ShortReal( aPntProj1.X() );
    myY1 = Standard_ShortReal( aPntProj1.Y() );
    myX2 = Standard_ShortReal( aPntProj2.X() );
    myY2 = Standard_ShortReal( aPntProj2.Y() );
	
    myAX1 = myX1;
    myAY1 = myY1; 
    myAX2 = myX2;
    myAY2 = myY2;
    
    myMinX = myX1;
    myMinY = myY1;
    myMaxX = myX1;
    myMaxY = myY1;

    if ( myX2 < myMinX ) myMinX	= myX2;
    if ( myY2 < myMinY ) myMinY	= myY2;
    if ( myX2 > myMaxX ) myMaxX	= myX2;
    if ( myY2 > myMaxY ) myMaxY	= myY2;
    
   ComputeArrows( Standard_True ); 

}

void Prs2d_Length::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  
{
  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX,myMaxX,myMinY,myMaxY );
  else
  {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn( minx,maxx,miny,maxy );
  }

  if ( IsIn ) 
  {

    DrawLineAttrib(aDrawer);
// enk code get absolute coordinates for text   
    Standard_Real theTxtX, theTxtY,theTxtAngle;
    gp_Pnt2d thePnt1(myX1,myY1), thePnt2(myX2,myY2);
    gp_Vec2d VX(1.,0.), theVec(thePnt2,thePnt1);
    theVec.Rotate(myTextAngle);
    theTxtX=myAbsX;
    theTxtY=myAbsY;
    theTxtAngle=myAbsAngle;
    gp_Pnt2d pntText(theTxtX,theTxtY);
    Standard_Real theSmbX = 0., theSmbY = 0.;
// end enk code    
    if ( myIsSymbol ) 
    {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_False );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      pntText.Coord( theSmbX, theSmbY );
      Standard_ShortReal ws, hs;
      aDrawer->GetTextSize( txtSymb, ws, hs );
        
      Standard_ShortReal ddd = aDrawer->Convert(2);
      theVec = VX.Rotated( theTxtAngle );
      theVec *= ( ws + ddd );
      gp_Pnt2d pntSymb = pntText.Translated( theVec );
      pntSymb.Coord( theTxtX, theTxtY );
    } else 
    {
      pntText.Coord( theTxtX, theTxtY );            
    }

    Standard_ShortReal a  = myX1 , b  = myY1 , c  = myX2 , d  = myY2;
    Standard_ShortReal a1 = myAX1, b1 = myAY1, c1 = myAX2, d1 = myAY2,
                       t1 = Standard_ShortReal(theTxtX), 
                       t2 = Standard_ShortReal(theTxtY),
                      ts1 = Standard_ShortReal(theSmbX), 
                      ts2 = Standard_ShortReal(theSmbY);

    Standard_ShortReal at = myXT1, bt = myYT1, ct = myXT2, dt = myYT2;

    TShort_Array1OfShortReal Xpoint1( 1, 3 ), Ypoint1( 1, 3 ), 
                            Xpoint2( 1, 3 ), Ypoint2( 1, 3 );

    if ( myGOPtr->IsTransformed() ) 
    {
      gp_GTrsf2d aTrsf = myGOPtr->Transform();
      Standard_Real AT, BT;
       
      if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) )
      {
        for ( Standard_Integer j = 1; j <= 3; j++ ) 
        {
          AT = Standard_Real( myXVert1(j) );
          BT = Standard_Real( myYVert1(j) );
          aTrsf.Transforms( AT, BT );
          Xpoint1(j) = Standard_ShortReal( AT );
          Ypoint1(j) = Standard_ShortReal( BT );
        }
      } // end if myArrow is FIRSTAR ot BOTHAR
   
      if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) 
      {
        for ( Standard_Integer j = 1; j <= 3; j++ ) 
        {
          AT = Standard_Real( myXVert2(j) );
          BT = Standard_Real( myYVert2(j) );
          aTrsf.Transforms( AT, BT );
          Xpoint2(j) = Standard_ShortReal( AT );
          Ypoint2(j) = Standard_ShortReal( BT );
        }
      } // end if myArrow is LASTAR ot BOTHAR   

       AT = Standard_Real( a );
       BT = Standard_Real( b );
       aTrsf.Transforms( AT, BT );
       a = Standard_ShortReal( AT );
       b = Standard_ShortReal( BT );
	   
       AT = Standard_Real( c );
       BT = Standard_Real( d );
       aTrsf.Transforms( AT, BT );
       c = Standard_ShortReal( AT );
       d = Standard_ShortReal( BT );
       
       AT = Standard_Real( t1 );
       BT = Standard_Real( t2 );
       aTrsf.Transforms( AT, BT );
       t1 = Standard_ShortReal( AT );
       t2 = Standard_ShortReal( BT );

       AT = Standard_Real( a1 );
       BT = Standard_Real( b1 );
       aTrsf.Transforms( AT, BT );
       a1 = Standard_ShortReal( AT );
       b1 = Standard_ShortReal( BT );
	   
       AT = Standard_Real( c1 );
       BT = Standard_Real( d1 );
       aTrsf.Transforms( AT, BT );

       c1 = Standard_ShortReal( AT );
       d1 = Standard_ShortReal( BT );

       AT = Standard_Real( ts1 );
       BT = Standard_Real( ts2 );
       aTrsf.Transforms( AT, BT );
       ts1 = Standard_ShortReal( AT );
       ts2 = Standard_ShortReal( BT );

       if ( ArrowIsReversed() ) 
       {
         AT = Standard_Real( at );
         BT = Standard_Real( bt );
         aTrsf.Transforms( AT, BT );
         at = Standard_ShortReal( AT );
         bt = Standard_ShortReal( BT );
	   
         AT = Standard_Real( ct );
         BT = Standard_Real( dt );
         aTrsf.Transforms( AT, BT );
         ct = Standard_ShortReal( AT );
         dt = Standard_ShortReal( BT );
       }
    }   else 
    {
       if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) 
       {
  		Xpoint1.Assign( myXVert1 );
                Ypoint1.Assign( myYVert1 );
       }
       if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) 
       {
                Xpoint2.Assign( myXVert2 );
                Ypoint2.Assign( myYVert2 );
       }
    }  // end if GO is transformed

    if ( ArrowIsReversed() ) 
        aDrawer->MapSegmentFromTo( at, bt, ct, dt );
    else
        aDrawer->MapSegmentFromTo( a, b, c, d );

    // Drawing auxiliary lines which connect a model and a dimension
    // begin enk code. Auxiliary lines which connect to model with same tolerance  
    if (!((a==a1 && b==b1)||(c==c1 && d==d1)))
    {
      gp_Pnt2d aAuxLinePnt1( a, b) , aAuxLinePnt2( a1, b1);
      gp_Pnt2d aAuxLinePnt3( c, d) , aAuxLinePnt4( c1, d1);
      gp_Vec2d aAuxVec(aAuxLinePnt2,aAuxLinePnt1);
      Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
      aAuxVec.Normalize();
        
      aAuxVec*=dxx;
    
      aAuxLinePnt1.Translate(aAuxVec);
      aAuxLinePnt2.Translate(aAuxVec);
      aAuxLinePnt3.Translate(aAuxVec);
      aAuxLinePnt4.Translate(aAuxVec);
    
      a =Standard_ShortReal(aAuxLinePnt1.X());
      b =Standard_ShortReal(aAuxLinePnt1.Y());
      a1=Standard_ShortReal(aAuxLinePnt2.X());
      b1=Standard_ShortReal(aAuxLinePnt2.Y());
      c =Standard_ShortReal(aAuxLinePnt3.X());
      d =Standard_ShortReal(aAuxLinePnt3.Y());
      c1=Standard_ShortReal(aAuxLinePnt4.X());
      d1=Standard_ShortReal(aAuxLinePnt4.Y());
    }
    //end enk code 
 
    aDrawer->MapSegmentFromTo( a, b, a1, b1 );
    aDrawer->MapSegmentFromTo( c, d, c1, d1 );

    if ( myIsSymbol ) 
    {
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      aDrawer->MapTextFromTo( txtSymb, ts1, ts2,Standard_ShortReal(theTxtAngle), 
                              0., 0., Aspect_TOT_SOLID );
    }
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, 
                            Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                            Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                            Standard_False );
    aDrawer->MapTextFromTo( myText, t1, t2, Standard_ShortReal(theTxtAngle), 
                            0., 0., Aspect_TOT_SOLID );

// Drawing arrows
    if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) )
    {
       if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
       {
	    aDrawer->MapPolygonFromTo( Xpoint1, Ypoint1 );
       } else 
       {
        aDrawer->MapPolylineFromTo( Xpoint1, Ypoint1 );
       } 
    }

   if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) )
   {
       if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
       {
	    aDrawer->MapPolygonFromTo( Xpoint2, Ypoint2 );
       } else 
       {
        aDrawer->MapPolylineFromTo( Xpoint2, Ypoint2 );
       } 
   }

  } // end if IsIn is true 

}

void Prs2d_Length::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                                const Standard_Integer anIndex )
{
  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else 
  {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) 
  {
   DrawLineAttrib( aDrawer );
// begin enk code text location
    Standard_Real theTxtX, theTxtY,theTxtAngle;
    gp_Pnt2d thePnt1(myX1,myY1), thePnt2(myX2,myY2);
    gp_Vec2d VX(1.,0.), theVec(thePnt2,thePnt1);
    theVec.Rotate(myTextAngle);
    theTxtX=myAbsX;
    theTxtY=myAbsY;
    theTxtAngle=myAbsAngle;
    gp_Pnt2d pntText(theTxtX,theTxtY);
// end enk code
    Standard_ShortReal a = myX1, b = myY1, c = myX2, d = myY2,
                       t1 = Standard_ShortReal(theTxtX), 
                       t2 = Standard_ShortReal(theTxtY), 
                       aa1 = myAX1, ba1 = myAY1, aa2 = myAX2, ba2 = myAY2;
    TShort_Array1OfShortReal Xpoint( 1, 3 ), Ypoint( 1, 3 ); 

    if ( anIndex == 1 ) 
    {
     Xpoint.Assign( myXVert1 );
     Ypoint.Assign( myYVert1 );
    } else if ( anIndex == 2 ) 
    {
     Xpoint.Assign( myXVert2 );
     Ypoint.Assign( myYVert2 );    
    } 

    if ( myGOPtr->IsTransformed () ) 
    {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real a1, b1, c1, d1, aa11, ba11, aa21, ba21;
      
      if ( anIndex == 1 ) 
      {
        for ( Standard_Integer j = 1; j <= 3; j++ ) 
        {
          a1 = Standard_Real( Xpoint(j) );
          b1 = Standard_Real( Xpoint(j) );
          aTrsf.Transforms( a1, b1 );
          Xpoint(j) = Standard_ShortReal( a1 );
          Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 2 ) 
      {
        for ( Standard_Integer j = 1; j <= 3; j++ ) 
        {
          a1 = Standard_Real( Xpoint(j) );
          b1 = Standard_Real( Xpoint(j) );
          aTrsf.Transforms( a1, b1 );
          Xpoint(j) = Standard_ShortReal( a1 );
          Ypoint(j) = Standard_ShortReal( b1 );
         }
      } else if ( anIndex == 4 ) 
      {
        a1 = Standard_Real( a );
        b1 = Standard_Real( b );
        c1 = Standard_Real( c );
        d1 = Standard_Real( d );
        aTrsf.Transforms( a1, b1 );
        aTrsf.Transforms( c1, d1 );
        a = Standard_ShortReal( a1 );
        b = Standard_ShortReal( b1 );
        c = Standard_ShortReal( c1 );
        d = Standard_ShortReal( d1 );
      } else if ( anIndex == 3 ) 
      {
        a1 = Standard_Real( t1 );
        b1 = Standard_Real( t2);
        aTrsf.Transforms( a1, b1 );
        t1 = Standard_ShortReal( a1 );
        t2 = Standard_ShortReal( b1);
      } else if ( anIndex == 5 ) 
      {
        aa11 = Standard_Real( aa1 );
        ba11 = Standard_Real( ba1 );
        aTrsf.Transforms( aa11, ba11 );
        aa1 = Standard_ShortReal( aa11 );
        ba1 = Standard_ShortReal( ba11 );
	          
      } else if ( anIndex == 6 ) 
      {
        aa21 = Standard_Real( aa2 );
        ba21 = Standard_Real( ba2 );
        aTrsf.Transforms( aa21, ba21 );
        aa2 = Standard_ShortReal( aa21 );
        ba2 = Standard_ShortReal( ba21 );
      } 
    } 

    if ( anIndex == 1 || anIndex == 2 ) 
      if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
      {
	    aDrawer->MapPolygonFromTo( Xpoint, Ypoint );
      } else 
      {
        aDrawer->MapPolylineFromTo( Xpoint, Ypoint );
      } 
    else if ( anIndex == 4 )
        aDrawer->MapSegmentFromTo( a, b, c, d );
    else if ( anIndex == 3 )
        aDrawer->MapTextFromTo( myText, t1, t2, Standard_ShortReal(theTxtAngle),
                                0., 0., Aspect_TOT_SOLID );
    else if ( anIndex == 5 )
        aDrawer->MapSegmentFromTo( a, b, aa1, ba1 );
    else if ( anIndex == 6 )
        aDrawer->MapSegmentFromTo( c, d, aa2, ba2 );

  } // end if IsIn is true 

}

void Prs2d_Length::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                               const Standard_Integer anIndex ) 
{
 Standard_Boolean IsIn = Standard_False;

 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
 else 
 {
   Standard_ShortReal minx, miny, maxx, maxy;
   MinMax( minx, maxx, miny, maxy );
   IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
 }
 if ( IsIn ) 
 {
  if ( anIndex == 1 || anIndex == 2 ) 
  {
   Standard_ShortReal X=0., Y=0.;
   if ( anIndex == 1 ) 
   {
     X = myX1; Y = myY1;
   } else if ( anIndex == 2 ) 
   {
     X = myX2; Y = myY2;
   }
   DrawMarkerAttrib( aDrawer );
   if ( myGOPtr->IsTransformed() ) 
   {
     gp_GTrsf2d aTrsf = myGOPtr->Transform();
     Standard_Real A = Standard_Real( X ), B = Standard_Real( Y );
     aTrsf.Transforms( A, B );
     X = Standard_ShortReal( A );
     Y = Standard_ShortReal( B );
   } 
   aDrawer->MapMarkerFromTo(VERTEXMARKER,X,Y, DEFAULTMARKERSIZE,
                            DEFAULTMARKERSIZE,0.0);
  }
 }
}

Standard_Boolean Prs2d_Length::Pick( const Standard_ShortReal X,
                                     const Standard_ShortReal Y,
                                     const Standard_ShortReal aPrecision,
                                     const Handle(Graphic2d_Drawer)& aDrawer )  
{
   Standard_ShortReal SRX = X, SRY = Y;
   Standard_Boolean Result = Standard_False;
   
   if (IsInMinMax (X, Y, aPrecision) ) 
   {
      if ( myGOPtr->IsTransformed () ) 
      {
        gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted();
        Standard_Real RX = Standard_Real(SRX), RY = Standard_Real(SRY);
        aTrsf.Transforms(RX, RY);
        SRX = Standard_ShortReal(RX);
        SRY = Standard_ShortReal(RY);
      }

      if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX1, myY1, aPrecision ) ) 
      {
        SetPickedIndex(-1);    
        return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, myX2, myY2, aPrecision ) ) 
      {
        SetPickedIndex(-2);    
        return Standard_True;
      }

      if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) 
      {
       for ( Standard_Integer i = 1; i < 3 ; i++)
	    if ( IsOn( SRX, SRY, myXVert1(i), myYVert1(i), 
                       myXVert1(i+1), myYVert1( i+1 ), aPrecision ) ) 
            {
	      SetPickedIndex(1);
	      return Standard_True;
            }
	
 	    if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
	     if ( IsIn( SRX, SRY,  myXVert1, myYVert1, aPrecision ) ) 
             {
	       SetPickedIndex(1);
	       return Standard_True;
             }
      } // end if myArrow == FIRSTAR or BOTHAR

    if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) )
    {
      for ( Standard_Integer i = 1; i < 3 ; i++)
	    if ( IsOn( SRX, SRY, myXVert2(i), myYVert2(i), 
                      myXVert2(i+1), myYVert2( i+1 ), aPrecision ) ) 
            {
              SetPickedIndex(2);
              return Standard_True;
	    }
	
	  if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) 
            if ( IsIn( SRX, SRY, myXVert2, myYVert2, aPrecision ) ) 
            {
	      SetPickedIndex(2);
	      return Standard_True;
            }
    } // end if myArrow == LASTAR or BOTHAR

    Standard_ShortReal width,height,xoffset,yoffset;
    Standard_ShortReal hscale = Standard_ShortReal(myTextScale*aDrawer->Scale()),
                       wscale = Standard_ShortReal(myTextScale*aDrawer->Scale());
    Standard_ShortReal TX = X, TY = Y;
    
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, hscale, wscale, 
                            Standard_False );
    if ( !aDrawer->GetTextSize( myText, width, height, xoffset, yoffset ) ) 
    {
      width = height = xoffset = yoffset = 0.;
    }
// begin enk code text location
    Standard_Real theTxtX, theTxtY,theTxtAngle;
    gp_Pnt2d thePnt1(myX1,myY1), thePnt2(myX2,myY2);
    gp_Vec2d VX(1.,0.), theVec(thePnt2,thePnt1);
    theVec.Rotate(myTextAngle);
    theTxtX=myAbsX;
    theTxtY=myAbsY;
    theTxtAngle=myAbsAngle;
    gp_Pnt2d pntText(theTxtX,theTxtY);
// end enk code
    Standard_ShortReal cosa = Standard_ShortReal(Cos( -theTxtAngle )),
                       sina = Standard_ShortReal(Sin( -theTxtAngle )),
                       dx = TX-Standard_ShortReal(theTxtX),
                       dy = TY-Standard_ShortReal(theTxtY), 
                       x = XROTATE(dx,dy),
                       y = YROTATE(dx,dy);
 
    Result   =   (x >= -aPrecision + xoffset)            
            &&   (x <= width + xoffset + aPrecision)
            &&   (y >= -yoffset - aPrecision) 
            &&   (y <= height - yoffset + aPrecision);
  
    if ( Result ) 
    {
      SetPickedIndex(3);
      return Standard_True;
    }
// begin enk code auxiality lines
    Standard_ShortReal a,b,c,d,a1,b1,c1,d1;
      a = myX1;
      b = myY1;
      a1= myAX1;
      b1= myAY1;
      c = myX2;
      d = myY2; 
      c1= myAX2;
      d1= myAY2;
    if (!((a==a1 && b==b1)||(c==c1 && d==d1)))
    {
      gp_Pnt2d aAuxLinePnt1( myX1, myY1) , aAuxLinePnt2( myAX1, myAY1);
      gp_Pnt2d aAuxLinePnt3( myX2, myY2) , aAuxLinePnt4( myAX2, myAY2);
      gp_Vec2d aAuxVec(aAuxLinePnt2,aAuxLinePnt1);
      Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    
      aAuxVec.Normalize();
      aAuxVec*=dxx;
      aAuxLinePnt1.Translate(aAuxVec);
      aAuxLinePnt2.Translate(aAuxVec);
      aAuxLinePnt3.Translate(aAuxVec);
      aAuxLinePnt4.Translate(aAuxVec);
    
      a =Standard_ShortReal(aAuxLinePnt1.X());
      b =Standard_ShortReal(aAuxLinePnt1.Y());
      a1=Standard_ShortReal(aAuxLinePnt2.X());
      b1=Standard_ShortReal(aAuxLinePnt2.Y());
      c =Standard_ShortReal(aAuxLinePnt3.X());
      d =Standard_ShortReal(aAuxLinePnt3.Y());
      c1=Standard_ShortReal(aAuxLinePnt4.X());
      d1=Standard_ShortReal(aAuxLinePnt4.Y());
    }
      // end enk code  
      
    if ( IsOn( SRX, SRY, myX1, myY1, myX2, myY2, aPrecision ) ) 
    {
       SetPickedIndex( 4 );
       return Standard_True;
    } else if ( IsOn( SRX, SRY, a, b, a1, b1, aPrecision ) ) 
    {
       SetPickedIndex( 5 );
       return Standard_True;
    } else if ( IsOn( SRX, SRY, c, d, c1, d1, aPrecision ) ) 
    {
       SetPickedIndex( 6 );
       return Standard_True;    
    }
  }
	return Standard_False;
}

void Prs2d_Length::GetPntProject( const Standard_Real aLength ) 
{
   
   gp_Pnt2d myPntAttach1( myAX1, myAY1 ), myPntAttach2( myAX2, myAY2 );
   gp_Pnt2d aPntProj1, aPntProj2, MinMaxPnt = myPntAttach1;

   switch ( myTypeDist ) 
   {
     case Prs2d_TOD_HORIZONTAL: 
     {
       if ( aLength < 0 ) 
       {
         if ( MinMaxPnt.Y() > myPntAttach2.Y() ) 
         {
           myPntAttach1 = myPntAttach2;
           myPntAttach2 = MinMaxPnt;		    
         }
       } else 
       {
         if ( MinMaxPnt.Y() < myPntAttach2.Y() ) 
         {
           myPntAttach1 = myPntAttach2;
           myPntAttach2 = MinMaxPnt;		               
         }	   
       }
       aPntProj1 = gp_Pnt2d( myPntAttach1.X(), myPntAttach1.Y() + aLength );
       Handle(Geom2d_Line) theLine = new Geom2d_Line( aPntProj1, 
                                            gp_Dir2d( aPntProj1.X(), 0. ) );
       Geom2dAPI_ProjectPointOnCurve theProj( myPntAttach2, theLine );
       aPntProj2 = theProj.Point(1);
	   
       break;
      }

     case Prs2d_TOD_VERTICAL: 
     {
       if ( aLength < 0 ) 
       {
         if ( MinMaxPnt.X() > myPntAttach2.X() ) 
         {
           myPntAttach1 = myPntAttach2;
           myPntAttach2 = MinMaxPnt;		    
         }
       } else 
       {
         if ( MinMaxPnt.X() < myPntAttach2.X() ) 
         {
           myPntAttach1 = myPntAttach2;
           myPntAttach2 = MinMaxPnt;		               
         }
       }
       aPntProj1 = gp_Pnt2d( myPntAttach1.X() + aLength , myPntAttach1.Y());
       Handle(Geom2d_Line) theLine = new Geom2d_Line( aPntProj1, 
                                            gp_Dir2d( 0., aPntProj1.Y() ) );
       Geom2dAPI_ProjectPointOnCurve theProj( myPntAttach2, theLine );
       aPntProj2 = theProj.Point(1);
       break;
     }
 
/*   case Prs2d_TOD_OBLIQUE: 
     {
       gp_Vec2d DirLin1( myPntAttach1, myPntAttach2 );
       gp_Vec2d DirLin2 = DirLin1.Reversed();
       DirLin1.Normalize();
       DirLin2.Normalize();
       DirLin1 *= aLength;
       DirLin2 *= aLength;
       DirLin1.Rotate( PI/2. );
       DirLin2.Rotate( -PI/2. );
       aPntProj1 = myPntAttach1.Translated( DirLin1 );
       aPntProj2 = myPntAttach2.Translated( DirLin2 );;
       break;
     } */ // commented by enk
     
   case Prs2d_TOD_AUTOMATIC:
   case Prs2d_TOD_OBLIQUE: 
     default: 
     {
       gp_Vec2d DirLin1( myPntAttach1, myPntAttach2 );
       gp_Vec2d DirLin2 = DirLin1.Reversed();
       DirLin1.Normalize();
       DirLin2.Normalize();
       DirLin1 *= aLength;
       DirLin2 *= aLength;
       DirLin1.Rotate( PI/2. );
       DirLin2.Rotate( -PI/2. );
       aPntProj1 = myPntAttach1.Translated( DirLin1 );
       aPntProj2 = myPntAttach2.Translated( DirLin2 );;
       break;
     }
   } // end switch TypeOfDist

   myAX1 = Standard_ShortReal( myPntAttach1.X() );
   myAY1 = Standard_ShortReal( myPntAttach1.Y() ); 
   myAX2 = Standard_ShortReal( myPntAttach2.X() );
   myAY2 = Standard_ShortReal( myPntAttach2.Y() );
   myX1 = Standard_ShortReal( aPntProj1.X() );
   myY1 = Standard_ShortReal( aPntProj1.Y() );
   myX2 = Standard_ShortReal( aPntProj2.X() );
   myY2 = Standard_ShortReal( aPntProj2.Y() );

   myMinX = Min( myX1, myX2 );
   myMinY = Min( myY1, myY2 );
   myMaxX = Max( myX1, myX2 );
   myMaxY = Max( myY1, myY2 );

   if ( myAX1 < myMinX ) myMinX = myAX1;
   if ( myAY1 < myMinY ) myMinY = myAY1;
   if ( myAX1 > myMaxX ) myMaxX = myAX1;
   if ( myAY1 > myMaxY ) myMaxY = myAY1;
    
   if ( myAX2 < myMinX ) myMinX = myAX2;
   if ( myAY2 < myMinY ) myMinY = myAY2;
   if ( myAX2 > myMaxX ) myMaxX = myAX2;
   if ( myAY2 > myMaxY ) myMaxY = myAY2;
   
}

void Prs2d_Length::ComputeArrows( const Standard_Boolean /*isnotPoints*/ ) 
{
  
  Standard_Real ArrAngle = PI/180. * ArrowAngle(), theAngle;
  gp_Pnt2d aPnt1( myX1, myY1 ), aPnt2( myX2, myY2 );
  gp_Pnt2d theOrigine( 0., 0.), P1, P2, P3;
  gp_Vec2d VX( 1., 0. ), VDir;
 
  myXT1 = myX1;
  myYT1 = myY1;
  myXT2 = myX2;
  myYT2 = myY2;

  if ( ArrowIsReversed() ) 
  {
    gp_Pnt2d tP1 = aPnt1, tP2 = aPnt2;
    gp_Vec2d tVD( aPnt1, aPnt2 );
    tVD.Normalize();
    tVD *= ( 1.3 * myArrowLen );
    tP1.Translate( -tVD );
    tP2.Translate( tVD );  
    Standard_Real X1, Y1, X2, Y2;
    tP1.Coord( X1, Y1 );
    tP2.Coord( X2, Y2 );  

    myXT1 = Standard_ShortReal( X1 );
    myYT1 = Standard_ShortReal( Y1 );
    myXT2 = Standard_ShortReal( X2 );
    myYT2 = Standard_ShortReal( Y2 );
  }

  if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) 
  {
     P1 = aPnt1;
     P2 = gp_Pnt2d( myArrowLen,  myArrowLen*Tan( ArrAngle/2. ) );
     P3 = gp_Pnt2d( myArrowLen, -myArrowLen*Tan( ArrAngle/2. ) );
     VDir = gp_Vec2d( aPnt1, aPnt2 );
     if ( ArrowIsReversed() ) VDir.Reverse();
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

     for ( Standard_Integer i = 1; i <= 3; i++ ) 
     {
       if ( myXVert1(i) < myMinX ) myMinX = myXVert1(i);
       if ( myYVert1(i) < myMinY ) myMinY = myYVert1(i);
       if ( myXVert1(i) > myMaxX ) myMaxX = myXVert1(i);
       if ( myYVert1(i) > myMaxY ) myMaxY = myYVert1(i);
     }
   } // end if myArrow is FIRSTAR or BOTHAR
 
  if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) )
  {
    P1 = aPnt2;
    P2 = gp_Pnt2d( myArrowLen,  myArrowLen * Tan( ArrAngle/2. ) );
    P3 = gp_Pnt2d( myArrowLen, -myArrowLen * Tan( ArrAngle/2. ) );
    VDir = gp_Vec2d( aPnt2, aPnt1 );
    if ( ArrowIsReversed() ) VDir.Reverse();
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

    for ( Standard_Integer i = 1; i <= 3; i++ ) 
    {
      if ( myXVert2(i) < myMinX ) myMinX = myXVert2(i);
      if ( myYVert2(i) < myMinY ) myMinY = myYVert2(i);
      if ( myXVert2(i) > myMaxX ) myMaxX = myXVert2(i);
      if ( myYVert2(i) > myMaxY ) myMaxY = myYVert2(i);
    }
  } // end if myArrow is FIRSTAR or BOTHAR

  myNumOfElem = 6;
  myNumOfVert = 2;
}

void Prs2d_Length::SetAttachLinLength( const Standard_Real aLength ) 
{
  GetPntProject(aLength);
  ComputeArrows( Standard_False );
}

void Prs2d_Length::Save(Aspect_FStream& aFStream) const
{
}

void Prs2d_Length::LineCoord( gp_Pnt2d& aFirstPnt, gp_Pnt2d& aSecondPnt ) const
{
  aFirstPnt = gp_Pnt2d( myXT1, myYT1 );
  aSecondPnt = gp_Pnt2d( myXT2, myYT2 );
}

void Prs2d_Length::CallOutCoord( gp_Pnt2d& aFirstPnt, gp_Pnt2d& aSecondPnt ) const
{
  aFirstPnt = gp_Pnt2d( myAX1, myAY1 );
  aSecondPnt = gp_Pnt2d( myAX2, myAY2 );
}

Prs2d_TypeOfDist Prs2d_Length::TypeOfDist() const
{
  return myTypeDist;
}
