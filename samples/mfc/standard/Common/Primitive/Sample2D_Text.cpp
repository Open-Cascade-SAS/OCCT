#include "stdafx.h"

#include "Sample2D_Text.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Text,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Text,AIS_InteractiveObject)

Sample2D_Text::Sample2D_Text   (const TCollection_AsciiString& theText,
                 const gp_Pnt&                  thePosition,
                 const Quantity_PlaneAngle      theAngle,
                 const Quantity_Color           theColor,
                 const Font_FontAspect          theFontAspect,
                 const Standard_CString         theFont,
                 const Standard_Real            theHeight,
                 const Graphic3d_HorizontalTextAlignment theHAlign,
                 const Graphic3d_VerticalTextAlignment theVAlign,
                 const Standard_Boolean         theIsZoomable)
:AIS_InteractiveObject()
{
  myText       =  theText      ; 
  myPosition   =  thePosition  ;
  myAngle      =  theAngle     ;
  myColor      =  theColor     ;
  myFontAspect =  theFontAspect;
  myFont       =  theFont      ;
  myColor      =  theColor     ;
  myHeight     =  theHeight    ;
  myIsZoomable = theIsZoomable ;
  myHAlign     = theHAlign     ;
  myVAlign     = theVAlign     ;
}

void Sample2D_Text::Compute (const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer /*aMode*/)
{
  aPresentation->Clear();

  Handle_Prs3d_TextAspect asp = myDrawer->TextAspect();

  asp->SetFont(myFont);
  asp->SetColor(myColor);
  asp->SetHeight(myHeight);

  asp->SetHorizontalJustification(myHAlign);
  asp->SetVerticalJustification(myVAlign);
  asp->Aspect()->SetTextZoomable(myIsZoomable);
  asp->Aspect()->SetTextAngle(myAngle);
  asp->Aspect()->SetTextFontAspect(myFontAspect);
  asp->SetHeight(myHeight);
  Prs3d_Text::Draw(aPresentation, asp, myText, myPosition);
}
