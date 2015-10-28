// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// AdvApp2Var_SysBase.cxx
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_Data.hxx>
#include <Standard.hxx>


static 
int __i__len();

static
int __s__cmp();

static
int macrbrk_();

static
int macrclw_(intptr_t *iadfld, 
	     intptr_t *iadflf, 
	     integer *nalloc);
static
int macrerr_(intptr_t *iad,
	     intptr_t *nalloc);
static
int macrgfl_(intptr_t *iadfld, 
	     intptr_t *iadflf, 
	     integer  *iphase, 
	     integer  *iznuti);
static
int macrmsg_(const char *crout, 
	     integer *num, 
	     integer *it, 
	     doublereal *xt, 
	     const char *ct, 
	     ftnlen crout_len,
	     ftnlen ct_len);

static
int macrstw_(intptr_t *iadfld, 
	     intptr_t *iadflf, 
	     integer *nalloc);

static
int madbtbk_(integer *indice);

static
int magtlog_(const char *cnmlog, 
	     const char *chaine, 
	     integer *long__, 
	     integer *iercod, 
	     ftnlen cnmlog_len, 
	     ftnlen chaine_len);


static
int mamdlng_(char *cmdlng, 
	     ftnlen cmdlng_len);

static
int maostrb_();

static
int maostrd_();

static
int maoverf_(integer *nbentr, 
	     doublereal *dtable);

static
int matrlog_(const char *cnmlog, 
	     const char *chaine, 
	     integer *length, 
	     integer *iercod, 
	     ftnlen cnmlog_len, 
	     ftnlen chaine_len);

static
int matrsym_(const char *cnmsym, 
	     const char *chaine, 
	     integer *length, 
	     integer *iercod, 
	     ftnlen cnmsym_len, 
	     ftnlen chaine_len);

static
int mcrcomm_(integer *kop, 
	     integer *noct, 
	     intptr_t *iadr, 
	     integer *ier);

static
int mcrfree_(integer *ibyte,
	     intptr_t iadr,
	     integer *ier);

static
int mcrgetv_(integer *sz,
	     intptr_t *iad,
	     integer *ier);

static
int mcrlocv_(void* t,
	     intptr_t *l);


static struct {
    integer lec, imp, keyb, mae, jscrn, itblt, ibb;
} mblank__;

#define mcrfill_ABS(a)  (((a)<0)?(-(a)):(a)) 


//=======================================================================
//function : AdvApp2Var_SysBase
//purpose  : 
//=======================================================================
AdvApp2Var_SysBase::AdvApp2Var_SysBase()
{
    mainial_();
    memset (&mcrstac_, 0, sizeof (mcrstac_));
}

//=======================================================================
//function : ~AdvApp2Var_SysBase
//purpose  : 
//=======================================================================
AdvApp2Var_SysBase::~AdvApp2Var_SysBase()
{
  assert (mcrgene_.ncore == 0); //otherwise memory leaking
}
  
//=======================================================================
//function : macinit_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macinit_(integer *imode, 
				 integer *ival)

{
 
  /* ************************************************************************/
  /*     FUNCTION : */
  /*     ---------- */
  /*   INITIALIZATION OF READING WRITING UNITS AND 'IBB' */
  
  /*     KEYWORDS : */
  /*     ----------- */
  /*   MANAGEMENT, CONFIGURATION, UNITS, INITIALIZATION */
  
  /*     INPUT ARGUMENTS  : */
  /*     -------------------- */
  /*        IMODE : MODE of INITIALIZATION : 
	    0= DEFAULT, IMP IS 6, IBB 0 and LEC 5 */
  /*        1= FORCE VALUE OF IMP */
  /*        2= FORCE VALUE OF IBB */
  /*        3= FORCE VALUE OF LEC */
  
  /*    ARGUMENT USED ONLY WHEN IMODE IS 1 OR 2 : */
  /*       IVAL : VALUE OF IMP WHEN IMODE IS 1 */
  /*              VALUE OF IBB WHEN IMODE IS 2 */
  /*              VALUE OF LEC WHEN IMODE IS 3 */
  /*    THERE IS NO CONTROL OF VALIDITY OF VALUE OF IVAL . */
  
  /*     OUTPUT ARGUMENTS  : */
  /*     -------------------- */
  /*                NONE */
  
  /*     COMMONS USED : */
  /*     -------------- */
  /*     REFERENCES CALLED : */
  /*     ------------------- */
  /*     DESCRIPTION/NOTES/LIMITATIONS : */
  /*     ------------------------------- */
  
  /*     THIS IS ONLY INITIALIZATION OF THE COMMON BLANK FOR ALL */
  /*     MODULES THAT A PRIORI DO NOT NEED TO KNOW THE COMMONS OF T . */
  /*     WHEN A MODIFICATION OF IBB IS REQUIRED (IMODE=2) AN INFO MESSAGE */
  /*     IS SUBMITTED ON IMP, WITH THE NEW VALUE OF IBB. */
  
  /*       IBB : MODE DEBUG OF STRIM T : RULES OF USE : */
  /*             0 RESTRAINED VERSION  */
  /*             >0 THE GREATER IS IBB THE MORE COMMENTS THE VERSION HAS. */
  /*                FOR EXAMPLE FOR IBB=1 THE ROUTINES CALLED */
  /*                INFORM ON IMP ('INPUT IN TOTO', */
  /*                AND 'OUTPUT FROM TOTO'), AND THE ROUTINES THAT RETURN */
  /*                NON NULL ERROR CODE INFORM IT AS WELL. */
  /*            (BUT IT IS NOT TRUE FOR ALL ROUTINES OF T) */
  /* > */
  /* ***********************************************************************
   */

  if (*imode == 0) {
    mblank__.imp = 6;
    mblank__.ibb = 0;
    mblank__.lec = 5;
  } else if (*imode == 1) {
    mblank__.imp = *ival;
  } else if (*imode == 2) {
    mblank__.ibb = *ival;
  } else if (*imode == 3) {
    mblank__.lec = *ival;
  }

  /* ----------------------------------------------------------------------*
   */
  
  return 0;
} /* macinit__ */

//=======================================================================
//function : macrai4_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macrai4_(integer *nbelem, 
				 integer *maxelm, 
				 integer *itablo,
				 intptr_t *iofset,
				 integer *iercod)

{
  
  /* ***********************************************************************
   */
  
  /*     FUNCTION : */
  /*     ---------- */
  /*       Require dynamic allocation of type INTEGER */
  
  /*     KEYWORDS : */
  /*     ---------- */
  /*       SYSTEM, ALLOCATION, MEMORY, REALISATION */
  
  /*     INPUT ARGUMENTS : */
  /*     ----------------- */
  /*       NBELEM : Number of required units */
  /*       MAXELM : Max number of units available in ITABLO */
  /*       ITABLO : Reference Address of the rented zone */
  
  /*     OUTPUT ARGUMENTS : */
  /*     ------------------- */
  /*       IOFSET : Offset */
  /*       IERCOD : Error code */
  /*               = 0 : OK */
  /*               = 1 : Max nb of allocations attained */
  /*               = 2 : Incorrect arguments */
  /*               = 3 : Refused dynamic allocation */
  
  /*     COMMONS USED : */
  /*     ------------------ */
  
  /*     REFERENCES CALLED : */
  /*     --------------------- */
  /*        MCRRQST */
  
  /*     DESCRIPTION/NOTES/LIMITATIONS : */
  /*     ----------------------------------- */
  /*     (Cf description in the heading of MCRRQST) */
  
  /*     Table ITABLO should be dimensioned to MAXELM by the caller. */
  /*     If the request is lower or equal to MAXELM, IOFSET becomes = 0.    */
  /*     Otherwise the demand of allocation is valid and IOFSET > 0. */
   /* > */
  /* ***********************************************************************
   */
  
  integer  iunit; 
  
  
  iunit = sizeof(integer);    
  /* Function Body */
  if (*nbelem > *maxelm) {
    /*AdvApp2Var_SysBase::*/mcrrqst_(&iunit, nbelem, itablo, iofset, iercod);
  } else {
    *iercod = 0;
    *iofset = 0;
  }
  return 0 ;
} /* macrai4_ */

//=======================================================================
//function : AdvApp2Var_SysBase::macrar8_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macrar8_(integer *nbelem, 
				 integer *maxelm,
				 doublereal *xtablo, 
				 intptr_t *iofset, 
				 integer *iercod)

{
  integer c__8 = 8;

  /* ***********************************************************************
   */
  
  /*     FUNCTION : */
  /*     ---------- */
  /*       Demand of dynamic allocation of type DOUBLE PRECISION */
  
  /*     KEYWORDS : */
  /*     ----------- */
  /*       SYSTEM, ALLOCATION, MEMORY, REALISATION */
  
  /*     INPUT ARGUMENTS  : */
  /*     ------------------ */
  /*       NBELEM : Nb of units required */
  /*       MAXELM : Max Nb of units available in XTABLO */
  /*       XTABLO : Reference address of the rented zone */
  
  /*     OUTPUT ARGUMENTS : */
  /*     ------------------ */
  /*       IOFSET : Offset */
  /*       IERCOD : Error code */
  /*               = 0 : OK */
  /*               = 1 : Max Nb of allocations reached */
  /*               = 2 : Arguments incorrect */
  /*               = 3 : Refuse of dynamic allocation */
  
  /*     COMMONS USED : */
  /*     ------------------ */
  
  /*     REFERENCES CALLED : */
  /*     --------------------- */
  /*        MCRRQST */
  
  /*     DESCRIPTION/NOTES/LIMITATIONS : */
  /*     ----------------------------------- */
  /*     (Cf description in the heading of MCRRQST) */
  
  /*     Table XTABLO should be dimensioned to MAXELM by the caller. */
  /*     If the request is less or equal to MAXELM, IOFSET becomes = 0.    */
  /*     Otherwise the demand of allocation is valid and IOFSET > 0. */

  /* > */
  /* ***********************************************************************
   */
  
  
  /* Function Body */
  if (*nbelem > *maxelm) {
    /*AdvApp2Var_SysBase::*/mcrrqst_(&c__8, nbelem, xtablo, iofset, iercod);
  } else {
    *iercod = 0;
    *iofset = 0;
  }
  return 0 ;
} /* macrar8_ */

//=======================================================================
//function : macrbrk_
//purpose  : 
//=======================================================================
int macrbrk_()
{
  return 0 ;
} /* macrbrk_ */

//=======================================================================
//function : macrchk_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macrchk_()
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  integer  i__, j;
  intptr_t ioff;
  doublereal* t = 0;
  intptr_t loc;
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       CONTROL OF EXCESSES OF ALLOCATED MEMORY ZONE */

/*     KEYWORDS : */
/*     ----------- */
/*       SYSTEM, ALLOCATION, MEMORY, CONTROL, EXCESS */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*       NONE */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*       NONE */

/*     COMMONS USED : */
/*     ------------------ */
/*       MCRGENE */

/*     REFERENCES CALLED : */
/*     --------------------- */
/*       MACRERR, MAOSTRD */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TABLE OF MANAGEMENT OF DYNAMIC MEMOTY ALLOCATIONS */

