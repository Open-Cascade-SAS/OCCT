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

// stepread.h

/* lecture du fichier STEP (par appel a lex+yac) */
#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif

void recfile_modeprint (int mode) ;     /* controle trace recfile */
/* creation du Direc a partir de recfile : entrys connues de c++ */
void lir_file_nbr(int* nbh, int* nbr, int* nbp) ;
int  lir_file_rec(char* *ident , char* *type , int* nbarg) ;
void lir_file_finrec() ;
int  lir_file_arg(int* type , char* *val) ;
void lir_file_fin(int mode);

/* Interruption passant par C++  */

#ifdef __cplusplus
}
#endif

int stepread(std::istream& theStream);
void StepFile_Interrupt (char* theNomfic);