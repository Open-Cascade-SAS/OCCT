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

#ifndef CGMINIT_H
#define CGMINIT_H

/*  Include all files on which this is dependent.  */

#include "cgmmach.h"
#include "cgmtypes.h"
#include "cgmpar.h"
#include "cgmelem.h"
#include "cgmstruc.h"
#include "cgmerr.h"

/*   All default values set within the control program  */
/*   with Character encoding defaults    */


/*  Initialise flags */

  Enum cgmstate = MF_CLOSED;  /*  Initial CGM State  */

  Logical cgmterm = FALSE;    /*  Flag set for input from stdin  */

  Logical cgmEOF = FALSE;     /*  End of File flag is OFF */

  Logical cgmfinished = FALSE ;  /* Interpreter finished flag */

/*  Default string start and terminator  */

  Code st_start = 0x1b58, st_term = 0x1b5c;

/*  Driver names */

struct cgmdrivers {
   char *name;   /* Name or abreviation */
   int chrs;     /* Minimum number of characters to match */
   Enum type;    /* Output driver */
};

/*  Arrays for decoding Clear Text elements */

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


/*  Metafile Default settings */

  const struct char_defaults chardef = {
      10,               /*  integer prec */
      10,               /*  index prec*/
      20,               /*  vdc integer prec */
      {10, -10, -10, 1},  /*  real precision */
      {10, -10, -10, 1},  /*  vdc real precision  */
      6,                /*  colour prec */
      10,               /*  colour index prec */
      {0, 0, 0, 0},       /*  minimum colour extent */
      {1, 63, 63, 63},    /*  maximum colour extent */
      0, 1, 1           /*  ring parameters for DCC (Huffman codes)  */
  };
  struct char_defaults mfchar, mfichar, curichar, curchar, oldchar;

  const struct text_defaults textdef = {
      -32767,        /* min integer  */
       32767,        /* max integer  */
           0,        /* min index  */
         127,        /* max index  */
    -32767.0F,        /* min real */
     32767.0F,        /* max real */
           4,        /* digits */
      {-32767,  0.0F},  /* min VDC (int,  real) */
      { 32767,  1.0F},  /* max VDC (int,  real) */
           4,        /* digits */
         127,        /* colour index*/
         255,        /* colour precision*/
   {0, 0, 0, 0},       /* minimum colour value extent */
   {1, 255, 255, 255}  /* maximum colour value extent */
  };
  struct text_defaults mftext, mfitext, curitext, curtext;

  const struct bin_defaults bindef = {
      16,         /* integer prec  */
      16,         /* index prec  */
      16,         /* vdc integer prec */
      1,          /* real type */
      1,          /* vdc type */
      16,         /* real whole */
      16,         /* real fraction */
      16,         /* vdc real whole  */
      16,         /* vdc real fraction */
       8,         /* colour prec */
       8,         /* colour index prec */
       {0, 0, 0, 0},       /* minimum colour value extent */
       {1, 255, 255, 255}  /* maximum colour value extent */
  };
  struct bin_defaults mfbin, mfibin, curibin, curbin;

  const struct defaults commondef = {
      63,         /* max colour index   */
      REAL,    /* VDC type */
      ON,         /* clip indicator */
      ON,         /* transparency */
      INDEXED,    /* colour mode */
      ABSTRACT,   /* scale mode */
      1.0F,        /* scale factor  */
      SCALED,     /* line width mode */
      SCALED,     /* marker size mode */
      SCALED,     /* edge width mode  */
      {0,  0,  0,  0}, /*  background colour */
      {0,  0,  0,  0}, /* auxilary colour (device dependent) */
      {{{0, 0.0F},     {0, 0.0F}},   /*  vdc extent - bottom left */
  {{32767, 1.0F}, {32767, 1.0F}}},                 /* top right */
      {{{0, 0.0F},     {0, 0.0F}},   /*  clip rectangle  - bottom left */
  {{32767, 1.0F}, {32767, 1.0F}}},                      /* top right */

/* set precisions to character defaults  */
       10,      /* int prec */
       10,      /* index prec */
       10,      /* vdcint prec */
       10,      /* real prec */
      -10,      /* real bits */
       10,      /* vdc prec  */
      -10,      /* vdc bits */
        6,      /* col_prec */
       10,      /* colind_prec */
 -32767.0F,      /* min_real */
  32767.0F,      /* max_real */
  0.00015F,      /* realmin  */
      0.0F,      /* min_vdc */
      1.0F,      /* max_vdc */
  0.00015F,      /* vdcmin  */
    -10, 1,     /* Real default exp and exponent allowed */
    -10, 1,     /* VDC default exp and exponent allowed */
    {0, 0, 0, 0},     /* min_rgb */
    {1, 255,255,255}, /* max_rgb */
    FALSE, FALSE    /* Colour prec & value extent not yet set */
  };

/*  also use structure for picture defaults and current settings  */

  struct defaults mf, cur, old;

