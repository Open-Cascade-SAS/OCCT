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
