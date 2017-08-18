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

// ====================== PLATFORM-SPECIFIC PART ========================
#ifndef _WIN32

//---------- Systemes autres que WNT : ----------------------------------

#include <sys/times.h>
#include <unistd.h>

#ifdef SOLARIS
# include <sys/resource.h>
#endif

//=======================================================================
//Selon les plateformes on doit avoir le nombre de clicks par secondes
//qui est l unite de mesure du temps.
//=======================================================================
#ifndef sysconf
# define _sysconf sysconf
#endif

#if defined(DECOSF1)
# include <time.h>
#endif

#  ifndef CLK_TCK
#   define CLK_TCK	CLOCKS_PER_SEC
#  endif

#if (defined(__APPLE__))
  #include <mach/task.h>
  #include <mach/mach.h>
#endif

//=======================================================================
//function : GetProcessCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetProcessCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
#if defined(__linux__) || defined(__FreeBSD__) || defined(__ANDROID__) || defined(__QNX__)
  static const long aCLK_TCK = sysconf(_SC_CLK_TCK);
#else
  static const long aCLK_TCK = CLK_TCK;
#endif

  tms CurrentTMS;
  times (&CurrentTMS);

  UserSeconds   = (Standard_Real)CurrentTMS.tms_utime / aCLK_TCK;
  SystemSeconds = (Standard_Real)CurrentTMS.tms_stime / aCLK_TCK;
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

//---------------------------- Systeme WNT --------------------------------

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
void OSD_Chronometer::GetProcessCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
#ifndef OCCT_UWP
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetProcessTimes (GetCurrentProcess(), &ftStart, &ftExit, &ftKernel, &ftUser);
  UserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  SystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
#else
  UserSeconds = 0.0;
  SystemSeconds = 0.0;
#endif
}

//=======================================================================
//function : GetThreadCPU
//purpose  :
//=======================================================================
void OSD_Chronometer::GetThreadCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
#ifndef OCCT_UWP
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetThreadTimes (GetCurrentThread(), &ftStart, &ftExit, &ftKernel, &ftUser);
  UserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  SystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
#else
  UserSeconds = 0.0;
  SystemSeconds = 0.0;
#endif
}

#endif /* _WIN32 */

// ====================== PLATFORM-INDEPENDENT PART ========================

//=======================================================================
//function : OSD_Chronometer
//purpose  :
//=======================================================================
OSD_Chronometer::OSD_Chronometer(const Standard_Boolean ThisThreadOnly)
{
  ThreadOnly = ThisThreadOnly;
  Start_user = Start_sys = 0.;
  Cumul_user = Cumul_sys = 0.;
  Stopped    = Standard_True;
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
  Stopped    = Standard_True;
  Start_user = Start_sys = 0.;
  Cumul_user = Cumul_sys = 0.;
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
void OSD_Chronometer::Stop ()
{
  if ( !Stopped ) {
    Standard_Real Curr_user, Curr_sys;
    if ( ThreadOnly )
      GetThreadCPU (Curr_user, Curr_sys);
    else
      GetProcessCPU (Curr_user, Curr_sys);

    Cumul_user += Curr_user - Start_user;
    Cumul_sys  += Curr_sys  - Start_sys;

    Stopped = Standard_True;
  }
//  else cerr << "WARNING: OSD_Chronometer already stopped !\n" << flush;
}

//=======================================================================
//function : Start
//purpose  :
//=======================================================================
void OSD_Chronometer::Start ()
{
  if ( Stopped ) {
    if ( ThreadOnly )
      GetThreadCPU (Start_user, Start_sys);
    else
      GetProcessCPU (Start_user, Start_sys);

    Stopped = Standard_False;
  }
//  else cerr << "WARNING: OSD_Chronometer already running !\n" << flush;
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
void OSD_Chronometer::Show() const
{
  Show (cout);
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
void OSD_Chronometer::Show (Standard_OStream& os) const
{
  Standard_Real aCumulUserSec = Cumul_user;
  Standard_Real aCumulSysSec  = Cumul_sys;
  if (!Stopped)
  {
    Standard_Real aCurrUser, aCurrSys;
    if (ThreadOnly)
      GetThreadCPU  (aCurrUser, aCurrSys);
    else
      GetProcessCPU (aCurrUser, aCurrSys);

    aCumulUserSec += aCurrUser - Start_user;
    aCumulSysSec  += aCurrSys  - Start_sys;
  }

  std::streamsize prec = os.precision (12);
  os << "CPU user time: "   << aCumulUserSec << " seconds " << endl;
  os << "CPU system time: " << aCumulSysSec  << " seconds " << endl;
  os.precision (prec);
}

//=======================================================================
//function : Show
//purpose  : Returns cpu user time
//=======================================================================
void OSD_Chronometer::Show (Standard_Real& theUserSec) const
{
  theUserSec = Cumul_user;
  if (Stopped)
  {
    return;
  }

  Standard_Real aCurrUser, aCurrSys;
  if (ThreadOnly)
    GetThreadCPU  (aCurrUser, aCurrSys);
  else
    GetProcessCPU (aCurrUser, aCurrSys);

  theUserSec += aCurrUser - Start_user;
}

//=======================================================================
//function : Show
//purpose  : Returns both user and system cpu times
//=======================================================================
void OSD_Chronometer::Show (Standard_Real& theUserSec,
                            Standard_Real& theSystemSec) const
{
  theUserSec   = Cumul_user;
  theSystemSec = Cumul_sys;
  if (Stopped)
  {
    return;
  }

  Standard_Real aCurrUser, aCurrSys;
  if (ThreadOnly)
    GetThreadCPU  (aCurrUser, aCurrSys);
  else
    GetProcessCPU (aCurrUser, aCurrSys);

  theUserSec   += aCurrUser - Start_user;
  theSystemSec += aCurrSys  - Start_sys;
}
