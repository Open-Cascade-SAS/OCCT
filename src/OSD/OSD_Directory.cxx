// Copyright (c) 1998-1999 Matra Datavision
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

#include <OSD_Directory.hxx>

#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_String.hxx>

#ifdef _WIN32
  #include <OSD_WNT.hxx>
  #include <stdio.h>

  #ifndef _INC_TCHAR
    #include <tchar.h>
  #endif

  void _osd_wnt_set_error (OSD_Error&, OSD_WhoAmI, ... );
#else
  #include <errno.h>
  #include <stdio.h>
  #include <sys/stat.h>
  #include <unistd.h>

  const OSD_WhoAmI Iam = OSD_WDirectory;
#endif

// =======================================================================
// function : OSD_Directory
// purpose  :
// =======================================================================
OSD_Directory::OSD_Directory()
{
  //
}

// =======================================================================
// function : OSD_Directory
// purpose  :
// =======================================================================
OSD_Directory::OSD_Directory (const OSD_Path& theName)
: OSD_FileNode (theName)
{
  //
}

// =======================================================================
// function : Build
// purpose  :
// =======================================================================
void OSD_Directory::Build (const OSD_Protection& theProtect)
{
#ifdef _WIN32
  TCollection_AsciiString aDirName;
  myPath.SystemName (aDirName);
  if (aDirName.IsEmpty())
  {
    throw Standard_ProgramError ( "OSD_Directory::Build(): incorrect call - no directory name");
  }

  Standard_Boolean isOK = Exists();
  if (!isOK)
  {
    // myError will be set to fail by Exists() if intermediate dirs do not exist
    myError.Reset();

    // create directory if it does not exist;
    TCollection_ExtendedString aDirNameW (aDirName);
    if (CreateDirectoryW (aDirNameW.ToWideString(), NULL))
    {
      isOK = Standard_True;
    }
    // if failed due to absence of intermediate directories, create them recursively
    else if (GetLastError() == ERROR_PATH_NOT_FOUND)
    {
      OSD_Path aSupPath = myPath;
      aSupPath.UpTrek();
      aSupPath.SetName (myPath.TrekValue (myPath.TrekLength())); // incredible, but required!
      OSD_Directory aSupDir (aSupPath);
      aSupDir.Build (theProtect);
      if (aSupDir.Failed())
      {
        myError = aSupDir.myError;
        return;
      }
      isOK = (CreateDirectoryW (aDirNameW.ToWideString(), NULL) != 0);
    }
  }

  if (isOK)
  {
#ifndef OCCT_UWP
    SetProtection (theProtect);
#else
    (void)theProtect;
#endif
  }
  else
  {
    _osd_wnt_set_error (myError, OSD_WDirectory);
  }
#else
  TCollection_AsciiString aBuffer;
  mode_t anInternalProt = (mode_t )theProtect.Internal();
  myPath.SystemName (aBuffer);
  umask (0);
  int aStatus = mkdir (aBuffer.ToCString(), anInternalProt);
  if (aStatus == -1 && errno == ENOENT)
  {
    OSD_Path aSupPath = myPath;
    aSupPath.UpTrek();
    aSupPath.SetName (myPath.TrekValue (myPath.TrekLength())); // incredible, but required!
    OSD_Directory aSupDir (aSupPath);
    aSupDir.Build (theProtect);
    if (aSupDir.Failed())
    {
      myError = aSupDir.myError;
      return;
    }
    aStatus = mkdir (aBuffer.ToCString(), anInternalProt);
  }
  if (aStatus == -1 && errno != EEXIST)
  {
    char anErrMsg[2048];
    Sprintf (anErrMsg, "OSD_Directory::Build Directory \"%.2000s\"", aBuffer.ToCString());
    myError.SetValue (errno, Iam, anErrMsg);
  }
#endif
}

// =======================================================================
// function : BuildTemporary
// purpose  :
// =======================================================================
OSD_Directory OSD_Directory::BuildTemporary()
{
#ifdef _WIN32
  wchar_t* aTmpNameW = _wtmpnam (NULL);
  if (aTmpNameW == NULL)
  {
    return OSD_Directory();
  }

  TCollection_AsciiString aTmpName (aTmpNameW);
  OSD_Path aDirPath (aTmpName);
  OSD_Directory aDir;
  aDir.SetPath (aDirPath);
  aDir.Build (OSD_Protection());
  return aDir;
#else
  // create a temporary directory with 0700 permissions
  char aTmpName[] = "/tmp/CSFXXXXXX";
  if (NULL == mkdtemp (aTmpName))
  {
    return OSD_Directory(); // can't create a directory
  }

  unlink (aTmpName); // destroys link but directory still exists while current process lives
  OSD_Directory aDir;
  aDir.SetPath (TCollection_AsciiString (aTmpName));
  return aDir;
#endif
}
