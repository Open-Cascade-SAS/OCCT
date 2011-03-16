//
// AdvApp2Var_MathBase.cxx
//
#include <math.h>
#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_Data.hxx>

// statics 
static
int mmchole_(integer *mxcoef, 
	     integer *dimens, 
	     doublereal *amatri, 
	     integer *aposit, 
	     integer *posuiv, 
	     doublereal *chomat, 
	     integer *iercod);




static
int mmrslss_(integer *mxcoef, 
	     integer *dimens, 
	     doublereal *smatri, 
	     integer *sposit,
	     integer *posuiv, 
	     doublereal *mscnmbr,
	     doublereal *soluti, 
	     integer *iercod);

static
int mfac_(doublereal *f,
	  integer *n);

static
int mmaper0_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvlgd, 
	     integer *ncfnew, 
	     doublereal *ycvmax, 
	     doublereal *errmax);
static
int mmaper2_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew, 
	     doublereal *ycvmax, 
	     doublereal *errmax);

static
int mmaper4_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew,
	     doublereal *ycvmax,
	     doublereal *errmax);

static
int mmaper6_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew,
	     doublereal *ycvmax,
	     doublereal *errmax);

static
int mmarc41_(integer *ndimax, 
	     integer *ndimen, 
	     integer *ncoeff,
	     doublereal *crvold,
	     doublereal *upara0,
	     doublereal *upara1,
	     doublereal *crvnew,
	     integer *iercod);

static
int mmatvec_(integer *nligne, 
	     integer *ncolon,
	     integer *gposit,
	     integer *gnstoc, 
	     doublereal *gmatri,
	     doublereal *vecin, 
	     integer *deblig,
	     doublereal *vecout,
	     integer *iercod);

static
int mmcvstd_(integer *ncofmx, 
	     integer *ndimax, 
	     integer *ncoeff,
	     integer *ndimen, 
	     doublereal *crvcan, 
	     doublereal *courbe);

static
int mmdrvcb_(integer *ideriv,
	     integer *ndim, 
	     integer *ncoeff,
	     doublereal *courbe, 
	     doublereal *tparam,
	     doublereal *tabpnt, 
	     integer *iercod);

static
int mmexthi_(integer *ndegre, 
	     doublereal *hwgaus);

static
int mmextrl_(integer *ndegre,
	     doublereal *rootlg);



static
int mmherm0_(doublereal *debfin, 
	     integer *iercod);

static
int mmherm1_(doublereal *debfin, 
	     integer *ordrmx, 
	     integer *iordre, 
	     doublereal *hermit, 
	     integer *iercod);
static
int mmloncv_(integer *ndimax,
	     integer *ndimen,
	     integer *ncoeff,
	     doublereal *courbe, 
	     doublereal *tdebut, 
	     doublereal *tfinal, 
	     doublereal *xlongc, 
	     integer *iercod);
static
int mmpojac_(doublereal *tparam, 
	     integer *iordre, 
	     integer *ncoeff, 
	     integer *nderiv, 
	     doublereal *valjac, 
	     integer *iercod);

static
int mmrslw_(integer *normax, 
	    integer *nordre, 
	    integer *ndimen, 
	    doublereal *epspiv,
	    doublereal *abmatr,
	    doublereal *xmatri, 
	    integer *iercod);
static
int mmtmave_(integer *nligne, 
	     integer *ncolon, 
	     integer *gposit, 
	     integer *gnstoc, 
	     doublereal *gmatri,
	     doublereal *vecin, 
	     doublereal *vecout, 
	     integer *iercod);
static
int mmtrpj0_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew);
static
int mmtrpj2_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew);

static
int mmtrpj4_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew);
static
int mmtrpj6_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew);
static
integer  pow__ii(integer *x, 
		 integer *n);

static
int mvcvin2_(integer *ncoeff, 
	     doublereal *crvold, 
	     doublereal *crvnew,
	     integer *iercod);

static
int mvcvinv_(integer *ncoeff,
	     doublereal *crvold, 
	     doublereal *crvnew, 
	     integer *iercod);

static
int mvgaus0_(integer *kindic, 
	     doublereal *urootl, 
	     doublereal *hiltab, 
	     integer *nbrval, 
	     integer *iercod);
static
int mvpscr2_(integer *ncoeff, 
	     doublereal *curve2, 
	     doublereal *tparam, 
	     doublereal *pntcrb);

static
int mvpscr3_(integer *ncoeff, 
	     doublereal *curve2, 
	     doublereal *tparam, 
	     doublereal *pntcrb);

static struct {
    doublereal eps1, eps2, eps3, eps4;
    integer niterm, niterr;
} mmprcsn_;

static struct {
    doublereal tdebut, tfinal, verifi, cmherm[576];	
} mmcmher_;

//=======================================================================
//function : AdvApp2Var_MathBase::mdsptpt_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mdsptpt_(integer *ndimen, 
				  doublereal *point1, 
				  doublereal *point2, 
				  doublereal *distan)

{
  static integer c__8 = 8;
  /* System generated locals */
  integer i__1;
  doublereal d__1;
  
  /* Local variables */
  static integer i__;
  static doublereal differ[100];
  static integer  ier;
  long int iofset, j;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        CALCULE LA DISTANCE ENTRE DEUX POINTS */

/*     MOTS CLES : */
/*     ----------- */
/*        DISTANCE,POINT. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN: Dimension de l' espace. */
/*        POINT1: Tableau des coordonnees du 1er point. */
/*        POINT2: Tableau des coordonnees du 2eme point. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        DISTAN: Distance des 2 points. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     21-07-94 : PMN ; La valeur seuil pour alloc passe de 3 a 100 */
/*     15-07-93 : PMN ; Protection des points... */
/*     08-09-90 : DHU ; Utilisation de MZSNORM */
/*     18-07-88 : RBD ; AJOUT D' UN EN TETE STANDARD */
/*     ??-??-?? : XXX ; CREATION */
/* > */
/* ********************************************************************** 
*/


/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --point2;
    --point1;

    /* Function Body */
    iofset = 0;
    ier = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    if (*ndimen > 100) {
	AdvApp2Var_SysBase::mcrrqst_(&c__8, ndimen, differ, &iofset, &ier);
    }

/* --- Si l'allocation est refuse, on applique la methode trivial */

    if (ier > 0) {

	*distan = 0.;
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	    d__1 = point1[i__] - point2[i__];
	    *distan += d__1 * d__1;
	}
	*distan = sqrt(*distan);

/* --- Sinon on utilise MZSNORM pour minimiser les risques d'overflow 
*/

    } else {
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j=iofset + i__ - 1;
	    differ[j] = point2[i__] - point1[i__];
	}

	*distan = AdvApp2Var_MathBase::mzsnorm_(ndimen, &differ[iofset]);

    }

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

/* --- Desallocation dynamique */

    if (iofset != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, ndimen, differ, &iofset, &ier);
    }

 return 0 ;
} /* mdsptpt_ */

//=======================================================================
//function : mfac_
//purpose  : 
//=======================================================================
int mfac_(doublereal *f, 
	  integer *n)

{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

/*    FORTRAN CONFORME AU TEXT */
/*     CALCUL DE MFACTORIEL N */
    /* Parameter adjustments */
    --f;

    /* Function Body */
    f[1] = (float)1.;
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* L10: */
	f[i__] = i__ * f[i__ - 1];
    }
    return 0;
} /* mfac_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmapcmp_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmapcmp_(integer *ndim, 
				  integer *ncofmx, 
				  integer *ncoeff, 
				  doublereal *crvold, 
				  doublereal *crvnew)

{
  /* System generated locals */
  integer crvold_dim1, crvold_offset, crvnew_dim1, crvnew_offset, i__1, 
  i__2;
  
  /* Local variables */
  static integer ipair, nd, ndegre, impair, ibb, idg;
  //extern  int  mgsomsg_();//mgenmsg_(),
  
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Compression de la courbe CRVOLD en un tableau comprenant */
/*        les coeff. de rang pair : CRVNEW(*,0,*) */
/*        et de rang impair : CRVNEW(*,1,*). */

/*     MOTS CLES : */
/*     ----------- */
/*        COMPRESSION,COURBE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIM   : Dimension de l' espace. */
/*     NCOFMX : Le nbre maximum de coeff. de la courbe a compacter. */
/*     NCOEFF : Le nbre maximum de coeff. de la courbe compactee. */
/*     CRVOLD : La courbe (0:NCOFMX-1,NDIM) a compacter. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     CRVNEW : La coube compactee en (0:(NCOEFF-1)/2,0,NDIM) (contenant 
*/
/*              les termes pairs) et en (0:(NCOEFF-1)/2,1,NDIM) */
/*              (contenant les termes impairs). */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Cette routine est utile pour preparer les coefficients d' une */
/*     courbe dans une base orthogonale (Legendre ou Jacobi) avant de */
/*     calculer les coefficients dans la base canonique [-1,1] par */
/*     MMJACAN. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     12-04-1989 : RBD ; Creation. */
/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */

    /* Parameter adjustments */
    crvold_dim1 = *ncofmx;
    crvold_offset = crvold_dim1;
    crvold -= crvold_offset;
    crvnew_dim1 = (*ncoeff - 1) / 2 + 1;
    crvnew_offset = crvnew_dim1 << 1;
    crvnew -= crvnew_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMAPCMP", 7L);
    }

    ndegre = *ncoeff - 1;
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	ipair = 0;
	i__2 = ndegre / 2;
	for (idg = 0; idg <= i__2; ++idg) {
	    crvnew[idg + (nd << 1) * crvnew_dim1] = crvold[ipair + nd * 
		    crvold_dim1];
	    ipair += 2;
/* L200: */
	}
	if (ndegre < 1) {
	    goto L400;
	}
	impair = 1;
	i__2 = (ndegre - 1) / 2;
	for (idg = 0; idg <= i__2; ++idg) {
	    crvnew[idg + ((nd << 1) + 1) * crvnew_dim1] = crvold[impair + nd *
		     crvold_dim1];
	    impair += 2;
/* L300: */
	}

L400:
/* L100: */
	;
    }

/* ---------------------------------- The end --------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMAPCMP", 7L);
    }
    return 0;
} /* mmapcmp_ */

//=======================================================================
//function : mmaper0_
//purpose  : 
//=======================================================================
int mmaper0_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvlgd, 
	     integer *ncfnew, 
	     doublereal *ycvmax, 
	     doublereal *errmax)

{
  /* System generated locals */
  integer crvlgd_dim1, crvlgd_offset, i__1, i__2;
  doublereal d__1;
  
  /* Local variables */
  static integer ncut;
    static doublereal bidon;
  static integer ii, nd;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calcule l' erreur d' approximation maxi faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Legendre (Jacobi */
/*        d' ordre 0). */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        ERRMAX : La precision de l' approximation. */

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
/* ***********************************************************************
 */


/* ------------------- Init pour calcul d' erreur ----------------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */
    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Degre minimum pouvant etre atteint : Arret a 1 ou NCFNEW ------ 
*/

    ncut = 1;
    if (*ncfnew + 1 > ncut) {
	ncut = *ncfnew + 1;
    }

/* -------------- Elimination des coefficients de haut degre ----------- 
*/
/* ----------- Boucle sur la serie de Legendre: NCUT --> NCOEFF -------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Facteur de renormalisation (Maximum de Li(t)). */
	bidon = ((ii - 1) * 2. + 1.) / 2.;
	bidon = sqrt(bidon);

	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[ii + nd * crvlgd_dim1], abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- L'erreur est la norme du vecteur erreur --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- Fin -------------------------------- 
*/

    return 0;
} /* mmaper0_ */

//=======================================================================
//function : mmaper2_
//purpose  : 
//=======================================================================
int mmaper2_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew, 
	     doublereal *ycvmax, 
	     doublereal *errmax)

{
  /* Initialized data */

    static doublereal xmaxj[57] = { .9682458365518542212948163499456,
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

    /* System generated locals */
    integer crvjac_dim1, crvjac_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer idec, ncut;
    static doublereal bidon;
    static integer ii, nd;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calcule l' erreur d' approximation maxi faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Jacobi d' ordre 2. */

/*     MOTS CLES : */
/*     ----------- */
/*        JACOBI,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        CRVJAC : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        ERRMAX : La precision de l' approximation. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     12-02-1992: RBD; Correction d'indice de lecture de XMAXJ */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ***********************************************************************
 */


/* ------------------ Table des maximums de (1-t2)*Ji(t) ---------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init pour calcul d' erreur ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Degre minimum pouvant etre atteint : Arret a 3 ou NCFNEW ------ 
*/

    idec = 3;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = max(i__1,i__2);

/* -------------- Elimination des coefficients de haut degre ----------- 
*/
/* ----------- Boucle sur la serie de Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- L'erreur est la norme du vecteur erreur --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- Fin -------------------------------- 
*/

    return 0;
} /* mmaper2_ */

/* MAPER4.f -- translated by f2c (version 19960827).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

/* Subroutine */ 
//=======================================================================
//function : mmaper4_
//purpose  : 
//=======================================================================
int mmaper4_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew,
	     doublereal *ycvmax,
	     doublereal *errmax)
{
    /* Initialized data */

    static doublereal xmaxj[55] = { 1.1092649593311780079813740546678,
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

    /* System generated locals */
    integer crvjac_dim1, crvjac_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer idec, ncut;
    static doublereal bidon;
    static integer ii, nd;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calcule l' erreur d' approximation maxi faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Jacobi d' ordre 4. */

/*     MOTS CLES : */
/*     ----------- */
/*        JACOBI,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        CRVJAC : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        ERRMAX : La precision de l' approximation. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     12-02-1992: RBD; Correction d'indice de lecture de XMAXJ */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ***********************************************************************
 */


/* ---------------- Table des maximums de ((1-t2)2)*Ji(t) --------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init pour calcul d' erreur ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Degre minimum pouvant etre atteint : Arret a 5 ou NCFNEW ------ 
*/

    idec = 5;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = max(i__1,i__2);

/* -------------- Elimination des coefficients de haut degre ----------- 
*/
/* ----------- Boucle sur la serie de Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- L'erreur est la norme du vecteur erreur --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- Fin -------------------------------- 
*/

    return 0;
} /* mmaper4_ */

//=======================================================================
//function : mmaper6_
//purpose  : 
//=======================================================================
int mmaper6_(integer *ncofmx, 
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *crvjac, 
	     integer *ncfnew,
	     doublereal *ycvmax,
	     doublereal *errmax)

{
    /* Initialized data */

    static doublereal xmaxj[53] = { 1.21091229812484768570102219548814,
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
    integer crvjac_dim1, crvjac_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer idec, ncut;
    static doublereal bidon;
    static integer ii, nd;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calcule l' erreur d' approximation maxi faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Jacobi d' ordre 6. */

/*     MOTS CLES : */
/*     ----------- */
/*        JACOBI,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        CRVJAC : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        ERRMAX : La precision de l' approximation. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     12-02-1992: RBD; Correction d'indice de lecture de XMAXJ */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ***********************************************************************
 */


/* ---------------- Table des maximums de ((1-t2)3)*Ji(t) --------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init pour calcul d' erreur ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Degre minimum pouvant etre atteint : Arret a 3 ou NCFNEW ------ 
*/

    idec = 7;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = max(i__1,i__2);

/* -------------- Elimination des coefficients de haut degre ----------- 
*/
/* ----------- Boucle sur la serie de Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- L'erreur est la norme du vecteur erreur --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- Fin -------------------------------- 
*/

    return 0;
} /* mmaper6_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmaperx_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmaperx_(integer *ncofmx, 
				  integer *ndimen, 
				  integer *ncoeff, 
				  integer *iordre, 
				  doublereal *crvjac, 
				  integer *ncfnew, 
				  doublereal *ycvmax, 
				  doublereal *errmax, 
				  integer *iercod)

{
  /* System generated locals */
  integer crvjac_dim1, crvjac_offset;
  
  /* Local variables */
  static integer jord;
 

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Calcule l' erreur d' approximation maxi faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Jacobi d' ordre */
/*        IORDRE. */

/*     MOTS CLES : */
/*     ----------- */
/*        JACOBI,LEGENDRE,POLYGONE,APPROXIMATION,ERREUR. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Degre maximum de la courbe. */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 de la courbe. */
/*        IORDRE : Ordre de continuite aux extremites. */
/*        CRVJAC : La courbe dont on veut baisser le degre. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire. */
/*        ERRMAX : La precision de l' approximation. */
/*        IERCOD = 0, OK */
/*               = 1, L'ordre des contraintes (IORDRE) n'est pas dans */
/*                    les valeurs autorisees. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Annule et remplace MMAPERR. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     08-08-91: RBD; Creation d'apres MMAPERR, utilisation des nouveaux 
*/
/*                    majorants, appel aux MMAPER0, 2, 4 et 6. */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */
    *iercod = 0;
/* --> L'ordre des polynomes de Jacobi */
    jord = ( *iordre + 1) << 1;

    if (jord == 0) {
	mmaper0_(ncofmx, ndimen, ncoeff, &crvjac[crvjac_offset], ncfnew, &
		ycvmax[1], errmax);
    } else if (jord == 2) {
	mmaper2_(ncofmx, ndimen, ncoeff, &crvjac[crvjac_offset], ncfnew, &
		ycvmax[1], errmax);
    } else if (jord == 4) {
	mmaper4_(ncofmx, ndimen, ncoeff, &crvjac[crvjac_offset], ncfnew, &
		ycvmax[1], errmax);
    } else if (jord == 6) {
	mmaper6_(ncofmx, ndimen, ncoeff, &crvjac[crvjac_offset], ncfnew, &
		ycvmax[1], errmax);
    } else {
	*iercod = 1;
    }

/* ----------------------------------- Fin ------------------------------ 
*/

    return 0;
} /* mmaperx_ */

//=======================================================================
//function : mmarc41_
//purpose  : 
//=======================================================================
 int mmarc41_(integer *ndimax, 
	      integer *ndimen, 
	      integer *ncoeff,
	      doublereal *crvold,
	      doublereal *upara0,
	      doublereal *upara1,
	      doublereal *crvnew,
	      integer *iercod)

{
  /* System generated locals */
    integer crvold_dim1, crvold_offset, crvnew_dim1, crvnew_offset, i__1, 
    i__2, i__3;
    
    /* Local variables */
    static integer nboct;
    static doublereal tbaux[61];
    static integer nd;
    static doublereal bid;
    static integer ncf, ncj;


/*      IMPLICIT DOUBLE PRECISION(A-H,O-Z) */
/*      IMPLICIT INTEGER (I-N) */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     Creation de la courbe C2(v) definie sur (0,1) identique a la */
/*     courbe C1(u) definie sur (U0,U1) (changement du parametre d' une */
/*     courbe). */

/*     MOTS CLES : */
/*     ----------- */
/*        LIMITATION, RESTRICTION, COURBE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMAX : Dimensionnement de l' espace. */
/*   NDIMEN   : Dimension de la courbe. */
/*   NCOEFF : Nbre de coefficients de la courbe. */
/*   CRVOLD : La courbe a limiter. */
/*   UPARA0     : Borne min de l' intervalle de restriction de la courbe. 
*/
/*   UPARA1     : Borne max de l' intervalle de restriction de la courbe. 
*/

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   CRVNEW : La courbe relimitee, definie dans (0,1) et egale a */
/*            CRVOLD definie dans (U0,U1). */
/*   IERCOD : = 0, OK */
/*            =10, Nbre de coeff. <1 ou > 61. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG              MCRFILL              MVCVIN2 */
/*           MVCVINV */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/* ---> L' algorithme employe dans le cas general est base sur le */
/*     principe suivant : */
/*        Soient S(t) = a0 + a1*t + a2*t**2 + ... de degre NCOEFF-1, et */
/*               U(t) = b0 + b1*t, on calcule alors les coeff. de */
/*        S(U(t)) de proche en proche a l' aide du tableau TBAUX. */
/*        A chaque etape numero N (N=2 a NCOEFF), TBAUX(n) contient le */
/*        n-ieme coefficient de U(t)**N pour n=1 a N. (RBD) */
/* ---> Reference : KNUTH, 'The Art of Computer Programming', */
/*                        Vol. 2/'Seminumerical Algorithms', */
/*                        Ex. 11 p:451 et solution p:562. (RBD) */

/* ---> L' ecrasement de l' argument d' entree CRVOLD par CRVNEW est */
/*     possible, c' est a dire que l' appel : */
/*       CALL MMARC41(NDIMAX,NDIMEN,NCOEFF,CURVE,UPARA0,UPARA1 */
/*                  ,CURVE,IERCOD) */
/*     est tout a fait LEGAL. (RBD) */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     18-09-1995 : JMF ; Verfor + implicit none */
/*     18-10-88   : RBD ; Documentation de la FONCTION. */
/*     24-06-88   : RBD ; Refonte totale du code pour le cas general : */
/*                        optimisation et suppression du commun des CNP */
/*                        qui ne sert plus. */
/*     22-06-88   : NAK ; TRAITEMENT DES CAS PARTICULIERS SIMPLES ET */
/*                        FREQUENTS. */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB. */
/*     22-02-1988 : JJM ; Appel GERMSG -> MAERMSG. */
/*     26-07-1985 : Remplacement de CAUX par CRVNEW, ajout du */
/*                  common MBLANK. */
/*     28-11-1985 : Creation JJM (NDIMAX en plus). */

/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

/*   Tableau auxiliaire des coefficients de (UPARA1-UPARA0)T+UPARA0 a */
/*   la puissance N=1 a NCOEFF-1. */


    /* Parameter adjustments */
    crvnew_dim1 = *ndimax;
    crvnew_offset = crvnew_dim1 + 1;
    crvnew -= crvnew_offset;
    crvold_dim1 = *ndimax;
    crvold_offset = crvold_dim1 + 1;
    crvold -= crvold_offset;

    /* Function Body */
    *iercod = 0;
/* ********************************************************************** 
*/
/*                CAS OU LE TRAITEMENT NE PEUT ETRE FAIT */
/* ********************************************************************** 
*/
    if (*ncoeff > 61 || *ncoeff < 1) {
	*iercod = 10;
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                         SI PAS DE CHANGEMENT */
/* ********************************************************************** 
*/
    if (*ndimen == *ndimax && *upara0 == 0. && *upara1 == 1.) {
	nboct = (*ndimax << 3) * *ncoeff;
	AdvApp2Var_SysBase::mcrfill_((integer *)&nboct,
		 (char *)&crvold[crvold_offset], 
		 (char *)&crvnew[crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                    INVERSION 3D : TRAITEMENT RAPIDE */
/* ********************************************************************** 
*/
    if (*upara0 == 1. && *upara1 == 0.) {
	if (*ndimen == 3 && *ndimax == 3 && *ncoeff <= 21) {
	    mvcvinv_(ncoeff, &crvold[crvold_offset], &crvnew[crvnew_offset], 
		    iercod);
	    goto L9999;
	}
/* ******************************************************************
**** */
/*                    INVERSION 2D : TRAITEMENT RAPIDE */
/* ******************************************************************
**** */
	if (*ndimen == 2 && *ndimax == 2 && *ncoeff <= 21) {
	    mvcvin2_(ncoeff, &crvold[crvold_offset], &crvnew[crvnew_offset], 
		    iercod);
	    goto L9999;
	}
    }
/* ********************************************************************** 
*/
/*                          TRAITEMENT GENERAL */
/* ********************************************************************** 
*/
/* -------------------------- Initialisations --------------------------- 
*/

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	crvnew[nd + crvnew_dim1] = crvold[nd + crvold_dim1];
/* L100: */
    }
    if (*ncoeff == 1) {
	goto L9999;
    }
    tbaux[0] = *upara0;
    tbaux[1] = *upara1 - *upara0;

/* ----------------------- Calcul des coeff. de CRVNEW ------------------ 
*/

    i__1 = *ncoeff - 1;
    for (ncf = 2; ncf <= i__1; ++ncf) {

/* ------------ Prise en compte du NCF-ieme coeff. de CRVOLD --------
---- */

	i__2 = ncf - 1;
	for (ncj = 1; ncj <= i__2; ++ncj) {
	    bid = tbaux[ncj - 1];
	    i__3 = *ndimen;
	    for (nd = 1; nd <= i__3; ++nd) {
		crvnew[nd + ncj * crvnew_dim1] += crvold[nd + ncf * 
			crvold_dim1] * bid;
/* L400: */
	    }
/* L300: */
	}

	bid = tbaux[ncf - 1];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    crvnew[nd + ncf * crvnew_dim1] = crvold[nd + ncf * crvold_dim1] * 
		    bid;
/* L500: */
	}

/* --------- Calcul des (NCF+1) coeff. de ((U1-U0)*t + U0)**(NCF) ---
---- */

	bid = *upara1 - *upara0;
	tbaux[ncf] = tbaux[ncf - 1] * bid;
	for (ncj = ncf; ncj >= 2; --ncj) {
	    tbaux[ncj - 1] = tbaux[ncj - 1] * *upara0 + tbaux[ncj - 2] * bid;
/* L600: */
	}
	tbaux[0] *= *upara0;

/* L200: */
    }

/* -------------- Prise en compte du dernier coeff. de CRVOLD ----------- 
*/

    i__1 = *ncoeff - 1;
    for (ncj = 1; ncj <= i__1; ++ncj) {
	bid = tbaux[ncj - 1];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    crvnew[nd + ncj * crvnew_dim1] += crvold[nd + *ncoeff * 
		    crvold_dim1] * bid;
/* L800: */
	}
/* L700: */
    }
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	crvnew[nd + *ncoeff * crvnew_dim1] = crvold[nd + *ncoeff * 
		crvold_dim1] * tbaux[*ncoeff - 1];
/* L900: */
    }

/* ---------------------------- The end --------------------------------- 
*/

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMARC41", iercod, 7L);
    }

 return 0 ;
} /* mmarc41_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmarcin_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmarcin_(integer *ndimax, 
				  integer *ndim, 
				  integer *ncoeff, 
				  doublereal *crvold, 
				  doublereal *u0, 
				  doublereal *u1, 
				  doublereal *crvnew, 
				  integer *iercod)

{
  /* System generated locals */
  integer crvold_dim1, crvold_offset, crvnew_dim1, crvnew_offset, i__1, 
  i__2, i__3;
  doublereal d__1;
  
  /* Local variables */
  static doublereal x0, x1;
  static integer nd;
  static doublereal tabaux[61];
  static integer ibb;
  static doublereal bid;
  static integer ncf;
  static integer ncj;
  static doublereal eps3;
  


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Creation de la courbe C2(v) definie sur [U0,U1] identique a */
/*     la courbe C1(u) definie sur [-1,1] (changement du parametre */
/*     d' une courbe) avec INVERSION des indices du tableau resultat. */

/*     MOTS CLES : */
/*     ----------- */
/*        LIMITATION GENERALISEE,RESTRICTION,INVERSION,COURBE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMAX : Dimensionnement maximal de l' espace. */
/*   NDIM   : Dimension de la courbe. */
/*   NCOEFF : Nbre de coefficients de la courbe. */
/*   CRVOLD : La courbe a limiter. */
/*   U0     : Borne min de l' intervalle de restriction de la courbe. */
/*   U1     : Borne max de l' intervalle de restriction de la courbe. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   CRVNEW : La courbe relimitee, definie dans [U0,U1] et egale a */
/*            CRVOLD definie dans [-1,1]. */
/*   IERCOD : = 0, OK */
/*            =10, Nbre de coeff. <1 ou > 61. */
/*            =13, L' intervalle de variation demande est nul. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*        21-11-1989 : RBD ; Correction Trait. general parametre X1. */
/*        12-04-1989 : RBD ; Creation d' apres MMARC41. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

/*   Tableau auxiliaire des coefficients de X1*T+X0 a */
/*   la puissance N=1 a NCOEFF-1. */


    /* Parameter adjustments */
    crvnew_dim1 = *ndimax;
    crvnew_offset = crvnew_dim1 + 1;
    crvnew -= crvnew_offset;
    crvold_dim1 = *ncoeff;
    crvold_offset = crvold_dim1 + 1;
    crvold -= crvold_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMARCIN", 7L);
    }

/* On teste au zero machine que l' intervalle d' arrivee n' est pas nul */

    AdvApp2Var_MathBase::mmveps3_(&eps3);
    if ((d__1 = *u1 - *u0, abs(d__1)) < eps3) {
	*iercod = 13;
	goto L9999;
    }
    *iercod = 0;

/* ********************************************************************** 
*/
/*                CAS OU LE TRAITEMENT NE PEUT ETRE FAIT */
/* ********************************************************************** 
*/
    if (*ncoeff > 61 || *ncoeff < 1) {
	*iercod = 10;
	goto L9999;
    }
/* ********************************************************************** 
*/
/*          SI PAS DE CHANGEMENT DE L' INTERVALLE DE DEFINITION */
/*          (SEULEMENT INVERSION DES INDICES DU TABLEAU CRVOLD) */
/* ********************************************************************** 
*/
    if (*ndim == *ndimax && *u0 == -1. && *u1 == 1.) {
	AdvApp2Var_MathBase::mmcvinv_(ndim, ncoeff, ndim, &crvold[crvold_offset], &crvnew[
		crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*          CAS OU LE NOUVEL INTERVALLE DE DEFINITION EST [0,1] */
/* ********************************************************************** 
*/
    if (*u0 == 0. && *u1 == 1.) {
	mmcvstd_(ncoeff, ndimax, ncoeff, ndim, &crvold[crvold_offset], &
		crvnew[crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                          TRAITEMENT GENERAL */
/* ********************************************************************** 
*/
/* -------------------------- Initialisations --------------------------- 
*/

    x0 = -(*u1 + *u0) / (*u1 - *u0);
    x1 = 2. / (*u1 - *u0);
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	crvnew[nd + crvnew_dim1] = crvold[nd * crvold_dim1 + 1];
/* L100: */
    }
    if (*ncoeff == 1) {
	goto L9999;
    }
    tabaux[0] = x0;
    tabaux[1] = x1;

/* ----------------------- Calcul des coeff. de CRVNEW ------------------ 
*/

    i__1 = *ncoeff - 1;
    for (ncf = 2; ncf <= i__1; ++ncf) {

/* ------------ Prise en compte du NCF-ieme coeff. de CRVOLD --------
---- */

	i__2 = ncf - 1;
	for (ncj = 1; ncj <= i__2; ++ncj) {
	    bid = tabaux[ncj - 1];
	    i__3 = *ndim;
	    for (nd = 1; nd <= i__3; ++nd) {
		crvnew[nd + ncj * crvnew_dim1] += crvold[ncf + nd * 
			crvold_dim1] * bid;
/* L400: */
	    }
/* L300: */
	}

	bid = tabaux[ncf - 1];
	i__2 = *ndim;
	for (nd = 1; nd <= i__2; ++nd) {
	    crvnew[nd + ncf * crvnew_dim1] = crvold[ncf + nd * crvold_dim1] * 
		    bid;
/* L500: */
	}

/* --------- Calcul des (NCF+1) coeff. de [X1*t + X0]**(NCF) --------
---- */

	tabaux[ncf] = tabaux[ncf - 1] * x1;
	for (ncj = ncf; ncj >= 2; --ncj) {
	    tabaux[ncj - 1] = tabaux[ncj - 1] * x0 + tabaux[ncj - 2] * x1;
/* L600: */
	}
	tabaux[0] *= x0;

/* L200: */
    }

/* -------------- Prise en compte du dernier coeff. de CRVOLD ----------- 
*/

    i__1 = *ncoeff - 1;
    for (ncj = 1; ncj <= i__1; ++ncj) {
	bid = tabaux[ncj - 1];
	i__2 = *ndim;
	for (nd = 1; nd <= i__2; ++nd) {
	    crvnew[nd + ncj * crvnew_dim1] += crvold[*ncoeff + nd * 
		    crvold_dim1] * bid;
/* L800: */
	}
/* L700: */
    }
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	crvnew[nd + *ncoeff * crvnew_dim1] = crvold[*ncoeff + nd * 
		crvold_dim1] * tabaux[*ncoeff - 1];
/* L900: */
    }

/* ---------------------------- The end --------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMARCIN", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMARCIN", 7L);
    }
    return 0;
} /* mmarcin_ */

//=======================================================================
//function : mmatvec_
//purpose  : 
//=======================================================================
int mmatvec_(integer *nligne, 
	     integer *,//ncolon,
	     integer *gposit,
	     integer *,//gnstoc, 
	     doublereal *gmatri,
	     doublereal *vecin, 
	     integer *deblig,
	     doublereal *vecout,
	     integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
    static logical ldbg;
  static integer jmin, jmax, i__, j, k;
  static doublereal somme;
  static integer aux;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*      EFFECUE LE PRODUIT MATRICE VECTEUR OU LA MATRICE EST SOUS FORME */
/*      DE PROFIL */


/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, MATRICE, PRODUIT, VECTEUR, PROFIL */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NLIGNE : NOMBRE DE LIGNE DE LA MATRICE DES CONTRAINTES */
/*       NCOLON :NOMBRE DE COLONNE DE LA MATRICE DES CONTRAINTES */
/*       GNSTOC: NOMBRE DE COEFFICIENTS DANS LE PROFILE de la matrice */
/*               GMATRI */

/*       GPOSIT: TABLE DE POSITIONNEMENT DES TERMES DE STOCKAGE */
/*               GPOSIT(1,I) CONTIENT LE NOMBRE DE TERMES-1 SUR LA LIGNE 
*/
/*               I DANS LE PROFIL DE LA MATRICE */
/*              GPOSIT(2,I) CONTIENT L'INDICE DE STOCKAGE DU TERME DIAGONA
L*/
/*               DE LA LIGNE I */
/*               GPOSIT(3,I) CONTIENT L'INDICE COLONE DU PREMIER TERME DU 
*/
/*                           PROFIL DE LA LIGNE I */
/*       GNSTOC: NOMBRE DE COEFFICIENTS DANS LE PROFILE de la matrice */
/*               GMATRI */
/*       GMATRI : MATRICE DES CONTRAINTES SOUS FORME DE PROFIL */
/*       VECIN  : VECTEUR ENTRE */
/*       DEBLIG : INDICE DE LIGNE A PARTIR DUQUEL ON VEUT CALCULER */
/*                LE PRODUIT MATRICE VECTEUR */
/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       VECOUT : VECTEUR PRODUIT */

/*       IERCOD : CODE D'ERREUR */


/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     22-09-95 : ...; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --vecout;
    gposit -= 4;
    --vecin;
    --gmatri;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMATVEC", 7L);
    }
    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */
    AdvApp2Var_SysBase::mvriraz_((integer *)nligne, 
	     (char *)&vecout[1]);
    i__1 = *nligne;
    for (i__ = *deblig; i__ <= i__1; ++i__) {
	somme = 0.;
	jmin = gposit[i__ * 3 + 3];
	jmax = gposit[i__ * 3 + 1] + gposit[i__ * 3 + 3] - 1;
	aux = gposit[i__ * 3 + 2] - gposit[i__ * 3 + 1] - jmin + 1;
	i__2 = jmax;
	for (j = jmin; j <= i__2; ++j) {
	    k = j + aux;
	    somme += gmatri[k] * vecin[j];
	}
	vecout[i__] = somme;
    }





    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

/* ___ DESALLOCATION, ... */

    AdvApp2Var_SysBase::maermsg_("MMATVEC", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMATVEC", 7L);
    }

 return 0 ;
} /* mmatvec_ */

//=======================================================================
//function : mmbulld_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmbulld_(integer *nbcoln, 
				  integer *nblign, 
				  doublereal *dtabtr, 
				  integer *numcle)

{
  /* System generated locals */
  integer dtabtr_dim1, dtabtr_offset, i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static doublereal daux;
  static integer nite1, nite2, nchan, i1, i2;
  
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        TRI PAR BULLE DES COLONNES D'UN TABLEAU D'ENTIER DANS LE SENS */
/*     CROISSANT */

/*     MOTS CLES : */
/*     ----------- */
/*     POINT-ENTREE, TRI, BULLE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       - NBCOLN : NOMBRE DE COLONNES DU TABLEAU */
/*       - NBLIGN : NOMBRE DE LIGNE DU TABLEAU */
/*       - DTABTR : TABLEAU D'ENTIER A TRIER */
/*       - NUMCLE : POSITION DE LA CLE SUR LA COLONNE */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       - DTABTR : TABLEAU TRIE */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     PARTICULIEREMENT PERFORMANT LORSQUE LE TABLEAU EST PRESQUE TRIE */
/*     Dans le cas contraire il vaut mieux utiliser MVSHELD */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     25-09-1995: PMN; ECRITURE VERSION ORIGINALE d'apres MBULLE */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    dtabtr_dim1 = *nblign;
    dtabtr_offset = dtabtr_dim1 + 1;
    dtabtr -= dtabtr_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMBULLD", 7L);
    }
    nchan = 1;
    nite1 = *nbcoln;
    nite2 = 2;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

/* ---->ALGORITHME EN N^2 / 2 ITERATION AU PLUS */

    while(nchan != 0) {

/* ----> PARCOURS DE GAUCHE A DROITE */

	nchan = 0;
	i__1 = nite1;
	for (i1 = nite2; i1 <= i__1; ++i1) {
	    if (dtabtr[*numcle + i1 * dtabtr_dim1] < dtabtr[*numcle + (i1 - 1)
		     * dtabtr_dim1]) {
		i__2 = *nblign;
		for (i2 = 1; i2 <= i__2; ++i2) {
		    daux = dtabtr[i2 + (i1 - 1) * dtabtr_dim1];
		    dtabtr[i2 + (i1 - 1) * dtabtr_dim1] = dtabtr[i2 + i1 * 
			    dtabtr_dim1];
		    dtabtr[i2 + i1 * dtabtr_dim1] = daux;
		}
		if (nchan == 0) {
		    nchan = 1;
		}
	    }
	}
	--nite1;

/* ----> PARCOURS DE DROITE A GAUCHE */

	if (nchan != 0) {
	    nchan = 0;
	    i__1 = nite2;
	    for (i1 = nite1; i1 >= i__1; --i1) {
		if (dtabtr[*numcle + i1 * dtabtr_dim1] < dtabtr[*numcle + (i1 
			- 1) * dtabtr_dim1]) {
		    i__2 = *nblign;
		    for (i2 = 1; i2 <= i__2; ++i2) {
			daux = dtabtr[i2 + (i1 - 1) * dtabtr_dim1];
			dtabtr[i2 + (i1 - 1) * dtabtr_dim1] = dtabtr[i2 + i1 *
				 dtabtr_dim1];
			dtabtr[i2 + i1 * dtabtr_dim1] = daux;
		    }
		    if (nchan == 0) {
			nchan = 1;
		    }
		}
	    }
	    ++nite2;
	}
    }


    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */

/* ----> PAS D'ERREURS EN APPELANT DES FONCTIONS, ON A UNIQUEMENT DES */
/*      TESTS ET DES BOUCLES. */

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMBULLD", 7L);
    }

 return 0 ;
} /* mmbulld_ */


