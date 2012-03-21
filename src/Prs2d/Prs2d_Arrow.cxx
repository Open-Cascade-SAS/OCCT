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

#include <Prs2d_Arrow.ixx>
#include <gp_GTrsf2d.hxx>

#define VERTEXMARKER 2
#define DEFAULTMARKERSIZE 3.0

Prs2d_Arrow::Prs2d_Arrow( const Handle(Graphic2d_GraphicObject)& aGraphicObject,
						  const gp_Ax2d                        & aLocation,
						  const Standard_Real                    anAngle,
						  const Standard_Real                    aLength,
    					  const Prs2d_TypeOfArrow                theType ) 

   :  Graphic2d_Line (aGraphicObject),
      myXVert( 1, 3 ),
      myYVert( 1, 3 ),
      myTypeOfArrow( theType )
{

     gp_Pnt2d theOrigine(0.,0.);
     gp_Pnt2d P1( aLocation.Location() );
     gp_Dir2d AxeX( 1., 0. );
     Standard_Real theAngle = AxeX.Angle( aLocation.Direction() );        
     theAngle += M_PI;
     gp_Pnt2d P2( aLength,  aLength*Tan( anAngle/2. ) );
     gp_Pnt2d P3( aLength, -aLength*Tan( anAngle/2. ) );
     P2.Rotate( theOrigine, theAngle );
     P3.Rotate( theOrigine, theAngle );
     P2.Translate( gp_Vec2d( theOrigine, P1 ) );
     P3.Translate( gp_Vec2d( theOrigine, P1 ) );
      
     myXVert(1) = Standard_ShortReal( P2.X() );
     myYVert(1) = Standard_ShortReal( P2.Y() );
     myXVert(2) = Standard_ShortReal( P1.X() );
     myYVert(2) = Standard_ShortReal( P1.Y() );
     myXVert(3) = Standard_ShortReal( P3.X() );
     myYVert(3) = Standard_ShortReal( P3.Y() );

     for ( Standard_Integer i = 1; i <= 3; i++ ) {
		
	if ( myXVert(i) < myMinX ) myMinX = myXVert(i);
        if ( myYVert(i) < myMinY ) myMinY = myYVert(i);
        if ( myXVert(i) > myMaxX ) myMaxX = myXVert(i);
	if ( myYVert(i) > myMaxY ) myMaxY = myYVert(i);
		
      }

}

void Prs2d_Arrow::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {

  Standard_Boolean IsIn = Standard_False;

  if ( !myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
	gp_GTrsf2d aTrsf = myGOPtr->Transform();
    Standard_Real MINX, MINY, MAXX, MAXY;
	MINX = Standard_Real( myMinX );
    MINY = Standard_Real( myMinY );
    MAXX = Standard_Real( myMaxX );
    MAXY = Standard_Real( myMaxY );
    aTrsf.Transforms( MINX, MINY );
    aTrsf.Transforms( MAXX, MAXY );
    minx = Standard_ShortReal( MINX );
    miny = Standard_ShortReal( MINY );
    maxx = Standard_ShortReal( MAXX );
    maxy = Standard_ShortReal( MAXY );
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

  if ( IsIn ) {
    DrawLineAttrib(aDrawer);
    TShort_Array1OfShortReal Xpoint( 1, 3 );
    TShort_Array1OfShortReal Ypoint( 1, 3 );
		
    if ( myGOPtr->IsTransformed () ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform ();
      Standard_Real a, b;
      for ( Standard_Integer j = 1; j <= 3; j++ ) {
        a = Standard_Real( myXVert(j) );
        b = Standard_Real( myYVert(j) );
        aTrsf.Transforms (a, b);
        Xpoint(j) = Standard_ShortReal (a);
        Ypoint(j) = Standard_ShortReal (b);
      }
      
    }	
    else {
      Xpoint.Assign( myXVert );
      Ypoint.Assign( myYVert );
		
    }

    if( myTypeOfArrow == Prs2d_TOA_CLOSED || 
        myTypeOfArrow == Prs2d_TOA_FILLED ) 
      aDrawer->MapPolygonFromTo( Xpoint, Ypoint);
    else 
      aDrawer->MapPolylineFromTo(Xpoint, Ypoint);
	 	 
  }

}

void Prs2d_Arrow::DrawElement( const Handle(Graphic2d_Drawer)& aDrawer,
                               const Standard_Integer anIndex) {

  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed () )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  Standard_Integer nbpoints = myXVert.Length();
  if ( IsIn ) {
    if( anIndex > 0 ) {
     Standard_ShortReal X1 = myXVert(anIndex) , Y1 = myYVert(anIndex), 
                        X2, Y2;
     if ( anIndex < nbpoints ) { 
        X2 = myXVert(anIndex + 1);
        Y2 = myYVert(anIndex + 1);
     } else if ( anIndex == nbpoints ) {
        X2 = myXVert(myXVert.Lower());
        Y2 = myYVert(myXVert.Lower());
     } else return;
     DrawLineAttrib( aDrawer );
     if ( myGOPtr->IsTransformed() ) {
       gp_GTrsf2d aTrsf = myGOPtr->Transform();
       Standard_Real A, B;
       A = Standard_Real( X1 );
       B = Standard_Real( Y1 );
       aTrsf.Transforms( A, B );
       X1 = Standard_ShortReal( A );
       Y1 = Standard_ShortReal( B );
       A = Standard_Real( X2 );
       B = Standard_Real( Y2 );
       aTrsf.Transforms( A, B );
       X2 = Standard_ShortReal( A );
       Y2 = Standard_ShortReal( B );
     }  // end if is transformed
     aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    } 
  } // end if is in
  
}

