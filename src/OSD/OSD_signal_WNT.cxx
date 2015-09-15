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

#ifdef _WIN32

//---------------------------- Windows NT System --------------------------------

#define STRICT
#ifdef NOUSER
#undef NOUSER
#endif
#include <windows.h>

#include <OSD_Exception_ACCESS_VIOLATION.hxx>
#include <OSD_Exception_ARRAY_BOUNDS_EXCEEDED.hxx>
#include <OSD_Exception_ILLEGAL_INSTRUCTION.hxx>
#include <OSD_Exception_IN_PAGE_ERROR.hxx>
#include <OSD_Exception_INT_DIVIDE_BY_ZERO.hxx>
#include <OSD_Exception_INT_OVERFLOW.hxx>
#include <OSD_Exception_INVALID_DISPOSITION.hxx>
#include <OSD_Exception_NONCONTINUABLE_EXCEPTION.hxx>
#include <OSD_Exception_PRIV_INSTRUCTION.hxx>
#include <OSD_Exception_STACK_OVERFLOW.hxx>
#include <OSD_Exception_STATUS_NO_MEMORY.hxx>
#include <OSD_Exception_CTRL_BREAK.hxx>

#include <OSD_Environment.hxx>
#include <Standard_Underflow.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_Overflow.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Mutex.hxx>

#include <OSD_WNT_1.hxx>

#ifdef _MSC_VER
#include <eh.h>
#include <malloc.h>
#endif

#include <process.h>
#include <signal.h>
#include <float.h>

static Standard_Boolean fMsgBox;
static Standard_Boolean fFltExceptions;
static Standard_Boolean fDbgLoaded;
static Standard_Boolean fCtrlBrk;

// used to forbid simultaneous execution of setting / executing handlers
static Standard_Mutex THE_SIGNAL_MUTEX;

static LONG __fastcall _osd_raise ( DWORD, LPSTR );
static BOOL WINAPI     _osd_ctrl_break_handler ( DWORD );

static LONG _osd_debug   ( void );

//# define _OSD_FPX ( _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW )
# define _OSD_FPX ( _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW )

//=======================================================================
//function : CallHandler
//purpose  :
//=======================================================================
static LONG CallHandler (DWORD dwExceptionCode,
                         ptrdiff_t ExceptionInformation1,
                         ptrdiff_t ExceptionInformation0)
{

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)

 Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
 static char          buffer[ 2048 ];
 int                  flterr = 0;

 buffer[0] = '\0' ;

