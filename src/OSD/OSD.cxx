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

#include <OSD.hxx>

// Convert Real to CString in format e with 16 significant digits.
// The decimal point character is always a period.
// The conversion is independant from current locale database

Standard_Boolean OSD::RealToCString(const Standard_Real aReal,
				    Standard_PCharacter& aString)
{
  char *p, *q ;
  
  if (Sprintf(aString,"%.17e",aReal)  <= 0) //BUC60808
    return Standard_False ;

  // Suppress "e+00" and unsignificant 0's 

  p = strchr(aString,'e');
  if (p) {
    if (!strcmp(p,"e+00"))
      *p = 0 ;
    for (q = p-1 ; *q == '0' ; q--) ;
    if (q != p-1) {
      if (*q != '.') q++ ;
      while (*p)
	*q++ = *p++ ;
      *q = 0 ;
    }
  }
  return Standard_True ;
}

// Make the RealToCString reciprocal conversion.

Standard_Boolean OSD::CStringToReal(const Standard_CString aString,
				    Standard_Real& aReal)
{
  char *endptr ;
  aReal = Strtod(aString, &endptr);
  if (*endptr)
    return Standard_False ;
  return Standard_True;
}

//=======================================================================
//function : OSDSecSleep
//purpose  : Cause the process to sleep during a amount of seconds 
//=======================================================================

#ifdef _WIN32
# include <Windows.h>
# define SLEEP(NSEC)                 Sleep(1000*(NSEC))
#else
#include <unistd.h>
# define SLEEP(NSEC)                 sleep(NSEC)
#endif

void OSD::SecSleep(const Standard_Integer aDelay)
{
  SLEEP(aDelay);
}

//=======================================================================
//function : MilliSecSleep
//purpose  : Cause the process to sleep during a amount of milliseconds  
//=======================================================================

#ifdef _WIN32

void OSD::MilliSecSleep(const Standard_Integer aDelay)
{
  Sleep(aDelay) ;
}

#else

#include <sys/time.h>

void OSD::MilliSecSleep(const Standard_Integer aDelay)
{
  struct timeval timeout ;

  timeout.tv_sec = aDelay / 1000 ;
  timeout.tv_usec = (aDelay % 1000) * 1000 ;

  select(0,NULL,NULL,NULL,&timeout) ;
}

#endif
