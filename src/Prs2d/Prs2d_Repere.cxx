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

#include <Prs2d_Repere.ixx>
#define ADDPIX 7

Prs2d_Repere::Prs2d_Repere( const Handle(Graphic2d_GraphicObject)& aGO,
                            const gp_Pnt2d&                        aPnt1,
                            const gp_Pnt2d&                        aPnt2,
                            const Standard_Real                    aLenBase,
                            const TCollection_ExtendedString&      aText,
                            const Standard_Real                    aTxtScale,
                            const Standard_Boolean                 aDrawArrow,
                            const Standard_Real                    anArrAngle,
                            const Standard_Real                    anArrLength,
                            const Prs2d_TypeOfArrow                anArrType,
                            const Prs2d_ArrowSide                  anArrow,
                            const Standard_Boolean                 IsRevArrow )

  : Prs2d_Dimension( aGO, aText, aTxtScale, anArrAngle, 
                     anArrLength, anArrType, anArrow, IsRevArrow ),
    myXVert( 1, 3 ),
    myYVert( 1, 3 ),
    myObtuse( Standard_False ),
    myDrawArrow( aDrawArrow )
{
 
  SetSymbolCode( 198 );
  myXVert(1) = Standard_ShortReal( aPnt1.X() );
  myXVert(2) = Standard_ShortReal( aPnt2.X() );
  myYVert(1) = Standard_ShortReal( aPnt1.Y() );
  myYVert(2) = Standard_ShortReal( aPnt2.Y() );

  gp_Vec2d theVec( aPnt2, aPnt1 );
  gp_Vec2d theVecX( 1., 0. );
  Standard_Real theAng = theVecX.Angle(theVec);
  Standard_Real nX = aPnt2.X();
//  Standard_Boolean isObtuse = Standard_False;
  Standard_Integer i;
 
  if ( myDrawArrow ) {
    
    gp_Pnt2d theOrig( 0., 0. ), P1, P2, P3;
    gp_Vec2d VX( 1., 0. ), VDir( aPnt1, aPnt2 );
    Standard_Real alpha = M_PI - anArrAngle;
    P1 = aPnt1;  
    P2 = gp_Pnt2d( anArrLength,  anArrLength*Tan( alpha/2. ) );
    P3 = gp_Pnt2d( anArrLength, -anArrLength*Tan( alpha/2. ) );
    if ( IsRevArrow ) VDir.Reverse();
    
    Standard_Real theAngle = VX.Angle( VDir );

    P2.Rotate( theOrig, theAngle );
    P3.Rotate( theOrig, theAngle );
  
    P2.Translate( gp_Vec2d( theOrig, P1 ) );
    P3.Translate( gp_Vec2d( theOrig, P1 ) );

    myXVert1(1) = Standard_ShortReal( P2.X() );
    myYVert1(1) = Standard_ShortReal( P2.Y() );
    myXVert1(2) = Standard_ShortReal( P1.X() );
    myYVert1(2) = Standard_ShortReal( P1.Y() );
    myXVert1(3) = Standard_ShortReal( P3.X() );
    myYVert1(3) = Standard_ShortReal( P3.Y() );

    for ( i = 1; i <= 3; i++ ) {
	   
	   if ( myXVert1(i) < myMinX ) myMinX = myXVert1(i);
           if ( myYVert1(i) < myMinY ) myMinY = myYVert1(i);
           if ( myXVert1(i) > myMaxX ) myMaxX = myXVert1(i);
	   if ( myYVert1(i) > myMaxY ) myMaxY = myYVert1(i);
	   
    }    
      
  }

  if ( theAng >= 0. && theAng < M_PI/2) {
        nX -= aLenBase;
  } else if ( theAng >= M_PI/2 && theAng <= M_PI ) {
        nX += aLenBase;
        myObtuse = Standard_True;  
  } else if ( theAng >= -M_PI && theAng < -M_PI/2) {
        nX += aLenBase;
        myObtuse = Standard_True;  
  } else if ( theAng >= -M_PI/2 && theAng < 0. ) 
        nX -= aLenBase;

  myXVert(3) = Standard_ShortReal( nX );
  myYVert(3) = Standard_ShortReal( aPnt2.Y() );

  for ( i = 1; i <= 3; i++ ) {
      if ( myXVert(i) < myMinX ) myMinX = myXVert(i);
      if ( myYVert(i) < myMinY ) myMinY = myYVert(i);
      if ( myXVert(i) > myMaxX ) myMaxX = myXVert(i);
      if ( myYVert(i) > myMaxY ) myMaxY = myYVert(i);
  }

}

