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

#include <Prs2d_RadiusIndep.ixx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

Prs2d_RadiusIndep::Prs2d_RadiusIndep( const Handle(Graphic2d_GraphicObject) & aGO, 
                                      const gp_Pnt2d                        & anAttachPnt, 
                                      const gp_Pnt2d                        & aCenter,
                                      const Standard_Real                     aRad,
                                      const TCollection_ExtendedString      & aText,
                                      const Standard_Real                     aLength, 
                                      const Prs2d_TypeOfRadius                aTypeRad,
                                      const Standard_Real                     aTxtScale ) 

  : Graphic2d_Line( aGO ),
  
    myX1( Standard_ShortReal( anAttachPnt.X() ) ),
	myY1( Standard_ShortReal( anAttachPnt.Y() ) ),
	myX2( Standard_ShortReal( aCenter.X() ) ),
	myY2( Standard_ShortReal( aCenter.Y() ) ),
    myTextScale( Standard_ShortReal( aTxtScale ) ),
    myText( aText ),
    myXVert( 1, 3 ),
	myYVert( 1, 3 ),
    myIsSymbol( Standard_False ),
    mySymbCode( 198 ),
    myTextFont( 0 ),
    mySymbFont( 0 ),
    myAttPnt( anAttachPnt ),
    myCenter( aCenter ),
    myRadius( aRad ),
    myLength( aLength ),
    myRadType( aTypeRad )
    
{
   gp_Vec2d theVec1( gp_Pnt2d( myX2, myY2 ), gp_Pnt2d( myX1, myY1 ) );
   gp_Vec2d theUnVec( 1., 0. );
   myTextAng = Standard_ShortReal( theUnVec.Angle( theVec1 ) );
   Standard_Real theLen = sqrt( (myX2-myX1)*(myX2-myX1) + (myY2-myY1)*(myY2-myY1) );
   gp_Vec2d VecAdd = theVec1.Normalized();
   theVec1.Normalize();
   theVec1 *= ( theLen + aLength );
   gp_Pnt2d P1 = aCenter.Translated( theVec1 );
   myTextX = Standard_ShortReal( P1.X() );
   myTextY = Standard_ShortReal( P1.Y() );
   gp_Pnt2d PArr( myX1, myY1 );
    
   if ( ( theLen + aLength ) > theLen ) 
     myInside = Standard_False;
   else 
     myInside = Standard_True;

  switch( aTypeRad ) {
   default:
   case Prs2d_TOR_STANDARD : {

     myX2 = Standard_ShortReal( P1.X() );
	 myY2 = Standard_ShortReal( P1.Y() );
     if  ( ( theLen + aLength ) > theLen ) {
         theVec1.Reverse();    
     }
   }
   break;          
   case Prs2d_TOR_CENTER : {
     
     if ( aLength > 0.0 ) {
       myX1 = Standard_ShortReal( P1.X() );
	   myY1 = Standard_ShortReal( P1.Y() );
       theVec1.Reverse();      
     }
   }
   break;
   case Prs2d_TOR_REVARROW : {
       
     myX2 = Standard_ShortReal( P1.X() );
	 myY2 = Standard_ShortReal( P1.Y() );
     if  ( ( theLen + aLength ) <= theLen ) 
         theVec1.Reverse();    
     else 
         VecAdd.Reverse();
     Standard_Real Dt = sqrt( ( myX2 - myX1 )*( myX2 - myX1 ) + ( myY2 - myY1 )*( myY2 - myY1 ) );
     VecAdd *= Dt/2;
     gp_Pnt2d PTmpn = PArr.Translated( VecAdd );
     myX1 = Standard_ShortReal( PTmpn.X() );
     myY1 = Standard_ShortReal( PTmpn.Y() );
   }

   break;
   case Prs2d_TOR_CENTREV : {

     if ( aLength > 0.0 ) {
       myX1 = Standard_ShortReal( P1.X() );
	   myY1 = Standard_ShortReal( P1.Y() );
     } else { 
       theVec1.Reverse();
       Standard_Real Dt = sqrt( ( myX2 - myX1 )*( myX2 - myX1 ) + ( myY2 - myY1 )*( myY2 - myY1 ) );
       VecAdd *= Dt/2;
       gp_Pnt2d PTmpn = PArr.Translated( VecAdd );
       myX1 = Standard_ShortReal( PTmpn.X() );
       myY1 = Standard_ShortReal( PTmpn.Y() );
     }
    }
    break;    
   } // end switch

   myMinX	= myX1;
   myMinY	= myY1;
   myMaxX	= myX1;
   myMaxY	= myY1;

   if ( myX2 < myMinX ) myMinX = myX2;
   if ( myY2 < myMinY ) myMinY = myY2;
   if ( myX2 > myMaxX ) myMaxX = myX2;
   if ( myY2 > myMaxY ) myMaxY = myY2;
   
   Standard_Real ArrANG = M_PI / 6., theArrLN  = aRad/5., theANG;
   gp_Pnt2d theOrig( 0., 0. ), P2, P3;
   gp_Vec2d VX( 1., 0. );
 
   P2 = gp_Pnt2d( theArrLN,  theArrLN*Tan( ArrANG/2. ) );
   P3 = gp_Pnt2d( theArrLN, -theArrLN*Tan( ArrANG/2. ) );
   theVec1.Reverse();
   theANG = VX.Angle( theVec1 );
   P2.Rotate( theOrig, theANG );
   P3.Rotate( theOrig, theANG );
  
   P2.Translate( gp_Vec2d( theOrig, PArr ) );
   P3.Translate( gp_Vec2d( theOrig, PArr ) );
  
   myXVert(1) = Standard_ShortReal( P2.X() );
   myYVert(1) = Standard_ShortReal( P2.Y() );
   myXVert(2) = Standard_ShortReal( PArr.X() );
   myYVert(2) = Standard_ShortReal( PArr.Y() );
   myXVert(3) = Standard_ShortReal( P3.X() );
   myYVert(3) = Standard_ShortReal( P3.Y() );

   for ( Standard_Integer i = 1; i <= 3; i++ ) {
	   
	   if ( myXVert(i) < myMinX ) myMinX = myXVert(i);
       if ( myYVert(i) < myMinY ) myMinY = myYVert(i);
       if ( myXVert(i) > myMaxX ) myMaxX = myXVert(i);
	   if ( myYVert(i) > myMaxY ) myMaxY = myYVert(i);
	   
   }
}

