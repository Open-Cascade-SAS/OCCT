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

#include <stdlib.h>
#include "igesread.h"

void iges_newparam(int typarg,int longval, char *parval);
void iges_newpart(int numsec);
void iges_curpart(int numsec,int dnum);
void iges_addparam(int longval, char* parval);

#define ArgVide 0
#define ArgQuid 1
#define ArgChar 2
#define ArgInt  3   /* Entier non signe : peut evoluer vers Real ou Char */
#define ArgSign 4   /* Entier signe : peut evoluer vers Real */
#define ArgReal 5
#define ArgExp  6   /* Real + lettre E : attendre confirmation */
#define ArgRexp 7   /* Real + Exposant : se ramene a Real */
#define ArgMexp 8   /* Real + Exposant INCOMPLET (pas de point decimal) */

/*  les structures de travail de  structiges  sont connues :
    a savoir declarations + curp  */
/*  #define VERIFPRINT  */

/*    Lecture section D
      Chaque entite tient sur deux lignes

      Dstat demarre a zero "on lit une nouvelle entite"
      et bascule avec un   "deuxieme ligne de l'entite"
*/


static int bases[] =
  { 1,10,100,1000,10000,100000,1000000, 10000000,100000000,1000000000 };

/*      DECODAGE : parce que scanf ne fait pas vraiment ce qu'il faut     */
/*      Utilitaire decodant un nombre en format fixe dans une ligne
	Il part de "depuis" inclus (debut ligne = 0) et prend "tant" caracteres
	Valeur lue en retour de fonction   */
static int IGES_decode(ligne,depuis,tant)
char* ligne; int depuis, tant;
{
  int val = 0; int i;
  int depart = depuis+tant-1;
  for (i = 0; i < tant; i ++) {
    char uncar = ligne[depart-i];
    if (uncar == ' ') break;
    else if (uncar == '+') continue;
    else if (uncar == '-') val = -val;
    else if (uncar != '0') val += (uncar - 48)*bases[i];
  }
  return val;
}

/*   Recopie d'une chaine de caracteres de longueur fixe (close par \0)  */
void IGES_copstr(ligne,depuis,tant,dans)
char *ligne; int depuis, tant; char* dans;
{
  int i;
  for (i = 0; i < tant; i ++) { dans[i] = ligne[depuis+i]; }
  dans[tant] = '\0';
}

/*                   Analyse section D                */
void iges_Dsect (Dstat,numsec,ligne)
int *Dstat,numsec; char* ligne;
{
  struct dirpart *curp;
  if (*Dstat == 0) {
    iges_newpart(numsec);
    curp = iges_get_curp();
    curp->typ  = IGES_decode(ligne, 0,8);
    curp->poi  = IGES_decode(ligne, 8,8);
    curp->pdef = IGES_decode(ligne,16,8);
    curp->tra  = IGES_decode(ligne,24,8);
    curp->niv  = IGES_decode(ligne,32,8);
    curp->vue  = IGES_decode(ligne,40,8);
    curp->trf  = IGES_decode(ligne,48,8);
    curp->aff  = IGES_decode(ligne,56,8);
    curp->blk  = IGES_decode(ligne,64,2);
    curp->sub  = IGES_decode(ligne,66,2);
    curp->use  = IGES_decode(ligne,68,2);
    curp->her  = IGES_decode(ligne,70,2);
#ifdef VERIFPRINT
    printf("Entite %d : type %d  ->P %d\n",numsec,typ,poi);
#endif
    *Dstat = 1;
  } else if (*Dstat == 1) {
    curp = iges_get_curp();
    curp->typ2 = IGES_decode(ligne, 0,8);
    curp->epa  = IGES_decode(ligne, 8,8);
    curp->col  = IGES_decode(ligne,16,8);
    curp->nbl  = IGES_decode(ligne,24,8);
    curp->form = IGES_decode(ligne,32,8);
    IGES_copstr (ligne,40,8,curp->res1);
    IGES_copstr (ligne,48,8,curp->res2);
    IGES_copstr (ligne,56,8,curp->nom);
    IGES_copstr (ligne,64,8,curp->num);
#ifdef VERIFPRINT
    printf("Entite %d : type %d (redite) form %d\n",numsec,typ2,form);
#endif
    *Dstat = 0;
  }
}


/*     Lecture section P : preanalyse
       Extraction du numero D et troncature a 64 caracteres  */

void iges_Psect(Pstat,numsec,ligne)
int *Pstat; int numsec; char ligne[80];
{
  int dnum;
  dnum = atoi(&ligne[65]);
  ligne[64] = '\0';
  iges_curpart(numsec,dnum);
#ifdef VERIFPRINT
  printf("Entite P:%d ->D:%d,soit %s\n",numsec,dnum,ligne);
#endif
}


/*     Depiautage ligne section P (ou G)
       Fractionnement en parametres
       Separes par Separateur courant (c_separ), de fin (c_fin), plus
       traitement des caracteres sous forme Hollerith ... (nnnH...)

       Pstat entree : 0 debut entite, 1 debut ligne, 2 courant, 3 pas fini(H)
       Pstat retour : 0 fin entite, 1 fin ligne, 2 courant (en voila un), 3
          pas fini (un nnnH... pas termine)
*/

