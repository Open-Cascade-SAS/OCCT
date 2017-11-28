// Created on: 1992-11-16
// Created by: Mireille MERCIEN
// Copyright (c) 1992-1999 Matra Datavision
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


#include <OSD_Chronometer.hxx>
#include <Standard_Stream.hxx>

#ifndef _WIN32

#include <sys/times.h>
#include <unistd.h>

#ifdef SOLARIS
  #include <sys/resource.h>
#endif

#ifndef sysconf
  #define _sysconf sysconf
#endif

#if defined(DECOSF1)
  #include <time.h>
#endif

#ifndef CLK_TCK
  #define CLK_TCK CLOCKS_PER_SEC
#endif

#if (defined(__APPLE__))
  #include <mach/task.h>
  #include <mach/mach.h>
#endif

//=======================================================================
//function : GetProcessCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetProcessCPU (Standard_Real& theUserSeconds,
                                     Standard_Real& theSystemSeconds)
{
#if defined(__linux__) || defined(__FreeBSD__) || defined(__ANDROID__) || defined(__QNX__)
  static const long aCLK_TCK = sysconf(_SC_CLK_TCK);
#else
  static const long aCLK_TCK = CLK_TCK;
#endif

  tms aCurrentTMS;
  times (&aCurrentTMS);

  theUserSeconds   = (Standard_Real)aCurrentTMS.tms_utime / aCLK_TCK;
  theSystemSeconds = (Standard_Real)aCurrentTMS.tms_stime / aCLK_TCK;
}

//=======================================================================
//function : GetThreadCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetThreadCPU (Standard_Real& theUserSeconds,
                                    Standard_Real& theSystemSeconds)
{
  theUserSeconds = theSystemSeconds = 0.0;
#if (defined(__APPLE__))
  struct task_thread_times_info aTaskInfo;
  mach_msg_type_number_t aTaskInfoCount = TASK_THREAD_TIMES_INFO_COUNT;
  if (KERN_SUCCESS == task_info(mach_task_self(), TASK_THREAD_TIMES_INFO,
      (task_info_t )&aTaskInfo, &aTaskInfoCount))
  {
    theUserSeconds   = Standard_Real(aTaskInfo.user_time.seconds)   + 0.000001 * aTaskInfo.user_time.microseconds;
    theSystemSeconds = Standard_Real(aTaskInfo.system_time.seconds) + 0.000001 * aTaskInfo.system_time.microseconds;
  }
#elif (defined(_POSIX_TIMERS) && defined(_POSIX_THREAD_CPUTIME)) || defined(__ANDROID__) || defined(__QNX__)
  // on Linux, only user times are available for threads via clock_gettime()
  struct timespec t;
  if (!clock_gettime (CLOCK_THREAD_CPUTIME_ID, &t))
  {
    theUserSeconds = t.tv_sec + 0.000000001 * t.tv_nsec;
  }
#elif defined(SOLARIS)
  // on Solaris, both user and system times are available as LWP times
  struct rusage rut;
  if (!getrusage (RUSAGE_LWP, &rut))
  {
    theUserSeconds   = rut.ru_utime.tv_sec + 0.000001 * rut.ru_utime.tv_usec;
    theSystemSeconds = rut.ru_stime.tv_sec + 0.000001 * rut.ru_stime.tv_usec;
  }
#else
  #pragma error "OS is not supported yet; code to be ported"
#endif
}

#else

#include <windows.h>

//=======================================================================
//function : EncodeFILETIME
//purpose  : Encode time defined by FILETIME structure
//           (100s nanoseconds since January 1, 1601) to 64-bit integer
//=======================================================================
static inline __int64 EncodeFILETIME (PFILETIME pFt)
{
  __int64 qw;

  qw   = pFt -> dwHighDateTime;
  qw <<= 32;
  qw  |= pFt -> dwLowDateTime;

  return qw;
}