void Prs2d_RadiusIndep::Values(gp_Pnt2d& anAttPnt,
                               gp_Pnt2d& aCenter,
                               Standard_Real& aRad,
                               TCollection_ExtendedString& aText,
                               Standard_Real& aLength,
                               Prs2d_TypeOfRadius& aTypeRad,
                               Standard_Real& aTxtScale) const {

    anAttPnt   = myAttPnt;
    aCenter    = myCenter;
    aRad       = myRadius;
    aText      = myText;
    aLength    = myLength;
    aTypeRad   = myRadType;
    aTxtScale  = myTextScale;
}

void Prs2d_RadiusIndep::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {

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
    aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, Standard_ShortReal( myTextScale*aDrawer->Scale() ), 
                            Standard_ShortReal( myTextScale*aDrawer->Scale()), Standard_False );

    Standard_ShortReal aWidth, aHeight;
    aDrawer->GetTextSize( myText, aWidth, aHeight );
    Standard_ShortReal Xt = myTextX, Yt = myTextY;
    if ( !myInside ) {
        gp_Pnt2d P1( myX1, myY1 ), P2( myX2, myY2 ), 
                 PT( myTextX, myTextY ), PTMP;
        ( PT.IsEqual( P1, 0. ) ) 
        ? PTMP.SetCoord( P2.X(), P2.Y() ) 
        : PTMP.SetCoord( P1.X(), P1.Y() );
        gp_Vec2d theVec1( PT, PTMP );
        theVec1.Normalize();
        theVec1 *= aWidth;
        gp_Pnt2d PTT = PT.Translated( theVec1 );
        Xt = Standard_ShortReal( PTT.X() ); 
        Yt = Standard_ShortReal( PTT.Y() );
    }

    Standard_Real theSmbX = 0., theSmbY = 0.;
    
    if ( myIsSymbol ) {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
      TCollection_ExtendedString txtSymb = TCollection_ExtendedString( char( mySymbCode ) );
      theSmbX = Xt; theSmbY = Yt;
      gp_Pnt2d pntText( Xt, Yt );
      Standard_ShortReal ws, hs;
      aDrawer->GetTextSize( txtSymb, ws, hs );
      Standard_ShortReal ddd = aDrawer->Convert(2);
      gp_Vec2d VX( 1., 0. ), theVec;
      theVec = VX.Rotated( myTextAng );
      theVec *= ( ws + ddd );
      gp_Pnt2d pntSymb = pntText.Translated( theVec );
      Standard_Real txtX, txtY;
      pntSymb.Coord( txtX, txtY );
      Xt = Standard_ShortReal( txtX ); 
      Yt = Standard_ShortReal( txtY );
    } 

    Standard_ShortReal a = myX1, b = myY1, 
                       c = myX2, d = myY2,
                       e = Xt,   f = Yt, 
                       w = 0.,   h = Standard_ShortReal( aHeight/5 ),
                       ts1 = Standard_ShortReal( theSmbX ), 
                       ts2 = Standard_ShortReal( theSmbY );

    TShort_Array1OfShortReal Xpoint( 1, 3 ), Ypoint( 1, 3 );
    Xpoint.Assign( myXVert );
    Ypoint.Assign( myYVert ); 

    if ( myGOPtr->IsTransformed() ) {

      gp_GTrsf2d aTrsf = myGOPtr->Transform();
      Standard_Real A1, B1;
      A1 = Standard_Real( a ); B1 = Standard_Real( b );
      aTrsf.Transforms( A1, B1 );
      a = Standard_ShortReal( A1 ); b = Standard_ShortReal( B1 );
      A1 = Standard_Real( c ); B1 = Standard_Real( d );
      aTrsf.Transforms( A1, B1 );
      c = Standard_ShortReal( A1 ); d = Standard_ShortReal( B1 );
      A1 = Standard_Real( e ); B1 = Standard_Real( f );
      aTrsf.Transforms( A1, B1 );
      e = Standard_ShortReal( A1 ); f = Standard_ShortReal( B1 );
      A1 = Standard_Real( w ); B1 = Standard_Real( h );
      aTrsf.Transforms( A1, B1 );
      w = Standard_ShortReal( A1 ); h = Standard_ShortReal( B1 );
      A1 = Standard_Real( ts1 ); B1 = Standard_Real( ts2 );
      aTrsf.Transforms( A1, B1 );
      ts1 = Standard_ShortReal( A1 ); ts2 = Standard_ShortReal( B1 );
  
      for ( int j = 1; j <= 3; j++ ) {
	   A1 = Standard_Real( myXVert(j) );
	   B1 = Standard_Real( myYVert(j) );
       aTrsf.Transforms( A1, B1 );
       Xpoint(j) = Standard_ShortReal(A1);
	   Ypoint(j) = Standard_ShortReal(B1);
      }
   
     }

     DrawLineAttrib( aDrawer );
     aDrawer->MapSegmentFromTo( a, b, c, d );
     aDrawer->MapPolygonFromTo( Xpoint, Ypoint);
     if ( myIsSymbol ) {
      aDrawer->SetTextAttrib( myColorIndex, mySymbFont, 0, Standard_ShortReal(myTextScale*aDrawer->Scale()), 
                              Standard_ShortReal(myTextScale*aDrawer->Scale()), Standard_False );
      TCollection_ExtendedString txtSymb = TCollection_ExtendedString( char( mySymbCode ) );
      aDrawer->MapTextFromTo( txtSymb, ts1, ts2, myTextAng, 0., 0., Aspect_TOT_SOLID );
    }

     aDrawer->SetTextAttrib( myColorIndex, myTextFont, 0, Standard_ShortReal( myTextScale*aDrawer->Scale() ), 
                            Standard_ShortReal( myTextScale*aDrawer->Scale()), Standard_False );

     aDrawer->MapTextFromTo( myText, e, f, myTextAng, 0., 0., Aspect_TOT_SOLID ); 
     
  }
}

