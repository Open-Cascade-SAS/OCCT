#include <Prs2d_AspectText.ixx>
#include <Prs2d_AspectName.hxx>
#include <Aspect_GenericColorMap.hxx> 
#include <Aspect_FontMap.hxx> 

Prs2d_AspectText::Prs2d_AspectText( const Quantity_NameOfColor aColor,
		    	const Standard_CString aFont,
			    const Quantity_Length aSize,
			    const Aspect_TypeOfText aType,
			    const Standard_Boolean isUnderlined )
  : 
    Prs2d_AspectRoot( Prs2d_AN_TEXT ),
    myColor( aColor ),
    myFont( aFont, aSize ),
    myRelativeSlant(0.),
    myType( aType ),
    myIsUnderlined( isUnderlined ),
    myFontIndex( 0 ),
	myColorIndex( 0 )
{ }

Prs2d_AspectText::Prs2d_AspectText( const Quantity_Color& aColor,
		    	const Aspect_FontStyle&  aFont,
  			    const Aspect_TypeOfText aType,
			    const Standard_Boolean isUnderlined)
  : 
    Prs2d_AspectRoot( Prs2d_AN_TEXT ),
    myColor( aColor ),
    myFont( aFont ),
    myRelativeSlant(0.),
    myType( aType ),
    myIsUnderlined( isUnderlined ),
    myFontIndex( 0 ),
	myColorIndex( 0 )
{ }

void Prs2d_AspectText::SetColor( const Quantity_NameOfColor aColor ) {

    SetColor(Quantity_Color(aColor));
}

void Prs2d_AspectText::SetColor( const Quantity_Color& aColor ) {

    if( myColor != aColor ) {
      myColor = aColor;
      myColorIndex = 0;
    }
}

void Prs2d_AspectText::SetFont( const Aspect_TypeOfFont aFont ) {

    if( myFont.Style() != aFont ) {
      myFont = Aspect_FontStyle(aFont,myFont.Size(),
				myFont.Slant(),myFont.CapsHeight());
      myFontIndex = 0;
    }
}

void Prs2d_AspectText::SetFont( const Aspect_FontStyle& aFont ) {

    if( myFont != aFont ) {
      myFont = aFont;
      myFontIndex = 0;
    }
}

void Prs2d_AspectText::SetRelativeSlant( const Quantity_PlaneAngle aSlant ) {

    myRelativeSlant = aSlant; 
}

void Prs2d_AspectText::SetHeight( const Quantity_Length anHeight,
			        const Standard_Boolean isCapsHeight ) {

    if( myFont.Size() != anHeight || myFont.CapsHeight() != isCapsHeight ) {
      myFont = Aspect_FontStyle(myFont.Style(),anHeight,
				myFont.Slant(),isCapsHeight);
      myFontIndex = 0;
    }
}

void Prs2d_AspectText::SetType( const Aspect_TypeOfText aType ) {
    myType = aType;
}

void Prs2d_AspectText::SetUnderlined( const Standard_Boolean anIsUnderline ) {
    myIsUnderlined = anIsUnderline;
}

void Prs2d_AspectText::Values( Quantity_Color& aColor,
		                       Aspect_FontStyle& aFont,
		                       Quantity_PlaneAngle& aSlant,
		                       Aspect_TypeOfText& aType,
		                       Standard_Boolean& isUnderlined) const {
    aColor = myColor;
    aFont = myFont;
    aSlant = myRelativeSlant + myFont.Slant();
    aType = myType;
    isUnderlined = myIsUnderlined;
}

Standard_Integer Prs2d_AspectText :: FontIndex (  ) const {

  return myFontIndex;
}  // end Standard_Integer Prs2d_AspectText :: FontIndex

Standard_Integer Prs2d_AspectText :: ColorIndex (  ) const {

 return myColorIndex;

}  // end Standard_Integer Prs2d_AspectText :: ColorIndex

void Prs2d_AspectText :: SetFontIndex ( const Standard_Integer anInd )  {

   myFontIndex = anInd;

}  // end void Prs2d_AspectText :: SetFontIndex

void Prs2d_AspectText :: SetColorIndex ( const Standard_Integer anInd )  {

  myColorIndex = anInd;

}  // end void Prs2d_AspectText :: SetColorIndex