/*     KEYWORDS : */
/*     ----------- */
/*        SYSTEM, MEMORY, ALLOCATION */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE OF EXISTING ALLOCATIONS, EACH HAVING : */
/*         1 : LEVEL OF PROTECTION (0=NOT PROTECTED, OTHER=PROTECTED) */
/*             (PROTECTED MEANS NOT DESTROYED BY CRRSET .) */
/*         2 : UNIT OF ALLOCATION */
/*         3 : NB OF ALLOCATED UNITS */
/*         4 : REFERENCE ADDRESS OF THE TABLE */
/*         5 : IOFSET */
/*         6 : STATIC ALLOCATION NUMBER */
/*         7 : Required allocation size */
/*         8 : address of the beginning of allocation */
/*         9 : Size of the USER ZONE */
/*        10 : ADDRESS of the START FLAG */
/*        11 : ADDRESS of the END FLAG */
/*        12 : Rank of creation of the allocation */

/*   NDIMCR : NB OF DATA OF EACH ALLOC IN ICORE */
/*   NCORE : NB OF CURRENT ALLOCS */
/*   LPROT : COMMUNICATION BETWEEN CRPROT AND MCRRQST, SET TO 0 BY MCRRQST */
/*   FLAG  : VALUE OF THE FLAG USED FOR EXCESSES */



/* ----------------------------------------------------------------------*
 */


/* ----------------------------------------------------------------------*
 */

/* CALCULATE ADDRESS OF T */
  mcrlocv_(t, &loc);  
  /* CONTROL OF FLAGS IN THE TABLE */
  i__1 = mcrgene_.ncore;
  for (i__ = 0; i__ < i__1; ++i__) {

    //p to access startaddr and endaddr
    intptr_t* p = &mcrgene_.icore[i__].startaddr;
    for (j = 0; j <= 1; ++j) {
      intptr_t* pp = p + j;
      if (*pp != -1) {
	
	ioff = (*pp - loc) / 8;
	
	if (t[ioff] != -134744073.) {
	  
	  /* MSG : '*** ERREUR  : REMOVAL FROM MEMORY OF ADDRESS
	     E:',ICORE(J,I) */
	  /*       AND OF RANK ICORE(12,I) */
	  macrerr_(pp, p + 2);
	  
	  /* BACK-PARCING IN PHASE OF PRODUCTION */
	  maostrb_();
	  
	  /* REMOVAL OF THE ADDRESS OF FLAG TO AVOID REMAKING ITS CONTROL */
	  *pp = -1;
	  
	}
	
      }
      
      /* L100: */
    }
    
    /* L1000: */
  }
  return 0 ;
} /* macrchk_ */

//=======================================================================
//function : macrclw_
//purpose  : 
//=======================================================================
int macrclw_(intptr_t *,//iadfld, 
	     intptr_t *,//iadflf, 
	     integer  *)//nalloc)

{
  return 0 ;
} /* macrclw_ */

//=======================================================================
//function : AdvApp2Var_SysBase::macrdi4_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macrdi4_(integer *nbelem, 
				 integer *,//maxelm, 
				 integer *itablo, 
				 intptr_t *iofset, /* Offset long (pmn) */
				 integer *iercod)

{
  
  /* ***********************************************************************
 */

/*     FuNCTION : */
/*     ---------- */
/*       Destruction of dynamic allocation of type INTEGER */

/*     KEYWORDS : */
/*     ----------- */
/*       SYSTEM, ALLOCATION, MEMORY, DESTRUCTION */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*       NBELEM : Nb of units required */
/*       MAXELM : Max Nb of units available in ITABLO */
/*       ITABLO : Reference Address of the allocated zone */
/*       IOFSET : Offset */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       IERCOD : Error Code */
/*               = 0 : OK */
/*               = 1 : Pb of de-allocation of a zone allocated in table */
/*               = 2 : The system refuses the demand of de-allocation */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */
/*        MCRDELT */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     (Cf description in the heading of MCRDELT) */
/* > */
/* ***********************************************************************
 */
  integer iunit;
  
  iunit = sizeof(integer); 
  /* Function Body */
  if (*iofset != 0) {
    AdvApp2Var_SysBase::mcrdelt_(&iunit, 
				 nbelem, 
				 itablo, 
				 iofset, 
				 iercod);
  } else {
    *iercod = 0;
  }
  return 0 ;
} /* macrdi4_ */

//=======================================================================
//function : AdvApp2Var_SysBase::macrdr8_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macrdr8_(integer *nbelem,
				 integer *,//maxelm, 
				 doublereal *xtablo, 
				 intptr_t *iofset, 
				 integer *iercod)

{
  integer c__8 = 8;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       Destruction of dynamic allocation of type DOUBLE PRECISION 
*/

/*     KEYWORDS : */
/*     ----------- */
/*       SYSTEM, ALLOCATION, MEMORY, DESTRUCTION */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       NBELEM : Nb of units required */
/*       MAXELM : Max nb of units available in XTABLO */
/*       XTABLO : Reference Address of the allocated zone */
/*       IOFSET : Offset */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*       IERCOD : Error Code */
/*               = 0 : OK */
/*               = 1 : Pb of de-allocation of a zone allocated on table */
/*               = 2 : The system refuses the demand of de-allocation */

/*     COMMONS USED : */
/*     -------------- */

/*     REFERENCES CALLEDS : */
/*     -------------------- */
/*        MCRDELT */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     (Cf description in the heading of MCRDELT) */

/* > */
/* ***********************************************************************
 */
  
  /* Function Body */
  if (*iofset != 0) {
    AdvApp2Var_SysBase::mcrdelt_(&c__8, nbelem, xtablo, iofset, iercod);
  } else {
    *iercod = 0;
  }
  return 0 ;
} /* macrdr8_ */

//=======================================================================
//function : macrerr_
//purpose  : 
//=======================================================================
int macrerr_(intptr_t *,//iad,
	     intptr_t *)//nalloc)

{
  //integer c__1 = 1;
  /* Builtin functions */
  //integer /*do__fio(),*/;
  
  /* Fortran I/O blocks */
  //cilist io___1 = { 0, 6, 0, "(X,A,I9,A,I3)", 0 };

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       WRITING OF ADDRESS REMOVED IN ALLOCS . */

/*     KEYWORDS : */
/*     ----------- */
/*       ALLOC CONTROL */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*       IAD    : ADDRESS TO INFORM OF REMOVAL */
/*       NALLOC : NUMBER OF ALLOCATION */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       NONE */

/*     COMMONS USED : */
/*     -------------- */

/*     REFERENCES CALLED : */
/*     ------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */
  /*
  do__fio(&c__1, "*** ERREUR : Ecrasement de la memoire d'adresse ", 48L);
  do__fio(&c__1, (char *)&(*iad), (ftnlen)sizeof(long int));
  do__fio(&c__1, " sur l'allocation ", 18L);
  do__fio(&c__1, (char *)&(*nalloc), (ftnlen)sizeof(integer));
  */
  
  return 0 ;
} /* macrerr_ */


//=======================================================================
//function : macrgfl_
//purpose  : 
//=======================================================================
int macrgfl_(intptr_t *iadfld, 
	     intptr_t *iadflf, 
	     integer  *iphase, 
	     integer  *iznuti)

{
  /* Initialized data */
  
  /* original code used static integer ifois=0 which served as static
     initialization flag and was only used to call matrsym_() once; now
     this flag is not used as matrsym_() always returns 0 and has no
     useful contents
  */
  integer ifois = 1;
  
  char cbid[1];
  integer ibid, ienr;
  doublereal* t = 0;
  integer novfl = 0;
  intptr_t ioff,iadt;
  
  
  /* ***********************************************************************
   */
  
  /*     FUNCTION : */
  /*     ---------- */
  /*       IMPLEMENTATION OF TWO FLAGS START AND END OF THE ALLOCATED ZONE */
  /*       AND SETTING TO OVERFLOW OF THE USER SPACE IN PHASE OF PRODUCTION. */
  
  /*     KEYWORDS : */
  /*     ----------- */
  /*       ALLOCATION, CONTROL, EXCESS */
  
  /*     INPUT ARGUMENTS  : */
  /*     ------------------ */
  /*       IADFLD : ADDRESS OF THE START FLAG */
  /*       IADFLF : ADDRESS OF THE END FLAG */
  /*       IPHASE : TYPE OF SOFTWARE VERSION : */
  /*                0 = OFFICIAL VERSION  */
  /*                1 = PRODUCTION VERSION */
  /*       IZNUTI : SIZE OF THE USER ZONE IN OCTETS */
  
  /*     OUTPUT ARGUMENTS : */
  /*     ------------------ */
  /*       NONE */
  
  /*     COMMONS USED : */
  /*     ------------------ */
  
  /*     REFERENCES CALLED : */
  /*     ------------------- */
  /*       CRLOCT,MACRCHK */
  
  /*     DESCRIPTION/NOTES/LIMITATIONS : */
  /*     ------------------------------- */

  /* > */
  /* ***********************************************************************
   */
  
 

  /* ***********************************************************************
   */
  
  /*     FUNCTION : */
  /*     ---------- */
  /*        TABLE FOR MANAGEMENT OF DYNAMIC ALLOCATIONS OF MEMORY */
  
  /*     KEYWORDS : */
  /*     ----------- */
  /*        SYSTEM, MEMORY, ALLOCATION */
  
  /*     DEMSCRIPTION/NOTES/LIMITATIONS : */
  /*     ----------------------------------- */
  

  /* > */
  /* ***********************************************************************
   */
  /*   ICORE : TABLE OF EXISTING ALLOCATIONS, EACH HAVING : */
/*         1 : LEVEL OF PROTECTION (0=NOT PROTECTED, OTHER=PROTECTED) */
/*             (PROTECTED MEANS NOT DESTROYED BY CRRSET .) */
/*         2 : UNIT OF ALLOCATION */
/*         3 : NB OF ALLOCATED UNITS */
/*         4 : REFERENCE ADDRESS OF THE TABLE */
/*         5 : IOFSET */
/*         6 : STATIC ALLOCATION NUMBER */
/*         7 : Required allocation size */
/*         8 : address of the beginning of allocation */
/*         9 : Size of the USER ZONE */
/*        10 : ADDRESS of the START FLAG */
/*        11 : ADDRESS of the END FLAG */
/*        12 : Rank of creation of the allocation */

/*   NDIMCR : NB OF DATA OF EACH ALLOC IN ICORE */
/*   NCORE : NB OF CURRENT ALLOCS */
/*   LPROT : COMMUNICATION BETWEEN CRPROT AND MCRRQST, SET TO 0 BY MCRRQST */
/*   FLAG  : VALUE OF THE FLAG USED FOR EXCESSES */


  
  

  /* ----------------------------------------------------------------------*
   */
  

  if (ifois == 0) {
    matrsym_("NO_OVERFLOW", cbid, &novfl, &ibid, 11L, 1L);
    ifois = 1;
  }
  
  /*  CALCULATE THE ADDRESS OF T */
  mcrlocv_(t, &iadt);
  
  /* CALCULATE THE OFFSET */
  ioff = (*iadfld - iadt) / 8;
  
  /*  SET TO OVERFLOW OF THE USER ZONE IN CASE OF PRODUCTION VERSION */
  if (*iphase == 1 && novfl == 0) {
    ienr = *iznuti / 8;
    maoverf_(&ienr, &t[ioff + 1]);
  }
    
  /*  UPDATE THE START FLAG */
  t[ioff] = -134744073.;
  
  /*  FAKE CALL TO STOP THE DEBUGGER : */
  macrbrk_();
  
  /*  UPDATE THE START FLAG */
  ioff = (*iadflf - iadt) / 8;
  t[ioff] = -134744073.;
    
  /*  FAKE CALL TO STOP THE DEBUGGER : */
  macrbrk_();
  
  return 0 ;
} /* macrgfl_ */

