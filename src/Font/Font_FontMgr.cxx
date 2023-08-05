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

#include <Font_FontMgr.hxx>

#include <Font_NameOfFont.hxx>
#include <Font_FTLibrary.hxx>
#include <Font_SystemFont.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Environment.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#ifdef HAVE_FREETYPE
  #include "Font_DejavuSans_Latin_woff.pxx"

  #include <ft2build.h>
  #include FT_FREETYPE_H
#endif
IMPLEMENT_STANDARD_RTTIEXT(Font_FontMgr,Standard_Transient)

#if defined(_WIN32)

  #include <windows.h>
  #include <stdlib.h>

  namespace
  {
    // list of supported extensions
    static Standard_CString Font_FontMgr_Extensions[] =
    {
      "ttf",
      "otf",
      "ttc",
      NULL
    };
  }

#else

  #include <OSD_DirectoryIterator.hxx>
  #include <OSD_FileIterator.hxx>
  #include <OSD_Path.hxx>
  #include <OSD_File.hxx>
  #include <OSD_OpenMode.hxx>
  #include <OSD_Protection.hxx>

  namespace
  {

    // list of supported extensions
    static Standard_CString Font_FontMgr_Extensions[] =
    {
      "ttf",
      "otf",
      "ttc",
      "pfa",
      "pfb",
    #ifdef __APPLE__
      // Datafork TrueType (OS X), obsolete
      //"dfont",
    #endif
      NULL
    };

  #if defined(HAVE_FREETYPE) && !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
    // X11 configuration file in plain text format (obsolete - doesn't exists in modern distributives)
    static Standard_CString myFontServiceConf[] = {"/etc/X11/fs/config",
                                                   "/usr/X11R6/lib/X11/fs/config",
                                                   "/usr/X11/lib/X11/fs/config",
                                                   NULL
                                                  };

    // Although fontconfig library can be built for various platforms,
    // practically it is useful only on desktop Linux distributions, where it is always packaged.
    #include <fontconfig/fontconfig.h>
  #endif

  #ifdef __APPLE__
    // default fonts paths in Mac OS X
    static Standard_CString myDefaultFontsDirs[] = {"/System/Library/Fonts",
                                                    "/Library/Fonts",
                                                    NULL
                                                   };
  #else
    // default fonts paths in most Unix systems (Linux and others)
    static Standard_CString myDefaultFontsDirs[] = {"/system/fonts",         // Android
                                                    "/usr/share/fonts",
                                                    "/usr/local/share/fonts",
                                                    NULL
                                                   };
  #endif

    static void addDirsRecursively (const OSD_Path&                           thePath,
                                    NCollection_Map<TCollection_AsciiString>& theDirsMap)
    {
      TCollection_AsciiString aDirName;
      thePath.SystemName (aDirName);
      if (!theDirsMap.Add (aDirName))
      {
        return;
      }

      for (OSD_DirectoryIterator aDirIterator (thePath, "*"); aDirIterator.More(); aDirIterator.Next())
      {
        OSD_Path aChildDirPath;
        aDirIterator.Values().Path (aChildDirPath);

        TCollection_AsciiString aChildDirName;
        aChildDirPath.SystemName (aChildDirName);
        if (!aChildDirName.IsEqual (".") && !aChildDirName.IsEqual (".."))
        {
          aChildDirName = aDirName + "/" + aChildDirName;
          OSD_Path aPath (aChildDirName);
          addDirsRecursively (aPath, theDirsMap);
        }
      }
    }

  } // anonymous namespace

#endif

