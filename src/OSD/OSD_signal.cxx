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


#include <OSD.hxx>
#include <OSD_Exception_CTRL_BREAK.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_Overflow.hxx>
#include <Standard_Assert.hxx>

#include <Standard_WarningDisableFunctionCast.hxx>

static OSD_SignalMode OSD_WasSetSignal = OSD_SignalMode_AsIs;
static Standard_Integer OSD_SignalStackTraceLength = 0;

//=======================================================================
//function : SignalMode
//purpose  :
//=======================================================================
OSD_SignalMode OSD::SignalMode()
{
  return OSD_WasSetSignal;
}

// =======================================================================
// function : SignalStackTraceLength
// purpose  :
// =======================================================================
Standard_Integer OSD::SignalStackTraceLength()
{
  return OSD_SignalStackTraceLength;
}

// =======================================================================
// function : SetSignalStackTraceLength
// purpose  :
// =======================================================================
void OSD::SetSignalStackTraceLength (Standard_Integer theLength)
{
  OSD_SignalStackTraceLength = theLength;
}

#ifdef _WIN32
//---------------------------- Windows NT System --------------------------------

#ifdef NOUSER
#undef NOUSER
#endif
#ifdef NONLS
#undef NONLS
#endif
#include <windows.h>

#include <strsafe.h>

#ifndef STATUS_FLOAT_MULTIPLE_FAULTS
  // <ntstatus.h>
  #define STATUS_FLOAT_MULTIPLE_FAULTS     (0xC00002B4L)
  #define STATUS_FLOAT_MULTIPLE_TRAPS      (0xC00002B5L)
#endif

#include <OSD_Exception_ACCESS_VIOLATION.hxx>
#include <OSD_Exception_ARRAY_BOUNDS_EXCEEDED.hxx>
#include <OSD_Exception_ILLEGAL_INSTRUCTION.hxx>
#include <OSD_Exception_IN_PAGE_ERROR.hxx>
#include <OSD_Exception_INT_OVERFLOW.hxx>
#include <OSD_Exception_INVALID_DISPOSITION.hxx>
#include <OSD_Exception_NONCONTINUABLE_EXCEPTION.hxx>
#include <OSD_Exception_PRIV_INSTRUCTION.hxx>
#include <OSD_Exception_STACK_OVERFLOW.hxx>
#include <OSD_Exception_STATUS_NO_MEMORY.hxx>

#include <OSD_Environment.hxx>
#include <Standard_Underflow.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Mutex.hxx>

#ifdef _MSC_VER
#include <eh.h>
#include <malloc.h>
#endif

#include <process.h>
#include <signal.h>
#include <float.h>

static Standard_Boolean fCtrlBrk;

static Standard_Boolean fMsgBox;

// used to forbid simultaneous execution of setting / executing handlers
static Standard_Mutex THE_SIGNAL_MUTEX;

static LONG __fastcall _osd_raise (DWORD theCode, const char* theMsg, const char* theStack);
static BOOL WINAPI     _osd_ctrl_break_handler ( DWORD );

#if ! defined(OCCT_UWP) && !defined(__MINGW32__) && !defined(__CYGWIN32__)
static Standard_Boolean fDbgLoaded;
static LONG _osd_debug   ( void );
#endif

# define _OSD_FPX ( _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW )

#ifdef OCC_CONVERT_SIGNALS
#define THROW_OR_JUMP(Type,Message,Stack) Type::NewInstance(Message,Stack)->Jump()
#else
#define THROW_OR_JUMP(Type,Message,Stack) throw Type(Message,Stack)
#endif

