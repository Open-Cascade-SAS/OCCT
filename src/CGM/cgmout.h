/*
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

#ifndef CGMOUT_H
#define CGMOUT_H

/*  Include all files on which this is dependent.  */

#include "cgmmach.h"
#include "cgmtypes.h"
#include "cgmstruc.h"

/*  Include file that all modules using this file will use.  */

#include "cgmatt.h"
#include "cgmerr.h"

/*  external structures needed by driver  */

  extern const struct char_defaults chardef;
  extern struct char_defaults mfchar, curchar, oldchar;
  extern const struct text_defaults textdef;
  extern struct text_defaults mftext, curtext;
  extern const struct bin_defaults bindef;
  extern struct bin_defaults mfbin, curbin;
  extern struct defaults cur, old;
  extern struct attributes curatt, oldatt;

  extern struct flags att;

  extern Enum cgmdriver;
  extern Enum cgmstate;

  extern long *null;

  extern Code st_start, st_term;

/*  Global variable set as arguments for output control */

  extern Logical cgmralgks, cgmverbose, cgmquiet, cgmEOF, cgmfinished;

/*  Variables for random frame access    */

  extern long cgmnext, cgmpresent, cgmabort;

/*  Structures for font list and character set list */

  extern struct cgmfont cgmfonts;
  extern struct cgmcset cgmcsets;

/* Macros for common Maths functions  */

#define REQUAL(x,y) ( FABS((x)-(y)) <= cur.realmin )
#define VEQUAL(x,y) ( FABS((x)-(y)) <= cur.vdcmin )
#define PEQUAL(p,q) ( VEQUAL((p)->x, (q)->x) && VEQUAL((p)->y, (q)->y) )
#define XPROD(a,b,c,d) ( (b->x - a->x)*(d->y - c->y)\
                       - (d->x - c->x)*(b->y - a->y) )
#endif   /*  end of cgmout.h */
