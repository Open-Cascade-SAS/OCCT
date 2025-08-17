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

#include <stdlib.h>
#include <string.h>

#include "igesread.h"

/*   Temporary IGES structures (recording entities and parameters)
     Include: declarations, and management of the current entity  */

static int nbparts;
static int nbparams;

/*                IGES parameter list (header or current part)        */
static struct parlist *curlist;
static struct parlist *starts;   /*  Start Section of IGES file  */
static struct parlist *header;   /*  Header of IGES file  */


/*                Declaration of a portion of IGES Directory              */
static struct dirpart *curp;

struct dirpart *iges_get_curp (void)
{
  return curp;
}

/*                   Declaration of an IGES parameter (Psect)              */
static struct oneparam {
  struct oneparam *next;
  int typarg;
  char *parval;
} *curparam;

#define Maxparts 1000
static struct dirpage {
  int used;
  struct dirpage *next;
  struct dirpart  parts[Maxparts];
} *firstpage = NULL;

#define Maxpar 20000
static struct parpage {    /* a page of parameters; see AddParam */
  struct parpage* next;
  int             used;
  struct oneparam params[Maxpar+1];
} *oneparpage;


static int curnumpart = 0;
static struct dirpage *curpage;


/*           UTILITY ROUTINES for text processing (char*)          */

/*     Current text management: it's a dynamically allocated text
       iges_newchar allocates one (discards the previous allocated if not read)
       rec_gettext reads the current text, which will not be deallocated afterwards
       rec_settext forces another by discarding the previous (same as rec_newtext)
       while rec_newtext allocates a text, without link to the current one
*/

#define Maxcar 10000

  static struct carpage {
    struct carpage* next;        /*  chaining of character pages  */
    int             used;        /*  space already taken  */
    char  cars[Maxcar+1];        /*  character page  */
  } *onecarpage;

  static char* restext = NULL ;  /* current text  (dynamic allocation) */
/*  static int   resalloc = 0 ; */    /*   allocated (memory to free) or not   */

/*    Utility: Character reservation
      Replaces series of mini-malloc with page management   */

static char* iges_newchar (int lentext)
{
  int lnt = onecarpage->used;
  if (lnt > Maxcar-lentext-1) {  /* allouer nouvelle page */
    struct carpage *newpage;
    unsigned int sizepage = sizeof(struct carpage);
    if (lentext >= Maxcar) sizepage += (lentext+1 - Maxcar);
    newpage = (struct carpage*) malloc (sizepage);
    newpage->next = onecarpage;
    onecarpage = newpage;
    lnt = onecarpage->used = 0;
  }
  restext  = onecarpage->cars + lnt;
  onecarpage->used = (lnt + lentext + 1);
/*   strcpy   */
  restext[lentext] = '\0';
  return restext;
}


/*             IGES FILE  Properly Speaking             */

/*             Initialization of file recording            */
void iges_initfile()
{
  onecarpage = (struct carpage*) malloc ( sizeof(struct carpage) );
  onecarpage->used = 0; onecarpage->next = NULL;  restext = NULL;
  oneparpage = (struct parpage*) malloc ( sizeof(struct parpage) );
  oneparpage->used = 0; oneparpage->next = NULL;

  starts = (struct parlist*) malloc ( sizeof(struct parlist) );
  starts->first = starts->last = NULL; starts->nbparam = 0;
  header = (struct parlist*) malloc ( sizeof(struct parlist) );
  header->first = header->last = NULL; header->nbparam = 0;

  curlist = starts;    /* We start recording the start section */
  nbparts = nbparams = 0;
  firstpage = (struct dirpage*) malloc ( sizeof(struct dirpage) );
  firstpage->next = NULL; firstpage->used = 0;
  curpage = firstpage;
}  

/*   Switch to Header (Global Section), reading as writing    */
void iges_setglobal()
{  if (curlist == header) return;  curlist = header;    curparam = curlist->first;  }


/*   Definition and Selection of a new dirpart   */

void iges_newpart(int numsec)
{
  if (curpage->used >= Maxparts) {
    struct dirpage* newpage;
    newpage = (struct dirpage*) malloc ( sizeof(struct dirpage) );
    newpage->next = NULL; newpage->used = 0;
    curpage->next = newpage; curpage = newpage;
  }
  curnumpart = curpage->used;
  curp = &(curpage->parts[curnumpart]);
  curlist = &(curp->list);
  curp->numpart = numsec; curlist->nbparam = 0;
  curlist->first = curlist->last = NULL;
  curpage->used ++;  nbparts ++;
}


/*   Selection of dirpart dnum, corresponds to numsec in Psect   */

