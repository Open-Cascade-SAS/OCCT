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

//---------- All Systems except windowsNT : ----------------------------------

#include <OSD_Environment.hxx>
#include <OSD_EnvironmentIterator.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_WhoAmI.hxx>

//const OSD_WhoAmI Iam = OSD_WEnvironmentIterator;
#ifdef __APPLE__
  #import <TargetConditionals.h>
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    extern char **environ;
  #else
    #include <crt_externs.h>
    #define environ (*_NSGetEnviron())
  #endif
#else
  extern char **environ;
#endif

OSD_EnvironmentIterator::OSD_EnvironmentIterator(){
 myCount = 0;
}

// For Windows NT compatibility

void OSD_EnvironmentIterator::Destroy () {}

// Is there another environment variable entry ?

Standard_Boolean OSD_EnvironmentIterator::More(){
 if (environ[myCount+1] == NULL) return(Standard_False); 
                            else return(Standard_True);
}

// Find next environment variable

void OSD_EnvironmentIterator::Next(){
  if (More()) myCount++;
}


OSD_Environment OSD_EnvironmentIterator::Values(){
 TCollection_AsciiString name,value;

 name = environ[myCount];  // Copy environment variable

// Pour DEBUG  cout << name << endl;

 value = &environ[myCount][name.Search("=")]; // Gets its value
 if (name.Length() != 0){
    name = name.Token("="); // Gets its name
 }

 OSD_Environment result(name,value);
 return(result);
}


void OSD_EnvironmentIterator::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_EnvironmentIterator::Failed()const{
 return( myError.Failed());
}

void OSD_EnvironmentIterator::Perror() {
 myError.Perror();
}


Standard_Integer OSD_EnvironmentIterator::Error()const{
 return( myError.Error());
}
   
#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_Directory --------------
//------------------------------------------------------------------------

#define STRICT
#include <windows.h>


#include <OSD_Environment.hxx>
#include <OSD_EnvironmentIterator.hxx>
#include <OSD_OSDError.hxx>

OSD_EnvironmentIterator :: OSD_EnvironmentIterator () {

 myEnv   = GetEnvironmentStrings ();
 myCount = ( Standard_Integer )myEnv;

}  // end constructor

void OSD_EnvironmentIterator :: Destroy () {

 FreeEnvironmentStrings (  ( LPTSTR )myEnv  );

}  // end OSD_EnvironmentIterator :: Destroy

Standard_Boolean OSD_EnvironmentIterator :: More () {

 return *(  ( Standard_CString )myCount  ) ? Standard_True : Standard_False;

}  // end OSD_EnvironmentIterator :: More

void OSD_EnvironmentIterator :: Next () {

 if (  More ()  ) {
 
  while (   *( Standard_CString )myCount  ) ++myCount;

  ++myCount;
 
 }  // end if

}  // end OSD_EnvironmentIterator :: Next

OSD_Environment OSD_EnvironmentIterator :: Values () {

 TCollection_AsciiString env, name, value;

 env = ( Standard_CString )myCount;

 name  = env.Token (  TEXT( "=" ), 1  );
 value = env.Token (  TEXT( "=" ), 2  );

 if (  env.Value ( 1 ) == TEXT( '=' )  ) name.Insert (  1, TEXT( '=' )  );

 return OSD_Environment ( name, value );

}  // end OSD_EnvironmentIterator :: Values

Standard_Boolean OSD_EnvironmentIterator :: Failed () const {

 return myError.Failed ();

}  // end OSD_EnvironmentIterator :: Failed

void OSD_EnvironmentIterator :: Reset () {

 myError.Reset ();

}  // end OSD_EnvironmentIterator :: Reset

void OSD_EnvironmentIterator :: Perror () {

 myError.Perror ();

}  // end OSD_EnvironmentIterator :: Perror

Standard_Integer OSD_EnvironmentIterator :: Error () const {

 return myError.Error ();

}  // end OSD_EnvironmentIterator :: Error

#endif
