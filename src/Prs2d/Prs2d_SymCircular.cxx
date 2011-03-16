#include <Prs2d_SymCircular.ixx>

Prs2d_SymCircular::Prs2d_SymCircular( const Handle(Graphic2d_GraphicObject)& aGO,
                                      const Standard_Real aX,
                                      const Standard_Real aY,
                                      const Standard_Real aLength,
                                      const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_SymCircular::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
    const Standard_Real alpha = Standard_PI / 12;

    Standard_ShortReal X1 = myX - myLength/2, 
                       Y1 = myY - myLength/2, 
                       X2 = Standard_ShortReal(X1 + myLength * Cos( Standard_PI / 4 ) ), 
                       Y2 = myY + myLength/2;
    
    
    gp_Pnt2d P1( X2, Y2 );
    gp_Vec2d V1( P1, gp_Pnt2d(X1, Y1) );
    gp_Vec2d V2 = V1.Rotated(  alpha );
    gp_Vec2d V3 = V1.Rotated( -alpha );
    V2 /= 2;
    V3 /= 2;
    gp_Pnt2d P2 = P1.Translated( V2 ), 
             P3 = P1.Translated( V3 );

    Standard_ShortReal X3 = Standard_ShortReal( P2.X() ), 
                       Y3 = Standard_ShortReal( P2.Y() ), 
                       X4 = Standard_ShortReal( P3.X() ), 
                       Y4 = Standard_ShortReal( P3.Y() );

    gp_Pnt2d PP1( X1, Y1 ),
             PP2( X2, Y2 ),
             PP3( X3, Y3 ),
             PP4( X4, Y4 ),
             PPR( myX, myY );
    PP1.Rotate( PPR, myAngle );
    PP2.Rotate( PPR, myAngle );
    PP3.Rotate( PPR, myAngle );
    PP4.Rotate( PPR, myAngle );

    X1 = Standard_ShortReal( PP1.X() );
    Y1 = Standard_ShortReal( PP1.Y() );
    X2 = Standard_ShortReal( PP2.X() );
    Y2 = Standard_ShortReal( PP2.Y() );
    X3 = Standard_ShortReal( PP3.X() );
    Y3 = Standard_ShortReal( PP3.Y() );
    X4 = Standard_ShortReal( PP4.X() );
    Y4 = Standard_ShortReal( PP4.Y() );

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
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    aDrawer->MapSegmentFromTo( X2, Y2, X3, Y3 );
    aDrawer->MapSegmentFromTo( X2, Y2, X4, Y4 );
    aDrawer->MapSegmentFromTo( X4, Y4, X3, Y3 );
  }
  
}

void Prs2d_SymCircular::Save(Aspect_FStream& aFStream) const
{
}
