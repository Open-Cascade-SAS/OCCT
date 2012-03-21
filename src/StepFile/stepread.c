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

/* pdn PRO16162: do restart in order to restore after possible crash or wrong data
*/ 
/*rln 10.01.99 - transmission of define's into this file
*/ 
/**
*/ 
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "recfile.ph"

/*    StepFile_Error.c

      Ce programme substitue au yyerror standard, qui fait "exit" (brutal !)
      une action plus adaptee a un fonctionnement en process :

      Affichage de la ligne qui a provoque l' erreur,
      Preparation d'un eventuel appel suivant (vu qu on ne fait plus exit),
      en pour le retour, on s'arrange pour lever une exception
      (c-a-d qu on provoque un plantage)

      Adaptation pour flex (flex autorise d avoir plusieurs lex dans un meme
      executable) : les fonctions et variables sont renommees; et la
      continuation a change
*/

static int   lastno;
extern int   steplineno;

extern void StepFile_Interrupt (char* nomfic); /* rln 13.09.00 port on HP*/
int stepparse(void);
void  rec_debfile();
void steprestart(FILE *input_file);
void rec_finfile();

void steperror (char *mess)
{
  char newmess[80];
  if (steplineno == lastno) return;
  lastno = steplineno;
  sprintf    (newmess,"At line %d, %s",steplineno+1,mess);

/*  yysbuf[0] = '\0';
    yysptr    = yysbuf;
 *  yylineno  = 0;  */

  StepFile_Interrupt(newmess);
}

/*   But de ce mini-programme : appeler yyparse et si besoin preciser un
     fichier d'entree
     StepFile_Error  redefinit yyerror pour ne pas stopper (s'y reporter)
*/

extern FILE* stepin ;  /*  input de yyparse (executeur de lex-yacc)  */
extern int   steplineno;  /*  compteur de ligne lex  (pour erreurs)  */


/*   Designation d'un fichier de lecture
    (par defaut, c'est l'entree standard)

    Appel :  iflag = stepread_setinput ("...") ou (char[] ...) ;
                     stepread_setinput ("") [longueur nulle] laisse en standard
     iflag retourne vaut 0 si c'est OK, 1 sinon
*/

FILE* stepread_setinput (char* nomfic)
{
  FILE* newin ;
  if (strlen(nomfic) == 0) return stepin ;
  newin = fopen(nomfic,"r") ;
  if (newin == NULL) {
    return NULL ;
  } else {
    stepin = newin ; return newin ;
  }
}

void stepread_endinput (FILE* infic, char* nomfic)
{
  if (!infic) return;
  if (strlen(nomfic) == 0) return;
  fclose (infic);
}

/*  Lecture d'un fichier ia grammaire lex-yacc
    Appel : i = stepread() ;  i est la valeur retournee par yyparse
    (0 si OK, 1 si erreur)
*/
int stepread ()
{
  int letat;
  lastno = 0;
  steplineno = 0;
  rec_debfile() ;
  steprestart(stepin);
  letat = stepparse() ;
  rec_finfile() ;
  return letat;
}

int stepwrap ()  {  return 1;  }