//=======================================================================
//function : macrmsg_
//purpose  : 
//=======================================================================
int macrmsg_(const char *,//crout, 
	     integer *,//num, 
	     integer *it, 
	     doublereal *xt, 
	     const char *ct, 
	     ftnlen ,//crout_len,
	     ftnlen ct_len)

{
  
  /* Local variables */
  integer inum;
  char /*cfm[80],*/ cln[3];
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        MESSAGING OF ROUTINES OF ALLOCATION */

/*     KEYWORDS : */
/*     ----------- */
/*       ALLOC, MESSAGE */

/*     INPUT ARGUMENTSEE : */
/*     ------------------- */
/*       CROUT : NAME OF THE CALLING ROUTINE : MCRRQST, MCRDELT, MCRLIST 
*/
/*                ,CRINCR OR CRPROT */
/*       NUM :  MESSAGE NUMBER */
/*       IT : TABLE OF INTEGER DATA */
/*       XT : TABLE OF REAL DATA */
/*       CT : ------------------ CHARACTER */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       NONE */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ROUTINE FOR TEMPORARY USE, WAITING FOR THE 'NEW' MESSAGE */
/*    (STRIM 3.3 ?), TO MAKE THE ROUTINES OF ALLOC USABLE */
/*    IN STRIM T-M . */

/*   DEPENDING ON THE LANGUAGE, WRITING OF THE REQUIRED MESSAGE ON */
/*   UNIT IMP . */
/*   (REUSE OF SPECIFS OF VFORMA) */

/*   THE MESSAGE IS INITIALIZED AT 'MESSAGE MISSING', AND IT IS */
/*   REPLACED BY THE REQUIRED MESSAGE IF EXISTS. */
/* > */
/* ***********************************************************************
 */

/*  LOCAL : */

/* ----------------------------------------------------------------------*
 */
/*  FIND MESSAGE DEPENDING ON THE LANGUAGE , THE ROUTINE */
/*  AND THE MESSAGE NUMBER */

/*  READING OF THE LANGUAGE : */
    /* Parameter adjustments */
  ct -= ct_len;
  --xt;
  --it;
  
  /* Function Body */
  mamdlng_(cln, 3L);
  
/*  INUM : TYPE OF MESSAGE  : 0 AS TEXT, 1 1 INTEGER TO BE WRITTEN */
/*        -1 MESSAGE INEXISTING (1 INTEGER AND 1 CHAIN) */

  inum = -1;
/*
  if (__s__cmp(cln, "FRA", 3L, 3L) == 0) {
    __s__copy(cfm, "('   Il manque le message numero ',I5' pour le programm\
e de nom : ',A8)", 80L, 71L);
    if (__s__cmp(crout, "MCRLIST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(/,' Nombre d''allocation(s) de memoire effectu\
ee(s) : ',I6,/)", 80L, 62L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' Taille de l''allocation = ',I12)", 80L, 35L);
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' Taille totale allouee  = ',I12 /)", 80L, 36L);
      }
    } else if (__s__cmp(crout, "MCRDELT", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' L''allocation de memoire a detruire n''exist\
e pas  ')", 80L, 56L);
      } else if (*num == 2) {
	inum = 0;
	__s__copy(cfm, "(' Le systeme refuse une destruction d''allocat\
ion de memoire  ')", 80L, 65L);
      }
    } else if (__s__cmp(crout, "MCRRQST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' Le nombre maxi d''allocations de memoire est\
 atteint :',I6)", 80L, 62L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' Unite d''allocation invalide : ',I12)", 80L, 
		  40L);
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' Le systeme refuse une allocation de memoire \
de ',I12,' octets')", 80L, 66L);
      }
    } else if (__s__cmp(crout, "CRINCR", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' L''allocation de memoire a incrementer n''ex\
iste pas')", 80L, 57L);
      }
    } else if (__s__cmp(crout, "CRPROT", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' Le niveau de protection est invalide ( =< 0 \
) : ',I12)", 80L, 57L);
      }
    }
    
  } else if (__s__cmp(cln, "DEU", 3L, 3L) == 0) {
    __s__copy(cfm, "('   Es fehlt die Meldung Nummer ',I5,' fuer das Progra\
mm des Namens : ',A8)", 80L, 76L);
    if (__s__cmp(crout, "MCRLIST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(/,' Anzahl der ausgefuehrten dynamischen Anwei\
sung(en) : ',I6,/)", 80L, 65L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' Groesse der Zuweisung = ',I12)", 80L, 33L);
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' Gesamtgroesse der Zuweisung = ',I12,/)", 80L, 
		  41L);
      }
    } else if (__s__cmp(crout, "MCRDELT", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' Zu loeschende dynamische Zuweisung existiert\
 nicht !! ')", 80L, 59L);
      } else if (*num == 2) {
	inum = 0;
	__s__copy(cfm, "(' System verweigert Loeschung der dynamischen \
Zuweisung !!')", 80L, 61L);
      }
    } else if (__s__cmp(crout, "MCRRQST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' Hoechstzahl dynamischer Zuweisungen ist erre\
icht :',I6)", 80L, 58L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' Falsche Zuweisungseinheit : ',I12)", 80L, 37L)
	  ;
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' System verweigert dynamische Zuweisung von '\
,I12,' Bytes')", 80L, 61L);
      }
    } else if (__s__cmp(crout, "CRINCR", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' Zu inkrementierende dynamische Zuweisung exi\
stiert nicht !! ')", 80L, 65L);
      }
    } else if (__s__cmp(crout, "CRPROT", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' Sicherungsniveau ist nicht richtig ( =< 0 ) \
: ',I12)", 80L, 55L);
      }
    }
    
  } else {
    __s__copy(cfm, "('   Message number ',I5,' is missing '                \
            ,'for program named: ',A8)", 80L, 93L);
    if (__s__cmp(crout, "MCRLIST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(/,' number of memory allocations carried out: \
',I6,/)", 80L, 54L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' size of allocation = ',I12)", 80L, 30L);
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' total size allocated = ',I12,/)", 80L, 34L);
      }
    } else if (__s__cmp(crout, "MCRDELT", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' Memory allocation to delete does not exist !\
! ')", 80L, 51L);
      } else if (*num == 2) {
	inum = 0;
	__s__copy(cfm, "(' System refuses deletion of memory allocation\
 !! ')", 80L, 53L);
      }
    } else if (__s__cmp(crout, "MCRRQST", crout_len, 7L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' max number of memory allocations reached :',\
I6)", 80L, 50L);
      } else if (*num == 2) {
	inum = 1;
	__s__copy(cfm, "(' incorrect unit of allocation : ',I12)", 80L, 
		  40L);
      } else if (*num == 3) {
	inum = 1;
	__s__copy(cfm, "(' system refuses a memory allocation of ',I12,\
' bytes ')", 80L, 57L);
      }
    } else if (__s__cmp(crout, "CRINCR", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 0;
	__s__copy(cfm, "(' Memory allocation to increment does not exis\
t !! ')", 80L, 54L);
      }
    } else if (__s__cmp(crout, "CRPROT", crout_len, 6L) == 0) {
      if (*num == 1) {
	inum = 1;
	__s__copy(cfm, "(' level of protection is incorrect ( =< 0 ) : \
',I12)", 80L, 53L);
      }
    }
  }
  */
  /* ----------------------------------------------------------------------*
   */
  /*  iMPLEMENTATION OF WRITE , WITH OR WITHOUT DATA : */
  
  if (inum == 0) {
  } else if (inum == 1) {
    /*
    do__fio(&c__1, (char *)&it[1], (ftnlen)sizeof(integer));
    */
  } else {
    /*  MESSAGE DOES NOT EXIST ... */
    /*
    do__fio(&c__1, (char *)&(*num), (ftnlen)sizeof(integer));
    do__fio(&c__1, crout, crout_len);
    */
  }
  
  return 0;
} /* macrmsg_ */
//=======================================================================
//function : macrstw_
//purpose  : 
//=======================================================================
int macrstw_(intptr_t *,//iadfld, 
	     intptr_t *,//iadflf, 
	     integer *)//nalloc)

{
  return 0 ;
} /* macrstw_ */

//=======================================================================
//function : madbtbk_
//purpose  : 
//=======================================================================
int madbtbk_(integer *indice)
{
  *indice = 0;
  return 0 ;
} /* madbtbk_ */

//=======================================================================
//function : AdvApp2Var_SysBase::maermsg_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::maermsg_(const char *,//cnompg, 
				 integer *,//icoder, 
				 ftnlen )//cnompg_len)

{
  return 0 ;
} /* maermsg_ */

//=======================================================================
//function : magtlog_
//purpose  : 
//=======================================================================
int magtlog_(const char *cnmlog, 
	     const char *,//chaine, 
	     integer *long__, 
	     integer *iercod, 
	     ftnlen cnmlog_len, 
	     ftnlen )//chaine_len)

{
 
  /* Local variables */
  char cbid[255];
  integer ibid, ier;
  

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        RETURN TRANSLATION OF "NAME LOGIC STRIM" IN */
/*        "INTERNAL SYNTAX" CORRESPONDING TO "PLACE OF RANKING" */

/*     KEYWORDS : */
/*     ----------- */
/*        NOM LOGIQUE STRIM , TRADUCTION */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        CNMLOG : NAME OF "NAME LOGIC STRIM" TO TRANSLATE */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*        CHAINE : ADDRESS OF "PLACE OF RANKING" */
/*        LONG   : USEFUL LENGTH OF "PLACE OF RANKING" */
/*        IERCOD : ERROR CODE */
/*        IERCOD = 0 : OK */
/*        IERCOD = 5 : PLACE OF RANKING CORRESPONDING TO INEXISTING LOGIC NAME */
        
/*        IERCOD = 6 : TRANSLATION TOO LONG FOR THE 'CHAIN' VARIABLE */
/*        IERCOD = 7 : CRITICAL ERROR */

/*     COMMONS USED   : */
/*     ---------------- */
/*        NONE */

/*     REFERENCES CALLED   : */
/*     --------------------- */
/*        GNMLOG, MACHDIM */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */

/*        SPECIFIC SGI ROUTINE */

/*        IN ALL CASES WHEN IERCOD IS >0, NO RESULT IS RETURNED*/
/*        NOTION OF  "USER SYNTAX' AND "INTERNAL SYNTAX" */
/*        --------------------------------------------------- */

/*       THE "USER SYNTAX" IS THE SYNTAX WHERE THE USER*/
/*       VISUALIZES OR INDICATES THE FILE OR DIRECTORY NAME */
/*       DURING A SESSION OF STRIM100 */

/*        "INTERNAL SYNTAX" IS SYNTAX USED TO CARRY OUT */
/*        OPERATIONS OF FILE PROCESSING INSIDE THE CODE */
/*        (OPEN,INQUIRE,...ETC) */

/* > */
/* ***********************************************************************
 */
/*              DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*              PROCESSING */
/* ***********************************************************************
 */

  *long__ = 0;
  *iercod = 0;
  
  /* CONTROL OF EXISTENCE OF THE LOGIC NAME */
  
  matrlog_(cnmlog, cbid, &ibid, &ier, cnmlog_len, 255L);
  if (ier == 1) {
    goto L9500;
  }
  if (ier == 2) {
    goto L9700;
  }
  
  /* CONTROL OF THE LENGTH OF CHAIN */
  
  if (ibid > __i__len()/*chaine, chaine_len)*/) {
    goto L9600;
  }
  
  //__s__copy(chaine, cbid, chaine_len, ibid);
  *long__ = ibid;
  
  goto L9999;
  
  /* ***********************************************************************
   */
  /*              ERROR PROCESSING */
  /* ***********************************************************************
   */
  
 L9500:
  *iercod = 5;
  //__s__copy(chaine, " ", chaine_len, 1L);
  goto L9999;
  
 L9600:
  *iercod = 6;
  //__s__copy(chaine, " ", chaine_len, 1L);
  goto L9999;
  
 L9700:
  *iercod = 7;
  //__s__copy(chaine, " ", chaine_len, 1L);
  
  /* ***********************************************************************
   */
  /*              RETURN TO THE CALLING PROGRAM */
  /* ***********************************************************************
   */
  
 L9999:
  return 0;
} /* magtlog_ */

