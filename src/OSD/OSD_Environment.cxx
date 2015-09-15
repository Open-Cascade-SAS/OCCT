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


#include <OSD_Environment.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const OSD_WhoAmI Iam = OSD_WEnvironment;

// ----------------------------------------------------------------------
//
// Updated by : JPT Dec,7 1992
// What       : OSD_Environment::OSD_Environment
//                              (const TCollection_AsciiString& Name,
//                               const TCollection_AsciiString& Value)
//              Value could contain the character $ (Ex setenv a = $c)
// 
// ----------------------------------------------------------------------
// Create object

OSD_Environment::OSD_Environment()
{
}


// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

OSD_Environment::OSD_Environment(const TCollection_AsciiString& Name)
{

 if (!Name.IsAscii() || Name.Search("$") != -1 ) 
   Standard_ConstructionError::Raise("OSD_Environment::OSD_Environment: bad argument");

 myName = Name; 
}


// ----------------------------------------------------------------------
// Create an environment variable and initialize it
// ----------------------------------------------------------------------

OSD_Environment::OSD_Environment(const TCollection_AsciiString& Name,
                                 const TCollection_AsciiString& Value)
{

 if (!Name.IsAscii() || !Value.IsAscii() || 
// JPT : Dec-7-1992     Name.Search("$") != -1 || Value.Search("$") != -1) 
     Name.Search("$") != -1 ) 
   Standard_ConstructionError::Raise("OSD_Environment::OSD_Environment: bad argument");

 myName = Name; 
 myValue = Value;
}


// ----------------------------------------------------------------------
// Returns the name of the symbol
// ----------------------------------------------------------------------

TCollection_AsciiString OSD_Environment::Name () const
{
 return myName;
}

// ----------------------------------------------------------------------
// Set new value for environment variable
// ----------------------------------------------------------------------

void OSD_Environment::SetName (const TCollection_AsciiString& Name)
{
 myError.Reset();
 if (!Name.IsAscii() || Name.Search("$") != -1 ) 
   Standard_ConstructionError::Raise("OSD_Environment::SetName: bad argument");

 myName = Name;
}

// ----------------------------------------------------------------------
// Change value 
// ----------------------------------------------------------------------

void OSD_Environment::SetValue (const TCollection_AsciiString& Value)
{
 if (!Value.IsAscii() || Value.Search("$") != -1) 
   Standard_ConstructionError::Raise("OSD_Environment::Change: bad argument");

 myValue = Value;
}

// ----------------------------------------------------------------------
// Get environment variable physically
// ----------------------------------------------------------------------

TCollection_AsciiString OSD_Environment::Value()
{
 char *result = getenv(myName.ToCString());
 if (result == NULL) myValue.Clear();
 else myValue = result;
 return myValue;
}

// ----------------------------------------------------------------------
// Sets physically the environment variable
// ----------------------------------------------------------------------

void OSD_Environment::Build ()
{
  // Static buffer to hold definitions of new variables for the environment.
  // Note that they need to be static since putenv does not make a copy
  // of the string, but just adds its pointer to the environment.
  static char **buffer  = 0 ;     // JPT:
  static int    Ibuffer = 0 ;     // Tout ca pour putenv,getenv

  // Use mutex to avoid concurrent access to the buffer
  static Standard_Mutex theMutex;
  Standard_Mutex::Sentry aSentry ( theMutex );

  // check if such variable has already been created in the buffer
  int index = -1, len = myName.Length();
  for ( int i=0; i < Ibuffer; i++ ) {
    if ( ! strncmp ( buffer[i], myName.ToCString(), len ) && buffer[i][len] == '=' ) {
      index = i;
      break;
    }
  }

  // and either add a new entry, or remember the old entry for a while
  char *old_value = 0;
  if ( index >=0 ) {
    old_value = buffer[index];
  }
  else {
    // Allocation memoire. Surtout tout la heap!
    index = Ibuffer++;
    char **aTmp;
    aTmp = (char **) realloc ( buffer, Ibuffer * sizeof(char*) );
    if (aTmp)
    {
      buffer = aTmp;
    }
    else
    {
      myError.SetValue(errno, Iam, "Memory realloc failure");
      return;
    }
  }
   
  // create a new entry in the buffer and add it to environment
  buffer[index] = (char *) malloc ( len + myValue.Length() + 2 );
  sprintf(buffer[index], "%s=%s", myName.ToCString(), myValue.ToCString());
  putenv(buffer[index]);

  // then (and only then!) free old entry, if existed
  if ( old_value ) 
    free ( old_value );
  
  // check the result
  char *result = getenv(myName.ToCString());
  if (result == NULL)
    myError.SetValue(errno, Iam, "Set Environment");
}

