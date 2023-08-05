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

#include <Font_FontAspect.hxx>
#include <Standard.hxx>
#include <NCollection_DefineAlloc.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! This class stores information about the font, which is merely a file path and cached metadata about the font.
class Font_SystemFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Font_SystemFont, Standard_Transient)
public:

  //! Creates a new font object.
  Standard_EXPORT Font_SystemFont (const TCollection_AsciiString& theFontName);

  //! Returns font family name (lower-cased).
  const TCollection_AsciiString& FontKey() const { return myFontKey; }

  //! Returns font family name.
  const TCollection_AsciiString& FontName() const { return myFontName; }
  
  //! Returns font file path.
  const TCollection_AsciiString& FontPath (Font_FontAspect theAspect) const
  {
    return myFilePaths[theAspect != Font_FontAspect_UNDEFINED ? theAspect : Font_FontAspect_Regular];
  }

  //! Returns font file path.
  Standard_Integer FontFaceId (Font_FontAspect theAspect) const
  {
    return myFaceIds[theAspect != Font_FontAspect_UNDEFINED ? theAspect : Font_FontAspect_Regular];
  }

  //! Sets font file path for specific aspect.
  Standard_EXPORT void SetFontPath (Font_FontAspect theAspect,
                                    const TCollection_AsciiString& thePath,
                                    const Standard_Integer theFaceId = 0);

  //! Returns TRUE if dedicated file for specified font aspect has been defined.
  bool HasFontAspect (Font_FontAspect theAspect) const
  {
    return !myFilePaths[theAspect != Font_FontAspect_UNDEFINED ? theAspect : Font_FontAspect_Regular].IsEmpty();
  }

  //! Returns any defined font file path.
  const TCollection_AsciiString& FontPathAny (Font_FontAspect theAspect,
                                              bool& theToSynthesizeItalic,
                                              Standard_Integer& theFaceId) const
  {
    const Font_FontAspect anAspect = theAspect != Font_FontAspect_UNDEFINED ? theAspect : Font_FontAspect_Regular;
    const TCollection_AsciiString& aPath = myFilePaths[anAspect];
    theFaceId = myFaceIds[anAspect];
    if (!aPath.IsEmpty())
    {
      return aPath;
    }

    if (theAspect == Font_FontAspect_Italic
     || theAspect == Font_FontAspect_BoldItalic)
    {
      if (theAspect == Font_FontAspect_BoldItalic
      && !myFilePaths[Font_FontAspect_Bold].IsEmpty())
      {
        theToSynthesizeItalic = true;
        theFaceId = myFaceIds[Font_FontAspect_Bold];
        return myFilePaths[Font_FontAspect_Bold];
      }
      else if (!myFilePaths[Font_FontAspect_Regular].IsEmpty())
      {
        theToSynthesizeItalic = true;
        theFaceId = myFaceIds[Font_FontAspect_Regular];
        return myFilePaths[Font_FontAspect_Regular];
      }
    }

    if (!myFilePaths[Font_FontAspect_Regular].IsEmpty())
    {
      theFaceId = myFaceIds[Font_FontAspect_Regular];
      return myFilePaths[Font_FontAspect_Regular];
    }

    for (int anAspectIter = 0; anAspectIter < Font_FontAspect_NB; ++anAspectIter)
    {
      if (!myFilePaths[anAspectIter].IsEmpty())
      {
        theFaceId = myFaceIds[anAspectIter];
        return myFilePaths[anAspectIter];
      }
    }
    theFaceId = myFaceIds[Font_FontAspect_Regular];
    return myFilePaths[Font_FontAspect_Regular];
  }

  //! Return true if the FontName, FontAspect and FontSize are the same.
  Standard_EXPORT Standard_Boolean IsEqual (const Handle(Font_SystemFont)& theOtherFont) const;

  //! Return TRUE if this is single-stroke (one-line) font, FALSE by default.
  //! Such fonts define single-line glyphs instead of closed contours, so that they are rendered incorrectly by normal software.
  Standard_Boolean IsSingleStrokeFont() const { return myIsSingleLine; }

  //! Set if this font should be rendered as single-stroke (one-line).
  void SetSingleStrokeFont (Standard_Boolean theIsSingleLine) { myIsSingleLine = theIsSingleLine; }

  //! Format font description.
  Standard_EXPORT TCollection_AsciiString ToString() const;

public:

  bool operator==(const Font_SystemFont& theFont) const
  {
    return myFontKey.IsEqual(theFont.FontKey());
  }

private:

  TCollection_AsciiString myFilePaths[Font_FontAspect_NB]; //!< paths to the font file
  Standard_Integer        myFaceIds  [Font_FontAspect_NB]; //!< face ids per font file
  TCollection_AsciiString myFontKey;      //!< font family name, lower cased
  TCollection_AsciiString myFontName;     //!< font family name
  Standard_Boolean        myIsSingleLine; //!< single stroke font flag, FALSE by default

};

namespace std
{
  template<>
  struct hash<Handle(Font_SystemFont)>
  {
    size_t operator()(const Handle (Font_SystemFont)& theLink) const noexcept
    {
      if (theLink.IsNull()) return 0;
      return std::hash<TCollection_AsciiString>{}(theLink->FontKey());
    }
  };
};

DEFINE_STANDARD_HANDLE(Font_SystemFont, Standard_Transient)

#endif // _Font_SystemFont_HeaderFile
