// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define ASFS 18

typedef short Logical;
typedef long Index;
typedef unsigned short Enum;
typedef short Code;
typedef long Prec;
typedef unsigned long Posint;
typedef float Ptype;
typedef double Mtype;

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

/*  Define real precision structure (character encoding) */

struct real_prec {
   Prec prec, min, defexp, expald;
};

/*  CGM element codes for clear text input and output */
struct colour {
   Index index;
   Posint red, green, blue;
};
typedef struct colour Colour;
struct vdc {
   long intr;
   float real;
};
typedef struct vdc Vdc;
struct point {
  struct vdc x, y;
};
typedef struct point Cpoint;

struct rect {
  struct point a, b;
};

typedef struct { float left, right, top, bot; } Rect;

struct colourentry {
   float red, green, blue;
};
typedef struct colourentry Colourentry;

struct commands {
     Code code;
     const char *string;
  };

/*  Structures for metafile default elements  */

/*   Encoding Specific defaults   */

struct char_defaults{
      Prec int_prec, index_prec, vdcint_prec;
      struct real_prec real, vdc;
      Prec col_prec, colind_prec;
      struct colour min_rgb, max_rgb;
      long ring_angres, ring_int, ring_real;
};

struct text_defaults{
      long min_int, max_int, min_index, max_index;
      float min_real, max_real;
      Prec real_digits;
      struct vdc min_vdc, max_vdc;
      Prec vdc_digits, colind_prec, col_prec;
      struct colour min_rgb, max_rgb;
};

struct bin_defaults{
      Prec int_prec, index_prec, vdcint_prec;
      Enum real_type, vdc_type;
      Prec real_whole, real_fraction, vdc_whole, vdc_fraction;
      Prec col_prec, colind_prec;
      struct colour min_rgb, max_rgb;
};

struct defaults {

/*  Encoding independent defaults        */

      Index max_colind;
      Enum vdc_type, transparency, clip_ind;
      Enum color_mode, scale_mode;
      float scale_factor;
      Enum linewidth_mode, markersize_mode, edgewidth_mode;
      struct colour back, aux;
      struct rect vdc_extent, clip_rect;

/*  Retain encoding defaults for common access  */

      Prec int_bits,     /*  integer prec in bits */
           index_bits,   /*  index prec in bits  */
           vdcint_bits,  /*  vdc integer precision in bits */
           real_bits,    /*  real prec in bits */
           real_places,  /*  binary bits after decimal place */
           vdc_bits,     /*  vdc prec in bits  */
           vdc_places,   /*  binary bits after decimal place */
           col_bits,     /*  colour prec in bits  */
           colind_bits;  /*  colour index prec in bits */
      float min_real,   /*  min real value */
            max_real,   /*  max real value */
            realmin,    /*  smallest real value  */
            min_vdc,    /*  min real vdc value   */
            max_vdc,    /*  max real vdc value   */
            vdcmin;     /*  smallest vdc real value */
      Prec  real_defexp, /* Real default exponent */
            real_expald, /* Real exponenet allowed */
            vdc_defexp,  /* VDC default exponent */
            vdc_expald;   /* VDC exponenet allowed */
      struct colour min_rgb,  /* integer values: index/RGB */
                    max_rgb;  /* integer values: index/RGB */
      Logical colprec_flag,   /* Flag to show if colour precision set */
              colval_flag;    /* Flag to show if colour value extent set */
  };

/*  Graphics attributes  */

struct attributes {
      Index line_ind, mark_ind, text_ind, fill_ind, edge_ind;
      Index line_type, mark_type, text_font, edge_type;
      struct vdc line_width, mark_size, edge_width;
      struct colour line, marker, text, fill, edge;
      Enum text_prec, text_path, text_halign, text_valign, edge_vis;
      float text_hcont, text_vcont;
      Index char_set, altchar_set;
      float char_exp, char_space;
      struct point char_up, char_base;
      struct vdc char_height;
      Enum int_style;
      Index hatch_ind, pat_ind;
      struct point fill_ref;
      struct rect pat_size;
      Enum asf[ASFS];
/*  Flags for VDC dependent defaults */
      Logical linewidth_flag, markersize_flag, edgewidth_flag,
              charheight_flag, cliprect_flag;
  };


/* Structure for attribute change flags  */

