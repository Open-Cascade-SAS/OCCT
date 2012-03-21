// Copyright (c) 1991-1999 Matra Datavision
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


#if defined (__osf__ ) || defined ( DECOSF1 )
int DECOSF1_a_horreur_du_vide_a_la_compilation;
#endif
#if defined ( __hpux ) || defined ( HPUX ) 
int HPUX_a_horreur_du_vide_a_la_compilation;
#endif

#ifdef WNT

#define STRICT
#include <windows.h>

#include <stdio.h>
#include <string.h>

#include <InterfaceGraphic_wntio.hxx>

typedef struct _env {

  DWORD len;
  char* ptr;

} ENV, *PENV;

static DWORD tlsIndex;

class Init_ {

public:

  Init_ () { tlsIndex = TlsAlloc (); }

}; // end Init_

static Init_ init;

int cPrintf ( char* fmt, ... ) {

  static BOOL   first = TRUE;
  static HANDLE hConsole = NULL;

  char    buffer[ 256 ];
  va_list argptr;
  int     cnt;
  DWORD   lpcchWritten;

  if ( first ) {
    hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
    if ( hConsole == NULL ) {
      AllocConsole ();
      hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
    }  /* end if */

    first = FALSE;

  }  /* end if */

  va_start( argptr, fmt );
  cnt = vsprintf ( buffer, fmt, argptr ); 
  va_end ( argptr );
  WriteConsole ( hConsole, buffer, strlen ( buffer ), &lpcchWritten, NULL );
  return cnt;

}  /* end cPrintf */

int fcPrintf ( int dummy, char* fmt, ... ) {

  static BOOL   first = TRUE;
  static HANDLE hConsole = NULL;

  char    buffer[ 256 ];
  va_list argptr;
  int     cnt;
  DWORD   lpcchWritten;

  if ( first ) {
    hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
    if ( hConsole == NULL ) {
      AllocConsole ();
      hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );
    }  /* end if */

    first = FALSE;
  }  /* end if */

  va_start( argptr, fmt );
  cnt = vsprintf ( buffer, fmt, argptr ); 
  va_end ( argptr );
  WriteConsole ( hConsole, buffer, strlen ( buffer ), &lpcchWritten, NULL );

  return cnt;

}  /* end fcPrintf */

char* GetEnv ( char* name ) {

  DWORD dwLen;
  PENV  env = ( PENV )TlsGetValue ( tlsIndex );

  if ( env == NULL ) {
    env = ( PENV )HeapAlloc (
      GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
      ( DWORD )sizeof ( ENV )
      );
    TlsSetValue (  tlsIndex, ( LPVOID )env  );
  }  /* end if */

  SetLastError ( ERROR_SUCCESS );
  dwLen = GetEnvironmentVariable ( name, NULL, 0 );

  if (  dwLen == 0 && GetLastError () != ERROR_SUCCESS  ) return NULL;

  ++dwLen;

  if ( env -> len < dwLen ) {
    if ( env -> ptr != NULL )
      env -> ptr = ( char* )HeapReAlloc (
      GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
      env -> ptr, dwLen
      );
    else
      env -> ptr = ( char* )HeapAlloc (
      GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
      dwLen
      );
    env -> len = dwLen;
  }  /* end if */

  GetEnvironmentVariable ( name, env -> ptr, dwLen );
  return env -> ptr;

}  /* end GetEnv */
#endif /* WNT */
