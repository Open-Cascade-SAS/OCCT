#include <Prs2d_ToleranceFrame.ixx>
#include <Prs2d_Tolerance.hxx>
#include <Prs2d_Straightness.hxx>
#include <Prs2d_Flatness.hxx>
#include <Prs2d_Circularity.hxx>
#include <Prs2d_Cylindric.hxx>
#include <Prs2d_LineProfile.hxx>
#include <Prs2d_SurfProfile.hxx>
#include <Prs2d_Parallelism.hxx>
#include <Prs2d_Perpendicular.hxx>
#include <Prs2d_Angularity.hxx>
#include <Prs2d_Position.hxx>
#include <Prs2d_Concentric.hxx>
#include <Prs2d_Symmetry.hxx>
#include <Prs2d_SymCircular.hxx>
#include <Prs2d_SymTotal.hxx>
#include <Prs2d_Taper.hxx>

Prs2d_ToleranceFrame::Prs2d_ToleranceFrame( const Handle(Graphic2d_GraphicObject)& aGO,
                                            const Quantity_Length aX,
                                            const Quantity_Length aY,
                                            const Quantity_Length aHeight,
                                            const Prs2d_TypeOfTolerance aTolType,
                                            const Standard_CString aTolTxt,
                                            const Standard_CString aTxt1,
                                            const Standard_CString aTxt2,
                                            const Standard_Real anAngle,
                                            const Standard_Real aScale )

  : Graphic2d_Line( aGO ),
   
   myX( Standard_ShortReal( aX ) ),
   myY( Standard_ShortReal( aY ) ),
   myTolTxt( TCollection_ExtendedString(aTolTxt) ),
   myTxt1( TCollection_ExtendedString(aTxt1) ),
   myTxt2( TCollection_ExtendedString(aTxt2) ),
   myHeight( Standard_ShortReal( aHeight ) ),
   myAngle( Standard_ShortReal( anAngle ) ),
   myScale( Standard_ShortReal( aScale ) ),
   myXVert( 1, 4 ),
   myYVert( 1, 4 )
  
