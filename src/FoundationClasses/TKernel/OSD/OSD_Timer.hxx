// Created on: 2018-03-15
// Created by: Stephan GARNAUD (ARM)
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _OSD_Timer_HeaderFile
#define _OSD_Timer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <OSD_Chronometer.hxx>
#include <Standard_OStream.hxx>

//! Working on heterogeneous platforms
//! we need to use the system call gettimeofday.
//! This function is portable and it measures ELAPSED
//! time and CPU time in seconds and microseconds.
//! Example: OSD_Timer aTimer;
//! aTimer.Start();   // Start the timers (t1).
//! .....             // Do something.
//! aTimer.Stop();    // Stop the timers (t2).
//! aTimer.Show();    // Give the elapsed time between t1 and t2.
//! // Give also the process CPU time between
//! // t1 and t2.
class OSD_Timer : public OSD_Chronometer
{
public:
  //! Returns current time in seconds with system-defined precision.
  //! The could be a system uptime or a time from some date.
  //! Returned value is intended for precise elapsed time measurements as a delta between
  //! timestamps. On Windows implemented via QueryPerformanceCounter(), on other systems via
  //! gettimeofday().
  Standard_EXPORT static double GetWallClockTime();

public:
  DEFINE_STANDARD_ALLOC

  //! Builds a Chronometer initialized and stopped.
  //! @param theThisThreadOnly when TRUE, measured CPU time will account time of the current thread
  //! only;
  //!                          otherwise CPU of the process (all threads, and completed children) is
  //!                          measured; this flag does NOT affect ElapsedTime() value, only values
  //!                          returned by OSD_Chronometer
  Standard_EXPORT OSD_Timer(bool theThisThreadOnly = false);

  //! Stops and reinitializes the timer with specified elapsed time.
  Standard_EXPORT void Reset(const double theTimeElapsedSec);

  //! Stops and reinitializes the timer with zero elapsed time.
  Standard_EXPORT virtual void Reset() override;

  //! Restarts the Timer.
  Standard_EXPORT virtual void Restart() override;

  //! Shows both the elapsed time and CPU time on the standard output
  //! stream <cout>.The chronometer can be running (Lap Time) or
  //! stopped.
  Standard_EXPORT virtual void Show() const override;

  //! Shows both the elapsed time and CPU time on the
  //! output stream <OS>.
  Standard_EXPORT virtual void Show(Standard_OStream& os) const override;

  //! returns both the elapsed time(seconds,minutes,hours)
  //! and CPU time.
  Standard_EXPORT void Show(double&    theSeconds,
                            int& theMinutes,
                            int& theHours,
                            double&    theCPUtime) const;

  //! Stops the Timer.
  Standard_EXPORT virtual void Stop() override;

  //! Starts (after Create or Reset) or restarts (after Stop)
  //! the Timer.
  Standard_EXPORT virtual void Start() override;

  //! Returns elapsed time in seconds.
  Standard_EXPORT double ElapsedTime() const;

private:
  double myTimeStart;
  double myTimeCumul;
};

#endif // _OSD_Timer_HeaderFile
