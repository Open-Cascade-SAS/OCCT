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

#include <Aspect_TypeOfStyleText.hxx>
#include <Aspect_TypeOfDisplayText.hxx>
#include <Graphic3d_AspectTextDefinitionError.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Font_FontAspect.hxx>
#include <Font_NameOfFont.hxx>
#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Creates and updates a group of attributes for
//! text primitives. This group contains the color,
//! font, expansion factor (height/width ratio), and
//! inter-character space.
//!
//! NOTE: The font name is stored in the aspect instance
//! so it is safe to pass it as const char* to OpenGl package
//! without copying the string. However, the aspect should not
//! be deleted until the text drawn using this aspect is no longer
//! visible. The best practice is to keep the aspect in the object's drawer.
class Graphic3d_AspectText3d : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectText3d, Standard_Transient)
public:

  //! Creates a context table for text primitives
  //! defined with the following default values:
  //!
  //! Color                     : NOC_YELLOW
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
  Standard_EXPORT Graphic3d_AspectText3d (const Quantity_Color&  theColor,
                                          const Standard_CString theFont,
                                          const Standard_Real    theExpansionFactor,
                                          const Standard_Real    theSpace,
                                          const Aspect_TypeOfStyleText   theStyle = Aspect_TOST_NORMAL,
                                          const Aspect_TypeOfDisplayText theDisplayType = Aspect_TODT_NORMAL);

  //! Return the text color.
  const Quantity_Color& Color() const { return myColor.GetRGB(); }

  //! Return the text color.
  const Quantity_ColorRGBA& ColorRGBA() const { return myColor; }

  //! Modifies the color.
  void SetColor (const Quantity_Color& theColor) { myColor.SetRGB (theColor); }

  //! Modifies the color.
  void SetColor (const Quantity_ColorRGBA& theColor) { myColor = theColor; }

  //! Modifies the expansion factor (height/width ratio)
  //! If the factor is less than 1, the characters will
  //! be higher than they are wide.
  void SetExpansionFactor (const Standard_Real theFactor)
  {
    if (theFactor <= 0.0)
    {
      throw Graphic3d_AspectTextDefinitionError("Bad value for TextScaleFactor");
    }
    myFactor = theFactor;
  }

  //! Return the font.
  const TCollection_AsciiString& Font() const { return myFont; }

  //! Modifies the font.
  void SetFont (const TCollection_AsciiString& theFont)
  {
    myFont = !theFont.IsEmpty() ? theFont : TCollection_AsciiString (Font_NOF_ASCII_MONO);
  }

  //! Modifies the font.
  void SetFont (const Standard_CString theFont)
  {
    SetFont (TCollection_AsciiString (theFont));
  }

  //! Return the space between characters.
  Standard_Real Space() const { return mySpace; }

  //! Modifies the space between the characters.
  void SetSpace (const Standard_Real theSpace) { mySpace = theSpace; }

  //! Return the text style.
  Aspect_TypeOfStyleText Style() const { return myStyle; }

  //! Modifies the style of the text.
  //! * TOST_NORMAL
  //!   Default text. The text is displayed like any other graphic object.
  //!   This text can be hidden by another object that is nearest from the point of view.
  //! * TOST_ANNOTATION
  //!   The text is always visible.
  //!   The text is displayed over the other object according to the priority.
  void SetStyle (const Aspect_TypeOfStyleText theStyle) { myStyle = theStyle; }

  //! Return display type.
  Aspect_TypeOfDisplayText DisplayType() const { return myDisplayType; }

  //! Define the display type of the text.
  //!
  //! TODT_NORMAL     Default display. Text only.
  //! TODT_SUBTITLE   There is a subtitle under the text.
  //! TODT_DEKALE     The text is displayed with a 3D style.
  //! TODT_BLEND      The text is displayed in XOR.
  //! TODT_DIMENSION  Dimension line under text will be invisible.
  void SetDisplayType (const Aspect_TypeOfDisplayText theDisplayType) { myDisplayType = theDisplayType; }

  //! Return subtitle color.
  const Quantity_ColorRGBA& ColorSubTitleRGBA() const { return myColorSubTitle; }

  //! Return subtitle color.
  const Quantity_Color& ColorSubTitle() const { return myColorSubTitle.GetRGB(); }

  //! Modifies the color of the subtitle for the TODT_SUBTITLE TextDisplayType
  //! and the color of background for the TODT_DEKALE TextDisplayType.
  void SetColorSubTitle (const Quantity_Color& theColor) { myColorSubTitle.SetRGB (theColor); }

  //! Modifies the color of the subtitle for the TODT_SUBTITLE TextDisplayType
  //! and the color of background for the TODT_DEKALE TextDisplayType.
  void SetColorSubTitle (const Quantity_ColorRGBA& theColor) { myColorSubTitle = theColor; }

  //! Returns TRUE when the Text Zoomable is on.
  bool GetTextZoomable() const { return myTextZoomable; }

  //! Turns usage of text zoomable on/off
  void SetTextZoomable (const bool theFlag) { myTextZoomable = theFlag; }

  //! Returns Angle of degree
  Standard_Real GetTextAngle() const { return myTextAngle; }

  //! Turns usage of text rotated
  void SetTextAngle (const Standard_Real theAngle) { myTextAngle = theAngle; }

  //! Returns text FontAspect
  Font_FontAspect GetTextFontAspect() const { return myTextFontAspect; }

  //! Turns usage of Aspect text
  void SetTextFontAspect (const Font_FontAspect theFontAspect) { myTextFontAspect = theFontAspect; }

  //! Return the shader program.
  const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const { return myProgram; }

  //! Sets up OpenGL/GLSL shader program.
  void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram) { myProgram = theProgram; }

