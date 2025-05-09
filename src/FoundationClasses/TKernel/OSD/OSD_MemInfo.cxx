// Created on: 2011-10-05
// Created by: Kirill GAVRILOV
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

#if (defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
  #include <winbase.h>
  #include <process.h>
  #include <malloc.h>
  #include <psapi.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "Psapi.lib")
  #endif
#elif (defined(__APPLE__))
  #include <mach/task.h>
  #include <mach/mach.h>
  #include <malloc/malloc.h>
#else
  #include <unistd.h>
  #include <malloc.h>
#endif

#include <OSD_MemInfo.hxx>

#if defined(__EMSCRIPTEN__)
  #include <emscripten/heap.h>
#endif
#if defined(USE_MIMALLOC)
  #include <mimalloc.h>
#endif

#ifndef _WIN32
  #include <pthread.h>
#endif

//=================================================================================================

OSD_MemInfo::OSD_MemInfo(const Standard_Boolean theImmediateUpdate)
{
  SetActive(Standard_True);
  if (theImmediateUpdate)
  {
    Update();
  }
  else
  {
    Clear();
  }
}

//=================================================================================================

void OSD_MemInfo::SetActive(const Standard_Boolean theActive)
{
  for (Standard_Integer anIter = 0; anIter < MemCounter_NB; ++anIter)
  {
    SetActive((Counter)anIter, theActive);
  }
}

//=================================================================================================

void OSD_MemInfo::Clear()
{
  for (Standard_Integer anIter = 0; anIter < MemCounter_NB; ++anIter)
  {
    myCounters[anIter] = Standard_Size(-1);
  }
}

//=================================================================================================

