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
  // it is important to undefine NOUSER and enforce including <windows.h> before
  // Standard_Macro.hxx defines it and includes <windows.h> causing compilation errors
  #ifdef NOUSER
    #undef NOUSER // we need SW_HIDE from windows.h
  #endif
  #include <windows.h>
#endif

#include <OSD_Process.hxx>

#include <NCollection_Array1.hxx>
#include <OSD_Environment.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_WhoAmI.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Quantity_Date.hxx>

#ifdef _WIN32
  #include <OSD_WNT.hxx>
  #include <lmcons.h> // for UNLEN - maximum user name length GetUserName()
#else
const OSD_WhoAmI Iam = OSD_WProcess;
  #include <errno.h>
  #include <stdlib.h>
  #include <sys/param.h>
  #include <sys/time.h>
  #if !defined(__EMSCRIPTEN__)
    #include <pwd.h> // For command getpwuid
  #endif
  #include <unistd.h>
#endif

#if defined(__APPLE__)
  #include <mach-o/dyld.h>
#endif

#ifndef _WIN32

OSD_Process::OSD_Process() {}

void OSD_Process::TerminalType(TCollection_AsciiString& Name)
{
  TCollection_AsciiString which = "TERM";
  OSD_Environment         term(which, "");

  term.Value();
  which = term.Value();
  Name  = term.Name();
}

// Get date of system date

Quantity_Date OSD_Process::SystemDate()
{
  Quantity_Date    result;
  Standard_Integer month = 0, day = 0, year = 0, hh = 0, mn = 0, ss = 0;
  struct tm        transfert;
  struct timeval   tval;
  struct timezone  tzone;
  int              status;

  status = gettimeofday(&tval, &tzone);
  if (status == -1)
    myError.SetValue(errno, Iam, "GetSystem");
  else
  {
    memcpy(&transfert, localtime((time_t*)&tval.tv_sec), sizeof(struct tm));
    month = transfert.tm_mon + 1; // Add to January (month #1)
    day   = transfert.tm_mday;
    year  = transfert.tm_year;
    hh    = transfert.tm_hour;
    mn    = transfert.tm_min;
    ss    = transfert.tm_sec;
  }

  result.SetValues(month, day, year + 1900, hh, mn, ss);
  return (result);
}

Standard_Integer OSD_Process::ProcessId()
{
  return (getpid());
}

TCollection_AsciiString OSD_Process::UserName()
{
  #if defined(__EMSCRIPTEN__)
  // Emscripten SDK raises TODO exception in runtime while calling getpwuid()
  return TCollection_AsciiString();
  #else
  struct passwd* anInfos = getpwuid(getuid());
  return TCollection_AsciiString(anInfos ? anInfos->pw_name : "");
  #endif
}

Standard_Boolean OSD_Process::IsSuperUser()
{
  if (getuid())
  {
    return Standard_False;
  }
  else
  {
    return Standard_True;
  }
}

OSD_Path OSD_Process::CurrentDirectory()
{
  char                    cwd[MAXPATHLEN + 1];
  OSD_Path                result;
  TCollection_AsciiString Name;

  if (!getcwd(cwd, MAXPATHLEN + 1))
    myError.SetValue(errno, Iam, "Where");
  else
  {
    Name = cwd;

    //   JPT : August,20 1993. This code has been replaced by #ifdef ... #endif
    //   position = Name.SearchFromEnd(".");
    //   if (position != -1){
    //     Ext = Name;
    //     Ext.Remove(1,position);
    //     Name.Remove( position,Ext.Length()+1);
    //   }
    //   result.SetValues("","","","","",Name,Ext);
    //   End

  #if defined(vax) || defined(__vms)
    Standard_Integer iDisk = Name.Search(":");
    if (iDisk)
    {
      TCollection_AsciiString Disk;
      TCollection_AsciiString Directory;
      Disk      = Name.SubString(1, iDisk - 1);
      Directory = Name.SubString(iDisk + 1, Name.Length());
      result.SetValues("", "", "", Disk, Directory, "", "");
    }
  #else
    Name += TCollection_AsciiString("/");
    result = OSD_Path(Name);
      //      result.SetValues("","","","",Name,"","");
  #endif
  }
  return (result);
}

