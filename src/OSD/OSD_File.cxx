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

#ifdef _WIN32
  #include <windows.h>
#endif

#include <OSD_File.hxx>

#include <NCollection_Array1.hxx>
#include <OSD.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_ExtendedString.hxx>

#ifdef _WIN32

  #include <OSD_WNT.hxx>

  #include <stdio.h>
  #include <io.h>

  #include <Strsafe.h>

  #define ACE_HEADER_SIZE (sizeof(ACCESS_ALLOWED_ACE) - sizeof (DWORD))

  #define OPEN_NEW    0
  #define OPEN_OLD    1
  #define OPEN_APPEND 2

  void                            _osd_wnt_set_error        ( OSD_Error&, OSD_WhoAmI, ... );

#ifndef OCCT_UWP
  PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd ( const OSD_Protection&, BOOL, const wchar_t* );
  BOOL                 __fastcall _osd_wnt_sd_to_protection (PSECURITY_DESCRIPTOR, OSD_Protection&, BOOL);

  static int OSD_File_getBuffer (HANDLE theChannel,
                                 char* theBuffer,
                                 DWORD theSize,
                                 BOOL theIsPeek,
                                 BOOL theIsSocket)
  {
    if (theIsSocket)
    {
      const int aFlags = theIsPeek ? MSG_PEEK : 0;
      const int aRetVal = recv ((SOCKET )theChannel, theBuffer, (int )theSize, aFlags);
      return aRetVal != SOCKET_ERROR
           ? aRetVal
           : -1;
    }

    DWORD aBytesRead = 0;
    if (theIsPeek)
    {
      DWORD aDummy = 0;
      if (!PeekNamedPipe (theChannel, theBuffer, theSize, &aBytesRead, &aDummy, &aDummy)
        && GetLastError() != ERROR_BROKEN_PIPE)
      {
        return -1;
      }
      return (int )aBytesRead;
    }
    else if (!ReadFile (theChannel, theBuffer, theSize, &aBytesRead, NULL))
    {
      return -1;
    }
    return (int )aBytesRead;
  }

  static OSD_SingleProtection OSD_File_getProtection (DWORD theMask)
  {
    switch (theMask)
    {
      case FILE_GENERIC_READ:
        return OSD_R;
      case FILE_GENERIC_WRITE:
        return OSD_W;
      case FILE_GENERIC_READ | FILE_GENERIC_WRITE:
        return OSD_RW;
      case FILE_GENERIC_EXECUTE:
        return OSD_X;
      case FILE_GENERIC_READ | FILE_GENERIC_EXECUTE:
        return OSD_RX;
      case FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE:
        return OSD_WX;
      case FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE:
        return OSD_RWX;
      case DELETE:
        return OSD_D;
      case FILE_GENERIC_READ | DELETE:
        return OSD_RD;
      case FILE_GENERIC_WRITE | DELETE:
        return OSD_WD;
      case FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE:
        return OSD_RWD;
      case FILE_GENERIC_EXECUTE | DELETE:
        return OSD_XD;
      case FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | DELETE:
        return OSD_RXD;
      case FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE:
        return OSD_WXD;
      case FILE_ALL_ACCESS:
      case FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE:
        return OSD_RWXD;
    }
    return OSD_None;
  }

  static OSD_SingleProtection OSD_File_getProtectionDir (DWORD theMask)
  {
    switch (theMask)
    {
      case GENERIC_READ:
        return OSD_R;
      case GENERIC_WRITE:
        return OSD_W;
      case GENERIC_READ | GENERIC_WRITE:
        return OSD_RW;
      case GENERIC_EXECUTE:
        return OSD_X;
      case GENERIC_READ | GENERIC_EXECUTE:
        return OSD_RX;
      case GENERIC_WRITE | GENERIC_EXECUTE:
        return OSD_WX;
      case GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE:
        return OSD_RWX;
      case DELETE:
        return OSD_D;
      case GENERIC_READ | DELETE:
        return OSD_RD;
      case GENERIC_WRITE | DELETE:
        return OSD_WD;
      case GENERIC_READ | GENERIC_WRITE | DELETE:
        return OSD_RWD;
      case GENERIC_EXECUTE | DELETE:
        return OSD_XD;
      case GENERIC_READ | GENERIC_EXECUTE | DELETE:
        return OSD_RXD;
      case GENERIC_WRITE | GENERIC_EXECUTE | DELETE:
        return OSD_WXD;
      case FILE_ALL_ACCESS:
      case GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | DELETE:
        return OSD_RWXD;
      case 0:
        return OSD_None;
      default:
        // remote directories (on Samba server) have flags like for files
        return OSD_File_getProtection (theMask);
    }
  }

  static DWORD OSD_File_getAccessMask (OSD_SingleProtection theProtection)
  {
    switch (theProtection)
    {
      case OSD_None: return 0;
      case OSD_R:    return FILE_GENERIC_READ;
      case OSD_W:    return FILE_GENERIC_WRITE;
      case OSD_RW:   return FILE_GENERIC_READ | FILE_GENERIC_WRITE;
      case OSD_X:    return FILE_GENERIC_EXECUTE;
      case OSD_RX:   return FILE_GENERIC_READ | FILE_GENERIC_EXECUTE;
      case OSD_WX:   return FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE;
      case OSD_RWX:  return FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE;
      case OSD_D:    return DELETE;
      case OSD_RD:   return FILE_GENERIC_READ | DELETE;
      case OSD_WD:   return FILE_GENERIC_WRITE | DELETE;
      case OSD_RWD:  return FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE;
      case OSD_XD:   return FILE_GENERIC_EXECUTE | DELETE;
      case OSD_RXD:  return FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | DELETE;
      case OSD_WXD:  return FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE;
      case OSD_RWXD: return FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE;
    }
    throw Standard_ProgramError ("OSD_File_getAccessMask(): incorrect parameter");
  }

  static DWORD OSD_File_getDirAccessMask (OSD_SingleProtection theProtection)
  {
    switch (theProtection)
    {
      case OSD_None: return 0;
      case OSD_R:    return GENERIC_READ;
      case OSD_W:    return GENERIC_WRITE;
      case OSD_RW:   return GENERIC_READ | GENERIC_WRITE;
      case OSD_X:    return GENERIC_EXECUTE;
      case OSD_RX:   return GENERIC_READ  | GENERIC_EXECUTE;
      case OSD_WX:   return GENERIC_WRITE | GENERIC_EXECUTE;
      case OSD_RWX:  return GENERIC_READ  | GENERIC_WRITE | GENERIC_EXECUTE;
      case OSD_D:    return DELETE;
      case OSD_RD:   return GENERIC_READ  | DELETE;
      case OSD_WD:   return GENERIC_WRITE | DELETE;
      case OSD_RWD:  return GENERIC_READ  | GENERIC_WRITE | DELETE;
      case OSD_XD:   return GENERIC_EXECUTE | DELETE;
      case OSD_RXD:  return GENERIC_READ  | GENERIC_EXECUTE | DELETE;
      case OSD_WXD:  return GENERIC_WRITE | GENERIC_EXECUTE | DELETE;
      case OSD_RWXD: return GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | DELETE;
    }
    throw Standard_ProgramError ("OSD_File_getDirAccessMask(): incorrect parameter");
  }

  struct OSD_File_WntKey
  {
    HKEY           hKey;
    const wchar_t* keyPath;
  };

  #endif /* ! OCCT_UWP */

  Standard_Integer __fastcall _get_file_type (Standard_CString theFileName,
                                              HANDLE theFileHandle)
  {
    const int aFileType = theFileHandle == INVALID_HANDLE_VALUE
                        ? FILE_TYPE_DISK
                        : GetFileType (theFileHandle);
    switch (aFileType)
    {
      case FILE_TYPE_UNKNOWN:
        return FLAG_SOCKET;
      case FILE_TYPE_DISK:
      {
        const TCollection_ExtendedString aFileNameW (theFileName, Standard_True);
        WIN32_FILE_ATTRIBUTE_DATA aFileInfo;
        if (GetFileAttributesExW (aFileNameW.ToWideString(), GetFileExInfoStandard, &aFileInfo))
        {
          return aFileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? FLAG_DIRECTORY : FLAG_FILE;
        }
        return 0x80000000;
      }
      case FILE_TYPE_CHAR:
        return FLAG_DEVICE;
      case FILE_TYPE_PIPE:
        return FLAG_PIPE;
    }
    return 0;
  }

  //! Returns number of bytes in the string (including end \n, but excluding \r);
  static Standard_Integer OSD_File_getLine (char* theBuffer, DWORD theBuffSize, LONG& theSeekPos)
  {
    theBuffer[theBuffSize] = 0;
    for (char* aCharIter = theBuffer; *aCharIter != 0; )
    {
      if (*aCharIter == '\n')
      {
        ++aCharIter;   // jump newline char
        *aCharIter = '\0';
        theSeekPos = LONG(aCharIter - theBuffer - theBuffSize);
        return Standard_Integer(aCharIter - theBuffer);
      }
      else if (aCharIter[0] == '\r'
            && aCharIter[1] == '\n')
      {
        *(aCharIter++) = '\n'; // Substitute carriage return by newline
        *aCharIter = 0;
        theSeekPos = LONG(aCharIter + 1 - theBuffer - theBuffSize);
        return Standard_Integer(aCharIter - theBuffer);
      }
      else if (aCharIter[0] == '\r'
            && aCharIter[1] == '\0')
      {
        *aCharIter = '\n' ; // Substitute carriage return by newline
        return -1;
      }
      ++aCharIter;
    }

    theSeekPos = 0;
    return theBuffSize;
  }

  static HANDLE OSD_File_openFile (const TCollection_AsciiString& theFileName,
                                   OSD_OpenMode theOpenMode,
                                   DWORD theOptions, bool* theIsNew = NULL)
  {
    DWORD  dwDesiredAccess = 0;
    switch (theOpenMode)
    {
      case OSD_ReadOnly:
        dwDesiredAccess = GENERIC_READ;
        break;
      case OSD_WriteOnly:
        dwDesiredAccess = GENERIC_WRITE;
        break;
      case OSD_ReadWrite:
        dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
        break;
      default:
        throw Standard_ProgramError ("OSD_File_openFile(): incorrect parameter");
    }

    DWORD dwCreationDistribution = (theOptions != OPEN_NEW) ? OPEN_EXISTING : CREATE_ALWAYS;
    const TCollection_ExtendedString aFileNameW (theFileName);
  #ifndef OCCT_UWP
    HANDLE aFileHandle = CreateFileW (aFileNameW.ToWideString(), dwDesiredAccess,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL, dwCreationDistribution, FILE_ATTRIBUTE_NORMAL, NULL);
  #else
    CREATEFILE2_EXTENDED_PARAMETERS pCreateExParams = {};
    pCreateExParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    pCreateExParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    pCreateExParams.lpSecurityAttributes = NULL;
    pCreateExParams.hTemplateFile = NULL;
    HANDLE aFileHandle = CreateFile2 (aFileNameW.ToWideString(), dwDesiredAccess,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      dwCreationDistribution, &pCreateExParams);
  #endif
    if (aFileHandle    != INVALID_HANDLE_VALUE
     || theOptions     != OPEN_APPEND
     || GetLastError() != ERROR_FILE_NOT_FOUND)
    {
      return aFileHandle;
    }

    dwCreationDistribution = CREATE_ALWAYS;
  #ifndef OCCT_UWP
    aFileHandle = CreateFileW (aFileNameW.ToWideString(), dwDesiredAccess,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, dwCreationDistribution, FILE_ATTRIBUTE_NORMAL, NULL );
  #else
    CREATEFILE2_EXTENDED_PARAMETERS pCreateExParams2 = {};
    pCreateExParams2.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    pCreateExParams2.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    pCreateExParams2.lpSecurityAttributes = NULL;
    pCreateExParams2.hTemplateFile = NULL;
    aFileHandle = CreateFile2 (aFileNameW.ToWideString(), dwDesiredAccess,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               dwCreationDistribution, &pCreateExParams2 );
  #endif
    *theIsNew = true;
    return aFileHandle;
  }

