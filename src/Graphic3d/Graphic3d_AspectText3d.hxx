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

#ifndef _Graphic3d_AspectText3d_HeaderFile
#define _Graphic3d_AspectText3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Real.hxx>
#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>
#include <Standard_Boolean.hxx>
#include <Font_FontAspect.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
class Graphic3d_AspectTextDefinitionError;
class Quantity_Color;


class Graphic3d_AspectText3d;
DEFINE_STANDARD_HANDLE(Graphic3d_AspectText3d, MMgt_TShared)

//! Creates and updates a group of attributes for
//! text primitives. This group contains the colour,
//! font, expansion factor (height/width ratio), and
//! inter-character space.
//!
//! NOTE: The font name is stored in the aspect instance
//! so it is safe to pass it as const char* to OpenGl package
//! without copying the string. However, the aspect should not
//! be deleted until the text drawn using this aspect is no longer
//! visible. The best practice is to keep the aspect in the object's drawer.
class Graphic3d_AspectText3d : public MMgt_TShared
{

public:

  
  //! Creates a context table for text primitives
  //! defined with the following default values:
  //!
  //! Colour                    : NOC_YELLOW
  //! Font                      : NOF_ASCII_MONO
  //! Expansion factor          : 1.
  //! Space between characters  : 0.
  //! The style                 : TOST_NORMAL
  //! The display type          : TODT_NORMAL
  Standard_EXPORT Graphic3d_AspectText3d();
  
  //! Creates a context table for text primitives
  //! defined with the specified values.
  //! AFont may be to take means from User(example "Courier New")
  //! or Font name defined in Font_NameOfFont(example Font_NOF_ASCII_MONO)
  //! or use default font("Courier")
  Standard_EXPORT Graphic3d_AspectText3d(const Quantity_Color& AColor, const Standard_CString AFont, const Standard_Real AExpansionFactor, const Standard_Real ASpace, const Aspect_TypeOfStyleText AStyle = Aspect_TOST_NORMAL, const Aspect_TypeOfDisplayText ADisplayType = Aspect_TODT_NORMAL);
  
  //! Modifies the colour of <me>.
  Standard_EXPORT void SetColor (const Quantity_Color& AColor);
  
  //! Modifies the expansion factor (height/width ratio)
  //! If the factor is less than 1, the characters will
  //! be higher than they are wide.
  Standard_EXPORT void SetExpansionFactor (const Standard_Real AFactor);
  
  //! Modifies the font of <me>.
  Standard_EXPORT void SetFont (const Standard_CString AFont);
  
  //! Modifies the space between the characters.
  Standard_EXPORT void SetSpace (const Standard_Real ASpace);
  
  //! Modifies the style of the text.
  //! TOST_NORMAL     Default text. The text is displayed like any other graphic object.
  //! This text can be hidden by another object that is nearest from the
  //! point of view.
  //! TOST_ANNOTATION The text is always visible. The texte is displayed
  //! over the other object according to the priority.
  Standard_EXPORT void SetStyle (const Aspect_TypeOfStyleText AStyle);
  
  //! Define the display type of the text.
  //!
  //! TODT_NORMAL     Default display. Text only.
  //! TODT_SUBTITLE   There is a subtitle under the text.
  //! TODT_DEKALE     The text is displayed with a 3D style.
  //! TODT_BLEND      The text is displayed in XOR.
  //! TODT_DIMENSION  Dimension line under text will be invisible.
  Standard_EXPORT void SetDisplayType (const Aspect_TypeOfDisplayText ADisplayType);
  
  //! Modifies the colour of the subtitle for the TODT_SUBTITLE TextDisplayType
  //! and the colour of backgroubd for the TODT_DEKALE TextDisplayType.
  Standard_EXPORT void SetColorSubTitle (const Quantity_Color& AColor);
  
  //! Turns usage of text zoomable on/off
  Standard_EXPORT void SetTextZoomable (const Standard_Boolean AFlag);
  
  //! Returns TRUE when the Text Zoomable is on.
  Standard_EXPORT Standard_Boolean GetTextZoomable() const;
  
  //! Turns usage of text rotated
  Standard_EXPORT void SetTextAngle (const Standard_Real AAngle);
  
  //! Returns Angle of degree
  Standard_EXPORT Standard_Real GetTextAngle() const;
  
  //! Turns usage of Aspect text
  Standard_EXPORT void SetTextFontAspect (const Font_FontAspect AFontAspect);
  
  //! Returns text FontAspect
  Standard_EXPORT Font_FontAspect GetTextFontAspect() const;
  
  //! Sets up OpenGL/GLSL shader program.
  Standard_EXPORT void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram);
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AnExpansionFactor, Standard_Real& ASpace) const;
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AnExpansionFactor, Standard_Real& ASpace, Aspect_TypeOfStyleText& AStyle, Aspect_TypeOfDisplayText& ADisplayType, Quantity_Color& AColorSubTitle) const;
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AnExpansionFactor, Standard_Real& ASpace, Aspect_TypeOfStyleText& AStyle, Aspect_TypeOfDisplayText& ADisplayType, Quantity_Color& AColorSubTitle, Standard_Boolean& ATextZoomable, Standard_Real& ATextAngle) const;
  
  //! Returns the current values of the group <me>.
  Standard_EXPORT void Values (Quantity_Color& AColor, Standard_CString& AFont, Standard_Real& AnExpansionFactor, Standard_Real& ASpace, Aspect_TypeOfStyleText& AStyle, Aspect_TypeOfDisplayText& ADisplayType, Quantity_Color& AColorSubTitle, Standard_Boolean& ATextZoomable, Standard_Real& ATextAngle, Font_FontAspect& ATextFontAspect) const;
  
  Standard_EXPORT const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const;




  DEFINE_STANDARD_RTTI(Graphic3d_AspectText3d,MMgt_TShared)

protected:




private:


  TCollection_AsciiString MyFont;
  Quantity_Color MyColor;
  Standard_Real MyFactor;
  Standard_Real MySpace;
  Aspect_TypeOfStyleText MyStyle;
  Aspect_TypeOfDisplayText MyDisplayType;
  Quantity_Color MyColorSubTitle;
  Standard_Boolean MyTextZoomable;
  Standard_Real MyTextAngle;
  Font_FontAspect MyTextFontAspect;
  Handle(Graphic3d_ShaderProgram) MyShaderProgram;


};







#endif // _Graphic3d_AspectText3d_HeaderFile
