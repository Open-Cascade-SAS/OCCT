#include <Prs2d_Symmetry.ixx>

Prs2d_Symmetry::Prs2d_Symmetry( const Handle(Graphic2d_GraphicObject)& aGO,
                                const Standard_Real aX,
                                const Standard_Real aY,
                                const Standard_Real aLength,
                                const Standard_Real anAngle ) 

 : Prs2d_Tolerance( aGO, aX, aY, aLength, anAngle )
{
}

void Prs2d_Symmetry::Draw( const Handle(Graphic2d_Drawer)& aDrawer )  {
 
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
    Standard_ShortReal X1 = myX - myLength/2, 
                       Y1 = myY,
                       X2 = myX + myLength/2, 
                       Y2 = myY,
                       X3 = myX - myLength/3, 
                       Y3 = myY - myLength/3,
                       X4 = myX + myLength/3, 
                       Y4 = Y3,
                       X5 = X3,
                       Y5 = myY + myLength/3,
                       X6 = X4,
                       Y6 = Y5;
   
    gp_Pnt2d PP1( X1, Y1 ),
             PP2( X2, Y2 ),
             PP3( X3, Y3 ),
             PP4( X4, Y4 ),
             PP5( X5, Y5 ),
             PP6( X6, Y6 ),
             PPR( myX, myY );
    PP1.Rotate( PPR, myAngle );
    PP2.Rotate( PPR, myAngle );
    PP3.Rotate( PPR, myAngle );
    PP4.Rotate( PPR, myAngle );
    PP5.Rotate( PPR, myAngle );
    PP6.Rotate( PPR, myAngle );

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
    }
    aDrawer->MapSegmentFromTo( X1, Y1, X2, Y2 );
    aDrawer->MapSegmentFromTo( X3, Y3, X4, Y4 );
    aDrawer->MapSegmentFromTo( X5, Y5, X6, Y6 );
  }
  
}

void Prs2d_Symmetry::Save(Aspect_FStream& aFStream) const
{
}
