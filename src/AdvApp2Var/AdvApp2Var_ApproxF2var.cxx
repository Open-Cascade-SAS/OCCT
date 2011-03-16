//
// AdvApp2Var_ApproxF2var.cxx
//
#include <math.h>
#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_Data.hxx>
#include <AdvApp2Var_ApproxF2var.hxx>


static
int mmjacpt_(const integer *ndimen,
	     const integer *ncoefu, 
	     const integer *ncoefv, 
	     const integer *iordru, 
	     const integer *iordrv, 
	     const doublereal *ptclgd, 
	     doublereal *ptcaux, 
	     doublereal *ptccan);



static
int mma2ce2_(integer *numdec, 
	     integer *ndimen, 
	     integer *nbsesp, 
	     integer *ndimse, 
	     integer *ndminu, 
	     integer *ndminv, 
	     integer *ndguli, 
	     integer *ndgvli, 
	     integer *ndjacu, 
	     integer *ndjacv, 
	     integer *iordru, 
	     integer *iordrv, 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *epsapr, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *gssutb, 
	     doublereal *gssvtb, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *vecerr, 
	     doublereal *chpair, 
	     doublereal *chimpr, 
	     doublereal *patjac, 
	     doublereal *errmax, 
	     doublereal *errmoy, 
	     integer *ndegpu, 
	     integer *ndegpv, 
	     integer *itydec, 
	     integer *iercod);

static
int mma2cfu_(integer *ndujac, 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *gssutb, 
	     doublereal *chpair, 
	     doublereal *chimpr);

static
int mma2cfv_(integer *ndvjac, 
	     integer *mindgu,
	     integer *maxdgu, 
	     integer *nbpntv, 
	     doublereal *gssvtb, 
	     doublereal *chpair, 
	     doublereal *chimpr, 
	     doublereal *patjac);

static
int mma2er1_(integer *ndjacu, 
	     integer *ndjacv, 
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *patjac, 
	     doublereal *vecerr, 
	     doublereal *erreur);

static
int mma2er2_(integer *ndjacu, 
	     integer *ndjacv,
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *patjac, 
	     doublereal *epmscut, 
	     doublereal *vecerr, 
	     doublereal *erreur, 
	     integer *newdgu, 
	     integer *newdgv);

static
int mma2moy_(integer *ndgumx, 
	     integer *ndgvmx, 
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *patjac, 
	     doublereal *errmoy);

static
int mma2ds2_(integer *ndimen, 
	     doublereal *uintfn, 
	     doublereal *vintfn, 
	     void (*foncnp) (
			    int *,
			    double *,
			    double *,
			    int *,
			    double *,
			    int *,
			    double *,
			    int *,
			    int *,
			    double *,
			    int *
			    ), 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *urootb, 
	     doublereal *vrootb, 
	     integer *iiuouv, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *fpntab, 
	     doublereal *ttable, 
	     integer *iercod);




static
int mma1fdi_(integer *ndimen, 
	     doublereal *uvfonc, 
	     void (*foncnp) (// see AdvApp2Var_EvaluatorFunc2Var.hxx for details
			    int *,
			    double *,
			    double *,
			    int *,
			    double *,
			    int *,
			    double *,
			    int *,
			    int *,
			    double *,
			    int *
			    ), 
	     integer *isofav, 
	     doublereal *tconst, 
	     integer *nbroot, 
	     doublereal *ttable, 
	     integer *iordre, 
	     integer *ideriv, 
	     doublereal *fpntab, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *contr1,
	     doublereal *contr2, 
	     integer *iercod);

static
int mma1cdi_(integer *ndimen, 
	     integer *nbroot, 
	     doublereal *rootlg, 
	     integer *iordre, 
	     doublereal *contr1, 
	     doublereal *contr2, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *fpntab, 
	     doublereal *hermit, 
	     integer *iercod);
static
int mma1jak_(integer *ndimen, 
	     integer *nbroot, 
	     integer *iordre,
	     integer *ndgjac, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *cgauss, 
	     doublereal *crvjac, 
	     integer *iercod);
static
int mma1cnt_(integer *ndimen, 
	     integer *iordre, 
	     doublereal *contr1, 
	     doublereal *contr2, 
	     doublereal *hermit, 
	     integer *ndgjac, 
	     doublereal *crvjac);

static
int mma1fer_(integer *ndimen, 
	     integer *nbsesp, 
	     integer *ndimse, 
	     integer *iordre, 
	     integer *ndgjac, 
	     doublereal *crvjac, 
	     integer *ncflim, 
	     doublereal *epsapr, 
	     doublereal *ycvmax, 
	     doublereal *errmax, 
	     doublereal *errmoy, 
	     integer *ncoeff, 
	     integer *iercod);

static
int mma1noc_(doublereal *dfuvin, 
	     integer *ndimen, 
	     integer *iordre, 
	     doublereal *cntrin, 
	     doublereal *duvout, 
	     integer *isofav, 
	     integer *ideriv, 
	     doublereal *cntout);


static
  int mmmapcoe_(integer *ndim, 
		integer *ndgjac, 
		integer *iordre, 
		integer *nbpnts, 
		doublereal *somtab, 
		doublereal *diftab, 
		doublereal *gsstab, 
		doublereal *crvjac);

static
  int mmaperm_(integer *ncofmx, 
	       integer *ndim, 
	       integer *ncoeff, 
	       integer *iordre, 
	       doublereal *crvjac, 
	       integer *ncfnew, 
	       doublereal *errmoy);


#define mmapgss_1 mmapgss_
#define mmapgs0_1 mmapgs0_
#define mmapgs1_1 mmapgs1_
#define mmapgs2_1 mmapgs2_

//=======================================================================
//function : mma1cdi_
//purpose  : 
//=======================================================================
int mma1cdi_(integer *ndimen, 
	     integer *nbroot, 
	     doublereal *rootlg, 
	     integer *iordre, 
	     doublereal *contr1, 
	     doublereal *contr2, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *fpntab, 
	     doublereal *hermit, 
	     integer *iercod)
{
  static integer c__1 = 1;

  /* System generated locals */
  integer contr1_dim1, contr1_offset, contr2_dim1, contr2_offset, 
  somtab_dim1, somtab_offset, diftab_dim1, diftab_offset, 
  fpntab_dim1, fpntab_offset, hermit_dim1, hermit_offset, i__1, 
  i__2, i__3;
  
  /* Local variables */
  static integer nroo2, ncfhe, nd, ii, kk;
  static integer ibb, kkm, kkp;
  static doublereal bid1, bid2, bid3;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation sur les parametres des polynomes d'interpolation */
/*     des contraintes a l'ordre IORDRE. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     NBROOT: Nbre de parametres INTERNES de discretisation. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     ROOTLG: Tableau des parametres de discretisation SUR (-1,1). */
/*     IORDRE: Ordre de contrainte impose aux extremites de l'iso. */
/*             = 0, on calcule les extremites de l'iso */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso */
/*     CONTR1: Contient, si IORDRE>=0, les IORDRE+1 valeurs en TTABLE(0) 
*/
/*             (1ere extremitee) de derivees de F(Uc,Ve) ou F(Ue,Vc), */
/*             voir ci dessous. */
/*     CONTR2: Contient, si IORDRE>=0, les IORDRE+1 valeurs en */
/*             TTABLE(NBROOT+1) (2eme extremitee) de: */
/*                Si ISOFAV=1, derivee d'ordre IDERIV en U, derivee */
/*             d'ordre 0 a IORDRE en V de F(Uc,Ve) ou Uc=TCONST */
/*             (valeur de l'iso fixe) et Ve est l'extremite fixe. */
/*                Si ISOFAV=2, derivee d'ordre IDERIV en V, derivee */
/*             d'ordre 0 a IORDRE en U de F(Ue,Vc) ou Vc=TCONST */
/*             (valeur de l'iso fixe) et Ue est l'extremite fixe. */

/*     SOMTAB: Tableau des NBROOT/2 sommes des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2. */
/*     DIFTAB: Tableau des NBROOT/2 differences des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     SOMTAB: Tableau des NBROOT/2 sommes des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2 */
/*     DIFTAB: Tableau des NBROOT/2 differences des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2 */
/*     FPNTAB: Tableau auxiliaire. */
/*     HERMIT: Table des coeff. des 2*(IORDRE+1) polynomes d'Hermite */
/*             de degre 2*IORDRE+1. */
/*     IERCOD: Code d'erreur, */
/*             = 0, Tout est OK */
/*             = 1, La valeur de IORDRE est hors de (0,2) */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les resultats de la discretisation sont ranges dans 2 tableaux */
/*     SOMTAB et DIFTAB pour gagner du temps par la suite lors du */
/*     calcul des coefficients de la courbe d' approximation. */

/*     Si NBROOT est impair, on stocke dans SOMTAB(0,*) et DIFTAB(0,*) */
/*     les valeurs de la racine mediane de Legendre (0.D0 dans (-1,1)). */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     02-07-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    diftab_dim1 = *nbroot / 2 + 1;
    diftab_offset = diftab_dim1;
    diftab -= diftab_offset;
    somtab_dim1 = *nbroot / 2 + 1;
    somtab_offset = somtab_dim1;
    somtab -= somtab_offset;
    --rootlg;
    hermit_dim1 = (*iordre << 1) + 2;
    hermit_offset = hermit_dim1;
    hermit -= hermit_offset;
    fpntab_dim1 = *nbroot;
    fpntab_offset = fpntab_dim1 + 1;
    fpntab -= fpntab_offset;
    contr2_dim1 = *ndimen;
    contr2_offset = contr2_dim1 + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_offset = contr1_dim1 + 1;
    contr1 -= contr1_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1CDI", 7L);
    }
    *iercod = 0;

/* --- Recup des 2*(IORDRE+1) coeff des 2*(IORDRE+1) polyn. d'Hermite --- 
*/

    AdvApp2Var_ApproxF2var::mma1her_(iordre, &hermit[hermit_offset], iercod);
    if (*iercod > 0) {
	goto L9100;
    }

/* ------------------- Discretisation des polynomes d'Hermite ----------- 
*/

    ncfhe = (*iordre + 1) << 1;
    i__1 = ncfhe;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nbroot;
	for (kk = 1; kk <= i__2; ++kk) {
	    AdvApp2Var_MathBase::mmmpocur_(&ncfhe, &c__1, &ncfhe, &hermit[ii * hermit_dim1], &
		    rootlg[kk], &fpntab[kk + ii * fpntab_dim1]);
/* L200: */
	}
/* L100: */
    }

/* ---- On retranche les discretisations des polynomes de contrainte ---- 
*/

    nroo2 = *nbroot / 2;
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *iordre + 1;
	for (ii = 1; ii <= i__2; ++ii) {
	    bid1 = contr1[nd + ii * contr1_dim1];
	    bid2 = contr2[nd + ii * contr2_dim1];
	    i__3 = nroo2;
	    for (kk = 1; kk <= i__3; ++kk) {
		kkm = nroo2 - kk + 1;
		bid3 = bid1 * fpntab[kkm + ((ii << 1) - 1) * fpntab_dim1] + 
			bid2 * fpntab[kkm + (ii << 1) * fpntab_dim1];
		somtab[kk + nd * somtab_dim1] -= bid3;
		diftab[kk + nd * diftab_dim1] += bid3;
/* L500: */
	    }
	    i__3 = nroo2;
	    for (kk = 1; kk <= i__3; ++kk) {
		kkp = (*nbroot + 1) / 2 + kk;
		bid3 = bid1 * fpntab[kkp + ((ii << 1) - 1) * fpntab_dim1] + 
			bid2 * fpntab[kkp + (ii << 1) * fpntab_dim1];
		somtab[kk + nd * somtab_dim1] -= bid3;
		diftab[kk + nd * diftab_dim1] -= bid3;
/* L600: */
	    }
/* L400: */
	}
/* L300: */
    }

/* ------------ Cas ou l' on discretise sur les racines d' un ----------- 
*/
/* ---------- polynome de Legendre de degre impair, 0 est racine -------- 
*/

    if (*nbroot % 2 == 1) {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = *iordre + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		bid3 = fpntab[nroo2 + 1 + ((ii << 1) - 1) * fpntab_dim1] * 
			contr1[nd + ii * contr1_dim1] + fpntab[nroo2 + 1 + (
			ii << 1) * fpntab_dim1] * contr2[nd + ii * 
			contr2_dim1];
/* L800: */
	    }
	    somtab[nd * somtab_dim1] -= bid3;
	    diftab[nd * diftab_dim1] -= bid3;
/* L700: */
	}
    }

    goto L9999;

/* ------------------------------ The End ------------------------------- 
*/
/* --> IORDRE n'est pas dans la plage autorisee. */
L9100:
    *iercod = 1;
    goto L9999;

L9999:
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1CDI", 7L);
    }
    return 0;
} /* mma1cdi_ */

//=======================================================================
//function : mma1cnt_
//purpose  : 
//=======================================================================
int mma1cnt_(integer *ndimen, 
	     integer *iordre, 
	     doublereal *contr1, 
	     doublereal *contr2, 
	     doublereal *hermit, 
	     integer *ndgjac, 
	     doublereal *crvjac)
{
  /* System generated locals */
  integer contr1_dim1, contr1_offset, contr2_dim1, contr2_offset, 
  hermit_dim1, hermit_offset, crvjac_dim1, crvjac_offset, i__1, 
  i__2, i__3;
  
  /* Local variables */
  static integer nd, ii, jj, ibb;
  static doublereal bid;
  
  
  /* ***********************************************************************
   */
  
  /*     FONCTION : */
  /*     ---------- */
  /*     Ajout du polynome de contrainte. */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*     TOUS,AB_SPECIFI::COURE&,APPROXIMATION,ADDITION,&CONTRAINTE */
  
  /*     ARGUMENTS D'ENTREE : */
  /*     -------------------- */
  /*     NDIMEN: Dimension de l'espace */
  /*     IORDRE: Ordre de contrainte. */
  /*     CONTR1: pt de contrainte en -1, de l'ordre 0 a IORDRE. */
  /*     CONTR2: Pt de contrainte en +1, de l'ordre 0 a IORDRE. */
  /*     HERMIT: Table des polynomes d'hermite d'ordre IORDRE. */
  /*     CRVJAV: Courbe d'approximation dans la base de Jacobi. */
  
  /*     ARGUMENTS DE SORTIE : */
  /*     --------------------- */
  /*     CRVJAV: Courbe d'approximation dans la base de Jacobi */
  /*             a laquelle on a ajoute le polynome d'interpolation des */
  /*             contraintes. */
  
  /*     COMMONS UTILISES : */
  /*     ------------------ */
  
  
  /*     REFERENCES APPELEES : */
  /*     --------------------- */
  
  
/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     07-08-91:RBD; Ecriture version originale. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */
/*   Le nom de la routine */

/* ***********************************************************************
 */
/*                         INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
  hermit_dim1 = (*iordre << 1) + 2;
  hermit_offset = hermit_dim1;
  hermit -= hermit_offset;
  contr2_dim1 = *ndimen;
  contr2_offset = contr2_dim1 + 1;
  contr2 -= contr2_offset;
  contr1_dim1 = *ndimen;
  contr1_offset = contr1_dim1 + 1;
  contr1 -= contr1_offset;
  crvjac_dim1 = *ndgjac + 1;
  crvjac_offset = crvjac_dim1;
  crvjac -= crvjac_offset;
  
  /* Function Body */
  ibb = AdvApp2Var_SysBase::mnfndeb_();
  if (ibb >= 3) {
    AdvApp2Var_SysBase::mgenmsg_("MMA1CNT", 7L);
    }

/* ***********************************************************************
 */
/*                            TRAITEMENT */
/* ***********************************************************************
 */

  i__1 = *ndimen;
  for (nd = 1; nd <= i__1; ++nd) {
    i__2 = (*iordre << 1) + 1;
    for (ii = 0; ii <= i__2; ++ii) {
      bid = 0.;
      i__3 = *iordre + 1;
      for (jj = 1; jj <= i__3; ++jj) {
	bid = bid + contr1[nd + jj * contr1_dim1] *
	  hermit[ii + ((jj  << 1) - 1) * hermit_dim1] + 
	    contr2[nd + jj * contr2_dim1] * hermit[ii + (jj << 1) * hermit_dim1];
	/* L300: */
      }
      crvjac[ii + nd * crvjac_dim1] = bid;
      /* L200: */
    }
    /* L100: */
  }

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1CNT", 7L);
    }

  return 0 ;
} /* mma1cnt_ */

//=======================================================================
//function : mma1fdi_
//purpose  : 
//=======================================================================
int mma1fdi_(integer *ndimen, 
	     doublereal *uvfonc, 
	     void (*foncnp) (// see AdvApp2Var_EvaluatorFunc2Var.hxx for details
			    int *,
			    double *,
			    double *,
			    int *,
			    double *,
			    int *,
			    double *,
			    int *,
			    int *,
			    double *,
			    int *
			    ), 
	     integer *isofav, 
	     doublereal *tconst, 
	     integer *nbroot, 
	     doublereal *ttable, 
	     integer *iordre, 
	     integer *ideriv, 
	     doublereal *fpntab, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *contr1,
	     doublereal *contr2, 
	     integer *iercod)
{
  /* System generated locals */
  integer fpntab_dim1, somtab_dim1, somtab_offset, diftab_dim1, 
  diftab_offset, contr1_dim1, contr1_offset, contr2_dim1, 
  contr2_offset, i__1, i__2;
  doublereal d__1;
  
  /* Local variables */
  static integer ideb, ifin, nroo2, ideru, iderv;
  static doublereal renor;
  static integer ii, nd, ibb, iim, nbp, iip;
  static doublereal bid1, bid2;
  
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation d' une fonction non polynomiale F(U,V) ou d'une */
/*     de ses derivees a isoparametre fixe. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::FONCTION&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     UVFONC: Bornes du pave de definition en U et en V de la fonction */
/*             a approcher. */
/*     FONCNP: Le NOM de la fonction non polynomiale a approcher */
/*             (programme externe). */
/*     ISOFAV: Isoparametre fixe pour la discretisation; */
/*             = 1, on discretise a U fixe et V variable. */
/*             = 2, on discretise a V fixe et U variable. */
/*     TCONST: Valeur de l'iso fixe. */
/*     NBROOT: Nbre de parametres INTERNES de discretisation. */
/*             (s'il y a des contraintes, on doit ajouter 2 extremites). 
*/
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     TTABLE: Tableau des parametres de discretisation et des 2 */
/*             extremites */
/*             (Respectivement (-1, NBROOT racines de Legendre,1) */
/*             recadrees dans l'intervalle adequat. */
/*     IORDRE: Ordre de contrainte impose aux extremites de l'iso. */
/*             (Si Iso-U, on doit calculer les derivees en V et vice */
/*             versa). */
/*             = 0, on calcule les extremites de l'iso */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso */
/*     IDERIV: Ordre de derivee transverse a l'iso fixee (Si Iso-U=Uc */
/*             fixee, on discretise la derivee d'ordre IDERIV en U de */
/*             F(Uc,v). Idem si on fixe une iso-V). */
/*             Varie de 0 (positionnement) a 2 (derivee 2nde). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     FPNTAB: Tableau auxiliaire. */
/*     SOMTAB: Tableau des NBROOT/2 sommes des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2 */
/*     DIFTAB: Tableau des NBROOT/2 differences des 2 points d'indices */
/*             NBROOT-II+1 et II, pour II = 1, NBROOT/2 */
/*     CONTR1: Contient, si IORDRE>=0, les IORDRE+1 valeurs en TTABLE(0) 
*/
/*             (1ere extremitee) de derivees de F(Uc,Ve) ou F(Ue,Vc), */
/*             voir ci dessous. */
/*     CONTR2: Contient, si IORDRE>=0, les IORDRE+1 valeurs en */
/*             TTABLE(NBROOT+1) (2eme extremitee) de: */
/*                Si ISOFAV=1, derivee d'ordre IDERIV en U, derivee */
/*             d'ordre 0 a IORDRE en V de F(Uc,Ve) ou Uc=TCONST */
/*             (valeur de l'iso fixe) et Ve est l'extremite fixe. */
/*                Si ISOFAV=2, derivee d'ordre IDERIV en V, derivee */
/*             d'ordre 0 a IORDRE en U de F(Ue,Vc) ou Vc=TCONST */
/*             (valeur de l'iso fixe) et Ue est l'extremite fixe. */
/*     IERCOD: Code d' erreur > 100; Pb dans l' evaluation de FONCNP, */
/*             le code d'erreur renvoye est egal au code d' erreur */
/*             de FONCNP + 100. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les resultats de la discretisation sont ranges dans 2 tableaux */
/*     SOMTAB et DIFTAB pour gagner du temps par la suite lors du */
/*     calcul des coefficients de la courbe d' approximation. */

/*     Si NBROOT est impair, on stocke dans SOMTAB(0,*) et DIFTAB(0,*) */
/*     les valeurs de la racine mediane de Legendre (0.D0 dans (-1,1)). */

/*     La fonction F(u,v) definie dans UVFONC est reparametre dans */
/*     (-1,1)x(-1,1). On renormalise donc les derivees 1eres et 2ndes. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     24-06-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    uvfonc -= 3;
    diftab_dim1 = *nbroot / 2 + 1;
    diftab_offset = diftab_dim1;
    diftab -= diftab_offset;
    somtab_dim1 = *nbroot / 2 + 1;
    somtab_offset = somtab_dim1;
    somtab -= somtab_offset;
    fpntab_dim1 = *ndimen;
    --fpntab;
    contr2_dim1 = *ndimen;
    contr2_offset = contr2_dim1 + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_offset = contr1_dim1 + 1;
    contr1 -= contr1_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1FDI", 7L);
    }
    *iercod = 0;

/* --------------- Definition du nbre de points a calculer -------------- 
*/
/* --> Si contraintes, on prend aussi les bornes */
    if (*iordre >= 0) {
	ideb = 0;
	ifin = *nbroot + 1;
/* --> Sinon, seule les racines de Legendre (recadrees) sont utilisees
. */
    } else {
	ideb = 1;
	ifin = *nbroot;
    }
/* --> Nbre de point a calculer. */
    nbp = ifin - ideb + 1;
    nroo2 = *nbroot / 2;

/* --------------- Determination de l'ordre de derivation global -------- 
*/
/* --> Ici ISOFAV ne prend que les valeurs 1 ou 2. */
/*    Si Iso-U, on derive en U a l'ordre IDERIV */
    if (*isofav == 1) {
	ideru = *ideriv;
	iderv = 0;
	d__1 = (uvfonc[4] - uvfonc[3]) / 2.;
	renor = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
/*    Si Iso-V, on derive en V a l'ordre IDERIV */
    } else {
	ideru = 0;
	iderv = *ideriv;
	d__1 = (uvfonc[6] - uvfonc[5]) / 2.;
	renor = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
    }

/* ----------- Discretisation sur les racines du polynome --------------- 
*/
/* ---------------------- de Legendre de degre NBROOT ------------------- 
*/

    (*foncnp)(ndimen, 
	      &uvfonc[3], 
	      &uvfonc[5], 
	      isofav, 
	      tconst, 
	      &nbp, 
	      &ttable[ideb], 
	      &ideru, 
	      &iderv, 
	      &fpntab[ideb * fpntab_dim1 + 1], 
	      iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = nroo2;
	for (ii = 1; ii <= i__2; ++ii) {
	    iip = (*nbroot + 1) / 2 + ii;
	    iim = nroo2 - ii + 1;
	    bid1 = fpntab[nd + iim * fpntab_dim1];
	    bid2 = fpntab[nd + iip * fpntab_dim1];
	    somtab[ii + nd * somtab_dim1] = renor * (bid2 + bid1);
	    diftab[ii + nd * diftab_dim1] = renor * (bid2 - bid1);
/* L200: */
	}
/* L100: */
    }

/* ------------ Cas ou l' on discretise sur les racines d' un ----------- 
*/
/* ---------- polynome de Legendre de degre impair, 0 est racine -------- 
*/

    if (*nbroot % 2 == 1) {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    somtab[nd * somtab_dim1] = renor * fpntab[nd + (nroo2 + 1) * 
		    fpntab_dim1];
	    diftab[nd * diftab_dim1] = renor * fpntab[nd + (nroo2 + 1) * 
		    fpntab_dim1];
/* L300: */
	}
    } else {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    somtab[nd * somtab_dim1] = 0.;
	    diftab[nd * diftab_dim1] = 0.;
	}
    }


/* --------------------- Prise en compte des contraintes ---------------- 
*/

    if (*iordre >= 0) {
/* --> Recup des extremites deja calculees. */
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    contr1[nd + contr1_dim1] = renor * fpntab[nd];
	    contr2[nd + contr2_dim1] = renor * fpntab[nd + (*nbroot + 1) * 
		    fpntab_dim1];
/* L400: */
	}
/* --> Nbre de pts a calculer/appel a FONCNP */
	nbp = 1;
/*    Si Iso-U, on derive en V jusqu'a l'ordre IORDRE */
	if (*isofav == 1) {
/* --> Facteur de normalisation derivee 1ere. */
	    bid1 = (uvfonc[6] - uvfonc[5]) / 2.;
	    i__1 = *iordre;
	    for (iderv = 1; iderv <= i__1; ++iderv) {
		(*foncnp)(ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, ttable, &ideru, &iderv, &contr1[(iderv + 1) * 
			contr1_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L500: */
	    }
	    i__1 = *iordre;
	    for (iderv = 1; iderv <= i__1; ++iderv) {
		(*foncnp)(ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, &ttable[*nbroot + 1], &ideru, &iderv, &contr2[(
			iderv + 1) * contr2_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L510: */
	    }
/*    Si Iso-V, on derive en U jusqu'a l'ordre IORDRE */
	} else {
/* --> Facteur de normalisation derivee 1ere. */
	    bid1 = (uvfonc[4] - uvfonc[3]) / 2.;
	    i__1 = *iordre;
	    for (ideru = 1; ideru <= i__1; ++ideru) {
		(*foncnp)(ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, ttable, &ideru, &iderv, &contr1[(ideru + 1) * 
			contr1_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L600: */
	    }
	    i__1 = *iordre;
	    for (ideru = 1; ideru <= i__1; ++ideru) {
		(*foncnp)(ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, &ttable[*nbroot + 1], &ideru, &iderv, &contr2[(
			ideru + 1) * contr2_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L610: */
	    }
	}

/* ------------------------- Normalisation des derivees -------------
---- */
/* (La fonction est redefinie sur (-1,1)*(-1,1)) */
	bid2 = renor;
	i__1 = *iordre;
	for (ii = 1; ii <= i__1; ++ii) {
	    bid2 = bid1 * bid2;
	    i__2 = *ndimen;
	    for (nd = 1; nd <= i__2; ++nd) {
		contr1[nd + (ii + 1) * contr1_dim1] *= bid2;
		contr2[nd + (ii + 1) * contr2_dim1] *= bid2;
/* L710: */
	    }
/* L700: */
	}
    }

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	*iercod += 100;
	AdvApp2Var_SysBase::maermsg_("MMA1FDI", iercod, 7L);
    }
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1FDI", 7L);
    }
    return 0;
} /* mma1fdi_ */

//=======================================================================
//function : mma1fer_
//purpose  : 
//=======================================================================
int mma1fer_(integer *,//ndimen, 
	     integer *nbsesp, 
	     integer *ndimse, 
	     integer *iordre, 
	     integer *ndgjac, 
	     doublereal *crvjac, 
	     integer *ncflim, 
	     doublereal *epsapr, 
	     doublereal *ycvmax, 
	     doublereal *errmax, 
	     doublereal *errmoy, 
	     integer *ncoeff, 
	     integer *iercod)
{
  /* System generated locals */
  integer crvjac_dim1, crvjac_offset, i__1, i__2;
  
  /* Local variables */
  static integer idim, ncfja, ncfnw, ndses, ii, kk, ibb, ier;
  static integer nbr0;
   
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     Calcul du degre et les erreurs d'approximation d'une frontiere. */

/*     MOTS CLES : */
/*     ----------- */
/*      TOUS,AB_SPECIFI :: COURBE&,TRONCATURE, &PRECISION */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*     NDIMEN: Dimension totale de l' espace (somme des dimensions */
/*             des sous-espaces) */
/*     NBSESP: Nombre de sous-espaces "independants". */
/*     NDIMSE: Table des dimensions des sous-espaces. */
/*     IORDRE: Ordre de contrainte aux extremites de la frontiere */
/*              -1 = pas de contraintes, */
/*               0 = contraintes de passage aux bornes (i.e. C0), */
/*               1 = C0 + contraintes de derivees 1eres (i.e. C1), */
/*               2 = C1 + contraintes de derivees 2ndes (i.e. C2). */
/*     NDGJAC: Degre du developpement en serie a utiliser pour le calcul 
*/
/*             dans la base de Jacobi. */
/*     CRVJAC: Table des coeff. de la courbe d'approximation dans la */
/*             base de Jacobi. */
/*     NCFLIM: Nombre maxi de coeff de la "courbe" polynomiale */
/*             d' approximation (doit etre superieur ou egal a */
/*             2*IORDRE+2 et inferieur ou egal a 50). */
/*     EPSAPR: Table des erreurs d' approximations a ne pas depasser, */
/*             sous-espace par sous-espace. */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     YCVMAX: Tableau auxiliaire. */
/*     ERRMAX: Table des erreurs (sous-espace par sous espace) */
/*             MAXIMALES commises dans l' approximation de FONCNP par */
/*             COURBE. */
/*     ERRMOY: Table des erreurs (sous-espace par sous espace) */
/*             MOYENNES commises dans l' approximation de FONCNP par */
/*             COURBE. */
/*     NCOEFF: Nombre de coeff. significatifs de la "courbe" calculee. */
/*     IERCOD: Code d'erreur */
/*             = 0, ok, */
/*             =-1, warning, la tolerance demandee ne peut etre */
/*                  satisfaite avec NCFLIM coefficients. */
/*             = 1, L'ordre des contraintes (IORDRE) n'est pas dans les */
/*                  valeurs autorisees. */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     07-02-92: RBD; Correction du retour du code d'erreur negatif. */
/*     07-08-91: RBD; VERSION ORIGINALE */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    --ycvmax;
    --errmoy;
    --errmax;
    --epsapr;
    --ndimse;
    crvjac_dim1 = *ndgjac + 1;
    crvjac_offset = crvjac_dim1;
    crvjac -= crvjac_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1FER", 7L);
    }
    *iercod = 0;
    idim = 1;
    *ncoeff = 0;
    ncfja = *ndgjac + 1;

/* ------------ Calcul du degre de la courbe et de l' erreur Max -------- 
*/
/* -------------- de l' approximation pour tous les sous-espaces -------- 
*/

    i__1 = *nbsesp;
    for (ii = 1; ii <= i__1; ++ii) {
	ndses = ndimse[ii];

/* ------------ coupure des coeff. et calcul de l' erreur Max -------
---- */

	AdvApp2Var_MathBase::mmtrpjj_(&ncfja, &ndses, &ncfja, &epsapr[ii], iordre, &crvjac[idim * 
		crvjac_dim1], &ycvmax[1], &errmax[ii], &ncfnw);

/* ******************************************************************
**** */
/* ------------- Si precision OK, calcul de l' erreur moyenne -------
---- */
/* ******************************************************************
**** */

	if (ncfnw <= *ncflim) {
	    mmaperm_(&ncfja, &ndses, &ncfja, iordre, &crvjac[idim * 
		    crvjac_dim1], &ncfnw, &errmoy[ii]);
	    *ncoeff = max(ncfnw,*ncoeff);

/* ------------- Mise a 0.D0 des coefficients ecartes -----------
-------- */

	    nbr0 = *ncflim - ncfnw;
	    if (nbr0 > 0) {
		i__2 = ndses;
		for (kk = 1; kk <= i__2; ++kk) {
		  AdvApp2Var_SysBase::mvriraz_(&nbr0, 
			     (char *)&crvjac[ncfnw + (idim + kk - 1) * crvjac_dim1]);
/* L200: */
		}
	    }
	} else {

/* **************************************************************
******** */
/* ------------------- Si precision souhaitee non atteinte ------
-------- */
/* **************************************************************
******** */

	    *iercod = -1;

/* ------------------------- calcul de l' erreur Max ------------
-------- */

	    AdvApp2Var_MathBase::mmaperx_(&ncfja, &ndses, &ncfja, iordre, &crvjac[idim * 
		    crvjac_dim1], ncflim, &ycvmax[1], &errmax[ii], &ier);
	    if (ier > 0) {
		goto L9100;
	    }

/* -------------------- du nbre de coeff a renvoyer -------------
-------- */

	    *ncoeff = *ncflim;

/* ------------------- et calcul de l' erreur moyenne -----------
-------- */

	    mmaperm_(&ncfja, &ndses, &ncfja, iordre, &crvjac[idim * 
		    crvjac_dim1], ncflim, &errmoy[ii]);
	}
	idim += ndses;
/* L100: */
    }

    goto L9999;

/* ------------------------------ The end ------------------------------- 
*/
/* --> L'ordre des contraintes n'est pas dans les valeurs autorisees. */
L9100:
    *iercod = 1;
    goto L9999;

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMA1FER", iercod, 7L);
    }
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1FER", 7L);
    }
    return 0;
} /* mma1fer_ */


//=======================================================================
//function : mma1her_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma1her_(const integer *iordre, 
				     doublereal *hermit, 
				     integer *iercod)
{
  /* System generated locals */
  integer hermit_dim1, hermit_offset;
  
  /* Local variables */
  static integer ibb;
  


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des 2*(IORDRE+1) polynomes d'Hermite de degre 2*IORDRE+1 */
/*     sur (-1,1) */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, INTERPOLATION, &POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     IORDRE: Ordre de contrainte. */
/*      = 0, Polynome d'interpolation a l'ordre C0 sur (-1,1). */
/*      = 1, Polynome d'interpolation a l'ordre C0 et C1 sur (-1,1). */
/*      = 2, Polynome d'interpolation a l'ordre C0, C1 et C2 sur (-1,1). 
*/

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     HERMIT: Table des 2*IORDRE+2 coeff. de chacun des 2*(IORDRE+1) */
/*             polynomes d'HERMITE. */
/*     IERCOD: Code d'erreur, */
/*      = 0, Ok */
/*      = 1, L'ordre de contrainte demande n'est pas gere ici. */
/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     La partie du tableau HERMIT(*,2*i+j) ou j=1 ou 2 et i=0 a IORDRE, 
*/
/*     contient les coefficients du polynome de degre 2*IORDRE+1 */
/*     tel que TOUTES les valeurs en -1 et en +1 de ce polynome et de */
/*     ses derivees jusqu'a l'ordre de derivation IORDRE sont NULLES, */
/*     SAUF la derivee d'ordre i: */
/*        - qui vaut 1 en -1 si j=1 */
/*        - qui vaut 1 en +1 si j=2. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     02-07-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    hermit_dim1 = (*iordre + 1) << 1;
    hermit_offset = hermit_dim1 + 1;
    hermit -= hermit_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1HER", 7L);
    }
    *iercod = 0;

/* --- Recup des (IORDRE+2) coeff des 2*(IORDRE+1) polynomes d'Hermite -- 
*/

    if (*iordre == 0) {
	hermit[hermit_dim1 + 1] = .5;
	hermit[hermit_dim1 + 2] = -.5;

	hermit[(hermit_dim1 << 1) + 1] = .5;
	hermit[(hermit_dim1 << 1) + 2] = .5;
    } else if (*iordre == 1) {
	hermit[hermit_dim1 + 1] = .5;
	hermit[hermit_dim1 + 2] = -.75;
	hermit[hermit_dim1 + 3] = 0.;
	hermit[hermit_dim1 + 4] = .25;

	hermit[(hermit_dim1 << 1) + 1] = .5;
	hermit[(hermit_dim1 << 1) + 2] = .75;
	hermit[(hermit_dim1 << 1) + 3] = 0.;
	hermit[(hermit_dim1 << 1) + 4] = -.25;

	hermit[hermit_dim1 * 3 + 1] = .25;
	hermit[hermit_dim1 * 3 + 2] = -.25;
	hermit[hermit_dim1 * 3 + 3] = -.25;
	hermit[hermit_dim1 * 3 + 4] = .25;

	hermit[(hermit_dim1 << 2) + 1] = -.25;
	hermit[(hermit_dim1 << 2) + 2] = -.25;
	hermit[(hermit_dim1 << 2) + 3] = .25;
	hermit[(hermit_dim1 << 2) + 4] = .25;
    } else if (*iordre == 2) {
	hermit[hermit_dim1 + 1] = .5;
	hermit[hermit_dim1 + 2] = -.9375;
	hermit[hermit_dim1 + 3] = 0.;
	hermit[hermit_dim1 + 4] = .625;
	hermit[hermit_dim1 + 5] = 0.;
	hermit[hermit_dim1 + 6] = -.1875;

	hermit[(hermit_dim1 << 1) + 1] = .5;
	hermit[(hermit_dim1 << 1) + 2] = .9375;
	hermit[(hermit_dim1 << 1) + 3] = 0.;
	hermit[(hermit_dim1 << 1) + 4] = -.625;
	hermit[(hermit_dim1 << 1) + 5] = 0.;
	hermit[(hermit_dim1 << 1) + 6] = .1875;

	hermit[hermit_dim1 * 3 + 1] = .3125;
	hermit[hermit_dim1 * 3 + 2] = -.4375;
	hermit[hermit_dim1 * 3 + 3] = -.375;
	hermit[hermit_dim1 * 3 + 4] = .625;
	hermit[hermit_dim1 * 3 + 5] = .0625;
	hermit[hermit_dim1 * 3 + 6] = -.1875;

	hermit[(hermit_dim1 << 2) + 1] = -.3125;
	hermit[(hermit_dim1 << 2) + 2] = -.4375;
	hermit[(hermit_dim1 << 2) + 3] = .375;
	hermit[(hermit_dim1 << 2) + 4] = .625;
	hermit[(hermit_dim1 << 2) + 5] = -.0625;
	hermit[(hermit_dim1 << 2) + 6] = -.1875;

	hermit[hermit_dim1 * 5 + 1] = .0625;
	hermit[hermit_dim1 * 5 + 2] = -.0625;
	hermit[hermit_dim1 * 5 + 3] = -.125;
	hermit[hermit_dim1 * 5 + 4] = .125;
	hermit[hermit_dim1 * 5 + 5] = .0625;
	hermit[hermit_dim1 * 5 + 6] = -.0625;

	hermit[hermit_dim1 * 6 + 1] = .0625;
	hermit[hermit_dim1 * 6 + 2] = .0625;
	hermit[hermit_dim1 * 6 + 3] = -.125;
	hermit[hermit_dim1 * 6 + 4] = -.125;
	hermit[hermit_dim1 * 6 + 5] = .0625;
	hermit[hermit_dim1 * 6 + 6] = .0625;
    } else {
	*iercod = 1;
    }

/* ------------------------------ The End ------------------------------- 
*/

    AdvApp2Var_SysBase::maermsg_("MMA1HER", iercod, 7L);
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1HER", 7L);
    }
    return 0;
} /* mma1her_ */
//=======================================================================
//function : mma1jak_
//purpose  : 
//=======================================================================
int mma1jak_(integer *ndimen, 
	     integer *nbroot, 
	     integer *iordre,
	     integer *ndgjac, 
	     doublereal *somtab, 
	     doublereal *diftab, 
	     doublereal *cgauss, 
	     doublereal *crvjac, 
	     integer *iercod)
{
  /* System generated locals */
  integer somtab_dim1, somtab_offset, diftab_dim1, diftab_offset, 
  crvjac_dim1, crvjac_offset, cgauss_dim1;
  
  /* Local variables */
  static integer ibb;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Calcule la courbe d' approximation d' une fonction non */
/*      polynomiale dans la base de Jacobi. */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,DISCRETISATION,APPROXIMATION,CONTRAINTE,COURBE,JACOBI */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension totale de l' espace (somme des dimensions */
/*             des sous-espaces) */
/*     NBROOT: Nbre de points de discretisation de l'iso, extremites non 
*/
/*             comprises. */
/*     IORDRE: Ordre de contrainte aux extremites de la frontiere */
/*              -1 = pas de contraintes, */
/*               0 = contraintes de passage aux bornes (i.e. C0), */
/*               1 = C0 + contraintes de derivees 1eres (i.e. C1), */
/*               2 = C1 + contraintes de derivees 2ndes (i.e. C2). */
/*     NDGJAC: Degre du developpement en serie a utiliser pour le calcul 
*/
/*             dans la base de Jacobi. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     CRVJAC : Courbe d' approximation de FONCNP avec (eventuellement) */
/*              prise en compte des contraintes aux extremites. */
/*              Cette courbe est de degre NDGJAC. */
/*     IERCOD : Code d' erreur : */
/*               0 = Tout est ok. */
/*              33 = Pb dans la recuperation des donnees du block data */
/*                   des coeff. d' integration par la methode de GAUSS. */
/*                   par le programme MMAPPTT. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     07-08-1991 : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

    /* Parameter adjustments */
    diftab_dim1 = *nbroot / 2 + 1;
    diftab_offset = diftab_dim1;
    diftab -= diftab_offset;
    somtab_dim1 = *nbroot / 2 + 1;
    somtab_offset = somtab_dim1;
    somtab -= somtab_offset;
    crvjac_dim1 = *ndgjac + 1;
    crvjac_offset = crvjac_dim1;
    crvjac -= crvjac_offset;
    cgauss_dim1 = *nbroot / 2 + 1;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1JAK", 7L);
    }
    *iercod = 0;

