//
// AdvApp2Var_SysBase.cxx
//
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_SysBase.hxx>
//
#include <AdvApp2Var_Data.hxx>


static 
int __i__len();

static
int __s__cmp();

static
int macrbrk_();

static
int macrchk_();

static
int macrclw_(long int *iadfld, 
	     long int *iadflf, 
	     integer  *nalloc);
static
int macrerr_(long int *iad,
	     integer *nalloc);
static
int macrgfl_(long int *iadfld, 
	     long int *iadflf, 
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
int macrstw_(integer *iadfld, 
	     integer *iadflf, 
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
	     long int *iadr, 
	     integer *ier);

static
int mcrfree_(integer *ibyte,
	     uinteger *iadr,
	     integer *ier);

static
int mcrgetv_(integer *sz,
	     uinteger *iad,
	     integer *ier);

static
int mcrlist_(integer *ier);

static
int mcrlocv_(long int t,
	     long int *l);


/* Structures */
static struct {
    long int icore[12000];	
    integer ncore, lprot;
} mcrgene_;

static struct {
    integer nrqst[2], ndelt[2], nbyte[2], mbyte[2];
} mcrstac_;

static struct {
    integer lec, imp, keyb, mae, jscrn, itblt, ibb;
} mblank__;

#define mcrfill_ABS(a)  (((a)<0)?(-(a)):(a)) 


//=======================================================================
//function : macinit_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::macinit_(integer *imode, 
				 integer *ival)

{
 
  /* Fortran I/O blocks */
  static cilist io______1 = { 0, 0, 0, (char*) "(' --- Debug-mode : ',I10,' ---')", 0 };
  
  /* ************************************************************************/
  /*     FONCTION : */
  /*     ---------- */
  /*   INITIALISATION DES UNITES DE LECTURE-ECRITURE, ET DE 'IBB' */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*   GESTION, CONFIGURATION, UNITES, INITIALISATION */
  
  /*     ARGUMENTS D'ENTREE : */
  /*     -------------------- */
  /*        IMODE : MODE D'INIT : 0= DEFAUT, IMP VAUDRA 6 ET IBB 0 */
  /*                                ET LEC 5 */
  /*                              1= FORCE LA VALEUR DE IMP */
  /*                              2= FORCE LA VALEUR DE IBB */
  /*                              3= FORCE LA VALEUR DE LEC */
  
  /*    ARGUMENT UTILISE QUE LORSQUE IMODE VAUT 1 OU 2 : */
  /*       IVAL : VALEUR DE IMP LORSQUE IMODE VAUT 1 */
  /*              VALEUR DE IBB LORSQUE IMODE VAUT 2 */
  /*              VALEUR DE LEC LORSQUE IMODE VAUT 3 */
  /*    IL N'Y A PAS DE CONTROLE SUR LA VALIDITE DE LA VALEUR DE IVAL . */
  
  /*     ARGUMENTS DE SORTIE : */
  /*     --------------------- */
  /*                NEANT */
  
  /*     COMMONS UTILISES : */
  /*     ------------------ */
  /*     REFERENCES APPELEES : */
  /*     --------------------- */
  /*     DESCRIPTION/REMARQUES/LIMITATIONS : */
  /*     ----------------------------------- */
  
  /*     IL NE S'AGIT QUE D'INITIALISER LE COMMON BLANK POUR TOUS LES */
  /*     MODULES QUI N'ONT A PRIORI PAS BESOIN DE CONNAITRE LES COMMONS */
  /*     DE T . */
  /*     LORSQU'UNE MODIFICATION DE IBB EST DEMANDEE (IMODE=2) UN MESSAGE */
  /*     D'INFORMATION EST EMIS SUR IMP, AVEC LA NOUVELLE VALEUR DE IBB. */
  
  /*       IBB : MODE DEBUG DE STRIM T : REGLES D'UTILISATION : */
  /*             0 VERSION SOBRE */
  /*             >0 LA VERSION A D'AUTANT PLUS DE COMMENTAIRES */
  /*                QUE IBB EST GRAND . */
  /*                PAR EXEMPLE AVEC IBB=1 LES ROUTINES APPELEES */
  /*                SE SIGNALENT SUR IMP ('ENTREE DANS TOTO', */
  /*                ET 'SORTIE DE TOTO'), ET LES ROUTINES RENVOYANT */
  /*                UN CODE ERREUR NON NUL LE SIGNALENT EGALEMENT. */
  /*            (MAIS CECI N'EST PAS VRAI POUR TOUTES LES ROUTINES DE T) */
  
  /* $    HISTORIQUE DES MODIFICATIONS : */
  /*     ------------------------------ */
  /*         22-12-89 : DGZ; MODIFICATION EN-TETE */
  /*         30-05-88 : PP ; AJOUT DE LEC */
  /*         15-03-88 : PP ; ECRITURE VERSION ORIGINALE */
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
    io______1.ciunit = mblank__.imp;
    /*
    s__wsfe(&io______1);
    */
    /*
    do__fio(&c____1, (char *)&mblank__.ibb, (ftnlen)sizeof(integer));
    */
    AdvApp2Var_SysBase::e__wsfe();
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
				 long int *iofset,
				 integer *iercod)

