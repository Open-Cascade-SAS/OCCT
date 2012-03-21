// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Prs2d_Angle.ixx>
#include <gp_Vec2d.hxx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0
#define XROTATE(x,y) ((x)*cosa - (y)*sina)
#define YROTATE(x,y) ((y)*cosa + (x)*sina)
#define ADDPIX 7

Prs2d_Angle::Prs2d_Angle( const Handle(Graphic2d_GraphicObject)& aGO,
			  const gp_Pnt2d&                   anAttachPnt1,
			  const gp_Pnt2d&                   anAttachPnt2,
			  const gp_Pnt2d&                   anAttachPnt3,
			  const Quantity_Length             aRadius,
                          const TCollection_ExtendedString& aText,
                          const Standard_Real               aTxtScale,
                          const Standard_Real               anArrAngle,
			  const Standard_Real               anArrLength,
	                  const Prs2d_TypeOfArrow           anArrType,
                          const Prs2d_ArrowSide             anArrow,
                          const Standard_Boolean            IsRevArrow ) 
  
 :     Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                       anArrLength, anArrType, anArrow, IsRevArrow ),
       myCentX( Standard_ShortReal( anAttachPnt1.X() ) ),
       myCentY( Standard_ShortReal( anAttachPnt1.Y() ) ),
       myRad( Standard_ShortReal( aRadius ) ) 
{
   
  SetSymbolCode( 176 );
  gp_Vec2d V1( anAttachPnt1, anAttachPnt2 );
  gp_Vec2d V2( anAttachPnt1, anAttachPnt3 );
  gp_Vec2d VX( 1., 0. );
  myFAngle = Standard_ShortReal( VX.Angle(V1) );
  mySAngle = Standard_ShortReal( VX.Angle(V2) );
  
  //myRad = Standard_ShortReal(anAttachPnt1.Distance( anOffsetPoint ));
  if ( myRad <= ShortRealEpsilon ())
     Graphic2d_CircleDefinitionError::Raise ("The radius = 0." );

  Standard_ShortReal TwoPI = Standard_ShortReal(2. * M_PI);
  while( myFAngle < 0. ) myFAngle += TwoPI;
  while( myFAngle > 2. * M_PI ) myFAngle -= TwoPI;
  while( mySAngle < 0. ) mySAngle += TwoPI;
  while( mySAngle > 2. * M_PI ) mySAngle -= TwoPI;
  if ( mySAngle < myFAngle ) mySAngle += TwoPI;

  if ( ( mySAngle - myFAngle < ShortRealEpsilon() ) || 
       ( mySAngle - myFAngle >= TwoPI ) ) {
	 myFAngle = 0.;
	 mySAngle = TwoPI;
  	 myMinX = myCentX - myRad;
  	 myMinY = myCentY - myRad;
  	 myMaxX = myCentX + myRad;
  	 myMaxY = myCentY + myRad;
  } else {
	 Standard_ShortReal Acur,Xcur,Ycur,Xsav;
         myMinX = myMaxX = Standard_ShortReal(Cos(myFAngle));
         myMinY = myMaxY = Standard_ShortReal(Sin(myFAngle));
	 Xcur = Standard_ShortReal(Cos(mySAngle));
	 Ycur = Standard_ShortReal(Sin(mySAngle));
         myMinX = myMinX < Xcur ? myMinX : Xcur;
         myMinY = myMinY < Ycur ? myMinY : Ycur;
         myMaxX = myMaxX > Xcur ? myMaxX : Xcur;
         myMaxY = myMaxY > Ycur ? myMaxY : Ycur;

     for ( Acur = 0., Xcur = 1., Ycur = 0.; 
	   Acur < mySAngle; Acur += Standard_ShortReal(M_PI / 2.)) {
	   if ( Acur > myFAngle ) {
           myMinX = ( myMinX < Xcur ? myMinX : Xcur );
           myMaxX = ( myMaxX < Xcur ? Xcur : myMaxX );
           myMinY = ( myMinY < Ycur ? myMinY : Ycur );
           myMaxY = ( myMaxY < Ycur ? Ycur : myMaxY );
	    }
        Xsav = Xcur; Xcur = -Ycur; Ycur = Xsav;
	 }
       myMinX = myCentX + myRad * myMinX;
       myMaxX = myCentX + myRad * myMaxX;
       myMinY = myCentY + myRad * myMinY;
       myMaxY = myCentY + myRad * myMaxY;
  }

  // Arrows

  Standard_Real ArrAngle = M_PI/180.*anArrAngle;
     
  gp_Pnt2d theOrigine(0.,0.);
  gp_Pnt2d P1, P2, P3;
  gp_Vec2d VDir;
  Standard_Real theAngle;

  if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
    P1 = gp_Pnt2d( myCentX + myRad * Cos(myFAngle),
               myCentY + myRad * Sin(myFAngle) );
  
    P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
    P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
    VDir = V1.Rotated(M_PI/2);
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
    P1 = gp_Pnt2d ( myCentX + myRad * Cos(mySAngle),
                    myCentY + myRad * Sin(mySAngle) );
  
    P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( ArrAngle/2. ) );
    P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( ArrAngle/2. ) );
  
    VDir = V2.Rotated(-M_PI/2);
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
  myNumOfVert = 3;

}