/* ----------------- Recup des coeff. d'integration par Gauss ----------- 
*/

    AdvApp2Var_ApproxF2var::mmapptt_(ndgjac, nbroot, iordre, cgauss, iercod);
    if (*iercod > 0) {
	*iercod = 33;
	goto L9999;
    }

/* --------------- Calcul de la courbe dans la base de Jacobi ----------- 
*/

    mmmapcoe_(ndimen, ndgjac, iordre, nbroot, &somtab[somtab_offset], &diftab[
	    diftab_offset], cgauss, &crvjac[crvjac_offset]);

/* ------------------------------ The End ------------------------------- 
*/

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMA1JAK", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1JAK", 7L);
    }
    return 0;
} /* mma1jak_ */

//=======================================================================
//function : mma1noc_
//purpose  : 
//=======================================================================
int mma1noc_(doublereal *dfuvin, 
	     integer *ndimen, 
	     integer *iordre, 
	     doublereal *cntrin, 
	     doublereal *duvout, 
	     integer *isofav, 
	     integer *ideriv, 
	     doublereal *cntout)
{
  /* System generated locals */
  integer i__1;
  doublereal d__1;
 
  
  /* Local variables */
  static doublereal rider, riord;
  static integer nd, ibb;
  static doublereal bid;
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Normalisation des contraintes de derivees, definies sur DFUVIN */
/*     sur le pave DUVOUT. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::VECTEUR&,DERIVEE&,NORMALISATION,&VECTEUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     DFUVIN: Bornes du pave de definition en U et en V ou sont definies 
*/
/*             les contraintes CNTRIN. */
/*     NDIMEN: Dimension de l' espace. */
/*     IORDRE: Ordre de contrainte impose aux extremites de l'iso. */
/*             (Si Iso-U, on doit calculer les derivees en V et vice */
/*             versa). */
/*             = 0, on a calcule les extremites de l'iso */
/*             = 1, on a calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso */
/*             = 2, on a calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso */
/*     CNTRIN: Contient, si IORDRE>=0, les IORDRE+1 derivees */
/*             d'ordre IORDRE de F(Uc,v) ou de F(u,Vc), suivant la */
/*             valeur de ISOFAV, RENORMALISEES pour u et v dans (-1,1). */
/*     DUVOUT: Bornes du pave de definition en U et en V ou seront */
/*             definies les contraintes CNTOUT. */
/*     ISOFAV: Isoparametre fixe pour la discretisation; */
/*             = 1, on discretise a U=Uc fixe et V variable. */
/*             = 2, on discretise a V=Vc fixe et U variable. */
/*     IDERIV: Ordre de derivee transverse a l'iso fixee (Si Iso-U=Uc */
/*             fixee, on discretise la derivee d'ordre IDERIV en U de */
/*             F(Uc,v). Idem si on fixe une iso-V). */
/*             Varie de 0 (positionnement) a 2 (derivee 2nde). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     CNTOUT: Contient, si IORDRE>=0, les IORDRE+1 derivees */
/*             d'ordre IORDRE de F(Uc,v) ou de F(u,Vc), suivant la */
/*             valeur de ISOFAV, RENORMALISEES pour u et v dans DUVOUT. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     CNTRIN peut etre un argument d'entree/sortie, */
/*     c'est a dire que l'appel: */

/*      CALL MMA1NOC(DFUVIN,NDIMEN,IORDRE,CNTRIN,DUVOUT */
/*     1           ,ISOFAV,IDERIV,CNTRIN) */

/*     est correct. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     10-02-1992: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    dfuvin -= 3;
    --cntout;
    --cntrin;
    duvout -= 3;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1NOC", 7L);
    }

/* --------------- Determination des coefficients de normalisation -------
 */

    if (*isofav == 1) {
	d__1 = (dfuvin[4] - dfuvin[3]) / (duvout[4] - duvout[3]);
	rider = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
	d__1 = (dfuvin[6] - dfuvin[5]) / (duvout[6] - duvout[5]);
	riord = AdvApp2Var_MathBase::pow__di(&d__1, iordre);

    } else {
	d__1 = (dfuvin[6] - dfuvin[5]) / (duvout[6] - duvout[5]);
	rider = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
	d__1 = (dfuvin[4] - dfuvin[3]) / (duvout[4] - duvout[3]);
	riord = AdvApp2Var_MathBase::pow__di(&d__1, iordre);
    }

/* ------------- Renormalisation du vecteur de contrainte --------------- 
*/

    bid = rider * riord;
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	cntout[nd] = bid * cntrin[nd];
/* L100: */
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1NOC", 7L);
    }
    return 0;
} /* mma1noc_ */

//=======================================================================
//function : mma1nop_
//purpose  : 
//=======================================================================
int mma1nop_(integer *nbroot, 
	     doublereal *rootlg, 
	     doublereal *uvfonc, 
	     integer *isofav, 
	     doublereal *ttable, 
	     integer *iercod)

{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static doublereal alinu, blinu, alinv, blinv;
  static integer ii, ibb;
  


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     Normalisation de parametres d'une iso, a partir du pave */
/*     parametrique et des parametres sur (-1,1). */

/*     MOTS CLES : */
/*     ----------- */
/*      TOUS,AB_SPECIFI :: ISO&,POINT&,NORMALISATION,&POINT,&ISO */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*        NBROOT: Nbre de pts de discretisation INTERIEURS a l'iso */
/*                definie sur (-1,1). */
/*        ROOTLG: Table des parametres de discretisation sur )-1,1( */
/*                de l'iso. */
/*        UVFONC: Pave de definition de l'iso */
/*        ISOFAV: = 1, c'est une iso-u; =2, c'est une iso-v. */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*        TTABLE: Table des parametres renormalises sur UVFONC de l'iso. 
*/
/*        IERCOD: = 0, OK */
/*                = 1, ISOFAV est hors des valeurs permises. */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     06-02-92: RBD; Creation version originale, d'apres MA1NPA. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


    /* Parameter adjustments */
    --rootlg;
    uvfonc -= 3;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1NOP", 7L);
    }

    alinu = (uvfonc[4] - uvfonc[3]) / 2.;
    blinu = (uvfonc[4] + uvfonc[3]) / 2.;
    alinv = (uvfonc[6] - uvfonc[5]) / 2.;
    blinv = (uvfonc[6] + uvfonc[5]) / 2.;

    if (*isofav == 1) {
	ttable[0] = uvfonc[5];
	i__1 = *nbroot;
	for (ii = 1; ii <= i__1; ++ii) {
	    ttable[ii] = alinv * rootlg[ii] + blinv;
/* L100: */
	}
	ttable[*nbroot + 1] = uvfonc[6];
    } else if (*isofav == 2) {
	ttable[0] = uvfonc[3];
	i__1 = *nbroot;
	for (ii = 1; ii <= i__1; ++ii) {
	    ttable[ii] = alinu * rootlg[ii] + blinu;
/* L200: */
	}
	ttable[*nbroot + 1] = uvfonc[4];
    } else {
	goto L9100;
    }

    goto L9999;

/* ------------------------------ THE END ------------------------------- 
*/

L9100:
    *iercod = 1;
    goto L9999;

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMA1NOP", iercod, 7L);
    }
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA1NOP", 7L);
    }

 return 0 ;

} /* mma1nop_ */

//=======================================================================
//function : mma2ac1_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2ac1_(integer const *ndimen, 
				     integer const *mxujac, 
				     integer const *mxvjac, 
				     integer const *iordru, 
				     integer const *iordrv, 
				     doublereal const *contr1, 
				     doublereal const * contr2, 
				     doublereal const *contr3, 
				     doublereal const *contr4, 
				     doublereal const *uhermt, 
				     doublereal const *vhermt, 
				     doublereal *patjac)

{
  /* System generated locals */
  integer contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
  contr2_offset, contr3_dim1, contr3_dim2, contr3_offset, 
  contr4_dim1, contr4_dim2, contr4_offset, uhermt_dim1, 
  uhermt_offset, vhermt_dim1, vhermt_offset, patjac_dim1, 
  patjac_dim2, patjac_offset, i__1, i__2, i__3, i__4, i__5;
  
  /* Local variables */
  static logical ldbg;
  static integer ndgu, ndgv;
  static doublereal bidu1, bidu2, bidv1, bidv2;
  static integer ioru1, iorv1, ii, nd, jj, ku, kv;
  static doublereal cnt1, cnt2, cnt3, cnt4;
  


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Ajout des polynomes de contraintes des coins. */

/*     MOTS CLES : */
/*     ----------- */
/*  TOUS,AB_SPECIFI::POINT&,CONTRAINTE&,ADDITION,&POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN: Dimension de l'espace. */
/*   MXUJAC: Degre maxi du polynome d' approximation en U. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXUJAC-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   MXVJAC: Degre maxi du polynome d' approximation en V. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXVJAC-2*(IORDRV+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   IORDRU: Ordre de la base de Jacobi (-1,0,1 ou 2) en U. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   IORDRV: Ordre de la base de Jacobi (-1,0,1 ou 2) en V. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   CONTR1: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*           extremitees de F(U0,V0)et de ses derivees. */
/*   CONTR2: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*           extremitees de F(U1,V0)et de ses derivees. */
/*   CONTR3: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*           extremitees de F(U0,V1)et de ses derivees. */
/*   CONTR4: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*           extremitees de F(U1,V1)et de ses derivees. */
/*   UHERMT: Coeff. des polynomes d'Hermite d'ordre IORDRU. */
/*   VHERMT: Coeff. des polynomes d'Hermite d'ordre IORDRV. */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) SANS prise en compte des contraintes. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) AVEC prise en compte des contraintes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     06-02-1992: RBD; Creation d'apres MA2CA1. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */

/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    patjac_dim1 = *mxujac + 1;
    patjac_dim2 = *mxvjac + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;
    uhermt_dim1 = (*iordru << 1) + 2;
    uhermt_offset = uhermt_dim1;
    uhermt -= uhermt_offset;
    vhermt_dim1 = (*iordrv << 1) + 2;
    vhermt_offset = vhermt_dim1;
    vhermt -= vhermt_offset;
    contr4_dim1 = *ndimen;
    contr4_dim2 = *iordru + 2;
    contr4_offset = contr4_dim1 * (contr4_dim2 + 1) + 1;
    contr4 -= contr4_offset;
    contr3_dim1 = *ndimen;
    contr3_dim2 = *iordru + 2;
    contr3_offset = contr3_dim1 * (contr3_dim2 + 1) + 1;
    contr3 -= contr3_offset;
    contr2_dim1 = *ndimen;
    contr2_dim2 = *iordru + 2;
    contr2_offset = contr2_dim1 * (contr2_dim2 + 1) + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_dim2 = *iordru + 2;
    contr1_offset = contr1_dim1 * (contr1_dim2 + 1) + 1;
    contr1 -= contr1_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2AC1", 7L);
    }

/* ------------ SOUSTRACTION des CONTRAINTES DE COINS ------------------- 
*/

    ioru1 = *iordru + 1;
    iorv1 = *iordrv + 1;
    ndgu = (*iordru << 1) + 1;
    ndgv = (*iordrv << 1) + 1;

    i__1 = iorv1;
    for (jj = 1; jj <= i__1; ++jj) {
	i__2 = ioru1;
	for (ii = 1; ii <= i__2; ++ii) {
	    i__3 = *ndimen;
	    for (nd = 1; nd <= i__3; ++nd) {
		cnt1 = contr1[nd + (ii + jj * contr1_dim2) * contr1_dim1];
		cnt2 = contr2[nd + (ii + jj * contr2_dim2) * contr2_dim1];
		cnt3 = contr3[nd + (ii + jj * contr3_dim2) * contr3_dim1];
		cnt4 = contr4[nd + (ii + jj * contr4_dim2) * contr4_dim1];
		i__4 = ndgv;
		for (kv = 0; kv <= i__4; ++kv) {
		    bidv1 = vhermt[kv + ((jj << 1) - 1) * vhermt_dim1];
		    bidv2 = vhermt[kv + (jj << 1) * vhermt_dim1];
		    i__5 = ndgu;
		    for (ku = 0; ku <= i__5; ++ku) {
			bidu1 = uhermt[ku + ((ii << 1) - 1) * uhermt_dim1];
			bidu2 = uhermt[ku + (ii << 1) * uhermt_dim1];
			patjac[ku + (kv + nd * patjac_dim2) * patjac_dim1] = 
				patjac[ku + (kv + nd * patjac_dim2) * 
				patjac_dim1] - bidu1 * bidv1 * cnt1 - bidu2 * 
				bidv1 * cnt2 - bidu1 * bidv2 * cnt3 - bidu2 * 
				bidv2 * cnt4;
/* L500: */
		    }
/* L400: */
		}
/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2AC1", 7L);
    }
    return 0;
} /* mma2ac1_ */

//=======================================================================
//function : mma2ac2_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2ac2_(const integer *ndimen, 
				     const integer *mxujac, 
				     const integer *mxvjac, 
				     const integer *iordrv, 
				     const integer *nclimu, 
				     const integer *ncfiv1, 
				     const doublereal *crbiv1, 
				     const integer *ncfiv2, 
				     const doublereal *crbiv2, 
				     const doublereal *vhermt, 
				     doublereal *patjac)

{
  /* System generated locals */
  integer crbiv1_dim1, crbiv1_dim2, crbiv1_offset, crbiv2_dim1, crbiv2_dim2,
  crbiv2_offset, patjac_dim1, patjac_dim2, patjac_offset, 
  vhermt_dim1, vhermt_offset, i__1, i__2, i__3, i__4;
  
  /* Local variables */
  static logical ldbg;
  static integer ndgv1, ndgv2, ii, jj, nd, kk;
  static doublereal bid1, bid2;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Ajout des polynomes de contraintes */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN: Dimension de l'espace. */
/*   MXUJAC: Degre maxi du polynome d' approximation en U. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXUJAC-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   MXVJAC: Degre maxi du polynome d' approximation en V. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXVJAC-2*(IORDRV+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   IORDRV: Ordre de la base de Jacobi (-1,0,1 ou 2) en V. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   NCLIMU: Nbre LIMITE de coeff. en u de la solution P(u,v). */
/*   NCFIV1: Nbre de Coeff. des courbes stockees dans CRBIV1. */
/*   CRBIV1: Table des coeffs de l'approximation de l'iso-V0 et ses */
/*           derivees jusqu'a l'ordre IORDRV. */
/*   NCFIV2: Nbre de Coeff. des courbes stockees dans CRBIV2. */
/*   CRBIV2: Table des coeffs de l'approximation de l'iso-V1 et ses */
/*           derivees jusqu'a l'ordre IORDRV. */
/*   VHERMT: Table des coeff. des polynomes d'Hermite d'ordre IORDRV. */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) SANS prise en compte des contraintes. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) AVEC prise en compte des contraintes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     06-02-1992: RBD; Creation d'apres MA2CA2. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */

/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    patjac_dim1 = *mxujac + 1;
    patjac_dim2 = *mxvjac + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;
    vhermt_dim1 = (*iordrv << 1) + 2;
    vhermt_offset = vhermt_dim1;
    vhermt -= vhermt_offset;
    --ncfiv2;
    --ncfiv1;
    crbiv2_dim1 = *nclimu;
    crbiv2_dim2 = *ndimen;
    crbiv2_offset = crbiv2_dim1 * (crbiv2_dim2 + 1);
    crbiv2 -= crbiv2_offset;
    crbiv1_dim1 = *nclimu;
    crbiv1_dim2 = *ndimen;
    crbiv1_offset = crbiv1_dim1 * (crbiv1_dim2 + 1);
    crbiv1 -= crbiv1_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2AC2", 7L);
    }

/* ------------ AJOUT des coeff en u des courbes, en v d'Hermite -------- 
*/

    i__1 = *iordrv + 1;
    for (ii = 1; ii <= i__1; ++ii) {
	ndgv1 = ncfiv1[ii] - 1;
	ndgv2 = ncfiv2[ii] - 1;
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    i__3 = (*iordrv << 1) + 1;
	    for (jj = 0; jj <= i__3; ++jj) {
		bid1 = vhermt[jj + ((ii << 1) - 1) * vhermt_dim1];
		i__4 = ndgv1;
		for (kk = 0; kk <= i__4; ++kk) {
		    patjac[kk + (jj + nd * patjac_dim2) * patjac_dim1] += 
			    bid1 * crbiv1[kk + (nd + ii * crbiv1_dim2) * 
			    crbiv1_dim1];
/* L400: */
		}
		bid2 = vhermt[jj + (ii << 1) * vhermt_dim1];
		i__4 = ndgv2;
		for (kk = 0; kk <= i__4; ++kk) {
		    patjac[kk + (jj + nd * patjac_dim2) * patjac_dim1] += 
			    bid2 * crbiv2[kk + (nd + ii * crbiv2_dim2) * 
			    crbiv2_dim1];
/* L500: */
		}
/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2AC2", 7L);
    }
    return 0;
} /* mma2ac2_ */


//=======================================================================
//function : mma2ac3_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2ac3_(const integer *ndimen, 
				     const integer *mxujac, 
				     const integer *mxvjac, 
				     const integer *iordru, 
				     const integer *nclimv, 
				     const integer *ncfiu1, 
				     const doublereal * crbiu1, 
				     const integer *ncfiu2, 
				     const doublereal *crbiu2, 
				     const doublereal *uhermt, 
				     doublereal *patjac)

{
  /* System generated locals */
  integer crbiu1_dim1, crbiu1_dim2, crbiu1_offset, crbiu2_dim1, crbiu2_dim2,
  crbiu2_offset, patjac_dim1, patjac_dim2, patjac_offset, 
  uhermt_dim1, uhermt_offset, i__1, i__2, i__3, i__4;
  
  /* Local variables */
  static logical ldbg;
  static integer ndgu1, ndgu2, ii, jj, nd, kk;
  static doublereal bid1, bid2;
  
  


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Ajout des polynomes de contraintes */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN: Dimension de l'espace. */
/*   MXUJAC: Degre maxi du polynome d' approximation en U. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXUJAC-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   MXVJAC: Degre maxi du polynome d' approximation en V. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre MXVJAC-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*   IORDRU: Ordre de la base de Jacobi (-1,0,1 ou 2) en V. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   NCLIMV: Nbre LIMITE de coeff. en v de la solution P(u,v). */
/*   NCFIU1: Nbre de Coeff. des courbes stockees dans CRBIU1. */
/*   CRBIU1: Table des coeffs de l'approximation de l'iso-U0 et ses */
/*           derivees jusqu'a l'ordre IORDRU. */
/*   NCFIU2: Nbre de Coeff. des courbes stockees dans CRBIU2. */
/*   CRBIU2: Table des coeffs de l'approximation de l'iso-U1 et ses */
/*           derivees jusqu'a l'ordre IORDRU. */
/*   UHERMT: Table des coeff. des polynomes d'Hermite d'ordre IORDRU. */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) SANS prise en compte des contraintes. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) AVEC prise en compte des contraintes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     06-02-1991: RBD; Creation d'apres MA2CA3. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */

/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    patjac_dim1 = *mxujac + 1;
    patjac_dim2 = *mxvjac + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;
    uhermt_dim1 = (*iordru << 1) + 2;
    uhermt_offset = uhermt_dim1;
    uhermt -= uhermt_offset;
    --ncfiu2;
    --ncfiu1;
    crbiu2_dim1 = *nclimv;
    crbiu2_dim2 = *ndimen;
    crbiu2_offset = crbiu2_dim1 * (crbiu2_dim2 + 1);
    crbiu2 -= crbiu2_offset;
    crbiu1_dim1 = *nclimv;
    crbiu1_dim2 = *ndimen;
    crbiu1_offset = crbiu1_dim1 * (crbiu1_dim2 + 1);
    crbiu1 -= crbiu1_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2AC3", 7L);
    }

/* ------------ AJOUT des coeff en u des courbes, en v d'Hermite -------- 
*/

    i__1 = *iordru + 1;
    for (ii = 1; ii <= i__1; ++ii) {
	ndgu1 = ncfiu1[ii] - 1;
	ndgu2 = ncfiu2[ii] - 1;
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    i__3 = ndgu1;
	    for (jj = 0; jj <= i__3; ++jj) {
		bid1 = crbiu1[jj + (nd + ii * crbiu1_dim2) * crbiu1_dim1];
		i__4 = (*iordru << 1) + 1;
		for (kk = 0; kk <= i__4; ++kk) {
		    patjac[kk + (jj + nd * patjac_dim2) * patjac_dim1] += 
			    bid1 * uhermt[kk + ((ii << 1) - 1) * uhermt_dim1];
/* L400: */
		}
/* L300: */
	    }
	    i__3 = ndgu2;
	    for (jj = 0; jj <= i__3; ++jj) {
		bid2 = crbiu2[jj + (nd + ii * crbiu2_dim2) * crbiu2_dim1];
		i__4 = (*iordru << 1) + 1;
		for (kk = 0; kk <= i__4; ++kk) {
		    patjac[kk + (jj + nd * patjac_dim2) * patjac_dim1] += 
			    bid2 * uhermt[kk + (ii << 1) * uhermt_dim1];
/* L600: */
		}
/* L500: */
	    }

/* L200: */
	}
/* L100: */
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2AC3", 7L);
    }
    return 0;
} /* mma2ac3_ */

//=======================================================================
//function : mma2can_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2can_(const integer *ncfmxu, 
				     const integer *ncfmxv,
				     const integer *ndimen, 
				     const integer *iordru, 
				     const integer *iordrv, 
				     const integer *ncoefu, 
				     const integer *ncoefv, 
				     const doublereal *patjac, 
				     doublereal *pataux, 
				     doublereal *patcan, 
				     integer *iercod)

{
  /* System generated locals */
  integer patjac_dim1, patjac_dim2, patjac_offset, patcan_dim1, patcan_dim2,
  patcan_offset, i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer ilon1, ilon2, ii, nd;
  
  
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Changement de base Jacobi vers canonique (-1,1) et ecriture dans */
/*     un tableau + grand. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI,CARREAU&,CONVERSION,JACOBI,CANNONIQUE,&CARREAU */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*     NCFMXU: Dimension en U du tableau resultat PATCAN */
/*     NCFMXV: Dimension en V du tableau resultat PATCAN */
/*     NDIMEN: Dimension de l'espace de travail. */
/*     IORDRU: Ordre de contrainte en U */
/*     IORDRV: Ordre de contrainte en V. */
/*     NCOEFU: Nbre de coeff en U du carreau PATJAC */
/*     NCOEFV: Nbre de coeff en V du carreau PATJAC */
/*     PATJAC: Carreau dans la base de Jacobi d'ordre IORDRU en U et */
/*             IORDRV en V. */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     PATAUX: Tableau auxiliaire. */
/*     PATCAN: Tableau des coefficients dans la base canonique. */
/*     IERCOD: Code d'erreur. */
/*             = 0, tout va tres bien, toutes choses etant egales par */
/*                  ailleurs. */
/*             = 1, le programme refuse de traiter avec des arguments */
/*                  d'entrees aussi stupides. */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     29-01-1992: RBD; Ecriture version originale. */
/* > */
/* ********************************************************************** 
*/


    /* Parameter adjustments */
    patcan_dim1 = *ncfmxu;
    patcan_dim2 = *ncfmxv;
    patcan_offset = patcan_dim1 * (patcan_dim2 + 1) + 1;
    patcan -= patcan_offset;
    --pataux;
    patjac_dim1 = *ncoefu;
    patjac_dim2 = *ncoefv;
    patjac_offset = patjac_dim1 * (patjac_dim2 + 1) + 1;
    patjac -= patjac_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CAN", 7L);
    }
    *iercod = 0;

    if (*iordru < -1 || *iordru > 2) {
	goto L9100;
    }
    if (*iordrv < -1 || *iordrv > 2) {
	goto L9100;
    }
    if (*ncoefu > *ncfmxu || *ncoefv > *ncfmxv) {
	goto L9100;
    }

/* -------------------- Ah les jolis changements de bases --------------- 
*/
/* ------------ (Sur l'air des 'jolies colonies de vacances') ----------- 
*/

/* --> On passe en base canonique (-1,1) */
    mmjacpt_(ndimen, ncoefu, ncoefv, iordru, iordrv, &patjac[patjac_offset], &
	    pataux[1], &patcan[patcan_offset]);

/* --> On ecrit le tout dans un tableau + grand */
    AdvApp2Var_MathBase::mmfmca8_((integer *)ncoefu, 
	     (integer *)ncoefv, 
	     (integer *)ndimen, 
	     (integer *)ncfmxu, 
	     (integer *)ncfmxv, 
	     (integer *)ndimen, 
	     (doublereal *)&patcan[patcan_offset], 
	     (doublereal *)&patcan[patcan_offset]);

/* --> On complete avec des zeros le tableau resultat. */
    ilon1 = *ncfmxu - *ncoefu;
    ilon2 = *ncfmxu * (*ncfmxv - *ncoefv);
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	if (ilon1 > 0) {
	    i__2 = *ncoefv;
	    for (ii = 1; ii <= i__2; ++ii) {
		AdvApp2Var_SysBase::mvriraz_(&ilon1, 
			 (char *)&patcan[*ncoefu + 1 + (ii + nd * patcan_dim2) * patcan_dim1]);
/* L110: */
	    }
	}
	if (ilon2 > 0) {
	    AdvApp2Var_SysBase::mvriraz_(&ilon2, 
		     (char *)&patcan[(*ncoefv + 1 + nd * patcan_dim2) * patcan_dim1 + 1]);
	}
/* L100: */
    }

    goto L9999;

/* ---------------------- A la revoyure M'sieu dames -------------------- 
*/

L9100:
    *iercod = 1;
    goto L9999;

L9999:
    AdvApp2Var_SysBase::maermsg_("MMA2CAN", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CAN", 7L);
    }
 return 0 ;
} /* mma2can_ */

//=======================================================================
//function : mma2cd1_
//purpose  : 
//=======================================================================
int mma2cd1_(integer *ndimen, 
	     integer *nbpntu, 
	     doublereal *urootl, 
	     integer *nbpntv, 
	     doublereal *vrootl, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *contr1, 
	     doublereal *contr2, 
	     doublereal *contr3, 
	     doublereal *contr4, 
	     doublereal *fpntbu, 
	     doublereal *fpntbv, 
	     doublereal *uhermt, 
	     doublereal *vhermt, 
	     doublereal *sosotb, 
	     doublereal *soditb, 
	     doublereal *disotb, 
	     doublereal *diditb)