static int nbcarH = 0; static int numcar = 0; static int reste = 0;
/*  reste : 0 cas normal; 1 completer parametre; -1 le sauter  */
static int typarg;
/*  +  definitions des types de parametres en tete  */


void iges_param(Pstat,ligne,c_separ,c_fin,lonlin)
int *Pstat; char c_separ,c_fin, *ligne; int lonlin;
{
  int i,i0,j; char param[80]; char unpar;
  if (*Pstat == 0) reste  = 0;
  if (*Pstat != 2) numcar = 0;
  if (*Pstat < 3)  nbcarH = 0;
  else {
    numcar = nbcarH;
    if (numcar > lonlin) {
      iges_addparam(lonlin,ligne);
      nbcarH -= lonlin;   /*  ??? enregistrer ...  ???  */
      return;
    } else {
      iges_addparam(nbcarH,ligne);
      nbcarH = 0;
    }
  }
  i0 = 0;     /*  debut param utile (apres blancs eventuels), par defaut a 0 */
  typarg = ArgVide;
  for (i = 0; (unpar = ligne[numcar+i]) != '\0'; i ++) {
    if (unpar == c_separ) {
      *Pstat = 2;  param[i] = '\0';
#ifdef VERIFPRINT
      printf("numcar = %d type %d param: %s ",numcar,typarg,&param[i0]);
#endif
      if (reste == 0) iges_newparam(typarg,i-i0+1,&param[i0]);
      else if (reste > 0) iges_addparam(i-i0+1,&param[i0]);
      reste = 0;
      for (j = i+1; (unpar = ligne[numcar+j]) != '\0'; j++) {
	if (unpar != ' ') { numcar += i+1; return; }
      }
      *Pstat = 1; return;
    }
    if (unpar == c_fin) {
      *Pstat = 1;  param[i] = '\0';
#ifdef VERIFPRINT
      printf("numcar = %d type %d param: %s ",numcar,typarg,&param[i0]);
#endif
      if (reste == 0) iges_newparam(typarg,i-i0+1,&param[i0]);
      else if (reste > 0) iges_addparam(i-i0+1,&param[i0]);
      reste = 0;
      return;
    }
    param[i] = unpar;

/*    Type du parametre ? */

    if (unpar > 47 && unpar < 58) {
      if (typarg == ArgInt) continue;
      if (typarg == ArgVide) typarg = ArgInt;
      else if (typarg == ArgExp) typarg = ArgRexp;
    }

    else if (unpar == '+' || unpar == '-') {
      if (typarg == ArgVide) typarg = ArgSign;
      else if (typarg != ArgExp && typarg != ArgMexp) typarg = ArgQuid;
    }

    else if (unpar == '.') {
      if (typarg == ArgVide) typarg = ArgReal;
      else if (typarg == ArgInt || typarg == ArgSign) typarg = ArgReal;
      else typarg = ArgQuid;
    }

    else if (unpar == 'E' || unpar == 'e' || unpar == 'D' || unpar == 'd') {
      if (typarg == ArgReal) typarg = ArgExp;
      else if (typarg == ArgInt || typarg == ArgSign) typarg = ArgMexp;
      else typarg = ArgQuid;
    }

    else if (unpar == 'H') {         /* format Hollerith ? */
      if (typarg != ArgInt) { typarg = ArgQuid; continue; }
      typarg = ArgChar;
      nbcarH = 0;
      for (j = i0; j < i; j++) {
	if (param[j] > 47 && param[j] < 58) nbcarH = nbcarH*10 + (param[j]-48);
	else { nbcarH = 0; break; }
      }
      if (numcar+i+nbcarH >= lonlin) {   /* texte a cheval sur +ieurs lignes */
	for (j = 1; j < lonlin-numcar-i; j++) param[i+j] = ligne[numcar+i+j];
	param[lonlin-numcar] = '\0';
	nbcarH = (numcar+i +nbcarH+1 -lonlin);
	*Pstat =3;
#ifdef VERIFPRINT
	printf("numcar = %d param: %s ",numcar,param);
#endif
	iges_newparam(typarg,lonlin-i0,&param[i0]);
	reste = 1;
	return;
      } else {
	for (j = 1; j <= nbcarH; j++) param[i+j] = ligne[numcar+i+j];
        i += nbcarH;
      }
    }

/*   blanc : leading (facile) ou trailing (chercher la suite), sinon mauvais */
    else if (unpar == ' ') {
      if (typarg == ArgVide) i0 = i+1;
      else {
	for (j = i+1; (unpar = ligne[numcar+j]) != '\0' ; j ++) {
	  if (unpar == c_separ || unpar == c_fin) break;
	  if (unpar != ' ')  {  typarg = ArgQuid;  break;  }
	}
      }
    }

    else typarg = ArgQuid;   /* caractere non reconnu */
  }
/*  Ici, fin de ligne sans separateur : noter parametre en cours !  */
  *Pstat = 1;  param[i] = '\0'; reste = -1;
#ifdef VERIFPRINT
  printf ("Fin de ligne sans separateur, numcar,i : %d %d\n",numcar,i);
  if (i > i0) printf("numcar = %d type %d param: %s ",numcar,typarg,&param[i0]);
#endif
  if (i > i0) iges_newparam(typarg,i-i0+1,&param[i0]);
}
