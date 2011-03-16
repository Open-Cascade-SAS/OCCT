#include <Prs2d_Straightness.ixx>

Prs2d_Straightness::Prs2d_Straightness( const Handle(Graphic2d_GraphicObject)& aGO,
                                        const Standard_Real aX,
                                        const Standard_Real aY,
                                        const Standard_Real aLength,
                                        const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_Straightness::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
                       Y1 = Standard_ShortReal( myY ),
                       X2 = Standard_ShortReal( myX + myLength/2 ),
                       Y2 = Standard_ShortReal( myY );
                
    gp_Pnt2d P1( X1, Y1 ),
             P2( X2, Y2 ),
             PR( myX, myY );
    P1.Rotate( PR, myAngle );
    P2.Rotate( PR, myAngle );

    X1 = Standard_ShortReal( P1.X() );
    Y1 = Standard_ShortReal( P1.Y() );
    X2 = Standard_ShortReal( P2.X() );
    Y2 = Standard_ShortReal( P2.Y() );

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
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );   
  }
}

void Prs2d_Straightness::Save(Aspect_FStream& aFStream) const
{
}
