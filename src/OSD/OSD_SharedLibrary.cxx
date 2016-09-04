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

#ifndef _WIN32


#include <OSD_Function.hxx>
#include <OSD_LoadMode.hxx>
#include <OSD_SharedLibrary.hxx>

#include <stdio.h>
#ifdef __some_crappy_system__
/*
 * Values for 'mode' argument in dlopen().
 *
*/
#define RTLD_LAZY		1
#define RTLD_NOW	        2
/*
 * Interface to rld via unsupported __rld_libdl_interface() call.
 *
 */
#define _LIBDL_RLD_DLOPEN	1
#define _LIBDL_RLD_DLCLOSE	2
#define _LIBDL_RLD_DLSYM	3
#define _LIBDL_RLD_DLERROR	4
extern "C" {void	*dlopen(char *path, int mode);}
extern "C" {void*   dlsym   (       void*  handle,char* name);}
extern "C" {int     dlclose (       void  *handle  );}
extern "C" {void    *dlerror (void);}
#endif

#include <dlfcn.h>

#define BAD(X)  ((X) == NULL)

// ----------------------------------------------------------------
//
// Create and initialize a shared library object to NULL
//
// ----------------------------------------------------------------
OSD_SharedLibrary::OSD_SharedLibrary():myHandle(NULL),myName(NULL){
}
// ----------------------------------------------------------------
//
// Create and initialize a shared library object to the
// name given as argument
//
// ----------------------------------------------------------------
OSD_SharedLibrary::OSD_SharedLibrary(const Standard_CString aName):myHandle(NULL) 
{
  if (aName != NULL) {
    myName = new char [(strlen (aName) + 1 )];
    strcpy (myName,aName);
  }
}
// ----------------------------------------------------------------
//
// Name: Returns the shared library name
//
// ----------------------------------------------------------------
Standard_CString  OSD_SharedLibrary::Name() const {
  return myName; 
}
// ----------------------------------------------------------------
//
// SetName: Sets a name to a shared library object
//
// ----------------------------------------------------------------
void  OSD_SharedLibrary::SetName(const Standard_CString aName)  {
  if (aName != NULL) {
    myName = new char [(strlen (aName) + 1 )];
    strcpy (myName,aName);
  }
}
// ----------------------------------------------------------------
//
// DlOpen:   The dlopen function provides an interface to the dynamic 
// library loader to allow shared libraries to be loaded and called at
// runtime.  
// The dlopen function attempts to load filename, in the address space 
// of the process, resolving symbols as appropriate.  Any libraries that      
// filename depends upon are also loaded.
//
// If mode is RTLD_LAZY, then the runtime loader does symbol resolution 
// only as needed.  Typically, this means that the first call	
// to a function in the newly loaded library will cause the resolution 
// of the address of that function to occur.  
//
// If mode is RTLD_NOW, then the runtime loader must do all
// symbol binding during the dlopen call.  
// The dlopen function returns a handle that is used by dlsym or 
// dlclose call.  If there is an error, a NULLpointer is returned.
//
// If a NULL filename is specified, dlopen returns a handle for the main      
// executable, which allows access to dynamic symbols in the running program.
//
// ----------------------------------------------------------------
Standard_Boolean  OSD_SharedLibrary::DlOpen(const OSD_LoadMode aMode ) {
if (aMode == OSD_RTLD_LAZY){
  myHandle = dlopen (myName,RTLD_LAZY);
}
else if (aMode == OSD_RTLD_NOW){
  myHandle = dlopen (myName,RTLD_NOW);
}

if (!BAD(myHandle)){
  return Standard_True;
 }
else {
  return Standard_False;
 }
}
// ----------------------------------------------------------------
//
// DlSymb: The dlsym function returns the address of the	
// symbol name found in the shared library corresponding to handle.  
// If the symbol is not	found, a NULL
// pointer is returned.
//
// ----------------------------------------------------------------
OSD_Function  OSD_SharedLibrary::DlSymb(const Standard_CString aName )const{
void (*fp)();
fp =  (void (*)()) dlsym (myHandle,aName);
if (!BAD(fp)){
  return (OSD_Function)fp;
 }
else {
  return (OSD_Function)NULL;
 }
}
// ----------------------------------------------------------------
//
//DlClose: The dlclose function deallocates the address space for the library
//corresponding	to handle.  If any user	function continues to call a symbol
//resolved in the address space	of a library that has been since been deallo-
//cated	by dlclose, the	results	are undefined.
//
// ----------------------------------------------------------------
void OSD_SharedLibrary::DlClose()const{
 dlclose(myHandle);
}
// ----------------------------------------------------------------
//
// DlError:  returns a string	describing the last error that
// occurred from a call to dlopen, dlclose or dlsym.
//
// ----------------------------------------------------------------
Standard_CString OSD_SharedLibrary::DlError()const{
return (char*) dlerror();
}
// ----------------------------------------------------------------------------
// Destroy
// ----------------------------------------------------------------------------
void OSD_SharedLibrary::Destroy() {
  if (myName != NULL) {
     delete [] myName;
     myName = NULL;
     myHandle = NULL;
  }
}

