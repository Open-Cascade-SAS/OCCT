#include <Prs2d_LineProfile.ixx>

Prs2d_LineProfile::Prs2d_LineProfile( const Handle(Graphic2d_GraphicObject)& aGO,
                                      const Standard_Real aX,
                                      const Standard_Real aY,
                                      const Standard_Real aLength,
                                      const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO,aX, aY, aLength, anAngle )
{
}

void Prs2d_LineProfile::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {

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
                       Y1 = myY - myLength/4;
   
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
    
   aDrawer->MapArcFromTo( X1, Y1, myLength/2, myAngle, Standard_ShortReal( Standard_PI + myAngle ) );
  }
}

void Prs2d_LineProfile::Save(Aspect_FStream& aFStream) const
{
}