void Prs2d_Angle::Values( gp_Pnt2d        & aPnt1,
                          gp_Pnt2d        & aPnt2,
                          gp_Pnt2d        & aPnt3,
                          Quantity_Length & aRad ) const {

   aPnt1.SetCoord( myCentX, myCentY );
   gp_Vec2d VX( myRad, 0. ), V2, V3;
   V2 = VX.Rotated( myFAngle );  
   aPnt2 = aPnt1.Translated( V2 );
   V3 = VX.Rotated( mySAngle ); 
   aPnt3 = aPnt1.Translated( V3 );
   aRad = myRad;
}

void Prs2d_Angle::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {

  Standard_Boolean IsIn = Standard_False;

  if ( !myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {

    DrawLineAttrib(aDrawer);
    Standard_ShortReal theScale = Standard_ShortReal(myTextScale*aDrawer->Scale());
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0.0, theScale, theScale, Standard_False );
    
    // Text 
    /*Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    gp_Pnt2d P1 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(myFAngle),
                            myCentY + ( dxx + myRad ) * Sin(myFAngle) );
  
    gp_Pnt2d P2 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(mySAngle),
                            myCentY + ( dxx + myRad ) * Sin(mySAngle) );
  

    gp_Pnt2d theCent(myCentX, myCentY);
    gp_Vec2d V1( theCent, P1), V2( theCent, P2), VX(1., 0.);
    Standard_Real delta = V1.Angle(V2);
    gp_Vec2d textV1 = V1.Rotated( delta/2 );
    Standard_Real theAngle = VX.Angle(textV1);
    gp_Pnt2d pntText = theCent.Translated(textV1);
    Standard_Real txtX, txtY;
    pntText.Coord(txtX, txtY);
    Standard_ShortReal txtAngle = Standard_ShortReal( theAngle + 3*M_PI/2);
    
    Standard_ShortReal ws, hs;
    aDrawer->GetTextSize( myText, ws, hs );
    Standard_Real theSmbX, theSmbY;
    Standard_ShortReal ddd = aDrawer->Convert(2);
    gp_Vec2d theVec = VX.Rotated( txtAngle );
    theVec *= ( ws + ddd );
    gp_Pnt2d pntSymb = pntText.Translated( theVec );
    pntSymb.Coord( theSmbX, theSmbY );*/ //correct by enk Fri Nov 29 15:17 2002
    
    /////////////////////////////////////////
    Standard_ShortReal txtAngle=Standard_ShortReal(myAbsAngle);
    Standard_Real theSmbX,theSmbY;
    theSmbX=myAbsX;
    theSmbY=myAbsY;
    /////////////////////////////////////////
    Standard_ShortReal a = myCentX, b = myCentY, c = myRad, e = myFAngle, f = mySAngle,
                       t1 = Standard_ShortReal(myAbsX), t2 = Standard_ShortReal(myAbsY),
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
        
        Standard_Real c1, d1;

        for ( Standard_Integer j = 1; j <= 3; j++ ) {
                     c1 = Standard_Real( myXVert2(j) );
		     d1 = Standard_Real( myYVert2(j) );
                     aTrsf.Transforms (c1, d1);
                     Xpoint2(j) = Standard_ShortReal(c1);
		     Ypoint2(j) = Standard_ShortReal(d1);
         }
      } // end if myArrow is LASTAR ot BOTHAR   

      Standard_Real A, B, E, F, 
                    X0, Y0, X1, Y1, X2, Y2, T1, T2;
      A = Standard_Real( a ); 
      B = Standard_Real( b );
      E = Standard_Real( e ); 
      F = Standard_Real( f );
      T1 = Standard_Real( t1 );
      T2 = Standard_Real( t2);
          aTrsf.Transforms( A, B );
          aTrsf.Transforms( T1, T2 );
      a = Standard_ShortReal( A ); 
      b = Standard_ShortReal( B );
      t1 = Standard_ShortReal( T1 ); 
      t2 = Standard_ShortReal( T2 );
      A = Standard_Real( ts1 );
      B = Standard_Real( ts2 );	   
          aTrsf.Transforms( A, B );
      ts1 = Standard_ShortReal( A );
      ts2 = Standard_ShortReal( B );

	  if ( Abs(f-e) < Standard_ShortReal(2. * M_PI) ) {
	    aTrsf.SetValue( 1, 3, 0.0 );
	    aTrsf.SetValue( 2, 3, 0.0 );

	    X1 = Cos(E); 
            Y1 = Sin(E );
	    X2 = Cos(F); 
            Y2 = Sin(F );
	    aTrsf.Transforms( X1, Y1 ); 
	    aTrsf.Transforms( X2, Y2 ); 
            X0 = Y0 = 0.;
	    aTrsf.Transforms( X0, Y0 );
	    X1 -= X0; 
            Y1 -= Y0;
	    X2 -= X0; 
            Y2 -= Y0;
            c = myRad * Standard_ShortReal( Sqrt(X1*X1 + Y1*Y1) );
            e = Standard_ShortReal( atan2(Y1,X1) );
            f = Standard_ShortReal( atan2(Y2,X2) );
            if ( aTrsf.IsNegative() ) {
              Standard_ShortReal t = e; e = f; f = t; 
            }
          } // end if Abs(f-e) < 2PI
   } else {
   
    if ( ( myArrow == Prs2d_AS_FIRSTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ){
      Xpoint1.Assign( myXVert1 );
      Ypoint1.Assign( myYVert1 );
    }
    if ( ( myArrow == Prs2d_AS_LASTAR ) || ( myArrow == Prs2d_AS_BOTHAR ) ) {
      Xpoint2.Assign( myXVert2 );
      Ypoint2.Assign( myYVert2 );
    }
        
   } // end if GO is transformed

   aDrawer->MapArcFromTo( a, b, c, e, f );
   aDrawer->MapTextFromTo( myText, t1, t2, txtAngle, 0., 0., Aspect_TOT_SOLID );

   if ( myIsSymbol ) {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
//      TCollection_ExtendedString txtSymb( char( SymbolCode() ) );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      Standard_ShortReal dx=0.,dy=0.;
      aDrawer->MapTextFromTo( txtSymb, ts1, ts2, txtAngle, dx, dy, Aspect_TOT_SOLID );
    }


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

void Prs2d_Angle::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                               const Standard_Integer anIndex) {
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
    
    // Text 
    /*
    Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    gp_Pnt2d P1 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(myFAngle),
                            myCentY + ( dxx + myRad ) * Sin(myFAngle) );
  
    gp_Pnt2d P2 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(mySAngle),
                            myCentY + ( dxx + myRad ) * Sin(mySAngle) );
  

    gp_Pnt2d theCent(myCentX, myCentY);
    gp_Vec2d V1( theCent, P1), V2( theCent, P2), VX(1, 0);
    Standard_Real delta = V1.Angle(V2);
    gp_Vec2d textV1 = V1.Rotated( delta/2 );
    Standard_Real theAngle = VX.Angle(textV1);
    gp_Pnt2d pntText = theCent.Translated(textV1);
    Standard_Real txtX, txtY;
    pntText.Coord(txtX, txtY);
    Standard_ShortReal txtAngle = Standard_ShortReal( theAngle + 3*M_PI/2);*/
    //correct by enk Mon Dec 2 11:39 2002
    
    
    /////////////////////////////////////////
    Standard_ShortReal txtAngle=Standard_ShortReal(myAbsAngle);
    Standard_Real theSmbX,theSmbY;
    theSmbX=myAbsX;
    theSmbY=myAbsY;
    /////////////////////////////////////////
    Standard_ShortReal a = myCentX, b = myCentY, c = myRad, e = myFAngle, 
                       f = mySAngle,t1 = Standard_ShortReal( myAbsX ),
                       t2 = Standard_ShortReal( myAbsY );
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
      Standard_Real a1, b1;
      
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

          Standard_Real A, B, E, F, 
                    X0, Y0, X1, Y1, X2, Y2;
	  A = Standard_Real( a ); 
          B = Standard_Real( b );
	  E = Standard_Real( e ); 
          F = Standard_Real( f );
          aTrsf.Transforms( A, B );
          a = Standard_ShortReal( A ); 
          b = Standard_ShortReal( B );
      
	      if ( Abs(f-e) < Standard_ShortReal(2. * M_PI) ) {
	        aTrsf.SetValue( 1, 3, 0.0 );
	        aTrsf.SetValue( 2, 3, 0.0 );
	        X1 = Cos(E); 
                Y1 = Sin(E );
	        X2 = Cos(F); 
                Y2 = Sin(F );
	        aTrsf.Transforms( X1, Y1 ); 
	        aTrsf.Transforms( X2, Y2 ); 
                X0 = Y0 = 0.;
	        aTrsf.Transforms( X0, Y0 );
	        X1 -= X0; 
                Y1 -= Y0;
	        X2 -= X0; 
                Y2 -= Y0;
                c = myRad * Standard_ShortReal( Sqrt(X1*X1 + Y1*Y1) );
                e = Standard_ShortReal( atan2(Y1,X1) );
                f = Standard_ShortReal( atan2(Y2,X2) );
                if ( aTrsf.IsNegative() ) {
                  Standard_ShortReal t = e; e = f; f = t; 
                }
              } // end if Abs(f-e) < 2PI
         
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
        aDrawer->MapArcFromTo( a, b, c, e, f );
    else if ( anIndex == 3 )
        aDrawer->MapTextFromTo( myText, t1, t2, txtAngle, 0., 0., Aspect_TOT_SOLID );
  
  } // end if IsIn is true 

}

