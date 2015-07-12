// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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

#include <Font_NameOfFont.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_AspectTextDefinitionError.hxx>
#include <OSD_Environment.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

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
MyFont (Font_NOF_ASCII_MONO), MyColor (Quantity_NOC_YELLOW), MyFactor (1.0), MySpace (0.0), MyStyle (Aspect_TOST_NORMAL), MyDisplayType (Aspect_TODT_NORMAL), MyColorSubTitle (Quantity_NOC_WHITE) {
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
    MyFont.AssignCat(Font_NOF_ASCII_MONO);

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
    aTemp.AssignCat(Font_NOF_ASCII_MONO);
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

void Graphic3d_AspectText3d::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram)
{
  MyShaderProgram = theProgram;
}

const Handle(Graphic3d_ShaderProgram)& Graphic3d_AspectText3d::ShaderProgram() const
{
  return MyShaderProgram;
}
