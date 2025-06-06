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

#ifndef _MoniTool_TimerSentry_HeaderFile
#define _MoniTool_TimerSentry_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
class MoniTool_Timer;

//! A tool to facilitate using MoniTool_Timer functionality
//! by automatically ensuring consistency of start/stop actions
//!
//! When instance of TimerSentry is created, a timer
//! with corresponding name is started
//! When instance is deleted, timer stops
class MoniTool_TimerSentry
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor creates an instance and runs the corresponding timer
  MoniTool_TimerSentry(const Standard_CString cname);

  //! Constructor creates an instance and runs the corresponding timer
  MoniTool_TimerSentry(const Handle(MoniTool_Timer)& timer);

  //! Destructor stops the associated timer
  ~MoniTool_TimerSentry();

  Handle(MoniTool_Timer) Timer() const;

  //! Manually stops the timer
  void Stop();

protected:
private:
  Handle(MoniTool_Timer) myTimer;
};

#include <MoniTool_TimerSentry.lxx>

#endif // _MoniTool_TimerSentry_HeaderFile
