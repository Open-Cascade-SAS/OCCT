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

#include <Prs2d_SymTotal.ixx>

Prs2d_SymTotal::Prs2d_SymTotal( const Handle(Graphic2d_GraphicObject)& aGO,
                                const Standard_Real aX,
                                const Standard_Real aY,
                                const Standard_Real aLength,
                                const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{         
}

void Prs2d_SymTotal::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
 Standard_Boolean IsIn = Standard_False;    
  
  if ( !myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {
    DrawLineAttrib( aDrawer );
    const Standard_Real alpha = M_PI / 12;

    Standard_ShortReal X1 = myX - myLength/2, 
                       Y1 = myY - myLength/2, 
                       X2 = Standard_ShortReal(X1 + myLength * Cos( M_PI / 4 ) ), 
                       Y2 = myY + myLength/2,

                       X5 = myX,
                       Y5 = myY - myLength/2,
                       X6 = Standard_ShortReal(X5 + myLength * Cos( M_PI / 4 ) ), 
                       Y6 = myY + myLength/2;
    
    gp_Pnt2d P1( X2, Y2 );
    gp_Vec2d V1( P1, gp_Pnt2d(X1, Y1) );
    gp_Vec2d V2 = V1.Rotated(  alpha );
    gp_Vec2d V3 = V1.Rotated( -alpha );
    V2 /= 2;
    V3 /= 2;
    gp_Pnt2d P2 = P1.Translated( V2 ), 
             P3 = P1.Translated( V3 );

    gp_Pnt2d P4( X6, Y6 );
    gp_Vec2d V4( P4, gp_Pnt2d( X5, Y5 ) );
    gp_Vec2d V5 = V4.Rotated(  alpha );
    gp_Vec2d V6 = V4.Rotated( -alpha );
    V5 /= 2;
    V6 /= 2;
    gp_Pnt2d P5 = P4.Translated( V5 ), 
             P6 = P4.Translated( V6 );


   Standard_ShortReal X3 = Standard_ShortReal( P2.X() ), 
                      Y3 = Standard_ShortReal( P2.Y() ), 
                      X4 = Standard_ShortReal( P3.X() ), 
                      Y4 = Standard_ShortReal( P3.Y() ),
                      X7 = Standard_ShortReal( P5.X() ), 
                      Y7 = Standard_ShortReal( P5.Y() ), 
                      X8 = Standard_ShortReal( P6.X() ), 
                      Y8 = Standard_ShortReal( P6.Y() );


    gp_Pnt2d PP1( X1, Y1 ),
             PP2( X2, Y2 ),
             PP3( X3, Y3 ),
             PP4( X4, Y4 ),
             PP5( X5, Y5 ),
             PP6( X6, Y6 ),
             PP7( X7, Y7 ),
             PP8( X8, Y8 ),
             PPR( myX, myY );
    PP1.Rotate( PPR, myAngle );
    PP2.Rotate( PPR, myAngle );
    PP3.Rotate( PPR, myAngle );
    PP4.Rotate( PPR, myAngle );
    PP5.Rotate( PPR, myAngle );
    PP6.Rotate( PPR, myAngle );
    PP7.Rotate( PPR, myAngle );
    PP8.Rotate( PPR, myAngle );

    X1 = Standard_ShortReal( PP1.X() );
    Y1 = Standard_ShortReal( PP1.Y() );
    X2 = Standard_ShortReal( PP2.X() );
    Y2 = Standard_ShortReal( PP2.Y() );
    X3 = Standard_ShortReal( PP3.X() );
    Y3 = Standard_ShortReal( PP3.Y() );
    X4 = Standard_ShortReal( PP4.X() );
    Y4 = Standard_ShortReal( PP4.Y() );
    X5 = Standard_ShortReal( PP5.X() );
    Y5 = Standard_ShortReal( PP5.Y() );
    X6 = Standard_ShortReal( PP6.X() );
    Y6 = Standard_ShortReal( PP6.Y() );
    X7 = Standard_ShortReal( PP7.X() );
    Y7 = Standard_ShortReal( PP7.Y() );
    X8 = Standard_ShortReal( PP8.X() );
    Y8 = Standard_ShortReal( PP8.Y() );

    
    if ( myGOPtr->IsTransformed () ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
     Standard_Real a1, b1;
     a1 = X1;
	 b1 = Y1;
     aTrsf.Transforms( a1, b1 );
     X1 = Standard_ShortReal( a1 );
	 Y1 = Standard_ShortReal( b1 );
     a1 = X2;
	 b1 = Y2;
     aTrsf.Transforms( a1, b1 );
     X2 = Standard_ShortReal( a1 );
	 Y2 = Standard_ShortReal( b1 );
     a1 = X3;
	 b1 = Y3;
     aTrsf.Transforms( a1, b1 );
     X3 = Standard_ShortReal( a1 );
	 Y3 = Standard_ShortReal( b1 );
     a1 = X4;
	 b1 = Y4;
     aTrsf.Transforms( a1, b1 );
     X4 = Standard_ShortReal( a1 );
	 Y4 = Standard_ShortReal( b1 );
     a1 = X5;
	 b1 = Y5;
     aTrsf.Transforms( a1, b1 );
     X5 = Standard_ShortReal( a1 );
	 Y5 = Standard_ShortReal( b1 );
     a1 = X6;
	 b1 = Y6;
     aTrsf.Transforms( a1, b1 );
     X6 = Standard_ShortReal( a1 );
	 Y6 = Standard_ShortReal( b1 );
     a1 = X7;
	 b1 = Y7;
     aTrsf.Transforms( a1, b1 );
     X7 = Standard_ShortReal( a1 );
	 Y7 = Standard_ShortReal( b1 );
     a1 = X8;
	 b1 = Y8;
     aTrsf.Transforms( a1, b1 );
     X8 = Standard_ShortReal( a1 );
	 Y8 = Standard_ShortReal( b1 );
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    aDrawer->MapSegmentFromTo( X2, Y2, X3, Y3 );
    aDrawer->MapSegmentFromTo( X2, Y2, X4, Y4 );
    aDrawer->MapSegmentFromTo( X4, Y4, X3, Y3 );

    aDrawer->MapSegmentFromTo( X5, Y5, X6, Y6 );
    aDrawer->MapSegmentFromTo( X6, Y6, X7, Y7 );
    aDrawer->MapSegmentFromTo( X6, Y6, X8, Y8 );
    aDrawer->MapSegmentFromTo( X8, Y8, X7, Y7 );

    aDrawer->MapSegmentFromTo( X1, Y1, X5, Y5 );
 }
}

void Prs2d_SymTotal::Save(Aspect_FStream& aFStream) const
{
}