//! Retrieve font information.
//! @param theFonts   [out] list of validated fonts
//! @param theFTLib    [in] font library
//! @param theFontPath [in] path to the file
//! @param theFaceId   [in] face id, or -1 to load all faces within the file
//! @return TRUE if at least one font face has been detected
static bool checkFont (NCollection_Sequence<Handle(Font_SystemFont)>& theFonts,
                       const Handle(Font_FTLibrary)& theFTLib,
                       const TCollection_AsciiString& theFontPath,
                       signed long theFaceId = -1) // FT_Long
{
#ifdef HAVE_FREETYPE
  const FT_Long aFaceId = theFaceId != -1 ? theFaceId : 0;
  FT_Face aFontFace;
  FT_Error aFaceError = FT_New_Face (theFTLib->Instance(), theFontPath.ToCString(), aFaceId, &aFontFace);
  if (aFaceError != FT_Err_Ok)
  {
    return false;
  }
  if (aFontFace->family_name == NULL // skip broken fonts (error in FreeType?)
   || FT_Select_Charmap (aFontFace, ft_encoding_unicode) != 0) // Font_FTFont supports only UNICODE fonts
  {
    FT_Done_Face (aFontFace);
    return false;
  }

  // FreeType decomposes font definition into Family Name and Style Name,
  // so that fonts within the same Family and different Styles can be identified.
  // OCCT Font Manager natively handles 4 basic styles: Regular, Bold, Italic and Bold+Italic.
  // To include other non-standard Styles, their names can be appended to Family Name; for this, names of normal Styles should be removed.
  TCollection_AsciiString aFamily (aFontFace->family_name);
  TCollection_AsciiString aStyle (aFontFace->style_name != NULL ? aFontFace->style_name : "");
  Font_FontAspect anAspect = Font_FA_Regular;
  if (aFontFace->style_flags == (FT_STYLE_FLAG_ITALIC | FT_STYLE_FLAG_BOLD))
  {
    anAspect = Font_FA_BoldItalic;
    const Standard_Integer aRemoveItalic = aStyle.Search ("Italic");
    if (aRemoveItalic != -1)
    {
      aStyle.Remove (aRemoveItalic, 6);
    }
    else
    {
      // synonym
      const Standard_Integer aRemoveOblique = aStyle.Search ("Oblique");
      if (aRemoveOblique != -1)
      {
        aStyle.Remove (aRemoveOblique, 7);
      }
    }

    const Standard_Integer aRemoveBold = aStyle.Search ("Bold");
    if (aRemoveBold != -1)
    {
      aStyle.Remove (aRemoveBold, 4);
    }
  }
  else if (aFontFace->style_flags == FT_STYLE_FLAG_ITALIC)
  {
    anAspect = Font_FA_Italic;
    const Standard_Integer aRemoveItalic = aStyle.Search ("Italic");
    if (aRemoveItalic != -1)
    {
      aStyle.Remove (aRemoveItalic, 6);
    }
    else
    {
      // synonym
      const Standard_Integer aRemoveOblique = aStyle.Search ("Oblique");
      if (aRemoveOblique != -1)
      {
        aStyle.Remove (aRemoveOblique, 7);
      }
    }
  }
  else if (aFontFace->style_flags == FT_STYLE_FLAG_BOLD)
  {
    anAspect = Font_FA_Bold;
    const Standard_Integer aRemoveBold = aStyle.Search ("Bold");
    if (aRemoveBold != -1)
    {
      aStyle.Remove (aRemoveBold, 4);
    }
  }

  const Standard_Integer aRemoveReg = aStyle.Search ("Regular");
  if (aRemoveReg != -1)
  {
    aStyle.Remove (aRemoveReg, 7);
  }
  else
  {
    // synonym
    const Standard_Integer aRemoveBook = aStyle.Search ("Book");
    if (aRemoveBook != -1)
    {
      aStyle.Remove (aRemoveBook, 4);
    }
  }

  aStyle.LeftAdjust();
  aStyle.RightAdjust();
  for (;;)
  {
    // remove double spaces after removal of several keywords in-between, like "Condensed Bold Italic Oblique"
    const Standard_Integer aRemoveSpace = aStyle.Search ("  ");
    if (aRemoveSpace == -1)
    {
      break;
    }

    aStyle.Remove (aRemoveSpace, 1);
  }

  if (!aStyle.IsEmpty())
  {
    aFamily = aFamily + " " + aStyle;
  }

  Handle(Font_SystemFont) aResult = new Font_SystemFont (aFamily);
  aResult->SetFontPath (anAspect, theFontPath, (Standard_Integer )aFaceId);
  // automatically identify some known single-line fonts
  aResult->SetSingleStrokeFont (aResult->FontKey().StartsWith ("olf "));
  theFonts.Append (aResult);

  if (theFaceId < aFontFace->num_faces)
  {
    const FT_Long aNbInstances = aFontFace->style_flags >> 16;
    for (FT_Long anInstIter = 1; anInstIter < aNbInstances; ++anInstIter)
    {
      const FT_Long aSubFaceId = aFaceId + (anInstIter << 16);
      checkFont (theFonts, theFTLib, theFontPath, aSubFaceId);
    }
  }
  if (theFaceId == -1)
  {
    for (FT_Long aFaceIter = 1; aFaceIter < aFontFace->num_faces; ++aFaceIter)
    {
      checkFont (theFonts, theFTLib, theFontPath, aFaceIter);
    }
  }

  FT_Done_Face (aFontFace);
  return true;
#else
  (void )theFonts;
  (void )theFTLib;
  (void )theFontPath;
  (void )theFaceId;
  return false;
#endif
}

// =======================================================================
// function : GetInstance
// purpose  :
// =======================================================================
Handle(Font_FontMgr) Font_FontMgr::GetInstance()
{
  static Handle(Font_FontMgr) _mgr;
  if (_mgr.IsNull())
  {
    _mgr = new Font_FontMgr();
  }

  return _mgr;
}

// =======================================================================
// function : ToUseUnicodeSubsetFallback
// purpose  :
// =======================================================================
Standard_Boolean& Font_FontMgr::ToUseUnicodeSubsetFallback()
{
  static Standard_Boolean TheToUseUnicodeSubsetFallback = true;
  return TheToUseUnicodeSubsetFallback;
}

// =======================================================================
// function : AddFontAlias
// purpose  :
// =======================================================================
bool Font_FontMgr::AddFontAlias (const TCollection_AsciiString& theAliasName,
                                 const TCollection_AsciiString& theFontName)
{
  TCollection_AsciiString anAliasName (theAliasName);
  anAliasName.LowerCase();
  Handle(Font_FontAliasSequence) anAliases;
  if (!myFontAliases.Find (anAliasName, anAliases))
  {
    anAliases = new Font_FontAliasSequence();
    myFontAliases.Bind (anAliasName, anAliases);
  }

  for (Font_FontAliasSequence::Iterator anAliasIter (*anAliases); anAliasIter.More(); anAliasIter.Next())
  {
    if (anAliasIter.Value().FontName.IsEqual (anAliasName))
    {
      return false;
    }
  }

  anAliases->Append (Font_FontAlias (theFontName));
  return true;
}