/*  Hold current attributes seperately   */

  const struct attributes defatt = {
      1,         /* line index */
      1,         /* marker index */
      1,         /* text index*/
      1,         /* fill Index */
      1,         /* edge index */
      1,         /*  line type */
      3,         /* marker type */
      1,         /* text font */
      1,         /* edge type  */
      {33, 1.0F},   /*  line width */
      {327, 1.0F},  /* marker size */
      {33, 1.0F},   /* edge width */
      {1, 1,1,1},  /* line colour */
      {1, 1,1,1},  /* marker colour */
      {1, 1,1,1},  /* text colour */
      {1, 1,1,1},  /* fill colour */
      {1, 1,1,1},  /* edge colour */
      0,         /* text prec */
      0,         /* text path */
      0,         /* horiz alignment */
      0,         /* vertical alignment */
    OFF,         /* edge vis */
    1.0F,         /* continuous horiz alignment */
    1.0F,         /* continuous vertical alignment */
     1,          /* character set */
     1,          /* alt character set  */
   1.0F,          /* character expansion */
   0.0F,          /* character space */
     {{0, 0.0F}, {1, 1.0F}},  /* character up vector*/
     {{1, 1.0F}, {0, 0.0F}},  /* character base vector */
    {327, 0.01F},        /* character height  */
      0,         /* interior style */
      1,         /* hatch index */
      1,         /* pattern index */
      {{0, 0.0F}, {0, 0.0F}}, /* fill Reference point */
      {{{0, 0.0F}, {1, 1.0F}}, /* pattern size - height */
      {{1, 1.0F}, {0, 0.0F}}},                /* width */
      {0, 0, 0,        /*  Aspect source flags - line (all individual) */
      0, 0, 0,                               /* marker */
      0, 0, 0, 0, 0,                         /* text */
      0, 0, 0, 0,                            /* fill */
      0, 0, 0},                               /* edge */
   FALSE, FALSE, FALSE, FALSE, FALSE    /* VDC dependent flag settings */
  };
  struct attributes mfatt, curatt, oldatt;

/*  Flags for changes to attributes - initially set to false  */

  struct flags att;



/*   Set default values for common flags  */

  Int cgmerrcount = ERROR_MAX;   /*  USE error count limit by default */

  Logical cgmralgks = DEFRALGKSGDP;    /*  Use RAL-GKS GDPs */

  Logical cgmralbin = FALSE;           /*  Old version of RALCGM Binary CGM */

  Logical cgmverbose = DEFVERBOSE;     /*  Verbose output */

  Logical cgmquiet = DEFQUIET;         /* Quiet output */

/*  Clear text listing */

  Logical cgmlist = FALSE;


/*  Structures for Font list and Character set */

  struct cgmfont cgmfonts;
  struct cgmcset cgmcsets;

/*  Null pointer for function calls - to fool LINT  */

  long *null = NULL;

