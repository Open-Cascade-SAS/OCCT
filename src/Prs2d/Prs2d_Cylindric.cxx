#include <Prs2d_Cylindric.ixx>

Prs2d_Cylindric::Prs2d_Cylindric( const Handle(Graphic2d_GraphicObject)& aGO,
                                  const Standard_Real aX,
                                  const Standard_Real aY,
                                  const Standard_Real aLength,
                                  const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_Cylindric::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {

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

    Standard_ShortReal X1 = Standard_ShortReal( myX - myLength/2 ), 
                       Y1 = Standard_ShortReal( myY - myLength/2 ), 
                       X4 = Standard_ShortReal( myX + myLength/2 ),
                       Y4 = Y1,
                       X2 = Standard_ShortReal( X1 + myLength/2 ), 
                       Y2 = Standard_ShortReal( myY + myLength/2 ),

                       X3 = Standard_ShortReal( X4 + myLength/2 ),
                       Y3 = Y2;
    gp_Pnt2d thePnt1( X1, Y1);
    gp_Vec2d theVec( thePnt1, gp_Pnt2d( X3, Y3 ) );
    theVec /= 2;
    thePnt1.Translate( theVec );
    Standard_ShortReal theRad = Standard_ShortReal( myLength/2*Sin(Standard_PI / 3) );
    Standard_ShortReal Xc = Standard_ShortReal(thePnt1.X()), 
                       Yc = Standard_ShortReal(thePnt1.Y());

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
    aDrawer->MapArcFromTo( Xc, Yc, theRad, 0.0, Standard_ShortReal( 2. * Standard_PI ) );
  }
}

void Prs2d_Cylindric::Save(Aspect_FStream& aFStream) const
{
}