// =======================================================================
// function : RemoveFontAlias
// purpose  :
// =======================================================================
bool Font_FontMgr::RemoveFontAlias (const TCollection_AsciiString& theAliasName,
                                    const TCollection_AsciiString& theFontName)
{
  if (theAliasName.IsEmpty())
  {
    if (myFontAliases.IsEmpty())
    {
      return false;
    }
    myFontAliases.Clear();
    return true;
  }

  TCollection_AsciiString anAliasName (theAliasName);
  anAliasName.LowerCase();
  Handle(Font_FontAliasSequence) anAliases;
  if (!myFontAliases.Find (anAliasName, anAliases))
  {
    return false;
  }

  if (theFontName.IsEmpty())
  {
    myFontAliases.UnBind (anAliasName);
    return true;
  }

  for (Font_FontAliasSequence::Iterator aFontIter (*anAliases); aFontIter.More(); aFontIter.Next())
  {
    if (aFontIter.Value().FontName.IsEqual (theFontName))
    {
      anAliases->Remove (aFontIter);
      if (anAliases->IsEmpty())
      {
        myFontAliases.UnBind (anAliasName);
      }
      return true;
    }
  }
  return false;
}

// =======================================================================
// function : GetAllAliases
// purpose  :
// =======================================================================
void Font_FontMgr::GetAllAliases (TColStd_SequenceOfHAsciiString& theAliases) const
{
  for (NCollection_DataMap<TCollection_AsciiString, Handle(Font_FontAliasSequence)>::Iterator anAliasIter (myFontAliases);
       anAliasIter.More(); anAliasIter.Next())
  {
    theAliases.Append (new TCollection_HAsciiString (anAliasIter.Key()));
  }
}

// =======================================================================
// function : GetFontAliases
// purpose  :
// =======================================================================
void Font_FontMgr::GetFontAliases (TColStd_SequenceOfHAsciiString& theFontNames,
                                   const TCollection_AsciiString& theAliasName) const
{
  TCollection_AsciiString anAliasName (theAliasName);
  anAliasName.LowerCase();
  Handle(Font_FontAliasSequence) anAliases;
  if (!myFontAliases.Find (anAliasName, anAliases))
  {
    return;
  }

  for (Font_FontAliasSequence::Iterator aFontIter (*anAliases); aFontIter.More(); aFontIter.Next())
  {
    theFontNames.Append (new TCollection_HAsciiString (aFontIter.Value().FontName));
  }
}

// =======================================================================
// function : addFontAlias
// purpose  :
// =======================================================================
void Font_FontMgr::addFontAlias (const TCollection_AsciiString& theAliasName,
                                 const Handle(Font_FontAliasSequence)& theAliases,
                                 Font_FontAspect theAspect)
{
  if (theAliases.IsNull()
   || theAliases->IsEmpty())
  {
    return;
  }

  Handle(Font_FontAliasSequence) anAliases = theAliases;
  if (theAspect != Font_FA_Undefined)
  {
    anAliases = new Font_FontAliasSequence();
    for (Font_FontAliasSequence::Iterator anAliasIter (*theAliases); anAliasIter.More(); anAliasIter.Next())
    {
      const TCollection_AsciiString& aName = anAliasIter.Value().FontName;
      anAliases->Append (Font_FontAlias (aName, theAspect));
    }
  }

  TCollection_AsciiString anAliasName (theAliasName);
  anAliasName.LowerCase();
  myFontAliases.Bind (anAliasName, anAliases);
}

// =======================================================================
// function : Font_FontMgr
// purpose  :
// =======================================================================
Font_FontMgr::Font_FontMgr()
: myToTraceAliases (Standard_False)
{
  Handle(Font_FontAliasSequence) aMono   = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aSerif  = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aSans   = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aSymbol = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aScript = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aWinDin = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) anIris  = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aCJK    = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) aKorean = new Font_FontAliasSequence();
  Handle(Font_FontAliasSequence) anArab  = new Font_FontAliasSequence();

  // best matches - pre-installed on Windows, some of them are pre-installed on macOS,
  // and sometimes them can be found installed on other systems (by user)
  aMono  ->Append (Font_FontAlias ("courier new"));
  aSerif ->Append (Font_FontAlias ("times new roman"));
  aSans  ->Append (Font_FontAlias ("arial"));
  aSymbol->Append (Font_FontAlias ("symbol"));
  aScript->Append (Font_FontAlias ("script"));
  aWinDin->Append (Font_FontAlias ("wingdings"));
  anIris ->Append (Font_FontAlias ("lucida console"));

#if defined(__ANDROID__)
  // Noto font family is usually installed on Android 6+ devices
  aMono  ->Append (Font_FontAlias ("noto mono"));
  aSerif ->Append (Font_FontAlias ("noto serif"));
  // Droid font family is usually installed on Android 4+ devices
  aMono  ->Append (Font_FontAlias ("droid sans mono"));
  aSerif ->Append (Font_FontAlias ("droid serif"));
  aSans  ->Append (Font_FontAlias ("roboto")); // actually DroidSans.ttf
