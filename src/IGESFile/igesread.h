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

/* Appel externe aux routines de lecture (en C) */ 
#include <stdio.h>

/*  structiges : */
struct parlist {
  struct oneparam *first, *last;
  int nbparam;
};

struct dirpart {
  int typ,poi,pdef,tra,niv,vue,trf,aff,blk,sub,use,her;  /* ligne 1 */
  int typ2,epa,col,nbl,form;                             /* ligne 2 */
  char res1[10],res2[10],nom[10],num[10];
  struct parlist list;                                   /* liste Psect */
  int numpart;                                           /* n0 en Dsect */
};

#ifdef __cplusplus
extern "C" {
#endif

  int  igesread   (char* nomfic,int lesect[6],int modefnes);

  /*  structiges : */
  int  iges_lirpart
   (int* *tabval,char* *res1,char* *res2,char* *nom,char* *num,int* nbparam);
  void iges_stats    (int* nbpart, int* nbparam);
  void iges_setglobal ();
  void iges_nextpart ();
  int  iges_lirparam (int* typarg,char* *parval);
  void iges_finfile  (int mode);
  struct dirpart *iges_get_curp (void);

  void iges_initfile();
  int  iges_lire (FILE* lefic, int *numsec, char ligne[100], int modefnes);
  void iges_newparam(int typarg,int longval, char *parval);
  void iges_param(int *Pstat,char *ligne,char c_separ,char c_fin,int lonlin);
  void iges_Dsect (int *Dstat,int numsec,char* ligne);
  void iges_Psect(int *Pstat,int numsec,char ligne[80]);

  /* MGE 20/07/98 */
  void IGESFile_Check2 (int mode,char * code, int num, char * str);
  void IGESFile_Check3 (int mode,char * code);

#ifdef __cplusplus
}
#endif

/*  Definition des types de parametres de l'analyseur de base IGES */
#define ArgVide 0
#define ArgQuid 1
#define ArgChar 2
#define ArgInt  3   /* Entier non signe : peut evoluer vers Real ou Char */
#define ArgSign 4   /* Entier signe : peut evoluer vers Real */
#define ArgReal 5
#define ArgExp  6   /* Real + lettre E : attendre confirmation */
#define ArgRexp 7   /* Real + Exposant : se ramene a Real */
#define ArgMexp 8   /* Real + Exposant INCOMPLET (pas de point decimal) */