//=======================================================================
//function : AdvApp2Var_MathBase::mmcdriv_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmcdriv_(integer *ndimen, 
				  integer *ncoeff, 
				  doublereal *courbe, 
				  integer *ideriv, 
				  integer *ncofdv, 
				  doublereal *crvdrv)


{
  /* System generated locals */
  integer courbe_dim1, courbe_offset, crvdrv_dim1, crvdrv_offset, i__1, 
  i__2;
  
  /* Local variables */
  static integer i__, j, k;
  static doublereal mfactk, bid;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     CALCUL DE LA MATRICE D'UNE COURBE DERIVEE D' ORDRE IDERIV. */
/*     AVEC PARAMETRES D' ENTRE DISTINCT DES PARAMETRES DE SORTIE. */


/*     MOTS CLES : */
/*     ----------- */
/*     COEFFICIENTS,COURBE,DERIVEE I-EME. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDIMEN   : Dimension de l'espace (2 ou 3 en general) */
/*   NCOEFF  : Le degre +1 de la courbe. */
/*   COURBE  : Tableau des coefficients de la courbe. */
/*   IDERIV : Ordre de derivation demande : 1=derivee 1ere, etc... */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   NCOFDV  : Le degre +1 de la derivee d' ordre IDERIV de la courbe. */
/*   CRVDRV  : Tableau des coefficients de la derivee d' ordre IDERIV */
/*            de la courbe. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* ---> Il est possible de prendre comme argument de sortie la courbe */
/*     et le nombre de coeff passes en entree en faisant : */
/*        CALL MMCDRIV(NDIMEN,NCOEFF,COURBE,IDERIV,NCOEFF,COURBE). */
/*     Apres cet appel, NCOEFF doone le nbre de coeff de la courbe */
/*     derivee dont les coefficients sont stockes dans COURBE. */
/*     Attention alors aux coefficients de COURBE de rang superieur a */
/*     NCOEFF : il ne sont pas mis a zero. */

/* ---> Algorithme : */
/*     Le code ci dessous a ete ecrit a partir de l' algorithme suivant: 
*/

/*     Soit P(t) = a1 + a2*t + ... an*t**n. La derivee d' ordre k de P */
/*     (comportant n-k coefficients) est calculee ainsi : */

/*       Pk(t) = a(k+1)*CNP(k,k)*k! */
/*             + a(k+2)*CNP(k+1,k)*k! * t */
/*             . */
/*             . */
/*             . */
/*             + a(n)*CNP(n-1,k)*k! * t**(n-k-1). */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     09-01-90 : TE ; COMMON MCCNP -> MMCMCNP.INC & INDICES DES CNP */
/*     07-10-88 : RBD; Creation. */
/* > */
/* ***********************************************************************
 */


/* -------------- Cas ou l' ordre de derivee est plus ------------------- 
*/
/* ---------------- grand que le degre de la courbe --------------------- 
*/

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a fournir les coefficients du binome (triangle de Pascal). */

/*     MOTS CLES : */
/*     ----------- */
/*      Coeff du binome de 0 a 60. read only . init par block data */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les coefficients du binome forment une matrice triangulaire. */
/*     On complete cette matrice dans le tableau CNP par sa transposee. */
/*     On a donc: CNP(I,J) = CNP(J,I) pour I et J = 0, ..., 60. */

/*     L'initialisation est faite a partir du block-data MMLLL09.RES, */
/*     cree par le programme MQINICNP.FOR (voir l'equipe (AC) ). */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     03-07-90 : RBD; Ajout commentaires (nom du block-data). */
/*     19-06-90 : RBD; Le commun MMCMCNP remplace MCCNP qui est obsolete. 
*/
/*     08-01-90 : TE ; CREATION */
/* > */
/* ********************************************************************** 
*/



/* ***********************************************************************
 */

    /* Parameter adjustments */
    crvdrv_dim1 = *ndimen;
    crvdrv_offset = crvdrv_dim1 + 1;
    crvdrv -= crvdrv_offset;
    courbe_dim1 = *ndimen;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */
    if (*ideriv >= *ncoeff) {
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    crvdrv[i__ + crvdrv_dim1] = 0.;
/* L10: */
	}
	*ncofdv = 1;
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                         Traitement general */
/* ********************************************************************** 
*/
/* --------------------- Calcul de Factorielle(IDERIV) ------------------ 
*/

    k = *ideriv;
    mfactk = 1.;
    i__1 = k;
    for (i__ = 2; i__ <= i__1; ++i__) {
	mfactk *= i__;
/* L50: */
    }

/* ------------ Calcul des coeff de la derivee d' ordre IDERIV ---------- 
*/
/* ---> Attention : le coefficient binomial C(n,m) est represente dans */
/*                 MCCNP par CNP(N+1,M+1). */

    i__1 = *ncoeff;
    for (j = k + 1; j <= i__1; ++j) {
	bid = mmcmcnp_.cnp[j - 1 + k * 61] * mfactk;
	i__2 = *ndimen;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    crvdrv[i__ + (j - k) * crvdrv_dim1] = bid * courbe[i__ + j * 
		    courbe_dim1];
/* L200: */
	}
/* L100: */
    }

    *ncofdv = *ncoeff - *ideriv;

/* -------------------------------- The end ----------------------------- 
*/

L9999:
    return 0;
} /* mmcdriv_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmcglc1_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmcglc1_(integer *ndimax, 
				  integer *ndimen, 
				  integer *ncoeff, 
				  doublereal *courbe, 
				  doublereal *tdebut, 
				  doublereal *tfinal, 
				  doublereal *epsiln, 
				  doublereal *xlongc, 
				  doublereal *erreur, 
				  integer *iercod)


{
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1;
  doublereal d__1;
  
  /* Local variables */
  static integer ndec;
  static doublereal tdeb, tfin;
  static integer iter;
  static doublereal oldso;
  static integer itmax;
  static doublereal sottc;
  static integer kk, ibb;
  static doublereal dif, pas;
  static doublereal som;
 

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*      Permet de calculer la longueur d'un arc de courbe POLYNOMIAL */
/*      sur un intervalle [A,B] quelconque. */

/*     MOTS CLES : */
/*     ----------- */
/*        LONGUEUR,COURBE,GAUSS,PRIVE. */

/*     ARGUMENTS DD'ENTREE : */
/*     ------------------ */
/*      NDIMAX : Nombre de lignes maximum des tableaux */
/*               (i.e. nbre maxi des polynomes). */
/*      NDIMEN : Dimension de l'espace (nbre de polynomes). */
/*      NCOEFF : Nombre de coefficients du polynome. C'est le degre + 1. 
*/
/*      COURBE(NDIMAX,NCOEFF) : Coefficients de la courbe. */
/*      TDEBUT : Borne inferieure de l'intervalle d'integration pour */
/*               le calcul de la longueur. */
/*      TFINAL : Borne superieure de l'intervalle d'integration pour */
/*               le calcul de la longueur. */
/*      EPSILN : Precision DEMANDEE sur le calcul de la longueur. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*      XLONGC : Longueur de l'arc de courbe */
/*      ERREUR : Precision OBTENUE sur le calcul de la longueur. */
/*      IERCOD : Code d' erreur, 0 OK, >0 Erreur grave. */
/*               = 1 Trop d'iterations, on sort le meilleur resultat */
/*                   calcule (a ERREUR pres) */
/*               = 2 Pb MMLONCV (pas de resultat) */
/*               = 3 NDIM ou NCOEFF invalides (pas de resultat) */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*      Le polynome est en fait un ensemble de polynomes dont les */
/*      coefficients sont ranges dans un tableau a 2 indices, chaque */
/*      ligne etant relative a 1 polynome. */
/*      Le polynome est defini par ses coefficients ordonne par les */
/*      puissances croissantes de la variable. */
/*      Tous les polynomes ont le meme nombre de coefficients (donc le */
/*      meme degre). */

/*      Ce programme annule et remplace LENGCV, MLONGC et MLENCV. */

/*      ATTENTION : si TDEBUT > TFINAL, la longueur est alors NEGATIVE. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     22-04-1991: ALR; ITMAX en dur a 13 */
/*     14-05-1990: RBD; Appel MITERR au lieu de MEPSNR pour ITMAX */
/*     26-04-1990: RBD; Creation. */
/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */


/* ------------------------ Initialisation generale --------------------- 
*/

    /* Parameter adjustments */
    courbe_dim1 = *ndimax;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMCGLC1", 7L);
    }

    *iercod = 0;
    *xlongc = 0.;
    *erreur = 0.;

/* ------ Test d'egalite des bornes */

    if (*tdebut == *tfinal) {
	*iercod = 0;
	goto L9999;
    }

/* ------ Test de la dimension et du nombre de coefficients */

    if (*ndimen <= 0 || *ncoeff <= 0) {
	goto L9003;
    }

/* ------ Nbre de decoupe en cours, nbre d'iteration, */
/*       nbre max d'iterations */

    ndec = 1;
    iter = 1;

/* ALR     NE PAS APPELER DE NOMBRE D ITERATION VENANT */
/*        D'ON NE SAIT OU !! 8 EST MIS EN DUR EXPRES !! */

    itmax = 13;

/* ------ Variation du nombre d'intervalles */
/*       On multiplie par 2 a chaque iteration */

L5000:
    pas = (*tfinal - *tdebut) / ndec;
    sottc = 0.;

/* ------ Boucle sur tous les NDEC intervalles en cours */

    i__1 = ndec;
    for (kk = 1; kk <= i__1; ++kk) {

/* ------ Bornes de l'intervalle d'integration en cours */

	tdeb = *tdebut + (kk - 1) * pas;
	tfin = tdeb + pas;
	mmloncv_(ndimax, ndimen, ncoeff, &courbe[courbe_offset], &tdeb, &tfin,
		 &som, iercod);
	if (*iercod > 0) {
	    goto L9002;
	}

	sottc += som;

/* L100: */
    }


/* ----------------- Test sur le nombre maximum d'iterations ------------ 
*/

/*  Test si passe au moins 1 fois ** */

    if (iter == 1) {
	oldso = sottc;
	ndec <<= 1;
	++iter;
	goto L5000;
    } else {

/* ------ Prise en compte du DIF - Test de convergence */

	++iter;
	dif = (d__1 = sottc - oldso, abs(d__1));

/* ------ Si DIF est OK, on va sortir..., sinon: */

	if (dif > *epsiln) {

/* ------ Si nbre iteration depasse, on sort */

	    if (iter > itmax) {
		*iercod = 1;
		goto L9000;
	    } else {

/* ------ Sinon on continue en decoupant l'intervalle initial.
 */

		oldso = sottc;
		ndec <<= 1;
		goto L5000;
	    }
	}
    }

/* ------------------------------ THE END ------------------------------- 
*/

L9000:
    *xlongc = sottc;
    *erreur = dif;
    goto L9999;

/* ---> PB dans MMLONCV */

L9002:
    *iercod = 2;
    goto L9999;

/* ---> NCOEFF ou NDIM invalides. */

L9003:
    *iercod = 3;
    goto L9999;

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMCGLC1", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMCGLC1", 7L);
    }
    return 0;
} /* mmcglc1_ */

//=======================================================================
//function : mmchole_
//purpose  : 
//=======================================================================
int mmchole_(integer *,//mxcoef, 
	     integer *dimens, 
	     doublereal *amatri, 
	     integer *aposit, 
	     integer *posuiv, 
	     doublereal *chomat, 
	     integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2, i__3;
  doublereal d__1;
  
  /* Builtin functions */
  //double sqrt();
  
    /* Local variables */
  static logical ldbg;
  static integer kmin, i__, j, k;
  static doublereal somme;
  static integer ptini, ptcou;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ----------                                                  T */
/*     Effectue la decomposition de choleski de la matrice A en S.S */
/*     Calcul la matrice triangulaire inferieure S. */

/*     MOTS CLES : */
/*     ----------- */
/*     RESOLUTION, MFACTORISATION, MATRICE_PROFILE, CHOLESKI */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*     MXCOEF : Nombres maximale de termes dans le profile du hessien */
/*     DIMENS : Dimension du probleme */
/*     AMATRI(MXCOEF) : Coefficients du profil de la matrice */
/*        APOSIT(1,*) : Distance diagonnale-extrimite gauche de la ligne 
*/
/*        APOSIT(2,*) : Position des termes diagonnaux dans HESSIE */
/*     POSUIV(MXCOEF): premiere ligne inferieure non hors profil */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*      CHOMAT(MXCOEF) : Matrice triangulaire inferieure qui conserve */
/*                       le profil de AMATRI. */
/*      IERCOD : code d'erreur */
/*               = 0 : ok */
/*               = 1 : Matrice non definie positive */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     NIVEAU DE DEBUG = 4 */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     14-02-1994: PMN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --chomat;
    --posuiv;
    --amatri;
    aposit -= 3;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 4;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMCHOLE", 7L);
    }
    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    i__1 = *dimens;
    for (j = 1; j <= i__1; ++j) {

	ptini = aposit[(j << 1) + 2];

	somme = 0.;
	i__2 = ptini - 1;
	for (k = ptini - aposit[(j << 1) + 1]; k <= i__2; ++k) {
/* Computing 2nd power */
	    d__1 = chomat[k];
	    somme += d__1 * d__1;
	}

	if (amatri[ptini] - somme < 1e-32) {
	    goto L9101;
	}
	chomat[ptini] = sqrt(amatri[ptini] - somme);

	ptcou = ptini;

	while(posuiv[ptcou] > 0) {

	    i__ = posuiv[ptcou];
	    ptcou = aposit[(i__ << 1) + 2] - (i__ - j);

/*           Calcul la somme de S  .S   pour k =1 a j-1 */
/*                               ik  jk */
	    somme = 0.;
/* Computing MAX */
	    i__2 = i__ - aposit[(i__ << 1) + 1], i__3 = j - aposit[(j << 1) + 
		    1];
	    kmin = max(i__2,i__3);
	    i__2 = j - 1;
	    for (k = kmin; k <= i__2; ++k) {
		somme += chomat[aposit[(i__ << 1) + 2] - (i__ - k)] * chomat[
			aposit[(j << 1) + 2] - (j - k)];
	    }

	    chomat[ptcou] = (amatri[ptcou] - somme) / chomat[ptini];
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    AdvApp2Var_SysBase::maermsg_("MMCHOLE", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMCHOLE", 7L);
    }

 return 0 ;
} /* mmchole_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmcvctx_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmcvctx_(integer *ndimen, 
				  integer *ncofmx, 
				  integer *nderiv, 
				  doublereal *ctrtes, 
				  doublereal *crvres, 
				  doublereal *tabaux, 
				  doublereal *xmatri, 
				  integer *iercod)

{
  /* System generated locals */
  integer ctrtes_dim1, ctrtes_offset, crvres_dim1, crvres_offset, 
  xmatri_dim1, xmatri_offset, tabaux_dim1, tabaux_offset, i__1, 
  i__2;
  
  /* Local variables */
  static integer moup1, nordr;
  static integer nd;
  static integer ibb, ncf, ndv;
  static doublereal eps1;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calcul d' une courbe polynomiale verifiant des */
/*        contraintes de passages (interpolation) */
/*        de derivees premieres etc... aux extremites. */
/*        Les parametres aux extremites sont supposes etre -1 et 1. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTES&,INTERPOLATION,&COURBE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NDIMEN : Dimension de l' espace. */
/*     NCOFMX : Nre de coeff. de la courbe CRVRES sur chaque */
/*              dimension. */
/*     NDERIV : Ordre de contrainte aux derivees : */
/*              0 --> interpolation simple. */
/*              1 --> interpolation+contraintes aux derivees 1eres. */
/*              2 --> cas (0)+ (1) +   "         "     "     2emes. */
/*                 etc... */
/*     CTRTES : Tableau des contraintes. */
/*              CTRTES(*,1,*) = contraintes en -1. */
/*              CTRTES(*,2,*) = contraintes en  1. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     CRVRES : La courbe resultat definie dans (-1,1). */
/*     TABAUX : Matrice auxilliaire. */
/*     XMATRI : Matrice auxilliaire. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG         R*8  DFLOAT              MGENMSG */
/*           MGSOMSG              MMEPS1               MMRSLW */
/*      I*4  MNFNDEB */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*        Le polynome (ou la courbe) est calculee en resolvant un */
/*        systeme d' equations lineaires. Si le degre impose est grand */
/*        il est preferable de faire appel a une routine basee sur */
/*        l' interpolation de Lagrange ou d' Hermite suivant le cas. */
/*        (pour un degre eleve la matrice du systeme peut etre mal */
/*        conditionnee). */
/*        Cette routine retourne une courbe definie dans (-1,1). */
/*        Pour un cas general, il faut utiliser MCVCTG. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     18-09-1995 : JMF ; Verfor */
/*     14-02-1990 : RBD ; Correction declaration de NOMPRG. */
/*     12-04-1989 : RBD ; Suppression des chaines de caracteres pour */
/*                        les appel a MMRSLW. */
/*     31-05-1988 : JJM ; Reorganisation contraintes. */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     22-02-1988 : JJM ; Appel GERMSG -> MAERMSG */
/*     24-11-1987 : Cree par RBD. */

/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */


    /* Parameter adjustments */
    crvres_dim1 = *ncofmx;
    crvres_offset = crvres_dim1 + 1;
    crvres -= crvres_offset;
    xmatri_dim1 = *nderiv + 1;
    xmatri_offset = xmatri_dim1 + 1;
    xmatri -= xmatri_offset;
    tabaux_dim1 = *nderiv + 1 + *ndimen;
    tabaux_offset = tabaux_dim1 + 1;
    tabaux -= tabaux_offset;
    ctrtes_dim1 = *ndimen;
    ctrtes_offset = ctrtes_dim1 * 3 + 1;
    ctrtes -= ctrtes_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMCVCTX", 7L);
    }
/*   Les precisions. */
    AdvApp2Var_MathBase::mmeps1_(&eps1);

/* ****************** CALCUL DES COEFFICIENTS PAIRS ********************* 
*/
/* ------------------------- Initialisation ----------------------------- 
*/

    nordr = *nderiv + 1;
    i__1 = nordr;
    for (ncf = 1; ncf <= i__1; ++ncf) {
	tabaux[ncf + tabaux_dim1] = 1.;
/* L100: */
    }

/* ---------------- Calcul des termes correspondants aux derivees ------- 
*/

    i__1 = nordr;
    for (ndv = 2; ndv <= i__1; ++ndv) {
	i__2 = nordr;
	for (ncf = 1; ncf <= i__2; ++ncf) {
	    tabaux[ncf + ndv * tabaux_dim1] = tabaux[ncf + (ndv - 1) * 
		    tabaux_dim1] * (doublereal) ((ncf << 1) - ndv);
/* L300: */
	}
/* L200: */
    }

/* ------------------ Ecriture du deuxieme membre ----------------------- 
*/

    moup1 = 1;
    i__1 = nordr;
    for (ndv = 1; ndv <= i__1; ++ndv) {
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    tabaux[nordr + nd + ndv * tabaux_dim1] = (ctrtes[nd + ((ndv << 1) 
		    + 2) * ctrtes_dim1] + moup1 * ctrtes[nd + ((ndv << 1) + 1)
		     * ctrtes_dim1]) / 2.;
/* L500: */
	}
	moup1 = -moup1;
/* L400: */
    }

/* -------------------- Resolution du systeme --------------------------- 
*/

    mmrslw_(&nordr, &nordr, ndimen, &eps1, &tabaux[tabaux_offset], &xmatri[
	    xmatri_offset], iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = nordr;
	for (ncf = 1; ncf <= i__2; ++ncf) {
	    crvres[(ncf << 1) - 1 + nd * crvres_dim1] = xmatri[ncf + nd * 
		    xmatri_dim1];
/* L700: */
	}
/* L600: */
    }

/* ***************** CALCUL DES COEFFICIENTS IMPAIRS ******************** 
*/
/* ------------------------- Initialisation ----------------------------- 
*/


    i__1 = nordr;
    for (ncf = 1; ncf <= i__1; ++ncf) {
	tabaux[ncf + tabaux_dim1] = 1.;
/* L1100: */
    }

/* ---------------- Calcul des termes correspondants aux derivees ------- 
*/

    i__1 = nordr;
    for (ndv = 2; ndv <= i__1; ++ndv) {
	i__2 = nordr;
	for (ncf = 1; ncf <= i__2; ++ncf) {
	    tabaux[ncf + ndv * tabaux_dim1] = tabaux[ncf + (ndv - 1) * 
		    tabaux_dim1] * (doublereal) ((ncf << 1) - ndv + 1);
/* L1300: */
	}
/* L1200: */
    }

/* ------------------ Ecriture du deuxieme membre ----------------------- 
*/

    moup1 = -1;
    i__1 = nordr;
    for (ndv = 1; ndv <= i__1; ++ndv) {
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    tabaux[nordr + nd + ndv * tabaux_dim1] = (ctrtes[nd + ((ndv << 1) 
		    + 2) * ctrtes_dim1] + moup1 * ctrtes[nd + ((ndv << 1) + 1)
		     * ctrtes_dim1]) / 2.;
/* L1500: */
	}
	moup1 = -moup1;
/* L1400: */
    }

/* -------------------- Resolution du systeme --------------------------- 
*/

    mmrslw_(&nordr, &nordr, ndimen, &eps1, &tabaux[tabaux_offset], &xmatri[
	    xmatri_offset], iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = nordr;
	for (ncf = 1; ncf <= i__2; ++ncf) {
	    crvres[(ncf << 1) + nd * crvres_dim1] = xmatri[ncf + nd * 
		    xmatri_dim1];
/* L1700: */
	}
/* L1600: */
    }

/* --------------------------- The end ---------------------------------- 
*/

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMCVCTX", iercod, 7L);
    }
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMCVCTX", 7L);
    }

 return 0 ;
} /* mmcvctx_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmcvinv_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmcvinv_(integer *ndimax, 
			    integer *ncoef,
			    integer *ndim, 
			    doublereal *curveo, 
			    doublereal *curve)

{
  /* Initialized data */
  
  static char nomprg[8+1] = "MMCVINV ";
  
  /* System generated locals */
  integer curve_dim1, curve_offset, curveo_dim1, curveo_offset, i__1, i__2;
  
  /* Local variables */
  static integer i__, nd, ibb;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Inversion des arguments de la courbe finale. */

/*     MOTS CLES : */
/*     ----------- */
/*        LISSAGE,COURBE */


/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */

/*        NDIM: Dimension de l' espace. */
/*        NCOEF: Degre du polynome. */
/*        CURVEO: La courbe avant inversion. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CURVE: La courbe apres inversion. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB */
/*        15-07-1987: Cree par JJM. */

/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */
    /* Parameter adjustments */
    curve_dim1 = *ndimax;
    curve_offset = curve_dim1 + 1;
    curve -= curve_offset;
    curveo_dim1 = *ncoef;
    curveo_offset = curveo_dim1 + 1;
    curveo -= curveo_offset;

    /* Function Body */

    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_(nomprg, 6L);
    }

    i__1 = *ncoef;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *ndim;
	for (nd = 1; nd <= i__2; ++nd) {
	    curve[nd + i__ * curve_dim1] = curveo[i__ + nd * curveo_dim1];
/* L300: */
	}
    }

/* L9999: */
    return 0;
} /* mmcvinv_ */

//=======================================================================
//function : mmcvstd_
//purpose  : 
//=======================================================================
int mmcvstd_(integer *ncofmx, 
	     integer *ndimax, 
	     integer *ncoeff,
	     integer *ndimen, 
	     doublereal *crvcan, 
	     doublereal *courbe)

{
  /* System generated locals */
  integer courbe_dim1, crvcan_dim1, crvcan_offset, i__1, i__2, i__3;
  
  /* Local variables */
  static integer ndeg, i__, j, j1, nd, ibb;
  static doublereal bid;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Transforme une courbe definie entre [-1,1] a [0,1]. */

/*     MOTS CLES : */
/*     ----------- */
/*        LIMITATION,RESTRICTION,COURBE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMAX : Dimensionnement de l' espace. */
/*        NDIMEN   : Dimension de la courbe. */
/*        NCOEFF : Degre de la courbe. */
/*        CRVCAN(NCOFMX,NDIMEN): La courbe definie entre [-1,1]. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        COURBE(NDIMAX,NCOEFF): La courbe definie dans [0,1]. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     09-01-90 : TE ; COMMON MCCNP -> MMCMCNP.INC & INDICES DES CNP */
/*     12-04-89 : RBD ; Appel MGSOMSG. */
/*     22-02-88 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     19-02-88 : JJM ; Remontee des PARAMETER */
/*     14-01-88 : JJM ; Suppression de MINOMBR */
/*     28-11-86 : Creation JJM. */
/* > */
/* ***********************************************************************
 */

/*   Le nom du programme. */


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a fournir les coefficients du binome (triangle de Pascal). */

/*     MOTS CLES : */
/*     ----------- */
/*      Coeff du binome de 0 a 60. read only . init par block data */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les coefficients du binome forment une matrice triangulaire. */
/*     On complete cette matrice dans le tableau CNP par sa transposee. */
/*     On a donc: CNP(I,J) = CNP(J,I) pour I et J = 0, ..., 60. */

/*     L'initialisation est faite a partir du block-data MMLLL09.RES, */
/*     cree par le programme MQINICNP.FOR (voir l'equipe (AC) ). */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     03-07-90 : RBD; Ajout commentaires (nom du block-data). */
/*     19-06-90 : RBD; Le commun MMCMCNP remplace MCCNP qui est obsolete. 
*/
/*     08-01-90 : TE ; CREATION */
/* > */
/* ********************************************************************** 
*/



/* ***********************************************************************
 */

    /* Parameter adjustments */
    courbe_dim1 = *ndimax;
    --courbe;
    crvcan_dim1 = *ncofmx;
    crvcan_offset = crvcan_dim1;
    crvcan -= crvcan_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMCVSTD", 7L);
    }
    ndeg = *ncoeff - 1;

/* ------------------ Construction de la courbe resultat ---------------- 
*/

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = ndeg;
	for (j = 0; j <= i__2; ++j) {
	    bid = 0.;
	    i__3 = ndeg;
	    for (i__ = j; i__ <= i__3; i__ += 2) {
		bid += crvcan[i__ + nd * crvcan_dim1] * mmcmcnp_.cnp[i__ + j 
			* 61];
/* L410: */
	    }
	    courbe[nd + j * courbe_dim1] = bid;

	    bid = 0.;
	    j1 = j + 1;
	    i__3 = ndeg;
	    for (i__ = j1; i__ <= i__3; i__ += 2) {
		bid += crvcan[i__ + nd * crvcan_dim1] * mmcmcnp_.cnp[i__ + j 
			* 61];
/* L420: */
	    }
	    courbe[nd + j * courbe_dim1] -= bid;
/* L400: */
	}
/* L300: */
    }

/* ------------------- Renormalisation de COURBE -------------------------
 */

    bid = 1.;
    i__1 = ndeg;
    for (i__ = 0; i__ <= i__1; ++i__) {
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    courbe[nd + i__ * courbe_dim1] *= bid;
/* L510: */
	}
	bid *= 2.;
/* L500: */
    }

/* ----------------------------- The end -------------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMCVSTD", 7L);
    }
    return 0;
} /* mmcvstd_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmdrc11_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmdrc11_(integer *iordre, 
				  integer *ndimen, 
				  integer *ncoeff, 
				  doublereal *courbe, 
				  doublereal *points, 
				  doublereal *mfactab)

{
  /* System generated locals */
  integer courbe_dim1, courbe_offset, points_dim2, points_offset, i__1, 
  i__2;
  
  /* Local variables */
  
  static integer ndeg, i__, j, ndgcb, nd, ibb;
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Calcul des derivees successives de l' equation COURBE au */
/*        parametres -1, 1 de l' ordre 0 jusqu' a l' ordre IORDRE */
/*        inclus.Le calcul se fait sans connaitre les coefficients des */
/*        derivees de la courbe. */

/*     MOTS CLES : */
/*     ----------- */
/*        POSITIONNEMENT,EXTREMITES,COURBE,DERIVEE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        IORDRE   : Ordre maximal de calcul des derivees. */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFF  : Nombre de coefficients de la courbe (degre+1). */
/*        COURBE  : Tableau des coefficients de la courbe. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        POINTS    : Tableau des valeurs des derivees successives */
/*                 au parametres -1.D0 et 1.D0. */
/*        MFACTAB : Tableau auxiliaire pour le calcul de factorielle(I). 
*/

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*        Aucun. */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* ---> ATTENTION, les coefficients de la courbe sont ranges */
/*     "A L' ENVERS". */

/* ---> L' algorithme de calcul des derivees est base sur la */
/*     generalisation du schema de Horner : */
/*                          k             2 */
/*          Soit C(t) = uk.t  + ... + u2.t  + u1.t + u0 . */


/*     On pose a0 = uk, b0 = 0, c0 = 0 et pour 1<=j<=k, on calcule : */

/*          aj = a(j-1).x + u(k-j) */
/*          bj = b(j-1).x + a(j-1) */
/*          cj = c(j-1).x + b(j-1) */

