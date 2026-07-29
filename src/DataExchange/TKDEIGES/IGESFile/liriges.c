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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*    Basic routine for reading an IGES file

      This routine reads a line, except if the "re-read in place" status is set
      (used for section change): it is reset afterwards

  This routine returns:
  - status (function return): section no: S,G,D,P,T (char 73) or
    0 (EOF) or -1 (try to skip) or -2 (char 73 false)
  - a line number in the section (char 74 to 80)
  - the line truncated to 72 characters (binary 0 in the 73rd)
  Must provide it (buffer) a line reserved for 81 characters

  Error case: false line from the start -> abort. Otherwise try to step over
*/

static int iges_fautrelire = 0;
static int iges_readline(void* theContext,
                         IGES_ReadLineCallback theReadLine,
                         char theLine[100])
{
  int anIter;
  for (anIter = 0; anIter < 100; ++anIter)
  {
    theLine[anIter] = '\0';
  }

  if (theReadLine == NULL)
  {
    return 0;
  }

  return theReadLine(theContext, theLine, 100);
}

int iges_lire_stream(void* theContext,
                     IGES_ReadLineCallback theReadLine,
                     int* numsec,
                     char line[100],
                     int modefnes)
{
  int i, result;
  char typesec;
/*  int length;*/
  if (iges_fautrelire == 0)
  {
    if (*numsec == 0)
      line[72] = line[79] = ' ';

    line[0] = '\0';
    if(modefnes)
    {
      if (iges_readline(theContext, theReadLine, line) == 0)
        return 0;
    }
    else
    {
      do
      {
      }
      while (iges_readline(theContext, theReadLine, line) != 0
             && (line[0] == '\r' || line[0] == '\n'));

      if (line[0] == '\0')
        return 0;
    }

    if (*numsec == 0 && line[72] != 'S' && line[79] == ' ')
    {
      line[0] = '\0';

      if(modefnes)
      {
        if (iges_readline(theContext, theReadLine, line) == 0)
          return 0;
      }
      else
      {
        do
        {
        }
        while (iges_readline(theContext, theReadLine, line) != 0
               && (line[0] == '\r' || line[0] == '\n'));
        if (line[0] == '\0')
          return 0;
      }
    }

    if ((line[0] & 128) && modefnes)
    {
      for (i = 0; i < 80; i++)
        line[i] = (char)(line[i] ^ (150 + (i & 3)));
    }
  }

  i = (int)strlen(line);
  if (i > 0 && line[i - 1] == '\r')
  {
    line[i - 1] = '\0';
  }

  {//0x1A is END_OF_FILE for OS DOS and WINDOWS. For other OS we set this rule forcefully.
    char *fc = strchr(line, 0x1A);
    if(fc != 0)
    {
      fc[0] = '\0';
      return 0;
    }
  }

  iges_fautrelire = 0;
  if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r')
    return iges_lire_stream(theContext, theReadLine, numsec, line, modefnes); /* 0 */

  {
    char* anEndPtr = NULL;
    result         = (int)strtol(&line[73], &anEndPtr, 10);
    if (anEndPtr != &line[73])
    {
      *numsec = result;
      typesec = line[72];
      switch (typesec) {
       case 'S' :  line[72] = '\0'; return (1);
       case 'G' :  line[72] = '\0'; return (2);
       case 'D' :  line[72] = '\0'; return (3);
       case 'P' :  line[72] = '\0'; return (4);
       case 'T' :  line[72] = '\0'; return (5);
       default  :;
      }
      /* the column 72 is empty, try to check the neighbour*/
      if (strlen(line) == 80
          && (line[79] == '\n' || line[79] == '\r') && (line[0] <= '9' && line[0] >= '0')) {
         /*check in case of loss.*/
         int index;
         for(index = 1; line[index]<='9' && line[index]>='0'; index++);
         if (line[index]=='D' || line[index]=='d') {
           for(index = 79; index > 0; index--)
             line[index] = line[index-1];
           line[0]='.';
         }
         typesec = line[72];
         switch (typesec) {
         case 'S' :  line[72] = '\0'; return (1);
         case 'G' :  line[72] = '\0'; return (2);
         case 'D' :  line[72] = '\0'; return (3);
         case 'P' :  line[72] = '\0'; return (4);
         case 'T' :  line[72] = '\0'; return (5);
         default  :;
        }
      }
    }
  }

  // the line is not conform to standard, try to read it (if there are some missing spaces)
  // find the number end
  i = (int)strlen(line);
  while ((line[i] == '\0' || line[i] == '\n' || line[i] == '\r' || line[i] == ' ') && i > 0)
    i--;
  if (i != (int)strlen(line))
    line[i + 1] = '\0';
  // find the number start
  while (line[i] >= '0' && line[i] <= '9' && i > 0)
    i--;
  {
    char* aTailEndPtr = NULL;
    result            = (int)strtol(&line[i + 1], &aTailEndPtr, 10);
    if (aTailEndPtr == &line[i + 1])
      return -1;
  }
  *numsec = result;
  // find type of line
  while (line[i] == ' ' && i > 0)
    i--;
  typesec = line[i];
  switch (typesec) {
    case 'S' :  line[i] = '\0'; return (1);
    case 'G' :  line[i] = '\0'; return (2);
    case 'D' :  line[i] = '\0'; return (3);
    case 'P' :  line[i] = '\0'; return (4);
    case 'T' :  line[i] = '\0'; return (5);
    default  :; /* printf("Incorrect line, ignored n0.%d :\n%s\n",*numl,line); */
  }
  return -1;
}

/*          To control re-reading in place            */

void iges_arelire()
{  iges_fautrelire = 1;  }
