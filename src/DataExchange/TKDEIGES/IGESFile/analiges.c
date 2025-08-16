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

#include <stdlib.h>
#include "igesread.h"

void iges_newparam(int typarg,int longval, char *parval);
void iges_newpart(int numsec);
void iges_curpart(int dnum);
void iges_addparam(int longval, char* parval);

#define ArgVide 0
#define ArgQuid 1
#define ArgChar 2
#define ArgInt  3   /* Unsigned integer: can evolve to Real or Char */
#define ArgSign 4   /* Signed integer: can evolve to Real */
#define ArgReal 5
#define ArgExp  6   /* Real + letter E: wait for confirmation */
#define ArgRexp 7   /* Real + Exponent: reduces to Real */
#define ArgMexp 8   /* Real + INCOMPLETE Exponent (no decimal point) */

/*  the working structures of structiges are known:
    namely declarations + curp  */
/*  #define VERIFPRINT  */

/*    Reading section D
      Each entity spans two lines

      Dstat starts at zero "we read a new entity"
      and switches with a   "second line of the entity"
*/


static int bases[] =
  { 1,10,100,1000,10000,100000,1000000, 10000000,100000000,1000000000 };

/*      DECODING: because scanf doesn't really do what's needed     */
/*      Utility decoding a number in fixed format in a line
	It starts from "depuis" included (line start = 0) and takes "tant" characters
	Value read as function return   */
static int IGES_decode (char* line, int depuis, int tant)
{
  int val = 0; int i;
  int depart = depuis+tant-1;
  for (i = 0; i < tant; i ++) {
    char uncar = line[depart-i];
    if (uncar == ' ') break;
    else if (uncar == '+') continue;
    else if (uncar == '-') val = -val;
    else if (uncar != '0') val += (uncar - 48)*bases[i];
  }
  return val;
}

/*   Copy of a fixed-length character string (closed by \0)  */
void IGES_copstr(char *line, int depuis, int tant, char* dans)
{
  int i;
  for (i = 0; i < tant; i ++) { dans[i] = line[depuis+i]; }
  dans[tant] = '\0';
}

/*                   Analysis section D                */
void iges_Dsect (int *Dstat, int numsec, char* line)
{
  struct dirpart *curp;
  if (*Dstat == 0) {
    iges_newpart(numsec);
    curp = iges_get_curp();
    curp->typ  = IGES_decode(line, 0,8);
    curp->poi  = IGES_decode(line, 8,8);
    curp->pdef = IGES_decode(line,16,8);
    curp->tra  = IGES_decode(line,24,8);
    curp->niv  = IGES_decode(line,32,8);
    curp->vue  = IGES_decode(line,40,8);
    curp->trf  = IGES_decode(line,48,8);
    curp->aff  = IGES_decode(line,56,8);
    curp->blk  = IGES_decode(line,64,2);
    curp->sub  = IGES_decode(line,66,2);
    curp->use  = IGES_decode(line,68,2);
    curp->her  = IGES_decode(line,70,2);
#ifdef VERIFPRINT
    printf("Entity %d : type %d  ->P %d\n",numsec,typ,poi);
#endif
    *Dstat = 1;
  } else if (*Dstat == 1) {
    curp = iges_get_curp();
    curp->typ2 = IGES_decode(line, 0,8);
    curp->epa  = IGES_decode(line, 8,8);
    curp->col  = IGES_decode(line,16,8);
    curp->nbl  = IGES_decode(line,24,8);
    curp->form = IGES_decode(line,32,8);
    IGES_copstr (line,40,8,curp->res1);
    IGES_copstr (line,48,8,curp->res2);
    IGES_copstr (line,56,8,curp->nom);
    IGES_copstr (line,64,8,curp->num);
#ifdef VERIFPRINT
    printf("Entity %d : type %d (redite) form %d\n",numsec,typ2,form);
#endif
    *Dstat = 0;
  }
}


/*     Reading section P: pre-analysis
       Extraction of D number and truncation to 64 characters  */

void iges_Psect (int numsec, char line[80])
{
  int dnum;
  dnum = atoi(&line[65]);
  line[64] = '\0';
  iges_curpart(dnum);
#ifdef VERIFPRINT
  printf("Entity P:%d ->D:%d,soit %s\n",numsec,dnum,line);
#else
  (void)numsec; // just to avoid warning
#endif
}


/*     Parsing line section P (or G)
       Parameter breakdown
       Separated by current Separator (c_separ), end (c_fin), plus
       character processing in Hollerith form ... (nnnH...)

       Pstat input : 0 entity start, 1 line start, 2 current, 3 not finished(H)
       Pstat return : 0 entity end, 1 line end, 2 current (here's one), 3
          not finished (a nnnH... not finished)
*/

static int nbcarH = 0; static int numcar = 0; static int reste = 0;
/*  reste: 0 normal case; 1 complete parameter; -1 skip it  */
static int typarg;
/*  +  parameter type definitions at head  */


void iges_param (int *Pstat, char *line, char c_separ, char c_fin, int lonlin)
{
  int i,i0,j; char param[80]; char unpar;
  if (*Pstat == 0) reste  = 0;
  if (*Pstat != 2) numcar = 0;
  if (*Pstat < 3)  nbcarH = 0;
  else {
    numcar = nbcarH;
    if (numcar > lonlin) {
      iges_addparam(lonlin,line);
      nbcarH -= lonlin;   /*  ??? enregistrer ...  ???  */
      return;
    } else {
      iges_addparam(nbcarH,line);
      nbcarH = 0;
    }
  }
  i0 = 0;     /*  useful param start (after possible blanks), default to 0 */
  typarg = ArgVide;
  for (i = 0; (unpar = line[numcar+i]) != '\0'; i ++) {
    if (unpar == c_separ) {
      *Pstat = 2;  param[i] = '\0';
#ifdef VERIFPRINT
      printf("numcar = %d type %d param: %s ",numcar,typarg,&param[i0]);
#endif
      if (reste == 0) iges_newparam(typarg,i-i0+1,&param[i0]);
      else if (reste > 0) iges_addparam(i-i0+1,&param[i0]);
      reste = 0;
      for (j = i+1; (unpar = line[numcar+j]) != '\0'; j++) {
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

/*    Parameter type ? */

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
      if (numcar+i+nbcarH >= lonlin) {   /* text spanning multiple lines */
	for (j = 1; j < lonlin-numcar-i; j++) param[i+j] = line[numcar+i+j];
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
	for (j = 1; j <= nbcarH; j++) param[i+j] = line[numcar+i+j];
        i += nbcarH;
      }
    }

/*   blank : leading (easy) or trailing (look for continuation), otherwise bad */
    else if (unpar == ' ') {
      if (typarg == ArgVide) i0 = i+1;
      else {
	for (j = i+1; (unpar = line[numcar+j]) != '\0' ; j ++) {
	  if (unpar == c_separ || unpar == c_fin) break;
	  if (unpar != ' ')  {  typarg = ArgQuid;  break;  }
	}
      }
    }

    else typarg = ArgQuid;   /* unrecognized character */
  }
/*  Here, end of line without separator : note current parameter !  */
  *Pstat = 1;  param[i] = '\0'; reste = -1;
#ifdef VERIFPRINT
  printf ("End of line without separator, numcar,i : %d %d\n",numcar,i);
  if (i > i0) printf("numcar = %d type %d param: %s ",numcar,typarg,&param[i0]);
#endif
  if (i > i0) iges_newparam(typarg,i-i0+1,&param[i0]);
}
