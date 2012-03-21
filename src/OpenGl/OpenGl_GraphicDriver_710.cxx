// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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