#elif !defined(_WIN32) && !defined(__APPLE__) //X11
  aSerif ->Append (Font_FontAlias ("times"));
  aSans  ->Append (Font_FontAlias ("helvetica"));
  // GNU FreeFonts family is usually installed on Linux
  aMono  ->Append (Font_FontAlias ("freemono"));
  aSerif ->Append (Font_FontAlias ("freeserif"));
  aSans  ->Append (Font_FontAlias ("freesans"));
  // DejaVu font family is usually installed on Linux
  aMono  ->Append (Font_FontAlias ("dejavu sans mono"));
  aSerif ->Append (Font_FontAlias ("dejavu serif"));
  aSans  ->Append (Font_FontAlias ("dejavu sans"));
#endif

  // default CJK (Chinese/Japanese/Korean) fonts
  aCJK   ->Append (Font_FontAlias ("simsun"));        // Windows
  aCJK   ->Append (Font_FontAlias ("droid sans fallback")); // Android, Linux
  aCJK   ->Append (Font_FontAlias ("noto sans sc"));  // Android
  aCJK   ->Append (Font_FontAlias ("noto sans cjk jp"));

#if defined(_WIN32)
  aKorean->Append (Font_FontAlias ("malgun gothic")); // introduced since Vista
  aKorean->Append (Font_FontAlias ("gulim"));         // used on older systems (Windows XP)
#elif defined(__APPLE__)
  aKorean->Append (Font_FontAlias ("applegothic"));
  aKorean->Append (Font_FontAlias ("stfangsong"));
#endif
  aKorean->Append (Font_FontAlias ("nanumgothic"));   // Android, Linux
  aKorean->Append (Font_FontAlias ("noto sans kr"));  // Android
  aKorean->Append (Font_FontAlias ("nanummyeongjo")); // Linux
  aKorean->Append (Font_FontAlias ("noto serif cjk jp")); // Linux
  aKorean->Append (Font_FontAlias ("noto sans cjk jp")); // Linux
  aKorean->Append (Font_FontAlias ("droid sans fallback")); // Linux

#if defined(_WIN32)
  anArab->Append (Font_FontAlias ("times new roman"));
#elif defined(__APPLE__)
  anArab->Append (Font_FontAlias ("decotype naskh"));
#elif defined(__ANDROID__)
  anArab->Append (Font_FontAlias ("droid arabic naskh"));
  anArab->Append (Font_FontAlias ("noto naskh arabic"));
#endif

  addFontAlias ("mono",              aMono);
  addFontAlias ("courier",           aMono);                      // Font_NOF_ASCII_MONO
  addFontAlias ("monospace",         aMono);                      // Font_NOF_MONOSPACE
  addFontAlias ("rock",              aSans);                      // Font_NOF_CARTOGRAPHIC_SIMPLEX
  addFontAlias ("sansserif",         aSans);                      // Font_NOF_SANS_SERIF
  addFontAlias ("sans-serif",        aSans);
  addFontAlias ("sans",              aSans);
  addFontAlias ("arial",             aSans);
  addFontAlias ("times",             aSerif);
  addFontAlias ("serif",             aSerif);                     // Font_NOF_SERIF
  addFontAlias ("times-roman",       aSerif);                     // Font_NOF_ASCII_SIMPLEX
  addFontAlias ("times-bold",        aSerif, Font_FA_Bold);       // Font_NOF_ASCII_DUPLEX
  addFontAlias ("times-italic",      aSerif, Font_FA_Italic);     // Font_NOF_ASCII_ITALIC_COMPLEX
  addFontAlias ("times-bolditalic",  aSerif, Font_FA_BoldItalic); // Font_NOF_ASCII_ITALIC_TRIPLEX
  addFontAlias ("symbol",            aSymbol);                    // Font_NOF_GREEK_MONO
  addFontAlias ("iris",              anIris);                     // Font_NOF_KANJI_MONO
  addFontAlias ("korean",            aKorean);                    // Font_NOF_KOREAN
  addFontAlias ("cjk",               aCJK);                       // Font_NOF_CJK
  addFontAlias ("nsimsun",           aCJK);
  addFontAlias ("arabic",            anArab);                     // Font_NOF_ARABIC
  addFontAlias (Font_NOF_SYMBOL_MONO,          aWinDin);
  addFontAlias (Font_NOF_ASCII_SCRIPT_SIMPLEX, aScript);

  myFallbackAlias = aSans;

  InitFontDataBase();
}

// =======================================================================
// function : CheckFont
// purpose  :
// =======================================================================
Standard_Boolean Font_FontMgr::CheckFont (NCollection_Sequence<Handle(Font_SystemFont)>& theFonts,
                                          const TCollection_AsciiString& theFontPath) const
{
  Handle(Font_FTLibrary) aFtLibrary = new Font_FTLibrary();
  return checkFont (theFonts, aFtLibrary, theFontPath, 0);
}

// =======================================================================
// function : CheckFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::CheckFont (Standard_CString theFontPath) const
{
  Handle(Font_FTLibrary) aFtLibrary = new Font_FTLibrary();
  NCollection_Sequence<Handle(Font_SystemFont)> aFonts;
  return checkFont (aFonts, aFtLibrary, theFontPath, 0)
       ? aFonts.First()
       : Handle(Font_SystemFont)();
}