void OSD_Process::SetCurrentDirectory(const OSD_Path& where)
{
  TCollection_AsciiString Name;
  int                     status;

  where.SystemName(Name);

  status = chdir(Name.ToCString());
  if (status == -1)
    myError.SetValue(errno, Iam, "Move to directory");
}

void OSD_Process::Reset()
{
  myError.Reset();
}

Standard_Boolean OSD_Process::Failed() const
{
  return (myError.Failed());
}

void OSD_Process::Perror()
{
  myError.Perror();
}

Standard_Integer OSD_Process::Error() const
{
  return (myError.Error());
}

#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Path ---------------------------
//------------------------------------------------------------------------

void _osd_wnt_set_error(OSD_Error&, Standard_Integer, ...);

// =======================================================================
// function : OSD_Process
// purpose  :
// =======================================================================
OSD_Process::OSD_Process()
{
  //
}

void OSD_Process ::TerminalType(TCollection_AsciiString& Name)
{

  Name = "WIN32 console";

} // end OSD_Process :: TerminalType

Quantity_Date OSD_Process ::SystemDate()
{

  Quantity_Date retVal;
  SYSTEMTIME    st;

  GetLocalTime(&st);

  retVal
    .SetValues(st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

  return retVal;

} // end OSD_Process :: SystemDate

// =======================================================================
// function : UserName
// purpose  :
// =======================================================================
TCollection_AsciiString OSD_Process::UserName()
{
  #ifndef OCCT_UWP
  wchar_t                 aUserName[UNLEN + 1];
  DWORD                   aNameSize = UNLEN + 1;
  TCollection_AsciiString retVal;
  if (!GetUserNameW(aUserName, &aNameSize))
  {
    _osd_wnt_set_error(myError, OSD_WProcess);
    return TCollection_AsciiString();
  }
  return TCollection_AsciiString(aUserName);
  #else
  return TCollection_AsciiString();
  #endif
}

Standard_Boolean OSD_Process ::IsSuperUser()
{
  #ifndef OCCT_UWP
  Standard_Boolean retVal = FALSE;
  PSID             pSIDadmin;
  HANDLE           hProcessToken = INVALID_HANDLE_VALUE;
  PTOKEN_GROUPS    pTKgroups     = NULL;

  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hProcessToken)
      || (pTKgroups = (PTOKEN_GROUPS)GetTokenInformationEx(hProcessToken, TokenGroups)) == NULL)

    _osd_wnt_set_error(myError, OSD_WProcess);

  else
  {

    pSIDadmin = AdminSid();

    for (int i = 0; i < (int)pTKgroups->GroupCount; ++i)

      if (EqualSid(pTKgroups->Groups[i].Sid, pSIDadmin))
      {

        retVal = TRUE;
        break;

      } // end if

  } // end else

  if (hProcessToken != INVALID_HANDLE_VALUE)
    CloseHandle(hProcessToken);
  if (pTKgroups != NULL)
    FreeTokenInformation(pTKgroups);

  return retVal;
  #else
  return FALSE;
  #endif
} // end OSD_Process :: IsSuperUser

// =======================================================================
// function : ProcessId
// purpose  :
// =======================================================================
Standard_Integer OSD_Process::ProcessId()
{
  return (Standard_Integer)GetCurrentProcessId();
}

// =======================================================================
// function : CurrentDirectory
// purpose  :
// =======================================================================
OSD_Path OSD_Process::CurrentDirectory()
{
  OSD_Path anCurrentDirectory;
  #ifndef OCCT_UWP
  const DWORD aBuffLen = GetCurrentDirectoryW(0, NULL);
  if (aBuffLen > 0)
  {
    wchar_t* aBuff = new wchar_t[aBuffLen + 1];
    GetCurrentDirectoryW(aBuffLen, aBuff);
    aBuff[aBuffLen] = L'\0';
    const TCollection_AsciiString aPath(aBuff);
    delete[] aBuff;

    anCurrentDirectory = OSD_Path(aPath);
  }
  else
  {
    _osd_wnt_set_error(myError, OSD_WProcess);
  }
  #endif
  return anCurrentDirectory;
}

void OSD_Process ::SetCurrentDirectory(const OSD_Path& where)
{

  TCollection_AsciiString path;

  where.SystemName(path);
  TCollection_ExtendedString pathW(path);

  if (!::SetCurrentDirectoryW(pathW.ToWideString()))

    _osd_wnt_set_error(myError, OSD_WProcess);

} // end OSD_Process :: SetCurrentDirectory