void OSD_MemInfo::Update()
{
  Clear();
#ifndef OCCT_UWP
  #if defined(_WIN32)
    #if (_WIN32_WINNT >= 0x0500)
  if (IsActive(MemVirtual))
  {
    MEMORYSTATUSEX aStatEx;
    aStatEx.dwLength = sizeof(aStatEx);
    GlobalMemoryStatusEx(&aStatEx);
    myCounters[MemVirtual] = Standard_Size(aStatEx.ullTotalVirtual - aStatEx.ullAvailVirtual);
  }
    #else
  if (IsActive(MemVirtual))
  {
    MEMORYSTATUS aStat;
    aStat.dwLength = sizeof(aStat);
    GlobalMemoryStatus(&aStat);
    myCounters[MemVirtual] = Standard_Size(aStat.dwTotalVirtual - aStat.dwAvailVirtual);
  }
    #endif

  if (IsActive(MemPrivate) || IsActive(MemWorkingSet) || IsActive(MemWorkingSetPeak)
      || IsActive(MemSwapUsage) || IsActive(MemSwapUsagePeak))
  {
    // use Psapi library
    HANDLE aProcess = GetCurrentProcess();
    #if (_WIN32_WINNT >= 0x0501)
    PROCESS_MEMORY_COUNTERS_EX aProcMemCnts;
    #else
    PROCESS_MEMORY_COUNTERS aProcMemCnts;
    #endif
    if (GetProcessMemoryInfo(aProcess,
                             (PROCESS_MEMORY_COUNTERS*)&aProcMemCnts,
                             sizeof(aProcMemCnts)))
    {
    #if (_WIN32_WINNT >= 0x0501)
      myCounters[MemPrivate] = aProcMemCnts.PrivateUsage;
    #endif
      myCounters[MemWorkingSet]     = aProcMemCnts.WorkingSetSize;
      myCounters[MemWorkingSetPeak] = aProcMemCnts.PeakWorkingSetSize;
      myCounters[MemSwapUsage]      = aProcMemCnts.PagefileUsage;
      myCounters[MemSwapUsagePeak]  = aProcMemCnts.PeakPagefileUsage;
    }
  }

  #if (_WIN32_WINNT >= 0x0602)
  if (IsActive(MemStackSize))
  {
    ULONG_PTR aStackLow = 0, aStackUp = 0;
    GetCurrentThreadStackLimits(&aStackLow, &aStackUp);
    myCounters[MemStackSize] = Standard_Size(aStackUp - aStackLow);
  }
  #endif

  if (IsActive(MemHeapUsage))
  {
    _HEAPINFO hinfo;
    int       heapstatus;
    hinfo._pentry = NULL;

    myCounters[MemHeapUsage] = 0;
    while ((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
    {
      if (hinfo._useflag == _USEDENTRY)
      {
        myCounters[MemHeapUsage] += hinfo._size;
      }
    }
  }

  #elif defined(__EMSCRIPTEN__)
  #if defined(USE_MIMALLOC)
  // -sMALLOC=mimalloc provides own diagnostics API
  if (IsActive(MemWorkingSet)
   || IsActive(MemWorkingSetPeak))
  {
    size_t current_rss = 0, peak_rss = 0;
    mi_process_info(nullptr, nullptr, nullptr, &current_rss, &peak_rss, nullptr, nullptr, nullptr);
    // workaround negative number returned by mi_process_info() (which mi_stats_print(NULL) prints as such)
    if (intptr_t(current_rss) < 0) { current_rss = 0; }
    if (intptr_t(peak_rss) < 0) { peak_rss = 0; }
    myCounters[MemWorkingSet] = current_rss;
    myCounters[MemWorkingSetPeak] = peak_rss;
  }
  if (IsActive(MemHeapUsage))
  {
    struct MiVisitor
    {
      static bool visitor(const mi_heap_t* , const mi_heap_area_t* theArea, void*, size_t, void* thePtr)
      {
        Standard_Size* aCounter = (Standard_Size*)thePtr;
        *aCounter += theArea->committed;
        return true;
      }
    };

    myCounters[MemHeapUsage] = 0;
    const mi_heap_t* aDefHeap = mi_heap_get_default();
    mi_heap_visit_blocks(aDefHeap, false, &MiVisitor::visitor, &myCounters[MemHeapUsage]);
  }
  #else
  // -sMALLOC=dlmalloc provides mallinfo()
  if (IsActive(MemHeapUsage)
   || IsActive(MemWorkingSet)
   || IsActive(MemWorkingSetPeak))
  {
    const struct mallinfo aMI = mallinfo();
    if (IsActive(MemHeapUsage))
    {
      myCounters[MemHeapUsage] = aMI.uordblks;
    }
    if (IsActive(MemWorkingSet))
    {
      myCounters[MemWorkingSet] = aMI.uordblks;
    }
    if (IsActive(MemWorkingSetPeak))
    {
      myCounters[MemWorkingSetPeak] = aMI.usmblks;
    }
  }
  #endif
  if (IsActive(MemVirtual))
  {
    myCounters[MemVirtual] = emscripten_get_heap_size();
  }
  if (IsActive(MemVirtualMax))
  {
    myCounters[MemVirtualMax] = emscripten_get_heap_max();
  }
  #elif (defined(__linux__) || defined(__linux))
  if (IsActive(MemHeapUsage))
  {
    #if defined(__GLIBC__)
      #define HAS_MALLINFO
      #if defined(__GLIBC_PREREQ) && __GLIBC_PREREQ(2, 33)
        #define HAS_MALLINFO2
      #endif
    #endif

    #ifdef HAS_MALLINFO
      #ifdef HAS_MALLINFO2
    const struct mallinfo2 aMI = mallinfo2();
      #else
    const struct mallinfo aMI = mallinfo();
      #endif
    myCounters[MemHeapUsage] = aMI.uordblks;
    #else
    myCounters[MemHeapUsage] = 0;
    #endif
  }

  if (!IsActive(MemVirtual) && !IsActive(MemWorkingSet) && !IsActive(MemWorkingSetPeak)
      && !IsActive(MemPrivate))
  {
    return;
  }

  // use procfs on Linux
  char aBuff[4096];
  snprintf(aBuff, sizeof(aBuff), "/proc/%d/status", getpid());
  std::ifstream aFile;
  aFile.open(aBuff);
  if (!aFile.is_open())
  {
    return;
  }

  while (!aFile.eof())
  {
    memset(aBuff, 0, sizeof(aBuff));
    aFile.getline(aBuff, 4096);
    if (aBuff[0] == '\0')
    {
      continue;
    }

    if (IsActive(MemVirtual) && strncmp(aBuff, "VmSize:", strlen("VmSize:")) == 0)
    {
      myCounters[MemVirtual] = atol(aBuff + strlen("VmSize:")) * 1024;
    }
    // else if (strncmp (aBuff, "VmPeak:", strlen ("VmPeak:")) == 0)
    //   myVirtualPeak = atol (aBuff + strlen ("VmPeak:")) * 1024;
    else if (IsActive(MemWorkingSet) && strncmp(aBuff, "VmRSS:", strlen("VmRSS:")) == 0)
    {
      // clang-format off
      myCounters[MemWorkingSet] = atol (aBuff + strlen ("VmRSS:")) * 1024; // RSS - resident set size
    }
    else if (IsActive (MemWorkingSetPeak)
          && strncmp (aBuff, "VmHWM:", strlen ("VmHWM:")) == 0)
    {
      myCounters[MemWorkingSetPeak] = atol (aBuff + strlen ("VmHWM:")) * 1024; // HWM - high water mark
      // clang-format on
    }
    else if (IsActive(MemPrivate) && strncmp(aBuff, "VmData:", strlen("VmData:")) == 0)
    {
      if (myCounters[MemPrivate] == Standard_Size(-1))
        ++myCounters[MemPrivate];
      myCounters[MemPrivate] += atol(aBuff + strlen("VmData:")) * 1024;
    }
    else if (IsActive(MemPrivate) && strncmp(aBuff, "VmStk:", strlen("VmStk:")) == 0)
    {
      if (myCounters[MemPrivate] == Standard_Size(-1))
        ++myCounters[MemPrivate];
      myCounters[MemPrivate] += atol(aBuff + strlen("VmStk:")) * 1024;
    }
  }
  aFile.close();
  #elif (defined(__APPLE__))
  if (IsActive(MemVirtual) || IsActive(MemWorkingSet) || IsActive(MemHeapUsage))
  {
    struct task_basic_info aTaskInfo;
    mach_msg_type_number_t aTaskInfoCount = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&aTaskInfo, &aTaskInfoCount)
        == KERN_SUCCESS)
    {
      // On Mac OS X, these values in bytes, not pages!
      myCounters[MemVirtual]    = aTaskInfo.virtual_size;
      myCounters[MemWorkingSet] = aTaskInfo.resident_size;

      // Getting malloc statistics
      malloc_statistics_t aStats;
      malloc_zone_statistics(NULL, &aStats);

      myCounters[MemHeapUsage] = aStats.size_in_use;
    }
  }
  #endif
#endif

#ifndef _WIN32
  if (IsActive(MemStackSize))
  {
    // pthread_attr_init() returns default attributes for creating new threads;
    // some systems provide non-POSIX extensions to get values of specific thread:
    // - pthread_getattr_np() on Linux and Android
    // - pthread_get_stacksize_np() on macOS
#if defined(__ANDROID__)
#define HAS_GETATTR_NP
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ) && !defined(__EMSCRIPTEN__)
#if __GLIBC_PREREQ(2,4)
#define HAS_GETATTR_NP
#endif
#endif

#ifdef __APPLE__
    myCounters[MemStackSize] = pthread_get_stacksize_np(pthread_self());
#else
    pthread_attr_t aPAttr;
#ifdef HAS_GETATTR_NP
    if (pthread_getattr_np(pthread_self(), &aPAttr) == 0)
#else
    if (pthread_attr_init(&aPAttr) == 0)
#endif
    {
      size_t aStackSize = 0;
      if (pthread_attr_getstacksize(&aPAttr, &aStackSize) == 0)
      {
        myCounters[MemStackSize] = aStackSize;
      }
      pthread_attr_destroy(&aPAttr);
    }
#endif
  }