//=======================================================================
//function : mainial_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mainial_()
{
  mcrgene_.ncore = 0;
  mcrgene_.lprot = 0;
  return 0 ;
} /* mainial_ */

//=======================================================================
//function : AdvApp2Var_SysBase::maitbr8_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::maitbr8_(integer *itaill, 
				 doublereal *xtab, 
				 doublereal *xval) 

{
  integer c__504 = 504;

  /* Initialized data */

  doublereal buff0[63] = { 
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0. 
    };
  
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  integer i__;
  doublereal buffx[63];
  integer nbfois, noffst, nreste, nufois;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       INITIALIZATION TO A GIVEN VALUE OF A TABLE OF REAL *8 */

/*     KEYWORDS : */
/*     ----------- */
/*       MANIPULATIONS, MEMORY, INITIALIZATION, DOUBLE-PRECISION */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*        ITAILL : SIZE OF THE TABLE */
/*        XTAB   : TABLE TO INITIALIZE WITH XVAL */
/*        XVAL   : VALUE TO SET IN XTAB(FROM 1 TO ITAILL) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------ */
/*        XTAB   : INITIALIZED TABLE */

/*     COMMONS USED : */
/*     -------------- */

/*     REFERENCES CALLED : */
/*     ------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ONE CALLS MCRFILL WHICH MOVES BY PACKS OF 63 REALS */

/*   THE INITIAL PACK IS BUFF0 INITIATED BY DATA IF THE VALUE IS 0 */
/*   OR OTHERWISE BUFFX INITIATED BY XVAL (LOOP). */


/*   PORTABILITY : YES */
/*   ACCESS : FREE */


/* > */
/* ***********************************************************************
 */

  
  /* Parameter adjustments */
  --xtab;
  
  /* Function Body */
  
  /* ----------------------------------------------------------------------*
   */
  
  nbfois = *itaill / 63;
  noffst = nbfois * 63;
  nreste = *itaill - noffst;
  
  if (*xval == 0.) {
    if (nbfois >= 1) {
      i__1 = nbfois;
      for (nufois = 1; nufois <= i__1; ++nufois) {
	AdvApp2Var_SysBase::mcrfill_(&c__504, buff0, &xtab[(nufois - 1) * 63 + 1]);
	/* L1000: */
      }
    }
    
    if (nreste >= 1) {
      i__1 = nreste << 3;
      AdvApp2Var_SysBase::mcrfill_(&i__1, buff0, &xtab[noffst + 1]);
    }
  } else {
    for (i__ = 1; i__ <= 63; ++i__) {
      buffx[i__ - 1] = *xval;
      /* L2000: */
    }
    if (nbfois >= 1) {
      i__1 = nbfois;
      for (nufois = 1; nufois <= i__1; ++nufois) {
	AdvApp2Var_SysBase::mcrfill_(&c__504, buffx, &xtab[(nufois - 1) * 63 + 1]);
	/* L3000: */
      }
    }
    
    if (nreste >= 1) {
      i__1 = nreste << 3;
      AdvApp2Var_SysBase::mcrfill_(&i__1, buffx, &xtab[noffst + 1]);
    }
  }
  
  /* ----------------------------------------------------------------------*
   */
  
  return 0;
} /* maitbr8_ */

//=======================================================================
//function : mamdlng_
//purpose  : 
//=======================================================================
int mamdlng_(char *,//cmdlng, 
	     ftnlen )//cmdlng_len)

{
 

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*   RETURN THE CURRENT LANGUAGE */

/*     KEYWORDS : */
/*     ----------- */
/*   MANAGEMENT, CONFIGURATION, LANGUAGE, READING */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       CMDLNG : LANGUAGE */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*       NONE */

/*     COMMONS USED : */
/*     ------------------ */
/*       MACETAT */

/*     REFERENCES CALLED : */
/*     --------------------- */
/*       NONE */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       RIGHT OF USAGE : ANY APPLICATION */

/*       ATTENTION : THIS ROUTINE DEPENDS ON PRELIMINARY INITIALISATION */
/*       ----------  WITH AMDGEN. */
/*                   SO IT IS ENOUGH TO PROVIDE THAT THIS INIT IS */
/*                   CORRECTLY IMPLEMENTED IN THE RESPECTIVE PROGRAMS */
/* > */
/* ***********************************************************************
 */


/*     INCLUDE MACETAT */
/* < */

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        CONTAINS INFORMATIONS ABOUT THE COMPOSITION OF */
/*        THE EXECUTABLE AND ITS ENVIRONMENT : */
/*        - LANGUAGES */
/*        - PRESENT APPLICATIONS */
/*        - AUTHORIZED TYPES OF ENTITIES (NON USED) */
/*        AND INFORMATION DESCRIBING THE CURRENT STATE : */
/*        - CURRENT APPLICATION */
/*        - MODE OF USAGE (NOT USED) */

/*     KEYWORDS : */
/*     ----------- */
/*        APPLICATION, LANGUAGE */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     A) CHLANG*4 : LIST OF POSSIBLE VALUES OF THE LANGUAGE : */
/*                   'FRA ','DEU ','ENG ' */

/*        CHL10N*4 : LIST OF POSSIBLE VALUES OF THE LOCALIZATION : */
/*                   'FRA ','DEU ','ENG ', 'JIS ' */

/*     B) CHCOUR*4, CHPREC*4, CHSUIV*4 : CURRENT, PREVIOUS AND NEXT APPLICATION */

/*     C) CHMODE*4 : CURRENT MODE (NOT USED) */

/*     D) CHPRES*2 (1:NBRMOD) : LIST OF APPLICATIONS TAKEN INTO ACCOUNT */

/*       Rang ! Code interne  !   Application */
/*       ---------------------------------------------------------- */
/*        1   !   CD          !   Modeling 2D */
/*        2   !   CA          !   Modeling 2D by learning */
/*        3   !   CP          !   Parameterized 2D modelization */
/*        4   !   PC          !   Rheological 2D modelization */
/*        5   !   CU          !   Milling 2 Axes 1/2 */
/*        6   !   CT          !   Turning */
/*        7   !   TS          !   3D surface modeling */
/*        8   !   TV          !   3D volume modeling */
/*        9   !   MC          !   Surface Meshing */
/*        10  !   MV          !   Volume Meshing */
/*        11  !   TU          !   Machining by 3 axes */
/*        12  !   T5          !   Machining by 3-5 axes */
/*        13  !   TR          !   Machinning by 5 axes of regular surfaces */
/*        14  !   IG          !   Interface IGES */
/*        15  !   ST          !   Interface SET */
/*        16  !   VD          !   Interface VDA */
/*        17  !   IM          !   Interface of modeling */
/*        18  !   GA          !   Generator APT/IFAPT */
/*        19  !   GC          !   Generator COMPACT II */
/*        20  !   GP          !   Generator PROMO */
/*        21  !   TN          !   Machining by numerical copying */
/*        22  !   GM          !   Management of models */
/*        23  !   GT          !   Management of trace */
/*       ---------------------------------------------------------- */



/* > */
/* ***********************************************************************
 */

/*     NUMBER OF APPLICATIONS TAKEN INTO ACCOUNT */


/*     NUMBER OF ENTITY TYPES MANAGED BY STRIM 100 */
  //__s__copy(cmdlng, macetat_.chlang, cmdlng_len, 4L);
  
  return 0 ;
} /* mamdlng_ */

//=======================================================================
//function : maostrb_
//purpose  : 
//=======================================================================
int maostrb_()
{
  return 0 ;
} /* maostrb_ */

//=======================================================================
//function : maostrd_
//purpose  : 
//=======================================================================
int maostrd_()
{
  integer imod;
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       REFINE TRACE-BACK IN PRODUCTION PHASE */

/*     KEYWORDS : */
/*     ----------- */
/*       FUNCTION, SYSTEM, TRACE-BACK, REFINING, DEBUG */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*       NONE */

/*     OUTPUT ARGUMENTS E : */
/*     -------------------- */
/*        NONE */

/*     COMMONS USED : */
/*     -------------- */
/*        NONE */

/*     REFERENCES CALLED : */
/*     ------------------- */
/*       MADBTBK */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       THIS ROUTINE SHOULD BE CALLED TO REFINE */
/*       TRACE-BACK IN PRODUCTION PHASE AND LEAVE TO TESTERS THE  */
/*       POSSIBILITY TO GET TRACE-BACK IN */
/*       CLIENT VERSIONS IF ONE OF THE FOLLOWING CONDITIONS IS */
/*       VERIFIED : */
/*       - EXISTENCE OF SYMBOL 'STRMTRBK' */
/*       - EXISTENCE OF FILE 'STRMINIT:STRMTRBK.DAT' */


/* > */
/* ***********************************************************************
 */
  madbtbk_(&imod);
  if (imod == 1) {
    maostrb_();
  }
  return 0 ;
} /* maostrd_ */

//=======================================================================
//function : maoverf_
//purpose  : 
//=======================================================================
int maoverf_(integer *nbentr, 
	     doublereal *dtable) 

{
  /* Initialized data */
  
  integer ifois = 0;
  
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  integer ibid;
  doublereal buff[63];
  integer ioct, indic, nrest, icompt;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       Initialisation in overflow of a tableau with DOUBLE PRECISION */

/*     KEYWORDS : */
/*     ----------- */
/*       MANIPULATION, MEMORY, INITIALISATION, OVERFLOW */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*       NBENTR : Number of entries in the table */

/*     OUTPUT ARGUMENTS : */
/*     ------------------ */
/*       DATBLE : Table double precision initialized in overflow */

/*     COMMONS USED : */
/*     ------------------ */
/*       R8OVR contained in the include MAOVPAR.INC */

/*     REFERENCES CALLED : */
/*     --------------------- */
/*       MCRFILL */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       1) Doc. programmer : */

/*       This routine initialized to positive overflow a table with */
/*       DOUBLE PRECISION. */

/*       Other types of tables (INTEGER*2, INTEGER, REAL, ...) */
/*       are not managed by the routine. */

/*       It is usable in phase of developpement to detect the */
/*       errors of initialization. */

/*       In official version, these calls will be inactive. */

/*       ACCESs : Agreed with AC. */

/*       The routine does not return error code. */

/*       Argument NBELEM should be positive. */
/*       If it is negative or null, display message "MAOVERF : NBELEM = */
/*       valeur_de_NBELEM" and a Trace Back by the call of routine MAOSTRB. */


/*       2) Doc. designer  : */

/*                  The idea is to minimize the number of calls */
/*                to the routine of transfer of numeric zones, */
/*   ----------   for the reason of performance. */
/*  !  buffer  !    For this a table of NLONGR */
/*  !__________!  DOUBLE PRECISIONs is reserved. This buffer is initialized by */
/*  <---------->  the instruction DATA. The overflow is accessed in a  */
/*    NLONGR*8    specific COMMON not by a routine as */
/*                the initialisation is done by DATA. */