void Prs2d_Angle::DrawVertex (const Handle(Graphic2d_Drawer)& aDrawer,
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
   if ( anIndex > 0 && anIndex < 4 ) {
    DrawMarkerAttrib( aDrawer );
    Standard_ShortReal X=0.,Y=0.;
    if ( anIndex == 1 ) {
        X = myCentX; Y = myCentY; 
    } else if ( anIndex == 2 ) {
        X = Standard_ShortReal(myCentX + myRad *Cos( myFAngle ));
        Y = Standard_ShortReal(myCentY + myRad *Sin( myFAngle ));
    } else if ( anIndex == 3 ) {
        X = Standard_ShortReal( myCentX + myRad *Cos( mySAngle ) );
        Y = Standard_ShortReal( myCentY + myRad *Sin( mySAngle ) );
    }
    if ( myGOPtr->IsTransformed() ) {
        gp_GTrsf2d aTrsf = myGOPtr->Transform ();
        Standard_Real A, B;
        A = Standard_Real( X );
        B = Standard_Real( Y );
        aTrsf.Transforms (A, B);
        X = Standard_ShortReal( A );
        Y = Standard_ShortReal( B );
    } 
     aDrawer->MapMarkerFromTo( VERTEXMARKER, X, Y,
			DEFAULTMARKERSIZE,DEFAULTMARKERSIZE, 0.0 );
    }
  }  // end if IsIn is true         

}

