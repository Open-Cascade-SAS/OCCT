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

/*
 pdn PRO16162: do restart in order to restore after possible crash or wrong data 
 rln 10.01.99 - transmission of define's into this file
*/ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "recfile.ph"

void rec_debfile();
void rec_finfile();

/*
  Lecture d'un fichier ia grammaire lex-yacc
  Appel : i = stepread() ;  i est la valeur retournee par yyparse
  (0 si OK, 1 si erreur)
*/

int stepread(std::istream& theStream)
{
  int aLetat = 0;
  rec_debfile();
  step::scanner aScanner(&theStream);
  aScanner.yyrestart(theStream);
  step::parser aParser(&aScanner);
  aLetat = aParser.parse();
  rec_finfile();
  return aLetat;
}