struct flags {
  Logical line_index, line_type, line_width, line_color;
  Logical mark_index, mark_type, mark_size, mark_color;
  Logical text_index, text_font, text_prec, char_expan, char_space,
          text_color, char_height, char_orient, char_set,
          altchar_set, text_path, text_align;
  Logical fill_index, int_style, hatch_ind,
          pat_ind, pat_size, fill_color, fill_ref;
  Logical edge_index, edge_type, edge_width, edge_color, edge_vis;
};



/*  Non-Op  */

#define NONOP            (Code) 0

/*  Character CGM hex values  */

#define BEGMF            (Code) 0x3020
#define ENDMF            (Code) 0x3021
#define BEGPIC           (Code) 0x3022
#define BEGPICBODY       (Code) 0x3023
#define ENDPIC           (Code) 0x3024

/*  Metafile Descriptor elements  */

#define MFVERSION        (Code) 0x3120
#define MFDESC           (Code) 0x3121
#define VDCTYPE          (Code) 0x3122
#define INTEGERPREC      (Code) 0x3123
#define REALPREC         (Code) 0x3124
#define INDEXPREC        (Code) 0x3125
#define COLRPREC         (Code) 0x3126
#define COLRINDEXPREC    (Code) 0x3127
#define MAXCOLRINDEX     (Code) 0x3128
#define COLRVALUEEXT     (Code) 0x3129
#define MFELEMLIST       (Code) 0x312A
#define BEGMFDEFAULTS    (Code) 0x312B
#define ENDMFDEFAULTS    (Code) 0x312C
#define FONTLIST         (Code) 0x312D
#define CHARSETLIST      (Code) 0x312E
#define CHARCODING       (Code) 0x312F

/*  Picture descriptor elements  */

#define SCALEMODE        (Code) 0x3220
#define COLRMODE         (Code) 0x3221
#define LINEWIDTHMODE    (Code) 0x3222
#define MARKERSIZEMODE   (Code) 0x3223
#define EDGEWIDTHMODE    (Code) 0x3224
#define VDCEXT           (Code) 0x3225
#define BACKCOLR         (Code) 0x3226

/*  Control elements   */

#define VDCINTEGERPREC   (Code) 0x3320
#define VDCREALPREC      (Code) 0x3321
#define AUXCOLR          (Code) 0x3322
#define TRANSPARENCY     (Code) 0x3323
#define CLIPRECT         (Code) 0x3324
#define CLIP             (Code) 0x3325

/*  Graphics primitives  */

#define LINE             (Code) 0x20
#define DISJTLINE        (Code) 0x21
#define MARKER           (Code) 0x22
#define TEXT             (Code) 0x23
#define RESTRTEXT        (Code) 0x24
#define APNDTEXT         (Code) 0x25
#define POLYGON          (Code) 0x26
#define POLYGONSET       (Code) 0x27
#define CELLARRAY        (Code) 0x28
#define GDP              (Code) 0x29
#define RECT             (Code) 0x2A

#define CIRCLE           (Code) 0x3420
#define ARC3PT           (Code) 0x3421
#define ARC3PTCLOSE      (Code) 0x3422
#define ARCCTR           (Code) 0x3423
#define ARCCTRCLOSE      (Code) 0x3424
#define ELLIPSE          (Code) 0x3425
#define ELLIPARC         (Code) 0x3426
#define ELLIPARCCLOSE    (Code) 0x3427

/*  Attribute elements  */

#define LINEINDEX        (Code) 0x3520
#define LINETYPE         (Code) 0x3521
#define LINEWIDTH        (Code) 0x3522
#define LINECOLR         (Code) 0x3523
#define MARKERINDEX      (Code) 0x3524
#define MARKERTYPE       (Code) 0x3525
#define MARKERSIZE       (Code) 0x3526
#define MARKERCOLR       (Code) 0x3527

#define TEXTINDEX        (Code) 0x3530
#define TEXTFONTINDEX    (Code) 0x3531
#define TEXTPREC         (Code) 0x3532
#define CHAREXPAN        (Code) 0x3533
#define CHARSPACE        (Code) 0x3534
#define TEXTCOLR         (Code) 0x3535
#define CHARHEIGHT       (Code) 0x3536
#define CHARORI          (Code) 0x3537
#define TEXTPATH         (Code) 0x3538
#define TEXTALIGN        (Code) 0x3539
#define CHARSETINDEX     (Code) 0x353A
#define ALTCHARSETINDEX  (Code) 0x353B