#else

  const OSD_WhoAmI Iam = OSD_WFile;

  #if defined (sun) || defined(SOLARIS)
    #define POSIX
  #else
    #define SYSV
  #endif

  #include <errno.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/stat.h>

  #define NEWLINE '\10';
#endif

// =======================================================================
// function : OSD_File
// purpose  :
// =======================================================================
OSD_File::OSD_File() :
#ifdef _WIN32
  myFileHandle (INVALID_HANDLE_VALUE),
#else
  myFileChannel (-1),
#endif
  myFILE (NULL),
  myIO (0),
  myLock (OSD_NoLock),
  myMode (OSD_ReadWrite),
  ImperativeFlag (Standard_False)
{
  //
}

// =======================================================================
// function : OSD_File
// purpose  :
// =======================================================================
OSD_File::OSD_File (const OSD_Path& theName)
: OSD_FileNode (theName),
#ifdef _WIN32
  myFileHandle (INVALID_HANDLE_VALUE),
#else
  myFileChannel (-1),
#endif
  myFILE (NULL),
  myIO (0),
  myLock (OSD_NoLock),
  myMode (OSD_ReadWrite),
  ImperativeFlag (Standard_False)
{
  //
}

// =======================================================================
// function : ~OSD_File
// purpose  :
// =======================================================================
OSD_File::~OSD_File()
{
  if (IsOpen())
  {
    if (IsLocked())
    {
      UnLock();
    }
    Close();
  }
}

// =======================================================================
// function : Build
// purpose  :
// =======================================================================
void OSD_File::Build (const OSD_OpenMode theMode,
                      const OSD_Protection& theProtect)
{
  if (OSD_File::KindOfFile() == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::Build(): it is a directory");
  }
  if (IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Build(): incorrect call - file already opened");
  }

  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
#ifdef _WIN32
  if (aFileName.IsEmpty())
  {
    throw Standard_ProgramError ("OSD_File::Build(): incorrect call - no filename given");
  }

  myMode = theMode;
  myFileHandle = OSD_File_openFile (aFileName, theMode, OPEN_NEW);
  if (myFileHandle == INVALID_HANDLE_VALUE)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
  else
  {
  #ifndef OCCT_UWP
    SetProtection (theProtect);
  #else
    (void)theProtect;
  #endif
    myIO |= FLAG_FILE;
  }
#else
  if (myPath.Name().Length() == 0)
  {
    throw Standard_ProgramError ("OSD_File::Build(): no name was given");
  }

  const char* anFDOpenMode = "r";
  Standard_Integer anOpenMode = O_CREAT | O_TRUNC;
  switch (theMode)
  {
    case OSD_ReadOnly:
      anOpenMode |= O_RDONLY;
      anFDOpenMode = "r";
      break;
    case OSD_WriteOnly:
      anOpenMode |= O_WRONLY;
      anFDOpenMode = "w";
      break;
    case OSD_ReadWrite:
      anOpenMode |= O_RDWR;
      anFDOpenMode = "w+";
      break;
  }

  myMode = theMode;
  myFileChannel = open (aFileName.ToCString(), anOpenMode, theProtect.Internal());
  if (myFileChannel >= 0)
  {
    myFILE = fdopen (myFileChannel, anFDOpenMode);
  }
  else
  {
    myError.SetValue (errno, Iam, "Open");
  }
#endif
}

