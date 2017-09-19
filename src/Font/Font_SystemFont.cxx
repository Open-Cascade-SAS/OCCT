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

#include <Font_SystemFont.hxx>

#include <OSD_Path.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Font_SystemFont, Standard_Transient)

// =======================================================================
// function : Font_SystemFont
// purpose  :
// =======================================================================
Font_SystemFont::Font_SystemFont()
: myFontAspect (Font_FA_Undefined),
  myFaceSize (-1),
  myIsSingleLine (Standard_False),
  myIsDefined (Standard_False)
{
  //
}

// =======================================================================
// function : Font_SystemFont
// purpose  :
// =======================================================================
Font_SystemFont::Font_SystemFont (const Handle(TCollection_HAsciiString)& theFontName,
                                  const Font_FontAspect theFontAspect,
                                  const Handle(TCollection_HAsciiString)& theFilePath)
: myFontName (theFontName),
  myFontAspect (theFontAspect),
  myFaceSize (-1),
  myFilePath (theFilePath),
  myIsSingleLine (Standard_False),
  myIsDefined (Standard_True)
{
  //
}

// =======================================================================
// function : Font_SystemFont
// purpose  :
// =======================================================================
Font_SystemFont::Font_SystemFont (const Handle(TCollection_HAsciiString)& theXLFD,
                                  const Handle(TCollection_HAsciiString)& theFilePath)
: myFontAspect (Font_FA_Regular),
  myFaceSize (-1),
  myFilePath (theFilePath),
  myIsSingleLine (Standard_False),
  myIsDefined (Standard_True)
{
  if (theXLFD.IsNull()
   || theXLFD->IsEmpty())
  {
    myIsDefined = Standard_False;
    return;
  }

  myFontName = theXLFD->Token ("-", 2);
  const TCollection_AsciiString& aXLFD = theXLFD->String();

  // Getting font size for fixed size fonts
  if (aXLFD.Search ("-0-0-0-0-") >= 0)
  {
    myFaceSize = -1; // Scalable font
  }
  else
  {
    myFaceSize = aXLFD.Token ("-", 7).IntegerValue();
  }

  // Detect font aspect
  if (aXLFD.Token ("-", 3).IsEqual ("bold")
   && (aXLFD.Token ("-", 4).IsEqual ("i")
    || aXLFD.Token ("-", 4).IsEqual ("o")))
  {
    myFontAspect = Font_FA_BoldItalic;
  }
  else if (aXLFD.Token ("-", 3).IsEqual ("bold"))
  {
    myFontAspect = Font_FA_Bold;
  }
  else if (aXLFD.Token ("-", 4).IsEqual ("i")
        || aXLFD.Token ("-", 4).IsEqual ("o"))
  {
    myFontAspect = Font_FA_Italic;
  }
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
Standard_Boolean Font_SystemFont::IsValid() const
{
  return myIsDefined
     &&  myFontAspect != Font_FA_Undefined
     && !myFontName->IsEmpty()
     &&  OSD_Path::IsValid (myFilePath->String());
}

// =======================================================================
// function : IsEqual
// purpose  :
// =======================================================================
Standard_Boolean Font_SystemFont::IsEqual (const Handle(Font_SystemFont)& theOtherFont) const
{
  return myFontName->IsSameString (myFontName, Standard_False)
      && myFontAspect == theOtherFont->myFontAspect
      && myFaceSize   == theOtherFont->myFaceSize;
}
