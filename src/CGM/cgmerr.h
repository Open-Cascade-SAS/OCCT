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

#ifndef CGMERR_H
#define CGMERR_H

/*  Include the files on which this is dependent.  */

#include "cgmtypes.h"

#ifdef CGMERR_C
#define Extern
#else
#define Extern extern
#endif

#ifdef CGMINIT_H
  FILE *cgmerr;
#else
  extern FILE *cgmerr;
#endif

/*  Error function call */
#ifdef PROTO
   Extern int CGMerror ( char*, int, Enum, char*);
#else
   Extern int CGMerror ();
#endif

/* Get Error Count */
#ifdef PROTO
   Extern int CGMcounterr ( void );
#else
  Extern int CGMcounterr();
#endif

/*  Macro to make life a little simpler  */

#define CGMERROR (void) CGMerror

/*  Error categories */

#define  WARNING    (Enum) 1
#define  ERROR      (Enum) 2
#define  FATAL      (Enum) 3

/*  Return Codes - (System Dependent) */

#ifndef  RC_FILERR
#define  RC_FILERR  (Index) 28
#endif
#ifndef  RC_FATAL
#define  RC_FATAL   (Index) 99
#endif
#define  RC_NULL    (Index)  0

/*  Error codes */

#define  ERR_NOTFOUND     -1
#define  ERR_MANYERR     100
#define  ERR_APNDTXT     101
#define  ERR_BIGCELL     102
#define  ERR_BIGEDGES    103
#define  ERR_BIGINTS     104
#define  ERR_BIGPATT     105
#define  ERR_BIGPOINTS   106
#define  ERR_BIGSTR      107
#define  ERR_DIRCELL     108
#define  ERR_DIRPATT     109
#define  ERR_ELEMENT     110
#define  ERR_ENCODING    111
#define  ERR_EOF         112
#define  ERR_FEWPNTS     113
#define  ERR_FONTFILE    114
#define  ERR_INTCHR      115
#define  ERR_INVATT      116
#define  ERR_INVCOLR     117
#define  ERR_INVCONT     118
#define  ERR_INVDELIM    119
#define  ERR_INVELEM     120
#define  ERR_INVENUM     121
#define  ERR_INVESC      122
#define  ERR_INVMFDESC   123
#define  ERR_INVOPCODE   124
#define  ERR_INVOUT      125
#define  ERR_INVPDESC    126
#define  ERR_INVPRIM     127
#define  ERR_MAXCOLR     128
#define  ERR_MENUFONT    129
#define  ERR_MFOPEN      130
#define  ERR_NOBACKSP    131
#define  ERR_NOCLT       132
#define  ERR_NOCODE      133
#define  ERR_NOFONT      134
#define  ERR_NOGDP       135
#define  ERR_NOMEMORY    136
#define  ERR_NOTAPNDTXT  137
#define  ERR_NOTCGM      138
#define  ERR_NOTERM      139
#define  ERR_OPENFILE    140
#define  ERR_PATINDEX    141
#define  ERR_PATTABLE    142
#define  ERR_PICOPEN     143
#define  ERR_REDIRECT    144
#define  ERR_SEEKEOF     145
#define  ERR_VERSION     146
#define  ERR_DIRCOL      147
#define  ERR_HUFFMAN     148
#define  ERR_STATE       149
#define  ERR_NAME2LONG   150
#define  ERR_INVARG      151
#define  ERR_INVASF      152
#define  ERR_RALGKS111   153
#define  ERR_NOENDPIC    154
#define  ERR_NODRIVER    155
#define  ERR_RANDOM      156
#define  ERR_INVOPT      157
#define  ERR_NOFILE      158
#define  ERR_CELL2SMALL  159
#define  ERR_ELMNOTSUP   160
#define  ERR_BADPENDEF   161
#define  ERR_PICLGRPAP   162
#define  ERR_GDIFAIL     163

/*  General font system messages  */