{
  static integer c__1 = 1;
 
/* System generated locals */
    integer contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
	     contr2_offset, contr3_dim1, contr3_dim2, contr3_offset, 
	    contr4_dim1, contr4_dim2, contr4_offset, uhermt_dim1, 
	    uhermt_offset, vhermt_dim1, vhermt_offset, fpntbu_dim1, 
	    fpntbu_offset, fpntbv_dim1, fpntbv_offset, sosotb_dim1, 
	    sosotb_dim2, sosotb_offset, diditb_dim1, diditb_dim2, 
	    diditb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
	    disotb_dim1, disotb_dim2, disotb_offset, i__1, i__2, i__3, i__4, 
	    i__5;

    /* Local variables */
    static integer ncfhu, ncfhv, nuroo, nvroo, nd, ii, jj, kk, ll, ibb, kkm, 
	    llm, kkp, llp;
    static doublereal bid1, bid2, bid3, bid4;
    static doublereal diu1, diu2, div1, div2, sou1, sou2, sov1, sov2;




/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation sur les parametres des polynomes d'interpolation */
/*     des contraintes aux coins a l'ordre IORDRE. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     NBPNTU: Nbre de parametres INTERNES de discretisation EN U. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     UROOTL: Tableau des parametres de discretisation SUR (-1,1) EN U. 
*/
/*     NBPNTV: Nbre de parametres INTERNES de discretisation EN V. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     VROOTL: Tableau des parametres de discretisation SUR (-1,1) EN V. 
*/
/*     IORDRU: Ordre de contrainte impose aux extremites de l'iso-V */
/*             = 0, on calcule les extremites de l'iso-V */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso-V */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso-V */
/*     IORDRV: Ordre de contrainte impose aux extremites de l'iso-U */
/*             = 0, on calcule les extremites de l'iso-U. */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso-U */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso-U */
/*     CONTR1: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U0,V0)et de ses derivees. */
/*     CONTR2: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U1,V0)et de ses derivees. */
/*     CONTR3: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U0,V1)et de ses derivees. */
/*     CONTR4: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U1,V1)et de ses derivees. */
/*     SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*     DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     FPNTBU: Tableau auxiliaire. */
/*     FPNTBV: Tableau auxiliaire. */
/*     UHERMT: Table des 2*(IORDRU+1) coeff. des 2*(IORDRU+1) polynomes */
/*             d'Hermite. */
/*     VHERMT: Table des 2*(IORDRV+1) coeff. des 2*(IORDRV+1) polynomes */
/*             d'Hermite. */
/*   SOSOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     09-08-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    --urootl;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    --vrootl;
    uhermt_dim1 = (*iordru << 1) + 2;
    uhermt_offset = uhermt_dim1;
    uhermt -= uhermt_offset;
    fpntbu_dim1 = *nbpntu;
    fpntbu_offset = fpntbu_dim1 + 1;
    fpntbu -= fpntbu_offset;
    vhermt_dim1 = (*iordrv << 1) + 2;
    vhermt_offset = vhermt_dim1;
    vhermt -= vhermt_offset;
    fpntbv_dim1 = *nbpntv;
    fpntbv_offset = fpntbv_dim1 + 1;
    fpntbv -= fpntbv_offset;
    contr4_dim1 = *ndimen;
    contr4_dim2 = *iordru + 2;
    contr4_offset = contr4_dim1 * (contr4_dim2 + 1) + 1;
    contr4 -= contr4_offset;
    contr3_dim1 = *ndimen;
    contr3_dim2 = *iordru + 2;
    contr3_offset = contr3_dim1 * (contr3_dim2 + 1) + 1;
    contr3 -= contr3_offset;
    contr2_dim1 = *ndimen;
    contr2_dim2 = *iordru + 2;
    contr2_offset = contr2_dim1 * (contr2_dim2 + 1) + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_dim2 = *iordru + 2;
    contr1_offset = contr1_dim1 * (contr1_dim2 + 1) + 1;
    contr1 -= contr1_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CD1", 7L);
    }

/* ------------------- Discretisation des polynomes d'Hermite ----------- 
*/

    ncfhu = (*iordru + 1) << 1;
    i__1 = ncfhu;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nbpntu;
	for (ll = 1; ll <= i__2; ++ll) {
	    AdvApp2Var_MathBase::mmmpocur_(&ncfhu, &c__1, &ncfhu, &uhermt[ii * uhermt_dim1], &
		    urootl[ll], &fpntbu[ll + ii * fpntbu_dim1]);
/* L20: */
	}
/* L10: */
    }
    ncfhv = (*iordrv + 1) << 1;
    i__1 = ncfhv;
    for (jj = 1; jj <= i__1; ++jj) {
	i__2 = *nbpntv;
	for (kk = 1; kk <= i__2; ++kk) {
	    AdvApp2Var_MathBase::mmmpocur_(&ncfhv, &c__1, &ncfhv, &vhermt[jj * vhermt_dim1], &
		    vrootl[kk], &fpntbv[kk + jj * fpntbv_dim1]);
/* L40: */
	}
/* L30: */
    }

/* ---- On retranche les discretisations des polynomes de contrainte ---- 
*/

    nuroo = *nbpntu / 2;
    nvroo = *nbpntv / 2;
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {

	i__2 = *iordrv + 1;
	for (jj = 1; jj <= i__2; ++jj) {
	    i__3 = *iordru + 1;
	    for (ii = 1; ii <= i__3; ++ii) {
		bid1 = contr1[nd + (ii + jj * contr1_dim2) * contr1_dim1];
		bid2 = contr2[nd + (ii + jj * contr2_dim2) * contr2_dim1];
		bid3 = contr3[nd + (ii + jj * contr3_dim2) * contr3_dim1];
		bid4 = contr4[nd + (ii + jj * contr4_dim2) * contr4_dim1];

		i__4 = nvroo;
		for (kk = 1; kk <= i__4; ++kk) {
		    kkp = (*nbpntv + 1) / 2 + kk;
		    kkm = nvroo - kk + 1;
		    sov1 = fpntbv[kkp + ((jj << 1) - 1) * fpntbv_dim1] + 
			    fpntbv[kkm + ((jj << 1) - 1) * fpntbv_dim1];
		    div1 = fpntbv[kkp + ((jj << 1) - 1) * fpntbv_dim1] - 
			    fpntbv[kkm + ((jj << 1) - 1) * fpntbv_dim1];
		    sov2 = fpntbv[kkp + (jj << 1) * fpntbv_dim1] + fpntbv[kkm 
			    + (jj << 1) * fpntbv_dim1];
		    div2 = fpntbv[kkp + (jj << 1) * fpntbv_dim1] - fpntbv[kkm 
			    + (jj << 1) * fpntbv_dim1];
		    i__5 = nuroo;
		    for (ll = 1; ll <= i__5; ++ll) {
			llp = (*nbpntu + 1) / 2 + ll;
			llm = nuroo - ll + 1;
			sou1 = fpntbu[llp + ((ii << 1) - 1) * fpntbu_dim1] + 
				fpntbu[llm + ((ii << 1) - 1) * fpntbu_dim1];
			diu1 = fpntbu[llp + ((ii << 1) - 1) * fpntbu_dim1] - 
				fpntbu[llm + ((ii << 1) - 1) * fpntbu_dim1];
			sou2 = fpntbu[llp + (ii << 1) * fpntbu_dim1] + fpntbu[
				llm + (ii << 1) * fpntbu_dim1];
			diu2 = fpntbu[llp + (ii << 1) * fpntbu_dim1] - fpntbu[
				llm + (ii << 1) * fpntbu_dim1];
			sosotb[ll + (kk + nd * sosotb_dim2) * sosotb_dim1] = 
				sosotb[ll + (kk + nd * sosotb_dim2) * 
				sosotb_dim1] - bid1 * sou1 * sov1 - bid2 * 
				sou2 * sov1 - bid3 * sou1 * sov2 - bid4 * 
				sou2 * sov2;
			soditb[ll + (kk + nd * soditb_dim2) * soditb_dim1] = 
				soditb[ll + (kk + nd * soditb_dim2) * 
				soditb_dim1] - bid1 * sou1 * div1 - bid2 * 
				sou2 * div1 - bid3 * sou1 * div2 - bid4 * 
				sou2 * div2;
			disotb[ll + (kk + nd * disotb_dim2) * disotb_dim1] = 
				disotb[ll + (kk + nd * disotb_dim2) * 
				disotb_dim1] - bid1 * diu1 * sov1 - bid2 * 
				diu2 * sov1 - bid3 * diu1 * sov2 - bid4 * 
				diu2 * sov2;
			diditb[ll + (kk + nd * diditb_dim2) * diditb_dim1] = 
				diditb[ll + (kk + nd * diditb_dim2) * 
				diditb_dim1] - bid1 * diu1 * div1 - bid2 * 
				diu2 * div1 - bid3 * diu1 * div2 - bid4 * 
				diu2 * div2;
/* L450: */
		    }
/* L400: */
		}

/* ------------ Cas ou l' on discretise sur les racines d' un 
----------- */
/* ---------- polynome de Legendre de degre impair, 0 est raci
ne -------- */

		if (*nbpntu % 2 == 1) {
		    sou1 = fpntbu[nuroo + 1 + ((ii << 1) - 1) * fpntbu_dim1];
		    sou2 = fpntbu[nuroo + 1 + (ii << 1) * fpntbu_dim1];
		    i__4 = nvroo;
		    for (kk = 1; kk <= i__4; ++kk) {
			kkp = (*nbpntv + 1) / 2 + kk;
			kkm = nvroo - kk + 1;
			sov1 = fpntbv[kkp + ((jj << 1) - 1) * fpntbv_dim1] + 
				fpntbv[kkm + ((jj << 1) - 1) * fpntbv_dim1];
			div1 = fpntbv[kkp + ((jj << 1) - 1) * fpntbv_dim1] - 
				fpntbv[kkm + ((jj << 1) - 1) * fpntbv_dim1];
			sov2 = fpntbv[kkp + (jj << 1) * fpntbv_dim1] + fpntbv[
				kkm + (jj << 1) * fpntbv_dim1];
			div2 = fpntbv[kkp + (jj << 1) * fpntbv_dim1] - fpntbv[
				kkm + (jj << 1) * fpntbv_dim1];
			sosotb[(kk + nd * sosotb_dim2) * sosotb_dim1] = 
				sosotb[(kk + nd * sosotb_dim2) * sosotb_dim1] 
				- bid1 * sou1 * sov1 - bid2 * sou2 * sov1 - 
				bid3 * sou1 * sov2 - bid4 * sou2 * sov2;
			diditb[(kk + nd * diditb_dim2) * diditb_dim1] = 
				diditb[(kk + nd * diditb_dim2) * diditb_dim1] 
				- bid1 * sou1 * div1 - bid2 * sou2 * div1 - 
				bid3 * sou1 * div2 - bid4 * sou2 * div2;
/* L500: */
		    }
		}

		if (*nbpntv % 2 == 1) {
		    sov1 = fpntbv[nvroo + 1 + ((jj << 1) - 1) * fpntbv_dim1];
		    sov2 = fpntbv[nvroo + 1 + (jj << 1) * fpntbv_dim1];
		    i__4 = nuroo;
		    for (ll = 1; ll <= i__4; ++ll) {
			llp = (*nbpntu + 1) / 2 + ll;
			llm = nuroo - ll + 1;
			sou1 = fpntbu[llp + ((ii << 1) - 1) * fpntbu_dim1] + 
				fpntbu[llm + ((ii << 1) - 1) * fpntbu_dim1];
			diu1 = fpntbu[llp + ((ii << 1) - 1) * fpntbu_dim1] - 
				fpntbu[llm + ((ii << 1) - 1) * fpntbu_dim1];
			sou2 = fpntbu[llp + (ii << 1) * fpntbu_dim1] + fpntbu[
				llm + (ii << 1) * fpntbu_dim1];
			diu2 = fpntbu[llp + (ii << 1) * fpntbu_dim1] - fpntbu[
				llm + (ii << 1) * fpntbu_dim1];
			sosotb[ll + nd * sosotb_dim2 * sosotb_dim1] = sosotb[
				ll + nd * sosotb_dim2 * sosotb_dim1] - bid1 * 
				sou1 * sov1 - bid2 * sou2 * sov1 - bid3 * 
				sou1 * sov2 - bid4 * sou2 * sov2;
			diditb[ll + nd * diditb_dim2 * diditb_dim1] = diditb[
				ll + nd * diditb_dim2 * diditb_dim1] - bid1 * 
				diu1 * sov1 - bid2 * diu2 * sov1 - bid3 * 
				diu1 * sov2 - bid4 * diu2 * sov2;
/* L600: */
		    }
		}

		if (*nbpntu % 2 == 1 && *nbpntv % 2 == 1) {
		    sou1 = fpntbu[nuroo + 1 + ((ii << 1) - 1) * fpntbu_dim1];
		    sou2 = fpntbu[nuroo + 1 + (ii << 1) * fpntbu_dim1];
		    sov1 = fpntbv[nvroo + 1 + ((jj << 1) - 1) * fpntbv_dim1];
		    sov2 = fpntbv[nvroo + 1 + (jj << 1) * fpntbv_dim1];
		    sosotb[nd * sosotb_dim2 * sosotb_dim1] = sosotb[nd * 
			    sosotb_dim2 * sosotb_dim1] - bid1 * sou1 * sov1 - 
			    bid2 * sou2 * sov1 - bid3 * sou1 * sov2 - bid4 * 
			    sou2 * sov2;
		    diditb[nd * diditb_dim2 * diditb_dim1] = diditb[nd * 
			    diditb_dim2 * diditb_dim1] - bid1 * sou1 * sov1 - 
			    bid2 * sou2 * sov1 - bid3 * sou1 * sov2 - bid4 * 
			    sou2 * sov2;
		}

/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }
    goto L9999;

/* ------------------------------ The End ------------------------------- 
*/

L9999:
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CD1", 7L);
    }
    return 0;
} /* mma2cd1_ */

//=======================================================================
//function : mma2cd2_
//purpose  : 
//=======================================================================
int mma2cd2_(integer *ndimen, 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *vrootl, 
	     integer *iordrv, 
	     doublereal *sotbv1, 
	     doublereal *sotbv2, 
	     doublereal *ditbv1, 
	     doublereal *ditbv2, 
	     doublereal *fpntab, 
	     doublereal *vhermt, 
	     doublereal *sosotb, 
	     doublereal *soditb, 
	     doublereal *disotb, 
	     doublereal *diditb)

{
  static integer c__1 = 1;
  /* System generated locals */
  integer sotbv1_dim1, sotbv1_dim2, sotbv1_offset, sotbv2_dim1, sotbv2_dim2,
  sotbv2_offset, ditbv1_dim1, ditbv1_dim2, ditbv1_offset, 
  ditbv2_dim1, ditbv2_dim2, ditbv2_offset, fpntab_dim1, 
  fpntab_offset, vhermt_dim1, vhermt_offset, sosotb_dim1, 
  sosotb_dim2, sosotb_offset, diditb_dim1, diditb_dim2, 
  diditb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  disotb_dim1, disotb_dim2, disotb_offset, i__1, i__2, i__3, i__4;
  
  /* Local variables */
  static integer ncfhv, nuroo, nvroo, ii, nd, jj, kk, ibb, jjm, jjp;
  static doublereal bid1, bid2, bid3, bid4;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation sur les parametres des polynomes d'interpolation */
/*     des contraintes sur les 2 bords iso-V a l'ordre IORDRV. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     NBPNTU: Nbre de parametres INTERNES de discretisation EN U. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     NBPNTV: Nbre de parametres INTERNES de discretisation EN V. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     VROOTL: Tableau des parametres de discretisation SUR (-1,1) EN V. 
*/
/*     IORDRV: Ordre de derivation de l'iso-V */
/*             = 0, on calcule l'iso-V. */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  transverse a l'iso-V (donc en V). */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  transverse a l'iso-V (donc en V). */
/*     SOTBV1: Tableau des NBPNTV/2 sommes des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V0. */
/*     SOTBV2: Tableau des NBPNTV/2 sommes des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V1. */
/*     DITBV1: Tableau des NBPNTV/2 differences des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V0. */
/*     DITBV2: Tableau des NBPNTV/2 differences des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V1. */
/*     SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*     DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     FPNTAB: Tableau auxiliaire. */
/*     VHERMT: Table des 2*(IORDRV+1) coeff. des 2*(IORDRV+1) polynomes */
/*             d'Hermite. */
/*   SOSOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    --vrootl;
    vhermt_dim1 = (*iordrv << 1) + 2;
    vhermt_offset = vhermt_dim1;
    vhermt -= vhermt_offset;
    fpntab_dim1 = *nbpntv;
    fpntab_offset = fpntab_dim1 + 1;
    fpntab -= fpntab_offset;
    ditbv2_dim1 = *nbpntu / 2 + 1;
    ditbv2_dim2 = *ndimen;
    ditbv2_offset = ditbv2_dim1 * (ditbv2_dim2 + 1);
    ditbv2 -= ditbv2_offset;
    ditbv1_dim1 = *nbpntu / 2 + 1;
    ditbv1_dim2 = *ndimen;
    ditbv1_offset = ditbv1_dim1 * (ditbv1_dim2 + 1);
    ditbv1 -= ditbv1_offset;
    sotbv2_dim1 = *nbpntu / 2 + 1;
    sotbv2_dim2 = *ndimen;
    sotbv2_offset = sotbv2_dim1 * (sotbv2_dim2 + 1);
    sotbv2 -= sotbv2_offset;
    sotbv1_dim1 = *nbpntu / 2 + 1;
    sotbv1_dim2 = *ndimen;
    sotbv1_offset = sotbv1_dim1 * (sotbv1_dim2 + 1);
    sotbv1 -= sotbv1_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CD2", 7L);
    }

/* ------------------- Discretisation des polynomes d'Hermite ----------- 
*/

    ncfhv = (*iordrv + 1) << 1;
    i__1 = ncfhv;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nbpntv;
	for (jj = 1; jj <= i__2; ++jj) {
	    AdvApp2Var_MathBase::mmmpocur_(&ncfhv, &c__1, &ncfhv, &vhermt[ii * vhermt_dim1], &
		    vrootl[jj], &fpntab[jj + ii * fpntab_dim1]);
/* L60: */
	}
/* L50: */
    }

/* ---- On retranche les discretisations des polynomes de contrainte ---- 
*/

    nuroo = *nbpntu / 2;
    nvroo = *nbpntv / 2;

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *iordrv + 1;
	for (ii = 1; ii <= i__2; ++ii) {

	    i__3 = nuroo;
	    for (kk = 1; kk <= i__3; ++kk) {
		bid1 = sotbv1[kk + (nd + ii * sotbv1_dim2) * sotbv1_dim1];
		bid2 = sotbv2[kk + (nd + ii * sotbv2_dim2) * sotbv2_dim1];
		bid3 = ditbv1[kk + (nd + ii * ditbv1_dim2) * ditbv1_dim1];
		bid4 = ditbv2[kk + (nd + ii * ditbv2_dim2) * ditbv2_dim1];
		i__4 = nvroo;
		for (jj = 1; jj <= i__4; ++jj) {
		    jjp = (*nbpntv + 1) / 2 + jj;
		    jjm = nvroo - jj + 1;
		    sosotb[kk + (jj + nd * sosotb_dim2) * sosotb_dim1] = 
			    sosotb[kk + (jj + nd * sosotb_dim2) * sosotb_dim1]
			     - bid1 * (fpntab[jjp + ((ii << 1) - 1) * 
			    fpntab_dim1] + fpntab[jjm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid2 * (fpntab[jjp + (ii << 1) * 
			    fpntab_dim1] + fpntab[jjm + (ii << 1) * 
			    fpntab_dim1]);
		    disotb[kk + (jj + nd * disotb_dim2) * disotb_dim1] = 
			    disotb[kk + (jj + nd * disotb_dim2) * disotb_dim1]
			     - bid3 * (fpntab[jjp + ((ii << 1) - 1) * 
			    fpntab_dim1] + fpntab[jjm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid4 * (fpntab[jjp + (ii << 1) * 
			    fpntab_dim1] + fpntab[jjm + (ii << 1) * 
			    fpntab_dim1]);
		    soditb[kk + (jj + nd * soditb_dim2) * soditb_dim1] = 
			    soditb[kk + (jj + nd * soditb_dim2) * soditb_dim1]
			     - bid1 * (fpntab[jjp + ((ii << 1) - 1) * 
			    fpntab_dim1] - fpntab[jjm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid2 * (fpntab[jjp + (ii << 1) * 
			    fpntab_dim1] - fpntab[jjm + (ii << 1) * 
			    fpntab_dim1]);
		    diditb[kk + (jj + nd * diditb_dim2) * diditb_dim1] = 
			    diditb[kk + (jj + nd * diditb_dim2) * diditb_dim1]
			     - bid3 * (fpntab[jjp + ((ii << 1) - 1) * 
			    fpntab_dim1] - fpntab[jjm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid4 * (fpntab[jjp + (ii << 1) * 
			    fpntab_dim1] - fpntab[jjm + (ii << 1) * 
			    fpntab_dim1]);
/* L400: */
		}
/* L300: */
	    }
/* L200: */
	}

/* ------------ Cas ou l' on discretise sur les racines d' un -------
---- */
/* ---------- polynome de Legendre de degre impair, 0 est racine ----
---- */

	if (*nbpntv % 2 == 1) {
	    i__2 = *iordrv + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		i__3 = nuroo;
		for (kk = 1; kk <= i__3; ++kk) {
		    bid1 = sotbv1[kk + (nd + ii * sotbv1_dim2) * sotbv1_dim1] 
			    * fpntab[nvroo + 1 + ((ii << 1) - 1) * 
			    fpntab_dim1] + sotbv2[kk + (nd + ii * sotbv2_dim2)
			     * sotbv2_dim1] * fpntab[nvroo + 1 + (ii << 1) * 
			    fpntab_dim1];
		    sosotb[kk + nd * sosotb_dim2 * sosotb_dim1] -= bid1;
		    bid2 = ditbv1[kk + (nd + ii * ditbv1_dim2) * ditbv1_dim1] 
			    * fpntab[nvroo + 1 + ((ii << 1) - 1) * 
			    fpntab_dim1] + ditbv2[kk + (nd + ii * ditbv2_dim2)
			     * ditbv2_dim1] * fpntab[nvroo + 1 + (ii << 1) * 
			    fpntab_dim1];
		    diditb[kk + nd * diditb_dim2 * diditb_dim1] -= bid2;
/* L550: */
		}
/* L500: */
	    }
	}

	if (*nbpntu % 2 == 1) {
	    i__2 = *iordrv + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		i__3 = nvroo;
		for (jj = 1; jj <= i__3; ++jj) {
		    jjp = (*nbpntv + 1) / 2 + jj;
		    jjm = nvroo - jj + 1;
		    bid1 = sotbv1[(nd + ii * sotbv1_dim2) * sotbv1_dim1] * (
			    fpntab[jjp + ((ii << 1) - 1) * fpntab_dim1] + 
			    fpntab[jjm + ((ii << 1) - 1) * fpntab_dim1]) + 
			    sotbv2[(nd + ii * sotbv2_dim2) * sotbv2_dim1] * (
			    fpntab[jjp + (ii << 1) * fpntab_dim1] + fpntab[
			    jjm + (ii << 1) * fpntab_dim1]);
		    sosotb[(jj + nd * sosotb_dim2) * sosotb_dim1] -= bid1;
		    bid2 = sotbv1[(nd + ii * sotbv1_dim2) * sotbv1_dim1] * (
			    fpntab[jjp + ((ii << 1) - 1) * fpntab_dim1] - 
			    fpntab[jjm + ((ii << 1) - 1) * fpntab_dim1]) + 
			    sotbv2[(nd + ii * sotbv2_dim2) * sotbv2_dim1] * (
			    fpntab[jjp + (ii << 1) * fpntab_dim1] - fpntab[
			    jjm + (ii << 1) * fpntab_dim1]);
		    diditb[jj + nd * diditb_dim2 * diditb_dim1] -= bid2;
/* L650: */
		}
/* L600: */
	    }
	}

	if (*nbpntu % 2 == 1 && *nbpntv % 2 == 1) {
	    i__2 = *iordrv + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		bid1 = sotbv1[(nd + ii * sotbv1_dim2) * sotbv1_dim1] * fpntab[
			nvroo + 1 + ((ii << 1) - 1) * fpntab_dim1] + sotbv2[(
			nd + ii * sotbv2_dim2) * sotbv2_dim1] * fpntab[nvroo 
			+ 1 + (ii << 1) * fpntab_dim1];
		sosotb[nd * sosotb_dim2 * sosotb_dim1] -= bid1;
/* L700: */
	    }
	}

/* L100: */
    }
    goto L9999;

/* ------------------------------ The End ------------------------------- 
*/

L9999:
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CD2", 7L);
    }
    return 0;
} /* mma2cd2_ */

//=======================================================================
//function : mma2cd3_
//purpose  : 
//=======================================================================
int mma2cd3_(integer *ndimen,
	     integer *nbpntu, 
	     doublereal *urootl, 
	     integer *nbpntv, 
	     integer *iordru, 
	     doublereal *sotbu1, 
	     doublereal *sotbu2, 
	     doublereal *ditbu1, 
	     doublereal *ditbu2, 
	     doublereal *fpntab, 
	     doublereal *uhermt, 
	     doublereal *sosotb, 
	     doublereal *soditb, 
	     doublereal *disotb, 
	     doublereal *diditb)

{
  static integer c__1 = 1;
  
   /* System generated locals */
    integer sotbu1_dim1, sotbu1_dim2, sotbu1_offset, sotbu2_dim1, sotbu2_dim2,
	     sotbu2_offset, ditbu1_dim1, ditbu1_dim2, ditbu1_offset, 
	    ditbu2_dim1, ditbu2_dim2, ditbu2_offset, fpntab_dim1, 
	    fpntab_offset, uhermt_dim1, uhermt_offset, sosotb_dim1, 
	    sosotb_dim2, sosotb_offset, diditb_dim1, diditb_dim2, 
	    diditb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
	    disotb_dim1, disotb_dim2, disotb_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    static integer ncfhu, nuroo, nvroo, ii, nd, jj, kk, ibb, kkm, kkp;
    static doublereal bid1, bid2, bid3, bid4;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation sur les parametres des polynomes d'interpolation */
/*     des contraintes sur les 2 bords iso-U a l'ordre IORDRU. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     NBPNTU: Nbre de parametres INTERNES de discretisation EN U. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     UROOTL: Tableau des parametres de discretisation SUR (-1,1) EN U. 
*/
/*     NBPNTV: Nbre de parametres INTERNES de discretisation EN V. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     IORDRU: Ordre de derivation de l'iso-U */
/*             = 0, on calcule l'iso-U. */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  transverse a l'iso-U (donc en U). */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  transverse a l'iso-U (donc en U). */
/*     SOTBU1: Tableau des NBPNTU/2 sommes des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U0. */
/*     SOTBU2: Tableau des NBPNTU/2 sommes des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U1. */
/*     DITBU1: Tableau des NBPNTU/2 differences des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U0. */
/*     DITBU2: Tableau des NBPNTU/2 differences des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U1. */
/*     SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*     DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     FPNTAB: Tableau auxiliaire. */
/*     UHERMT: Table des 2*(IORDRU+1) coeff. des 2*(IORDRU+1) polynomes */
/*             d'Hermite. */
/*   SOSOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    --urootl;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    uhermt_dim1 = (*iordru << 1) + 2;
    uhermt_offset = uhermt_dim1;
    uhermt -= uhermt_offset;
    fpntab_dim1 = *nbpntu;
    fpntab_offset = fpntab_dim1 + 1;
    fpntab -= fpntab_offset;
    ditbu2_dim1 = *nbpntv / 2 + 1;
    ditbu2_dim2 = *ndimen;
    ditbu2_offset = ditbu2_dim1 * (ditbu2_dim2 + 1);
    ditbu2 -= ditbu2_offset;
    ditbu1_dim1 = *nbpntv / 2 + 1;
    ditbu1_dim2 = *ndimen;
    ditbu1_offset = ditbu1_dim1 * (ditbu1_dim2 + 1);
    ditbu1 -= ditbu1_offset;
    sotbu2_dim1 = *nbpntv / 2 + 1;
    sotbu2_dim2 = *ndimen;
    sotbu2_offset = sotbu2_dim1 * (sotbu2_dim2 + 1);
    sotbu2 -= sotbu2_offset;
    sotbu1_dim1 = *nbpntv / 2 + 1;
    sotbu1_dim2 = *ndimen;
    sotbu1_offset = sotbu1_dim1 * (sotbu1_dim2 + 1);
    sotbu1 -= sotbu1_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CD3", 7L);
    }

/* ------------------- Discretisation des polynomes d'Hermite ----------- 
*/

    ncfhu = (*iordru + 1) << 1;
    i__1 = ncfhu;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nbpntu;
	for (kk = 1; kk <= i__2; ++kk) {
	    AdvApp2Var_MathBase::mmmpocur_(&ncfhu, 
					   &c__1, 
					   &ncfhu,
					   &uhermt[ii * uhermt_dim1],
					   &urootl[kk], 
					   &fpntab[kk + ii * fpntab_dim1]);
/* L60: */
	}
/* L50: */
    }

/* ---- On retranche les discretisations des polynomes de contrainte ---- 
*/

    nvroo = *nbpntv / 2;
    nuroo = *nbpntu / 2;

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *iordru + 1;
	for (ii = 1; ii <= i__2; ++ii) {

	    i__3 = nvroo;
	    for (jj = 1; jj <= i__3; ++jj) {
		bid1 = sotbu1[jj + (nd + ii * sotbu1_dim2) * sotbu1_dim1];
		bid2 = sotbu2[jj + (nd + ii * sotbu2_dim2) * sotbu2_dim1];
		bid3 = ditbu1[jj + (nd + ii * ditbu1_dim2) * ditbu1_dim1];
		bid4 = ditbu2[jj + (nd + ii * ditbu2_dim2) * ditbu2_dim1];
		i__4 = nuroo;
		for (kk = 1; kk <= i__4; ++kk) {
		    kkp = (*nbpntu + 1) / 2 + kk;
		    kkm = nuroo - kk + 1;
		    sosotb[kk + (jj + nd * sosotb_dim2) * sosotb_dim1] = 
			    sosotb[kk + (jj + nd * sosotb_dim2) * sosotb_dim1]
			     - bid1 * (fpntab[kkp + ((ii << 1) - 1) * 
			    fpntab_dim1] + fpntab[kkm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid2 * (fpntab[kkp + (ii << 1) * 
			    fpntab_dim1] + fpntab[kkm + (ii << 1) * 
			    fpntab_dim1]);
		    disotb[kk + (jj + nd * disotb_dim2) * disotb_dim1] = 
			    disotb[kk + (jj + nd * disotb_dim2) * disotb_dim1]
			     - bid1 * (fpntab[kkp + ((ii << 1) - 1) * 
			    fpntab_dim1] - fpntab[kkm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid2 * (fpntab[kkp + (ii << 1) * 
			    fpntab_dim1] - fpntab[kkm + (ii << 1) * 
			    fpntab_dim1]);
		    soditb[kk + (jj + nd * soditb_dim2) * soditb_dim1] = 
			    soditb[kk + (jj + nd * soditb_dim2) * soditb_dim1]
			     - bid3 * (fpntab[kkp + ((ii << 1) - 1) * 
			    fpntab_dim1] + fpntab[kkm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid4 * (fpntab[kkp + (ii << 1) * 
			    fpntab_dim1] + fpntab[kkm + (ii << 1) * 
			    fpntab_dim1]);
		    diditb[kk + (jj + nd * diditb_dim2) * diditb_dim1] = 
			    diditb[kk + (jj + nd * diditb_dim2) * diditb_dim1]
			     - bid3 * (fpntab[kkp + ((ii << 1) - 1) * 
			    fpntab_dim1] - fpntab[kkm + ((ii << 1) - 1) * 
			    fpntab_dim1]) - bid4 * (fpntab[kkp + (ii << 1) * 
			    fpntab_dim1] - fpntab[kkm + (ii << 1) * 
			    fpntab_dim1]);
/* L400: */
		}
/* L300: */
	    }
/* L200: */
	}

/* ------------ Cas ou l' on discretise sur les racines d' un -------
---- */
/* ---------- polynome de Legendre de degre impair, 0 est racine ----
---- */

	if (*nbpntu % 2 == 1) {
	    i__2 = *iordru + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		i__3 = nvroo;
		for (jj = 1; jj <= i__3; ++jj) {
		    bid1 = sotbu1[jj + (nd + ii * sotbu1_dim2) * sotbu1_dim1] 
			    * fpntab[nuroo + 1 + ((ii << 1) - 1) * 
			    fpntab_dim1] + sotbu2[jj + (nd + ii * sotbu2_dim2)
			     * sotbu2_dim1] * fpntab[nuroo + 1 + (ii << 1) * 
			    fpntab_dim1];
		    sosotb[(jj + nd * sosotb_dim2) * sosotb_dim1] -= bid1;
		    bid2 = ditbu1[jj + (nd + ii * ditbu1_dim2) * ditbu1_dim1] 
			    * fpntab[nuroo + 1 + ((ii << 1) - 1) * 
			    fpntab_dim1] + ditbu2[jj + (nd + ii * ditbu2_dim2)
			     * ditbu2_dim1] * fpntab[nuroo + 1 + (ii << 1) * 
			    fpntab_dim1];
		    diditb[(jj + nd * diditb_dim2) * diditb_dim1] -= bid2;
/* L550: */
		}
/* L500: */
	    }
	}

	if (*nbpntv % 2 == 1) {
	    i__2 = *iordru + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		i__3 = nuroo;
		for (kk = 1; kk <= i__3; ++kk) {
		    kkp = (*nbpntu + 1) / 2 + kk;
		    kkm = nuroo - kk + 1;
		    bid1 = sotbu1[(nd + ii * sotbu1_dim2) * sotbu1_dim1] * (
			    fpntab[kkp + ((ii << 1) - 1) * fpntab_dim1] + 
			    fpntab[kkm + ((ii << 1) - 1) * fpntab_dim1]) + 
			    sotbu2[(nd + ii * sotbu2_dim2) * sotbu2_dim1] * (
			    fpntab[kkp + (ii << 1) * fpntab_dim1] + fpntab[
			    kkm + (ii << 1) * fpntab_dim1]);
		    sosotb[kk + nd * sosotb_dim2 * sosotb_dim1] -= bid1;
		    bid2 = sotbu1[(nd + ii * sotbu1_dim2) * sotbu1_dim1] * (
			    fpntab[kkp + ((ii << 1) - 1) * fpntab_dim1] - 
			    fpntab[kkm + ((ii << 1) - 1) * fpntab_dim1]) + 
			    sotbu2[(nd + ii * sotbu2_dim2) * sotbu2_dim1] * (
			    fpntab[kkp + (ii << 1) * fpntab_dim1] - fpntab[
			    kkm + (ii << 1) * fpntab_dim1]);
		    diditb[kk + nd * diditb_dim2 * diditb_dim1] -= bid2;
/* L650: */
		}
/* L600: */
	    }
	}

	if (*nbpntu % 2 == 1 && *nbpntv % 2 == 1) {
	    i__2 = *iordru + 1;
	    for (ii = 1; ii <= i__2; ++ii) {
		bid1 = sotbu1[(nd + ii * sotbu1_dim2) * sotbu1_dim1] * fpntab[
			nuroo + 1 + ((ii << 1) - 1) * fpntab_dim1] + sotbu2[(
			nd + ii * sotbu2_dim2) * sotbu2_dim1] * fpntab[nuroo 
			+ 1 + (ii << 1) * fpntab_dim1];
		sosotb[nd * sosotb_dim2 * sosotb_dim1] -= bid1;
/* L700: */
	    }
	}

/* L100: */
    }
    goto L9999;

/* ------------------------------ The End ------------------------------- 
*/

L9999:
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CD3", 7L);
    }
    return 0;
} /* mma2cd3_ */

//=======================================================================
//function : mma2cdi_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2cdi_( integer *ndimen, 
				     integer *nbpntu, 
				     doublereal *urootl, 
				     integer *nbpntv, 
				     doublereal *vrootl, 
				     integer *iordru, 
				     integer *iordrv, 
				     doublereal *contr1, 
				     doublereal *contr2, 
				     doublereal *contr3, 
				     doublereal *contr4, 
				     doublereal *sotbu1, 
				     doublereal *sotbu2, 
				     doublereal *ditbu1, 
				     doublereal *ditbu2, 
				     doublereal *sotbv1, 
				     doublereal *sotbv2, 
				     doublereal *ditbv1, 
				     doublereal *ditbv2, 
				     doublereal *sosotb, 
				     doublereal *soditb, 
				     doublereal *disotb, 
				     doublereal *diditb, 
				     integer *iercod)

{
  static integer c__8 = 8;

    /* System generated locals */
    integer contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
	     contr2_offset, contr3_dim1, contr3_dim2, contr3_offset, 
	    contr4_dim1, contr4_dim2, contr4_offset, sosotb_dim1, sosotb_dim2,
	     sosotb_offset, diditb_dim1, diditb_dim2, diditb_offset, 
	    soditb_dim1, soditb_dim2, soditb_offset, disotb_dim1, disotb_dim2,
	     disotb_offset;

    /* Local variables */
    static integer ilong;
    static long int iofwr;
    static doublereal wrkar[1];
    static integer iszwr;
    static integer ibb, ier;
    static integer isz1, isz2, isz3, isz4;
    static long int ipt1, ipt2, ipt3, ipt4;




/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation sur les parametres des polynomes d'interpolation */
/*     des contraintes a l'ordre IORDRE. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension de l' espace. */
/*     NBPNTU: Nbre de parametres INTERNES de discretisation EN U. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     UROOTL: Tableau des parametres de discretisation SUR (-1,1) EN U. 
*/
/*     NBPNTV: Nbre de parametres INTERNES de discretisation EN V. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     VROOTL: Tableau des parametres de discretisation SUR (-1,1) EN V. 
*/
/*     IORDRU: Ordre de contrainte impose aux extremites de l'iso-V */
/*             = 0, on calcule les extremites de l'iso-V */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso-V */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso-V */
/*     IORDRV: Ordre de contrainte impose aux extremites de l'iso-U */
/*             = 0, on calcule les extremites de l'iso-U. */
/*             = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*                  de l'iso-U */
/*             = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*                  de l'iso-U */
/*     CONTR1: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U0,V0)et de ses derivees. */
/*     CONTR2: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U1,V0)et de ses derivees. */
/*     CONTR3: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U0,V1)et de ses derivees. */
/*     CONTR4: Contient, si IORDRU et IORDRV>=0, les valeurs aux */
/*             extremitees de F(U1,V1)et de ses derivees. */
/*     SOTBU1: Tableau des NBPNTU/2 sommes des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U0. */
/*     SOTBU2: Tableau des NBPNTU/2 sommes des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U1. */
/*     DITBU1: Tableau des NBPNTU/2 differences des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U0. */
/*     DITBU2: Tableau des NBPNTU/2 differences des 2 points d'indices */
/*             NBPNTU-II+1 et II, pour II = 1, NBPNTU/2 sur l'iso-U1. */
/*     SOTBV1: Tableau des NBPNTV/2 sommes des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V0. */
/*     SOTBV2: Tableau des NBPNTV/2 sommes des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V1. */
/*     DITBV1: Tableau des NBPNTV/2 differences des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V0. */
/*     DITBV2: Tableau des NBPNTV/2 differences des 2 points d'indices */
/*             NBPNTV-II+1 et II, pour II = 1, NBPNTV/2 sur l'iso-V1. */
/*     SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*     SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*     DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   SOSOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes de contraintes */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   IERCOD: = 0, OK, */
/*           = 1, Valeur de IORDRV ou IORDRU hors des valeurs permises. */
/*           =13, Pb d'alloc dynamique. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    --urootl;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    --vrootl;
    contr4_dim1 = *ndimen;
    contr4_dim2 = *iordru + 2;
    contr4_offset = contr4_dim1 * (contr4_dim2 + 1) + 1;
    contr4 -= contr4_offset;
    contr3_dim1 = *ndimen;
    contr3_dim2 = *iordru + 2;
    contr3_offset = contr3_dim1 * (contr3_dim2 + 1) + 1;
    contr3 -= contr3_offset;
    contr2_dim1 = *ndimen;
    contr2_dim2 = *iordru + 2;
    contr2_offset = contr2_dim1 * (contr2_dim2 + 1) + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_dim2 = *iordru + 2;
    contr1_offset = contr1_dim1 * (contr1_dim2 + 1) + 1;
    contr1 -= contr1_offset;
    --sotbu1;
    --sotbu2;
    --ditbu1;
    --ditbu2;
    --sotbv1;
    --sotbv2;
    --ditbv1;
    --ditbv2;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CDI", 7L);
    }
    *iercod = 0;
    iofwr = 0;
    if (*iordru < -1 || *iordru > 2) {
	goto L9100;
    }
    if (*iordrv < -1 || *iordrv > 2) {
	goto L9100;
    }

/* ------------------------- Mise a zero -------------------------------- 
*/

    ilong = (*nbpntu / 2 + 1) * (*nbpntv / 2 + 1) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&ilong, (char *)&sosotb[sosotb_offset]);
    AdvApp2Var_SysBase::mvriraz_(&ilong, (char *)&diditb[diditb_offset]);
    ilong = *nbpntu / 2 * (*nbpntv / 2) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&ilong, (char *)&soditb[soditb_offset]);
    AdvApp2Var_SysBase::mvriraz_(&ilong, (char *)&disotb[disotb_offset]);
    if (*iordru == -1 && *iordrv == -1) {
	goto L9999;
    }



    isz1 = ((*iordru + 1) << 2) * (*iordru + 1);
    isz2 = ((*iordrv + 1) << 2) * (*iordrv + 1);
    isz3 = ((*iordru + 1) << 1) * *nbpntu;
    isz4 = ((*iordrv + 1) << 1) * *nbpntv;
    iszwr = isz1 + isz2 + isz3 + isz4;
    AdvApp2Var_SysBase::mcrrqst_(&c__8, &iszwr, wrkar, &iofwr, &ier);
    if (ier > 0) {
	goto L9013;
    }
    ipt1 = iofwr;
    ipt2 = ipt1 + isz1;
    ipt3 = ipt2 + isz2;
    ipt4 = ipt3 + isz3;

    if (*iordru >= 0 && *iordru <= 2) {

/* --- Recup des 2*(IORDRU+1) coeff des 2*(IORDRU+1) polyn. d'Hermite 
--- */

	AdvApp2Var_ApproxF2var::mma1her_(iordru, &wrkar[ipt1], iercod);
	if (*iercod > 0) {
	    goto L9100;
	}

/* ---- On retranche les discretisations des polynomes de contrainte 
---- */

	mma2cd3_(ndimen, nbpntu, &urootl[1], nbpntv, iordru, &sotbu1[1], &
		sotbu2[1], &ditbu1[1], &ditbu2[1], &wrkar[ipt3], &wrkar[ipt1],
		 &sosotb[sosotb_offset], &soditb[soditb_offset], &disotb[
		disotb_offset], &diditb[diditb_offset]);
    }

    if (*iordrv >= 0 && *iordrv <= 2) {

/* --- Recup des 2*(IORDRV+1) coeff des 2*(IORDRV+1) polyn. d'Hermite 
--- */

	AdvApp2Var_ApproxF2var::mma1her_(iordrv, &wrkar[ipt2], iercod);
	if (*iercod > 0) {
	    goto L9100;
	}

/* ---- On retranche les discretisations des polynomes de contrainte 
---- */

	mma2cd2_(ndimen, nbpntu, nbpntv, &vrootl[1], iordrv, &sotbv1[1], &
		sotbv2[1], &ditbv1[1], &ditbv2[1], &wrkar[ipt4], &wrkar[ipt2],
		 &sosotb[sosotb_offset], &soditb[soditb_offset], &disotb[
		disotb_offset], &diditb[diditb_offset]);
    }

/* --------------- On retranche les contraintes de coins ---------------- 
*/

    if (*iordru >= 0 && *iordrv >= 0) {
	mma2cd1_(ndimen, nbpntu, &urootl[1], nbpntv, &vrootl[1], iordru, 
		iordrv, &contr1[contr1_offset], &contr2[contr2_offset], &
		contr3[contr3_offset], &contr4[contr4_offset], &wrkar[ipt3], &
		wrkar[ipt4], &wrkar[ipt1], &wrkar[ipt2], &sosotb[
		sosotb_offset], &soditb[soditb_offset], &disotb[disotb_offset]
		, &diditb[diditb_offset]);
    }
    goto L9999;

/* ------------------------------ The End ------------------------------- 
*/
/* --> IORDRE n'est pas dans la plage autorisee. */
L9100:
    *iercod = 1;
    goto L9999;
/* --> PB d'alloc dyn. */
L9013:
    *iercod = 13;
    goto L9999;

L9999:
    if (iofwr != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, &iszwr, wrkar, &iofwr, &ier);
    }
    if (ier > 0) {
	*iercod = 13;
    }
    AdvApp2Var_SysBase::maermsg_("MMA2CDI", iercod, 7L);
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CDI", 7L);
    }
    return 0;
} /* mma2cdi_ */