#define FILLINDEX        (Code) 0x3620
#define INTSTYLE         (Code) 0x3621
#define FILLCOLR         (Code) 0x3622
#define HATCHINDEX       (Code) 0x3623
#define PATINDEX         (Code) 0x3624
#define EDGEINDEX        (Code) 0x3625
#define EDGETYPE         (Code) 0x3626
#define EDGEWIDTH        (Code) 0x3627
#define EDGECOLR         (Code) 0x3628
#define EDGEVIS          (Code) 0x3629
#define FILLREFPT        (Code) 0x362A
#define PATTABLE         (Code) 0x362B
#define PATSIZE          (Code) 0x362C

#define COLRTABLE        (Code) 0x3630
#define ASF              (Code) 0x3631

/*  Escape Elements  */

#define ESCAPE           (Code) 0x3720
#define MESSAGE          (Code) 0x3721
#define APPLDATA         (Code) 0x3722

#define DOMAINRING       (Code) 0x3730

/*  Version 2 Elements - Addendum 1  */

#if CGMVERSION > 1

#define BEGSEG           (Code) 0x3025
#define ENDSEG           (Code) 0x3026
#define BEGFIG           (Code) 0x3027
#define ENDFIG           (Code) 0x3028

#define NAMEPREC         (Code) 0x3130
#define MAXVDCEXT        (Code) 0x3131
#define SEGPRIEXT        (Code) 0x3132

#define DEVVP            (Code) 0x3227
#define DEVVPMODE        (Code) 0x3228
#define DEVVPMAP         (Code) 0x3229
#define LINEREP          (Code) 0x322A
#define MARKERREP        (Code) 0x322B
#define TEXTREP          (Code) 0x322C
#define FILLREP          (Code) 0x322D
#define EDGEREP          (Code) 0x322E

#define LINECLIPMODE     (Code) 0x3326

#define MARKERCLIPMODE   (Code) 0x3327

#define EDGECLIPMODE     (Code) 0x3328

#define NEWREGION        (Code) 0x3329
#define SAVEPRIMCONT     (Code) 0x332A

#define RESPRIMCONT      (Code) 0x332B

#define ARCCTRREV        (Code) 0x3428
#define CONNEDGE         (Code) 0x3429

#define PICKID           (Code) 0x3632

#define COPYSEG          (Code) 0x3820
#define INHFILTER        (Code) 0x3821
#define CLIPINH          (Code) 0x3822
#define SEGTRANS         (Code) 0x3823
#define SEGHIGHLIGHT     (Code) 0x3824

#define SEGDISPPRI       (Code) 0x3825
#define SEGPICKPRI       (Code) 0x3826

/*  GKS Annex elements */

#define BEGGKSSESSIONMF  (Code) 0x3029
#define BEGGKSSESSION    (Code) 0x302A
#define ENDGKSSESSION    (Code) 0x302B

#define WSVP             (Code) 0x332C
#define CLEAR            (Code) 0x332D
#define UPD              (Code) 0x332E
#define DEFERST          (Code) 0x332F

#define DELSEG           (Code) 0x3827
#define RENAMESEG        (Code) 0x3828
#define REDRAWALLSEG     (Code) 0x3829
#define SEGVIS           (Code) 0x382A
#define SEGDET           (Code) 0x382B

/*  Version 3 Elements - Addendum 3  */

#if CGMVERSION > 2

#define BEGPROTREGION       (Code) 0x302C
#define ENDPROTREGION       (Code) 0x302D
#define BEGCOMPOLINE        (Code) 0x302E
#define ENDCOMPOLINE        (Code) 0x302F
#define BEGCOMPOTEXTPATH    (Code) 0x3030
#define ENDCOMPOTEXTPATH    (Code) 0x3031
#define BEGTILEARRAY        (Code) 0x3032
#define ENDTILEARRAY        (Code) 0x3033

#define COLRMODEL           (Code) 0x3133
#define COLRCALIB           (Code) 0x3134
#define FONTPROP            (Code) 0x3135
#define GLYPHMAP            (Code) 0x3136
#define SYMBOLLIBLIST       (Code) 0x3137

#define INTSTYLEMODE        (Code) 0x322F
#define LINEEDGETYPEDEF     (Code) 0x3230
#define HATCHSTYLEDEF       (Code) 0x3231
#define GEOPATDEF           (Code) 0x3232

