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
#include <NCollection_UtfString.hxx>

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
   throw Standard_ConstructionError("OSD_Environment::OSD_Environment: bad argument");

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
   throw Standard_ConstructionError("OSD_Environment::OSD_Environment: bad argument");

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
   throw Standard_ConstructionError("OSD_Environment::SetName: bad argument");

 myName = Name;
}

// ----------------------------------------------------------------------
// Change value 
// ----------------------------------------------------------------------

void OSD_Environment::SetValue (const TCollection_AsciiString& Value)
{
 if (!Value.IsAscii() || Value.Search("$") != -1) 
   throw Standard_ConstructionError("OSD_Environment::Change: bad argument");

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

#include <windows.h>

#include <OSD_Environment.hxx>

#include <OSD_WNT.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_UtfString.hxx>
#include <Standard_Mutex.hxx>

#ifdef OCCT_UWP
namespace
{
  // emulate global map of environment variables
  static Standard_Mutex THE_ENV_LOCK;
  static NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> THE_ENV_MAP;
}
#else
static void __fastcall _set_error ( OSD_Error&, DWORD );
#endif

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

TCollection_AsciiString OSD_Environment::Value()
{
  myValue.Clear();
#ifdef OCCT_UWP
  Standard_Mutex::Sentry aLock (THE_ENV_LOCK);
  THE_ENV_MAP.Find (myName, myValue);
#else

  // msvc C-runtime (_wputenv()) puts variable using WinAPI internally (calls SetEnvironmentVariableW())
  // and also caches its value in its own map,
  // so that _wgetenv() ignores WinAPI and retieves variable from this cache.
  //
  // Using _wgetenv() might lead to awkward results in context when several C-runtimes are used
  // at once within application or WinAPI is used directly for setting environment variable.
  //
  // Using _wputenv() + GetEnvironmentVariableW() pair should provide most robust behavior in tricky scenarios.
  // So that  _wgetenv() users will retrieve proper value set by OSD_Environment if used C-runtime library is the same as used by OCCT,
  // and OSD_Environment will retreieve most up-to-date value of environment variable nevertheless C-runtime version used (or not used at all) for setting value externally,
  // considering msvc C-runtime implementation details.
  SetLastError (ERROR_SUCCESS);
  NCollection_UtfWideString aNameWide (myName.ToCString());
  DWORD aSize = GetEnvironmentVariableW (aNameWide.ToCString(), NULL, 0);
  if (aSize == 0 && GetLastError() != ERROR_SUCCESS)
  {
    _set_error (myError, ERROR_ENVVAR_NOT_FOUND);
    return myValue;
  }

  NCollection_Utf8String aValue;
  aSize += 1; // NULL-terminator
  wchar_t* aBuff = new wchar_t[aSize];
  GetEnvironmentVariableW (aNameWide.ToCString(), aBuff, aSize);
  aBuff[aSize - 1] = L'\0';
  aValue.FromUnicode (aBuff);
  delete[] aBuff;
  Reset();

  myValue = aValue.ToCString();
#endif
  return myValue;
}

void OSD_Environment :: SetName ( const TCollection_AsciiString& name ) {

 myName = name;

}  // end OSD_Environment :: SetName

TCollection_AsciiString OSD_Environment :: Name () const {

 return myName;

}  // end OSD_Environment :: Name

void OSD_Environment::Build()
{
#ifdef OCCT_UWP
  Standard_Mutex::Sentry aLock(THE_ENV_LOCK);
  THE_ENV_MAP.Bind (myName, myValue);
#else
  NCollection_Utf8String aSetVariable = NCollection_Utf8String(myName.ToCString()) + "=" + myValue.ToCString();
  _wputenv (aSetVariable.ToUtfWide().ToCString());
#endif
}

void OSD_Environment::Remove()
{
#ifdef OCCT_UWP
  Standard_Mutex::Sentry aLock(THE_ENV_LOCK);
  THE_ENV_MAP.UnBind (myName);
#else
  NCollection_Utf8String aSetVariable = NCollection_Utf8String(myName.ToCString()) + "=";
  _wputenv (aSetVariable.ToUtfWide().ToCString());
#endif
}

Standard_Boolean OSD_Environment :: Failed () const {

 return myError.Failed ();

}  // end OSD_Environment :: Failed

void OSD_Environment :: Reset () {

 myError.Reset ();

}  // end OSD_Environment :: Reset

void OSD_Environment :: Perror ()
{
  myError.Perror ();
}  // end OSD_Environment :: Perror

Standard_Integer OSD_Environment :: Error () const {

 return myError.Error ();

}  // end OSD_Environment :: Error

#ifndef OCCT_UWP
static void __fastcall _set_error (OSD_Error& theErr, DWORD theCode)
{
  wchar_t aBuffer[2048];
  const DWORD anErrCode = theCode != 0 ? theCode : GetLastError();
  if (!FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                       0, anErrCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
                       aBuffer, 2048, NULL))
  {
    theErr.SetValue (anErrCode, OSD_WEnvironment, TCollection_AsciiString ("error code ") + (Standard_Integer)anErrCode);
    SetLastError (anErrCode);
  }
  else
  {
    theErr.SetValue (anErrCode, OSD_WEnvironment, TCollection_AsciiString (aBuffer));
  }
}
#endif

#endif