//=======================================================================
//function : mma2ce1_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2ce1_(integer *numdec, 
				     integer *ndimen, 
				     integer *nbsesp, 
				     integer *ndimse, 
				     integer *ndminu, 
				     integer *ndminv, 
				     integer *ndguli, 
				     integer *ndgvli, 
				     integer *ndjacu, 
				     integer *ndjacv, 
				     integer *iordru, 
				     integer *iordrv, 
				     integer *nbpntu, 
				     integer *nbpntv, 
				     doublereal *epsapr, 
				     doublereal *sosotb, 
				     doublereal *disotb, 
				     doublereal *soditb, 
				     doublereal *diditb, 
				     doublereal *patjac, 
				     doublereal *errmax, 
				     doublereal *errmoy, 
				     integer *ndegpu, 
				     integer *ndegpv, 
				     integer *itydec, 
				     integer *iercod)
     
{
  static integer c__8 = 8;
  
    /* System generated locals */
    integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
	     disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
	    diditb_dim1, diditb_dim2, diditb_offset, patjac_dim1, patjac_dim2,
	     patjac_offset;

    /* Local variables */
    static logical ldbg;
    static long int iofwr;
    static doublereal wrkar[1];
    static integer iszwr;
    static integer ier;
    static integer isz1, isz2, isz3, isz4, isz5, isz6, isz7;
    static long int ipt1, ipt2, ipt3, ipt4, ipt5, ipt6, ipt7;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des coefficients de l' approximation polynomiale de degre */
/*     (NDJACU,NDJACV) d'une fonction F(u,v) quelconque, a partir de sa */
/*     discretisation sur les racines du polynome de Legendre de degre */
/*     NBPNTU en U et NBPNTV en V. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::FONCTION&,APPROXIMATION,&POLYNOME,&ERREUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NUMDEC: Indique si on PEUT decouper encore la fonction F(u,v). */
/*           = 5, On PEUT couper en U ou en V ou dans les 2 sens a la */
/*                fois. */
/*           = 4, On PEUT couper en U ou en V MAIS PAS dans les 2 sens */
/*                a la fois (decoupe en V favorisee). */
/*           = 3, On PEUT couper en U ou en V MAIS PAS dans les 2 sens */
/*                a la fois (decoupe en U favorisee). */
/*           = 2, on ne PEUT couper qu'en V (i.e. inserer un parametre */
/*                de decoupe Vj). */
/*           = 1, on ne PEUT couper qu'en U (i.e. inserer un parametre */
/*                de decoupe Ui). */
/*           = 0, on ne PEUT plus rien couper */
/*   NDIMEN: Dimension de l'espace. */
/*   NBSESP: Nbre de sous-espaces independant sur lesquels on calcule */
/*           les erreurs. */
/*   NDIMSE: Table des dimensions de chacun des sous-espaces. */
/*   NDMINU: Degre minimum en U a conserver pour l'approximation. */
/*   NDMINV: Degre minimum en V a conserver pour l'approximation. */
/*   NDGULI: Limite du nbre de coefficients en U de la solution. */
/*   NDGVLI: Limite du nbre de coefficients en V de la solution. */
/*   NDJACU: Degre maxi du polynome d' approximation en U. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre NDJACU-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2. */
/*           On doit avoir 2*IORDRU+1 <= NDMINU <= NDGULI < NDJACU */
/*   NDJACV: Degre maxi du polynome d' approximation en V. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre NDJACV-2*(IORDRV+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*           On doit avoir 2*IORDRV+1 <= NDMINV <= NDGVLI < NDJACV */
/*   IORDRU: Ordre de la base de Jacobi (-1,0,1 ou 2) en U. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   IORDRV: Ordre de la base de Jacobi (-1,0,1 ou 2) en V. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   NBPNTU: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant u */
/*           par la methode de Gauss. On doit avoir NBPNTU = 30, 40, */
/*           50 ou 61 et NDJACU-2*(IORDRU+1) < NBPNTU. */
/*   NBPNTV: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant v */
/*           par la methode de Gauss. On doit avoir NBPNTV = 30, 40, */
/*           50 ou 61 et NDJACV-2*(IORDRV+1) < NBPNTV. */
/*   EPSAPR: Table des NBSESP tolerances imposees sur chacun des */
/*           sous-espaces. */
/*   SOSOTB: Tableau de F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau SOSOTB(0,j) contient F(0,vj) + F(0,-vj), */
/*           le tableau SOSOTB(i,0) contient F(ui,0) + F(-ui,0) et */
/*           SOSOTB(0,0) contient F(0,0). */
/*   DISOTB: Tableau de F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau de F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau de F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau DIDITB(0,j) contient F(0,vj) - F(0,-vj), */
/*           et le tableau DIDITB(i,0) contient F(ui,0) - F(-ui,0). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) avec eventuellement prise en compte des */
/*           contraintes. P(u,v) est de degre (NDJACU,NDJACV). */
/*           Ce tableau ne contient les coeff que si ITYDEC = 0. */
/*   ERRMAX: Pour 1<=i<=NBSESP, ERRMAX(i) contient les erreurs maxi */
/*           sur chacun des sous-espaces SI ITYDEC = 0. */
/*   ERRMOY: Contient les erreurs moyennes pour chacun des NBSESP */
/*           sous-espaces SI ITYDEC = 0. */
/*   NDEGPU: Degre en U pour le carreau PATJAC. Valable si ITYDEC=0. */
/*   NDEGPV: Degre en V pour le carreau PATJAC. Valable si ITYDEC=0. */
/*   ITYDEC: Indique si on DOIT decouper encore la fonction F(u,v). */
/*           = 0, on ne DOIT plus rien couper, PATJAC est OK. */
/*           = 1, on ne DOIT couper qu'en U (i.e. inserer un parametre */
/*                de decoupe Ui). */
/*           = 2, on ne DOIT couper qu'en V (i.e. inserer un parametre */
/*                de decoupe Vj). */
/*           = 3, On DOIT couper en U ET en V a la fois. */
/*   IERCOD: Code d'erreur. */
/*           =  0, Eh bien tout va tres bien. */
/*           = -1, On a une solution, la meilleure possible, mais la */
/*                 tolerance utilisateur n'est pas satisfaite (3*helas) */
/*           =  1, Entrees incoherentes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     22-01-1992: RBD; Creation d'apres MA2CF1. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    --errmoy;
    --errmax;
    --epsapr;
    --ndimse;
    patjac_dim1 = *ndjacu + 1;
    patjac_dim2 = *ndjacv + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CE1", 7L);
    }
    *iercod = 0;
    iofwr = 0;

    isz1 = (*nbpntu / 2 + 1) * (*ndjacu - ((*iordru + 1) << 1) + 1);
    isz2 = (*nbpntv / 2 + 1) * (*ndjacv - ((*iordrv + 1) << 1) + 1);
    isz3 = (*nbpntv / 2 + 1) * (*ndjacu - ((*iordru + 1) << 1) + 1) * *ndimen;
    isz4 = *nbpntv / 2 * (*ndjacu - ((*iordru + 1) << 1) + 1) * *ndimen;
    isz5 = *ndjacu + 1 - ((*iordru + 1) << 1);
    isz6 = *ndjacv + 1 - ((*iordrv + 1) << 1);
    isz7 = *ndimen << 2;
    iszwr = isz1 + isz2 + isz3 + isz4 + isz5 + isz6 + isz7;
    AdvApp2Var_SysBase::mcrrqst_(&c__8, &iszwr, wrkar, &iofwr, &ier);
    if (ier > 0) {
	goto L9013;
    }
    ipt1 = iofwr;
    ipt2 = ipt1 + isz1;
    ipt3 = ipt2 + isz2;
    ipt4 = ipt3 + isz3;
    ipt5 = ipt4 + isz4;
    ipt6 = ipt5 + isz5;
    ipt7 = ipt6 + isz6;

/* ----------------- Recup des coeff. d'integr. de Gauss ---------------- 
*/

    AdvApp2Var_ApproxF2var::mmapptt_(ndjacu, nbpntu, iordru, &wrkar[ipt1], iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    AdvApp2Var_ApproxF2var::mmapptt_(ndjacv, nbpntv, iordrv, &wrkar[ipt2], iercod);
    if (*iercod > 0) {
	goto L9999;
    }

/* ------------------- Recup des max des polynomes de Jacobi ------------ 
*/

    AdvApp2Var_ApproxF2var::mma2jmx_(ndjacu, iordru, &wrkar[ipt5]);
    AdvApp2Var_ApproxF2var::mma2jmx_(ndjacv, iordrv, &wrkar[ipt6]);

/* ------ Calcul des coefficients et de leur contribution a l'erreur ---- 
*/

    mma2ce2_(numdec, ndimen, nbsesp, &ndimse[1], ndminu, ndminv, ndguli, 
	    ndgvli, ndjacu, ndjacv, iordru, iordrv, nbpntu, nbpntv, &epsapr[1]
	    , &sosotb[sosotb_offset], &disotb[disotb_offset], &soditb[
	    soditb_offset], &diditb[diditb_offset], &wrkar[ipt1], &wrkar[ipt2]
	    , &wrkar[ipt5], &wrkar[ipt6], &wrkar[ipt7], &wrkar[ipt3], &wrkar[
	    ipt4], &patjac[patjac_offset], &errmax[1], &errmoy[1], ndegpu, 
	    ndegpv, itydec, iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    goto L9999;

/* ------------------------------ The end ------------------------------- 
*/

L9013:
    *iercod = 13;
    goto L9999;

L9999:
    if (iofwr != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, &iszwr, wrkar, &iofwr, &ier);
    }
    if (ier > 0) {
	*iercod = 13;
    }
    AdvApp2Var_SysBase::maermsg_("MMA2CE1", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CE1", 7L);
    }
    return 0;
} /* mma2ce1_ */

//=======================================================================
//function : mma2ce2_
//purpose  : 
//=======================================================================
int mma2ce2_(integer *numdec, 
	     integer *ndimen, 
	     integer *nbsesp, 
	     integer *ndimse, 
	     integer *ndminu, 
	     integer *ndminv, 
	     integer *ndguli, 
	     integer *ndgvli, 
	     integer *ndjacu, 
	     integer *ndjacv, 
	     integer *iordru, 
	     integer *iordrv, 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *epsapr, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *gssutb, 
	     doublereal *gssvtb, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *vecerr, 
	     doublereal *chpair, 
	     doublereal *chimpr, 
	     doublereal *patjac, 
	     doublereal *errmax, 
	     doublereal *errmoy, 
	     integer *ndegpu, 
	     integer *ndegpv, 
	     integer *itydec, 
	     integer *iercod)

{
  /* System generated locals */
  integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
  disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  diditb_dim1, diditb_dim2, diditb_offset, gssutb_dim1, gssvtb_dim1,
  chpair_dim1, chpair_dim2, chpair_offset, chimpr_dim1, 
  chimpr_dim2, chimpr_offset, patjac_dim1, patjac_dim2, 
  patjac_offset, vecerr_dim1, vecerr_offset, i__1, i__2, i__3, i__4;
  
  /* Local variables */
  static logical ldbg;
  static integer idim, igsu, minu, minv, maxu, maxv, igsv;
  static doublereal vaux[3];
  static integer i2rdu, i2rdv, ndses, nd, ii, jj, kk, nu, nv;
  static doublereal zu, zv;
  static integer nu1, nv1;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des coefficients de l' approximation polynomiale de degre */
/*     (NDJACU,NDJACV) d'une fonction F(u,v) quelconque, a partir de sa */
/*     discretisation sur les racines du polynome de Legendre de degre */
/*     NBPNTU en U et NBPNTV en V. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::FONCTION&,APPROXIMATION,&COEFFICIENT,&POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NUMDEC: Indique si on PEUT decouper encore la fonction F(u,v). */
/*           = 5, On PEUT couper en U ou en V ou dans les 2 sens a la */
/*                fois. */
/*           = 4, On PEUT couper en U ou en V MAIS PAS dans les 2 sens */
/*                a la fois (decoupe en V favorisee). */
/*           = 3, On PEUT couper en U ou en V MAIS PAS dans les 2 sens */
/*                a la fois (decoupe en U favorisee). */
/*           = 2, on ne PEUT couper qu'en V (i.e. inserer un parametre */
/*                de decoupe Vj). */
/*           = 1, on ne PEUT couper qu'en U (i.e. inserer un parametre */
/*                de decoupe Ui). */
/*           = 0, on ne PEUT plus rien couper */
/*   NDIMEN: Dimension totale de l'espace */
/*   NBSESP: Nbre de sous-espaces independant sur lesquels on calcule */
/*           les erreurs. */
/*   NDIMSE: Table des dimensions de chacun des sous-espaces. */
/*   NDMINU: Degre minimum en U a conserver pour l'approximation. */
/*   NDMINV: Degre minimum en V a conserver pour l'approximation. */
/*   NDGULI: Limite du degre en U de la solution. */
/*   NDGVLI: Limite du degre en V de la solution. */
/*   NDJACU: Degre maxi du polynome d' approximation en U. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre NDJACU-2*(IORDRU+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2. */
/*           On doit avoir 2*IORDRU+1 <= NDMINU <= NDGULI < NDJACU */
/*   NDJACV: Degre maxi du polynome d' approximation en V. La */
/*           representation dans la base orthogonale part du degre */
/*           0 au degre NDJACV-2*(IORDRV+1). La base polynomiale est */
/*           la base de Jacobi d' ordre -1 (Legendre), 0, 1 ou 2 */
/*           On doit avoir 2*IORDRV+1 <= NDMINV <= NDGVLI < NDJACV */
/*   IORDRU: Ordre de la base de Jacobi (-1,0,1 ou 2) en U. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   IORDRV: Ordre de la base de Jacobi (-1,0,1 ou 2) en V. Correspond */
/*           a pas de contraintes, contraintes C0, C1 ou C2. */
/*   NBPNTU: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant u */
/*           par la methode de Gauss. On doit avoir NBPNTU = 30, 40, */
/*           50 ou 61 et NDJACU-2*(IORDRU+1) < NBPNTU. */
/*   NBPNTV: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant v */
/*           par la methode de Gauss. On doit avoir NBPNTV = 30, 40, */
/*           50 ou 61 et NDJACV-2*(IORDRV+1) < NBPNTV. */
/*   EPSAPR: Table des NBSESP tolerances imposees sur chacun des */
/*           sous-espaces. */
/*   SOSOTB: Tableau de F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau SOSOTB(0,j) contient F(0,vj) + F(0,-vj), */
/*           le tableau SOSOTB(i,0) contient F(ui,0) + F(-ui,0) et */
/*           SOSOTB(0,0) contient F(0,0). */
/*   DISOTB: Tableau de F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau de F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau de F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau DIDITB(0,j) contient F(0,vj) - F(0,-vj), */
/*           et le tableau DIDITB(i,0) contient F(ui,0) - F(-ui,0). */
/*   GSSUTB: Table des coefficients d' integration par la methode de */
/*           Gauss suivant U: i varie de 0 a NBPNTU/2 et k varie de 0 a */
/*           NDJACU-2*(IORDRU+1). */
/*   GSSVTB: Table des coefficients d' integration par la methode de */
/*           Gauss suivant V: i varie de 0 a NBPNTV/2 et k varie de 0 a */
/*           NDJACV-2*(IORDRV+1). */
/*   XMAXJU: Valeur maximale des polynomes de Jacobi d'ordre IORDRU, */
/*           du degre 0 au degre NDJACU - 2*(IORDRU+1) */
/*   XMAXJV: Valeur maximale des polynomes de Jacobi d'ordre IORDRV, */
/*           du degre 0 au degre NDJACV - 2*(IORDRV+1) */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   VECERR: Tableau auxiliaire. */
/*   CHPAIR: Tableau auxiliaire de termes lies au degre NDJACU en U */
/*           pour calculer les coeff. de l'approximation de degre PAIR */
/*           en V. */
/*   CHIMPR: Tableau auxiliaire de termes lies au degre NDJACU en U */
/*           pour calculer les coeff. de l'approximation de degre IMPAIR 
*/
/*           en V. */
/*   PATJAC: Table des coefficients du polynome P(u,v) d' approximation */
/*           de F(u,v) avec eventuellement prise en compte des */
/*           contraintes. P(u,v) est de degre (NDJACU,NDJACV). */
/*           Ce tableau ne contient les coeff que si ITYDEC = 0. */
/*   ERRMAX: Pour 1<=i<=NBSESP, ERRMAX(i) contient les erreurs maxi */
/*           sur chacun des sous-espaces SI ITYDEC = 0. */
/*   ERRMOY: Contient les erreurs moyennes pour chacun des NBSESP */
/*           sous-espaces SI ITYDEC = 0. */
/*   NDEGPU: Degre en U pour le carreau PATJAC. Valable si ITYDEC=0. */
/*   NDEGPV: Degre en V pour le carreau PATJAC. Valable si ITYDEC=0. */
/*   ITYDEC: Indique si on DOIT decouper encore la fonction F(u,v). */
/*           = 0, on ne DOIT plus rien couper, PATJAC est OK ou alors */
/*                NUMDEC etant egal a zero, on ne pouvait plus couper. */
/*           = 1, on ne DOIT couper qu'en U (i.e. inserer un parametre */
/*                de decoupe Ui). */
/*           = 2, on ne DOIT couper qu'en V (i.e. inserer un parametre */
/*                de decoupe Vj). */
/*           = 3, On DOIT couper en U ET en V a la fois. */
/*   IERCOD: Code d'erreur. */
/*           =  0, Eh bien tout va tres bien. */
/*           = -1, On a une solution, la meilleure possible, mais la */
/*                 tolerance utilisateur n'est pas satisfaite (3*helas) */
/*           =  1, Entrees incoherentes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     07-02-1992: RBD; Gestion des cas MINU>MAXU et/ou MINV>MAXV */
/*     05-02-1992: RBD: Prise en compte decalages de CHPAIR et CHIMPR */
/*     22-01-1992: RBD; Creation d'apres MA2CF2. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    vecerr_dim1 = *ndimen;
    vecerr_offset = vecerr_dim1 + 1;
    vecerr -= vecerr_offset;
    --errmoy;
    --errmax;
    --epsapr;
    --ndimse;
    patjac_dim1 = *ndjacu + 1;
    patjac_dim2 = *ndjacv + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;
    gssutb_dim1 = *nbpntu / 2 + 1;
    chimpr_dim1 = *nbpntv / 2;
    chimpr_dim2 = *ndjacu - ((*iordru + 1) << 1) + 1;
    chimpr_offset = chimpr_dim1 * chimpr_dim2 + 1;
    chimpr -= chimpr_offset;
    chpair_dim1 = *nbpntv / 2 + 1;
    chpair_dim2 = *ndjacu - ((*iordru + 1) << 1) + 1;
    chpair_offset = chpair_dim1 * chpair_dim2;
    chpair -= chpair_offset;
    gssvtb_dim1 = *nbpntv / 2 + 1;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CE2", 7L);
    }
/* --> A priori tout va bien */
    *iercod = 0;
/* --> test des entrees */
    if (*numdec < 0 || *numdec > 5) {
	goto L9001;
    }
    if ((*iordru << 1) + 1 > *ndminu) {
	goto L9001;
    }
    if (*ndminu > *ndguli) {
	goto L9001;
    }
    if (*ndguli >= *ndjacu) {
	goto L9001;
    }
    if ((*iordrv << 1) + 1 > *ndminv) {
	goto L9001;
    }
    if (*ndminv > *ndgvli) {
	goto L9001;
    }
    if (*ndgvli >= *ndjacv) {
	goto L9001;
    }
/* --> A priori, pas de decoupes a faire. */
    *itydec = 0;
/* --> Degres mini a retourner: NDMINU,NDMINV */
    *ndegpu = *ndminu;
    *ndegpv = *ndminv;
/* --> Pour le moment, les erreurs max sont nulles */
    AdvApp2Var_SysBase::mvriraz_(nbsesp, (char *)&errmax[1]);
    nd = *ndimen << 2;
    AdvApp2Var_SysBase::mvriraz_(&nd, (char *)&vecerr[vecerr_offset]);
/* --> et le carreau aussi. */
    nd = (*ndjacu + 1) * (*ndjacv + 1) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&nd, (char *)&patjac[patjac_offset]);

    i2rdu = (*iordru + 1) << 1;
    i2rdv = (*iordrv + 1) << 1;

/* ********************************************************************** 
*/
/* -------------------- ICI, ON PEUT ENCORE DECOUPER -------------------- 
*/
/* ********************************************************************** 
*/

    if (*numdec > 0 && *numdec <= 5) {

/* ******************************************************************
**** */
/* ---------------------- Calcul des coeff de la zone 4 -------------
---- */

	minu = *ndguli + 1;
	maxu = *ndjacu;
	minv = *ndgvli + 1;
	maxv = *ndjacv;
	if (minu > maxu) {
	    goto L9001;
	}
	if (minv > maxv) {
	    goto L9001;
	}

/* ---------------- Calcul des termes lies au degre en U ------------
---- */

	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = maxu;
	    for (kk = minu; kk <= i__2; ++kk) {
		igsu = kk - i2rdu;
		mma2cfu_(&kk, nbpntu, nbpntv, &sosotb[nd * sosotb_dim2 * 
			sosotb_dim1], &disotb[(nd * disotb_dim2 + 1) * 
			disotb_dim1 + 1], &soditb[(nd * soditb_dim2 + 1) * 
			soditb_dim1 + 1], &diditb[nd * diditb_dim2 * 
			diditb_dim1], &gssutb[igsu * gssutb_dim1], &chpair[(
			igsu + nd * chpair_dim2) * chpair_dim1], &chimpr[(
			igsu + nd * chimpr_dim2) * chimpr_dim1 + 1]);
/* L110: */
	    }
/* L100: */
	}

/* ------------------- Calcul des coefficients de PATJAC ------------
---- */

	igsu = minu - i2rdu;
	i__1 = maxv;
	for (jj = minv; jj <= i__1; ++jj) {
	    igsv = jj - i2rdv;
	    i__2 = *ndimen;
	    for (nd = 1; nd <= i__2; ++nd) {
		mma2cfv_(&jj, &minu, &maxu, nbpntv, &gssvtb[igsv * 
			gssvtb_dim1], &chpair[(igsu + nd * chpair_dim2) * 
			chpair_dim1], &chimpr[(igsu + nd * chimpr_dim2) * 
			chimpr_dim1 + 1], &patjac[minu + (jj + nd * 
			patjac_dim2) * patjac_dim1]);
/* L130: */
	    }

/* ----- Contribution des termes calcules a l'erreur d'approximati
on ---- */
/* pour les termes (I,J) avec MINU <= I <= MAXU, J fixe. */

	    idim = 1;
	    i__2 = *nbsesp;
	    for (nd = 1; nd <= i__2; ++nd) {
		ndses = ndimse[nd];
		mma2er1_(ndjacu, ndjacv, &ndses, &minu, &maxu, &jj, &jj, 
			iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
			patjac_dim2 * patjac_dim1], &vecerr[vecerr_dim1 + 1], 
			&vecerr[nd + (vecerr_dim1 << 2)]);
		if (vecerr[nd + (vecerr_dim1 << 2)] > epsapr[nd]) {
		    goto L9300;
		}
		idim += ndses;
/* L140: */
	    }
/* L120: */
	}