#define PROTREGION          (Code) 0x3330
#define GENTEXTPATHMODE     (Code) 0x3331
#define MITRELLIMIT         (Code) 0x3332
#define TRANSPCELLCOLR      (Code) 0x3333

#define HYPERBARC           (Code) 0x342A
#define PARABARC            (Code) 0x342B
#define NUB                 (Code) 0x342C
#define NURB                (Code) 0x342D
#define POLYBEZIER          (Code) 0x342E
#define SYMBOL              (Code) 0x342F
#define INCRSYMBOL          (Code) 0x342F
#define BITONALTILE         (Code) 0x3430
#define TILE                (Code) 0x3431

#define LINECAP             (Code) 0x3528
#define LINEJOIN            (Code) 0x3529
#define LINETYPECONT        (Code) 0x352A
#define LINETYPEINITOFFSET  (Code) 0x352B
#define TEXTSCORETYPE       (Code) 0x352C
#define RESTTEXTTYPE        (Code) 0x352D
#define INTERPINT           (Code) 0x352E
#define EDGECAP             (Code) 0x352F
#define EDGEJOIN            (Code) 0x362D
#define EDGETYPECONT        (Code) 0x362E
#define EDGETYPEINITOFFSET  (Code) 0x362F
#define SYMBOLLIBINDEX      (Code) 0x3633
#define SYMBOLCOLR          (Code) 0x3634
#define SYMBOLSIZE          (Code) 0x3635
#define SYMBOLORI           (Code) 0x3636

#endif /* Version 3 */

#endif  /* Version 2 */




/*  CGM Statelists */

#define MF_CLOSED    (Enum) 0
#define MF_DESC      (Enum) 1
#define MF_ELEMLIST  (Enum) 2
#define MF_DEFAULTS  (Enum) 3
#define PIC_DESC     (Enum) 4
#define PIC_OPEN     (Enum) 5
#define PIC_CLOSED   (Enum) 6
#define TEXT_OPEN    (Enum) 7

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

/*  Mathematical quantities  */

#ifndef ZERO
#define ZERO        (Int) 0
#endif

#define LOG2        0.30103
#define PARABIT        0x40


/*   All default values set within the control program  */
/*   with Character encoding defaults    */


/*  Initialise flags */

#ifndef AIX
#define FALSE (Enum) 0
#define TRUE  (Enum) 1
#endif
/*
  Enum cgmstate = MF_CLOSED;    Initial CGM State  

  Logical cgmterm = FALSE;      Flag set for input from stdin  

  Logical cgmEOF = FALSE;       End of File flag is OFF 

  Logical cgmfinished = FALSE ;   Interpreter finished flag 

  Default string start and terminator 

  Code st_start = 0x1b58, st_term = 0x1b5c;


  Arrays for coding and decoding Clear Text elements */

