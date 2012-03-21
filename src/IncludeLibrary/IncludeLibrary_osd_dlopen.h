/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#include <stdio.h>
#include <stdlib.h>
#ifndef WNT
#include <unistd.h>
#endif
#include <string.h>
#include <Standard_Stream.hxx>
#if defined(__hpux) || defined(HPUX)
//#include <cxxdl.h>
# include <dl.h>
#else
#ifndef WNT
# include <dlfcn.h>
#endif
#endif

typedef void (*voidfct)() ;

static char chartmp[512] ;

inline int osd_dlenv( char *Path )
{
 char *engine_path = new char [ strlen( Path ) + 4 ] ;
 char *ld_path = new char [ 4096 ] ;
 int ret = 0 ;

#if defined (__hpux) || defined(HPUX)
 strcpy( ld_path , "SHLIB_PATH=" ) ;
#elif defined(WNT)
 strcpy( ld_path , "PATH=" ) ;
#elif defined(_AIX) || defined(AIX)
 strcpy( ld_path , "LIBPATH=" ) ;
#else
 strcpy( ld_path , "LD_LIBRARY_PATH=" ) ;
#endif

 strcpy( engine_path , Path ) ;
 strcat( engine_path , ".ld" ) ;
 ifstream fileld( engine_path ) ;
 if ( !fileld.bad() ) {
   fileld.getline( &ld_path[ strlen( ld_path ) ] ,
                   4096 - strlen( ld_path ) ) ;
   fileld.close() ;
   if ( !putenv( ld_path ) )
     ret = 1;
 }
/* delete [] engine_path ; */
/* delete [] ld_path ; */
 return ret ;
}

inline void *osd_dlopen( char *Path , int LazyMode )
{
 void *handle ;

 handle = NULL ;
#if defined (__hpux) || defined(HPUX)
 if ( Path != NULL ) {
   if ( LazyMode & 1 ) {
     if ( LazyMode & 2 )
//       handle = (void *) cxxshl_load( Path , BIND_FIRST | BIND_TOGETHER |
       handle = (void *) shl_load( Path , BIND_FIRST | BIND_TOGETHER |
                                      BIND_DEFERRED | BIND_VERBOSE |
                                      DYNAMIC_PATH , 0L);
     else
//       handle = (void *) cxxshl_load( Path , BIND_DEFERRED | BIND_VERBOSE |
       handle = (void *) shl_load( Path , BIND_DEFERRED | BIND_VERBOSE |
                                      DYNAMIC_PATH , 0L);
   }
   else {
     if ( LazyMode & 2 )
//       handle = (void *) cxxshl_load( Path , BIND_FIRST | BIND_TOGETHER |
       handle = (void *) shl_load( Path , BIND_FIRST | BIND_TOGETHER |
                                      BIND_IMMEDIATE | BIND_VERBOSE |
                                      DYNAMIC_PATH, 0L);
     else
//       handle = (void *) cxxshl_load( Path , BIND_IMMEDIATE | BIND_VERBOSE |
       handle = (void *) shl_load( Path , BIND_IMMEDIATE | BIND_VERBOSE |
                                      DYNAMIC_PATH , 0L);
   }
 }
// else
//   return cxxshl_load( Path , BIND_IMMEDIATE ,0L);
// ===> findsym( &NULL ... for HP

#elif defined(sun) || defined(SOLARIS)  || defined(IRIX) || defined(sgi) || defined(linux) || defined(LIN) || defined(__FreeBSD__)
 if ( Path != NULL ) {
   if ( LazyMode )
     handle = dlopen( Path , RTLD_LAZY | RTLD_GLOBAL );
   else
     handle = dlopen( Path , RTLD_GLOBAL );
 }
 else
   handle = dlopen( Path , RTLD_NOW );
#elif defined(WNT)
 if ( Path != NULL )
   handle = (void *) LoadLibraryEx( Path , NULL ,
                                    LOAD_WITH_ALTERED_SEARCH_PATH );
 else
   handle = (void *) GetModuleHandle( NULL ) ;
#else
 if ( Path != NULL ) {
   if ( LazyMode )
     handle = dlopen( Path , RTLD_LAZY );
   else
     handle = dlopen( Path , RTLD_NOW );
 }
 else
   handle = dlopen( Path , RTLD_NOW );
#endif

// cout << "dlopen handle " << hex << handle << dec << endl ;
 return handle ;
}

