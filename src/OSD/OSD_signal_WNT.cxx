// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <OSD.ixx>

static Standard_Boolean fSETranslator =
#ifdef _MSC_VER
                           Standard_True;
#else
                           Standard_False;
#endif

#ifdef WNT

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

static LONG __fastcall _osd_raise ( DWORD, LPTSTR );
static BOOL WINAPI     _osd_ctrl_break_handler ( DWORD );

extern "C" Standard_EXPORT LONG _osd_debug   ( void );
extern "C" Standard_EXPORT void _debug_break ( Standard_PCharacter );

MB_DESC fatalErrorDesc[] = {
									  
                 { MBT_ICON,   ( int )IDI_HAND              },
                 { MBT_BUTTON, IDYES,    TEXT( "Continue" ) },
                 { MBT_BUTTON, IDNO,     TEXT( "Debugger" ) },
                 { MBT_BUTTON, IDCANCEL, TEXT( "Stop"     ) }

};

static LONG CallHandler (DWORD, ptrdiff_t, ptrdiff_t);
static void SIGWntHandler (int, int);


//# define _OSD_FPX ( _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW )
# define _OSD_FPX ( _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW )

//============================================================================
//==== WntHandler
//============================================================================

Standard_Integer OSD :: WntHandler ( const Standard_Address exceptionInfo )
{

 LPEXCEPTION_POINTERS lpXP = ( LPEXCEPTION_POINTERS )exceptionInfo;
 DWORD                dwExceptionCode = lpXP -> ExceptionRecord -> ExceptionCode;

// cout << "WntHandler " << dwExceptionCode << " " << lpXP->ExceptionRecord->ExceptionInformation[1]
//      << " " <<lpXP->ExceptionRecord->ExceptionInformation[0] << endl ;

 return CallHandler( dwExceptionCode ,
                     lpXP -> ExceptionRecord -> ExceptionInformation[ 1 ] ,
                     lpXP -> ExceptionRecord -> ExceptionInformation[ 0 ] ) ;

}

//============================================================================
//==== SIGWntHandler
//============================================================================

static void SIGWntHandler(int signum , int sub_code ) {

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)

//        cout << "SIGWntHandler " << signum << " subcode " << sub_code << endl ;
	switch( signum ) {
	case SIGFPE :
	  if ( signal( signum , ( void (*)(int) ) &SIGWntHandler ) == SIG_ERR )
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
            cout << "SIGWntHandler(default) -> Standard_NumericError::Raise(\"Floating Point Error\");"
                 << endl ;
            Standard_NumericError::Raise("Floating Point Error");
            break ;
          }
          break ;
	case SIGSEGV :
	  if ( signal( signum , ( void (*)(int) )  &SIGWntHandler ) == SIG_ERR )
            cout << "signal error" << endl ;
          CallHandler( EXCEPTION_ACCESS_VIOLATION ,0,0) ;
	  break ;
	case SIGILL :
	  if ( signal( signum , ( void (*)(int) )  &SIGWntHandler ) == SIG_ERR )
            cout << "signal error" << endl ;
          CallHandler( EXCEPTION_ILLEGAL_INSTRUCTION ,0,0) ;
	  break ;
        default:
          cout << "SIGWntHandler unexpected signal : "
               << signum << endl ;
          break ;
	}
// cout << "return from SIGWntHandler -> DebugBreak " << endl ;
 DebugBreak ();

#endif

}

//============================================================================
//==== CallHandler 
//============================================================================

static LONG CallHandler (DWORD dwExceptionCode ,
			 ptrdiff_t ExceptionInformation1 ,
			 ptrdiff_t ExceptionInformation0)
{

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)

 static TCHAR         buffer[ 2048 ];
 int                  flterr = 0;

 buffer[0] = '\0' ;