/*                * If NBENTR<NLONGR, a part of the buffer is transfered*/
/*     DTABLE     in DTABLE. */
/*   __________ */
/*  !  amorce  !  * Otherwise, the entire buffer is transfered in DTABLE. */
/*  !__________!  This initiates it. Then a loop is execute, which at each  
*/
/*  !  temps 1 !  iteration transfers the part of the already initialized table */
/*  !__________!  in the one that was not yet initialized. */
/*  !          !  The size of the zone transfered by each call to MCRFILL 
*/
/*  !  temps 2 !  is NLONGR*2**(numero_de_l'iteration). When  
*/
/*  !          !  the size of the table to be initialized is */
/*  !__________!  less than the already initialized size, the loop is */
/*  !          !  abandoned and thev last transfer is carried out to */
/*  !          !  initialize the remaining table, except for the case when the size */
/*  !          !  of the table is of type NLONGR*2**K. */
/*  !  temps 3 ! */
/*  !          !  * NLONGR will be equal to 19200. */
/*  !          ! */
/*  !          ! */
/*  !__________! */
/*  !  reste   ! */
/*  !__________! */


/* > */
/* ***********************************************************************
 */

/* Inclusion of MAOVPAR.INC */

/*      CONSTANTS */
/*     INCLUDE MAOVPAR */
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       DEFINES SPECIFIC LIMITED VALUES. */

/*     KEYWORDS : */
/*     ----------- */
/*       SYSTEM, LIMITS, VALUES, SPECIFIC */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     *** THEY CAN'T BE REMOVED DURING EXECUTION. */

/*     *** THE VALUES OF UNDERFLOW AND OVERFLOW CAN'T BE  */
/*     DEFINED IN DECIMAL VALUES (ERROR OF COMPILATION D_FLOAT) */
/*     THEY ARE DEFINED AS HEXADECIMAL VALUES */


/* > */
/* ***********************************************************************
 */


/*    DECLARATION OF THE COMMON FOR NUMERIC TYPES */


/*    DECLARATION OF THE COMMON FOR CHARACTER TYPES*/



/*      LOCAL VARIABLES */

/*      TABLES */

/*      DATAS */
    /* Parameter adjustments */
  --dtable;
  
  /* Function Body */
  
  /* vJMB R8OVR IS NOT YET initialized, so impossible to use DATA
   */
  /*         DATA BUFF / NLONGR * R8OVR / */
  
  /*    init of BUFF is done only once */
  
  if (ifois == 0) {
    for (icompt = 1; icompt <= 63; ++icompt) {
      buff[icompt - 1] = maovpar_.r8ovr;
      /* L20: */
    }
    ifois = 1;
  }
  
  /* ^JMB */
  /* Exception */
  if (*nbentr < 63) {
    nrest = *nbentr << 3;
    AdvApp2Var_SysBase::mcrfill_(&nrest, buff, &dtable[1]);
  } else {
    
    /* Start & initialization */
    ioct = 504;
    AdvApp2Var_SysBase::mcrfill_(&ioct, buff, &dtable[1]);
    indic = 63;
    
    /* Loop. The upper limit is the integer value of the logarithm of base 2
     */
    /* of NBENTR/NLONGR. */
    i__1 = (integer) (log((real) (*nbentr) / (float)63.) / log((float)2.))
      ;
    for (ibid = 1; ibid <= i__1; ++ibid) {
      
      AdvApp2Var_SysBase::mcrfill_(&ioct, &dtable[1], &dtable[indic + 1]);
      ioct += ioct;
      indic += indic;
      
      /* L10: */
    }
    
    nrest = ( *nbentr - indic ) << 3;
    
    if (nrest > 0) {
      AdvApp2Var_SysBase::mcrfill_(&nrest, &dtable[1], &dtable[indic + 1]);
    }
    
  }
  return 0 ;
} /* maoverf_ */

//=======================================================================
//function : AdvApp2Var_SysBase::maovsr8_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::maovsr8_(integer *ivalcs) 
{
  *ivalcs = maovpar_.r8ncs;
  return 0 ;
} /* maovsr8_ */

//=======================================================================
//function : matrlog_
//purpose  : 
//=======================================================================
int matrlog_(const char *,//cnmlog, 
	     const char *,//chaine, 
	     integer *length, 
	     integer *iercod, 
	     ftnlen ,//cnmlog_len, 
	     ftnlen )//chaine_len)

{
  *iercod = 1;
  *length = 0;
  
  return 0 ;
} /* matrlog_ */

//=======================================================================
//function : matrsym_
//purpose  : 
//=======================================================================
int matrsym_(const char *cnmsym, 
	     const char *,//chaine, 
	     integer *length, 
	     integer *iercod, 
	     ftnlen cnmsym_len, 
	     ftnlen )//chaine_len)

{
  /* Local variables */
  char chainx[255];

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       RETURN THE VALUE OF A SYMBOL DEFINED DURING THE */
/*       INITIALISATION OF A USER */

/*     KEYWORDS : */
/*     ----------- */
/*       TRANSLATION, SYMBOL */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       CNMSYM : NAME OF THE SYMBOL */

/*     OUTPUT ARGUMENTS : */
/*     ------------------ */
/*       CHAINE : TRANSLATION OF THE SYMBOL */
/*       LENGTH : USEFUL LENGTH OF THE CHAIN */
/*       IERCOD : ERROR CODE */
/*              = 0 : OK */
/*              = 1 : INEXISTING SYMBOL */
/*              = 2 : OTHER ERROR */

/*     COMMONS USED : */
/*     ------------------ */
/*       NONE */

/*     REFERENCES CALLED : */
/*     --------------------- */
/*       LIB$GET_SYMBOL,MACHDIM */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       - THIS ROUTINE IS VAX SPECIFIC */
/*       - IN CASE OF ERROR (IERCOD>0), CHAIN = ' ' AND LENGTH = 0 */
/*       - IF THE INPUT VARIABLE CNMSYM IS EMPTY, THE ROUTINE RETURNS IERCOD=1*/
/* > */
/* ***********************************************************************
 */


/* SGI...v */
  
  /* SGI  CALL MAGTLOG (CNMSYM,CHAINE,LENGTH,IERCOD) */
  magtlog_(cnmsym, chainx, length, iercod, cnmsym_len, 255L);
  /* SO...v */
  if (*iercod == 5) {
    *iercod = 1;
  }
  /* SO...^ */
  if (*iercod >= 2) {
    *iercod = 2;
  }
  //if (__s__cmp(chainx, "NONE", 255L, 4L) == 0) {
  if (__s__cmp() == 0) {
    //__s__copy(chainx, " ", 255L, 1L);
    *length = 0;
  }
  //__s__copy(chaine, chainx, chaine_len, 255L);
  /* SGI...^ */
  
  
  /* ***********************************************************************
   */
  /*     ERROR PROCESSING */
  /* ***********************************************************************
   */
  
  
  /* L9999: */
  return 0;
} /* matrsym_ */

//=======================================================================
//function : mcrcomm_
//purpose  : 
//=======================================================================
int mcrcomm_(integer *kop, 
	     integer *noct, 
	     intptr_t *iadr, 
	     integer *ier)

{
  /* Initialized data */
  
  integer ntab = 0;
  
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  intptr_t ideb;
  doublereal dtab[32000];
  intptr_t itab[160]	/* was [4][40] */;
  intptr_t ipre;
  integer i__, j, k;
  

/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*        DYNAMIC ALLOCATION ON COMMON */

/*     KEYWORDS : */
/*     ----------- */
/*        . ALLOCDYNAMIQUE, MEMORY, COMMON, ALLOC */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        KOP    : (1,2) = (ALLOCATION,DESTRUCTION) */
/*        NOCT   : NUMBER OF OCTETS */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*        IADR   : ADDRESS IN MEMORY OF THE FIRST OCTET */
/*        *      : */
/*        *      : */
/*        IERCOD : ERROR CODE */

/*        IERCOD = 0 : OK */
/*        IERCOD > 0 : CRITICAL ERROR  */
/*        IERCOD < 0 : WARNING */
/*        IERCOD = 1 : ERROR DESCRIPTION */
/*        IERCOD = 2 : ERROR DESCRIPTION */

/*     COMMONS USED   : */
/*     ---------------- */

/*    CRGEN2 */

/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     Type  Name */
/*           MCRLOCV */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ATTENTION .... ITAB ARE NTAB NOT SAVED BETWEEN 2 CALLS.. 
*/

/* > */
/* ***********************************************************************
 */

/* JPF  PARAMETER ( MAXNUM = 40 , MAXCOM = 500 * 1024 ) */

/*  ITAB : TABLE OF MANAGEMENT OF DTAB, ALLOCATED MEMORY ZONE . */
/*  NTAB : NUMBER OF COMPLETED ALLOCATIONS. */
/*     FORMAT OF ITAB : NUMBER OF ALLOCATED REAL*8, ADDRESS OF THE 1ST REAL*8 
*/
/*                      , NOCT , VIRTUAL ADDRESS */

/* PP      COMMON / CRGEN2 / DTAB */


/* ----------------------------------------------------------------------*
 */

  *ier = 0;
  
  /*  ALLOCATION : FIND A HOLE */
  
  if (*kop == 1) {
    *iadr = 0;
    if (*noct < 1) {
      *ier = 1;
      goto L9900;
    }
    if (ntab >= 40) {
      *ier = 2;
      goto L9900;
    }
    
    i__1 = ntab + 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
      if (i__ <= 1) {
	ipre = 1;
      } else {
	ipre = itab[((i__ - 1) << 2) - 3] + itab[((i__ - 1) << 2) - 4];
      }
      if (i__ <= ntab) {
	ideb = itab[(i__ << 2) - 3];
      } else {
	ideb = 32001;
      }
      if ((ideb - ipre) << 3 >= *noct) {
	/*  A HOLE WAS FOUND */
	i__2 = i__;
	for (j = ntab; j >= i__2; --j) {
	  for (k = 1; k <= 4; ++k) {
	    itab[k + ((j + 1) << 2) - 5] = itab[k + (j << 2) - 5];
	    /* L1003: */
	  }
	  /* L1002: */
	}
	++ntab;
	itab[(i__ << 2) - 4] = *noct / 8 + 1;
	itab[(i__ << 2) - 3] = ipre;
	itab[(i__ << 2) - 2] = *noct;
	mcrlocv_(&dtab[ipre - 1], iadr);
	itab[(i__ << 2) - 1] = *iadr;
	goto L9900;
      }
      /* L1001: */
    }
    
    /*  NO HOLE */
    
    *ier = 3;
    goto L9900;
    
    /* ----------------------------------- */
    /*  DESTRUCTION OF THE ALLOCATION NUM : */
    
  } else {
    i__1 = ntab;
    for (i__ = 1; i__ <= i__1; ++i__) {
      if (*noct != itab[(i__ << 2) - 2]) {
	goto L2001;
      }
      if (*iadr != itab[(i__ << 2) - 1]) {
	goto L2001;
      }
      /*  THE ALLOCATION TO BE REMOVED WAS FOUND */
      i__2 = ntab;
      for (j = i__ + 1; j <= i__2; ++j) {
	for (k = 1; k <= 4; ++k) {
	  itab[k + ((j - 1) << 2) - 5] = itab[k + (j << 2) - 5];
	  /* L2003: */
	}
	/* L2002: */
      }
      --ntab;
      goto L9900;
    L2001:
      ;
    }
    
    /*        THE ALLOCATION DOES NOT EXIST */
    
    *ier = 4;
    /* PP         GOTO 9900 */
  }
  
 L9900:
  return 0;
} /* mcrcomm_ */

