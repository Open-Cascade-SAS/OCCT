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

#include <Graphic3d_Aspects.hxx>

//! Creates and updates a group of attributes for text primitives.
class Graphic3d_AspectText3d : public Graphic3d_Aspects
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectText3d, Graphic3d_Aspects)
public:
  //! Creates a context table for text primitives defined with the following default values:
  //! Color            : Quantity_NOC_YELLOW
  //! Font             : Font_NOF_ASCII_MONO
  //! The style        : Aspect_TOST_NORMAL
  //! The display type : Aspect_TODT_NORMAL
  Standard_EXPORT Graphic3d_AspectText3d();

  //! Creates a context table for text primitives defined with the specified values.
  //! @param[in] theColor  text color
  //! @param[in] theFont   font family name or alias like Font_NOF_ASCII_MONO
  //! @param[in] theExpansionFactor  deprecated parameter, has no effect
  //! @param[in] theSpace  deprecated parameter, has no effect
  //! @param[in] theStyle  font style
  //! @param[in] theDisplayType  display mode
  Standard_EXPORT Graphic3d_AspectText3d(
    const Quantity_Color&    theColor,
    Standard_CString         theFont,
    Standard_Real            theExpansionFactor,
    Standard_Real            theSpace,
    Aspect_TypeOfStyleText   theStyle       = Aspect_TOST_NORMAL,
    Aspect_TypeOfDisplayText theDisplayType = Aspect_TODT_NORMAL);

  //! Return the text color.
  const Quantity_Color& Color() const { return myInteriorColor.GetRGB(); }

  //! Return the text color.
  const Quantity_ColorRGBA& ColorRGBA() const { return myInteriorColor; }

  //! Modifies the color.
  void SetColor(const Quantity_Color& theColor) { myInteriorColor.SetRGB(theColor); }

  //! Modifies the color.
  void SetColor(const Quantity_ColorRGBA& theColor) { myInteriorColor = theColor; }

  //! Return the font.
  const TCollection_AsciiString& Font() const
  {
    if (myTextFont.IsNull())
    {
      static const TCollection_AsciiString anEmpty;
      return anEmpty;
    }
    return myTextFont->String();
  }

  //! Modifies the font.
  void SetFont(const TCollection_AsciiString& theFont)
  {
    if (!theFont.IsEmpty())
    {
      myTextFont = new TCollection_HAsciiString(theFont);
    }
    else
    {
      myTextFont.Nullify();
    }
  }

  //! Modifies the font.
  void SetFont(const Standard_CString theFont) { SetFont(TCollection_AsciiString(theFont)); }

  //! Return the text style.
  Aspect_TypeOfStyleText Style() const { return myTextStyle; }

  //! Modifies the style of the text.
  void SetStyle(Aspect_TypeOfStyleText theStyle) { myTextStyle = theStyle; }

  //! Return display type.
  Aspect_TypeOfDisplayText DisplayType() const { return myTextDisplayType; }

  //! Define the display type of the text.
  void SetDisplayType(Aspect_TypeOfDisplayText theDisplayType)
  {
    myTextDisplayType = theDisplayType;
  }

  //! Returns TRUE when the Text Zoomable is on.
  bool GetTextZoomable() const { return myIsTextZoomable; }

  //! Returns Angle of degree
  Standard_ShortReal GetTextAngle() const { return myTextAngle; }

  //! Turns usage of text rotated
  void SetTextAngle(const Standard_Real theAngle) { myTextAngle = (Standard_ShortReal)theAngle; }

  //! Returns text FontAspect
  Font_FontAspect GetTextFontAspect() const { return myTextFontAspect; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;
};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectText3d, Graphic3d_Aspects)

#endif // _Graphic3d_AspectText3d_HeaderFile