// cout << "CallHandler " << dwExceptionCode << endl ;
 switch ( dwExceptionCode ) {
 
  case EXCEPTION_FLT_DENORMAL_OPERAND:
//      cout << "CallHandler : EXCEPTION_FLT_DENORMAL_OPERAND:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT DENORMAL OPERAND" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
//      cout << "CallHandler : EXCEPTION_FLT_DIVIDE_BY_ZERO:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT DIVIDE BY ZERO" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_INEXACT_RESULT:
//      cout << "CallHandler : EXCEPTION_FLT_INEXACT_RESULT:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT INEXACT RESULT" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_INVALID_OPERATION:
//      cout << "CallHandler : EXCEPTION_FLT_INVALID_OPERATION:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT INVALID OPERATION" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_FLT_OVERFLOW:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT OVERFLOW" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_STACK_CHECK:
//      cout << "CallHandler : EXCEPTION_FLT_STACK_CHECK:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT STACK CHECK" )  );
      flterr = 1 ;
      break ;
  case EXCEPTION_FLT_UNDERFLOW:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT UNDERFLOW" )  );
      flterr = 1 ;
      break ;
  case STATUS_FLOAT_MULTIPLE_TRAPS:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT MULTIPLE TRAPS (possible overflow in conversion of double to integer)" )  );
      flterr = 1 ;
      break ;
  case STATUS_FLOAT_MULTIPLE_FAULTS:
//      cout << "CallHandler : EXCEPTION_FLT_UNDERFLOW:" << endl ;
      lstrcpy (  buffer, TEXT( "FLT MULTIPLE FAULTS" )  );
      flterr = 1 ;
      break ;

  case STATUS_NO_MEMORY:
//      cout << "CallHandler : STATUS_NO_MEMORY:" << endl ;
      OSD_Exception_STATUS_NO_MEMORY ::
      Raise (  TEXT( "MEMORY ALLOCATION ERROR ( no room in the process heap )" )  );

  case EXCEPTION_ACCESS_VIOLATION:
//      cout << "CallHandler : EXCEPTION_ACCESS_VIOLATION:" << endl ;
      wsprintf ( buffer, TEXT( "%s%s%s0x%.8p%s%s%s" ), TEXT( "ACCESS VIOLATION" ),
                 fMsgBox ? "\n" : " ", TEXT( "at address " ),
                 ExceptionInformation1 ,
                 TEXT( " during '" ),
                 ExceptionInformation0 ? TEXT( "WRITE" ) : TEXT( "READ" ),
                 TEXT( "' operation" ));
  break;

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
//      cout << "CallHandler : EXCEPTION_ARRAY_BOUNDS_EXCEEDED:" << endl ;
      lstrcpy (  buffer, TEXT( "ARRAY BOUNDS EXCEEDED" )  );
  break;

  case EXCEPTION_DATATYPE_MISALIGNMENT:
//      cout << "CallHandler : EXCEPTION_DATATYPE_MISALIGNMENT:" << endl ;
      lstrcpy (  buffer, TEXT( "DATATYPE MISALIGNMENT" )  );
  break;

  case EXCEPTION_ILLEGAL_INSTRUCTION:
//      cout << "CallHandler : EXCEPTION_ILLEGAL_INSTRUCTION:" << endl ;
      lstrcpy (  buffer, TEXT( "ILLEGAL INSTRUCTION" )  );
  break;

  case EXCEPTION_IN_PAGE_ERROR:
//      cout << "CallHandler : EXCEPTION_IN_PAGE_ERROR:" << endl ;
      lstrcpy (  buffer, TEXT( "IN_PAGE ERROR" )  );
  break;

  case EXCEPTION_INT_DIVIDE_BY_ZERO:
//      cout << "CallHandler : EXCEPTION_INT_DIVIDE_BY_ZERO:" << endl ;
      lstrcpy (  buffer, TEXT( "INTEGER DIVISION BY ZERO" )  );
  break;

  case EXCEPTION_INT_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_INT_OVERFLOW:" << endl ;
      lstrcpy (  buffer, TEXT( "INTEGER OVERFLOW" )  );
  break;

  case EXCEPTION_INVALID_DISPOSITION:
//      cout << "CallHandler : EXCEPTION_INVALID_DISPOSITION:" << endl ;
      lstrcpy (  buffer, TEXT( "INVALID DISPOSITION" )  );
  break;

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
//      cout << "CallHandler : EXCEPTION_NONCONTINUABLE_EXCEPTION:" << endl ;
      lstrcpy (  buffer, TEXT( "NONCONTINUABLE EXCEPTION" )  );
  break;

  case EXCEPTION_PRIV_INSTRUCTION:
//      cout << "CallHandler : EXCEPTION_PRIV_INSTRUCTION:" << endl ;
      lstrcpy (  buffer, TEXT( "PRIVELEGED INSTRUCTION ENCOUNTERED" )  );
  break;

  case EXCEPTION_STACK_OVERFLOW:
//      cout << "CallHandler : EXCEPTION_STACK_OVERFLOW:" << endl ;
#if defined( _MSC_VER ) && ( _MSC_VER >= 1300 )
    // try recovering from stack overflow: available in MS VC++ 7.0
    if (!_resetstkoflw())
      lstrcpy (  buffer, TEXT( "Unrecoverable STACK OVERFLOW" )  );
    else
#endif
      lstrcpy (  buffer, TEXT( "STACK OVERFLOW" )  );
  break;
 
  default:
    wsprintf( buffer, TEXT("unknown exception code 0x%x, params 0x%p 0x%p"),
	      dwExceptionCode, ExceptionInformation1, ExceptionInformation0 );

 }  // end switch

 int idx = lstrlen ( buffer );


 if ( idx && fMsgBox && dwExceptionCode != EXCEPTION_NONCONTINUABLE_EXCEPTION ) {
  MessageBeep ( MB_ICONHAND );
  int msgID = MsgBox ( NULL, buffer, TEXT( "Error detected" ), 4, fatalErrorDesc );
//  cout << "flterr" << flterr << " fFltExceptions " << fFltExceptions << endl ;
  if ( flterr ) {
    if ( !fFltExceptions )
      return EXCEPTION_EXECUTE_HANDLER;
    _fpreset () ;
    _clearfp() ;
    _controlfp ( 0, _OSD_FPX ) ;          // JR add :
//    cout << "OSD::WntHandler _controlfp( 0, _OSD_FPX ) " << hex << _controlfp(0,0) << dec << endl ;
  }
  buffer[ idx ] = 0;
  switch ( msgID ) {
   case IDYES: {
    PTCHAR ptr = _tcschr (  buffer, TEXT( '\n' )  );
    if ( ptr != NULL )
      *ptr = TEXT( ' ' );
//    cout << "CallHandler " << dwExceptionCode << endl ;
    _osd_raise ( dwExceptionCode, buffer );
   }  // IDYES
   case IDNO:
    LONG action ;
    action = _osd_debug ();
//    cout << "return from CallHandler -> DebugBreak " << endl ;
    DebugBreak ();
    _osd_raise ( dwExceptionCode, buffer );
//    cout << "CallHandler return : " << action << endl ;
    return action ;
   case IDCANCEL:
    exit ( 0xFFFF );
   }  // end switch
 }
 else {
   if ( flterr ) {
     if ( !fFltExceptions )
       return EXCEPTION_EXECUTE_HANDLER;
     _fpreset () ;
     _clearfp() ;
     _controlfp ( 0, _OSD_FPX ) ;          // JR add :
//     cout << "OSD::WntHandler _controlfp( 0, _OSD_FPX ) " << hex << _controlfp(0,0) << dec << endl ;
   }
 }

 return _osd_raise ( dwExceptionCode, buffer );