//=======================================================================
//function : CallHandler
//purpose  :
//=======================================================================
static LONG CallHandler (DWORD theExceptionCode,
                         EXCEPTION_POINTERS* theExcPtr)
{
  ptrdiff_t ExceptionInformation1 = 0, ExceptionInformation0 = 0;
  if (theExcPtr != NULL)
  {
    ExceptionInformation1 = theExcPtr->ExceptionRecord->ExceptionInformation[1];
    ExceptionInformation0 = theExcPtr->ExceptionRecord->ExceptionInformation[0];
  }

  Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
  static char aBuffer[2048];

  bool isFloatErr = false;
  aBuffer[0] = '\0';
  switch (theExceptionCode)
  {
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT DENORMAL OPERAND");
      isFloatErr = true;
      break;
    }
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT DIVIDE BY ZERO");
      isFloatErr = true;
      break;
     }
    case EXCEPTION_FLT_INEXACT_RESULT:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT INEXACT RESULT");
      isFloatErr = true;
      break;
    }
    case EXCEPTION_FLT_INVALID_OPERATION:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT INVALID OPERATION");
      isFloatErr = true;
      break;
    }
    case EXCEPTION_FLT_OVERFLOW:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT OVERFLOW");
      isFloatErr = true;
      break;
    }
    case EXCEPTION_FLT_STACK_CHECK:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT STACK CHECK");
      isFloatErr = true;
      break;
    }
    case EXCEPTION_FLT_UNDERFLOW:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT UNDERFLOW");
      isFloatErr = true;
      break;
    }
    case STATUS_FLOAT_MULTIPLE_TRAPS:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT MULTIPLE TRAPS (possible overflow in conversion of double to integer)");
      isFloatErr = true;
      break;
    }
    case STATUS_FLOAT_MULTIPLE_FAULTS:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "FLT MULTIPLE FAULTS");
      isFloatErr = true;
      break;
    }
    case STATUS_NO_MEMORY:
    {
      THROW_OR_JUMP (OSD_Exception_STATUS_NO_MEMORY, "MEMORY ALLOCATION ERROR ( no room in the process heap )", NULL);
      break;
    }
    case EXCEPTION_ACCESS_VIOLATION:
    {
      _snprintf_s (aBuffer, sizeof(aBuffer), _TRUNCATE, "%s%s%s0x%.8p%s%s%s", "ACCESS VIOLATION",
                   fMsgBox ? "\n" : " ",
                   "at address ", (void* )ExceptionInformation1,
                   " during '", ExceptionInformation0 ? "WRITE" : "READ", "' operation");
      break;
    }
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "ARRAY BOUNDS EXCEEDED");
      break;
    }
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "DATATYPE MISALIGNMENT");
      break;
    }
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "ILLEGAL INSTRUCTION");
      break;
    }
    case EXCEPTION_IN_PAGE_ERROR:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "IN_PAGE ERROR");
      break;
    }
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "INTEGER DIVISION BY ZERO");
      break;
    }
    case EXCEPTION_INT_OVERFLOW:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "INTEGER OVERFLOW");
      break;
    }
    case EXCEPTION_INVALID_DISPOSITION:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "INVALID DISPOSITION");
      break;
    }
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "NONCONTINUABLE EXCEPTION");
      break;
    }
    case EXCEPTION_PRIV_INSTRUCTION:
    {
      strcat_s (aBuffer, sizeof(aBuffer), "PRIVILEGED INSTRUCTION ENCOUNTERED");
      break;
    }
    case EXCEPTION_STACK_OVERFLOW:
    {
#if defined( _MSC_VER ) && ( _MSC_VER >= 1300 ) && !defined(OCCT_UWP)
      // try recovering from stack overflow: available in MS VC++ 7.0
      if (!_resetstkoflw())
      {
        strcat_s (aBuffer, sizeof(aBuffer), "Unrecoverable STACK OVERFLOW");
      }
      else
#endif
      {
        strcat_s (aBuffer, sizeof(aBuffer), "STACK OVERFLOW");
      }
      break;
    }
    default:
    {
      _snprintf_s (aBuffer, sizeof(aBuffer), _TRUNCATE, "unknown exception code 0x%x, params 0x%p 0x%p",
                   theExceptionCode, (void* )ExceptionInformation1, (void* )ExceptionInformation0);
    }
  }

  // reset FPE state (before message box, otherwise it may fail to show up)
  if (isFloatErr)
  {
    OSD::SetFloatingSignal (Standard_True);
  }

  const int aStackLength = OSD_SignalStackTraceLength;
  const int aStackBufLen = Max (aStackLength * 200, 2048);
  char* aStackBuffer = aStackLength != 0 ? (char* )alloca (aStackBufLen) : NULL;
  if (aStackBuffer != NULL)
  {
    memset (aStackBuffer, 0, aStackBufLen);
    Standard::StackTrace (aStackBuffer, aStackBufLen, aStackLength, theExcPtr->ContextRecord);
  }

#if !defined(OCCT_UWP) && !defined(__MINGW32__) && !defined(__CYGWIN32__)
  // provide message to the user with possibility to stop
  if (aBuffer[0] != '\0'
   && fMsgBox
   && theExceptionCode != EXCEPTION_NONCONTINUABLE_EXCEPTION)
  {
    MessageBeep (MB_ICONHAND);
    char aMsgBoxBuffer[2048];
    strcat_s (aMsgBoxBuffer, sizeof(aMsgBoxBuffer), aBuffer);
    if (aStackBuffer != NULL)
    {
      strcat_s (aMsgBoxBuffer, sizeof(aMsgBoxBuffer), aStackBuffer);
    }
    int aChoice = ::MessageBoxA (0, aMsgBoxBuffer, "OCCT Exception Handler", MB_ABORTRETRYIGNORE | MB_ICONSTOP);
    if (aChoice == IDRETRY)
    {
      _osd_debug();
      DebugBreak();
    }
    else if (aChoice == IDABORT)
    {
      exit (0xFFFF);
    }
  }
#endif

  return _osd_raise (theExceptionCode, aBuffer, aStackBuffer);
}