/*     On obtient alors : C(x) = ak, C'(x) = bk, C"(x) = 2.ck  . */

/*     L' algorithme se generalise facilement pour le calcul de */

/*               (n) */
/*              C  (x)   . */
/*             --------- */
/*                n! */

/*      Reference : D. KNUTH, "The Art of Computer Programming" */
/*      ---------              Vol. 2/Seminumerical Algorithms */
/*                             Addison-Wesley Pub. Co. (1969) */
/*                             pages 423-425. */


/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     29-01-1990 : RBD ; Correction de l' en-tete, mise au normes. */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     25-11-1987 : Cree par JJM (d' apres MDRCRV). */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

    /* Parameter adjustments */
    points_dim2 = *iordre + 1;
    points_offset = (points_dim2 << 1) + 1;
    points -= points_offset;
    courbe_dim1 = *ncoeff;
    courbe_offset = courbe_dim1;
    courbe -= courbe_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMDRC11", 7L);
    }

    if (*iordre < 0 || *ncoeff < 1) {
	goto L9999;
    }

/* ------------------- Initialisation du tableau POINTS ----------------- 
*/

    ndgcb = *ncoeff - 1;
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	points[(nd * points_dim2 << 1) + 1] = courbe[ndgcb + nd * courbe_dim1]
		;
	points[(nd * points_dim2 << 1) + 2] = courbe[ndgcb + nd * courbe_dim1]
		;
/* L100: */
    }

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = *iordre;
	for (j = 1; j <= i__2; ++j) {
	    points[((j + nd * points_dim2) << 1) + 1] = 0.;
	    points[((j + nd * points_dim2) << 1) + 2] = 0.;
/* L400: */
	}
/* L300: */
    }

/*    Calcul au parametre -1 et 1 */

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	i__2 = ndgcb;
	for (ndeg = 1; ndeg <= i__2; ++ndeg) {
	    for (i__ = *iordre; i__ >= 1; --i__) {
		points[((i__ + nd * points_dim2) << 1) + 1] = -points[((i__ + nd 
			* points_dim2) << 1) + 1] + points[((i__ - 1 + nd * 
			points_dim2) << 1) + 1];
		points[((i__ + nd * points_dim2) << 1) + 2] += points[((i__ - 1 
			+ nd * points_dim2) << 1) + 2];
/* L800: */
	    }
	    points[(nd * points_dim2 << 1) + 1] = -points[(nd * points_dim2 <<
		     1) + 1] + courbe[ndgcb - ndeg + nd * courbe_dim1];
	    points[(nd * points_dim2 << 1) + 2] += courbe[ndgcb - ndeg + nd * 
		    courbe_dim1];
/* L700: */
	}
/* L600: */
    }

/* --------------------- Multiplication par factorielle(I) -------------- 
*/

    if (*iordre > 1) {
	mfac_(&mfactab[1], iordre);

	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = *iordre;
	    for (i__ = 2; i__ <= i__2; ++i__) {
		points[((i__ + nd * points_dim2) << 1) + 1] = mfactab[i__] * 
			points[((i__ + nd * points_dim2) << 1) + 1];
		points[((i__ + nd * points_dim2) << 1) + 2] = mfactab[i__] * 
			points[((i__ + nd * points_dim2) << 1) + 2];
/* L1000: */
	    }
/* L900: */
	}
    }

/* ---------------------------- Fin ------------------------------------- 
*/

L9999:
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMDRC11", 7L);
    }
    return 0;
} /* mmdrc11_ */

//=======================================================================
//function : mmdrvcb_
//purpose  : 
//=======================================================================
int mmdrvcb_(integer *ideriv,
	     integer *ndim, 
	     integer *ncoeff,
	     doublereal *courbe, 
	     doublereal *tparam,
	     doublereal *tabpnt, 
	     integer *iercod)

{
  /* System generated locals */
  integer courbe_dim1, tabpnt_dim1, i__1, i__2, i__3;
  
  /* Local variables */
  static integer ndeg, i__, j, nd, ndgcrb, iptpnt, ibb;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*      Calcul des derivees successives de l' equation COURBE au */
/*      parametre TPARAM de l' ordre 0 jusqu' a l' ordre IDERIV inclus. */
/*      Le calcul se fait sans utiliser les coefficients des */
/*      derivees de COURBE. */

/*     MOTS CLES : */
/*     ----------- */
/*      POSITIONNEMENT,PARAMETRE,COURBE,DERIVEE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*      IDERIV : Ordre maximal de calcul des derivees. */
/*      NDIM   : Dimension de l' espace. */
/*      NCOEFF : Nombre de coefficients de la courbe (degre+1). */
/*      COURBE : Tableau des coefficients de la courbe. */
/*      TPARAM : Valeur du parametre ou la courbe doit etre evaluee. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*      TABPNT : Tableau des valeurs des derivees successives */
/*               au parametre TPARAM. */
/*      IERCOD : 0 = OK, */
/*               1 = Entrees incoherentes. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*        Aucun. */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     L' algorithme de calcul des derivees est base sur la */
/*     generalisation du schema de Horner : */
/*                          k             2 */
/*          Soit C(t) = uk.t  + ... + u2.t  + u1.t + u0 . */


/*     On pose a0 = uk, b0 = 0, c0 = 0 et pour 1<=j<=k, on calcule : */

/*          aj = a(j-1).x + u(k-j) */
/*          bj = b(j-1).x + a(j-1) */
/*          cj = c(j-1).x + b(j-1) */

/*     On obtient alors : C(x) = ak, C'(x) = bk, C"(x) = 2.ck  . */

/*     L' algorithme se generalise facilement pour le calcul de */

/*               (n) */
/*              C  (x)   . */
/*             --------- */
/*                n! */

/*      Reference : D. KNUTH, "The Art of Computer Programming" */
/*      ---------              Vol. 2/Seminumerical Algorithms */
/*                             Addison-Wesley Pub. Co. (1969) */
/*                             pages 423-425. */

/* ----> Pour evaluer les derivees en 0 et en 1, il est preferable */
/*      d' utiliser la routine MDRV01.FOR . */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     28-06-1988 : Cree par RBD. */

/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

    /* Parameter adjustments */
    tabpnt_dim1 = *ndim;
    --tabpnt;
    courbe_dim1 = *ndim;
    --courbe;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMDRVCB", 7L);
    }

    if (*ideriv < 0 || *ncoeff < 1) {
	*iercod = 1;
	goto L9999;
    }
    *iercod = 0;

/* ------------------- Initialisation du tableau TABPNT ----------------- 
*/

    ndgcrb = *ncoeff - 1;
    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
	tabpnt[nd] = courbe[nd + ndgcrb * courbe_dim1];
/* L100: */
    }

    if (*ideriv < 1) {
	goto L200;
    }
    iptpnt = *ndim * *ideriv;
    AdvApp2Var_SysBase::mvriraz_((integer *)&iptpnt, 
	     (char *)&tabpnt[tabpnt_dim1 + 1]);
L200:

/* ------------------------ Calcul au parametre TPARAM ------------------ 
*/

    i__1 = ndgcrb;
    for (ndeg = 1; ndeg <= i__1; ++ndeg) {
	i__2 = *ndim;
	for (nd = 1; nd <= i__2; ++nd) {
	    for (i__ = *ideriv; i__ >= 1; --i__) {
		tabpnt[nd + i__ * tabpnt_dim1] = tabpnt[nd + i__ * 
			tabpnt_dim1] * *tparam + tabpnt[nd + (i__ - 1) * 
			tabpnt_dim1];
/* L700: */
	    }
	    tabpnt[nd] = tabpnt[nd] * *tparam + courbe[nd + (ndgcrb - ndeg) * 
		    courbe_dim1];
/* L600: */
	}
/* L500: */
    }

/* --------------------- Multiplication par factorielle(I) ------------- 
*/

    i__1 = *ideriv;
    for (i__ = 2; i__ <= i__1; ++i__) {
	i__2 = i__;
	for (j = 2; j <= i__2; ++j) {
	    i__3 = *ndim;
	    for (nd = 1; nd <= i__3; ++nd) {
		tabpnt[nd + i__ * tabpnt_dim1] = (doublereal) j * tabpnt[nd + 
			i__ * tabpnt_dim1];
/* L1200: */
	    }
/* L1100: */
	}
/* L1000: */
    }

/* --------------------------- The end --------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMDRVCB", iercod, 7L);
    }
    return 0;
} /* mmdrvcb_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmdrvck_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmdrvck_(integer *ncoeff, 
				  integer *ndimen, 
				  doublereal *courbe, 
				  integer *ideriv, 
				  doublereal *tparam, 
				  doublereal *pntcrb)

{
  /* Initialized data */
  
  static doublereal mmfack[21] = { 1.,2.,6.,24.,120.,720.,5040.,40320.,
	    362880.,3628800.,39916800.,479001600.,6227020800.,87178291200.,
	    1.307674368e12,2.0922789888e13,3.55687428096e14,6.402373705728e15,
	    1.21645100408832e17,2.43290200817664e18,5.109094217170944e19 };
  
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1, i__2;
  
  /* Local variables */
  static integer i__, j, k, nd;
  static doublereal mfactk, bid;
  

/*      IMPLICIT INTEGER (I-N) */
/*      IMPLICIT DOUBLE PRECISION(A-H,O-Z) */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     CALCUL DE LA VALEUR D'UNE COURBE DERIVEE D' ORDRE IDERIV EN */
/*     UN POINT DE PARAMETRE TPARAM. */

/*     MOTS CLES : */
/*     ----------- */
/*     POSITIONNEMENT,COURBE,DERIVEE D' ORDRE K. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NCOEFF  : Le degre +1 de la courbe. */
/*   NDIMEN   : Dimension de l'espace (2 ou 3 en general) */
/*   COURBE  : Tableau des coefficients de la courbe. */
/*   IDERIV : Ordre de derivation demande : 1=derivee 1ere, etc... */
/*   TPARAM : Valeur du parametre de la courbe. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   PNTCRB  : Le point de parametre TPARAM sur la derivee d' ordre */
/*            IDERIV de COURBE. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*    MMCMCNP */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*      .Neant. */
/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*    Le code ci dessous a ete ecrit a partir de l' algorithme suivant : 
*/

/*    Soit P(t) = a1 + a2*t + ... an*t**n. La derivee d' ordre k de P */
/*    (comportant n-k coefficients) est calculee ainsi : */

/*       Pk(t) = a(k+1)*CNP(k,k)*k! */
/*             + a(k+2)*CNP(k+1,k)*k! * t */
/*             . */
/*             . */
/*             . */
/*             + a(n)*CNP(n-1,k)*k! * t**(n-k-1). */

/*    L' evaluation se fait suivant un schema de Horner classique. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*      8-09-1995 : JMF ; Performance */
/*     09-01-90 : TE ; COMMON MCCNP -> MMCMCNP.INC & INDICES DES CNP */
/*     06-07-88 : RBD; Creation, sur une idee de GD. */
/* > */
/* ***********************************************************************
 */


/*     Factorielles (1 a 21)  caculees  sur VAX en R*16 */


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a fournir les coefficients du binome (triangle de Pascal). */

/*     MOTS CLES : */
/*     ----------- */
/*      Coeff du binome de 0 a 60. read only . init par block data */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les coefficients du binome forment une matrice triangulaire. */
/*     On complete cette matrice dans le tableau CNP par sa transposee. */
/*     On a donc: CNP(I,J) = CNP(J,I) pour I et J = 0, ..., 60. */

/*     L'initialisation est faite a partir du block-data MMLLL09.RES, */
/*     cree par le programme MQINICNP.FOR (voir l'equipe (AC) ). */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     03-07-90 : RBD; Ajout commentaires (nom du block-data). */
/*     19-06-90 : RBD; Le commun MMCMCNP remplace MCCNP qui est obsolete. 
*/
/*     08-01-90 : TE ; CREATION */
/* > */
/* ********************************************************************** 
*/



/* ***********************************************************************
 */

    /* Parameter adjustments */
    --pntcrb;
    courbe_dim1 = *ndimen;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */

/* -------------- Cas ou l' ordre de derivee est plus ------------------- 
*/
/* ---------------- grand que le degre de la courbe --------------------- 
*/

    if (*ideriv >= *ncoeff) {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    pntcrb[nd] = 0.;
/* L100: */
	}
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                         Traitement general */
/* ********************************************************************** 
*/
/* --------------------- Calcul de Factorielle(IDERIV) ------------------ 
*/

    k = *ideriv;
    if (*ideriv <= 21 && *ideriv > 0) {
	mfactk = mmfack[k - 1];
    } else {
	mfactk = 1.;
	i__1 = k;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    mfactk *= i__;
/* L200: */
	}
    }

/* ------- Calcul de la derivee d' ordre IDERIV de COURBE en TPARAM ----- 
*/
/* ---> Attention : le coefficient binomial C(n,m) est represente dans */
/*                 MCCNP par CNP(N,M). */

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	pntcrb[nd] = courbe[nd + *ncoeff * courbe_dim1] * mmcmcnp_.cnp[*
		ncoeff - 1 + k * 61] * mfactk;
/* L300: */
    }

    i__1 = k + 1;
    for (j = *ncoeff - 1; j >= i__1; --j) {
	bid = mmcmcnp_.cnp[j - 1 + k * 61] * mfactk;
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    pntcrb[nd] = pntcrb[nd] * *tparam + courbe[nd + j * courbe_dim1] *
		     bid;
/* L500: */
	}
/* L400: */
    }

/* -------------------------------- The end ----------------------------- 
*/

L9999:

 return 0   ;

} /* mmdrvck_ */
//=======================================================================
//function : AdvApp2Var_MathBase::mmeps1_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmeps1_(doublereal *epsilo)
     
{
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Extraction du EPS1 du COMMON MPRCSN. EPS1 est le zero spatial */
/*     egal a 1.D-9 */

/*     MOTS CLES : */
/*     ----------- */
/*        MPRCSN,PRECISON,EPS1. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        Neant */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        EPSILO : Valeur de EPS1 (Le zero spatial (10**-9)) */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     EPS1 est le zero spatial ABSOLU , c.a.d. que l' on doit */
/*     l' utiliser chaque fois que l' on veut tester si une variable */
/*     est nulle. Par exemple, si la norme d' un vecteur est inferieure */
/*     a EPS1, c' est que ce vecteur est NUL ! (lorsqu' on travaille en */
/*     REAL*8) Il est vivement deconseille de tester des arguments par */
/*     rapport a EPS1**2. Vu les erreurs d' arrondis inevitables lors */
/*     des calculs, cela revient a tester par rapport a 0.D0. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     29-01-90 : DH ; Nettoyage */
/*     27-07-88 : RBD; Ajouts de commentaires. */
/*     29-10-87 : Cree par JJM. */
/* > */
/* ***********************************************************************
 */



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*          DONNE LES TOLERANCES DE NULLITE DANS STRIM */
/*          AINSI QUE LES BORNES DES PROCESSUS ITERATIFS */

/*          CONTEXTE GENERAL, MODIFIABLE PAR L'UTILISATEUR */

/*     MOTS CLES : */
/*     ----------- */
/*          PARAMETRE , TOLERANCE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  PROFIL , **VIA MPRFTX** A L' ENTREE DANS STRI
M*/

/*       CHARGEMENT DES VALEURS PAR DEFAUT DU PROFIL DANS MPRFTX A L'ENTRE
E*/
/*        DANS STRIM. ELLES SONT CONSERVEES DANS DES VARIABLES LOCALES */
/*        DE MPRFTX */

/*        REMISE DES VALEURS PAR DEFAUT                  : MDFINT */
/*        MODIFICATION INTERACTIVE  PAR L'UTILISATEUR    : MDBINT */

/*        FONCTION D'ACCES :  MMEPS1   ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*      01-02-90 : NAK  ; ENTETE */
/* > */
/* ***********************************************************************
 */

/*     NITERM : NB D'ITERATIONS MAXIMAL */
/*     NITERR : NB D'ITERATIONS RAPIDES */
/*     EPS1   : TOLERANCE DE DISTANCE 3D NULLE */
/*     EPS2   : TOLERANCE DE DISTANCE PARAMETRIQUE NULLE */
/*     EPS3   : TOLERANCE POUR EVITER LES DIVISIONS PAR 0.. */
/*     EPS4   : TOLERANCE ANGULAIRE */



/* ***********************************************************************
 */
    *epsilo = mmprcsn_.eps1;

 return 0 ;
} /* mmeps1_ */

//=======================================================================
//function : mmexthi_
//purpose  : 
//=======================================================================
int mmexthi_(integer *ndegre, 
	     doublereal *hwgaus)

{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer iadd, ideb, ndeg2, nmod2, ii, ibb;
  static integer kpt;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Extrait du commun LDGRTL les poids des formules de quadrature de */
/*  Gauss sur toutes les racines des polynomes de Legendre de degre */
/*  NDEGRE defini sur [-1,1]. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::COMMON&, EXTRACTION, &POIDS, &GAUSS. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDEGRE : Degre mathematique du polynome de Legendre. On doit avoir */
/*            2 <= NDEGRE <= 61. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   HWGAUS : Le tableau des poids des formules de quadrature de Gauss */
/*            relatifs aux NDEGRE racines d' un polynome de Legendre de */
/*            degre NDEGRE. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*     MLGDRTL */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: La condition sur NDEGRE ( 2 <= NDEGRE <= 61) n'est */
/*     pas testee. A l'appelant de faire le test. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/* 23-03-90 : RBD ; Mise a jour en-tete, declaration variables locales, */
/*                  correction poids associe racines negatives (bug */
/*                  ENORME). */
/* 15-01-90 : NAK ; MLGDRTL PAR INCLUDE MMLGDRT */
/* 22-04-88 : JJM ; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */


/*   Le common MLGDRTL: */
/*   Ce common comprend les racines POSITIVES des polynomes de Legendre */
/*   ET les poids des formules de quadrature de Gauss sur toutes les */
/*   racines POSITIVES des polynomes de Legendre. */



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*   Le common des racines de Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*        BASE LEGENDRE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     11-01-90 : NAK  ; Creation version originale */
/* > */
/* ***********************************************************************
 */




/*   ROOTAB : Tableau de toutes les racines des polynomes de Legendre */
/*   comprises entre ]0,1]. Elles sont rangees pour des degres croissants 
*/
/*   de 2 a 61. */
/*   HILTAB : Tableau des interpolants de Legendre concernant ROOTAB. */
/*   L' adressage est le meme. */
/*   HI0TAB : Tableau des interpolants de Legendre pour la racine x=0 */
/*   des polynomes de degre IMPAIR. */
/*   RTLTB0 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre PAIR. */
/*   RTLTB1 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre IMPAIR. */


/************************************************************************
*****/
    /* Parameter adjustments */
    --hwgaus;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMEXTHI", 7L);
    }

    ndeg2 = *ndegre / 2;
    nmod2 = *ndegre % 2;

/*   Adresse du poids de Gauss associe a la 1ere racine strictement */
/*   positive du polynome de Legendre de degre NDEGRE dans MLGDRTL. */

    iadd = ndeg2 * (ndeg2 - 1) / 2 + 1;

/*   Indice du 1er element de HWGAUS associe a la 1ere racine */
/*   strictement positive du polynome de Legendre de degre NDEGRE. */

    ideb = (*ndegre + 1) / 2 + 1;

/*   Lecture des poids associes aux racines strictement positives. */

    i__1 = *ndegre;
    for (ii = ideb; ii <= i__1; ++ii) {
	kpt = iadd + ii - ideb;
	hwgaus[ii] = mlgdrtl_.hiltab[kpt + nmod2 * 465 - 1];
/* L100: */
    }

/*   Pour les racines strictement negatives, les poids sont les memes. */
/*   i.e HW(1) = HW(NDEGRE), HW(2) = HW(NDEGRE-1), etc... */

    i__1 = ndeg2;
    for (ii = 1; ii <= i__1; ++ii) {
	hwgaus[ii] = hwgaus[*ndegre + 1 - ii];
/* L200: */
    }

/*   Cas NDEGRE impair, 0 est racine du polynome de Legendre, on */
/*   charge le poids de Gauss associe. */

    if (nmod2 == 1) {
	hwgaus[ndeg2 + 1] = mlgdrtl_.hi0tab[ndeg2];
    }

/* --------------------------- The end ---------------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMEXTHI", 7L);
    }
    return 0;
} /* mmexthi_ */

//=======================================================================
//function : mmextrl_
//purpose  : 
//=======================================================================
int mmextrl_(integer *ndegre,
	     doublereal *rootlg)
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer iadd, ideb, ndeg2, nmod2, ii, ibb;
  static integer kpt;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/* Extrait du Common LDGRTL les racines du polynome de Legendre */
/* de degre NDEGRE defini sur [-1,1]. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::COMMON&, EXTRACTION, &RACINE, &LEGENDRE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NDEGRE : Degre mathematique du polynome de Legendre. On doit avoir */
/*            2 <= NDEGRE <= 61. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   ROOTLG : Le tableau des racines du polynome de Legendre de degre */
/*            NDEGRE et defini sur [-1,1]. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*     MLGDRTL */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: La condition sur NDEGRE ( 2 <= NDEGRE <= 61) n'est */
/*     pas testee. A l'appelant de faire le test. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     23-03-90 : RBD ; Ajout commentaires + declarations. */
/*     15-01-90 : NAK ; MLGDRTL PAR INCLUDE MMLGDRT */
/*     04-03-88 : JJM ; Raccoursissement de MLGDRTL. */
/*     22-02-88 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     23-10-87 : JJM ; Cree par JJM */
/* > */
/* ********************************************************************** 
*/


/*   Le nom de la routine */


/*   Le common MLGDRTL: */
/*   Ce common comprend les racines POSITIVES des polynomes de Legendre */
/*   ET les poids des formules de quadrature de Gauss sur toutes les */
/*   racines POSITIVES des polynomes de Legendre. */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*   Le common des racines de Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*        BASE LEGENDRE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     11-01-90 : NAK  ; Creation version originale */
/* > */
/* ***********************************************************************
 */




/*   ROOTAB : Tableau de toutes les racines des polynomes de Legendre */
/*   comprises entre ]0,1]. Elles sont rangees pour des degres croissants 
*/
/*   de 2 a 61. */
/*   HILTAB : Tableau des interpolants de Legendre concernant ROOTAB. */
/*   L' adressage est le meme. */
/*   HI0TAB : Tableau des interpolants de Legendre pour la racine x=0 */
/*   des polynomes de degre IMPAIR. */
/*   RTLTB0 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre PAIR. */
/*   RTLTB1 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre IMPAIR. */


/************************************************************************
*****/
    /* Parameter adjustments */
    --rootlg;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMEXTRL", 7L);
    }

    ndeg2 = *ndegre / 2;
    nmod2 = *ndegre % 2;

/*   Adresse de la 1ere racine strictement positive du polynome de */
/*   Legendre de degre NDEGRE dans MLGDRTL. */

    iadd = ndeg2 * (ndeg2 - 1) / 2 + 1;

/*   Indice, dans ROOTLG, de la 1ere racine strictement positive du */
/*   polynome de Legendre de degre NDEGRE. */

    ideb = (*ndegre + 1) / 2 + 1;

/*   Lecture des racines strictement positives. */

    i__1 = *ndegre;
    for (ii = ideb; ii <= i__1; ++ii) {
	kpt = iadd + ii - ideb;
	rootlg[ii] = mlgdrtl_.rootab[kpt + nmod2 * 465 - 1];
/* L100: */
    }

/*   Les racines strictement negatives sont egales aux racines positives 
*/
/*   au signe pres i.e RT(1) = -RT(NDEGRE), RT(2) = -RT(NDEGRE-1), etc... 
*/

    i__1 = ndeg2;
    for (ii = 1; ii <= i__1; ++ii) {
	rootlg[ii] = -rootlg[*ndegre + 1 - ii];
/* L200: */
    }

/*   Cas NDEGRE impair, 0 est racine du polynome de Legendre. */

    if (nmod2 == 1) {
	rootlg[ndeg2 + 1] = 0.;
    }

/* -------------------------------- THE END ----------------------------- 
*/

    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMEXTRL", 7L);
    }
    return 0;
} /* mmextrl_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmfmca8_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmfmca8_(integer *ndimen,
				  integer *ncoefu,
				  integer *ncoefv,
				  integer *ndimax, 
				  integer *ncfumx, 
				  integer *,//ncfvmx, 
				  doublereal *tabini,
				  doublereal *tabres)

{
  /* System generated locals */
  integer tabini_dim1, tabini_dim2, tabini_offset, tabres_dim1, tabres_dim2,
  tabres_offset;

  /* Local variables */
  static integer i__, j, k, ilong;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Expansion d' un tableau ne contenant que l' essentiel */
/*        en un tableau de donnees plus grand. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, MATH_ACCES:: CARREAU&, DECOMPRESSION, &CARREAU */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN: Dimension de l' espace de travail. */
/*        NCOEFU: Le degre +1 du tableau en u. */
/*        NCOEFV: Le degre +1 du tableau en v. */
/*        NDIMAX: Dimension maxi de l' espace. */
/*        NCFUMX: Degre maximal +1 du tableau en u. */
/*        NCFVMX: Degre maximal +1 du tableau en v. */
/*        TABINI: Le tableau a decompacter. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        TABRES: Le tableau decompacte. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     L' appel suivant : */

/*  CALL MMFMCA8(NDIMEN,NCOEFU,NCOEFV,NDIMAX,NCFUMX,NCFVMX,TABINI,TABINI) 
*/

/*     ou TABINI est un argument d' entree/sortie, est possible pourvu */
/*     que l' appelant ait declare TABINI en (NDIMAX,NCFUMX,NCFVMX) */

/*     ATTENTION : on ne verifie pas que NDIMAX >= NDIMEN, */
/*                 NCOEFU >= NCFMXU et NCOEFV >= NCFMXV. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     03-08-1989 : RBD; Creation */
/* > */
/* ********************************************************************** 
*/


    /* Parameter adjustments */
    tabini_dim1 = *ndimen;
    tabini_dim2 = *ncoefu;
    tabini_offset = tabini_dim1 * (tabini_dim2 + 1) + 1;
    tabini -= tabini_offset;
    tabres_dim1 = *ndimax;
    tabres_dim2 = *ncfumx;
    tabres_offset = tabres_dim1 * (tabres_dim2 + 1) + 1;
    tabres -= tabres_offset;

    /* Function Body */
    if (*ndimax == *ndimen) {
	goto L1000;
    }

/* ----------------------- decompression NDIMAX<>NDIMEN ----------------- 
*/

    for (k = *ncoefv; k >= 1; --k) {
	for (j = *ncoefu; j >= 1; --j) {
	    for (i__ = *ndimen; i__ >= 1; --i__) {
		tabres[i__ + (j + k * tabres_dim2) * tabres_dim1] = tabini[
			i__ + (j + k * tabini_dim2) * tabini_dim1];
/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }
    goto L9999;

/* ----------------------- decompression NDIMAX=NDIMEN ------------------ 
*/

L1000:
    if (*ncoefu == *ncfumx) {
	goto L2000;
    }
    ilong = (*ndimen << 3) * *ncoefu;
    for (k = *ncoefv; k >= 1; --k) {
	AdvApp2Var_SysBase::mcrfill_((integer *)&ilong, 
		 (char *)&tabini[(k * tabini_dim2 + 1) * tabini_dim1 + 1], 
		 (char *)&tabres[(k * tabres_dim2 + 1) * tabres_dim1 + 1]);
/* L500: */
    }
    goto L9999;

/* ----------------- decompression NDIMAX=NDIMEN,NCOEFU=NCFUMX ---------- 
*/

L2000:
    ilong = (*ndimen << 3) * *ncoefu * *ncoefv;
    AdvApp2Var_SysBase::mcrfill_((integer *)&ilong, 
	     (char *)&tabini[tabini_offset], 
	     (char *)&tabres[tabres_offset]);
    goto L9999;

/* ---------------------------- The end --------------------------------- 
*/

L9999:
    return 0;
} /* mmfmca8_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmfmca9_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmfmca9_(integer *ndimax, 
				   integer *ncfumx, 
				   integer *,//ncfvmx, 
				   integer *ndimen, 
				   integer *ncoefu, 
				   integer *ncoefv, 
				   doublereal *tabini, 
				   doublereal *tabres)

{
  /* System generated locals */
  integer tabini_dim1, tabini_dim2, tabini_offset, tabres_dim1, tabres_dim2,
  tabres_offset, i__1, i__2, i__3;
  
    /* Local variables */
  static integer i__, j, k, ilong;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Compression d' un tableau de donnees en un tableau ne */
/*        contenant que l' essentiel (Le tableau d' entree n' est */
/*        pas ecrase). */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, MATH_ACCES:: CARREAU&, COMPRESSION, &CARREAU */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMAX: Dimension maxi de l' espace. */
/*        NCFUMX: Degre maximal +1 du tableau en u. */
/*        NCFVMX: Degre maximal +1 du tableau en v. */
/*        NDIMEN: Dimension de l' espace de travail. */
/*        NCOEFU: Le degre +1 du tableau en u. */
/*        NCOEFV: Le degre +1 du tableau en v. */
/*        TABINI: Le tableau a compacter. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        TABRES: Le tableau compacte. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     L' appel suivant : */

/* CALL MMFMCA9(NDIMAX,NCFUMX,NCFVMX,NDIMEN,NCOEFU,NCOEFV,TABINI,TABINI) 
*/

/*     ou TABINI est un argument d' entree/sortie, est possible pourvu */
/*     que l' appelant ait bien verifie que : */

/*            NDIMAX > NDIMEN, */
/*         ou NDIMAX = NDIMEN et NCFUMX > NCOEFU */
/*         ou NDIMAX = NDIMEN, NCFUMX = NCOEFU et NCFVMX > NCOEFV */

/*     Ces conditions ne sont pas testees dans le programme. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     18-01-199O : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/


    /* Parameter adjustments */
    tabini_dim1 = *ndimax;
    tabini_dim2 = *ncfumx;
    tabini_offset = tabini_dim1 * (tabini_dim2 + 1) + 1;
    tabini -= tabini_offset;
    tabres_dim1 = *ndimen;
    tabres_dim2 = *ncoefu;
    tabres_offset = tabres_dim1 * (tabres_dim2 + 1) + 1;
    tabres -= tabres_offset;

    /* Function Body */
    if (*ndimen == *ndimax) {
	goto L1000;
    }

/* ----------------------- Compression NDIMEN<>NDIMAX ------------------- 
*/

    i__1 = *ncoefv;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *ncoefu;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = *ndimen;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		tabres[i__ + (j + k * tabres_dim2) * tabres_dim1] = tabini[
			i__ + (j + k * tabini_dim2) * tabini_dim1];
/* L300: */
	    }
/* L200: */
	}
/* L100: */
    }
    goto L9999;

/* ----------------------- Compression NDIMEN=NDIMAX -------------------- 
*/

L1000:
    if (*ncoefu == *ncfumx) {
	goto L2000;
    }
    ilong = (*ndimen << 3) * *ncoefu;
    i__1 = *ncoefv;
    for (k = 1; k <= i__1; ++k) {
	AdvApp2Var_SysBase::mcrfill_((integer *)&ilong, 
		 (char *)&tabini[(k * tabini_dim2 + 1) * tabini_dim1 + 1], 
		 (char *)&tabres[(k * tabres_dim2 + 1) * tabres_dim1 + 1]);
/* L500: */
    }
    goto L9999;

/* ----------------- Compression NDIMEN=NDIMAX,NCOEFU=NCFUMX ------------ 
*/

L2000:
    ilong = (*ndimen << 3) * *ncoefu * *ncoefv;
    AdvApp2Var_SysBase::mcrfill_((integer *)&ilong,
	     (char *)&tabini[tabini_offset], 
	     (char *)&tabres[tabres_offset]);
    goto L9999;

/* ---------------------------- The end --------------------------------- 
*/

L9999:
    return 0;
} /* mmfmca9_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmfmcar_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmfmcar_(integer *ndimen,
				  integer *ncofmx, 
				  integer *ncoefu, 
				  integer *ncoefv, 
				  doublereal *patold, 
				  doublereal *upara1, 
				  doublereal *upara2, 
				  doublereal *vpara1, 
				  doublereal *vpara2, 
				  doublereal *patnew, 
				  integer *iercod)

{
  static integer c__8 = 8;
  /* System generated locals */
    integer patold_dim1, patold_dim2, patnew_dim1, patnew_dim2,
	    i__1, patold_offset,patnew_offset;

    /* Local variables */
    static doublereal tbaux[1];
    static integer ksize, numax, kk;
    static long int iofst;
    static integer ibb, ier;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       LIMITATION D'UN CARREAU DEFINI SUR (0,1)*(0,1) ENTRE LES ISOS */
/*       UPARA1 ET UPARA2 (EN U) ET VPARA1 ET VPARA2 EN V. */

/*     MOTS CLES : */
/*     ----------- */
/*       LIMITATION , CARREAU , PARAMETRE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NCOFMX: NBRE MAXI DE COEFF EN U DU CARREAU */
/*     NCOEFU: NBRE DE COEFF EN U DU CARREAU */
/*     NCOEFV: NBRE DE COEFF EN V DU CARREAU */
/*    PATOLD : LE CARREAU A LIMITER SUIVANT UPARA1,UPARA2 ET VPARA1,VPARA2
.*/
/*     UPARA1    : BORNE INF DES U */
/*     UPARA2    : BORNE SUP DES U */
/*     VPARA1    : BORNE INF DES V */
/*     VPARA2    : BORNE SUP DES V */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     PATNEW : LE CARREAU RELIMITE, DEFINI DANS (0,1)**2 */
/*     IERCOD : =10 NBR DE COEFF TROP GRAND OU NUL */
/*              =13 PB DANS L' ALLOCATION DYNAMIQUE */
/*              = 0 OK. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/* --->    L' appel suivant : */
/*   CALL MMFMCAR(NCOFMX,NCOEFU,NCOEFV,PATOLD,UPARA1,UPARA2,VPARA1,VPARA2 
*/
/*              ,PATOLD), */
/*        ou PATOLD est un argument d' entree/sortie est tout a fait */
/*        legal. */

