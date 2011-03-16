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


