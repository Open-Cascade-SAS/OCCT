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

#ifndef _OSD_HeaderFile
#define _OSD_HeaderFile

#include <Standard.hxx>
#include <Standard_PCharacter.hxx>
#include <OSD_SignalMode.hxx>

//! Set of Operating System Dependent (OSD) tools.
class OSD
{
public:

  DEFINE_STANDARD_ALLOC

  //! Sets or removes signal and FPE (floating-point exception) handlers.
  //! OCCT signal handlers translate signals raised by C subsystem to C++
  //! exceptions inheriting Standard_Failure.
  //!
  //! ### Windows-specific notes
  //!
  //! Compiled with MS VC++ sets 3 main handlers:
  //! @li Signal handlers (via ::signal() functions) that translate system signals
  //! (SIGSEGV, SIGFPE, SIGILL) into C++ exceptions (classes inheriting
  //! Standard_Failure). They only be called if function ::raise() is called
  //! with one of supported signal type set.
  //! @li Exception handler OSD::WntHandler() (via ::SetUnhandledExceptionFilter())
  //! that will be used when user's code is compiled with /EHs option.
  //! @li Structured exception (SE) translator (via _set_se_translator()) that
  //! translates SE exceptions (aka asynchronous exceptions) into the
  //! C++ exceptions inheriting Standard_Failure. This translator will be
  //! used when user's code is compiled with /EHa option.
  //!
  //! This approach ensures that regardless of the option the user chooses to
  //! compile his code with (/EHs or /EHa), signals (or SE exceptions) will be
  //! translated into Open CASCADE C++ exceptions.
  //!
  //! MinGW should use SEH exception mode for signal handling to work.
  //!
  //! ### Linux-specific notes
  //!
  //! OSD::SetSignal() sets handlers (via ::sigaction()) for multiple signals
  //! (SIGFPE, SIGSEGV, etc).
  //!
  //! ### Common notes
  //!
  //! If @a theFloatingSignal is TRUE then floating point exceptions will
  //! generate SIGFPE in accordance with the mask
  //! - Windows: _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW,
  //!            see _controlfp() system function.
  //! - Linux:   FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW,
  //!            see feenableexcept() system function.
  //!
  //! If @a theFloatingSignal is FALSE then floating point calculations will gracefully
  //! complete regardless of occurred exceptions (e.g. division by zero).
  //! Otherwise the (thread-specific) FPE flags are set to raise signal if one of
  //! floating-point exceptions (division by zero, overflow, or invalid operation) occurs.
  //!
  //! The recommended approach is to call OSD::SetSignal() in the beginning of the 
  //! execution of the program, in function main() or its equivalent.
  //! In multithreaded programs it is advisable to call OSD::SetSignal() or
  //! OSD::SetThreadLocalSignal() with the same parameters in other threads where 
  //! OCCT is used, to ensure consistency of behavior.
  //!
  //! Note that in order to handle signals as C++ exceptions on Linux and under 
  //! MinGW on Windows it is necessary to compile both OCCT and application with
  //! OCC_CONVERT_SIGNALS macro, and use macro OCC_CATCH_SIGNALS within each try{}
  //! block that has to catch this kind of exceptions. 
  //! 
  //! Refer to documentation of Standard_ErrorHandler.hxx for details.
  Standard_EXPORT static void SetSignal (OSD_SignalMode theSignalMode,
                                         Standard_Boolean theFloatingSignal);

  //! Sets signal and FPE handlers.
  //! Short-cut for OSD::SetSignal (OSD_SignalMode_Set, theFloatingSignal).
  static void SetSignal (const Standard_Boolean theFloatingSignal = Standard_True)
  {
    SetSignal (OSD_SignalMode_Set, theFloatingSignal);
  }

  //! Initializes thread-local signal handlers.
  //! This includes _set_se_translator() on Windows platform, and SetFloatingSignal().
  //! The main purpose of this method is initializing handlers for newly created threads
  //! without overriding global handlers (set by application or by OSD::SetSignal()).
  Standard_EXPORT static void SetThreadLocalSignal (OSD_SignalMode theSignalMode,
                                                    Standard_Boolean theFloatingSignal);

  //! Enables / disables generation of C signal on floating point exceptions (FPE).
  //! This call does NOT register a handler for signal raised in case of FPE -
  //! SetSignal() should be called beforehand for complete setup.
  //! Note that FPE setting is thread-local, new threads inherit it from parent.
  Standard_EXPORT static void SetFloatingSignal (Standard_Boolean theFloatingSignal);

  //! Returns signal mode set by the last call to SetSignal().
  //! By default, returns OSD_SignalMode_AsIs.
  Standard_EXPORT static OSD_SignalMode SignalMode();

  //! Returns true if floating point exceptions will raise C signal
  //! according to current (platform-dependent) settings in this thread.
  Standard_EXPORT static Standard_Boolean ToCatchFloatingSignals();

  //! Commands the process to sleep for a number of seconds.
  Standard_EXPORT static void SecSleep (const Standard_Integer theSeconds);

  //! Commands the process to sleep for a number of milliseconds
  Standard_EXPORT static void MilliSecSleep (const Standard_Integer theMilliseconds);

  //! Converts aReal into aCstring in exponential format with a period as decimal point,
  //! no thousand separator and no grouping of digits.
  //! The conversion is independent from the current locale
  Standard_EXPORT static Standard_Boolean RealToCString (const Standard_Real aReal, Standard_PCharacter& aString);

  //! Converts aCstring representing a real with a period as decimal point,
  //! no thousand separator and no grouping of digits into aReal.
  //!
  //! The conversion is independent from the current locale.
  Standard_EXPORT static Standard_Boolean CStringToReal (const Standard_CString aString, Standard_Real& aReal);

  //! since Windows NT does not support 'SIGINT' signal like UNIX,
  //! then this method checks whether Ctrl-Break keystroke was or
  //! not. If yes then raises Exception_CTRL_BREAK.
  Standard_EXPORT static void ControlBreak();

  //! Returns a length of stack trace to be put into exception redirected from signal;
  //! 0 by default meaning no stack trace.
  //! @sa Standard_Failure::GetStackString()
  Standard_EXPORT static Standard_Integer SignalStackTraceLength();

  //! Sets a length of stack trace to be put into exception redirected from signal.
  Standard_EXPORT static void SetSignalStackTraceLength (Standard_Integer theLength);

};

#endif // _OSD_HeaderFile