//=======================================================================
//function : AdvApp2Var_SysBase::mcrdelt_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrdelt_(integer *iunit, 
				 integer *isize, 
				 void *t, 
				 intptr_t *iofset, 
				 integer *iercod)

{
  integer ibid;
  doublereal xbid;
  integer noct, iver, ksys, i__, n, nrang, 
  ibyte, ier;
  intptr_t iadfd,  iadff, iaddr, loc; /* Les adrresses en long*/
  integer kop;
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        DESTRUCTION OF A DYNAMIC ALLOCATION */

/*     KEYWORDS : */
/*     ----------- */
/*        SYSTEM, ALLOCATION, MEMORY, DESTRUCTION */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        IUNIT  : NUMBER OF OCTETS OF THE ALLOCATION UNIT */
/*        ISIZE  : NUMBER OF UNITS REQUIRED */
/*        T      : REFERENCE ADDRESS */
/*        IOFSET : OFFSET */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        IERCOD : ERROR CODE */
/*               = 0 : OK */
/*               = 1 : PB OF DE-ALLOCATION OF A ZONE ALLOCATED IN COMMON */
/*               = 2 : THE SYSTEM REFUSES TO DEMAND DE-ALLOCATION */
/*               = 3 : THE ALLOCATION TO BE DESTROYED DOES NOT EXIST. */

/*     COMMONS USED   : */
/*     ---------------- */


/*     REFERENCES CALLED   : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     1) UTILISATEUR */
/*        ----------- */

/*       MCRDELT FREES ALLOCATED MEMORY ZONE */
/*       BY ROUTINE MCRRQST (OR CRINCR) */

/*       THE MEANING OF ARGUMENTS IS THE SAME AS MCRRQST */

/* *** ATTENTION : */
/*     ----------- */
/*     IERCOD=2 : CASE WHEN THE SYSTEM CANNOT FREE THE ALLOCATED MEMORY, */
/*     THE FOLLOWING MESSAGE APPEARS SYSTEMATICALLY ON CONSOLE ALPHA : */
/*     "THe system refuseS destruction of memory allocation" */

/*     IERCOD=3 CORRESPONDS TO THE CASE WHEN THE ARGUMENTS ARE NOT CORRECT */
/*     (THEY DO NOT ALLOW TO RECOGNIZE THE ALLOCATION IN THE TABLE) 
*/

/*     When the allocation is destroyed, the corresponding IOFSET is set to */
/*     2 147 483 647. So, if one gets access to the table via IOFSET, there is  */
/*     a trap. This allows to check that the freed memory zone is not usede. This verification is */
/*     valid only if the same sub-program uses and destroys the allocation. */

/* > */
/* ***********************************************************************
 */

/* COMMON OF PARAMETERS */

/* COMMON OF STATISTICS */
/*     INCLUDE MCRGENE */

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       TABLE OF MANAGEMENT OF DYNAMIC ALLOCATIONS IN MEMORY */

/*     KEYWORS : */
/*     ----------- */
/*       SYSTEM, MEMORY, ALLOCATION */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ***********************************************************************
 */
/*   ICORE : TABLE OF EXISTING ALLOCATIONS, EACH HAVING : */
/*         1 : LEVEL OF PROTECTION (0=NOT PROTECTED, OTHER=PROTECTED) */
/*             (PROTECTED MEANS NOT DESTROYED BY CRRSET .) */
/*         2 : UNIT OF ALLOCATION */
/*         3 : NB OF ALLOCATED UNITS */
/*         4 : REFERENCE ADDRESS OF THE TABLE */
/*         5 : IOFSET */
/*         6 : STATIC ALLOCATION NUMBER */
/*         7 : Required allocation size */
/*         8 : address of the beginning of allocation */
/*         9 : Size of the USER ZONE */
/*        10 : ADDRESS of the START FLAG */
/*        11 : ADDRESS of the END FLAG */
/*        12 : Rank of creation of the allocation */

/*   NDIMCR : NB OF DATA OF EACH ALLOC IN ICORE */
/*   NCORE : NB OF CURRENT ALLOCS */
/*   LPROT : COMMUNICATION BETWEEN CRPROT AND MCRRQST, SET TO 0 BY MCRRQST */
/*   FLAG  : VALUE OF THE FLAG USED FOR EXCESSES */



/* ----------------------------------------------------------------------*
 */


/*     20-10-86 : BF ; INITIAL VERSION  */


/*     NRQST : NUMBER OF ALLOCATIONS  */
/*     NDELT : NUMBER OF LIBERATIONS */
/*     NBYTE : TOTAL NUMBER OF OCTETS OF ALLOCATIONS */
/*     MBYTE : MAX NUMBER OF OCTETS */

    /* Function Body */
    *iercod = 0;

/* SEARCH IN MCRGENE */

    n = -1;
    mcrlocv_(t, &loc);

    for (i__ = mcrgene_.ncore - 1; i__ >= 0; --i__) {
	if (*iunit == mcrgene_.icore[i__].unit && *isize == 
		mcrgene_.icore[i__].reqsize && loc == mcrgene_.icore[i__].loc
        && *iofset == mcrgene_.icore[i__].offset) {
	    n = i__;
	    goto L1100;
	}
/* L1001: */
    }
L1100:

/* IF THE ALLOCATION DOES NOT EXIST, LEAVE */

    if (n < 0) {
	goto L9003;
    }

/* ALLOCATION RECOGNIZED : RETURN OTHER INFOS */

    ksys = mcrgene_.icore[n].alloctype;
    ibyte = mcrgene_.icore[n].size;
    iaddr = mcrgene_.icore[n].addr;
    iadfd = mcrgene_.icore[n].startaddr;
    iadff = mcrgene_.icore[n].endaddr;
    nrang = mcrgene_.icore[n].rank;

/*     Control of flags */

    madbtbk_(&iver);
    if (iver == 1) {
	macrchk_();
    }

    if (ksys == static_allocation) {
/* DE-ALLOCATION ON COMMON */
	kop = 2;
	mcrcomm_(&kop, &ibyte, &iaddr, &ier);
	if (ier != 0) {
	    goto L9001;
	}
    } else {
/* DE-ALLOCATION SYSTEM */
	mcrfree_(&ibyte, iaddr, &ier);
	if (ier != 0) {
	    goto L9002;
	}
    }

/* CALL ALLOWING TO CANCEL AUTOMATIC WATCH BY THE DEBUGGER */

    macrclw_(&iadfd, &iadff, &nrang);

/* UPDATE OF STATISTICS */
    ++mcrstac_.ndelt[ksys];
    mcrstac_.nbyte[ksys] -= mcrgene_.icore[n].unit * 
	    mcrgene_.icore[n].reqsize;

/* REMOVAL OF PARAMETERS IN MCRGENE */
    if (n < MAX_ALLOC_NB - 1) {
        noct = (mcrgene_.ncore - (n + 1)) * sizeof(mcrgene_.icore[0]);
	AdvApp2Var_SysBase::mcrfill_(&noct, 
				     &mcrgene_.icore[n + 1], 
				     &mcrgene_.icore[n]);
    }
    --mcrgene_.ncore;

/* *** Set to overflow of IOFSET */
    {
       /* nested scope needed to avoid gcc compilation error crossing
          initialization with goto*/
       /* assign max positive integer to *iofset */
       const size_t shift = sizeof (*iofset) * 8 - 1;
       *iofset = (uintptr_t(1) << shift) - 1 /*2147483647 for 32bit*/;
    }
    goto L9900;

/* ----------------------------------------------------------------------*
 */
/*     ERROR PROCESSING */

L9001:
/*  REFUSE DE-ALLOCATION BY ROUTINE 'MCRCOMM' (ALLOC DS COMMON) */
    *iercod = 1;
    AdvApp2Var_SysBase::maermsg_("MCRDELT", iercod, 7L);
    maostrd_();
    goto L9900;

/*  REFUSE DE-ALLOCATION BY THE SYSTEM */
L9002:
    *iercod = 2;
    AdvApp2Var_SysBase::maermsg_("MCRDELT", iercod, 7L);
    macrmsg_("MCRDELT", iercod, &ibid, &xbid, " ", 7L, 1L);
    maostrd_();
    goto L9900;

/* ALLOCATION DOES NOT EXIST */
L9003:
    *iercod = 3;
    AdvApp2Var_SysBase::maermsg_("MCRDELT", iercod, 7L);
    maostrd_();
    goto L9900;

L9900:

 return 0   ;

} /* mcrdelt_ */


/*
C*********************************************************************
C
C     FUNCTION :
C     ----------
C        Transfer a memory zone in another by managing intersections
C
C     KEYWORDS :
C     -----------
C        MANIPULATION, MEMORY, TRANSFER, CHARACTER
C
C     INPUT ARGUMENTS :
C     -----------------
C        nb_car    : integer*4  number of characters to transfer.
C        source    : source memory zone.
C             
C     OUTPUT ARGUMENTS  :
C     -------------------
C        dest      : zone memory destination.
C
C     COMMONS USED :
C     ----------------
C
C     REFERENCES CALLED :
C     -------------------
C
C     DEMSCRIPTION/NOTES/LIMITATIONS :
C     -----------------------------------
C        Routine portable UNIX (SGI, ULTRIX, BULL)
C

C>
C**********************************************************************
*/

//=======================================================================
//function : AdvApp2Var_SysBase::mcrfill_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrfill_(integer *size, 
				 void *tin, 
				 void *tout)

{
  char *jmin=static_cast<char*> (tin);
  char *jmout=static_cast<char*> (tout);
  if (mcrfill_ABS(jmout-jmin) >= *size)
    memcpy( tout, tin, *size);
  else if (tin > tout)
    {
      integer n = *size;
      while (n-- > 0) *jmout++ = *jmin++;
    }
  else
    {
      integer n = *size;
      jmin+=n;
      jmout+=n;
      while (n-- > 0) *--jmout = *--jmin;
    }
  return 0;
}


/*........................................................................*/
/*                                                                        */
/*   FUNCTION :                                                           */
/*   ----------                                                           */
/*               Routines for management of the dynamic memory.               */
/*                                                                        */
/*             Routine mcrfree                                            */
/*             --------------                                             */
/*                                                                        */
/*             Desallocation of a memory zone  .                          */
/*                                                                        */
/*             CALL MCRFREE (IBYTE,IADR,IER)                              */
/*                                                                        */
/*             IBYTE INTEGER*4 : Nb of Octets to free                     */
/*                                                                        */
/*             IADR POINTEUR   : Start Address                            */
/*                                                                        */
/*             IER  INTEGER*4  : Return Code                              */
/*                                                                        */
/*                                                                        */
/*........................................................................*/
/*                                                                        */

//=======================================================================
//function : mcrfree_
//purpose  : 
//=======================================================================
int mcrfree_(integer *,//ibyte,
	     intptr_t iadr,
	     integer *ier)

{
  *ier=0;
  Standard::Free((void*)iadr);
  return 0;
}