#define  ERR_NOCSETS     200
#define  ERR_BADCSETS    201
#define  ERR_NOFAMS      202
#define  ERR_BADFAMS     203
#define  ERR_NOFAMSYN    204
#define  ERR_BADFAMSYN   205
#define  ERR_NODSN       206
#define  ERR_BADDSN      207
#define  ERR_CSETRANGE   208
#define  ERR_CSETUNKNOWN 209
#define  ERR_FONTRANGE   210
#define  ERR_METHOD      211
#define  ERR_NOMETHODS   212
#define  ERR_METHRANGE   213
#define  ERR_NOREQFONTS  214
#define  ERR_PACKMISSING 215
#define  ERR_NOFMATCH    216
#define  ERR_NOFSETS     217
#define  ERR_BADFSETS    218
#define  ERR_XSFPERM     219
#define  ERR_XSISOCSETS  220
#define  ERR_XSISOFONTS  221
#define  ERR_XSFAMILY    222
#define  ERR_XSALIAS     223
#define  ERR_XSDESIGN    224
#define  ERR_XSSOURCE    225
#define  ERR_XSSFONTS    226
#define  ERR_XSRFONTS    227
#define  ERR_NOFNTSPACE  228
#define  ERR_XFONTLIST   229
#define  ERR_XOPENDISP   230

/*  Bezier font system messages  */

#define  ERR_BEZDIR      250
#define  ERR_BEZDBAD     251
#define  ERR_NOFONTS     252
#define  ERR_BEZOPEN     253
#define  ERR_BEZNODEF    254
#define  ERR_DBUNAV      255
#define  ERR_NPCINSTR    256
#define  ERR_FNTNOTSUP   257
#define  ERR_CURVE2BIG   258
#define  ERR_FNTNOTOPEN  259
#define  ERR_COMPZERO    260
#define  ERR_FILCOR      261

/*  Text and PostScript system messages  */

#define  ERR_NOPROLG     300
#define  ERR_NOALIAS     301
#define  ERR_TXTSYSNFND  302
#define  ERR_2MANYFILES  303
#define  ERR_CHBASEZERO  304
#define  ERR_CHUPZERO    305

/* Polygon and Polygon Set messages */

#define  ERR_NOMEMPSL    400
#define  ERR_BADEDGETAB  401

#undef Extern

#ifdef CGMERR_C

struct errmessage{
   int number;
   Index rc;
   char  *mess;
};

struct errmessage cgmerrs[] =
{

