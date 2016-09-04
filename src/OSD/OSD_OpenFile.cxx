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

#ifdef _WIN32
  #include <windows.h>
#endif

#include <OSD_OpenFile.hxx>

#include <sys/types.h>
#include <sys/stat.h>

// ==============================================
// function : OSD_OpenFile
// purpose : Opens file
// ==============================================
FILE* OSD_OpenFile(const char* theName,
                   const char* theMode)
{
  FILE* aFile = 0;
#if defined(_WIN32)
  // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  const TCollection_ExtendedString aFileModeW (theMode, Standard_True);
  aFile = ::_wfopen (aFileNameW.ToWideString(),
                     aFileModeW.ToWideString());
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
#if defined(_WIN32)
  const TCollection_ExtendedString aFileModeW (theMode, Standard_True);
  aFile = ::_wfopen (theName.ToWideString(),
                     aFileModeW.ToWideString());
#else
  // conversion in UTF-8 for linux
  NCollection_Utf8String aString (theName.ToExtString());
  aFile = ::fopen (aString.ToCString(),theMode);
#endif
  return aFile;
}

// ==============================================
// function : OSD_FileStatCTime
// purpose :
// ==============================================
Standard_Time OSD_FileStatCTime (const char* theName)
{
  Standard_Time aTime = 0;
#if defined(_WIN32)
  // file name is treated as UTF-8 string and converted to UTF-16 one
  const TCollection_ExtendedString aFileNameW (theName, Standard_True);
  struct __stat64 aStat;
  if (_wstat64 (aFileNameW.ToWideString(), &aStat) == 0)
  {
    aTime = (Standard_Time )aStat.st_ctime;
  }
#else
  struct stat aStat;
  if (stat (theName, &aStat) == 0)
  {
    aTime = (Standard_Time )aStat.st_ctime;
  }
#endif
  return aTime;
}