const struct commands cgmelement[] = {

/*  Graphics primitives  */

      {0x20, "LINE"},       {0x20, "INCRLINE"},
      {0x21, "DISJTLINE"},  {0x21, "INCRDISJTLINE"},
      {0x22, "MARKER"},     {0x22, "INCRMARKER"},
      {0x23, "TEXT"},
      {0x24, "RESTRTEXT"},
      {0x25, "APNDTEXT"},
      {0x26, "POLYGON"},    {0x26, "INCRPOLYGON"},
      {0x27, "POLYGONSET"}, {0x27, "INCRPOLYGONSET"},
      {0x28, "CELLARRAY"},
      {0x29, "GDP"},
      {0x2A, "RECT"},

      {0x3420, "CIRCLE"},
      {0x3421, "ARC3PT"},
      {0x3422, "ARC3PTCLOSE"},
      {0x3423, "ARCCTR"},
      {0x3424, "ARCCTRCLOSE"},
      {0x3425, "ELLIPSE"},
      {0x3426, "ELLIPARC"},
      {0x3427, "ELLIPARCCLOSE"},

/*  Attribute elements  */

      {0x3520, "LINEINDEX"},
      {0x3521, "LINETYPE"},
      {0x3522, "LINEWIDTH"},
      {0x3523, "LINECOLR"},
      {0x3524, "MARKERINDEX"},
      {0x3525, "MARKERTYPE"},
      {0x3526, "MARKERSIZE"},
      {0x3527, "MARKERCOLR"},

      {0x3530, "TEXTINDEX"},
      {0x3531, "TEXTFONTINDEX"},
      {0x3532, "TEXTPREC"},
      {0x3533, "CHAREXPAN"},
      {0x3534, "CHARSPACE"},
      {0x3535, "TEXTCOLR"},
      {0x3536, "CHARHEIGHT"},
      {0x3537, "CHARORI"},
      {0x3538, "TEXTPATH"},
      {0x3539, "TEXTALIGN"},
      {0x353A, "CHARSETINDEX"},
      {0x353B, "ALTCHARSETINDEX"},

      {0x3620, "FILLINDEX"},
      {0x3621, "INTSTYLE"},
      {0x3622, "FILLCOLR"},
      {0x3623, "HATCHINDEX"},
      {0x3624, "PATINDEX"},
      {0x3625, "EDGEINDEX"},
      {0x3626, "EDGETYPE"},
      {0x3627, "EDGEWIDTH"},
      {0x3628, "EDGECOLR"},
      {0x3629, "EDGEVIS"},

      {0x362A, "FILLREFPT"},
      {0x362B, "PATTABLE"},
      {0x362C, "PATSIZE"},
      {0x3630, "COLRTABLE"},
      {0x3631, "ASF"},

/*  Control elements   */

      {0x3320, "VDCINTEGERPREC"},
      {0x3321, "VDCREALPREC"},
      {0x3322, "AUXCOLR"},
      {0x3323, "TRANSPARENCY"},
      {0x3324, "CLIPRECT"},
      {0x3325, "CLIP"},

/*   Metafile Control Elements  */

      {0x3020, "BEGMF"},
      {0x3021, "ENDMF"},
      {0x3022, "BEGPIC"},
      {0x3023, "BEGPICBODY"},
      {0x3024, "ENDPIC"},

/*  Metafile Descriptor elements  */

      {0x3120, "MFVERSION"},
      {0x3121, "MFDESC"},
      {0x3122, "VDCTYPE"},
      {0x3123, "INTEGERPREC"},
      {0x3124, "REALPREC"},
      {0x3125, "INDEXPREC"},
      {0x3126, "COLRPREC"},
      {0x3127, "COLRINDEXPREC"},
      {0x3128, "MAXCOLRINDEX"},
      {0x3129, "COLRVALUEEXT"},
      {0x312A, "MFELEMLIST"},
      {0x312B, "BEGMFDEFAULTS"},
      {0x312C, "ENDMFDEFAULTS"},
      {0x312D, "FONTLIST"},
      {0x312E, "CHARSETLIST"},
      {0x312F, "CHARCODING"},

/*  Picture descriptor elements  */

      {0x3220, "SCALEMODE"},
      {0x3221, "COLRMODE"},
      {0x3222, "LINEWIDTHMODE"},
      {0x3223, "MARKERSIZEMODE"},
      {0x3224, "EDGEWIDTHMODE"},
      {0x3225, "VDCEXT"},
      {0x3226, "BACKCOLR"},

/*  Escape Elements  */

      {0x3720, "ESCAPE"},
      {0x3721, "MESSAGE"},
      {0x3722, "APPLDATA"},

/*  Version 2 element names  */

#if CGMVERSION > 1
      {0x3025, "BEGSEG"},
      {0x3026, "ENDSEG"},
      {0x3027, "BEGFIG"},
      {0x3028, "ENDFIG"},

      {0x3029, "BEGGKSSESSIONMF"},
      {0x302A, "BEGGKSSESSION"},
      {0x302B, "ENDGKSSESSION"},

      {0x3130, "NAMEPREC"},
      {0x3131, "MAXVDCEXT"},
      {0x3132, "SEGPRIEXT"},

      {0x3227, "DEVVP"},
      {0x3228, "DEVVPMODE"},
      {0x3229, "DEVVPMAP"},
      {0x322A, "LINEREP"},
      {0x322B, "MARKERREP"},
      {0x322C, "TEXTREP"},
      {0x322D, "FILLREP"},
      {0x322E, "EDGEREP"},

      {0x3326, "LINECLIPMODE"},
      {0x3327, "MARKERCLIPMODE"},
      {0x3328, "EDGECLIPMODE"},
      {0x3329, "NEWREGION"},
      {0x332A, "SAVEPRIMCONT"},
      {0x332B, "RESPRIMCONT"},

      {0x332C, "WSVP"},
      {0x332D, "CLEAR"},
      {0x332E, "UPD"},
      {0x332F, "DEFERST"},

      {0x3428, "ARCCTRREV"},
      {0x3429, "CONNEDGE"},

      {0x3632, "PICKID"},

      {0x3820, "COPYSEG"},
      {0x3821, "INHFILTER"},
      {0x3822, "CLIPINH"},
      {0x3823, "SEGTRANS"},
      {0x3824, "SEGHIGHLIGHT"},
      {0x3825, "SEGDISPPRI"},
      {0x3826, "SEGPICKPRI"},

      {0x3827, "DELSEG"},
      {0x3828, "RENAMESEG"},
      {0x3829, "REDRAWALLSEG"},
      {0x382A, "SEGVIS"},
      {0x382B, "SEGDET"},

#if CGMVERSION > 2

      {0x302C, "BEGPROTREGION"},
      {0x302D, "ENDPROTREGION"},
      {0x302E, "BEGCOMPOLINE"},
      {0x302F, "ENDCOMPOLINE"},
      {0x3030, "BEGCOMPOTEXTPATH"},
      {0x3031, "ENDCOMPOTEXTPATH"},
      {0x3032, "BEGTILEARRAY"},
      {0x3033, "ENDTILEARRAY"},

      {0x3133, "COLRMODEL"},
      {0x3134, "COLRCALIB"},
      {0x3135, "FONTPROP"},
      {0x3136, "GLYPHMAP"},
      {0x3137, "SYMBOLLIBLIST"},

      {0x322F, "INTSTYLEMODE"},
      {0x3230, "LINEEDGETYPEDEF"},
      {0x3231, "HATCHSTYLEDEF"},
      {0x3232, "GEOPATDEF"},

      {0x3330, "PROTREGION"},
      {0x3331, "GENTEXTPATHMODE"},
      {0x3332, "MITRELLIMIT"},
      {0x3333, "TRANSPCELLCOLR"},

      {0x342A, "HYPERBARC"},
      {0x342B, "PARABARC"},
      {0x342C, "NUB"},
      {0x342D, "NURB"},
      {0x342E, "POLYBEZIER"},
      {0x342F, "SYMBOL"}, {0x342F, "INCRSYMBOL"},
      {0x3430, "BITONALTILE"},
      {0x3431, "TILE"},

      {0x3528, "LINECAP"},
      {0x3529, "LINEJOIN"},
      {0x352A, "LINETYPECONT"},
      {0x352B, "LINETYPEINITOFFSET"},
      {0x352C, "TEXTSCORETYPE"},
      {0x352D, "RESTTEXTTYPE"},
      {0x352E, "INTERPINT"},
      {0x352F, "EDGECAP"},
      {0x362D, "EDGEJOIN"},
      {0x362E, "EDGETYPECONT"},
      {0x362F, "EDGETYPEINITOFFSET"},
      {0x3633, "SYMBOLLIBINDEX"},
      {0x3634, "SYMBOLCOLR"},
      {0x3635, "SYMBOLSIZE"},
      {0x3636, "SYMBOLORI"},

#endif
#endif

/* MFelemlist special names */

      {0x10, "DRAWINGSET"},
      {0x11, "DRAWINGPLUS"},
#if CGMVERSION > 1
      {0x12, "VERSION2"},
      {0x13, "EXTDPRIM"},
      {0x14, "VERSION2GKSM"},
#if CGMVERSION > 2
      {0x15, "VERSION3"},
#endif
#endif
      {EOF, "EOF"}
   };

extern "C" {
  /*  Metafile Default settings */
  extern struct char_defaults mfchar, mfichar, curichar, curchar, oldchar;
  extern struct text_defaults mftext, mfitext, curitext, curtext;
  extern struct bin_defaults mfbin, mfibin, curibin, curbin;
  extern struct defaults mf, cur, old;
  extern struct attributes mfatt, curatt, oldatt;

  /*  Flags for changes to attributes - initially set to false  */
  extern struct flags att;

/*
  FILE* OPEN_FILE(char*,int);
  void CLOSE_FILE();
*/
  void CGMOtext(FILE*,Code, Long*,float*,char*);
  void CGMObin (FILE*,Code, Long*,float*,char*);
  void CGMOchar (FILE*,Code, Long*,float*,char*);
  FILE* getfd();
}
FILE* OPEN_FILE(char*,int);
void CLOSE_FILE();