// =======================================================================
// function : Append
// purpose  :
// =======================================================================
void OSD_File::Append (const OSD_OpenMode theMode,
                       const OSD_Protection& theProtect)
{
  if (OSD_File::KindOfFile() == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::Append(): it is a directory");
  }
  if (IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Append(): incorrect call - file already opened");
  }

  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
#ifdef _WIN32
  if (aFileName.IsEmpty())
  {
    throw Standard_ProgramError ("OSD_File::Append(): incorrect call - no filename given");
  }

  bool isNewFile = false;
  myMode = theMode;
  myFileHandle = OSD_File_openFile (aFileName, theMode, OPEN_APPEND, &isNewFile);
  if (myFileHandle == INVALID_HANDLE_VALUE)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
  else
  {
    if (!isNewFile)
    {
      myIO |= _get_file_type (aFileName.ToCString(), myFileHandle);
      Seek (0, OSD_FromEnd);
    }
    else
    {
    #ifndef OCCT_UWP
      SetProtection (theProtect);
    #else
      (void)theProtect;
    #endif
      myIO |= FLAG_FILE;
    }
  }
#else
  if (myPath.Name().Length() == 0)
  {
    throw Standard_ProgramError ("OSD_File::Append(): no name was given");
  }

  const char* anFDOpenMode = "r";
  Standard_Integer anOpenMode = O_APPEND;
  switch (theMode)
  {
    case OSD_ReadOnly:
      anOpenMode |= O_RDONLY;
      anFDOpenMode = "r";
      break;
    case OSD_WriteOnly:
      anOpenMode |= O_WRONLY;
      anFDOpenMode = "a";
      break;
    case OSD_ReadWrite:
      anOpenMode |= O_RDWR;
      anFDOpenMode = "a+";
      break;
  }

  if (!Exists())
  {
    // if file doesn't exist, creates it
    anOpenMode |= O_CREAT;
  }

  myMode = theMode;
  myFileChannel = open (aFileName.ToCString(), anOpenMode, theProtect.Internal());
  if (myFileChannel >= 0)
  {
    myFILE = fdopen (myFileChannel, anFDOpenMode);
  }
  else
  {
    myError.SetValue (errno, Iam, "Open");
  }
#endif
}

// =======================================================================
// function : Open
// purpose  :
// =======================================================================
void OSD_File::Open (const OSD_OpenMode theMode,
                     const OSD_Protection& theProtect)
{
  if (OSD_File::KindOfFile() == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::Open(): it is a directory");
  }
  if (IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Open(): incorrect call - file already opened");
  }

  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
#ifdef _WIN32
  if (aFileName.IsEmpty())
  {
    throw Standard_ProgramError ("OSD_File::Open(): incorrect call - no filename given");
  }

  (void )theProtect;
  myMode = theMode;
  myFileHandle = OSD_File_openFile (aFileName, theMode, OPEN_OLD);
  if (myFileHandle == INVALID_HANDLE_VALUE)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
  else
  {
    myIO |= _get_file_type (aFileName.ToCString(), myFileHandle);
  }
#else
  if (myPath.Name().Length() == 0)
  {
    throw Standard_ProgramError ("OSD_File::Open(): no name was given");
  }

  const char* anFDOpenMode = "r";
  Standard_Integer anOpenMode = 0;
  switch (theMode)
  {
    case OSD_ReadOnly:
      anOpenMode |= O_RDONLY;
      anFDOpenMode = "r";
      break;
    case OSD_WriteOnly:
      anOpenMode |= O_WRONLY;
      anFDOpenMode = "w";
      break;
    case OSD_ReadWrite:
      anOpenMode |= O_RDWR;
      anFDOpenMode = "w+";
      break;
  }

  myMode = theMode;
  myFileChannel = open (aFileName.ToCString(), anOpenMode, theProtect.Internal());
  if (myFileChannel >= 0)
  {
    myFILE = fdopen (myFileChannel, anFDOpenMode);
  }
  else
  {
    myError.SetValue (errno, Iam, "Open");
  }
#endif
}