void Prs2d_Repere::Draw(const Handle(Graphic2d_Drawer)& aDrawer) {

  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {

    DrawLineAttrib(aDrawer);
    
    Standard_ShortReal dxx = aDrawer->Convert(ADDPIX);
    Standard_ShortReal t1 = Standard_ShortReal( myXVert(3) + dxx ), 
                       t2 = Standard_ShortReal( myYVert(3) + dxx );
    if ( myObtuse ) {
       t1 = Standard_ShortReal( myXVert(2) + dxx );
       t2 = Standard_ShortReal( myYVert(2) + dxx );    
    } 
    Standard_Real theSmbX = t1, theSmbY = t2;
    
    if ( myIsSymbol ) {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
//      TCollection_ExtendedString txtSymb( char( SymbolCode() ) );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      Standard_ShortReal ws=0., hs=0.;
      aDrawer->GetTextSize( txtSymb, ws, hs );
      Standard_ShortReal ddd = aDrawer->Convert( 2 );
      t1 += ( ws + ddd );      
    } 
    Standard_ShortReal ts1 = Standard_ShortReal( theSmbX ),
                       ts2 = Standard_ShortReal( theSmbY );

    TShort_Array1OfShortReal Xpoint( 1, 3 ), Ypoint( 1, 3 ),
                             Xpoint1( 1, 3 ), Ypoint1( 1, 3 );

    if ( myGOPtr->IsTransformed() ) {

      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real a, b;
      for ( Standard_Integer j = 1; j <= 3; j++ ) {
		 a = Standard_Real( myXVert(j) );
		 b = Standard_Real( myYVert(j) );
                 aTrsf.Transforms( a, b );
                 Xpoint(j) = Standard_ShortReal( a );
		 Ypoint(j) = Standard_ShortReal( b );
      }
      a = Standard_Real( t1 );
      b = Standard_Real( t2 );
      aTrsf.Transforms( a, b );
      t1 = Standard_ShortReal( a );
      t2 = Standard_ShortReal( b );
      a = Standard_Real( ts1 );
      b = Standard_Real( ts2 );
      aTrsf.Transforms( a, b );
      ts1 = Standard_ShortReal( a );
      ts2 = Standard_ShortReal( b );
      if ( myDrawArrow ) {
       for ( Standard_Integer i = 1; i <= 3; i++ ) {
		 a = Standard_Real( myXVert1(i) );
		 b = Standard_Real( myYVert1(i) );
                 aTrsf.Transforms(a, b);
                 Xpoint1(i) = Standard_ShortReal(a);
		 Ypoint1(i) = Standard_ShortReal(b);
       }
      } 
    } else {
      Xpoint.Assign( myXVert );
      Ypoint.Assign( myYVert );
      if ( myDrawArrow ) {
  		Xpoint1.Assign( myXVert1 );
                Ypoint1.Assign( myYVert1 );
      }
    }

    aDrawer->MapPolylineFromTo( Xpoint, Ypoint );
    
    if ( myIsSymbol ) {
//      TCollection_ExtendedString txtSymb( char( SymbolCode() ) );
      Standard_Character code = SymbolCode();
      TCollection_ExtendedString txtSymb( code );
      Standard_ShortReal angle=0.,dx=0.,dy=0.;
      aDrawer->MapTextFromTo( txtSymb, ts1, ts2, angle, dx, dy, Aspect_TOT_SOLID);
    }    
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                            Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
    
    aDrawer->MapTextFromTo( myText, t1, t2, 0., 0., 0., Aspect_TOT_SOLID );
    
    if ( myDrawArrow ) {
       if ( myArrType == Prs2d_TOA_CLOSED || myArrType == Prs2d_TOA_FILLED ) {
	    aDrawer->MapPolygonFromTo( Xpoint1, Ypoint1 );
       } else {
         aDrawer->MapPolylineFromTo( Xpoint1, Ypoint1 );
       } 
    }      
  }
}

void Prs2d_Repere::DrawElement( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
                                const Standard_Integer /*anIndex*/ ) {
}

void Prs2d_Repere::DrawVertex( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
                               const Standard_Integer /*anIndex*/ ) {
}

Standard_Boolean Prs2d_Repere::Pick( const Standard_ShortReal X,
                                     const Standard_ShortReal Y,
                                     const Standard_ShortReal aPrecision,
                                     const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {
    
  Standard_ShortReal SRX = X, SRY = Y;

  Standard_Integer Lower = myXVert.Lower(), Upper = myXVert.Upper();
  
  if ( IsInMinMax( X, Y, aPrecision ) ) {

	if ( myGOPtr->IsTransformed() ) {
          gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
          Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	  aTrsf.Transforms (RX, RY);
	  SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
	}
    for ( int i = Lower; i < Upper; i++ ) 
      if ( IsOn( SRX, SRY, myXVert (i), myYVert(i), 
                myXVert( i + 1 ), myYVert( i + 1 ), aPrecision ) ) 
        return Standard_True;
	
    
  }    
  
  return Standard_False;
}

void Prs2d_Repere::Save(Aspect_FStream& aFStream) const
{
}