/* --->    Le nombre maximum de coeff en u et en v de PATOLD est 61 */

/* --->    Si NCOEFU < NCOFMX, on compresse les donnees par MMFMCA9 avant 
*/
/*        la limitation en v pour gagner du temps lors de l' execution */
/*        de MMARC41 qui suit (le carreau est traite comme une courbe de 
*/
/*        dimension NDIMEN*NCOEFU possedant NCOEFV coefficients). */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*        02-08-89 : RBD; CREATION. */
/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */


    /* Parameter adjustments */
    patnew_dim1 = *ndimen;
    patnew_dim2 = *ncofmx;
    patnew_offset = patnew_dim1 * (patnew_dim2 + 1) + 1;
    patnew -= patnew_offset;
    patold_dim1 = *ndimen;
    patold_dim2 = *ncofmx;
    patold_offset = patold_dim1 * (patold_dim2 + 1) + 1;
    patold -= patold_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMFMCAR", 7L);
    }
    *iercod = 0;
    iofst = 0;

/* ********************************************************************** 
*/
/*                  TEST DES NOMBRES DE COEFFICIENTS */
/* ********************************************************************** 
*/

    if (*ncofmx < *ncoefu) {
	*iercod = 10;
	goto L9999;
    }
    if (*ncoefu < 1 || *ncoefu > 61 || *ncoefv < 1 || *ncoefv > 61) {
	*iercod = 10;
	goto L9999;
    }

/* ********************************************************************** 
*/
/*                    CAS OU UPARA1=VPARA1=0 ET UPARA2=VPARA2=1 */
/* ********************************************************************** 
*/

    if (*upara1 == 0. && *upara2 == 1. && *vpara1 == 0. && *vpara2 == 1.) {
	ksize = (*ndimen << 3) * *ncofmx * *ncoefv;
	AdvApp2Var_SysBase::mcrfill_((integer *)&ksize, 
		 (char *)&patold[patold_offset], 
		 (char *)&patnew[patnew_offset]);
	goto L9999;
    }

/* ********************************************************************** 
*/
/*                        LIMITATION EN U */
/* ********************************************************************** 
*/

    if (*upara1 == 0. && *upara2 == 1.) {
	goto L2000;
    }
    i__1 = *ncoefv;
    for (kk = 1; kk <= i__1; ++kk) {
	mmarc41_(ndimen, ndimen, ncoefu, &patold[(kk * patold_dim2 + 1) * 
		patold_dim1 + 1], upara1, upara2, &patnew[(kk * patnew_dim2 + 
		1) * patnew_dim1 + 1], iercod);
/* L100: */
    }

/* ********************************************************************** 
*/
/*                         LIMITATION EN V */
/* ********************************************************************** 
*/

L2000:
    if (*vpara1 == 0. && *vpara2 == 1.) {
	goto L9999;
    }

/* ----------- LIMITATION EN V (AVEC COMPRESSION I.E. NCOEFU<NCOFMX) ---- 
*/

    numax = *ndimen * *ncoefu;
    if (*ncofmx != *ncoefu) {
/* ------------------------- Allocation dynamique -------------------
---- */
	ksize = *ndimen * *ncoefu * *ncoefv;
	AdvApp2Var_SysBase::mcrrqst_(&c__8, &ksize, tbaux, &iofst, &ier);
	if (ier > 0) {
	    *iercod = 13;
	    goto L9900;
	}
/* --------------- Compression en (NDIMEN,NCOEFU,NCOEFV) ------------
---- */
	if (*upara1 == 0. && *upara2 == 1.) {
	  AdvApp2Var_MathBase::mmfmca9_(ndimen, 
					ncofmx, 
					ncoefv, 
					ndimen, 
					ncoefu, 
					ncoefv, 
					&patold[patold_offset], 
					&tbaux[iofst]);
	} else {
	  AdvApp2Var_MathBase::mmfmca9_(ndimen, 
					ncofmx, 
					ncoefv, 
					ndimen, 
					ncoefu, 
					ncoefv, 
					&patnew[patnew_offset],
					&tbaux[iofst]);
	}
/* ------------------------- Limitation en v ------------------------
---- */
	mmarc41_(&numax, &numax, ncoefv, &tbaux[iofst], vpara1, vpara2, &
		tbaux[iofst], iercod);
/* --------------------- Expansion de TBAUX dans PATNEW -------------
--- */
	AdvApp2Var_MathBase::mmfmca8_(ndimen, ncoefu, ncoefv, ndimen, ncofmx, ncoefv, &tbaux[iofst]
		, &patnew[patnew_offset]);
	goto L9900;

/* -------- LIMITATION EN V (SANS COMPRESSION I.E. NCOEFU=NCOFMX) ---
---- */

    } else {
	if (*upara1 == 0. && *upara2 == 1.) {
	    mmarc41_(&numax, &numax, ncoefv, &patold[patold_offset], vpara1, 
		    vpara2, &patnew[patnew_offset], iercod);
	} else {
	    mmarc41_(&numax, &numax, ncoefv, &patnew[patnew_offset], vpara1, 
		    vpara2, &patnew[patnew_offset], iercod);
	}
	goto L9999;
    }

/* ********************************************************************** 
*/
/*                             DESALLOCATION */
/* ********************************************************************** 
*/

L9900:
    if (iofst != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, &ksize, tbaux, &iofst, &ier);
    }
    if (ier > 0) {
	*iercod = 13;
    }

/* ------------------------------ The end ------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMFMCAR", iercod, 7L);
    }
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgsomsg_("MMFMCAR", 7L);
    }
    return 0;
} /* mmfmcar_ */


//=======================================================================
//function : AdvApp2Var_MathBase::mmfmcb5_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmfmcb5_(integer *isenmsc, 
				  integer *ndimax,
				  integer *ncf1mx, 
				  doublereal *courb1, 
				  integer *ncoeff, 
				  integer *ncf2mx,
				  integer *ndimen, 
				  doublereal *courb2, 
				  integer *iercod)

{
  /* System generated locals */
  integer courb1_dim1, courb1_offset, courb2_dim1, courb2_offset, i__1, 
  i__2;
  
  /* Local variables */
  static integer i__, nboct, nd;
  

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*       Reformattage (et compactage/decompactage eventuel) de courbe */
/*       (ndim,.) en (.,ndim) et reciproquement . */

/*     MOTS CLES : */
/*     ----------- */
/*      TOUS , MATH_ACCES :: */
/*      COURBE&, REORGANISATION,COMPRESSION,INVERSION , &COURBE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*        ISENMSC : sens du transfert demande : */
/*            1   :   passage de (NDIMEN,.) ---> (.,NDIMEN)  sens vers AB 
*/
/*          -1   :   passage de (.,NDIMEN) ---> (NDIMEN,.)  sens vers TS,T
V*/
/*        NDIMAX : format / dimension */
/*        NCF1MX : format en t de COURB1 */
/*   si ISENMSC= 1 : COURB1: La courbe a traiter (NDIMAX,.) */
/*        NCOEFF : nombre de coef de la courbe */
/*        NCF2MX : format en t de COURB2 */
/*        NDIMEN : dimension de la courbe et format de COURB2 */
/*   si ISENMSC=-1 : COURB2: La courbe a traiter (.,NDIMEN) */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*   si ISENMSC= 1 : COURB2: La courbe resultat (.,NDIMEN) */
/*   si ISENMSC=-1 : COURB1: La courbe resultat (NDIMAX,.) */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     permet de traiter les transferts usuels suivant : */
/*     | ---- ISENMSC = 1 ---- |      | ---- ISENMSC =-1 ----- | */
/*    TS  (3,21) --> (21,3)  AB  ;  AB  (21,3) --> (3,21)  TS */
/*    TS  (3,21) --> (NU,3)  AB  ;  AB  (NU,3) --> (3,21)  TS */
/*        (3,NU) --> (21,3)  AB  ;  AB  (21,3) --> (3,NU) */
/*        (3,NU) --> (NU,3)  AB  ;  AB  (NU,3) --> (3,NU) */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*       .07-08-89 : JG ; VERSION ORIGINALE (ANNULE ET REMPLACE MMCVINV) 
*/
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    courb1_dim1 = *ndimax;
    courb1_offset = courb1_dim1 + 1;
    courb1 -= courb1_offset;
    courb2_dim1 = *ncf2mx;
    courb2_offset = courb2_dim1 + 1;
    courb2 -= courb2_offset;

    /* Function Body */
    if (*ndimen > *ndimax || *ncoeff > *ncf1mx || *ncoeff > *ncf2mx) {
	goto L9119;
    }

    if (*ndimen == 1 && *ncf1mx == *ncf2mx) {
	nboct = *ncf2mx << 3;
	if (*isenmsc == 1) {
	    AdvApp2Var_SysBase::mcrfill_((integer *)&nboct, 
		     (char *)&courb1[courb1_offset], 
		     (char *)&courb2[courb2_offset]);
	}
	if (*isenmsc == -1) {
	    AdvApp2Var_SysBase::mcrfill_((integer *)&nboct, 
		     (char *)&courb2[courb2_offset], 
		     (char *)&courb1[courb1_offset]);
	}
	*iercod = -3136;
	goto L9999;
    }

    *iercod = 0;
    if (*isenmsc == 1) {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = *ncoeff;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		courb2[i__ + nd * courb2_dim1] = courb1[nd + i__ * 
			courb1_dim1];
/* L400: */
	    }
/* L500: */
	}
    } else if (*isenmsc == -1) {
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = *ncoeff;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		courb1[nd + i__ * courb1_dim1] = courb2[i__ + nd * 
			courb2_dim1];
/* L1400: */
	    }
/* L1500: */
	}
    } else {
	*iercod = 3164;
    }

    goto L9999;

/* ***********************************************************************
 */

L9119:
    *iercod = 3119;

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMFMCB5", iercod, 7L);
    }
    return 0;
} /* mmfmcb5_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmfmtb1_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmfmtb1_(integer *maxsz1, 
				  doublereal *table1, 
				  integer *isize1, 
				  integer *jsize1, 
				  integer *maxsz2, 
				  doublereal *table2, 
				  integer *isize2,
				  integer *jsize2, 
				  integer *iercod)
{
  static integer c__8 = 8;

   /* System generated locals */
    integer table1_dim1, table1_offset, table2_dim1, table2_offset, i__1, 
	    i__2;

    /* Local variables */
    static doublereal work[1];
    static integer ilong, isize, ii, jj, ier;
    static long int iofst,iipt, jjpt;


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*     Inversion des elements d'un tableau rectangulaire (T1(i,j) */
/*     est charge dans T2(j,i)) */

/*     MOTS CLES : */
/*     ----------- */
/*      TOUS, MATH_ACCES :: TABLEAU&, INVERSION, &TABLEAU */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     MAXSZ1: Nbre maxi d'elements suivant la 1ere dimension de */
/*             TABLE1. */
/*     TABLE1: Table de reels a deux dimensions. */
/*     ISIZE1: Nbre d'elements utiles de TABLE1 sur la 1ere dimension */
/*     JSIZE1: Nbre d'elements utiles de TABLE1 sur la 2eme dimension */
/*     MAXSZ2: Nbre maxi d'elements suivant la 1ere dimension de */
/*             TABLE2. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     TABLE2: Table de reels a deux dimensions, contenant la transposee 
*/
/*             du tableau rectangulaire TABLE1. */
/*     ISIZE2: Nbre d'elements utiles de TABLE2 sur la 1ere dimension */
/*     JSIZE2: Nbre d'elements utiles de TABLE2 sur la 2eme dimension */
/*     IERCOD: Code d'erreur. */
/*             = 0, ok. */
/*             = 1, erreur dans le dimensionnement des tables */
/*                  soit MAXSZ1 < ISIZE1 (tableau TABLE1 trop petit). */
/*                  soit MAXSZ2 < JSIZE1 (tableau TABLE2 trop petit). */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*    On peut utiliser TABLE1 comme tableau d'entree et de sortie i.e. */
/*    l'appel: */
/*    CALL MMFMTB1(MAXSZ1,TABLE1,ISIZE1,JSIZE1,MAXSZ2,TABLE1 */
/*               ,ISIZE2,JSIZE2,IERCOD) */
/*    est valable. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     07-06-91: RBD; Creation d'apres VCRINV de NAK. */
/* > */
/* ********************************************************************** 
*/


    /* Parameter adjustments */
    table1_dim1 = *maxsz1;
    table1_offset = table1_dim1 + 1;
    table1 -= table1_offset;
    table2_dim1 = *maxsz2;
    table2_offset = table2_dim1 + 1;
    table2 -= table2_offset;

    /* Function Body */
    *iercod = 0;
    if (*isize1 > *maxsz1 || *jsize1 > *maxsz2) {
	goto L9100;
    }

    iofst = 0;
    isize = *maxsz2 * *isize1;
    AdvApp2Var_SysBase::mcrrqst_(&c__8, &isize, work, &iofst, &ier);
    if (ier > 0) {
	goto L9200;
    }

/*             NE PAS CRAINDRE D'ECRASEMENT. */

    i__1 = *isize1;
    for (ii = 1; ii <= i__1; ++ii) {
	iipt = (ii - 1) * *maxsz2 + iofst;
	i__2 = *jsize1;
	for (jj = 1; jj <= i__2; ++jj) {
	    jjpt = iipt + (jj - 1);
	    work[jjpt] = table1[ii + jj * table1_dim1];
/* L200: */
	}
/* L100: */
    }
    ilong = isize << 3;
    AdvApp2Var_SysBase::mcrfill_((integer *)&ilong, 
	     (char *)&work[iofst], 
	     (char *)&table2[table2_offset]);

/* -------------- On recupere le nombre d'elements de TABLE2 ------------ 
*/

    ii = *isize1;
    *isize2 = *jsize1;
    *jsize2 = ii;

    goto L9999;

/* ------------------------------- THE END ------------------------------ 
*/
/* --> Entree invalide. */
L9100:
    *iercod = 1;
    goto L9999;
/* --> Pb d'alloc. */
L9200:
    *iercod = 2;
    goto L9999;

L9999:
    if (iofst != 0) {
	AdvApp2Var_SysBase::mcrdelt_(&c__8, &isize, work, &iofst, &ier);
    }
    if (ier > 0) {
	*iercod = 2;
    }
    return 0;
} /* mmfmtb1_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmgaus1_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmgaus1_(integer *ndimf,
				  int (*bfunx) (
						integer *ninteg, 
						doublereal *parame, 
						doublereal *vfunj1, 
						integer *iercod
						), 
				  
				  integer *k, 
				  doublereal *xd, 
				  doublereal *xf, 
				  doublereal *saux1, 
				  doublereal *saux2, 
				  doublereal *somme, 
				  integer *niter, 
				  integer *iercod)
{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static integer ndeg;
  static doublereal h__[20];
  static integer j;
  static doublereal t, u[20], x;
  static integer idimf;
  static doublereal c1x, c2x;
/* ********************************************************************** 
*/

/*      FONCTION : */
/*      -------- */

/*      Calcul de l'integrale de la fonction BFUNX passee en parametre */
/*      entre les bornes XD et XF . */
/*      La fonction doit etre calculable pour n'importe quelle valeur */
/*      de la variable dans l'intervalle donne.. */
/*      La methode utilisee est celle de GAUSS-LEGENDRE. Des explications 
*/
/*      peuvent etre obtenus sur le livre : */
/*          Complements de mathematiques a l'usage des Ingenieurs de */
/*          l'electrotechnique et des telecommunications. */
/*          Par Andre ANGOT - Collection technique et scientifique du CNET
 */
/*          page 772 .... */
/*      Le degre des polynomes de LEGENDRE utilise est passe en parametre.
 */

/*      MOTS CLES : */
/*      --------- */
/*         INTEGRATION,LEGENDRE,GAUSS */

/*      ARGUMENTS D'ENTREE : */
/*      ------------------ */

/*      NDIMF : Dimension de la fonction */
/*      BFUNX : Fonction a integrer passee en argument */
/*              Doit etre declaree en EXTERNAL dans la routine d'appel. */
/*                   SUBROUTINE BFUNX(NDIMF,X,VAL,IER) */
/*                   REAL *8 X,VAL */
/*     K      : Parametre determinant le degre du polynome de LEGENDRE qui
*/
/*               peut prendre une valeur comprise entre 0 et 10. */
/*               Le degre du polynome est egal a 4 k, c'est a dire 4, 8, 
*/
/*               12, 16, 20, 24, 28, 32, 36 et 40. */
/*               Si K n'est pas bon, le degre est pris a 40 directement. 
*/
/*      XD     : Borne inferieure de l'intervalle d'integration. */
/*      XF     : Borne superieure de l'intervalle d'integration. */
/*      SAUX1  : Tableau auxiliaire */
/*      SAUX2  : Tableau auxiliaire */

/*      ARGUMENTS DE SORTIE : */
/*      ------------------- */

/*      SOMME : Valeur de l'integrale */
/*      NITER : Nombre d'iterations effectues. */
/*              Il est egal au degre du polynome. */

/*      IER   : Code d'erreur : */
/*              < 0 ==> Attention - Warning */
/*              = 0 ==> Tout est OK */
/*              > 0 ==> Erreur severe - Faire un traitement special */
/*                  ==> Erreur dans le calcul de BFUNX (code de retour */
/*                      de cette routine */

/*              Si erreur => SOMME = 0 */

/*      COMMONS UTILISES : */
/*      ----------------- */



/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     Type  Name */
/*    @      BFUNX               MVGAUS0 */

/*      DESCRIPTION/REMARQUES/LIMITATIONS : */
/*      --------------------------------- */

/*      Voir les explications detaillees sur le listing */

/*      Utilisation de la methode de GAUSS (polynomes orthogonaux) */
/*      On utilise la symetrie des racines de ces polynomes */

/*      En fonction de K, le degre du polynome d'interpolation augmente. 
*/
/*      Si vous voulez calculer l'integrale avec une precision donnee, */
/*     boucler sur k variant de 1 a 10 et tester la difference de 2 iteres
*/
/*      consecutifs. Arreter la boucle si cette difference est inferieure 
*/
/*      a une valeur epsilon fixee a 10E-6 par exemple. */
/*      Si S1 et S2 sont 2 iteres successifs, tester suivant cet exemple :
 */

/*            AF=DABS(S1-S2) */
/*            AS=DABS(S2) */
/*            Si AS < 1 alors tester si FS < eps sinon tester AF/AS < eps 
*/
/*            --        -----                    ----- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ---------------------------- */
/*         3-09-1993 : PMN; CREATION D'APRES VGAUS1 (SAUX1 et SAUX2 en */
/*                    arguments) */
/*        . 04-10-89 : JP;AJOUT EXTERNAL BFUNX SGI_420_144 */
/*        . 20-08-87 : JP;INTEGRATION D'UNE FONCTION VECTORIELLE */
/*        . 08-08-87 : GD; Version originale */

/* > */
/************************************************************************
******/
/*     DECLARATIONS */
/************************************************************************
******/



/* ****** Initialisation generale ** */

    /* Parameter adjustments */
    --somme;
    --saux2;
    --saux1;

    /* Function Body */
    AdvApp2Var_SysBase::mvriraz_((integer *)ndimf, 
	     (char *)&somme[1]);
    *iercod = 0;

/* ****** Chargement des coefficients U et H ** */
/* -------------------------------------------- */

    mvgaus0_(k, u, h__, &ndeg, iercod);
    if (*iercod > 0) {
	goto L9999;
    }

/* ****** C1X => Point milieu intervalle  [XD,XF] */
/* ****** C2X => 1/2 amplitude intervalle [XD,XF] */

    c1x = (*xf + *xd) * .5;
    c2x = (*xf - *xd) * .5;

/* ---------------------------------------- */
/* ****** Integration pour un degre NDEG ** */
/* ---------------------------------------- */

    i__1 = ndeg;
    for (j = 1; j <= i__1; ++j) {
	t = c2x * u[j - 1];

	x = c1x + t;
	(*bfunx)(ndimf, &x, &saux1[1], iercod);
	if (*iercod != 0) {
	    goto L9999;
	}

	x = c1x - t;
	(*bfunx)(ndimf, &x, &saux2[1], iercod);
	if (*iercod != 0) {
	    goto L9999;
	}

	i__2 = *ndimf;
	for (idimf = 1; idimf <= i__2; ++idimf) {
	    somme[idimf] += h__[j - 1] * (saux1[idimf] + saux2[idimf]);
	}

    }

    *niter = ndeg << 1;
    i__1 = *ndimf;
    for (idimf = 1; idimf <= i__1; ++idimf) {
	somme[idimf] *= c2x;
    }

/* ****** Fin du sous-programme ** */

L9999:

 return 0   ;
} /* mmgaus1_ */
//=======================================================================
//function : mmherm0_
//purpose  : 
//=======================================================================
int mmherm0_(doublereal *debfin, 
	     integer *iercod)
{
  static integer c__576 = 576;
  static integer c__6 = 6;

  
   /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static doublereal amat[36]	/* was [6][6] */;
    static integer iord[2];
    static doublereal prod;
    static integer iord1, iord2;
    static doublereal miden[36]	/* was [6][6] */;
    static integer ncmat;
    static doublereal epspi, d1, d2;
    static integer ii, jj, pp, ncf;
    static doublereal cof[6];
    static integer iof[2], ier;
    static doublereal mat[36]	/* was [6][6] */;
    static integer cot;
    static doublereal abid[72]	/* was [12][6] */;
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*      INIT DES COEFFS. DES POLYNOMES D'INTERPOL. D'HERMITE */

/*     MOTS CLES : */
/*     ----------- */
/*      MATH_ACCES :: HERMITE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       DEBFIN : PARAMETRES OU SONT DONNEES LES CONTRAINTES */
/*                 DEBFIN(1) : PREMIER PARAMETRE */
/*                 DEBFIN(2) : DEUXIEME PARAMETRE */

/*     ON DOIT AVOIR: */
/*                 ABS (DEBFIN(I)) < 100 */
/*                 et */
/*                 (ABS(DEBFIN(1)+ABS(DEBFIN(2))) > 1/100 */
/*           (pour les overflows) */

/*      ABS(DEBFIN(2)-DEBFIN(1)) / (ABS(DEBFIN(1)+ABS(DEBFIN(2))) > 1/100 
*/
/*           (pour le conditionnement ) */


/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */

/*       IERCOD : Code d'erreur : 0 : O.K. */
/*                                1 : LES valeur de DEBFIN */
/*                                ne sont pas raisonnables */
/*                                -1 : L'init etait deja faite */
/*                                   (OK mais pas de traitement) */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*        Ce programme initialise les coefficients des polynomes */
/*     d'Hermite qui sont ensuite lus par MMHERM1 */

/*     HISTORIQUE */
/*     --------------------------------------------------------- */
/*     06-01-92: ALR; mise a 0 des termes de MAT non recalcules */
/*     23-12-91: ALR; 2 CORRECTIONS */
/*     12-11-91: ALR; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a STOCKER les coefficients des polynomes de */
/*      l'interpolation d'Hermite */

/*     MOTS CLES : */
/*     ----------- */
/*      HERMITE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     les coefficients des polynomes d'hermitesont calcules par */
/*     la routine MMHERM0 et lus par la routine MMHERM1 */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     23-11-91: ALR; MODIF DIMENSIONNEMENT */
/*     12-11-91: ALR; CREATION */
/* > */
/* ********************************************************************** 
*/





/*     NBCOEF est la taille de CMHERM (voir plus bas) */



/* ***********************************************************************
 */







/* ***********************************************************************
 */
/*     Verification des donnees */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --debfin;

    /* Function Body */
    d1 = abs(debfin[1]);
    if (d1 > (float)100.) {
	goto L9101;
    }

    d2 = abs(debfin[2]);
    if (d2 > (float)100.) {
	goto L9101;
    }

    d2 = d1 + d2;
    if (d2 < (float).01) {
	goto L9101;
    }

    d1 = (d__1 = debfin[2] - debfin[1], abs(d__1));
    if (d1 / d2 < (float).01) {
	goto L9101;
    }


/* ***********************************************************************
 */
/*     Initialisations */
/* ***********************************************************************
 */

    *iercod = 0;

    epspi = 1e-10;


/* ***********************************************************************
 */

/*     EST-CE DEJA INITIALISE ? */

    d1 = abs(debfin[1]) + abs(debfin[2]);
    d1 *= 16111959;

    if (debfin[1] != mmcmher_.tdebut) {
	goto L100;
    }
    if (debfin[2] != mmcmher_.tfinal) {
	goto L100;
    }
    if (d1 != mmcmher_.verifi) {
	goto L100;
    }


    goto L9001;


/* ***********************************************************************
 */
/*     CALCUL */
/* ***********************************************************************
 */


L100:

/*     Init. matrice identite: */

    ncmat = 36;
    AdvApp2Var_SysBase::mvriraz_((integer *)&ncmat, 
	     (char *)miden);

    for (ii = 1; ii <= 6; ++ii) {
	miden[ii + ii * 6 - 7] = 1.;
/* L110: */
    }



/*     Init a 0 du tableau CMHERM */

    AdvApp2Var_SysBase::mvriraz_((integer *)&c__576, (char *)mmcmher_.cmherm);

/*     Calcul par resolution de systemes lineaires */

    for (iord1 = -1; iord1 <= 2; ++iord1) {
	for (iord2 = -1; iord2 <= 2; ++iord2) {

	    iord[0] = iord1;
	    iord[1] = iord2;


	    iof[0] = 0;
	    iof[1] = iord[0] + 1;


	    ncf = iord[0] + iord[1] + 2;

/*        Calcul matrice MAT a inverser: */

	    for (cot = 1; cot <= 2; ++cot) {


		if (iord[cot - 1] > -1) {
		    prod = 1.;
		    i__1 = ncf;
		    for (jj = 1; jj <= i__1; ++jj) {
			cof[jj - 1] = 1.;
/* L200: */
		    }
		}

		i__1 = iord[cot - 1] + 1;
		for (pp = 1; pp <= i__1; ++pp) {

		    ii = pp + iof[cot - 1];

		    prod = 1.;

		    i__2 = pp - 1;
		    for (jj = 1; jj <= i__2; ++jj) {
			mat[ii + jj * 6 - 7] = (float)0.;
/* L300: */
		    }

		    i__2 = ncf;
		    for (jj = pp; jj <= i__2; ++jj) {

/*        tout se passe dans ces 3 lignes peu lisibles
 */

			mat[ii + jj * 6 - 7] = cof[jj - 1] * prod;
			cof[jj - 1] *= jj - pp;
			prod *= debfin[cot];

/* L400: */
		    }
/* L500: */
		}

/* L1000: */
	    }

/*     Inversion */

	    if (ncf >= 1) {
		AdvApp2Var_MathBase::mmmrslwd_(&c__6, &ncf, &ncf, mat, miden, &epspi, abid, amat, &
			ier);
		if (ier > 0) {
		    goto L9101;
		}
	    }

	    for (cot = 1; cot <= 2; ++cot) {
		i__1 = iord[cot - 1] + 1;
		for (pp = 1; pp <= i__1; ++pp) {
		    i__2 = ncf;
		    for (ii = 1; ii <= i__2; ++ii) {
			mmcmher_.cmherm[ii + (pp + (cot + ((iord1 + (iord2 << 
				2)) << 1)) * 3) * 6 + 155] = amat[ii + (pp + 
				iof[cot - 1]) * 6 - 7];
/* L1300: */
		    }
/* L1400: */
		}
/* L1500: */
	    }

/* L2000: */
	}
/* L2010: */
    }

/* ***********************************************************************
 */

/*     On positionne le flag initialise: */

    mmcmher_.tdebut = debfin[1];
    mmcmher_.tfinal = debfin[2];

    d1 = abs(debfin[1]) + abs(debfin[2]);
    mmcmher_.verifi = d1 * 16111959;


/* ***********************************************************************
 */

    goto L9999;

/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

L9001:
    *iercod = -1;
    goto L9999;

/* ***********************************************************************
 */

L9999:

    AdvApp2Var_SysBase::maermsg_("MMHERM0", iercod, 7L);

/* ***********************************************************************
 */
 return 0 ;
} /* mmherm0_ */

//=======================================================================
//function : mmherm1_
//purpose  : 
//=======================================================================
int mmherm1_(doublereal *debfin, 
	     integer *ordrmx, 
	     integer *iordre, 
	     doublereal *hermit, 
	     integer *iercod)
{
  /* System generated locals */
  integer hermit_dim1, hermit_dim2, hermit_offset;

  /* Local variables */
  static integer nbval;
  static doublereal d1;
  static integer cot;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*      lecture des coeffs. des polynomes d'interpol. d'HERMITE */

/*     MOTS CLES : */
/*     ----------- */
/*      MATH_ACCES :: HERMITE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       DEBFIN : PARAMETRES OU SONT DONNEES LES CONTRAINTES */
/*                 DEBFIN(1) : PREMIER PARAMETRE */
/*                 DEBFIN(2) : DEUXIEME PARAMETRE */

/*           Doivent etre egaux aux argeuments correspondant lors */
/*           du dernier appel a MMHERM0 pour l'init. des coeffs. */

/*       ORDRMX : sert a indiquer le dimensionnent de HERMIT: */
/*              on n'a pas le choix : ORDRMX doit etre egal a la valeur */
/*              du PARAMETER IORDMX de l'INCLUDE MMCMHER, soit 2 pour */
/*              l'instant. */

/*       IORDRE (2) : Ordres de contraintes en chaque parametre DEBFIN(I) 
*/
/*              corrspondant. doivent etre compris entre -1 (pas de */
/*              contrainte) et ORDRMX. */


/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */

/*       HERMIT : HERMIT(1:IORDRE(1)+IORDRE(2)+2, j, cote) sont les */
/*       coefficients dans la base canonique du polynome d'Hermite */
/*       correspondant aux ordres IORDRE aux paramtres DEBFIN pour */
/*       la contrainte d'ordre j en DEBFIN(cote). j est compris entre */
/*       0 et IORDRE(cote). */


/*       IERCOD : Code d'erreur : */
/*          -1: O.K mais on a du reinitialise les coefficients */
/*                 (info pour optimisation) */
/*          0 : O.K. */
/*          1 : Erreur dans MMHERM0 */
/*          2 : arguments invalides */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*        Ce programme lit les  coefficients des polynomes */
/*     d'Hermite qui ont ete au prealable initialise par MMHERM0 */

/* PMN : L'initialisation n'est plus a la charge de l'appelant. */

/*     HISTORIQUE */
/*     --------------------------------------------------------- */
/*     14-01-94: PMN; On appelle MMHERM0 si pas initialise. */
/*     12-11-91: ALR; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a STOCKER les coefficients des polynomes de */
/*      l'interpolation d'Hermite */

/*     MOTS CLES : */
/*     ----------- */
/*      HERMITE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*     les coefficients des polynomes d'hermitesont calcules par */
/*     la routine MMHERM0 et lus par la routine MMHERM1 */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     23-11-91: ALR; MODIF DIMENSIONNEMENT */
/*     12-11-91: ALR; CREATION */
/* > */
/* ********************************************************************** 
*/





/*     NBCOEF est la taille de CMHERM (voir plus bas) */



/* ***********************************************************************
 */





/* ***********************************************************************
 */
/*     Initialisations */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --debfin;
    hermit_dim1 = (*ordrmx << 1) + 2;
    hermit_dim2 = *ordrmx + 1;
    hermit_offset = hermit_dim1 * hermit_dim2 + 1;
    hermit -= hermit_offset;
    --iordre;

    /* Function Body */
    *iercod = 0;


/* ***********************************************************************
 */
/*     Verification des donnees */
/* ***********************************************************************
 */


    if (*ordrmx != 2) {
	goto L9102;
    }

    for (cot = 1; cot <= 2; ++cot) {
	if (iordre[cot] < -1) {
	    goto L9102;
	}
	if (iordre[cot] > *ordrmx) {
	    goto L9102;
	}
/* L100: */
    }


/*     EST-CE BIEN INITIALISE ? */

    d1 = abs(debfin[1]) + abs(debfin[2]);
    d1 *= 16111959;

/*     SINON ON INITIALISE */

    if (debfin[1] != mmcmher_.tdebut || debfin[2] != mmcmher_.tfinal || d1 
	    != mmcmher_.verifi) {
	*iercod = -1;
	mmherm0_(&debfin[1], iercod);
	if (*iercod > 0) {
	    goto L9101;
	}
    }


/* ***********************************************************************
 */
/*        LECTURE */
/* ***********************************************************************
 */

    nbval = 36;

    AdvApp2Var_SysBase::msrfill_(&nbval, &mmcmher_.cmherm[((((iordre[1] + (iordre[2] << 2)) << 1) 
	    + 1) * 3 + 1) * 6 + 156], &hermit[hermit_offset]);

/* ***********************************************************************
 */

    goto L9999;

/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

L9102:
    *iercod = 2;
    goto L9999;

/* ***********************************************************************
 */

L9999:

    AdvApp2Var_SysBase::maermsg_("MMHERM1", iercod, 7L);

/* ***********************************************************************
 */
 return 0 ;
} /* mmherm1_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmhjcan_
//purpose  : 
//=======================================================================
int AdvApp2Var_MathBase::mmhjcan_(integer *ndimen, 
			    integer *ncourb, 
			    integer *ncftab, 
			    integer *orcont, 
			    integer *ncflim, 
			    doublereal *tcbold, 
			    doublereal *tdecop, 
			    doublereal *tcbnew, 
			    integer *iercod)

{
  static integer c__2 = 2;
  static integer c__21 = 21;
  /* System generated locals */
    integer tcbold_dim1, tcbold_dim2, tcbold_offset, tcbnew_dim1, tcbnew_dim2,
	     tcbnew_offset, i__1, i__2, i__3, i__4, i__5;


    /* Local variables */
    static logical ldbg;
    static integer ndeg;
    static doublereal taux1[21];
    static integer d__, e, i__, k;
    static doublereal mfact;
    static integer ncoeff;
    static doublereal tjacap[21];
    static integer iordre[2];
    static doublereal hermit[36]/* was [6][3][2] */, ctenor, bornes[2];
    static integer ier;
    static integer aux1, aux2;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       CONVERSION LA TABLE TCBOLD DES COEFFICIENTS DES  COURBES */
