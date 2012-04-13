// Created on: 2011-10-05
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#if (defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
  #include <winbase.h>
  #include <process.h>
  #include <Psapi.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "Psapi.lib")
  #endif
#elif (defined(__APPLE__))
  #include <mach/task.h>
  #include <mach/mach.h>
#endif

#include <string>
#include <sstream>
#include <fstream>

#include <OSD_MemInfo.hxx>

// =======================================================================
// function : OSD_MemInfo
// purpose  :
// =======================================================================
OSD_MemInfo::OSD_MemInfo()
{
  Update();
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void OSD_MemInfo::Update()
{
  // reset values
  for (Standard_Integer anIter = 0; anIter < MemCounter_NB; ++anIter)
  {
    myCounters[anIter] = Standard_Size(-1);
  }

#if (defined(_WIN32) || defined(__WIN32__))
  MEMORYSTATUSEX aStatEx;
  aStatEx.dwLength = sizeof(aStatEx);
  GlobalMemoryStatusEx (&aStatEx);
  myCounters[MemVirtual] = Standard_Size(aStatEx.ullTotalVirtual - aStatEx.ullAvailVirtual);

  // use Psapi library
  HANDLE aProcess = GetCurrentProcess();
#if (_WIN32_WINNT >= 0x0501)
  PROCESS_MEMORY_COUNTERS_EX aProcMemCnts;
#else
  PROCESS_MEMORY_COUNTERS    aProcMemCnts;
#endif
  if (GetProcessMemoryInfo (aProcess, (PROCESS_MEMORY_COUNTERS* )&aProcMemCnts, sizeof(aProcMemCnts)))
  {
  #if (_WIN32_WINNT >= 0x0501)
    myCounters[MemPrivate]        = aProcMemCnts.PrivateUsage;
  #endif
    myCounters[MemWorkingSet]     = aProcMemCnts.WorkingSetSize;
    myCounters[MemWorkingSetPeak] = aProcMemCnts.PeakWorkingSetSize;
    myCounters[MemSwapUsage]      = aProcMemCnts.PagefileUsage;
    myCounters[MemSwapUsagePeak]  = aProcMemCnts.PeakPagefileUsage;
  }
 
#elif (defined(__linux__) || defined(__linux))
  // use procfs on Linux
  char aBuff[4096];
  snprintf (aBuff, sizeof(aBuff), "/proc/%d/status", getpid());
  std::ifstream aFile;
  aFile.open (aBuff);
  if (!aFile.is_open())
  {
    return;
  }

  while (!aFile.eof())
  {
    memset (aBuff, 0, sizeof(aBuff));
    aFile.getline (aBuff, 4096);
    if (aBuff[0] == '\0')
    {
      continue;
    }

    if (strncmp (aBuff, "VmSize:", strlen ("VmSize:")) == 0)
    {
      myCounters[MemVirtual] = atol (aBuff + strlen ("VmSize:")) * 1024;
    }
    //else if (strncmp (aBuff, "VmPeak:", strlen ("VmPeak:")) == 0)
    //  myVirtualPeak = atol (aBuff + strlen ("VmPeak:")) * 1024;
    else if (strncmp (aBuff, "VmRSS:", strlen ("VmRSS:")) == 0)
    {
      myCounters[MemWorkingSet] = atol (aBuff + strlen ("VmRSS:")) * 1024; // RSS - resident set size
    }
    else if (strncmp (aBuff, "VmHWM:", strlen ("VmHWM:")) == 0)
    {
      myCounters[MemWorkingSetPeak] = atol (aBuff + strlen ("VmHWM:")) * 1024; // HWM - high water mark
    }
    else if (strncmp (aBuff, "VmData:", strlen ("VmData:")) == 0)
    {
      if (myCounters[MemPrivate] == Standard_Size(-1)) ++myCounters[MemPrivate];
      myCounters[MemPrivate] += atol (aBuff + strlen ("VmData:")) * 1024;
    }
    else if (strncmp (aBuff, "VmStk:", strlen ("VmStk:")) == 0)
    {
      if (myCounters[MemPrivate] == Standard_Size(-1)) ++myCounters[MemPrivate];
      myCounters[MemPrivate] += atol (aBuff + strlen ("VmStk:")) * 1024;
    }
  }
  aFile.close();
#elif (defined(__APPLE__))
  struct task_basic_info aTaskInfo;
  mach_msg_type_number_t aTaskInfoCount = TASK_BASIC_INFO_COUNT;
  if (task_info (mach_task_self(), TASK_BASIC_INFO,
                 (task_info_t )&aTaskInfo, &aTaskInfoCount) == KERN_SUCCESS)
  {
    // On Mac OS X, these values in bytes, not pages!
    myCounters[MemVirtual]    = aTaskInfo.virtual_size;
    myCounters[MemWorkingSet] = aTaskInfo.resident_size;
  }
#endif
}

// =======================================================================
// function : ToString
// purpose  :
// =======================================================================
TCollection_AsciiString OSD_MemInfo::ToString() const
{
  TCollection_AsciiString anInfo;
  if (myCounters[MemPrivate] != Standard_Size(-1))
  {
    anInfo += TCollection_AsciiString("  Private memory:     ") + Standard_Integer (ValueMiB (MemPrivate)) + " MiB\n";
  }
  if (myCounters[MemWorkingSet] != Standard_Size(-1))
  {
    anInfo += TCollection_AsciiString("  Working Set:        ") +  Standard_Integer (ValueMiB (MemWorkingSet)) + " MiB";
    if (myCounters[MemWorkingSetPeak] != Standard_Size(-1))
    {
      anInfo += TCollection_AsciiString(" (peak: ") +  Standard_Integer (ValueMiB (MemWorkingSetPeak)) + " MiB)";
    }
    anInfo += "\n";
  }
  if (myCounters[MemSwapUsage] != Standard_Size(-1))
  {
    anInfo += TCollection_AsciiString("  Pagefile usage:     ") +  Standard_Integer (ValueMiB (MemSwapUsage)) + " MiB";
    if (myCounters[MemSwapUsagePeak] != Standard_Size(-1))
    {
      anInfo += TCollection_AsciiString(" (peak: ") +  Standard_Integer (ValueMiB (MemSwapUsagePeak)) + " MiB)";
    }
    anInfo += "\n";
  }
  if (myCounters[MemVirtual] != Standard_Size(-1))
  {
    anInfo += TCollection_AsciiString("  Virtual memory:     ") +  Standard_Integer (ValueMiB (MemVirtual)) + " MiB\n";
  }
  return anInfo;
}

// =======================================================================
// function : Value
// purpose  :
// =======================================================================
Standard_Size OSD_MemInfo::Value (const OSD_MemInfo::Counter theCounter) const
{
  if (theCounter < 0 || theCounter >= MemCounter_NB)
  {
    return Standard_Size(-1);
  }
  return myCounters[theCounter];
}

// =======================================================================
// function : ValueMiB
// purpose  :
// =======================================================================
Standard_Size OSD_MemInfo::ValueMiB (const OSD_MemInfo::Counter theCounter) const
{
  if (theCounter < 0 || theCounter >= MemCounter_NB)
  {
    return Standard_Size(-1);
  }
  return (myCounters[theCounter] == Standard_Size(-1))
       ? Standard_Size(-1) : (myCounters[theCounter] / (1024 * 1024));
}

// =======================================================================
// function : ShowInfo
// purpose  :
// =======================================================================
TCollection_AsciiString OSD_MemInfo::PrintInfo()
{
  OSD_MemInfo anInfo;
  return anInfo.ToString();
}
