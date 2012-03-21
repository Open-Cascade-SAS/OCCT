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

#include <Prs2d_AspectHidingPoly.ixx>

Prs2d_AspectHidingPoly::Prs2d_AspectHidingPoly
                          ( const Quantity_NameOfColor HidingColorInd,
  						    const Quantity_NameOfColor FrameColorInd,
       					    const Aspect_TypeOfLine FrameTypeInd,
						    const Aspect_WidthOfLine FrameWidthInd ): 
    
    Prs2d_AspectRoot( Prs2d_AN_HIDINGPOLY ),
    myHidingColorInd( HidingColorInd ),
    myFrameColorInd( FrameColorInd ),
    myFrameTypeInd( FrameTypeInd ),
    myFrameWidthInd( FrameWidthInd ) {
}

 void Prs2d_AspectHidingPoly::SetHidingColor( const Quantity_NameOfColor aColorInd ) {

	 myHidingColorInd = aColorInd;
 }

 void Prs2d_AspectHidingPoly::SetFrameColor( const Quantity_NameOfColor aFrameColorInd ) {

     myFrameColorInd = aFrameColorInd;
 }

 void Prs2d_AspectHidingPoly::SetFrameType( const Aspect_TypeOfLine aFrameTypeInd ) {

     myFrameTypeInd = aFrameTypeInd;
 }

 void Prs2d_AspectHidingPoly::SetFrameWidth( const Aspect_WidthOfLine aFrameWidthInd ) {

     myFrameWidthInd = aFrameWidthInd;
 }

 void Prs2d_AspectHidingPoly::Values( Quantity_NameOfColor& HidingColorInd, 
	                                  Quantity_NameOfColor& FrameColorInd, 
									  Aspect_TypeOfLine & FrameTypeInd, 
									  Aspect_WidthOfLine & FrameWidthInd ) const {
    HidingColorInd = myHidingColorInd; 
    FrameColorInd  = myFrameColorInd;
    FrameTypeInd   = myFrameTypeInd;
    FrameWidthInd  = myFrameWidthInd;
 }