//=======================================================================
//function : SIGWntHandler
//purpose  : Will only be used if user calls ::raise() function with
//           signal type set in OSD::SetSignal() - SIGSEGV, SIGFPE, SIGILL
//           (the latter will likely be removed in the future)
//=======================================================================
static void SIGWntHandler (int signum, int sub_code)
{
  Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
  switch( signum ) {
    case SIGFPE :
      if ( signal( signum , (void(*)(int))SIGWntHandler ) == SIG_ERR )
        std::cout << "signal error" << std::endl ;
      switch( sub_code ) {
        case _FPE_INVALID :
          CallHandler (EXCEPTION_FLT_INVALID_OPERATION, NULL);
          break ;
        case _FPE_DENORMAL :
          CallHandler (EXCEPTION_FLT_DENORMAL_OPERAND, NULL);
          break ;
        case _FPE_ZERODIVIDE :
          CallHandler (EXCEPTION_FLT_DIVIDE_BY_ZERO, NULL);
          break ;
        case _FPE_OVERFLOW :
          CallHandler (EXCEPTION_FLT_OVERFLOW, NULL);
          break ;
        case _FPE_UNDERFLOW :
          CallHandler (EXCEPTION_FLT_UNDERFLOW, NULL);
          break ;
        case _FPE_INEXACT :
          CallHandler (EXCEPTION_FLT_INEXACT_RESULT, NULL);
          break ;
        default:
          std::cout << "SIGWntHandler(default) -> throw Standard_NumericError(\"Floating Point Error\");" << std::endl;
          THROW_OR_JUMP (Standard_NumericError, "Floating Point Error", NULL);
          break ;
      }
      break ;
    case SIGSEGV :
      if ( signal( signum, (void(*)(int))SIGWntHandler ) == SIG_ERR )
        std::cout << "signal error" << std::endl ;
      CallHandler (EXCEPTION_ACCESS_VIOLATION, NULL);
      break ;
    case SIGILL :
      if ( signal( signum, (void(*)(int))SIGWntHandler ) == SIG_ERR )
        std::cout << "signal error" << std::endl ;
      CallHandler (EXCEPTION_ILLEGAL_INSTRUCTION, NULL);
      break ;
    default:
      std::cout << "SIGWntHandler unexpected signal : " << signum << std::endl ;
      break ;
  }
#ifndef OCCT_UWP
  DebugBreak ();
#endif
}

//=======================================================================
//function : TranslateSE
//purpose  : Translate Structural Exceptions into C++ exceptions
//           Will be used when user's code is compiled with /EHa option
//=======================================================================
#ifdef _MSC_VER
// If this file compiled with the default MSVC options for exception
// handling (/GX or /EHsc) then the following warning is issued:
//   warning C4535: calling _set_se_translator() requires /EHa
// However it is correctly inserted and used when user's code compiled with /EHa.
// So, here we disable the warning.
#pragma warning (disable:4535)

static void TranslateSE( unsigned int theCode, EXCEPTION_POINTERS* theExcPtr )
{
  Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
  CallHandler (theCode, theExcPtr);
}
#endif

//=======================================================================
//function : WntHandler
//purpose  : Will be used when user's code is compiled with /EHs
//           option and unless user sets his own exception handler with
//           ::SetUnhandledExceptionFilter().
//=======================================================================
static LONG WINAPI WntHandler (EXCEPTION_POINTERS *lpXP)
{
  DWORD dwExceptionCode = lpXP->ExceptionRecord->ExceptionCode;
  return CallHandler (dwExceptionCode, lpXP);
}

//=======================================================================
//function : SetFloatingSignal
//purpose  :
//=======================================================================
void OSD::SetFloatingSignal (Standard_Boolean theFloatingSignal)
{
  _fpreset();
  _clearfp();
  
  // Note: zero bit means exception will be raised
  _controlfp (theFloatingSignal ? 0 : _OSD_FPX, _OSD_FPX);
}

//=======================================================================
//function : ToCatchFloatingSignals
//purpose  :
//=======================================================================
Standard_Boolean OSD::ToCatchFloatingSignals()
{
  // return true if at least one of bits within _OSD_FPX
  // is unset, which means relevant FPE will raise exception
  int aControlWord = _controlfp (0, 0);
  return (_OSD_FPX & ~aControlWord) != 0;
}

//=======================================================================
//function : SetThreadLocalSignal
//purpose  :
//=======================================================================
void OSD::SetThreadLocalSignal (OSD_SignalMode theSignalMode,
                                Standard_Boolean theFloatingSignal)
{
#ifdef _MSC_VER
  _se_translator_function aPreviousFunc = NULL;
  if (theSignalMode == OSD_SignalMode_Set || theSignalMode == OSD_SignalMode_SetUnhandled)
    aPreviousFunc = _set_se_translator(TranslateSE);
  if (theSignalMode == OSD_SignalMode_Unset || (theSignalMode == OSD_SignalMode_SetUnhandled && aPreviousFunc != NULL))
    _set_se_translator(aPreviousFunc);
#else
  (void)theSignalMode;
#endif
  SetFloatingSignal (theFloatingSignal);
}

