// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

// Update JR 12-09-1997 :
//        - three methods of HashCoding of strings : we may keep the value
//          of the hashcode of the string itself. This value is used when
//          resizing of a Map or copying an item from a Map to another Map.
//        - three methods of HashCoding of strings converted to uppercase.

#include <Standard_Type.hxx>

#include <Standard_CLocaleSentry.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <Standard_OStream.hxx>
#include <string.h>
#include <stdarg.h>

//============================================================================
//==== HashCode of a CString
//============================================================================
Standard_Integer HashCode (const Standard_CString Value,
                           const Standard_Integer Upper )
{
  Standard_Integer aLen = (Standard_Integer)strlen(Value);
  return HashCode (HashCodes (Value, aLen), Upper);
}

//============================================================================
//==== HashCode of a CString
//============================================================================
Standard_Integer HashCodes (const Standard_CString Value,
                            const Standard_Integer Len)
{
  // compute DJB2 hash of a string
  Standard_Integer hash = 0;
  const Standard_Character *c = Value;
  for (Standard_Integer i = 0; i < Len; i++, c++)
  {
    /* hash = hash * 33 ^ c */
    hash = ((hash << 5) + hash) ^ (*c);
  }

  return hash;
}

//======================================================================
// Locale-independent equivalents of C functions dealing with conversion
// of string to real and vice-versa
//======================================================================

#ifdef __APPLE__
  // There are a lot of *_l functions availalbe on Mac OS X - we use them
  #define SAVE_TL()
#elif defined(_MSC_VER)
  // MSVCRT has equivalents with slightly different syntax
  #define SAVE_TL()
  #define strtod_l(thePtr, theNextPtr, theLocale)                _strtod_l(thePtr, theNextPtr, theLocale)
  #define vprintf_l(theLocale, theFormat, theArgPtr)             _vprintf_l(theFormat, theLocale, theArgPtr)
  #define vsprintf_l(theBuffer, theLocale, theFormat, theArgPtr) _vsprintf_l(theBuffer, theFormat, theLocale, theArgPtr)
  #define vfprintf_l(theFile,   theLocale, theFormat, theArgPtr) _vfprintf_l(theFile,   theFormat, theLocale, theArgPtr)
#else
  // glibc provides only limited xlocale implementation:
  // strtod_l/strtol_l/strtoll_l functions with explicitly specified locale
  // and newlocale/uselocale/freelocale to switch locale within current thread only.
  // So we switch to C locale temporarily
  #define SAVE_TL() Standard_CLocaleSentry aLocaleSentry;
  #ifndef OCCT_CLOCALE_POSIX2008
    // glibc version for android platform use locale-independent implementation of
    // strtod, strtol, strtoll functions. For other system with locale-depended
    // implementations problems may appear if "C" locale is not set explicitly.
    #if !defined(__ANDROID__) && !defined(__QNX__) && !defined(__MINGW32__)
      #error System does not support xlocale. Import/export could be broken if C locale did not specified by application.
    #endif
    #define strtod_l(thePtr, theNextPtr, theLocale)              strtod(thePtr, theNextPtr)
  #endif
  #define vprintf_l(theLocale, theFormat, theArgPtr)             vprintf(theFormat, theArgPtr)
  #define vsprintf_l(theBuffer, theLocale, theFormat, theArgPtr) vsprintf(theBuffer, theFormat, theArgPtr)
  #define vfprintf_l(theFile,   theLocale, theFormat, theArgPtr) vfprintf(theFile,   theFormat, theArgPtr)
#endif

/*
double Strtod (const char* theStr, char** theNextPtr)
{
  return strtod_l (theStr, theNextPtr, Standard_CLocaleSentry::GetCLocale());
}
*/

double Atof (const char* theStr)
{
  return Strtod (theStr, NULL);
}

int Printf  (const Standard_CString theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vprintf_l (Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}

int Fprintf (FILE* theFile, const char* theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vfprintf_l(theFile, Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}

int Sprintf (char* theBuffer, const char* theFormat, ...)
{
  SAVE_TL();
  va_list argp;
  va_start(argp, theFormat);
  int result = vsprintf_l(theBuffer, Standard_CLocaleSentry::GetCLocale(), theFormat, argp);
  va_end(argp);
  return result;
}
