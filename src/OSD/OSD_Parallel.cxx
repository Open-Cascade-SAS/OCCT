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
  #include <unistd.h>

  #ifdef __sun
    #include <sys/processor.h>
    #include <sys/procset.h>
  #else
    #include <sched.h>
  #endif
#endif

namespace {

#if defined(_WIN32) && !defined(OCCT_UWP)
  //! For a 64-bit app running under 64-bit Windows, this is FALSE.
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

#elif defined(__ANDROID__)

  //! Simple number parser.
  static const char* parseNumber (int&        theResult,
                                  const char* theInput,
                                  const char* theLimit,
                                  const int   theBase = 10)
  {
    const char* aCharIter = theInput;
    int aValue = 0;
    while (aCharIter < theLimit)
    {
      int aDigit = (*aCharIter - '0');
      if ((unsigned int )aDigit >= 10U)
      {
        aDigit = (*aCharIter - 'a');
        if ((unsigned int )aDigit >= 6U)
        {
          aDigit = (*aCharIter - 'A');
        }
        if ((unsigned int )aDigit >= 6U)
        {
          break;
        }
        aDigit += 10;
      }
      if (aDigit >= theBase)
      {
        break;
      }
      aValue = aValue * theBase + aDigit;
      ++aCharIter;
    }
    if (aCharIter == theInput)
    {
      return NULL;
    }

    theResult = aValue;
    return aCharIter;
  }

  //! Read CPUs mask from sysfs.
  static uint32_t readCpuMask (const char* thePath)
  {
    FILE* aFileHandle = fopen (thePath, "rb");
    if (aFileHandle == NULL)
    {
      return 0;
    }

    fseek (aFileHandle, 0, SEEK_END);
    long aFileLen = ftell (aFileHandle);
    if (aFileLen <= 0L)
    {
      fclose (aFileHandle);
      return 0;
    }

    char* aBuffer = (char* )Standard::Allocate (aFileLen);
    if (aBuffer == NULL)
    {
      return 0;
    }

    fseek (aFileHandle, 0, SEEK_SET);
    size_t aCountRead = fread (aBuffer, 1, aFileLen, aFileHandle);
    (void )aCountRead;
    fclose (aFileHandle);

    uint32_t aCpuMask = 0;
    const char* anEnd = aBuffer + aFileLen;
    for (const char* aCharIter = aBuffer; aCharIter < anEnd && *aCharIter != '\n';)
    {
      const char* aChunkEnd = (const char* )::memchr (aCharIter, ',', anEnd - aCharIter);
      if (aChunkEnd == NULL)
      {
        aChunkEnd = anEnd;
      }

      // get first value
      int anIndexLower = 0;
      aCharIter = parseNumber (anIndexLower, aCharIter, aChunkEnd);
      if (aCharIter == NULL)
      {
        Standard::Free (aBuffer);
        return aCpuMask;
      }

      // if we're not at the end of the item, expect a dash and and integer; extract end value.
      int anIndexUpper = anIndexLower;
      if (aCharIter < aChunkEnd && *aCharIter == '-')
      {
        aCharIter = parseNumber (anIndexUpper, aCharIter + 1, aChunkEnd);
        if (aCharIter == NULL)
        {
          Standard::Free (aBuffer);
          return aCpuMask;
        }
      }

      // set bits CPU list
      for (int aCpuIndex = anIndexLower; aCpuIndex <= anIndexUpper; ++aCpuIndex)
      {
        if ((unsigned int )aCpuIndex < 32)
        {
          aCpuMask |= (uint32_t )(1U << aCpuIndex);
        }
      }

      aCharIter = aChunkEnd;
      if (aCharIter < anEnd)
      {
        ++aCharIter;
      }
    }

    Standard::Free (aBuffer);
    return aCpuMask;
  }
#endif

}

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
#ifndef OCCT_UWP
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
#else
  GetNativeSystemInfo(&aSysInfo);
#endif
  aNumLogicalProcessors = aSysInfo.dwNumberOfProcessors;
#else

#if defined(__ANDROID__)
  uint32_t aCpuMaskPresent  = readCpuMask ("/sys/devices/system/cpu/present");
  uint32_t aCpuMaskPossible = readCpuMask ("/sys/devices/system/cpu/possible");
  aCpuMaskPresent &= aCpuMaskPossible;
  aNumLogicalProcessors = __builtin_popcount (aCpuMaskPresent);
  if (aNumLogicalProcessors >= 1)
  {
    return aNumLogicalProcessors;
  }
#endif

  // These are the choices. We'll check number of processors online.
  // _SC_NPROCESSORS_CONF   Number of processors configured
  // _SC_NPROCESSORS_MAX    Max number of processors supported by platform
  // _SC_NPROCESSORS_ONLN   Number of processors online
  aNumLogicalProcessors = (Standard_Integer)sysconf(_SC_NPROCESSORS_ONLN);
#endif
  return aNumLogicalProcessors;
}
