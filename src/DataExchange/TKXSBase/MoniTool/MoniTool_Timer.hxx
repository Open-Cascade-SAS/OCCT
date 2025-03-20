// Created on: 2001-12-13
// Created by: Sergey KUUl
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _MoniTool_Timer_HeaderFile
#define _MoniTool_Timer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <OSD_Timer.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_CString.hxx>
#include <MoniTool_DataMapOfTimer.hxx>

class MoniTool_Timer;
DEFINE_STANDARD_HANDLE(MoniTool_Timer, Standard_Transient)

//! Provides convenient service on global timers
//! accessed by string name, mostly aimed for debugging purposes
//!
//! As an instance, envelopes the OSD_Timer to have it as Handle
//!
//! As a tool, supports static dictionary of timers
//! and provides static methods to easily access them
class MoniTool_Timer : public Standard_Transient
{

public:
  //! Create timer in empty state
  MoniTool_Timer();

  const OSD_Timer& Timer() const;

  //! Return reference to embedded OSD_Timer
  OSD_Timer& Timer();

  void Start();

  void Stop();

  //! Start, Stop and reset the timer
  //! In addition to doing that to embedded OSD_Timer,
  //! manage also counter of hits
  void Reset();

  //! Return value of hits counter (count of Start/Stop pairs)
  Standard_Integer Count() const;

  //! Returns value of nesting counter
  Standard_Integer IsRunning() const;

  //! Return value of CPU time minus accumulated amendment
  Standard_Real CPU();

  //! Return value of accumulated amendment on CPU time
  Standard_Real Amend() const;

  //! Dumps current state of a timer shortly (one-line output)
  Standard_EXPORT void Dump(Standard_OStream& ostr);

  //! Returns a timer from a dictionary by its name
  //! If timer not existed, creates a new one
  Standard_EXPORT static Handle(MoniTool_Timer) Timer(const Standard_CString name);

  static void Start(const Standard_CString name);

  //! Inline methods to conveniently start/stop timer by name
  //! Shortcut to Timer(name)->Start/Stop()
  static void Stop(const Standard_CString name);

  //! Returns map of timers
  Standard_EXPORT static MoniTool_DataMapOfTimer& Dictionary();

  //! Clears map of timers
  Standard_EXPORT static void ClearTimers();

  //! Dumps contents of the whole dictionary
  Standard_EXPORT static void DumpTimers(Standard_OStream& ostr);

  //! Computes and remembers amendments for times to
  //! access, start, and stop of timer, and estimates
  //! second-order error measured by 10 nested timers
  Standard_EXPORT static void ComputeAmendments();

  //! The computed amendmens are returned (for information only)
  Standard_EXPORT static void GetAmendments(Standard_Real& Access,
                                            Standard_Real& Internal,
                                            Standard_Real& External,
                                            Standard_Real& Error10);

  DEFINE_STANDARD_RTTIEXT(MoniTool_Timer, Standard_Transient)

protected:
private:
  Standard_EXPORT static void AmendAccess();

  Standard_EXPORT void AmendStart();

  //! Internal functions to amend other timers to avoid
  //! side effects of operations with current one
  Standard_EXPORT void AmendStop();

  OSD_Timer              myTimer;
  Standard_Integer       myCount;
  Standard_Integer       myNesting;
  Standard_Real          myAmend;
  Handle(MoniTool_Timer) myPrev;
  Handle(MoniTool_Timer) myNext;
};

#include <MoniTool_Timer.lxx>

#endif // _MoniTool_Timer_HeaderFile