//=======================================================================
//function : SetSignal
//purpose  :
//=======================================================================
void OSD::SetSignal (OSD_SignalMode theSignalMode,
                     Standard_Boolean theFloatingSignal)
{
  Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
  OSD_WasSetSignal = theSignalMode;

#if !defined(OCCT_UWP) || defined(NTDDI_WIN10_TH2)
  OSD_Environment env ("CSF_DEBUG_MODE");
  TCollection_AsciiString val = env.Value();
  if (!env.Failed())
  {
    std::cout << "Environment variable CSF_DEBUG_MODE set.\n";
    fMsgBox = Standard_True;
    if (OSD_SignalStackTraceLength == 0)
    {
      // enable stack trace if CSF_DEBUG_MODE is set
      OSD_SignalStackTraceLength = 10;
    }
  }
  else
  {
    fMsgBox = Standard_False;
  }

  // Set exception handler (ignored when running under debugger). It will be used in most cases
  // when user's code is compiled with /EHs
  // Replaces the existing top-level exception filter for all existing and all future threads
  // in the calling process
  {
    LPTOP_LEVEL_EXCEPTION_FILTER aPreviousFunc = NULL;
    if (theSignalMode == OSD_SignalMode_Set || theSignalMode == OSD_SignalMode_SetUnhandled)
    {
      aPreviousFunc = ::SetUnhandledExceptionFilter(WntHandler);
    }
    if (theSignalMode == OSD_SignalMode_Unset || (theSignalMode == OSD_SignalMode_SetUnhandled && aPreviousFunc != NULL))
    {
      ::SetUnhandledExceptionFilter(aPreviousFunc);
    }
  }
#endif // NTDDI_WIN10_TH2

  // Signal handlers will only be used when function ::raise() is called
  const int NBSIG = 3;
  const int aSignalTypes[NBSIG] = { SIGSEGV, SIGILL, SIGFPE };
  for (int i = 0; i < NBSIG; ++i)
  {
    typedef void (*SignalFuncType)(int); // same as _crt_signal_t available since vc14
    SignalFuncType aPreviousFunc = SIG_DFL;
    if (theSignalMode == OSD_SignalMode_Set || theSignalMode == OSD_SignalMode_SetUnhandled)
    {
      aPreviousFunc = signal(aSignalTypes[i], (SignalFuncType)SIGWntHandler);
    }
    if (theSignalMode == OSD_SignalMode_Unset ||
        (theSignalMode == OSD_SignalMode_SetUnhandled && aPreviousFunc != SIG_DFL && aPreviousFunc != SIG_ERR))
    {
      aPreviousFunc = signal(aSignalTypes[i], aPreviousFunc);
    }
    Standard_ASSERT(aPreviousFunc != SIG_ERR, "signal() failed", std::cout << "OSD::SetSignal(): signal() returns SIG_ERR");
  }

  // Set Ctrl-C and Ctrl-Break handler
  fCtrlBrk = Standard_False;
#ifndef OCCT_UWP
  if (theSignalMode == OSD_SignalMode_Set || theSignalMode == OSD_SignalMode_SetUnhandled)
  {
    SetConsoleCtrlHandler(&_osd_ctrl_break_handler, true);
  }
  else if (theSignalMode == OSD_SignalMode_Unset)
  {
    SetConsoleCtrlHandler(&_osd_ctrl_break_handler, false);
  }
#endif

  SetThreadLocalSignal (theSignalMode, theFloatingSignal);
}

//============================================================================
//==== ControlBreak
//============================================================================
void OSD::ControlBreak () {
  if ( fCtrlBrk ) {
    fCtrlBrk = Standard_False;
    throw OSD_Exception_CTRL_BREAK ( "*** INTERRUPT ***" );
  }
}  // end OSD :: ControlBreak

#ifndef OCCT_UWP
//============================================================================
//==== _osd_ctrl_break_handler
//============================================================================
static BOOL WINAPI _osd_ctrl_break_handler ( DWORD dwCode ) {
  if ( dwCode == CTRL_C_EVENT || dwCode == CTRL_BREAK_EVENT ) {
    MessageBeep ( MB_ICONEXCLAMATION );
    fCtrlBrk = Standard_True;
  } else
    exit ( 254 );

  return TRUE;
}  // end _osd_ctrl_break_handler
#endif

