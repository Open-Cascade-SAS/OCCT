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

#include <Prs2d_Position.ixx>

Prs2d_Position::Prs2d_Position( const Handle(Graphic2d_GraphicObject)& aGO,
                                const Standard_Real aX,
                                const Standard_Real aY,
                                const Standard_Real aLength,
                                const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_Position::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
    Standard_ShortReal X1 = myX,
                       Y1 = myY - 2 * myLength/3, 
                       X2 = myX,
                       Y2 = myY + 2 * myLength/3, 
                       X3 = myX - 2 * myLength/3, 
                       Y3 = myY,
                       X4 = myX + 2 * myLength/3,
                       Y4 = myY,
                       Xc = myX, 
                       Yc = myY;

    gp_Pnt2d P1( X1, Y1 ),
             P2( X2, Y2 ),
             P3( X3, Y3 ),
             P4( X4, Y4 ),
             PC( Xc, Yc ),
             PR( myX, myY );
    P1.Rotate( PR, myAngle );
    P2.Rotate( PR, myAngle );
    P3.Rotate( PR, myAngle );
    P4.Rotate( PR, myAngle );
    PC.Rotate( PR, myAngle );

    X1 = Standard_ShortReal( P1.X() );
    Y1 = Standard_ShortReal( P1.Y() );
    X2 = Standard_ShortReal( P2.X() );
    Y2 = Standard_ShortReal( P2.Y() );
    X3 = Standard_ShortReal( P3.X() );
    Y3 = Standard_ShortReal( P3.Y() );
    X4 = Standard_ShortReal( P4.X() );
    Y4 = Standard_ShortReal( P4.Y() );
    Xc = Standard_ShortReal( PC.X() );
    Yc = Standard_ShortReal( PC.Y() );

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
     a1 = Xc;
	 b1 = Yc;
     aTrsf.Transforms( a1, b1 );
     Xc = Standard_ShortReal( a1 );
	 Yc = Standard_ShortReal( b1 );
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    aDrawer->MapSegmentFromTo( X4, Y4, X3, Y3 );
    aDrawer->MapArcFromTo( Xc, Yc, myLength/2, 0.0, Standard_ShortReal( 2. * M_PI ) );
  }
}

void Prs2d_Position::Save(Aspect_FStream& aFStream) const
{
}