// =======================================================================
// function : BuildTemporary
// purpose  :
// =======================================================================
void OSD_File::BuildTemporary()
{
#ifdef _WIN32

  TCollection_ExtendedString aTmpFolderW;
  BOOL fOK = FALSE;
#ifndef OCCT_UWP
  const OSD_File_WntKey TheRegKeys[2] =
  {
    { HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" },
    { HKEY_USERS,         L".DEFAULT\\Environment" }
  };
  for (int aKeyIter = 0; aKeyIter < 2; ++aKeyIter)
  {
    HKEY aRegKey = NULL;
    if (RegOpenKeyExW (TheRegKeys[aKeyIter].hKey, TheRegKeys[aKeyIter].keyPath, 0, KEY_QUERY_VALUE, &aRegKey) != ERROR_SUCCESS)
    {
      continue;
    }

    DWORD aKeyType = 0, aKeySize = 0;
    if (RegQueryValueExW (aRegKey, L"TEMP", NULL, &aKeyType, NULL, &aKeySize) == ERROR_SUCCESS)
    {
      NCollection_Array1<wchar_t> aKeyValW (0, aKeySize);
      RegQueryValueExW (aRegKey, L"TEMP", NULL, &aKeyType, (LPBYTE )&aKeyValW.ChangeFirst(), &aKeySize);
      if (aKeyType == REG_EXPAND_SZ)
      {
        wchar_t aTmpBuffer[MAX_PATH];
        ExpandEnvironmentStringsW (&aKeyValW.First(), aTmpBuffer, MAX_PATH);
        aTmpFolderW = TCollection_ExtendedString (aTmpBuffer);
      }
      else
      {
        aTmpFolderW = TCollection_ExtendedString (&aKeyValW.First());
      }
      fOK = TRUE;
    }
    RegCloseKey (aRegKey);
    if (fOK) break;
  }
#else
  // Windows Registry not supported by UWP
  {
    wchar_t aTmpBuffer[MAX_PATH];
    fOK = GetTempPathW (_countof(aTmpBuffer), aTmpBuffer) != 0;
    aTmpFolderW = TCollection_ExtendedString (aTmpBuffer);
  }
#endif
  if (!fOK)
  {
    aTmpFolderW = "./";
  }

  wchar_t aTmpPathW[MAX_PATH];
  GetTempFileNameW (aTmpFolderW.ToWideString(), L"CSF", 0, aTmpPathW);
  if (IsOpen())
  {
    Close();
  }

  SetPath (OSD_Path (TCollection_AsciiString (aTmpPathW)));
  Build (OSD_ReadWrite, OSD_Protection());

#else /* _WIN32 */

  if (IsOpen())
  {
    Close();
  }
#if defined(vax) || defined(__vms) || defined(VAXVMS)
  FILE* fic = tmpfile();
  int dummy = open("dummy", O_RDWR | O_CREAT); // open a dummy file
  myFileChannel = dummy - 1;                   // this is file channel of "fic" +1
  close (dummy);                               // close dummy file
  unlink ("dummy");                            // removes dummy file
#else
  char aTmpName[] = "/tmp/CSFXXXXXX";
  myFileChannel = mkstemp (aTmpName);
  const TCollection_AsciiString aName (aTmpName);
  const OSD_Path aPath (aName);
  SetPath (aPath);
  myFILE = fdopen (myFileChannel, "w+");
#endif
  myMode = OSD_ReadWrite;

#endif
}

// =======================================================================
// function : Read
// purpose  :
// =======================================================================
void OSD_File::Read (TCollection_AsciiString& theBuffer,
                     const Standard_Integer theNbBytes)
{
  if (OSD_File::KindOfFile() == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::Read(): it is a directory");
  }
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Read(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }
  if (myMode == OSD_WriteOnly)
  {
    throw Standard_ProgramError ("OSD_File::Read(): file is Write only");
  }
  if (theNbBytes <= 0)
  {
    throw Standard_ProgramError ("OSD_File::Read(): theNbBytes is 0");
  }

  NCollection_Array1<char> aBuffer (0, theNbBytes);
  Standard_Integer aNbBytesRead = 0;
#ifdef _WIN32
  Read (&aBuffer.ChangeFirst(), theNbBytes, aNbBytesRead);
#else
  aNbBytesRead = read (myFileChannel, &aBuffer.ChangeFirst(), theNbBytes);
  if (aNbBytesRead == -1)
  {
    aNbBytesRead = 0;
    myError.SetValue (errno, Iam, "Read");
  }
  else if (aNbBytesRead < theNbBytes)
  {
    myIO = EOF;
  }
#endif
  if (aNbBytesRead != 0)
  {
    aBuffer.ChangeValue (aNbBytesRead) = '\0';
    theBuffer = &aBuffer.First();
  }
  else
  {
    theBuffer.Clear();
  }
}

// =======================================================================
// function : ReadLine
// purpose  :
// =======================================================================
void OSD_File::ReadLine (TCollection_AsciiString& theBuffer,
                         const Standard_Integer theNbBytes,
                         Standard_Integer& theNbBytesRead)
{
  if (OSD_File::KindOfFile() == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): it is a directory");
  }
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }
  if (myMode == OSD_WriteOnly)
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): file is Write only");
  }
  if (theNbBytes <= 0)
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): theNbBytes is 0");
  }
#ifdef _WIN32
  if (myIO & FLAG_PIPE && !(myIO & FLAG_READ_PIPE))
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): attempt to read from write only pipe");
  }

  DWORD aNbBytesRead = 0;
  LONG  aSeekPos = 0;
  char  aPeekChar = '\0';
  // +----> leave space for end-of-string
  // |       plus <CR><LF> sequence
  // |
  NCollection_Array1<char> aBuffer (0, theNbBytes + 2);
  if (myIO & FLAG_FILE)
  {
    if (!ReadFile (myFileHandle, &aBuffer.ChangeFirst(), theNbBytes, &aNbBytesRead, NULL))
    {
      _osd_wnt_set_error (myError, OSD_WFile);
      theBuffer.Clear();
      theNbBytesRead = 0;
    }
    else if (aNbBytesRead == 0)
    {
      theBuffer.Clear();
      theNbBytesRead = 0;
      myIO |= FLAG_EOF;
    }
    else
    {
      myIO &= ~FLAG_EOF;  // if the file increased since last read (LD)
      theNbBytesRead = OSD_File_getLine (&aBuffer.ChangeFirst(), aNbBytesRead, aSeekPos);
      if (theNbBytesRead == -1) // last character in the buffer is <CR> -
      {                         // peek next character to see if it is a <LF>
        DWORD dwDummy = 0;
        if (!ReadFile (myFileHandle, &aPeekChar, 1, &dwDummy, NULL))
        {
          _osd_wnt_set_error (myError, OSD_WFile);
        }
        else if (dwDummy != 0) // end-of-file reached?
        {
          if (aPeekChar != '\n')  // if we did not get a <CR><LF> sequence
          {
            // adjust file position
            LARGE_INTEGER aDistanceToMove;
            aDistanceToMove.QuadPart = -1;
            SetFilePointerEx (myFileHandle, aDistanceToMove, NULL, FILE_CURRENT);
          }
        }
        else
        {
          myIO |= FLAG_EOF;
        }

        theNbBytesRead = aNbBytesRead;
      }
      else if (aSeekPos != 0)
      {
        LARGE_INTEGER aDistanceToMove;
        aDistanceToMove.QuadPart = aSeekPos;
        SetFilePointerEx (myFileHandle, aDistanceToMove, NULL, FILE_CURRENT);
      }
    }
  }
  else if (myIO & FLAG_SOCKET
        || myIO & FLAG_PIPE
        || myIO & FLAG_NAMED_PIPE)
  {
  #ifndef OCCT_UWP
    aNbBytesRead = (DWORD )OSD_File_getBuffer (myFileHandle, &aBuffer.ChangeFirst(), (DWORD )theNbBytes, TRUE, myIO & FLAG_SOCKET);
    if ((int )aNbBytesRead == -1)
    {
      _osd_wnt_set_error (myError, OSD_WFile);
      theBuffer.Clear();
      theNbBytesRead = 0;
    }
    else if (aNbBytesRead == 0) // connection closed - set end-of-file flag
    {
      theBuffer.Clear();
      theNbBytesRead = 0;
      myIO |= FLAG_EOF;
    }
    else
    {
      theNbBytesRead = OSD_File_getLine (&aBuffer.ChangeFirst(), aNbBytesRead, aSeekPos);
      if (theNbBytesRead == -1)  // last character in the buffer is <CR> - peek next character to see if it is a <LF>
      {
        theNbBytesRead = aNbBytesRead; // (LD) always fits this case

        const DWORD dwDummy = OSD_File_getBuffer (myFileHandle, &aPeekChar, 1, TRUE, myIO & FLAG_SOCKET);
        if ((int )dwDummy == -1)
        {
          _osd_wnt_set_error (myError, OSD_WFile);
        }
        else if (dwDummy != 0) // connection closed?
        {
          if (aPeekChar == '\n') // we got a <CR><LF> sequence
          {
            ++aNbBytesRead;  // (LD) we have to jump <LF>
          }
        }
        else
        {
          myIO |= FLAG_EOF;
        }
      }
      else if (aSeekPos != 0)
      {
        aNbBytesRead = aNbBytesRead + aSeekPos;
      }

      // do not rewrite data in aBuffer
      NCollection_Array1<char> aBuffer2 (0, theNbBytes + 2);
      // remove pending input
      OSD_File_getBuffer (myFileHandle, &aBuffer2.ChangeFirst(), aNbBytesRead, FALSE, myIO & FLAG_SOCKET);
    }
  #endif
  }
  else
  {
    throw Standard_ProgramError ("OSD_File::ReadLine(): incorrect call - file is a directory");
  }

  if (!Failed() && !IsAtEnd())
  {
    theBuffer = &aBuffer.First();
  }