//============================================================================
//==== _osd_raise
//============================================================================
static LONG __fastcall _osd_raise (DWORD theCode, const char* theMsg, const char* theStack)
{
  const char* aMsg = theMsg;
  if (aMsg[0] == '\x03')
  {
    ++aMsg;
  }

  switch (theCode)
  {
    case EXCEPTION_ACCESS_VIOLATION:
      THROW_OR_JUMP (OSD_Exception_ACCESS_VIOLATION, aMsg, theStack);
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      THROW_OR_JUMP (OSD_Exception_ARRAY_BOUNDS_EXCEEDED, aMsg, theStack);
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      THROW_OR_JUMP (Standard_ProgramError, aMsg, theStack);
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      THROW_OR_JUMP (OSD_Exception_ILLEGAL_INSTRUCTION, aMsg, theStack);
      break;
    case EXCEPTION_IN_PAGE_ERROR:
      THROW_OR_JUMP (OSD_Exception_IN_PAGE_ERROR, aMsg, theStack);
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      THROW_OR_JUMP (Standard_DivideByZero, aMsg, theStack);
      break;
    case EXCEPTION_INT_OVERFLOW:
      THROW_OR_JUMP (OSD_Exception_INT_OVERFLOW, aMsg, theStack);
      break;
    case EXCEPTION_INVALID_DISPOSITION:
      THROW_OR_JUMP (OSD_Exception_INVALID_DISPOSITION, aMsg, theStack);
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      THROW_OR_JUMP (OSD_Exception_NONCONTINUABLE_EXCEPTION, aMsg, theStack);
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      THROW_OR_JUMP (OSD_Exception_PRIV_INSTRUCTION, aMsg, theStack);
      break;
    case EXCEPTION_STACK_OVERFLOW:
      THROW_OR_JUMP (OSD_Exception_STACK_OVERFLOW, aMsg, theStack);
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      THROW_OR_JUMP (Standard_DivideByZero, aMsg, theStack);
      break;
    case EXCEPTION_FLT_STACK_CHECK:
    case EXCEPTION_FLT_OVERFLOW:
      THROW_OR_JUMP (Standard_Overflow, aMsg, theStack);
      break;
    case EXCEPTION_FLT_UNDERFLOW:
      THROW_OR_JUMP (Standard_Underflow, aMsg, theStack);
      break;
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case STATUS_FLOAT_MULTIPLE_TRAPS:
    case STATUS_FLOAT_MULTIPLE_FAULTS:
      THROW_OR_JUMP (Standard_NumericError, aMsg, theStack);
      break;
    default:
      break;
  }
  return EXCEPTION_EXECUTE_HANDLER;
}