/* ******************************************************************
**** */
/* ---------------------- Calcul des coeff de la zone 2 -------------
---- */

	minu = (*iordru + 1) << 1;
	maxu = *ndguli;
	minv = *ndgvli + 1;
	maxv = *ndjacv;

/* --> Si la zone 2 est vide, on passe a la zone 3. */
/*    VECERR(ND,2) a deja ete mis a zero. */
	if (minu > maxu) {
	    goto L300;
	}

/* ---------------- Calcul des termes lies au degre en U ------------
---- */

	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = maxu;
	    for (kk = minu; kk <= i__2; ++kk) {
		igsu = kk - i2rdu;
		mma2cfu_(&kk, nbpntu, nbpntv, &sosotb[nd * sosotb_dim2 * 
			sosotb_dim1], &disotb[(nd * disotb_dim2 + 1) * 
			disotb_dim1 + 1], &soditb[(nd * soditb_dim2 + 1) * 
			soditb_dim1 + 1], &diditb[nd * diditb_dim2 * 
			diditb_dim1], &gssutb[igsu * gssutb_dim1], &chpair[(
			igsu + nd * chpair_dim2) * chpair_dim1], &chimpr[(
			igsu + nd * chimpr_dim2) * chimpr_dim1 + 1]);
/* L210: */
	    }
/* L200: */
	}

/* ------------------- Calcul des coefficients de PATJAC ------------
---- */

	igsu = minu - i2rdu;
	i__1 = maxv;
	for (jj = minv; jj <= i__1; ++jj) {
	    igsv = jj - i2rdv;
	    i__2 = *ndimen;
	    for (nd = 1; nd <= i__2; ++nd) {
		mma2cfv_(&jj, &minu, &maxu, nbpntv, &gssvtb[igsv * 
			gssvtb_dim1], &chpair[(igsu + nd * chpair_dim2) * 
			chpair_dim1], &chimpr[(igsu + nd * chimpr_dim2) * 
			chimpr_dim1 + 1], &patjac[minu + (jj + nd * 
			patjac_dim2) * patjac_dim1]);
/* L230: */
	    }
/* L220: */
	}

/* ----- Contribution des termes calcules a l'erreur d'approximation 
---- */
/* pour les termes (I,J) avec MINU <= I <= MAXU, MINV <= J <= MAXV */

	idim = 1;
	i__1 = *nbsesp;
	for (nd = 1; nd <= i__1; ++nd) {
	    ndses = ndimse[nd];
	    mma2er1_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &maxv, 
		    iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
		    patjac_dim2 * patjac_dim1], &vecerr[vecerr_dim1 + 1], &
		    vecerr[nd + (vecerr_dim1 << 1)]);
	    idim += ndses;
/* L240: */
	}

/* ******************************************************************
**** */
/* ---------------------- Calcul des coeff de la zone 3 -------------
---- */

L300:
	minu = *ndguli + 1;
	maxu = *ndjacu;
	minv = (*iordrv + 1) << 1;
	maxv = *ndgvli;

/* --> Si la zone 3 est vide, on passe au test de decoupe. */
/*    VECERR(ND,3) a deja ete mis a zero */
	if (minv > maxv) {
	    goto L400;
	}

/* ----------- Les termes lies au degre en U sont deja calcules -----
---- */
/* ------------------- Calcul des coefficients de PATJAC ------------
---- */

	igsu = minu - i2rdu;
	i__1 = maxv;
	for (jj = minv; jj <= i__1; ++jj) {
	    igsv = jj - i2rdv;
	    i__2 = *ndimen;
	    for (nd = 1; nd <= i__2; ++nd) {
		mma2cfv_(&jj, &minu, &maxu, nbpntv, &gssvtb[igsv * 
			gssvtb_dim1], &chpair[(igsu + nd * chpair_dim2) * 
			chpair_dim1], &chimpr[(igsu + nd * chimpr_dim2) * 
			chimpr_dim1 + 1], &patjac[minu + (jj + nd * 
			patjac_dim2) * patjac_dim1]);
/* L330: */
	    }
/* L320: */
	}

/* ----- Contribution des termes calcules a l'erreur d'approximation 
---- */
/* pour les termes (I,J) avec MINU <= I <= MAXU, MINV <= J <= MAXV. */

	idim = 1;
	i__1 = *nbsesp;
	for (nd = 1; nd <= i__1; ++nd) {
	    ndses = ndimse[nd];
	    mma2er1_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &maxv, 
		    iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
		    patjac_dim2 * patjac_dim1], &vecerr[vecerr_dim1 + 1], &
		    vecerr[nd + vecerr_dim1 * 3]);
	    idim += ndses;
/* L340: */
	}

/* ******************************************************************
**** */
/* --------------------------- Tests de decoupe ---------------------
---- */

L400:
	i__1 = *nbsesp;
	for (nd = 1; nd <= i__1; ++nd) {
	    vaux[0] = vecerr[nd + (vecerr_dim1 << 1)];
	    vaux[1] = vecerr[nd + (vecerr_dim1 << 2)];
	    vaux[2] = vecerr[nd + vecerr_dim1 * 3];
	    ii = 3;
	    errmax[nd] = AdvApp2Var_MathBase::mzsnorm_(&ii, vaux);
	    if (errmax[nd] > epsapr[nd]) {
		ii = 2;
		zv = AdvApp2Var_MathBase::mzsnorm_(&ii, vaux);
		zu = AdvApp2Var_MathBase::mzsnorm_(&ii, &vaux[1]);
		if (zu > epsapr[nd] && zv > epsapr[nd]) {
		    goto L9300;
		}
		if (zu > zv) {
		    goto L9100;
		} else {
		    goto L9200;
		}
	    }
/* L410: */
	}

/* ******************************************************************
**** */
/* --- OK, le carreau est valable, on calcule les coeff de la zone 1 
---- */

	minu = (*iordru + 1) << 1;
	maxu = *ndguli;
	minv = (*iordrv + 1) << 1;
	maxv = *ndgvli;

/* --> Si la zone 1 est vide, on passe au calcul de l'erreur Maxi et 
*/
/*    Moyenne. */
	if (minu > maxu || minv > maxv) {
	    goto L600;
	}

/* ----------- Les termes lies au degre en U sont deja calcules -----
---- */
/* ------------------- Calcul des coefficients de PATJAC ------------
---- */

	igsu = minu - i2rdu;
	i__1 = maxv;
	for (jj = minv; jj <= i__1; ++jj) {
	    igsv = jj - i2rdv;
	    i__2 = *ndimen;
	    for (nd = 1; nd <= i__2; ++nd) {
		mma2cfv_(&jj, &minu, &maxu, nbpntv, &gssvtb[igsv * 
			gssvtb_dim1], &chpair[(igsu + nd * chpair_dim2) * 
			chpair_dim1], &chimpr[(igsu + nd * chimpr_dim2) * 
			chimpr_dim1 + 1], &patjac[minu + (jj + nd * 
			patjac_dim2) * patjac_dim1]);
/* L530: */
	    }
/* L520: */
	}

/* --------------- Maintenant, on baisse le degre au maximum --------
---- */

L600:
/* Computing MAX */
	i__1 = 1, i__2 = (*iordru << 1) + 1, i__1 = max(i__1,i__2);
	minu = max(i__1,*ndminu);
	maxu = *ndguli;
/* Computing MAX */
	i__1 = 1, i__2 = (*iordrv << 1) + 1, i__1 = max(i__1,i__2);
	minv = max(i__1,*ndminv);
	maxv = *ndgvli;
	idim = 1;
	i__1 = *nbsesp;
	for (nd = 1; nd <= i__1; ++nd) {
	    ndses = ndimse[nd];
	    if (maxu >= (*iordru + 1) << 1 && maxv >= (*iordrv + 1) << 1) {
		mma2er2_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &maxv, 
			iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
			patjac_dim2 * patjac_dim1], &epsapr[nd], &vecerr[
			vecerr_dim1 + 1], &errmax[nd], &nu, &nv);
	    } else {
		nu = maxu;
		nv = maxv;
	    }
	    nu1 = nu + 1;
	    nv1 = nv + 1;

/* --> Calcul de l'erreur moyenne. */
	    mma2moy_(ndjacu, ndjacv, &ndses, &nu1, ndjacu, &nv1, ndjacv, 
		    iordru, iordrv, &patjac[idim * patjac_dim2 * patjac_dim1],
		     &errmoy[nd]);

/* --> Mise a 0.D0 des coeff ecartes. */
	    i__2 = idim + ndses - 1;
	    for (ii = idim; ii <= i__2; ++ii) {
		i__3 = *ndjacv;
		for (jj = nv1; jj <= i__3; ++jj) {
		    i__4 = *ndjacu;
		    for (kk = nu1; kk <= i__4; ++kk) {
			patjac[kk + (jj + ii * patjac_dim2) * patjac_dim1] = 
				0.;
/* L640: */
		    }
/* L630: */
		}
/* L620: */
	    }

/* --> Recup des nbre de coeff de l'approximation. */
	    *ndegpu = max(*ndegpu,nu);
	    *ndegpv = max(*ndegpv,nv);
	    idim += ndses;
/* L610: */
	}

/* ******************************************************************
**** */
/* -------------------- LA, ON NE PEUT PLUS DECOUPER ----------------
---- */
/* ******************************************************************
**** */

    } else {
	minu = (*iordru + 1) << 1;
	maxu = *ndjacu;
	minv = (*iordrv + 1) << 1;
	maxv = *ndjacv;

/* ---------------- Calcul des termes lies au degre en U ------------
---- */

	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = maxu;
	    for (kk = minu; kk <= i__2; ++kk) {
		igsu = kk - i2rdu;
		mma2cfu_(&kk, nbpntu, nbpntv, &sosotb[nd * sosotb_dim2 * 
			sosotb_dim1], &disotb[(nd * disotb_dim2 + 1) * 
			disotb_dim1 + 1], &soditb[(nd * soditb_dim2 + 1) * 
			soditb_dim1 + 1], &diditb[nd * diditb_dim2 * 
			diditb_dim1], &gssutb[igsu * gssutb_dim1], &chpair[(
			igsu + nd * chpair_dim2) * chpair_dim1], &chimpr[(
			igsu + nd * chimpr_dim2) * chimpr_dim1 + 1]);
/* L710: */
	    }

/* ---------------------- Calcul de tous les coefficients -------
-------- */

	    igsu = minu - i2rdu;
	    i__2 = maxv;
	    for (jj = minv; jj <= i__2; ++jj) {
		igsv = jj - i2rdv;
		mma2cfv_(&jj, &minu, &maxu, nbpntv, &gssvtb[igsv * 
			gssvtb_dim1], &chpair[(igsu + nd * chpair_dim2) * 
			chpair_dim1], &chimpr[(igsu + nd * chimpr_dim2) * 
			chimpr_dim1 + 1], &patjac[minu + (jj + nd * 
			patjac_dim2) * patjac_dim1]);
/* L720: */
	    }
/* L700: */
	}

/* ----- Contribution des termes calcules a l'erreur d'approximation 
---- */
/* pour les termes (I,J) avec MINU <= I <= MAXU, MINV <= J <= MAXV */

	idim = 1;
	i__1 = *nbsesp;
	for (nd = 1; nd <= i__1; ++nd) {
	    ndses = ndimse[nd];
	    minu = (*iordru + 1) << 1;
	    maxu = *ndjacu;
	    minv = *ndgvli + 1;
	    maxv = *ndjacv;
	    mma2er1_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &maxv, 
		    iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
		    patjac_dim2 * patjac_dim1], &vecerr[vecerr_dim1 + 1], &
		    errmax[nd]);
	    minu = *ndguli + 1;
	    maxu = *ndjacu;
	    minv = (*iordrv + 1) << 1;
	    maxv = *ndgvli;
	    if (minv <= maxv) {
		mma2er1_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &maxv, 
			iordru, iordrv, xmaxju, xmaxjv, &patjac[idim * 
			patjac_dim2 * patjac_dim1], &vecerr[vecerr_dim1 + 1], 
			&errmax[nd]);
	    }

/* ---------------------------- Si ERRMAX > EPSAPR, stop --------
-------- */

	    if (errmax[nd] > epsapr[nd]) {
		*iercod = -1;
		nu = *ndguli;
		nv = *ndgvli;

/* ------------- Sinon, on essaie d'enlever encore des coeff 
------------ */

	    } else {
/* Computing MAX */
		i__2 = 1, i__3 = (*iordru << 1) + 1, i__2 = max(i__2,i__3);
		minu = max(i__2,*ndminu);
		maxu = *ndguli;
/* Computing MAX */
		i__2 = 1, i__3 = (*iordrv << 1) + 1, i__2 = max(i__2,i__3);
		minv = max(i__2,*ndminv);
		maxv = *ndgvli;
		if (maxu >= (*iordru + 1) << 1 && maxv >= (*iordrv + 1) << 1) {
		    mma2er2_(ndjacu, ndjacv, &ndses, &minu, &maxu, &minv, &
			    maxv, iordru, iordrv, xmaxju, xmaxjv, &patjac[
			    idim * patjac_dim2 * patjac_dim1], &epsapr[nd], &
			    vecerr[vecerr_dim1 + 1], &errmax[nd], &nu, &nv);
		} else {
		    nu = maxu;
		    nv = maxv;
		}
	    }

/* --------------------- Calcul de l'erreur moyenne -------------
-------- */

	    nu1 = nu + 1;
	    nv1 = nv + 1;
	    mma2moy_(ndjacu, ndjacv, &ndses, &nu1, ndjacu, &nv1, ndjacv, 
		    iordru, iordrv, &patjac[idim * patjac_dim2 * patjac_dim1],
		     &errmoy[nd]);

/* --------------------- Mise a 0.D0 des coeff ecartes ----------
-------- */

	    i__2 = idim + ndses - 1;
	    for (ii = idim; ii <= i__2; ++ii) {
		i__3 = *ndjacv;
		for (jj = nv1; jj <= i__3; ++jj) {
		    i__4 = *ndjacu;
		    for (kk = nu1; kk <= i__4; ++kk) {
			patjac[kk + (jj + ii * patjac_dim2) * patjac_dim1] = 
				0.;
/* L760: */
		    }
/* L750: */
		}
/* L740: */
	    }

/* --------------- Recup des nbre de coeff de l'approximation ---
-------- */

	    *ndegpu = max(*ndegpu,nu);
	    *ndegpv = max(*ndegpv,nv);
	    idim += ndses;
/* L730: */
	}
    }

    goto L9999;

/* ------------------------------ The end ------------------------------- 
*/
/* --> Erreur dans les entrees */
L9001:
    *iercod = 1;
    goto L9999;

/* --------- Gestion des decoupes, ici doit avoir 0 < NUMDEC <= 5 ------- 
*/

/* --> Ici on peut et on doit couper, on choisit en U si c'est possible */
L9100:
    if (*numdec <= 0 || *numdec > 5) {
	goto L9001;
    }
    if (*numdec != 2) {
	*itydec = 1;
    } else {
	*itydec = 2;
    }
    goto L9999;
/* --> Ici on peut et on doit couper, on choisit en V si c'est possible */
L9200:
    if (*numdec <= 0 || *numdec > 5) {
	goto L9001;
    }
    if (*numdec != 1) {
	*itydec = 2;
    } else {
	*itydec = 1;
    }
    goto L9999;
/* --> Ici on peut et on doit couper, on choisit en 4 si c'est possible */
L9300:
    if (*numdec <= 0 || *numdec > 5) {
	goto L9001;
    }
    if (*numdec == 5) {
	*itydec = 3;
    } else if (*numdec == 2 || *numdec == 4) {
	*itydec = 2;
    } else if (*numdec == 1 || *numdec == 3) {
	*itydec = 1;
    } else {
	goto L9001;
    }
    goto L9999;

L9999:
    AdvApp2Var_SysBase::maermsg_("MMA2CE2", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CE2", 7L);
    }
    return 0;
} /* mma2ce2_ */

//=======================================================================
//function : mma2cfu_
//purpose  : 
//=======================================================================
int mma2cfu_(integer *ndujac, 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *gssutb, 
	     doublereal *chpair, 
	     doublereal *chimpr)

{
  /* System generated locals */
  integer sosotb_dim1, disotb_dim1, disotb_offset, soditb_dim1, 
  soditb_offset, diditb_dim1, i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer nptu2, nptv2, ii, jj;
  static doublereal bid0, bid1, bid2;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des termes lies au degre NDUJAC en U de l' approximation */
/*     polynomiale d' une fonction F(u,v) quelconque, a partir de sa */
/*     discretisation sur les racines du polynome de Legendre de degre */
/*     NBPNTU en U et NBPNTV en V. */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDUJAC: Degre en U fixe pour lequel on calcule les termes */
/*           permettant d'obtenir les coeff. dans Legendre ou Jacobi */
/*           de degre pair ou impair en V. */
/*   NBPNTU: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant U */
/*           par la methode de Gauss. On doit avoir NBPNTU = 30, 40, */
/*           50 ou 61. */
/*   NBPNTV: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant v */
/*           par la methode de Gauss. On doit avoir NBPNTV = 30, 40, */
/*           50 ou 61. */
/*   SOSOTB: Tableau de F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau SOSOTB(0,j) contient F(0,vj) + F(0,-vj), */
/*           le tableau SOSOTB(i,0) contient F(ui,0) + F(-ui,0) et */
/*           SOSOTB(0,0) contient F(0,0). */
/*   DISOTB: Tableau de F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau de F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau de F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. De plus, */
/*           le tableau DIDITB(0,j) contient F(0,vj) - F(0,-vj), */
/*           et le tableau DIDITB(i,0) contient F(ui,0) - F(-ui,0). */
/*   GSSUTB: Table des coefficients d' integration par la methode de */
/*           Gauss suivant U pour NDUJAC fixe: i varie de 0 a NBPNTU/2. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   CHPAIR: Tableau de termes lies au degre NDUJAC en U pour calculer */
/*           les coeff. de l'approximation de degre PAIR en V. */
/*   CHIMPR: Tableau de termes lies au degre NDUJAC en U pour calculer */
/*           les coeff. de l'approximation de degre IMPAIR en V. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     10-06-1991 : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    --chimpr;
    diditb_dim1 = *nbpntu / 2 + 1;
    soditb_dim1 = *nbpntu / 2;
    soditb_offset = soditb_dim1 + 1;
    soditb -= soditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_offset = disotb_dim1 + 1;
    disotb -= disotb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CFU", 7L);
    }

    nptu2 = *nbpntu / 2;
    nptv2 = *nbpntv / 2;

/* ********************************************************************** 
*/
/*                    CALCUL DES COEFFICIENTS EN U */

/* ----------------- Calcul des coefficients de degre pair -------------- 
*/

    if (*ndujac % 2 == 0) {
	i__1 = nptv2;
	for (jj = 1; jj <= i__1; ++jj) {
	    bid1 = 0.;
	    bid2 = 0.;
	    i__2 = nptu2;
	    for (ii = 1; ii <= i__2; ++ii) {
		bid0 = gssutb[ii];
		bid1 += sosotb[ii + jj * sosotb_dim1] * bid0;
		bid2 += soditb[ii + jj * soditb_dim1] * bid0;
/* L200: */
	    }
	    chpair[jj] = bid1;
	    chimpr[jj] = bid2;
/* L100: */
	}

/* --------------- Calcul des coefficients de degre impair ----------
---- */

    } else {
	i__1 = nptv2;
	for (jj = 1; jj <= i__1; ++jj) {
	    bid1 = 0.;
	    bid2 = 0.;
	    i__2 = nptu2;
	    for (ii = 1; ii <= i__2; ++ii) {
		bid0 = gssutb[ii];
		bid1 += disotb[ii + jj * disotb_dim1] * bid0;
		bid2 += diditb[ii + jj * diditb_dim1] * bid0;
/* L250: */
	    }
	    chpair[jj] = bid1;
	    chimpr[jj] = bid2;
/* L150: */
	}
    }

/* ------- Ajout des termes lies a la racine supplementaire (0.D0) ------ 
*/
/* ----------- du polynome de Legendre de degre impair NBPNTU ----------- 
*/
/* --> Seul les termes NDUJAC pair sont modifies car GSSUTB(0) = 0 */
/*    lorsque NDUJAC est impair. */

    if (*nbpntu % 2 != 0 && *ndujac % 2 == 0) {
	bid0 = gssutb[0];
	i__1 = nptv2;
	for (jj = 1; jj <= i__1; ++jj) {
	    chpair[jj] += sosotb[jj * sosotb_dim1] * bid0;
	    chimpr[jj] += diditb[jj * diditb_dim1] * bid0;
/* L300: */
	}
    }

/* ------ Calcul des termes lies a la racine supplementaire (0.D0) ------ 
*/
/* ----------- du polynome de Legendre de degre impair NBPNTV ----------- 
*/

    if (*nbpntv % 2 != 0) {
/* --> Seul les termes CHPAIR sont calcules car GSSVTB(0,IH-IDEBV)=0 
*/
/*    lorsque IH est impair (voir MMA2CFV). */

	if (*ndujac % 2 == 0) {
	    bid1 = 0.;
	    i__1 = nptu2;
	    for (ii = 1; ii <= i__1; ++ii) {
		bid1 += sosotb[ii] * gssutb[ii];
/* L400: */
	    }
	    chpair[0] = bid1;
	} else {
	    bid1 = 0.;
	    i__1 = nptu2;
	    for (ii = 1; ii <= i__1; ++ii) {
		bid1 += diditb[ii] * gssutb[ii];
/* L500: */
	    }
	    chpair[0] = bid1;
	}
	if (*nbpntu % 2 != 0) {
	    chpair[0] += sosotb[0] * gssutb[0];
	}
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CFU", 7L);
    }
    return 0;
} /* mma2cfu_ */

//=======================================================================
//function : mma2cfv_
//purpose  : 
//=======================================================================
int mma2cfv_(integer *ndvjac, 
	     integer *mindgu,
	     integer *maxdgu, 
	     integer *nbpntv, 
	     doublereal *gssvtb, 
	     doublereal *chpair, 
	     doublereal *chimpr, 
	     doublereal *patjac)

{
  /* System generated locals */
  integer chpair_dim1, chpair_offset, chimpr_dim1, chimpr_offset, 
  patjac_offset, i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer nptv2, ii, jj;
  static doublereal bid1;
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des coefficients de l' approximation polynomiale de F(u,v) 
*/
/*     de degre NDVJAC en V et de degre en U variant de MINDGU a MAXDGU. 
*/

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDVJAC: Degre du polynome d' approximation en V. */
/*           La representation dans la base orthogonale part du degre */
/*           0. La base polynomiale est la base de Jacobi d' ordre -1 */
/*           (Legendre), 0, 1 ou 2 */
/*   MINDGU: Degre minimum en U des coeff. a calculer. */
/*   MAXDGU: Degre maximum en U des coeff. a calculer. */
/*   NBPNTV: Degre du polynome de Legendre sur les racines duquel */
/*           sont calcules les coefficients d' integration suivant V */
/*           par la methode de Gauss. On doit avoir NBPNTV = 30, 40, */
/*           50 ou 61 et NDVJAC < NBPNTV. */
/*   GSSVTB: Table des coefficients d' integration par la methode de */
/*           Gauss suivant V pour NDVJAC fixe: j varie de 0 a NBPNTV/2. */
/*   CHPAIR: Tableau de termes lies aux degres MINDGU a MAXDGU en U pour 
*/
/*           calculer les coeff. de l'approximation de degre PAIR NDVJAC 
*/
/*           en V. */
/*   CHIMPR: Tableau de termes lies aux degres MINDGU a MAXDGU en U pour 
*/
/*           calculer les coeff. de l'approximation de degre IMPAIR */
/*           NDVJAC en V. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PATJAC: Table des coefficients en U du polynome d' approximation */
/*           P(u,v) de degre MINDGU a MAXDGU en U et NDVJAC en V. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     11-06-1991 : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    patjac_offset = *mindgu;
    patjac -= patjac_offset;
    chimpr_dim1 = *nbpntv / 2;
    chimpr_offset = chimpr_dim1 * *mindgu + 1;
    chimpr -= chimpr_offset;
    chpair_dim1 = *nbpntv / 2 + 1;
    chpair_offset = chpair_dim1 * *mindgu;
    chpair -= chpair_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2CFV", 7L);
    }
    nptv2 = *nbpntv / 2;

/* --------- Calcul des coefficients pour un degre NDVJAC pair ---------- 
*/

    if (*ndvjac % 2 == 0) {
	i__1 = *maxdgu;
	for (ii = *mindgu; ii <= i__1; ++ii) {
	    bid1 = 0.;
	    i__2 = nptv2;
	    for (jj = 1; jj <= i__2; ++jj) {
		bid1 += chpair[jj + ii * chpair_dim1] * gssvtb[jj];
/* L200: */
	    }
	    patjac[ii] = bid1;
/* L100: */
	}

/* -------- Calcul des coefficients pour un degre NDVJAC impair -----
---- */

    } else {
	i__1 = *maxdgu;
	for (ii = *mindgu; ii <= i__1; ++ii) {
	    bid1 = 0.;
	    i__2 = nptv2;
	    for (jj = 1; jj <= i__2; ++jj) {
		bid1 += chimpr[jj + ii * chimpr_dim1] * gssvtb[jj];
/* L250: */
	    }
	    patjac[ii] = bid1;
/* L150: */
	}
    }

/* ------- Ajout des termes lies a la racine supplementaire (0.D0) ------ 
*/
/* ----------- du polynome de Legendre de degre impair NBPNTV ----------- 
*/

    if (*nbpntv % 2 != 0 && *ndvjac % 2 == 0) {
	bid1 = gssvtb[0];
	i__1 = *maxdgu;
	for (ii = *mindgu; ii <= i__1; ++ii) {
	    patjac[ii] += bid1 * chpair[ii * chpair_dim1];
/* L300: */
	}
    }

/* ------------------------------ The end ------------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2CFV", 7L);
    }
    return 0;
} /* mma2cfv_ */

//=======================================================================
//function : mma2ds1_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2ds1_(integer *ndimen, 
				     doublereal *uintfn, 
				     doublereal *vintfn,
				     void (*foncnp) (
						     int *,
						     double *,
						     double *,
						     int *,
						     double *,
						     int *,
						     double *,
						     int *,
						     int *,
						     double *,
						     int *
						     ),  
				     integer *nbpntu, 
				     integer *nbpntv, 
				     doublereal *urootb, 
				     doublereal *vrootb,
				     integer *isofav,
				     doublereal *sosotb,
				     doublereal *disotb,
				     doublereal *soditb,
				     doublereal *diditb,
				     doublereal *fpntab, 
				     doublereal *ttable,
				     integer *iercod)

{
  /* System generated locals */
  integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
  disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  diditb_dim1, diditb_dim2, diditb_offset, fpntab_dim1, 
  fpntab_offset, i__1;

  /* Local variables */
  static logical ldbg;
  static integer ibid1, ibid2, iuouv, nd;
  static integer isz1, isz2;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation d'une fonction F(u,v) sur les racines des */
/*     polynomes de Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION&,DISCRETISATION,&POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN: Dimension de l' espace. */
/*   UINTFN: Bornes de l' intervalle de definition en u de la fonction */
/*           a approcher: (UINTFN(1),UINTFN(2)). */
/*   VINTFN: Bornes de l' intervalle de definition en v de la fonction */
/*           a approcher: (VINTFN(1),VINTFN(2)). */
/*   FONCNP: Le NOM de la fonction non polynomiale a approcher. */
/*   NBPNTU: Le degre du polynome de Legendre sur les racines duquel */
/*           on discretise FONCNP en u. */
/*   NBPNTV: Le degre du polynome de Legendre sur les racines duquel */
/*           on discretise FONCNP en v. */
/*   UROOTB: Tableau des racines STRICTEMENTS POSITIVES du polynome */
/*           de Legendre de degre NBPNTU defini sur (-1,1). */
/*   VROOTB: Tableau des racines STRICTEMENTS POSITIVES du polynome */
/*           de Legendre de degre NBPNTV defini sur (-1,1). */
/*   ISOFAV: Indique le type d'iso de F(u,v) a extraire pour ameliorer */
/*           la rapidite de calcul (n'a aucune influence sur la forme */
/*           du resultat) */
/*           = 1, indique que l'on doit calculer les points de F(u,v) */
/*           avec u fixe (donc avec NBPNTV valeurs differentes de v). */
/*           = 2, indique que l'on doit calculer les points de F(u,v) */
/*           avec v fixe (donc avec NBPNTU valeurs differentes de u). */
/*   SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*   DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*   SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*   DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   SOSOTB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   FPNTAB: Tableau auxiliaire. */
/*   TTABLE: Tableau auxiliaire. */
/*   IERCOD: Code d' erreur >100 Pb dans l' evaluation de FONCNP, */
/*           le code d'erreur renvoye est egal au code d' erreur */
/*           de FONCNP + 100. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/* -->La fonction externe creee par l' appelant de MA2F1K, MA2FDK */
/*   ou de MA2FXK doit etre de la forme : */
/*    SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,ISOFAV,TCONST,NBPTAB */
/*                     ,TTABLE,IDERIU,IDERIV,PPNTAB,IERCOD) */
/*    ou les arguments d' entree sont : */
/*      - NDIMEN est un entier defini comme la somme des dimensions des */
/*               sous-espaces (i.e. dimension totale du probleme). */
/*      - UINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                  en u ou est definie la fonction a approximer */
/*                  (donc ici egal a UIFONC). */
/*      - VINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                  en v ou est definie la fonction a approximer */
/*                  (donc ici egal a VIFONC). */
/*      - ISOFAV, vaut 1 si l'on veut calculer des points a u constant, */
/*                vaut 2 si l'on calcule les points a v constant. Tout */
/*                autre valeur est une erreur. */
/*      - TCONST, un reel, valeur du parametre fixe. Prend ses valeurs */
/*                dans (UIFONC(1),UIFONC(2)) si ISOFAV = 1 ou dans */
/*                dans (VIFONC(1),VIFONC(2)) si ISOFAV = 2. */
/*      - NBPTAB, un entier. Indique le nombre de points a calculer. */
/*      - TTABLE, un tableau de NBPTAB reels. Ce sont les valeurs du */
/*                parametre 'libre' de discretisation (v si IISOFAV=1, */
/*                u si IISOFAV=2). */
/*      - IDERIU, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRE(1) (derivee partielle de la fonction en u a */
/*                l' ordre IORDRE(1) si IORDRE(1) > 0). */
/*      - IDERIV, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRE(2) (derivee partielle de la fonction en v a */
/*                l' ordre IORDRE(2) si IORDRE(2) > 0). */
/*                Si IDERIU=i et IDERIV=j, FONCNP devra calculer des */
/*                points de la derivee: */
/*                            i+j */
/*                           d     F(u,v) */
/*                        -------- */
/*                           i  j */
/*                         du dv */

/*     et les arguments de sortie sont : */
/*        - FPNTAB(NDIMEN,NBPTAB) contient, en sortie, le tableau des */
/*                                NBPTAB points calcules dans FONCNP. */
/*        - IERCOD est, en sortie, le code d' erreur de FONCNP. Ce code */
/*                 (entier) doit etre strictement positif s' il y a eu */
/*                 un probleme. */

/*     Les arguments d' entree NE DOIVENT PAS etre modifies sous FONCNP. 
*/

/* -->Comme FONCNP n' est pas forcement definie dans (-1,1)*(-1,1), on */
/* modifie les valeurs de UROOTB et VROOTB en consequence. */

/* -->Les resultats de la discretisation sont ranges dans 4 tableaux */
/* SOSOTB, DISOTB, SODITB et DIDITB pour gagner du temps par la suite */
/* lors du calcul des coefficients du polynome d' approximation. */

/*     Lorsque NBPNTU est impair: */
/*        le tableau SOSOTB(0,j) contient F(0,vj) + F(0,-vj), */
/*        le tableau DIDITB(0,j) contient F(0,vj) - F(0,-vj), */
/*     Lorsque NBPNTV est impair: */
/*        le tableau SOSOTB(i,0) contient F(ui,0) + F(-ui,0), */
/*        le tableau DIDITB(i,0) contient F(ui,0) - F(-ui,0), */
/*     Lorsque NBPNTU et NBPNTV sont impairs: */
/*        le terme SOSOTB(0,0) contient F(0,0). */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     06-06-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    fpntab_dim1 = *ndimen;
    fpntab_offset = fpntab_dim1 + 1;
    fpntab -= fpntab_offset;
    --uintfn;
    --vintfn;
    --urootb;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    --vrootb;
    --ttable;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2DS1", 7L);
    }
    *iercod = 0;
    if (*isofav < 1 || *isofav > 2) {
	iuouv = 2;
    } else {
	iuouv = *isofav;
    }