/*       POLYNOMIALES EXPRIMEES DANS LA BASE HERMITE JACOBI, EN UNE */
/*       TABLE DE COEFFICIENTS TCBNEW DES COURBES EXPRIMEES DANS LA */
/*       BASE CANONIQUE */

/*     MOTS CLES : */
/*     ----------- */
/*      CANNONIQUE, HERMITE, JACCOBI */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       ORDHER : ORDRE DES POLYNOMES D'HERMITE OU ORDRE DE CONTINUITE */
/*       NCOEFS : NOMBRE DE COEFFICIENTS DE UNE LA COURBE POLYNOMIALE */
/*                POUR UNE DE SES NDIM COMPOSANTS;(DEGRE+1 DE LA COURBE) 
*/
/*       NDIM   : DIMENSION DE LA COURBE */
/*       CBHEJA : TABLE DE COEFFICIENTS DE LA COURBE DANS LA BASE */
/*                HERMITE JACOBI */
/*                (H(0,-1),..,H(ORDHER,-1),H(0,1),..,H(ORDHER,1), */
/*                 JA(ORDHER+1,2*ORDHER+2),....,JA(ORDHER+1,NCOEFS-1) */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       CBRCAN : TABLE DE COEFFICIENTS DE LA COURBE DANS LA BASE */
/*                CANONIQUE */
/*                (1, t, ...) */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     8-09-95 : KHN/PMN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Sert a fournir les constantes entieres de 0 a 1000 */

/*     MOTS CLES : */
/*     ----------- */
/*        TOUS,ENTIERS */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     11-10-89 : DH ; Creation version originale */
/* > */
/* ***********************************************************************
 */


/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --ncftab;
    tcbnew_dim1 = *ndimen;
    tcbnew_dim2 = *ncflim;
    tcbnew_offset = tcbnew_dim1 * (tcbnew_dim2 + 1) + 1;
    tcbnew -= tcbnew_offset;
    tcbold_dim1 = *ndimen;
    tcbold_dim2 = *ncflim;
    tcbold_offset = tcbold_dim1 * (tcbold_dim2 + 1) + 1;
    tcbold -= tcbold_offset;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMHJCAN", 7L);
    }
    *iercod = 0;

    bornes[0] = -1.;
    bornes[1] = 1.;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    if (*orcont > 2) {
	goto L9101;
    }
    if (*ncflim > 21) {
	goto L9101;
    }

/*     CALCUL DES POLYNOMES D'HERMITE DANS LA BASE CANONIQUE SUR (-1,1) */


    iordre[0] = *orcont;
    iordre[1] = *orcont;
    mmherm1_(bornes, &c__2, iordre, hermit, &ier);
    if (ier > 0) {
	goto L9102;
    }


    aux1 = *orcont + 1;
    aux2 = aux1 << 1;

    i__1 = *ncourb;
    for (e = 1; e <= i__1; ++e) {

	ctenor = (tdecop[e] - tdecop[e - 1]) / 2;
	ncoeff = ncftab[e];
	ndeg = ncoeff - 1;
	if (ncoeff > 21) {
	    goto L9101;
	}

	i__2 = *ndimen;
	for (d__ = 1; d__ <= i__2; ++d__) {

/*     CONVERSION DES COEFFICIENTS DE LA PARTIE DE LA COURBE EXPRI
MEE */
/*     DANS LA BASE HERMITE, DANS LA BASE CANONIQUE */

	    AdvApp2Var_SysBase::mvriraz_((integer *)&ncoeff, (char *)taux1);

	    i__3 = aux2;
	    for (k = 1; k <= i__3; ++k) {
		i__4 = aux1;
		for (i__ = 1; i__ <= i__4; ++i__) {
		    i__5 = i__ - 1;
		    mfact = AdvApp2Var_MathBase::pow__di(&ctenor, &i__5);
		    taux1[k - 1] += (tcbold[d__ + (i__ + e * tcbold_dim2) * 
			    tcbold_dim1] * hermit[k + (i__ + 2) * 6 - 19] + 
			    tcbold[d__ + (i__ + aux1 + e * tcbold_dim2) * 
			    tcbold_dim1] * hermit[k + (i__ + 5) * 6 - 19]) * 
			    mfact;
		}
	    }


	    i__3 = ncoeff;
	    for (i__ = aux2 + 1; i__ <= i__3; ++i__) {
		taux1[i__ - 1] = tcbold[d__ + (i__ + e * tcbold_dim2) * 
			tcbold_dim1];
	    }

/*     CONVERSION DES COEFFICIENTS DE LA PARTIE DE LA COURBE EXPRI
MEE */
/*     DANS LA BASE CANONIQUE-JACOBI , DANS LA BASE CANONIQUE */


	    AdvApp2Var_MathBase::mmapcmp_(&minombr_.nbr[1], &c__21, &ncoeff, taux1, tjacap);
	    AdvApp2Var_MathBase::mmjacan_(orcont, &ndeg, tjacap, taux1);

/*        RECOPIE  DES COEFS RESULTANT DE LA CONVERSION DANS LA TA
BLE */
/*        DES RESULTAT */

	    i__3 = ncoeff;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		tcbnew[d__ + (i__ + e * tcbnew_dim2) * tcbnew_dim1] = taux1[
			i__ - 1];
	    }

	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;
L9102:
    *iercod = 2;
    goto L9999;

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    AdvApp2Var_SysBase::maermsg_("MMHJCAN", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMHJCAN", 7L);
    }
 return 0 ;
} /* mmhjcan_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mminltt_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mminltt_(integer *ncolmx,
			    integer *nlgnmx, 
			    doublereal *tabtri, 
			    integer *nbrcol, 
			    integer *nbrlgn, 
			    doublereal *ajoute, 
			    doublereal *,//epseg, 
			    integer *iercod)
{
  /* System generated locals */
  integer tabtri_dim1, tabtri_offset, i__1, i__2;
  
  /* Local variables */
  static logical idbg;
  static integer icol, ilgn, nlgn, noct, inser;
  static doublereal epsega;
  static integer ibb;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        . Insertion d'une ligne dans une table triee sans redondance */

/*     MOTS CLES : */
/*     ----------- */
/*      TOUS,MATH_ACCES :: TABLEAU&,INSERTION,&TABLEAU */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*        . NCOLMX : Nombre de colonnes du tableau */
/*        . NLGNMX : Nombre de lignes du tableau */
/*        . TABTRI : Tableau trie par lignes sans redondances */
/*        . NBRCOL : Nombre de colonnes utilisees */
/*        . NBRLGN : Nombre de lignes utilisees */
/*        . AJOUTE : Ligne a ajouter */
/*        . EPSEGA : Epsilon pour le test de redondance */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*        . TABTRI : Tableau trie par lignes sans redondances */
/*        . NBRLGN : Nombre de lignes utilisees */
/*        . IERCOD : 0 -> Pas de probleme */
/*                   1 -> La table est pleine */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*     REFERENCES APPELEES : */
/*     --------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*        . On n'insere la ligne que si il n'y a pas de ligne tq tous ses 
*/
/*     elements soient egaux a ceux qu'on veut inserer a epsilon pres. */

/*        . Niveau de debug = 3 */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*        . 24-06-91 : RBD; Suppression des accents (Pb. Bull). */
/*        . 01-10-89 : VV ; Version originale */
/* > */
/* ***********************************************************************
 */
/*     DECLARATIONS , CONTROLE DES ARGUMENTS D'ENTREE , INITIALISATION */
/* ***********************************************************************
 */

/* --- Parametres */


/* --- Fonctions */


/* --- Variables locales */


/* --- Messagerie */

    /* Parameter adjustments */
    tabtri_dim1 = *ncolmx;
    tabtri_offset = tabtri_dim1 + 1;
    tabtri -= tabtri_offset;
    --ajoute;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    idbg = ibb >= 3;
    if (idbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMINLTT", 7L);
    }

/* --- Controle arguments */

    if (*nbrlgn >= *nlgnmx) {
	goto L9001;
    }

/* -------------------- */
/* *** INITIALISATIONS */
/* -------------------- */

    *iercod = 0;

/* ---------------------------- */
/* *** RECHERCHE DE REDONDANCE */
/* ---------------------------- */

    i__1 = *nbrlgn;
    for (ilgn = 1; ilgn <= i__1; ++ilgn) {
	if (tabtri[ilgn * tabtri_dim1 + 1] >= ajoute[1] - epsega) {
	    if (tabtri[ilgn * tabtri_dim1 + 1] <= ajoute[1] + epsega) {
		i__2 = *nbrcol;
		for (icol = 1; icol <= i__2; ++icol) {
		    if (tabtri[icol + ilgn * tabtri_dim1] < ajoute[icol] - 
			    epsega || tabtri[icol + ilgn * tabtri_dim1] > 
			    ajoute[icol] + epsega) {
			goto L20;
		    }
/* L10: */
		}
		goto L9999;
	    } else {
		goto L30;
	    }
	}
L20:
	;
    }

/* ----------------------------------- */
/* *** RECHERCHE DU POINT D'INSERTION */
/* ----------------------------------- */

L30:

    i__1 = *nbrlgn;
    for (ilgn = 1; ilgn <= i__1; ++ilgn) {
	i__2 = *nbrcol;
	for (icol = 1; icol <= i__2; ++icol) {
	    if (tabtri[icol + ilgn * tabtri_dim1] < ajoute[icol]) {
		goto L50;
	    }
	    if (tabtri[icol + ilgn * tabtri_dim1] > ajoute[icol]) {
		goto L70;
	    }
/* L60: */
	}
L50:
	;
    }

    ilgn = *nbrlgn + 1;

/* -------------- */
/* *** INSERTION */
/* -------------- */

L70:

    inser = ilgn;
    ++(*nbrlgn);

/* --- Decalage vers le bas */

    nlgn = *nbrlgn - inser;
    if (nlgn > 0) {
	noct = (*ncolmx << 3) * nlgn;
	AdvApp2Var_SysBase::mcrfill_((integer *)&noct, 
		 (char *)&tabtri[inser * tabtri_dim1 + 1], 
		 (char *)&tabtri[(inser + 1)* tabtri_dim1 + 1]);
    }

/* --- Copie de la ligne */

    noct = *nbrcol << 3;
    AdvApp2Var_SysBase::mcrfill_((integer *)&noct, 
	     (char *)&ajoute[1], 
	     (char *)&tabtri[inser * tabtri_dim1 + 1]);

    goto L9999;

/* ******************************************************************** */
/*       SORTIE ERREUR , RETOUR PROGRAMME APPELANT , MESSAGERIE */
/* ******************************************************************** */

/* --- La table est deja pleine */

L9001:
    *iercod = 1;

/* --- Fin */

L9999:
    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMINLTT", iercod, 7L);
    }
    if (idbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMINLTT", 7L);
    }
 return 0 ;
} /* mminltt_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmjacan_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmjacan_(integer *ideriv, 
			    integer *ndeg, 
			    doublereal *poljac, 
			    doublereal *polcan)
{
    /* System generated locals */
  integer poljac_dim1, i__1, i__2;
  
  /* Local variables */
  static integer iptt, i__, j, ibb;
  static doublereal bid;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     Routine de transfert de Jacobi normalise a canonique [-1,1], les */
/*     tableaux etant ranges en termes de degre pair puis impair. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,JACOBI,PASSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        IDERIV : Ordre de Jacobi compris entre -1 et 2. */
/*        NDEG : Le degre vrai du polynome. */
/*        POLJAC : Le polynome dans la base de Jacobi. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        POLCAN : La courbe exprimee dans la base canonique [-1,1]. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     04-01-90 : NAK ; COMMON MMJCOBI PAR INCLUDE MMJCOBI */
/*     12-04-1989 : RBD ; Appel MGSOMSG. */
/*     27-04-1988 : JJM ; Test NDEG=0 */
/*     01-03-1988 : JJM ; Creation. */

/* > */
/* ***********************************************************************
 */

/*   Le nom de la routine */

/*   Matrices de conversion */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        MATRICE DE TRANSFORMATION DS LA BASE DE LEGENDRE */

/*     MOTS CLES : */
/*     ----------- */
/*        MATH */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     04-01-90 : NAK ; Creation version originale */
/* > */
/* ***********************************************************************
 */



/*  Common de Legendre/Casteljau comprime. */

/*   0:1 0 Concerne les termes pairs, 1 les termes impairs. */
/*   CANPLG : Matrice de passage de canonique vers Jacobi avec parites */
/*            comptees */
/*   PLGCAN : Matrice de passage de Jacobi vers canonique avec parites */
/*            comptees. */




/* ***********************************************************************
 */

    /* Parameter adjustments */
    poljac_dim1 = *ndeg / 2 + 1;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 5) {
	AdvApp2Var_SysBase::mgenmsg_("MMJACAN", 7L);
    }

/* ----------------- Expression des termes de degre pair ---------------- 
*/

    i__1 = *ndeg / 2;
    for (i__ = 0; i__ <= i__1; ++i__) {
	bid = 0.;
	iptt = i__ * 31 - (i__ + 1) * i__ / 2 + 1;
	i__2 = *ndeg / 2;
	for (j = i__; j <= i__2; ++j) {
	    bid += mmjcobi_.plgcan[iptt + j + *ideriv * 992 + 991] * poljac[
		    j];
/* L310: */
	}
	polcan[i__ * 2] = bid;
/* L300: */
    }

/* --------------- Expression des termes de degre impair ---------------- 
*/

    if (*ndeg == 0) {
	goto L9999;
    }

    i__1 = (*ndeg - 1) / 2;
    for (i__ = 0; i__ <= i__1; ++i__) {
	bid = 0.;
	iptt = i__ * 31 - (i__ + 1) * i__ / 2 + 1;
	i__2 = (*ndeg - 1) / 2;
	for (j = i__; j <= i__2; ++j) {
	    bid += mmjcobi_.plgcan[iptt + j + ((*ideriv << 1) + 1) * 496 + 
		    991] * poljac[j + poljac_dim1];
/* L410: */
	}
	polcan[(i__ << 1) + 1] = bid;
/* L400: */
    }

/* -------------------------------- The end ----------------------------- 
*/

L9999:
    if (ibb >= 5) {
	AdvApp2Var_SysBase::mgsomsg_("MMJACAN", 7L);
    }
    return 0;
} /* mmjacan_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmjaccv_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmjaccv_(integer *ncoef, 
			    integer *ndim, 
			    integer *ider, 
			    doublereal *crvlgd,
			    doublereal *polaux,
			    doublereal *crvcan)

{
  /* Initialized data */
  
  static char nomprg[8+1] = "MMJACCV ";
  
  /* System generated locals */
  integer crvlgd_dim1, crvlgd_offset, crvcan_dim1, crvcan_offset, 
  polaux_dim1, i__1, i__2;
  
  /* Local variables */
  static integer ndeg, i__, nd, ii, ibb;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Passage de la base de Jacobi normalisee a la base canonique. */

/*     MOTS CLES : */
/*     ----------- */
/*        LISSAGE,BASE,LEGENDRE */


/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIM: Dimension de l' espace. */
/*        NCOEF: Degre +1 du polynome. */
/*        IDER: Ordre des polynomes de Jacobi. */
/*        CRVLGD : La courbe dans la base de Jacobi. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        POLAUX : Espace auxilliaire. */
/*        CRVCAN : La courbe dans la base canonique [-1,1] */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     26-04-1988 : RBD ; Cas de la courbe reduite a 1 point. */
/*     01-03-1988 : JJM ; Creation. */

/* > */
/* ********************************************************************* 
*/

/*   Le nom de la routine */
    /* Parameter adjustments */
    polaux_dim1 = (*ncoef - 1) / 2 + 1;
    crvcan_dim1 = *ncoef - 1 + 1;
    crvcan_offset = crvcan_dim1;
    crvcan -= crvcan_offset;
    crvlgd_dim1 = *ncoef - 1 + 1;
    crvlgd_offset = crvlgd_dim1;
    crvlgd -= crvlgd_offset;

    /* Function Body */

    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_(nomprg, 6L);
    }

    ndeg = *ncoef - 1;

    i__1 = *ndim;
    for (nd = 1; nd <= i__1; ++nd) {
/*   Chargement du tableau auxilliaire. */
	ii = 0;
	i__2 = ndeg / 2;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    polaux[i__] = crvlgd[ii + nd * crvlgd_dim1];
	    ii += 2;
/* L310: */
	}

	ii = 1;
	if (ndeg >= 1) {
	    i__2 = (ndeg - 1) / 2;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		polaux[i__ + polaux_dim1] = crvlgd[ii + nd * crvlgd_dim1];
		ii += 2;
/* L320: */
	    }
	}
/*   Appel a la routine de changement de base. */
	AdvApp2Var_MathBase::mmjacan_(ider, &ndeg, polaux, &crvcan[nd * crvcan_dim1]);
/* L300: */
    }


/* L9999: */
    return 0;
} /* mmjaccv_ */

//=======================================================================
//function : mmloncv_
//purpose  : 
//=======================================================================
int mmloncv_(integer *ndimax,
	     integer *ndimen,
	     integer *ncoeff,
	     doublereal *courbe, 
	     doublereal *tdebut, 
	     doublereal *tfinal, 
	     doublereal *xlongc, 
	     integer *iercod)

{
  /* Initialized data */
  
  static integer kgar = 0;
  
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1, i__2;
  
  /* Local variables */
  static doublereal tran;
  static integer ngaus;
  static doublereal c1, c2, d1, d2, wgaus[20], uroot[20], x1, x2, dd;
  static integer ii, jj, kk;
  static doublereal som;
  static doublereal der1, der2;




/* ********************************************************************** 
*/

/*     FONCTION : Longueur d'un arc de courbe sur un intervalle donne */
/*     ---------- pour une fonction dont la representation mathematique */
/*                est faite un polynome multidimensionnel. */
/*      Le polynome est en fait un ensemble de polynomes dont les coeffi- 
*/
/*      cients sont ranges dans un tableau a 2 indices, chaque ligne */
/*      etant relative a 1 polynome. */
/*      Le polynome est defini par ses coefficients ordonne par les puis- 
*/
/*      sances croissantes de la variable. */
/*      Tous les polynomes ont le meme nombre de coefficients (donc le */
/*      meme degre). */

/*     MOTS CLES : LONGUEUR, COURBE */
/*     ----------- */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */

/*      NDIMAX : Nombre de lignes maximum des tableaux */
/*               (nombre maxi de polynomes). */
/*      NDIMEN  : Dimension du polynome (Nombre de polynomes). */
/*      NCOEFF : Nombre de coefficients du polynome (pas de limitation) */
/*               C'est le degre + 1 */
/*      COURBE : Coefficients du polynome ordonne par les puissances */
/*               croissantes. A dimensionner a (NDIMAX,NCOEFF). */
/*      TDEBUT : Bornes inferieure de l'integration pour calcul de la */
/*               longueur. */
/*      TFINAL : Bornes superieure de l'integration pour calcul de la */
/*               longueur. */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*      XLONGC : Longueur de l'arc de courbe */

/*      IERCOD : Code d'erreur : */
/*             = 0 ==> Tout est OK */
/*             = 1 ==> NDIMEN ou NCOEFF negatif ou nul */
/*             = 2 ==> Pb chargement racines Legendre et poids de Gauss */
/*                     par MVGAUS0. */

/*     Si erreur => XLONGC = 0 */

/*     COMMONS UTILISES : */
/*     ------------------ */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG         R*8  DSQRT          I*4  MIN */
/*           MVGAUS0 */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/*      Voir VGAUSS pour bien comprendre la technique. */
/*      On integre en verite SQRT (dpi^2) pour i=1,nbdime */
/*      Le calcul de la derivee est mele dans le code pour ne pas faire */
/*      un appel supplementaire a une routine. */

/*      La fonction que l'on integre est strictement croissante, il */
/*      n'est pas necessaire d'utiliser un haut degre pour la methode */
/*      GAUSS */

/*      Le degre du polynome de LEGENDRE est fonction du degre du */
/*      polynome a integrer. Il peut varier de 4 a 40 (par pas de 4). */

/*      La precision (relative) de l'integration est de l'ordre */
/*      de 1.D-8. */

/*      ATTENTION : si TDEBUT > TFINAL, la longueur est alors NEGATIVE. */

/*      Attention : la precision sur le resultat n'est pas controlee. */
/*      Si vous desirez la controler utiliser plutot MMCGLC1, tout en */
/*      sachant que les performances (en temps) seront quand meme moins */
/*      bonnes. */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*      8-09-1995 : Performance */
/*     08-04-94 : JMC ; Rem: Appeler MMCGLC1 pour controler la precision 
*/
/*     26-04-90 : RBD ; Augmentation du nbre de points KK pour calcul */
/*                      + precis, appel a MXVINIT et MXVSAVE, recup */
/*                      code d'erreur MVGAUS0, ajout commentaires. */
/*      08-06-89 : GD ; Suppression des 2 parties de l'integration, */
/*                      MVGAUS0 est appelle que si le degre a change. */
/*      10-06-88 : GD ; Variation dynamique du degre LEGENDRE */
/*      18-08-87 : GD ; Version originale */

/* >===================================================================== 
*/

/*      ATTENTION : SAUVER KGAR WGAUS et UROOT EVENTUELLEMENT */
/*     ,IERXV */
/*      INTEGER I1,I20 */
/*      PARAMETER (I1=1,I20=20) */

    /* Parameter adjustments */
    courbe_dim1 = *ndimax;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */

/* ****** Initialisation generale ** */

    *iercod = 999999;
    *xlongc = 0.;

/* ****** Initialisation de UROOT, WGAUS, NGAUS et KGAR ** */

/*      CALL MXVINIT(IERXV,'INTEGER',I1,KGAR,'INTEGER',I1,NGAUS */
/*     1    ,'DOUBLE PRECISION',I20,UROOT,'DOUBLE PRECISION',I20,WGAUS) */
/*      IF (IERXV.GT.0) KGAR=0 */

/* ****** Test d'egalite des bornes ** */

    if (*tdebut == *tfinal) {
	*iercod = 0;
	goto L9900;
    }

/* ****** Test de la dimension et du nombre de coefficients ** */

    if (*ndimen <= 0 || *ncoeff <= 0) {
	*iercod = 1;
	goto L9900;
    }

/* ****** Calcul du degre optimum ** */

    kk = *ncoeff / 4 + 1;
    kk = min(kk,10);

/* ****** Recuperation des coefficients pour l'integrale (DEGRE=4*KK) */
/*       si KK <> KGAR. */

    if (kk != kgar) {
	mvgaus0_(&kk, uroot, wgaus, &ngaus, iercod);
	if (*iercod > 0) {
	    kgar = 0;
	    *iercod = 2;
	    goto L9900;
	}
	kgar = kk;
    }

/*      C1 => Point milieu intervalle */
/*      C2 => 1/2 amplitude intervalle */

    c1 = (*tfinal + *tdebut) * .5;
    c2 = (*tfinal - *tdebut) * .5;

/* ----------------------------------------------------------- */
/* ****** Integration - Boucle sur les intervalles de GAUSS ** */
/* ----------------------------------------------------------- */

    som = 0.;

    i__1 = ngaus;
    for (jj = 1; jj <= i__1; ++jj) {

/* ****** Integration en tenant compte de la symetrie ** */

	tran = c2 * uroot[jj - 1];
	x1 = c1 + tran;
	x2 = c1 - tran;

/* ****** Derivation sur la dimension de l'espace ** */

	der1 = 0.;
	der2 = 0.;
	i__2 = *ndimen;
	for (kk = 1; kk <= i__2; ++kk) {
	    d1 = (*ncoeff - 1) * courbe[kk + *ncoeff * courbe_dim1];
	    d2 = d1;
	    for (ii = *ncoeff - 1; ii >= 2; --ii) {
		dd = (ii - 1) * courbe[kk + ii * courbe_dim1];
		d1 = d1 * x1 + dd;
		d2 = d2 * x2 + dd;
/* L100: */
	    }
	    der1 += d1 * d1;
	    der2 += d2 * d2;
/* L200: */
	}

/* ****** Integration ** */

	som += wgaus[jj - 1] * c2 * (sqrt(der1) + sqrt(der2));

/* ****** Fin de boucle dur les intervalles de GAUSS ** */

/* L300: */
    }

/* ****** Travail termine ** */

    *xlongc = som;

/* ****** On force IERCOD  =  0 ** */

    *iercod = 0;

/* ****** Traitement de fin ** */

L9900:

/* ****** Sauvegarde de UROOT, WGAUS, NGAUS et KGAR ** */

/*      CALL MXVSAVE(IERXV,'INTEGER',I1,KGAR,'INTEGER',I1,NGAUS */
/*     1    ,'DOUBLE PRECISION',I20,UROOT,'DOUBLE PRECISION',I20,WGAUS) */
/*      IF (IERXV.GT.0) KGAR=0 */

/* ****** Fin du sous-programme ** */

    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMLONCV", iercod, 7L);
    }
 return 0 ;
} /* mmloncv_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmpobas_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmpobas_(doublereal *tparam, 
			    integer *iordre, 
			    integer *ncoeff, 
			    integer *nderiv, 
			    doublereal *valbas, 
			    integer *iercod)

{
  static integer c__2 = 2;
  static integer c__1 = 1;

  
   /* Initialized data */

    static doublereal moin11[2] = { -1.,1. };

    /* System generated locals */
    integer valbas_dim1, i__1;

    /* Local variables */
    static doublereal vjac[80], herm[24];
    static integer iord[2];
    static doublereal wval[4];
    static integer nwcof, iunit;
    static doublereal wpoly[7];
    static integer ii, jj, iorjac;
    static doublereal hermit[36]	/* was [6][3][2] */;
    static integer kk1, kk2, kk3;
    static integer khe, ier;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       Positionnement sur les polynomes de la base hermite-Jacobi */
/*       et leurs derives succesives */

/*     MOTS CLES : */
/*     ----------- */
/*      PUBLIC, POSITIONEMENT, HERMITE, JACOBI */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       TPARAM : Parametre pour lequel on se positionne. */
/*       IORDRE : Ordre d'hermite-Jacobi (-1,0,1, ou 2) */
/*       NCOEFF : Nombre de coeeficients des polynomes (Nb de valeur a */
/*                calculer) */
/*       NDERIV : Nombre de derive a calculer (0<= N <=3) */
/*              0 -> Positionement simple sur les fonctions de base */
/*              N -> Positionement sur les fonctions de base et lerive */
/*              d'ordre 1 a N */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     VALBAS (NCOEFF, 0:NDERIV) : les valeur calculee */
/*           i */
/*          d    vj(t)  = VALBAS(J, I) */
/*          -- i */
/*          dt */

/*    IERCOD : Code d'erreur */
/*      0 : Ok */
/*      1 : Incoherance des arguments d'entre */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     19-07-1995: PMN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



    /* Parameter adjustments */
    valbas_dim1 = *ncoeff;
    --valbas;

    /* Function Body */

/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    if (*nderiv > 3) {
	goto L9101;
    }
    if (*ncoeff > 20) {
	goto L9101;
    }
    if (*iordre > 2) {
	goto L9101;
    }

    iord[0] = *iordre;
    iord[1] = *iordre;
    iorjac = (*iordre + 1) << 1;

/*  (1) Calculs generiques .... */

/*  (1.a) Calcul des polynomes d'hermite */

    if (*iordre >= 0) {
	mmherm1_(moin11, &c__2, iord, hermit, &ier);
	if (ier > 0) {
	    goto L9102;
	}
    }

/*  (1.b) Evaluation des polynomes d'hermite */

    jj = 1;
    iunit = *nderiv + 1;
    khe = (*iordre + 1) * iunit;

    if (*nderiv > 0) {

	i__1 = *iordre;
	for (ii = 0; ii <= i__1; ++ii) {
	    mmdrvcb_(nderiv, &c__1, &iorjac, &hermit[(ii + 3) * 6 - 18], 
		    tparam, &herm[jj - 1], &ier);
	    if (ier > 0) {
		goto L9102;
	    }

	    mmdrvcb_(nderiv, &c__1, &iorjac, &hermit[(ii + 6) * 6 - 18], 
		    tparam, &herm[jj + khe - 1], &ier);
	    if (ier > 0) {
		goto L9102;
	    }
	    jj += iunit;
	}

    } else {

	i__1 = *iordre;
	for (ii = 0; ii <= i__1; ++ii) {
	    AdvApp2Var_MathBase::mmpocrb_(&c__1, &iorjac, &hermit[(ii + 3) * 6 - 18], &c__1, 
		    tparam, &herm[jj - 1]);

	    AdvApp2Var_MathBase::mmpocrb_(&c__1, &iorjac, &hermit[(ii + 6) * 6 - 18], &c__1, 
		    tparam, &herm[jj + khe - 1]);
	    jj += iunit;
	}
    }

/*  (1.c) Evaluation des polynomes de Jaccobi */

    ii = *ncoeff - iorjac;

    mmpojac_(tparam, &iorjac, &ii, nderiv, vjac, &ier);
    if (ier > 0) {
	goto L9102;
    }

/*  (1.d) Evaluation de W(t) */

/* Computing MAX */
    i__1 = iorjac + 1;
    nwcof = max(i__1,1);
    AdvApp2Var_SysBase::mvriraz_((integer *)&nwcof, 
	     (char *)wpoly);
    wpoly[0] = 1.;
    if (*iordre == 2) {
	wpoly[2] = -3.;
	wpoly[4] = 3.;
	wpoly[6] = -1.;
    } else if (*iordre == 1) {
	wpoly[2] = -2.;
	wpoly[4] = 1.;
    } else if (*iordre == 0) {
	wpoly[2] = -1.;
    }

    mmdrvcb_(nderiv, &c__1, &nwcof, wpoly, tparam, wval, &ier);
    if (ier > 0) {
	goto L9102;
    }

    kk1 = *ncoeff - iorjac;
    kk2 = kk1 << 1;
    kk3 = kk1 * 3;

/*  (2) Evaluation a l'ordre 0 */

    jj = 1;
    i__1 = iorjac;
    for (ii = 1; ii <= i__1; ++ii) {
	valbas[ii] = herm[jj - 1];
	jj += iunit;
    }

    i__1 = kk1;
    for (ii = 1; ii <= i__1; ++ii) {
	valbas[ii + iorjac] = wval[0] * vjac[ii - 1];
    }

/*  (3) Evaluation a l'ordre 1 */

    if (*nderiv >= 1) {
	jj = 2;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + valbas_dim1] = herm[jj - 1];
	    jj += iunit;
	}


	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + valbas_dim1] = wval[0] * vjac[ii + kk1 - 1] 
		    + wval[1] * vjac[ii - 1];
	}
    }

/*  (4)  Evaluation a l'ordre 2 */

    if (*nderiv >= 2) {
	jj = 3;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + (valbas_dim1 << 1)] = herm[jj - 1];
	    jj += iunit;
	}

	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + (valbas_dim1 << 1)] = wval[0] * vjac[ii + 
		    kk2 - 1] + wval[1] * 2 * vjac[ii + kk1 - 1] + wval[2] * 
		    vjac[ii - 1];
	}
    }

/*  (5) Evaluation a l'ordre 3 */

    if (*nderiv >= 3) {
	jj = 4;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + valbas_dim1 * 3] = herm[jj - 1];
	    jj += iunit;
	}

	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + valbas_dim1 * 3] = wval[0] * vjac[ii + kk3 - 
		    1] + wval[1] * 3 * vjac[ii + kk2 - 1] + wval[2] * 3 * 
		    vjac[ii + kk1 - 1] + wval[3] * vjac[ii - 1];
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

L9102:
    *iercod = 2;

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMPOBAS", iercod, 7L);
    }
 return 0 ;
} /* mmpobas_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmpocrb_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmpocrb_(integer *ndimax, 
			    integer *ncoeff, 
			    doublereal *courbe, 
			    integer *ndim, 
			    doublereal *tparam, 
			    doublereal *pntcrb)

{
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1, i__2;
  
  /* Local variables */
  static integer ncof2;
  static integer isize, nd, kcf, ncf;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        CALCULE LES COORDONNEES D'UN POINT D'UNE COURBE DE PARAMETRE */
/*        DONNE TPARAM ( CECI EN 2D, 3D OU PLUS) */

/*     MOTS CLES : */
/*     ----------- */
/*       TOUS , MATH_ACCES :: COURBE&,PARAMETRE& , POSITIONNEMENT , &POINT
 */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMAX : format / dimension de la courbe */
/*        NCOEFF : Nbre de coefficients de la courbe */
/*        COURBE : Matrice des coefficients de la courbe */
/*        NDIM   : Dimension utile de l'espace de travail */
/*        TPARAM : Valeur du parametre ou est calcule le point */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        PNTCRB : Coordonnees du point calcule */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*           MIRAZ                MVPSCR2              MVPSCR3 */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*       20-11-89 : JG : VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */


/* ***********************************************************************
 */

    /* Parameter adjustments */
    courbe_dim1 = *ndimax;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;
    --pntcrb;

    /* Function Body */
    isize = *ndim << 3;
    AdvApp2Var_SysBase::miraz_((integer *)&isize, 
	   (char *)&pntcrb[1]);

    if (*ncoeff <= 0) {
	goto L9999;
    }