/*  Default colour table  */

  const RGBcolour GKScol[256] = {

/*   GKS  default colours */

    {0,   0,   0},   /* Background - Black */
  {255, 255, 255},   /* Foreground - White */
  {255,   0,   0},   /* Red */
    {0, 255,   0},   /* Green */
    {0,   0, 255},   /* Blue */
  {255, 255,   0},   /* Yellow */
    {0, 255, 255},   /* Cyan */
  {255,   0, 255},   /* Magenta */

 /*    8 Grey scales ( White to Black )  */

    {255, 255, 255},  {177, 177, 177},  {158, 158, 158},  {128, 128, 128},
    {100, 100, 100},  { 78,  78,  78},  { 64,  64,  64},  {  0,   0,   0},

 /*    GKS colour maps from Versatec  16 - 255 */

   {98, 192, 255},  {139, 122, 255}, {   92,  70, 255},  {  51,   0, 243},
   {28,   0, 133},  { 20,   0,  93}, {   14,   0,  65},  {  11,   0,  56},
    {9,   0,  46},  {128, 138, 255}, {   85,  85, 255},  {   0,   0, 255},
    {0,   0, 168},  {  0,  13, 116}, {    0,   8,  76},  {   0,   8,  68},

  {122, 139, 255},  { 80,  93, 255}, {    0,  59, 252},  {   0,  39, 168},
    {0,  28, 122},  {  0,  18,  80}, {    0,  16,  70},  { 122, 149, 255},
   {70, 108, 255},  {  0,  83, 230}, {    0,  61, 175},  {   0,  44, 128},
    {0,  29,  85},  {  0,  26,  74}, {  116, 154, 255},  {  59, 118, 255},

    {0,  97, 219},  {  0,  77, 182}, {    0,  56, 133},  {   0,  37,  89},
    {0,  33,  79},  {110, 159, 255}, {   43, 131, 255},  {   0, 112, 225},
    {0,  92, 186},  {  0,  68, 138}, {    0,  46,  93},  {   0,  40,  80},
  {177, 213, 255},  {104, 173, 255}, {    0, 151, 250},  {   0, 136, 225},

    {0, 120, 198},  {  0,  89, 147}, {    0,  59,  97},  {   0,  52,  86},
    {0,  43,  70},  {  0, 186, 186}, {    0, 160, 147},  {   0, 142, 129},
    {0, 121, 106},  {  0,  89,  78}, {    0,  62,  55},  {   0,  53,  46},
    {0, 192, 160},  {  0, 160, 123}, {    0, 138, 105},  {   0, 119,  91},

    {0,  89,  68},  {  0,  59,  44}, {    0,  53,  40},  {   0, 243, 169},
    {0, 192, 134},  {  0, 160, 113}, {    0, 138,  97},  {   0, 118,  83},
    {0,  89,  63},  {  0,  59,  41}, {    0,  51,  36},  {   0,  43,  30},
    {0, 216, 115},  {  0, 175, 113}, {    0, 151, 102},  {   0, 138,  91},

    {0, 104,  68},  {  0,  70,  46}, {    0,  61,  40},  {   0, 230,  74},
    {0, 189, 110},  {  0, 171, 100}, {    0, 156,  92},  {   0, 116,  69},
    {0,  80,  47},  {  0,  56,  33}, {  105, 238,   0},  {   0, 216,  46},
    {0, 202,  65},  {  0, 189,  71}, {    0, 142,  53},  {   0,  97,  36},

    {0,  83,  31},  {161, 240,   0}, {   99, 225,   0},  { 120, 219,   0},
   {92, 208,   0},  { 69, 156,   0}, {   46, 104,   0},  {  41,  93,   0},
  {197, 243,   0},  {172, 232,   0}, {  168, 227,   0},  { 160, 219,   0},
  {122, 168,   0},  { 80, 110,   0}, {   70,  97,   0},  { 255, 251, 175},

  {255, 250, 110},  {255, 249,  70}, {  254, 248,   0},  { 253, 245,   0},
  {189, 183,   0},  {128, 124,   0}, {  113, 110,   0},  {  89,  86,   0},
  {255, 209, 116},  {255, 194,  97}, {  255, 191,  85},  { 255, 177,  65},
  {195, 136,   0},  {133,  92,   0}, {  116,  81,   0},  { 255, 190, 119},

  {255, 163, 104},  {255, 168,  89}, {  255, 160,  80},  { 195, 113,   0},
  {133,  77,   0},  {116,  67,   0}, {  255, 173, 122},  { 255, 154, 107},
  {255, 142,  89},  {255, 112,   0}, {  195,  86,   0},  { 128,  56,   0},
  {113,  50,   0},  {255, 162, 122}, {  255, 125, 104},  { 255, 100,  65},

  {247,  80,   0},  {186,  60,   0}, {  122,  39,   0},  { 110,  35,   0},
  {255, 147, 124},  {255,  98,  89}, {  255,  56,  43},  { 238,  44,   0},
  {182,  34,   0},  {122,  22,   0}, {  104,  19,   0},  { 255, 168, 170},
  {255, 122, 128},  {255,  70,  80}, {  250,   0,  38},  { 225,   0,  35},

  {168,   0,  26},  {110,   0,  17}, {  100,   0,  15},  {  80,   0,  12},
  {255, 122, 147},  {255,  70, 108}, {  247,   0,  83},  { 225,   0,  72},
  {168,   0,  54},  {110,   0,  35}, {   97,   0,  31},  { 255, 166, 197},
  {255, 110, 170},  {255,  43, 150}, {  232,   0, 134},  { 211,   0, 122},

  {160,   0,  92},  {104,   0,  60}, {   93,   0,  54},  {  76,   0,  44},
  {255,  80, 204},  {227,   0, 189}, {  199,   0, 165},  { 168,   0, 142},
  {122,   0, 104},  { 85,   0,  72}, {   74,   0,  63},  { 255,  70, 225},
  {205,   0, 213},  {175,   0, 175}, {  141,   0, 147},  { 106,   0, 110},

   {71,   0,  74},  { 62,   0,  65}, {  245,  43, 255},  { 193,   0, 227},
  {153,   0, 195},  {107,   0, 147}, {   80,   0, 110},  {  55,   0,  76},
   {47,   0,  65},  {216,  89, 255}, {  150,  43, 255},  { 118,   0, 216},
   {80,   0, 147},  { 59,   0, 107}, {   38,   0,  70},  {  35,   0,  64},

  {187, 110, 255},  {122,  59, 255}, {   86,   0, 230},  {  55,   0, 147},
   {38,   0, 100},  { 25,   0,  68}, {   22,   0,  60},  { 177, 177, 177},
  {158, 158, 158},  {128, 128, 128}, {  100, 100, 100},  {  78,  78,  78},
    {0,   0,   0},  { 61,  50,  59}, {   52,  45,  62},  {  36,  61,  59},

  {127, 118, 109},  { 91,  70,  72}, {   71,  57,  64},  {  79,  62,  64},
   {28,  33,  32},  { 34,  31,  36}, {   31,   0,  22},  { 252, 182, 142},
  {223, 145, 135},  {191, 133, 119}, {  155, 100,  94},  { 137,  94,  90},
   {64,  51,  53},  { 48,  41,  42}, {  255, 148, 116},  { 151,  96,  91}
};

#endif  /*  end of cgminit.h */