/*........................................................................*/
/*                                                                        */
/*   FONCTION :                                                           */
/*   ----------                                                           */
/*               Routines for management of the dynamic memory.           */
/*                                                                        */
/*             Routine mcrgetv                                            */
/*             --------------                                             */
/*                                                                        */
/*               Demand of memory allocation.                             */
/*                                                                        */
/*               CALL MCRGETV(IBYTE,IADR,IER)                             */
/*                                                                        */
/*               IBYTE (INTEGER*4) Nb of Bytes of allocation required     */    
/*                                                                        */
/*               IADR   (INTEGER*4) : Result.                             */
/*                                                                        */
/*               IER (INTEGER*4)    : Error Code    :                     */
/*                                                                        */
/*                   = 0  ==> OK                                          */
/*                   = 1  ==> Allocation impossible                       */
/*                   = -1 ==> Ofset > 2**31 - 1                           */
/*                                                                        */

/*                                                                        */
/*........................................................................*/

//=======================================================================
//function : mcrgetv_
//purpose  : 
//=======================================================================
int mcrgetv_(integer *sz,
	     intptr_t *iad,
	     integer *ier)                                            

{
  
  *ier = 0;
  *iad = (intptr_t)Standard::Allocate(*sz);
  if ( !*iad ) *ier = 1;
  return 0;
}


//=======================================================================
//function : mcrlist_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrlist_(integer *ier) const

{
  /* System generated locals */
  integer i__1;
  
  /* Builtin functions */
  
  /* Local variables */
  char cfmt[1];
  doublereal dfmt;
  integer ifmt, i__, nufmt, ntotal;
  char subrou[7];
  

/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*   PRINT TABLE OF CURRENT DYNAMIC ALLOCATIONS */

/*     KEYWORDS : */
/*     ----------- */
/*   SYSTEM, ALLOCATION, MEMORY, LIST */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        . NONE */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        *      : */
/*        *      : */
/*        IERCOD : ERROR CODE */

/*        IERCOD = 0 : OK */
/*        IERCOD > 0 : SERIOUS ERROR  */
/*        IERCOD < 0 : WARNING */
/*        IERCOD = 1 : ERROR DESCRIPTION */
/*        IERCOD = 2 : ERROR DESCRIPTION */

/*     COMMONS USED   : */
/*     ---------------- */

/*    MCRGENE     VFORMT */

/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     Type  Name */
/*           VFORMA */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*         . NONE */



/* > */
/* ***********************************************************************
 */

/*     INCLUDE MCRGENE */
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        TABLE FOR MANAGEMENT OF DYNAMIC MEMORY ALLOCATIONS */

/*     KEYWORDS : */
/*     ----------- */
/*        SYSTEM, MEMORY, ALLOCATION */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE OF EXISTING ALLOCATIONS, EACH HAVING : */
/*         1 : LEVEL OF PROTECTION (0=NOT PROTECTED, OTHER=PROTECTED) */
/*             (PROTECTED MEANS NOT DESTROYED BY CRRSET .) */
/*         2 : UNIT OF ALLOCATION */
/*         3 : NB OF ALLOCATED UNITS */
/*         4 : REFERENCE ADDRESS OF THE TABLE */
/*         5 : IOFSET */
/*         6 : STATIC ALLOCATION NUMBER */
/*         7 : Required allocation size */
/*         8 : address of the beginning of allocation */
/*         9 : Size of the USER ZONE */
/*        10 : ADDRESS of the START FLAG */
/*        11 : ADDRESS of the END FLAG */
/*        12 : Rank of creation of the allocation */

/*   NDIMCR : NB OF DATA OF EACH ALLOC IN ICORE */
/*   NCORE : NB OF CURRENT ALLOCS */
/*   LPROT : COMMUNICATION BETWEEN CRPROT AND MCRRQST, SET TO 0 BY MCRRQST */
/*   FLAG  : VALUE OF THE FLAG USED FOR EXCESSES */



/* ----------------------------------------------------------------------*
 */


/* ----------------------------------------------------------------------*
 */

    *ier = 0;
    //__s__copy(subrou, "MCRLIST", 7L, 7L);

/*     WRITE HEADING */

    nufmt = 1;
    ifmt = mcrgene_.ncore;
    macrmsg_(subrou, &nufmt, &ifmt, &dfmt, cfmt, 7L, 1L);

    ntotal = 0;

    i__1 = mcrgene_.ncore;
    for (i__ = 0; i__ < i__1; ++i__) {
	nufmt = 2;
	ifmt = mcrgene_.icore[i__].unit * mcrgene_.icore[i__].reqsize
		;
	macrmsg_(subrou, &nufmt, &ifmt, &dfmt, cfmt, 7L, 1L);
	ntotal += ifmt;
/* L1001: */
    }

    nufmt = 3;
    ifmt = ntotal;
    macrmsg_(subrou, &nufmt, &ifmt, &dfmt, cfmt, 7L, 1L);

 return 0 ;
} /* mcrlist_ */


//=======================================================================
//function : mcrlocv_
//purpose  : 
//=======================================================================
int mcrlocv_(void* t,
	     intptr_t *l)

{
  *l = reinterpret_cast<intptr_t> (t);
  return 0 ;
}

//=======================================================================
//function : AdvApp2Var_SysBase::mcrrqst_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrrqst_(integer *iunit, 
				 integer *isize, 
				 void *t, 
				 intptr_t *iofset, 
				 integer *iercod)

{

  integer i__1, i__2;

  /* Local variables */
  doublereal dfmt;
  integer ifmt, iver;
  char subr[7];
  integer ksys , ibyte, irest, ier;
  intptr_t iadfd, iadff, iaddr,lofset, loc;
  integer izu;

  
/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*       IMPLEMENTATION OF DYNAMIC MEMORY ALLOCATION */

/*     KEYWORDS : */
/*     ----------- */
/*       SYSTEM, ALLOCATION, MEMORY, REALISATION */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        IUNIT  : NUMBER OF OCTET OF THE UNIT OF ALLOCATION */
/*        ISIZE  : NUMBER OF UNITS REQUIRED */
/*        T      : REFERENCE ADDRESS */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*        IOFSET : OFFSET */
/*        IERCOD : ERROR CODE, */
/*               = 0 : OK */
/*               = 1 : MAX NB OF ALLOCS REACHED */
/*               = 2 : ARGUMENTS INCORRECT */
/*               = 3 : REFUSED DYNAMIC ALLOCATION */

/*     COMMONS USED   : */
/*     ---------------- */
/*       MCRGENE, MCRSTAC */

/*     REFERENCES CALLED   : */
/*     ----------------------- */
/*       MACRCHK, MACRGFL, MACRMSG, MCRLOCV,MCRCOMM, MCRGETV */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     1) USER */
/*     -------------- */

/* T IS THE ADDRESS OF A TABLE, IOFSET REPRESENTS THE DEPLACEMENT IN */
/* UNITS OF IUNIT OCTETS BETWEEN THE ALLOCATED ZONE AND TABLE T */
/* IERCOD=0 SIGNALS THAT THE ALLOCATION WORKS WELL, ANY OTHER */
/* VALUE INDICATES A BUG. */

/*     EXAMPLE : */
/*          LET THE DECLARATION REAL*4 T(1), SO IUNIT=4 . */
/*          CALL TO MCRRQST PORODUCES DYNAMIC ALLOCATION */
/*          AND GIVES VALUE TO VARIABLE IOFSET, */
/*          IF IT IS REQUIRED TO WRITE 1. IN THE 5TH ZONE REAL*4 */
/*          ALLOCATED IN THIS WAY, MAKE: */
/*          T(5+IOFSET)=1. */

/*     CASE OF ERRORS : */
/*     --------------- */

/*     IERCOD=1 : MAX NB OF ALLOCATION REACHED (ACTUALLY 200) */
/*     AND THE FOLLOWING MESSAGE APPEARS IN THE CONSOLE ALPHA : */
/*     "The max number of memory allocation is reached : ,N" */

/*     IERCOD=2 : ARGUMENT IUNIT INCORRECT AS IT IS DIFFERENT FROM 1,2,4 OR 8 */
/*     AND THE FOLLOWING MESSAGE APPEARS IN THE CONSOLE ALPHA : */
/*     "Unit OF allocation invalid : ,IUNIT" */

/*     IERCOD=3 : REFUSED DYNAMIC ALLOCATION (MORE PLACE IN MEMORY) */
/*     AND THE FOLLOWING MESSAGE APPEARS IN THE CONSOLE ALPHA : */
/*    "The system refuses dynamic allocation of memory of N octets" 
*/
/*     with completev display of all allocations carried out till now */


/*     2) DESIGNER */
/*     -------------- */

/* MCRRQST MAKES DYNAMIC ALLOCATION OF VIRTUAL MEMORY ON THE BASE */
/* OF ENTITIES OF 8 OCTETS (QUADWORDS), WHILE THE ALLOCATION IS REQUIRED BY */
/* UNITS OF IUNIT OCTETS (1,2,4,8). */

/* THE REQUIRED QUANTITY IS IUNIT*ISIZE OCTETS, THIS VALUE IS ROUNDED */
/* SO THAT THE ALLOCATION WAS AN INTEGER NUMBER OF QUADWORDS. */



/* > */
/* ***********************************************************************
 */

/* COMMON OF PARAMETRES */
/* COMMON OF INFORMATION ON STATISTICS */
/*     INCLUDE MCRGENE */

/* ***********************************************************************
 */
/*     FUNCTION : */
/*     ---------- */
/*        TABLE FOR MANAGEMENT OF DYNAMIC MEMORY ALLOCATIONS */

/*     KEYWORDS : */
/*     ----------- */
/*        SYSTEM, MEMORY, ALLOCATION */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE OF EXISTING ALLOCATIONS, EACH HAVING : */
/*         1 : LEVEL OF PROTECTION (0=NOT PROTECTED, OTHER=PROTECTED) */
/*             (PROTECTED MEANS NOT DESTROYED BY CRRSET .) */
/*         2 : UNIT OF ALLOCATION */
/*         3 : NB OF ALLOCATED UNITS */
/*         4 : REFERENCE ADDRESS OF THE TABLE */
/*         5 : IOFSET */
/*         6 : STATIC ALLOCATION NUMBER */
/*         7 : Required allocation size */
/*         8 : address of the beginning of allocation */
/*         9 : Size of the USER ZONE */
/*        10 : ADDRESS of the START FLAG */
/*        11 : ADDRESS of the END FLAG */
/*        12 : Rank of creation of the allocation */

/*   NDIMCR : NB OF DATA OF EACH ALLOC IN ICORE */
/*   NCORE : NB OF CURRENT ALLOCS */
/*   LPROT : COMMUNICATION BETWEEN CRPROT AND MCRRQST, SET TO 0 BY MCRRQST */
/*   FLAG  : VALUE OF THE FLAG USED FOR EXCESSES */




/* ----------------------------------------------------------------------*
 */
/*     20-10-86 : BF ; INITIAL VERSION  */


/*     NRQST : NUMBER OF ALLOCATIONS  */
/*     NDELT : NUMBER OF LIBERATIONS */
/*     NBYTE : TOTAL NUMBER OF OCTETS OF ALLOCATIONS */
/*     MBYTE : MAX NUMBER OF OCTETS */


/* ----------------------------------------------------------------------*
 */

    /* Function Body */
    *iercod = 0;

    if (mcrgene_.ncore >= MAX_ALLOC_NB) {
	goto L9001;
    }
    if (*iunit != 1 && *iunit != 2 && *iunit != 4 && *iunit != 8) {
	goto L9002;
    }

/* Calculate the size required by the user */
    ibyte = *iunit * *isize;

/* Find the type of version (Phase of Production or Version Client) */
    madbtbk_(&iver);