Standard_Boolean Prs2d_Angle::Pick( const Standard_ShortReal X,
	                            const Standard_ShortReal Y,
				    const Standard_ShortReal aPrecision,
				    const Handle(Graphic2d_Drawer)& aDrawer) {

  Standard_Boolean Result = Standard_False;
  Standard_ShortReal SRX = X, SRY = Y;

  if ( IsInMinMax( X, Y, aPrecision ) ) {

    if ( myGOPtr->IsTransformed() ) {
          gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted ();
          Standard_Real RX = Standard_Real( SRX ), RY = Standard_Real( SRY );
	  aTrsf.Transforms( RX, RY );
	  SRX = Standard_ShortReal( RX ); 
          SRY = Standard_ShortReal( RY );
    }
    Standard_ShortReal x1 = Standard_ShortReal(myRad *Cos( myFAngle ) + myCentX),
                       y1 = Standard_ShortReal(myRad *Sin( myFAngle ) + myCentY),
                       x2 = Standard_ShortReal(myRad *Cos( mySAngle ) + myCentX),
                       y2 = Standard_ShortReal(myRad *Sin( mySAngle ) + myCentY);


     if ( Graphic2d_Primitive::IsOn( SRX, SRY, myCentX, myCentY, aPrecision ) ) {
         SetPickedIndex(-1);    
         return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, x1, y1, aPrecision ) ) {
         SetPickedIndex(-2);    
         return Standard_True;
      } else if ( Graphic2d_Primitive::IsOn( SRX, SRY, x2, y2, aPrecision ) ) {
         SetPickedIndex(-3);    
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
    // Text 
    /*Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    gp_Pnt2d P1 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(myFAngle),
                            myCentY + ( dxx + myRad ) * Sin(myFAngle) );
  
    gp_Pnt2d P2 = gp_Pnt2d( myCentX + ( dxx + myRad ) * Cos(mySAngle),
                            myCentY + ( dxx + myRad ) * Sin(mySAngle) );
  

    gp_Pnt2d theCent(myCentX, myCentY);
    gp_Vec2d V1( theCent, P1), V2( theCent, P2), VX(1, 0);
    Standard_Real delta = V1.Angle(V2);
    gp_Vec2d textV1 = V1.Rotated( delta/2 );
    Standard_Real theAngle = VX.Angle(textV1);
    gp_Pnt2d pntText = theCent.Translated(textV1);
    Standard_Real txtX, txtY;
    pntText.Coord(txtX, txtY);
    Standard_ShortReal txtAngle = Standard_ShortReal( theAngle + 3*M_PI/2);*/
    //correct by enk Mon Dec 2 11:41 2002

    /////////////////////////////////////////
    Standard_Real txtX,txtY,txtA;
    txtX=myAbsX;
    txtY=myAbsY;
    txtA=myAbsAngle;
    Standard_ShortReal txtAngle=Standard_ShortReal(txtA);
    /////////////////////////////////////////   
    
    Standard_ShortReal cosa = Standard_ShortReal(Cos( -txtAngle )),
                       sina = Standard_ShortReal(Sin( -txtAngle )),
                       dx = TX - Standard_ShortReal(txtX),
                       dy = TY - Standard_ShortReal(txtY), 
                       x = XROTATE(dx,dy),
                       y = YROTATE(dx,dy);

    Result   =   (x >= -aPrecision + xoffset)            
            &&   (x <= width + xoffset + aPrecision)
            &&   (y >= -yoffset - aPrecision) 
            &&   (y <= height - yoffset + aPrecision);
  
    if ( Result ) {
      SetPickedIndex(3);
      return Standard_True;
    }

    if ( Abs( Sqrt( (myCentX - SRX)*(myCentX - SRX) + 
        (myCentY - SRY)*(myCentY - SRY) ) - myRad ) < aPrecision ) {
      SetPickedIndex(4);
      return Standard_True;
    }
    
  }	// end if IsInMinMax is true 

  return Standard_False;
}

void Prs2d_Angle::Save(Aspect_FStream& aFStream) const
{
}
