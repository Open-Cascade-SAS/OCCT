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
#include <Font_FTLibrary.hxx>
#include <Font_SystemFont.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Environment.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <ft2build.h>
#include FT_FREETYPE_H
IMPLEMENT_STANDARD_RTTIEXT(Font_FontMgr,Standard_Transient)

struct Font_FontMgr_FontAliasMapNode
{
  const char *    EnumName;
  const char *    FontName;
  Font_FontAspect FontAspect;
};

static const Font_FontMgr_FontAliasMapNode Font_FontMgr_MapOfFontsAliases[] =
{

#if defined(_WIN32) || defined(__APPLE__)

  { "Courier"                  , "Courier New"    , Font_FA_Regular },
  { "Times-Roman"              , "Times New Roman", Font_FA_Regular  },
  { "Times-Bold"               , "Times New Roman", Font_FA_Bold },
  { "Times-Italic"             , "Times New Roman", Font_FA_Italic  },
  { "Times-BoldItalic"         , "Times New Roman", Font_FA_BoldItalic  },
  { "ZapfChancery-MediumItalic", "Script"         , Font_FA_Regular  },
  { "Symbol"                   , "Symbol"         , Font_FA_Regular  },
  { "ZapfDingbats"             , "WingDings"      , Font_FA_Regular  },
  { "Rock"                     , "Arial"          , Font_FA_Regular  },
  { "Iris"                     , "Lucida Console" , Font_FA_Regular  }

#elif defined(__ANDROID__)

  { "Courier"                  , "Droid Sans Mono", Font_FA_Regular },
  { "Times-Roman"              , "Droid Serif"    , Font_FA_Regular  },
  { "Times-Bold"               , "Droid Serif"    , Font_FA_Bold },
  { "Times-Italic"             , "Droid Serif"    , Font_FA_Italic  },
  { "Times-BoldItalic"         , "Droid Serif"    , Font_FA_BoldItalic  },
  { "Arial"                    , "Roboto"         , Font_FA_Regular  },

#else   //X11

  { "Courier"                  , "Courier"      , Font_FA_Regular },
  { "Times-Roman"              , "Times"        , Font_FA_Regular  },
  { "Times-Bold"               , "Times"        , Font_FA_Bold },
  { "Times-Italic"             , "Times"        , Font_FA_Italic  },
  { "Times-BoldItalic"         , "Times"        , Font_FA_BoldItalic  },
  { "Arial"                    , "Helvetica"    , Font_FA_Regular  },
  { "ZapfChancery-MediumItalic", "-adobe-itc zapf chancery-medium-i-normal--*-*-*-*-*-*-iso8859-1"              , Font_FA_Regular  },
  { "Symbol"                   , "-adobe-symbol-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"                , Font_FA_Regular  },
  { "ZapfDingbats"             , "-adobe-itc zapf dingbats-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"     , Font_FA_Regular  },
  { "Rock"                     , "-sgi-rock-medium-r-normal--*-*-*-*-p-*-iso8859-1"                             , Font_FA_Regular  },
  { "Iris"                     , "--iris-medium-r-normal--*-*-*-*-m-*-iso8859-1"                                , Font_FA_Regular  }
#endif

};

#define NUM_FONT_ENTRIES (int)(sizeof(Font_FontMgr_MapOfFontsAliases)/sizeof(Font_FontMgr_FontAliasMapNode))

#if defined(_WIN32)

  #include <windows.h>
  #include <stdlib.h>

  #ifdef _MSC_VER
    #pragma comment (lib, "freetype.lib")
  #endif

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

  };

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

  #if !defined(__ANDROID__) && !defined(__APPLE__)
    // X11 configuration file in plain text format (obsolete - doesn't exists in modern distributives)
    static Standard_CString myFontServiceConf[] = {"/etc/X11/fs/config",
                                                   "/usr/X11R6/lib/X11/fs/config",
                                                   "/usr/X11/lib/X11/fs/config",
                                                   NULL
                                                  };
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

