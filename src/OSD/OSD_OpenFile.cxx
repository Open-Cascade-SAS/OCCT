// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OSD_OpenFile.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_UtfString.hxx>

// ==============================================
// function : OSD_OpenFile
// purpose : Opens file
// ==============================================
FILE* OSD_OpenFile(const char* theName,
                   const char* theMode)
{
  FILE* aFile = 0;
#ifdef _WIN32
  // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  const TCollection_ExtendedString aFileModeW (theMode, Standard_True);
  aFile = ::_wfopen ((const wchar_t* )aFileNameW.ToExtString(),
                     (const wchar_t* )aFileModeW.ToExtString());
#else
  aFile = ::fopen (theName, theMode);
#endif
  return aFile;
}

// ==============================================
// function : OSD_OpenFile
// purpose : Opens file
// ==============================================
FILE* OSD_OpenFile(const TCollection_ExtendedString& theName,
                   const char* theMode)
{
  FILE* aFile = 0;
#ifdef _WIN32
  const TCollection_ExtendedString aFileModeW (theMode, Standard_True);
  aFile = ::_wfopen ((const wchar_t* )theName.ToExtString(),
                     (const wchar_t* )aFileModeW.ToExtString());
#else
  // conversion in UTF-8 for linux
  NCollection_Utf8String aString((const Standard_Utf16Char*)theName.ToExtString());
  aFile = ::fopen (aString.ToCString(),theMode);
#endif
  return aFile;
}

// ==============================================
// function : OSD_OpenFileBuf
// purpose : Opens file buffer
// ==============================================
void OSD_OpenFileBuf(std::filebuf& theBuff,
                     const char* theName,
                     const std::ios_base::openmode theMode)
{
#ifdef _WIN32
  // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  theBuff.open ((const wchar_t* )aFileNameW.ToExtString(), theMode);
#else
  theBuff.open (theName, theMode);
#endif
}

// ==============================================
// function : OSD_OpenFileBuf
// purpose : Opens file buffer
// ==============================================
void OSD_OpenFileBuf(std::filebuf& theBuff,
                     const TCollection_ExtendedString& theName,
                     const std::ios_base::openmode theMode)
{
#ifdef _WIN32
  theBuff.open ((const wchar_t* )theName.ToExtString(), theMode);
#else
  // conversion in UTF-8 for linux
  NCollection_Utf8String aString((const Standard_Utf16Char*)theName.ToExtString());
  theBuff.open (aString.ToCString(),theMode);
#endif
}

// ==============================================
// function : OSD_OpenStream
// purpose : Opens file stream
// ==============================================
void OSD_OpenStream(std::ofstream& theStream,
                    const char* theName,
                    const std::ios_base::openmode theMode)
{
#ifdef _WIN32
  // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  theStream.open ((const wchar_t* )aFileNameW.ToExtString(), theMode);
#else
  theStream.open (theName, theMode);
#endif
}

// ==============================================
// function : OSD_OpenStream
// purpose : Opens file stream
// ==============================================
void OSD_OpenStream(std::ofstream& theStream,
                    const TCollection_ExtendedString& theName,
                    const std::ios_base::openmode theMode)
{
#ifdef _WIN32
  theStream.open ((const wchar_t* )theName.ToExtString(), theMode);
#else
  // conversion in UTF-8 for linux
  NCollection_Utf8String aString((const Standard_Utf16Char*)theName.ToExtString());
  theStream.open (aString.ToCString(),theMode);
#endif
}

