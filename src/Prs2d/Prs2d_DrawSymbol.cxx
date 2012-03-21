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

#include <Prs2d_DrawSymbol.ixx>

Prs2d_DrawSymbol::Prs2d_DrawSymbol( const Handle(Graphic2d_GraphicObject)& aGO,
                                    const Prs2d_TypeOfSymbol aSymbType,
                                    const Standard_Real aX,
                                    const Standard_Real aY,
                                    const Standard_Real aWidth,
                                    const Standard_Real aHeight,
                                    const Standard_Real anAngle ) 
    : Graphic2d_Line( aGO ),
      myTypeSymb( aSymbType ),
	  myX( Standard_ShortReal( aX ) ),
	  myY( Standard_ShortReal( aY ) ),
	  myWidth( Standard_ShortReal( aWidth ) ),
	  myHeight( Standard_ShortReal( aHeight ) ),
	  myAngle( Standard_ShortReal( anAngle ) )

{
      if ( myTypeSymb > Prs2d_TOS_LAST &&  myTypeSymb < Prs2d_TOS_NONE )
           Prs2d_SymbolDefinitionError::Raise
           ( "The symbol must be from Prs2d_TypeOfSymbol enumeration" );
      
	  if ( myWidth <= 0.0 )
		   Prs2d_SymbolDefinitionError::Raise( "The width = 0." );

	  if ( myHeight <= 0.0 )
		   Prs2d_SymbolDefinitionError::Raise( "The height = 0." );

	  myMinX = myX - Standard_ShortReal( myWidth  / 2. ); 
      myMinY = myY - Standard_ShortReal( myHeight / 2. );
	  myMaxX = myX + Standard_ShortReal( myWidth  / 2. ); 
      myMaxY = myY + Standard_ShortReal( myHeight / 2. );

}

void Prs2d_DrawSymbol::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {
    
  Standard_Boolean IsIn = Standard_False;

  if ( ! myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }
  
   
  if ( IsIn ) {

   switch ( myTypeSymb ) {
   case Prs2d_TOS_DIAMETER: {
     Standard_ShortReal theRad = ( myWidth < myHeight) ? myWidth : myHeight;
     gp_Pnt2d theCent( myX, myY );
     gp_Vec2d theVec( theCent, gp_Pnt2d( myX + 13 * theRad/ 10, myY ) );
     theVec.Rotate( M_PI / 4 );
     gp_Pnt2d P1 = theCent.Translated( theVec );
     gp_Pnt2d P2 = theCent.Translated( theVec.Reversed() );     

     P1.Rotate( theCent, myAngle );
     P2.Rotate( theCent, myAngle );

     Standard_ShortReal a = myX, b = myY, 
                        c = Standard_ShortReal( P1.X() ), 
                        d = Standard_ShortReal( P1.Y() ),  
                        e = Standard_ShortReal( P2.X() ), 
                        f = Standard_ShortReal( P2.Y() );
   
     if ( myGOPtr->IsTransformed() ) {
      gp_GTrsf2d aTrsf = myGOPtr->Transform();
      Standard_Real A1, B1;
      A1 = Standard_Real( a ); B1 = Standard_Real( b );
      aTrsf.Transforms( A1, B1 );
      a = Standard_ShortReal( A1 ); b = Standard_ShortReal( B1 );
      A1 = Standard_Real( c ); B1 = Standard_Real( d );
      aTrsf.Transforms( A1, B1 );
      c = Standard_ShortReal( A1 ); d = Standard_ShortReal( B1 );
      A1 = Standard_Real( e ); B1 = Standard_Real( f );
      aTrsf.Transforms( A1, B1 );
      e = Standard_ShortReal( A1 ); f = Standard_ShortReal( B1 );
     }

     DrawLineAttrib( aDrawer );
     aDrawer->MapArcFromTo( a, b, theRad, 0., Standard_ShortReal( 2 * M_PI ) );
     aDrawer->MapSegmentFromTo( c, d, e, f );

    }// end TOS_DIAMETER
    break;
    default:
    break;
   } // end switch
  }
}

Standard_Boolean Prs2d_DrawSymbol::Pick( const Standard_ShortReal /*X*/,
                                         const Standard_ShortReal /*Y*/,
                                         const Standard_ShortReal /*aPrecision*/,
                                         const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {

    return Standard_False;
}

void Prs2d_DrawSymbol::Save(Aspect_FStream& aFStream) const
{
}
