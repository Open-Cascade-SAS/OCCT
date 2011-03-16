// File:        OSD_Chronometer.cxx
// Created:     Mon Nov 16 15:20:41 1992
// Author:      Mireille MERCIEN
//              <mip@sdsun3>

#include <OSD_Chronometer.ixx>
#include <Standard_Stream.hxx>

// ====================== PLATFORM-SPECIFIC PART ========================

#ifndef WNT

//---------- Systemes autres que WNT : ----------------------------------

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif

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

#if defined(HAVE_TIME_H) || defined(WNT) || defined(DECOSF1)
# include <time.h>
#endif

#  ifndef CLK_TCK
#   define CLK_TCK	CLOCKS_PER_SEC
#  endif

#ifdef HAVE_LIMITS
# include <limits>
#elif defined (HAVE_LIMITS_H)
# include <limits.h>
#endif

//=======================================================================
//function : GetProcessCPU
//purpose  : 
//=======================================================================
void OSD_Chronometer::GetProcessCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
#if defined(LIN) || defined(linux) || defined(__FreeBSD__)
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
void OSD_Chronometer::GetThreadCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
  UserSeconds = SystemSeconds = 0.;

#if defined(_POSIX_TIMERS) && defined(_POSIX_THREAD_CPUTIME)
  // on Linux, only user times are available for threads via clock_gettime()
  struct timespec t;
  if ( ! clock_gettime(CLOCK_THREAD_CPUTIME_ID,&t) )
  {
    UserSeconds = t.tv_sec + 0.000000001 * t.tv_nsec;
  }
#elif defined(SOLARIS)
  // on Solaris, both user and system times are available as LWP times
  struct rusage rut;
  if ( ! getrusage (RUSAGE_LWP, &rut) )
  {
    UserSeconds   = rut.ru_utime.tv_sec + 0.000001 * rut.ru_utime.tv_usec;
    SystemSeconds = rut.ru_stime.tv_sec + 0.000001 * rut.ru_stime.tv_usec;
  }
#else
#pragma error "OS is not supported yet; code to be ported"
#endif      
}

#else

//---------------------------- Systeme WNT --------------------------------

#define STRICT
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
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetProcessTimes (GetCurrentProcess(), &ftStart, &ftExit, &ftKernel, &ftUser);
  UserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  SystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
}

//=======================================================================
//function : GetThreadCPU
//purpose  : 
//=======================================================================
void OSD_Chronometer::GetThreadCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds)
{
  FILETIME ftStart, ftExit, ftKernel, ftUser;
  ::GetThreadTimes (GetCurrentThread(), &ftStart, &ftExit, &ftKernel, &ftUser);
  UserSeconds   = 0.0000001 * EncodeFILETIME (&ftUser);
  SystemSeconds = 0.0000001 * EncodeFILETIME (&ftKernel);
}

#endif /* WNT */

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
//function :  Destroy
//purpose  : 
//=======================================================================
void OSD_Chronometer::Destroy () 
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
void OSD_Chronometer::Show ()
{
  Show (cout);
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================
void OSD_Chronometer::Show (Standard_OStream& os)
{
  Standard_Boolean StopSav = Stopped;
  if (!StopSav) Stop();
  std::streamsize prec = os.precision (12); 
  os << "CPU user time: "   << Cumul_user  << " seconds " << endl;
  os << "CPU system time: " << Cumul_sys   << " seconds " << endl;
  os.precision (prec);
  if (!StopSav) Start();
}

//=======================================================================
//function : Show
//purpose  : Returns cpu user time
//=======================================================================
void OSD_Chronometer::Show (Standard_Real& second)
{
  Standard_Boolean StopSav = Stopped;
  if (!StopSav) Stop();
  second = Cumul_user; 
  if (!StopSav) Start();
}
//=======================================================================
//function : Show
//purpose  : Returns both user and system cpu times
//=======================================================================
void OSD_Chronometer::Show (Standard_Real& user,
                            Standard_Real& system)
{
  Standard_Boolean StopSav = Stopped;
  if (!StopSav) Stop();
  user = Cumul_user; 
  system = Cumul_sys; 
  if (!StopSav) Start();
}