   ERR_2MANYFILES,   RC_FATAL, "Too many metafiles in input file.",
   ERR_APNDTXT,      RC_NULL, "Append Text in wrong state",
   ERR_BADCSETS,     RC_NULL, "Character set file corrupt",
   ERR_BADDSN,       RC_NULL, "Design group file corrupt",
   ERR_BADEDGETAB,   RC_NULL, "Edge table corrupt",
   ERR_BADFAMS,      RC_NULL, "Font file corrupt",
   ERR_BADFAMSYN,    RC_NULL, "Font alias file corrupt",
   ERR_BADFSETS,     RC_NULL, "Fudge character set definition file corrupt",
   ERR_BEZDBAD,      RC_NULL, " ",
   ERR_DBUNAV,       RC_NULL, "Font database unavailable",
   ERR_BEZDIR,       RC_NULL, " ",
   ERR_FILCOR,       RC_NULL, "Bezier file corrupt",
   ERR_BEZNODEF,     RC_NULL, "No defined characters in font",
   ERR_NOFONTS,      RC_NULL, "No fonts in directory",
   ERR_BEZOPEN,      RC_NULL, " ",
   ERR_BIGCELL,      RC_NULL, "Cell Array too big",
   ERR_BIGEDGES,     RC_NULL, "Too many edges",
   ERR_BIGINTS,      RC_NULL, "Too many intersections",
   ERR_BIGPATT,      RC_NULL, "Patterns size too large",
   ERR_BIGPOINTS,    RC_NULL, "Too many points",
   ERR_BIGSTR,       RC_NULL, "String too long",
   ERR_COMPZERO,     RC_NULL, "Both character components are zero",
   ERR_CSETRANGE,    RC_NULL, "Character set index out of range",
   ERR_CSETUNKNOWN,  RC_NULL, "Character set index unknown",
   ERR_CHBASEZERO,   RC_NULL, "Character base vector zero",
   ERR_CHUPZERO,     RC_NULL, "Character up vector zero",
   ERR_CURVE2BIG,    RC_NULL, "Expanded curve too big for system",
   ERR_DIRCELL,      RC_NULL, "Direct Colour Cell arrays not supported",
   ERR_DIRCOL,      RC_FATAL, "Direct Colour not supported",
   ERR_DIRPATT,      RC_NULL, "Direct Colour Patterns not supported",
   ERR_ELEMENT,      RC_NULL, "Unknown Element",
   ERR_ENCODING,    RC_FATAL, "Unknown CGM encoding",
   ERR_EOF,          RC_NULL, "End of File reached",
   ERR_FEWPNTS,      RC_NULL, "Too few points",
   ERR_FNTNOTOPEN,   RC_NULL, "Font file could not be opened",
   ERR_FNTNOTSUP,    RC_NULL, "Font not supported",
   ERR_FONTFILE,     RC_NULL, "Supported fonts file invalid",
   ERR_FONTRANGE,    RC_NULL, "Font index outside CGM fontlist range",
   ERR_GDIFAIL,      RC_NULL, "Windows GDI function failed:",
   ERR_HUFFMAN,      RC_NULL, "Huffman points list not supported",
   ERR_INTCHR,       RC_NULL, "Invalid character in integer",
   ERR_INVARG,       RC_NULL, "Invalid argument",
   ERR_INVASF,       RC_NULL, "Invalid Aspect Source flag",
   ERR_INVATT,       RC_NULL, "Invalid Attribute",
   ERR_INVCOLR,      RC_NULL, "Invalid color type",
   ERR_INVCONT,      RC_NULL, "Invalid Control Element",
   ERR_INVDELIM,     RC_NULL, "Invalid Delimiter",
   ERR_INVELEM,      RC_NULL, "Invalid CGM Element",
   ERR_INVENUM,      RC_NULL, "Invalid Enumerated type",
   ERR_INVESC,       RC_NULL, "Invalid Escape Element",
   ERR_INVMFDESC,    RC_NULL, "Invalid Metafile Descriptor Element",
   ERR_INVOPCODE,    RC_NULL, "Invalid Opcode",
   ERR_INVOPT,       RC_NULL, "Invalid Option",
   ERR_INVOUT,       RC_NULL, "Invalid Output Element",
   ERR_INVPDESC,     RC_NULL, "Invalid Picture Descriptor Element",
   ERR_INVPRIM,      RC_NULL, "Invalid Graphics Primitive",
   ERR_MANYERR,     RC_FATAL, "Too many errors",
   ERR_MAXCOLR,      RC_NULL, "Maximum Colour Index Exceeded,",
   ERR_MENUFONT,     RC_NULL, "Menu font not supported",
   ERR_METHOD,       RC_NULL, "Text method outside allowed range",
   ERR_METHRANGE,    RC_NULL, "Text method outside allowed range",
   ERR_MFOPEN,       RC_NULL, "Metafile already open",
   ERR_NAME2LONG,    RC_NULL, "Illegal length in filename",
   ERR_NOALIAS,      RC_NULL, "No alias found in file alias.psd for font",
   ERR_NOBACKSP,     RC_NULL, "This input file cannot move backwards",
   ERR_NOCLT,        RC_NULL, "File is not a clear text metafile" ,
   ERR_NOCODE,      RC_FATAL, "Unknown Output Code",
   ERR_NOCSETS,      RC_NULL, "Character set file not opened",
   ERR_NODRIVER,    RC_FATAL, "Output driver not supported",
   ERR_NODSN,        RC_NULL, "Design group file not opened",
   ERR_NOENDPIC,     RC_NULL, "Missing ENDPIC",
   ERR_NOFAMS,       RC_NULL, "Font file not opened",
   ERR_NOFAMSYN,     RC_NULL, "Font alias file not opened",
   ERR_NOFMATCH,     RC_NULL, "No trace of requested font found",
   ERR_NOFNTSPACE,   RC_NULL, "No free memory for font system element",
   ERR_NOFONT,       RC_NULL, "Unable to obtain fontlist from X server",
   ERR_NOFILE,      RC_FATAL, "No file name specified",
   ERR_CELL2SMALL,   RC_NULL, "Cell array cells to small for this device",
   ERR_ELMNOTSUP,    RC_NULL, "Element not supported",
   ERR_BADPENDEF,    RC_NULL, "Bad HPGL pen definition",
   ERR_PICLGRPAP,   RC_FATAL, "Metric picture larger than page",
   ERR_NOFSETS,      RC_NULL, "Fudge character set definition file not opened",
   ERR_NOGDP,        RC_NULL, "Unknown GDP",
   ERR_NOMEMORY,    RC_FATAL, "Out of memory",
   ERR_NOMEMPSL,     RC_NULL, "Out of memory for Polygon Section List",
   ERR_NOMETHODS,    RC_NULL, "No text emulation methods supplied",
   ERR_NOPROLG,      RC_NULL, "PostScript prologue file not found",
   ERR_NOREQFONTS,   RC_NULL, "Cannot open requestable fonts list",
   ERR_NOTAPNDTXT,   RC_NULL, "Text not allowed in Append Text State",
   ERR_NOTCGM,      RC_FATAL, "Not a recognisable CGM",
   ERR_NOTERM,       RC_NULL, "Terminal address must be specified",
   ERR_NPCINSTR,     RC_NULL, "Non-printable character in string:",
   ERR_OPENFILE,   RC_FILERR, "Cannot open file",
   ERR_PACKMISSING,  RC_NULL, "Fonts specified but not found for package",
   ERR_PATINDEX,     RC_NULL, "Pattern Index too high",
   ERR_PATTABLE,     RC_NULL, "Pattern Table Full",
   ERR_PICOPEN,      RC_NULL, "Picture already open",
   ERR_RALGKS111,    RC_NULL, "RAL-GKS 1.11 CGM has invalid structure",
   ERR_RANDOM,      RC_FATAL, "Random frame access structure uninitialised",
   ERR_REDIRECT,   RC_FILERR, "Redirected input invalid for this encoding",
   ERR_SEEKEOF,      RC_NULL, "Seeking past end of file",
   ERR_STATE,        RC_NULL, "Element in wrong CGM state",
   ERR_TXTSYSNFND,   RC_NULL, "Did not find a valid text system",
   ERR_VERSION,     RC_FATAL, "Wrong Version number",
   ERR_XFONTLIST,    RC_NULL, "Inconsistency in X font list string",
   ERR_XSALIAS,      RC_NULL, "Too many aliases",
   ERR_XSDESIGN,     RC_NULL, "Too many design codes",
   ERR_XSFAMILY,     RC_NULL, "Too many type families",
   ERR_XSFPERM,      RC_NULL, "Too many fonts per implementation method for",
   ERR_XSISOCSETS,   RC_NULL, "Too many ISO character sets",
   ERR_XSISOFONTS,   RC_NULL, "Too many ISO fonts",
   ERR_XOPENDISP,   RC_FATAL, "Cannot open display",
   ERR_XSRFONTS,     RC_NULL, "Too many requestable fonts",
   ERR_XSSFONTS,     RC_NULL, "Too many special fonts",
   ERR_XSSOURCE,     RC_NULL, "Too many source codes",

/* NOTE: This code must be last */

   ERR_NOTFOUND,     RC_NULL, "Unknown error message"
};
#endif     /* end of CGMERR_C  */

#endif     /* end of cgmerr.h  */
