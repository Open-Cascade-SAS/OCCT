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

/*  Regroupement des sources "C" pour compilation   */ 
#include <stdio.h>
#include "igesread.h"

/*
void IGESFile_Check21 (int mode,char * code, int num, char * str);
*/
void IGESFile_Check3 (int mode,char * code);
void IGESFile_Check2 (int mode,char * code, int num, char * str);

/*  #include "structiges.c"    ...  fait par analiges qui en a l'usage  ...  */
void iges_initfile();
int  iges_lire (FILE* lefic, int *numsec, char ligne[100], int modefnes);
void iges_newparam(int typarg,int longval, char *parval);
void iges_param(int *Pstat,char *ligne,char c_separ,char c_fin,int lonlin);
void iges_Dsect (int *Dstat,int numsec,char* ligne);
void iges_Psect(int *Pstat,int numsec,char ligne[80]);



/*  Routine de lecture generale d'un fichier IGES
    Assure l'enchainement des appels necessaires
    Il en resulte un ensemble de donnees (struct C) interrogeables par
    routines ad hoc  (cf igesread.h qui les recapitule pour appel par C++)

    Retourne : 0 si OK, 1 si fichier pas pu etre ouvert
  */


/* MGE 16/06/98*/
/* To use strcpy*/
/*#include <string.h>*/
/* To use Msg class */
/*#include <MoniTool_Msg.hxx>*/

static  char sects [] = " SGDPT ";


int igesread(nomfic,lesect,modefnes)
char* nomfic; int lesect[6]; int modefnes;
{
  /* MGE 16/06/98 */

  FILE* lefic; char ligne[100]; int numsec, numl;  int i; int i0;int j;
  char str[2];

  int Dstat = 0; int Pstat = 0; char c_separ = ','; char c_fin = ';';
  iges_initfile();
  lefic = stdin; i0 = numsec = 0;  numl = 0;
  if (nomfic[1] != '\0') lefic = fopen(nomfic,"r");
  if (lefic == NULL) return -1;    /*  fichier pas pu etre ouvert  */
  for (i = 1; i < 6; i++) lesect[i] = 0;
  for (j = 0; j < 100; j++) ligne[j] = 0;
  while (1) {
    numl ++;
    i = iges_lire(lefic,&numsec,ligne,modefnes);
    if (i <= 0) {
      if (i  == 0) break;
      /* Sending of message : Syntax error */
      {
	str[1] = '\0';
	str[0] = sects[i0];
	IGESFile_Check2 (0,"XSTEP_18",numl,str); /* //gka 15 Sep 98: str instead of sects[i0]); */
      }
    
      if (i0 == 0) return -1;
      lesect[i0] ++;
      continue;
    }
    lesect[i] ++;  i0 = i;
    if (numsec != lesect[i]) {
      /* Sending of message : Syntax error */
      str[1] = '\0';
      str[0] = sects[i0];
      IGESFile_Check2 (0,"XSTEP_19",numl,str); /* //gka 15 Sep 98: str instead of sects[i0]); */
    }

    if (i == 1) {                                   /* Start Section (comm.) */
      ligne[72] = '\0';
      iges_newparam (0,72,ligne);
    }
    if (i == 2) {                                   /* Header (Global sect) */
      iges_setglobal();
      for (;;) {
	if (lesect[i] == 1) {    /* Separation specifique */
	  int n0 = 0;
	  if (ligne[0] != ',') {  c_separ = ligne[2]; n0 = 3;  }
	  if (ligne[n0+1] != c_separ) { c_fin = ligne[n0+3]; }
	}
	iges_param(&Pstat,ligne,c_separ,c_fin,72);
	if (Pstat != 2) break;
      }
    }
    if (i == 3) iges_Dsect(&Dstat,numsec,ligne);    /* Directory  (Dsect) */
    if (i == 4) {                                   /* Parametres (Psect) */
      iges_Psect(&Pstat,numsec,ligne);
      for (;;) {
	iges_param(&Pstat,ligne,c_separ,c_fin,64);
	if (Pstat != 2) break;
      }
    }
  }

  /* Sending of message : No Terminal Section */
  if (lesect[5] == 0) {
    IGESFile_Check3 (1, "XSTEP_20");
    //return -1;
  }
  

  fclose (lefic);

  return 0;
}
