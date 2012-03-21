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

#include <Prs2d_Dimension.ixx>

Prs2d_Dimension::Prs2d_Dimension( const Handle(Graphic2d_GraphicObject)& aGO,
                                  const TCollection_ExtendedString& aText,
                                  const Standard_Real aTxtScale,
                                  const Standard_Real anArrAngle,
                                  const Standard_Real anArrLength,
                                  const Prs2d_TypeOfArrow anArrType,
                                  const Prs2d_ArrowSide anArrow,
                                  const Standard_Boolean IsRevArrow ) 
   :  Graphic2d_Line( aGO ),
      myText( aText ),
      myTextScale( aTxtScale ),
      myTextFont( 0 ),
      myTextPosH( 0 ),
      myTextPosV( 0 ),
      myTextAngle( 0 ), 
      myAbsX( 0 ),
      myAbsY( 0 ),
      myAbsAngle( 0 ), 
      mySymbFont( 0 ),
      myXVert1( 1, 3 ),
      myYVert1( 1, 3 ),
      myXVert2( 1, 3 ),
      myYVert2( 1, 3 ),
      myArrType( anArrType ),
      myArrow( anArrow ),
      myArrowAng( anArrAngle ),
      myArrowLen( anArrLength ),
      myIsRevArr( IsRevArrow ),
      myIsSymbol( Standard_False ),
      mySymbCode( 248 )

{
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Dimension::ArrayOfFirstArrowPnt( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   for ( Standard_Integer i = 1; i <= 3; i++ ) 
   {
     points->SetValue( i, gp_Pnt2d( myXVert1(i), myYVert1(i) ) );
   }   
   return points;
}

const Handle(TColgp_HArray1OfPnt2d)  Prs2d_Dimension::ArrayOfSecondArrowPnt( ) const
{
   Handle(TColgp_HArray1OfPnt2d) points = new TColgp_HArray1OfPnt2d( 1, 3 );
		
   for ( Standard_Integer i = 1; i <= 3; i++ ) 
   {
     points->SetValue( i, gp_Pnt2d( myXVert2(i), myYVert2(i) ) );
   }   
   return points;
}


