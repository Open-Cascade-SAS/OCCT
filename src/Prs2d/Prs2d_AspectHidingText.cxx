#include <Prs2d_AspectHidingText.ixx>

Prs2d_AspectHidingText::Prs2d_AspectHidingText
                            ( const Quantity_NameOfColor ColorInd, 
				              const Quantity_NameOfColor HidingColorInd, 
                  		  const Quantity_NameOfColor FrameColorInd, 
				  const Aspect_WidthOfLine   FrameWidthInd,
				  const Aspect_TypeOfFont    FontInd,
				  const Standard_ShortReal   aSlantInd,
				  const Standard_ShortReal   aHScaleInd,
				  const Standard_ShortReal   aWScaleInd,
				  const Standard_Boolean     isUnderlined ): 
    Prs2d_AspectRoot( Prs2d_AN_HIDINGTEXT ),
    myFontInd( FontInd ),
    myColorInd( ColorInd ),
    HidingColorInd( HidingColorInd ),
    FrameColorInd( FrameColorInd ),
    FrameWidthInd( FrameWidthInd ),
    mySlantInd( aSlantInd ),
    myHScaleInd( aHScaleInd ),
    myWScaleInd( aWScaleInd ),
    myIsUnderlined( isUnderlined )
{
}

 void Prs2d_AspectHidingText::SetColorOfText(const Quantity_NameOfColor aColorInd) {

	 myColorInd = aColorInd;
 }

 void Prs2d_AspectHidingText::SetFontOfText(const Aspect_TypeOfFont aFontInd) {
    
	 myFontInd = aFontInd;
 }

 void Prs2d_AspectHidingText::SetSlant(const Standard_ShortReal aSlantInd) {

	 mySlantInd = aSlantInd;
 }

 void Prs2d_AspectHidingText::SetHScale(const Standard_ShortReal aHScaleInd) {
 
	 myHScaleInd = aHScaleInd;
 }

 void Prs2d_AspectHidingText::SetWScale(const Standard_ShortReal aWScaleInd) {

	 myWScaleInd = aWScaleInd;
 }

 void Prs2d_AspectHidingText::SetUnderlined(const Standard_Boolean anIsUnderline) {

	 myIsUnderlined = anIsUnderline;
 }

 void Prs2d_AspectHidingText::SetHidingColorOfText(const Quantity_NameOfColor aHidColorInd) {

	 HidingColorInd = aHidColorInd;
}

 void Prs2d_AspectHidingText::SetFrameColorOfText(const Quantity_NameOfColor aFrameColorInd) {

	 FrameColorInd =  aFrameColorInd;
 }

 void Prs2d_AspectHidingText::SetFrameWidthOfText(const Aspect_WidthOfLine aFrameWidthInd) {

	 FrameWidthInd = aFrameWidthInd;
 }

 void Prs2d_AspectHidingText::Values(Quantity_NameOfColor& ColorInd, 
	                                 Quantity_NameOfColor& HidingColorInd,
									 Quantity_NameOfColor& FrameColorInd,
									 Aspect_WidthOfLine&   FrameWidthInd,
									 Aspect_TypeOfFont&    FontInd,
									 Standard_ShortReal&   aSlantInd,
									 Standard_ShortReal&   aHScaleInd,
									 Standard_ShortReal&   aWScaleInd,
									 Standard_Boolean&     isUnderlined) const{
    FontInd        = myFontInd;
    ColorInd       = myColorInd;
    aSlantInd      = mySlantInd;
    aHScaleInd     = myHScaleInd;
    aWScaleInd     = myWScaleInd;
    isUnderlined   = myIsUnderlined;
    HidingColorInd = HidingColorInd;
    FrameColorInd  = FrameColorInd;
    FrameWidthInd  = FrameWidthInd;
 }