public:

  //! Returns the current values of the group.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Quantity_Color&   theColor,
               Standard_CString& theFont,
               Standard_Real&    theExpansionFactor,
               Standard_Real&    theSpace) const
  {
    theColor           = myColor.GetRGB();
    theFont            = myFont.ToCString();
    theExpansionFactor = myFactor;
    theSpace           = mySpace;
  }

  //! Returns the current values of the group.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Quantity_Color&   theColor,
               Standard_CString& theFont,
               Standard_Real&    theExpansionFactor,
               Standard_Real&    theSpace,
               Aspect_TypeOfStyleText&   theStyle,
               Aspect_TypeOfDisplayText& theDisplayType,
               Quantity_Color&           theColorSubTitle) const
  {
    theColor          = myColor.GetRGB();
    theFont           = myFont.ToCString();
    theExpansionFactor= myFactor;
    theSpace          = mySpace;
    theStyle          = myStyle;
    theDisplayType    = myDisplayType;
    theColorSubTitle  = myColorSubTitle.GetRGB();
  }

  //! Returns the current values of the group.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Quantity_Color&   theColor,
               Standard_CString& theFont,
               Standard_Real&    theExpansionFactor,
               Standard_Real&    theSpace,
               Aspect_TypeOfStyleText&   theStyle,
               Aspect_TypeOfDisplayText& theDisplayType,
               Quantity_Color&   theColorSubTitle,
               Standard_Boolean& theTextZoomable,
               Standard_Real&    theTextAngle) const
  {
    theColor          = myColor.GetRGB();
    theFont           = myFont.ToCString();
    theExpansionFactor= myFactor;
    theSpace          = mySpace;
    theStyle          = myStyle;
    theDisplayType    = myDisplayType;
    theColorSubTitle  = myColorSubTitle.GetRGB();

    theTextZoomable   = myTextZoomable;
    theTextAngle      = myTextAngle;
  }

  //! Returns the current values of the group.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Quantity_Color&   theColor,
               Standard_CString& theFont,
               Standard_Real&    theExpansionFactor,
               Standard_Real&    theSpace,
               Aspect_TypeOfStyleText&   theStyle,
               Aspect_TypeOfDisplayText& theDisplayType,
               Quantity_Color&   theColorSubTitle,
               Standard_Boolean& theTextZoomable,
               Standard_Real&    theTextAngle,
               Font_FontAspect&  theTextFontAspect) const
  {
    theColor          = myColor.GetRGB();
    theFont           = myFont.ToCString();
    theExpansionFactor= myFactor;
    theSpace          = mySpace;
    theStyle          = myStyle;
    theDisplayType    = myDisplayType;
    theColorSubTitle  = myColorSubTitle.GetRGB();

    theTextZoomable   = myTextZoomable;
    theTextAngle      = myTextAngle;
    theTextFontAspect = myTextFontAspect;
  }

protected:

  TCollection_AsciiString  myFont;
  Quantity_ColorRGBA       myColor;
  Standard_Real            myFactor;
  Standard_Real            mySpace;
  Aspect_TypeOfStyleText   myStyle;
  Aspect_TypeOfDisplayText myDisplayType;
  Quantity_ColorRGBA       myColorSubTitle;
  bool                     myTextZoomable;
  Standard_Real            myTextAngle;
  Font_FontAspect          myTextFontAspect;
  Handle(Graphic3d_ShaderProgram) myProgram;

};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectText3d, Standard_Transient)

#endif // _Graphic3d_AspectText3d_HeaderFile