/*   Traitement optimal 3d */

    if (*ndim == 3 && *ndimax == 3) {
	mvpscr3_(ncoeff, &courbe[courbe_offset], tparam, &pntcrb[1]);

/*   Traitement optimal 2d */

    } else if (*ndim == 2 && *ndimax == 2) {
	mvpscr2_(ncoeff, &courbe[courbe_offset], tparam, &pntcrb[1]);

/*   Dimension quelconque - schema de HORNER */

    } else if (*tparam == 0.) {
	i__1 = *ndim;
	for (nd = 1; nd <= i__1; ++nd) {
	    pntcrb[nd] = courbe[nd + courbe_dim1];
/* L100: */
	}
    } else if (*tparam == 1.) {
	i__1 = *ncoeff;
	for (ncf = 1; ncf <= i__1; ++ncf) {
	    i__2 = *ndim;
	    for (nd = 1; nd <= i__2; ++nd) {
		pntcrb[nd] += courbe[nd + ncf * courbe_dim1];
/* L300: */
	    }
/* L200: */
	}
    } else {
	ncof2 = *ncoeff + 2;
	i__1 = *ndim;
	for (nd = 1; nd <= i__1; ++nd) {
	    i__2 = *ncoeff;
	    for (ncf = 2; ncf <= i__2; ++ncf) {
		kcf = ncof2 - ncf;
		pntcrb[nd] = (pntcrb[nd] + courbe[nd + kcf * courbe_dim1]) * *
			tparam;
/* L500: */
	    }
	    pntcrb[nd] += courbe[nd + courbe_dim1];
/* L400: */
	}
    }

L9999:
 return 0   ;
} /* mmpocrb_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmmpocur_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmmpocur_(integer *ncofmx, 
			     integer *ndim, 
			     integer *ndeg, 
			     doublereal *courbe, 
			     doublereal *tparam, 
			     doublereal *tabval)

{
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1;
  
  /* Local variables */
  static integer i__, nd;
  static doublereal fu;
  
 
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Positionnement d'un point sur une courbe (ncofmx,ndim). */

/*     MOTS CLES : */
/*     ----------- */
/*        TOUS , AB_SPECIFI :: COURBE&,POLYNOME&,POSITIONNEMENT,&POINT */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX: Format / degre de la COURBE. */
/*        NDIM  : Dimension de l' espace. */
/*        NDEG  : Degre du polynome. */
/*        COURBE: Les coefficients de la courbe. */
/*        TPARAM: parametre sur la courbe */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        TABVAL(NDIM): Le point resultat (ou tableau de valeurs) */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     05-01-90 : JG : optimisation (supprim appel a MGENMSG) , nettoyage 
*/
/*     18-09-85 : Cree par JJM. */
/* > */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --tabval;
    courbe_dim1 = *ncofmx;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */
    if (*ndeg < 1) {
	i__1 = *ndim;
	for (nd = 1; nd <= i__1; ++nd) {
	    tabval[nd] = 0.;
/* L290: */
	}
    } else {
	i__1 = *ndim;
	for (nd = 1; nd <= i__1; ++nd) {
	    fu = courbe[*ndeg + nd * courbe_dim1];
	    for (i__ = *ndeg - 1; i__ >= 1; --i__) {
		fu = fu * *tparam + courbe[i__ + nd * courbe_dim1];
/* L120: */
	    }
	    tabval[nd] = fu;
/* L300: */
	}
    }
 return 0 ;
} /* mmmpocur_ */

//=======================================================================
//function : mmpojac_
//purpose  : 
//=======================================================================
int mmpojac_(doublereal *tparam, 
	     integer *iordre, 
	     integer *ncoeff, 
	     integer *nderiv, 
	     doublereal *valjac, 
	     integer *iercod)

{
  static integer c__2 = 2;
  
    /* Initialized data */

    static integer nbcof = -1;

    /* System generated locals */
    integer valjac_dim1, i__1, i__2;

    /* Local variables */
    static doublereal cofa, cofb, denom, tnorm[100];
    static integer ii, jj, kk1, kk2;
    static doublereal aux1, aux2;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       Positionnement sur les polynomes de Jacobi et leurs derives */
/*       successives par un algorithme de recurence */

/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, POSITIONEMENT, JACOBI */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       TPARAM : Parametre pour lequel on se positionne. */
/*       IORDRE : Ordre d'hermite-?? (-1,0,1, ou 2) */
/*       NCOEFF : Nombre de coeeficients des polynomes (Nb de valeur a */
/*                calculer) */
/*       NDERIV : Nombre de derive a calculer (0<= N <=3) */
/*              0 -> Positionement simple sur les fonctions de jacobi */
/*              N -> Positionement sur les fonctions de jacobi et leurs */
/*              derive d'ordre 1 a N. */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     VALJAC (NCOEFF, 0:NDERIV) : les valeur calculee */
/*           i */
/*          d    vj(t)  = VALJAC(J, I) */
/*          -- i */
/*          dt */

/*    IERCOD : Code d'erreur */
/*      0 : Ok */
/*      1 : Incoherance des arguments d'entre */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     19-07-1995: PMN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */


/*     varaibles statiques */



    /* Parameter adjustments */
    valjac_dim1 = *ncoeff;
    --valjac;

    /* Function Body */

/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    if (*nderiv > 3) {
	goto L9101;
    }
    if (*ncoeff > 100) {
	goto L9101;
    }

/*  --- Calcul des normes */

/*      IF (NCOEFF.GT.NBCOF) THEN */
    i__1 = *ncoeff;
    for (ii = 1; ii <= i__1; ++ii) {
	kk1 = ii - 1;
	aux2 = 1.;
	i__2 = *iordre;
	for (jj = 1; jj <= i__2; ++jj) {
	    aux2 = aux2 * (doublereal) (kk1 + *iordre + jj) / (doublereal) (
		    kk1 + jj);
	}
	i__2 = (*iordre << 1) + 1;
	tnorm[ii - 1] = sqrt(aux2 * (kk1 * 2. + (*iordre << 1) + 1) / pow__ii(&
		c__2, &i__2));
    }

    nbcof = *ncoeff;

/*      END IF */

/*  --- Positionements triviaux ----- */

    valjac[1] = 1.;
    aux1 = (doublereal) (*iordre + 1);
    valjac[2] = aux1 * *tparam;

    if (*nderiv >= 1) {
	valjac[valjac_dim1 + 1] = 0.;
	valjac[valjac_dim1 + 2] = aux1;

	if (*nderiv >= 2) {
	    valjac[(valjac_dim1 << 1) + 1] = 0.;
	    valjac[(valjac_dim1 << 1) + 2] = 0.;

	    if (*nderiv >= 3) {
		valjac[valjac_dim1 * 3 + 1] = 0.;
		valjac[valjac_dim1 * 3 + 2] = 0.;
	    }
	}
    }

/*  --- Positionement par reccurence */

    i__1 = *ncoeff;
    for (ii = 3; ii <= i__1; ++ii) {

	kk1 = ii - 1;
	kk2 = ii - 2;
	aux1 = (doublereal) (*iordre + kk2);
	aux2 = aux1 * 2;
	cofa = aux2 * (aux2 + 1) * (aux2 + 2);
	cofb = (aux2 + 2) * -2. * aux1 * aux1;
	denom = kk1 * 2. * (kk2 + (*iordre << 1) + 1) * aux2;
	denom = 1. / denom;

/*        --> Pi(t) */
	valjac[ii] = (cofa * *tparam * valjac[kk1] + cofb * valjac[kk2]) * 
		denom;
/*        --> P'i(t) */
	if (*nderiv >= 1) {
	    valjac[ii + valjac_dim1] = (cofa * *tparam * valjac[kk1 + 
		    valjac_dim1] + cofa * valjac[kk1] + cofb * valjac[kk2 + 
		    valjac_dim1]) * denom;
/*        --> P''i(t) */
	    if (*nderiv >= 2) {
		valjac[ii + (valjac_dim1 << 1)] = (cofa * *tparam * valjac[
			kk1 + (valjac_dim1 << 1)] + cofa * 2 * valjac[kk1 + 
			valjac_dim1] + cofb * valjac[kk2 + (valjac_dim1 << 1)]
			) * denom;
	    }
/*        --> P'i(t) */
	    if (*nderiv >= 3) {
		valjac[ii + valjac_dim1 * 3] = (cofa * *tparam * valjac[kk1 + 
			valjac_dim1 * 3] + cofa * 3 * valjac[kk1 + (
			valjac_dim1 << 1)] + cofb * valjac[kk2 + valjac_dim1 *
			 3]) * denom;
	    }
	}
    }

/*    ---> Normalisation */

    i__1 = *ncoeff;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nderiv;
	for (jj = 0; jj <= i__2; ++jj) {
	    valjac[ii + jj * valjac_dim1] = tnorm[ii - 1] * valjac[ii + jj * 
		    valjac_dim1];
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;


/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMPOJAC", iercod, 7L);
    }
 return 0 ;
} /* mmpojac_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmposui_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmposui_(integer *dimmat, 
			    integer *,//nistoc, 
			    integer *aposit, 
			    integer *posuiv, 
			    integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer imin, jmin, i__, j, k;
  static logical trouve;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       REMPLISSAGE DE LA TABLE DE POSITIONNEMENT POSUIV QUI PERMET DE */
/*       PARCOURIR EN COLONNE LA PARTIE TRAINGULAIRE INFERIEUR DE LA */
/*       MATRICE  SOUS FORME DE PROFIL */


/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, MATRICE, PROFIL */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */

/*       NISTOC: NOMBRE DE COEFFICIENTS DANS LE PROFILE */
/*       DIMMAT: NOMBRE DE LIGNE DE LA MATRICE CARRE SYMETRIQUE */
/*       APOSIT: TABLE DE POSITIONNEMENT DES TERMES DE STOCKAGE */
/*               APOSIT(1,I) CONTIENT LE NOMBRE DE TERMES-1 SUR LA LIGNE 
*/
/*               I DANS LE PROFIL DE LA MATRICE */
/*              APOSIT(2,I) CONTIENT L'INDICE DE STOCKAGE DU TERME DIAGONA
L*/
/*               DE LA LIGNE I */


/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       POSUIV: POSUIV(K) (OU K EST L'INDICE DE STOCKAGE DE MAT(I,J)) */
/*               CONTIENT LE PLUS PETIT NUMERO IMIN>I DE LA  LIGNE QUI */
/*               POSSEDE UN TERME MAT(IMIN,J) QUI EST DANS LE PROFIL. */
/*               S'IL N'Y A PAS LE TERME MAT(IMIN,J) DANS LE PROFIL */
/*               ALORS POSUIV(K)=-1 */




/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     23-08-95 : KHN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    aposit -= 3;
    --posuiv;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMPOSUI", 7L);
    }
    *iercod = 0;


/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */



    i__1 = *dimmat;
    for (i__ = 1; i__ <= i__1; ++i__) {
	jmin = i__ - aposit[(i__ << 1) + 1];
	i__2 = i__;
	for (j = jmin; j <= i__2; ++j) {
	    imin = i__ + 1;
	    trouve = FALSE_;
	    while(! trouve && imin <= *dimmat) {
		if (imin - aposit[(imin << 1) + 1] <= j) {
		    trouve = TRUE_;
		} else {
		    ++imin;
		}
	    }
	    k = aposit[(i__ << 1) + 2] - i__ + j;
	    if (trouve) {
		posuiv[k] = imin;
	    } else {
		posuiv[k] = -1;
	    }
	}
    }





    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

/* ___ DESALLOCATION, ... */

    AdvApp2Var_SysBase::maermsg_("MMPOSUI", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMPOSUI", 7L);
    }
 return 0 ;
} /* mmposui_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmresol_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmresol_(integer *hdimen, 
			    integer *gdimen, 
			    integer *hnstoc, 
			    integer *gnstoc, 
			    integer *mnstoc, 
			    doublereal *matsyh, 
			    doublereal *matsyg, 
			    doublereal *vecsyh, 
			    doublereal *vecsyg, 
			    integer *hposit, 
			    integer *hposui, 
			    integer *gposit, 
			    integer *mmposui, 
			    integer *mposit, 
			    doublereal *vecsol, 
			    integer *iercod)

{
  static integer c__100 = 100;
 
   /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static logical ldbg;
    static doublereal mcho[100];
    static integer jmin, jmax, i__, j, k, l;
    static long int iofv1, iofv2, iofv3, iofv4;
    static doublereal v1[100], v2[100], v3[100], v4[100];
    static integer deblig, dimhch;
    static doublereal hchole[100];
    static long int iofmch, iofmam, iofhch;
    static doublereal matsym[100];
    static integer ier;
    static integer aux;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       RESOLUTION DU SYSTEME */
/*       H  t(G)   V     B */
/*                    = */
/*       G    0    L     C */

/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, RESOLUTION, SYSTEME, LAGRANGIEN */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*      HDIMEN: NOMBRE DE LIGNE(OU COLONNE) DE LA MATRICE HESSIENNE */
/*      GDIMEN: NOMBRE DE LIGNE DE LA MATRICE DES CONTRAINTES */
/*      HNSTOC: NOMBRES DE TERMES DANS LE PROFIL DE LA MATRICE HESSIENNE 
*/
/*      GNSTOC: NOMBRES DE TERMES DANS LE PROFIL DE LA MATRICE DES */
/*              CONTRAINTES */
/*      MNSTOC: NOMBRES DE TERMES DANS LE PROFIL DE LA MATRICE */
/*              M= G H t(G) */
/*              ou H EST LA MATRICE HESSIENNE ET G LA MATRICE DES */
/*              CONTRAINTES */
/*      MATSYH: PARTIE TRIANGULAIRE INFERIEUR DE LA MATRICE */
/*              HESSIENNE SOUS FORME DE PROFIL */
/*      MATSYG: MATRICE DES CONTRAINTES SOUS FORME DE PROFIL */
/*      VECSYH: VECTEUR DU SECOND MEMBRE ASSOCIE A MATSYH */
/*      VECSYG: VECTEUR DU SECOND MEMBRE ASSOCIE A MATSYG */
/*      HPOSIT: TABLE DE POSITIONNEMENT DE LA MATRICE HESSIENNE */
/*              HPOSIT(1,I) CONTIENT LE NOMBRE DE TERMES -1 */
/*              QUI SONT DANS LE PROFIL A LA LIGNE I */
/*              HPOSIT(2,I) CONTIENT L'INDICE DE STOCKAGE DU TERME */
/*              DIAGNALE DE LA MATRICE A LA LIGNE I */
/*      HPOSUI: TABLE PERMETTANT DE BALAYER EN COLONNE LA MATRICE */
/*              HESSIENNE SOUS FORME DE PROFIL */
/*             HPOSUI(K) CONTIENT LE NUMERO DE LIGNE IMIN SUIVANT  LA LIGN
E*/
/*              COURANT I OU H(I,J)=MATSYH(K) TEL QUE IL EXISTE DANS LA */
/*              MEME COLONNE J UN TERME DANS LE PROFIL DE LA LIGNE IMIN */
/*              SI UN TEL TERME N'EXISTE PAS IMIN=-1 */
/*      GPOSIT: TABLE DE POSITIONNEMENT DE LA MATRICE DES CONTRAINTES */
/*              GPOSIT(1,I) CONTIENT LE NOMBRE DE TERMES DE LA LIGNE I */
/*                          QUI SONT DANS LE PROFIL */
/*              GPOSIT(2,I) CONTIENT L'INDICE DE STOKAGE DU DERNIER TERME 
*/
/*                          DE LA LIGNE I QUI EST DANS LE PROFIL */
/*              GPOSIT(3,I) CONTIENT LE NUMERO DE COLONNE CORRESPONDANT */
/*                          AU PREMIER TERME DE LA LIGNE I QUI EST DANS */
/*                          LE PROFIL */
/*      MMPOSUI, MPOSIT: MEME STRUCTURE QUE HPOSUI, MAIS POUR LA MATRICE 
*/
/*              M=G H t(G) */


/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       VECSOL: VECTEUR SOLUTION V DU SYSTEME */
/*       IERCOD: CODE D'ERREUR */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     21-09-96 : KHN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */

/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --vecsol;
    hposit -= 3;
    --vecsyh;
    --hposui;
    --matsyh;
    --matsyg;
    --vecsyg;
    gposit -= 4;
    --mmposui;
    mposit -= 3;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMRESOL", 7L);
    }
    *iercod = 0;
    iofhch = 0;
    iofv1 = 0;
    iofv2 = 0;
    iofv3 = 0;
    iofv4 = 0;
    iofmam = 0;
    iofmch = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

/*     Allocation dynamique */

    AdvApp2Var_SysBase::macrar8_(hdimen, &c__100, v1, &iofv1, &ier);
    if (ier > 0) {
	goto L9102;
    }
    dimhch = hposit[(*hdimen << 1) + 2];
    AdvApp2Var_SysBase::macrar8_(&dimhch, &c__100, hchole, &iofhch, &ier);
    if (ier > 0) {
	goto L9102;
    }

/*   RESOL DU SYST 1     H V1 = b */
/*     ou H=MATSYH  et b=VECSYH */

    mmchole_(hnstoc, hdimen, &matsyh[1], &hposit[3], &hposui[1], &hchole[
	    iofhch], &ier);
    if (ier > 0) {
	goto L9101;
    }
    mmrslss_(hnstoc, hdimen, &hchole[iofhch], &hposit[3], &hposui[1], &vecsyh[
	    1], &v1[iofv1], &ier);
    if (ier > 0) {
	goto L9102;
    }

/*     CAS OU IL Y A DES CONTRAINTES */

    if (*gdimen > 0) {

/*    CALCUL LE VECTEUR DU SECOND MEMBRE V2=G H(-1) b -c = G v1-c */
/*    DU SYSTEME D'INCONNU LE VECTEUR MULTIP DE LAGRANGE */
/*    ou G=MATSYG */
/*       c=VECSYG */

	AdvApp2Var_SysBase::macrar8_(gdimen, &c__100, v2, &iofv2, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	AdvApp2Var_SysBase::macrar8_(hdimen, &c__100, v3, &iofv3, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	AdvApp2Var_SysBase::macrar8_(gdimen, &c__100, v4, &iofv4, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	AdvApp2Var_SysBase::macrar8_(mnstoc, &c__100, matsym, &iofmam, &ier);
	if (ier > 0) {
	    goto L9102;
	}

	deblig = 1;
	mmatvec_(gdimen, hdimen, &gposit[4], gnstoc, &matsyg[1], &v1[iofv1], &
		deblig, &v2[iofv2], &ier);
	if (ier > 0) {
	    goto L9101;
	}
	i__1 = *gdimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    v2[i__ + iofv2 - 1] -= vecsyg[i__];
	}

/*     CALCUL de la matrice M= G H(-1) t(G) */
/*     RESOL DU SYST 2 : H qi = gi */
/*             ou gi est un vecteur colonne de t(G) */
/*                qi=v3 */
/*            puis calcul G qi */
/*            puis construire M sous forme de profil */



	i__1 = *gdimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    AdvApp2Var_SysBase::mvriraz_((integer *)hdimen, (char *)&v1[iofv1]);
	    AdvApp2Var_SysBase::mvriraz_((integer *)hdimen, (char *)&v3[iofv3]);
	    AdvApp2Var_SysBase::mvriraz_((integer *)gdimen, (char *)&v4[iofv4]);
	    jmin = gposit[i__ * 3 + 3];
	    jmax = gposit[i__ * 3 + 1] + gposit[i__ * 3 + 3] - 1;
	    aux = gposit[i__ * 3 + 2] - gposit[i__ * 3 + 1] - jmin + 1;
	    i__2 = jmax;
	    for (j = jmin; j <= i__2; ++j) {
		k = j + aux;
		v1[j + iofv1 - 1] = matsyg[k];
	    }
	    mmrslss_(hnstoc, hdimen, &hchole[iofhch], &hposit[3], &hposui[1], 
		    &v1[iofv1], &v3[iofv3], &ier);
	    if (ier > 0) {
		goto L9101;
	    }

	    deblig = i__;
	    mmatvec_(gdimen, hdimen, &gposit[4], gnstoc, &matsyg[1], &v3[
		    iofv3], &deblig, &v4[iofv4], &ier);
	    if (ier > 0) {
		goto L9101;
	    }

	    k = mposit[(i__ << 1) + 2];
	    matsym[k + iofmam - 1] = v4[i__ + iofv4 - 1];
	    while(mmposui[k] > 0) {
		l = mmposui[k];
		k = mposit[(l << 1) + 2] - l + i__;
		matsym[k + iofmam - 1] = v4[l + iofv4 - 1];
	    }
	}


/*    RESOL SYST 3  M L = V2 */
/*     AVEC L=V4 */


	AdvApp2Var_SysBase::mvriraz_((integer *)gdimen, (char *)&v4[iofv4]);
	AdvApp2Var_SysBase::macrar8_(mnstoc, &c__100, mcho, &iofmch, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	mmchole_(mnstoc, gdimen, &matsym[iofmam], &mposit[3], &mmposui[1], &
		mcho[iofmch], &ier);
	if (ier > 0) {
	    goto L9101;
	}
	mmrslss_(mnstoc, gdimen, &mcho[iofmch], &mposit[3], &mmposui[1], &v2[
		iofv2], &v4[iofv4], &ier);
	if (ier > 0) {
	    goto L9102;
	}


/*    CALCUL LE VECTEUR DU SECOND MEMBRE DU SYSTEME  Hx = b - t(G) L 
*/
/*                                                      = V1 */

	AdvApp2Var_SysBase::mvriraz_((integer *)hdimen, (char *)&v1[iofv1]);
	mmtmave_(gdimen, hdimen, &gposit[4], gnstoc, &matsyg[1], &v4[iofv4], &
		v1[iofv1], &ier);
	if (ier > 0) {
	    goto L9101;
	}
	i__1 = *hdimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    v1[i__ + iofv1 - 1] = vecsyh[i__] - v1[i__ + iofv1 - 1];
	}

/*    RESOL SYST 4   Hx = b - t(G) L */


	mmrslss_(hnstoc, hdimen, &hchole[iofhch], &hposit[3], &hposui[1], &v1[
		iofv1], &vecsol[1], &ier);
	if (ier > 0) {
	    goto L9102;
	}
    } else {
	i__1 = *hdimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    vecsol[i__] = v1[i__ + iofv1 - 1];
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */


L9101:
    *iercod = 1;
    goto L9999;

L9102:
    AdvApp2Var_SysBase::mswrdbg_("MMRESOL : PROBLEME AVEC DIMMAT", 30L);
    *iercod = 2;

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

/* ___ DESALLOCATION, ... */
    AdvApp2Var_SysBase::macrdr8_(hdimen, &c__100, v1, &iofv1, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(&dimhch, &c__100, hchole, &iofhch, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(gdimen, &c__100, v2, &iofv2, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(hdimen, &c__100, v3, &iofv3, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(gdimen, &c__100, v4, &iofv4, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(mnstoc, &c__100, matsym, &iofmam, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    AdvApp2Var_SysBase::macrdr8_(mnstoc, &c__100, mcho, &iofmch, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }

    AdvApp2Var_SysBase::maermsg_("MMRESOL", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMRESOL", 7L);
    }
 return 0 ;
} /* mmresol_ */

//=======================================================================
//function : mmrslss_
//purpose  : 
//=======================================================================
int mmrslss_(integer *,//mxcoef, 
	     integer *dimens, 
	     doublereal *smatri, 
	     integer *sposit,
	     integer *posuiv, 
	     doublereal *mscnmbr,
	     doublereal *soluti, 
	     integer *iercod)
{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer i__, j;
  static doublereal somme;
  static integer pointe, ptcour;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ----------                     T */
/*       Resoud le systeme lineaire SS x = b ou S est une matrice */
/*       triangulaire inferieure donnee sous forme profil */

/*     MOTS CLES : */
/*     ----------- */
/*     RESERVE, MATRICE_PROFILE, RESOLUTION, CHOLESKI */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*     MXCOEF  : Nombre maximal de coefficient non nuls dans la matrice */
/*     DIMENS  : Dimension de la matrice */
/*     SMATRI(MXCOEF) : Valeurs des coefficients de la matrice */
/*     SPOSIT(2,DIMENS): */
/*       SPOSIT(1,*) : Distance diagonnal-extrimite de la ligne */
/*       SPOSIT(2,*) : Position des termes diagonnaux dans AMATRI */
/*     POSUIV(MXCOEF): premiere ligne inferieure non hors profil */
/*     MSCNMBR(DIMENS): Vecteur second membre de l'equation */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     SOLUTI(NDIMEN) : Vecteur resultat */
/*     IERCOD   : Code d'erreur 0  : ok */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       T */
/*     SS  est la decomposition de choleski d'une matrice symetrique */
/*     definie postive, qui peut s'obtenir par la routine MMCHOLE. */

/*     Pour une matrice pleine on peut utiliser MRSLMSC */

/*     NIVEAU DE DEBUG = 4 */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     14-02-1994: PMN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --posuiv;
    --smatri;
    --soluti;
    --mscnmbr;
    sposit -= 3;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 4;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMRSLSS", 7L);
    }
    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

/* ----- Resolution de Sw = b */

    i__1 = *dimens;
    for (i__ = 1; i__ <= i__1; ++i__) {

	pointe = sposit[(i__ << 1) + 2];
	somme = 0.;
	i__2 = i__ - 1;
	for (j = i__ - sposit[(i__ << 1) + 1]; j <= i__2; ++j) {
	    somme += smatri[pointe - (i__ - j)] * soluti[j];
	}

	soluti[i__] = (mscnmbr[i__] - somme) / smatri[pointe];
    }
/*                     T */
/* ----- Resolution de S u = w */

    for (i__ = *dimens; i__ >= 1; --i__) {

	pointe = sposit[(i__ << 1) + 2];
	j = posuiv[pointe];
	somme = 0.;
	while(j > 0) {
	    ptcour = sposit[(j << 1) + 2] - (j - i__);
	    somme += smatri[ptcour] * soluti[j];
	    j = posuiv[ptcour];
	}

	soluti[i__] = (soluti[i__] - somme) / smatri[pointe];
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

    AdvApp2Var_SysBase::maermsg_("MMRSLSS", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMRSLSS", 7L);
    }
 return 0 ;
} /* mmrslss_ */

//=======================================================================
//function : mmrslw_
//purpose  : 
//=======================================================================
int mmrslw_(integer *normax, 
	    integer *nordre, 
	    integer *ndimen, 
	    doublereal *epspiv,
	    doublereal *abmatr,
	    doublereal *xmatri, 
	    integer *iercod)
{
  /* System generated locals */
    integer abmatr_dim1, abmatr_offset, xmatri_dim1, xmatri_offset, i__1, 
	    i__2, i__3;
    doublereal d__1;

    /* Local variables */
    static integer kpiv;
    static doublereal pivot;
    static integer ii, jj, kk;
    static doublereal akj;
    

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Resolution d' un systeme lineaire A.x = B de N equations a N */
/*  inconnues par la methode de Gauss (pivot partiel) ou : */
/*          A est une matrice NORDRE * NORDRE, */
/*          B est une matrice NORDRE (lignes) * NDIMEN (colonnes), */
/*          x est une matrice NORDRE (lignes) * NDIMEN (colonnes). */
/*  Dans ce programme, A et B sont stockes dans la matrice ABMATR dont */
/*  les lignes et les colonnes ont ete inversees. ABMATR(k,j) est le */
/*  terme A(j,k) si k <= NORDRE, B(j,k-NORDRE) sinon (cf. exemple). */

/*     MOTS CLES : */
/*     ----------- */
/* TOUS, MATH_ACCES::EQUATION&, MATRICE&, RESOLUTION, GAUSS, &SOLUTION */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*   NORMAX : Taille maximale du premier indice de XMATRI. Cet argument */
/*            ne sert que pour la declaration de dimension de XMATRI et */
/*            doit etre superieur ou egal a NORDRE. */
/*   NORDRE :  Ordre de la matrice i.e. nombre d'equations et */
/*             d'inconnues du systeme lineaire a resoudre. */
/*   NDIMEN : Nombre de second membre. */
/*   EPSPIV : Valeur minimale d'un pivot. Si au cours du calcul la */
/*            valeur absolue du pivot est inferieure a EPSPIV, le */
/*            systeme d'equations est declare singulier. EPSPIV doit */
/*            etre un "petit" reel. */

/*   ABMATR(NORDRE+NDIMEN,NORDRE) : Matrice auxiliaire contenant la */
/*                                  matrice A et la matrice B. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   XMATRI : Matrice contenant les NORDRE*NDIMEN solutions. */
/*   IERCOD=0 indique que toutes les solutions sont calculees. */
/*   IERCOD=1 indique que la matrice est de rang inferieur a NORDRE */
/*            (le systeme est singulier). */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION : les indices de ligne et de colonne sont inverses */
/*                 par rapport aux indices habituels. */
/*                 Le systeme : */
/*                        a1*x + b1*y = c1 */
/*                        a2*x + b2*y = c2 */
/*                 doit etre represente par la matrice ABMATR : */

/*                 ABMATR(1,1) = a1  ABMATR(1,2) = a2 */
/*                 ABMATR(2,1) = b1  ABMATR(2,2) = b2 */
/*                 ABMATR(3,1) = c1  ABMATR(3,2) = c2 */

/*     Pour resoudre ce systeme, il faut poser: */

/*                 NORDRE = 2 (il y a 2 equations a 2 inconnues), */
/*                 NDIMEN = 1 (il y a un seul second membre), */
/*                 NORMAX peut etre pris quelconque >= NORDRE. */

/*     Pour utiliser cette routine, il est conseille de se */
/*     servir de l'une des interfaces : MMRSLWI ou de MMMRSLWD. */

/*     HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*    24-11-1995 : JPI ; annulation des modifs concernant la factorisation
*/
/*                        de 1/PIVOT (Pb numerique) */
/*     08-09-1995 : JMF ; performances */
/*     06-04-1990 : RBD ; Ajout commentaires et Implicit none. */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     22-02-1988 : JJM ; Appel GERMSG -> MAERMSG */
/*     21-09-1987 : creation de la matrice unique ABMATR et des */
/*                  interfaces MMRSLWI et MMMRSLWD (RBD). */
/*     01-07-1987 : Cree par R. Beraud. */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

/*      INTEGER IBB,MNFNDEB */

/*      IBB=MNFNDEB() */
/*      IF (IBB.GE.2) CALL MGENMSG(NOMPR) */
    /* Parameter adjustments */
    xmatri_dim1 = *normax;
    xmatri_offset = xmatri_dim1 + 1;
    xmatri -= xmatri_offset;
    abmatr_dim1 = *nordre + *ndimen;
    abmatr_offset = abmatr_dim1 + 1;
    abmatr -= abmatr_offset;

    /* Function Body */
    *iercod = 0;

/* ********************************************************************* 
*/
/*                  Triangulation de la matrice ABMATR. */
/* ********************************************************************* 
*/

    i__1 = *nordre;
    for (kk = 1; kk <= i__1; ++kk) {

/* ---------- Recherche du pivot maxi sur la colonne KK. ------------
--- */

	pivot = *epspiv;
	kpiv = 0;
	i__2 = *nordre;
	for (jj = kk; jj <= i__2; ++jj) {
	    akj = (d__1 = abmatr[kk + jj * abmatr_dim1], abs(d__1));
	    if (akj > pivot) {
		pivot = akj;
		kpiv = jj;
	    }
/* L100: */
	}
	if (kpiv == 0) {
	    goto L9900;
	}

/* --------- Permutation de la ligne KPIV et avec la ligne KK. ------
--- */

	if (kpiv != kk) {
	    i__2 = *nordre + *ndimen;
	    for (jj = kk; jj <= i__2; ++jj) {
		akj = abmatr[jj + kk * abmatr_dim1];
		abmatr[jj + kk * abmatr_dim1] = abmatr[jj + kpiv * 
			abmatr_dim1];
		abmatr[jj + kpiv * abmatr_dim1] = akj;
/* L200: */
	    }
	}

/* -------------------- Elimination et triangularisation. -----------
--- */

	pivot = -abmatr[kk + kk * abmatr_dim1];
	i__2 = *nordre;
	for (ii = kk + 1; ii <= i__2; ++ii) {
	    akj = abmatr[kk + ii * abmatr_dim1] / pivot;
	    i__3 = *nordre + *ndimen;
	    for (jj = kk + 1; jj <= i__3; ++jj) {
		abmatr[jj + ii * abmatr_dim1] += akj * abmatr[jj + kk * 
			abmatr_dim1];
/* L400: */
	    }
/* L300: */
	}


/* L1000: */
    }

/* ********************************************************************* 
*/
/*          Resolution du systeme d'equations triangulaires. */
/*   La matrice ABMATR(NORDRE+JJ,II), contient les second membres du */
/*             systeme pour 1<=j<=NDIMEN et 1<=i<=NORDRE. */
/* ********************************************************************* 
*/


/* ---------------- Calcul des solutions en remontant. ----------------- 
*/

    for (kk = *nordre; kk >= 1; --kk) {
	pivot = abmatr[kk + kk * abmatr_dim1];
	i__1 = *ndimen;
	for (ii = 1; ii <= i__1; ++ii) {
	    akj = abmatr[ii + *nordre + kk * abmatr_dim1];
	    i__2 = *nordre;
	    for (jj = kk + 1; jj <= i__2; ++jj) {
		akj -= abmatr[jj + kk * abmatr_dim1] * xmatri[jj + ii * 
			xmatri_dim1];
/* L800: */
	    }
	    xmatri[kk + ii * xmatri_dim1] = akj / pivot;
/* L700: */
	}
/* L600: */
    }
    goto L9999;

/* ------Si la valeur absolue de l' un des pivot est plus petit -------- 
*/
/* ------------ que EPSPIV: recuperation du code d' erreur. ------------ 
*/

L9900:
    *iercod = 1;



L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMRSLW ", iercod, 7L);
    }
/*      IF (IBB.GE.2) CALL MGSOMSG(NOMPR) */
 return 0 ;
} /* mmrslw_ */
 
//=======================================================================
//function : AdvApp2Var_MathBase::mmmrslwd_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmmrslwd_(integer *normax, 
			     integer *nordre,
			     integer *ndim,
			     doublereal *amat, 
			     doublereal *bmat,
			     doublereal *epspiv, 
			     doublereal *aaux, 
			     doublereal *xmat, 
			     integer *iercod)

{
  /* System generated locals */
  integer amat_dim1, amat_offset, bmat_dim1, bmat_offset, xmat_dim1, 
  xmat_offset, aaux_dim1, aaux_offset, i__1, i__2;
  
  /* Local variables */
  static integer i__, j;
  static integer ibb;

/*      IMPLICIT DOUBLE PRECISION (A-H,O-Z) */
/*      IMPLICIT INTEGER (I-N) */


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Resolution d' un systeme lineaire par la methode de Gauss ou */
/*        le second membre est un tableau de vecteurs. Methode du pivot */
/*        partiel. */

/*     MOTS CLES : */
/*     ----------- */
/*        TOUS , MATH_ACCES :: */
/*        SYSTEME&,EQUATION&, RESOLUTION,GAUSS ,&VECTEUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NORMAX : Dimensionnement maxi de AMAT. */
/*        NORDRE :  Ordre de la matrice. */
/*        NDIM : Nombre de colonnes de BMAT et XMAT. */
/*        AMAT(NORMAX,NORDRE) : La matrice traitee. */
/*        BMAT(NORMAX,NDIM) : La matrice des second membre. */
/*        XMAT(NORMAX,NDIM) : La matrice des solutions. */
/*        EPSPIV : Valeur minimale d'un pivot. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        AAUX(NORDRE+NDIM,NORDRE) : Matrice auxiliaire. */
/*        XMAT(NORMAX,NDIM) : La matrice des solutions. */
/*        IERCOD=0 indique que les solutions dans XMAT sont valables. */
/*        IERCOD=1 indique que la matrice AMAT est de rang inferieur */
/*                 a NORDRE. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG              MGENMSG              MGSOMSG */
/*           MMRSLW          I*4  MNFNDEB */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*    ATTENTION :les lignes et les colonnes sont dans l' ordre */
/*               habituel : */
/*               1er indice  = indice ligne */
/*               2eme indice = indice colonne */
/*    Exemple, Le systeme : */
/*                 a1*x + b1*y = c1 */
/*                 a2*x + b2*y = c2 */
/*    est represente par la matrice AMAT : */

/*                 AMAT(1,1) = a1  AMAT(2,1) = a2 */
/*                 AMAT(1,2) = b1  AMAT(2,2) = b2 */

/*     Le premier indice est l' indice de ligne, le second indice */
/*     est l' indice des colonnes (Comparer avec MMRSLWI qui est */
/*     plus rapide). */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     11-09-1995 : JMF ; Implicit none */
/*     22-02-1988 : JJM ; Appel MFNDEB -> MNFNDEB */
/*     22-02-1988 : JJM ; Appel GERMSG -> MAERMSG */
/*     17-09-1987: Cree par RBD */
/* > */
/* ********************************************************************** 
*/

/*   Le nom de la routine */

    /* Parameter adjustments */
    amat_dim1 = *normax;
    amat_offset = amat_dim1 + 1;
    amat -= amat_offset;
    xmat_dim1 = *normax;
    xmat_offset = xmat_dim1 + 1;
    xmat -= xmat_offset;
    aaux_dim1 = *nordre + *ndim;
    aaux_offset = aaux_dim1 + 1;
    aaux -= aaux_offset;
    bmat_dim1 = *normax;
    bmat_offset = bmat_dim1 + 1;
    bmat -= bmat_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMMRSLW", 7L);
    }

/*   Initialisation de la matrice auxiliaire. */

    i__1 = *nordre;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *nordre;
	for (j = 1; j <= i__2; ++j) {
	    aaux[j + i__ * aaux_dim1] = amat[i__ + j * amat_dim1];
/* L200: */
	}
/* L100: */
    }

/*    Second membre. */

    i__1 = *nordre;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *ndim;
	for (j = 1; j <= i__2; ++j) {
	    aaux[j + *nordre + i__ * aaux_dim1] = bmat[i__ + j * bmat_dim1];
/* L400: */
	}
/* L300: */
    }

/*    Resolution du systeme d' equations. */

    mmrslw_(normax, nordre, ndim, epspiv, &aaux[aaux_offset], &xmat[
	    xmat_offset], iercod);


    if (*iercod != 0) {
	AdvApp2Var_SysBase::maermsg_("MMMRSLW", iercod, 7L);
    }
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMMRSLW", 7L);
    }
 return 0 ;
} /* mmmrslwd_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmrtptt_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmrtptt_(integer *ndglgd, 
			    doublereal *rtlegd)

{
  static integer ideb, nmod2, nsur2, ilong, ibb;


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*     Extrait du Common LDGRTL les racines STRICTEMENT positives du */
/*     polynome de Legendre de degre NDGLGD, pour 2 <= NDGLGD <= 61. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::COMMON&, EXTRACTION, &RACINE, &LEGENDRE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDGLGD : Degre mathematique du polynome de Legendre. */
/*                 Ce degre doit etre superieur ou egal a 2 et */
/*                 inferieur ou egal a 61. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        RTLEGD : Le tableau des racines strictement positives du */
/*                 polynome de Legendre de degre NDGLGD. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: La condition sur NDEGRE ( 2 <= NDEGRE <= 61) n'est */
/*     pas testee. A l'appelant de faire le test. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     23-03-1990 : RBD ; Ajout commentaires + declaration. */
/*     15-01-1990 : NAK ; MLGDRTL PAR INCLUDE MMLGDRT */
/*     21-04-1989 : RBD ; Creation. */
/* > */
/* ********************************************************************** 
*/
/*   Le nom de la routine */


/*   Le common MLGDRTL: */
/*   Ce common comprend les racines POSITIVES des polynomes de Legendre */
/*   ET les poids des formules de quadrature de Gauss sur toutes les */
/*   racines POSITIVES des polynomes de Legendre. */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*   Le common des racines de Legendre. */

/*     MOTS CLES : */
/*     ----------- */
/*        BASE LEGENDRE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     11-01-90 : NAK  ; Creation version originale */
/* > */
/* ***********************************************************************
 */




/*   ROOTAB : Tableau de toutes les racines des polynomes de Legendre */
/*   comprises entre ]0,1]. Elles sont rangees pour des degres croissants 
*/
/*   de 2 a 61. */
/*   HILTAB : Tableau des interpolants de Legendre concernant ROOTAB. */
/*   L' adressage est le meme. */
/*   HI0TAB : Tableau des interpolants de Legendre pour la racine x=0 */
/*   des polynomes de degre IMPAIR. */
/*   RTLTB0 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre PAIR. */
/*   RTLTB1 : Tableau des Li(uk) ou les uk sont les racines d' un */
/*  polynome de Legendre de degre IMPAIR. */


/************************************************************************
*****/
    /* Parameter adjustments */
    --rtlegd;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMRTPTT", 7L);
    }
    if (*ndglgd < 2) {
	goto L9999;
    }

    nsur2 = *ndglgd / 2;
    nmod2 = *ndglgd % 2;

    ilong = nsur2 << 3;
    ideb = nsur2 * (nsur2 - 1) / 2 + 1;
    AdvApp2Var_SysBase::mcrfill_((integer *)&ilong, 
	     (char *)&mlgdrtl_.rootab[ideb + nmod2 * 465 - 1], 
	     (char *)&rtlegd[1]);