{

    gp_Pnt2d PntP( myX, myY );
    gp_Vec2d theV1( gp_Pnt2d( myX, myY - myHeight/2 ), PntP );
    theV1.Rotate( myAngle );
    theV1.Reverse();
    gp_Pnt2d Pnt0 = PntP.Translated( theV1 );
    gp_Vec2d theVecG( Pnt0, gp_Pnt2d( Pnt0.X() + 1.0, Pnt0.Y() ) );
    theVecG.Rotate( myAngle );
    gp_Vec2d theVecV = theVecG.Rotated( Standard_PI / 2 );
    theVecG.Normalize();
    theVecV.Normalize();

    gp_Pnt2d Pnt1 = Pnt0.Translated( theVecV * myHeight );
    myXVert(1) = Standard_ShortReal( Pnt0.X() );
    myYVert(1) = Standard_ShortReal( Pnt0.Y() );
    myXVert(2) = Standard_ShortReal( Pnt1.X() );
    myYVert(2) = Standard_ShortReal( Pnt1.Y() );
    gp_Pnt2d Pnt2 = Pnt0.Translated( theVecG * myHeight );
    myXVert(4) = Standard_ShortReal( Pnt2.X() );
    myYVert(4) = Standard_ShortReal( Pnt2.Y() );
    Pnt2 = Pnt1.Translated( theVecG * myHeight );
    myXVert(3) = Standard_ShortReal( Pnt2.X() );
    myYVert(3) = Standard_ShortReal( Pnt2.Y() );
    
    myMinX	= myX;
    myMinY	= myY;
    myMaxX	= myX;
    myMaxY	= myY;

    for ( Standard_Integer i = 1; i <= 4; i++ ) {
	   if ( myXVert(i) < myMinX ) myMinX = myXVert(i);
       if ( myYVert(i) < myMinY ) myMinY = myYVert(i);
       if ( myXVert(i) > myMaxX ) myMaxX = myXVert(i);
	   if ( myYVert(i) > myMaxY ) myMaxY = myYVert(i);
    }

    gp_Vec2d VecTol( PntP, gp_Pnt2d( myX + myHeight/2, myY ) );
    VecTol.Rotate( myAngle );
    PntP.Translate( VecTol );
    Standard_Real  xTTol   = PntP.X(),
                   yTTol   = PntP.Y(),
                   theSz1  = myHeight * 2/5,
                   theSz2  = myHeight * 3/5;
    
    switch ( aTolType )  {
        case Prs2d_TOT_TAPER:
           myHTol = new Prs2d_Taper( aGO, xTTol, yTTol, theSz2, myAngle );
        break;
        case Prs2d_TOT_SYMTOTAL:
           myHTol = new Prs2d_SymTotal( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_SYMCIRCULAR:
           myHTol = new Prs2d_SymCircular( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_SYMMETRY:
           myHTol = new Prs2d_Symmetry( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_CONCENTRIC:
           myHTol = new Prs2d_Concentric( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_POSITION:
           myHTol = new Prs2d_Position( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_ANGULARITY:
           myHTol = new Prs2d_Angularity( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_PERPENDIC:
           myHTol = new Prs2d_Perpendicular( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_PARALLELISM:
           myHTol = new Prs2d_Parallelism( aGO, xTTol, yTTol, theSz1, myAngle  );
        break;
        case Prs2d_TOT_SURFACEPROF:
           myHTol = new Prs2d_SurfProfile( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_LINEPROF:
           myHTol = new Prs2d_LineProfile( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_CYLINDRIC:
           myHTol = new Prs2d_Cylindric( aGO, xTTol, yTTol, theSz1, myAngle  );
        break;
        case Prs2d_TOT_CIRCULARITY:
           myHTol = new Prs2d_Circularity( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_FLATNESS:
           myHTol = new Prs2d_Flatness( aGO, xTTol, yTTol, theSz1, myAngle  );
        break;
        case Prs2d_TOT_STRAIGHTNESS:
           myHTol = new Prs2d_Straightness( aGO, xTTol, yTTol, theSz2, myAngle  );
        break;
        case Prs2d_TOT_NONE:
        default:
        break;

    }
}

void Prs2d_ToleranceFrame::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {

   Standard_Boolean IsIn = Standard_False;

   if ( ! myGOPtr->IsTransformed() )
     IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY,myMaxY );
   else {
     Standard_ShortReal minx, miny, maxx, maxy;
     MinMax(minx,maxx,miny,maxy);
     IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
   }

   if ( IsIn ) {

    DrawLineAttrib( aDrawer );

    gp_GTrsf2d aTrsf; 
    Standard_Real a1, b1;
  	
    TShort_Array1OfShortReal Xpoint( 1, 4 ), Ypoint( 1, 4 );
    Xpoint.Assign( myXVert );
    Ypoint.Assign( myYVert );

    if ( myGOPtr->IsTransformed() ) {
      aTrsf = myGOPtr->Transform ();
      for ( Standard_Integer j = 1; j <= 4; j++ ) {
        a1 = Standard_Real( Xpoint(j) );
        b1 = Standard_Real( Ypoint(j) );
        aTrsf.Transforms( a1, b1 );
        Xpoint(j) = Standard_ShortReal( a1 );
        Ypoint(j) = Standard_ShortReal( b1 );
      }
    }
    aDrawer->MapPolygonFromTo( Xpoint, Ypoint );
    
    //------------additional text's cells

    if ( myTolTxt.Length() || myTxt1.Length() || myTxt1.Length() ) {
      gp_Pnt2d PntP( myX, myY );
      gp_Vec2d theV1( gp_Pnt2d( myX, myY - myHeight/2 ), PntP );
      theV1.Rotate( myAngle );
      theV1.Reverse();
      gp_Pnt2d Pnt0 = PntP.Translated( theV1 ),
               PntT = PntP.Translated( theV1 / 2 );
      gp_Vec2d theVecT( PntT, gp_Pnt2d( PntT.X() + 1.0, PntT.Y() ) );
      theVecT.Rotate( myAngle );
      theVecT.Normalize();
      gp_Vec2d theVecG( Pnt0, gp_Pnt2d( Pnt0.X() + 1.0, Pnt0.Y() ) );
      theVecG.Rotate( myAngle );
      gp_Vec2d theVecV = theVecG.Rotated( Standard_PI / 2 );
      theVecG.Normalize();
      theVecV.Normalize();

      gp_Pnt2d Pnt1 = Pnt0.Translated( theVecV * myHeight );
      gp_Pnt2d Pnt2 = Pnt0.Translated( theVecG * myHeight );
    
      const Standard_ShortReal offS = Standard_ShortReal( 5 );
      aDrawer->SetTextAttrib( myColorIndex, 0, 0, Standard_ShortReal(myScale*aDrawer->Scale()), 
                              Standard_ShortReal(myScale*aDrawer->Scale()), Standard_False );
    
      Standard_ShortReal tVecSize = myHeight;
      Standard_ShortReal ws = 0.0, hs;
      gp_Pnt2d tmpPntTxt;
      Standard_ShortReal X1=0., Y1=0., X2=0., Y2=0., X3, Y3, X4, Y4;
      if ( myTolTxt.Length() ) {
         tVecSize += offS;
         tmpPntTxt = PntT.Translated( theVecT * tVecSize );
         X1 = Standard_ShortReal( tmpPntTxt.X() ); 
         Y1 = Standard_ShortReal( tmpPntTxt.Y() );
         if ( myGOPtr->IsTransformed () ) {
           aTrsf = myGOPtr->Transform();
           a1 = Standard_Real( X1 );
           b1 = Standard_Real( Y1 );
    	   aTrsf.Transforms( a1, b1 );
           X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
         }
         aDrawer->MapTextFromTo( myTolTxt, X1, Y1, myAngle, 0.0, 0.0, Aspect_TOT_SOLID );    
         aDrawer->GetTextSize( myTolTxt, ws, hs );
         tVecSize += ws  + offS;

         Pnt2 = Pnt0.Translated( theVecG * tVecSize );
         X1 = Standard_ShortReal( Pnt2.X() );
         Y1 = Standard_ShortReal( Pnt2.Y() );
         Pnt2 = Pnt1.Translated( theVecG * tVecSize );
         X2 = Standard_ShortReal( Pnt2.X() );
         Y2 = Standard_ShortReal( Pnt2.Y() );
         if ( myGOPtr->IsTransformed () ) {
           aTrsf = myGOPtr->Transform();
           a1 = Standard_Real( X1 );
           b1 = Standard_Real( Y1 );
           aTrsf.Transforms( a1, b1 );
           X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
           a1 = Standard_Real( X2 );
           b1 = Standard_Real( Y2 );
           aTrsf.Transforms( a1, b1 );
           X2 = Standard_ShortReal( a1 ); Y2 = Standard_ShortReal( b1 );
         }
         aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
       }

    if ( myTxt1.Length() ) {
        tVecSize += offS;
        tmpPntTxt = PntT.Translated( theVecT * tVecSize );
        X1 = Standard_ShortReal( tmpPntTxt.X() ); Y1 = Standard_ShortReal( tmpPntTxt.Y() );
        if ( myGOPtr->IsTransformed () ) {
           aTrsf = myGOPtr->Transform();
           a1 = Standard_Real( X1 );
           b1 = Standard_Real( Y1 );
    	   aTrsf.Transforms( a1, b1 );
           X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
        }
        aDrawer->MapTextFromTo( myTxt1, X1, Y1, myAngle, 0., 0.,Aspect_TOT_SOLID );    
        aDrawer->GetTextSize( myTxt1, ws, hs );
        tVecSize += ws + offS;

        Pnt2 = Pnt0.Translated( theVecG * tVecSize );
        X1 = Standard_ShortReal( Pnt2.X() );
        Y1 = Standard_ShortReal( Pnt2.Y() );
        Pnt2 = Pnt1.Translated( theVecG * tVecSize );
        X2 = Standard_ShortReal( Pnt2.X() );
        Y2 = Standard_ShortReal( Pnt2.Y() );
        if ( myGOPtr->IsTransformed () ) {
          aTrsf = myGOPtr->Transform();
          a1 = Standard_Real( X1 );
          b1 = Standard_Real( Y1 );
    	  aTrsf.Transforms( a1, b1 );
          X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
          a1 = Standard_Real( X2 );
          b1 = Standard_Real( Y2 );
    	  aTrsf.Transforms( a1, b1 );
          X2 = Standard_ShortReal( a1 ); Y2 = Standard_ShortReal( b1 );
        }
        aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    }

    if ( myTxt2.Length() ) {
        tVecSize += offS;
        tmpPntTxt = PntT.Translated( theVecT * tVecSize );
        X1 = Standard_ShortReal( tmpPntTxt.X() ); Y1 = Standard_ShortReal( tmpPntTxt.Y() );
        if ( myGOPtr->IsTransformed () ) {
           aTrsf = myGOPtr->Transform();
           a1 = Standard_Real( X1 );
           b1 = Standard_Real( Y1 );
    	   aTrsf.Transforms( a1, b1 );
           X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
        }
        aDrawer->MapTextFromTo( myTxt2, X1, Y1, myAngle, 0., 0., Aspect_TOT_SOLID );    
        aDrawer->GetTextSize( myTxt2, ws, hs );
        tVecSize += ws + offS;

        Pnt2 = Pnt0.Translated( theVecG * tVecSize );
        X1 = Standard_ShortReal( Pnt2.X() );
        Y1 = Standard_ShortReal( Pnt2.Y() );
        Pnt2 = Pnt1.Translated( theVecG * tVecSize );
        X2 = Standard_ShortReal( Pnt2.X() );
        Y2 = Standard_ShortReal( Pnt2.Y() );
        if ( myGOPtr->IsTransformed () ) {
          aTrsf = myGOPtr->Transform();
          a1 = Standard_Real( X1 );
          b1 = Standard_Real( Y1 );
    	  aTrsf.Transforms( a1, b1 );
          X1 = Standard_ShortReal( a1 ); Y1 = Standard_ShortReal( b1 );
          a1 = Standard_Real( X2 );
          b1 = Standard_Real( Y2 );
    	  aTrsf.Transforms( a1, b1 );
          X2 = Standard_ShortReal( a1 ); Y2 = Standard_ShortReal( b1 );
        }
        aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    }

    X3 = Standard_ShortReal( Pnt0.X() );
    Y3 = Standard_ShortReal( Pnt0.Y() );
    X4 = Standard_ShortReal( Pnt1.X() );
    Y4 = Standard_ShortReal( Pnt1.Y() );
    if ( myGOPtr->IsTransformed () ) {
      aTrsf = myGOPtr->Transform();
      a1 = Standard_Real( X3 );
      b1 = Standard_Real( Y3 );
      aTrsf.Transforms( a1, b1 );
      X3 = Standard_ShortReal( a1 ); Y3 = Standard_ShortReal( b1 );
      a1 = Standard_Real( X4 );
      b1 = Standard_Real( Y4 );
      aTrsf.Transforms( a1, b1 );
      X4 = Standard_ShortReal( a1 ); Y4 = Standard_ShortReal( b1 );
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X3, Y3 );
    aDrawer->MapSegmentFromTo( X2, Y2, X4, Y4 );
    
   } // end if one of text strings isn't empty
 } // end if IsIn
 
 if ( ! myHTol.IsNull() )
  myHTol->Draw( aDrawer );
       
}

Standard_Boolean Prs2d_ToleranceFrame::Pick( 
			const Standard_ShortReal X,
               		const Standard_ShortReal Y,
               		const Standard_ShortReal aPrecision,
               		const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {
    if ( IsInMinMax( X, Y, aPrecision ) ) 
        return Standard_True;
    return Standard_False;
}

void Prs2d_ToleranceFrame::SetHeight( const Standard_Real aHSize ) {
    
  myHeight = Standard_ShortReal( aHSize ); 
}

Standard_Real Prs2d_ToleranceFrame::Height() const {
    
  return myHeight;    
}

void Prs2d_ToleranceFrame::Save(Aspect_FStream& aFStream) const
{
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_ToleranceFrame::ArrayOfPnt2d( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 4 );
		
   for ( Standard_Integer i = 1; i <= 4; i++ ) 
   {
     points->SetValue( i, gp_Pnt2d( myXVert(i), myYVert(i) ) );
   }   
   return points;
}

void Prs2d_ToleranceFrame::SetTolText( const Standard_CString aTolTxt ) 
{
  myTolTxt = TCollection_ExtendedString(aTolTxt);
}

void Prs2d_ToleranceFrame::SetText1( const Standard_CString aTxt1 ) 
{
  myTxt1 = TCollection_ExtendedString(aTxt1);
}

void Prs2d_ToleranceFrame::SetText2( const Standard_CString aTxt2 ) 
{
  myTxt2 = TCollection_ExtendedString(aTxt2);
}

TCollection_ExtendedString Prs2d_ToleranceFrame::TolText( ) const 
{
  return myTolTxt;
}

TCollection_ExtendedString Prs2d_ToleranceFrame::Text1() const 
{
  return myTxt1;
}

TCollection_ExtendedString Prs2d_ToleranceFrame::Text2() const 
{
  return myTxt2;
}

Standard_Real Prs2d_ToleranceFrame::TextScale( ) const
{
  return myScale;
}
