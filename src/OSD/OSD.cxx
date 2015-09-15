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
#include <Standard_Stream.hxx>

#include <math.h>
#include <stdio.h>
#if defined(isfinite)
# define finite isfinite
#endif

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
#if !defined(__CYGWIN32__) && !defined(__MINGW32__)
//# include <Mapiwin.h>
#endif
# define _DEXPLEN	             11
# define _IEEE		              1
# define DMAXEXP	             ((1 << _DEXPLEN - 1) - 1 + _IEEE)
# define finite                      _finite
# define SLEEP(NSEC)                 Sleep(1000*(NSEC))
#else
# define SLEEP(NSEC)                 sleep(NSEC)
#endif
#ifndef _WIN32
#include <unistd.h>
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

//=======================================================================
//function : IsDivisible
//purpose  : 
//=======================================================================

Standard_Boolean OSD::IsDivisible(const Standard_Real theDividend,const Standard_Real theDivisor)
{
  if ( theDivisor == 0. || ! finite(theDividend) ) return Standard_False;
  //
  // you may divide by infinity
  //
  if (! finite(theDivisor)) return Standard_True;
#ifdef OCCT_DEBUG
//   Standard_Integer aExp1,  aExp2;
//   Standard_Real aMant1 = frexp(theDividend, &aExp1);
//   Standard_Real aMant2 = frexp(theDivisor, &aExp2);
#endif
// Code de :KL:dev:ref :
//  return ((aExp1-aExp2 < DMAXEXP) ||        // exponent of the result
//	  (aExp1-aExp2 == DMAXEXP && aMant1 < aMant2)) ;

// Code de :KAS:C30 :
  return Standard_True;


// this is unacceptable return for Linux because of (temporary?) undefined  aExp1 and aExp2.
// Testing both over and underflow should be (:KL:dev ) :

//  return ((aExp1-aExp2 < DMAXEXP) && (aExp1-aExp2 > DMINEXP) ||
//	  (aExp1-aExp2 == DMAXEXP && aMant1 < aMant2) ||
//	  (aExp1-aExp2 == DMINEXP && aMant1 > aMant2)) ;
}

//=======================================================================
//function : GetExponent
//purpose  : 
//=======================================================================

//Standard_Integer OSD::GetExponent(const Standard_Real aReal)
Standard_Integer OSD::GetExponent(const Standard_Real )
{
// Code de :KAS:C30 :

  cout << "Function OSD::GetExponent() not yet implemented." << endl;
  return 0 ;

// Code de :KL:dev:ref :

//  Standard_Integer Exp ;

////  Standard_Real Mant = frexp(aReal, &Exp) ;
//  frexp(aReal, &Exp) ;
//  return Exp ;
}

//=======================================================================
//function : GetMantissa
//purpose  : 
//=======================================================================

//Standard_Real OSD::GetMantissa(const Standard_Real aReal)
Standard_Real OSD::GetMantissa(const Standard_Real )
{
// Code de :KAS:C30 :
  cout << "Function OSD::GetMantissa() not yet implemented." << endl;
  return 0 ;

// Code de :KL:dev:ref :

//  Standard_Integer Exp ;
//  return frexp(aReal, &Exp) ;
}

//=======================================================================
// Code de :KAS:C30 :
//=======================================================================
Standard_Integer OSD::AvailableMemory()
{
  cout << "Function OSD::AvailableMemory() not yet implemented." << endl;
  return 0 ;
}

// Code de :KL:dev:ref ??????????????? :
#if 0
//=======================================================================
//function : AvailableMemory
//purpose  : 
//=======================================================================
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

# if defined(SUN) || defined(__sun) || defined(SOLARIS)
#  define SIZE_MAX  0x7fffffff
# elif defined(__osf__)  || defined(DECOSF1)        
#  define SIZE_MAX  0x10000000000 
# elif defined(_WIN32)
#  define SIZE_MAX 0x7ffdefff
# else
#  define SIZE_MAX  0xffffffff
# endif

Standard_Integer OSD::AvailableMemory()
{
  size_t min = 1024 ;
  size_t max = SIZE_MAX ;
  size_t middle = SIZE_MAX ;
  void * addr ;
  int nballoc = 0 ;
  int nbfree = 0 ;

  while (min + 1024 < max) {
    if ((addr =  malloc (middle))== (void *)-1) {
      perror("OSD::AvailableMemory()_malloc error :") ;
      return 0 ;
    }
    nballoc++ ;
    if (addr == 0)
      max = middle ;
    else {
      free(addr) ;
      nbfree++ ;
      min = middle ;
    }
    middle = min + ((max - min ) >> 6) * 63 ;
  }
  return min >> 10 ;
}

#endif