/* ----------------------------- The end -------------------------------- 
*/

L9999:
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgsomsg_("MMRTPTT", 7L);
    }
    return 0;
} /* mmrtptt_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmsrre2_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmsrre2_(doublereal *tparam,
			    integer *nbrval, 
			    doublereal *tablev, 
			    doublereal *epsil, 
			    integer *numint, 
			    integer *itypen, 
			    integer *iercod)
{
  /* System generated locals */
  doublereal d__1;
  
  /* Local variables */
  static integer ideb, ifin, imil, ibb;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     -------- */

/*     Recherche l'intervalle correspondant a une valeur donnee dans */
/*     une suite croissante de reels double precision. */

/*     MOTS CLES : */
/*     --------- */
/*     TOUS,MATH_ACCES::TABLEAU&,POINT&,CORRESPONDANCE,&RANG */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */

/*     TPARAM  : Valeur a tester. */
/*     NBRVAL  : Taille de TABLEV */
/*     TABLEV  : Tableau de reels. */
/*     EPSIL   : Epsilon de precision */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */

/*     NUMINT  : Numero de l'intervalle (entre 1 et NBRVAL-1). */
/*     ITYPEN  : = 0 TPARAM est a l'interieur de l'intervalle NUMINT */
/*               = 1 : TPARAM correspond a la borne inferieure de */
/*                    l'intervalle fourni. */
/*               = 2 : TPARAM correspond a la borne superieure de */
/*                    l'intervalle fourni. */

/*     IERCOD : Code d'erreur */
/*                     = 0 : OK */
/*                     = 1 : TABLEV ne contient pas assez d' elements. */
/*                     = 2 : TPARAM hors des bornes de TABLEV. */

/*     COMMONS UTILISES : */
/*     ---------------- */

/*     REFERENCES APPELEES : */
/*     ------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     --------------------------------- */
/*     Il y a NBRVAL valeurs dans TABLEV soit NBRVAL-1 intervalles. */
/*     On fait une recherche de l' intervalle contenant TPARAM par */
/*     dichotomie. Complexite de l' algorithme : Log(n)/Log(2).(RBD). */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ---------------------------- */
/*     13-07-93 : MCL ; Version originale (a partir de MSRREI) */
/* > */
/* ***********************************************************************
 */


/* Initialisations */

    /* Parameter adjustments */
    --tablev;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 6) {
	AdvApp2Var_SysBase::mgenmsg_("MMSRRE2", 7L);
    }

    *iercod = 0;
    *numint = 0;
    *itypen = 0;
    ideb = 1;
    ifin = *nbrval;

/* TABLEV doit contenir au moins deux valeurs */

    if (*nbrval < 2) {
	*iercod = 1;
	goto L9999;
    }

/* TPARAM doit etre entre les bornes extremes de TABLEV. */

    if (*tparam < tablev[1] || *tparam > tablev[*nbrval]) {
	*iercod = 2;
	goto L9999;
    }

/* ----------------------- RECHERCHE DE L'INTERVALLE -------------------- 
*/

L1000:

/* Test de fin de boucle (on a trouve). */

    if (ideb + 1 == ifin) {
	*numint = ideb;
	goto L2000;
    }

/* Recherche par dichotomie sur les valeurs croissantes de TABLEV. */

    imil = (ideb + ifin) / 2;
    if (*tparam >= tablev[ideb] && *tparam <= tablev[imil]) {
	ifin = imil;
    } else {
	ideb = imil;
    }

    goto L1000;

/* -------------- TEST POUR VOIR SI TPARAM N'EST PAS UNE VALEUR --------- 
*/
/* ------------------------ DE TABLEV A EPSIL PRES ---------------------- 
*/

L2000:
    if ((d__1 = *tparam - tablev[ideb], abs(d__1)) < *epsil) {
	*itypen = 1;
	goto L9999;
    }
    if ((d__1 = *tparam - tablev[ifin], abs(d__1)) < *epsil) {
	*itypen = 2;
	goto L9999;
    }

/* --------------------------- THE END ---------------------------------- 
*/

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MMSRRE2", iercod, 7L);
    }
    if (ibb >= 6) {
	AdvApp2Var_SysBase::mgsomsg_("MMSRRE2", 7L);
    }
 return 0 ;
} /* mmsrre2_ */

//=======================================================================
//function : mmtmave_
//purpose  : 
//=======================================================================
int mmtmave_(integer *nligne, 
	     integer *ncolon, 
	     integer *gposit, 
	     integer *,//gnstoc, 
	     doublereal *gmatri,
	     doublereal *vecin, 
	     doublereal *vecout, 
	     integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static logical ldbg;
  static integer imin, imax, i__, j, k;
  static doublereal somme;
  static integer aux;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*                          t */
/*      EFFECUE LE PRODUIT   G V */
/*      OU LA MATRICE G EST SOUS FORME DE PROFIL */

/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, PRODUIT, MATRICE, PROFIL, VECTEUR */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       NLIGNE : NOMBRE DE LIGNE DE LA MATRICE */
/*       NCOLON : NOMBRE DE COLONNE DE LA MATRICE */
/*       GPOSIT: TABLE DE POSITIONNEMENT DES TERMES DE STOCKAGE */
/*               GPOSIT(1,I) CONTIENT LE NOMBRE DE TERMES-1 SUR LA LIGNE 
*/
/*               I DANS LE PROFIL DE LA MATRICE */
/*              GPOSIT(2,I) CONTIENT L'INDICE DE STOCKAGE DU TERME DIAGONA
L*/
/*               DE LA LIGNE I */
/*               GPOSIT(3,I) CONTIENT L'INDICE COLONE DU PREMIER TERME DU 
*/
/*                           PROFIL DE LA LIGNE I */
/*       GNSTOC : NOMBRE DE TERME DANS LE PROFIL DE GMATRI */
/*       GMATRI : MATRICE DES CONTRAINTES SOUS FORME DE PROFIL */
/*       VECIN : VECTEUR D'ENTRE */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*       VECOUT :VECTEUR PRODUIT */
/*       IERCOD : CODE D'ERREUR */


/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     21-08-95 : KHN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --vecin;
    gposit -= 4;
    --vecout;
    --gmatri;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMTMAVE", 7L);
    }
    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */



    i__1 = *ncolon;
    for (i__ = 1; i__ <= i__1; ++i__) {
	somme = 0.;
	i__2 = *nligne;
	for (j = 1; j <= i__2; ++j) {
	    imin = gposit[j * 3 + 3];
	    imax = gposit[j * 3 + 1] + gposit[j * 3 + 3] - 1;
	    aux = gposit[j * 3 + 2] - gposit[j * 3 + 1] - imin + 1;
	    if (imin <= i__ && i__ <= imax) {
		k = i__ + aux;
		somme += gmatri[k] * vecin[j];
	    }
	}
	vecout[i__] = somme;
    }





    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:

/* ___ DESALLOCATION, ... */

    AdvApp2Var_SysBase::maermsg_("MMTMAVE", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMTMAVE", 7L);
    }
 return 0 ;
} /* mmtmave_ */

//=======================================================================
//function : mmtrpj0_
//purpose  : 
//=======================================================================
int mmtrpj0_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew)

{
  /* System generated locals */
  integer crvlgd_dim1, crvlgd_offset, i__1, i__2;
  doublereal d__1;
  
  /* Local variables */
  static integer ncut, i__;
  static doublereal bidon, error;
  static integer nd;
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Baisse le degre d' une courbe definie sur (-1,1) au sens de */
/*        Legendre a une precision donnee. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,TRONCONNAGE,COURBE,LISSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Nbre maxi de coeff. de la courbe (dimensionnement). */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 du polynome. */
/*        EPSI3D  : La precision demandee pour l' approximation. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        EPSTRC : La precision de l' approximation. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/* 17-05-1991 : RBD ; Si le degre n'est pas baisse, l'erreur est nulle */
/* 12-12-1989 : RBD ; Creation. */
/* > */
/* ***********************************************************************
 */


/* ------- Degre minimum pouvant etre atteint : Arret a 1 (RBD) --------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */
    *ncfnew = 1;
/* ------------------- Init pour calcul d' erreur ----------------------- 
*/
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Coupure des coefficients. */

    ncut = 2;
/* ------ Boucle sur la serie de Legendre :NCOEFF --> 2 (RBD) ----------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Facteur de renormalisation. */
	bidon = ((i__ - 1) * 2. + 1.) / 2.;
	bidon = sqrt(bidon);
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   On arrete de couper si la norme devient trop grande. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L9999;
	}

/* --- Erreur max cumulee lorsque le I-eme coeff est ote. */

	*epstrc = error;

/* L300: */
    }

/* --------------------------------- Fin -------------------------------- 
*/

L9999:
    return 0;
} /* mmtrpj0_ */

//=======================================================================
//function : mmtrpj2_
//purpose  : 
//=======================================================================
int mmtrpj2_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew)

{
    /* Initialized data */

    static doublereal xmaxj[57] = { .9682458365518542212948163499456,
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

    /* System generated locals */
    integer crvlgd_dim1, crvlgd_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer ncut, i__;
    static doublereal bidon, error;
    static integer ia, nd;
    static doublereal bid, eps1;


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Baisse le degre d' une courbe definie sur (-1,1) au sens de */
/*        Legendre a une precision donnee. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,TRONCONNAGE,COURBE,LISSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Nbre maxi de coeff. de la courbe (dimensionnement). */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 du polynome. */
/*        EPSI3D : La precision demandee pour l' approximation. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        EPSTRC : La precision de l' approximation. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/* 17-05-1991 : RBD ; Si le degre n'est pas baisse, l'erreur est nulle */
/* 15-01-1991 : RBD ; Correction coupure des coeff. nuls du polynome */
/*                    d' interpolation. */
/* 12-12-1989 : RBD ; Creation. */

/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Degre minimum pouvant etre atteint : Arret a IA (RBD). ------------- 
*/
    ia = 2;
    *ncfnew = ia;
/* Init pour calcul d' erreur. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Coupure des coefficients. */

    ncut = ia + 1;
/* ------ Boucle sur la serie de Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   On arrete de couper si la norme devient trop grande. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* --- Erreur max cumulee lorsque le I-eme coeff est ote. */

	*epstrc = error;

/* L300: */
    }

/* ------- Coupure des coeff. nuls du pol. d' interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- Si tous les coeff peuvent etre otes, c'est un point. */
	*ncfnew = 1;
    }

/* --------------------------------- Fin -------------------------------- 
*/

L9999:
    return 0;
} /* mmtrpj2_ */

//=======================================================================
//function : mmtrpj4_
//purpose  : 
//=======================================================================
int mmtrpj4_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew)
{
    /* Initialized data */

    static doublereal xmaxj[55] = { 1.1092649593311780079813740546678,
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

    /* System generated locals */
    integer crvlgd_dim1, crvlgd_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer ncut, i__;
    static doublereal bidon, error;
    static integer ia, nd;
    static doublereal bid, eps1;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Baisse le degre d' une courbe definie sur (-1,1) au sens de */
/*        Legendre a une precision donnee. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,TRONCONNAGE,COURBE,LISSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Nbre maxi de coeff. de la courbe (dimensionnement). */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 du polynome. */
/*        EPSI3D  : La precision demandee pour l' approximation. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        EPSTRC : La precision de l' approximation. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/* 17-05-1991 : RBD ; Si le degre n'est pas baisse, l'erreur est nulle */
/* 15-01-1991 : RBD ; Correction coupure des coeff. nuls du polynome */
/*                    d' interpolation. */
/* 12-12-1989 : RBD ; Creation. */

/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Degre minimum pouvant etre atteint : Arret a IA (RBD). ------------- 
*/
    ia = 4;
    *ncfnew = ia;
/* Init pour calcul d' erreur. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Coupure des coefficients. */

    ncut = ia + 1;
/* ------ Boucle sur la serie de Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   On arrete de couper si la norme devient trop grande. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* --- Erreur max cumulee lorsque le I-eme coeff est ote. */

	*epstrc = error;

/* L300: */
    }

/* ------- Coupure des coeff. nuls du pol. d' interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- Si tous les coeff peuvent etre otes, c'est un point. */
	*ncfnew = 1;
    }

/* --------------------------------- Fin -------------------------------- 
*/

L9999:
    return 0;
} /* mmtrpj4_ */

//=======================================================================
//function : mmtrpj6_
//purpose  : 
//=======================================================================
int mmtrpj6_(integer *ncofmx,
	     integer *ndimen, 
	     integer *ncoeff, 
	     doublereal *epsi3d, 
	     doublereal *crvlgd, 
	     doublereal *ycvmax, 
	     doublereal *epstrc, 
	     integer *ncfnew)

{
    /* Initialized data */

    static doublereal xmaxj[53] = { 1.21091229812484768570102219548814,
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
    integer crvlgd_dim1, crvlgd_offset, i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer ncut, i__;
    static doublereal bidon, error;
    static integer ia, nd;
    static doublereal bid, eps1;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Baisse le degre d' une courbe definie sur (-1,1) au sens de */
/*        Legendre a une precision donnee. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,TRONCONNAGE,COURBE,LISSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Nbre maxi de coeff. de la courbe (dimensionnement). */
/*        NDIMEN   : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 du polynome. */
/*        EPSI3D  : La precision demandee pour l' approximation. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        YCVMAX : Tableau auxiliaire (erreur max sur chaque dimension). 
*/
/*        EPSTRC : La precision de l' approximation. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/* 17-05-1991 : RBD ; Si le degre n'est pas baisse, l'erreur est nulle */
/* 15-01-1991 : RBD ; Correction coupure des coeff. nuls du polynome */
/*                    d' interpolation. */
/* 12-12-1989 : RBD ; Creation. */

/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Degre minimum pouvant etre atteint : Arret a IA (RBD). ------------- 
*/
    ia = 6;
    *ncfnew = ia;
/* Init pour calcul d' erreur. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Coupure des coefficients. */

    ncut = ia + 1;
/* ------ Boucle sur la serie de Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Facteur de renormalisation. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   On arrete de couper si la norme devient trop grande. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* --- Erreur max cumulee lorsque le I-eme coeff est ote. */

	*epstrc = error;

/* L300: */
    }

/* ------- Coupure des coeff. nuls du pol. d' interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- Si tous les coeff peuvent etre otes, c'est un point. */
	*ncfnew = 1;
    }

/* --------------------------------- Fin -------------------------------- 
*/

L9999:
    return 0;
} /* mmtrpj6_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmtrpjj_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmtrpjj_(integer *ncofmx, 
			    integer *ndimen, 
			    integer *ncoeff, 
			    doublereal *epsi3d, 
			    integer *iordre, 
			    doublereal *crvlgd, 
			    doublereal *ycvmax, 
			    doublereal *errmax, 
			    integer *ncfnew)
{
    /* System generated locals */
    integer crvlgd_dim1, crvlgd_offset;

    /* Local variables */
    static integer ia;
   

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Baisse le degre d' une courbe definie sur (-1,1) au sens de */
/*        Legendre a une precision donnee. */

/*     MOTS CLES : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,TRONCONNAGE,COURBE,LISSAGE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOFMX : Nbre maxi de coeff. de la courbe (dimensionnement). */
/*        NDIMEN : Dimension de l' espace. */
/*        NCOEFF : Le degre +1 du polynome. */
/*        EPSI3D : La precision demandee pour l' approximation. */
/*        IORDRE : Ordre de continuite aux extremites. */
/*        CRVLGD : La courbe dont on veut baisser le degre. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        ERRMAX : La precision de l' approximation. */
/*        NCFNEW : Le degre +1 du polynome resultat. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     06-08-91 : RBD; Declaration de dimension de YCVMAX. */
/*     18-01-90 : RBD; Creation. */

/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */
    ia = (*iordre + 1) << 1;

    if (ia == 0) {
	mmtrpj0_(ncofmx, ndimen, ncoeff, epsi3d, &crvlgd[crvlgd_offset], &
		ycvmax[1], errmax, ncfnew);
    } else if (ia == 2) {
	mmtrpj2_(ncofmx, ndimen, ncoeff, epsi3d, &crvlgd[crvlgd_offset], &
		ycvmax[1], errmax, ncfnew);
    } else if (ia == 4) {
	mmtrpj4_(ncofmx, ndimen, ncoeff, epsi3d, &crvlgd[crvlgd_offset], &
		ycvmax[1], errmax, ncfnew);
    } else {
	mmtrpj6_(ncofmx, ndimen, ncoeff, epsi3d, &crvlgd[crvlgd_offset], &
		ycvmax[1], errmax, ncfnew);
    }

/* ------------------------ Fin ----------------------------------------- 
*/

    return 0;
} /* mmtrpjj_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmunivt_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmunivt_(integer *ndimen, 
	     doublereal *vector, 
	     doublereal *vecnrm, 
	     doublereal *epsiln, 
	     integer *iercod)
{
 
  static doublereal c_b2 = 10.;
  
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    static integer nchif, iunit, izero;
    static doublereal vnorm;
    static integer ii;
    static doublereal bid;
    static doublereal eps0;




/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        CALCUL DU VECTEUR NORME A PARTIR D'UN VECTEUR QUELCONQUE */
/*        AVEC UNE PRECISION DONNEE PAR L' UTILISATEUR. */

/*     MOTS CLES : */
/*     ----------- */
/*        TOUS, MATH_ACCES :: */
/*        VECTEUR&, NORMALISATION, &VECTEUR */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN   : DIMENSION DE L'ESPACE */
/*        VECTOR : VECTEUR A NORMER */
/*        EPSILN : L' EPSILON EN DESSOUS DUQUEL ON CONSIDERE QUE LA */
/*                 NORME DU VECTEUR EST NULLE. SI EPSILN<=0, UNE VALEUR */
/*                 PAR DEFAUT EST IMPOSEE (10.D-17 SUR VAX). */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        VECNRM : VECTEUR NORME */
/*        IERCOD  101 : LE VECTEUR EST NUL A EPSILN PRES. */
/*                  0 : OK. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     VECTOR et VECNRM peuvent etre identiques. */

/*     On calcule la norme du vecteur et on divise chaque composante par 
*/
/*     cette norme. Apres cela on verifie si toutes les composantes du */
/*     vecteur sauf une vaut 0 a la precision machine pres. Dans */
/*     ce cas on met les composantes quasi-nulles a 0.D0. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     14-12-90 : RBD; Correction cas ou une seule composante est */
/*                     significative, appel a MAOVSR8 pour la precision */
/*                     machine. */
/*     11-01-89 : RBD; Correction precision par defaut. */
/*     05-10-88 : RBD; Creation d' apres UNITVT. */
/*     23-01-85 : DH ; Creation version originale de UNITVT. */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --vecnrm;
    --vector;

    /* Function Body */
    *iercod = 0;

/* -------- Precision par defaut : le zero machine 10.D-17 sur Vax ------ 
*/

    AdvApp2Var_SysBase::maovsr8_(&nchif);
    if (*epsiln <= 0.) {
	i__1 = -nchif;
	eps0 = AdvApp2Var_MathBase::pow__di(&c_b2, &i__1);
    } else {
	eps0 = *epsiln;
    }

/* ----------------------------- Calcul de la norme --------------------- 
*/

    vnorm = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vector[1]);
    if (vnorm <= eps0) {
	AdvApp2Var_SysBase::mvriraz_((integer *)ndimen, (char *)&vecnrm[1]);
	*iercod = 101;
	goto L9999;
    }

/* ---------------------- Calcul du vecteur norme ----------------------- 
*/

    izero = 0;
    i__1 = (-nchif - 1) / 2;
    eps0 = AdvApp2Var_MathBase::pow__di(&c_b2, &i__1);
    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	vecnrm[ii] = vector[ii] / vnorm;
	if ((d__1 = vecnrm[ii], abs(d__1)) <= eps0) {
	    ++izero;
	} else {
	    iunit = ii;
	}
/* L20: */
    }

/* ------ Cas ou toutes les coordonnees sauf une sont presque nulles ---- 
*/
/* ------------- alors l' une des coordonnees vaut 1.D0 ou -1.D0 -------- 
*/

    if (izero == *ndimen - 1) {
	bid = vecnrm[iunit];
	i__1 = *ndimen;
	for (ii = 1; ii <= i__1; ++ii) {
	    vecnrm[ii] = 0.;
/* L30: */
	}
	if (bid > 0.) {
	    vecnrm[iunit] = 1.;
	} else {
	    vecnrm[iunit] = -1.;
	}
    }

/* -------------------------------- The end ----------------------------- 
*/

L9999:
    return 0;
} /* mmunivt_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmveps3_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmveps3_(doublereal *eps03)
{
  /* Initialized data */
  
  static char nomprg[8+1] = "MMEPS1  ";
  
  static integer ibb;
  


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*        Extraction du EPS1 du COMMON MPRCSN. */

/*     MOTS CLES : */
/*     ----------- */
/*        MPRCSN,PRECISON,EPS3. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*       Humm. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        EPS3 : Le zero spatial du denominateur (10**-9) */
/*       EPS3 devrait valoir 10**-15 */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*        08-01-90 : ACS ; MPRCSN REMPLACE PAR INCLUDE */
/*        21-01-1988: JJM ; Creation. */

/* > */
/* ***********************************************************************
 */



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*          DONNE LES TOLERANCES DE NULLITE DANS STRIM */
/*          AINSI QUE LES BORNES DES PROCESSUS ITERATIFS */

/*          CONTEXTE GENERAL, MODIFIABLE PAR L'UTILISATEUR */

/*     MOTS CLES : */
/*     ----------- */
/*          PARAMETRE , TOLERANCE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  PROFIL , **VIA MPRFTX** A L' ENTREE DANS STRI
M*/

/*       CHARGEMENT DES VALEURS PAR DEFAUT DU PROFIL DANS MPRFTX A L'ENTRE
E*/
/*        DANS STRIM. ELLES SONT CONSERVEES DANS DES VARIABLES LOCALES */
/*        DE MPRFTX */

/*        REMISE DES VALEURS PAR DEFAUT                  : MDFINT */
/*        MODIFICATION INTERACTIVE  PAR L'UTILISATEUR    : MDBINT */

/*        FONCTION D'ACCES :  MMEPS1   ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*      01-02-90 : NAK  ; ENTETE */
/* > */
/* ***********************************************************************
 */

/*     NITERM : NB D'ITERATIONS MAXIMAL */
/*     NITERR : NB D'ITERATIONS RAPIDES */
/*     EPS1   : TOLERANCE DE DISTANCE 3D NULLE */
/*     EPS2   : TOLERANCE DE DISTANCE PARAMETRIQUE NULLE */
/*     EPS3   : TOLERANCE POUR EVITER LES DIVISIONS PAR 0.. */
/*     EPS4   : TOLERANCE ANGULAIRE */



/* ***********************************************************************
 */

    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 5) {
	AdvApp2Var_SysBase::mgenmsg_(nomprg, 6L);
    }

    *eps03 = mmprcsn_.eps3;

    return 0;
} /* mmveps3_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmvncol_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mmvncol_(integer *ndimen, 
			    doublereal *vecin, 
			    doublereal *vecout, 
			    integer *iercod)

{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static logical ldbg;
  static integer d__;
  static doublereal vaux1[3], vaux2[3];
  static logical colin;
  static doublereal valaux;
  static integer aux;
  static logical nul;
 
/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*       CALCUL UN  VECTEUR NON COLINEAIRE A UN VECTEUR DONNEE */
/*       NON NUL */

/*     MOTS CLES : */
/*     ----------- */
/*      PUBLIC, VECTEUR, LIBRE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*       ndimen :dimension de l'espace */
/*       vecin  :vecteur entre */


/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */

/*       vecout : vecteur non colineaire a vecin */
/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */


/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     25-08-95 : KHN; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    /* Parameter adjustments */
    --vecout;
    --vecin;

    /* Function Body */
    ldbg = AdvApp2Var_SysBase::mnfndeb_() >= 2;
    if (ldbg) {
	AdvApp2Var_SysBase::mgenmsg_("MMVNCOL", 7L);
    }
    *iercod = 0;

/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

    if (*ndimen <= 1 || *ndimen > 3) {
	goto L9101;
    }
    nul = FALSE_;
    d__ = 1;
    aux = 0;
    while(d__ <= *ndimen) {
	if (vecin[d__] == 0.) {
	    ++aux;
	}
	++d__;
    }
    if (aux == *ndimen) {
	goto L9101;
    }


    for (d__ = 1; d__ <= 3; ++d__) {
	vaux1[d__ - 1] = 0.;
    }
    i__1 = *ndimen;
    for (d__ = 1; d__ <= i__1; ++d__) {
	vaux1[d__ - 1] = vecin[d__];
	vaux2[d__ - 1] = vecin[d__];
    }
    colin = TRUE_;
    d__ = 0;
    while(colin) {
	++d__;
	if (d__ > 3) {
	    goto L9101;
	}
	vaux2[d__ - 1] += 1;
	valaux = vaux1[1] * vaux2[2] - vaux1[2] * vaux2[1];
	if (valaux == 0.) {
	    valaux = vaux1[2] * vaux2[0] - vaux1[0] * vaux2[2];
	    if (valaux == 0.) {
		valaux = vaux1[0] * vaux2[1] - vaux1[1] * vaux2[0];
		if (valaux != 0.) {
		    colin = FALSE_;
		}
	    } else {
		colin = FALSE_;
	    }
	} else {
	    colin = FALSE_;
	}
    }
    if (colin) {
	goto L9101;
    }
    i__1 = *ndimen;
    for (d__ = 1; d__ <= i__1; ++d__) {
	vecout[d__] = vaux2[d__ - 1];
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   TRAITEMENT DES ERREURS */
/* ***********************************************************************
 */


L9101:
    *iercod = 1;
    goto L9999;


/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

L9999:


    AdvApp2Var_SysBase::maermsg_("MMVNCOL", iercod, 7L);
    if (ldbg) {
	AdvApp2Var_SysBase::mgsomsg_("MMVNCOL", 7L);
    }
 return 0 ;
} /* mmvncol_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mmwprcs_
//purpose  : 
//=======================================================================
void AdvApp2Var_MathBase::mmwprcs_(doublereal *epsil1, 
				   doublereal *epsil2, 
				   doublereal *epsil3, 
				   doublereal *epsil4, 
				   integer *niter1, 
				   integer *niter2)

{


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     ACCES EN ECRITURE POUR LE COMMON MPRCSN */

/*     MOTS CLES : */
/*     ----------- */
/*     ECRITURE */

/*     ARGUMENTS D'ENTREE : */
/*     -------------------- */
/*     EPSIL1  : TOLERANCE DE DISTANCE 3D NULLE */
/*     EPSIL2  : TOLERANCE DE DISTANCE PARAMETRIQUE NULLE */
/*     EPSIL3  : TOLERANCE POUR EVITER LES DIVISIONS PAR 0.. */
/*     EPSIL4  : TOLERANCE ANGULAIRE */
/*     NITER1  : NB D'ITERATIONS MAXIMAL */
/*     NITER2  : NB D'ITERATIONS RAPIDES */

/*     ARGUMENTS DE SORTIE : */
/*     --------------------- */
/*     NEANT */

/*     COMMONS UTILISES : */
/*     ------------------ */