Standard_Boolean OSD_Process ::Failed() const
{

  return myError.Failed();

} // end OSD_Process :: Failed

void OSD_Process ::Reset()
{

  myError.Reset();

} // end OSD_Process :: Reset

void OSD_Process ::Perror()
{

  myError.Perror();

} // end OSD_Process :: Perror

Standard_Integer OSD_Process ::Error() const
{

  return myError.Error();

} // end OSD_Process :: Error

#endif

// =======================================================================
// function : ExecutablePath
// purpose  :
// =======================================================================
TCollection_AsciiString OSD_Process::ExecutablePath()
{
#ifdef _WIN32
  wchar_t aBuff[MAX_PATH + 2];
  DWORD   aLenFilled  = GetModuleFileNameW(0, aBuff, MAX_PATH + 1);
  aBuff[MAX_PATH + 1] = 0;
  if (aLenFilled == 0)
  {
    return TCollection_AsciiString();
  }
  else if (aLenFilled <= MAX_PATH)
  {
    return TCollection_AsciiString(aBuff);
  }

  // buffer is not large enough (e.g. path uses \\?\ prefix)
  wchar_t* aBuffDyn = NULL;
  for (int anIter = 2;; ++anIter)
  {
    size_t aBuffLen = MAX_PATH * anIter;
    aBuffDyn = reinterpret_cast<wchar_t*>(realloc(aBuffDyn, sizeof(wchar_t) * (aBuffLen + 1)));
    if (aBuffDyn == NULL)
    {
      return TCollection_AsciiString();
    }

    aLenFilled = GetModuleFileNameW(NULL, aBuffDyn, DWORD(aBuffLen));
    if (aLenFilled != aBuffLen)
    {
      aBuffDyn[aBuffLen] = L'\0';
      TCollection_AsciiString aRes(aBuffDyn);
      free(aBuffDyn);
      return aRes;
    }
  }
#elif defined(__APPLE__)
  // determine buffer size
  uint32_t aNbBytes = 0;
  _NSGetExecutablePath(NULL, &aNbBytes);
  if (aNbBytes == 0)
  {
    return TCollection_AsciiString();
  }

  // retrieve path to executable (probably link)
  NCollection_Array1<char> aBuff(0, aNbBytes);
  _NSGetExecutablePath(&aBuff.ChangeFirst(), &aNbBytes);
  aBuff[aNbBytes] = '\0';

  // retrieve real path to executable (resolve links and normalize)
  char* aResultBuf = realpath(&aBuff.First(), NULL);
  if (aResultBuf == NULL)
  {
    return TCollection_AsciiString();
  }

  TCollection_AsciiString aProcessPath(aResultBuf);
  free(aResultBuf); // according to man for realpath()
  return aProcessPath;
#elif defined(__linux__)
  // get info from /proc/PID/exe

  TCollection_AsciiString aSimLink =
    TCollection_AsciiString("/proc/") + TCollection_AsciiString(getpid()) + "/exe";
  char    aBuff[4096];
  ssize_t aBytes = readlink(aSimLink.ToCString(), aBuff, 4096);
  if (aBytes > 0)
  {
    aBuff[aBytes] = '\0';
    return TCollection_AsciiString(aBuff);
  }
  return TCollection_AsciiString();
#else
  // not implemented
  return TCollection_AsciiString();
#endif
}

// =======================================================================
// function : ExecutableFolder
// purpose  :
// =======================================================================
TCollection_AsciiString OSD_Process::ExecutableFolder()
{
  TCollection_AsciiString aFullPath  = ExecutablePath();
  Standard_Integer        aLastSplit = -1;
#ifdef _WIN32
  const char THE_FILE_SEPARATOR = '\\';
#else
  const char THE_FILE_SEPARATOR = '/';
#endif
  for (Standard_Integer anIter = 1; anIter <= aFullPath.Length(); ++anIter)
  {
    if (aFullPath.Value(anIter) == THE_FILE_SEPARATOR)
    {
      aLastSplit = anIter;
    }
  }

  if (aLastSplit != -1)
  {
    return aFullPath.SubString(1, aLastSplit);
  }
  return TCollection_AsciiString();
}