#endif
}

//=================================================================================================

TCollection_AsciiString OSD_MemInfo::ToString() const
{
  TCollection_AsciiString anInfo;
  if (hasValue(MemPrivate))
  {
    anInfo += TCollection_AsciiString("  Private memory:     ")
              + Standard_Integer(ValueMiB(MemPrivate)) + " MiB\n";
  }
  if (hasValue(MemWorkingSet))
  {
    anInfo += TCollection_AsciiString("  Working Set:        ")
              + Standard_Integer(ValueMiB(MemWorkingSet)) + " MiB";
    if (hasValue(MemWorkingSetPeak))
    {
      anInfo += TCollection_AsciiString(" (peak: ") + Standard_Integer(ValueMiB(MemWorkingSetPeak))
                + " MiB)";
    }
    anInfo += "\n";
  }
  if (hasValue(MemSwapUsage))
  {
    anInfo += TCollection_AsciiString("  Pagefile usage:     ")
              + Standard_Integer(ValueMiB(MemSwapUsage)) + " MiB";
    if (hasValue(MemSwapUsagePeak))
    {
      anInfo += TCollection_AsciiString(" (peak: ") + Standard_Integer(ValueMiB(MemSwapUsagePeak))
                + " MiB)";
    }
    anInfo += "\n";
  }
  if (hasValue(MemVirtual))
  {
    anInfo += TCollection_AsciiString("  Virtual memory:     ")
              + Standard_Integer(ValueMiB(MemVirtual)) + " MiB";
    if (hasValue(MemVirtualMax))
    {
      anInfo += TCollection_AsciiString(" (limit: ")
                + Standard_Integer(ValueMiB(MemVirtualMax)) + " MiB)";
    }
    anInfo += "\n";
  }
  if (hasValue(MemHeapUsage))
  {
    anInfo += TCollection_AsciiString("  Heap memory:        ")
              + Standard_Integer(ValueMiB(MemHeapUsage)) + " MiB\n";
  }
  if (hasValue(MemStackSize))
  {
    anInfo += TCollection_AsciiString("  Thread stack size:  ");
    const Standard_Size aValMiB = ValueMiB(MemStackSize);
    if (aValMiB != 0)
    {
      anInfo += TCollection_AsciiString(Standard_Integer(aValMiB)) + " MiB\n";
    }
    else
    {
      anInfo += TCollection_AsciiString(Standard_Integer(Value(MemStackSize) / 1024)) + " KiB\n";
    }
  }
  return anInfo;
}