#else
 return 0;
#endif

}  // end OSD :: WntHandler

//=======================================================================
//function : TranslateSE
//purpose  : Translate Structural Exceptions into C++ exceptions
//=======================================================================

#ifdef _MSC_VER
static void TranslateSE( unsigned int theCode, EXCEPTION_POINTERS* theExcPtr )
{
  ptrdiff_t info1 = 0, info0 = 0;
  if ( theExcPtr ) {
    info1 = theExcPtr->ExceptionRecord->ExceptionInformation[1];
    info0 = theExcPtr->ExceptionRecord->ExceptionInformation[0];
  }
  CallHandler(theCode, info1, info0);
}
#endif

//============================================================================
//==== SetSignal 
//============================================================================

#ifdef _MSC_VER
// MSV 31.08.2005
// If we compile this file under MSVC 7.1 with the default options for 
// exception handling (/GX or /EHsc) then the following warning is issued:
//   warning C4535: calling _set_se_translator() requires /EHa
// Till now all worked with the default options, and there was no difference
// found in exception handling behaviour between /EHa and /EHs options. 
// So, here we disable the warning, and leave the default compiler options.
// If some reason appears to turn to /EHa option this pragma can be removed.
#pragma warning (disable:4535)
#endif

void OSD :: SetSignal ( const Standard_Boolean aFloatingSignal ) {

#if !defined(__CYGWIN32__) && !defined(__MINGW32__)

 static int first_time = 1 ;
 LPTOP_LEVEL_EXCEPTION_FILTER aPreviousFilter ;

 if ( first_time ) {
//   OSD_Environment         env (  TEXT( "CSF_EXCEPTION_PROMPT" )  );
   OSD_Environment         env (  TEXT( "CSF_DEBUG_MODE" )  );
   TCollection_AsciiString val;

   val = env.Value ();

   if (  !env.Failed ()  ) {
     cout << "Environment variable CSF_DEBUG_MODE setted." << endl ;
     fMsgBox = Standard_True;
   }
   else {
//     cout << "Environment variable CSF_DEBUG_MODE not setted." << endl ;
     fMsgBox = Standard_False;
   }

   if (!fSETranslator) {
     aPreviousFilter =
       SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER)&OSD::WntHandler);
//   cout << "SetUnhandledExceptionFilter previous filer : " << hex << aPreviousFilter << dec << endl ;

     if ( signal( SIGSEGV , ( void (*)(int) ) &SIGWntHandler ) == SIG_ERR )
       cout << "signal(OSD::SetSignal) error" << endl ;
     if ( signal( SIGFPE , ( void (*)(int) ) &SIGWntHandler ) == SIG_ERR )
       cout << "signal(OSD::SetSignal) error" << endl ;
     if ( signal( SIGILL , ( void (*)(int) ) &SIGWntHandler ) == SIG_ERR )
       cout << "signal(OSD::SetSignal) error" << endl ;
   }

   fCtrlBrk = Standard_False;
   SetConsoleCtrlHandler ( &_osd_ctrl_break_handler, TRUE );
 }