/* ********************************************************************** 
*/
/* --------- Discretisation en U sur les racines du polynome de --------- 
*/
/* --------------- Legendre de degre NBPNTU, iso-V par iso-V ------------ 
*/
/* ********************************************************************** 
*/

    if (iuouv == 2) {
	mma2ds2_(ndimen, &uintfn[1], &vintfn[1], foncnp, nbpntu, nbpntv, &
		urootb[1], &vrootb[1], &iuouv, &sosotb[sosotb_offset], &
		disotb[disotb_offset], &soditb[soditb_offset], &diditb[
		diditb_offset], &fpntab[fpntab_offset], &ttable[1], iercod);

/* ******************************************************************
**** */
/* --------- Discretisation en V sur les racines du polynome de -----
---- */
/* --------------- Legendre de degre NBPNTV, iso-U par iso-U --------
---- */
/* ******************************************************************
**** */

    } else {
/* --> Inversion des indices des tableaux */
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    isz1 = *nbpntu / 2 + 1;
	    isz2 = *nbpntv / 2 + 1;
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &sosotb[nd * sosotb_dim2 * sosotb_dim1], &isz1, &
		    isz2, &isz2, &sosotb[nd * sosotb_dim2 * sosotb_dim1], &
		    ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &diditb[nd * diditb_dim2 * diditb_dim1], &isz1, &
		    isz2, &isz2, &diditb[nd * diditb_dim2 * diditb_dim1], &
		    ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    isz1 = *nbpntu / 2;
	    isz2 = *nbpntv / 2;
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &soditb[(nd * soditb_dim2 + 1) * soditb_dim1 + 1],
		     &isz1, &isz2, &isz2, &soditb[(nd * soditb_dim2 + 1) * 
		    soditb_dim1 + 1], &ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &disotb[(nd * disotb_dim2 + 1) * disotb_dim1 + 1],
		     &isz1, &isz2, &isz2, &disotb[(nd * disotb_dim2 + 1) * 
		    disotb_dim1 + 1], &ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
/* L100: */
	}

	mma2ds2_(ndimen, &vintfn[1], &uintfn[1], foncnp, nbpntv, nbpntu, &
		vrootb[1], &urootb[1], &iuouv, &sosotb[sosotb_offset], &
		soditb[soditb_offset], &disotb[disotb_offset], &diditb[
		diditb_offset], &fpntab[fpntab_offset], &ttable[1], iercod);
/* --> Inversion des indices des tableaux */
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    isz1 = *nbpntv / 2 + 1;
	    isz2 = *nbpntu / 2 + 1;
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &sosotb[nd * sosotb_dim2 * sosotb_dim1], &isz1, &
		    isz2, &isz2, &sosotb[nd * sosotb_dim2 * sosotb_dim1], &
		    ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &diditb[nd * diditb_dim2 * diditb_dim1], &isz1, &
		    isz2, &isz2, &diditb[nd * diditb_dim2 * diditb_dim1], &
		    ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    isz1 = *nbpntv / 2;
	    isz2 = *nbpntu / 2;
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &soditb[(nd * soditb_dim2 + 1) * soditb_dim1 + 1],
		     &isz1, &isz2, &isz2, &soditb[(nd * soditb_dim2 + 1) * 
		    soditb_dim1 + 1], &ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
	    AdvApp2Var_MathBase::mmfmtb1_(&isz1, &disotb[(nd * disotb_dim2 + 1) * disotb_dim1 + 1],
		     &isz1, &isz2, &isz2, &disotb[(nd * disotb_dim2 + 1) * 
		    disotb_dim1 + 1], &ibid1, &ibid2, iercod);
	    if (*iercod > 0) {
		goto L9999;
	    }
/* L200: */
	}
    }

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	*iercod += 100;
	AdvApp2Var_SysBase::maermsg_("MMA2DS1", iercod, 7L);
    }
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2DS1", 7L);
    }
    return 0;
} /* mma2ds1_ */
 
//=======================================================================
//function : mma2ds2_
//purpose  : 
//=======================================================================
int mma2ds2_(integer *ndimen, 
	     doublereal *uintfn, 
	     doublereal *vintfn, 
	     void (*foncnp) (
			     int *,
			     double *,
			     double *,
			     int *,
			     double *,
			     int *,
			     double *,
			     int *,
			     int *,
			     double *,
			     int *
			     ), 
	     integer *nbpntu, 
	     integer *nbpntv, 
	     doublereal *urootb, 
	     doublereal *vrootb, 
	     integer *iiuouv, 
	     doublereal *sosotb, 
	     doublereal *disotb, 
	     doublereal *soditb, 
	     doublereal *diditb, 
	     doublereal *fpntab, 
	     doublereal *ttable, 
	     integer *iercod)

{
  static integer c__0 = 0;
  /* System generated locals */
  integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
  disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  diditb_dim1, diditb_dim2, diditb_offset, fpntab_dim1, 
  fpntab_offset, i__1, i__2, i__3;

  /* Local variables */
  static integer jdec;
  static logical ldbg;
  static doublereal alinu, blinu, alinv, blinv, tcons;
  static doublereal dbfn1[2], dbfn2[2];
  static integer nuroo, nvroo, id, iu, iv;
  static doublereal um, up;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Discretisation d'une fonction F(u,v) sur les racines des polynomes 
*/
/*     de Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION&,DISCRETISATION,&POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN: Dimension de l' espace. */
/*   UINTFN: Bornes de l' intervalle de definition en u de la fonction */
/*           a approcher: (UINTFN(1),UINTFN(2)). */
/*   VINTFN: Bornes de l' intervalle de definition en v de la fonction */
/*           a approcher: (VINTFN(1),VINTFN(2)). */
/*   FONCNP: Le NOM de la fonction non polynomiale a approcher. */
/*   NBPNTU: Le degre du polynome de Legendre sur les racines duquel */
/*           on discretise FONCNP en u. */
/*   NBPNTV: Le degre du polynome de Legendre sur les racines duquel */
/*           on discretise FONCNP en v. */
/*   UROOTB: Tableau des racines STRICTEMENTS POSITIVES du polynome */
/*           de Legendre de degre NBPNTU defini sur (-1,1). */
/*   VROOTB: Tableau des racines STRICTEMENTS POSITIVES du polynome */
/*           de Legendre de degre NBPNTV defini sur (-1,1). */
/*   IIUOUV: Indique le type d'iso de F(u,v) a extraire pour ameliorer */
/*           la rapidite de calcul (n'a aucune influence sur la forme */
/*           du resultat) */
/*           = 1, indique que l'on doit calculer les points de F(u,v) */
/*           avec u fixe (donc avec NBPNTV valeurs differentes de v). */
/*           = 2, indique que l'on doit calculer les points de F(u,v) */
/*           avec v fixe (donc avec NBPNTU valeurs differentes de u). */
/*   SOSOTB: Tableau deja initialise (argument d'entree/sortie). */
/*   DISOTB: Tableau deja initialise (argument d'entree/sortie). */
/*   SODITB: Tableau deja initialise (argument d'entree/sortie). */
/*   DIDITB: Tableau deja initialise (argument d'entree/sortie). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   SOSOTB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DISOTB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   SODITB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   DIDITB: Tableau ou l'on ajoute les termes */
/*           F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui et vj racines positives du polynome de Legendre */
/*           de degre NBPNTU et NBPNTV respectivement. */
/*   FPNTAB: Tableau auxiliaire. */
/*   TTABLE: Tableau auxiliaire. */
/*   IERCOD: Code d' erreur >100 Pb dans l' evaluation de FONCNP, */
/*           le code d'erreur renvoye est egal au code d' erreur */
/*           de FONCNP + 100. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/* -->La fonction externe creee par l' appelant de MA2F1K, MA2FDK */
/*   ou de MA2FXK doit etre de la forme : */
/*    SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,IIIUOUV,TCONST,NBPTAB */
/*                     ,TTABLE,IDERIU,IDERIV,PPNTAB,IERCOD) */
/*    ou les arguments d' entree sont : */
/*      - NDIMEN est un entier defini comme la somme des dimensions des */
/*               sous-espaces (i.e. dimension totale du probleme). */
/*      - UINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                  en u ou est definie la fonction a approximer */
/*                  (donc ici egal a UIFONC). */
/*      - VINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                  en v ou est definie la fonction a approximer */
/*                  (donc ici egal a VIFONC). */
/*      - IIIUOUV, vaut 1 si l'on veut calculer des points a u constant, 
*/
/*                vaut 2 si l'on calcule les points a v constant. Tout */
/*                autre valeur est une erreur. */
/*      - TCONST, un reel, valeur du parametre fixe. Prend ses valeurs */
/*                dans (UIFONC(1),UIFONC(2)) si IIUOUV = 1 ou dans */
/*                dans (VIFONC(1),VIFONC(2)) si IIUOUV = 2. */
/*      - NBPTAB, un entier. Indique le nombre de points a calculer. */
/*      - TTABLE, un tableau de NBPTAB reels. Ce sont les valeurs du */
/*                parametre 'libre' de discretisation (v si IIIUOUV=1, */
/*                u si IIIUOUV=2). */
/*      - IDERIU, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRE(1) (derivee partielle de la fonction en u a */
/*                l' ordre IORDRE(1) si IORDRE(1) > 0). */
/*      - IDERIV, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRE(2) (derivee partielle de la fonction en v a */
/*                l' ordre IORDRE(2) si IORDRE(2) > 0). */
/*                Si IDERIU=i et IDERIV=j, FONCNP devra calculer des */
/*                points de la derivee: */
/*                            i+j */
/*                           d     F(u,v) */
/*                        -------- */
/*                           i  j */
/*                         du dv */

/*     et les arguments de sortie sont : */
/*        - FPNTAB(NDIMEN,NBPTAB) contient, en sortie, le tableau des */
/*                                NBPTAB points calcules dans FONCNP. */
/*        - IERCOD est, en sortie, le code d' erreur de FONCNP. Ce code */
/*                 (entier) doit etre strictement positif s' il y a eu */
/*                 un probleme. */

/*     Les arguments d' entree NE DOIVENT PAS etre modifies sous FONCNP. 
*/

/* -->Comme FONCNP n' est pas forcement definie dans (-1,1)*(-1,1), on */
/* modifie les valeurs de UROOTB et VROOTB en consequence. */

/* -->Les resultats de la discretisation sont ranges dans 4 tableaux */
/* SOSOTB, DISOTB, SODITB et DIDITB pour gagner du temps par la suite */
/* lors du calcul des coefficients du polynome d' approximation. */

/*     Lorsque NBPNTU est impair: */
/*        le tableau SOSOTB(0,j) contient F(0,vj) + F(0,-vj), */
/*        le tableau DIDITB(0,j) contient F(0,vj) - F(0,-vj), */
/*     Lorsque NBPNTV est impair: */
/*        le tableau SOSOTB(i,0) contient F(ui,0) + F(-ui,0), */
/*        le tableau DIDITB(i,0) contient F(ui,0) - F(-ui,0), */
/*     Lorsque NBPNTU et NBPNTV sont impairs: */
/*        le terme SOSOTB(0,0) contient F(0,0). */

/*   ATTENTION: On remplit toujours ces 4 tableaux en faisant varier */
/*   le 1er indice d'abord. C'est a dire que les discretisations */
/*   de F(...,t) (pour IIUOUV = 2) ou de F(t,...) (IIUOUV = 1) */
/*   sont stockees dans SOSOTB(...,t), SODITB(...,t), etc... */
/*   (ceci permet un gain de temps non negligeable). */
/*   Il faut donc que l'appelant, dans le cas ou IIUOUV=1, */
/*   intervertisse les roles de u et v, de SODITB et DISOTB AVANT le */
/*   calcul puis, APRES le calcul prenne la transposee des 4 tableau. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     26-09-1996: JCT; TCONS toujours defini sur VINTFN, d'ou init. */
/*                      de DBFN1, DBFN2 en fonction de IIUOUV. */
/*     06-06-1991: RBD; Finalisation du developpement. */
/*     31-07-1989: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

/* --> Indices de boucles. */

/* --------------------------- Initialisations -------------------------- 
*/

    /* Parameter adjustments */
    --uintfn;
    --vintfn;
    --ttable;
    fpntab_dim1 = *ndimen;
    fpntab_offset = fpntab_dim1 + 1;
    fpntab -= fpntab_offset;
    --urootb;
    diditb_dim1 = *nbpntu / 2 + 1;
    diditb_dim2 = *nbpntv / 2 + 1;
    diditb_offset = diditb_dim1 * diditb_dim2;
    diditb -= diditb_offset;
    soditb_dim1 = *nbpntu / 2;
    soditb_dim2 = *nbpntv / 2;
    soditb_offset = soditb_dim1 * (soditb_dim2 + 1) + 1;
    soditb -= soditb_offset;
    disotb_dim1 = *nbpntu / 2;
    disotb_dim2 = *nbpntv / 2;
    disotb_offset = disotb_dim1 * (disotb_dim2 + 1) + 1;
    disotb -= disotb_offset;
    sosotb_dim1 = *nbpntu / 2 + 1;
    sosotb_dim2 = *nbpntv / 2 + 1;
    sosotb_offset = sosotb_dim1 * sosotb_dim2;
    sosotb -= sosotb_offset;
    --vrootb;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2DS2", 7L);
    }
    *iercod = 0;

    alinu = (uintfn[2] - uintfn[1]) / 2.;
    blinu = (uintfn[2] + uintfn[1]) / 2.;
    alinv = (vintfn[2] - vintfn[1]) / 2.;
    blinv = (vintfn[2] + vintfn[1]) / 2.;

    if (*iiuouv == 1) {
     dbfn1[0] = vintfn[1];
     dbfn1[1] = vintfn[2];
     dbfn2[0] = uintfn[1];
     dbfn2[1] = uintfn[2];
    } else {
     dbfn1[0] = uintfn[1];
     dbfn1[1] = uintfn[2];
     dbfn2[0] = vintfn[1];
     dbfn2[1] = vintfn[2];
    }

/* ********************************************************************** 
*/
/* -------- Discretisation en U sur les racines du polynome de ---------- 
*/
/* ---------------- Legendre de degre NBPNTU, a Vj fixe  ---------------- 
*/
/* ********************************************************************** 
*/

    nuroo = *nbpntu / 2;
    nvroo = *nbpntv / 2;
    jdec = (*nbpntu + 1) / 2;

/* ----------- Chargement des parametres de discretisation en U --------- 
*/

    i__1 = *nbpntu;
    for (iu = 1; iu <= i__1; ++iu) {
	ttable[iu] = blinu + alinu * urootb[iu];
/* L100: */
    }

/* -------------- Pour Vj fixe, racine de Legendre negative ------------- 
*/

    i__1 = nvroo;
    for (iv = 1; iv <= i__1; ++iv) {
	tcons = blinv + alinv * vrootb[iv];
	(*foncnp)(ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
		ttable[1], &c__0, &c__0, &fpntab[fpntab_offset], iercod);
	if (*iercod > 0) {
	    goto L9999;
	}
	i__2 = *ndimen;
	for (id = 1; id <= i__2; ++id) {
	    i__3 = nuroo;
	    for (iu = 1; iu <= i__3; ++iu) {
		up = fpntab[id + (iu + jdec) * fpntab_dim1];
		um = fpntab[id + (nuroo - iu + 1) * fpntab_dim1];
		sosotb[iu + (nvroo - iv + 1 + id * sosotb_dim2) * sosotb_dim1]
			 = sosotb[iu + (nvroo - iv + 1 + id * sosotb_dim2) * 
			sosotb_dim1] + up + um;
		disotb[iu + (nvroo - iv + 1 + id * disotb_dim2) * disotb_dim1]
			 = disotb[iu + (nvroo - iv + 1 + id * disotb_dim2) * 
			disotb_dim1] + up - um;
		soditb[iu + (nvroo - iv + 1 + id * soditb_dim2) * soditb_dim1]
			 = soditb[iu + (nvroo - iv + 1 + id * soditb_dim2) * 
			soditb_dim1] - up - um;
		diditb[iu + (nvroo - iv + 1 + id * diditb_dim2) * diditb_dim1]
			 = diditb[iu + (nvroo - iv + 1 + id * diditb_dim2) * 
			diditb_dim1] - up + um;
/* L220: */
	    }
	    if (*nbpntu % 2 != 0) {
		up = fpntab[id + jdec * fpntab_dim1];
		sosotb[(nvroo - iv + 1 + id * sosotb_dim2) * sosotb_dim1] += 
			up;
		diditb[(nvroo - iv + 1 + id * diditb_dim2) * diditb_dim1] -= 
			up;
	    }
/* L210: */
	}
/* L200: */
    }

/* --------- Pour Vj = 0 (NBPNTV impair), discretisation en U ----------- 
*/

    if (*nbpntv % 2 != 0) {
	tcons = blinv;
	(*foncnp)(ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
		ttable[1], &c__0, &c__0, &fpntab[fpntab_offset], iercod);
	if (*iercod > 0) {
	    goto L9999;
	}
	i__1 = *ndimen;
	for (id = 1; id <= i__1; ++id) {
	    i__2 = nuroo;
	    for (iu = 1; iu <= i__2; ++iu) {
		up = fpntab[id + (jdec + iu) * fpntab_dim1];
		um = fpntab[id + (nuroo - iu + 1) * fpntab_dim1];
		sosotb[iu + id * sosotb_dim2 * sosotb_dim1] = sosotb[iu + id *
			 sosotb_dim2 * sosotb_dim1] + up + um;
		diditb[iu + id * diditb_dim2 * diditb_dim1] = diditb[iu + id *
			 diditb_dim2 * diditb_dim1] + up - um;
/* L310: */
	    }
	    if (*nbpntu % 2 != 0) {
		up = fpntab[id + jdec * fpntab_dim1];
		sosotb[id * sosotb_dim2 * sosotb_dim1] += up;
	    }
/* L300: */
	}
    }

/* -------------- Pour Vj fixe, racine de Legendre positive ------------- 
*/

    i__1 = nvroo;
    for (iv = 1; iv <= i__1; ++iv) {
	tcons = alinv * vrootb[(*nbpntv + 1) / 2 + iv] + blinv;
	(*foncnp)(ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
		ttable[1], &c__0, &c__0, &fpntab[fpntab_offset], iercod);
	if (*iercod > 0) {
	    goto L9999;
	}
	i__2 = *ndimen;
	for (id = 1; id <= i__2; ++id) {
	    i__3 = nuroo;
	    for (iu = 1; iu <= i__3; ++iu) {
		up = fpntab[id + (iu + jdec) * fpntab_dim1];
		um = fpntab[id + (nuroo - iu + 1) * fpntab_dim1];
		sosotb[iu + (iv + id * sosotb_dim2) * sosotb_dim1] = sosotb[
			iu + (iv + id * sosotb_dim2) * sosotb_dim1] + up + um;
		disotb[iu + (iv + id * disotb_dim2) * disotb_dim1] = disotb[
			iu + (iv + id * disotb_dim2) * disotb_dim1] + up - um;
		soditb[iu + (iv + id * soditb_dim2) * soditb_dim1] = soditb[
			iu + (iv + id * soditb_dim2) * soditb_dim1] + up + um;
		diditb[iu + (iv + id * diditb_dim2) * diditb_dim1] = diditb[
			iu + (iv + id * diditb_dim2) * diditb_dim1] + up - um;
/* L420: */
	    }
	    if (*nbpntu % 2 != 0) {
		up = fpntab[id + jdec * fpntab_dim1];
		sosotb[(iv + id * sosotb_dim2) * sosotb_dim1] += up;
		diditb[(iv + id * diditb_dim2) * diditb_dim1] += up;
	    }
/* L410: */
	}
/* L400: */
    }

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	*iercod += 100;
	AdvApp2Var_SysBase::maermsg_("MMA2DS2", iercod, 7L);
    }
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2DS2", 7L);
    }
    return 0;
} /* mma2ds2_ */

//=======================================================================
//function : mma2er1_
//purpose  : 
//=======================================================================
int mma2er1_(integer *ndjacu, 
	     integer *ndjacv, 
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *patjac, 
	     doublereal *vecerr, 
	     doublereal *erreur)

{
  /* System generated locals */
  integer patjac_dim1, patjac_dim2, patjac_offset, i__1, i__2, i__3;
  doublereal d__1;
  
  /* Local variables */
  static logical ldbg;
  static integer minu, minv;
  static doublereal vaux[2];
  static integer ii, nd, jj;
  static doublereal bid0, bid1;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Calcule l' erreur d' approximation maxi faite lorsque l'on */
/*  enleve les coefficients de PATJAC t.q. le degre en U varie entre */
/*  MINDGU et MAXDGU et le degre en V varie entre MINDGV et MAXDGV. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI:: CARREAU&,CALCUL,&ERREUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDJACU: Dimension en U du tableau PATJAC. */
/*     NDJACV: Dimension en V du tableau PATJAC. */
/*     NDIMEN: Dimension de l'espace. */
/*     MINDGU: Borne inf de l'indice en U des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MAXDGU: Borne sup de l'indice en U des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MINDGV: Borne inf de l'indice en V des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MAXDGV: Borne sup de l'indice en V des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     IORDRU: Ordre de continuite en U assure par le carreau PATJAC */
/*             (de -1 a 2) */
/*     IORDRV: Ordre de continuite en V assure par le carreau PATJAC */
/*             (de -1 a 2) */
/*     XMAXJU: Valeur maximale des polynomes de Jacobi d'ordre IORDRU, */
/*             du degre 0 a MAXDGU - 2*(IORDU+1) */
/*     XMAXJV: Valeur maximale des polynomes de Jacobi d'ordre IORDRV, */
/*             du degre 0 a MAXDGV - 2*(IORDV+1) */
/*     PATJAC: Table des coeff. du carreau d'approximation avec */
/*             contraintes d'ordre IORDRU en U et IORDRV en V. */

/*     VECERR: Vecteur auxiliaire. */
/*     ERREUR: L'erreur MAXI commise enlevant les coeff de PATJAC */
/*             DEJA CALCULEE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     ERREUR: L'erreur MAXI commise enlevant les coeff de PATJAC */
/*             d'indices MINDGU a MAXDGU en U et MINDGV a MAXDGV en V */
/*             PLUS l'erreur deja calculee. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Dans le tableau PATJAC sont stockes les coeff. Cij du carreau */
/*     d'approximation de F(U,V). Les indices i et j indique le degre en 
*/
/*     U et en V des polynomes de base. Ces polynomes de base sont de la 
*/
/*     forme: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), ou */

/*     le polynome J(i-2*(IORDU+1)(U) est le polynome de Jacobi d'ordre */
/*     IORDRU+1 (idem en V en remplacant U par V dans l'expression ci */
/*     dessus). */

/*     La contribution a l'erreur du terme Cij lorsque celui-ci est */
/*     enleve de PATJAC est majoree par: */

/*  DABS(Cij)*XMAXJU(i-2*(IORDRU+1))*XMAXJV(J-2*(IORDRV+1)) ou on a */

/*  XMAXJU(i-2*(IORDRU+1) = ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), 
*/
/*  XMAXJV(i-2*(IORDRV+1) = ((1 - V*V)**(IORDRV+1)).J(j-2*(IORDRV+1)(V). 
*/

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     22-01-1992:RBD; Creation d'apres MA2ERR. */
/* > */
/* ***********************************************************************
 */
/*   Le nom de la routine */


/* ----------------------------- Initialisations ------------------------ 
*/

    /* Parameter adjustments */
    --vecerr;
    patjac_dim1 = *ndjacu + 1;
    patjac_dim2 = *ndjacv + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2ER1", 7L);
    }

    minu = (*iordru + 1) << 1;
    minv = (*iordrv + 1) << 1;

/* ------------------- Calcul du majorant de l'erreur max --------------- 
*/
/* ----- lorsque sont enleves les coeff. d'indices MINDGU a MAXDGU ------ 
*/
/* ---------------- en U et d'indices MINDGV a MAXDGV en V -------------- 
*/

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	bid1 = 0.;
	i__2 = *maxdgv;
	for (jj = *mindgv; jj <= i__2; ++jj) {
	    bid0 = 0.;
	    i__3 = *maxdgu;
	    for (ii = *mindgu; ii <= i__3; ++ii) {
		bid0 += (d__1 = patjac[ii + (jj + nd * patjac_dim2) * 
			patjac_dim1], abs(d__1)) * xmaxju[ii - minu];
/* L300: */
	    }
	    bid1 = bid0 * xmaxjv[jj - minv] + bid1;
/* L200: */
	}
	vecerr[nd] = bid1;

/* L100: */
    }

/* ----------------------- Calcul de l' erreur max ---------------------- 
*/

    bid1 = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vecerr[1]);
    vaux[0] = *erreur;
    vaux[1] = bid1;
    nd = 2;
    *erreur = AdvApp2Var_MathBase::mzsnorm_(&nd, vaux);

/* ------------------------- The end ------------------------------------ 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2ER1", 7L);
    }
    return 0;
} /* mma2er1_ */

//=======================================================================
//function : mma2er2_
//purpose  : 
//=======================================================================
int mma2er2_(integer *ndjacu, 
	     integer *ndjacv,
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *xmaxju, 
	     doublereal *xmaxjv, 
	     doublereal *patjac, 
	     doublereal *epmscut, 
	     doublereal *vecerr, 
	     doublereal *erreur, 
	     integer *newdgu, 
	     integer *newdgv)

{
  /* System generated locals */
  integer patjac_dim1, patjac_dim2, patjac_offset, i__1, i__2;
  doublereal d__1;
  
  /* Local variables */
  static logical ldbg;
  static doublereal vaux[2];
  static integer i2rdu, i2rdv;
  static doublereal errnu, errnv;
  static integer ii, nd, jj, nu, nv;
  static doublereal bid0, bid1;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Enleve des coefficients de PATJAC jusqu'a obtenir les degre en U */
/*  et V minimum verifiant la tolerance imposee. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI:: CARREAU&,CALCUL,&ERREUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/* NDJACU: Degre en U du tableau PATJAC. */
/* NDJACV: Degre en V du tableau PATJAC. */
/* NDIMEN: Dimension de l'espace. */
/* MINDGU: Borne de l'indice en U des coeff. de PATJAC a GARDER */
/*         (doit etre >= 0). */
/* MAXDGU: Borne sup de l'indice en U des coeff. de PATJAC a prendre */
/*         en compte. */
/* MINDGV: Borne de l'indice en V des coeff. de PATJAC a GARDER */
/*         (doit etre >= 0). */
/* MAXDGV: Borne sup de l'indice en V des coeff. de PATJAC a prendre */
/*         en compte. */
/* IORDRU: Ordre de continuite en U assure par le carreau PATJAC */
/*         (de -1 a 2) */
/* IORDRV: Ordre de continuite en V assure par le carreau PATJAC */
/*         (de -1 a 2) */
/* XMAXJU: Valeur maximale des polynomes de Jacobi d'ordre IORDRU, */
/*         du degre 0 a MAXDGU - 2*(IORDU+1) */
/* XMAXJV: Valeur maximale des polynomes de Jacobi d'ordre IORDRV, */
/*         du degre 0 a MAXDGV - 2*(IORDV+1) */
/* PATJAC: Table des coeff. du carreau d'approximation avec */
/*         contraintes d'ordre IORDRU en U et IORDRV en V. */
/* EPMSCUT: Tolerance d'approximation. */
/* VECERR: tableau auxiliaire. */
/* ERREUR: L'erreur MAXI commise DEJA CALCULEE. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/* ERREUR: L'erreur MAXI commise en ne gardant que les coeff de */
/*         PATJAC d'indices 0 a NEWDGU en U et 0 a NEWDGV en V, */
/*         PLUS l'erreur maxi deja calculee. */
/* NEWDGU: Degre en U minimum t.q. le carreau d'approximation */
/*         verifie la tolerance. On a toujours NEWDGU >= MINDGU >= 0. */
/* NEWDGV: Degre en V minimum t.q. le carreau d'approximation */
/*         verifie la tolerance. On a toujours NEWDGV >= MINDGV >= 0. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Dans le tableau PATJAC sont stockes les coeff. Cij du carreau */
/*     d'approximation de F(U,V). Les indices i et j indique le degre */
/*     en U et en V des polynomes de base. Ces polynomes de base sont */
/*     de la forme: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), ou */

/*     le polynome J(i-2*(IORDU+1)(U) est le polynome de Jacobi d'ordre */
/*     IORDRU+1 (idem en V en remplacant U par V dans l'expression ci */
/*     dessus). */

/*     La contribution a l'erreur du terme Cij lorsque celui-ci est */
/*     enleve de PATJAC est majoree par: */

/*  DABS(Cij)*XMAXJU(i-2*(IORDRU+1))*XMAXJV(J-2*(IORDRV+1)) ou on a */

/*  XMAXJU(i-2*(IORDRU+1) = ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), 
*/
/*  XMAXJV(i-2*(IORDRV+1) = ((1 - V*V)**(IORDRV+1)).J(j-2*(IORDRV+1)(V). 
*/

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     23-01-1992: RBD; Creation d'apres MA2CUT. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* ----------------------------- Initialisations ------------------------ 
*/

    /* Parameter adjustments */
    --vecerr;
    patjac_dim1 = *ndjacu + 1;
    patjac_dim2 = *ndjacv + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2ER2", 7L);
    }

    i2rdu = (*iordru + 1) << 1;
    i2rdv = (*iordrv + 1) << 1;
    nu = *maxdgu;
    nv = *maxdgv;

/* ********************************************************************** 
*/
/* -------------------- Coupure des coefficients ------------------------ 
*/
/* ********************************************************************** 
*/

L1001:

/* ------------------- Calcul du majorant de l'erreur max --------------- 
*/
/* ----- lorsque sont enleves les coeff. d'indices MINDGU a MAXDGU ------ 
*/
/* ---------------- en U, le degre en V etant fixe a NV ----------------- 
*/

    bid0 = 0.;
    if (nv > *mindgv) {
	bid0 = xmaxjv[nv - i2rdv];
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    bid1 = 0.;
	    i__2 = nu;
	    for (ii = i2rdu; ii <= i__2; ++ii) {
		bid1 += (d__1 = patjac[ii + (nv + nd * patjac_dim2) * 
			patjac_dim1], abs(d__1)) * xmaxju[ii - i2rdu] * bid0;
/* L200: */
	    }
	    vecerr[nd] = bid1;
/* L100: */
	}
    } else {
	vecerr[1] = *epmscut * 2;
    }
    errnv = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vecerr[1]);

/* ------------------- Calcul du majorant de l'erreur max --------------- 
*/
/* ----- lorsque sont enleves les coeff. d'indices MINDGV a MAXDGV ------ 
*/
/* ---------------- en V, le degre en U etant fixe a NU ----------------- 
*/

    bid0 = 0.;
    if (nu > *mindgu) {
	bid0 = xmaxju[nu - i2rdu];
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    bid1 = 0.;
	    i__2 = nv;
	    for (jj = i2rdv; jj <= i__2; ++jj) {
		bid1 += (d__1 = patjac[nu + (jj + nd * patjac_dim2) * 
			patjac_dim1], abs(d__1)) * xmaxjv[jj - i2rdv] * bid0;
/* L400: */
	    }
	    vecerr[nd] = bid1;
/* L300: */
	}
    } else {
	vecerr[1] = *epmscut * 2;
    }
    errnu = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vecerr[1]);

/* ----------------------- Calcul de l' erreur max ---------------------- 
*/

    vaux[0] = *erreur;
    vaux[1] = errnu;
    nd = 2;
    errnu = AdvApp2Var_MathBase::mzsnorm_(&nd, vaux);
    vaux[1] = errnv;
    errnv = AdvApp2Var_MathBase::mzsnorm_(&nd, vaux);

    if (errnu > errnv) {
	if (errnv < *epmscut) {
	    *erreur = errnv;
	    --nv;
	} else {
	    goto L2001;
	}
    } else {
	if (errnu < *epmscut) {
	    *erreur = errnu;
	    --nu;
	} else {
	    goto L2001;
	}
    }

    goto L1001;

/* -------------------------- Recuperation des degres ------------------- 
*/

L2001:
    *newdgu = max(nu,1);
    *newdgv = max(nv,1);

/* ----------------------------------- The end -------------------------- 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2ER2", 7L);
    }
    return 0;
} /* mma2er2_ */

//=======================================================================
//function : mma2fnc_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2fnc_(integer *ndimen, 
				     integer *nbsesp, 
				     integer *ndimse, 
				     doublereal *uvfonc, 
				     void (*foncnp) (
						     int *,
						     double *,
						     double *,
						     int *,
						     double *,
						     int *,
						     double *,
						     int *,
						     int *,
						     double *,
						     int *
						     ),  
				     doublereal *tconst, 
				     integer *isofav, 
				     integer *nbroot, 
				     doublereal *rootlg, 
				     integer *iordre, 
				     integer *ideriv, 
				     integer *ndgjac, 
				     integer *nbcrmx, 
				     integer *ncflim, 
				     doublereal *epsapr, 
				     integer *ncoeff, 
				     doublereal *courbe, 
				     integer *nbcrbe, 
				     doublereal *somtab, 
				     doublereal *diftab, 
				     doublereal *contr1, 
				     doublereal *contr2, 
				     doublereal *tabdec, 
				     doublereal *errmax, 
				     doublereal *errmoy, 
				     integer *iercod)