#if ! defined(OCCT_UWP) && !defined(__MINGW32__) && !defined(__CYGWIN32__)
//============================================================================
//==== _osd_debug
//============================================================================
LONG _osd_debug ( void ) {

  LONG action ;

  if ( !fDbgLoaded ) {

    HKEY                hKey = NULL;
    HANDLE              hEvent = INVALID_HANDLE_VALUE;
    DWORD               dwKeyType;
    DWORD               dwValueLen;
    TCHAR               keyValue[ MAX_PATH ];
    TCHAR               cmdLine[ MAX_PATH ];
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO         si;

    __try {

      if (  RegOpenKey (
              HKEY_LOCAL_MACHINE,
              TEXT( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug" ),
              &hKey
            ) != ERROR_SUCCESS
      ) __leave;

      dwValueLen = sizeof ( keyValue );

      if (  RegQueryValueEx (
             hKey, TEXT( "Debugger" ), NULL, &dwKeyType, ( unsigned char* )keyValue, &dwValueLen
            ) != ERROR_SUCCESS
      ) __leave;

      sa.nLength              = sizeof ( SECURITY_ATTRIBUTES );
      sa.lpSecurityDescriptor = NULL;
      sa.bInheritHandle       = TRUE;

      if (   (  hEvent = CreateEvent ( &sa, TRUE, FALSE, NULL )  ) == NULL   ) __leave;

      StringCchPrintf(cmdLine, _countof(cmdLine), keyValue, GetCurrentProcessId(), hEvent);

      ZeroMemory (  &si, sizeof ( STARTUPINFO )  );

      si.cb      = sizeof ( STARTUPINFO );
      si.dwFlags = STARTF_FORCEONFEEDBACK;

  //   std::cout << "_osd_debug -> CreateProcess" << std::endl ;
      if (  !CreateProcess (
              NULL, cmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE,
              NULL, NULL, &si, &pi
             )
      ) __leave;

  //   std::cout << "_osd_debug -> WaitForSingleObject " << std::endl ;
      WaitForSingleObject ( hEvent, INFINITE );
  //   std::cout << "_osd_debug <- WaitForSingleObject -> CloseHandle " << std::endl ;

      CloseHandle ( pi.hProcess );
      CloseHandle ( pi.hThread  );

  //   std::cout << "_osd_debug fDbgLoaded  " << std::endl ;
      fDbgLoaded = TRUE;

    }  // end __try

    __finally {

//   std::cout << "_osd_debug -> CloseHandle(hKey) " << std::endl ;
      if ( hKey   != INVALID_HANDLE_VALUE ) CloseHandle ( hKey   );
//   std::cout << "_osd_debug -> CloseHandle(hEvent) " << std::endl ;
      if ( hEvent != INVALID_HANDLE_VALUE ) CloseHandle ( hEvent );
//   std::cout << "_osd_debug end __finally " << std::endl ;

    }  // end __finally

  }  /* end if */

  action = fDbgLoaded ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER;
// std::cout << "_osd_debug return " << action << " EXCEPTION_CONTINUE_EXECUTION("
//      << EXCEPTION_CONTINUE_EXECUTION << ")" << std::endl ;
  return action ;

}  // end _osd_debug
#endif /* ! OCCT_UWP && ! __CYGWIN__ && ! __MINGW32__ */

#else /* ! _WIN32 */

//---------- All Systems except Windows NT : ----------------------------------

# include <stdio.h>

#include <OSD_WhoAmI.hxx>
#include <OSD_SIGHUP.hxx>
#include <OSD_SIGINT.hxx>
#include <OSD_SIGQUIT.hxx>
#include <OSD_SIGILL.hxx>
#include <OSD_SIGKILL.hxx>
#include <OSD_SIGBUS.hxx>
#include <OSD_SIGSEGV.hxx>
#include <OSD_SIGSYS.hxx>
#include <Standard_NumericError.hxx>

#include <Standard_ErrorHandler.hxx>

// POSIX threads
#include <pthread.h>

#ifdef __linux__
#include  <cfenv>
//#include  <fenv.h>
#endif

// variable signalling that Control-C has been pressed (SIGINT signal)
static Standard_Boolean fCtrlBrk;

//const OSD_WhoAmI Iam = OSD_WPackage;

typedef void (ACT_SIGIO_HANDLER)(void) ;
ACT_SIGIO_HANDLER *ADR_ACT_SIGIO_HANDLER = NULL ;

#ifdef __GNUC__
# include <stdlib.h>
# include <stdio.h>
#else
#  ifdef SA_SIGINFO
#  include <sys/siginfo.h>
#  endif
#endif
typedef void (* SIG_PFV) (int);

#include <signal.h>

#if !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__)
  #include <sys/signal.h>
#endif

# define _OSD_FPX (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW)

//============================================================================
//==== Handler
//====     Catch the different signals:
//====          1- The Fatal signals, which cause the end of process:
//====          2- The exceptions which are "signaled" by Raise.
//====     The Fatal Signals:
//====        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGKILL, SIGBUS, SIGSYS
//====     The Exceptions:
//====        SIGFPE
//====         (SUN versions)
//====           FPE_INTOVF_TRAP    // ..... integer overflow
//====           FPE_INTDIV_TRAP    // ..... integer divide by zero
//====           FPE_FLTINEX_TRAP   // ..... [floating inexact result]
//====           FPE_FLTDIV_TRAP    // ..... [floating divide by zero]
//====           FPE_FLTUND_TRAP    // ..... [floating underflow]
//====           FPE_FLTOPER_TRAP   // ..... [floating inexact result]
//====           FPE_FLTOVF_TRAP    // ..... [floating overflow]
//==== SIGSEGV is handled by "SegvHandler()"
//============================================================================
#ifdef SA_SIGINFO
static void Handler (const int theSignal, siginfo_t */*theSigInfo*/, const Standard_Address /*theContext*/)
#else
static void Handler (const int theSignal)
#endif
{
  struct sigaction oldact, act;
  // re-install the signal
  if ( ! sigaction (theSignal, NULL, &oldact) ) {
    // std::cout << " signal is " << theSignal << " handler is " <<  oldact.sa_handler << std::endl;
    if (sigaction (theSignal, &oldact, &act)) perror ("sigaction");
  }
  else {
    perror ("sigaction");
  }

  // std::cout << "OSD::Handler: signal " << (int) theSignal << " occurred inside a try block " <<  std::endl ;
  if ( ADR_ACT_SIGIO_HANDLER != NULL )
    (*ADR_ACT_SIGIO_HANDLER)() ;

  sigset_t set;
  sigemptyset(&set);
  switch (theSignal) {
  case SIGHUP:
    OSD_SIGHUP::NewInstance("SIGHUP 'hangup' detected.")->Jump();
    exit(SIGHUP);
    break;
  case SIGINT:
    // For safe handling of Control-C as stop event, arm a variable but do not
    // generate longjump (we are out of context anyway)
    fCtrlBrk = Standard_True;
    // OSD_SIGINT::NewInstance("SIGINT 'interrupt' detected.")->Jump();
    // exit(SIGINT);
    break;
  case SIGQUIT:
    OSD_SIGQUIT::NewInstance("SIGQUIT 'quit' detected.")->Jump();
    exit(SIGQUIT);
    break;
  case SIGILL:
    OSD_SIGILL::NewInstance("SIGILL 'illegal instruction' detected.")->Jump();
    exit(SIGILL);
    break;
  case SIGKILL:
    OSD_SIGKILL::NewInstance("SIGKILL 'kill' detected.")->Jump();
    exit(SIGKILL);
    break;
  case SIGBUS:
    sigaddset(&set, SIGBUS);
    sigprocmask(SIG_UNBLOCK, &set, NULL) ;
    OSD_SIGBUS::NewInstance("SIGBUS 'bus error' detected.")->Jump();
    exit(SIGBUS);
    break;
  case SIGSEGV:
    OSD_SIGSEGV::NewInstance("SIGSEGV 'segmentation violation' detected.")->Jump();
    exit(SIGSEGV);
    break;
#ifdef SIGSYS
  case SIGSYS:
    OSD_SIGSYS::NewInstance("SIGSYS 'bad argument to system call' detected.")->Jump();
    exit(SIGSYS);
    break;
#endif
  case SIGFPE:
    sigaddset(&set, SIGFPE);
    sigprocmask(SIG_UNBLOCK, &set, NULL) ;
#ifdef __linux__
    OSD::SetFloatingSignal (Standard_True);
#endif
#if (!defined (__sun)) && (!defined(SOLARIS))
    Standard_NumericError::NewInstance("SIGFPE Arithmetic exception detected")->Jump();
    break;
#else
    // Reste SOLARIS
    if (aSigInfo) {
      switch(aSigInfo->si_code) {
      case FPE_FLTDIV_TRAP :
        Standard_DivideByZero::NewInstance("Floating Divide By Zero")->Jump();
        break;
      case FPE_INTDIV_TRAP :
        Standard_DivideByZero::NewInstance("Integer Divide By Zero")->Jump();
        break;
      case FPE_FLTOVF_TRAP :
        Standard_Overflow::NewInstance("Floating Overflow")->Jump();
        break;
      case FPE_INTOVF_TRAP :
        Standard_Overflow::NewInstance("Integer Overflow")->Jump();
        break;
      case FPE_FLTUND_TRAP :
        Standard_NumericError::NewInstance("Floating Underflow")->Jump();
        break;
      case FPE_FLTRES_TRAP:
        Standard_NumericError::NewInstance("Floating Point Inexact Result")->Jump();
        break;
      case FPE_FLTINV_TRAP :
        Standard_NumericError::NewInstance("Invalid Floating Point Operation")->Jump();
        break;
      default:
        Standard_NumericError::NewInstance("Numeric Error")->Jump();
        break;
      }
    } else {
      Standard_NumericError::NewInstance("SIGFPE Arithmetic exception detected")->Jump();
    }
#endif
    break;
  default:
#ifdef OCCT_DEBUG
    std::cout << "Unexpected signal " << theSignal << std::endl ;
#endif
    break;
  }
}

//============================================================================
//==== SegvHandler
//============================================================================
#ifdef SA_SIGINFO

static void SegvHandler(const int theSignal,
                        siginfo_t* theSigInfo,
                        const Standard_Address theContext)
{
  (void)theSignal;
  (void)theContext;
  if (theSigInfo != NULL)
  {
    sigset_t set;
    sigemptyset (&set);
    sigaddset (&set, SIGSEGV);
    sigprocmask (SIG_UNBLOCK, &set, NULL);
    void* anAddress = theSigInfo->si_addr;
    {
      char aMsg[100];
      sprintf (aMsg, "SIGSEGV 'segmentation violation' detected. Address %lx.", (long )anAddress);

      const int aStackLength = OSD_SignalStackTraceLength;
      const int aStackBufLen = Max (aStackLength * 200, 2048);
      char* aStackBuffer = aStackLength != 0 ? (char* )alloca (aStackBufLen) : NULL;
      if (aStackBuffer != NULL)
      {
        memset (aStackBuffer, 0, aStackBufLen);
        Standard::StackTrace (aStackBuffer, aStackBufLen, aStackLength);
      }

      OSD_SIGSEGV::NewInstance (aMsg, aStackBuffer)->Jump();
    }
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Wrong undefined address." << std::endl;
  }
#endif
  exit (SIGSEGV);
}

#elif defined (_hpux) || defined(HPUX)
// Not ACTIVE ? SA_SIGINFO is defined on SUN, OSF, SGI and HP (and Linux) !
// pour version 09.07

static void SegvHandler(const int theSignal,
                        siginfo_t* theSigInfo,
                        const Standard_Address theContext)
{
  if (theContext != NULL)
  {
    unsigned long aSpace   = ((struct sigcontext *)theContext)->sc_sl.sl_ss.ss_cr20;
    unsigned long anOffset = ((struct sigcontext *)theContext)->sc_sl.sl_ss.ss_cr21;
    {
      char aMsg[100];
      sprintf (aMsg, "SIGSEGV 'segmentation violation' detected. Address %lx", anOffset);
      OSD_SIGSEGV::NewInstance (aMsg)->Jump();
    }
  }
#ifdef OCCT_DEBUG
  else
  {
    std::cout << "Wrong undefined address." << std::endl;
  }
#endif
  exit (SIGSEGV);
}

#endif

//=======================================================================
//function : SetFloatingSignal
//purpose  :
//=======================================================================
void OSD::SetFloatingSignal (Standard_Boolean theFloatingSignal)
{
#if defined (__linux__)
  feclearexcept (FE_ALL_EXCEPT);
  if (theFloatingSignal)
  {
    feenableexcept (_OSD_FPX);
  }
  else
  {
    fedisableexcept (_OSD_FPX);
  }
#elif defined (__sun) || defined (SOLARIS)
  int aSunStat = 0;
  sigfpe_handler_type anFpeHandler = (theFloatingSignal ? (sigfpe_handler_type)Handler : NULL);
  aSunStat = ieee_handler ("set", "invalid",  anFpeHandler);
  aSunStat = ieee_handler ("set", "division", anFpeHandler) || aSunStat;
  aSunStat = ieee_handler ("set", "overflow", anFpeHandler) || aSunStat;
  if (aSunStat)
  {
#ifdef OCCT_DEBUG
    std::cerr << "ieee_handler does not work !!! KO\n";
#endif
  }
#else
  (void)theFloatingSignal;
#endif
}

//=======================================================================
//function : ToCatchFloatingSignals
//purpose  :
//=======================================================================
Standard_Boolean OSD::ToCatchFloatingSignals()
{
#if defined (__linux__)
  return (fegetexcept() & _OSD_FPX) != 0;
#else
  return Standard_False;
#endif
}

//=======================================================================
//function : SetThreadLocalSignal
//purpose  :
//=======================================================================
void OSD::SetThreadLocalSignal (OSD_SignalMode /*theSignalMode*/,
                                Standard_Boolean theFloatingSignal)
{
  SetFloatingSignal (theFloatingSignal);
}

//============================================================================
//==== SetSignal
//====     Set the different signals:
//============================================================================

void OSD::SetSignal (OSD_SignalMode theSignalMode,
                     Standard_Boolean theFloatingSignal)
{
  SetFloatingSignal (theFloatingSignal);

  OSD_WasSetSignal = theSignalMode;
  if (theSignalMode == OSD_SignalMode_AsIs)
  {
    return; // nothing to be done with signal handlers
  }

  // Prepare signal descriptors
  struct sigaction anActSet, anActDfl, anActOld;
  sigemptyset(&anActSet.sa_mask);
  sigemptyset(&anActDfl.sa_mask);
  sigemptyset(&anActOld.sa_mask);
#ifdef SA_RESTART
  anActSet.sa_flags = anActDfl.sa_flags = anActOld.sa_flags = SA_RESTART;
#else
  anActSet.sa_flags = anActDfl.sa_flags = anActOld.sa_flags = 0;
#endif
#ifdef SA_SIGINFO
  anActSet.sa_flags = anActSet.sa_flags | SA_SIGINFO;
  anActSet.sa_sigaction = Handler;
#else
  anActSet.sa_handler = Handler;
#endif
  anActDfl.sa_handler = SIG_DFL;

  // Set signal handlers; NB: SIGSEGV must be the last one!
  const int NBSIG = 8;
  const int aSignalTypes[NBSIG] = { SIGFPE, SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGBUS, SIGSYS, SIGSEGV };
  for (int i = 0; i < NBSIG; ++i)
  {
    // SIGSEGV has special handler
    if (aSignalTypes[i] == SIGSEGV)
    {
#ifdef SA_SIGINFO
      anActSet.sa_sigaction = /*(void(*)(int, siginfo_t *, void*))*/ SegvHandler;
#else
      anActSet.sa_handler = /*(SIG_PFV)*/ SegvHandler;
#endif
    }

    // set handler according to specified mode and current handler
    int retcode = -1;
    if (theSignalMode == OSD_SignalMode_Set || theSignalMode == OSD_SignalMode_SetUnhandled)
    {
      retcode = sigaction (aSignalTypes[i], &anActSet, &anActOld);
    }
    else if (theSignalMode == OSD_SignalMode_Unset)
    {
      retcode = sigaction (aSignalTypes[i], &anActDfl, &anActOld);
    }
    if (theSignalMode == OSD_SignalMode_SetUnhandled && retcode == 0 && anActOld.sa_handler != SIG_DFL)
    {
      struct sigaction anActOld2;
      sigemptyset(&anActOld2.sa_mask);
      retcode = sigaction (aSignalTypes[i], &anActOld, &anActOld2);
    }
    Standard_ASSERT(retcode == 0, "sigaction() failed", std::cout << "OSD::SetSignal(): sigaction() failed for " << aSignalTypes[i] << std::endl);
  }
}

//============================================================================
//==== ControlBreak
//============================================================================

void OSD :: ControlBreak ()
{
  if ( fCtrlBrk ) {
    fCtrlBrk = Standard_False;
    throw OSD_Exception_CTRL_BREAK ("*** INTERRUPT ***");
  }
}

#endif