/*     REFERENCES APPELEES : */
/*     --------------------- */


/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     13-05-96 :  JPI; ECRITURE VERSION ORIGINALE. */
/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

/* ***********************************************************************
 */
/*                      TRAITEMENT */
/* ***********************************************************************
 */

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*          DONNE LES TOLERANCES DE NULLITE DANS STRIM */
/*          AINSI QUE LES BORNES DES PROCESSUS ITERATIFS */

/*          CONTEXTE GENERAL, MODIFIABLE PAR L'UTILISATEUR */

/*     MOTS CLES : */
/*     ----------- */
/*          PARAMETRE , TOLERANCE */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  PROFIL , **VIA MPRFTX** A L' ENTREE DANS STRI
M*/

/*       CHARGEMENT DES VALEURS PAR DEFAUT DU PROFIL DANS MPRFTX A L'ENTRE
E*/
/*        DANS STRIM. ELLES SONT CONSERVEES DANS DES VARIABLES LOCALES */
/*        DE MPRFTX */

/*        REMISE DES VALEURS PAR DEFAUT                  : MDFINT */
/*        MODIFICATION INTERACTIVE  PAR L'UTILISATEUR    : MDBINT */

/*        FONCTION D'ACCES :  MMEPS1   ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*      01-02-90 : NAK  ; ENTETE */
/* > */
/* ***********************************************************************
 */

/*     NITERM : NB D'ITERATIONS MAXIMAL */
/*     NITERR : NB D'ITERATIONS RAPIDES */
/*     EPS1   : TOLERANCE DE DISTANCE 3D NULLE */
/*     EPS2   : TOLERANCE DE DISTANCE PARAMETRIQUE NULLE */
/*     EPS3   : TOLERANCE POUR EVITER LES DIVISIONS PAR 0.. */
/*     EPS4   : TOLERANCE ANGULAIRE */



/* ***********************************************************************
 */
    mmprcsn_.eps1 = *epsil1;
    mmprcsn_.eps2 = *epsil2;
    mmprcsn_.eps3 = *epsil3;
    mmprcsn_.eps4 = *epsil4;
    mmprcsn_.niterm = *niter1;
    mmprcsn_.niterr = *niter2;
 return ;
} /* mmwprcs_  */


//=======================================================================
//function : AdvApp2Var_MathBase::pow__di
//purpose  : 
//=======================================================================
 doublereal AdvApp2Var_MathBase::pow__di (doublereal *x,
				   integer *n)
{

  register integer ii ;
  doublereal result ;
  integer    absolute ;
  result = 1.0e0 ;
  if ( *n > 0 ) {absolute = *n;}
  else {absolute = -*n;}
    /* System generated locals */
  for(ii = 0 ; ii < absolute ; ii++) {
      result *=  *x ;
   }
  if (*n < 0) {
   result = 1.0e0 / result ;
 }
 return result ;
}
   

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Calcul la fonction puissance entiere pas forcement de la maniere
	  la plus efficace ; 
*/

/*     MOTS CLES : */
/*     ----------- */
/*       PUISSANCE */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        X      :  argument de X**N */
/*        N      :  puissance */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        retourne X**N */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     16-10-95 : XAB ; Creation */
/* > */
/* ***********************************************************************/

//=======================================================================
//function : pow__ii
//purpose  : 
//=======================================================================
integer pow__ii(integer *x, 
		integer *n)

{
  register integer ii ;
  integer result ;
  integer    absolute ;
  result = 1 ;
  if ( *n > 0 ) {absolute = *n;}
  else {absolute = -*n;}
    /* System generated locals */
  for(ii = 0 ; ii < absolute ; ii++) {
      result *=  *x ;
   }
  if (*n < 0) {
   result = 1 / result ;
 }
 return result ;
}
   

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        Calcul la fonction puissance entiere pas forcement de la maniere
	  la plus efficace ; 
*/

/*     MOTS CLES : */
/*     ----------- */
/*       PUISSANCE   */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        X      :  argument de X**N */
/*        N      :  puissance */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        retourne X**N */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     16-10-95 : XAB ; Creation */
/* > */
/* ***********************************************************************/

//=======================================================================
//function : AdvApp2Var_MathBase::msc_
//purpose  : 
//=======================================================================
 doublereal AdvApp2Var_MathBase::msc_(integer *ndimen, 
			       doublereal *vecte1, 
			       doublereal *vecte2)

{
  /* System generated locals */
  integer i__1;
  doublereal ret_val;
  
  /* Local variables */
  static integer i__;
  static doublereal x;
  


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*        Calcul du produit scalaire de 2 vecteurs dans l' espace */
/*        de dimension NDIMEN. */

/*     MOTS CLES : */
/*     ----------- */
/*        PRODUIT MSCALAIRE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN : Dimension de l' espace. */
/*        VECTE1,VECTE2: Les vecteurs. */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ----------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*        18-07-1988: RBD ; Changement de nom des arguments pour plus */
/*                          de lisibilite. */
/*        16-01-1987: Verification implicite NDIMEN >0 JJM. */

/* > */
/* ***********************************************************************
 */


/*     PRODUIT MSCALAIRE */
    /* Parameter adjustments */
    --vecte2;
    --vecte1;

    /* Function Body */
    x = 0.;

    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x += vecte1[i__] * vecte2[i__];
/* L100: */
    }
    ret_val = x;

/* ----------------------------------- THE END -------------------------- 
*/

    return ret_val;
} /* msc_ */

//=======================================================================
//function : mvcvin2_
//purpose  : 
//=======================================================================
int mvcvin2_(integer *ncoeff, 
	     doublereal *crvold, 
	     doublereal *crvnew,
	     integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static integer m1jm1, ncfm1, j, k;
  static doublereal bid;
  static doublereal cij1, cij2;
  


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*        INVERSION DU PARAMETRAGE SUR UNE CRBE 2D. */

/*     MOTS CLES : */
/*     ----------- */
/*        COURBE,2D,INVERSION,PARAMETRE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOEFF   : NBRE DE COEFF DE LA COURBE. */
/*        CRVOLD   : LA COURBE D'ORIGINE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CRVNEW   : LA CRBE RESULTAT APRES CHANGT DE T EN 1-T */
/*        IERCOD   :  0 OK, */
/*                   10 NBRE DE COEFF NUL OU TROP GRAND. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*    MCCNP */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*            Neant */
/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     L' APPEL SUIVANT EST TOUT A FAIT LEGAL : */
/*          CALL MVCVIN2(NCOEFF,CURVE,CURVE,IERCOD), LE TABLEAU CURVE */
/*     DEVENANT UN ARGUMENT D' ENTREE ET DE SORTIE (RBD). */
/*     A CAUSE DE MCCNP, LE NBRE DE COEFF DE LA COURBE EST LIMITE A */
/*     NDGCNP+1 = 61. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     24-09-93 : MPS ; PRISE EN COMPTE NCOEFF=1 */
/*                      IMPLICIT NONE */
/*     09-01-90 : TE ; COMMON MCCNP -> MCNCNP.INC & INDICES DES CNP */
/*     05-08-88 : RBD ; ACTIVATION DE L' IERCOD */
/*     27-06-88 : RBD ; VERIFICATION QUE LES IDENTIFICATEURS CRVNEW ET */
/*                      CRVOLD PEUVENT DESIGNER LA MEME COURBE. */
/*     14-04-88 : NAK ; VERSION ORIGINALE */
/* > */
/* ***********************************************************************
 */


/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a fournir les coefficients du binome (triangle de Pascal). */

/*     MOTS CLES : */
/*     ----------- */
/*      Coeff du binome de 0 a 60. read only . init par block data */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les coefficients du binome forment une matrice triangulaire. */
/*     On complete cette matrice dans le tableau CNP par sa transposee. */
/*     On a donc: CNP(I,J) = CNP(J,I) pour I et J = 0, ..., 60. */

/*     L'initialisation est faite a partir du block-data MMLLL09.RES, */
/*     cree par le programme MQINICNP.FOR (voir l'equipe (AC) ). */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     03-07-90 : RBD; Ajout commentaires (nom du block-data). */
/*     19-06-90 : RBD; Le commun MMCMCNP remplace MCCNP qui est obsolete. 
*/
/*     08-01-90 : TE ; CREATION */
/* > */
/* ********************************************************************** 
*/



/* ***********************************************************************
 */

    /* Parameter adjustments */
    crvnew -= 3;
    crvold -= 3;

    /* Function Body */
    if (*ncoeff < 1 || *ncoeff - 1 > 60) {
	*iercod = 10;
	goto L9999;
    }
    *iercod = 0;


/* TERME CONSTANT DE LA NOUVELLE COURBE */

    cij1 = crvold[3];
    cij2 = crvold[4];
    i__1 = *ncoeff;
    for (k = 2; k <= i__1; ++k) {
	cij1 += crvold[(k << 1) + 1];
	cij2 += crvold[(k << 1) + 2];
    }
    crvnew[3] = cij1;
    crvnew[4] = cij2;
    if (*ncoeff == 1) {
	goto L9999;
    }

/* PUISSANCES INTERMEDIAIRES DU PARAMETRE */

    ncfm1 = *ncoeff - 1;
    m1jm1 = 1;
    i__1 = ncfm1;
    for (j = 2; j <= i__1; ++j) {
	m1jm1 = -m1jm1;
	cij1 = crvold[(j << 1) + 1];
	cij2 = crvold[(j << 1) + 2];
	i__2 = *ncoeff;
	for (k = j + 1; k <= i__2; ++k) {
	    bid = mmcmcnp_.cnp[k - 1 + (j - 1) * 61];
	    cij1 += crvold[(k << 1) + 1] * bid;
	    cij2 += crvold[(k << 1) + 2] * bid;
	}
	crvnew[(j << 1) + 1] = cij1 * m1jm1;
	crvnew[(j << 1) + 2] = cij2 * m1jm1;
    }

/* TERME DE PLUS HAUT DEGRE */

    crvnew[(*ncoeff << 1) + 1] = -crvold[(*ncoeff << 1) + 1] * m1jm1;
    crvnew[(*ncoeff << 1) + 2] = -crvold[(*ncoeff << 1) + 2] * m1jm1;

L9999:
    if (*iercod > 0) {
	AdvApp2Var_SysBase::maermsg_("MVCVIN2", iercod, 7L);
    }
 return 0 ;
} /* mvcvin2_ */

//=======================================================================
//function : mvcvinv_
//purpose  : 
//=======================================================================
int mvcvinv_(integer *ncoeff,
	     doublereal *crvold, 
	     doublereal *crvnew, 
	     integer *iercod)

{
  /* System generated locals */
  integer i__1, i__2;
  
  /* Local variables */
  static integer m1jm1, ncfm1, j, k;
  static doublereal bid;
  //extern /* Subroutine */ int maermsg_();
  static doublereal cij1, cij2, cij3;
  
 
/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*        INVERSION DU PARAMETRAGE SUR UNE CRBE 3D (I.E. INVERSION DU */
/*        SENS DE PARCOURS). */

/*     MOTS CLES : */
/*     ----------- */
/*        COURBE,INVERSION,PARAMETRE. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NCOEFF   : NBRE DE COEFF DE LA COURBE. */
/*        CRVOLD   : lA COURBE D'ORIGINE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        CRVNEW   : LA CRBE RESULTAT APRES CHANGT DE T EN 1-T */
/*        IERCOD   :  0 OK, */
/*                   10 NBRE DE COEFF NUL OU TROP GRAND. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*    MCCNP */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*            Neant */
/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     L' APPEL SUIVANT EST TOUT A FAIT LEGAL : */
/*          CALL MVCVINV(NCOEFF,CURVE,CURVE,IERCOD), LE TABLEAU CURVE */
/*     DEVENANT UN ARGUMENT D' ENTREE ET DE SORTIE (RBD). */
/*     LE NOMBRE DE COEFF DE LA COURBE EST LIMITE A NDGCNP+1 = 61 */
/*     A CAUSE DE L' UTILISATION DU COMMUN MCCNP. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     10-05-90 : JG ; NCOEFF=1 n'etait pas gere */
/*     09-01-90 : TE ; COMMON MCCNP -> MMCMCNP.INC & INDICES DES CNP */
/*     05-08-88 : RBD ; ACTIVATION DE L' IERCOD */
/*     27-06-88 : RBD ; VERIFICATION QUE LES IDENTIFICATEURS CRVNEW ET */
/*                      CRVOLD PEUVENT DESIGNER LA MEME COURBE. */
/*     02-03-87 : NAK ; BRSTN --> MCCNP */
/*     01-10-86 : NAK ; PRISE EN COMPTE LES ISOS DE LA FORME 1-T */
/* > */
/* ***********************************************************************
 */

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*      Sert a fournir les coefficients du binome (triangle de Pascal). */

/*     MOTS CLES : */
/*     ----------- */
/*      Coeff du binome de 0 a 60. read only . init par block data */

/*     DEMSCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Les coefficients du binome forment une matrice triangulaire. */
/*     On complete cette matrice dans le tableau CNP par sa transposee. */
/*     On a donc: CNP(I,J) = CNP(J,I) pour I et J = 0, ..., 60. */

/*     L'initialisation est faite a partir du block-data MMLLL09.RES, */
/*     cree par le programme MQINICNP.FOR (voir l'equipe (AC) ). */

/* $    HISTORIQUE DES MODIFICATIONS : */
/*     ------------------------------ */
/*     03-07-90 : RBD; Ajout commentaires (nom du block-data). */
/*     19-06-90 : RBD; Le commun MMCMCNP remplace MCCNP qui est obsolete. 
*/
/*     08-01-90 : TE ; CREATION */
/* > */
/* ********************************************************************** 
*/



/* ***********************************************************************
 */

    /* Parameter adjustments */
    crvnew -= 4;
    crvold -= 4;

    /* Function Body */
    if (*ncoeff < 1 || *ncoeff - 1 > 60) {
	*iercod = 10;
	goto L9999;
    }
    *iercod = 0;

/* TERME CONSTANT DE LA NOUVELLE COURBE */

    cij1 = crvold[4];
    cij2 = crvold[5];
    cij3 = crvold[6];
    i__1 = *ncoeff;
    for (k = 2; k <= i__1; ++k) {
	cij1 += crvold[k * 3 + 1];
	cij2 += crvold[k * 3 + 2];
	cij3 += crvold[k * 3 + 3];
/* L30: */
    }
    crvnew[4] = cij1;
    crvnew[5] = cij2;
    crvnew[6] = cij3;
    if (*ncoeff == 1) {
	goto L9999;
    }

/* PUISSANCES INTERMEDIAIRES DU PARAMETRE */

    ncfm1 = *ncoeff - 1;
    m1jm1 = 1;
    i__1 = ncfm1;
    for (j = 2; j <= i__1; ++j) {
	m1jm1 = -m1jm1;
	cij1 = crvold[j * 3 + 1];
	cij2 = crvold[j * 3 + 2];
	cij3 = crvold[j * 3 + 3];
	i__2 = *ncoeff;
	for (k = j + 1; k <= i__2; ++k) {
	    bid = mmcmcnp_.cnp[k - 1 + (j - 1) * 61];
	    cij1 += crvold[k * 3 + 1] * bid;
	    cij2 += crvold[k * 3 + 2] * bid;
	    cij3 += crvold[k * 3 + 3] * bid;
/* L40: */
	}
	crvnew[j * 3 + 1] = cij1 * m1jm1;
	crvnew[j * 3 + 2] = cij2 * m1jm1;
	crvnew[j * 3 + 3] = cij3 * m1jm1;
/* L50: */
    }

/* TERME DE PLUS HAUT DEGRE */

    crvnew[*ncoeff * 3 + 1] = -crvold[*ncoeff * 3 + 1] * m1jm1;
    crvnew[*ncoeff * 3 + 2] = -crvold[*ncoeff * 3 + 2] * m1jm1;
    crvnew[*ncoeff * 3 + 3] = -crvold[*ncoeff * 3 + 3] * m1jm1;

L9999:
    AdvApp2Var_SysBase::maermsg_("MVCVINV", iercod, 7L);
    return 0;
} /* mvcvinv_ */

//=======================================================================
//function : mvgaus0_
//purpose  : 
//=======================================================================
int mvgaus0_(integer *kindic, 
	     doublereal *urootl, 
	     doublereal *hiltab, 
	     integer *nbrval, 
	     integer *iercod)

{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static doublereal tamp[40];
    static integer ndegl, kg, ii;
   
/* ********************************************************************** 
*/

/*      FONCTION : */
/*      -------- */
/*  Chargement pour un degre donne des racines du polynome de LEGENDRE */
/*  DEFINI SUR [-1,1] et des poids des formules de quadrature de Gauss */
/*  (bases sur les interpolants de LAGRANGE correspondants). */
/*  La symetrie par rapport a 0 entre [-1,0] et [0,1] est utilisee. */

/*      MOTS CLES : */
/*      --------- */
/*         . VOLUMIQUE,LEGENDRE,LAGRANGE,GAUSS */

/*      ARGUMENTS D'ENTREE : */
/*      ------------------ */

/*  KINDIC : Prends les valeurs de 1 a 10 en fonction du degre du */
/*           polynome a utiliser. */
/*           Le degre du polynome est egal a 4 k, c'est a dire 4, 8, */
/*           12, 16, 20, 24, 28, 32, 36 et 40. */

/*      ARGUMENTS DE SORTIE : */
/*      ------------------- */

/*  UROOTL : Racines du polynome de LEGENDRE dans le domaine [1,0] */
/*           ordonnees en decroissant. Pour le domaine [-1,0], il faut */
/*           prendre les valeurs opposees. */
/*  HILTAB : Interpolant de LAGRANGE associes aux racines. Pour les */
/*           racines opposes, les interpolants sont egaux. */
/*  NBRVAL : Nombre de coefficients. C'est egal a la moitie du degre en */
/*           raison de la symetrie (i.e. 2*KINDIC). */

/*  IERCOD  :  Code d'erreur : */
/*          < 0 ==> Attention - Warning */
/*          =-1 ==> Valeur de KINDIC erronne. NBRVAL est force a 20 */
/*                  (ordre 40) */
/*          = 0 ==> Tout est OK */

/*      COMMON UTILISES : */
/*      ---------------- */

/*      REFERENCES APPELEES : */
/*      ------------------- */

/*      DESCRIPTION/REMARQUES/LIMITATIONS : */
/*      --------------------------------- */
/*      Si KINDIC n'est pas bon (i.e < 1 ou > 10), le degre est pris */
/*      a 40 directement (ATTENTION au debordement - pour l'eviter, */
/*      prevoir UROOTL et HILTAB dimensionne a 20 au moins). */

/*      La valeur des coefficients a ete calculee en quadruple precision 
*/
/*      par JJM avec l'aide de GD. */
/*      La verification des racines a ete faite par GD. */

/*      Voir les explications detaillees sur le listing */

/* $    HISTORIQUES DES MODIFICATIONS : */
/*     ----------------------------- */
/*        . 23-03-90 : RBD; Les valeurs sont extraites du commun MLGDRTL 
*/
/*                          via MMEXTHI et MMEXTRL. */
/*        . 28-06-88 : JP; DECLARATIONS REAL *8  MAL PLACEES */
/*        . 08-08-87 : GD; Version originale */
/* > */
/* ********************************************************************** 
*/


/* ------------------------------------ */
/* ****** Test de validite de KINDIC ** */
/* ------------------------------------ */

    /* Parameter adjustments */
    --hiltab;
    --urootl;

    /* Function Body */
    *iercod = 0;
    kg = *kindic;
    if (kg < 1 || kg > 10) {
	kg = 10;
	*iercod = -1;
    }
    *nbrval = kg << 1;
    ndegl = *nbrval << 1;

/* ---------------------------------------------------------------------- 
*/
/* ****** Chargement des NBRVAL racines positives en fonction du degre ** 
*/
/* ---------------------------------------------------------------------- 
*/
/* ATTENTION : Le signe moins (-) dans la boucle est intentionnel. */

    mmextrl_(&ndegl, tamp);
    i__1 = *nbrval;
    for (ii = 1; ii <= i__1; ++ii) {
	urootl[ii] = -tamp[ii - 1];
/* L100: */
    }

/* ------------------------------------------------------------------- */
/* ****** Chargement des NBRVAL poids de Gauss en fonction du degre ** */
/* ------------------------------------------------------------------- */

    mmexthi_(&ndegl, tamp);
    i__1 = *nbrval;
    for (ii = 1; ii <= i__1; ++ii) {
	hiltab[ii] = tamp[ii - 1];
/* L200: */
    }

/* ------------------------------- */
/* ****** Fin du sous-programme ** */
/* ------------------------------- */

    return 0;
} /* mvgaus0_ */

//=======================================================================
//function : mvpscr2_
//purpose  : 
//=======================================================================
int mvpscr2_(integer *ncoeff, 
	     doublereal *curve2, 
	     doublereal *tparam, 
	     doublereal *pntcrb)
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer ndeg, kk;
  static doublereal xxx, yyy;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  POSITIONNEMENT SUR UNE COURBE (NCF,2) DANS L'ESPACE DE DIMENSION 2. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS,MATH_ACCES:: COURBE&,POSITIONNEMENT,&POINT. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NCOEFF : NOMBRE DE COEFFICIENTS DE LA COURBE */
/*     CURVE2 : EQUATION DE LA COURBE 2D */
/*     TPARAM : VALEUR DU PARAMETRE AU POINT CONSIDERE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     PNTCRB : COORDONNEES DU POINT CORRESPONDANT AU PARAMETRE */
/*              TPARAM SUR LA COURBE 2D CURVE2. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     MSCHEMA DE HORNER. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     16-05-90 : RBD; Optimisation sur une idee de GD. */
/*     12-09-86 : NAK;ECRITURE VERSION ORIGINALE */
/* > */
/* ********************************************************************** 
*/


/* -------- INITIALISATIONS ET TRAITEMENT DES CAS PARTICULIERS ---------- 
*/

/* ---> Cas ou NCOEFF > 1 (cas STANDARD). */
    /* Parameter adjustments */
    --pntcrb;
    curve2 -= 3;

    /* Function Body */
    if (*ncoeff >= 2) {
	goto L1000;
    }
/* ---> Cas ou NCOEFF <= 1. */
    if (*ncoeff <= 0) {
	pntcrb[1] = 0.;
	pntcrb[2] = 0.;
	goto L9999;
    } else if (*ncoeff == 1) {
	pntcrb[1] = curve2[3];
	pntcrb[2] = curve2[4];
	goto L9999;
    }

/* -------------------- MSCHEMA DE HORNER (CAS PARTICULIER) --------------
 */

L1000:

    if (*tparam == 1.) {
	xxx = 0.;
	yyy = 0.;
	i__1 = *ncoeff;
	for (kk = 1; kk <= i__1; ++kk) {
	    xxx += curve2[(kk << 1) + 1];
	    yyy += curve2[(kk << 1) + 2];
/* L100: */
	}
	goto L5000;
    } else if (*tparam == 0.) {
	pntcrb[1] = curve2[3];
	pntcrb[2] = curve2[4];
	goto L9999;
    }

/* ---------------------------- MSCHEMA DE HORNER ------------------------
 */
/* ---> Ici TPARAM est different de 1.D0 et de 0.D0. */

    ndeg = *ncoeff - 1;
    xxx = curve2[(*ncoeff << 1) + 1];
    yyy = curve2[(*ncoeff << 1) + 2];
    for (kk = ndeg; kk >= 1; --kk) {
	xxx = xxx * *tparam + curve2[(kk << 1) + 1];
	yyy = yyy * *tparam + curve2[(kk << 1) + 2];
/* L200: */
    }
    goto L5000;

/* ------------------------ RECUPERATION DU POINT CALCULE --------------- 
*/

L5000:
    pntcrb[1] = xxx;
    pntcrb[2] = yyy;

/* ------------------------------ THE END ------------------------------- 
*/

L9999:
    return 0;
} /* mvpscr2_ */

//=======================================================================
//function : mvpscr3_
//purpose  : 
//=======================================================================
int mvpscr3_(integer *ncoeff, 
	     doublereal *curve3, 
	     doublereal *tparam, 
	     doublereal *pntcrb)

{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  static integer ndeg, kk;
  static doublereal xxx, yyy, zzz;



/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/* POSITIONNEMENT SUR UNE COURBE (3,NCF) DANS L'ESPACE DE DIMENSION 3. */

/*     MOTS CLES : */
/*     ----------- */
/*     TOUS, MATH_ACCES:: COURBE&,POSITIONNEMENT,&POINT. */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*     NCOEFF : NOMBRE DE COEFFICIENTS DE LA COURBE */
/*     CURVE3 : EQUATION DE LA COURBE 3D */
/*     TPARAM : VALEUR DU PARAMETRE AU POINT CONSIDERE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*     PNTCRB : COORDONNEES DU POINT CORRESPONDANT AU PARAMETRE */
/*              TPARAM SUR LA COURBE 3D CURVE3. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*            Neant */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     MSCHEMA DE HORNER. */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     16-05-90 : RBD; Optimisation sur une idee de GD (gain=10 pour */
/*                     cent pour des courbes de degre 10 a 20). */
/*     12-09-86 : NAK; ECRITURE VERSION ORIGINALE */
/* > */
/* ********************************************************************** 
*/
/*                           DECLARATIONS */
/* ********************************************************************** 
*/


/* -------- INITIALISATIONS ET TRAITEMENT DES CAS PARTICULIERS ---------- 
*/

/* ---> Cas ou NCOEFF > 1 (cas STANDARD). */
    /* Parameter adjustments */
    --pntcrb;
    curve3 -= 4;

    /* Function Body */
    if (*ncoeff >= 2) {
	goto L1000;
    }
/* ---> Cas ou NCOEFF <= 1. */
    if (*ncoeff <= 0) {
	pntcrb[1] = 0.;
	pntcrb[2] = 0.;
	pntcrb[3] = 0.;
	goto L9999;
    } else if (*ncoeff == 1) {
	pntcrb[1] = curve3[4];
	pntcrb[2] = curve3[5];
	pntcrb[3] = curve3[6];
	goto L9999;
    }

/* -------------------- MSCHEMA DE HORNER (CAS PARTICULIER) --------------
 */

L1000:

    if (*tparam == 1.) {
	xxx = 0.;
	yyy = 0.;
	zzz = 0.;
	i__1 = *ncoeff;
	for (kk = 1; kk <= i__1; ++kk) {
	    xxx += curve3[kk * 3 + 1];
	    yyy += curve3[kk * 3 + 2];
	    zzz += curve3[kk * 3 + 3];
/* L100: */
	}
	goto L5000;
    } else if (*tparam == 0.) {
	pntcrb[1] = curve3[4];
	pntcrb[2] = curve3[5];
	pntcrb[3] = curve3[6];
	goto L9999;
    }

/* ---------------------------- MSCHEMA DE HORNER ------------------------
 */
/* ---> Ici TPARAM est different de 1.D0 et de 0.D0. */

    ndeg = *ncoeff - 1;
    xxx = curve3[*ncoeff * 3 + 1];
    yyy = curve3[*ncoeff * 3 + 2];
    zzz = curve3[*ncoeff * 3 + 3];
    for (kk = ndeg; kk >= 1; --kk) {
	xxx = xxx * *tparam + curve3[kk * 3 + 1];
	yyy = yyy * *tparam + curve3[kk * 3 + 2];
	zzz = zzz * *tparam + curve3[kk * 3 + 3];
/* L200: */
    }
    goto L5000;

/* ------------------------ RECUPERATION DU POINT CALCULE --------------- 
*/

L5000:
    pntcrb[1] = xxx;
    pntcrb[2] = yyy;
    pntcrb[3] = zzz;

/* ------------------------------ THE END ------------------------------- 
*/

L9999:
    return 0;
} /* mvpscr3_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mvsheld_
//purpose  : 
//=======================================================================
 int AdvApp2Var_MathBase::mvsheld_(integer *n, 
			    integer *is, 
			    doublereal *dtab, 
			    integer *icle)

{
  /* System generated locals */
  integer dtab_dim1, dtab_offset, i__1, i__2;
  
  /* Local variables */
  static integer incr;
  static doublereal dsave;
  static integer i3, i4, i5, incrp1;


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*       TRI LES COLONNES D'UN TABLEAU DE REAL*8 SUIVANT LA METHODE DE SHE
LL*/
/*        (DANS L'ORDRE CROISSANT) */

/*     MOTS CLES : */
/*     ----------- */
/*        POINT-ENTREE, TRI, SHELL */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        N      : NOMBRE DE COLONNES DU TABLEAU */
/*        IS     : NOMBRE DE LIGNE DU TABLEAU */
/*        DTAB   : TABLEAU DE REAL*8 A TRIER */
/*        ICLE   : POSITION DE LA CLE SUR LA COLONNE */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        DTAB   : TABLEAU TRIE */

/*     COMMONS UTILISES   : */
/*     ---------------- */


/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*            Neant */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*     METHODE CLASSIQUE DE SHELL : TRI PAR SERIES */
/*     La declaration DTAB(IS, 1) correspond en fait a DTAB(IS, *) */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     24-09-93 : PMN; NETTOYAGE ET CORRECTION DE L'EN-TETE */
/*     13-07-84 : BF ; VERSION D'ORIGINE */

/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    dtab_dim1 = *is;
    dtab_offset = dtab_dim1 + 1;
    dtab -= dtab_offset;

    /* Function Body */
    if (*n <= 1) {
	goto L9900;
    }
/*     ------------------------ */

/*  INITIALISATION DE LA SUITE DES INCREMENTS */
/*  RECHERCHE DU PLUS GRAND INCREMENT TEL QUE INCR < N/9 */

    incr = 1;
L1001:
    if (incr >= *n / 9) {
	goto L1002;
    }
/*     ----------------------------- */
    incr = incr * 3 + 1;
    goto L1001;

/*  BOUCLE SUR LES INCREMENTS JUSQU'A INCR = 1 */
/*  TRI PAR SERIES DISTANTES DE INCR */

L1002:
    incrp1 = incr + 1;
/*     ----------------- */
    i__1 = *n;
    for (i3 = incrp1; i3 <= i__1; ++i3) {
/*        ---------------------- */

/*  METTRE L'ELEMENT I3 A SA PLACE DANS SA SERIE */

	i4 = i3 - incr;
L1004:
	if (i4 < 1) {
	    goto L1003;
	}
/*           ------------------------- */
	if (dtab[*icle + i4 * dtab_dim1] <= dtab[*icle + (i4 + incr) * 
		dtab_dim1]) {
	    goto L1003;
	}

	i__2 = *is;
	for (i5 = 1; i5 <= i__2; ++i5) {
/*              ------------------ */
	    dsave = dtab[i5 + i4 * dtab_dim1];
	    dtab[i5 + i4 * dtab_dim1] = dtab[i5 + (i4 + incr) * dtab_dim1];
	    dtab[i5 + (i4 + incr) * dtab_dim1] = dsave;
	}
/*              -------- */
	i4 -= incr;
	goto L1004;

L1003:
	;
    }
/*           -------- */

/*  PASSAGE A L'INCREMENT SUIVANT */

    incr /= 3;
    if (incr >= 1) {
	goto L1002;
    }

L9900:
 return 0   ;
} /* mvsheld_ */

//=======================================================================
//function : AdvApp2Var_MathBase::mzsnorm_
//purpose  : 
//=======================================================================
 doublereal AdvApp2Var_MathBase::mzsnorm_(integer *ndimen, 
				   doublereal *vecteu)
   
{
  /* System generated locals */
  integer i__1;
  doublereal ret_val, d__1, d__2;

  /* Local variables */
  static doublereal xsom;
  static integer i__, irmax;
  
  

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Sert a calculer la norme euclidienne d'un vecteur : */
/*                       ____________________________ */
/*                  Z = V  V(1)**2 + V(2)**2 + ... */

/*     MOTS CLES : */
/*     ----------- */
/*        SURMFACIQUE, */

/*     ARGUMENTS D'ENTREE : */
/*     ------------------ */
/*        NDIMEN : Dimension du vecteur */
/*        VECTEU : vecteur de dimension NDIMEN */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*        MZSNORM : Valeur de la norme euclidienne du vecteur VECTEU */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES APPELEES   : */
/*     ---------------------- */
/*     Type  Name */
/*      R*8  ABS            R*8  SQRT */

/*     DESCRIPTION/REMARQUES/LIMITATIONS : */
/*     ----------------------------------- */
/*        Pour limiter les risques d'overflow, on met en facteur */
/*     le terme de plus forte valeur absolue : */
/*                                _______________________ */
/*                  Z = !V(1)! * V  1 + (V(2)/V(1))**2 + ... */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     11-09-1995 : JMF ; implicit none */
/*     20-03-89 : DH ; Creation version originale */
/* > */
/* ***********************************************************************
 */
/*                      DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                     TRAITEMENT */
/* ***********************************************************************
 */

/* ___ Recherche du terme de plus forte valeur absolue */

    /* Parameter adjustments */
    --vecteu;

    /* Function Body */
    irmax = 1;
    i__1 = *ndimen;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if ((d__1 = vecteu[irmax], abs(d__1)) < (d__2 = vecteu[i__], abs(d__2)
		)) {
	    irmax = i__;
	}
/* L100: */
    }

/* ___ Calcul de la norme */

    if ((d__1 = vecteu[irmax], abs(d__1)) < 1.) {
	xsom = 0.;
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	    d__1 = vecteu[i__];
	    xsom += d__1 * d__1;
/* L200: */
	}
	ret_val = sqrt(xsom);
    } else {
	xsom = 0.;
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ == irmax) {
		xsom += 1.;
	    } else {
/* Computing 2nd power */
		d__1 = vecteu[i__] / vecteu[irmax];
		xsom += d__1 * d__1;
	    }
/* L300: */
	}
	ret_val = (d__1 = vecteu[irmax], abs(d__1)) * sqrt(xsom);
    }

/* ***********************************************************************
 */
/*                   RETOUR PROGRAMME APPELANT */
/* ***********************************************************************
 */

    return ret_val;
} /* mzsnorm_ */