void Prs2d_RadiusIndep::DrawElement( const Handle(Graphic2d_Drawer)& /*aDrawer*/, 
                                     const Standard_Integer /*anIndex*/ ) {

}

void Prs2d_RadiusIndep::DrawVertex( const Handle(Graphic2d_Drawer)& /*aDrawer*/, 
                                    const Standard_Integer /*anIndex*/ ) {
}

Standard_Boolean Prs2d_RadiusIndep::Pick( const Standard_ShortReal X,
                                          const Standard_ShortReal Y,
                                          const Standard_ShortReal aPrecision,
                                          const Handle(Graphic2d_Drawer)& /*aDrawer*/) {
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
   Result = IsOn( SRX, SRY, myX1, myY1, myX2, myY2, aPrecision );
   }
   return Result;

}

void Prs2d_RadiusIndep::SetTextScale( const Standard_Real aTxtScale ) {
     myTextScale = Standard_ShortReal( aTxtScale );
}

void Prs2d_RadiusIndep::SetText( const TCollection_ExtendedString & aText ) {
     myText = aText;
}

void Prs2d_RadiusIndep::DrawSymbol(const Standard_Boolean isDraw ) {
  myIsSymbol = isDraw;
}
 
void Prs2d_RadiusIndep::SetSymbolCode( const Standard_Integer aCode ) {
  mySymbCode = aCode;
}

void Prs2d_RadiusIndep::SetTextFont(const Standard_Integer aTF) {
    myTextFont = aTF;
}
 
void Prs2d_RadiusIndep::SetFontOfSymb(const Standard_Integer aFS) {
   mySymbFont = aFS;
}

Standard_Boolean Prs2d_RadiusIndep::IsDrawSymbol() const {
  return myIsSymbol;
}

Standard_Integer Prs2d_RadiusIndep::SymbolCode() const {
  return mySymbCode;
 
}

Standard_Integer Prs2d_RadiusIndep::TextFont() const {
  return myTextFont;
}
 
Standard_Integer Prs2d_RadiusIndep::FontOfSymb() const {
  return mySymbFont;
}

void Prs2d_RadiusIndep::Save(Aspect_FStream& aFStream) const
{
}