// =======================================================================
// function : RegisterFont
// purpose  :
// =======================================================================
Standard_Boolean Font_FontMgr::RegisterFont (const Handle(Font_SystemFont)& theFont,
                                             const Standard_Boolean         theToOverride)
{
  if (theFont.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer anOldIndex = myFontMap.FindIndex (theFont);
  if (anOldIndex == 0)
  {
    myFontMap.Add (theFont);
    return Standard_True;
  }

  Handle(Font_SystemFont) anOldFont = myFontMap.FindKey (anOldIndex);
  for (int anAspectIter = 0; anAspectIter < Font_FontAspect_NB; ++anAspectIter)
  {
    if (anOldFont->FontPath ((Font_FontAspect )anAspectIter).IsEqual (theFont->FontPath ((Font_FontAspect )anAspectIter))
     && anOldFont->FontFaceId ((Font_FontAspect )anAspectIter) == theFont->FontFaceId ((Font_FontAspect )anAspectIter))
    {
      continue;
    }
    else if (theToOverride
         || !anOldFont->HasFontAspect ((Font_FontAspect )anAspectIter))
    {
      anOldFont->SetFontPath ((Font_FontAspect )anAspectIter,
                              theFont->FontPath ((Font_FontAspect )anAspectIter),
                              theFont->FontFaceId ((Font_FontAspect )anAspectIter));
    }
    else if (theFont->HasFontAspect ((Font_FontAspect )anAspectIter))
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

// =======================================================================
// function : ClearFontDataBase()
// purpose  :
// =======================================================================
void Font_FontMgr::ClearFontDataBase()
{
  myFontMap.Clear();
}

// =======================================================================
// function : InitFontDataBase
// purpose  :
// =======================================================================
void Font_FontMgr::InitFontDataBase()
{
  myFontMap.Clear();
  Handle(Font_FTLibrary) aFtLibrary = new Font_FTLibrary();
  NCollection_Sequence<Handle(Font_SystemFont)> aFonts;

#if defined(OCCT_UWP)
  // system font files are not accessible
  (void )aFtLibrary;
#elif defined(_WIN32)

  // font directory is placed in "C:\Windows\Fonts\"
  UINT aStrLength = GetSystemWindowsDirectoryA (NULL, 0);
  if (aStrLength == 0)
  {
    return;
  }

  char* aWinDir = new char[aStrLength];
  GetSystemWindowsDirectoryA (aWinDir, aStrLength);
  TCollection_AsciiString aFontsDir (aWinDir);
  aFontsDir.AssignCat ("\\Fonts\\");
  delete[] aWinDir;

  // read fonts list from registry
  HKEY aFontsKey;
  if (RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                     0, KEY_READ, &aFontsKey) != ERROR_SUCCESS)
  {
    return;
  }

  NCollection_Map<TCollection_AsciiString> aSupportedExtensions;
  for (Standard_Integer anIter = 0; Font_FontMgr_Extensions[anIter] != NULL; ++anIter)
  {
    Standard_CString anExt = Font_FontMgr_Extensions[anIter];
    aSupportedExtensions.Add (TCollection_AsciiString (anExt));
  }

  static const DWORD aBufferSize = 256;
  char aNameBuff[aBufferSize];
  char aPathBuff[aBufferSize];
  DWORD aNameSize = aBufferSize;
  DWORD aPathSize = aBufferSize;
  for (DWORD anIter = 0;
       RegEnumValueA (aFontsKey, anIter,
                      aNameBuff, &aNameSize, NULL, NULL,
                      (LPBYTE )aPathBuff, &aPathSize) != ERROR_NO_MORE_ITEMS;
      ++anIter, aNameSize = aBufferSize, aPathSize = aBufferSize)
  {
    aPathBuff[(aPathSize < aBufferSize) ? aPathSize : (aBufferSize - 1)] = '\0'; // ensure string is NULL-terminated

    TCollection_AsciiString aFontName (aNameBuff), aFontPath (aPathBuff);
    if (aFontPath.Search ("\\") == -1)
    {
      aFontPath.Insert (1, aFontsDir); // make absolute path
    }

    // check file extension is in list of supported
    const Standard_Integer anExtensionPosition = aFontPath.SearchFromEnd (".") + 1;
    if (anExtensionPosition > 0 && anExtensionPosition < aFontPath.Length())
    {
      TCollection_AsciiString aFontExtension = aFontPath.SubString (anExtensionPosition, aFontPath.Length());
      aFontExtension.LowerCase();
      if (aSupportedExtensions.Contains (aFontExtension))
      {
        aFonts.Clear();
        checkFont (aFonts, aFtLibrary, aFontPath.ToCString());
        RegisterFonts (aFonts, false);
      }
    }
  }

  // close registry key
  RegCloseKey (aFontsKey);

#else

  NCollection_Map<TCollection_AsciiString> aMapOfFontsDirs;
#if defined(HAVE_FREETYPE) && !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
  if (FcConfig* aFcCfg = FcInitLoadConfig())
  {
    if (FcStrList* aFcFontDir = FcConfigGetFontDirs (aFcCfg))
    {
      for (;;)
      {
        FcChar8* aFcFolder = FcStrListNext (aFcFontDir);
        if (aFcFolder == NULL)
        {
          break;
        }

        TCollection_AsciiString aPathStr ((const char* )aFcFolder);
        OSD_Path aPath (aPathStr);
        addDirsRecursively (aPath, aMapOfFontsDirs);
      }
      FcStrListDone (aFcFontDir);
    }
    FcConfigDestroy (aFcCfg);
  }

  const OSD_Protection aProtectRead (OSD_R, OSD_R, OSD_R, OSD_R);
  if (aMapOfFontsDirs.IsEmpty())
  {
    Message::SendAlarm ("Font_FontMgr, fontconfig library returns an empty folder list");

    // read fonts directories from font service config file (obsolete)
    for (Standard_Integer anIter = 0; myFontServiceConf[anIter] != NULL; ++anIter)
    {
      const TCollection_AsciiString aFileOfFontsPath (myFontServiceConf[anIter]);
      OSD_File aFile (aFileOfFontsPath);
      if (!aFile.Exists())
      {
        continue;
      }

      aFile.Open (OSD_ReadOnly, aProtectRead);
      if (!aFile.IsOpen())
      {
        continue;
      }

      Standard_Integer aNByte = 256;
      Standard_Integer aNbyteRead;
      TCollection_AsciiString aStr; // read string with information
      while (!aFile.IsAtEnd())
      {
        Standard_Integer aLocation = -1;
        Standard_Integer aPathLocation = -1;

        aFile.ReadLine (aStr, aNByte, aNbyteRead); // reading 1 line (256 bytes)
        aLocation = aStr.Search ("catalogue=");
        if (aLocation < 0)
        {
          aLocation = aStr.Search ("catalogue =");
        }

        aPathLocation = aStr.Search ("/");
        if (aLocation > 0 && aPathLocation > 0)
        {
          aStr = aStr.Split (aPathLocation - 1);
          TCollection_AsciiString aFontPath;
          Standard_Integer aPathNumber = 1;
          do
          {
            // Getting directory paths, which can be split by "," or ":"
            aFontPath = aStr.Token (":,", aPathNumber);
            aFontPath.RightAdjust();
            if (!aFontPath.IsEmpty())
            {
              OSD_Path aPath(aFontPath);
              addDirsRecursively (aPath, aMapOfFontsDirs);
            }
            aPathNumber++;
          }
          while (!aFontPath.IsEmpty());
        }
      }
      aFile.Close();
    }
  }
#endif

  // append default directories
  for (Standard_Integer anIter = 0; myDefaultFontsDirs[anIter] != NULL; ++anIter)
  {
    Standard_CString anItem = myDefaultFontsDirs[anIter];
    TCollection_AsciiString aPathStr (anItem);
    OSD_Path aPath (aPathStr);
    addDirsRecursively (aPath, aMapOfFontsDirs);
  }

  NCollection_Map<TCollection_AsciiString> aSupportedExtensions;
  for (Standard_Integer anIter = 0; Font_FontMgr_Extensions[anIter] != NULL; ++anIter)
  {
    Standard_CString anExt = Font_FontMgr_Extensions[anIter];
    aSupportedExtensions.Add (TCollection_AsciiString (anExt));
  }

  for (NCollection_Map<TCollection_AsciiString>::Iterator anIter (aMapOfFontsDirs);
       anIter.More(); anIter.Next())
  {
  #if defined(HAVE_FREETYPE) && !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
    OSD_File aReadFile (anIter.Value() + "/fonts.dir");
    if (!aReadFile.Exists())
    {
  #endif
      OSD_Path aFolderPath (anIter.Value());
      for (OSD_FileIterator aFileIter (aFolderPath, "*"); aFileIter.More(); aFileIter.Next())
      {
        OSD_Path aFontFilePath;
        aFileIter.Values().Path (aFontFilePath);

        TCollection_AsciiString aFontFileName;
        aFontFilePath.SystemName (aFontFileName);
        aFontFileName = anIter.Value() + "/" + aFontFileName;

        aFonts.Clear();
        checkFont (aFonts, aFtLibrary, aFontFileName);
        RegisterFonts (aFonts, false);
      }

  #if defined(HAVE_FREETYPE) && !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
      continue;
    }

    aReadFile.Open (OSD_ReadOnly, aProtectRead);
    if (!aReadFile.IsOpen())
    {
      continue; // invalid fonts directory
    }

    Standard_Integer aNbyteRead, aNByte = 256;
    TCollection_AsciiString aLine (aNByte);
    Standard_Boolean isFirstLine = Standard_True;
    const TCollection_AsciiString anEncoding ("iso8859-1\n");
    while (!aReadFile.IsAtEnd())
    {
      aReadFile.ReadLine (aLine, aNByte, aNbyteRead);
      if (isFirstLine)
      {
        // first line contains the number of fonts in this file
        // just ignoring it...
        isFirstLine = Standard_False;
        continue;
      }

      Standard_Integer anExtensionPosition = aLine.Search (".") + 1;
      if (anExtensionPosition == 0)
      {
        continue; // can't find extension position in the font description
      }

      Standard_Integer anEndOfFileName = aLine.Location (" ", anExtensionPosition, aLine.Length()) - 1;
      if (anEndOfFileName < 0 || anEndOfFileName < anExtensionPosition)
      {
        continue; // font description have empty extension
      }

      TCollection_AsciiString aFontExtension = aLine.SubString (anExtensionPosition, anEndOfFileName);
      aFontExtension.LowerCase();
      if (aSupportedExtensions.Contains (aFontExtension) && (aLine.Search (anEncoding) > 0))
      {
        // In current implementation use fonts with ISO-8859-1 coding page.
        // OCCT not give to manage coding page by means of program interface.
        // TODO: make high level interface for choosing necessary coding page.
        TCollection_AsciiString aXLFD (aLine.SubString (anEndOfFileName + 2, aLine.Length()));
        TCollection_AsciiString aFontPath (anIter.Value().ToCString());
        if (aFontPath.SearchFromEnd ("/") != aFontPath.Length())
        {
          aFontPath.AssignCat ("/");
        }
        TCollection_AsciiString aFontFileName (aLine.SubString (1, anEndOfFileName));
        aFontPath.AssignCat (aFontFileName);

        aFonts.Clear();
        if (checkFont (aFonts, aFtLibrary, aFontPath))
        {
          RegisterFonts (aFonts, false);
          const Handle(Font_SystemFont)& aNewFont = aFonts.First();
          if (!aXLFD.IsEmpty()
            && aXLFD.Search ("-0-0-0-0-") != -1) // ignore non-resizable fonts
          {
            const TCollection_AsciiString anXName = aXLFD.Token ("-", 2);
            Font_FontAspect anXAspect = Font_FA_Regular;
            if (aXLFD.Token ("-", 3).IsEqual ("bold")
             && (aXLFD.Token ("-", 4).IsEqual ("i")
              || aXLFD.Token ("-", 4).IsEqual ("o")))
            {
              anXAspect = Font_FA_BoldItalic;
            }
            else if (aXLFD.Token ("-", 3).IsEqual ("bold"))
            {
              anXAspect = Font_FA_Bold;
            }
            else if (aXLFD.Token ("-", 4).IsEqual ("i")
                  || aXLFD.Token ("-", 4).IsEqual ("o"))
            {
              anXAspect = Font_FA_Italic;
            }

            Handle(Font_SystemFont) aNewFontFromXLFD = new Font_SystemFont (anXName);
            aNewFontFromXLFD->SetFontPath (anXAspect, aFontPath, 0);
            if (!aNewFont->IsEqual (aNewFontFromXLFD))
            {
              RegisterFont (aNewFontFromXLFD, false);
            }
          }
        }
      }
    }
    aReadFile.Close();
  #endif
  }
#endif
}

// =======================================================================
// function : GetAvailableFontsNames
// purpose  :
// =======================================================================
void Font_FontMgr::GetAvailableFontsNames (TColStd_SequenceOfHAsciiString& theFontsNames) const
{
  theFontsNames.Clear();
  for (Font_FontMap::Iterator aFontIter (myFontMap);
       aFontIter.More(); aFontIter.Next())
  {
    const Handle(Font_SystemFont)& aFont = aFontIter.Value();
    theFontsNames.Append (new TCollection_HAsciiString(aFont->FontName()));
  }
}

// =======================================================================
// function : GetFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::GetFont (const Handle(TCollection_HAsciiString)& theFontName,
                                               const Font_FontAspect  theFontAspect,
                                               const Standard_Integer theFontSize) const
{
  if ((theFontSize < 2 && theFontSize != -1) || theFontName.IsNull())
  {
    return Handle(Font_SystemFont)();
  }

  Handle(Font_SystemFont) aFont = myFontMap.Find (theFontName->String());
  return (aFont.IsNull()
       || theFontAspect == Font_FontAspect_UNDEFINED
       || aFont->HasFontAspect (theFontAspect))
       ? aFont
       : Handle(Font_SystemFont)();
}

// =======================================================================
// function : GetFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::GetFont (const TCollection_AsciiString& theFontName) const
{
  return myFontMap.Find (theFontName);
}

// =======================================================================
// function : FindFallbackFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::FindFallbackFont (Font_UnicodeSubset theSubset,
                                                        Font_FontAspect theFontAspect) const
{
  Font_FontAspect aFontAspect = theFontAspect;
  Handle(Font_SystemFont) aFont;
  switch (theSubset)
  {
    case Font_UnicodeSubset_Western: aFont = FindFont (Font_NOF_SANS_SERIF, Font_StrictLevel_Aliases, aFontAspect, false); break;
    case Font_UnicodeSubset_Korean:  aFont = FindFont (Font_NOF_KOREAN,     Font_StrictLevel_Aliases, aFontAspect, false); break;
    case Font_UnicodeSubset_CJK:     aFont = FindFont (Font_NOF_CJK,        Font_StrictLevel_Aliases, aFontAspect, false); break;
    case Font_UnicodeSubset_Arabic:  aFont = FindFont (Font_NOF_ARABIC,     Font_StrictLevel_Aliases, aFontAspect, false); break;
  }
  if (aFont.IsNull())
  {
    const char* aRange = "";
    switch (theSubset)
    {
      case Font_UnicodeSubset_Western: aRange = "Western"; break;
      case Font_UnicodeSubset_Korean:  aRange = "Korean";  break;
      case Font_UnicodeSubset_CJK:     aRange = "CJK";     break;
      case Font_UnicodeSubset_Arabic:  aRange = "Arabic";  break;
    }
    Message::SendFail (TCollection_AsciiString("Font_FontMgr, error: unable to find ") + aRange + " fallback font!");
  }
  return aFont;
}

