/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

typedef unsigned short char16 ;

#include <Resource_Shiftjis.h>
#include <Resource_gb2312.h>

#define isjis(c) (((c)>=0x21 && (c)<=0x7e))
#define iseuc(c) (((c)>=0xa1 && (c)<=0xfe))
#define issjis1(c) (((c)>=0x81 && (c)<=0x9f) || ((c)>=0xe0 && (c)<=0xef))
#define issjis2(c) ((c)>=0x40 && (c)<=0xfc && (c)!=0x7f)
#define ishankana(c) ((c)>=0xa0 && (c)<=0xdf)
#define isshift(c) (((c)>=0x80 && (c)<=0xff))


static void sjis_to_jis (unsigned int *ph, unsigned int *pl)
{

  if ( ! issjis1 ( *ph ) || ! issjis2 ( *pl ) ) {
    return ;
  }

  if (*ph <= 0x9f)
    {
      if (*pl < 0x9f)
	*ph = (*ph << 1) - 0xe1;
      else
	*ph = (*ph << 1) - 0xe0;
    }
  else
    {
      if (*pl < 0x9f)
	*ph = (*ph << 1) - 0x161;
      else
	*ph = (*ph << 1) - 0x160;
    }
  if (*pl < 0x7f)
    *pl -= 0x1f;
  else if (*pl < 0x9f)
    *pl -= 0x20;
  else
    *pl -= 0x7e;
}

static void jis_to_sjis (unsigned int *ph, unsigned int *pl)
{
  if (*ph & 1)
    {
      if (*pl < 0x60)
	*pl += 0x1f;
      else
	*pl += 0x20;
    }
  else
    *pl += 0x7e;
  if (*ph < 0x5f)
    *ph = (*ph + 0xe1) >> 1;
  else
    *ph = (*ph + 0x161) >> 1;
}

static void euc_to_sjis (unsigned int *ph, unsigned int *pl)
{
  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }

  if ( ! iseuc ( *ph ) || ! iseuc ( *pl ) ) {
    return ;
  }


  *ph &= 0x7F ;
  *pl &= 0x7F	;

  jis_to_sjis ( ph , pl ) ;

}

static void sjis_to_euc (unsigned int *ph, unsigned int *pl)
{
  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }

  if ( ! issjis1 ( *ph ) || ! issjis2 ( *pl ) ) {
    return ;
  }

  if ( *ph == 0 && *pl == 0 )
    return ;

  sjis_to_jis ( ph , pl ) ;

  *ph |= 0x80 ;
  *pl |= 0x80 ;

}

void Resource_sjis_to_unicode (unsigned int *ph, unsigned int *pl)
{
  char16 sjis ;
  char16 uni  ;
	
  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }

  if ( ! issjis1 ( *ph ) || ! issjis2 ( *pl ) ) {
    return ;
  }

  sjis = (char16)(((*ph) << 8) | (*pl)) ;
  uni  = sjisuni [sjis] ;
  *ph = uni >> 8 ;
  *pl = uni & 0xFF ;
}

void Resource_unicode_to_sjis (unsigned int *ph, unsigned int *pl)
{
  char16 sjis ;
  char16 uni  ;
	
  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }
  if ( *ph == 0 && *pl == 0 )
    return ;

  uni  = (char16)(((*ph) << 8) | (*pl)) ;
  sjis = unisjis [uni] ;
  *ph = sjis >> 8 ;
  *pl = sjis & 0xFF ;
}

void Resource_unicode_to_euc (unsigned int *ph, unsigned int *pl)
{

  if ( *ph == 0 && *pl == 0 )
    return ;

  Resource_unicode_to_sjis ( ph , pl ) ;
  if (issjis1(*ph)) {		/* let's believe it is ANSI code if it is not sjis*/
    sjis_to_euc     ( ph , pl ) ;
  }

}

void Resource_euc_to_unicode (unsigned int *ph, unsigned int *pl)
{

  if ( ! iseuc ( *ph ) || ! iseuc ( *pl ) ) {
    return ;
  }


  if ( *ph == 0 && *pl == 0 )
    return ;

  euc_to_sjis     ( ph , pl ) ;
  Resource_sjis_to_unicode ( ph , pl ) ;

}


void Resource_gb_to_unicode (unsigned int *ph, unsigned int *pl)
{
  char16 gb   ;
  char16 uni  ;
	

  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }

  if ( ! isshift ( *ph ) || ! isshift ( *pl ) ) {
    return ;
  }

  *ph  = (*ph) & 0x7f ;
  *pl  = (*pl) & 0x7f ;

  gb   = (char16)(((*ph) << 8) | (*pl)) ;
  uni  = gbuni [gb] ;
  *ph  = uni >> 8 ;
  *pl  = uni & 0xFF ;
}

void Resource_unicode_to_gb (unsigned int *ph, unsigned int *pl)
{
  char16 gb   ;
  char16 uni  ;
	
  if ( (*ph & 0xFFFFFF00) || (*pl & 0xFFFFFF00) ) {
    *ph = 0 ;
    *pl = 0 ;
    return ;
  }
  if ( *ph == 0 && *pl == 0 )
    return ;

  uni  = (char16)(((*ph) << 8) | (*pl));
  gb   = unigb [uni] ;
  if (gb != 0) {
    *ph  = ( gb >> 8   ) | 0x80 ;
    *pl  = ( gb & 0xFF ) | 0x80 ;
  }
  else {
    *ph = 0;
    *pl = 0 ;
  }
}
