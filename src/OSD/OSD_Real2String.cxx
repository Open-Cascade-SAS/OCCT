// File:	OSD_Real2String.cxx
// Created:	Fri Jan 25 12:58:55 2002
// Author:	doneux <doneux@samcef.com>

const static char sccsid[] = "@(#) OSD_Real2String.cxx    %V%-1, 06/17/02@(#)";
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

