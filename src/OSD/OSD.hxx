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
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_CString.hxx>
class OSD_Error;
class OSD_Protection;
class OSD_Path;
class OSD_FileNode;
class OSD_Disk;
class OSD_File;
class OSD_FileIterator;
class OSD_Directory;
class OSD_DirectoryIterator;
class OSD_Timer;
class OSD_Host;
class OSD_Environment;
class OSD_EnvironmentIterator;
class OSD_Process;
class OSD_SharedLibrary;
class OSD_Thread;


//! Set of Operating Sytem Dependent Tools
//! (O)perating (S)ystem (D)ependent
class OSD 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Sets signal and exception handlers.
  //!
  //! ### Windows-specific notes
  //!
  //! Compiled with MS VC++ sets 3 main handlers:
  //! @li Signal handlers (via ::signal() functions) that translate system signals
  //! (SIGSEGV, SIGFPE, SIGILL) into C++ exceptions (classes inheriting
  //! Standard_Failure). They only be called if user calls ::raise() function
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
  //! If @a theFloatingSignal is TRUE then floating point exceptions will be
  //! generated in accordance with the mask
  //! <tt>_EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW</tt> that is
  //! used to call ::_controlfp() system function. If @a theFloatingSignal is FALSE
  //! corresponding operations (e.g. division by zero) will gracefully complete
  //! without an exception.
  //!
  //! ### Unix-specific notes
  //!
  //! OSD::SetSignal() sets handlers (via ::sigaction()) for multiple signals
  //! (SIGFPE, SIGSEGV, etc). Currently the number of handled signals is much
  //! greater than for Windows, in the future this may change to provide better
  //! consistency with Windows.
  //!
  //! @a theFloatingSignal is recognized on Sun Solaris, Linux, and SGI Irix to
  //! generate floating-point exception according to the mask
  //! <tt>FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW</tt> (in Linux conventions).<br>
  //! When compiled with OBJS macro defined, already set signal handlers (e.g.
  //! by Data Base Managers) are not redefined.
  //!
  //! ### Common notes
  //!
  //! If OSD::SetSignal() method is used in at least one thread, it must also be
  //! called in any other thread where Open CASCADE will be used, to ensure
  //! consistency of behavior. Its @a aFloatingSignal argument must be consistent
  //! across threads.
  //!
  //! Keep in mind that whether the C++ exception will really be thrown (i.e.
  //! ::throw() will be called) is regulated by the
  //! OCC_CONVERT_SIGNALS macro used during compilation of Open CASCADE and
  //! user's code. Refer to Foundation Classes User's Guide for further details.
  //!
  Standard_EXPORT static void SetSignal (const Standard_Boolean theFloatingSignal = Standard_True);
  
  //! Commands the process to sleep for a number of seconds.
  Standard_EXPORT static void SecSleep (const Standard_Integer aDelay);
  
  //! Commands the process to sleep for a number of milliseconds
  Standard_EXPORT static void MilliSecSleep (const Standard_Integer aDelay);
  
  //! Converts aReal into aCstring in exponential format with a period as
  //! decimal point, no thousand separator and no grouping of digits.
  //! The conversion is independant from the current locale
  Standard_EXPORT static Standard_Boolean RealToCString (const Standard_Real aReal, Standard_PCharacter& aString);

  //! Converts aCstring representing a real with a period as
  //! decimal point, no thousand separator and no grouping of digits
  //! into aReal .
  //! The conversion is independant from the current locale.
  Standard_EXPORT static Standard_Boolean CStringToReal (const Standard_CString aString, Standard_Real& aReal);
  
  //! since Windows NT does not support 'SIGINT' signal like UNIX,
  //! then this method checks whether Ctrl-Break keystroke was or
  //! not. If yes then raises Exception_CTRL_BREAK.
  Standard_EXPORT static void ControlBreak();

};

#endif // _OSD_HeaderFile