{
  
  /* ***********************************************************************
   */
  
  /*     FONCTION : */
  /*     ---------- */
  /*       Demande d'allocation dynamique de type INTEGER */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*       SYSTEME, ALLOCATION, MEMOIRE, REALISATION */
  
  /*     ARGUMENTS D'ENTREE : */
  /*     -------------------- */
  /*       NBELEM : Nombre d'unites demandes */
  /*       MAXELM : Nombre maxi d'unites disponibles dans ITABLO */
  /*       ITABLO : Adresse de reference de la zone allouee */
  
  /*     ARGUMENTS DE SORTIE : */
  /*     --------------------- */
  /*       IOFSET : Decalage */
  /*       IERCOD : Code d'erreur */
  /*               = 0 : OK */
  /*               = 1 : Nbre maxi d'allocs atteint */
  /*               = 2 : Arguments incorrects */
  /*               = 3 : Refus d'allocation dynamique */
  
  /*     COMMONS UTILISES : */
  /*     ------------------ */
  
  /*     REFERENCES APPELEES : */
  /*     --------------------- */
  /*        MCRRQST */
  
  /*     DESCRIPTION/REMARQUES/LIMITATIONS : */
  /*     ----------------------------------- */
  /*     (Cf description dans l'entete de MCRRQST) */
  
  /*     Le tableau ITABLO doit etre dimensionne a MAXELM par l'appelant. */
  /*     Si la demande est inferieure ou egale a MAXELM, IOFSET rendu = 0. 
   */
  /*     Sinon, la demande d'allocation est effective et IOFSET > 0. */
  
  /* $    HISTORIQUE DES MODIFICATIONS : */
  /*     ------------------------------ */
  /*       16-10-91 : DGZ ; Recuperation version FBI */
  /* > */
  /* ***********************************************************************
   */
  
  integer  iunit; 
  /* Parameter adjustments */
  --itablo;
  
  
  iunit = sizeof(integer);    
  /* Function Body */
  if (*nbelem > *maxelm) {
    AdvApp2Var_SysBase::mcrrqst_(&iunit, nbelem, (doublereal *)&itablo[1], iofset, iercod);
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
				 long int *iofset, 
				 integer *iercod)

{
  static integer c__8 = 8;

  /* ***********************************************************************
   */
  
  /*     FONCTION : */
  /*     ---------- */
  /*       Demande d'allocation dynamique de type DOUBLE PRECISION */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*       SYSTEME, ALLOCATION, MEMOIRE, REALISATION */
  
  /*     ARGUMENTS D'ENTREE : */
  /*     -------------------- */
  /*       NBELEM : Nombre d'unites demandes */
  /*       MAXELM : Nombre maxi d'unites disponibles dans XTABLO */
  /*       XTABLO : Adresse de reference de la zone allouee */
  
  /*     ARGUMENTS DE SORTIE : */
  /*     --------------------- */
  /*       IOFSET : Decalage */
  /*       IERCOD : Code d'erreur */
  /*               = 0 : OK */
  /*               = 1 : Nbre maxi d'allocs atteint */
  /*               = 2 : Arguments incorrects */
  /*               = 3 : Refus d'allocation dynamique */
  
  /*     COMMONS UTILISES : */
  /*     ------------------ */
  
  /*     REFERENCES APPELEES : */
  /*     --------------------- */
  /*        MCRRQST */
  
  /*     DESCRIPTION/REMARQUES/LIMITATIONS : */
  /*     ----------------------------------- */
  /*     (Cf description dans l'entete de MCRRQST) */
  
  /*     Le tableau XTABLO doit etre dimensionne a MAXELM par l'appelant. */
  /*     Si la demande est inferieure ou egale a MAXELM, IOFSET rendu = 0. 
   */
  /*     Sinon, la demande d'allocation est effective et IOFSET > 0. */

  /* $    HISTORIQUE DES MODIFICATIONS : */
  /*     ------------------------------ */
  /*       16-10-91 : DGZ ; Recuperation version FBI */
  /* > */
  /* ***********************************************************************
   */
  
  
  /* Parameter adjustments */
  --xtablo;
  
  /* Function Body */
  if (*nbelem > *maxelm) {
    AdvApp2Var_SysBase::mcrrqst_(&c__8, nbelem, &xtablo[1], iofset, iercod);
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
int macrchk_()
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer  i__, j;
  static long int ioff;
  static doublereal t[1];
  static integer loc;
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       CONTROLE LES DEBORDEMENTS DE ZONE MEMOIRE ALLOUEES */

/*     MOTS CLES : */
/*     ----------- */
/*       SYSTEME, ALLOCATION, MEMOIRE, CONTROLE, DEBORDEMENT */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NEANT */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */
/*       MCRGENE */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*       MACRERR, MAOSTRD */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*       18-11-91 : DGZ; AC91118Z0000 : Resactivation */
/*       17-10-91 : FCR; AC91118Z0000 : Desactivation */
/*       25-09-91 : DGZ; GESTION DES FLAGS DANS MCRGENE */
/*       31-07-90 : DGZ; AJOUT TRACE-BACK EN PHASE DE PRODUCTION */
/*       04-11-89 : CR ; AJOUT DE OPTIONS /CHECK=NOBOUNDS . */
/*       03-10-89 : DGZ; REMPLACE COMMON ACFLAG PAR INCLUDE ACFLAG.INC */
/*       09-06-89 : PP ; CORRECTION DES CALCULS D'OFFSET */
/*       31-05-89 : DGZ; APPEL MCRLOCV EN DEHORS BOUCLE DO */
/*       25-05-89 : DGZ; CHANGE DIM ACRTAB : MALLOC PASSE DE 10000 A 200 
*/
/*       16-05-89 : PP ; AJOUT DE MACRERR, POUR ARRET SOUS  DBG */
/*       11-05-89 : DGZ ; CREATION DE LA VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TABLE DE GESTION DES ALLOCATIONS DYNAMIQUES DE MEMOIRE */

/*     MOTS CLES : */
/*     ----------- */
/*        SYSTEME, MEMOIRE, ALLOCATION */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       23-11-93 : FCR; AF93125U3A007 : MAXCR 200 --> 1000 */
/*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*       25-09-91 : DGZ; AJOUT INFOs SUPPLEMENTAIREs POUR GESTION FLAGS */
/*       18-01-91 : DGZ; MAXCR PASSE DE 100 A 200 SUR DEMANDE GDD */
/*       18-05-90 : DGZ; DECLARATION TYPE INTEGER POUR MAXCR */
/*       20-06-88 : PP ; MAXCR PASSE DE 50 A 100, SUR DEMANDE OG */
/*                       + AJOUT DE COMMENTAIRES */
/*       26-02-88 : PP ; MAXCR PASSE DE 40 A 50, SUR DEMANDE AB . */
/*       15-04-85 : BF ; VERSION D'ORIGINE */
/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE DES ALLOCS EXISTANTES, AVEC POUR CHACUNE : */
/*         1 : NIVEAU DE PROTECTION (0=PAS PROTEGE, AUTRE=PROTEGE) */
/*             (PROTEGE SIGNIFIE PAS DETRUIT PAR CRRSET .) */
/*         2 : UNITE D'ALLOCATION */
/*         3 : NB D'UNITES ALLOUEES */
/*         4 : ADRESSE DE REFERENCE DU TABLEAU */
/*         5 : IOFSET */
/*         6 : NUMERO ALLOCATION STATIQUE */
/*         7 : Taille demandee en allocation */
/*         8 : adresse du debut de l'allocation */
/*         9 : Taille de la ZONE UTILISATEUR */
/*        10 : ADRESSE DU FLAG DE DEBUT */
/*        11 : ADRESSE DU FLAG DE FIN */
/*        12 : Rang de creation de l'allocation */

/*   NDIMCR : NBRE DE DONNEES DE CHAQUE ALLOC DANS ICORE */
/*   NCORE : NBRE D'ALLOCS EN COURS */
/*   LPROT : COMMUNICATION ENTRE CRPROT ET MCRRQST, REMIS A 0 PAR MCRRQST 
*/
/*   FLAG  : VALEUR DU FLAG UTILISE POUR LES DEBORDEMENTS */



/* ----------------------------------------------------------------------*
 */


/* ----------------------------------------------------------------------*
 */

/* CALCUL ADRESSE DE T */
  mcrlocv_((long int)t, (long int *)&loc);
  
  /* CONTROLE DES FLAGS DANS LE TABLEAU */
  i__1 = mcrgene_.ncore;
  for (i__ = 1; i__ <= i__1; ++i__) {
    
    for (j = 10; j <= 11; ++j) {
      
      if (mcrgene_.icore[j + i__ * 12 - 13] != -1) {
	
	ioff = (mcrgene_.icore[j + i__ * 12 - 13] - loc) / 8;
	
	if (t[ioff] != -134744073.) {
	  
	  /* MSG : '*** ERREUR  : ECRASEMENT DE LA MEMOIRE D''ADRESS
	     E:',ICORE(J,I) */
	  /*       ET DE RANG ICORE(12,I) */
	  macrerr_((long int *)&mcrgene_.icore[j + i__ * 12 - 13], 
		   (integer *)&mcrgene_.icore[i__ * 12 - 1]);
	  
	  /* TRACE-BACK EN PHASE DE PRODUCTION */
	  maostrb_();
	  
	  /* SUPPRESSION DE L'ADRESSE DU FLAG POUR NE PLUS REFAIRE S
	     ON CONTROLE */
	  mcrgene_.icore[j + i__ * 12 - 13] = -1;
	  
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
int macrclw_(long int *,//iadfld, 
	     long int *,//iadflf, 
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
				 long int *iofset, /* Offset en long (pmn) */
				 integer *iercod)

{
  
  /* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       Destruction d'une allocation dynamique de type INTEGER */

/*     MOTS CLES : */
/*     ----------- */
/*       SYSTEME, ALLOCATION, MEMOIRE, DESTRUCTION */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NBELEM : Nombre d'unites demandes */
/*       MAXELM : Nombre maxi d'unites disponibles dans ITABLO */
/*       ITABLO : Adresse de reference de la zone allouee */
/*       IOFSET : Decalage */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       IERCOD : Code d'erreur */
/*               = 0 : OK */
/*               = 1 : Pb de de-allocation d'une zone allouee sur table */
/*               = 2 : Le systeme refuse la demande de de-allocation */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*        MCRDELT */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     (Cf description dans l'entete de MCRDELT) */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       16-10-91 : DGZ ; Recuperation version FBI */
/* > */
/* ***********************************************************************
 */
  integer iunit;
  
  /* Parameter adjustments */
  --itablo;
  iunit = sizeof(integer); 
  /* Function Body */
  if (*iofset != 0) {
    AdvApp2Var_SysBase::mcrdelt_(&iunit, 
				 nbelem, 
				 (doublereal *)&itablo[1], 
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
				 long int *iofset, 
				 integer *iercod)

{
  static integer c__8 = 8;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       Destruction d'une allocation dynamique de type DOUBLE PRECISION 
*/

/*     MOTS CLES : */
/*     ----------- */
/*       SYSTEME, ALLOCATION, MEMOIRE, DESTRUCTION */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NBELEM : Nombre d'unites demandes */
/*       MAXELM : Nombre maxi d'unites disponibles dans XTABLO */
/*       XTABLO : Adresse de reference de la zone allouee */
/*       IOFSET : Decalage */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       IERCOD : Code d'erreur */
/*               = 0 : OK */
/*               = 1 : Pb de de-allocation d'une zone allouee sur table */
/*               = 2 : Le systeme refuse la demande de de-allocation */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*        MCRDELT */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     (Cf description dans l'entete de MCRDELT) */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       16-10-91 : DGZ ; Recuperation version FBI */
/* > */
/* ***********************************************************************
 */
  
  
  /* Parameter adjustments */
  --xtablo;
  
  /* Function Body */
  if (*iofset != 0) {
    AdvApp2Var_SysBase::mcrdelt_(&c__8, nbelem, &xtablo[1], iofset, iercod);
  } else {
    *iercod = 0;
  }
  return 0 ;
} /* macrdr8_ */

//=======================================================================
//function : macrerr_
//purpose  : 
//=======================================================================
int macrerr_(long int *,//iad,
	     integer *)//nalloc)

{
  //static integer c__1 = 1;
  /* Builtin functions */
  //integer /*s__wsfe(),*/ /*do__fio(),*/ e__wsfe();
  
  /* Fortran I/O blocks */
  //static cilist io___1 = { 0, 6, 0, "(X,A,I9,A,I3)", 0 };

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       ECRITURE D'UNE ADRESSE ECRASEE DANS LES ALLOCS . */

/*     MOTS CLES : */
/*     ----------- */
/*       ALLOC CONTROLE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       IAD    : ADRESSE A SIGNALER ECRASEE */
/*       NALLOC : NUMERO DE L'ALLOCATION */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       30-09-91 : DGZ; AJOUT DU NUMERO DE L'ALLOCATION */
/*       04-11-89 : CR ; AJOUT DE OPTIONS /CHECK=NOBOUNDS . */
/*       17-05-89 : PP ; CREATION */
/* > */
/* ***********************************************************************
 */
  /*
  s__wsfe(&io___1);
  */
  /*
  do__fio(&c__1, "*** ERREUR : Ecrasement de la memoire d'adresse ", 48L);
  do__fio(&c__1, (char *)&(*iad), (ftnlen)sizeof(long int));
  do__fio(&c__1, " sur l'allocation ", 18L);
  do__fio(&c__1, (char *)&(*nalloc), (ftnlen)sizeof(integer));
  */
  AdvApp2Var_SysBase::e__wsfe();
  
  return 0 ;
} /* macrerr_ */


//=======================================================================
//function : macrgfl_
//purpose  : 
//=======================================================================
int macrgfl_(long int *iadfld, 
	     long int *iadflf, 
	     integer  *iphase, 
	     integer  *iznuti)

{
  /* Initialized data */
  
  static integer ifois = 0;
  
  static char cbid[1];
  static integer ibid, ienr;
  static doublereal t[1];
  static integer novfl;
  static long int ioff,iadrfl, iadt;
  
  
  /* ***********************************************************************
   */
  
  /*     FONCTION : */
  /*     ---------- */
  /*       MISE EN PLACE DES DEUX FLAGS DE DEBUT ET DE FIN DE LA ZONE */
  /*       ALLOUEE ET MISE A OVERFLOW DE L'ESPACE UTILISATEUR EN PHASE */
  /*       DE PRODUCTION. */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*       ALLOCATION, CONTROLE, DEBORDEMENT */
  
  /*     ARGUMENTS D'ENTREE : */
  /*     -------------------- */
  /*       IADFLD : ADRESSE DU FLAG DE DEBUT */
  /*       IADFLF : ADRESSE DU FLAG DE FIN */
  /*       IPHASE : TYPE DE VERSION LOGICIELLE : */
  /*                0 = VERSION OFFICIELLE */
  /*                1 = VERSION PRODUCTION */
  /*       IZNUTI : TAILLE DE LA ZONE UTILISATEUR EN OCTETS */
  
  /*     ARGUMENTS DE SORTIE : */
  /*     --------------------- */
  /*       NEANT */
  
  /*     COMMONS UTILISES : */
  /*     ------------------ */
  
  /*     REFERENCES APPELEES : */
  /*     --------------------- */
  /*       CRLOCT,MACRCHK */
  
  /*     DESCRIPTION/REMARQUES/LIMITATIONS : */
  /*     ----------------------------------- */
  
  /* $    HISTORIQUE DES MODIFICATIONS : */
  /*     ------------------------------ */
  /*       25-09-91 : DGZ ; GERE LES FLAGS DANS LE COMMUN MCRGENE */
  /*       21-08-90 : DGZ ; APPELS DE MACRCHK DANS LES DEUX CAS (AJOUT,SUPP)
   */
  /*       04-11-89 : CR ; AJOUT DE OPTIONS /CHECK=NOBOUNDS . */
  /*       03-10-89 : DGZ ; REMPLACE COMMON ACFLAG PAR INCLUDE ACFLAG.INC */
  /*       09-06-89 : PP  ; CORRECTION DU CALCUL DE L'OFFSET */
  /*       31-05-89 : DGZ ; OPTIMISATION DE LA GESTION DU TABLEAU DES FLAGS 
   */
  /*       23-05-89 : DGZ ; CORRECTION DEBORDEMENT DU TABLEAU ACRTAB */
  /*       11-05-89 : DGZ ; CREATION DE LA VERSION ORIGINALE */
  /* > */
  /* ***********************************************************************
   */
  
 

  /* ***********************************************************************
   */
  
  /*     FONCTION : */
  /*     ---------- */
  /*        TABLE DE GESTION DES ALLOCATIONS DYNAMIQUES DE MEMOIRE */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*        SYSTEME, MEMOIRE, ALLOCATION */
  
  /*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
  /*     ----------------------------------- */
  
  /* $    HISTORIQUE DES MODIFICATIONS : */
  /*     ------------------------------ */
  /*       23-11-93 : FCR; AF93125U3A007 : MAXCR 200 --> 1000 */
  /*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
  /*       25-09-91 : DGZ; AJOUT INFOs SUPPLEMENTAIREs POUR GESTION FLAGS */
  /*       18-01-91 : DGZ; MAXCR PASSE DE 100 A 200 SUR DEMANDE GDD */
  /*       18-05-90 : DGZ; DECLARATION TYPE INTEGER POUR MAXCR */
  /*       20-06-88 : PP ; MAXCR PASSE DE 50 A 100, SUR DEMANDE OG */
  /*                       + AJOUT DE COMMENTAIRES */
  /*       26-02-88 : PP ; MAXCR PASSE DE 40 A 50, SUR DEMANDE AB . */
  /*       15-04-85 : BF ; VERSION D'ORIGINE */
  /* > */
  /* ***********************************************************************
   */
  
  /*   ICORE : TABLE DES ALLOCS EXISTANTES, AVEC POUR CHACUNE : */
  /*         1 : NIVEAU DE PROTECTION (0=PAS PROTEGE, AUTRE=PROTEGE) */
  /*             (PROTEGE SIGNIFIE PAS DETRUIT PAR CRRSET .) */
  /*         2 : UNITE D'ALLOCATION */
  /*         3 : NB D'UNITES ALLOUEES */
  /*         4 : ADRESSE DE REFERENCE DU TABLEAU */
  /*         5 : IOFSET */
  /*         6 : NUMERO ALLOCATION STATIQUE */
  /*         7 : Taille demandee en allocation */
  /*         8 : adresse du debut de l'allocation */
  /*         9 : Taille de la ZONE UTILISATEUR */
  /*        10 : ADRESSE DU FLAG DE DEBUT */
  /*        11 : ADRESSE DU FLAG DE FIN */
  /*        12 : Rang de creation de l'allocation */
  
  /*   NDIMCR : NBRE DE DONNEES DE CHAQUE ALLOC DANS ICORE */
  /*   NCORE : NBRE D'ALLOCS EN COURS */
  /*   LPROT : COMMUNICATION ENTRE CRPROT ET MCRRQST, REMIS A 0 PAR MCRRQST 
   */
  /*   FLAG  : VALEUR DU FLAG UTILISE POUR LES DEBORDEMENTS */
  
  

  /* ----------------------------------------------------------------------*
   */
  

  if (ifois == 0) {
    matrsym_("NO_OVERFLOW", cbid, &novfl, &ibid, 11L, 1L);
    ifois = 1;
  }
  
  /*  CALCUL DE L'ADRESSE DE T */
  mcrlocv_((long int)t, (long int *)&iadt);
  
  /* CALCUL DE l"OFFSET */
  ioff = (*iadfld - iadt) / 8;
  
  /*  MISE A OVERFLOW DE LA ZONE UTILISATEUR EN CAS DE VERSION PRODUCTION */
  if (*iphase == 1 && novfl == 0) {
    ienr = *iznuti / 8;
    maoverf_(&ienr, &t[ioff + 1]);
  }
    
  /*  MISE A JOUR DU FLAG DE DEBUT */
  t[ioff] = -134744073.;
  
  /*  APPEL BIDON POUR PERMETTRE L'ARRET AU DEBUGGER : */
  iadrfl = *iadfld;
  macrbrk_();
  
  /*  MISE A JOUR DU FLAG DE DEBUT */
  ioff = (*iadflf - iadt) / 8;
  t[ioff] = -134744073.;
    
  /*  APPEL BIDON POUR PERMETTRE L'ARRET AU DEBUGGER : */
  iadrfl = *iadflf;
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
  static integer inum, iunite;
  static char cfm[80], cln[3];
  
  /* Fortran I/O blocks */
  static cilist io___5 = { 0, 0, 0, cfm, 0 };
  static cilist io___6 = { 0, 0, 0, cfm, 0 };
  static cilist io___7 = { 0, 0, 0, cfm, 0 };
 

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        MESSAGERIE DES ROUTINES D'ALLOC */

/*     MOTS CLES : */
/*     ----------- */
/*       ALLOC,MESSAGE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       CROUT : NOM DE LA ROUTINE APPELANTE : MCRRQST, MCRDELT, MCRLIST 
*/
/*                ,CRINCR OU CRPROT */
/*       NUM : NUMERO DU MESSAGE */
/*       IT : TABLEAU DE DONNEES ENTIERES */
/*       XT : TABLEAU DE DONNEES REELLES */
/*       CT : ------------------ CHARACTER */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ROUTINE A USAGE TEMPORAIRE, EN ATTENDANT LA 'NOUVELLE' MESSAGERIE */
/*    (STRIM 3.3 ?) , POUR RENDRE LES ROUTINES D'ALLOC UTILISABLES */
/*    AILLEURS QUE DANS STRIM T-M . */

/*   EN FONCTION DE LA LANGUE, ECRITURE DU MESSAGE DEMANDE SUR */
/*   L'UNITE IMP . */
/*   (REPRISE DES SPECIFS DE VFORMA) */

/*   LE MESSAGE EST INITIALISE A 'IL MANQUE LE MESSAGE', ET CELUI-LA */
/*   EST REMPLACE PAR LE MESSAGE DEMANDE S'IL EXISTE . */


/*   LES MESSAGES FRANCAIS ONT ETE PRIS DANS LA 3.2 LE 26.2.88, ALORS */
/*   QUE LES ANGLAIS ONT ETE PRIS DANS ENGUS, ET QUE LES */
/*   ALLEMANDS VIENNENT DE LA 312 . */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*      4-09-1991 : FCR ; MENAGE */
/*      02-05-88 : PP ; CORRECTION DE SYNTAXE DE FORMAT */
/*      26.2.88 : PP ECRITURE VERSION ORIGINALE . */
/* > */
/* ***********************************************************************
 */

/*  LOCAL : */

/* ----------------------------------------------------------------------*
 */
/*  RECHERCHE DU MESSAGE EN FONCTION DE LA LANGUE , DE LA ROUTINE */
/*  CONCERNEE, ET DU NUMERO DE MESSAGE */

/*  LECTURE DE LA LANGUE : */
    /* Parameter adjustments */
  ct -= ct_len;
  --xt;
  --it;
  
  /* Function Body */
  mamdlng_(cln, 3L);
  
/*  INUM : TYPE DE MESSAGE  : 0 QUE DU TEXTE, 1 1 ENTIER A ECRIRE */
/*        -1 MESSAGE INEXISTANT (1 ENTIER ET 1 CHAINE) */

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
  /*  REALISATION DU WRITE , AVEC OU SANS DONNEES : */
  
  iunite = AdvApp2Var_SysBase::mnfnimp_();
  if (inum == 0) {
    io___5.ciunit = iunite;
    /*
    s__wsfe(&io___5);
    */
    AdvApp2Var_SysBase::e__wsfe();
  } else if (inum == 1) {
    io___6.ciunit = iunite;
    /*
    s__wsfe(&io___6);
    */
    /*
    do__fio(&c__1, (char *)&it[1], (ftnlen)sizeof(integer));
    */
    AdvApp2Var_SysBase::e__wsfe();
  } else {
    /*  LE MESSAGE N'EXISTE PAS ... */
    io___7.ciunit = iunite;
    /*
    s__wsfe(&io___7);
    */
    /*
    do__fio(&c__1, (char *)&(*num), (ftnlen)sizeof(integer));
    do__fio(&c__1, crout, crout_len);
    */
    AdvApp2Var_SysBase::e__wsfe();
  }
  
  return 0;
} /* macrmsg_ */
//=======================================================================
//function : macrstw_
//purpose  : 
//=======================================================================
int macrstw_(integer *,//iadfld, 
	     integer *,//iadflf, 
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
  static char cbid[255];
  static integer ibid, ier;
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        RENVOIE LA TRADUCTION D'UN "NOM LOGIQUE STRIM" DANS LA */
/*        "SYNTAXE INTERNE" CORRESPONDANT A UN "LIEU DE RANGEMENT" */

/*     MOTS CLES : */
/*     ----------- */
/*        NOM LOGIQUE STRIM , TRADUCTION */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        CNMLOG : NOM DU "NOM LOGIQUE STRIM" A TRADUIRE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CHAINE : ADRESSE DU "LIEU DE RANGEMENT" */
/*        LONG   : LONGUEUR UTILE DU "LIEU DE RANGEMENT" */
/*        IERCOD : CODE D'ERREUR */
/*        IERCOD = 0 : OK */
/*        IERCOD = 5 : LIEU DE RANGEMENT CORRESPONDANT AU NOM LOGIQUE */
/*                     INEXISTANT */
/*        IERCOD = 6 : TRADUCTION TROP LONGUE POUR LA VARIABLE 'CHAINE' */
/*        IERCOD = 7 : ERREUR SEVERE */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*        NEANT */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */
/*        GNMLOG, MACHDIM */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*        ROUTINE SPECIFIQUE SGI */

/*        DANS TOUS LES CAS OU IERCOD EST >0,AUCUN RESULTAT N'EST RENVOYE 
*/

/*        NOTION DE "SYNTAXE UTILISATEUR' ET "SYNTAXE INTERNE" */
/*        --------------------------------------------------- */

/*       LA "SYNTAXE UTILISATEUR" EST LA SYNTAXE DANS LAQUELLE L'UTILISATE
UR*/
/*        VISUALISE OU DESIGNE UN NOM DE FICHIER OU LE NOM REPERTOIRE AU 
*/
/*        COURS D'UNE SESSION DE STRIM100 */

/*        LA "SYNTAXE INTERNE" EST LA SYNTAXE UTILISEE POUR EFFECTUER DES 
*/
/*        OPERATIONS DE TRAITEMENTS DE FICHIERS A L'INTERIEUR DU CODE */
/*        (OPEN,INQUIRE,...ETC) */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */

/*    08-01-91 : B. Achispon ; Mise en forme et suppresion appel a MACHDIM
*/
/*     26-10-88 : C. Guinamard ; Adaptation UNIX  Traduction effective */
/*                               du nom logique */
/*     10-08-88 : DGZ ; CHANGE BNMLOG PAR MATRLOG */
/*     05-02-88 : DGZ ; MODIF D'ENTETE */
/*     26-08-87 : DGZ ; APPEL DE BNMLOG */
/*     25-08-87 : BJ  ; MODIF ENTETE */
/*     24-12-86 : DGZ ; CREATION VERSION ORIGINALE */

/* > */
/* ***********************************************************************
 */
/*              DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*              TRAITEMENT */
/* ***********************************************************************
 */

  *long__ = 0;
  *iercod = 0;
  
  /* CONTROLE DE L'EXISTENCE DU NOM LOGIQUE */
  
  matrlog_(cnmlog, cbid, &ibid, &ier, cnmlog_len, 255L);
  if (ier == 1) {
    goto L9500;
  }
  if (ier == 2) {
    goto L9700;
  }
  
  /* CONTROLE DE LA LONGUEUR DE CHAINE */
  
  if (ibid > __i__len()/*chaine, chaine_len)*/) {
    goto L9600;
  }
  
  //__s__copy(chaine, cbid, chaine_len, ibid);
  *long__ = ibid;
  
  goto L9999;
  
  /* ***********************************************************************
   */
  /*              TRAITEMENT DES ERREURS */
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
  /*              RETOUR AU PROGRAMME APPELANT */
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
  static integer c__504 = 504;

  /* Initialized data */

  static doublereal buff0[63] = { 
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
    0.,0.,0.,0.,0. 
    };
  
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer i__;
  static doublereal buffx[63];
  static integer nbfois, noffst, nreste, nufois;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       INITIALISATION A UNE VALEUR DONNEE D'UN TABLEAU DE REEL *8 */

/*     MOTS CLES : */
/*     ----------- */
/*       MANIPULATIONS, MEMOIRE, INITIALISATION, DOUBLE-PRECISION */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*        ITAILL : TAILLE DU TABLEAU */
/*        XTAB   : TABLEAU A INITIALISER AVEC XVAL */
/*        XVAL   : VALEUR A METTRE DANS XTAB(1 A ITAILL) */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*        XTAB   : TABLEAU INITIALISE */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ON APPELLE MCRFILL QUI FAIT DES MOVE PAR PAQUETS DE 63 REELS */

/*   LE PAQUET INITIAL EST BUFF0 INITE PAR DATA SI LA VALEUR EST 0 */
/*   OU BUFFX INITE PAR XVAL (BOUCLE) SINON . */


/*   PORTABILITE : OUI */
/*   ACCES : LIBRE */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     13-11-1991 : FCR ; VERFOR : Menage */
/*         06-05-91 : DGZ; MODIFICATION EN-TETE */
/*         05-07-88 : PP ; OPTIMISATION PAR POMPAGE SUR MVRMIRAZ */
/*         28-04-88 : PP ; CREATION */
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
	AdvApp2Var_SysBase::mcrfill_(&c__504, (char *)buff0, (char *)&xtab[(nufois - 1) * 63 + 1]);
	/* L1000: */
      }
    }
    
    if (nreste >= 1) {
      i__1 = nreste << 3;
      AdvApp2Var_SysBase::mcrfill_(&i__1, (char *)buff0, (char *)&xtab[noffst + 1]);
    }
  } else {
    for (i__ = 1; i__ <= 63; ++i__) {
      buffx[i__ - 1] = *xval;
      /* L2000: */
    }
    if (nbfois >= 1) {
      i__1 = nbfois;
      for (nufois = 1; nufois <= i__1; ++nufois) {
	AdvApp2Var_SysBase::mcrfill_(&c__504, (char *)buffx, (char *)&xtab[(nufois - 1) * 63 + 1]);
	/* L3000: */
      }
    }
    
    if (nreste >= 1) {
      i__1 = nreste << 3;
      AdvApp2Var_SysBase::mcrfill_(&i__1, (char *)buffx, (char *)&xtab[noffst + 1]);
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

/*     FONCTION : */
/*     ---------- */
/*   RENVOIE LA LANGUE COURANTE */

/*     MOTS CLES : */
/*     ----------- */
/*   GESTION, CONFIGURATION, LANGUE, LECTURE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       CMDLNG : LANGUE */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */
/*       MACETAT */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*       NEANT */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       DROIT D'UTILISATION : TOUTES APPLICATIONS */

/*       ATTENTION : CETTE ROUTINE DEPEND D'UNE INITIALISATION */
/*       ----------  PREALABLE FAITE AVEC AMDGEN. */
/*                   IL CONVIENT DONC DE S'ASSURER QUE CETTE INIT EST */
/*                   BIEN REALISEE DANS LE OU LES PROGRAMMES CONCERNES */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     25-01-93 : JMB ; NETTOYAGE DE MAMDLNG */
/*       23-03-90 : DGZ ; CORRECTION DE L'EN-TETE */
/*       22-12-89 : DGZ ; CORRECTION DE L'EN-TETE */
/*       22-02-88 : DGZ ; CREATION VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */


/*     INCLUDE MACETAT */
/* < */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        CONTIENT LES INFORMATIONS RELATIVES A LA COMPOSITION DE */
/*        L'EXECUTABLE ET A SON ENVIRONNEMENT : */
/*        - LANGUES */
/*        - APPLICATIONS PRESENTES */
/*        - TYPES D'ENTITES AUORISEES (NON UTILISE) */
/*        AINSI QUE DES INFORMATIONS DECRIVANTS L'ETAT COURANT : */
/*        - APPLICATION EN COURS */
/*        - MODE D'UTILISATION (NON UTILISE) */

/*     MOTS CLES : */
/*     ----------- */
/*        APPLICATION, LANGUE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     A) CHLANG*4 : LISTE DES VALEURS POSSIBLES DE LA LANGUE : */
/*                   'FRA ','DEU ','ENG ' */

/*        CHL10N*4 : LISTE DES VALEURS POSSIBLES DE LA LOCALISATION : */
/*                   'FRA ','DEU ','ENG ', 'JIS ' */

/*     B) CHCOUR*4, CHPREC*4, CHSUIV*4 : APPLICATION COURANTE, PRECEDENTE 
*/
/*                   ET SUIVANTE */

/*     C) CHMODE*4 : MODE COURANT (NON UTILISE) */

/*     D) CHPRES*2 (1:NBRMOD) : LISTE DES APPLICATIONS PRISES EN COMPTE */

/*       Rang ! Code interne  !   Application */
/*       ---------------------------------------------------------- */
/*        1   !   CD          !   Modelisation 2D */
/*        2   !   CA          !   Modelisation 2D par apprentissage */
/*        3   !   CP          !   Modelisation 2D parametree */
/*        4   !   PC          !   Modelisation rheologique 2D */
/*        5   !   CU          !   Fraisage 2 Axes 1/2 */
/*        6   !   CT          !   Tournage */
/*        7   !   TS          !   Modelisation 3D surfacique */
/*        8   !   TV          !   Modelisation 3D volumique */
/*        9   !   MC          !   Maillage coque */
/*        10  !   MV          !   Maillage volumique */
/*        11  !   TU          !   Usinage 3 axes continus */
/*        12  !   T5          !   Usinage 3-5 axes */
/*        13  !   TR          !   Usinage 5 axes de surfaces reglees */
/*        14  !   IG          !   Interface IGES */
/*        15  !   ST          !   Interface SET */
/*        16  !   VD          !   Interface VDA */
/*        17  !   IM          !   Interface de modelisation */
/*        18  !   GA          !   Generateur APT/IFAPT */
/*        19  !   GC          !   Generateur COMPACT II */
/*        20  !   GP          !   Generateur PROMO */
/*        21  !   TN          !   Usinage par copiage numerique */
/*        22  !   GM          !   Gestion des modeles */
/*        23  !   GT          !   Gestion de trace */
/*       ---------------------------------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*        05-05-93 : JMB ; Livraison GI93033FGR019 */
/*        8-03-1993: STT ; AJOUT CHL10N */
/*        31-07-92 : FCR ; GI91050G0348 : Suppression de CHTYPE */
/*        18-06-90 : DGZ ; AJOUT EXTENSION PAR COPIAGE NUMERIQUE */
/*        15-03-89 : DGZ ; MODIF DES APPLICATIONS POUR STANDARDS METIERS 
*/
/*        13-09-88 : DGZ ; AJOUT DES MODULES CC (TVCC) ET CG (CA GLOBAL) 
*/
/*        13-09-88 : DGZ ; AJOUT DES MODULES SET, IGES, VDA */
/*        22-02-88 : DGZ ; CREATION VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */

/*     NOMBRE D'APPLICATIONS PRISES EN COMPTE */


/*     NOMBRES DE TYPES D'ENTITE GERES PAR STRIM 100 */
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
  static integer imod;
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       AFFICHAGE DU TRACE-BACK EN PHASE DE PRODUCTION */

/*     MOTS CLES : */
/*     ----------- */
/*       FONCTION, SYSTEME, TRACE-BACK, AFFICHAGE, DEBUGGAGE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NEANT */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */
/*       NEANT */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*       MADBTBK */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       CETTE ROUTINE DOIT ETRE APPELE POUR REALISER UN AFFICHAGE */
/*       DE TRACE-BACK EN PHASE DE PRODUCTION ET LAISSER QUAND MEME */
/*       LA POSSIBILITE AUX TESTEURS D'OBTENIR CES TRACE-BACK DANS */
/*       LES VERSIONS CLIENTS SI UNE DES CONTIONS SUIVANTES EST */
/*       VERIFIEE : */
/*       - EXISTENCE DU SYMBOLE 'STRMTRBK' */
/*       - EXISTENCE DU FICHIER 'STRMINIT:STRMTRBK.DAT' */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       26-07-90 : DGZ ; CREATION DE LA VERSION ORIGINALE */
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
  
  static integer ifois = 0;
  
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer ibid;
  static doublereal buff[63];
  static integer ioct, indic, nrest, icompt;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       Initialisation en overflow d'un tableau en DOUBLE PRECISION */

/*     MOTS CLES : */
/*     ----------- */
/*       MANIPULATION, MEMOIRE, INITIALISATION, OVERFLOW */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NBENTR : Nombre d'entrees du tableau */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       DATBLE : Tableau double precision initialise en overflow */

/*     COMMONS UTILISES : */
/*     ------------------ */
/*       R8OVR contenu dans l'include MAOVPAR.INC */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*       MCRFILL */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       1) Doc. programmeur : */

/*       Cette routine initialise a l'overflow positif un tableau en */
/*       DOUBLE PRECISION. */

/*       Les autres types de tableaux (INTEGER*2, INTEGER, REAL, ...) */
/*       ne sont pas geres par la routine. */

/*       Elle est utilisable en phase de developpement pour deceler les */
/*       erreurs d'initialisation. */

/*       En version officielle, ses appels seront desactives. */

/*       ACCES : Sur accord avec AC. */

/*       La routine ne renvoie pas de code d'erreur. */

/*       L'argument NBELEM doit etre positif. */
/*       S'il est negatif ou nul, affichage du message "MAOVERF : NBELEM =
 */
/*       valeur_de_NBELEM" et d'un Trace Back par l'appel a la routine */
/*       MAOSTRB. */


/*       2) Doc. concepteur : */

/*                  L'idee est de minimiser le nombre d'appels a */
/*                la routine de transfert de zones numeriques, */
/*   ----------   pour des raisons de performances. */
/*  !  buffer  !    Pour cela, on se reserve un tableau de NLONGR */
/*  !__________!  DOUBLE PRECISIONs. Ce buffer est initialise par */
/*  <---------->  l'instruction DATA. L'overflow est accede dans un */
/*    NLONGR*8    COMMON specifique et non par une routine car */
/*                l'initialisation se fait par DATA. */

/*                * Si NBENTR<NLONGR, une partie du buffer est transferee 
*/
/*     DTABLE     dans DTABLE. */
/*   __________ */
/*  !  amorce  !  * Sinon, tout le buffer est transfere dans DTABLE. */
/*  !__________!  C'est l'amorce. Puis on execute une boucle qui a chaque 
*/
/*  !  temps 1 !  iteration transfere la partie du tableau deja */
/*  !__________!  initialisee dans celle qui ne l'a pas encore ete. La */
/*  !          !  taille de la zone transferee par chaque appel a MCRFILL 
*/
/*  !  temps 2 !  est donc de NLONGR*2**(numero_de_l'iteration). Lorsque 
*/
/*  !          !  la taille du tableau restant a initialiser est */
/*  !__________!  inferieure a celle deja initialisee, on sort de la */
/*  !          !  boucle et un dernier transfert est effectue pour */
/*  !          !  initialiser le reste du tableau, sauf si la taille */
/*  !          !  du tableau est du type NLONGR*2**K. */
/*  !  temps 3 ! */
/*  !          !  * NLONGR sera egal a 19200. */
/*  !          ! */
/*  !          ! */
/*  !__________! */
/*  !  reste   ! */
/*  !__________! */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     05-03-93 : JMB ; Prise en compte MAOVPAR non specifique */
/*     02-10-91 : DGZ ; Reprise et livraison */
/*     17-08-90 : EVT ; Creation version originale. */
/* > */
/* ***********************************************************************
 */

/* Inclusion de MAOVPAR.INC */

/*      CONSTANTES */
/*     INCLUDE MAOVPAR */
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       DEFINIT LES VALEURS LIMITES SPECIFIQUES MACHINE. */

/*     MOTS CLES : */
/*     ----------- */
/*       SYSTEME, LIMITES, VALEURS, SPECIFIQUE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     *** ELLES NE PEUVENT PAS ETRE ECRASEES EN COURS D'EXECUTION. */

/*     *** LES VALEURS D'UNDERFLOW ET D'OVERFLOW NE PEUVENT PAS ETRE */
/*     DEFINIES EN VALEUR DECIMALES (ERREUR A LA COMPILATION D_FLOAT) */
/*     ON LES DEFINIT DONC EN VALEUR HEXADECIMALES */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     02-02-1993 : JMB ; SUPPRESSION DE LA SPECIFICITE DE L'INCLUDE */
/*     29-08-1990 : DGZ ; AJOUT DES REELS X4OVR ET X4UND */
/*     10-08-1990 : DGZ ; AJOUT DES FORMATS FRMR4,FRMR8,FRMR8G */
/*     18-06-1990 : CS/DGZ ; CREATION VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */


/*    DECLARATION DU COMMON POUR LES TYPES NUMERIQUES */


/*    DECLARATION DU COMMON POUR LES TYPES CARACTERES */



/*      VARIABLES LOCALES */

/*      TABLEAUX */

/*      DATAS */
    /* Parameter adjustments */
  --dtable;
  
  /* Function Body */
  
  /* vJMB R8OVR n est pas encore initialise, donc impossible d utiliser DATA
   */
  /*         DATA BUFF / NLONGR * R8OVR / */
  
  /*    l init de BUFF n est faite qu'une fois */
  
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
    AdvApp2Var_SysBase::mcrfill_(&nrest, (char *)buff, (char *)&dtable[1]);
  } else {
    
    /* Amorce & initialisations */
    ioct = 504;
    AdvApp2Var_SysBase::mcrfill_(&ioct, (char *)buff, (char *)&dtable[1]);
    indic = 63;
    
    /* Boucle. La borne sup. est la valeur entiere du logarithme de base 2
     */
    /* de NBENTR/NLONGR. */
    i__1 = (integer) (log((real) (*nbentr) / (float)63.) / log((float)2.))
      ;
    for (ibid = 1; ibid <= i__1; ++ibid) {
      
      AdvApp2Var_SysBase::mcrfill_(&ioct, (char *)&dtable[1], (char *)&dtable[indic + 1]);
      ioct += ioct;
      indic += indic;
      
      /* L10: */
    }
    
    nrest = ( *nbentr - indic ) << 3;
    
    if (nrest > 0) {
      AdvApp2Var_SysBase::mcrfill_(&nrest, (char *)&dtable[1], (char *)&dtable[indic + 1]);
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
  static char chainx[255];

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       RECUPERE LA VALEUR D'UN SYMBOLE DEFINI AU MOMENT DE */
/*       L'INITIALISATION D'UN UTILISATEUR */

/*     MOTS CLES : */
/*     ----------- */
/*       TRADUCTION, SYMBOLE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       CNMSYM : NOM DU SYMBOLE */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       CHAINE : TRADUCTION DU SYMBOLE */
/*       LENGTH : LONGUEUR UTILE DE LA CHAINE */
/*       IERCOD : CODE D'ERREUR */
/*              = 0 : OK */
/*              = 1 : SYMBOLE INEXISTANT */
/*              = 2 : AUTRE ERREUR */

/*     COMMONS UTILISES : */
/*     ------------------ */
/*       NEANT */

/*     REFERENCES APPELEES : */
/*     --------------------- */
/*       LIB$GET_SYMBOL,MACHDIM */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       - CETTE ROUTINE EST SPECIFIQUE VAX */
/*       - EN CAS D'ERREUR (IERCOD>0), CHAINE = ' ' ET LENGTH = 0 */
/*      - SI LA VARIABLE D'ENTREE CNMSYM EST VIDE, LA ROUTINE RENVOIE IERC
OD=1*/

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/* SGI_H  16-04-91 : CSO ; CORRECTION CAS SYMBOLE INEXISTANT ==> IERCOD=1 
*/
/* SGI_   07-01-91 : SVN ; MODIF IERCOD NE DOIT PAS DEPASSER 2 */
/*                        CHAINEVIDE VAUT CARACTERE BLANC */
/*       22-02-88 : DGZ ; CREATION DE LA VERSION ORIGINALE */
/*       07-09-88 : SGI_H : CS; SOUS UNIX SYMBOLE=NOM LOGIQUE = VARIABLE 
*/
/*                           ==> idem MAGTLOG */
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
  /*     TRAITEMENT DES ERREURS */
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
	     long int *iadr, 
	     integer *ier)

{
  /* Initialized data */
  
  static integer ntab = 0;
  
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static integer ideb;
  static doublereal dtab[32000];
  static long int itab[160]	/* was [4][40] */;
  static integer ipre, i__, j, k;
  

/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*        ALLOCATION DYNAMIQUE SUR COMMON */

/*     MOTS CLES : */
/*     ----------- */
/*        . ALLOCDYNAMIQUE,MEMOIRE,COMMON,ALLOC */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        KOP    : (1,2) = (ALLOCATION,DESTRUCTION) */
/*        NOCT   : NOMBRE D'OCTETS */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        IADR   : ADRESSE EN MEMOIRE DU PREMIER OCTET */
/*        *      : */
/*        *      : */
/*        IERCOD : CODE D'ERREUR */

/*        IERCOD = 0 : OK */
/*        IERCOD > 0 : ERREUR GRAVE */
/*        IERCOD < 0 : WARNING */
/*        IERCOD = 1 : DESCRIPTION DE L'ERREUR */
/*        IERCOD = 2 : DESCRIPTION DE L'ERREUR */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*    CRGEN2 */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     Type  Name */
/*           MCRLOCV */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*   ATTENTION .... ITAB ET NTAB NE SONT PAS SAUVEGARDES ENTRE 2 APPELS.. 
*/

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*       04-11-89 : CR ; AJOUT DE OPTIONS /CHECK=NOBOUNDS. */
/*     16-05-89 : DGZ; SUPPRESSION DU COMMON CRGEN2 */
/*     02-05-88 : PP ; AJOUT DE COMMENTAIRES */
/*     20-01-88 : JPF; MAXCOM DE 500 --> 250 */
/*     09-12-85 : BF ; UTILISE LES ROUTINES STANDARDS */
/*     08-11-85 : BF ; BUG SUR DEPLACEMENT TROU */
/*     07-11-85 : BF ; VERSION D'ORIGINE */

/* > */
/* ***********************************************************************
 */

/* JPF  PARAMETER ( MAXNUM = 40 , MAXCOM = 500 * 1024 ) */

/*  ITAB : TABLE DE GESTION DE DTAB, ZONE DE MEMOIRE ALLOUABLE . */
/*  NTAB : NOMBRE D'ALLOCS REALISEES . */
/*     FORMAT DE ITAB : NOMBRE DE REAL*8 ALLOUES , ADRESSE DU 1ER REAL*8 
*/
/*                      , NOCT , ADRESSE VIRTUELLE */

/* PP      COMMON / CRGEN2 / DTAB */


/* ----------------------------------------------------------------------*
 */

  *ier = 0;
  
  /*  ALLOCATION : RECHERCHE D'UN TROU */
  
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
	/*   ON A TROUVE UN TROU */
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
	mcrlocv_((long int)&dtab[ipre - 1], (long int *)iadr);
	itab[(i__ << 2) - 1] = *iadr;
	goto L9900;
      }
      /* L1001: */
    }
    
    /*  PAS DE TROU */
    
    *ier = 3;
    goto L9900;
    
    /* ----------------------------------- */
    /*  DESTRUCTION DE L'ALLOCATION NUM : */
    
  } else {
    i__1 = ntab;
    for (i__ = 1; i__ <= i__1; ++i__) {
      if (*noct != itab[(i__ << 2) - 2]) {
	goto L2001;
      }
      if (*iadr != itab[(i__ << 2) - 1]) {
	goto L2001;
      }
      /*  ON A TROUVE L'ALLOCATION A SUPPRIMER */
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
    
    /*        L'ALLOCATION N'EXISTE PAS */
    
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
				 doublereal *t, 
				 long int *iofset, 
				 integer *iercod)

{
  static integer ibid;
  static doublereal xbid;
  static integer noct, iver, ksys, i__, n, nrang, 
  ibyte, ier;
  static long int iadfd,  iadff, iaddr, loc; /* Les adrresses en long*/
  static integer kop;
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        DESTRUCTION D'UNE ALLOCATION DYNAMIQUE */

/*     MOTS CLES : */
/*     ----------- */
/*        SYSTEME, ALLOCATION, MEMOIRE, DESTRUCTION */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        IUNIT  : NOMBRE D'OCTETS DE L'UNITE D'ALLOCATION */
/*        ISIZE  : NOMBRE D'UNITES DEMANDEES */
/*        T      : ADRESSE DE REFERENCE */
/*        IOFSET : DECALAGE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        IERCOD : CODE D'ERREUR */
/*               = 0 : OK */
/*               = 1 : PB DE DE-ALLOCATION D'UNE ZONE ALLOUEE EN COMMON */
/*               = 2 : LE SYSTEME REFUSE LA DEMANDE DE DE-ALLOCATION */
/*               = 3 : L'ALLOCATION A DETRUIRE N'EXISTE PAS. */

/*     COMMONS UTILISES   : */
/*     ---------------- */


/*     REFERENCES APPELEES   : */
/*     ---------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     1) UTILISATEUR */
/*        ----------- */

/*       MCRDELT FAIT UNE LIBERATION DE ZONE MEMOIRE ALLOUEE */
/*       PAR LA ROUTINE MCRRQST (OU CRINCR) */

/*       LA SIGNIFICATION DES ARGUMENTS EST LA MEME QUE MCRRQST */

/* *** ATTENTION : */
/*     ----------- */
/*     IERCOD=2 : CAS OU LE SYSTEME NE PEUT LIBERER LA MEMOIRE ALLOUEE, */
/*     LE MESSAGE SUIVANT APPARAIT SYSTEMATIQUEMENT SUR LA CONSOLE */
/*     ALPHA : */
/*     "Le systeme refuse une destruction d'allocation de memoire" */

/*     IERCOD=3 CORRESPOND AU CAS OU LES ARGUMENTS SONT MAUVAIS */
/*     (ILS NE PERMETTENT PAS DE RECONNAITRE L'ALLOCATION DANS LA TABLE) 
*/

/*     Lorsque l'allocation est detruite, l'IOFSET correspondant est mis 
*/
/*     a 2 147 483 647. Ainsi, si on accede au tableau via l'IOFSET, un */
/*     trap se produira. Ceci permet de verifier qu'on ne se sert plus */
/*     d'une zone de memoire qu'on a liberee. Cette verification n'est */
/*     valable que si c'est le meme sous-programme qui utilise et qui */
/*     detruit l'allocation. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*    05-03-93 : FCR : DMSF52088 : On prend les memes et on recommence ...
*/
/*                      IERCOD = 3 et I4UND. */
/*     22-02-93 : FCR : Pour TOYOTA : Desactivation de l'affectation de */
/*                      l'IOFSET a I4UND et suppression de IERCOD = 3. */
/*     10-02-93 : FCR ; DMSFRO253 : Ajout d'un appel a MAERMSG si IERCOD 
*/
/*                      = 3 */
/*     22-01-93 : FCR ; DMSF52088 : Ajout de l'IERCOD 3. */
/*                      Ajout de l'IOFSET mis a I4UND lorsque */
/*                      l'allocation est detruite. */
/*     08-10-92 : FCR ; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*     08-09-92 : FCR ; Optimisation */
/*     18-11-91 : DGZ ; APPEL MACRCHK EN PHASE DE DEVELOPPEMENT */
/*     23-09-91 : DGZ ; RENOMME EN .FOR ET MODIFS DE COMMENTAIRES */
/*     14-05-91 : DGZ ; SUPPRIME L'OPTION /CHECK=NBOUNDS */
/*     21-08-90 : DGZ ; AFFICHAGE DU TRACE-BACK EN PHASE DE PRODUCTION */
/*                      ET RENOMME EN .VAX */
/*     22-12-89 : DGZ ; CORRECTION DE L'EN-TETE */
/*     04-11-89 : CR ; AJOUT DE OPTIONS /CHECK=NOBOUNDS. */
/*     11-05-89 : DGZ; CONTROLE DEBORDEMENT DE MEMOIRE */
/*     27-06-88 : PP ; VIRE 9001 INUTILISE */
/*     PP 26.2.88 CHANGE LE VFORMA EN MACRMSG, POUR USAGE DANS C */
/*     09-01-87 : BF ; ALLOCATIONS SYSTEME */
/*     03-11-86 : BF ; RAJOUTE STATISTIQUES */
/*     09-12-85 : BF ; UTILISE LES ROUTINES STANDARDS */
/*     09-12-85 : BF ; PLUS D'ERREUR SI L'ALLOCATION N'EXISTE PAS */
/*     07-11-85 : BF ; VERSION D'ORIGINE */
/* > */
/* ***********************************************************************
 */

/* COMMON DES PARAMETRES */

/* COMMON DES STATISTIQUES */
/*     INCLUDE MCRGENE */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TABLE DE GESTION DES ALLOCATIONS DYNAMIQUES DE MEMOIRE */

/*     MOTS CLES : */
/*     ----------- */
/*        SYSTEME, MEMOIRE, ALLOCATION */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       23-11-93 : FCR; AF93125U3A007 : MAXCR 200 --> 1000 */
/*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*       25-09-91 : DGZ; AJOUT INFOs SUPPLEMENTAIREs POUR GESTION FLAGS */
/*       18-01-91 : DGZ; MAXCR PASSE DE 100 A 200 SUR DEMANDE GDD */
/*       18-05-90 : DGZ; DECLARATION TYPE INTEGER POUR MAXCR */
/*       20-06-88 : PP ; MAXCR PASSE DE 50 A 100, SUR DEMANDE OG */
/*                       + AJOUT DE COMMENTAIRES */
/*       26-02-88 : PP ; MAXCR PASSE DE 40 A 50, SUR DEMANDE AB . */
/*       15-04-85 : BF ; VERSION D'ORIGINE */
/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE DES ALLOCS EXISTANTES, AVEC POUR CHACUNE : */
/*         1 : NIVEAU DE PROTECTION (0=PAS PROTEGE, AUTRE=PROTEGE) */
/*             (PROTEGE SIGNIFIE PAS DETRUIT PAR CRRSET .) */
/*         2 : UNITE D'ALLOCATION */
/*         3 : NB D'UNITES ALLOUEES */
/*         4 : ADRESSE DE REFERENCE DU TABLEAU */
/*         5 : IOFSET */
/*         6 : NUMERO ALLOCATION STATIQUE */
/*         7 : Taille demandee en allocation */
/*         8 : adresse du debut de l'allocation */
/*         9 : Taille de la ZONE UTILISATEUR */
/*        10 : ADRESSE DU FLAG DE DEBUT */
/*        11 : ADRESSE DU FLAG DE FIN */
/*        12 : Rang de creation de l'allocation */

/*   NDIMCR : NBRE DE DONNEES DE CHAQUE ALLOC DANS ICORE */
/*   NCORE : NBRE D'ALLOCS EN COURS */
/*   LPROT : COMMUNICATION ENTRE CRPROT ET MCRRQST, REMIS A 0 PAR MCRRQST 
*/
/*   FLAG  : VALEUR DU FLAG UTILISE POUR LES DEBORDEMENTS */



/* ----------------------------------------------------------------------*
 */


/*     20-10-86 : BF ; VERSION D'ORIGINE */


/*     NRQST : NOMBRE D'ALLOCATIONS EFFECTUEES */
/*     NDELT : NOMBRE DE LIBERATIONS EFFECTUEES */
/*     NBYTE : NOMBRE TOTAL D'OCTETS DES ALLOCATIONS */
/*     MBYTE : NOMBRE MAXI D'OCTETS */

    /* Parameter adjustments */
    --t;

    /* Function Body */
    *iercod = 0;

/* RECHERCHE DANS MCRGENE */

    n = 0;
    mcrlocv_((long int)&t[1], (long int *)&loc);

    for (i__ = mcrgene_.ncore; i__ >= 1; --i__) {
	if (*iunit == mcrgene_.icore[i__ * 12 - 11] && *isize == 
		mcrgene_.icore[i__ * 12 - 10] && loc == mcrgene_.icore[i__ *
		 12 - 9] && *iofset == mcrgene_.icore[i__ * 12 - 8]) {
	    n = i__;
	    goto L1100;
	}
/* L1001: */
    }
L1100:

/* SI L'ALLOCATION N'EXISTE PAS , ON SORT */

    if (n <= 0) {
	goto L9003;
    }

/* ALLOCATION RECONNUE : ON RECUPERE LES AUTRES INFOS */

    ksys = mcrgene_.icore[n * 12 - 7];
    ibyte = mcrgene_.icore[n * 12 - 6];
    iaddr = mcrgene_.icore[n * 12 - 5];
    iadfd = mcrgene_.icore[n * 12 - 3];
    iadff = mcrgene_.icore[n * 12 - 2];
    nrang = mcrgene_.icore[n * 12 - 1];

/*     Controle des flags */

    madbtbk_(&iver);
    if (iver == 1) {
	macrchk_();
    }

    if (ksys <= 1) {
/* DE-ALLOCATION SUR COMMON */
	kop = 2;
	mcrcomm_(&kop, &ibyte, &iaddr, &ier);
	if (ier != 0) {
	    goto L9001;
	}
    } else {
/* DE-ALLOCATION SYSTEME */
	mcrfree_((integer *)&ibyte, (uinteger *)&iaddr, (integer *)&ier);
	if (ier != 0) {
	    goto L9002;
	}
    }

/* APPEL PERMETTANT LE CANCEL WATCH AUTOMATQUE PAR LE DEBUGGER */

    macrclw_(&iadfd, &iadff, &nrang);

/* MISE A JOUR DES STATISTIQUES */
    if (ksys <= 1) {
	i__ = 1;
    } else {
	i__ = 2;
    }
    ++mcrstac_.ndelt[i__ - 1];
    mcrstac_.nbyte[i__ - 1] -= mcrgene_.icore[n * 12 - 11] * 
	    mcrgene_.icore[n * 12 - 10];

/* SUPPRESSION DES PARAMETRES DANS MCRGENE */
    if (n < 1000) {
/*	noct = (mcrgene_1.ncore - n) * 48; */
        noct = (mcrgene_.ncore - n) * 12 * sizeof(long int);
	AdvApp2Var_SysBase::mcrfill_((integer *)&noct, 
				     (char *)&mcrgene_.icore[(n + 1) * 12 - 12], 
				     (char *)&mcrgene_.icore[n * 12 - 12]);
    }
    --mcrgene_.ncore;

/* *** Mise a l'overflow de l'IOFSET */
    *iofset = 2147483647;
    goto L9900;

/* ----------------------------------------------------------------------*
 */
/*     TRAITEMENT DES ERREURS */

L9001:
/*  REFUS DE DE-ALLOCATION PAR LA ROUTINE 'MCRCOMM' (ALLOC DS COMMON) */
    *iercod = 1;
    AdvApp2Var_SysBase::maermsg_("MCRDELT", iercod, 7L);
    maostrd_();
    goto L9900;

/*  REFUS DE DE-ALLOCATION PAR LE SYSTEME */
L9002:
    *iercod = 2;
    AdvApp2Var_SysBase::maermsg_("MCRDELT", iercod, 7L);
    macrmsg_("MCRDELT", iercod, &ibid, &xbid, " ", 7L, 1L);
    maostrd_();
    goto L9900;

/* ALLOCATION INEXISTANTE */
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
C     FONCTION :
C     ----------
C        Transfert une zone memoire dans une autre en gerant les 
C        recouvrements
C
C     MOTS CLES :
C     -----------
C        MANIPULATION, MEMOIRE, TRANSFERT, CARACTERE
C
C     ARGUMENTS D'ENTREE :
C     ------------------
C        nb_car    : integer*4  nombre de caracteres a transferer.
C        source    : zone memoire source.
C             
C     ARGUMENTS DE SORTIE :
C     -------------------
C        dest      : zone memeoire destination.
C
C     COMMONS UTILISES :
C     ----------------
C
C     REFERENCES APPELEES :
C     -------------------
C
C     DEMSCRIPTION/REMARQUES/LIMITATIONS :
C     -----------------------------------
C        Routine portable UNIX (SGI, ULTRIX, BULL)
C
C$    HISTORIQUE DES MODIFICATIONS :
C     ----------------------------
C     24/01/92 : DGZ ; Recuperation de la version BULL
C>
C**********************************************************************
*/

//=======================================================================
//function : AdvApp2Var_SysBase::mcrfill_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrfill_(integer *size, 
				 char *tin, 
				 char *tout)

{
 
  if (mcrfill_ABS(tout-tin) >= *size)
    memcpy( tout, tin, *size);
  else if (tin > tout)
    {
      register integer n = *size;
      register char *jmin=tin;
      register char *jmout=tout;
      while (n-- > 0) *jmout++ = *jmin++;
    }
  else
    {
      register integer n = *size;
      register char *jmin=tin+n;
      register char *jmout=tout+n;
      while (n-- > 0) *--jmout = *--jmin;
    }
  return 0;
}


/*........................................................................*/
/*                                                                        */
/*   FONCTION :                                                           */
/*   ----------                                                           */
/*               Routines de gestion de la memoire dynamique.             */
/*                                                                        */
/*             Routine mcrfree                                             */
/*             --------------                                             */
/*                                                                        */
/*             Desallocation d'une zone memoire.                          */
/*                                                                        */
/*             CALL MCRFREE (IBYTE,IADR,IER)                               */
/*                                                                        */
/*             IBYTE INTEGER*4 : Nombre d'Octetes a Liberer               */
/*                                                                        */
/*             IADR POINTEUR   : Adresse de Depart                        */
/*                                                                        */
/*             IER  INTEGER*4  : Code de Retour                           */
/*                                                                        */
/*                                                                        */
/*   MOTS CLES :                                                          */
/*   -----------                                                          */
/*                                                                        */
/*   ARGUMENTS D'ENTREE :                                                 */
/*   --------------------                                                 */
/*                                                                        */
/*   ARGUMENTS DE SORTIE :                                                */
/*   ---------------------                                                */
/*                                                                        */
/*   COMMONS UTILISES :                                                   */
/*   ------------------                                                   */
/*                                                                        */
/*   REFERENCES APPELEES :                                                */
/*   ---------------------                                                */
/*                                                                        */
/*   DEMSCRIPTION/REMARQUES/LIMITATIONS :                                  */
/*   -----------------------------------                                  */
/*                                                                        */
/*                      ** SPECIFIQUE SPS9 **                             */
/*                                                                        */
/*                                                                        */
/*   HISTORIQUE DES MODIFICATIONS :                                       */
/*   ------------------------------                                       */
/*                                                                        */
/*   07-03-86 : FS; INSERTION DE L'ENTETE STANDARD C                      */
/*   16-09-86 : FS; MODIFICATIONS PASSAGE NIVEAU INFERIEUR                */
/* SGI_H 05-04-90 : ACT ; ECLATEMENT DU PACKAGE CRALOC                    */
/*                                                                        */
/*........................................................................*/
/*                                                                        */

//=======================================================================
//function : mcrfree_
//purpose  : 
//=======================================================================
int mcrfree_(integer *,//ibyte,
	     uinteger *iadr,
	     integer *ier)

{
  *ier=0;
  free((void*)*iadr);
  if ( !*iadr ) *ier = 1;
  return 0;
}

/*........................................................................*/
/*                                                                        */
/*   FONCTION :                                                           */
/*   ----------                                                           */
/*               Routines de gestion de la memoire dynamique.             */
/*                                                                        */
/*             Routine mcrgetv                                             */
/*             --------------                                             */
/*                                                                        */
/*               Demande d'allocation de memoire.                         */
/*                                                                        */
/*               CALL MCRGETV(IBYTE,IADR,IER)                              */
/*                                                                        */
/*               IBYTE (INTEGER*4) Nombre de Bytes d'allocation           */
/*                                 demandee                               */    
/*                                                                        */
/*               IADR   (INTEGER*4) : Resultat.                           */
/*                                                                        */
/*               IER (INTEGER*4)    : Code d'erreur :                     */
/*                                                                        */
/*                   = 0  ==> OK                                          */
/*                   = 1  ==> Allocation impossible                       */
/*                   = -1 ==> Ofset > 2**31 - 1                           */
/*                                                                        */
/*   MOTS CLES :                                                          */
/*   -----------                                                          */
/*                                                                        */
/*   ARGUMENTS D'ENTREE :                                                 */
/*   --------------------                                                 */
/*                                                                        */
/*   ARGUMENTS DE SORTIE :                                                */
/*   ---------------------                                                */
/*                                                                        */
/*   COMMONS UTILISES :                                                   */
/*   ------------------                                                   */
/*                                                                        */
/*   REFERENCES APPELEES :                                                */
/*   ---------------------                                                */
/*                                                                        */
/*   DEMSCRIPTION/REMARQUES/LIMITATIONS :                                  */
/*   -----------------------------------                                  */
/*                                                                        */
/*                      ** SPECIFIQUE SPS9 **                             */
/*                                                                        */
/*                                                                        */
/*   HISTORIQUE DES MODIFICATIONS :                                       */
/*   ------------------------------                                       */
/*                                                                        */
/*   07-03-86 : FS; INSERTION DE L'ENTETE STANDARD C                      */
/*   16-09-86 : FS; MODIFICATIONS PASSAGE NIVEAU INFERIEUR                */
/*SGI_H 05-04-90 : ACT ; ECLATEMENT DU PACKAGE CRALOC                     */
/*                                                                        */
/*........................................................................*/

//=======================================================================
//function : mcrgetv_
//purpose  : 
//=======================================================================
int mcrgetv_(integer *sz,
	     uinteger *iad,
	     integer *ier)                                            

{
  
  *ier = 0;
  *iad = (uinteger)malloc(*sz);
  if ( !*iad ) *ier = 1;
  return 0;
}


//=======================================================================
//function : mcrlist_
//purpose  : 
//=======================================================================
int mcrlist_(integer *ier)

{
  /* System generated locals */
  integer i__1;
  
  /* Builtin functions */
  
  /* Local variables */
  static char cfmt[1];
  static doublereal dfmt;
  static integer ifmt, i__, nufmt, ntotal;
  static char subrou[7];
  

/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*   IMPRESSION DU TABLEAU DES ALLOCATIONS DYNAMIQUES EN COURS */

/*     MOTS CLES : */
/*     ----------- */
/*   SYSTEME, ALLOCATION, MEMOIRE, LISTE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        . NEANT */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        *      : */
/*        *      : */
/*        IERCOD : CODE D'ERREUR */

/*        IERCOD = 0 : OK */
/*        IERCOD > 0 : ERREUR GRAVE */
/*        IERCOD < 0 : WARNING */
/*        IERCOD = 1 : DESCRIPTION DE L'ERREUR */
/*        IERCOD = 2 : DESCRIPTION DE L'ERREUR */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*    MCRGENE     VFORMT */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     Type  Name */
/*           VFORMA */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*         . NEANT */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     04-08-92 : HCE ; CORRECTION CTLCODE */
/*     10-06-92 : FCR ; CORRECTION CTLCODE */
/*     16-09-1991: FCR ; Suppression INCLUDE VFORMT */
/*      22-12-89 : DGZ ; CORRECTION DE L'EN-TETE */
/*     PP 26.2.88 MIS VFORMA A LA PLACE DE MCRLIST */
/*     04-11-85 : BF ; VERSION D'ORIGINE */

/* > */
/* ***********************************************************************
 */

/*     INCLUDE MCRGENE */
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TABLE DE GESTION DES ALLOCATIONS DYNAMIQUES DE MEMOIRE */

/*     MOTS CLES : */
/*     ----------- */
/*        SYSTEME, MEMOIRE, ALLOCATION */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       23-11-93 : FCR; AF93125U3A007 : MAXCR 200 --> 1000 */
/*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*       25-09-91 : DGZ; AJOUT INFOs SUPPLEMENTAIREs POUR GESTION FLAGS */
/*       18-01-91 : DGZ; MAXCR PASSE DE 100 A 200 SUR DEMANDE GDD */
/*       18-05-90 : DGZ; DECLARATION TYPE INTEGER POUR MAXCR */
/*       20-06-88 : PP ; MAXCR PASSE DE 50 A 100, SUR DEMANDE OG */
/*                       + AJOUT DE COMMENTAIRES */
/*       26-02-88 : PP ; MAXCR PASSE DE 40 A 50, SUR DEMANDE AB . */
/*       15-04-85 : BF ; VERSION D'ORIGINE */
/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE DES ALLOCS EXISTANTES, AVEC POUR CHACUNE : */
/*         1 : NIVEAU DE PROTECTION (0=PAS PROTEGE, AUTRE=PROTEGE) */
/*             (PROTEGE SIGNIFIE PAS DETRUIT PAR CRRSET .) */
/*         2 : UNITE D'ALLOCATION */
/*         3 : NB D'UNITES ALLOUEES */
/*         4 : ADRESSE DE REFERENCE DU TABLEAU */
/*         5 : IOFSET */
/*         6 : NUMERO ALLOCATION STATIQUE */
/*         7 : Taille demandee en allocation */
/*         8 : adresse du debut de l'allocation */
/*         9 : Taille de la ZONE UTILISATEUR */
/*        10 : ADRESSE DU FLAG DE DEBUT */
/*        11 : ADRESSE DU FLAG DE FIN */
/*        12 : Rang de creation de l'allocation */

/*   NDIMCR : NBRE DE DONNEES DE CHAQUE ALLOC DANS ICORE */
/*   NCORE : NBRE D'ALLOCS EN COURS */
/*   LPROT : COMMUNICATION ENTRE CRPROT ET MCRRQST, REMIS A 0 PAR MCRRQST 
*/
/*   FLAG  : VALEUR DU FLAG UTILISE POUR LES DEBORDEMENTS */



/* ----------------------------------------------------------------------*
 */


/* ----------------------------------------------------------------------*
 */

    *ier = 0;
    //__s__copy(subrou, "MCRLIST", 7L, 7L);

/*     ECRITURE DE L'EN TETE */

    nufmt = 1;
    ifmt = mcrgene_.ncore;
    macrmsg_(subrou, &nufmt, &ifmt, &dfmt, cfmt, 7L, 1L);

    ntotal = 0;

    i__1 = mcrgene_.ncore;
    for (i__ = 1; i__ <= i__1; ++i__) {
	nufmt = 2;
	ifmt = mcrgene_.icore[i__ * 12 - 11] * mcrgene_.icore[i__ * 12 - 10]
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
int mcrlocv_(long int t,
	     long int *l)

{
  *l = t;
  return 0 ;
}

//=======================================================================
//function : AdvApp2Var_SysBase::mcrrqst_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mcrrqst_(integer *iunit, 
				 integer *isize, 
				 doublereal *t, 
				 long int *iofset, 
				 integer *iercod)

{

  integer i__1, i__2;

  /* Local variables */
  static doublereal dfmt;
  static integer ifmt, iver;
  static char subr[7];
  static integer ksys , ibyte, irest, isyst, ier;
  static long int iadfd, iadff, iaddr,lofset, loc;
  static integer izu;

  
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*       REALISATION D'UNE ALLOCATION DYNAMIQUE DE MEMOIRE */

/*     MOTS CLES : */
/*     ----------- */
/*       SYSTEME, ALLOCATION, MEMOIRE, REALISATION */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        IUNIT  : NOMBRE D'OCTEST DE L'UNITE D'ALLOCATION */
/*        ISIZE  : NOMBRE D'UNITES DEMANDEES */
/*        T      : ADRESSE DE REFERENCE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        IOFSET : DECALAGE */
/*        IERCOD : CODE D'ERREUR, */
/*               = 0 : OK */
/*               = 1 : NBRE MAXI D'ALLOCS ATTEINT */
/*               = 2 : ARGUMENTS INCORRECTS */
/*               = 3 : REFUS D'ALLOCATION DYNAMIQUE */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*       MCRGENE, MCRSTAC */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */
/*       MACRCHK, MACRGFL, MACRMSG, MCRLOCV,MCRCOMM, MCRGETV */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     1) UTILISATEUR */
/*     -------------- */

/* T EST L'ADRESSE D'UN TABLEAU BANAL,IOFSET REPRESENTE LE DEPLACEMENT EN 
*/
/* UNITES DE IUNIT OCTETS ENTRE LA ZONE ALLOUEE ET LE TABLEAU T */
/* IERCOD=0 SIGNALE QUE L'ALLOCATION S'EST BIEN DEROULEE ,TOUTE AUTRE */
/* VALEUR INDIQUE UNE ANOMALIE. */

/*     EXEMPLE : */
/*          SOIT LA DECLARATION REAL*4 T(1), DONC IUNIT=4 . */
/*          L'APPEL A MCRRQST FAIT UNE ALLOCATION DYNAMIQUE */
/*          ET DONNE UNE VALEUR A LA VARIABLE IOFSET, */
/*          SI L'ON VEUT ECRIRE 1. DANS LA CINQUIEME ZONE REAL*4 */
/*          AINSI ALLOUEE ,FAIRE: */
/*          T(5+IOFSET)=1. */

/*     CAS D'ERREURS : */
/*     --------------- */

/*     IERCOD=1 : NOMBRE MAXI D'ALLOCATION ATTEINT (ACTUELLEMENT 200) */
/*     ET LE MESSAGE SUIVANT APPARAIT SUR LA CONSOLE ALPHA : */
/*     "Le nombre maxi d'allocation de memoire est atteint : ,N" */

/*     IERCOD=2 : ARGUMENT IUNIT INCORRECT CAR DIFFERENT DE 1,2,4 OU 8 */
/*     ET LE MESSAGE SUIVANT APPARAIT SUR LA CONSOLE ALPHA : */
/*     "Unite d'allocation invalide : ,IUNIT" */

/*     IERCOD=3 : REFUS D'ALLOCATION DYNAMIQUE (PLUS DE PLACE MEMOIRE) */
/*     ET LE MESSAGE SUIVANT APPARAIT SUR LA CONSOLE ALPHA : */
/*    "Le systeme refuse une allocation dynamique de memoire de N octets" 
*/
/*     AVEC UN AFFICHAGE COMPLET DE TOUTES LES ALLOCATIONS EFFECTUEES */
/*     JUSQU'A PRESENT. */


/*     2) CONCEPTEUR */
/*     -------------- */

/* MCRRQST FAIT UNE ALLOCATION DYNAMIQUE DE MEMOIRE VIRTUELLE SUR LA BASE 
*/
/* D'ENTITES DE 8 OCTETS (QUADWORDS) ,BIEN QUE L'ALLOCATION SOIT DEMANDEE 
*/
/* PAR UNITES DE IUNIT OCTETS (1,2,4,8). */

/* LA QUANTITE DEMANDEE EST IUNIT*ISIZE OCTETS,CETTE VALEUR EST ARRONDIE 
*/
/* POUR QUE L'ALLOCATION SOIT UN NOMBRE ENTIER DE QUADWORDS. */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     14-04-94 : JMB; Suppression message ALLOC < 16 octets */
/*     08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*     23-09-91 : DGZ; REND LA ROUTINE PORTABLE */
/*     22-08-90 : DGZ; CORRECTION DE L'EN-TETE */
/*     21-08-90 : DGZ; AFFICHAGE DU TRACE_BACK EN PHASE DE PRODUCTION */
/*     22-12-89 : DGZ; CORRECTION DE L'EN-TETE */
/*     19-05-89 : DGZ; AJOUT DOUBLE MOT SI DECALAGE ET SUPP APPEL ACRVRF 
*/
/*     17-05-89 : DGZ; CALCUL DE IOFSET DANS LE CAS OU IL EST NEGATIF */
/*     11-05-89 : DGZ; CONTROLE DES ECRASEMENTS DE ZONE MEMOIRE */
/*     04-05-88 : PP ; CHANGE MOVFLW EN MAOVERF */
/*     23-03-88 : PP ; CORR DE PASSAGES D'ARGUMENTS DANS MACRMSG ET MOVFLW
 */
/*     26.2.88 PP VIRE VFORMA, ET MIS MACRMSG */
/*     22.2.88 : PP : CHANGE I*4 EN I ET R*8 EN D P, AJOUT DE ISYST */
/*                   ,ET VIRE LE TEST SUR IBB, A REMETTRE AVANT LIVRAISON 
*/
/*     09-10-1987 : Initialisation a OVERFLOW si IBB <> 0 JJM */
/*     10-04-87 : BF ; ALLOCATIONS CADREES SUR DOUBLES MOTS */
/*     07-11-85 : BF ; VERSION D'ORIGINE */

/* > */
/* ***********************************************************************
 */

/* COMMON DES PARAMETRES */
/* COMMON DES INFORMATIONS SUR LES STATISTIQUES */
/*     INCLUDE MCRGENE */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TABLE DE GESTION DES ALLOCATIONS DYNAMIQUES DE MEMOIRE */

/*     MOTS CLES : */
/*     ----------- */
/*        SYSTEME, MEMOIRE, ALLOCATION */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       23-11-93 : FCR; AF93125U3A007 : MAXCR 200 --> 1000 */
/*       08-10-92 : FCR; DMSFRO131 : Modif pour DEBUG-ALLOC */
/*       25-09-91 : DGZ; AJOUT INFOs SUPPLEMENTAIREs POUR GESTION FLAGS */
/*       18-01-91 : DGZ; MAXCR PASSE DE 100 A 200 SUR DEMANDE GDD */
/*       18-05-90 : DGZ; DECLARATION TYPE INTEGER POUR MAXCR */
/*       20-06-88 : PP ; MAXCR PASSE DE 50 A 100, SUR DEMANDE OG */
/*                       + AJOUT DE COMMENTAIRES */
/*       26-02-88 : PP ; MAXCR PASSE DE 40 A 50, SUR DEMANDE AB . */
/*       15-04-85 : BF ; VERSION D'ORIGINE */
/* > */
/* ***********************************************************************
 */

/*   ICORE : TABLE DES ALLOCS EXISTANTES, AVEC POUR CHACUNE : */
/*         1 : NIVEAU DE PROTECTION (0=PAS PROTEGE, AUTRE=PROTEGE) */
/*             (PROTEGE SIGNIFIE PAS DETRUIT PAR CRRSET .) */
/*         2 : UNITE D'ALLOCATION */
/*         3 : NB D'UNITES ALLOUEES */
/*         4 : ADRESSE DE REFERENCE DU TABLEAU */
/*         5 : IOFSET */
/*         6 : NUMERO ALLOCATION STATIQUE */
/*         7 : Taille demandee en allocation */
/*         8 : adresse du debut de l'allocation */
/*         9 : Taille de la ZONE UTILISATEUR */
/*        10 : ADRESSE DU FLAG DE DEBUT */
/*        11 : ADRESSE DU FLAG DE FIN */
/*        12 : Rang de creation de l'allocation */

/*   NDIMCR : NBRE DE DONNEES DE CHAQUE ALLOC DANS ICORE */
/*   NCORE : NBRE D'ALLOCS EN COURS */
/*   LPROT : COMMUNICATION ENTRE CRPROT ET MCRRQST, REMIS A 0 PAR MCRRQST 
*/
/*   FLAG  : VALEUR DU FLAG UTILISE POUR LES DEBORDEMENTS */



/* ----------------------------------------------------------------------*
 */


/*     20-10-86 : BF ; VERSION D'ORIGINE */


/*     NRQST : NOMBRE D'ALLOCATIONS EFFECTUEES */
/*     NDELT : NOMBRE DE LIBERATIONS EFFECTUEES */
/*     NBYTE : NOMBRE TOTAL D'OCTETS DES ALLOCATIONS */
/*     MBYTE : NOMBRE MAXI D'OCTETS */

/* ----------------------------------------------------------------------*
 */

    /* Parameter adjustments */
    --t;

    /* Function Body */
    *iercod = 0;

    if (mcrgene_.ncore >= 1000) {
	goto L9001;
    }
    if (*iunit != 1 && *iunit != 2 && *iunit != 4 && *iunit != 8) {
	goto L9002;
    }

/* Calcul de la taille demandee par l'utilsateur */
    ibyte = *iunit * *isize;

/* Recheche le type de version (Phase de Production ou Version Client) */
    madbtbk_(&iver);

/* Controle sur la taille allouee en phase de Production */

    if (iver == 1) {

	if (ibyte == 0) {
	    //s__wsle(&io___3);
	    //do__lio(&c__9, &c__1, "Demande d'allocation nulle", 26L);
	    AdvApp2Var_SysBase::e__wsle();
	    maostrb_();
	} else if (ibyte >= 4096000) {
	    //s__wsle(&io___4);
	    //do__lio(&c__9, &c__1, "Demande d'allocation superieure a 4 Mega-Octets : ", 50L);
	    //do__lio(&c__3, &c__1, (char *)&ibyte, (ftnlen)sizeof(integer));
	    AdvApp2Var_SysBase::e__wsle();
	    maostrb_();
	}

    }

/* ON CALCUL LA TAILLE DE LA ZONE UTILSATEUR (IZU) */
/*     . ajout taille demandee par l'utilisateur (IBYTE) */
/*     . ajout d'un delta pour alignement avec la base */
/*     . on arrondit au multiple de 8 superieur */

  mcrlocv_((long int)&t[1], (long int *)&loc);
    izu = ibyte + loc % *iunit;
    irest = izu % 8;
    if (irest != 0) {
	izu = izu + 8 - irest;
    }

/* ON CALCUL LA TAILLE QUI VA ETRE DEMANDEE A LA PRIMITIVE D'ALLOC */
/*     . ajout de la taille de la zone utilisateur */
/*     . ajout de 8 pour un alignement de l'adresse de debut */
/*       d'allocation sur un multiple de 8 de facon a pouvoir */
/*       poser des flags en Double Precision sans pb d'alignement */
/*     . ajout de 16 octets pour les deux flags */

    ibyte = izu + 24;

/* DEMANDE D'ALLOCATION */

    isyst = 0;
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
    ksys = 2;
    mcrgetv_((integer *)&ibyte, (uinteger *)&iaddr, (integer *)&ier);
    if (ier != 0) {
	goto L9003;
    }
/*      ENDIF */

/* CALCUL DES ADRESSES DES FLAGS */

    iadfd = iaddr + 8 - iaddr % 8;
    iadff = iadfd + 8 + izu;

/* CALCUL DE L'OFFSET UTILISATEUR : */
/*     . difference entre l'adresse de depart utilisateur et */
/*       l'adresse de la base */
/*     . convertit cette difference dans l'unite utilisateur */

    lofset = iadfd + 8 + loc % *iunit - loc;
    *iofset = lofset / *iunit;

/* Si phase de production alors controle des flags */
    if (iver == 1) {
	macrchk_();
    }

/*     MISE EN PLACE DES FLAGS */
/*     . le premier flag est mis en IADFD et le second en IADFF */
/*     . Si phase de production alors on met a overflow la ZU */
    macrgfl_(&iadfd, &iadff, &iver, &izu);

/* RANGEMENT DES PARAMETRES DANS MCRGENE */

    ++mcrgene_.ncore;
    mcrgene_.icore[mcrgene_.ncore * 12 - 12] = mcrgene_.lprot;
    mcrgene_.icore[mcrgene_.ncore * 12 - 11] = *iunit;
    mcrgene_.icore[mcrgene_.ncore * 12 - 10] = *isize;
    mcrgene_.icore[mcrgene_.ncore * 12 - 9] = loc;
    mcrgene_.icore[mcrgene_.ncore * 12 - 8] = *iofset;
    mcrgene_.icore[mcrgene_.ncore * 12 - 7] = ksys;
    mcrgene_.icore[mcrgene_.ncore * 12 - 6] = ibyte;
    mcrgene_.icore[mcrgene_.ncore * 12 - 5] = iaddr;
    mcrgene_.icore[mcrgene_.ncore * 12 - 4] = mcrgene_.ncore;
    mcrgene_.icore[mcrgene_.ncore * 12 - 3] = iadfd;
    mcrgene_.icore[mcrgene_.ncore * 12 - 2] = iadff;
    mcrgene_.icore[mcrgene_.ncore * 12 - 1] = mcrgene_.ncore;

    mcrgene_.lprot = 0;

/* APPEL PERMETTANT UNE MISE EN PLACE AUTO DU SET WATCH PAR LE DEBUGGER */

    macrstw_((integer *)&iadfd, (integer *)&iadff, (integer *)&mcrgene_.ncore);

/* STATISTIQUES */

    ++mcrstac_.nrqst[ksys - 1];
    mcrstac_.nbyte[ksys - 1] += mcrgene_.icore[mcrgene_.ncore * 12 - 11] * 
	    mcrgene_.icore[mcrgene_.ncore * 12 - 10];
/* Computing MAX */
    i__1 = mcrstac_.mbyte[ksys - 1], i__2 = mcrstac_.nbyte[ksys - 1];
    mcrstac_.mbyte[ksys - 1] = max(i__1,i__2);

    goto L9900;

/* ----------------------------------------------------------------------*
 */
/*   TRAITEMENT DES ERREURS */

/*  NBRE MAXI D'ALLOC ATTEINT : */
L9001:
    *iercod = 1;
    ifmt = 1000;
    //__s__copy(subr, "MCRRQST", 7L, 7L);
    macrmsg_(subr, iercod, &ifmt, &dfmt, " ", 7L, 1L);
    maostrd_();
    goto L9900;

/*  AURGUMENTS INCORRECTS */
L9002:
    *iercod = 2;
    ifmt = *iunit;
    //__s__copy(subr, "MCRRQST", 7L, 7L);
    macrmsg_(subr, iercod, &ifmt, &dfmt, " ", 7L, 1L);
    goto L9900;

/* LE SYSTEME REFUSE L'ALLOCATION */
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
C     FONCTION : CALL MIRAZ(LENGTH,ITAB)
C     ---------- 
C
C     EFFECTUE UNE REMISE A ZERO D'UN TABLEAU DE LOGICAL OU D'INTEGER.
C
C     MOTS CLES :
C     -----------
C        RAZ INTEGER
C
C     ARGUMENTS D'ENTREE :
C     ------------------
C               LENGTH : NOMBRE D'OCTETS A TRANSFERER
C               ITAB   : NOM DU TABLEAU
C
C     ARGUMENTS DE SORTIE :
C     --------------------
C               ITAB   : NOM DU TABLEAU REMIS A ZERO
C
C     COMMONS UTILISES   :
C     ----------------
C
C     REFERENCES APPELEES   :
C     -----------------------
C
C     DEMSCRIPTION/REMARQUES/LIMITATIONS :
C     -----------------------------------
C
C           Portable VAX-SGI
C
C$    HISTORIQUE DES MODIFICATIONS   :
C     --------------------------------
C
C     05-04-93 : JMB ; portabilite VAX SGI
C     06-01-86 : FS,GFa; CREATION (ADAPTATION VAX)
CSGI_H 16-02-89 : FS ; Optimisation en C en utilisant memset
C
C>
C***********************************************************************
*/
//=======================================================================
//function : AdvApp2Var_SysBase::miraz_
//purpose  : 
//=======================================================================
void AdvApp2Var_SysBase::miraz_(integer *taille,
				char *adt)

{
  integer offset;
  offset = *taille;
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
//function : AdvApp2Var_SysBase::mnfnimp_
//purpose  : 
//=======================================================================
integer AdvApp2Var_SysBase::mnfnimp_()
{
  integer ret_val;
  ret_val = 6;
  return ret_val;
} /* mnfnimp_ */

//=======================================================================
//function : AdvApp2Var_SysBase::msifill_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::msifill_(integer *nbintg, 
				 integer *ivecin,
				 integer *ivecou)
{
  static integer nocte;
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Effectue le transfert d'Integer d'une zone dans une autre */

/*     MOTS CLES : */
/*     ----------- */
/*        TRANSFERT , ENTIER , MEMOIRE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NBINTG : Nombre d'entiers */
/*        IVECIN : vecteur d'entree */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        IVECOU : vecteur de sortie */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     26-07-89 : PCR; Declaration en * pour transfert long. nulle */
/*                          (trap sinon). */
/*     17-10-88 : HK ; Ecriture version originale. */
/* > */
/* ***********************************************************************
 */

/* ___ NOCTE : Nombre d'octets a transferer */

    /* Parameter adjustments */
    --ivecou;
    --ivecin;

    /* Function Body */
    nocte =  *nbintg * sizeof(integer);
    AdvApp2Var_SysBase::mcrfill_((integer *)&nocte, (char *)&ivecin[1], (char *)&ivecou[1]);
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
  static integer nocte;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Effectue le transfert de reel d'une zone dans une autre */

/*     MOTS CLES : */
/*     ----------- */
/*        TRANSFERT , REEL , MEMOIRE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NBREEL : Nombre de reels */
/*        VECENT : vecteur d'entree */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        VECSOR : vecteur de sortie */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     26-07-89 : PCR; Declaration en * pour transfert long. nulle */
/*                          (trap sinon). */
/*     06-06-89 : HK ; Nettoyages. */
/*     17-10-88 : HK ; Ecriture version originale */
/* > */
/* ***********************************************************************
 */

/* ___ NOCTE : Nombre d'octets a transferer */

    /* Parameter adjustments */
    --vecsor;
    --vecent;

    /* Function Body */
    nocte = *nbreel << 3;
    AdvApp2Var_SysBase::mcrfill_((integer *)&nocte, (char *)&vecent[1], (char *)&vecsor[1]);
 return 0 ;
} /* msrfill_ */

//=======================================================================
//function : AdvApp2Var_SysBase::mswrdbg_
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::mswrdbg_(const char *,//ctexte, 
				 ftnlen )//ctexte_len)

{

  static cilist io___1 = { 0, 0, 0, 0, 0 };
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Ecrit un message sur la console alpha si IBB>0 */

/*     MOTS CLES : */
/*     ----------- */
/*        MESSAGE,DEBUG */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        CTEXTE : Texte a ecrire */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        Neant */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     21-11-90 : DHU; Mise au propre avant transfert a AC */
/* > */
/* ***********************************************************************
 */
/*                      DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                      TRAITEMENT */
/* ***********************************************************************
 */

    if (AdvApp2Var_SysBase::mnfndeb_() >= 1) {
	io___1.ciunit = AdvApp2Var_SysBase::mnfnimp_();
	//s__wsle(&io___1);
	//do__lio(&c__9, &c__1, "Dbg ", 4L);
	//do__lio(&c__9, &c__1, ctexte, ctexte_len);
	AdvApp2Var_SysBase::e__wsle();
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
//=======================================================================
//function : e__wsfe
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::e__wsfe ()
{
  return 0;
}
//=======================================================================
//function : e__wsle
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::e__wsle ()
{
  return 0;
}
//=======================================================================
//function : s__wsfe
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::s__wsfe ()
{
  return 0;
}
//=======================================================================
//function : s__wsle
//purpose  : 
//=======================================================================
int AdvApp2Var_SysBase::s__wsle ()
{
  return 0;
}


/*
C*****************************************************************************
C
C     FONCTION : CALL MVRIRAZ(NBELT,DTAB)
C     ---------- 
C     Effectue une remise a zero d'un tableau de DOUBLE PRECISION
C
C     MOTS CLES :
C     -----------
C     MVRMIRAZ DOUBLE
C
C     ARGUMENTS D'ENTREE :
C     ------------------
C     NBELT  : Nombre d'elements du tableau
C     DTAB   : Tableau a initialiser a zero
C
C     ARGUMENTS DE SORTIE :
C     --------------------
C     DTAB   : Tableau remis a zero
C
C     COMMONS UTILISES   :
C     ----------------
C
C     REFERENCES APPELEES   :
C     -----------------------
C
C     DEMSCRIPTION/REMARQUES/LIMITATIONS :
C     -----------------------------------
C
C           
C
C$    HISTORIQUE DES MODIFICATIONS   :
C     --------------------------------
C     21-11-95 : JMF ; Creation a partir de miraz
C
C>
C***********************************************************************
*/
//=======================================================================
//function : AdvApp2Var_SysBase::mvriraz_
//purpose  : 
//=======================================================================
void AdvApp2Var_SysBase::mvriraz_(integer *taille,
				  char *adt)

{
  integer offset;
  offset = *taille * 8 ;
  /*    printf(" adt %d  long %d\n",adt,offset); */
  memset(adt , '\0' , offset) ;
}