#ifdef _MSC_VER
 if (fSETranslator) {
   // use Structural Exception translator (one per thread)
   _se_translator_function pOldSeFunc = _set_se_translator( TranslateSE );
 }
#endif

 fFltExceptions = aFloatingSignal;
 if ( aFloatingSignal ) {
   _controlfp ( 0, _OSD_FPX );          // JR add :
   if ( first_time ) {
//     cout << "SetSignal with floating point traps : " << hex << _controlfp(0,0) << dec << endl ;
     first_time = 0 ;
   }
 }
 else {
   _controlfp ( _OSD_FPX , _OSD_FPX );          // JR add :
   if ( first_time ) {
//     cout << "SetSignal without floating point traps : " << hex << _controlfp(0,0) << dec << endl ;
     first_time = 0 ;
   }
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

static LONG __fastcall _osd_raise ( DWORD dwCode, LPTSTR msg ) 
{
  if (  msg[ 0 ] == TEXT( '\x03' )  ) ++msg;

 switch ( dwCode ) {
    
  case EXCEPTION_ACCESS_VIOLATION:

   OSD_Exception_ACCESS_VIOLATION :: Raise ( msg );

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:

   OSD_Exception_ARRAY_BOUNDS_EXCEEDED :: Raise ( msg );

  case EXCEPTION_DATATYPE_MISALIGNMENT:

   Standard_ProgramError :: Raise ( msg );

  case EXCEPTION_ILLEGAL_INSTRUCTION:

   OSD_Exception_ILLEGAL_INSTRUCTION :: Raise ( msg );

  case EXCEPTION_IN_PAGE_ERROR:

   OSD_Exception_IN_PAGE_ERROR :: Raise ( msg );

  case EXCEPTION_INT_DIVIDE_BY_ZERO:

   Standard_DivideByZero :: Raise ( msg );

  case EXCEPTION_INT_OVERFLOW:

   OSD_Exception_INT_OVERFLOW :: Raise ( msg );

  case EXCEPTION_INVALID_DISPOSITION:

   OSD_Exception_INVALID_DISPOSITION :: Raise ( msg );

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:

   OSD_Exception_NONCONTINUABLE_EXCEPTION :: Raise ( msg );

  case EXCEPTION_PRIV_INSTRUCTION:

   OSD_Exception_PRIV_INSTRUCTION :: Raise ( msg );

  case EXCEPTION_STACK_OVERFLOW:

   OSD_Exception_STACK_OVERFLOW :: Raise ( msg );
    
  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
       Standard_DivideByZero :: Raise ( msg );
  case EXCEPTION_FLT_STACK_CHECK:
  case EXCEPTION_FLT_OVERFLOW:
       Standard_Overflow :: Raise ( msg );
  case EXCEPTION_FLT_UNDERFLOW:
       Standard_Underflow :: Raise ( msg );
  case EXCEPTION_FLT_INVALID_OPERATION:
  case EXCEPTION_FLT_DENORMAL_OPERAND:
  case EXCEPTION_FLT_INEXACT_RESULT:
  case STATUS_FLOAT_MULTIPLE_TRAPS:
  case STATUS_FLOAT_MULTIPLE_FAULTS:
       Standard_NumericError :: Raise ( msg );
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
 
  HKEY                hKey;
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

//============================================================================
//==== _debug_break 
//============================================================================

void _debug_break ( Standard_PCharacter msg ) {

 OSD_Environment    env ( "CSF_DEBUG_MODE" );
 Standard_Character buff[ 2048 ];

 env.Value ();

 if (  env.Failed ()  ) return;

 lstrcpy ( buff, msg );
 lstrcat (  buff, _TEXT( "\nExit to debugger ?" )  );

 if (  MessageBox (
        NULL, buff, _TEXT( "DEBUG" ), MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO
       ) == IDYES
 ) {
 
  _osd_debug ();
  DebugBreak ();
 
 }  // end if

}  // end _debug_break

// Must be there for compatibility with UNIX system code ----------------------

//void OSD::Handler(const OSD_Signals aSig,
//                  const OSD_Signals aCode){}
void OSD::Handler(const OSD_Signals theSignal,
		  const Standard_Address theSigInfo,
		  const Standard_Address theContext) {}

void OSD::SegvHandler(const OSD_Signals aSig,
                      const Standard_Address code,
                      const Standard_Address scp){}

#endif // WNT


//=======================================================================
//function : UseSETranslator
//purpose  : Defines whether to use _se_translator_function or
//           SetUnhandledExceptionFilter and signal to catch system exceptions
//=======================================================================

void OSD::UseSETranslator( const Standard_Boolean
#ifdef _MSC_VER
                          useSE
#endif
                          )
{
#ifdef _MSC_VER
  fSETranslator = useSE;
#endif
}

Standard_Boolean OSD::UseSETranslator()
{
  return fSETranslator;
}
