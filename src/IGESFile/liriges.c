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
  if (iges_fautrelire == 0)
  {
    if (*numsec == 0)
      ligne[72] = ligne[79] = ' ';

    ligne[0] = '\0'; 
    if(modefnes)
    {
      if (fgets(ligne,99,lefic) == NULL) /*for kept compatibility with fnes*/
        return 0;
    }
    else
    {
      /* PTV: 21.03.2002 it is neccessary for files that have only `\r` but no `\n` 
              examle file is 919-001-T02-04-CP-VL.iges */
      while ( fgets ( ligne, 2, lefic ) && ( ligne[0] == '\r' || ligne[0] == '\n' ) )
      {
      }
      
      if (fgets(&ligne[1],80,lefic) == NULL)
        return 0;
    }
    
    if (*numsec == 0 && ligne[72] != 'S' && ligne[79] == ' ')
    {/*        ON A DU FNES : Sauter la 1re ligne          */
      ligne[0] = '\0';
      
      if(modefnes)
      {
        if (fgets(ligne,99,lefic) == NULL) /*for kept compatibility with fnes*/
          return 0;
      }
      else
      {
        while ( fgets ( ligne, 2, lefic ) && ( ligne[0] == '\r' || ligne[0] == '\n' ) )
        {
        }
        if (fgets(&ligne[1],80,lefic) == NULL)
          return 0;
      }
    }

    if ((ligne[0] & 128) && modefnes)
    {
      for (i = 0; i < 80; i ++)
        ligne[i] = (char)(ligne[i] ^ (150 + (i & 3)));
    }
  }

  if (feof(lefic))
    return 0;

  {//0x1A is END_OF_FILE for OS DOS and WINDOWS. For other OS we set this rule forcefully.
    char *fc = strchr(ligne, 0x1A);
    if(fc != 0)
    {
      fc[0] = '\0';
      return 0;
    }
  }

  iges_fautrelire = 0;
  if (ligne[0] == '\0' || ligne[0] == '\n' || ligne[0] == '\r')
    return iges_lire(lefic,numsec,ligne,modefnes); /* 0 */

  if (sscanf(&ligne[73],"%d",&result) != 0) {
    *numsec = result;
    typesec = ligne[72];
    switch (typesec) {
     case 'S' :  ligne[72] = '\0'; return (1);
     case 'G' :  ligne[72] = '\0'; return (2);
     case 'D' :  ligne[72] = '\0'; return (3);
     case 'P' :  ligne[72] = '\0'; return (4);
     case 'T' :  ligne[72] = '\0'; return (5);
     default  :;
    }
    /* the column 72 is empty, try to check the neighbour*/
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
       default  :;
      }
    }
  }

  // the line is not conform to standard, try to read it (if there are some missing spaces)
  // find the number end
  i = (int)strlen(ligne);
  while ((ligne[i] == '\0' || ligne[i] == '\n' || ligne[i] == '\r' || ligne[i] == ' ') && i > 0)
    i--;
  if (i != (int)strlen(ligne))
    ligne[i + 1] = '\0';
  // find the number start
  while (ligne[i] >= '0' && ligne[i] <= '9' && i > 0)
    i--;
  if (sscanf(&ligne[i + 1],"%d",&result) == 0)
    return -1;
  *numsec = result;
  // find type of line
  while (ligne[i] == ' ' && i > 0)
    i--;
  typesec = ligne[i];
  switch (typesec) {
    case 'S' :  ligne[i] = '\0'; return (1);
    case 'G' :  ligne[i] = '\0'; return (2);
    case 'D' :  ligne[i] = '\0'; return (3);
    case 'P' :  ligne[i] = '\0'; return (4);
    case 'T' :  ligne[i] = '\0'; return (5);
    default  :; /* printf("Ligne incorrecte, ignoree n0.%d :\n%s\n",*numl,ligne); */
  }
  return -1;
}

/*          Pour commander la relecture sur place            */

void iges_arelire()
{  iges_fautrelire = 1;  }