//=======================================================================
//function : GetProcessCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetProcessCPU (Standard_Real& theUserSeconds,
                                     Standard_Real& theSystemSeconds)
{
#ifndef OCCT_UWP
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetProcessTimes (GetCurrentProcess(), &ftStart, &ftExit, &ftKernel, &ftUser);
  theUserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  theSystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
#else
  theUserSeconds = 0.0;
  theSystemSeconds = 0.0;
#endif
}

//=======================================================================
//function : GetThreadCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetThreadCPU (Standard_Real& theUserSeconds,
                                    Standard_Real& theSystemSeconds)
{
#ifndef OCCT_UWP
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetThreadTimes (GetCurrentThread(), &ftStart, &ftExit, &ftKernel, &ftUser);
  theUserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  theSystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
#else
  theUserSeconds = 0.0;
  theSystemSeconds = 0.0;
#endif
}

#endif /* _WIN32 */

//=======================================================================
//function : OSD_Chronometer
//purpose  :
//=======================================================================
OSD_Chronometer::OSD_Chronometer (Standard_Boolean theThisThreadOnly)
: myStartCpuUser (0.0),
  myStartCpuSys  (0.0),
  myCumulCpuUser (0.0),
  myCumulCpuSys  (0.0),
  myIsStopped    (Standard_True),
  myIsThreadOnly (theThisThreadOnly)
{
  //
}

//=======================================================================
//function : ~OSD_Chronometer
//purpose  : Destructor
//=======================================================================
OSD_Chronometer::~OSD_Chronometer()
{
}

//=======================================================================
//function : Reset
//purpose  :
//=======================================================================
void OSD_Chronometer::Reset ()
{
  myIsStopped    = Standard_True;
  myStartCpuUser = myStartCpuSys = 0.;
  myCumulCpuUser = myCumulCpuSys = 0.;
}


//=======================================================================
//function : Restart
//purpose  :
//=======================================================================
void OSD_Chronometer::Restart ()
{
  Reset();
  Start();
}

//=======================================================================
//function : Stop
//purpose  :
//=======================================================================
void OSD_Chronometer::Stop()
{
  if (!myIsStopped)
  {
    Standard_Real Curr_user, Curr_sys;
    if (myIsThreadOnly)
      GetThreadCPU (Curr_user, Curr_sys);
    else
      GetProcessCPU (Curr_user, Curr_sys);

    myCumulCpuUser += Curr_user - myStartCpuUser;
    myCumulCpuSys  += Curr_sys  - myStartCpuSys;

    myIsStopped = Standard_True;
  }
}

//=======================================================================
//function : Start
//purpose  :
//=======================================================================
void OSD_Chronometer::Start ()
{
  if (myIsStopped)
  {
    if (myIsThreadOnly)
      GetThreadCPU (myStartCpuUser, myStartCpuSys);
    else
      GetProcessCPU (myStartCpuUser, myStartCpuSys);

    myIsStopped = Standard_False;
  }
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
void OSD_Chronometer::Show() const
{
  Show (std::cout);
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
void OSD_Chronometer::Show (Standard_OStream& theOStream) const
{
  Standard_Real aCumulUserSec = 0.0, aCumulSysSec = 0.0;
  Show (aCumulUserSec, aCumulSysSec);
  std::streamsize prec = theOStream.precision (12);
  theOStream << "CPU user time: "   << aCumulUserSec << " seconds\n";
  theOStream << "CPU system time: " << aCumulSysSec  << " seconds\n";
  theOStream.precision (prec);
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
void OSD_Chronometer::Show (Standard_Real& theUserSec, Standard_Real& theSystemSec) const
{
  theUserSec   = myCumulCpuUser;
  theSystemSec = myCumulCpuSys;
  if (myIsStopped)
  {
    return;
  }

  Standard_Real aCurrUser, aCurrSys;
  if (myIsThreadOnly)
    GetThreadCPU  (aCurrUser, aCurrSys);
  else
    GetProcessCPU (aCurrUser, aCurrSys);

  theUserSec   += aCurrUser - myStartCpuUser;
  theSystemSec += aCurrSys  - myStartCpuSys;
}