#else
  NCollection_Array1<char> aBuffer (0, theNbBytes);
  char* aBufferGets = fgets (&aBuffer.ChangeFirst(), theNbBytes, (FILE* )myFILE);
  if (aBufferGets == NULL)
  {
    if (!feof ((FILE* )myFILE))
    {
      myError.SetValue (errno, Iam, "ReadLine");
      return;
    }

    myIO = EOF;
    theBuffer.Clear();
    theNbBytesRead = 0;
  }
  else
  {
    aBuffer.ChangeLast() = '\0';
    theNbBytesRead = (Standard_Integer )strlen (aBufferGets);
    theBuffer.SetValue (1, aBufferGets);
    theBuffer.Trunc (theNbBytesRead);
  }
#endif
}

// =======================================================================
// function : KindOfFile
// purpose  :
// =======================================================================
OSD_KindFile OSD_File::KindOfFile() const
{
  TCollection_AsciiString aFullName;
  myPath.SystemName (aFullName);
#ifdef _WIN32
  Standard_Integer aFlags = myIO;
  if (myFileHandle == INVALID_HANDLE_VALUE)
  {
    if (aFullName.IsEmpty())
    {
      throw Standard_ProgramError ("OSD_File::KindOfFile(): incorrect call - no filename given");
    }
    aFlags = _get_file_type (aFullName.ToCString(), INVALID_HANDLE_VALUE);
  }

  switch (aFlags & FLAG_TYPE)
  {
    case FLAG_FILE:      return OSD_FILE;
    case FLAG_DIRECTORY: return OSD_DIRECTORY;
    case FLAG_SOCKET:    return OSD_SOCKET;
  }
  return OSD_UNKNOWN;
#else
  struct stat aStatBuffer;
  if (stat (aFullName.ToCString(), &aStatBuffer) == 0)
  {
    if      (S_ISDIR (aStatBuffer.st_mode)) { return OSD_DIRECTORY; }
    else if (S_ISREG (aStatBuffer.st_mode)) { return OSD_FILE; }
    else if (S_ISLNK (aStatBuffer.st_mode)) { return OSD_LINK; }
    else if (S_ISSOCK(aStatBuffer.st_mode)) { return OSD_SOCKET; }
  }
  return OSD_UNKNOWN;
#endif
}

// =======================================================================
// function : Read
// purpose  :
// =======================================================================
void OSD_File::Read (const Standard_Address  theBuffer,
                     const Standard_Integer  theNbBytes,
                           Standard_Integer& theNbReadBytes)
{
  if (OSD_File::KindOfFile ( ) == OSD_DIRECTORY)
  {
    throw Standard_ProgramError ("OSD_File::Read(): it is a directory");
  }
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Read(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }
  if (myMode == OSD_WriteOnly)
  {
    throw Standard_ProgramError ("OSD_File::Read(): file is Write only");
  }
  if (theNbBytes <= 0)
  {
    throw Standard_ProgramError ("OSD_File::Read(): theNbBytes is 0");
  }
  if (theBuffer == NULL)
  {
    throw Standard_ProgramError ("OSD_File::Read(): theBuffer is NULL");
  }
#ifdef _WIN32
  if (myIO & FLAG_PIPE && !(myIO & FLAG_READ_PIPE))
  {
    throw Standard_ProgramError ("OSD_File::Read(): attempt to read from write only pipe");
  }

  DWORD aNbReadBytes = 0;
  if (!ReadFile (myFileHandle, theBuffer, (DWORD )theNbBytes, &aNbReadBytes, NULL))
  {
    _osd_wnt_set_error (myError, OSD_WFile);
    aNbReadBytes = 0;
  }
  else if (aNbReadBytes == 0)
  {
    myIO |= FLAG_EOF;
  }
  else
  {
    myIO &= ~FLAG_EOF;
  }

  theNbReadBytes = (Standard_Integer )aNbReadBytes;
#else
  theNbReadBytes = 0;
  int aNbReadBytes = read (myFileChannel, (char* )theBuffer, theNbBytes);
  if (aNbReadBytes == -1)
  {
    myError.SetValue (errno, Iam, "Read");
  }
  else
  {
    if (aNbReadBytes < theNbBytes)
    {
      myIO = EOF;
    }
    theNbReadBytes = aNbReadBytes;
  }
#endif
}

// =======================================================================
// function : Write
// purpose  :
// =======================================================================
void OSD_File::Write (const Standard_Address theBuffer,
                      const Standard_Integer theNbBytes)
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Write(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }
  if (myMode == OSD_ReadOnly)
  {
    throw Standard_ProgramError ("OSD_File::Write(): file is Read only");
  }
  if (theNbBytes <= 0)
  {
    throw Standard_ProgramError ("OSD_File::Write(): theNbBytes is null");
  }
#ifdef _WIN32
  if ((myIO & FLAG_PIPE) != 0
   && (myIO & FLAG_READ_PIPE) != 0)
  {
    throw Standard_ProgramError ("OSD_File::Write(): attempt to write to read only pipe");
  }

  DWORD aNbWritten = 0;
  if (!WriteFile (myFileHandle, theBuffer, (DWORD )theNbBytes, &aNbWritten, NULL)
  ||  aNbWritten != (DWORD )theNbBytes)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
#else
  const int aNbWritten = write (myFileChannel, (const char* )theBuffer, theNbBytes);
  if (aNbWritten == -1)
  {
    myError.SetValue (errno, Iam, "Write");
  }
  else if (aNbWritten < theNbBytes)
  {
    myIO = EOF;
  }
#endif
}

// =======================================================================
// function : Seek
// purpose  :
// =======================================================================
void OSD_File::Seek (const Standard_Integer theOffset,
                     const OSD_FromWhere theWhence)
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Seek(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }

#ifdef _WIN32
  DWORD aWhere = 0;
  if (myIO & FLAG_FILE
   || myIO & FLAG_DIRECTORY)
  {
    switch (theWhence)
    {
      case OSD_FromBeginning: aWhere = FILE_BEGIN;   break;
      case OSD_FromHere:      aWhere = FILE_CURRENT; break;
      case OSD_FromEnd:       aWhere = FILE_END;     break;
      default:
        throw Standard_ProgramError ("OSD_File::Seek(): invalid parameter");
    }

    LARGE_INTEGER aDistanceToMove, aNewFilePointer;
    aNewFilePointer.QuadPart = 0;
    aDistanceToMove.QuadPart = theOffset;
    if (!SetFilePointerEx (myFileHandle, aDistanceToMove, &aNewFilePointer, aWhere))
    {
      _osd_wnt_set_error (myError, OSD_WFile);
    }
  }
  myIO &= ~FLAG_EOF;
#else
  int aWhere = 0;
  switch (theWhence)
  {
    case OSD_FromBeginning: aWhere = SEEK_SET; break;
    case OSD_FromHere:      aWhere = SEEK_CUR; break;
    case OSD_FromEnd:       aWhere = SEEK_END; break;
    default:
      throw Standard_ProgramError ("OSD_File::Seek(): invalid parameter");
  }

  off_t aStatus = lseek (myFileChannel, theOffset, aWhere);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "Seek");
  }