inline char *osd_dlopenerror( char *Path )
{

#if defined (__hpux) || defined(HPUX)
   if ( Path != NULL && errno != 0 ) {
     strcpy( chartmp , "dlopen(\"" ) ;
     strcat( chartmp , Path ) ;
     strcat( chartmp , "\") perror " ) ;
     perror( chartmp ) ;
   }
   else
     chartmp[0] = '\0' ;
#elif defined(WNT)
   int i ;
   strcpy( chartmp , "LoadLibraryEx(\"" ) ;
   strcat( chartmp , Path ) ;
   strcat( chartmp , "\") : " ) ;
   i = strlen(chartmp) ;
   FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError() ,
                  MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ), &chartmp[i] , 450, NULL);
   //strcat( chartmp ,  GetLastError()  ) ;
#else
   strcpy( chartmp , "dlopen(\"" ) ;
   strcat( chartmp , Path ) ;
   strcat( chartmp , "\") : " ) ;
   strcat( chartmp , dlerror() ) ;
#endif

   return chartmp ;
 }

inline voidfct osd_dlsym( void *handle , char *signature )

{

#if defined (__hpux) || defined(HPUX)
        static voidfct adr_get ;
        errno = 0 ;
        adr_get = NULL ;
// cout << "dlsym handle *" << hex << &handle << " = " << handle << dec << endl ;
        if ( shl_findsym( (shl_t *) &handle , signature , TYPE_PROCEDURE,
                          &adr_get) == -1 ) {
//          cout << "shl_findsym error " << endl ;
          adr_get = NULL ;
	}
        return adr_get ;

#elif defined(WNT)
        return (voidfct ) GetProcAddress( (struct HINSTANCE__ *) handle ,
                                           signature ) ;
#else
        return (voidfct ) dlsym( handle , signature ) ;
#endif
}

inline voidfct osd_dlsymall( void *handle , char *signature )
{

#if defined (__hpux) || defined(HPUX)
 handle = NULL ;
//#elif defined(WNT)
// handle = osd_dlopen( NULL , 0 ) ;
#else
// cout << "Search of " << signature << " with handle " << hex << handle << dec << endl ;
 if ( handle == NULL )
   handle = osd_dlopen( NULL , 0 ) ;
#endif

 return osd_dlsym( handle , signature ) ;
}

inline char *osd_dlsymerror( char *signature )
{

#if defined (__hpux) || defined(HPUX)
 if ( errno != 0 ) {
   strcpy( chartmp , "shl_findsym(\"" ) ;
   strcat( chartmp , signature ) ;
   strcat( chartmp , "\") perror " ) ;
   perror( chartmp ) ;
 }
 else
   chartmp[0] = '\0' ;
#elif defined(WNT)
   int i ;
   strcpy( chartmp , "GetProcAddress(\"" ) ;
   strcat( chartmp , signature ) ;
   strcat( chartmp , "\") : " ) ;
   i = strlen( chartmp ) ;
   FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError() ,
                  MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ), &chartmp[i] , 450, NULL);
  // strcat( chartmp , osd_getmsg( GetLastError() ) ) ;
#else
   strcpy( chartmp , "dlsym(\"" ) ;
   strcat( chartmp , signature ) ;
   strcat( chartmp , "\") : " ) ;
   strcat( chartmp , dlerror() ) ;
#endif

 return chartmp ;
}
