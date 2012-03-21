/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

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

  sjis = ((*ph) << 8) | (*pl) ;
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

  uni  = ((*ph) << 8) | (*pl) ;
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

  gb   = ((*ph) << 8) | (*pl) ;
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

  uni  = ((*ph) << 8) | (*pl) ;
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