// ----------------------------------------------------------------------
// Remove physically the environment variable
// ----------------------------------------------------------------------

void OSD_Environment::Remove ()
{
  myValue.Clear();
  Build();
}



// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
void OSD_Environment::Reset()
{
  myError.Reset();
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
Standard_Boolean OSD_Environment::Failed() const
{
  return myError.Failed();
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
void OSD_Environment::Perror() 
{
  myError.Perror();
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
Standard_Integer OSD_Environment::Error() const
{
  return myError.Error();
}

#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Environment --------------------
//------------------------------------------------------------------------

#define STRICT
#include <OSD_Environment.hxx>

#include <OSD_WNT.hxx>

#include <windows.h>

#pragma warning( disable : 4700 )

static void __fastcall _set_error ( OSD_Error&, DWORD );

OSD_Environment :: OSD_Environment () {

}  // end constructor ( 1 )

OSD_Environment :: OSD_Environment ( const TCollection_AsciiString& Name ) {

 myName = Name;

}  // end constructor ( 2 )

OSD_Environment :: OSD_Environment (
                    const TCollection_AsciiString& Name,
                    const TCollection_AsciiString& Value
                   ) {

 myName  = Name;
 myValue = Value;

}  // end constructor ( 3 )

void OSD_Environment :: SetValue ( const TCollection_AsciiString& Value ) {

 myValue = Value;

}  // end OSD_Environment :: SetValue

TCollection_AsciiString OSD_Environment :: Value () {

 Standard_PCharacter pBuff=0;
 DWORD            dwSize = 0;
 char*            envVal = NULL;

 myValue.Clear ();

 SetLastError ( ERROR_SUCCESS );
 dwSize = GetEnvironmentVariable (  myName.ToCString (), pBuff, dwSize  );

 if (    ( dwSize == 0 && GetLastError () != ERROR_SUCCESS ) ||
         (  envVal = getenv (  myName.ToCString ()  )  ) == NULL
 )

  _set_error ( myError, ERROR_ENVVAR_NOT_FOUND );

 else if ( envVal != NULL )

  myValue = envVal;

 else {

  ++dwSize; 
  pBuff = new Standard_Character[ dwSize ];
  GetEnvironmentVariable (  (char *)myName.ToCString (), pBuff, dwSize  );
  myValue = pBuff;
  delete [] pBuff;
  Reset ();
 
 }  // end else

 return myValue;

}  // end OSD_Environment :: Value

void OSD_Environment :: SetName ( const TCollection_AsciiString& name ) {

 myName = name;

}  // end OSD_Environment :: SetName

TCollection_AsciiString OSD_Environment :: Name () const {

 return myName;

}  // end OSD_Environment :: Name

void OSD_Environment :: Build () {

 TCollection_AsciiString str;

 str = myName + TEXT( "=" ) + myValue;

 putenv (  str.ToCString ()  );

}  // end OSD_Environment :: Build

void OSD_Environment :: Remove () {

 TCollection_AsciiString str;

 str = myName + TEXT( "=" );

 putenv (  str.ToCString ()  );

}  // end OSD_Environment :: Remove

Standard_Boolean OSD_Environment :: Failed () const {

 return myError.Failed ();

}  // end OSD_Environment :: Failed

void OSD_Environment :: Reset () {

 myError.Reset ();

}  // end OSD_Environment :: Reset

void OSD_Environment :: Perror () {

 if (  ErrorPrefix ()  )

  (  *ErrorStream ()  ) << TEXT( '\'' ) << myName.ToCString () << TEXT( "' - " );

 myError.Perror ();

}  // end OSD_Environment :: Perror

Standard_Integer OSD_Environment :: Error () const {

 return myError.Error ();

}  // end OSD_Environment :: Error

static void __fastcall _set_error ( OSD_Error& err, DWORD code ) {

 DWORD              errCode;
 Standard_Character buffer[ 2048 ];

 errCode = code ? code : GetLastError ();

 if (  !FormatMessage (
         FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
         0, errCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
         buffer, 2048, NULL
        )
 ) {
 
  sprintf ( buffer, "error code %d", (Standard_Integer)errCode );
  SetLastError ( errCode );

 }  // end if

 err.SetValue ( errCode, OSD_WEnvironment, buffer );

}  // end _set_error

#endif
