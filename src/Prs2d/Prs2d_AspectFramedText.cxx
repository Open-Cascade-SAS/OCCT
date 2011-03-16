#include <Prs2d_AspectFramedText.ixx>

Prs2d_AspectFramedText::Prs2d_AspectFramedText
 ( const Quantity_NameOfColor ColorInd,
   const Quantity_NameOfColor FrameColorInd,
   const Aspect_WidthOfLine   FrameWidthInd,
   const Aspect_TypeOfFont    FontInd,
   const Standard_ShortReal   aSlant,
   const Standard_ShortReal   aHScale,
   const Standard_ShortReal   aWScale,
   const Standard_Boolean     isUnderlined ):

   Prs2d_AspectRoot( Prs2d_AN_FRAMEDTEXT ),
   myColor( ColorInd ), 
   myFrameColor( FrameColorInd ), 
   myFont( FontInd ),
   myFrameWidth( FrameWidthInd ),
   mySlant( aSlant ),
   myHScale( aHScale ),
   myWScale( aWScale ),
   myIsUnderlined( isUnderlined ) {

 }

 void Prs2d_AspectFramedText::SetColorOfText(const Quantity_NameOfColor aColor) {
  
	 myColor = aColor;
 
 }

 void Prs2d_AspectFramedText::SetFontOfText(const Aspect_TypeOfFont aFont) {
     
	 myFont = aFont;
 
 }

 void Prs2d_AspectFramedText::SetSlant(const Standard_ShortReal aSlant) {

     mySlant = aSlant;
 }

 void Prs2d_AspectFramedText::SetHScale(const Standard_ShortReal aHScale) {
    
     myHScale = aHScale;
 }

 void Prs2d_AspectFramedText::SetWScale(const Standard_ShortReal aWScale) {

     myWScale = aWScale;
 }

 void Prs2d_AspectFramedText::SetUnderlined(const Standard_Boolean anIsUnderlined) {
     
    myIsUnderlined = anIsUnderlined; 
 }

 void Prs2d_AspectFramedText::SetFrameColor(const Quantity_NameOfColor aFrameColor) {

   myFrameColor = aFrameColor;
 }

 void Prs2d_AspectFramedText::SetFrameWidth(const Aspect_WidthOfLine aFrameWidth) {

   myFrameWidth = aFrameWidth;
 
 }

 void Prs2d_AspectFramedText::Values(Quantity_NameOfColor& aColorInd, 
	                                 Quantity_NameOfColor& aFrameColorInd, 
	                                 Aspect_WidthOfLine& aFrameWidthInd, 
									 Aspect_TypeOfFont& aFontInd, 
	                                 Standard_ShortReal& aSlant, 
									 Standard_ShortReal& aHScale, 
	                                 Standard_ShortReal& aWScale, 
									 Standard_Boolean& isUnderlined) const {
    
    aFontInd       = myFont;
    aColorInd      = myColor;
    aSlant         = mySlant;
    aHScale        = myHScale;
    aWScale        = myWScale;
    isUnderlined   = myIsUnderlined;
    aFrameWidthInd = myFrameWidth;
    aFrameColorInd = myFrameColor;
 
 
 }