void iges_curpart (int dnum)
{
  if (curp == NULL) return;
  if (dnum == curp->numpart) return;
  if (curnumpart < curpage->used - 1) curnumpart ++;
  else {
    if (curpage->next == NULL) curpage = firstpage;
    else curpage = curpage->next;
    curnumpart = 0;
  }
  curp = &(curpage->parts[curnumpart]);
  curlist = &(curp->list);
  if (dnum == curp->numpart) return;
  curpage = firstpage;
  while (curpage != NULL) {
    int i; int nbp = curpage->used;
    for (i = 0; i < nbp; i ++) {
      if (curpage->parts[i].numpart == dnum) {
	curnumpart = i;
	curp = &(curpage->parts[i]);
	curlist = &(curp->list);
	return;
      }
    }
    curpage = curpage->next;
  }
  curp = NULL;    /*  not found  */
}


/*     Definition of a new parameter    */
/*   (missing management of Hollerith over multiple lines)   */

/*   longval: length of parval, including the final zero   */
void iges_newparam (int typarg, int longval, char *parval)
{
  char *newval;
  int i;

  if (curlist == NULL) return;      /*  not defined: abort  */

  newval = iges_newchar(longval);
  for (i = 0; i < longval; i++) newval[i] = parval[i];

  /*  curparam = (struct oneparam*) malloc ( sizeof(struct oneparam) );  */
  if (oneparpage->used > Maxpar) {
    struct parpage* newparpage;
    newparpage = (struct parpage*) malloc ( sizeof(struct parpage) );
    newparpage->next = oneparpage; newparpage->used = 0;
    oneparpage = newparpage;
  }
  curparam = &(oneparpage->params[oneparpage->used]);
  oneparpage->used ++;
  curparam->typarg = typarg;
  curparam->parval = newval;
  curparam->next = NULL;
  if (curlist->first == NULL) curlist->first = curparam;
  else curlist->last->next = curparam;
  curlist->last = curparam;
  curlist->nbparam ++;
  nbparams ++;
}

/*     Complement of current parameter (see Hollerith over multiple lines)    */
void iges_addparam (int longval, char* parval)
{
  char *newval, *oldval;
  int i, long0;
  if (longval <= 0) return;
  oldval = curparam->parval;
  long0 = (int)strlen(oldval);
/*  newval = (char*) malloc(long0+longval+1);  */
  newval = iges_newchar (long0 + longval + 1);
  for (i = 0; i < long0;   i ++) newval[i] = oldval[i];
  for (i = 0; i < longval; i ++) newval[i+long0] = parval[i];
  newval[long0+longval] = '\0';
  curparam->parval = newval;
}


/*               Re-reading: Initialization              */
/*  entities re-read following lirpart + {lirparam}
    initial lirparam: to re-read the startup (start section)   */
void iges_stats (int* nbpart, int* nbparam)
{
  curpage  = firstpage; curnumpart = 0;
  curlist  = starts;
  curparam = curlist->first;
  *nbpart  = nbparts;
  *nbparam = nbparams;
}

/*      Reading a part: return = section n0, 0 if end         */
/* \par tabval integer receiver array (reserve 17 values) */
/* \par res1 res2 nom num char: transmitted to part */
int iges_lirpart (int* *tabval, char* *res1, char* *res2, char* *nom, char* *num, int *nbparam)
{
  if (curpage == NULL) return 0;
  curp = &(curpage->parts[curnumpart]);
  curlist = &(curp->list);
  *nbparam = curlist->nbparam;
  curparam = curlist->first;
  *tabval = &(curp->typ);    /* address of curp = address of array */
  *res1 = curp->res1; *res2 = curp->res2;
  *nom  = curp->nom;  *num  = curp->num;
  return curp->numpart;
}

/*               Move to next (once parameters are read)          */
void iges_nextpart()
{
  curnumpart ++;
  if (curnumpart >= curpage->used) {  /* caution, addressing from 0 to used-1 */
    curpage = curpage->next;
    curnumpart = 0;
  }
}

/*               Read parameter + move to next                   */
int iges_lirparam (int *typarg, char* *parval)    /* returns 0 if end of list, 1 otherwise */
{
  if (curparam == NULL) return 0;
  *typarg = curparam->typarg;
  *parval = curparam->parval;
  curparam = curparam->next;
  return 1;
}

/*               End for this file: free the space                  */
/*    mode = 0: all; 1: parameters; 2: characters  */
void iges_finfile (int mode)
{
  struct dirpage* oldpage;
  if (mode == 0 || mode == 2) {  free (starts);  free (header);  }

  if (mode == 0 || mode == 1) {
    curpage = firstpage;
    while (curpage != NULL) {
      oldpage = curpage->next;
      free (curpage);
      curpage = oldpage;
    }

    while (oneparpage != NULL) {
      struct parpage* oldparpage;  oldparpage = oneparpage->next;
      free (oneparpage);
      oneparpage = oldparpage;
    }
  }

  if (mode == 0 || mode == 2) {
    while (onecarpage != NULL) {
      struct carpage* oldcarpage; oldcarpage = onecarpage->next;
      free (onecarpage);
      onecarpage = oldcarpage;
    }
  }
}
