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

int main(int argc, char **argv) { 
  if(argc!=3) { 
    fprintf(stderr,"\n DIFF File1 File2 \n");
    return(-1);
  }
  FILE *fp1 = fopen(argv[1],"r");
  if(fp1 == NULL) { 
    fprintf(stderr,"\n Unable to open file1:%s \n",argv[1]);
    return(-2);
  }
  FILE *fp2 = fopen(argv[2],"r");
  if(fp2 == NULL) { 
    fprintf(stderr,"\n Unable to open file2:%s \n",argv[2]);
    return(-3);
  }

  int ok1 = fseek(fp1,0L,SEEK_END);
  long size1 = ftell(fp1);


  int ok2 = fseek(fp2,0L,SEEK_END);
  long size2 = ftell(fp2);

  //-- printf("\n size:   %s:%ld    %s:%ld\n",argv[1],size1,argv[2],size2);

  if(size2>size1) { 
    fseek(fp2,size1,SEEK_SET);
    while(!feof(fp2)) { 
      int c=fgetc(fp2);
      if(c!=-1) { 
	fputc(c,stdout);
      }    
    }
  }
  
  return(0);
}
  
