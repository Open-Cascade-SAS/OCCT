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
#include <Standard_Integer.hxx>


//! Working on heterogeneous platforms
//! we need to use the system call gettimeofday.
//! This function is portable and it measures ELAPSED
//! time and CPU time in seconds and microseconds.
//! Example: OSD_Timer aTimer;
//! aTimer.Start();   // Start  the timers (t1).
//! .....            // Do something.
//! aTimer.Stop();    // Stop the timers (t2).
//! aTimer.Show();    // Give the elapsed time between t1 and t2.
//! // Give also the process CPU time between
//! // t1 and t2.
class OSD_Timer  : public OSD_Chronometer
{
public:

  DEFINE_STANDARD_ALLOC

  //! Builds a Chronometer initialized and stopped.
  //! @param theThisThreadOnly when TRUE, measured CPU time will account time of the current thread only;
  //!                          otherwise CPU of the process (all threads, and completed children) is measured;
  //!                          this flag does NOT affect ElapsedTime() value, only values returned by OSD_Chronometer
  Standard_EXPORT OSD_Timer (Standard_Boolean theThisThreadOnly = Standard_False);

  //! Stops and reinitializes the timer with specified elapsed time.
  Standard_EXPORT void Reset (const Standard_Real theTimeElapsedSec);

  //! Stops and reinitializes the timer with zero elapsed time.
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Restarts the Timer.
  Standard_EXPORT virtual void Restart() Standard_OVERRIDE;

  //! Shows both the elapsed time and CPU time on the standard output
  //! stream <cout>.The chronometer can be running (Lap Time) or
  //! stopped.
  Standard_EXPORT virtual void Show() const Standard_OVERRIDE;
  
  //! Shows both the elapsed time and CPU  time on the
  //! output stream <OS>.
  Standard_EXPORT virtual void Show (Standard_OStream& os) const Standard_OVERRIDE;
  
  //! returns both the elapsed time(seconds,minutes,hours)
  //! and CPU  time.
  Standard_EXPORT void Show (Standard_Real& theSeconds, Standard_Integer& theMinutes, Standard_Integer& theHours, Standard_Real& theCPUtime) const;
  
  //! Stops the Timer.
  Standard_EXPORT virtual void Stop() Standard_OVERRIDE;
  
  //! Starts (after Create or Reset) or restarts (after Stop)
  //! the Timer.
  Standard_EXPORT virtual void Start() Standard_OVERRIDE;
  
  //! Returns elapsed time in seconds.
  Standard_EXPORT Standard_Real ElapsedTime() const;

private:

  Standard_Real myTimeStart;
  Standard_Real myTimeCumul;

};

#endif // _OSD_Timer_HeaderFile
