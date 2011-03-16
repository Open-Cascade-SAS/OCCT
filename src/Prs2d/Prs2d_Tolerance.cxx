#include <Prs2d_Tolerance.ixx>

Prs2d_Tolerance::Prs2d_Tolerance( const Handle(Graphic2d_GraphicObject)& aGO,
                                  const Standard_Real aX,
                                  const Standard_Real aY,
                                  const Standard_Real aLength,
                                  const Standard_Real anAngle ) 
  : Graphic2d_Line( aGO ),
  myX( Standard_ShortReal( aX ) ),
  myY( Standard_ShortReal( aY ) ),
  myLength( Standard_ShortReal( aLength ) ),
  myAngle( Standard_ShortReal( anAngle ) )
{
     gp_Pnt2d P1( myX - myLength/2, myY - myLength/2);
     gp_Pnt2d P2( myX + myLength/2, myY + myLength/2);
     gp_Pnt2d PR( myX, myY );
     P1.Rotate( PR, myAngle );
     P2.Rotate( PR, myAngle );
    
     myMinX = Standard_ShortReal( P1.X() );
     myMinY = Standard_ShortReal( P1.Y() );
     myMaxX = Standard_ShortReal( P2.X() );
     myMaxY = Standard_ShortReal( P2.Y() );

}

void Prs2d_Tolerance::SetCoord( const Standard_Real aX, 
                                const Standard_Real aY ) {
    
    myX = Standard_ShortReal( aX );
    myY = Standard_ShortReal( aY );
}

void Prs2d_Tolerance::SetSize( const Standard_Real aLen ) {

   myLength = Standard_ShortReal( aLen ); 
    
}

Standard_Boolean Prs2d_Tolerance::Pick( const Standard_ShortReal X,
                                        const Standard_ShortReal Y,
                                        const Standard_ShortReal aPrecision,
                                        const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {
    if ( IsInMinMax( X, Y, aPrecision ) ) 
        return Standard_True;
    return Standard_False;
}