{
  static integer c__8 = 8;

   /* System generated locals */
    integer courbe_dim1, courbe_dim2, courbe_offset, somtab_dim1, somtab_dim2,
	     somtab_offset, diftab_dim1, diftab_dim2, diftab_offset, 
	    contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
	     contr2_offset, errmax_dim1, errmax_offset, errmoy_dim1, 
	    errmoy_offset, i__1;
    doublereal d__1;

    /* Local variables */
    static integer ideb;
    static doublereal tmil;
    static integer  ideb1, ibid1, ibid2, ncfja, ndgre, ilong, 
	    ndwrk;
    static doublereal wrkar[1];
    static integer nupil;
    static long int iofwr;
    static doublereal uvpav[4]	/* was [2][2] */;
    static integer nd, ii;
    static integer ibb;
    static integer ier;
    static doublereal uv11[4]	/* was [2][2] */;
  static integer ncb1;
    static doublereal eps3;
    static integer isz1, isz2, isz3, isz4, isz5;
    static long int ipt1, ipt2, ipt3, ipt4, ipt5,iptt, jptt;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/* Approximation d'UNE frontiere d'une fonction non polynomiale F(u,v) */
/* (dans l' espace de dimension NDIMEN) par PLUSIEURS courbes */
/* polynomiales, par la methode des moindres carres. Le parametre de la */
/* fonction est conserve. */

/*     MOTS CLES : */
/*     ----------- */
/* TOUS, AB_SPECIFI :: FONCTION&,EXTREMITE&, APPROXIMATION, &COURBE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN: Dimension totale de l' espace (somme des dimensions */
/*             des sous-espaces) */
/*     NBSESP: Nombre de sous-espaces "independants". */
/*     NDIMSE: Table des dimensions des sous-espaces. */
/*     UVFONC: Bornes de l' intervalle (a,b)x(c,d) de definition de la */
/*             fonction a approcher en U (UVFONC(*,1) contient (a,b)) */
/*             et en V (UVFONC(*,2) contient (c,d)). */
/*     FONCNP: Fonction externe de positionnement sur la fonction non */
/*             polynomiale a approcher. */
/*     TCONST: Valeur de l'isoparametre de F(u,v) a discretiser. */
/*     ISOFAV: Type d'iso choisi, = 1, indique que l'on discretise a u */
/*             fixe; = 2, indique que v est fixe. */
/*     NBROOT: Nbre de points de discretisation de l'iso, extremites non 
*/
/*             comprises. */
/*     ROOTLG: Table des racines du polynome de Legendre defini sur */
/*             (-1,1), de degre NBROOT. */
/*     IORDRE: Ordre de contrainte aux extremites de la frontiere */
/*              -1 = pas de contraintes, */
/*               0 = contraintes de passage aux bornes (i.e. C0), */
/*               1 = C0 + contraintes de derivees 1eres (i.e. C1), */
/*               2 = C1 + contraintes de derivees 2ndes (i.e. C2). */
/*     IDERIV: Ordre de derivee de la frontiere. */
/*     NDGJAC: Degre du developpement en serie a utiliser pour le calcul 
*/
/*             dans la base de Jacobi. */
/*     NBCRMX: Nbre maxi de courbes a creer. */
/*     NCFLIM: Nombre maxi de coeff de la "courbe" polynomiale */
/*             d' approximation (doit etre superieur ou egal a */
/*             2*IORDRE+2 et inferieur ou egal a 50). */
/*     EPSAPR: Table des erreurs d' approximations souhaitees */
/*             sous-espace par sous-espace. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     NCOEFF: Nombre de coeff. significatifs des courbes calculees. */
/*     COURBE: Tableau des coeff. des courbes polynomiales calculees. */
/*             Doit etre dimensionne en (NCFLIM,NDIMEN,NBCRMX). */
/*             Ces courbes sont TOUJOURS parametrees dans (-1,1). */
/*     NBCRBE: Nbre de courbes calculees. */
/*     SOMTAB: Pour F definie sur (-1,1) (sinon on recale les */
/*             parametres), c'est la table des sommes F(u,vj) + F(u,-vj) 
*/
/*             si ISOFAV = 1 (et IDERIV=0, sinon on prend les derivees */
/*             en u d'ordre IDERIV) ou des sommes F(ui,v) + F(-ui,v) si */
/*             ISOFAV = 2 (et IDERIV=0, sinon on prend les derivees en */
/*             v d'ordre IDERIV). */
/*     DIFTAB: Pour F definie sur (-1,1) (sinon on recale les */
/*             parametres), c'est la table des sommes F(u,vj) - F(u,-vj) 
*/
/*             si ISOFAV = 1 (et IDERIV=0, sinon on prend les derivees */
/*             en u d'ordre IDERIV) ou des sommes F(ui,v) - F(-ui,v) si */
/*             ISOFAV = 2 (et IDERIV=0, sinon on prend les derivees en */
/*             v d'ordre IDERIV). */
/*     CONTR1: Contient les coordonnees de l'extremite gauche de l'iso */
/*             et de ses derivees jusqu'a l'ordre IORDRE */
/*     CONTR2: Contient les coordonnees de l'extremite droite de l'iso */
/*             et de ses derivees jusqu'a l'ordre IORDRE */
/*     TABDEC: Table des NBCRBE+1 parametres de decoupe de UVFONC(1:2,1) 
*/
/*             si ISOFAV=2, ou de UVFONC(1:2,2) si ISOFAV=1. */
/*     ERRMAX: Table des erreurs (sous-espace par sous espace) */
/*             MAXIMALES commises dans l' approximation de FONCNP par */
/*             les NBCRBE courbes. */
/*     ERRMOY: Table des erreurs (sous-espace par sous espace) */
/*             MOYENNES commises dans l' approximation de FONCNP par */
/*             les NBCRBE courbes. */
/*     IERCOD: Code d' erreur : */
/*             -1 = ERRMAX > EPSAPR pour au moins un des sous-espace. */
/*                  (les courbes resultat de degre mathematique NCFLIM-1 
*/
/*                  au plus , sont quand meme calculees). */
/*              0 = Tout est ok. */
/*              1 = Pb d' incoherence des entrees. */
/*             10 = Pb de calcul de l' interpolation des contraintes. */
/*             13 = Pb dans l' allocation dynamique. */
/*             33 = Pb dans la recuperation des donnees du block data */
/*                  des coeff. d' integration par la methode de GAUSS. */
/*             >100 Pb dans l' evaluation de FONCNP, le code d' erreur */
/*                  renvoye est egal au code d' erreur de FONCNP + 100. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/* --> La partie approximation est faite dans l' espace de dimension */
/*    NDIMEN (la somme des dimensions des sous-espaces). Par exemple : */
/*        Si NBSESP=2 et NDIMSE(1)=3, NDIMSE(2)=2, on a un lissage avec */
/*        NDIMEN=5. Le resultat (dans COURBE(NDIMEN,NCOEFF,i) ), sera */
/*        compose du resultat du lissage de la fonction 3D dans */
/*        COURBE(1:3,1:NCOEFF,i) et du lissage de la fonction 2D dans */
/*        COURBE(4:5,1:NCOEFF,i). */

/* -->  La routine FONCNP doit etre declaree EXTERNAL dans le programme */
/*     appelant MMA2FNC. */

/* -->  La fonction FONCNP, declaree ici en externe, doit etre declaree */
/*     IMPERATIVEMENT sous la forme : */
/*          SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,IIUOUV,TCONST,NBPTAB */
/*                           ,TTABLE,IDERIU,IDERIV,IERCOD) */
/*     ou les arguments d' entree sont : */
/*      - NDIMEN est un entier defini comme la somme des dimensions des */
/*               sous-espaces (i.e. dimension totale du probleme). */
/*      - UINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                en U ou est definie la fonction a approximer */
/*                (donc ici egal a UIFONC). */
/*      - VINTFN(2) est un tableau de 2 reels contenant l' intervalle */
/*                en V ou est definie la fonction a approximer */
/*                (donc ici egal a VIFONC). */
/*      - IIUOUV, indique que les points a calculer sont a U constant */
/*                (IIUOUV=1) ou a V constant (IIUOUV=2). */
/*      - TCONST, un reel, le parametre fixe de discretisation qui prend 
*/
/*                ses valeurs dans (UINTFN(1),UINTFN(2)) si IIUOUV=1, */
/*                ou dans (VINTFN(1),VINTFN(2)) si IIUOUV=2. */
/*      - NBPTAB, Le nbre de point de discretisation suivant la variable 
*/
/*                libre: V si IIUOUV=1 ou U si IIUOUV = 2. */
/*      - TTABLE, La table des NBPTAB parametres de discretisation. */
/*      - IDERIU, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRU (derivee partielle en U de la fonction a */
/*                l' ordre IORDRU si IORDRU > 0). */
/*      - IDERIV, un entier, prend ses valeurs entre 0 (positionnement) */
/*                et IORDRV (derivee partielle en V de la fonction a */
/*                l' ordre IORDRV si IORDRV > 0). */
/*     et les arguments de sortie sont : */
/*      - FPNTAB(NDIMEN,NBPTAB) contient, en sortie, le tableau des */
/*                NBPTAB points calcules de dimension NDIMEN. */
/*      - IERCOD est, en sortie, le code d' erreur de FONCNP. Ce code */
/*               (entier) doit etre strictement positif s' il y a eu */
/*               un probleme. */
/*     Les arguments d' entree NE DOIVENT PAS etre modifies sous FONCNP. 
*/

/* -->  Si IERCOD=-1, la precision demandee n' est pas atteinte (ERRMAX */
/*     est superieur a EPSAPR sur au moins l' un des sous espaces), mais 
*/
/*     on donne le meilleur resultat possible pour NCFLIM et EPSAPR */
/*     choisis par l'utilisateur. Dans ce cas (ainsi que pour */
/*     IERCOD=0), on a une solution. */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     04-02-1992: RBD; Correction passage SOMTAB et DIFTAB en argument */
/*                      et appel a MMFMCA8. */
/*     26-09-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */

    /* Parameter adjustments */
    --epsapr;
    --ndimse;
    uvfonc -= 3;
    --rootlg;
    errmoy_dim1 = *nbsesp;
    errmoy_offset = errmoy_dim1 + 1;
    errmoy -= errmoy_offset;
    errmax_dim1 = *nbsesp;
    errmax_offset = errmax_dim1 + 1;
    errmax -= errmax_offset;
    contr2_dim1 = *ndimen;
    contr2_dim2 = *iordre + 2;
    contr2_offset = contr2_dim1 * (contr2_dim2 + 1) + 1;
    contr2 -= contr2_offset;
    contr1_dim1 = *ndimen;
    contr1_dim2 = *iordre + 2;
    contr1_offset = contr1_dim1 * (contr1_dim2 + 1) + 1;
    contr1 -= contr1_offset;
    diftab_dim1 = *nbroot / 2 + 1;
    diftab_dim2 = *ndimen;
    diftab_offset = diftab_dim1 * (diftab_dim2 + 1);
    diftab -= diftab_offset;
    somtab_dim1 = *nbroot / 2 + 1;
    somtab_dim2 = *ndimen;
    somtab_offset = somtab_dim1 * (somtab_dim2 + 1);
    somtab -= somtab_offset;
    --ncoeff;
    courbe_dim1 = *ncflim;
    courbe_dim2 = *ndimen;
    courbe_offset = courbe_dim1 * (courbe_dim2 + 1) + 1;
    courbe -= courbe_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 1) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2FNC", 7L);
    }
    *iercod = 0;
    iofwr = 0;

/* ---------------- Mise a zero des coefficients de COURBE -------------- 
*/

    ilong = *ndimen * *ncflim * *nbcrmx;
    AdvApp2Var_SysBase::mvriraz_(&ilong, (char *)&courbe[courbe_offset]);

/* ********************************************************************** 
*/
/* -------------------------- Verification des entrees ------------------ 
*/
/* ********************************************************************** 
*/

    AdvApp2Var_MathBase::mmveps3_(&eps3);
    if ((d__1 = uvfonc[4] - uvfonc[3], abs(d__1)) < eps3) {
	goto L9100;
    }
    if ((d__1 = uvfonc[6] - uvfonc[5], abs(d__1)) < eps3) {
	goto L9100;
    }

    uv11[0] = -1.;
    uv11[1] = 1.;
    uv11[2] = -1.;
    uv11[3] = 1.;

/* ********************************************************************** 
*/
/* ------------- Preparation des parametres de discretisation ----------- 
*/
/* ********************************************************************** 
*/

/* -- Allocation d'une table de parametres et de pts de discretisation -- 
*/
/* --> Pour les parametres de discretisation. */
    isz1 = *nbroot + 2;
/* --> Pour les pts de discretisation dans MMA1FDI et MMA1CDI et la courbe
 */
/*    auxiliaire pour MMAPCMP */
    ibid1 = *ndimen * (*nbroot + 2);
    ibid2 = ((*iordre + 1) << 1) * *nbroot;
    isz2 = max(ibid1,ibid2);
    ibid1 = (((*ncflim - 1) / 2 + 1) << 1) * *ndimen;
    isz2 = max(ibid1,isz2);
/* --> Pour recuperer les polynomes d'hermite. */
    isz3 = ((*iordre + 1) << 2) * (*iordre + 1);
/* --> Pour les coeff. d'integration de Gauss. */
    isz4 = (*nbroot / 2 + 1) * (*ndgjac + 1 - ((*iordre + 1) << 1));
/* --> Pour les coeff de la courbe dans la base de Jacobi */
    isz5 = (*ndgjac + 1) * *ndimen;

    ndwrk = isz1 + isz2 + isz3 + isz4 + isz5;
    AdvApp2Var_SysBase::mcrrqst_(&c__8, &ndwrk, wrkar, &iofwr, &ier);
    if (ier > 0) {
	goto L9013;    }
/* --> Pour les parametres de discretisation (NBROOT+2 extremites). */
    ipt1 = iofwr;
/* --> Pour les pts de discretisation FPNTAB(NDIMEN,NBROOT+2), */
/*    FPNTAB(NBROOT,2*(IORDRE+1)) et pour WRKAR de MMAPCMP. */
    ipt2 = ipt1 + isz1;
/* --> Pour les polynomes d'Hermite */
    ipt3 = ipt2 + isz2;
/* --> Pour les coeff d'integration de Gauss. */
    ipt4 = ipt3 + isz3;
/* --> Pour la courbe dans Jacobi. */
    ipt5 = ipt4 + isz4;

/* ------------------ Initialisation de la gestion des decoupes --------- 
*/

    if (*isofav == 1) {
	uvpav[0] = uvfonc[3];
	uvpav[1] = uvfonc[4];
	tabdec[0] = uvfonc[5];
	tabdec[1] = uvfonc[6];
    } else if (*isofav == 2) {
	tabdec[0] = uvfonc[3];
	tabdec[1] = uvfonc[4];
	uvpav[2] = uvfonc[5];
	uvpav[3] = uvfonc[6];
    } else {
	goto L9100;
    }

    nupil = 1;
    *nbcrbe = 0;

/* ********************************************************************** 
*/
/*                       APPROXIMATION AVEC DECOUPES */
/* ********************************************************************** 
*/

L1000:
/* --> Lorsque l' on a atteint le haut de la pile, c' est fini ! */
    if (nupil - *nbcrbe == 0) {
	goto L9900;
    }
    ncb1 = *nbcrbe + 1;
    if (*isofav == 1) {
	uvpav[2] = tabdec[*nbcrbe];
	uvpav[3] = tabdec[*nbcrbe + 1];
    } else if (*isofav == 2) {
	uvpav[0] = tabdec[*nbcrbe];
	uvpav[1] = tabdec[*nbcrbe + 1];
    } else {
	goto L9100;
    }

/* -------------------- Normalisation des parametres -------------------- 
*/

    mma1nop_(nbroot, &rootlg[1], uvpav, isofav, &wrkar[ipt1], &ier);
    if (ier > 0) {
	goto L9100;
    }

/* -------------------- Discretisation de FONCNP ------------------------ 
*/

    mma1fdi_(ndimen, uvpav, foncnp, isofav, tconst, nbroot, &wrkar[ipt1], 
	    iordre, ideriv, &wrkar[ipt2], &somtab[(ncb1 * somtab_dim2 + 1) * 
	    somtab_dim1], &diftab[(ncb1 * diftab_dim2 + 1) * diftab_dim1], &
	    contr1[(ncb1 * contr1_dim2 + 1) * contr1_dim1 + 1], &contr2[(ncb1 
	    * contr2_dim2 + 1) * contr2_dim1 + 1], iercod);
    if (*iercod > 0) {
	goto L9900;
    }

/* -----------On retranche la discretisation des contraintes ------------ 
*/

    if (*iordre >= 0) {
	mma1cdi_(ndimen, nbroot, &rootlg[1], iordre, &contr1[(ncb1 * 
		contr1_dim2 + 1) * contr1_dim1 + 1], &contr2[(ncb1 * 
		contr2_dim2 + 1) * contr2_dim1 + 1], &somtab[(ncb1 * 
		somtab_dim2 + 1) * somtab_dim1], &diftab[(ncb1 * diftab_dim2 
		+ 1) * diftab_dim1], &wrkar[ipt2], &wrkar[ipt3], &ier);
	if (ier > 0) {
	    goto L9100;
	}
    }

/* ********************************************************************** 
*/
/* -------------------- Calcul de la courbe d'approximation ------------- 
*/
/* ********************************************************************** 
*/

    mma1jak_(ndimen, nbroot, iordre, ndgjac, &somtab[(ncb1 * somtab_dim2 + 1) 
	    * somtab_dim1], &diftab[(ncb1 * diftab_dim2 + 1) * diftab_dim1], &
	    wrkar[ipt4], &wrkar[ipt5], &ier);
    if (ier > 0) {
	goto L9100;
    }

/* ********************************************************************** 
*/
/* ---------------- Ajout du polynome d'interpolation ------------------- 
*/
/* ********************************************************************** 
*/

    if (*iordre >= 0) {
	mma1cnt_(ndimen, iordre, &contr1[(ncb1 * contr1_dim2 + 1) * 
		contr1_dim1 + 1], &contr2[(ncb1 * contr2_dim2 + 1) * 
		contr2_dim1 + 1], &wrkar[ipt3], ndgjac, &wrkar[ipt5]);
    }

/* ********************************************************************** 
*/
/* --------------- Calcul de l'erreur Max et Moyenne -------------------- 
*/
/* ********************************************************************** 
*/

    mma1fer_(ndimen, nbsesp, &ndimse[1], iordre, ndgjac, &wrkar[ipt5], ncflim,
	     &epsapr[1], &wrkar[ipt2], &errmax[ncb1 * errmax_dim1 + 1], &
	    errmoy[ncb1 * errmoy_dim1 + 1], &ncoeff[ncb1], &ier);
    if (ier > 0) {
	goto L9100;
    }

    if (ier == 0 || (ier == -1 && nupil == *nbcrmx)) {

/* ******************************************************************
**** */
/* ----------------------- Compression du resultat ------------------
---- */
/* ******************************************************************
**** */

	if (ier == -1) {
	    *iercod = -1;
	}
	ncfja = *ndgjac + 1;
/* -> Compression du resultat dans WRKAR(IPT2) */
	/*pkv f*/
	/*
	AdvApp2Var_MathBase::mmapcmp_(ndimen, 
	&ncfja, &ncoeff[ncb1], &wrkar[ipt5], &wrkar[ipt2]);
	*/
	AdvApp2Var_MathBase::mmapcmp_((integer*)ndimen, 
				      &ncfja, 
				      &ncoeff[ncb1], 
				      &wrkar[ipt5], 
				      &wrkar[ipt2]);
	/*pkv t*/
	ilong = *ndimen * *ncflim;
	AdvApp2Var_SysBase::mvriraz_(&ilong, (char*)&wrkar[ipt5]);
/* -> Passage a la base canonique (-1,1) (resultat dans WRKAR(IPT5)). 
*/
	ndgre = ncoeff[ncb1] - 1;
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    iptt = ipt2 + ((nd - 1) << 1) * (ndgre / 2 + 1);
	    jptt = ipt5 + (nd - 1) * ncoeff[ncb1];
	    AdvApp2Var_MathBase::mmjacan_(iordre, &ndgre, &wrkar[iptt], &wrkar[jptt]);
/* L400: */
	}

/* -> On stocke la courbe calculee */
	ibid1 = 1;
	AdvApp2Var_MathBase::mmfmca8_(&ncoeff[ncb1], ndimen, &ibid1, ncflim, ndimen, &ibid1, &
		wrkar[ipt5], &courbe[(ncb1 * courbe_dim2 + 1) * courbe_dim1 + 
		1]);

/* -> Les contraintes ayant ete normalisee sur (-1,1), on recalcule */
/*   les contraintes vraies. */
	i__1 = *iordre;
	for (ii = 0; ii <= i__1; ++ii) {
	    mma1noc_(uv11, ndimen, &ii, &contr1[(ii + 1 + ncb1 * contr1_dim2) 
		    * contr1_dim1 + 1], uvpav, isofav, ideriv, &contr1[(ii + 
		    1 + ncb1 * contr1_dim2) * contr1_dim1 + 1]);
	    mma1noc_(uv11, ndimen, &ii, &contr2[(ii + 1 + ncb1 * contr2_dim2) 
		    * contr2_dim1 + 1], uvpav, isofav, ideriv, &contr2[(ii + 
		    1 + ncb1 * contr2_dim2) * contr2_dim1 + 1]);
/* L200: */
	}
	ii = 0;
	ibid1 = (*nbroot / 2 + 1) * *ndimen;
	mma1noc_(uv11, &ibid1, &ii, &somtab[(ncb1 * somtab_dim2 + 1) * 
		somtab_dim1], uvpav, isofav, ideriv, &somtab[(ncb1 * 
		somtab_dim2 + 1) * somtab_dim1]);
	mma1noc_(uv11, &ibid1, &ii, &diftab[(ncb1 * diftab_dim2 + 1) * 
		diftab_dim1], uvpav, isofav, ideriv, &diftab[(ncb1 * 
		diftab_dim2 + 1) * diftab_dim1]);
	ii = 0;
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    mma1noc_(uv11, &ncoeff[ncb1], &ii, &courbe[(nd + ncb1 * 
		    courbe_dim2) * courbe_dim1 + 1], uvpav, isofav, ideriv, &
		    courbe[(nd + ncb1 * courbe_dim2) * courbe_dim1 + 1]);
/* L210: */
	}

/* -> Mise ajour du nbre de courbes deja crees */
	++(*nbcrbe);

/* -> ...sinon on essai de decouper l' intervalle courant en 2... */
    } else {
	tmil = (tabdec[*nbcrbe + 1] + tabdec[*nbcrbe]) / 2.;
	ideb = *nbcrbe + 1;
	ideb1 = ideb + 1;
	ilong = (nupil - *nbcrbe) << 3;
	AdvApp2Var_SysBase::mcrfill_(&ilong, (char *)&tabdec[ideb],(char *)&tabdec[ideb1]);
	tabdec[ideb] = tmil;
	++nupil;
    }

/* ---------- On fait l' approximation de la suite de la pile ----------- 
*/

    goto L1000;

/* --------------------- Recuperation du code d' erreur ----------------- 
*/
/* --> Pb alloc. dynamique. */
L9013:
    *iercod = 13;
    goto L9900;
/* --> Entrees incoherentes. */
L9100:
    *iercod = 1;
    goto L9900;

/* -------------------------- Desallocation dynamique ------------------- 
*/

L9900:
    if (iofwr != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, &ndwrk, wrkar, &iofwr, &ier);
    }
    if (ier > 0) {
	*iercod = 13;
    }
    goto L9999;

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMA2FNC", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2FNC", 7L);
    }
    return 0;
} /* mma2fnc_ */

//=======================================================================
//function : mma2fx6_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2fx6_(integer *ncfmxu,
				     integer *ncfmxv, 
				     integer *ndimen, 
				     integer *nbsesp, 
				     integer *ndimse, 
				     integer *nbupat, 
				     integer *nbvpat, 
				     integer *iordru, 
				     integer *iordrv, 
				     doublereal *epsapr, 
				     doublereal *epsfro, 
				     doublereal *patcan, 
				     doublereal *errmax, 
				     integer *ncoefu, 
				     integer *ncoefv)

{
  /* System generated locals */
  integer epsfro_dim1, epsfro_offset, patcan_dim1, patcan_dim2, patcan_dim3,
  patcan_dim4, patcan_offset, errmax_dim1, errmax_dim2, 
  errmax_offset, ncoefu_dim1, ncoefu_offset, ncoefv_dim1, 
  ncoefv_offset, i__1, i__2, i__3, i__4, i__5;
  doublereal d__1, d__2;
  
  /* Local variables */
  static integer idim, ncfu, ncfv, id, ii, nd, jj, ku, kv, ns, ibb;
  static doublereal bid;
  static doublereal tol;
  
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Reduction de degre lorsque les carreaux sont les carreaux de */
/*     contraintes. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::CARREAU&,REDUCTION,&CARREAU */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/* NCFMXU: Nbre maximal de coeff en u de la solution P(u,v) (tableau */
/*         PATCAN). Cet argument sert uniquement a declarer la taille */
/*         de ce tableau. */
/* NCFMXV: Nbre maximal de coeff en v de la solution P(u,v) (tableau */
/*         PATCAN). Cet argument sert uniquement a declarer la taille */
/*         de ce tableau. */
/* NDIMEN: Dimension totale de l' espace ou la fonction a approcher */
/*         prend ses valeurs.(somme des dimensions des sous-espaces) */
/* NBSESP: Nombre de sous-espaces independants ou l'on mesure les */
/*         erreurs. */
/* NDIMSE: Table des dimensions des NBSESP sous-espaces. */
/* NBUPAT: Nbre de carreau solution en u. */
/* NBVPAT: Nbre de carreau solution en v. */
/* IORDRU: Ordre de contrainte impose aux extremites de l'iso-V */
/*         = 0, on calcule les extremites de l'iso-V */
/*         = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*              de l'iso-V */
/*         = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*              de l'iso-V */
/* IORDRV: Ordre de contrainte impose aux extremites de l'iso-U */
/*         = 0, on calcule les extremites de l'iso-U. */
/*         = 1, on calcule, en plus, la derivee 1ere dans le sens */
/*              de l'iso-U */
/*         = 2, on calcule, en plus, la derivee 2nde dans le sens */
/*              de l'iso-U */
/* EPSAPR: Table des precisions imposees, sous-espace par sous-espace. */
/* EPSFRO: Table des precisions imposees, sous-espace par sous-espace */
/*         sur les frontieres des carreaux. */
/* PATCAN: Tableau des coeff. dans la base canonique des carreaux P(u,v) 
*/
/*         calcules, pour (u,v) dans (-1,1). */
/* ERRMAX: Table des erreurs (sous-espace par sous espace) */
/*         MAXIMALES commises dans l' approximation de F(u,v) par */
/*         les P(u,v). */
/* NCOEFU: Table des Nbres de coeff. significatifs en u des carreaux */
/*         calcules. */
/* NCOEFV: Table des Nbres de coeff. significatifs en v des carreaux */
/*         calcules. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/* NCOEFU: Table des Nbres de coeff. significatifs en u des carreaux */
/*         calcules. */
/* NCOEFV: Table des Nbres de coeff. significatifs en v des carreaux */
/*         calcules. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     15-07-1996: JCT/RBD; Initialisation de TOL quand on reduit */
/*                          le degre uniquement en V */
/*     14-02-1992: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    epsfro_dim1 = *nbsesp;
    epsfro_offset = epsfro_dim1 * 5 + 1;
    epsfro -= epsfro_offset;
    --epsapr;
    --ndimse;
    ncoefv_dim1 = *nbupat;
    ncoefv_offset = ncoefv_dim1 + 1;
    ncoefv -= ncoefv_offset;
    ncoefu_dim1 = *nbupat;
    ncoefu_offset = ncoefu_dim1 + 1;
    ncoefu -= ncoefu_offset;
    errmax_dim1 = *nbsesp;
    errmax_dim2 = *nbupat;
    errmax_offset = errmax_dim1 * (errmax_dim2 + 1) + 1;
    errmax -= errmax_offset;
    patcan_dim1 = *ncfmxu;
    patcan_dim2 = *ncfmxv;
    patcan_dim3 = *ndimen;
    patcan_dim4 = *nbupat;
    patcan_offset = patcan_dim1 * (patcan_dim2 * (patcan_dim3 * (patcan_dim4 
	    + 1) + 1) + 1) + 1;
    patcan -= patcan_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2FX6", 7L);
    }


    i__1 = *nbvpat;
    for (jj = 1; jj <= i__1; ++jj) {
	i__2 = *nbupat;
	for (ii = 1; ii <= i__2; ++ii) {
	    ncfu = ncoefu[ii + jj * ncoefu_dim1];
	    ncfv = ncoefv[ii + jj * ncoefv_dim1];

/* **************************************************************
******** */
/* -------------------- Reduction du degre en U -----------------
-------- */
/* **************************************************************
******** */

L200:
	    if (ncfu <= (*iordru + 1) << 1 && ncfu > 2) {

		idim = 0;
		i__3 = *nbsesp;
		for (ns = 1; ns <= i__3; ++ns) {
		    tol = epsapr[ns];
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 9];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 10];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 11];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 12];
		    tol = min(d__1,d__2);
		    if (ii == 1 || ii == *nbupat || jj == 1 || jj == *nbvpat) 
			    {
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 5];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 6];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 7];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + (epsfro_dim1 << 3)];
			tol = min(d__1,d__2);
		    }
		    bid = 0.;

		    i__4 = ndimse[ns];
		    for (nd = 1; nd <= i__4; ++nd) {
			id = idim + nd;
			i__5 = ncfv;
			for (kv = 1; kv <= i__5; ++kv) {
			    bid += (d__1 = patcan[ncfu + (kv + (id + (ii + jj 
				    * patcan_dim4) * patcan_dim3) * 
				    patcan_dim2) * patcan_dim1], abs(d__1));
/* L230: */
			}
/* L220: */
		    }

		    if (bid > tol * 1e-6 || bid > errmax[ns + (ii + jj * 
			    errmax_dim2) * errmax_dim1]) {
			goto L300;
		    }
		    idim += ndimse[ns];
/* L210: */
		}

		--ncfu;
		goto L200;
	    }

/* **************************************************************
******** */
/* -------------------- Reduction du degre en V -----------------
-------- */
/* **************************************************************
******** */

L300:
	    if (ncfv <= (*iordrv + 1) << 1 && ncfv > 2) {

		idim = 0;
		i__3 = *nbsesp;
		for (ns = 1; ns <= i__3; ++ns) {
		    tol = epsapr[ns];
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 9];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 10];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 11];
		    tol = min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 12];
		    tol = min(d__1,d__2);
		    if (ii == 1 || ii == *nbupat || jj == 1 || jj == *nbvpat) 
			    {
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 5];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 6];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 7];
			tol = min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + (epsfro_dim1 << 3)];
			tol = min(d__1,d__2);
		    }
		    bid = 0.;

		    i__4 = ndimse[ns];
		    for (nd = 1; nd <= i__4; ++nd) {
			id = idim + nd;
			i__5 = ncfu;
			for (ku = 1; ku <= i__5; ++ku) {
			    bid += (d__1 = patcan[ku + (ncfv + (id + (ii + jj 
				    * patcan_dim4) * patcan_dim3) * 
				    patcan_dim2) * patcan_dim1], abs(d__1));
/* L330: */
			}
/* L320: */
		    }

		    if (bid > tol * 1e-6 || bid > errmax[ns + (ii + jj * 
			    errmax_dim2) * errmax_dim1]) {
			goto L400;
		    }
		    idim += ndimse[ns];
/* L310: */
		}

		--ncfv;
		goto L300;
	    }

/* --- On recupere les nbres de coeff. et on passe au carreau suiv
ant --- */

L400:
	    ncoefu[ii + jj * ncoefu_dim1] = max(ncfu,2);
	    ncoefv[ii + jj * ncoefv_dim1] = max(ncfv,2);
/* L110: */
	}
/* L100: */
    }

/* ------------------------------ The End ------------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2FX6", 7L);
    }

 return 0 ;
} /* mma2fx6_ */

