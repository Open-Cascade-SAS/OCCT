// Created on: 2014-08-19
// Created by: Alexander Zaikin
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OSD_Parallel.hxx>

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <sys/types.h>

    #ifdef __sun
        #include <sys/processor.h>
        #include <sys/procset.h>
    #else
        #include <sched.h>
    #endif
#endif

#ifdef _WIN32
namespace {
  // for a 64-bit app running under 64-bit Windows, this is FALSE
  static bool isWow64()
  {
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE , PBOOL);
    BOOL bIsWow64 = FALSE;
    HMODULE aKern32Module = GetModuleHandleW(L"kernel32");
    LPFN_ISWOW64PROCESS aFunIsWow64 = (aKern32Module == NULL) ? (LPFN_ISWOW64PROCESS )NULL
      : (LPFN_ISWOW64PROCESS)GetProcAddress(aKern32Module, "IsWow64Process");

    return aFunIsWow64 != NULL &&
           aFunIsWow64(GetCurrentProcess(), &bIsWow64) &&
           bIsWow64 != FALSE;
  }
}
#endif

//=======================================================================
//function : NbLogicalProcessors
//purpose  : Returns number of logical proccessors.
//=======================================================================
Standard_Integer OSD_Parallel::NbLogicalProcessors()
{
  static Standard_Integer aNumLogicalProcessors = 0;
  if ( aNumLogicalProcessors != 0 )
  {
    return aNumLogicalProcessors;
  }
#ifdef _WIN32
  // GetSystemInfo() will return the number of processors in a data field in a SYSTEM_INFO structure.
  SYSTEM_INFO aSysInfo;
  if ( isWow64() )
  {
    typedef BOOL (WINAPI *LPFN_GSI)(LPSYSTEM_INFO );
    HMODULE aKern32 = GetModuleHandleW(L"kernel32");
    LPFN_GSI aFuncSysInfo = (LPFN_GSI )GetProcAddress(aKern32, "GetNativeSystemInfo");
    // So, they suggest 32-bit apps should call this instead of the other in WOW64
    if ( aFuncSysInfo != NULL )
    {
      aFuncSysInfo(&aSysInfo);
    }
    else
    {
      GetSystemInfo(&aSysInfo);
    }
  }
  else
  {
    GetSystemInfo(&aSysInfo);
  }
  aNumLogicalProcessors = aSysInfo.dwNumberOfProcessors;
#else
  // These are the choices. We'll check number of processors online.
  // _SC_NPROCESSORS_CONF   Number of processors configured
  // _SC_NPROCESSORS_MAX    Max number of processors supported by platform
  // _SC_NPROCESSORS_ONLN   Number of processors online
  aNumLogicalProcessors = (Standard_Integer)sysconf(_SC_NPROCESSORS_ONLN);
#endif
  return aNumLogicalProcessors;
}
