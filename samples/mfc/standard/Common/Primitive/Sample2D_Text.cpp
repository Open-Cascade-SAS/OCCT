#include "stdafx.h"

#include "Sample2D_Text.h"

IMPLEMENT_STANDARD_HANDLE(Sample2D_Text,AIS2D_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Text,AIS2D_InteractiveObject)

Sample2D_Text::Sample2D_Text
                 (const TCollection_AsciiString& aText, 
                  const Standard_Real            anX ,              // = 0
                  const Standard_Real            anY ,              // = 0
			      const Aspect_TypeOfText        aType,             // = SOLID,
			      const Quantity_PlaneAngle      anAngle,           // = 0.0
			      const Standard_Real            aslant,            // = 0.0
			      const Standard_Integer         aColorIndex,       // = 0
			      const Standard_Integer         aFontIndex,        // = 1
			      const Quantity_Factor          aScale,            // = 1
                  const Standard_Boolean         aIsUnderline,      // = Standard_False,
                  const Standard_Boolean         aIsZoomable,       // = Standard_True
                  const Graphic2d_TypeOfAlignment aTypeOfAlignment) // = Graphic2d_TOA_LEFT
:AIS2D_InteractiveObject()
{
  myText       =  aText       ; 
  myX          =  anX         ;
  myY          =  anY         ;
  myTypeOfText =  aType       ;
  myAngle      =  anAngle     ;
  mySlant      =  aslant      ;
  myFontIndex  =  aFontIndex  ;
  myColorIndex =  aColorIndex ;
  myScale      =  aScale      ;
  myWidth      =  0           ;
  myHeight     =  0           ;
  myIsUnderline = aIsUnderline ;
  myIsZoomable  = aIsZoomable ;
  myTypeOfAlignment = aTypeOfAlignment;

}

void Sample2D_Text::SetContext(const Handle(AIS2D_InteractiveContext)& theContext)
{
  AIS2D_InteractiveObject::SetContext(theContext);

  Handle(Graphic2d_Text) text;
  text = new Graphic2d_Text(this, myText, myX, myY, myAngle,myTypeOfText,myScale);
  text->SetFontIndex  (myFontIndex);
  text->SetColorIndex (myColorIndex);
  text->SetSlant      (mySlant);
  text->SetUnderline  (myIsUnderline);
  text->SetZoomable   (myIsZoomable);
  text->SetAlignment  (myTypeOfAlignment);
}

