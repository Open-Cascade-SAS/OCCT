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

// Updated:

#include <Font_SystemFont.hxx>
#include <OSD_Path.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Font_SystemFont,Standard_Transient)

Font_SystemFont::Font_SystemFont():
MyFontName(),
MyFontAspect(Font_FA_Undefined),
MyFaceSize(-1),
MyVerification(Standard_False)
{
}

Font_SystemFont::Font_SystemFont( const Handle(TCollection_HAsciiString)& FontName,
                                const Font_FontAspect FontAspect,
                                const Handle(TCollection_HAsciiString)& FilePath ):
MyFontName(FontName),
MyFontAspect(FontAspect),
MyFaceSize(-1),
MyFilePath(FilePath),
MyVerification(Standard_True)
{

}

Font_SystemFont::Font_SystemFont (const Handle(TCollection_HAsciiString)& theXLFD,
                                  const Handle(TCollection_HAsciiString)& theFilePath) :
MyFontAspect(Font_FA_Regular),
MyFaceSize(-1),
MyFilePath(theFilePath)
{
  MyVerification = Standard_True;
  if (theXLFD.IsNull())
  {
    MyVerification = Standard_False; // empty font description handler
  }
  if (theXLFD->IsEmpty())
  {
    MyVerification = Standard_False; // empty font description
  }

  if (MyVerification)
  {
    MyFontName = theXLFD->Token ("-", 2);
    TCollection_AsciiString aXLFD (theXLFD->ToCString());

    // Getting font size for fixed size fonts
    if (aXLFD.Search ("-0-0-0-0-") >= 0)
      MyFaceSize = -1; // Scalable font
    else
      //TODO catch exeption
      MyFaceSize = aXLFD.Token ("-", 7).IntegerValue();

    // Detect font aspect
    if (aXLFD.Token ("-", 3).IsEqual ("bold") &&
       (aXLFD.Token ("-", 4).IsEqual ("i") || aXLFD.Token ("-", 4).IsEqual ("o")))
    {
      MyFontAspect = Font_FA_BoldItalic;
    }
    else if (aXLFD.Token ("-", 3).IsEqual ("bold"))
    {
      MyFontAspect = Font_FA_Bold;
    }
    else if (aXLFD.Token ("-", 4).IsEqual ("i") || aXLFD.Token ("-", 4).IsEqual ("o"))
    {
      MyFontAspect = Font_FA_Italic;
    }
  }
}

Standard_Boolean Font_SystemFont::IsValid() const{
  if ( !MyVerification)
    return Standard_False;

  if ( MyFontAspect == Font_FA_Undefined )
    return Standard_False;

  if ( MyFontName->IsEmpty() || !MyFontName->IsAscii() )
    return Standard_False;

  OSD_Path path;
  return path.IsValid( MyFilePath->String() );
}

Handle(TCollection_HAsciiString) Font_SystemFont::FontPath() const{
  return MyFilePath;
}

Handle(TCollection_HAsciiString) Font_SystemFont::FontName() const{
  return MyFontName;
}

Font_FontAspect Font_SystemFont::FontAspect() const{
  return MyFontAspect;
}

Standard_Integer Font_SystemFont::FontHeight() const {
  return MyFaceSize;
}

Standard_Boolean Font_SystemFont::IsEqual(const Handle(Font_SystemFont)& theOtherFont) const
{
  if (!MyFontName->IsSameString (theOtherFont->FontName(), Standard_False))
  {
    return Standard_False;
  }

  if (MyFontAspect != theOtherFont->FontAspect())
  {
    return Standard_False;
  }

  if (MyFaceSize != theOtherFont->FontHeight())
  {
    return Standard_False;
  }

  return Standard_True;
}
