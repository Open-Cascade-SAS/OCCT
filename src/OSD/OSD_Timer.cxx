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

#ifndef _WIN32

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
  Show (cout);
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