// =======================================================================
// function : checkFont
// purpose  :
// =======================================================================
static Handle(Font_SystemFont) checkFont (const Handle(Font_FTLibrary)& theFTLib,
                                          const Standard_CString        theFontPath)
{
  FT_Face aFontFace;
  FT_Error aFaceError = FT_New_Face (theFTLib->Instance(), theFontPath, 0, &aFontFace);
  if (aFaceError != FT_Err_Ok)
  {
    return NULL;
  }

  Font_FontAspect anAspect = Font_FA_Regular;
  if (aFontFace->style_flags == (FT_STYLE_FLAG_ITALIC | FT_STYLE_FLAG_BOLD))
  {
    anAspect = Font_FA_BoldItalic;
  }
  else if (aFontFace->style_flags == FT_STYLE_FLAG_ITALIC)
  {
    anAspect = Font_FA_Italic;
  }
  else if (aFontFace->style_flags == FT_STYLE_FLAG_BOLD)
  {
    anAspect = Font_FA_Bold;
  }

  Handle(Font_SystemFont) aResult;
  if (aFontFace->family_name != NULL                           // skip broken fonts (error in FreeType?)
   && FT_Select_Charmap (aFontFace, ft_encoding_unicode) == 0) // Font_FTFont supports only UNICODE fonts
  {
    Handle(TCollection_HAsciiString) aFontName = new TCollection_HAsciiString (aFontFace->family_name);
    Handle(TCollection_HAsciiString) aFontPath = new TCollection_HAsciiString (theFontPath);
    aResult = new Font_SystemFont (aFontName, anAspect, aFontPath);
  }

  FT_Done_Face (aFontFace);

  return aResult;
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
// function : Font_FontMgr
// purpose  :
// =======================================================================
Font_FontMgr::Font_FontMgr()
{
  InitFontDataBase();
}

// =======================================================================
// function : CheckFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::CheckFont (Standard_CString theFontPath) const
{
  Handle(Font_FTLibrary) aFtLibrary = new Font_FTLibrary();
  return checkFont (aFtLibrary, theFontPath);
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

  for (Font_NListOfSystemFont::Iterator aFontIter (myListOfFonts);
       aFontIter.More(); aFontIter.Next())
  {
    if (!aFontIter.Value()->FontName()->IsSameString (theFont->FontName(), Standard_False))
    {
      continue;
    }

    if (theFont->FontAspect() != Font_FA_Undefined
     && aFontIter.Value()->FontAspect() != theFont->FontAspect())
    {
      continue;
    }

    if (theFont->FontHeight() == -1 || aFontIter.Value()->FontHeight() == -1
     || theFont->FontHeight() ==       aFontIter.Value()->FontHeight())
    {
      if (theFont->FontPath()->String() == aFontIter.Value()->FontPath()->String())
      {
        return Standard_True;
      }
      else if (theToOverride)
      {
        myListOfFonts.Remove (aFontIter);
      }
      else
      {
        return Standard_False;
      }
    }
  }

  myListOfFonts.Append (theFont);
  return Standard_True;
}

// =======================================================================
// function : InitFontDataBase
// purpose  :
// =======================================================================
void Font_FontMgr::InitFontDataBase()
{
  myListOfFonts.Clear();
  Handle(Font_FTLibrary) aFtLibrary;

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
  Handle(TCollection_HAsciiString) aFontsDir = new TCollection_HAsciiString (aWinDir);
  aFontsDir->AssignCat ("\\Fonts\\");
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

  aFtLibrary = new Font_FTLibrary();
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

    Handle(TCollection_HAsciiString) aFontName = new TCollection_HAsciiString (aNameBuff);
    Handle(TCollection_HAsciiString) aFontPath = new TCollection_HAsciiString (aPathBuff);
    if (aFontPath->Search ("\\") == -1)
    {
      aFontPath->Insert (1, aFontsDir); // make absolute path
    }

    // check file extension is in list of supported
    const Standard_Integer anExtensionPosition = aFontPath->SearchFromEnd (".") + 1;
    if (anExtensionPosition > 0 && anExtensionPosition < aFontPath->Length())
    {
      Handle(TCollection_HAsciiString) aFontExtension = aFontPath->SubString (anExtensionPosition, aFontPath->Length());
      aFontExtension->LowerCase();
      if (aSupportedExtensions.Contains (aFontExtension->String()))
      {
        Handle(Font_SystemFont) aNewFont = checkFont (aFtLibrary, aFontPath->ToCString());
        if (!aNewFont.IsNull())
        {
          myListOfFonts.Append (aNewFont);
        }
      }
    }
  }

  // close registry key
  RegCloseKey (aFontsKey);

#else

  NCollection_Map<TCollection_AsciiString> aMapOfFontsDirs;
#if !defined(__ANDROID__) && !defined(__APPLE__)
  const OSD_Protection aProtectRead (OSD_R, OSD_R, OSD_R, OSD_R);

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
          // Getting directory paths, which can be splitted by "," or ":"
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

  aFtLibrary = new Font_FTLibrary();
  for (NCollection_Map<TCollection_AsciiString>::Iterator anIter (aMapOfFontsDirs);
       anIter.More(); anIter.Next())
  {
  #if defined(__ANDROID__) || defined(__APPLE__)
    OSD_Path aFolderPath (anIter.Value());
    for (OSD_FileIterator aFileIter (aFolderPath, "*"); aFileIter.More(); aFileIter.Next())
    {
      OSD_Path aFontFilePath;
      aFileIter.Values().Path (aFontFilePath);

      TCollection_AsciiString aFontFileName;
      aFontFilePath.SystemName (aFontFileName);
      aFontFileName = anIter.Value() + "/" + aFontFileName;

      Handle(Font_SystemFont) aNewFont = checkFont (aFtLibrary, aFontFileName.ToCString());
      if (!aNewFont.IsNull())
      {
        myListOfFonts.Append (aNewFont);
      }
    }
  #else
    OSD_File aReadFile (anIter.Value() + "/fonts.dir");
    if (!aReadFile.Exists())
    {
      continue; // invalid fonts directory
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
        // OCCT not give to manage coding page by means of programm interface.
        // TODO: make high level interface for choosing necessary coding page.
        Handle(TCollection_HAsciiString) aXLFD =
          new TCollection_HAsciiString (aLine.SubString (anEndOfFileName + 2, aLine.Length()));
        Handle(TCollection_HAsciiString) aFontPath =
          new TCollection_HAsciiString (anIter.Value().ToCString());
        if (aFontPath->SearchFromEnd ("/") != aFontPath->Length())
        {
          aFontPath->AssignCat ("/");
        }
        Handle(TCollection_HAsciiString) aFontFileName =
        new TCollection_HAsciiString (aLine.SubString (1, anEndOfFileName));
        aFontPath->AssignCat (aFontFileName);

        Handle(Font_SystemFont) aNewFontFromXLFD = new Font_SystemFont (aXLFD, aFontPath);
        Handle(Font_SystemFont) aNewFont = checkFont (aFtLibrary, aFontPath->ToCString());

        if (aNewFontFromXLFD->IsValid() && !aNewFont.IsNull() &&
           !aNewFont->IsEqual (aNewFontFromXLFD))
        {
          myListOfFonts.Append (aNewFont);
          myListOfFonts.Append (aNewFontFromXLFD);
        }
        else if (!aNewFont.IsNull())
        {
          myListOfFonts.Append (aNewFont);
        }
        else if (aNewFontFromXLFD->IsValid())
        {
          myListOfFonts.Append (aNewFontFromXLFD);
        }
      }
    }
    aReadFile.Close();
  #endif
  }
