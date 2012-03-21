// Copyright (c) 1998-1999 Matra Datavision
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


#ifndef WNT

//---------- All Systems except windowsNT : ----------------------------------

#include <OSD_EnvironmentIterator.ixx>
#include <OSD_WhoAmI.hxx>

//const OSD_WhoAmI Iam = OSD_WEnvironmentIterator;
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
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

#include <OSD_EnvironmentIterator.ixx>

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
