#include <Prs2d_SurfProfile.ixx>

Prs2d_SurfProfile::Prs2d_SurfProfile( const Handle(Graphic2d_GraphicObject)& aGO,
                                      const Standard_Real aX,
                                      const Standard_Real aY,
                                      const Standard_Real aLength,
                                      const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_SurfProfile::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
                       Y1 = Standard_ShortReal( myY - myLength/4 ),
                       X2 = Standard_ShortReal( myX + myLength/2 ),
                       Y2 = Standard_ShortReal( myY - myLength/4 ),
                       Xc = Standard_ShortReal( myX ),
                       Yc = Y2;
    gp_Pnt2d P1( X1, Y1 ),
             P2( X2, Y2 ),
             PC( Xc, Yc ),
             PR( myX, myY );
    P1.Rotate( PR, myAngle );
    P2.Rotate( PR, myAngle );
    PC.Rotate( PR, myAngle );

    X1 = Standard_ShortReal( P1.X() );
    Y1 = Standard_ShortReal( P1.Y() );
    X2 = Standard_ShortReal( P2.X() );
    Y2 = Standard_ShortReal( P2.Y() );
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
     a1 = Xc;
	 b1 = Yc;
     aTrsf.Transforms( a1, b1 );
     Xc = Standard_ShortReal( a1 );
	 Yc = Standard_ShortReal( b1 );
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    aDrawer->MapArcFromTo( Xc, Yc, myLength/2, myAngle, Standard_ShortReal( Standard_PI + myAngle ) );
  }
}

void Prs2d_SurfProfile::Save(Aspect_FStream& aFStream) const
{
}