// cout << "CallHandler " << dwExceptionCode << endl ;
 switch ( dwExceptionCode ) {
 
  case EXCEPTION_FLT_DENORMAL_OPERAND:
//      cout << "CallHandler : EXCEPTION_FLT_DENORMAL_OPERAND:" << endl ;
      lstrcpyA (  buffer, "FLT DENORMAL OPERAND"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
//      cout << "CallHandler : EXCEPTION_FLT_DIVIDE_BY_ZERO:" << endl ;
      lstrcpyA (  buffer, "FLT DIVIDE BY ZERO"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_INEXACT_RESULT:
//      cout << "CallHandler : EXCEPTION_FLT_INEXACT_RESULT:" << endl ;
      lstrcpyA (  buffer, "FLT INEXACT RESULT"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_INVALID_OPERATION:
//      cout << "CallHandler : EXCEPTION_FLT_INVALID_OPERATION:" << endl ;
      lstrcpyA (  buffer, "FLT INVALID OPERATION"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_FLT_OVERFLOW:" << endl ;
      lstrcpyA (  buffer, "FLT OVERFLOW"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_STACK_CHECK:
//      cout << "CallHandler : EXCEPTION_FLT_STACK_CHECK:" << endl ;
      lstrcpyA (  buffer, "FLT STACK CHECK"  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_UNDERFLOW:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpyA (  buffer, "FLT UNDERFLOW"  );
      flterr = 1 ;
      break ;
  case STATUS_FLOAT_MULTIPLE_TRAPS:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpyA (  buffer, "FLT MULTIPLE TRAPS (possible overflow in conversion of double to integer)"  );
      flterr = 1 ;
      break ;
  case STATUS_FLOAT_MULTIPLE_FAULTS:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpyA (  buffer, "FLT MULTIPLE FAULTS"  );
      flterr = 1 ;
      break ;

  case STATUS_NO_MEMORY:
//      cout << "CallHandler : STATUS_NO_MEMORY:" << endl ;
      OSD_Exception_STATUS_NO_MEMORY ::
      Raise (  "MEMORY ALLOCATION ERROR ( no room in the process heap )"  );

  case EXCEPTION_ACCESS_VIOLATION:
//      cout << "CallHandler : EXCEPTION_ACCESS_VIOLATION:" << endl ;
      wsprintf ( buffer, "%s%s%s0x%.8p%s%s%s", "ACCESS VIOLATION",
                 fMsgBox ? "\n" : " ", "at address ",
                 ExceptionInformation1 ,
                 " during '",
                 ExceptionInformation0 ? "WRITE" : "READ",
                 "' operation");
  break;

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
//      cout << "CallHandler : EXCEPTION_ARRAY_BOUNDS_EXCEEDED:" << endl ;
      lstrcpyA (  buffer, "ARRAY BOUNDS EXCEEDED"  );
  break;

  case EXCEPTION_DATATYPE_MISALIGNMENT:
//      cout << "CallHandler : EXCEPTION_DATATYPE_MISALIGNMENT:" << endl ;
      lstrcpyA (  buffer, "DATATYPE MISALIGNMENT"  );
  break;

  case EXCEPTION_ILLEGAL_INSTRUCTION:
//      cout << "CallHandler : EXCEPTION_ILLEGAL_INSTRUCTION:" << endl ;
      lstrcpyA (  buffer, "ILLEGAL INSTRUCTION"  );
  break;

  case EXCEPTION_IN_PAGE_ERROR:
//      cout << "CallHandler : EXCEPTION_IN_PAGE_ERROR:" << endl ;
      lstrcpyA (  buffer, "IN_PAGE ERROR"  );
  break;

  case EXCEPTION_INT_DIVIDE_BY_ZERO:
//      cout << "CallHandler : EXCEPTION_INT_DIVIDE_BY_ZERO:" << endl ;
      lstrcpyA (  buffer, "INTEGER DIVISION BY ZERO"  );
  break;

  case EXCEPTION_INT_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_INT_OVERFLOW:" << endl ;
      lstrcpyA (  buffer, "INTEGER OVERFLOW"  );
  break;

  case EXCEPTION_INVALID_DISPOSITION:
//      cout << "CallHandler : EXCEPTION_INVALID_DISPOSITION:" << endl ;
      lstrcpyA (  buffer, "INVALID DISPOSITION"  );
  break;

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
//      cout << "CallHandler : EXCEPTION_NONCONTINUABLE_EXCEPTION:" << endl ;
      lstrcpyA (  buffer, "NONCONTINUABLE EXCEPTION"  );
  break;

  case EXCEPTION_PRIV_INSTRUCTION:
//      cout << "CallHandler : EXCEPTION_PRIV_INSTRUCTION:" << endl ;
      lstrcpyA (  buffer, "PRIVELEGED INSTRUCTION ENCOUNTERED"  );
  break;

  case EXCEPTION_STACK_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_STACK_OVERFLOW:" << endl ;
#if defined( _MSC_VER ) && ( _MSC_VER >= 1300 )
    // try recovering from stack overflow: available in MS VC++ 7.0
    if (!_resetstkoflw())
      lstrcpyA (  buffer, "Unrecoverable STACK OVERFLOW"  );
    else
#endif
      lstrcpyA (  buffer, "STACK OVERFLOW"  );
  break;
 
  default:
    wsprintf( buffer, "unknown exception code 0x%x, params 0x%p 0x%p",
	      dwExceptionCode, ExceptionInformation1, ExceptionInformation0 );

 }  // end switch

 // provide message to the user with possibility to stop
 int idx = lstrlenA ( buffer );
 if ( idx && fMsgBox && dwExceptionCode != EXCEPTION_NONCONTINUABLE_EXCEPTION ) {
     // reset FP operations before message box, otherwise it may fail to show up
    _fpreset();
    _clearfp();

    MessageBeep ( MB_ICONHAND );
    int aChoice = ::MessageBox (0, buffer, "OCCT Exception Handler", MB_ABORTRETRYIGNORE | MB_ICONSTOP);
    if (aChoice == IDRETRY)
    {
      _osd_debug();
      DebugBreak();
    } 
    else if (aChoice == IDABORT)
      exit(0xFFFF);
 }

 // reset FPE state
 if ( flterr ) {
   if ( !fFltExceptions )
     return EXCEPTION_EXECUTE_HANDLER;
   _fpreset () ;
   _clearfp() ;
   _controlfp ( 0, _OSD_FPX ) ;          // JR add :
//     cout << "OSD::WntHandler _controlfp( 0, _OSD_FPX ) " << hex << _controlfp(0,0) << dec << endl ;
 }
 return _osd_raise ( dwExceptionCode, buffer );
#else
 return 0;
#endif
}

//=======================================================================
//function : SIGWntHandler
//purpose  : Will only be used if user calls ::raise() function with
//           signal type set in OSD::SetSignal() - SIGSEGV, SIGFPE, SIGILL
//           (the latter will likely be removed in the future)
//=======================================================================
static void SIGWntHandler (int signum, int sub_code)
{
#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
    Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
    switch( signum ) {
    case SIGFPE :
      if ( signal( signum , (void(*)(int))SIGWntHandler ) == SIG_ERR )
          cout << "signal error" << endl ;
      switch( sub_code ) {
      case _FPE_INVALID :
        CallHandler( EXCEPTION_FLT_INVALID_OPERATION ,0,0) ;
        break ;
      case _FPE_DENORMAL :
        CallHandler( EXCEPTION_FLT_DENORMAL_OPERAND ,0,0) ;
        break ;
      case _FPE_ZERODIVIDE :
        CallHandler( EXCEPTION_FLT_DIVIDE_BY_ZERO ,0,0) ;
        break ;
      case _FPE_OVERFLOW :
        CallHandler( EXCEPTION_FLT_OVERFLOW ,0,0) ;
        break ;
      case _FPE_UNDERFLOW :
        CallHandler( EXCEPTION_FLT_UNDERFLOW ,0,0) ;
        break ;
      case _FPE_INEXACT :
        CallHandler( EXCEPTION_FLT_INEXACT_RESULT ,0,0) ;
        break ;
      default:
        cout << "SIGWntHandler(default) -> Standard_NumericError::Raise(\"Floating Point Error\");" << endl;
        Standard_NumericError::Raise("Floating Point Error");
        break ;
      }
      break ;
    case SIGSEGV :
      if ( signal( signum, (void(*)(int))SIGWntHandler ) == SIG_ERR )
        cout << "signal error" << endl ;
      CallHandler( EXCEPTION_ACCESS_VIOLATION ,0,0) ;
      break ;
    case SIGILL :
      if ( signal( signum, (void(*)(int))SIGWntHandler ) == SIG_ERR )
        cout << "signal error" << endl ;
      CallHandler( EXCEPTION_ILLEGAL_INSTRUCTION ,0,0) ;
      break ;
    default:
      cout << "SIGWntHandler unexpected signal : " << signum << endl ;
      break ;
    }
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
  ptrdiff_t info1 = 0, info0 = 0;
  if ( theExcPtr ) {
    info1 = theExcPtr->ExceptionRecord->ExceptionInformation[1];
    info0 = theExcPtr->ExceptionRecord->ExceptionInformation[0];
  }
  CallHandler(theCode, info1, info0);
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
  DWORD                dwExceptionCode = lpXP->ExceptionRecord->ExceptionCode;

  return CallHandler (dwExceptionCode,
                      lpXP->ExceptionRecord->ExceptionInformation[1],
                      lpXP->ExceptionRecord->ExceptionInformation[0]);
}
//=======================================================================
//function : SetSignal
//purpose  :
//=======================================================================
void OSD::SetSignal (const Standard_Boolean theFloatingSignal)
{
#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
  Standard_Mutex::Sentry aSentry (THE_SIGNAL_MUTEX); // lock the mutex to prevent simultaneous handling
  LPTOP_LEVEL_EXCEPTION_FILTER aPreviousFilter;

  OSD_Environment env (TEXT("CSF_DEBUG_MODE"));
  TCollection_AsciiString val = env.Value();
  if (!env.Failed())
  {
    cout << "Environment variable CSF_DEBUG_MODE setted.\n";
    fMsgBox = Standard_True;
  }
  else
  {
    fMsgBox = Standard_False;
  }

  // Set exception handler (ignored when running under debugger). It will be used in most cases
  // when user's code is compiled with /EHs
  // Replaces the existing top-level exception filter for all existing and all future threads
  // in the calling process
  aPreviousFilter = ::SetUnhandledExceptionFilter (/*(LPTOP_LEVEL_EXCEPTION_FILTER)*/ WntHandler);

  // Signal handlers will only be used when the method ::raise() will be used
  // Handlers must be set for every thread
  if (signal (SIGSEGV, (void(*)(int))SIGWntHandler) == SIG_ERR)
    cout << "signal(OSD::SetSignal) error\n";
  if (signal (SIGFPE,  (void(*)(int))SIGWntHandler) == SIG_ERR)
    cout << "signal(OSD::SetSignal) error\n";
  if (signal (SIGILL,  (void(*)(int))SIGWntHandler) == SIG_ERR)
    cout << "signal(OSD::SetSignal) error\n";

  // Set Ctrl-C and Ctrl-Break handler
  fCtrlBrk = Standard_False;
  SetConsoleCtrlHandler (&_osd_ctrl_break_handler, TRUE);

#ifdef _MSC_VER
//  _se_translator_function pOldSeFunc = 
    _set_se_translator (TranslateSE);
#endif

  fFltExceptions = theFloatingSignal;
  if (theFloatingSignal)
  {
    _controlfp (0, _OSD_FPX);        // JR add :
  }
  else {
    _controlfp (_OSD_FPX, _OSD_FPX); // JR add :
  }
#endif
}  // end OSD :: SetSignal

//============================================================================
//==== ControlBreak 
//============================================================================

void OSD :: ControlBreak () {

 if ( fCtrlBrk ) {
 
  fCtrlBrk = Standard_False;
  OSD_Exception_CTRL_BREAK :: Raise (  TEXT( "*** INTERRUPT ***" )  );
 
 }  // end if

}  // end OSD :: ControlBreak

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

//============================================================================
//==== _osd_raise 
//============================================================================

static LONG __fastcall _osd_raise ( DWORD dwCode, LPSTR msg ) 
{
  if (msg[0] == '\x03') ++msg;

  switch (dwCode)
  {
    case EXCEPTION_ACCESS_VIOLATION:
      OSD_Exception_ACCESS_VIOLATION::Raise (msg);
      break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      OSD_Exception_ARRAY_BOUNDS_EXCEEDED::Raise (msg);
      break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
      Standard_ProgramError::Raise (msg);
      break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
      OSD_Exception_ILLEGAL_INSTRUCTION::Raise (msg);
      break;
    case EXCEPTION_IN_PAGE_ERROR:
      OSD_Exception_IN_PAGE_ERROR::Raise (msg);
      break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
      Standard_DivideByZero::Raise (msg);
      break;
    case EXCEPTION_INT_OVERFLOW:
      OSD_Exception_INT_OVERFLOW::Raise (msg);
      break;
    case EXCEPTION_INVALID_DISPOSITION:
      OSD_Exception_INVALID_DISPOSITION::Raise (msg);
      break;
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      OSD_Exception_NONCONTINUABLE_EXCEPTION::Raise (msg);
      break;
    case EXCEPTION_PRIV_INSTRUCTION:
      OSD_Exception_PRIV_INSTRUCTION::Raise (msg);
      break;
    case EXCEPTION_STACK_OVERFLOW:
      OSD_Exception_STACK_OVERFLOW::Raise (msg);
      break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
      Standard_DivideByZero::Raise (msg);
      break;
    case EXCEPTION_FLT_STACK_CHECK:
    case EXCEPTION_FLT_OVERFLOW:
      Standard_Overflow::Raise (msg);
      break;
    case EXCEPTION_FLT_UNDERFLOW:
      Standard_Underflow::Raise (msg);
      break;
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case STATUS_FLOAT_MULTIPLE_TRAPS:
    case STATUS_FLOAT_MULTIPLE_FAULTS:
      Standard_NumericError::Raise (msg);
      break;
    default:
      break;
  }  // end switch
  return EXCEPTION_EXECUTE_HANDLER;
}  // end _osd_raise

//============================================================================
//==== _osd_debug 
//============================================================================

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return 0
#endif

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

   wsprintf (  cmdLine, keyValue, GetCurrentProcessId (), hEvent  );

   ZeroMemory (  &si, sizeof ( STARTUPINFO )  );

   si.cb      = sizeof ( STARTUPINFO );
   si.dwFlags = STARTF_FORCEONFEEDBACK;

//   cout << "_osd_debug -> CreateProcess" << endl ;
   if (  !CreateProcess (
           NULL, cmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE,
           NULL, NULL, &si, &pi
          )
   ) __leave;

//   cout << "_osd_debug -> WaitForSingleObject " << endl ;
   WaitForSingleObject ( hEvent, INFINITE );
//   cout << "_osd_debug <- WaitForSingleObject -> CloseHandle " << endl ;

   CloseHandle ( pi.hProcess );
   CloseHandle ( pi.hThread  );

//   cout << "_osd_debug fDbgLoaded  " << endl ;
   fDbgLoaded = TRUE;
  
  }  // end __try

  __finally {
  
//   cout << "_osd_debug -> CloseHandle(hKey) " << endl ;
   if ( hKey   != INVALID_HANDLE_VALUE ) CloseHandle ( hKey   );
//   cout << "_osd_debug -> CloseHandle(hEvent) " << endl ;
   if ( hEvent != INVALID_HANDLE_VALUE ) CloseHandle ( hEvent );
//   cout << "_osd_debug end __finally " << endl ;
  
  }  // end __finally

 }  /* end if */

 action = fDbgLoaded ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER;
// cout << "_osd_debug return " << action << " EXCEPTION_CONTINUE_EXECUTION("
//      << EXCEPTION_CONTINUE_EXECUTION << ")" << endl ;
 return action ;

}  // end _osd_debug

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif
#endif // _WIN32