/* Control allocated size in Production phase */

    if (iver == 1) {

	if (ibyte == 0) {
	    //do__lio(&c__9, &c__1, "Require zero allocation", 26L);
	    maostrb_();
	} else if (ibyte >= 4096000) {
	    //do__lio(&c__9, &c__1, "Require allocation above 4 Mega-Octets : ", 50L);
	    //do__lio(&c__3, &c__1, (char *)&ibyte, (ftnlen)sizeof(integer));
	    maostrb_();
	}

    }

/* CALCULATE THE SIZE OF THE USER ZONE (IZU) */
/*     . add size required by the user (IBYTE) */
/*     . add delta for alinement with the base */
/*     . round to multiple of 8 above */

  mcrlocv_(t, &loc);
    izu = ibyte + loc % *iunit;
    irest = izu % 8;
    if (irest != 0) {
	izu = izu + 8 - irest;
    }

/* CALCULATE THE SIZE REQUIRED FROM THE PRIMITIVE OF ALLOC */
/*     . add size of the user zone */
/*     . add 8 for alinement of start address of */
/*       allocation on multiple of 8 so that to be able to  */
/*       set flags with Double Precision without other pb than alignement */
/*     . add 16 octets for two flags */

    ibyte = izu + 24;

/* DEMAND OF ALLOCATION */

/* L1001: */
/*      IF ( ISYST.EQ.0.AND.IBYTE .LE. 100 * 1024 ) THEN */
/*        ALLOCATION SUR TABLE */
/*         KSYS = 1 */
/*         KOP = 1 */
/*         CALL MCRCOMM ( KOP , IBYTE , IADDR , IER ) */
/*         IF ( IER .NE. 0 ) THEN */
/*            ISYST=1 */
/*            GOTO 1001 */
/*         ENDIF */
/*      ELSE */
/*        ALLOCATION SYSTEME */
    ksys = heap_allocation;
    mcrgetv_(&ibyte, &iaddr, &ier);
    if (ier != 0) {
	goto L9003;
    }
/*      ENDIF */

/* CALCULATE THE ADDRESSES OF FLAGS */

    iadfd = iaddr + 8 - iaddr % 8;
    iadff = iadfd + 8 + izu;

/* CALCULATE USER OFFSET : */
/*     . difference between the user start address and the */
/*       base address */
/*     . converts this difference in the user unit */

    lofset = iadfd + 8 + loc % *iunit - loc;
    *iofset = lofset / *iunit;

/* If phase of production control flags */
    if (iver == 1) {
	macrchk_();
    }

/*     SET FLAGS */
/*     . the first flag is set by IADFD and the second by IADFF */
/*     . if phase of production, set to overflow the ZU */
    macrgfl_(&iadfd, &iadff, &iver, &izu);

/* RANGING OF PARAMETERS IN MCRGENE */

    mcrgene_.icore[mcrgene_.ncore].prot = mcrgene_.lprot;
    mcrgene_.icore[mcrgene_.ncore].unit = (unsigned char)(*iunit);
    mcrgene_.icore[mcrgene_.ncore].reqsize = *isize;
    mcrgene_.icore[mcrgene_.ncore].loc = loc;
    mcrgene_.icore[mcrgene_.ncore].offset = *iofset;
    mcrgene_.icore[mcrgene_.ncore].alloctype = (unsigned char)ksys;
    mcrgene_.icore[mcrgene_.ncore].size = ibyte;
    mcrgene_.icore[mcrgene_.ncore].addr = iaddr;
    mcrgene_.icore[mcrgene_.ncore].userzone = mcrgene_.ncore;
    mcrgene_.icore[mcrgene_.ncore].startaddr = iadfd;
    mcrgene_.icore[mcrgene_.ncore].endaddr = iadff;
    mcrgene_.icore[mcrgene_.ncore].rank = mcrgene_.ncore + 1;
    ++mcrgene_.ncore;

    mcrgene_.lprot = 0;

/* CALL ALLOWING AUTOIMPLEMENTATION OF THE SET WATCH BY THE DEBUGGER */

    macrstw_(&iadfd, &iadff, &mcrgene_.ncore);

/* STATISTICS */

    ++mcrstac_.nrqst[ksys];
    mcrstac_.nbyte[ksys] += mcrgene_.icore[mcrgene_.ncore - 1].unit * 
	    mcrgene_.icore[mcrgene_.ncore - 1].reqsize;
/* Computing MAX */
    i__1 = mcrstac_.mbyte[ksys], i__2 = mcrstac_.nbyte[ksys];
    mcrstac_.mbyte[ksys] = advapp_max(i__1,i__2);

    goto L9900;

/* ----------------------------------------------------------------------*
 */
/*  ERROR PROCESSING */

/*  MAX NB OF ALLOC REACHED : */
L9001:
    *iercod = 1;
    ifmt = MAX_ALLOC_NB;
    //__s__copy(subr, "MCRRQST", 7L, 7L);
    macrmsg_(subr, iercod, &ifmt, &dfmt, " ", 7L, 1L);
    maostrd_();
    goto L9900;

/*  INCORRECT ARGUMENTS */
L9002:
    *iercod = 2;
    ifmt = *iunit;
    //__s__copy(subr, "MCRRQST", 7L, 7L);
    macrmsg_(subr, iercod, &ifmt, &dfmt, " ", 7L, 1L);
    goto L9900;

/* SYSTEM REFUSES ALLOCATION */
L9003:
    *iercod = 3;
    ifmt = ibyte;
    //__s__copy(subr, "MCRRQST", 7L, 7L);
    macrmsg_(subr, iercod, &ifmt, &dfmt, " ", 7L, 1L);
    maostrd_();
    mcrlist_(&ier);
    goto L9900;

/* ----------------------------------------------------------------------*
 */

L9900:
    mcrgene_.lprot = 0;
 return 0 ;
} /* mcrrqst_ */

//=======================================================================
//function : AdvApp2Var_SysBase::mgenmsg_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mgenmsg_(const char *,//nomprg, 
				 ftnlen )//nomprg_len)

{
  return 0;
} /* mgenmsg_ */

//=======================================================================
//function : AdvApp2Var_SysBase::mgsomsg_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mgsomsg_(const char *,//nomprg, 
				 ftnlen )//nomprg_len)

{
  return 0;
} /* mgsomsg_ */


/*
C
C*****************************************************************************
C
C     FUNCTION : CALL MIRAZ(LENGTH,ITAB)
C     ---------- 
C
C     RESET TO ZERO A TABLE OF LOGIC OR INTEGER.
C
C     KEYWORDS :
C     -----------
C        RAZ INTEGER
C
C     INPUT ARGUMENTS  :
C     ------------------
C               LENGTH : NUMBER OF OCTETS TO TRANSFER
C               ITAB   : NAME OF THE TABLE
C
C     OUTPUT ARGUMENTS  :
C     -------------------
C               ITAB   : NAME OF THE TABLE SET TO ZERO
C
C     COMMONS USED   :
C     ----------------
C
C     REFERENCES CALLED   :
C     ---------------------
C
C     DEMSCRIPTION/NOTES/LIMITATIONS :
C     -----------------------------------
C
C           Portable VAX-SGI

C>
C***********************************************************************
*/
//=======================================================================
//function : AdvApp2Var_SysBase::miraz_
//purpose  : 
//=======================================================================
void AdvApp2Var_SysBase::miraz_(integer *taille,
				void *adt)

{
  memset(adt , '\0' , *taille) ;
}
//=======================================================================
//function : AdvApp2Var_SysBase::mnfndeb_
//purpose  : 
//=======================================================================
integer AdvApp2Var_SysBase::mnfndeb_()
{
  integer ret_val;
  ret_val = 0;
  return ret_val;
} /* mnfndeb_ */

//=======================================================================
//function : AdvApp2Var_SysBase::msifill_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::msifill_(integer *nbintg, 
				 integer *ivecin,
				 integer *ivecou)
{
  integer nocte;
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*      transfer Integer from one  zone to another */

/*     KEYWORDS : */
/*     ----------- */
/*        TRANSFER , INTEGER , MEMORY */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NBINTG : Nb of integers */
/*        IVECIN : Input vector */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        IVECOU : Output vector */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */

/* ___ NOCTE : Number of octets to transfer */

    /* Parameter adjustments */
    --ivecou;
    --ivecin;

    /* Function Body */
    nocte =  *nbintg * sizeof(integer);
    AdvApp2Var_SysBase::mcrfill_(&nocte, &ivecin[1], &ivecou[1]);
 return 0 ;
} /* msifill_ */

//=======================================================================
//function : AdvApp2Var_SysBase::msrfill_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::msrfill_(integer *nbreel, 
				 doublereal *vecent,
				 doublereal * vecsor)
{
  integer nocte;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Transfer real from one zone to another */

/*     KEYWORDS : */
/*     ----------- */
/*        TRANSFER , REAL , MEMORY */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*        NBREEL : Number of reals */
/*        VECENT : Input vector */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        VECSOR : Output vector */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */

/* ___ NOCTE : Nb of octets to transfer */

    /* Parameter adjustments */
    --vecsor;
    --vecent;

    /* Function Body */
    nocte = *nbreel * sizeof (doublereal);
    AdvApp2Var_SysBase::mcrfill_(&nocte, &vecent[1], &vecsor[1]);
 return 0 ;
} /* msrfill_ */

//=======================================================================
//function : AdvApp2Var_SysBase::mswrdbg_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mswrdbg_(const char *,//ctexte, 
				 ftnlen )//ctexte_len)

{

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Write message on console alpha if IBB>0 */

/*     KEYWORDS : */
/*     ----------- */
/*        MESSAGE, DEBUG */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*        CTEXTE : Text to be written */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*        None */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ***********************************************************************
 */
/*                      DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                      PROCESSING */
/* ***********************************************************************
 */

    if (AdvApp2Var_SysBase::mnfndeb_() >= 1) {
	//do__lio(&c__9, &c__1, "Dbg ", 4L);
	//do__lio(&c__9, &c__1, ctexte, ctexte_len);
    }
 return 0 ;
} /* mswrdbg_ */



int __i__len()
{
  return 0;
}

int __s__cmp()
{
  return 0;
}

//=======================================================================
//function : do__fio
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::do__fio() 
{
return 0;
}
//=======================================================================
//function : do__lio
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::do__lio ()
{
  return 0;
}

/*
C*****************************************************************************
C
C     FUNCTION : CALL MVRIRAZ(NBELT,DTAB)
C     ---------- 
C     Reset to zero a table with DOUBLE PRECISION
C
C     KEYWORDS :
C     -----------
C     MVRMIRAZ DOUBLE
C
C     INPUT ARGUMENTS :
C     ------------------
C     NBELT  : Number of elements of the table
C     DTAB   : Table to initializer to zero
C
C     OUTPUT ARGUMENTS :
C     --------------------
C     DTAB   : Table reset to zero
C
C     COMMONS USED  :
C     ----------------
C
C     REFERENCES CALLED   :
C     -----------------------
C
C     DEMSCRIPTION/NOTES/LIMITATIONS :
C     -----------------------------------
C     
C
C>
C***********************************************************************
*/
//=======================================================================
//function : AdvApp2Var_SysBase::mvriraz_
//purpose  : 
//=======================================================================
void AdvApp2Var_SysBase::mvriraz_(integer *taille,
				  void *adt)

{
  integer offset;
  offset = *taille * 8 ;
  /*    printf(" adt %d  long %d\n",adt,offset); */
  memset(adt , '\0' , offset) ;
}
