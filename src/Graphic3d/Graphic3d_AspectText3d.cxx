// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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


//    30/11/98 ; FMN : S3819. Textes always visible.   


//-Version  

//-Design Declaration of variables specific to the context
//    of trace of texts 3d

//-Warning  A context of trace of text is defined by :
//    - the font used
//    - the color
//    - the scale
//    - the space between characters

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectText3d.ixx>

#include <TCollection_AsciiString.hxx>
#include <OSD_Environment.hxx>

#include <Graphic3d_NameOfFont.hxx>

//-Aliases

//-Global data definitions

//  -- la font utilisee
//  MyFont    : NameOfFont;

//  -- la couleur
//  MyColor   : Color;

//  -- l'echelle
//  MyFactor  : Standard_Real;

//  -- l'espace entre caracteres
//  MySpace   : Standard_Real;

//  -- le style
//  MyStyle   : TypeOfStyleText;

//  -- le display type
//  MyDisplayType : TypeOfDisplayText;

//  -- la couleur du sous-titrage et pour le fond en mode decalage.
//  MyColorSubTitle : Color;

//-Constructors

//-Destructors

//-Methods, in order



Graphic3d_AspectText3d::Graphic3d_AspectText3d ():
MyFont (Graphic3d_NOF_ASCII_MONO), MyColor (Quantity_NOC_YELLOW), MyFactor (1.0), MySpace (0.0), MyStyle (Aspect_TOST_NORMAL), MyDisplayType (Aspect_TODT_NORMAL), MyColorSubTitle (Quantity_NOC_WHITE) {
  MyTextZoomable = Standard_False;
  MyTextAngle = 0.0;
  MyTextFontAspect = Font_FA_Regular;
}

Graphic3d_AspectText3d::Graphic3d_AspectText3d (
  const Quantity_Color& AColor, 
  const Standard_CString AFont, 
  const Standard_Real AFactor, 
  const Standard_Real ASpace,
  const Aspect_TypeOfStyleText AStyle,
  const Aspect_TypeOfDisplayText ADisplayType):
MyFont(AFont), MyColor (AColor), MyFactor (AFactor), MySpace (ASpace), MyStyle (AStyle), MyDisplayType(ADisplayType), MyColorSubTitle   (Quantity_NOC_WHITE) {
  MyTextZoomable = Standard_False;
  MyTextAngle = 0.0;
  MyTextFontAspect = Font_FA_Regular;
  if(MyFont.Length() == 0)
    MyFont.AssignCat(Graphic3d_NOF_ASCII_MONO);

  if (AFactor <= 0.0)
    Graphic3d_AspectTextDefinitionError::Raise
    ("Bad value for TextScaleFactor");

}

void Graphic3d_AspectText3d::SetColor (const Quantity_Color& AColor) {

  MyColor = AColor;

}

void Graphic3d_AspectText3d::SetExpansionFactor (const Standard_Real AFactor) {

  if (AFactor <= 0.0)
    Graphic3d_AspectTextDefinitionError::Raise
    ("Bad value for TextScaleFactor");

  MyFactor = AFactor;

}

void Graphic3d_AspectText3d::SetFont (const Standard_CString AFont) {


  TCollection_AsciiString aTemp("");
  if( !strlen(AFont))
    aTemp.AssignCat(Graphic3d_NOF_ASCII_MONO);
  else
    aTemp.AssignCat(AFont);
  MyFont = aTemp;  
}

void Graphic3d_AspectText3d::SetSpace (const Standard_Real ASpace) {

  MySpace = ASpace;

}

void Graphic3d_AspectText3d::SetStyle(const Aspect_TypeOfStyleText AStyle) {

  MyStyle = AStyle;

}

void Graphic3d_AspectText3d::SetDisplayType(const Aspect_TypeOfDisplayText ADisplayType) {

  MyDisplayType = ADisplayType;

}

void Graphic3d_AspectText3d::SetColorSubTitle (const Quantity_Color& AColor) {

  MyColorSubTitle = AColor;


}


void Graphic3d_AspectText3d::SetTextZoomable(const Standard_Boolean AFlag) 
{

  MyTextZoomable = AFlag;

}

Standard_Boolean Graphic3d_AspectText3d::GetTextZoomable() const
{
  return MyTextZoomable;
}

void Graphic3d_AspectText3d::SetTextAngle(const Standard_Real AAngle) 
{
  MyTextAngle = AAngle;
}

Standard_Real Graphic3d_AspectText3d::GetTextAngle() const
{
  return MyTextAngle;
}

void Graphic3d_AspectText3d::SetTextFontAspect(const Font_FontAspect AFontAspect) 
{
  MyTextFontAspect = AFontAspect;
}

Font_FontAspect Graphic3d_AspectText3d::GetTextFontAspect() const
{
  return MyTextFontAspect;
}



void Graphic3d_AspectText3d::Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AFactor, Standard_Real& ASpace) const {

  AColor    = MyColor;
  AFont     = MyFont.ToCString();
  AFactor   = MyFactor;
  ASpace    = MySpace;

}
void Graphic3d_AspectText3d::Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AFactor, Standard_Real& ASpace, Aspect_TypeOfStyleText& AStyle,Aspect_TypeOfDisplayText& ADisplayType,Quantity_Color& AColorSubTitle) const {

  AColor          = MyColor;
  AFont           = MyFont.ToCString();
  AFactor         = MyFactor;
  ASpace          = MySpace;
  AStyle          = MyStyle;
  ADisplayType    = MyDisplayType;
  AColorSubTitle  = MyColorSubTitle;

}
void Graphic3d_AspectText3d::Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AFactor, Standard_Real& ASpace, Aspect_TypeOfStyleText& AStyle,Aspect_TypeOfDisplayText& ADisplayType,Quantity_Color& AColorSubTitle, Standard_Boolean& ATextZoomable,Standard_Real& ATextAngle ) const {

  AColor          = MyColor;
  AFont           = MyFont.ToCString();
  AFactor         = MyFactor;
  ASpace          = MySpace;
  AStyle          = MyStyle;
  ADisplayType    = MyDisplayType;
  AColorSubTitle  = MyColorSubTitle;

  ATextZoomable   = MyTextZoomable;  
  ATextAngle      = MyTextAngle;  

}

void Graphic3d_AspectText3d::Values ( Quantity_Color& AColor, 
                                      Standard_CString& AFont,
                                      Standard_Real& AFactor, 
                                      Standard_Real& ASpace,
                                      Aspect_TypeOfStyleText& AStyle,
                                      Aspect_TypeOfDisplayText& ADisplayType,
                                      Quantity_Color& AColorSubTitle, 
                                      Standard_Boolean& ATextZoomable,
                                      Standard_Real& ATextAngle,
                                      Font_FontAspect& ATextFontAspect ) const 
{

  AColor          = MyColor;
  AFont           = MyFont.ToCString();
  AFactor         = MyFactor;
  ASpace          = MySpace;
  AStyle          = MyStyle;
  ADisplayType    = MyDisplayType;
  AColorSubTitle  = MyColorSubTitle;

  ATextZoomable   = MyTextZoomable;  
  ATextAngle      = MyTextAngle;  
  ATextFontAspect = MyTextFontAspect;

}



