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

#include "igesread.h"
#include <string.h>
/*    Routine de base de lecture d'un fichier IGES

      Cette routine lit une ligne, sauf si le statut "relire sur place" est mis
      (utilise pour changement de section) : il est reannule ensuite

  Cette routine retourne :
  - statut (retour fonction) : no de section : S,G,D,P,T (car 73) ou
    0 (EOF) ou -1 (tacher de sauter) ou -2 (car. 73 faux)
  - un numero de ligne dans la section (car. 74 a 80)
  - la ligne tronquee a 72 caracteres (0 binaire dans le 73ieme)
  Il faut lui fournir (buffer) une ligne reservee a 81 caracteres

  Cas d erreur : ligne fausse des le debut -> abandon. Sinon tacher d enjamber
*/

static int iges_fautrelire = 0;
int  iges_lire (FILE* lefic, int *numsec, char ligne[100], int modefnes)
/*int iges_lire (lefic,numsec,ligne,modefnes)*/
/*FILE* lefic; int *numsec; char ligne[100]; int modefnes;*/
{
  int i,result; char typesec;
/*  int length;*/
  if (iges_fautrelire == 0) {
    if (*numsec == 0) ligne[72] = ligne[79] = ' ';
    ligne[0] = '\0'; 
    if(modefnes)	
	fgets(ligne,99,lefic); /*for kept compatibility with fnes*/
    else {
      /* PTV: 21.03.2002 it is neccessary for files that have only `\r` but no `\n` 
              examle file is 919-001-T02-04-CP-VL.iges */
      while ( fgets ( ligne, 2, lefic ) && ( ligne[0] == '\r' || ligne[0] == '\n' ) ) 
	{
	}
      fgets(&ligne[1],80,lefic);
/*     	fgets(ligne,81,lefic); */
    }
    if (*numsec == 0 && ligne[72] != 'S' && ligne[79] == ' ') {
/*        ON A DU FNES : Sauter la 1re ligne          */
    ligne[0] = '\0';
    if(modefnes)	
	fgets(ligne,99,lefic);/*for kept compatibility with fnes*/
    else {
      while ( fgets ( ligne, 2, lefic ) && ( ligne[0] == '\r' || ligne[0] == '\n' ) )
	{
	}
      fgets(&ligne[1],80,lefic);
/*     	fgets(ligne,81,lefic); */
      }
    }
    if ((ligne[0] & 128)&&modefnes) {
      for (i = 0; i < 80; i ++)  ligne[i] = ligne[i] ^ (150 + (i & 3));
    }
  }
  if (feof(lefic)) return 0;
  iges_fautrelire = 0;
  if (ligne[0] == '\0' || ligne[0] == '\n' || ligne[0] == '\r')
    return iges_lire(lefic,numsec,ligne,modefnes); /* 0 */
  if (sscanf(&ligne[73],"%d",&result) == 0)  return -1;
/*  { printf("Erreur, ligne n0.%d :\n%s\n",*numl,ligne); return (*numsec > 0 ? -1 : -2); } */
  *numsec = result;
  typesec = ligne[72];
  switch (typesec) {
   case 'S' :  ligne[72] = '\0'; return (1);
   case 'G' :  ligne[72] = '\0'; return (2);
   case 'D' :  ligne[72] = '\0'; return (3);
   case 'P' :  ligne[72] = '\0'; return (4);
   case 'T' :  ligne[72] = '\0'; return (5);
   default  :; /* printf("Ligne incorrecte, ignoree n0.%d :\n%s\n",*numl,ligne); */
  }
  /* the column 72 is empty, try to check the neghbour*/
  if(strlen(ligne)==80 
     && (ligne[79]=='\n' || ligne[79]=='\r') && (ligne[0]<='9' && ligne[0]>='0')) {
    /*check if the case of losted .*/
    int index;
    for(index = 1; ligne[index]<='9' && ligne[index]>='0'; index++);
    if (ligne[index]=='D' || ligne[index]=='d') {
      for(index = 79; index > 0; index--)
	ligne[index] = ligne[index-1];
      ligne[0]='.';
    }
      
    typesec = ligne[72];
    switch (typesec) {
    case 'S' :  ligne[72] = '\0'; return (1);
    case 'G' :  ligne[72] = '\0'; return (2);
    case 'D' :  ligne[72] = '\0'; return (3);
    case 'P' :  ligne[72] = '\0'; return (4);
    case 'T' :  ligne[72] = '\0'; return (5);
    default  :; /* printf("Ligne incorrecte, ignoree n0.%d :\n%s\n",*numl,ligne); */
    }
  }

  return -1;
}

/*          Pour commander la relecture sur place            */

void iges_arelire()
{  iges_fautrelire = 1;  }
