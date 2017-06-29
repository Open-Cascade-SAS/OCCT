// Created on: 1992-12-04
// Created by: Didier PIFFAULT , Mireille MERCIEN
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

// Update: J.P. TIRAULT Sep,1993
//         On heterogeneous platforms we need to use the 
//         system call gettimeofday. This function is portable and give us
//         elapsed time in seconds and microseconds.

#include <OSD_Timer.hxx>

#ifndef _WIN32

//---------- No Windows NT Systems ----------------------------------

#include <sys/time.h>

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
#ifndef OCCT_UWP
         0.001 * GetTickCount();
#else
         0.001 * GetTickCount64();
#endif
}

#endif /* _WIN32 */

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

void OSD_Timer::Reset (const Standard_Real theTimeElapsedSec)
{
  TimeStart = 0.0;
  TimeCumul = theTimeElapsedSec;
  OSD_Chronometer::Reset();
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
//function : Restart
//purpose  :
//=======================================================================

void OSD_Timer::Restart ()
{
  TimeStart = GetWallClockTime();
  TimeCumul = 0.;
  OSD_Chronometer::Restart();
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show() const
{
  Show (cout);
}

//=======================================================================
//function : ElapsedTime
//purpose  :
//=======================================================================

Standard_Real OSD_Timer::ElapsedTime() const
{
  if (Stopped)
  {
    return TimeCumul;
  }

  return TimeCumul + GetWallClockTime() - TimeStart;
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show (Standard_Real&    theSeconds,
                      Standard_Integer& theMinutes,
                      Standard_Integer& theHours,
                      Standard_Real&    theCPUtime) const
{
  const Standard_Real aTimeCumul = Stopped
                                 ? TimeCumul
                                 : TimeCumul + GetWallClockTime() - TimeStart;
  Compute (aTimeCumul, theHours, theMinutes, theSeconds);
  OSD_Chronometer::Show (theCPUtime);
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show (Standard_OStream& os) const
{
  const Standard_Real aTimeCumul = Stopped
                                 ? TimeCumul
                                 : TimeCumul + GetWallClockTime() - TimeStart;

  Standard_Integer anHours, aMinutes;
  Standard_Real    aSeconds;
  Compute (aTimeCumul, anHours, aMinutes, aSeconds);

  std::streamsize prec = os.precision (12);
  os << "Elapsed time: " << anHours  << " Hours "   <<
                            aMinutes << " Minutes " <<
                            aSeconds << " Seconds " << endl;
  OSD_Chronometer::Show(os);
  os.precision (prec);
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
