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

/*   Structures temporaires IGES (enregistrement des entites et parametres)
     Comprennent : les declarations, et la gestion de l'entite en cours  */

static int nbparts;
static int nbparams;

/*                Liste de parametres IGES (header ou current part)        */
static struct parlist *curlist;
static struct parlist *starts;   /*  Start Section du fichier IGES  */
static struct parlist *header;   /*  Entete du fichier IGES  */


/*                Declaration d'une portion de Directory IGES              */
static struct dirpart *curp;

struct dirpart *iges_get_curp (void)
{
  return curp;
}

/*                   Declaration d'un parametre IGES (Psect)              */
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
static struct parpage {    /* une page de parametres ; cf AddParam */
  struct parpage* next;
  int             used;
  struct oneparam params[Maxpar+1];
} *oneparpage;


static int curnumpart = 0;
static struct dirpage *curpage;


/*           ROUTINES UTILITAIRES de traitement des textes (char*)          */

/*     Gestion du texte courant : c'est un texte alloue dynamiquement
       iges_newchar en alloue un (jete le precedent alloue si pas lu)
       rec_gettext lit le texte en cours, qui ne sera pas desalloue ensuite
       rec_settext en force un autre en jetant le precedent (idem rec_newtext)
       tandis que rec_newtext alloue un texte, sans lien avec le courant
*/

#define Maxcar 10000

  static struct carpage {
    struct carpage* next;        /*  chainage des pages de caracteres  */
    int             used;        /*  place deja prise  */
    char  cars[Maxcar+1];        /*  page de caracteres  */
  } *onecarpage;

  static char* restext = NULL ;  /* texte courant  (allocation dynamique) */
/*  static int   resalloc = 0 ; */    /*   alloue (memoire a liberer) ou non   */

/*    Utilitaire : Reservation de caracteres
      Remplace suite de mini-malloc par gestion de page   */

char* iges_newchar (char* newtext, int lentext)
{
  int i, lnt;
  if ((lnt = onecarpage->used) > Maxcar-lentext-1) {  /* allouer nouvelle page */
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
  for (i = lentext-1; i >= 0; i --) restext[i] = newtext[i];
  restext[lentext] = '\0';
  return (restext);
}


/*             FICHIER  IGES  Proprement Dit             */

/*             Initialisation de l'enregistrement d'un fichier            */
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

  curlist = starts;    /* On commence a enregistrer la start section */
  nbparts = nbparams = 0;
  firstpage = (struct dirpage*) malloc ( sizeof(struct dirpage) );
  firstpage->next = NULL; firstpage->used = 0;
  curpage = firstpage;
}  

/*   Passage au Header (Global Section), lecture comme ecriture    */
void iges_setglobal()
{  if (curlist == header) return;  curlist = header;    curparam = curlist->first;  }


/*   Definition et Selection d'un nouveau dirpart   */

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


/*   Selection du dirpart dnum, correspond a numsec en Psect   */

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
  curp = NULL;    /*  pas trouve  */
}


/*     Definition d'un nouveau parametre    */
/*   (manque la gestion d'un Hollerith sur plusieurs lignes)   */

/*   longval : longueur de parval, incluant le zero final   */
void iges_newparam (int typarg, int longval, char *parval)
{
  char *newval;
  if (curlist == NULL) return;      /*  non defini : abandon  */
  newval = iges_newchar(parval,longval);
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

/*     Complement du parametre courant (cf Hollerith sur +ieurs lignes)    */
void iges_addparam (int longval, char* parval)
{
  char *newval, *oldval;
  int i, long0;
  if (longval <= 0) return;
  oldval = curparam->parval;
  long0 = (int)strlen(oldval);
/*  newval = (char*) malloc(long0+longval+1);  */
  newval = iges_newchar("",long0+longval+1);
  for (i = 0; i < long0;   i ++) newval[i] = oldval[i];
  for (i = 0; i < longval; i ++) newval[i+long0] = parval[i];
  newval[long0+longval] = '\0';
  curparam->parval = newval;
}


/*               Relecture : Initialiation              */
/*  entites relues par suite de lirpart + {lirparam}
    lirparam initiaux : pour relire le demarrage (start section)   */
void iges_stats (int* nbpart, int* nbparam)
{
  curpage  = firstpage; curnumpart = 0;
  curlist  = starts;
  curparam = curlist->first;
  *nbpart  = nbparts;
  *nbparam = nbparams;
}

/*      Lecture d'une part : retour = n0 section, 0 si fin         */
/* \par tabval tableau recepteur des entiers (reserver 17 valeurs) */
/* \par res1 res2 nom num char : transmis a part */
int iges_lirpart (int* *tabval, char* *res1, char* *res2, char* *nom, char* *num, int *nbparam)
{
  if (curpage == NULL) return 0;
  curp = &(curpage->parts[curnumpart]);
  curlist = &(curp->list);
  *nbparam = curlist->nbparam;
  curparam = curlist->first;
  *tabval = &(curp->typ);    /* adresse de curp = adresse du tableau */
  *res1 = curp->res1; *res2 = curp->res2;
  *nom  = curp->nom;  *num  = curp->num;
  return curp->numpart;
}

/*               Passage au suivant (une fois lus les parametres)          */
void iges_nextpart()
{
  curnumpart ++;
  if (curnumpart >= curpage->used) {  /* attention, adressage de 0 a used-1 */
    curpage = curpage->next;
    curnumpart = 0;
  }
}

/*               Lecture parametre + passage au suivant                   */
int iges_lirparam (int *typarg, char* *parval)    /* renvoie 0 si fin de liste, 1 sinon */
{
  if (curparam == NULL) return 0;
  *typarg = curparam->typarg;
  *parval = curparam->parval;
  curparam = curparam->next;
  return 1;
}

/*               Fin pour ce fichier : liberer la place                  */
/*    mode = 0 : tout; 1 : parametres; 2 : caracteres  */
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
