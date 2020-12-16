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

#include <OSD_Timer.hxx>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/time.h>
#endif

namespace
{
  //! Auxiliary function splits elapsed time in seconds into Hours, Minutes and Seconds.
  //! @param theTimeSec [in]  elapsed time in seconds
  //! @param theHours   [out] clamped elapsed hours
  //! @param theMinutes [out] clamped elapsed minutes within range [0, 59]
  //! @param theSeconds [out] clamped elapsed seconds within range [0, 60)
  static void timeToHoursMinutesSeconds (Standard_Real     theTimeSec,
                                         Standard_Integer& theHours,
                                         Standard_Integer& theMinutes,
                                         Standard_Real&    theSeconds)
  {
    Standard_Integer aSec = (Standard_Integer)theTimeSec;
    theHours   = aSec / 3600;
    theMinutes = (aSec - theHours * 3600) / 60;
    theSeconds = theTimeSec - theHours * 3600 - theMinutes * 60;
  }

#ifdef _WIN32
  //! Define a structure for initializing global constant of pair values.
  struct PerfCounterFreq
  {
    LARGE_INTEGER    Freq;
    Standard_Boolean IsOk;

    PerfCounterFreq()
    {
      IsOk = QueryPerformanceFrequency (&Freq) != FALSE;
    }
  };
#endif
}

//=======================================================================
//function : GetWallClockTime
//purpose  :
//=======================================================================
Standard_Real OSD_Timer::GetWallClockTime()
{
#ifdef _WIN32
  // compute clock frequence on first call
  static const PerfCounterFreq aFreq;

  LARGE_INTEGER aTime;
  return aFreq.IsOk && QueryPerformanceCounter (&aTime)
       ? (Standard_Real )aTime.QuadPart / (Standard_Real )aFreq.Freq.QuadPart
       #if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0600)
       : 0.001 * GetTickCount64();
       #else
       : 0.001 * GetTickCount();
       #endif
#else
  struct timeval aTime;
  // use time of first call as base for computing total time,
  // to avoid loss of precision due to big values of tv_sec (counted since 1970)
  static const time_t aStartSec = (gettimeofday (&aTime, NULL) == 0 ? aTime.tv_sec : 0);
  return gettimeofday (&aTime, NULL) == 0
       ? (aTime.tv_sec - aStartSec) + 0.000001 * aTime.tv_usec
       : 0.0;
#endif
}

//=======================================================================
//function : OSD_Timer
//purpose  : 
//=======================================================================

OSD_Timer::OSD_Timer (Standard_Boolean theThisThreadOnly)
: OSD_Chronometer (theThisThreadOnly),
  myTimeStart (0.0),
  myTimeCumul (0.0)
{
  //
}

//=======================================================================
//function : Reset
//purpose  :
//=======================================================================

void OSD_Timer::Reset (const Standard_Real theTimeElapsedSec)
{
  myTimeStart = 0.0;
  myTimeCumul = theTimeElapsedSec;
  OSD_Chronometer::Reset();
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void OSD_Timer::Reset ()
{
  myTimeStart = myTimeCumul = 0.0;
  OSD_Chronometer::Reset();
}

//=======================================================================
//function : Restart
//purpose  :
//=======================================================================

void OSD_Timer::Restart ()
{
  myTimeStart = GetWallClockTime();
  myTimeCumul = 0.0;
  OSD_Chronometer::Restart();
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show() const
{
  Show (std::cout);
}

//=======================================================================
//function : ElapsedTime
//purpose  :
//=======================================================================

Standard_Real OSD_Timer::ElapsedTime() const
{
  if (myIsStopped)
  {
    return myTimeCumul;
  }

  return myTimeCumul + GetWallClockTime() - myTimeStart;
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
  const Standard_Real aTimeCumul = myIsStopped
                                 ? myTimeCumul
                                 : myTimeCumul + GetWallClockTime() - myTimeStart;
  timeToHoursMinutesSeconds (aTimeCumul, theHours, theMinutes, theSeconds);
  OSD_Chronometer::Show (theCPUtime);
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void OSD_Timer::Show (Standard_OStream& theOStream) const
{
  const Standard_Real aTimeCumul = ElapsedTime();

  Standard_Integer anHours, aMinutes;
  Standard_Real    aSeconds;
  timeToHoursMinutesSeconds (aTimeCumul, anHours, aMinutes, aSeconds);

  std::streamsize prec = theOStream.precision (12);
  theOStream << "Elapsed time: " << anHours  << " Hours "   <<
                                    aMinutes << " Minutes " <<
                                    aSeconds << " Seconds\n";
  OSD_Chronometer::Show (theOStream);
  theOStream.precision (prec);
}

//=======================================================================
//function : Stop
//purpose  : 
//=======================================================================

void OSD_Timer::Stop ()
{
  if (!myIsStopped)
  {
    myTimeCumul += GetWallClockTime() - myTimeStart;
    OSD_Chronometer::Stop();
  }
}

//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

void OSD_Timer::Start()
{
  if (myIsStopped)
  {
    myTimeStart = GetWallClockTime();
    OSD_Chronometer::Start();
  }
}
