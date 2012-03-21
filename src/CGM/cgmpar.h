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

#ifndef CGMPAR_H
#define CGMPAR_H

/*  Include all files on which this is dependent.  */

#include "cgmtypes.h"

/*  CGM Encodings */

#define  CHARACTER  (Enum) 2
#define  BINARY     (Enum) 3
#define  CLEAR_TEXT (Enum) 4

/*  CGM profiles allowed  */

#define PROF_CALS     (Enum) 1
#define PROF_GKS      (Enum) 2
#define PROF_INDEXCOL (Enum) 3
#define PROF_INTVDC   (Enum) 4

/*  CGM Statelists */

#define MF_CLOSED    (Enum) 0
#define MF_DESC      (Enum) 1
#define MF_ELEMLIST  (Enum) 2
#define MF_DEFAULTS  (Enum) 3
#define PIC_DESC     (Enum) 4
#define PIC_OPEN     (Enum) 5
#define PIC_CLOSED   (Enum) 6
#define TEXT_OPEN    (Enum) 7

#if CGMVERSION > 1
#define SEG_GLOBAL   (Enum) 8
#define SEG_LOCAL    (Enum) 9
#define FIG_OPEN     (Enum) 10
#endif

/*  CGM Enumerated types */

#define INTEGER     (Enum) 0
#define REAL        (Enum) 1

#define ABSTRACT    (Enum) 0
#define METRIC      (Enum) 1

#define INDEXED     (Enum) 0
#define DIRECT      (Enum) 1

#define ABS         (Enum) 0
#define ABSOLUTE    (Enum) 0
#define SCALED      (Enum) 1

#define INDIV       (Enum) 0
#define INDIVIDUAL  (Enum) 0
#define BUNDLED     (Enum) 1

#define ALLOWED     (Enum) 0
#define FORBIDDEN   (Enum) 1

#define OFF         (Enum) 0
#define ON          (Enum) 1

#define NOACTION    (Enum) 0
#define ACTION      (Enum) 1

/* Enumerated types for text precision */
#define STRING      (Enum) 0
#define CHAR        (Enum) 1
#define STROKE      (Enum) 2

/* Enumerated types for Text Path Values */
#define P_RIGHT     (Enum) 0
#define P_LEFT      (Enum) 1
#define P_UP        (Enum) 2
#define UP          (Enum) 2
#define P_DOWN      (Enum) 3
#define DOWN        (Enum) 3

/* Enumerated types for Text's Horziontal Alignment */
#define NORMHORIZ   (Enum) 0
#define LEFT        (Enum) 1
#define CTR         (Enum) 2
#define RIGHT       (Enum) 3
#define CONTHORIZ   (Enum) 4

/* Enumerated types for Text's Vertical Alignment */
#define NORMVERT    (Enum) 0
#define TOP         (Enum) 1
#define CAP         (Enum) 2
#define HALF        (Enum) 3
#define BASE        (Enum) 4
#define BOTTOM      (Enum) 5
#define CONTVERT    (Enum) 6

/* Final text flag */
#define NOTFINAL    (Enum) 0
#define FINAL       (Enum) 1

/*  Interior styles  */
#define HOLLOW      (Enum) 0
#define SOLID       (Enum) 1
#define PAT         (Enum) 2
#define PATTERN     (Enum) 2
#define HATCH       (Enum) 3
#define EMPTY       (Enum) 4

/*  Closure types */
#define PIE         (Enum) 0
#define CHORD       (Enum) 1
#define NOCLOSE     (Enum) 2

/*  Polygon set enumerated types  */

#define EDGEVISIB   (Enum) 1
#define EDGECLOSE   (Enum) 2

#define INVIS       (Enum) 0
#define VIS         (Enum) 1
#define CLOSEINVIS  (EDGECLOSE + INVIS)
#define CLOSEVIS    (EDGECLOSE + VIS)

/*  MF element list extra elements */
#define DRAWINGSET      (Enum) 0
#define DRAWINGSETPLUS  (Enum) 1
#define VER2            (Enum) 2
#define EXTPRIM         (Enum) 3
#define VER2GKSM        (Enum) 4


/*  Character sets enumerated types */
#define STD94           (Enum) 0
#define STD96           (Enum) 1
#define STD94MULTIBYTE  (Enum) 2
#define STD96MULTIBYTE  (Enum) 3
#define COMPLETECODE    (Enum) 4

/*  Character coding announcer enumerated types */
#define BASIC7BIT   (Enum) 0
#define BASIC8BIT   (Enum) 1
#define EXTD7BIT    (Enum) 2
#define EXTD8BIT    (Enum) 3

/*  ASF settings */

#define ALL         (Enum) 511
#define ALLLINE     (Enum) 510
#define ALLMARKER   (Enum) 509
#define ALLTEXT     (Enum) 508
#define ALLFILL     (Enum) 507
#define ALLEDGE     (Enum) 506

/*  New Version 2 Enumerated types */

#define FRACTION       (Enum) 0
#define MM             (Enum) 1
#define PHYDEVUNITS    (Enum) 2

#define FORCED         (Enum) 0
#define NOTFORCED      (Enum) 2

#define LOCUS          (Enum) 0
#define SHAPE          (Enum) 1
#define LOCUSTHENSHAPE (Enum) 2

#define NO             (Enum) 0
#define YES            (Enum) 1

#define STATELIST      (Enum) 0
#define SEGMENT        (Enum) 1
#define INTERSECTION   (Enum) 1

#define NORMAL         (Enum) 0
#define HIGHLIGHT      (Enum) 1

/*  Element Group names in addition to normal elements */

#define LINEATTR         (Enum) 35
#define MARKERATTR       (Enum) 36
#define TEXTATTR         (Enum) 37
#define CHARATTR         (Enum) 38
#define FILLATTR         (Enum) 39
#define EDGEATTR         (Enum) 40
#define PATATTR          (Enum) 41
#define OUTPUTCTRL       (Enum) 42
#define ALLATTRCTRL      (Enum) 44
#define ALLINH           (Enum) 45
#define LINETYPEASF      (Enum) 46
#define LINEWIDTHASF     (Enum) 47
#define LINECOLRASF      (Enum) 48
#define MARKERTYPEASF    (Enum) 49
#define MARKERSIZEASF    (Enum) 50
#define MARKERCOLRASF    (Enum) 51
#define TEXTFONTINDEXASF (Enum) 52
#define TEXTPRECASF      (Enum) 53
#define CHAREXPANASF     (Enum) 54
#define CHARSPACEASF     (Enum) 55
#define TEXTCOLRASF      (Enum) 56
#define INTSTYLEASF      (Enum) 57
#define FILLCOLRASF      (Enum) 58
#define HATCHINDEXASF    (Enum) 59
#define PATINDEXASF      (Enum) 60
#define EDGETYPEASF      (Enum) 61
#define EDGEWIDTHASF     (Enum) 62
#define EDGECOLRASF      (Enum) 63

/*  Useful local Parameters for enumerated types  */

#define VDC         (Enum) 0

#define MULTIPLE      FALSE
#define SINGLE         TRUE

#define NOSET       (Enum) 0
#define SET         (Enum) 1

#define NOP         (Enum) 0
#define NONE        (Enum) 0
#define ANY         (Enum) 0

/*  Number of character substitutions possible */

#define CHARSUBNUM  (Int) 34

/*  Mathematical quantities  */

#ifndef ZERO
#define ZERO        (Int) 0
#endif

#define LOG2        0.30103
#define PARABIT        0x40

#endif  /*  end of cgmpar.h */