#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_SharedLibrary ----------
//------------------------------------------------------------------------

//it is important to define STRICT and enforce including <windows.h> before
//Standard_Macro.hxx undefines it and includes <windows.h> causing compilation errors
#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

#include <OSD_Path.hxx>
#include <OSD_SharedLibrary.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

static DWORD              lastDLLError;

static wchar_t errMsg[1024];
static char errMsgA[1024];

OSD_SharedLibrary :: OSD_SharedLibrary () {

 myHandle = NULL;
 myName   = NULL;

}  // end constructor ( 1 )

OSD_SharedLibrary :: OSD_SharedLibrary ( const Standard_CString aFilename ) {

 myHandle = NULL;
 myName   = NULL;

 SetName ( aFilename );

}  // end constructro ( 2 )

void OSD_SharedLibrary :: SetName ( const Standard_CString aName ) {

 OSD_Path                path ( aName );
 TCollection_AsciiString name ( aName );

 if ( myName != NULL )

  delete [] myName;

 myName = new Standard_Character[ strlen ( aName ) + 1 ];

 strcpy ( myName, aName );

 name = path.Name ();
 name.AssignCat (  path.Extension ()  );

 TCollection_ExtendedString nameW (name);
#ifndef OCCT_UWP
 myHandle = GetModuleHandleW (nameW.ToWideString());
#else
 myHandle = LoadPackagedLibrary (nameW.ToWideString(), NULL);
 FreeLibrary ((HMODULE) myHandle);
#endif

}  // end OSD_SharedLibrary :: SetName

Standard_CString OSD_SharedLibrary :: Name () const {

 return myName;

}  // end OSD_SharedLibrary :: Name

Standard_Boolean OSD_SharedLibrary :: DlOpen ( const OSD_LoadMode /*Mode*/ ) {

 Standard_Boolean retVal = Standard_True;

 if (myHandle == NULL)
 {
  TCollection_ExtendedString myNameW (myName);
#ifndef OCCT_UWP
  myHandle = (HINSTANCE)LoadLibraryExW (myNameW.ToWideString(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
  myHandle = (HINSTANCE)LoadPackagedLibrary (myNameW.ToWideString(), NULL);
#endif
  if ( myHandle == NULL ) {
   lastDLLError = GetLastError ();
   retVal       = Standard_False;
  }
 }  // end if

 return retVal;

}  // end OSD_SharedLibrary :: DlOpen

OSD_Function OSD_SharedLibrary :: DlSymb ( const Standard_CString Name ) const {

 OSD_Function func = ( OSD_Function )GetProcAddress (  ( HMODULE )myHandle, Name  );

 if ( func == NULL )

  lastDLLError = GetLastError ();

 return func;

}  // end OSD_SharedLibrary :: DlSymb

void OSD_SharedLibrary :: DlClose () const {

 FreeLibrary (  ( HMODULE )myHandle  );

}  // end OSD_SharedLibrary :: DlClose

Standard_CString OSD_SharedLibrary :: DlError () const {

 FormatMessageW (
  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
  0, lastDLLError, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
   errMsg, 1024, ( va_list* )&myName
 );

 WideCharToMultiByte(CP_UTF8, 0, errMsg, -1, errMsgA, sizeof(errMsgA), NULL, NULL);
 return errMsgA;
}  // end OSD_SharedLibrary :: DlError

void OSD_SharedLibrary :: Destroy () {

 if ( myName != NULL ) delete [] myName;

}  // end OSD_SharedLibrary :: Destroy

#endif
