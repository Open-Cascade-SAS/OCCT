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


