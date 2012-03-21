/*
 Copyright (c) 1998-1999 Matra Datavision
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

#include <stdio.h>
#include <stdlib.h>



int main(int argc,char **argv) { 
  char t[10000];
  do { 
    if(fgets(t,10000,stdin)) { 
      if (t[0] == 'D' && t[1] == 'r' && t[2] == 'a' && t[3] == 'w' &&
	  t[4] == '[' && t[5] == '1' && t[6] == ']')
	fputs(t+9,stdout);
      else
	fputs(t,stdout);
    }
  }
  while(!feof(stdin));

  return(1);
}
