// File:      OpenGl_GraphicDriver_710.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <TCollection_AsciiString.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_Text.hxx>

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const Standard_CString AText,
 const Graphic3d_Vertex& APoint,
 const Standard_Real AHeight,
 const Quantity_PlaneAngle AAngle,
 const Graphic3d_TextPath ATp,
 const Graphic3d_HorizontalTextAlignment AHta,
 const Graphic3d_VerticalTextAlignment AVta,
 const Standard_Boolean EvalMinMax
 ) 
{
  TCollection_ExtendedString TheText(AText);
  OpenGl_GraphicDriver::Text(ACGroup,TheText,APoint,AHeight,AAngle,ATp,AHta,AVta,EvalMinMax);
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const Standard_CString AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Standard_Boolean EvalMinMax
 ) 
{
  TCollection_ExtendedString TheText(AText);
  OpenGl_GraphicDriver::Text(ACGroup,TheText,APoint,AHeight,EvalMinMax);
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const TCollection_ExtendedString& AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Quantity_PlaneAngle AAngle, 
 const Graphic3d_TextPath ATp, 
 const Graphic3d_HorizontalTextAlignment AHta, 
 const Graphic3d_VerticalTextAlignment AVta, 
 const Standard_Boolean 
 ) 
{
  if (ACGroup.ptrGroup)
  {
    Standard_Real theHeight = AHeight;
    if (theHeight < 0.)
      theHeight = DefaultTextHeight();

    OpenGl_Text *atext = new OpenGl_Text( AText, APoint, theHeight, AHta, AVta );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelText, atext );
  }
}

void OpenGl_GraphicDriver::Text 
(
 const Graphic3d_CGroup& ACGroup,
 const TCollection_ExtendedString& AText, 
 const Graphic3d_Vertex& APoint, 
 const Standard_Real AHeight, 
 const Standard_Boolean 
 )
{
  if (ACGroup.ptrGroup)
  {
    Standard_Real theHeight = AHeight;
    if (theHeight < 0.)
      theHeight = DefaultTextHeight();

    OpenGl_Text *atext = new OpenGl_Text( AText, APoint, theHeight, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelText, atext );
  }
}
