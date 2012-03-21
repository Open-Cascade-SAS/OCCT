// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <stdio.h>
#include <sys/stat.h>

#ifdef WNT
# include <io.h>
#endif /* WNT */

static FILE* filed;
/***************************************************OPEN_FILE**************/
FILE* OPEN_FILE (char* f_name, int TypeCgm)
{
  switch (TypeCgm){
    case 1:
    case 2:
      if ((filed = fopen(f_name,"wb" ))) {
#ifdef WNT
        chmod (f_name, S_IREAD | S_IWRITE);
#else
        chmod (f_name, 00777);
#endif
/*        setbuf (filed, NULL);*/
      }
      break;
    case 3:
      if ((filed = fopen(f_name,"w" ))) {
#ifdef WNT
        chmod (f_name, S_IREAD | S_IWRITE);
#else
        chmod (f_name, 00777);
#endif
/*        setbuf (filed, NULL);*/
      }
      break;
  }
  return filed;
}

/**************************************************CLOSE_FILE************/
void CLOSE_FILE ()
{
  fclose (filed);
}

/**************************************************getfd************/
FILE* getfd ()
{
  return filed;
}
