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