#endif
}

// =======================================================================
// function : Close
// purpose  :
// =======================================================================
void OSD_File::Close()
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Close(): file is not open");
  }
  if (Failed())
  {
    Perror();
  }
#ifdef _WIN32
  CloseHandle (myFileHandle);
  myFileHandle = INVALID_HANDLE_VALUE;
#else
  // note: it probably should be single call to fclose()...
  int status = close (myFileChannel);
  if (status == -1)
  {
    myError.SetValue (errno, Iam, "Close");
  }
  myFileChannel = -1;
  if (myFILE != NULL)
  {
    status = fclose ((FILE* )myFILE);
    myFILE = NULL;
  }
#endif
  myIO = 0;
}

// =======================================================================
// function : IsAtEnd
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::IsAtEnd()
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::IsAtEnd(): file is not open");
  }

#ifdef _WIN32
  return (myIO & FLAG_EOF) != 0;
#else
  return myIO == EOF;
#endif
}

// =======================================================================
// function : Link
// purpose  :
// =======================================================================
/*void OSD_File::Link (const TCollection_AsciiString& theToFile)
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Link(): file is not open");
  }

  TCollection_AsciiString aFilePath;
  myPath.SystemName (aFilePath);
  link (aFilePath.ToCString(), theToFile.ToCString());
}*/

#if defined(__CYGWIN32__) || defined(__MINGW32__)
  #ifdef __try /* is defined on MinGw as either "try" or "if (true)" */
  #undef __try
  #endif
  #define __try
  #define __finally
  #define __leave return
#endif

// =======================================================================
// function : SetLock
// purpose  :
// =======================================================================
void OSD_File::SetLock (const OSD_LockType theLock)
{
  if (!IsOpen())
  {
    throw Standard_ProgramError("OSD_File::SetLock(): file is not open");
  }
#ifdef _WIN32
  DWORD dwFlags = 0;
  myLock = theLock;
  if (theLock == OSD_NoLock)
  {
    UnLock();
    return;
  }
  else if (theLock == OSD_ReadLock
        || theLock == OSD_ExclusiveLock)
  {
    dwFlags = LOCKFILE_EXCLUSIVE_LOCK;
  }

  OVERLAPPED anOverlapped;
  ZeroMemory (&anOverlapped, sizeof(OVERLAPPED));
  __try
  {
    LARGE_INTEGER aSize;
    aSize.QuadPart = Size();
    if (!LockFileEx (myFileHandle, dwFlags, 0, aSize.LowPart, aSize.HighPart, &anOverlapped))
    {
      _osd_wnt_set_error (myError, OSD_WFile);
      __leave;
    }
    ImperativeFlag = Standard_True;
  }
  __finally {}

#elif defined(POSIX)
  int aLock = 0;
  switch (theLock)
  {
    case OSD_ExclusiveLock:
    case OSD_WriteLock:
      aLock = F_LOCK;
      break;
    case OSD_ReadLock:
      return;
    default:
      myError.SetValue (EINVAL, Iam, "SetLock");
      return;
  }

  struct stat aStatBuf;
  if (fstat (myFileChannel, &aStatBuf) == -1)
  {
    myError.SetValue (errno, Iam, "SetLock");
    return;
  }

  const int aStatus = lockf (myFileChannel, aLock, aStatBuf.st_size);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "SetLock");
  }
  else
  {
    myLock = theLock;
  }
#elif defined(SYSV)
  struct flock aLockKey;
  aLockKey.l_whence = 0;
  aLockKey.l_start = 0;
  aLockKey.l_len = 0;
  switch (theLock)
  {
    case OSD_ExclusiveLock:
    case OSD_WriteLock:
      aLockKey.l_type = F_WRLCK;
      break;
    case OSD_ReadLock:
      aLockKey.l_type = F_RDLCK;
      break;
    case OSD_NoLock:
      return;
    //default: myError.SetValue (EINVAL, Iam, "SetLock");
  }

  const int aStatus = fcntl (myFileChannel, F_SETLKW, &aLockKey);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "SetLock");
  }
  else
  {
    myLock = theLock;
  }

  if (theLock == OSD_ExclusiveLock)
  {
    struct stat aStatBuf;
    fstat (myFileChannel, &aStatBuf);
    TCollection_AsciiString aFilePath;
    myPath.SystemName (aFilePath);
    chmod (aFilePath.ToCString(), aStatBuf.st_mode | S_ISGID);
    ImperativeFlag = Standard_True;
  }
#else   /* BSD */
  int aLock = 0;
  switch (theLock)
  {
    case OSD_ExclusiveLock:
    case OSD_WriteLock:
      aLock = F_WRLCK;
      break;
    case OSD_ReadLock:
      aLock = F_RDLCK;
      break;
    default:
      myError.SetValue (EINVAL, Iam, "SetLock");
      return;
  }

  const int aStatus = flock (myFileChannel, aLock);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "SetLock");
  }
  else
  {
    myLock = theLock;
  }
#endif
}

#if defined(__CYGWIN32__) || defined(__MINGW32__)
  #undef __try
  #undef __finally
  #undef __leave
#endif

// =======================================================================
// function : UnLock
// purpose  :
// =======================================================================
void OSD_File::UnLock()
{
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::UnLock(): file is not open");
  }
#ifdef _WIN32
  if (ImperativeFlag)
  {
    LARGE_INTEGER aSize;
    aSize.QuadPart = Size();

    OVERLAPPED anOverlappedArea;
    anOverlappedArea.Offset = 0;
    anOverlappedArea.OffsetHigh = 0;
    if (!UnlockFileEx (myFileHandle, 0, aSize.LowPart, aSize.HighPart, &anOverlappedArea))
    {
      _osd_wnt_set_error (myError, OSD_WFile);
    }
    ImperativeFlag = Standard_False;
  }
#elif defined(POSIX)
  struct stat aStatBuf;
  if (fstat (myFileChannel, &aStatBuf) == -1)
  {
    myError.SetValue (errno, Iam, "UnsetLock");
    return;
  }

  const int aStatus = lockf (myFileChannel, F_ULOCK, aStatBuf.st_size);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "SetLock");
  }
  else
  {
    myLock = OSD_NoLock;
  }
#elif defined(SYSV)
  if (ImperativeFlag)
  {
    struct stat aStatBuf;
    fstat (myFileChannel, &aStatBuf);
    TCollection_AsciiString aBuffer;
    myPath.SystemName (aBuffer);
    chmod (aBuffer.ToCString(), aStatBuf.st_mode & ~S_ISGID);
    ImperativeFlag = Standard_False;
  }

  struct flock aLockKey;
  aLockKey.l_type = F_UNLCK;
  const int aStatus = fcntl (myFileChannel, F_SETLK, &aLockKey);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "UnSetLock");
  }
  else
  {
    myLock = OSD_NoLock;
  }