#endif
}

// =======================================================================
// function : GetAvailableFonts
// purpose  :
// =======================================================================
const Font_NListOfSystemFont& Font_FontMgr::GetAvailableFonts() const
{
  return myListOfFonts;
}

// =======================================================================
// function : GetAvailableFontsNames
// purpose  :
// =======================================================================
void Font_FontMgr::GetAvailableFontsNames (TColStd_SequenceOfHAsciiString& theFontsNames) const
{
  theFontsNames.Clear();
  for (Font_NListOfSystemFont::Iterator anIter(myListOfFonts); anIter.More(); anIter.Next())
  {
    theFontsNames.Append (anIter.Value()->FontName());
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
  if ( (theFontSize < 2 && theFontSize != -1) || theFontName.IsNull())
  {
    return NULL;
  }

  for (Font_NListOfSystemFont::Iterator aFontsIterator (myListOfFonts);
       aFontsIterator.More(); aFontsIterator.Next())
  {
    if (!theFontName->IsEmpty() && !aFontsIterator.Value()->FontName()->IsSameString (theFontName, Standard_False))
    {
      continue;
    }

    if (theFontAspect != Font_FA_Undefined && aFontsIterator.Value()->FontAspect() != theFontAspect)
    {
      continue;
    }

    if (theFontSize == -1 || aFontsIterator.Value()->FontHeight() == -1 ||
        theFontSize == aFontsIterator.Value()->FontHeight())
    {
      return aFontsIterator.Value();
    }
  }

  return NULL;
}

// =======================================================================
// function : FindFont
// purpose  :
// =======================================================================
Handle(Font_SystemFont) Font_FontMgr::FindFont (const Handle(TCollection_HAsciiString)& theFontName,
                                                const Font_FontAspect  theFontAspect,
                                                const Standard_Integer theFontSize) const
{
  Handle(TCollection_HAsciiString) aFontName   = theFontName;
  Font_FontAspect                  aFontAspect = theFontAspect;
  Standard_Integer                 aFontSize   = theFontSize;

  Handle(Font_SystemFont) aFont = GetFont (aFontName, aFontAspect, aFontSize);
  if (!aFont.IsNull())
  {
    return aFont;
  }

  // Trying to use font names mapping
  for (Standard_Integer anIter = 0; anIter < NUM_FONT_ENTRIES; ++anIter)
  {
    Handle(TCollection_HAsciiString) aFontAlias =
      new TCollection_HAsciiString (Font_FontMgr_MapOfFontsAliases[anIter].EnumName);

    if (aFontAlias->IsSameString (aFontName, Standard_False))
    {
      aFontName = new TCollection_HAsciiString (Font_FontMgr_MapOfFontsAliases[anIter].FontName);
      aFontAspect = Font_FontMgr_MapOfFontsAliases[anIter].FontAspect;
      break;
    }
  }

  // check font family alias with specified font aspect
  if (theFontAspect != Font_FA_Undefined
   && theFontAspect != Font_FA_Regular
   && theFontAspect != aFontAspect)
  {
    aFont = GetFont (aFontName, theFontAspect, aFontSize);
    if (!aFont.IsNull())
    {
      return aFont;
    }
  }

  // check font alias with aspect in the name
  aFont = GetFont (aFontName, aFontAspect, aFontSize);
  if (!aFont.IsNull())
  {
    return aFont;
  }

  // Requested family name not found -> search for any font family with given aspect and height
  aFontName = new TCollection_HAsciiString ("");
  aFont = GetFont (aFontName, aFontAspect, aFontSize);
  if (!aFont.IsNull())
  {
    return aFont;
  }

  // The last resort: trying to use ANY font available in the system
  aFontAspect = Font_FA_Undefined;
  aFontSize = -1;
  aFont = GetFont (aFontName, aFontAspect, aFontSize);
  if (!aFont.IsNull())
  {
    return aFont;
  }

  return NULL; // Fonts are not found in the system.
}