//=================================================================================================

Standard_Size OSD_MemInfo::Value(const OSD_MemInfo::Counter theCounter) const
{
  if (theCounter < 0 || theCounter >= MemCounter_NB || !IsActive(theCounter))
  {
    return Standard_Size(-1);
  }
  return myCounters[theCounter];
}

//=================================================================================================

Standard_Size OSD_MemInfo::ValueMiB(const OSD_MemInfo::Counter theCounter) const
{
  if (theCounter < 0 || theCounter >= MemCounter_NB || !IsActive(theCounter))
  {
    return Standard_Size(-1);
  }
  return (myCounters[theCounter] == Standard_Size(-1)) ? Standard_Size(-1)
                                                       : (myCounters[theCounter] / (1024 * 1024));
}

//=================================================================================================

Standard_Real OSD_MemInfo::ValuePreciseMiB(const OSD_MemInfo::Counter theCounter) const
{
  if (theCounter < 0 || theCounter >= MemCounter_NB || !IsActive(theCounter))
  {
    return -1.0;
  }
  return (myCounters[theCounter] == Standard_Size(-1))
           ? -1.0
           : ((Standard_Real)myCounters[theCounter] / (1024.0 * 1024.0));
}

//=================================================================================================

TCollection_AsciiString OSD_MemInfo::PrintInfo()
{
  OSD_MemInfo anInfo;
  return anInfo.ToString();
}
