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