//=======================================================================
//function : mma2jmx_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2jmx_(integer *ndgjac, 
				     integer *iordre, 
				     doublereal *xjacmx)
{
    /* Initialized data */

    static doublereal xmax2[57] = { .9682458365518542212948163499456,
	    .986013297183269340427888048593603,
	    1.07810420343739860362585159028115,
	    1.17325804490920057010925920756025,
	    1.26476561266905634732910520370741,
	    1.35169950227289626684434056681946,
	    1.43424378958284137759129885012494,
	    1.51281316274895465689402798226634,
	    1.5878364329591908800533936587012,
	    1.65970112228228167018443636171226,
	    1.72874345388622461848433443013543,
	    1.7952515611463877544077632304216,
	    1.85947199025328260370244491818047,
	    1.92161634324190018916351663207101,
	    1.98186713586472025397859895825157,
	    2.04038269834980146276967984252188,
	    2.09730119173852573441223706382076,
	    2.15274387655763462685970799663412,
	    2.20681777186342079455059961912859,
	    2.25961782459354604684402726624239,
	    2.31122868752403808176824020121524,
	    2.36172618435386566570998793688131,
	    2.41117852396114589446497298177554,
	    2.45964731268663657873849811095449,
	    2.50718840313973523778244737914028,
	    2.55385260994795361951813645784034,
	    2.59968631659221867834697883938297,
	    2.64473199258285846332860663371298,
	    2.68902863641518586789566216064557,
	    2.73261215675199397407027673053895,
	    2.77551570192374483822124304745691,
	    2.8177699459714315371037628127545,
	    2.85940333797200948896046563785957,
	    2.90044232019793636101516293333324,
	    2.94091151970640874812265419871976,
	    2.98083391718088702956696303389061,
	    3.02023099621926980436221568258656,
	    3.05912287574998661724731962377847,
	    3.09752842783622025614245706196447,
	    3.13546538278134559341444834866301,
	    3.17295042316122606504398054547289,
	    3.2099992681699613513775259670214,
	    3.24662674946606137764916854570219,
	    3.28284687953866689817670991319787,
	    3.31867291347259485044591136879087,
	    3.35411740487202127264475726990106,
	    3.38919225660177218727305224515862,
	    3.42390876691942143189170489271753,
	    3.45827767149820230182596660024454,
	    3.49230918177808483937957161007792,
	    3.5260130200285724149540352829756,
	    3.55939845146044235497103883695448,
	    3.59247431368364585025958062194665,
	    3.62524904377393592090180712976368,
	    3.65773070318071087226169680450936,
	    3.68992700068237648299565823810245,
	    3.72184531357268220291630708234186 };
    static doublereal xmax4[55] = { 1.1092649593311780079813740546678,
	    1.05299572648705464724876659688996,
	    1.0949715351434178709281698645813,
	    1.15078388379719068145021100764647,
	    1.2094863084718701596278219811869,
	    1.26806623151369531323304177532868,
	    1.32549784426476978866302826176202,
	    1.38142537365039019558329304432581,
	    1.43575531950773585146867625840552,
	    1.48850442653629641402403231015299,
	    1.53973611681876234549146350844736,
	    1.58953193485272191557448229046492,
	    1.63797820416306624705258190017418,
	    1.68515974143594899185621942934906,
	    1.73115699602477936547107755854868,
	    1.77604489805513552087086912113251,
	    1.81989256661534438347398400420601,
	    1.86276344480103110090865609776681,
	    1.90471563564740808542244678597105,
	    1.94580231994751044968731427898046,
	    1.98607219357764450634552790950067,
	    2.02556989246317857340333585562678,
	    2.06433638992049685189059517340452,
	    2.10240936014742726236706004607473,
	    2.13982350649113222745523925190532,
	    2.17661085564771614285379929798896,
	    2.21280102016879766322589373557048,
	    2.2484214321456956597803794333791,
	    2.28349755104077956674135810027654,
	    2.31805304852593774867640120860446,
	    2.35210997297725685169643559615022,
	    2.38568889602346315560143377261814,
	    2.41880904328694215730192284109322,
	    2.45148841120796359750021227795539,
	    2.48374387161372199992570528025315,
	    2.5155912654873773953959098501893,
	    2.54704548720896557684101746505398,
	    2.57812056037881628390134077704127,
	    2.60882970619319538196517982945269,
	    2.63918540521920497868347679257107,
	    2.66919945330942891495458446613851,
	    2.69888301230439621709803756505788,
	    2.72824665609081486737132853370048,
	    2.75730041251405791603760003778285,
	    2.78605380158311346185098508516203,
	    2.81451587035387403267676338931454,
	    2.84269522483114290814009184272637,
	    2.87060005919012917988363332454033,
	    2.89823818258367657739520912946934,
	    2.92561704377132528239806135133273,
	    2.95274375377994262301217318010209,
	    2.97962510678256471794289060402033,
	    3.00626759936182712291041810228171,
	    3.03267744830655121818899164295959,
	    3.05886060707437081434964933864149 };
    static doublereal xmax6[53] = { 1.21091229812484768570102219548814,
	    1.11626917091567929907256116528817,
	    1.1327140810290884106278510474203,
	    1.1679452722668028753522098022171,
	    1.20910611986279066645602153641334,
	    1.25228283758701572089625983127043,
	    1.29591971597287895911380446311508,
	    1.3393138157481884258308028584917,
	    1.3821288728999671920677617491385,
	    1.42420414683357356104823573391816,
	    1.46546895108549501306970087318319,
	    1.50590085198398789708599726315869,
	    1.54550385142820987194251585145013,
	    1.58429644271680300005206185490937,
	    1.62230484071440103826322971668038,
	    1.65955905239130512405565733793667,
	    1.69609056468292429853775667485212,
	    1.73193098017228915881592458573809,
	    1.7671112206990325429863426635397,
	    1.80166107681586964987277458875667,
	    1.83560897003644959204940535551721,
	    1.86898184653271388435058371983316,
	    1.90180515174518670797686768515502,
	    1.93410285411785808749237200054739,
	    1.96589749778987993293150856865539,
	    1.99721027139062501070081653790635,
	    2.02806108474738744005306947877164,
	    2.05846864831762572089033752595401,
	    2.08845055210580131460156962214748,
	    2.11802334209486194329576724042253,
	    2.14720259305166593214642386780469,
	    2.17600297710595096918495785742803,
	    2.20443832785205516555772788192013,
	    2.2325216999457379530416998244706,
	    2.2602654243075083168599953074345,
	    2.28768115912702794202525264301585,
	    2.3147799369092684021274946755348,
	    2.34157220782483457076721300512406,
	    2.36806787963276257263034969490066,
	    2.39427635443992520016789041085844,
	    2.42020656255081863955040620243062,
	    2.44586699364757383088888037359254,
	    2.47126572552427660024678584642791,
	    2.49641045058324178349347438430311,
	    2.52130850028451113942299097584818,
	    2.54596686772399937214920135190177,
	    2.5703922285006754089328998222275,
	    2.59459096001908861492582631591134,
	    2.61856915936049852435394597597773,
	    2.64233265984385295286445444361827,
	    2.66588704638685848486056711408168,
	    2.68923766976735295746679957665724,
	    2.71238965987606292679677228666411 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static logical ldbg;
    static integer numax, ii;
    static doublereal bid;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Calcule les max des polynomes de Jacobi multiplies par le poids */
/*  sur (-1,1) pour ordre 0,4,6 ou Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDGJAC: Nbre de coeff. de l'approximation de Jacobi. */
/*     IORDRE: Ordre de continuite (de -1 a 2) */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     XJACMX: Table des maximums des polynomes de Jacobi. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     20-08-1991: RBD; Creation. */
/* > */
/* ***********************************************************************
 */
/*   Le nom de la routine */
/* ----------------------------- Initialisations ------------------------ 
*/

    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2JMX", 7L);
    }

    numax = *ndgjac - ((*iordre + 1) << 1);
    if (*iordre == -1) {
	i__1 = numax;
	for (ii = 0; ii <= i__1; ++ii) {
	    bid = (ii * 2. + 1.) / 2.;
	    xjacmx[ii] = sqrt(bid);
/* L100: */
	}
    } else if (*iordre == 0) {
	i__1 = numax;
	for (ii = 0; ii <= i__1; ++ii) {
	    xjacmx[ii] = xmax2[ii];
/* L200: */
	}
    } else if (*iordre == 1) {
	i__1 = numax;
	for (ii = 0; ii <= i__1; ++ii) {
	    xjacmx[ii] = xmax4[ii];
/* L400: */
	}
    } else if (*iordre == 2) {
	i__1 = numax;
	for (ii = 0; ii <= i__1; ++ii) {
	    xjacmx[ii] = xmax6[ii];
/* L600: */
	}
    }

/* ------------------------- The end ------------------------------------ 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2JMX", 7L);
    }
    return 0;
} /* mma2jmx_ */

//=======================================================================
//function : mma2moy_
//purpose  : 
//=======================================================================
int mma2moy_(integer *ndgumx, 
	     integer *ndgvmx, 
	     integer *ndimen, 
	     integer *mindgu, 
	     integer *maxdgu, 
	     integer *mindgv, 
	     integer *maxdgv, 
	     integer *iordru, 
	     integer *iordrv, 
	     doublereal *patjac, 
	     doublereal *errmoy)
{
  /* System generated locals */
    integer patjac_dim1, patjac_dim2, patjac_offset, i__1, i__2, i__3;
   
    /* Local variables */
    static logical ldbg;
    static integer minu, minv, idebu, idebv, ii, nd, jj;
    static doublereal bid0, bid1;
    
    
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Calcule l'erreur moyenne d'approximation faite lorsque l'on ne */
/*  garde que les coefficients de PATJAC de degre compris entre */
/*  2*(IORDRU+1) et MINDGU en U et 2*(IORDRV+1) et MINDGV en V. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,APPROXIMATION,ERREUR MOYENNE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDGUMX: Dimension en U du tableau PATJAC. */
/*     NDGVMX: Dimension en V du tableau PATJAC. */
/*     NDIMEN: Dimension de l'espace. */
/*     MINDGU: Borne inf de l'indice en U des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MAXDGU: Borne sup de l'indice en U des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MINDGV: Borne inf de l'indice en V des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     MAXDGV: Borne sup de l'indice en V des coeff. de PATJAC a prendre 
*/
/*             en compte. */
/*     IORDRU: Ordre de continuite en U assure par le carreau PATJAC */
/*             (de -1 a 2) */
/*     IORDRV: Ordre de continuite en V assure par le carreau PATJAC */
/*             (de -1 a 2) */
/*     PATJAC: Table des coeff. du carreau d'approximation avec */
/*             contraintes d'ordre IORDRU en U et IORDRV en V. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     ERRMOY: L'erreur moyenne commise en ne gardant que les coeff de */
/*             PATJAC 2*(IORDRU+1) a MINDGU en U et 2*(IORDRV+1) a */
/*             MINDGV en V. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Dans le tableau PATJAC sont stockes les coeff. Cij du carreau */
/*     d'approximation de F(U,V). Les indices i et j indique le degre en 
*/
/*     U et en V des polynomes de base. Ces polynomes de base sont de la 
*/
/*     forme: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), ou */

/*     le polynome J(i-2*(IORDU+1)(U) est le polynome de Jacobi d'ordre */
/*     IORDRU+1 (idem en V en remplacant U par V dans l'expression ci */
/*     dessus). */

/*     La contribution a l'erreur moyenne du terme Cij lorsque */
/*     celui-ci est enleve de PATJAC est Cij*Cij. */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     13-06-1991: RBD; Creation. */
/* > */
/* ***********************************************************************
 */
/*   Le nom de la routine */


/* ----------------------------- Initialisations ------------------------ 
*/

    /* Parameter adjustments */
    patjac_dim1 = *ndgumx + 1;
    patjac_dim2 = *ndgvmx + 1;
    patjac_offset = patjac_dim1 * patjac_dim2;
    patjac -= patjac_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 3;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2MOY", 7L);
    }

    idebu = (*iordru + 1) << 1;
    idebv = (*iordrv + 1) << 1;
    minu = max(idebu,*mindgu);
    minv = max(idebv,*mindgv);
    bid0 = 0.;
    *errmoy = 0.;

/* ------------------ Calcul du majorant de l'erreur moyenne ------------ 
*/
/* ----- lorsque sont enleves les coeff. d'indices MINDGU a MAXDGU ------ 
*/
/* ---------------- en U et d'indices MINDGV a MAXDGV en V -------------- 
*/

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *maxdgv;
	for (jj = minv; jj <= i__2; ++jj) {
	    i__3 = *maxdgu;
	    for (ii = idebu; ii <= i__3; ++ii) {
		bid1 = patjac[ii + (jj + nd * patjac_dim2) * patjac_dim1];
		bid0 += bid1 * bid1;
/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = minv - 1;
	for (jj = idebv; jj <= i__2; ++jj) {
	    i__3 = *maxdgu;
	    for (ii = minu; ii <= i__3; ++ii) {
		bid1 = patjac[ii + (jj + nd * patjac_dim2) * patjac_dim1];
		bid0 += bid1 * bid1;
/* L600: */
	    }
/* L500: */
	}
/* L400: */
    }

/* ----------------------- Calcul de l'erreur moyenne ------------------- 
*/

    bid0 /= 4;
    *errmoy = sqrt(bid0);

/* ------------------------- The end ------------------------------------ 
*/

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2MOY", 7L);
    }
    return 0;
} /* mma2moy_ */

//=======================================================================
//function : mma2roo_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mma2roo_(integer *nbpntu, 
				     integer *nbpntv, 
				     doublereal *urootl, 
				     doublereal *vrootl)
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer ii, ibb;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Recuperation des racines de Legendre pour les discretisations. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NBPNTU: Nbre de parametres INTERNES de discretisation EN U. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */
/*     NBPNTV: Nbre de parametres INTERNES de discretisation EN V. */
/*             C'est aussi le nbre de racine du polynome de Legendre ou */
/*             on discretise. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     UROOTL: Tableau des parametres de discretisation SUR (-1,1) EN U. 
*/
/*     VROOTL: Tableau des parametres de discretisation SUR (-1,1) EN V. 
*/

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     02-07-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


    /* Parameter adjustments */
    --urootl;
    --vrootl;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2ROO", 7L);
    }

/* ---------------- Recup des racines POSITIVES sur U ------------------ 
*/

    AdvApp2Var_MathBase::mmrtptt_(nbpntu, &urootl[(*nbpntu + 1) / 2 + 1]);
    i__1 = *nbpntu / 2;
    for (ii = 1; ii <= i__1; ++ii) {
	urootl[ii] = -urootl[*nbpntu - ii + 1];
/* L100: */
    }
    if (*nbpntu % 2 == 1) {
	urootl[*nbpntu / 2 + 1] = 0.;
    }

/* ---------------- Recup des racines POSITIVES sur V ------------------ 
*/

    AdvApp2Var_MathBase::mmrtptt_(nbpntv, &vrootl[(*nbpntv + 1) / 2 + 1]);
    i__1 = *nbpntv / 2;
    for (ii = 1; ii <= i__1; ++ii) {
	vrootl[ii] = -vrootl[*nbpntv - ii + 1];
/* L110: */
    }
    if (*nbpntv % 2 == 1) {
	vrootl[*nbpntv / 2 + 1] = 0.;
    }

/* ------------------------------ The End ------------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMA2ROO", 7L);
    }
    return 0;
} /* mma2roo_ */
//=======================================================================
//function : mmmapcoe_
//purpose  : 
//=======================================================================
int mmmapcoe_(integer *ndim, 
	      integer *ndgjac, 
	      integer *iordre, 
	      integer *nbpnts, 
	      doublereal *somtab, 
	      doublereal *diftab, 
	      doublereal *gsstab, 
	      doublereal *crvjac)

{
  /* System generated locals */
  integer somtab_dim1, somtab_offset, diftab_dim1, diftab_offset, 
  crvjac_dim1, crvjac_offset, gsstab_dim1, i__1, i__2, i__3;
  
  /* Local variables */
  static integer igss, ikdeb;
  static doublereal bidon;
  static integer nd, ik, ir, nbroot, ibb;
  


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Calcul des coefficients de la courbe d' approximation polynomiale 
*/
/*     de degre NDGJAC par la methode des moindres carres a partir de la 
*/
/*     discretisation de la fonction sur les racines du polynome de */
/*     Legendre de degre NBPNTS. */

/*     MOTS CLES : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIM   : Dimension de l' espace. */
/*        NDGJAC : Degre maxi du polynome d' approximation. La */
/*                 representation dans la base orthogonale part du degre 
*/
/*                 0 au degre NDGJAC-2*(JORDRE+1). La base polynomiale */
/*                 est la base de Jacobi d' ordre -1 (Legendre), 0, 1 */
/*                 et 2 */
/*        IORDRE : Ordre de la base de Jacobi (-1,0,1 ou 2). Correspond */
/*                 a pas de contraintes, contraintes C0,C1 ou C2. */
/*        NBPNTS : Degre du polynome de Legendre sur les racines duquel */
/*                 sont calcules les coefficients d' integration par la */
/*                 methode de Gauss. On doit avoir NBPNTS=30,40,50 ou 61 
*/
/*                 et NDGJAC < NBPNTS. */
/*        SOMTAB : Tableau de F(ti)+F(-ti) avec ti dans ROOTAB. */
/*        DIFTAB : Tableau de F(ti)-F(-ti) avec ti dans ROOTAB. */
/*        GSSTAB(i,k) : Table des coefficients d' integration par la */
/*                      methode de Gauss : i varie de 0 a NBPNTS et */
/*                      k varie de 0 a NDGJAC-2*(JORDRE+1). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CRVJAC : Courbe d' approximation de FONCNP avec eventuellement 
*/
/*                 prise en compte des contraintes aux extremites. */
/*                 Cette courbe est de degre NDGJAC. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     11-04-1989 : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

    /* Parameter adjustments */
    crvjac_dim1 = *ndgjac + 1;
    crvjac_offset = crvjac_dim1;
    crvjac -= crvjac_offset;
    gsstab_dim1 = *nbpnts / 2 + 1;
    diftab_dim1 = *nbpnts / 2 + 1;
    diftab_offset = diftab_dim1;
    diftab -= diftab_offset;
    somtab_dim1 = *nbpnts / 2 + 1;
    somtab_offset = somtab_dim1;
    somtab -= somtab_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMMAPCO", 7L);
    }
    ikdeb = (*iordre + 1) << 1;
    nbroot = *nbpnts / 2;

    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {

/* ----------------- Calcul des coefficients de degre pair ----------
---- */

	i__2 = *ndgjac;
	for (ik = ikdeb; ik <= i__2; ik += 2) {
	    igss = ik - ikdeb;
	    bidon = 0.;
	    i__3 = nbroot;
	    for (ir = 1; ir <= i__3; ++ir) {
		bidon += somtab[ir + nd * somtab_dim1] * gsstab[ir + igss * 
			gsstab_dim1];
/* L300: */
	    }
	    crvjac[ik + nd * crvjac_dim1] = bidon;
/* L200: */
	}

/* --------------- Calcul des coefficients de degre impair ----------
---- */

	i__2 = *ndgjac;
	for (ik = ikdeb + 1; ik <= i__2; ik += 2) {
	    igss = ik - ikdeb;
	    bidon = 0.;
	    i__3 = nbroot;
	    for (ir = 1; ir <= i__3; ++ir) {
		bidon += diftab[ir + nd * diftab_dim1] * gsstab[ir + igss * 
			gsstab_dim1];
/* L500: */
	    }
	    crvjac[ik + nd * crvjac_dim1] = bidon;
/* L400: */
	}

/* L100: */
    }

/* ------- Ajout des termes lies a la racine supplementaire (0.D0) ------ 
*/
/* ----------- du polynome de Legendre de degre impair NBPNTS ----------- 
*/

    if (*nbpnts % 2 == 0) {
	goto L9999;
    }
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *ndgjac;
	for (ik = ikdeb; ik <= i__2; ik += 2) {
	    igss = ik - ikdeb;
	    crvjac[ik + nd * crvjac_dim1] += somtab[nd * somtab_dim1] * 
		    gsstab[igss * gsstab_dim1];
/* L700: */
	}
/* L600: */
    }

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMMAPCO", 7L);
    }
    return 0;
} /* mmmapcoe_ */
//=======================================================================
//function : mmaperm_
//purpose  : 
//=======================================================================
int mmaperm_(integer *ncofmx, 
	     integer *ndim, 
	     integer *ncoeff, 
	     integer *iordre, 
	     doublereal *crvjac, 
	     integer *ncfnew, 
	     doublereal *errmoy)
{
  /* System generated locals */
  integer crvjac_dim1, crvjac_offset, i__1, i__2;

  /* Local variables */
  static doublereal bidj;
  static integer i__, ia, nd, ncfcut, ibb;
  static doublereal bid;
  
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Calcule la racine carree de l' erreur quadratique moyenne */
/*        d' approximation faite lorsque l' on ne conserve que les */
/*        premiers NCFNEW coefficients d' une courbe de degre NCOEFF-1 */
/*        ecrite dans la base de Jacobi NORMALISEE d' ordre */
/*        2*(IORDRE+1). */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIM   : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        IORDRE : Ordre de contrainte de continuite aux extremites. */
/*        CRVJAC : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        ERRMOY : La precision moyenne de l' approximation. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     23-12-1989 : RBD ; Creation */

/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */

    /* Parameter adjustments */
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMAPERM", 7L);
    }

/* --------- Degre minimum pouvant etre atteint : Arret a 1 ou IA ------- 
*/

    ia = (*iordre + 1) << 1;
    ncfcut = ia + 1;
    if (*ncfnew + 1 > ncfcut) {
	ncfcut = *ncfnew + 1;
    }

/* -------------- Elimination des coefficients de haut degre ------------ 
*/
/* ----------- Boucle sur la serie de Jacobi :NCFCUT --> NCOEFF --------- 
*/

    *errmoy = 0.;
    bid = 0.;
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *ncoeff;
	for (i__ = ncfcut; i__ <= i__2; ++i__) {
	    bidj = crvjac[i__ + nd * crvjac_dim1];
	    bid += bidj * bidj;
/* L200: */
	}
/* L100: */
    }

/* ----------- Racine carree de l' erreur quadratique moyenne ----------- 
*/

    bid /= 2.;
    *errmoy = sqrt(bid);

/* ------------------------------- The end ------------------------------ 
*/

    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMAPERM", 7L);
    }
    return 0;
} /* mmaperm_ */
//=======================================================================
//function : mmapptt_
//purpose  : 
//=======================================================================
int AdvApp2Var_ApproxF2var::mmapptt_(const integer *ndgjac, 
				     const integer *nbpnts, 
				     const integer *jordre, 
				     doublereal *cgauss, 
				     integer *iercod)
{
  /* System generated locals */
  integer cgauss_dim1, i__1;
  
  /* Local variables */
  static integer kjac, iptt, ipdb0, infdg, iptdb, mxjac, ilong, ibb;
	    


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Charge les elements necessaires a une integration par la */
/*        methode de Gauss pour obtenir les coefficients dans la base de 
*/
/*        Legendre de l' approximation par les moindres carres d' une */
/*        fonction. les elements sont stockes dans les communs MMAPGSS */
/*        (cas sans contrainte), MMAPGS0 (contraintes C0), MMAPGS1 */
/*        (contraintes C1) et MMAPGS2 (contraintes C2). */

/*     MOTS CLES : */
/*     ----------- */
/*        INTEGRATION,GAUSS,JACOBI */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDGJAC : Degre maxi du polynome d' approximation. La */
/*                 representation dans la base orthogonale part du degre 
*/
/*                 0 au degre NDGJAC-2*(JORDRE+1). La base polynomiale */
/*                 est la base de Jacobi d' ordre -1 (Legendre), 0, 1 */
/*                 et 2 */
/*        NBPNTS : Degre du polynome de Legendre sur les racines duquel */
/*                 sont calcules les coefficients d' integration par la */
/*                 methode de Gauss. On doit avoir NBPNTS=8,10,15,20,25, 
*/
/*                  30,40,50 ou 61 et NDGJAC < NBPNTS. */
/*        JORDRE : Ordre de la base de Jacobi (-1,0,1 ou 2). Correspond */
/*                 a pas de contraintes, contraintes C0,C1 ou C2. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CGAUSS(i,k) : Table des coefficients d' integration par la */
/*                      methode de Gauss : i varie de 0 a la partie */
/*                      entiere de NBPNTS/2 et k varie de 0 a */
/*                      NDGJAC-2*(JORDRE+1). */
/*                      Ce sont donc les coeff. d'integration associes */
/*                      aux racines positives du polynome de Legendre de 
*/
/*                      degre NBPNTS. CGAUSS(0,k) contient les coeff. */
/*                      d'integration associes a la racine t = 0 lorsque 
*/
/*                      NBPNTS est impair. */
/*        IERCOD : Code d' erreur. */
/*                 = 0 OK, */
/*                 = 11 NBPNTS ne vaut pas 8,10,15,20,25,30,40,50 ou 61. 
*/
/*                 = 21 JORDRE ne vaut pas -1,0,1 ou 2. */
/*                 = 31 NDGJAC est trop grand ou trop petit. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*        MMAPGSS,MMAPGS0,MMAPGS1,MMAPGS2. */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     12-03-93  : MPS ; Ajout des valeurs 8,10,15,20,25 pour */
/*                       le nombre de points d'integration */
/*     13-05-91  : RBD ; Ajout commentaires. */
/*     03-03-90  : NAK ; Includes. */
/*     21-04-87  : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */

/*     MOTS CLES : */
/*     ----------- */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*      INITIALISATION : BLOCK DATA */
/*      FONCTION D'ACCES SEULEMENT : MMAPPTT */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     12-03-93 : MPS ; Modification des parametres IPTGSS et IP0GSS */
/*     02-03-90 : NAK ; Creation version originale */
/* > */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */

/*     MOTS CLES : */
/*     ----------- */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*      INITIALISATION : BLOCK DATA */
/*      FONCTION D'ACCES SEULEMENT : MMAPPTT */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     12-03-93: MPS ; Modification des parametres IPTGS0 et IP0GS0 */
/*     02-03-90 : NAK ; Creation version originale */
/* > */
/* ***********************************************************************
 */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */

/*     MOTS CLES : */
/*     ----------- */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*      INITIALISATION : BLOCK DATA */
/*      FONCTION D'ACCES SEULEMENT : MMAPPTT */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     12-03-93 : MPS ; Modification des parametres IPTGS1 etIP0GS1 */
/*     02-03-90 : NAK ; Creation version originale */
/* > */
/* ***********************************************************************
 */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */

/*     MOTS CLES : */
/*     ----------- */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*      INITIALISATION : BLOCK DATA */
/*      FONCTION D'ACCES SEULEMENT : MMAPPTT */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     12-03-93 : MPS ; Modification des parametres IPTGS2 et IP0GS2 */
/*     02-03-90 : NAK ; Creation version originale */
/* > */

/* ***********************************************************************
 */
    /* Parameter adjustments */
    cgauss_dim1 = *nbpnts / 2 + 1;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMAPPTT", 7L);
    }
    *iercod = 0;

/* ------------------- Tests sur la validite des entrees ---------------- 
*/

    infdg = (*jordre + 1) << 1;
    if (*nbpnts != 8 && *nbpnts != 10 && *nbpnts != 15 && *nbpnts != 20 && *
	    nbpnts != 25 && *nbpnts != 30 && *nbpnts != 40 && *nbpnts != 50 &&
	     *nbpnts != 61) {
	goto L9100;
    }

    if (*jordre < -1 || *jordre > 2) {
	goto L9200;
    }

    if (*ndgjac >= *nbpnts || *ndgjac < infdg) {
	goto L9300;
    }

/* --------------- Calcul du pointeur de debut suivant NBPNTS ----------- 
*/

    iptdb = 0;
    if (*nbpnts > 8) {
	iptdb += (8 - infdg) << 2;
    }
    if (*nbpnts > 10) {
	iptdb += (10 - infdg) * 5;
    }
    if (*nbpnts > 15) {
	iptdb += (15 - infdg) * 7;
    }
    if (*nbpnts > 20) {
	iptdb += (20 - infdg) * 10;
    }
    if (*nbpnts > 25) {
	iptdb += (25 - infdg) * 12;
    }
    if (*nbpnts > 30) {
	iptdb += (30 - infdg) * 15;
    }
    if (*nbpnts > 40) {
	iptdb += (40 - infdg) * 20;
    }
    if (*nbpnts > 50) {
	iptdb += (50 - infdg) * 25;
    }

    ipdb0 = 1;
    if (*nbpnts > 15) {
	ipdb0 = ipdb0 + (14 - infdg) / 2 + 1;
    }
    if (*nbpnts > 25) {
	ipdb0 = ipdb0 + (24 - infdg) / 2 + 1;
    }

/* ------------------ Choix du commun en fonction de JORDRE ------------- 
*/

    if (*jordre == -1) {
	goto L1000;
    }
    if (*jordre == 0) {
	goto L2000;
    }
    if (*jordre == 1) {
	goto L3000;
    }
    if (*jordre == 2) {
	goto L4000;
    }

/* ---------------- Commun MMAPGSS (cas sans contraintes) ----------------
 */

L1000:
    ilong = *nbpnts / 2 << 3;
    i__1 = *ndgjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 (char *)&mmapgss_.gslxjs[iptt - 1], 
		 (char *)&cgauss[kjac * cgauss_dim1 + 1]);
/* L100: */
    }
/* --> Cas ou le nbre de points est impair. */
    if (*nbpnts % 2 == 1) {
	iptt = ipdb0;
	i__1 = *ndgjac;
	for (kjac = 0; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = mmapgss_.gsl0js[iptt - 1];
	    ++iptt;
/* L150: */
	}
	i__1 = *ndgjac;
	for (kjac = 1; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = 0.;
/* L160: */
	}
    }
    goto L9999;

/* ---------------- Commun MMAPGS0 (cas avec contraintes C0) -------------
 */

L2000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 (char *)&mmapgs0_.gslxj0[iptt - 1], 
		 (char *)&cgauss[kjac * cgauss_dim1 + 1]);
/* L200: */
    }
/* --> Cas ou le nbre de points est impair. */
    if (*nbpnts % 2 == 1) {
	iptt = ipdb0;
	i__1 = mxjac;
	for (kjac = 0; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = mmapgs0_.gsl0j0[iptt - 1];
	    ++iptt;
/* L250: */
	}
	i__1 = mxjac;
	for (kjac = 1; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = 0.;
/* L260: */
	}
    }
    goto L9999;

/* ---------------- Commun MMAPGS1 (cas avec contraintes C1) -------------
 */

L3000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 (char *)&mmapgs1_.gslxj1[iptt - 1], 
		 (char *)&cgauss[kjac * cgauss_dim1 + 1]);
/* L300: */
    }
/* --> Cas ou le nbre de points est impair. */
    if (*nbpnts % 2 == 1) {
	iptt = ipdb0;
	i__1 = mxjac;
	for (kjac = 0; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = mmapgs1_.gsl0j1[iptt - 1];
	    ++iptt;
/* L350: */
	}
	i__1 = mxjac;
	for (kjac = 1; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = 0.;
/* L360: */
	}
    }
    goto L9999;

/* ---------------- Commun MMAPGS2 (cas avec contraintes C2) -------------
 */

L4000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 (char *)&mmapgs2_.gslxj2[iptt - 1], 
		 (char *)&cgauss[kjac * cgauss_dim1 + 1]);
/* L400: */
    }
/* --> Cas ou le nbre de points est impair. */
    if (*nbpnts % 2 == 1) {
	iptt = ipdb0;
	i__1 = mxjac;
	for (kjac = 0; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = mmapgs2_.gsl0j2[iptt - 1];
	    ++iptt;
/* L450: */
	}
	i__1 = mxjac;
	for (kjac = 1; kjac <= i__1; kjac += 2) {
	    cgauss[kjac * cgauss_dim1] = 0.;
/* L460: */
	}
    }
    goto L9999;

/* ------------------------- Recuperation du code d' erreur --------------
 */
/* --> NBPNTS n' est pas bon */
L9100:
    *iercod = 11;
    goto L9999;
/* --> JORDRE n' est pas bon */
L9200:
    *iercod = 21;
    goto L9999;
/* --> NDGJAC n' est pas bon */
L9300:
    *iercod = 31;
    goto L9999;

/* -------------------------------- The end ----------------------------- 
*/

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMAPPTT", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMAPPTT", 7L);
    }

 return 0 ;
} /* mmapptt_ */

//=======================================================================
//function : mmjacpt_
//purpose  : 
//=======================================================================
int mmjacpt_(const integer *ndimen,
	     const integer *ncoefu, 
	     const integer *ncoefv, 
	     const integer *iordru, 
	     const integer *iordrv, 
	     const doublereal *ptclgd, 
	     doublereal *ptcaux, 
	     doublereal *ptccan)
{
    /* System generated locals */
  integer ptccan_dim1, ptccan_dim2, ptccan_offset, ptclgd_dim1, ptclgd_dim2,
  ptclgd_offset, ptcaux_dim1, ptcaux_dim2, ptcaux_dim3, 
  ptcaux_offset, i__1, i__2, i__3;
  
  /* Local variables */
  static integer kdim, nd, ii, jj, ibb;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Passage de la base canonique a la base de Jacobi pour */
/*        un "carreau" dans un espace de dimension qcq. */

/*     MOTS CLES : */
/*     ----------- */
/*        LISSAGE,BASE,LEGENDRE */


/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFU : Degre+1 en U. */
/*        NCOEFV : Degre+1 en V. */
/*        IORDRU : Ordre des polynomes de Jacobi en U. */
/*        IORDRV : Ordre des polynomes de Jacobi en V. */
/*        PTCLGD : Le carreau dans la base de Jacobi. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        PTCAUX : Espace auxilliaire. */
/*        PTCCAN : Le carreau dans la base canonique (-1,1) */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Annule et remplace MJACPC */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     03-08-1989 : RBD; Creation. */
/* > */
/* ********************************************************************* 
*/
/*   Le nom de la routine */


    /* Parameter adjustments */
    ptccan_dim1 = *ncoefu;
    ptccan_dim2 = *ncoefv;
    ptccan_offset = ptccan_dim1 * (ptccan_dim2 + 1) + 1;
    ptccan -= ptccan_offset;
    ptcaux_dim1 = *ncoefv;
    ptcaux_dim2 = *ncoefu;
    ptcaux_dim3 = *ndimen;
    ptcaux_offset = ptcaux_dim1 * (ptcaux_dim2 * (ptcaux_dim3 + 1) + 1) + 1;
    ptcaux -= ptcaux_offset;
    ptclgd_dim1 = *ncoefu;
    ptclgd_dim2 = *ncoefv;
    ptclgd_offset = ptclgd_dim1 * (ptclgd_dim2 + 1) + 1;
    ptclgd -= ptclgd_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMJACPT", 7L);
    }

/*   Passage dans canonique en u. */

    kdim = *ndimen * *ncoefv;
    AdvApp2Var_MathBase::mmjaccv_((integer *)ncoefu, 
	     (integer *)&kdim, 
	     (integer *)iordru, 
	     (doublereal *)&ptclgd[ptclgd_offset], 
	     (doublereal *)&ptcaux[ptcaux_offset], 
	     (doublereal *)&ptccan[ptccan_offset]);

/*   Permutation des u et des v. */

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *ncoefv;
	for (jj = 1; jj <= i__2; ++jj) {
	    i__3 = *ncoefu;
	    for (ii = 1; ii <= i__3; ++ii) {
		ptcaux[jj + (ii + (nd + ptcaux_dim3) * ptcaux_dim2) * 
			ptcaux_dim1] = ptccan[ii + (jj + nd * ptccan_dim2) * 
			ptccan_dim1];
/* L320: */
	    }
/* L310: */
	}
/* L300: */
    }

/*   Passage dans canonique en v. */

    kdim = *ndimen * *ncoefu;
    AdvApp2Var_MathBase::mmjaccv_((integer *)ncoefv, 
	     (integer *)&kdim, 
	     (integer *)iordrv, 
	     (doublereal *)&ptcaux[((ptcaux_dim3 + 1) * ptcaux_dim2 + 1) * ptcaux_dim1 + 1], 
	     (doublereal *)&ptccan[ptccan_offset], 
	     (doublereal *)&ptcaux[(((ptcaux_dim3 << 1) + 1) * ptcaux_dim2 + 1) * ptcaux_dim1 + 1]);

/*   Permutation des u et des v. */

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *ncoefv;
	for (jj = 1; jj <= i__2; ++jj) {
	    i__3 = *ncoefu;
	    for (ii = 1; ii <= i__3; ++ii) {
		ptccan[ii + (jj + nd * ptccan_dim2) * ptccan_dim1] = ptcaux[
			jj + (ii + (nd + (ptcaux_dim3 << 1)) * ptcaux_dim2) * 
			ptcaux_dim1];
/* L420: */
	    }
/* L410: */
	}
/* L400: */
    }

/* ---------------------------- THAT'S ALL FOLKS ------------------------ 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMJACPT", 7L);
    }
    return 0;
} /* mmjacpt_ */