void Prs2d_Arrow::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
                              const Standard_Integer anIndex ) {

 Standard_Boolean IsIn = Standard_False;

 if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
 else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
 }

 Standard_Integer nbpoints = myXVert.Length();
 if ( IsIn ) {
  if ( anIndex > 0 && anIndex <= nbpoints ) { 
    Standard_ShortReal X,Y;
    DrawMarkerAttrib( aDrawer );
    if ( myGOPtr->IsTransformed() ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
     Standard_Real A, B;
     A = Standard_Real( myXVert(anIndex) );
     B = Standard_Real( myYVert(anIndex) );
     aTrsf.Transforms( A, B );
     X = Standard_ShortReal( A );
     Y = Standard_ShortReal( B );
    } else {
     X = Standard_ShortReal( myXVert(anIndex) );
     Y = Standard_ShortReal( myYVert(anIndex) );
    }
    aDrawer->MapMarkerFromTo( VERTEXMARKER, X, Y,
			DEFAULTMARKERSIZE, DEFAULTMARKERSIZE, 0.0);
   }
 }
  
}

Standard_Boolean Prs2d_Arrow::Pick( const Standard_ShortReal X,
								    const Standard_ShortReal Y,
									const Standard_ShortReal aPrecision, 
								    const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {

 Standard_ShortReal SRX = X, SRY = Y;
 Standard_Integer i, theLow = myXVert.Lower(), theUp = myXVert.Upper();;
 
 if ( IsInMinMax( X, Y, aPrecision ) ) {
    if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = ( myGOPtr->Transform() ).Inverted();
      Standard_Real RX = Standard_Real( SRX ), 
                    RY = Standard_Real( SRY );
	  aTrsf.Transforms( RX, RY );
	  SRX = Standard_ShortReal( RX ); 
      SRY = Standard_ShortReal( RY );
	}

    
    for ( i = theLow; i <= theUp; i++ ) {
      if ( Graphic2d_Primitive::IsOn( SRX, SRY, myXVert(i), myYVert(i), aPrecision) ) {
         SetPickedIndex(-i);
         return Standard_True;
      } else if ( (i < theUp) && IsOn( SRX, SRY, myXVert(i), myYVert(i),
                                 myXVert(i+1), myYVert(i+1), aPrecision ) ) {
         SetPickedIndex(i);
         return Standard_True;
      } 
    }

	if ( myTypeOfArrow == Prs2d_TOA_CLOSED || 
		 myTypeOfArrow == Prs2d_TOA_FILLED ) 
	  if ( IsOn( SRX, SRY, myXVert(theUp), myYVert(theUp),
                 myXVert(theLow), myYVert(theLow), aPrecision ) ) {
         SetPickedIndex( theUp );
         return Standard_True;
      }

	  if ( IsIn( SRX, SRY,  myXVert, myYVert, aPrecision ) ) {
	      SetPickedIndex(0);
	      return Standard_True;
	  }
	
	  return Standard_False;

 } else
	return Standard_False;

}

void Prs2d_Arrow::Save(Aspect_FStream& aFStream) const
{
}

Prs2d_TypeOfArrow Prs2d_Arrow::TypeOfArrow( ) const
{
  return myTypeOfArrow;
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Arrow::ArrayOfPnt2d( const Standard_Boolean withTrsf ) const
{
   Handle(TColgp_HArray1OfPnt2d) point = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   Standard_Boolean isTrsf = ( myGOPtr->IsTransformed() && withTrsf );
   gp_GTrsf2d aTrsf = myGOPtr->Transform();
   Standard_Real a, b;
   for ( Standard_Integer j = 1; j <= 3; j++ ) {
     if( isTrsf )
     {
       a = Standard_Real( myXVert(j) );
       b = Standard_Real( myYVert(j) );
       aTrsf.Transforms (a, b);
       point->SetValue( j, gp_Pnt2d(a, b) );
     }
     else
       point->SetValue( j, gp_Pnt2d( myXVert(j), myYVert(j) ) );
   }   
   return point;
}
