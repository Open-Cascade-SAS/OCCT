// Created on: 2008-01-20
// Created by: Alexander A. BORODIN
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Font_SystemFont_HeaderFile
#define _Font_SystemFont_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Font_FontAspect.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

//! This class stores information about the font, which is merely a file path and cached metadata about the font.
class Font_SystemFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Font_SystemFont, Standard_Transient)
public:

  //! Creates an empty font object.
  Standard_EXPORT Font_SystemFont();

  //! Creates a new font object.
  Standard_EXPORT Font_SystemFont (const Handle(TCollection_HAsciiString)& theFontName,
                                   const Font_FontAspect theFontAspect,
                                   const Handle(TCollection_HAsciiString)& theFilePath);

  //! Creates a font object and initialize class fields with values taken from XLFD (X Logical Font Description)
  Standard_EXPORT Font_SystemFont (const Handle(TCollection_HAsciiString)& theXLFD,
                                   const Handle(TCollection_HAsciiString)& theFilePath);

  //! Returns font family name.
  const Handle(TCollection_HAsciiString)& FontName() const { return myFontName; }
  
  //! Returns font file path.
  const Handle(TCollection_HAsciiString)& FontPath() const { return myFilePath; }
  
  //! Returns font aspect.
  Font_FontAspect FontAspect() const { return myFontAspect; }
  
  //! Returns font height.
  //! If returned value is equal -1 it means that font is resizable.
  Standard_Integer FontHeight() const { return myFaceSize; }

  Standard_EXPORT Standard_Boolean IsValid() const;
  
  //! Return true if the FontName, FontAspect and FontSize are the same.
  Standard_EXPORT Standard_Boolean IsEqual (const Handle(Font_SystemFont)& theOtherFont) const;

  //! Return TRUE if this is single-stroke (one-line) font, FALSE by default.
  //! Such fonts define single-line glyphs instead of closed contours, so that they are rendered incorrectly by normal software.
  Standard_Boolean IsSingleStrokeFont() const { return myIsSingleLine; }

  //! Set if this font should be rendered as single-stroke (one-line).
  void SetSingleStrokeFont (Standard_Boolean theIsSingleLine) { myIsSingleLine = theIsSingleLine; }

private:

  Handle(TCollection_HAsciiString) myFontName;
  Font_FontAspect                  myFontAspect;
  Standard_Integer                 myFaceSize;
  Handle(TCollection_HAsciiString) myFilePath;
  Standard_Boolean                 myIsSingleLine; //!< single stroke font flag, FALSE by default
  Standard_Boolean                 myIsDefined;

};

DEFINE_STANDARD_HANDLE(Font_SystemFont, Standard_Transient)

#endif // _Font_SystemFont_HeaderFile
