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

#ifndef _OSD_Chronometer_HeaderFile
#define _OSD_Chronometer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>


//! This class measures CPU time (both user and system) consumed
//! by current process or thread. The chronometer can be started
//! and stopped multiple times, and measures cumulative time.
//!
//! If only the thread is measured, calls to Stop() and Show()
//! must occur from the same thread where Start() was called
//! (unless chronometer is stopped); otherwise measurement will
//! yield false values.
class OSD_Chronometer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes a stopped Chronometer.
  //!
  //! If ThisThreadOnly is True, measured CPU time will account
  //! time of the current thread only; otherwise CPU of the
  //! process (all threads, and completed children) is measured.
  Standard_EXPORT OSD_Chronometer(const Standard_Boolean ThisThreadOnly = Standard_False);
  Standard_EXPORT virtual ~OSD_Chronometer();

  //! Return true if timer has been started.
  Standard_Boolean IsStarted() const { return !Stopped; }

  //! Stops and Reinitializes the Chronometer.
  Standard_EXPORT virtual void Reset();

  //! Restarts the Chronometer.
  Standard_EXPORT virtual void Restart();

  //! Stops the Chronometer.
  Standard_EXPORT virtual void Stop();
  
  //! Starts (after Create or Reset) or restarts (after Stop)
  //! the chronometer.
  Standard_EXPORT virtual void Start();
  
  //! Shows the current CPU user and system time on the
  //! standard output stream <cout>.
  //! The chronometer can be running (laps Time) or stopped.
  Standard_EXPORT virtual void Show() const;
  
  //! Shows the current CPU user and system time on the output
  //! stream <os>.
  //! The chronometer can be running (laps Time) or stopped.
  Standard_EXPORT virtual void Show (Standard_OStream& os) const;
  
  //! Returns the current CPU user time in a variable.
  //! The chronometer can be running (laps Time) or stopped.
  Standard_EXPORT void Show (Standard_Real& theUserSeconds) const;
  
  //! Returns the current CPU user and system time in variables.
  //! The chronometer can be running (laps Time) or stopped.
  Standard_EXPORT void Show (Standard_Real& theUserSeconds, Standard_Real& theSystemSeconds) const;
  
  //! Returns CPU time (user and system) consumed by the current
  //! process since its start, in seconds. The actual precision of
  //! the measurement depends on granularity provided by the system,
  //! and is platform-specific.
  Standard_EXPORT static void GetProcessCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds);
  
  //! Returns CPU time (user and system) consumed by the current
  //! thread since its start. Note that this measurement is
  //! platform-specific, as threads are implemented and managed
  //! differently on different platforms and CPUs.
  Standard_EXPORT static void GetThreadCPU (Standard_Real& UserSeconds, Standard_Real& SystemSeconds);




protected:



  Standard_Boolean Stopped;


private:



  Standard_Boolean ThreadOnly;
  Standard_Real Start_user;
  Standard_Real Start_sys;
  Standard_Real Cumul_user;
  Standard_Real Cumul_sys;


};







#endif // _OSD_Chronometer_HeaderFile
