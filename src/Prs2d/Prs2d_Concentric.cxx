#include <Prs2d_Concentric.ixx>

Prs2d_Concentric::Prs2d_Concentric( const Handle(Graphic2d_GraphicObject)& aGO,
                                    const Standard_Real aX,
                                    const Standard_Real aY,
                                    const Standard_Real aLength,
                                    const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_Concentric::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
    Standard_ShortReal X1 = Standard_ShortReal( myX ), 
                       Y1 = Standard_ShortReal( myY );
    gp_Pnt2d P1( X1, Y1 ),
             PR( myX, myY );
    P1.Rotate( PR, myAngle );
    X1 = Standard_ShortReal( P1.X() );
    Y1 = Standard_ShortReal( P1.Y() );
                       
    if ( myGOPtr->IsTransformed () ) {
     gp_GTrsf2d aTrsf = myGOPtr->Transform ();
     Standard_Real a1, b1;
     a1 = X1;
	 b1 = Y1;
     aTrsf.Transforms( a1, b1 );
     X1 = Standard_ShortReal( a1 );
	 Y1 = Standard_ShortReal( b1 );
    }
    aDrawer->MapArcFromTo( X1, Y1, myLength/2, 0.0, Standard_ShortReal( 2. * Standard_PI ) );
    aDrawer->MapArcFromTo( X1, Y1, myLength/3, 0.0, Standard_ShortReal( 2. * Standard_PI ) );
 }

}

void Prs2d_Concentric::Save(Aspect_FStream& aFStream) const
{
}