#else
  const int aStatus = flock (myFileChannel, LOCK_UN);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "UnSetLock");
  }
  else
  {
    myLock = OSD_NoLock;
  }
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
Standard_Size OSD_File::Size()
{
#ifdef _WIN32
  if (!IsOpen())
  {
    throw Standard_ProgramError ("OSD_File::Size(): file is not open");
  }
#if (_WIN32_WINNT >= 0x0500)
  LARGE_INTEGER aSize;
  aSize.QuadPart = 0;
  if (GetFileSizeEx (myFileHandle, &aSize) == 0)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
  return (Standard_Size )aSize.QuadPart;
#else
  DWORD aSize = GetFileSize (myFileHandle, NULL);
  if (aSize == INVALID_FILE_SIZE)
  {
    _osd_wnt_set_error (myError, OSD_WFile);
  }
  return aSize;
#endif
#else
  if (myPath.Name().Length() == 0)
  {
    throw Standard_ProgramError ("OSD_File::Size(): empty file name");
  }

  TCollection_AsciiString aFilePath;
  myPath.SystemName (aFilePath);

  struct stat aStatBuf;
  const int aStatus = stat (aFilePath.ToCString(), &aStatBuf);
  if (aStatus == -1)
  {
    myError.SetValue (errno, Iam, "Size");
    return 0;
  }
  return (Standard_Size )aStatBuf.st_size;
#endif
}

// =======================================================================
// function : IsOpen
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::IsOpen() const
{
#ifdef _WIN32
  return myFileHandle != INVALID_HANDLE_VALUE;
#else
  return myFileChannel != -1;
#endif
}

// =======================================================================
// function : IsReadable
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::IsReadable()
{
  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
#ifdef _WIN32
  HANDLE aChannel = OSD_File_openFile (aFileName, OSD_ReadOnly, OPEN_OLD);
  if (aChannel == INVALID_HANDLE_VALUE)
  {
    return Standard_False;
  }

  CloseHandle (aChannel);
  return Standard_True;
#else
  return access (aFileName.ToCString(), F_OK | R_OK) == 0;
#endif
}

// =======================================================================
// function : IsWriteable
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::IsWriteable()
{
  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
#ifdef _WIN32
  HANDLE aChannel = OSD_File_openFile (aFileName, OSD_ReadWrite, OPEN_OLD);
  if (aChannel == INVALID_HANDLE_VALUE)
  {
    return Standard_False;
  }

  CloseHandle (aChannel);
  return Standard_True;
#else
  return access (aFileName.ToCString(), F_OK | R_OK | W_OK) == 0;
#endif
}

// =======================================================================
// function : IsExecutable
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::IsExecutable()
{
#ifdef _WIN32
  return IsReadable();
#else
  TCollection_AsciiString aFileName;
  myPath.SystemName (aFileName);
  return access (aFileName.ToCString(), F_OK | X_OK) == 0;
#endif
}

// =======================================================================
// function : Capture
// purpose  :
// =======================================================================
int OSD_File::Capture (int theDescr)
{
#ifdef _WIN32
  // Get POSIX file descriptor from this file handle
  int dFile = _open_osfhandle (reinterpret_cast<intptr_t>(myFileHandle), myMode);
  if (0 > dFile)
  {
    _osd_wnt_set_error (myError, OSD_WFile, myFileHandle);
    return -1;
  }

  // Duplicate an old file descriptor of the given one to be able to restore output to it later.
  int oldDescr = _dup (theDescr);
  // Redirect the output to this file
  _dup2 (dFile, theDescr);

  // Return the old descriptor
  return oldDescr;
#else
  // Duplicate an old file descriptor of the given one to be able to restore output to it later.
  int oldDescr = dup (theDescr);
  // Redirect the output to this file
  dup2 (myFileChannel, theDescr);

  // Return the old descriptor
  return oldDescr;
#endif
}

// =======================================================================
// function : Rewind
// purpose  :
// =======================================================================
void OSD_File::Rewind()
{
#ifdef _WIN32
  LARGE_INTEGER aDistanceToMove;
  aDistanceToMove.QuadPart = 0;
  SetFilePointerEx (myFileHandle, aDistanceToMove, NULL, FILE_BEGIN);
#else
  rewind ((FILE* )myFILE);
#endif
}

// =======================================================================
// function : ReadLastLine
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::ReadLastLine (TCollection_AsciiString& theLine,
                                         const Standard_Integer theDelay,
                                         const Standard_Integer theNbTries)
{
  if (theNbTries <= 0)
  {
    return Standard_False;
  }

  const Standard_Integer TheMaxLength = 1000;
  for (Standard_Integer Count = theNbTries; Count > 0; --Count)
  {
    Standard_Integer aLen = 0;
    ReadLine (theLine, TheMaxLength, aLen);
    if (!theLine.IsEmpty())
    {
      return Standard_True;
    }
    OSD::SecSleep (theDelay);
  }
  return Standard_False;
}

// =======================================================================
// function : Edit
// purpose  :
// =======================================================================
Standard_Boolean OSD_File::Edit()
{
  std::cout << "Function OSD_File::Edit() not yet implemented.\n";
  return Standard_False;
}


// None of the existing security APIs are supported in a UWP applications
#ifdef _WIN32
#ifndef OCCT_UWP

#if defined(__CYGWIN32__) || defined(__MINGW32__)
  #define __try
  #define __finally
  #define __leave return retVal
#endif

PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd (const OSD_Protection& theProtection, BOOL theIsDir, const wchar_t* theFileName)
{
  BOOL                 fOK      = FALSE;
  PACL                 pACL     = NULL;
  HANDLE               hProcess = NULL;
  PSID                 pSIDowner;
  DWORD                dwACLsize       = sizeof(ACL);
  DWORD                dwIndex         = 0;
  PTOKEN_OWNER         pTkOwner        = NULL;
  PTOKEN_GROUPS        pTkGroups       = NULL;
  PTOKEN_PRIMARY_GROUP pTkPrimaryGroup = NULL;
  PSECURITY_DESCRIPTOR retVal = NULL;
  PSECURITY_DESCRIPTOR pfSD = NULL;
  BOOL                 fDummy;
  PFILE_ACE            pFileACE;

  __try
  {
    const int j = theIsDir ? 1 : 0;
    if (!OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hProcess))
    {
       __leave;
    }
    if ((pTkGroups = (PTOKEN_GROUPS )GetTokenInformationEx (hProcess, TokenGroups)) == NULL)
    {
      __leave;
    }
    if ((pTkOwner = (PTOKEN_OWNER )GetTokenInformationEx (hProcess, TokenOwner)) == NULL)
    {
      __leave;
    }
    if ((pTkPrimaryGroup = (PTOKEN_PRIMARY_GROUP )GetTokenInformationEx (hProcess, TokenPrimaryGroup)) == NULL)
    {
      __leave;
    }

retry:
    if (theFileName == NULL)
    {
      pSIDowner = pTkOwner->Owner;
    }
    else
    {
      pfSD = GetFileSecurityEx (theFileName, OWNER_SECURITY_INFORMATION);
      if (pfSD == NULL || !GetSecurityDescriptorOwner (pfSD, &pSIDowner, &fDummy))
      {
        theFileName = NULL;
        goto retry;
      }
    }

    PSID pSIDadmin = AdminSid();
    PSID pSIDworld = WorldSid();

    DWORD dwAccessAdmin = OSD_File_getAccessMask (theProtection.System());
    DWORD dwAccessGroup = OSD_File_getAccessMask (theProtection.Group());
    DWORD dwAccessOwner = OSD_File_getAccessMask (theProtection.User());
    DWORD dwAccessWorld = OSD_File_getAccessMask (theProtection.World());

    DWORD dwAccessAdminDir = OSD_File_getDirAccessMask (theProtection.System());
  //DWORD dwAccessGroupDir = OSD_File_getDirAccessMask (theProtection.Group());
    DWORD dwAccessOwnerDir = OSD_File_getDirAccessMask (theProtection.User());
  //DWORD dwAccessWorldDir = OSD_File_getDirAccessMask (theProtection.World());
    if (dwAccessGroup != 0)
    {
      for (int aGroupIter = 0; aGroupIter < (int )pTkGroups->GroupCount; ++aGroupIter)
      {
        PSID pSIDtemp = pTkGroups->Groups[aGroupIter].Sid;
        if (!NtPredefinedSid (pSIDtemp)
         && !EqualSid        (pSIDtemp, pSIDworld)
         && !EqualSid        (pSIDtemp, pTkPrimaryGroup->PrimaryGroup)
         &&  IsValidSid      (pSIDtemp))
        {
          dwACLsize += ((GetLengthSid (pSIDtemp) + ACE_HEADER_SIZE) << j);
        }
      }
    }

    dwACLsize += (((GetLengthSid (pSIDowner) + ACE_HEADER_SIZE) << j)
                + ((GetLengthSid (pSIDadmin) + ACE_HEADER_SIZE) << j)
                + ((GetLengthSid (pSIDworld) + ACE_HEADER_SIZE) << j));
    if ((pACL = CreateAcl (dwACLsize)) == NULL)
    {
      __leave;
    }

    if (dwAccessAdmin != 0)
    {
      if ((pFileACE = (PFILE_ACE )AllocAccessAllowedAce (dwAccessAdmin, 0, pSIDadmin)) != NULL)
      {
        AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        if (theIsDir)
        {
          pFileACE->dwMask = dwAccessAdminDir;
          pFileACE->header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
          AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        }
        FreeAce (pFileACE);
      }
    }

    if (dwAccessOwner != 0)
    {
      if ((pFileACE = (PFILE_ACE )AllocAccessAllowedAce (dwAccessOwner, 0, pSIDowner)) != NULL)
      {
        AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        if (theIsDir)
        {
          pFileACE->dwMask = dwAccessOwnerDir;
          pFileACE->header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
          AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        }
        FreeAce (pFileACE);
      }
    }

    if (dwAccessWorld != 0)
    {
      if ((pFileACE = (PFILE_ACE )AllocAccessAllowedAce (dwAccessWorld, 0, pSIDworld)) != NULL)
      {
        AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        if (theIsDir)
        {
          pFileACE->header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
          AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
        }
        FreeAce (pFileACE);
      }
    }

    if (dwAccessGroup != 0)
    {
      for (int aGroupIter = 0; aGroupIter < (int )pTkGroups->GroupCount; ++aGroupIter)
      {
        PSID pSIDtemp = pTkGroups->Groups[aGroupIter].Sid;
        if (!NtPredefinedSid(pSIDtemp)
         && !EqualSid       (pSIDtemp, pSIDworld)
         && !EqualSid       (pSIDtemp, pTkPrimaryGroup->PrimaryGroup)
         && IsValidSid      (pSIDtemp))
        {
          if (dwAccessGroup != 0)
          {
            if ((pFileACE = (PFILE_ACE )AllocAccessAllowedAce (dwAccessGroup, 0, pSIDtemp)) != NULL)
            {
              AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
              if (theIsDir)
              {
                pFileACE->header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
                AddAce (pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE->header.AceSize);
              }
              FreeAce (pFileACE);
            }
          }
        }
      }
    }

    if ((retVal = AllocSD()) == NULL)
    {
      __leave;
    }

    if (!SetSecurityDescriptorDacl (retVal, TRUE, pACL, TRUE))
    {
      __leave;
    }
    fOK = TRUE;
  }  // end __try

  __finally
  {
    if (!fOK)
    {
      if (retVal != NULL)
      {
        FreeSD (retVal);
      }
      else if (pACL != NULL)
      {
        FreeAcl (pACL);
      }
      retVal = NULL;
    }

    if (hProcess != NULL)
    {
      CloseHandle (hProcess);
    }
    if (pTkOwner != NULL)
    {
      FreeTokenInformation (pTkOwner);
    }
    if (pTkGroups != NULL)
    {
      FreeTokenInformation (pTkGroups);
    }
    if (pTkPrimaryGroup != NULL)
    {
      FreeTokenInformation (pTkPrimaryGroup);
    }
    if (pfSD            != NULL)
    {
      FreeFileSecurity (pfSD);
    }
  }

  return retVal;
}

BOOL __fastcall _osd_wnt_sd_to_protection (PSECURITY_DESCRIPTOR pSD, OSD_Protection& theProtection, BOOL theIsDir)
{
  BOOL fPresent = FALSE;
  BOOL fDefaulted = FALSE;
  PACL pACL;
  PSID pSIDowner;
  BOOL retVal = FALSE;
  __try
  {
    if (!GetSecurityDescriptorOwner (pSD, &pSIDowner, &fDefaulted))
    {
      __leave;
    }
    if (!GetSecurityDescriptorDacl (pSD, &fPresent, &pACL, &fDefaulted)
     || !fPresent)
    {
      __leave;
    }
    if (pSIDowner == NULL || pACL == NULL)
    {
      SetLastError (ERROR_NO_SECURITY_ON_OBJECT);
      __leave;
    }

    PSID pSIDadmin = AdminSid();
    PSID pSIDworld = WorldSid();
    DWORD dwAccessOwner = 0;
    DWORD dwAccessGroup = 0;
    DWORD dwAccessAdmin = 0;
    DWORD dwAccessWorld = 0;
    for (DWORD anAceIter = 0; anAceIter < pACL->AceCount; ++anAceIter)
    {
      LPVOID pACE;
      if (GetAce (pACL, anAceIter, &pACE))
      {
        const DWORD dwAccess = ((PACE_HEADER )pACE)->AceType == ACCESS_DENIED_ACE_TYPE
                             ?  0
                             : *GET_MSK(pACE);
        if (EqualSid (pSIDowner, GET_SID(pACE)))
        {
          dwAccessOwner = dwAccess;
        }
        else if (EqualSid (pSIDadmin, GET_SID(pACE)))
        {
          dwAccessAdmin = dwAccess;
        }
        else if (EqualSid (pSIDworld, GET_SID(pACE)))
        {
          dwAccessWorld = dwAccess;
        }
        else
        {
          dwAccessGroup = dwAccess;
        }
      }
    }

    typedef OSD_SingleProtection (*OSD_File_getProtection_t)(DWORD );
    OSD_File_getProtection_t aGetProtFunc = theIsDir ? &OSD_File_getProtectionDir : &OSD_File_getProtection;
    theProtection.SetValues (aGetProtFunc (dwAccessAdmin),
                             aGetProtFunc (dwAccessOwner),
                             aGetProtFunc (dwAccessGroup),
                             aGetProtFunc (dwAccessWorld));
    retVal = TRUE;
  }  // end __try
  __finally {}

  return retVal;
}  // end _osd_wnt_sd_to_protection

#if defined(__CYGWIN32__) || defined(__MINGW32__)
  #undef __try
  #undef __finally
  #undef __leave
#endif

#endif
#endif /* _WIN32 */