// =======================================================================
// function : FindFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::FindFont (const TCollection_AsciiString& theFontName,
                                                Font_StrictLevel theStrictLevel,
                                                Font_FontAspect& theFontAspect,
                                                Standard_Boolean theDoFailMsg) const
{
  TCollection_AsciiString aFontName (theFontName);
  aFontName.LowerCase();
  Handle(Font_SystemFont) aFont = myFontMap.Find (aFontName);
  if (!aFont.IsNull()
    || theStrictLevel == Font_StrictLevel_Strict)
  {
    return aFont;
  }

  // Trying to use font names mapping
  for (int aPass = 0; aPass < 2; ++aPass)
  {
    Handle(Font_FontAliasSequence) anAliases;
    if (aPass == 0)
    {
      myFontAliases.Find (aFontName, anAliases);
    }
    else if (theStrictLevel == Font_StrictLevel_Any)
    {
      anAliases = myFallbackAlias;
    }

    if (anAliases.IsNull()
     || anAliases->IsEmpty())
    {
      continue;
    }

    bool isAliasUsed = false, isBestAlias = false;
    for (Font_FontAliasSequence::Iterator anAliasIter (*anAliases); anAliasIter.More(); anAliasIter.Next())
    {
      const Font_FontAlias& anAlias = anAliasIter.Value();
      if (Handle(Font_SystemFont) aFont2 = myFontMap.Find (anAlias.FontName))
      {
        if (aFont.IsNull())
        {
          aFont = aFont2;
          isAliasUsed = true;
        }

        if ((anAlias.FontAspect != Font_FontAspect_UNDEFINED
          && aFont2->HasFontAspect (anAlias.FontAspect)))
        {
          // special case - alias refers to styled font (e.g. "times-bold")
          isBestAlias = true;
          theFontAspect = anAlias.FontAspect;
          break;
        }
        else if (anAlias.FontAspect == Font_FontAspect_UNDEFINED
              && (theFontAspect == Font_FontAspect_UNDEFINED
                || aFont2->HasFontAspect (theFontAspect)))
        {
          isBestAlias = true;
          break;
        }
      }
    }

    if (aPass == 0)
    {
      if (isAliasUsed && myToTraceAliases)
      {
        Message::SendTrace (TCollection_AsciiString("Font_FontMgr, using font alias '") + aFont->FontName() + "'"
                          " instead of requested '" + theFontName + "'");
      }
      if (isBestAlias)
      {
        return aFont;
      }
      else if (!aFont.IsNull())
      {
        break;
      }
    }
  }

  if (aFont.IsNull()
   && theStrictLevel == Font_StrictLevel_Any)
  {
    // try finding ANY font in case if even default fallback alias myFallbackAlias cannot be found
    aFont = myFontMap.Find (TCollection_AsciiString());
  }
  if (aFont.IsNull())
  {
    if (theDoFailMsg)
    {
      Message::SendFail ("Font_FontMgr, error: unable to find any font!");
    }
    return Handle(Font_SystemFont)();
  }

  if ((theFontAspect != Font_FA_Undefined
    && !aFont->HasFontAspect (theFontAspect))
   || (!aFontName.IsEmpty()
    && !aFontName.IsEqual (aFont->FontKey())))
  {
    TCollection_AsciiString aDesc = TCollection_AsciiString() + "'" + theFontName + "'"
                                  + TCollection_AsciiString() + " [" + Font_FontMgr::FontAspectToString (theFontAspect) + "]";
    Message::SendWarning (TCollection_AsciiString("Font_FontMgr, warning: unable to find font ")
                        + aDesc + "; " + aFont->ToString() + " is used instead");
  }
  return aFont;
}

// =======================================================================
// function : Font_FontMap::Find
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::Font_FontMap::Find (const TCollection_AsciiString& theFontName) const
{
  if (IsEmpty())
  {
    return Handle(Font_SystemFont)();
  }
  else if (theFontName.IsEmpty())
  {
    return FindKey (1); // return any font
  }
  Handle(Font_SystemFont) aTmpFont = new Font_SystemFont(theFontName);
  const int anInd = FindIndex(aTmpFont);
  if (anInd > 0)
  {
    return FindKey(anInd);
  }
  return Handle(Font_SystemFont)();
}

// =======================================================================
// function : EmbedFallbackFont
// purpose  :
// =======================================================================
Handle(NCollection_Buffer) Font_FontMgr::EmbedFallbackFont()
{
#ifdef HAVE_FREETYPE
  return new NCollection_Buffer (Handle(NCollection_BaseAllocator)(),
                                 Font_DejavuSans_Latin_woff_size,
                                 const_cast<Standard_Byte*>(Font_DejavuSans_Latin_woff));
#else
  return Handle(NCollection_Buffer)();
#endif
}
