// Created on: 2002-01-25
// Created by: doneux
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


// Lastly modified by :
// +---------------------------------------------------------------------------+
// !   doneux ! Creation                                !  06/17/02!    %V%-1!
// +---------------------------------------------------------------------------+

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <OSD_Real2String.ixx>
#include <stdio.h>
#if defined(HAVE_STDLIB_H) || defined(WNT)
# include <stdlib.h>
#endif

//=======================================================================
//function : OSD_Real2String
//purpose  : 
//=======================================================================

OSD_Real2String::OSD_Real2String():
 myReadDecimalPoint(0)
{
  float F1 = (float ) 1.1 ;
  char str[5] ;

  sprintf(str,"%.1f",F1) ;
                             //  printf("%s\n",str) ;
  myLocalDecimalPoint = str[1] ;
}

//=======================================================================
//function : RealToCString
//purpose  : 
//=======================================================================

Standard_Boolean OSD_Real2String::RealToCString(const Standard_Real theReal,
						Standard_PCharacter& theString) const
{
  char *p, *q ;

  if (sprintf(theString,"%.17e",theReal)  <= 0) 
    return Standard_False ;

  // Suppress "e+00" and unsignificant 0's 

  if ((p = strchr(theString,'e'))) {
    if (!strcmp(p,"e+00"))
      *p = 0 ;
    for (q = p-1 ; *q == '0' ; q--) ;
    if (q != p-1) {
 
      if (*q != myLocalDecimalPoint) q++ ;

      while (*p)
	*q++ = *p++ ;
      *q = 0 ;
    }
  }
  return Standard_True ;
}

//=======================================================================
//function : CStringToReal
//purpose  : 
//=======================================================================

Standard_Boolean OSD_Real2String::CStringToReal(const Standard_CString theString,
						Standard_Real& theReal)
{
  char *endptr ;

  if (! theString) return Standard_False;

   // Get the decimal point character in the string (if any)
  if (!myReadDecimalPoint) {
    if (strchr(theString, ',')) myReadDecimalPoint = ',';
    else if (strchr(theString, '.')) myReadDecimalPoint = '.';
  }


  const char *str = theString;
  if (myReadDecimalPoint) {
      if (myReadDecimalPoint != myLocalDecimalPoint) {
	  const char * p;
          char buff[1024]; 
	  // replace the decimal point by the local one
          if(myReadDecimalPoint != myLocalDecimalPoint && 
             (p = strchr(theString,myReadDecimalPoint))&& ((p-theString) < 1000) )
          {
            strncpy(buff, theString, 1000);
            buff[p-theString] = myLocalDecimalPoint;
            str = buff;
          }
      }
  }

  theReal = strtod(str,&endptr) ;
  if (*endptr)
    return Standard_False ;

  return Standard_True;
}

