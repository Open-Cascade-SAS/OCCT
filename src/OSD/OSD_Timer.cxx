
// File:	OSD_Timer.cxx
// Created:	Fri Dec 4 15:02:28 1992
// Author:	Didier PIFFAULT , Mireille MERCIEN
//		<model@sdsun2>
//
// Update: J.P. TIRAULT Sep,1993
//         On heterogeneous platforms we need to use the 
//         system call gettimeofday. This function is portable and give us
//         elapsed time in seconds and microseconds.
//          

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <OSD_Timer.ixx>

#ifndef WNT

//---------- No Windows NT Systems ----------------------------------

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

//=======================================================================
//function : GetWallClockTime
//purpose  : Get current time in seconds with system-defined precision
//=======================================================================

static inline Standard_Real GetWallClockTime ()
{
  struct timeval tv;
  // use time of first call as base for computing total time,
  // to avoid loss of precision due to big values of tv_sec (counted since 1970)
  static time_t startSec = (gettimeofday (&tv, NULL) ? 0 : tv.tv_sec);
  return gettimeofday (&tv, NULL) ? 0. : (tv.tv_sec - startSec) + 0.000001 * tv.tv_usec;
}

#else

//-------------------  Windows NT  ------------------

#define STRICT
#include <windows.h>

//=======================================================================
//function : GetWallClockTime
//purpose  : Get current time in seconds with system-defined precision
//=======================================================================

static inline Standard_Real GetWallClockTime ()
{
  // compute clock frequence on first call
  static LARGE_INTEGER freq;
  static BOOL isOk = QueryPerformanceFrequency (&freq);

  LARGE_INTEGER time;
  return isOk && QueryPerformanceCounter (&time) ? 
         (Standard_Real)time.QuadPart / (Standard_Real)freq.QuadPart :
         0.000001 * GetTickCount();
}

#endif /* WNT */

// ====================== PLATFORM-INDEPENDENT PART ========================

//=======================================================================
//function : OSD_Timer
//purpose  : 
//=======================================================================

OSD_Timer::OSD_Timer()
{
  TimeStart = TimeCumul = 0.;
}

//=======================================================================
//function : Compute
//purpose  : Calcul les Heures,Minutes,Secondes,Millisecondes a partir
//           de deux variables input qui sont:
//           TimeCumulInt : Contient un periode de temps exprimee en secondes,
//           MicroCumulInt: Contient cette meme periode exprimee en 
//                       microsecondes.
//=======================================================================

static void Compute (Standard_Real     Time,
		     Standard_Integer& heure,
		     Standard_Integer& minut,
		     Standard_Real&    second) 
{
  Standard_Integer sec = (Standard_Integer)Time;
  heure = sec / 3600;
  minut = (sec - heure * 3600) / 60;
  second = Time - heure * 3600 - minut * 60;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void OSD_Timer::Reset ()
{
  TimeStart = TimeCumul = 0.;
  OSD_Chronometer::Reset();
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show ()
{
  Show (cout);
}


//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show (Standard_Real&    seconds,
                      Standard_Integer& minutes,
                      Standard_Integer& hours,
                      Standard_Real&    CPUtime) 
{
  Standard_Boolean StopSav=Stopped;
  if (!StopSav) Stop();

  Compute (TimeCumul, hours, minutes, seconds);
  OSD_Chronometer::Show(CPUtime);

  if (!StopSav) Start();
}


//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show (Standard_OStream& os)
{
  Standard_Boolean StopSav=Stopped;
  if (!StopSav) Stop();
  
  Standard_Integer heure,minut;
  Standard_Real    second;
  Compute (TimeCumul, heure, minut, second);

  std::streamsize prec = os.precision (12); 
  os << "Elapsed time: " << heure << " Hours " << 
                            minut << " Minutes " <<
                           second << " Seconds " << endl;
  OSD_Chronometer::Show(os);
  os.precision (prec);
  
  if (!StopSav) Start();  
}

//=======================================================================
//function : Stop
//purpose  : 
//=======================================================================

void OSD_Timer::Stop ()
{
  if (!Stopped) {
    TimeCumul += GetWallClockTime () - TimeStart;
    OSD_Chronometer::Stop();
  }
//  else cout << "WARNING: OSD_Timer already Stopped !\n";
}

//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

void OSD_Timer::Start()
{
  if (Stopped) {
    TimeStart = GetWallClockTime();
    OSD_Chronometer::Start();
  }
//  else cout << "WARNING: OSD_Timer already Running !\n";
}
