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

/*  Get local machine id, operating system, compiler and/or library */ 
#ifndef CGMMACH_H
#define CGMMACH_H

/*  C Library routines needed in standard include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

/*  Current CGM version */
#define CGMVERSION   1

#define const
#define volatile
#define FREE(x) free ( (char *) x )

#ifndef NULL     /*  Null Pointer */
#define NULL     (int *) 0
#endif

/*  Define anything that has not been explicitly set  **************/

#ifndef SignChar
#define SignChar signed char
#endif
#ifndef Int16
#define Int16   short
#endif
#ifndef Char
#define Char    char
#endif
#ifndef Int
#define Int     int
#endif
#ifndef Long
#define Long    long
#endif
#ifndef Float
#define Float   float
#endif
#ifndef Double
#define Double  double
#endif

/* Define macros for some maths routines which may cause problems */

#ifndef SQRT
#define SQRT(x)  sqrt( (double) (x) )
#endif
#ifndef FABS
#define FABS(x)  fabs( (double) (x) )
#endif
#ifndef MALLOC
#define MALLOC(n,s)   malloc( (size_t) ((n)*(s)) )
#endif

/*  Define if font system needed */

#ifdef XW
#define FONTSYSTEM
#else
#ifdef POSTSCRIPT
#define FONTSYSTEM
#else
#ifdef IGL
#define FONTSYSTEM
#endif
#endif
#endif


#ifndef FIXED_BUFFER  /* Use variable communications Area by default */
#define VAR_BUFFER
#endif

#ifndef ARRAY_MAX    /*  Maximum Sizes for Interface buffer  */
#define ARRAY_MAX   1024
#endif

#ifndef STRING_MAX   /*  Maximum String length */
#define STRING_MAX  1024
#endif

#ifndef DEFOUT       /*  Default Standard Output type  */
#define DEFOUT  'T'
#endif

#ifndef ERROR_MAX    /*  Maximum Number of errors */
#define ERROR_MAX     16
#endif

#ifndef DEFVERBOSE   /*  Default Verbose mode flag */
#define DEFVERBOSE    FALSE
#endif

#ifndef DEFQUIET     /* Default quiet output flag */
#define DEFQUIET      FALSE
#endif

#ifndef DEFMOVIE     /*  Default Continuous play mode */
#define DEFMOVIE      FALSE
#endif

#ifndef DEFDOUBLE    /*  Default Double buffer mode */
#define DEFDOUBLE     FALSE
#endif

#ifndef DEFFULLSCRN  /*  Default Full screen mode */
#define DEFFULLSCRN   TRUE
#endif

#ifndef DEFRALGKSGDP /*  RAL-GKS GDPs recognised */
#define DEFRALGKSGDP  FALSE
#endif

#ifndef LPOINT       /*  Define default LPOINT commands */
#define Lpoint Point

#define LPOINT(p)          Lpoint *p
#define BEGINLIST(p)
#define STOREPOINT(p,q)    (*p).x = q.x; (*p++).y = q.y
#define STOREVALUE(p,q,r)  (*p).x = q; (*p++).y = r
#define CLOSELIST(p)
#define NEWSUBLIST(p)
#define ENDLIST(p)
#endif

#ifndef MAXFNSIZE    /*  Maximum font name size */
#define MAXFNSIZE     40
#endif

#ifndef DATADIR      /*  Directory for font & data files */
#define DATADIR      ""
#endif

#ifndef FILESEP      /*  File seperator  */
#define FILESEP      "."
#endif

#ifndef PATHDELIM    /*  Path seperator  */
#define PATHDELIM    '/'
#endif

#ifndef MAXFLSIZE       /*  Maximum length of filename - whole tree */
#define MAXFLSIZE    80
#endif

#ifndef NAMELGTH        /*  Maximum length of filename - name only */
#define NAMELGTH     8
#endif

#ifndef EXTLGTH         /*  Maximum length of filename - extension */
#define EXTLGTH      3
#endif

#ifndef LFILEDEF        /*  Maximum length of filename extra data */
#define LFILEDEF     0
#endif

#ifndef F_READ         /*  Read file mode  */
#define F_READ     'r'
#endif

#ifndef F_WRITE        /*  Write file mode  */
#define F_WRITE    'w'
#endif

#ifndef BUFF_LEN     /*  CGM output record length */
#define BUFF_LEN      (size_t) 80
#endif

#ifndef OBUFF_LEN    /*  CGM output buffer size */
#define OBUFF_LEN    (size_t) 512
#endif

#ifndef NFRAME       /*  Maximum number of frames */
#define NFRAME       100
#endif

#ifndef NMETA        /*  Maximum number of metafiles */
#define NMETA        10
#endif

#ifndef MAXPTS      /*  Maximum storage for fill area processing */
#define MAXPTS     4096
#endif

#ifndef SET_MAX      /*  Maximum number of Polygon sets ?? */
#define SET_MAX       50
#endif

#ifndef COLOUR_TAB  /*  Colour table size */
#define COLOUR_TAB  256
#endif

#ifndef COLOUR_BITS /*  Number of bit planes */
#define COLOUR_BITS   8
#endif

#ifndef PATT_TAB   /*  Pattern table size */
#define PATT_TAB   1024
#endif

#ifndef PATTERNS  /* Maximum number of Patterns */
#define PATTERNS     40
#endif

#ifndef NPOWER    /* No of entries in power-of-two table  */
#define NPOWER       32
#endif

#ifndef HASHSIZE  /* No of entries in Code hash table  */
#if CGMVERSION == 1
#define HASHSIZE     280
#endif
#if CGMVERSION == 2
#define HASHSIZE     512
#endif
#if CGMVERSION == 3
#define HASHSIZE     1024
#endif
#endif

#ifndef NULLSTR   /* Null string pointer */
#define NULLSTR  (char *) NULL
#endif

#ifdef DEBUG            /* turn on extra debugging in font package  */
#define CSETNAMES
#define DESNAMES
#endif

     /*  All the following definitions provide limits for the number
      |  of values that can be read in by the font utility; they do
      |  not cause any permanent memory to be allocated, so can be
      |  set high for safety.
      */

#ifndef MAXCSET           /*  maximum number of ISO character sets  */
#define MAXCSET   1000
#endif

#ifndef MAXFUDGE          /*  maximum number of fudge character sets  */
#define MAXFUDGE    25
#endif

#ifndef MAXFAMILY         /*  maximum number of typeface family definitions  */
#define MAXFAMILY 2000
#endif

#ifndef MAXALIAS          /*  maximum number of family aliases  */
#define MAXALIAS   200
#endif

#ifndef MAXSFONT          /*  maximum number of specific fonts  */
#define MAXFONT   2000
#endif

#ifndef MAXSOURCE         /*  maximum number of typeface sources  */
#define MAXSOURCE  100
#endif

#ifndef MAXDESIGN         /*  maximum number of ISO design groups  */
#define MAXDESIGN  200
#endif

#ifndef MAXMETHOD         /*  maximum number of output methods  */
#define MAXMETHOD    5
#endif

#ifndef MAXFPERM          /*  maximum number of fonts per method  */
#define MAXFPERM  1000
#endif

#ifndef MAXREQ            /*  maximum number of fonts per method  */
#define MAXREQ    1000
#endif

#endif /* end of cgmmach.h */
