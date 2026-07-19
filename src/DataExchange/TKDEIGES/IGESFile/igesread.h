/*
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

/* External call to reading routines (in C) */
#include <stdio.h>

/*  structiges : */
struct parlist
{
  struct oneparam *first, *last;
  int              nbparam;
};

struct dirpart
{
  int            typ, poi, pdef, tra, niv, vue, trf, aff, blk, sub, use, her; /* line 1 */
  int            typ2, epa, col, nbl, form;                                   /* line 2 */
  char           res1[10], res2[10], nom[10], num[10];
  struct parlist list;    /* Psect list */
  int            numpart; /* n0 en Dsect */
};

#ifdef __cplusplus
extern "C"
{
#endif

  int igesread(char* nomfic, int lesect[6], int modefnes);

  /*  structiges : */
  int  iges_lirpart(int** tabval, char** res1, char** res2, char** nom, char** num, int* nbparam);
  void iges_stats(int* nbpart, int* nbparam);
  void iges_setglobal();
  void iges_nextpart();
  int  iges_lirparam(int* typarg, char** parval);
  void iges_finfile(int mode);
  struct dirpart* iges_get_curp(void);

  void iges_initfile();
  int  iges_lire(FILE* lefic, int* numsec, char line[100], int modefnes);
  void iges_newparam(int typarg, int longval, char* parval);
  void iges_param(int* Pstat, char* line, char c_separ, char c_fin, int lonlin);
  void iges_Dsect(int* Dstat, int numsec, char* line);
  void iges_Psect(int numsec, char line[80]);

  /* MGE 20/07/98 */
  void IGESFile_Check2(int mode, char* code, int num, char* str);
  void IGESFile_Check3(int mode, char* code);

#ifdef __cplusplus
}
#endif

/*  Definition of parameter types for the basic IGES analyzer */
#define ArgVide 0
#define ArgQuid 1
#define ArgChar 2
#define ArgInt 3  /* Unsigned integer: can evolve to Real or Char */
#define ArgSign 4 /* Signed integer: can evolve to Real */
#define ArgReal 5
#define ArgExp 6  /* Real + letter E: wait for confirmation */
#define ArgRexp 7 /* Real + Exponent: reduces to Real */
#define ArgMexp 8 /* Real + INCOMPLETE Exponent (no decimal point) */
