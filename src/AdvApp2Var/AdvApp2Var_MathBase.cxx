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

// AdvApp2Var_MathBase.cxx
#include <math.h>
#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_Data.hxx>
#include <NCollection_Array1.hxx>

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
	     NCollection_Array1<doublereal>& hwgaus);

static
int mmextrl_(integer *ndegre,
	     NCollection_Array1<doublereal>& rootlg);



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
       NCollection_Array1<doublereal>& valjac, 
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
  integer c__8 = 8;
  /* System generated locals */
  integer i__1;
  doublereal d__1;

  /* Local variables */
  integer i__;
  doublereal* differ = 0;
  integer  ier;
  intptr_t iofset, j;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        CALCULATE DISTANCE BETWEEN TWO POINTS */

/*     KEYWORDS : */
/*     ----------- */
/*        DISTANCE,POINT. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMEN: Space Dimension. */
/*        POINT1: Table of coordinates of the 1st point. */
/*        POINT2: Table of coordinates of the 2nd point. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        DISTAN: Distance between 2 points. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ********************************************************************** 
*/


/* ***********************************************************************
 */
/*                      INITIALISATION */
/* ***********************************************************************
 */

    /* Parameter adjustment */
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

    AdvApp2Var_SysBase anAdvApp2Var_SysBase;
    if (*ndimen > 100) {
	anAdvApp2Var_SysBase.mcrrqst_(&c__8, ndimen, differ, &iofset, &ier);
    }

/* --- If allocation is refused, the trivial method is applied. */

    if (ier > 0) {

	*distan = 0.;
	i__1 = *ndimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	    d__1 = point1[i__] - point2[i__];
	    *distan += d__1 * d__1;
	}
	*distan = sqrt(*distan);

/* --- Otherwise MZSNORM is used to minimize the risks of overflow 
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
/*                   RETURN CALLING PROGRAM */
/* ***********************************************************************
 */

/* --- Dynamic Desallocation */

    if (iofset != 0) {
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, ndimen, differ, &iofset, &ier);
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
    integer i__;

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
  integer ipair, nd, ndegre, impair, ibb, idg;
  //extern  int  mgsomsg_();//mgenmsg_(),

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Compression of curve CRVOLD in a table of  */
/*        coeff. of even : CRVNEW(*,0,*) */
/*        and uneven range : CRVNEW(*,1,*). */

/*     KEYWORDS : */
/*     ----------- */
/*        COMPRESSION,CURVE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIM   : Space Dimension. */
/*     NCOFMX : Max nb of coeff. of the curve to compress. */
/*     NCOEFF : Max nb of coeff. of the compressed curve. */
/*     CRVOLD : The curve (0:NCOFMX-1,NDIM) to compress. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     CRVNEW : Curve compacted in (0:(NCOEFF-1)/2,0,NDIM) (containing 
*/
/*              even terms) and in (0:(NCOEFF-1)/2,1,NDIM) */
/*              (containing uneven terms). */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     This routine is useful to prepare coefficients of a */
/*     curve in an orthogonal base (Legendre or Jacobi) before */
/*     calculating the coefficients in the canonical; base [-1,1] by */
/*     MMJACAN. */
/* ***********************************************************************
 */

/*   Name of the routine */

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
  integer ncut;
  doublereal bidon;
  integer ii, nd;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Calculate the max error of approximation done when */
/*        only the first NCFNEW coefficients of a curve are preserved.  
*/
/*        Degree NCOEFF-1 written in the base of Legendre (Jacobi */
/*        of  order 0). */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,POLYGON,APPROXIMATION,ERROR. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max. degree of the curve. */
/*        NDIMEN : Space dimension. */
/*        NCOEFF : Degree +1 of the curve. */
/*        CRVLGD : Curve the degree which of should be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary Table (max error on each dimension). 
*/
/*        ERRMAX : Precision of the approximation. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* ***********************************************************************
 */


/* ------------------- Init to calculate an error ----------------------- 
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

/* ------ Minimum that can be reached : Stop at 1 or NCFNEW ------ 
*/

    ncut = 1;
    if (*ncfnew + 1 > ncut) {
	ncut = *ncfnew + 1;
    }

/* -------------- Elimination of high degree coefficients----------- 
*/
/* ----------- Loop on the series of Legendre: NCUT --> NCOEFF -------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Factor of renormalization (Maximum of Li(t)). */
	bidon = ((ii - 1) * 2. + 1.) / 2.;
	bidon = sqrt(bidon);

	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[ii + nd * crvlgd_dim1], advapp_abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- The error is the norm of the vector error --------------- 
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
    integer idec, ncut;
    doublereal bidon;
    integer ii, nd;



/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Calculate max approximation error i faite lorsque l' on */
/*        ne conserve que les premiers NCFNEW coefficients d' une courbe 
*/
/*        de degre NCOEFF-1 ecrite dans la base de Jacobi d' ordre 2. */

/*     KEYWORDS : */
/*     ----------- */
/*        JACOBI, POLYGON, APPROXIMATION, ERROR. */
/**/
/*  INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max. degree of the curve. */
/*        NDIMEN : Space dimension. */
/*        NCOEFF : Degree +1 of the curve. */
/*        CRVLGD : Curve the degree which of should be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary Table (max error on each dimension). 
*/
/*        ERRMAX : Precision of the approximation. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */



/* ------------------ Table of maximums of (1-t2)*Ji(t) ---------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init for error  calculation ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Min. Degree that can be attained : Stop at 3 or NCFNEW ------ 
*/

    idec = 3;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = advapp_max(i__1,i__2);

/* -------------- Removal of coefficients of high degree ----------- 
*/
/* ----------- Loop on the series of Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Factor of renormalization. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], advapp_abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- The error is the norm of the vector error --------------- 
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
    integer idec, ncut;
    doublereal bidon;
    integer ii, nd;



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Calculate the max. error of approximation made when  */
/*        only first NCFNEW coefficients of a curve are preserved 
*/
/*        degree NCOEFF-1 is written in the base of Jacobi of order 4. */
/*        KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,POLYGON,APPROXIMATION,ERROR. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max. degree of the curve. */
/*        NDIMEN : Space dimension. */
/*        NCOEFF : Degree +1 of the curve. */
/*        CRVJAC : Curve the degree which of should be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary Table (max error on each dimension). 
*/
/*        ERRMAX : Precision of the approximation. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */


/* ***********************************************************************
 */


/* ---------------- Table of maximums of ((1-t2)2)*Ji(t) --------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init for error calculation ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Min. Degree that can be attained : Stop at 5 or NCFNEW ------ 
*/

    idec = 5;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = advapp_max(i__1,i__2);

/* -------------- Removal of high degree coefficients ----------- 
*/
/* ----------- Loop on the series of Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Factor of renormalisation. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], advapp_abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- The error is the norm of the error vector --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- End -------------------------------- 
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
    integer idec, ncut;
    doublereal bidon;
    integer ii, nd;



/* ***********************************************************************
 */
/*     FUNCTION : */
/*     ---------- */
/*        Calculate the max. error of approximation made when  */
/*        only first NCFNEW coefficients of a curve are preserved 
*/
/*        degree NCOEFF-1 is written in the base of Jacobi of order 6. */
/*        KEYWORDS : */
/*     ----------- */
/*        JACOBI,POLYGON,APPROXIMATION,ERROR. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max. degree of the curve. */
/*        NDIMEN : Space dimension. */
/*        NCOEFF : Degree +1 of the curve. */
/*        CRVJAC : Curve the degree which of should be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary Table (max error on each dimension). 
*/
/*        ERRMAX : Precision of the approximation. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/* > */
/* ***********************************************************************
 */


/* ---------------- Table of maximums of ((1-t2)3)*Ji(t) --------------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */



/* ------------------- Init for error calculation ----------------------- 
*/

    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	ycvmax[ii] = 0.;
/* L100: */
    }

/* ------ Min Degree that can be attained : Stop at 3 or NCFNEW ------ 
*/

    idec = 7;
/* Computing MAX */
    i__1 = idec, i__2 = *ncfnew + 1;
    ncut = advapp_max(i__1,i__2);

/* -------------- Removal of high degree coefficients ----------- 
*/
/* ----------- Loop on the series of Jacobi :NCUT --> NCOEFF ---------- 
*/

    i__1 = *ncoeff;
    for (ii = ncut; ii <= i__1; ++ii) {
/*   Factor of renormalization. */
	bidon = xmaxj[ii - idec];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvjac[ii + nd * crvjac_dim1], advapp_abs(d__1)) * 
		    bidon;
/* L310: */
	}
/* L300: */
    }

/* -------------- The error is the norm of the vector error --------------- 
*/

    *errmax = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);

/* --------------------------------- END -------------------------------- 
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
  integer jord;

/* ********************************************************************** 
*/
/*     FUNCTION : */
/*     ---------- */
/*        Calculate the max. error of approximation made when  */
/*        only first NCFNEW coefficients of a curve are preserved 
*/
/*        degree NCOEFF-1 is written in the base of Jacobi of order IORDRE. */
/*        KEYWORDS : */
/*     ----------- */
/*        JACOBI,LEGENDRE,POLYGON,APPROXIMATION,ERROR. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max. degree of the curve. */
/*        NDIMEN : Space dimension. */
/*        NCOEFF : Degree +1 of the curve. */ 
/*        IORDRE : Order of continuity at the extremities. */
/*        CRVJAC : Curve the degree which of should be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary Table (max error on each dimension). 
*/
/*        ERRMAX : Precision of the approximation. */
/*        IERCOD = 0, OK */
/*               = 1, order of constraints (IORDRE) is not within the */
/*                    autorized values. */
/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Canceled and replaced MMAPERR. */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */
    *iercod = 0;
/* --> Order of Jacobi polynoms */
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
    integer nboct;
    doublereal tbaux[61];
    integer nd;
    doublereal bid;
    integer ncf, ncj;


/*      IMPLICIT DOUBLE PRECISION(A-H,O-Z) */
/*      IMPLICIT INTEGER (I-N) */

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*     Creation of curve C2(v) defined on (0,1) identic to */
/*     curve C1(u) defined on (U0,U1) (change of parameter */
/*     of a curve). */

/*     KEYWORDS : */
/*     ----------- */
/*        LIMITATION, RESTRICTION, CURVE */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMAX : Space Dimensioning. */
/*   NDIMEN : Curve Dimension. */
/*   NCOEFF : Nb of coefficients of the curve. */
/*   CRVOLD : Curve to be limited. */
/*   UPARA0     : Min limit of the interval limiting the curve. 
*/
/*   UPARA1     : Max limit of the interval limiting the curve. 
*/

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   CRVNEW : Relimited curve, defined on (0,1) and equal to */
/*            CRVOLD defined on (U0,U1). */
/*   IERCOD : = 0, OK */
/*            =10, Nb of coeff. <1 or > 61. */

/*     COMMONS USED   : */
/*     ---------------- */
/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG              MCRFILL              MVCVIN2 */
/*           MVCVINV */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* ---> Algorithm used in this general case is based on the */
/*     following principle  : */
/*        Let S(t) = a0 + a1*t + a2*t**2 + ... of degree NCOEFF-1, and */
/*               U(t) = b0 + b1*t, then the coeff. of */
/*        S(U(t)) are calculated step by step with help of table TBAUX. */
/*        At each step number N (N=2 to NCOEFF), TBAUX(n) contains */
/*        the n-th coefficient of U(t)**N for n=1 to N. (RBD) */
/* ---> Reference : KNUTH, 'The Art of Computer Programming', */
/*                        Vol. 2/'Seminumerical Algorithms', */
/*                        Ex. 11 p:451 et solution p:562. (RBD) */

/* ---> Removal of the input argument CRVOLD by CRVNEW is */
/*     possible, which means that the call : */
/*       CALL MMARC41(NDIMAX,NDIMEN,NCOEFF,CURVE,UPARA0,UPARA1 */
/*                  ,CURVE,IERCOD) */
/*     is absolutely LEGAL. (RBD) */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

/*   Auxiliary table of coefficients of (UPARA1-UPARA0)T+UPARA0  */
/*   with power N=1 to NCOEFF-1. */


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
/*                CASE WHEN PROCESSING CAN'T BE DONE */
/* ********************************************************************** 
*/
    if (*ncoeff > 61 || *ncoeff < 1) {
	*iercod = 10;
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                         IF NO CHANGES */
/* ********************************************************************** 
*/
    if (*ndimen == *ndimax && *upara0 == 0. && *upara1 == 1.) {
	nboct = (*ndimax << 3) * *ncoeff;
	AdvApp2Var_SysBase::mcrfill_(&nboct,
		 &crvold[crvold_offset], 
		 &crvnew[crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                    INVERSION 3D : FAST PROCESSING */
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
/*                    INVERSION 2D : FAST PROCESSING */
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
/*                          GENERAL PROCESSING */
/* ********************************************************************** 
*/
/* -------------------------- Initializations --------------------------- 
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

/* ----------------------- Calculation of coeff. of CRVNEW ------------------ 
*/

    i__1 = *ncoeff - 1;
    for (ncf = 2; ncf <= i__1; ++ncf) {

/* ------------ Take into account NCF-th coeff. of CRVOLD --------
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

/* --------- Calculate (NCF+1) coeff. of ((U1-U0)*t + U0)**(NCF) ---
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

/* -------------- Take into account the last coeff. of CRVOLD ----------- 
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
  doublereal x0, x1;
  integer nd;
  doublereal tabaux[61];
  integer ibb;
  doublereal bid;
  integer ncf;
  integer ncj;
  doublereal eps3;
  


/* ********************************************************************** 
*//*     FUNCTION : */
/*     ---------- */
/*     Creation of curve C2(v) defined on [U0,U1] identic to */
/*     curve C1(u) defined on [-1,1] (change of parameter */
/*     of a curve) with INVERSION of indices of the resulting table. */

/*     KEYWORDS : */
/*     ----------- */
/*        GENERALIZED LIMITATION, RESTRICTION, INVERSION, CURVE */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMAX : Maximum Space Dimensioning. */
/*   NDIMEN : Curve Dimension. */
/*   NCOEFF : Nb of coefficients of the curve. */
/*   CRVOLD : Curve to be limited. */
/*   U0     : Min limit of the interval limiting the curve. 
*/
/*   U1     : Max limit of the interval limiting the curve. 
*/

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   CRVNEW : Relimited curve, defined on  [U0,U1] and equal to */
/*            CRVOLD defined on [-1,1]. */
/*   IERCOD : = 0, OK */
/*            =10, Nb of coeff. <1 or > 61. */
/*            =13, the requested interval of variation is null. */
/*     COMMONS USED   : */
/*     ---------------- */
/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

/*   Auxiliary table of coefficients of X1*T+X0 */
/*   with power N=1 to NCOEFF-1. */


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

/* At zero machine it is tested if the output interval is not null */

    AdvApp2Var_MathBase::mmveps3_(&eps3);
    if ((d__1 = *u1 - *u0, advapp_abs(d__1)) < eps3) {
	*iercod = 13;
	goto L9999;
    }
    *iercod = 0;

/* ********************************************************************** 
*/
/*                CASE WHEN THE PROCESSING IS IMPOSSIBLE */
/* ********************************************************************** 
*/
    if (*ncoeff > 61 || *ncoeff < 1) {
	*iercod = 10;
	goto L9999;
    }
/* ********************************************************************** 
*/
/*          IF NO CHANGE OF THE INTERVAL OF DEFINITION */
/*          (ONLY INVERSION OF INDICES OF TABLE CRVOLD) */
/* ********************************************************************** 
*/
    if (*ndim == *ndimax && *u0 == -1. && *u1 == 1.) {
	AdvApp2Var_MathBase::mmcvinv_(ndim, ncoeff, ndim, &crvold[crvold_offset], &crvnew[
		crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*          CASE WHEN THE NEW INTERVAL OF DEFINITION IS [0,1] */
/* ********************************************************************** 
*/
    if (*u0 == 0. && *u1 == 1.) {
	mmcvstd_(ncoeff, ndimax, ncoeff, ndim, &crvold[crvold_offset], &
		crvnew[crvnew_offset]);
	goto L9999;
    }
/* ********************************************************************** 
*/
/*                          GENERAL PROCESSING */
/* ********************************************************************** 
*/
/* -------------------------- Initialization --------------------------- 
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

/* ----------------------- Calculation of coeff. of CRVNEW ------------------ 
*/

    i__1 = *ncoeff - 1;
    for (ncf = 2; ncf <= i__1; ++ncf) {

/* ------------ Take into account the NCF-th coeff. of CRVOLD --------
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

/* --------- Calculation of (NCF+1) coeff. of [X1*t + X0]**(NCF) --------
---- */

	tabaux[ncf] = tabaux[ncf - 1] * x1;
	for (ncj = ncf; ncj >= 2; --ncj) {
	    tabaux[ncj - 1] = tabaux[ncj - 1] * x0 + tabaux[ncj - 2] * x1;
/* L600: */
	}
	tabaux[0] *= x0;

/* L200: */
    }

/* -------------- Take into account the last coeff. of CRVOLD ----------- 
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
    logical ldbg;
  integer jmin, jmax, i__, j, k;
  doublereal somme;
  integer aux;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*      Produce vector matrix in form of profile */


/*     MOTS CLES : */
/*     ----------- */
/*      RESERVE, MATRIX, PRODUCT, VECTOR, PROFILE */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       NLIGNE : Line number of the matrix of constraints */
/*       NCOLON : Number of column of the matrix of constraints */
/*       GNSTOC: Number of coefficients in the profile of matrix GMATRI */

/*       GPOSIT: Table of positioning of terms of storage */
/*               GPOSIT(1,I) contains the number of terms-1 on the line I */
/*               in the profile of the matrix. */
/*              GPOSIT(2,I) contains the index of storage of diagonal term*/
/*               of line I */
/*               GPOSIT(3,I) contains the index of column of the first term of */
/*                           profile of line I */
/*       GNSTOC: Number of coefficients in the profile of matrix */
/*               GMATRI */
/*       GMATRI : Matrix of constraints in form of profile */
/*       VECIN  : Input vector */
/*       DEBLIG : Line indexusing which the vector matrix is calculated */
/**/               
/*     OUTPUT ARGUMENTS */
/*     --------------------- */
/*       VECOUT : VECTOR PRODUCT */

/*       IERCOD : ERROR CODE */


/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
/*                    Processing */
/* ***********************************************************************
 */
    AdvApp2Var_SysBase::mvriraz_(nligne, 
	     &vecout[1]);
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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  logical ldbg;
  doublereal daux;
  integer nite1, nite2, nchan, i1, i2;
  
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Parsing of columns of a table of integers in increasing order */
/*     KEYWORDS : */
/*     ----------- */
/*     POINT-ENTRY, PARSING */
/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       - NBCOLN : Number of columns in the table */
/*       - NBLIGN : Number of lines in the table */
/*       - DTABTR : Table of integers to be parsed */
/*       - NUMCLE : Position of the key on the column */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       - DTABTR : Parsed table */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Particularly performant if the table is almost parsed */
/*     In the opposite case it is better to use MVSHELD */
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
/*                     PROCESSING */
/* ***********************************************************************
 */

/* ---->ALGORITHM in N^2 / 2 additional iteration */

    while(nchan != 0) {

/* ----> Parsing from left to the right */

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

/* ----> Parsing from right to the left */

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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */

/* ----> No errors at calling functions, only tests and loops. */

/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  integer i__, j, k;
  doublereal mfactk, bid;
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*     Calculate matrix of a derivate curve of order IDERIV. */
/*     with input parameters other than output parameters. */


/*     KEYWORDS : */
/*     ----------- */
/*     COEFFICIENTS,CURVE,DERIVATE I-EME. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMEN  : Space dimension (2 or 3 in general) */
/*   NCOEFF  : Degree +1 of the curve. */
/*   COURBE  : Table of coefficients of the curve. */
/*   IDERIV  : Required order of derivation : 1=1st derivate, etc... */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   NCOFDV  : Degree +1 of the derivative of order IDERIV of the curve. */
/*   CRVDRV  : Table of coefficients of the derivative of order IDERIV */
/*            of the curve. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* ---> It is possible to take as output argument the curve */
/*     and the number of coeff passed at input by making : */
/*        CALL MMCDRIV(NDIMEN,NCOEFF,COURBE,IDERIV,NCOEFF,COURBE). */
/*     After this call, NCOEFF does the number of coeff of the derived */
/*     curve the coefficients which of are stored in CURVE. */
/*     Attention to the coefficients of CURVE of rank superior to */
/*     NCOEFF : they are not set to zero. */

/* ---> Algorithm : */
/*     The code below was written basing on the following algorithm: 
*/

/*     Let P(t) = a1 + a2*t + ... an*t**n. Derivate of order k of P */
/*     (containing n-k coefficients) is calculated as follows : */

/*       Pk(t) = a(k+1)*CNP(k,k)*k! */
/*             + a(k+2)*CNP(k+1,k)*k! * t */
/*             . */
/*             . */
/*             . */
/*             + a(n)*CNP(n-1,k)*k! * t**(n-k-1). */
/* ***********************************************************************
 */


/* -------------- Case when the order of derivative is  ------------------- 
*/
/* ---------------- greater than the degree of the curve --------------------- 
*/

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Serves to provide the coefficients of binome (Pascal's triangle). */

/*     KEYWORDS : */
/*     ----------- */
/*      Binomial coeff from 0 to 60. read only . init par block data */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Binomial coefficients form a triangular matrix. */
/*     This matrix is completed in table CNP by its transposition. */
/*     So: CNP(I,J) = CNP(J,I) for I and J = 0, ..., 60. */

/*     Initialization is done by block-data MMLLL09.RES, */
/*     created by program MQINICNP.FOR). */
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
/*                        General processing */
/* ********************************************************************** 
*/
/* --------------------- Calculation of Factorial(IDERIV) ------------------ 
*/

    k = *ideriv;
    mfactk = 1.;
    i__1 = k;
    for (i__ = 2; i__ <= i__1; ++i__) {
	mfactk *= i__;
/* L50: */
    }

/* ------------ Calculation of coeff of the derived of order IDERIV ---------- 
*/
/* ---> Attention :  coefficient binomial C(n,m) is represented in */
/*                 MCCNP by CNP(N+1,M+1). */

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
  integer ndec;
  doublereal tdeb, tfin;
  integer iter;
  doublereal oldso = 0.;
  integer itmax;
  doublereal sottc;
  integer kk, ibb;
  doublereal dif, pas;
  doublereal som;
 

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*      Allows calculating the length of an arc of curve POLYNOMIAL */
/*      on an interval [A,B]. */

/*     KEYWORDS : */
/*     ----------- */
/*        LENGTH,CURVE,GAUSS,PRIVATE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*      NDIMAX : Max. number of lines of tables */
/*               (i.e. max. nb of polynoms). */
/*      NDIMEN : Dimension of the space (nb of polynoms). */
/*      NCOEFF : Nb of coefficients of the polynom. This is degree + 1. 
*/
/*      COURBE(NDIMAX,NCOEFF) : Coefficients of the curve. */
/*      TDEBUT : Lower limit of the interval of integration for  */
/*               length calculation. */
/*      TFINAL : Upper limit of the interval of integration for */
/*               length calculation. */
/*      EPSILN : REQIRED precision for length calculation. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*      XLONGC : Length of the arc of curve */
/*      ERREUR : Precision OBTAINED for the length calculation. */
/*      IERCOD : Error code, 0 OK, >0 Serious error. */
/*               = 1 Too much iterations, the best calculated resultat */
/*                   (is almost ERROR) */
/*               = 2 Pb MMLONCV (no result) */
/*               = 3 NDIM or NCOEFF invalid (no result) */

/*     COMMONS USED : */
/*     ---------------- */

/*     REFERENCES CALLED : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*      The polynom is actually a set of polynoms with */
/*      coefficients arranged in a table of 2 indices, */
/*      each line relative to the polynom. */
/*      The polynom is defined by these coefficients ordered */
/*      by increasing power of the variable. */
/*      All polynoms have the same number of coefficients (the */
/*      same degree). */

/*      This program cancels and replaces LENGCV, MLONGC and MLENCV. */

/*      ATTENTION : if TDEBUT > TFINAL, the length is NEGATIVE. */

/* > */
/* ***********************************************************************
 */

/*   Name of the routine */


/* ------------------------ General Initialization --------------------- 
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

/* ------ Test of equity of limits */

    if (*tdebut == *tfinal) {
	*iercod = 0;
	goto L9999;
    }

/* ------ Test of the dimension and the number of coefficients */

    if (*ndimen <= 0 || *ncoeff <= 0) {
	goto L9003;
    }

/* ----- Nb of current cutting, nb of iteration, */
/*       max nb of iterations */

    ndec = 1;
    iter = 1;

    itmax = 13;

/* ------ Variation of the nb of intervals */
/*       Multiplied by 2 at each iteration */

L5000:
    pas = (*tfinal - *tdebut) / ndec;
    sottc = 0.;

/* ------ Loop on all current NDEC intervals */

    i__1 = ndec;
    for (kk = 1; kk <= i__1; ++kk) {

/* ------ Limits of the current integration interval */

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


/* ----------------- Test of the maximum number of iterations ------------ 
*/

/*  Test if passes at least once ** */

    if (iter == 1) {
	oldso = sottc;
	ndec <<= 1;
	++iter;
	goto L5000;
    } else {

/* ------ Take into account DIF - Test of convergence */

	++iter;
	dif = (d__1 = sottc - oldso, advapp_abs(d__1));

/* ------ If DIF is OK, leave..., otherwise: */

	if (dif > *epsiln) {

/* ------ If nb iteration exceeded, leave */

	    if (iter > itmax) {
		*iercod = 1;
		goto L9000;
	    } else {

/* ------ Otherwise continue by cutting the initial interval.
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

/* ---> PB in MMLONCV */

L9002:
    *iercod = 2;
    goto L9999;

/* ---> NCOEFF or NDIM invalid. */

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
  logical ldbg;
  integer kmin, i__, j, k;
  doublereal somme;
  integer ptini, ptcou;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ----------                                                  T */
/*     Produce decomposition of choleski of matrix A in S.S */
/*     Calculate inferior triangular matrix S. */

/*     KEYWORDS : */
/*     ----------- */
/*     RESOLUTION, MFACTORISATION, MATRIX_PROFILE, CHOLESKI */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*     MXCOEF : Max number of terms in the hessian profile */
/*     DIMENS : Dimension of the problem */
/*     AMATRI(MXCOEF) : Coefficients of the matrix profile */
/*        APOSIT(1,*) : Distance diagonal-left extremity of the line 
*/
/*        APOSIT(2,*) : Position of diagonal terms in HESSIE */
/*     POSUIV(MXCOEF) :  first line inferior not out of profile */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*      CHOMAT(MXCOEF) : Inferior triangular matrix preserving the */
/*                       profile of AMATRI. */
/*      IERCOD : error code */
/*               = 0 : ok */
/*               = 1 : non-defined positive matrix */

/*     COMMONS USED : */
/*     ------------------ */

/*      .Neant. */

/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     DEBUG LEVEL = 4 */
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
/*                    PROCESSING */
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

/*           Calculate the sum of S  .S   for k =1 a j-1 */
/*                               ik  jk */
	    somme = 0.;
/* Computing MAX */
	    i__2 = i__ - aposit[(i__ << 1) + 1], i__3 = j - aposit[(j << 1) + 
		    1];
	    kmin = advapp_max(i__2,i__3);
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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

/* ***********************************************************************
 */
/*                  RETURN CALLING PROGRAM */
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
  integer moup1, nordr;
  integer nd;
  integer ibb, ncf, ndv;
  doublereal eps1;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Calculate a polynomial curve checking the  */
/*        passage constraints (interpolation) */
/*        from first derivatives, etc... to extremities. */
/*        Parameters at the extremities are supposed to be -1 and 1. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::CONSTRAINTS&,INTERPOLATION,&CURVE */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN : Space Dimension. */
/*     NCOFMX : Nb of coeff. of curve CRVRES on each */
/*              dimension. */
/*     NDERIV : Order of constraint with derivatives : */
/*              0 --> interpolation simple. */
/*              1 --> interpolation+constraints with 1st. */
/*              2 --> cas (0)+ (1) +   "         "   2nd derivatives. */
/*                 etc... */
/*     CTRTES : Table of constraints. */
/*              CTRTES(*,1,*) = contraints at -1. */
/*              CTRTES(*,2,*) = contraints at  1. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     CRVRES : Resulting curve defined on (-1,1). */
/*     TABAUX : Auxilliary matrix. */
/*     XMATRI : Auxilliary matrix. */

/*     COMMONS UTILISES   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG         R*8  DFLOAT              MGENMSG */
/*           MGSOMSG              MMEPS1               MMRSLW */
/*      I*4  MNFNDEB */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*        The polynom (or the curve) is calculated by solving a */
/*        system of linear equations. If the imposed degree is great */
/*        it is preferable to call a routine based on */
/*        Lagrange or Hermite interpolation depending on the case. */
/*        (for a high degree the matrix of the system can be badly */
/*        conditionned). */
/*        This routine returns a curve defined in (-1,1). */
/*        In general case, it is necessary to use MCVCTG. */
/* > */
/* ***********************************************************************
 */

/*   Name of the routine */


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
/*   Precision. */
    AdvApp2Var_MathBase::mmeps1_(&eps1);

/* ****************** CALCULATION OF EVEN COEFFICIENTS ********************* 
*/
/* ------------------------- Initialization ----------------------------- 
*/

    nordr = *nderiv + 1;
    i__1 = nordr;
    for (ncf = 1; ncf <= i__1; ++ncf) {
	tabaux[ncf + tabaux_dim1] = 1.;
/* L100: */
    }

/* ---------------- Calculation of terms corresponding to derivatives ------- 
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

/* ------------------ Writing the second member ----------------------- 
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

/* -------------------- Resolution of the system --------------------------- 
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

/* ***************** CALCULATION OF UNEVEN COEFFICIENTS ******************** 
*/
/* ------------------------- Initialization ----------------------------- 
*/


    i__1 = nordr;
    for (ncf = 1; ncf <= i__1; ++ncf) {
	tabaux[ncf + tabaux_dim1] = 1.;
/* L1100: */
    }

/* ---------------- Calculation of terms corresponding to derivatives ------- 
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

/* ------------------ Writing of the second member ----------------------- 
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

/* -------------------- Solution of the system --------------------------- 
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
  integer i__, nd, ibb;
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Inversion of arguments of the final curve. */

/*     KEYWORDS : */
/*     ----------- */
/*        SMOOTHING,CURVE */


/*     INPUT ARGUMENTS : */
/*     ------------------ */

/*        NDIM: Space Dimension. */
/*        NCOEF: Degree of the polynom. */
/*        CURVEO: The curve before inversion. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        CURVE: The curve after inversion. */

/*     COMMONS USED : */
/*     ---------------- */
/*     REFERENCES APPELEES   : */
/*     ----------------------- */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* ***********************************************************************
 */

/*   The name of the routine */
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
  integer ndeg, i__, j, j1, nd, ibb;
  doublereal bid;
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Transform curve defined between [-1,1] into [0,1]. */

/*     KEYWORDS : */
/*     ----------- */
/*        LIMITATION,RESTRICTION,CURVE */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMAX : Dimension of the space. */
/*        NDIMEN : Dimension of the curve. */
/*        NCOEFF : Degree of the curve. */
/*        CRVCAN(NCOFMX,NDIMEN): The curve is defined at the interval [-1,1]. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        CURVE(NDIMAX,NCOEFF): Curve defined at the interval [0,1]. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */

/*   Name of the program. */


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Provides binomial coefficients (Pascal triangle). */

/*     KEYWORDS : */
/*     ----------- */
/*      Binomial coefficient from 0 to 60. read only . init by block data */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Binomial coefficients form a triangular matrix. */
/*     This matrix is completed in table CNP by its transposition. */
/*     So: CNP(I,J) = CNP(J,I) for I and J = 0, ..., 60. */

/*     Initialization is done with block-data MMLLL09.RES, */
/*     created by the program MQINICNP.FOR. */
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

/* ------------------ Construction of the resulting curve ---------------- 
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

/* ------------------- Renormalization of the CURVE -------------------------
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
  
  integer ndeg, i__, j, ndgcb, nd, ibb;
  

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Calculation of successive derivatives of equation CURVE with */
/*        parameters -1, 1 from order 0 to order IORDRE */
/*        included. The calculation is produced without knowing the coefficients of */
/*        derivatives of the curve. */

/*     KEYWORDS : */
/*     ----------- */
/*        POSITIONING,EXTREMITIES,CURVE,DERIVATIVE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        IORDRE  : Maximum order of calculation of derivatives. */
/*        NDIMEN  : Dimension of the space. */
/*        NCOEFF  : Number of coefficients of the curve (degree+1). */
/*        COURBE  : Table of coefficients of the curve. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        POINTS    : Table of values of consecutive derivatives */
/*                 of parameters -1.D0 and 1.D0. */
/*        MFACTAB : Auxiliary table for calculation of factorial(I). 
*/

/*     COMMONS USED   : */
/*     ---------------- */
/*        None. */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* ---> ATTENTION, the coefficients of the curve are  */
/*     in a reverse order. */

/* ---> The algorithm of calculation of derivatives is based on */
/*     generalization of Horner scheme : */
/*                          k             2 */
/*          Let C(t) = uk.t  + ... + u2.t  + u1.t + u0 . */


/*      a0 = uk, b0 = 0, c0 = 0 and for 1<=j<=k, it is calculated : */

/*          aj = a(j-1).x + u(k-j) */
/*          bj = b(j-1).x + a(j-1) */
/*          cj = c(j-1).x + b(j-1) */

/*     So : C(x) = ak, C'(x) = bk, C"(x) = 2.ck  . */

/*     The algorithm is generalized easily for calculation of */

/*               (n) */
/*              C  (x)   . */
/*             --------- */
/*                n! */

/*      Reference : D. KNUTH, "The Art of Computer Programming" */
/*      ---------              Vol. 2/Seminumerical Algorithms */
/*                             Addison-Wesley Pub. Co. (1969) */
/*                             pages 423-425. */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

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

/* ------------------- Initialization of table POINTS ----------------- 
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

/*    Calculation with parameter -1 and 1 */

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

/* --------------------- Multiplication by factorial(I) -------------- 
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

/* ---------------------------- End ------------------------------------- 
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
  integer ndeg, i__, j, nd, ndgcrb, iptpnt, ibb;
  

/* *********************************************************************** */
/*     FUNCTION : */
/*     ---------- */

/*        Calculation of successive derivatives of equation CURVE with */
/*        parameter TPARAM from order 0 to order IDERIV included. */
/*        The calculation is produced without knowing the coefficients of */
/*        derivatives of the CURVE. */

/*     KEYWORDS : */
/*     ----------- */
/*        POSITIONING,PARAMETER,CURVE,DERIVATIVE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        IORDRE  : Maximum order of calculation of derivatives. */
/*        NDIMEN  : Dimension of the space. */
/*        NCOEFF  : Number of coefficients of the curve (degree+1). */
/*        COURBE  : Table of coefficients of the curve. */
/*        TPARAM  : Value of the parameter where the curve should be evaluated. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        TABPNT  : Table of values of consecutive derivatives */
/*                  of parameter TPARAM. */
  /*        IERCOD  : 0 = OK, */
/*                    1 = incoherent input. */

/*     COMMONS USED  : */
/*     ---------------- */
/*        None. */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     The algorithm of  calculation of derivatives is based on */
/*     generalization of the Horner scheme : */
/*                          k             2 */
/*          Let C(t) = uk.t  + ... + u2.t  + u1.t + u0 . */


/*      a0 = uk, b0 = 0, c0 = 0 and for 1<=j<=k, it is calculated : */

/*          aj = a(j-1).x + u(k-j) */
/*          bj = b(j-1).x + a(j-1) */
/*          cj = c(j-1).x + b(j-1) */

/*     So, it is obtained : C(x) = ak, C'(x) = bk, C"(x) = 2.ck  . */

/*     The algorithm can be easily generalized for the calculation of */

/*               (n) */
/*              C  (x)   . */
/*             --------- */
/*                n! */

/*      Reference : D. KNUTH, "The Art of Computer Programming" */
/*      ---------              Vol. 2/Seminumerical Algorithms */
/*                             Addison-Wesley Pub. Co. (1969) */
/*                             pages 423-425. */

/* ---> To evaluare derivatives at 0 and 1, it is preferable */
/*      to use routine MDRV01.FOR . */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

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

/* ------------------- Initialization of table TABPNT ----------------- 
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
    AdvApp2Var_SysBase::mvriraz_(&iptpnt, 
	     &tabpnt[tabpnt_dim1 + 1]);
L200:

/* ------------------------ Calculation of parameter TPARAM ------------------ 
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

/* --------------------- Multiplication by factorial(I) ------------- 
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
  integer i__, j, k, nd;
  doublereal mfactk, bid;
  

/*      IMPLICIT INTEGER (I-N) */
/*      IMPLICIT DOUBLE PRECISION(A-H,O-Z) */


/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*     Calculate the value of a derived curve of order IDERIV in */
/*     a point of parameter TPARAM. */

/*     KEYWORDS : */
/*     ----------- */
/*     POSITIONING,CURVE,DERIVATIVE of ORDER K. */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*   NCOEFF  : Degree +1 of the curve. */
/*   NDIMEN   : Dimension of the space (2 or 3 in general) */
/*   COURBE  : Table of coefficients of the curve. */
/*   IDERIV : Required order of derivation : 1=1st derivative, etc... */
/*   TPARAM : Value of parameter of the curve. */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*   PNTCRB  : Point of parameter TPARAM on the derivative of order */
/*            IDERIV of CURVE. */

/*     COMMONS USED   : */
/*     ---------------- */
/*    MMCMCNP */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*      None. */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*    The code below was written basing on the following algorithm : 
*/

/*    Let P(t) = a1 + a2*t + ... an*t**n. The derivative of order k of P */
/*    (containing n-k coefficients) is calculated as follows : */

/*       Pk(t) = a(k+1)*CNP(k,k)*k! */
/*             + a(k+2)*CNP(k+1,k)*k! * t */
/*             . */
/*             . */
/*             . */
/*             + a(n)*CNP(n-1,k)*k! * t**(n-k-1). */

/*    Evaluation is produced following the classic Horner scheme. */
/* > */
/* ***********************************************************************
 */


/*     Factorials (1 to 21)  caculated on VAX in R*16 */


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Serves to provide binomial coefficients (Pascal triangle). */

/*     KEYWORDS : */
/*     ----------- */
/*      Binomial Coeff from 0 to 60. read only . init by block data */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Binomial coefficients form a triangular matrix. */
/*     This matrix is completed in table CNP by its transposition. */
/*     So: CNP(I,J) = CNP(J,I) for I and J = 0, ..., 60. */

/*     Initialization is done by block-data MMLLL09.RES, */
/*     created by program MQINICNP.FOR. */
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

/* -------------- Case when the order of derivative is greater than ------------------- 
*/
/* ---------------- the degree of the curve --------------------- 
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
/*                         General processing*/
/* ********************************************************************** 
*/
/* --------------------- Calculation of Factorial(IDERIV) ------------------ 
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

/* ------- Calculation of derivative of order IDERIV of CURVE in TPARAM ----- 
*/
/* ---> Attention : binomial coefficient C(n,m) is represented in */
/*                 MCCNP by CNP(N,M). */

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

/*     FUNCTION : */
/*     ---------- */
/*        Extraction of EPS1 from COMMON MPRCSN. EPS1 is spatial zero  */
/*     equal to 1.D-9 */

/*     KEYWORDS : */
/*     ----------- */
/*        MPRCSN,PRECISON,EPS1. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        None */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        EPSILO : Value of EPS1 (spatial zero (10**-9)) */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     EPS1 is ABSOLUTE spatial zero, so it is necessary */
/*     to use it whenever it is necessary to test if a variable */
/*     is null. For example, if the norm of a vector is lower than */
/*     EPS1, this vector is NULL ! (when one works in */
/*     REAL*8) It is absolutely not advised to test arguments  */
/*     compared to EPS1**2. Taking into account the rounding errors inevitable */
/*     during calculations, this causes testing compared to 0.D0. */
/* > */
/* ***********************************************************************
 */



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*          Gives tolerances of invalidity in stream */
/*          as well as limits of iterative processes */

/*          general context, modifiable by the user */

/*     KEYWORDS : */
/*     ----------- */
/*          PARAMETER , TOLERANCE */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  profile , **VIA MPRFTX** at input in stream */
/*       loading of default values of the profile in MPRFTX at input */
/*       in stream. They are preserved in local variables of MPRFTX */

/*        Reset of default values                  : MDFINT */
/*        Interactive modification by the user   : MDBINT */

/*        ACCESS FUNCTION  :  MMEPS1   ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */
/* > */
/* ***********************************************************************
 */

/*     NITERM : max nb of iterations */
/*     NITERR : nb of rapid iterations */
/*     EPS1   : tolerance of 3D null distance */
/*     EPS2   : tolerance of parametric null distance */
/*     EPS3   : tolerance to avoid division by 0.. */
/*     EPS4   : angular tolerance */



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
	     NCollection_Array1<doublereal>& hwgaus)

{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  integer iadd, ideb, ndeg2, nmod2, ii, ibb;
  integer kpt;

/* ********************************************************************** 
*/

/*     FONCTION : */
/*     ---------- */
/*  Extract of common LDGRTL the weight of formulas of  */
/*  Gauss quadrature on all roots of Legendre polynoms of degree */
/*  NDEGRE defined on [-1,1]. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::COMMON&, EXTRACTION, &WEIGHT, &GAUSS. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDEGRE : Mathematic degree of Legendre polynom. It should have */
/*            2 <= NDEGRE <= 61. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   HWGAUS : The table of weights of Gauss quadrature formulas */
/*            relative to NDEGRE roots of a polynome de Legendre de */
/*            degre NDEGRE. */

/*     COMMONS UTILISES   : */
/*     ---------------- */
/*     MLGDRTL */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: The condition on NDEGRE ( 2 <= NDEGRE <= 61) is not  */
/*     tested. The caller should make the test. */

/*   Name of the routine */


/*   Common MLGDRTL: */
/*   This common includes POSITIVE roots of Legendre polynims */
/*   AND weights of Gauss quadrature formulas on all */
/*   POSITIVE roots of Legendre polynoms. */



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*   The common of Legendre roots. */

/*     KEYWORDS : */
/*     ----------- */
/*        BASE LEGENDRE */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */




/*   ROOTAB : Table of all roots of Legendre polynoms */
/*   within the interval [0,1]. They are ranked for the degrees increasing from */
/*   2 to 61. */
/*   HILTAB : Table of Legendre interpolators concerning ROOTAB. */
/*   The adressing is the same. */
/*   HI0TAB : Table of Legendre interpolators for root x=0 */
/*   of polynoms of UNEVEN degree. */
/*   RTLTB0 : Table of Li(uk) where uk are the roots of */
/*  Legendre polynom of EVEN degree. */
/*   RTLTB1 : Table of Li(uk) where uk are the roots of */
/*  Legendre polynom of UNEVEN degree. */


/************************************************************************
*****/

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMEXTHI", 7L);
    }

    ndeg2 = *ndegre / 2;
    nmod2 = *ndegre % 2;

/*   Address of Gauss weight associated to the 1st strictly */
/*   positive root of Legendre polynom of degree NDEGRE in MLGDRTL. */

    iadd = ndeg2 * (ndeg2 - 1) / 2 + 1;

/*   Index of the 1st HWGAUS element associated to the 1st strictly  */
/*   positive root of Legendre polynom of degree NDEGRE. */

    ideb = (*ndegre + 1) / 2 + 1;

/*   Reading of weights associated to strictly positive roots. */

    i__1 = *ndegre;
    for (ii = ideb; ii <= i__1; ++ii) {
	kpt = iadd + ii - ideb;
	hwgaus(ii) = mlgdrtl_.hiltab[kpt + nmod2 * 465 - 1];
/* L100: */
    }

/*   For strictly negative roots, the weight is the same. */
/*   i.e HW(1) = HW(NDEGRE), HW(2) = HW(NDEGRE-1), etc... */

    i__1 = ndeg2;
    for (ii = 1; ii <= i__1; ++ii) {
	hwgaus(ii) = hwgaus(*ndegre + 1 - ii);
/* L200: */
    }

/*   Case of uneven NDEGRE, 0 is root of Legendre polynom, */
/*   associated Gauss weights are loaded. */

    if (nmod2 == 1) {
	hwgaus(ndeg2 + 1) = mlgdrtl_.hi0tab[ndeg2];
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
	     NCollection_Array1<doublereal>& rootlg)
{
  /* System generated locals */
  integer i__1;
  
  /* Local variables */
  integer iadd, ideb, ndeg2, nmod2, ii, ibb;
  integer kpt;


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/* Extract of the Common LDGRTL of Legendre polynom roots */
/* of degree NDEGRE defined on [-1,1]. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::COMMON&, EXTRACTION, &ROOT, &LEGENDRE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDEGRE : Mathematic degree of Legendre polynom.  */
/*            It is required to have 2 <= NDEGRE <= 61. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   ROOTLG : The table of roots of Legendre polynom of degree */
/*            NDEGRE defined on [-1,1]. */

/*     COMMONS USED   : */
/*     ---------------- */
/*     MLGDRTL */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: Condition of NDEGRE ( 2 <= NDEGRE <= 61) is not */
/*     tested. The caller should make the test. */
/* > */
/* ********************************************************************** 
*/


/*   Name of the routine */


/*   Common MLGDRTL: */
/*   This common includes POSITIVE roots of Legendre polynoms */
/*   AND the weight of Gauss quadrature formulas on all */
/*   POSITIVE roots of Legendre polynoms. */

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*   The common of Legendre roots. */

/*     KEYWORDS : */
/*     ----------- */
/*        BASE LEGENDRE */


/* ***********************************************************************
 */

/*   ROOTAB : Table of all roots of Legendre polynoms */
/*   within the interval [0,1]. They are ranked for the degrees increasing from */
/*   2 to 61. */
/*   HILTAB : Table of Legendre interpolators concerning ROOTAB. */
/*   The adressing is the same. */
/*   HI0TAB : Table of Legendre interpolators for root x=0 */
/*   of polynoms of UNEVEN degree. */
/*   RTLTB0 : Table of Li(uk) where uk are the roots of */
/*  Legendre polynom of EVEN degree. */
/*   RTLTB1 : Table of Li(uk) where uk are the roots of */
/*  Legendre polynom of UNEVEN degree. */


/************************************************************************
*****/

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMEXTRL", 7L);
    }

    ndeg2 = *ndegre / 2;
    nmod2 = *ndegre % 2;

/*   Address of the 1st strictly positive root of Legendre polynom */
/*   of degree NDEGRE in MLGDRTL. */

    iadd = ndeg2 * (ndeg2 - 1) / 2 + 1;

/*   Indice, in ROOTLG, of the 1st strictly positive root */
/*   of Legendre polynom of degree NDEGRE. */

    ideb = (*ndegre + 1) / 2 + 1;

/*   Reading of strictly positive roots. */

    i__1 = *ndegre;
    for (ii = ideb; ii <= i__1; ++ii) {
	kpt = iadd + ii - ideb;
	rootlg(ii) = mlgdrtl_.rootab[kpt + nmod2 * 465 - 1];
/* L100: */
    }

/*   Strictly negative roots are equal to positive roots 
*/
/*   to the sign i.e RT(1) = -RT(NDEGRE), RT(2) = -RT(NDEGRE-1), etc... 
*/

    i__1 = ndeg2;
    for (ii = 1; ii <= i__1; ++ii) {
	rootlg(ii) = -rootlg(*ndegre + 1 - ii);
/* L200: */
    }

/*   Case NDEGRE uneven, 0 is root of Legendre polynom. */

    if (nmod2 == 1) {
	rootlg(ndeg2 + 1) = 0.;
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
int AdvApp2Var_MathBase::mmfmca8_(const integer *ndimen,
				  const integer *ncoefu,
				  const integer *ncoefv,
				  const integer *ndimax, 
				  const integer *ncfumx, 
				  const integer *,//ncfvmx, 
				  doublereal *tabini,
				  doublereal *tabres)

{
  /* System generated locals */
  integer tabini_dim1, tabini_dim2, tabini_offset, tabres_dim1, tabres_dim2,
  tabres_offset;

  /* Local variables */
  integer i__, j, k, ilong;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Expansion of a table containing only most important things into a  */
/*        greater data table. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, MATH_ACCES:: CARREAU&, DECOMPRESSION, &CARREAU */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMEN: Dimension of the workspace. */
/*        NCOEFU: Degree +1 of the table by u. */
/*        NCOEFV: Degree +1 of the table by v. */
/*        NDIMAX: Max dimension of the space. */
/*        NCFUMX: Max Degree +1 of the table by u. */
/*        NCFVMX: Max Degree +1 of the table by v. */
/*        TABINI: The table to be decompressed. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        TABRES: Decompressed table. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The following call : */

/*  CALL MMFMCA8(NDIMEN,NCOEFU,NCOEFV,NDIMAX,NCFUMX,NCFVMX,TABINI,TABINI) 
*/

/*     where TABINI is input/output argument, is possible provided */
/*     that the caller has declared TABINI in (NDIMAX,NCFUMX,NCFVMX) */

/*     ATTENTION : it is not checked that NDIMAX >= NDIMEN, */
/*                 NCOEFU >= NCFMXU and NCOEFV >= NCFMXV. */
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
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &tabini[(k * tabini_dim2 + 1) * tabini_dim1 + 1], 
		 &tabres[(k * tabres_dim2 + 1) * tabres_dim1 + 1]);
/* L500: */
    }
    goto L9999;

/* ----------------- decompression NDIMAX=NDIMEN,NCOEFU=NCFUMX ---------- 
*/

L2000:
    ilong = (*ndimen << 3) * *ncoefu * *ncoefv;
    AdvApp2Var_SysBase::mcrfill_(&ilong, 
	     &tabini[tabini_offset], 
	     &tabres[tabres_offset]);
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
  integer i__, j, k, ilong;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Compression of a data table in a table */
/*        containing only the main data (the input table is not removed). */

/*     KEYWORDS: */
/*     ----------- */
/*     ALL, MATH_ACCES:: CARREAU&, COMPRESSION, &CARREAU */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMAX: Max dimension of the space. */
/*        NCFUMX: Max degree +1 of the table by u. */
/*        NCFVMX: Max degree +1 of the table by v. */
/*        NDIMEN: Dimension of the workspace. */
/*        NCOEFU: Degree +1 of the table by u. */
/*        NCOEFV: Degree +1 of the table by v. */
/*        TABINI: The table to compress. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        TABRES: The compressed table. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The following call : */

/* CALL MMFMCA9(NDIMAX,NCFUMX,NCFVMX,NDIMEN,NCOEFU,NCOEFV,TABINI,TABINI) 
*/

/*     where TABINI is input/output argument, is possible provided */
/*     that the caller has checked that : */

/*            NDIMAX > NDIMEN, */
/*         or NDIMAX = NDIMEN and NCFUMX > NCOEFU */
/*         or  NDIMAX = NDIMEN, NCFUMX = NCOEFU and NCFVMX > NCOEFV */

/*     These conditions are not tested in the program. */

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
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &tabini[(k * tabini_dim2 + 1) * tabini_dim1 + 1], 
		 &tabres[(k * tabres_dim2 + 1) * tabres_dim1 + 1]);
/* L500: */
    }
    goto L9999;

/* ----------------- Compression NDIMEN=NDIMAX,NCOEFU=NCFUMX ------------ 
*/

L2000:
    ilong = (*ndimen << 3) * *ncoefu * *ncoefv;
    AdvApp2Var_SysBase::mcrfill_(&ilong,
	     &tabini[tabini_offset], 
	     &tabres[tabres_offset]);
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
  integer c__8 = 8;
  /* System generated locals */
    integer patold_dim1, patold_dim2, patnew_dim1, patnew_dim2,
	    i__1, patold_offset,patnew_offset;

    /* Local variables */
    doublereal* tbaux = 0;
    integer ksize, numax, kk;
    intptr_t iofst;
    integer ibb, ier;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       LIMITATION OF A SQUARE DEFINED ON (0,1)*(0,1) BETWEEN ISOS */
/*       UPARA1 AND UPARA2 (BY U) AND VPARA1 AND VPARA2 BY V. */

/*     KEYWORDS : */
/*     ----------- */
/*       LIMITATION , SQUARE , PARAMETER */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NCOFMX: MAX NUMBER OF COEFF OF THE SQUARE BY U */
/*     NCOEFU: NUMBER OF COEFF OF THE SQUARE BY U */
/*     NCOEFV: NUMBER OF COEFF OF THE SQUARE BY V */
/*     PATOLD : THE SQUARE IS LIMITED BY UPARA1,UPARA2 AND VPARA1,VPARA2
.*/
/*     UPARA1    : LOWER LIMIT OF U */
/*     UPARA2    : UPPER LIMIT OF U */
/*     VPARA1    : LOWER LIMIT OF V */
/*     VPARA2    : UPPER LIMIT OF V */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     PATNEW : RELIMITED SQUARE, DEFINED ON (0,1)**2 */
/*     IERCOD : =10 COEFF NB TOO GREAT OR NULL */
/*              =13 PB IN THE DYNAMIC ALLOCATION */
/*              = 0 OK. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* --->    The following call : */
/*   CALL MMFMCAR(NCOFMX,NCOEFU,NCOEFV,PATOLD,UPARA1,UPARA2,VPARA1,VPARA2 
*/
/*              ,PATOLD), */
/*        where PATOLD is input/output argument is absolutely legal. */

/* --->    The max number of coeff by u and v of PATOLD is 61 */

/* --->    If NCOEFU < NCOFMX, the data is compressed by MMFMCA9 before */
/*        limitation by v to get time during the execution */
/*        of MMARC41 that follows (the square is processed as a curve of 
*/
/*        dimension NDIMEN*NCOEFU possessing NCOEFV coefficients). */
/* > */
/* ***********************************************************************
 */

/*   Name of the routine */


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
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;

/* ********************************************************************** 
*/
/*                  TEST OF COEFFICIENT NUMBERS */
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
/*                  CASE WHEN UPARA1=VPARA1=0 AND UPARA2=VPARA2=1 */
/* ********************************************************************** 
*/

    if (*upara1 == 0. && *upara2 == 1. && *vpara1 == 0. && *vpara2 == 1.) {
	ksize = (*ndimen << 3) * *ncofmx * *ncoefv;
	AdvApp2Var_SysBase::mcrfill_(&ksize, 
		 &patold[patold_offset], 
		 &patnew[patnew_offset]);
	goto L9999;
    }

/* ********************************************************************** 
*/
/*                        LIMITATION BY U */
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
/*                         LIMITATION BY V */
/* ********************************************************************** 
*/

L2000:
    if (*vpara1 == 0. && *vpara2 == 1.) {
	goto L9999;
    }

/* ----------- LIMITATION BY V (WITH COMPRESSION I.E. NCOEFU<NCOFMX) ---- 
*/

    numax = *ndimen * *ncoefu;
    if (*ncofmx != *ncoefu) {
/* ------------------------- Dynamic allocation -------------------
---- */
	ksize = *ndimen * *ncoefu * *ncoefv;
	anAdvApp2Var_SysBase.mcrrqst_(&c__8, &ksize, tbaux, &iofst, &ier);
	if (ier > 0) {
	    *iercod = 13;
	    goto L9900;
	}
/* --------------- Compression by (NDIMEN,NCOEFU,NCOEFV) ------------
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
/* ------------------------- Limitation by v ------------------------
---- */
	mmarc41_(&numax, &numax, ncoefv, &tbaux[iofst], vpara1, vpara2, &
		tbaux[iofst], iercod);
/* --------------------- Expansion of TBAUX into PATNEW -------------
--- */
	AdvApp2Var_MathBase::mmfmca8_(ndimen, ncoefu, ncoefv, ndimen, ncofmx, ncoefv, &tbaux[iofst]
		, &patnew[patnew_offset]);
	goto L9900;

/* -------- LIMITATION BY V (WITHOUT COMPRESSION I.E. NCOEFU=NCOFMX) ---
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
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, &ksize, tbaux, &iofst, &ier);
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
  integer i__, nboct, nd;
  

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*       Reformating (and  eventual compression/decompression) of curve */
/*       (ndim,.) by (.,ndim) and vice versa. */

/*     KEYWORDS : */
/*     ----------- */
/*      ALL , MATH_ACCES :: */
/*      COURBE&, REORGANISATION,COMPRESSION,INVERSION , &COURBE */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*        ISENMSC : required direction of the transfer : */
/*           1   :  passage of (NDIMEN,.) ---> (.,NDIMEN)  direction to AB 
*/
/*          -1   :  passage of (.,NDIMEN) ---> (NDIMEN,.)  direction to TS,T
V*/
/*        NDIMAX : format / dimension */
/*        NCF1MX : format by t of COURB1 */
/*   if ISENMSC= 1 : COURB1: The curve to be processed (NDIMAX,.) */
/*        NCOEFF : number of coeff of the curve */
/*        NCF2MX : format by t of COURB2 */
/*        NDIMEN : dimension of the curve and format of COURB2 */
/*   if ISENMSC=-1 : COURB2: The curve to be processed (.,NDIMEN) */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*   if ISENMSC= 1 : COURB2: The resulting curve (.,NDIMEN) */
/*   if ISENMSC=-1 : COURB1: The resulting curve (NDIMAX,.) */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     allow to process the usual transfers as follows : */
/*     | ---- ISENMSC = 1 ---- |      | ---- ISENMSC =-1 ----- | */
/*    TS  (3,21) --> (21,3)  AB  ;  AB  (21,3) --> (3,21)  TS */
/*    TS  (3,21) --> (NU,3)  AB  ;  AB  (NU,3) --> (3,21)  TS */
/*        (3,NU) --> (21,3)  AB  ;  AB  (21,3) --> (3,NU) */
/*        (3,NU) --> (NU,3)  AB  ;  AB  (NU,3) --> (3,NU) */
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
	    AdvApp2Var_SysBase::mcrfill_(&nboct, 
		     &courb1[courb1_offset], 
		     &courb2[courb2_offset]);
	}
	if (*isenmsc == -1) {
	    AdvApp2Var_SysBase::mcrfill_(&nboct, 
		     &courb2[courb2_offset], 
		     &courb1[courb1_offset]);
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
  integer c__8 = 8;

   /* System generated locals */
    integer table1_dim1, table1_offset, table2_dim1, table2_offset, i__1, 
	    i__2;

    /* Local variables */
    doublereal* work = 0;
    integer ilong, isize, ii, jj, ier = 0;
    intptr_t iofst = 0,iipt, jjpt;


/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*     Inversion of elements of a rectangular table (T1(i,j) */
/*     loaded in T2(j,i)) */

/*     KEYWORDS : */
/*     ----------- */
/*      ALL, MATH_ACCES :: TABLEAU&, INVERSION, &TABLEAU */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     MAXSZ1: Max Nb of elements by the 1st dimension of TABLE1. */
/*     TABLE1: Table of reals by two dimensions. */
/*     ISIZE1: Nb of useful elements of TABLE1 on the 1st dimension */
/*     JSIZE1: Nb of useful elements of TABLE1 on the 2nd dimension */
/*     MAXSZ2: Nb max of elements by the 1st dimension of TABLE2. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     TABLE2: Table of reals by two dimensions, containing the transposition */
/*             of the rectangular table TABLE1. */
/*     ISIZE2: Nb of useful elements of TABLE2 on the 1st dimension */
/*     JSIZE2: Nb of useful elements of TABLE2 on the 2nd dimension */
/*     IERCOD: Erroe coder. */
/*             = 0, ok. */
/*             = 1, error in the dimension of tables */
/*                  ether MAXSZ1 < ISIZE1 (table TABLE1 too small). */
/*                  or MAXSZ2 < JSIZE1 (table TABLE2 too small). */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*    It is possible to use TABLE1 as input and output table i.e. */
/*    call: */
/*    CALL MMFMTB1(MAXSZ1,TABLE1,ISIZE1,JSIZE1,MAXSZ2,TABLE1 */
/*               ,ISIZE2,JSIZE2,IERCOD) */
/*    is valuable. */
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
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;

    /* Function Body */
    *iercod = 0;
    if (*isize1 > *maxsz1 || *jsize1 > *maxsz2) {
	goto L9100;
    }

    iofst = 0;
    isize = *maxsz2 * *isize1;
    anAdvApp2Var_SysBase.mcrrqst_(&c__8, &isize, work, &iofst, &ier);
    if (ier > 0) {
	goto L9200;
    }

/*             DO NOT BE AFRAID OF CRUSHING. */

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
    AdvApp2Var_SysBase::mcrfill_(&ilong, 
	     &work[iofst], 
	     &table2[table2_offset]);

/* -------------- The number of elements of TABLE2 is returned ------------ 
*/

    ii = *isize1;
    *isize2 = *jsize1;
    *jsize2 = ii;

    goto L9999;

/* ------------------------------- THE END ------------------------------ 
*/
/* --> Invalid input. */
L9100:
    *iercod = 1;
    goto L9999;
/* --> Pb of allocation. */
L9200:
    *iercod = 2;
    goto L9999;

L9999:
    if (iofst != 0) {
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, &isize, work, &iofst, &ier);
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
  integer ndeg;
  doublereal h__[20];
  integer j;
  doublereal t, u[20], x;
  integer idimf;
  doublereal c1x, c2x;
/* ********************************************************************** 
*/

/*      FUNCTION : */
/*      -------- */

/*      Calculate the integral of  function BFUNX passed in parameter */
/*      between limits XD and XF . */
/*      The function should be calculated for any value */
/*      of the variable in the given interval.. */
/*      The method GAUSS-LEGENDRE is used. */
/*      For explications refer to the book : */
/*          Complements de mathematiques a l'usage des Ingenieurs de */
/*          l'electrotechnique et des telecommunications. */
/*          Par Andre ANGOT - Collection technique et scientifique du CNET
 */
/*          page 772 .... */
/*      The degree of LEGENDRE polynoms used is passed in parameter.
 */
/*      KEYWORDS : */
/*      --------- */
/*         INTEGRATION,LEGENDRE,GAUSS */

/*      INPUT ARGUMENTS : */
/*      ------------------ */

/*      NDIMF : Dimension of the function */
/*      BFUNX : Function to integrate passed as argument */
/*              Should be declared as EXTERNAL in the call routine. */
/*                   SUBROUTINE BFUNX(NDIMF,X,VAL,IER) */
/*                   REAL *8 X,VAL */
/*     K      : Parameter determining the degree of the LEGENDRE polynom that 
*/
/*               can take a value between 0 and 10. */
/*               The degree of the polynom is equal to 4 k, that is 4, 8, 
*/
/*               12, 16, 20, 24, 28, 32, 36 and 40. */
/*               If K is not correct, the degree is set to 40 directly. 
*/
/*      XD     : Lower limit of the interval of integration. */
/*      XF     : Upper limit of the interval of integration. */
/*      SAUX1  : Auxiliary table */
/*      SAUX2  : Auxiliary table */

/*      OUTPUT ARGUMENTS : */
/*      ------------------- */

/*      SOMME : Value of the integral */
/*      NITER : Number of iterations to be carried out. */
/*              It is equal to the degree of the polynom. */

/*      IER   : Error code : */
/*              < 0 ==> Attention - Warning */
/*              = 0 ==> Everything is OK */
/*              > 0 ==> Critical error - Apply special processing */
/*                  ==> Error in the calculation of BFUNX (return code */
/*                      of this routine */

/*              If error => SUM = 0 */

/*      COMMONS USED : */
/*      ----------------- */



/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     Type  Name */
/*    @      BFUNX               MVGAUS0 */

/*      DESCRIPTION/NOTES/LIMITATIONS : */
/*      --------------------------------- */

/*      See the explanations detailed in the listing */
/*      Use of the GAUSS method (orthogonal polynoms) */
/*      The symmetry of roots of these polynomes is used */
/*      Depending on K, the degree of the interpolated polynom grows. 
*/
/*      If you wish to calculate the integral with a given precision, */
/*      loop on k varying from 1 to 10 and test the difference of 2
*/
/*      consecutive iterations. Stop the loop if this difference is less that */
/*      an epsilon value set to 10E-6 for example. */
/*      If S1 and S2 are 2 successive iterations, test following this example :
 */

/*            AF=DABS(S1-S2) */
/*            AS=DABS(S2) */
/*            If AS < 1 test if FS < eps otherwise test if AF/AS < eps 
*/
/*            --        -----                    ----- */
/* > */
/************************************************************************
******/
/*     DECLARATIONS */
/************************************************************************
******/



/* ****** General Initialization */

    /* Parameter adjustments */
    --somme;
    --saux2;
    --saux1;

    /* Function Body */
    AdvApp2Var_SysBase::mvriraz_(ndimf, 
	     &somme[1]);
    *iercod = 0;

/* ****** Loading of coefficients U and H ** */
/* -------------------------------------------- */

    mvgaus0_(k, u, h__, &ndeg, iercod);
    if (*iercod > 0) {
	goto L9999;
    }

/* ****** C1X => Medium interval point  [XD,XF] */
/* ****** C2X => 1/2 amplitude interval [XD,XF] */

    c1x = (*xf + *xd) * .5;
    c2x = (*xf - *xd) * .5;

/* ---------------------------------------- */
/* ****** Integration for degree NDEG ** */
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

/* ****** End of sub-program ** */

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
  integer c__576 = 576;
  integer c__6 = 6;

  
   /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    doublereal amat[36]	/* was [6][6] */;
    integer iord[2];
    doublereal prod;
    integer iord1, iord2;
    doublereal miden[36]	/* was [6][6] */;
    integer ncmat;
    doublereal epspi, d1, d2;
    integer ii, jj, pp, ncf;
    doublereal cof[6];
    integer iof[2], ier;
    doublereal mat[36]	/* was [6][6] */;
    integer cot;
    doublereal abid[72]	/* was [12][6] */;
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*      INIT OF COEFFS. OF POLYNOMS OF HERMIT INTERPOLATION */

/*     KEYWORDS : */
/*     ----------- */
/*      MATH_ACCES :: HERMITE */

/*     INPUT ARGUMENTS */
/*     -------------------- */
/*       DEBFIN : PARAMETERS DEFINING THE CONSTRAINTS */
/*                 DEBFIN(1) : FIRST PARAMETER */
/*                 DEBFIN(2) : SECOND PARAMETER */

/*      ONE SHOULD HAVE: */
/*                 ABS (DEBFIN(I)) < 100 */
/*                 and */
/*                 (ABS(DEBFIN(1)+ABS(DEBFIN(2))) > 1/100 */
/*           (for overflows) */

/*      ABS(DEBFIN(2)-DEBFIN(1)) / (ABS(DEBFIN(1)+ABS(DEBFIN(2))) > 1/100 
*/
/*           (for the conditioning) */


/*     OUTPUT ARGUMENTS : */
/*     --------------------- */

/*       IERCOD : Error code : 0 : O.K. */
/*                                1 : value of DEBFIN */
/*                                are unreasonable */
/*                                -1 : init was already done */
/*                                   (OK but no processing) */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     ---------------------- */
/*     Type  Name */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*        This program initializes the coefficients of Hermit polynoms */
/*     that are read later by MMHERM1 */
/* ***********************************************************************
 */



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Used to STORE  coefficients of Hermit interpolation polynoms */

/*     KEYWORDS : */
/*     ----------- */
/*      HERMITE */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     The coefficients of hermit polynoms are calculated by */
/*     the routine MMHERM0 and read by the routine MMHERM1 */
/* > */
/* ********************************************************************** 
*/





/*     NBCOEF is the size of CMHERM (see below) */
/* ***********************************************************************
 */







/* ***********************************************************************
 */
/*     Data checking */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --debfin;

    /* Function Body */
    d1 = advapp_abs(debfin[1]);
    if (d1 > (float)100.) {
	goto L9101;
    }

    d2 = advapp_abs(debfin[2]);
    if (d2 > (float)100.) {
	goto L9101;
    }

    d2 = d1 + d2;
    if (d2 < (float).01) {
	goto L9101;
    }

    d1 = (d__1 = debfin[2] - debfin[1], advapp_abs(d__1));
    if (d1 / d2 < (float).01) {
	goto L9101;
    }


/* ***********************************************************************
 */
/*     Initialization */
/* ***********************************************************************
 */

    *iercod = 0;

    epspi = 1e-10;


/* ***********************************************************************
 */

/*     IS IT ALREADY INITIALIZED ? */

    d1 = advapp_abs(debfin[1]) + advapp_abs(debfin[2]);
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
/*     CALCULATION */
/* ***********************************************************************
 */


L100:

/*     Init. matrix identity : */

    ncmat = 36;
    AdvApp2Var_SysBase::mvriraz_(&ncmat, 
	     miden);

    for (ii = 1; ii <= 6; ++ii) {
	miden[ii + ii * 6 - 7] = 1.;
/* L110: */
    }



/*     Init to 0 of table CMHERM */

    AdvApp2Var_SysBase::mvriraz_(&c__576, mmcmher_.cmherm);

/*     Calculation by solution of linear systems */

    for (iord1 = -1; iord1 <= 2; ++iord1) {
	for (iord2 = -1; iord2 <= 2; ++iord2) {

	    iord[0] = iord1;
	    iord[1] = iord2;


	    iof[0] = 0;
	    iof[1] = iord[0] + 1;


	    ncf = iord[0] + iord[1] + 2;

/*        Calculate matrix MAT to invert: */

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

/*        everything is done in these 3 lines 
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

/*     The initialized flag is located: */

    mmcmher_.tdebut = debfin[1];
    mmcmher_.tfinal = debfin[2];

    d1 = advapp_abs(debfin[1]) + advapp_abs(debfin[2]);
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
  integer nbval;
  doublereal d1;
  integer cot;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*      reading of coeffs. of HERMIT interpolation polynoms */

/*     KEYWORDS : */
/*     ----------- */
/*      MATH_ACCES :: HERMIT */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       DEBFIN : PARAMETES DEFINING THE CONSTRAINTS */
/*                 DEBFIN(1) : FIRST PARAMETER */
/*                 DEBFIN(2) : SECOND PARAMETER */

/*           Should be equal to the corresponding arguments during the */
/*           last call to MMHERM0 for the initialization of coeffs. */

/*       ORDRMX : indicates the dimensioning of HERMIT: */
/*              there is no choice : ORDRMX should be equal to the value */
/*              of PARAMETER IORDMX of INCLUDE MMCMHER, or 2 for the moment */

/*       IORDRE (2) : Orders of constraints in each corresponding parameter DEBFIN(I) */
/*              should be between -1 (no constraints) and ORDRMX. */


/*     OUTPUT ARGUMENTS : */
/*     --------------------- */

/*       HERMIT : HERMIT(1:IORDRE(1)+IORDRE(2)+2, j, cote) are the  */
/*       coefficients in the canonic base of Hermit polynom */
/*       corresponding to orders IORDRE with parameters DEBFIN for */
/*       the constraint of order j on DEBFIN(cote). j is between 0 and IORDRE(cote). */


/*       IERCOD : Error code : */
/*          -1: O.K but necessary to reinitialize the coefficients */
/*                 (info for optimization) */
/*          0 : O.K. */
/*          1 : Error in MMHERM0 */
/*          2 : arguments invalid */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     This program reads coefficients of Hermit polynoms */
/*     that were earlier initialized by MMHERM0 */

/* PMN : initialisation is no more done by the caller. */


/* ***********************************************************************
 */



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Serves to STORE the coefficients of Hermit interpolation polynoms */

/*     KEYWORDS : */
/*     ----------- */
/*      HERMITE */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*     the coefficients of Hetmit polynoms are calculated by */
/*     routine MMHERM0 and read by routine MMHERM1 */

/* > */
/* ********************************************************************** 
*/





/*     NBCOEF is the size of CMHERM (see lower) */



/* ***********************************************************************
 */





/* ***********************************************************************
 */
/*     Initializations */
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
/*     Data Checking */
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


/*     IS-IT CORRECTLY INITIALIZED ? */

    d1 = advapp_abs(debfin[1]) + advapp_abs(debfin[2]);
    d1 *= 16111959;

/*     OTHERWISE IT IS INITIALIZED */

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
/*        READING */
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
  integer c__2 = 2;
  integer c__21 = 21;
  /* System generated locals */
    integer tcbold_dim1, tcbold_dim2, tcbold_offset, tcbnew_dim1, tcbnew_dim2,
	     tcbnew_offset, i__1, i__2, i__3, i__4, i__5;


    /* Local variables */
    logical ldbg;
    integer ndeg;
    doublereal taux1[21];
    integer d__, e, i__, k;
    doublereal mfact;
    integer ncoeff;
    doublereal tjacap[21];
    integer iordre[2];
    doublereal hermit[36]/* was [6][3][2] */, ctenor, bornes[2];
    integer ier;
    integer aux1, aux2;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       CONVERSION OF TABLE TCBOLD OF POLYNOMIAL CURVE COEFFICIENTS */
/*       EXPRESSED IN HERMIT JACOBI BASE, INTO A */
/*       TABLE OF COEFFICIENTS TCBNEW OF COURVES EXPRESSED IN THE CANONIC BASE */

/*     KEYWORDS : */
/*     ----------- */
/*      CANNONIC, HERMIT, JACCOBI */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       ORDHER : ORDER OF HERMIT POLYNOMS OR ORDER OF CONTINUITY */
/*       NCOEFS : NUMBER OF COEFFICIENTS OF A POLYNOMIAL CURVE */
/*                FOR ONE OF ITS NDIM COMPONENTS;(DEGREE+1 OF THE CURVE) 
*/
/*       NDIM   : DIMENSION OF THE CURVE */
/*       CBHEJA : TABLE OF COEFFICIENTS OF THE CURVE IN THE BASE */
/*                HERMIT JACOBI */
/*                (H(0,-1),..,H(ORDHER,-1),H(0,1),..,H(ORDHER,1), */
/*                 JA(ORDHER+1,2*ORDHER+2),....,JA(ORDHER+1,NCOEFS-1) */

/*     OUTPUT ARGUMENTS  : */
/*     --------------------- */
/*       CBRCAN : TABLE OF COEFFICIENTS OF THE CURVE IN THE CANONIC BASE */
/*                (1, t, ...) */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/* ***********************************************************************
 */


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Providesinteger constants from 0 to 1000 */

/*     KEYWORDS : */
/*     ----------- */
/*        ALL, INTEGER */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */


/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                      INITIALIZATION */
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
/*                     PROCESSING */
/* ***********************************************************************
 */

    if (*orcont > 2) {
	goto L9101;
    }
    if (*ncflim > 21) {
	goto L9101;
    }

/*     CALCULATION OF HERMIT POLYNOMS IN THE CANONIC BASE ON (-1,1) */


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

/*     CONVERSION OF THE COEFFICIENTS OF THE PART OF THE CURVE EXPRESSED */
/*     IN HERMIT BASE, INTO THE CANONIC BASE */

	    AdvApp2Var_SysBase::mvriraz_(&ncoeff, taux1);

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

/*     CONVERSION OF THE COEFFICIENTS OF THE PART OF THE CURVE EXPRESSED */
/*     IN CANONIC-JACOBI BASE, INTO THE CANONIC BASE */



	    AdvApp2Var_MathBase::mmapcmp_(&minombr_.nbr[1], &c__21, &ncoeff, taux1, tjacap);
	    AdvApp2Var_MathBase::mmjacan_(orcont, &ndeg, tjacap, taux1);

/*        RECOPY THE COEFS RESULTING FROM THE CONVERSION IN THE TABLE */
/*        OF RESULTS */

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
/*                   PROCESSING OF ERRORS */
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
/*                   RETURN CALLING PROGRAM */
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
  logical idbg;
  integer icol, ilgn, nlgn, noct, inser;
  doublereal epsega = 0.;
  integer ibb;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        . Insert a line in a table parsed without redundance */

/*     KEYWORDS : */
/*     ----------- */
/*      TOUS,MATH_ACCES :: TABLEAU&,INSERTION,&TABLEAU */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*        . NCOLMX : Number of columns in the table */
/*        . NLGNMX : Number of lines in the table */
/*        . TABTRI : Table parsed by lines without redundances */
/*        . NBRCOL : Number of columns used */
/*        . NBRLGN : Number of lines used */
/*        . AJOUTE : Line to be added */
/*        . EPSEGA : Epsilon to test the redundance */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*        . TABTRI : Table parsed by lines without redundances */
/*        . NBRLGN : Number of lines used */
/*        . IERCOD : 0 -> No problem */
/*                   1 -> The table is full */

/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*        . The line is inserted only if there is no line with all 
*/
/*     elements equl to those which are planned to be insered, to epsilon. */

/*        . Level of de debug = 3 */


/**/
/*     DECLARATIONS , CONTROL OF INPUT ARGUMENTS , INITIALIZATION */
/* ***********************************************************************
 */

/* --- Parameters */


/* --- Functions */


/* --- Local variables */


/* --- Messages */

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

/* --- Control arguments */

    if (*nbrlgn >= *nlgnmx) {
	goto L9001;
    }

/* -------------------- */
/* *** INITIALIZATION */
/* -------------------- */

    *iercod = 0;

/* ---------------------------- */
/* *** SEARCH OF REDUNDANCE */
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
/* *** SEARCH OF THE INSERTION POINT */
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

/* --- Shift lower */

    nlgn = *nbrlgn - inser;
    if (nlgn > 0) {
	noct = (*ncolmx << 3) * nlgn;
	AdvApp2Var_SysBase::mcrfill_(&noct, 
		 &tabtri[inser * tabtri_dim1 + 1], 
		 &tabtri[(inser + 1)* tabtri_dim1 + 1]);
    }

/* --- Copy line */

    noct = *nbrcol << 3;
    AdvApp2Var_SysBase::mcrfill_(&noct, 
	     &ajoute[1], 
	     &tabtri[inser * tabtri_dim1 + 1]);

    goto L9999;

/* ******************************************************************** */
/*       OUTPUT ERROR , RETURN CALLING PROGRAM , MESSAGES */
/* ******************************************************************** */

/* --- The table is already full */

L9001:
    *iercod = 1;

/* --- End */

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
 int AdvApp2Var_MathBase::mmjacan_(const integer *ideriv, 
			    integer *ndeg, 
			    doublereal *poljac, 
			    doublereal *polcan)
{
    /* System generated locals */
  integer poljac_dim1, i__1, i__2;
  
  /* Local variables */
  integer iptt, i__, j, ibb;
  doublereal bid;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*     Routine of transfer of Jacobi normalized to canonic [-1,1], */
/*     the tables are ranked by even, then by uneven degree. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,JACOBI,PASSAGE. */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*        IDERIV : Order of Jacobi between -1 and 2. */
/*        NDEG :   The true degree of the polynom. */
/*        POLJAC : The polynom in the Jacobi base. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        POLCAN : The curve expressed in the canonic base [-1,1]. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */

/*   Name of the routine */

/*   Matrices of conversion */


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        MATRIX OF TRANSFORMATION OF LEGENDRE BASE */

/*     KEYWORDS : */
/*     ----------- */
/*        MATH */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */



/*  Legendre common / Restricted Casteljau. */

/*   0:1      0 Concerns the even terms, 1 the uneven terms. */
/*   CANPLG : Matrix of passage to canonic from Jacobi with calculated parities */
/*   PLGCAN : Matrix of passage from Jacobi to canonic with calculated parities */


/* ***********************************************************************
 */

    /* Parameter adjustments */
    poljac_dim1 = *ndeg / 2 + 1;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 5) {
	AdvApp2Var_SysBase::mgenmsg_("MMJACAN", 7L);
    }

/* ----------------- Expression of terms of even degree ---------------- 
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

/* --------------- Expression of terms of uneven degree ---------------- 
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
 int AdvApp2Var_MathBase::mmjaccv_(const integer *ncoef, 
			    const integer *ndim, 
			    const integer *ider, 
			    const doublereal *crvlgd,
			    doublereal *polaux,
			    doublereal *crvcan)

{
  /* Initialized data */
  
  static char nomprg[8+1] = "MMJACCV ";
  
  /* System generated locals */
  integer crvlgd_dim1, crvlgd_offset, crvcan_dim1, crvcan_offset, 
  polaux_dim1, i__1, i__2;
  
  /* Local variables */
  integer ndeg, i__, nd, ii, ibb;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Passage from the normalized Jacobi base to the canonic base. */

/*     KEYWORDS : */
/*     ----------- */
/*        SMOOTHING, BASE, LEGENDRE */


/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIM: Space Dimension. */
/*        NCOEF: Degree +1 of the polynom. */
/*        IDER: Order of Jacobi polynoms. */
/*        CRVLGD : Curve in the base of Jacobi. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        POLAUX : Auxilliary space. */
/*        CRVCAN : The curve in the canonic base [-1,1] */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ********************************************************************* 
*/

/*   Name of the routine */
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
/*  Loading of the auxilliary table. */
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
/*   Call the routine of base change. */
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
  
  integer kgar = 0;
  
  /* System generated locals */
  integer courbe_dim1, courbe_offset, i__1, i__2;
  
  /* Local variables */
  doublereal tran;
  integer ngaus = 0;
  doublereal c1, c2, d1, d2,
    wgaus[20] = {0.}, uroot[20] = {0.}, x1, x2, dd;
  integer ii, jj, kk;
  doublereal som;
  doublereal der1, der2;




/* ********************************************************************** 
*/

/*     FUNCTION : Length of an arc of curve on a given interval */
/*     ---------- for a function the mathematic representation  */
/*                which of is a multidimensional polynom. */
/*      The polynom is a set of polynoms the coefficients which of are ranked */
/*  in a table with 2 indices, each line relative to 1 polynom. */
/*      The polynom is defined by its coefficients ordered by increasing 
*       power of the variable. */
/*      All polynoms have the same number of coefficients (and the same degree). */

/*     KEYWORDS : LENGTH, CURVE */
/*     ----------- */

/*     INPUT ARGUMENTS : */
/*     -------------------- */

/*      NDIMAX : Max number of lines of tables (max number of polynoms). */
/*      NDIMEN : Dimension of the polynom (Nomber of polynoms). */
/*      NCOEFF : Number of coefficients of the polynom (no limitation) */
/*               This is degree + 1 */
/*      COURBE : Coefficients of the polynom ordered by increasing power */
/*               Dimension to (NDIMAX,NCOEFF). */
/*      TDEBUT : Lower limit of integration for length calculation. */
/*      TFINAL : Upper limit of integration for length calculation.  */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*      XLONGC : Length of arc of curve */

/*      IERCOD : Error code : */
/*             = 0 ==> All is OK */
/*             = 1 ==> NDIMEN or NCOEFF negative or null */
/*             = 2 ==> Pb loading Legendre roots and Gauss weight */
/*                     by MVGAUS0. */

/*     If error => XLONGC = 0 */

/*     COMMONS USED : */
/*     ------------------ */

/*      .Neant. */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG         R*8  DSQRT          I*4  MIN */
/*           MVGAUS0 */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/*      See VGAUSS to understand well the technique. */
/*      Actually SQRT (dpi^2) is integrated for i=1,nbdime */
/*      Calculation of the derivative is included in the code to avoid an additional */
/*      call of the routine. */

/*      The integrated function is strictly increasing, it */
/*      is not necessary to use a high degree for the GAUSS method GAUSS. */

/*      The degree of LEGENDRE polynom results from the degree of the */
/*      polynom to be integrated. It can vary from 4 to 40 (with step of 4). */

/*      The precision (relative) of integration is of order 1.D-8. */

/*      ATTENTION : if TDEBUT > TFINAL, the length is NEGATIVE. */

/*      Attention : the precision of the result is not controlled. */
/*      If you wish to control it, use  MMCGLC1, taking into account that  */
/*      the performance (in time) will be worse. */

/* >===================================================================== 
*/

/*      ATTENTION : SAVE KGAR WGAUS and UROOT EVENTUALLY */
/*     ,IERXV */
/*      INTEGER I1,I20 */
/*      PARAMETER (I1=1,I20=20) */

    /* Parameter adjustments */
    courbe_dim1 = *ndimax;
    courbe_offset = courbe_dim1 + 1;
    courbe -= courbe_offset;

    /* Function Body */

/* ****** General initialization ** */

    *iercod = 999999;
    *xlongc = 0.;

/* ****** Initialization of UROOT, WGAUS, NGAUS and KGAR ** */

/*      CALL MXVINIT(IERXV,'INTEGER',I1,KGAR,'INTEGER',I1,NGAUS */
/*     1    ,'DOUBLE PRECISION',I20,UROOT,'DOUBLE PRECISION',I20,WGAUS) */
/*      IF (IERXV.GT.0) KGAR=0 */

/* ****** Test the equity of limits ** */

    if (*tdebut == *tfinal) {
	*iercod = 0;
	goto L9900;
    }

/* ****** Test the dimension and the number of coefficients ** */

    if (*ndimen <= 0 || *ncoeff <= 0) {
	*iercod = 1;
	goto L9900;
    }

/* ****** Calculate the optimal degree ** */

    kk = *ncoeff / 4 + 1;
    kk = advapp_min(kk,10);

/* ****** Return the coefficients for the integral (DEGRE=4*KK) */
/*       if KK <> KGAR. */

    if (kk != kgar) {
	mvgaus0_(&kk, uroot, wgaus, &ngaus, iercod);
	if (*iercod > 0) {
	    kgar = 0;
	    *iercod = 2;
	    goto L9900;
	}
	kgar = kk;
    }

/*      C1 => Point medium interval */
/*      C2 => 1/2 amplitude interval */

    c1 = (*tfinal + *tdebut) * .5;
    c2 = (*tfinal - *tdebut) * .5;

/* ----------------------------------------------------------- */
/* ****** Integration - Loop on GAUSS intervals ** */
/* ----------------------------------------------------------- */

    som = 0.;

    i__1 = ngaus;
    for (jj = 1; jj <= i__1; ++jj) {

/* ****** Integration taking the symmetry into account ** */

	tran = c2 * uroot[jj - 1];
	x1 = c1 + tran;
	x2 = c1 - tran;

/* ****** Derivation on the dimension of the space ** */

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

/* ****** End of loop on GAUSS intervals ** */

/* L300: */
    }

/* ****** Work ended ** */

    *xlongc = som;

/* ****** It is forced IERCOD  =  0 ** */

    *iercod = 0;

/* ****** Final processing ** */

L9900:

/* ****** Save UROOT, WGAUS, NGAUS and KGAR ** */

/*      CALL MXVSAVE(IERXV,'INTEGER',I1,KGAR,'INTEGER',I1,NGAUS */
/*     1    ,'DOUBLE PRECISION',I20,UROOT,'DOUBLE PRECISION',I20,WGAUS) */
/*      IF (IERXV.GT.0) KGAR=0 */

/* ****** End of sub-program ** */

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
  integer c__2 = 2;
  integer c__1 = 1;

  
   /* Initialized data */

    doublereal moin11[2] = { -1.,1. };

    /* System generated locals */
    integer valbas_dim1, i__1;

    /* Local variables */
    doublereal vjacc[80], herm[24];
    NCollection_Array1<doublereal> vjac (vjacc[0], 1, 80);
    integer iord[2];
    doublereal wval[4];
    integer nwcof, iunit;
    doublereal wpoly[7];
    integer ii, jj, iorjac;
    doublereal hermit[36]	/* was [6][3][2] */;
    integer kk1, kk2, kk3;
    integer khe, ier;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       Position on the polynoms of base hermit-Jacobi */
/*       and their succesive derivatives */

/*     KEYWORDS : */
/*     ----------- */
/*      PUBLIC, POSITION, HERMIT, JACOBI */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       TPARAM : Parameter for which the position is found. */
/*       IORDRE : Orderof hermit-Jacobi (-1,0,1, ou 2) */
/*       NCOEFF : Number of coefficients of polynoms (Nb of value to calculate) */
/*       NDERIV : Number of derivative to calculate (0<= N <=3) */
/*              0 -> Position simple on base functions */
/*              N -> Position on base functions and derivative */
/*              of order 1 to N */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     VALBAS (NCOEFF, 0:NDERIV) : calculated value */
/*           i */
/*          d    vj(t)  = VALBAS(J, I) */
/*          -- i */
/*          dt */

/*    IERCOD : Error code */
/*      0 : Ok */
/*      1 : Incoherence of input arguments */

/*     COMMONS USED : */
/*     -------------- */


/*     REFERENCES CALLED : */
/*     ------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
/*                      INITIALIZATIONS */
/* ***********************************************************************
 */

    *iercod = 0;

/* ***********************************************************************
 */
/*                     PROCESSING */
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

/*  (1) Generic Calculations .... */

/*  (1.a) Calculation of hermit polynoms */

    if (*iordre >= 0) {
	mmherm1_(moin11, &c__2, iord, hermit, &ier);
	if (ier > 0) {
	    goto L9102;
	}
    }

/*  (1.b) Evaluation of hermit polynoms */

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

/*  (1.c) Evaluation of Jacobi polynoms */

    ii = *ncoeff - iorjac;

    mmpojac_(tparam, &iorjac, &ii, nderiv, vjac, &ier);
    if (ier > 0) {
	goto L9102;
    }

/*  (1.d) Evaluation of W(t) */

/* Computing MAX */
    i__1 = iorjac + 1;
    nwcof = advapp_max(i__1,1);
    AdvApp2Var_SysBase::mvriraz_(&nwcof, 
	     wpoly);
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

/*  (2) Evaluation of order 0 */

    jj = 1;
    i__1 = iorjac;
    for (ii = 1; ii <= i__1; ++ii) {
	valbas[ii] = herm[jj - 1];
	jj += iunit;
    }

    i__1 = kk1;
    for (ii = 1; ii <= i__1; ++ii) {
	valbas[ii + iorjac] = wval[0] * vjac(ii);
    }

/*  (3) Evaluation of order 1 */

    if (*nderiv >= 1) {
	jj = 2;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + valbas_dim1] = herm[jj - 1];
	    jj += iunit;
	}


	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + valbas_dim1] = wval[0] * vjac(ii + kk1)
		    + wval[1] * vjac(ii);
	}
    }

/*  (4)  Evaluation of order 2 */

    if (*nderiv >= 2) {
	jj = 3;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + (valbas_dim1 << 1)] = herm[jj - 1];
	    jj += iunit;
	}

	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + (valbas_dim1 << 1)] = wval[0] * vjac(ii + 
		    kk2) + wval[1] * 2 * vjac(ii + kk1) + wval[2] * 
		    vjac(ii);
	}
    }

/*  (5) Evaluation of order 3 */

    if (*nderiv >= 3) {
	jj = 4;
	i__1 = iorjac;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + valbas_dim1 * 3] = herm[jj - 1];
	    jj += iunit;
	}

	i__1 = kk1;
	for (ii = 1; ii <= i__1; ++ii) {
	    valbas[ii + iorjac + valbas_dim1 * 3] = wval[0] * vjac(ii + kk3)
		  + wval[1] * 3 * vjac(ii + kk2) + wval[2] * 3 * 
		    vjac(ii + kk1) + wval[3] * vjac(ii);
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;

L9102:
    *iercod = 2;

/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  integer ncof2;
  integer isize, nd, kcf, ncf;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        CALCULATE THE COORDINATES OF A POINT OF A CURVE OF GIVEN PARAMETER */
/*        TPARAM ( IN 2D, 3D OR MORE) */

/*     KEYWORDS : */
/*     ----------- */
/*       TOUS , MATH_ACCES :: COURBE&,PARAMETRE& , POSITIONNEMENT , &POINT
 */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*        NDIMAX : format / dimension of the curve */
/*        NCOEFF : Nb of coefficients of the curve */
/*        COURBE : Matrix of coefficients of the curve */
/*        NDIM   : Dimension useful of the workspace  */
/*        TPARAM : Value of the parameter where the point is calculated */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        PNTCRB : Coordinates of the calculated point */

/*     COMMONS USED   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */
/*           MIRAZ                MVPSCR2              MVPSCR3 */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
    AdvApp2Var_SysBase::miraz_(&isize, 
	   &pntcrb[1]);

    if (*ncoeff <= 0) {
	goto L9999;
    }

/*   optimal processing 3d */

    if (*ndim == 3 && *ndimax == 3) {
	mvpscr3_(ncoeff, &courbe[courbe_offset], tparam, &pntcrb[1]);

/*   optimal processing 2d */

    } else if (*ndim == 2 && *ndimax == 2) {
	mvpscr2_(ncoeff, &courbe[courbe_offset], tparam, &pntcrb[1]);

/*   Any dimension - scheme of HORNER */

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
  integer i__, nd;
  doublereal fu;
  
 
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Position of a point on curve (ncofmx,ndim). */

/*     KEYWORDS : */
/*     ----------- */
/*        TOUS , AB_SPECIFI :: COURBE&,POLYNOME&,POSITIONNEMENT,&POINT */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*        NCOFMX: Format / degree of the CURVE. */
/*        NDIM  : Dimension of the space. */
/*        NDEG  : Degree of the polynom. */
/*        COURBE: Coefficients of the curve. */
/*        TPARAM: Parameter on the curve */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*        TABVAL(NDIM): The resulting point (or table of values) */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
	     NCollection_Array1<doublereal>& valjac, 
	     integer *iercod)

{
  integer c__2 = 2;
  
    /* System generated locals */
    integer valjac_dim1, i__1, i__2;

    /* Local variables */
    doublereal cofa, cofb, denom, tnorm[100];
    integer ii, jj, kk1, kk2;
    doublereal aux1, aux2;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       Positioning on Jacobi polynoms and their derivatives */
/*       successive by a recurrent algorithm */

/*     KEYWORDS : */
/*     ----------- */
/*      RESERVE, POSITIONING, JACOBI */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       TPARAM : Parameter for which positioning is done. */
/*       IORDRE : Order of hermit-?? (-1,0,1, or 2) */
/*       NCOEFF : Number of coeeficients of polynoms (Nb of value to */
/*                calculate) */
/*       NDERIV : Number of derivative to calculate (0<= N <=3) */
/*              0 -> Position simple on jacobi functions */
/*              N -> Position on jacobi functions and their */
/*              derivatives of order 1 to N. */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     VALJAC (NCOEFF, 0:NDERIV) : the calculated values */
/*           i */
/*          d    vj(t)  = VALJAC(J, I) */
/*          -- i */
/*          dt */

/*    IERCOD : Error Code */
/*      0 : Ok */
/*      1 : Incoherence of input arguments */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */


/*     static varaibles */



    /* Parameter adjustments */
    valjac_dim1 = *ncoeff;

    /* Function Body */

/* ***********************************************************************
 */
/*                      INITIALISATIONS */
/* ***********************************************************************
 */

    *iercod = 0;

/* ***********************************************************************
 */
/*                     Processing */
/* ***********************************************************************
 */

    if (*nderiv > 3) {
	goto L9101;
    }
    if (*ncoeff > 100) {
	goto L9101;
    }

/*  --- Calculation of norms */

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

/*      END IF */

/*  --- Trivial Positions ----- */

    valjac(1) = 1.;
    aux1 = (doublereal) (*iordre + 1);
    valjac(2) = aux1 * *tparam;

    if (*nderiv >= 1) {
	valjac(valjac_dim1 + 1) = 0.;
	valjac(valjac_dim1 + 2) = aux1;

	if (*nderiv >= 2) {
	    valjac((valjac_dim1 << 1) + 1) = 0.;
	    valjac((valjac_dim1 << 1) + 2) = 0.;

	    if (*nderiv >= 3) {
		valjac(valjac_dim1 * 3 + 1) = 0.;
		valjac(valjac_dim1 * 3 + 2) = 0.;
	    }
	}
    }

/*  --- Positioning by recurrence */

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
	valjac(ii) = (cofa * *tparam * valjac(kk1) + cofb * valjac(kk2)) * 
		denom;
/*        --> P'i(t) */
	if (*nderiv >= 1) {
	    valjac(ii + valjac_dim1) = (cofa * *tparam * valjac(kk1 + 
		    valjac_dim1) + cofa * valjac(kk1) + cofb * valjac(kk2 + 
		    valjac_dim1)) * denom;
/*        --> P''i(t) */
	    if (*nderiv >= 2) {
		valjac(ii + (valjac_dim1 << 1)) = (cofa * *tparam * valjac(
			kk1 + (valjac_dim1 << 1)) + cofa * 2 * valjac(kk1 + 
			valjac_dim1) + cofb * valjac(kk2 + (valjac_dim1 << 1))
			) * denom;
	    }
/*        --> P'i(t) */
	    if (*nderiv >= 3) {
		valjac(ii + valjac_dim1 * 3) = (cofa * *tparam * valjac(kk1 + 
			valjac_dim1 * 3) + cofa * 3 * valjac(kk1 + (
			valjac_dim1 << 1)) + cofb * valjac(kk2 + valjac_dim1 *
			 3)) * denom;
	    }
	}
    }

/*    ---> Normalization */

    i__1 = *ncoeff;
    for (ii = 1; ii <= i__1; ++ii) {
	i__2 = *nderiv;
	for (jj = 0; jj <= i__2; ++jj) {
	    valjac(ii + jj * valjac_dim1) = tnorm[ii - 1] * valjac(ii + jj * 
		    valjac_dim1);
	}
    }

    goto L9999;

/* ***********************************************************************
 */
/*                   PROCESSING OF ERRORS */
/* ***********************************************************************
 */

L9101:
    *iercod = 1;
    goto L9999;


/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  logical ldbg;
  integer imin, jmin, i__, j, k;
  logical trouve;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       FILL THE TABLE OF POSITIONING POSUIV WHICH ALLOWS TO */
/*       PARSE BY COLUMN THE INFERIOR TRIANGULAR PART OF THE */
/*       MATRIX  IN FORM OF PROFILE */


/*     KEYWORDS : */
/*     ----------- */
/*      RESERVE, MATRIX, PROFILE */

/*     INPUT ARGUMENTS : */
/*     -------------------- */

/*       NISTOC: NUMBER OF COEFFICIENTS IN THE PROFILE */
/*       DIMMAT: NUMBER OF LINE OF THE SYMMETRIC SQUARE MATRIX */
/*       APOSIT: TABLE OF POSITIONING OF STORAGE TERMS */
/*               APOSIT(1,I) CONTAINS THE NUMBER OF TERMES-1 ON LINE */
/*               I IN THE PROFILE OF THE MATRIX */
/*               APOSIT(2,I) CONTAINS THE INDEX OF STORAGE OF DIAGONAL TERM */
/*               OF LINE I */


/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       POSUIV: POSUIV(K) (WHERE K IS THE INDEX OF STORAGE OF MAT(I,J)) */
/*               CONTAINS THE SMALLEST NUMBER IMIN>I OF THE  LINE THAT */
/*               POSSESSES A TERM MAT(IMIN,J) THAT IS IN THE PROFILE. */
/*               IF THERE IS NO TERM MAT(IMIN,J) IN THE PROFILE THEN POSUIV(K)=-1 */


/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */



/* ***********************************************************************
 */
/*                      INITIALIZATIONS */
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
/*                     PROCESSING */
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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */




/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  integer c__100 = 100;
 
   /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    logical ldbg;
    doublereal* mcho = 0;
    integer jmin, jmax, i__, j, k, l;
    intptr_t iofv1, iofv2, iofv3, iofv4;
    doublereal *v1 = 0, *v2 = 0, *v3 = 0, *v4 = 0;
    integer deblig, dimhch;
    doublereal* hchole = 0;
    intptr_t iofmch, iofmam, iofhch;
    doublereal* matsym = 0;
    integer ier;
    integer aux;



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       SOLUTION OF THE SYSTEM */
/*       H  t(G)   V     B */
/*                    = */
/*       G    0    L     C */

/*     KEYWORDS : */
/*     ----------- */
/*      RESERVE, SOLUTION, SYSTEM, LAGRANGIAN */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*      HDIMEN: NOMBER OF LINE (OR COLUMN) OF THE HESSIAN MATRIX */
/*      GDIMEN: NOMBER OF LINE OF THE MATRIX OF CONSTRAINTS */
/*      HNSTOC: NOMBErS OF TERMS IN THE PROFILE OF HESSIAN MATRIX 
*/
/*      GNSTOC: NOMBERS OF TERMS IN THE PROFILE OF THE MATRIX OF CONSTRAINTS */
/*      MNSTOC: NOMBERS OF TERMS IN THE PROFILE OF THE MATRIX M= G H t(G) */
/*              where H IS THE HESSIAN MATRIX AND G IS THE MATRIX OF CONSTRAINTS */
/*      MATSYH: TRIANGULAR INFERIOR PART OF THE HESSIAN MATRIX */
/*              IN FORM OF PROFILE */
/*      MATSYG: MATRIX OF CONSTRAINTS IN FORM OF PROFILE */
/*      VECSYH: VECTOR OF THE SECOND MEMBER ASSOCIATED TO MATSYH */
/*      VECSYG: VECTOR OF THE SECOND MEMBER ASSOCIATED TO MATSYG */
/*      HPOSIT: TABLE OF POSITIONING OF THE HESSIAN MATRIX */
/*              HPOSIT(1,I) CONTAINS THE NUMBER OF TERMS -1 */
/*              WHICH ARE IN THE PROFILE AT LINE I */
/*              HPOSIT(2,I) CONTAINS THE INDEX OF STORAGE OF TERM */
/*              DIAGONAL OF THE MATRIX AT LINE I */
/*      HPOSUI: TABLE ALLOWING TO PARSE THE HESSIAN MATRIX BY COLUMN */
/*              IN FORM OF PROFILE */
/*             HPOSUI(K) CONTAINS THE NUMBER OF LINE IMIN FOLLOWING THE CURRENT LINE*/
/*              I WHERE H(I,J)=MATSYH(K) AS IT EXISTS IN THE */
/*              SAME COLUMN J A TERM IN THE PROFILE OF LINE IMIN */
/*              IF SUCH TERM DOES NOT EXIST IMIN=-1 */
/*      GPOSIT: TABLE OF POSITIONING OF THE MATRIX OF CONSTRAINTS */
/*              GPOSIT(1,I) CONTAINS THE NUMBER OF TERMS OF LINE I */
/*                          WHICH ARE IN THE PROFILE */
/*              GPOSIT(2,I) CONTAINS THE INDEX OF STORAGE OF THE LAST TERM */
/*                          OF LINE I WHICH IS IN THE PROFILE */
/*              GPOSIT(3,I) CONTAINS THE NUMBER OF COLUMN CORRESPONDING */
/*                          TO THE FIRST TERM OF LINE I WHICH IS IN THE PROFILE */
/*      MMPOSUI, MPOSIT: SAME STRUCTURE AS HPOSUI, BUT FOR MATRIX */
/*              M=G H t(G) */


/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       VECSOL: VECTOR SOLUTION V OF THE SYSTEM */
/*       IERCOD: ERROR CODE */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
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
/*                     PROCESSING */
/* ***********************************************************************
 */

/*    Dynamic allocation */
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;
    anAdvApp2Var_SysBase.macrar8_(hdimen, &c__100, v1, &iofv1, &ier);
    if (ier > 0) {
	goto L9102;
    }
    dimhch = hposit[(*hdimen << 1) + 2];
    anAdvApp2Var_SysBase.macrar8_(&dimhch, &c__100, hchole, &iofhch, &ier);
    if (ier > 0) {
	goto L9102;
    }

/*   solution of system 1     H V1 = b */
/*   where H=MATSYH  and b=VECSYH */

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

/*    Case when there are constraints */

    if (*gdimen > 0) {

/*    Calculate the vector of the second member V2=G H(-1) b -c = G v1-c */
/*    of system of unknown Lagrangian vector MULTIP */
/*    where G=MATSYG */
/*          c=VECSYG */

	anAdvApp2Var_SysBase.macrar8_(gdimen, &c__100, v2, &iofv2, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	anAdvApp2Var_SysBase.macrar8_(hdimen, &c__100, v3, &iofv3, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	anAdvApp2Var_SysBase.macrar8_(gdimen, &c__100, v4, &iofv4, &ier);
	if (ier > 0) {
	    goto L9102;
	}
	anAdvApp2Var_SysBase.macrar8_(mnstoc, &c__100, matsym, &iofmam, &ier);
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

/*     Calculate the matrix M= G H(-1) t(G) */
/*     RESOL DU SYST 2 : H qi = gi */
/*            where is a vector column of t(G) */
/*                qi=v3 */
/*            then calculate G qi */
/*            then construct M in form of profile */



	i__1 = *gdimen;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    AdvApp2Var_SysBase::mvriraz_(hdimen, &v1[iofv1]);
	    AdvApp2Var_SysBase::mvriraz_(hdimen, &v3[iofv3]);
	    AdvApp2Var_SysBase::mvriraz_(gdimen, &v4[iofv4]);
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


/*    SOLVE SYST 3  M L = V2 */
/*     WITH L=V4 */


	AdvApp2Var_SysBase::mvriraz_(gdimen, &v4[iofv4]);
	anAdvApp2Var_SysBase.macrar8_(mnstoc, &c__100, mcho, &iofmch, &ier);
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


/*    CALCULATE THE VECTOR OF THE SECOND MEMBER OF THE SYSTEM  Hx = b - t(G) L 
*/
/*                                                      = V1 */

	AdvApp2Var_SysBase::mvriraz_(hdimen, &v1[iofv1]);
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
/*                   PROCESSING OF ERRORS */
/* ***********************************************************************
 */


L9101:
    *iercod = 1;
    goto L9999;

L9102:
    AdvApp2Var_SysBase::mswrdbg_("MMRESOL : PROBLEM WITH DIMMAT", 30L);
    *iercod = 2;

/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
/* ***********************************************************************
 */

L9999:

/* ___ DESALLOCATION, ... */
    anAdvApp2Var_SysBase.macrdr8_(hdimen, &c__100, v1, &iofv1, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(&dimhch, &c__100, hchole, &iofhch, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(gdimen, &c__100, v2, &iofv2, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(hdimen, &c__100, v3, &iofv3, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(gdimen, &c__100, v4, &iofv4, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(mnstoc, &c__100, matsym, &iofmam, &ier);
    if (*iercod == 0 && ier > 0) {
	*iercod = 3;
    }
    anAdvApp2Var_SysBase.macrdr8_(mnstoc, &c__100, mcho, &iofmch, &ier);
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
  logical ldbg;
  integer i__, j;
  doublereal somme;
  integer pointe, ptcour;

/* ***********************************************************************
 */

/*     FuNCTION : */
/*     ----------                     T */
/*       Solves linear system SS x = b where S is a  */
/*       triangular lower matrix given in form of profile */

/*     KEYWORDS : */
/*     ----------- */
/*     RESERVE, MATRICE_PROFILE, RESOLUTION, CHOLESKI */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*     MXCOEF  : Maximum number of non-null coefficient in the matrix */
/*     DIMENS  : Dimension of the matrix */
/*     SMATRI(MXCOEF) : Values of coefficients of the matrix */
/*     SPOSIT(2,DIMENS): */
/*       SPOSIT(1,*) : Distance diagonal-extremity of the line */
/*       SPOSIT(2,*) : Position of diagonal terms in AMATRI */
/*     POSUIV(MXCOEF): first line inferior not out of profile */
/*     MSCNMBR(DIMENS): Vector second member of the equation */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     SOLUTI(NDIMEN) : Result vector */
/*     IERCOD   : Error code 0  : ok */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       T */
/*     SS  is the decomposition of choleski of a symmetric matrix */
/*     defined postive, that can result from routine MMCHOLE. */

/*     For a full matrix it is possible to use MRSLMSC */

/*     LEVEL OF DEBUG = 4 */
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
/*                     PROCESSING */
/* ***********************************************************************
 */

/* ----- Solution of Sw = b */

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
/* ----- Solution of S u = w */

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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                   RETURN PROGRAM CALLING */
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
    integer kpiv;
    doublereal pivot;
    integer ii, jj, kk;
    doublereal akj;
    

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  Solution of a linear system A.x = B of N equations to N */
/*  unknown by Gauss method (partial pivot) or : */
/*          A is matrix NORDRE * NORDRE, */
/*          B is matrix NORDRE (lines) * NDIMEN (columns), */
/*          x is matrix NORDRE (lines) * NDIMEN (columns). */
/*  In this program, A and B are stored in matrix ABMATR  */
/*  the lines and columns which of were inverted. ABMATR(k,j) is */
/*  term A(j,k) if k <= NORDRE, B(j,k-NORDRE) otherwise (see example). */

/*     KEYWORDS : */
/*     ----------- */
/* TOUS, MATH_ACCES::EQUATION&, MATRICE&, RESOLUTION, GAUSS, &SOLUTION */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NORMAX : Max size of the first index of XMATRI. This argument */
/*            serves only for the declaration of dimension of XMATRI and should be */
/*            above or equal to NORDRE. */
/*   NORDRE : Order of the matrix i.e. number of equations and  */
/*            unknown quantities of the linear system to be solved. */
/*   NDIMEN : Number of the second member. */
/*   EPSPIV : Minimal value of a pivot. If during the calculation  */
/*            the absolute value of the pivot is below EPSPIV, the */
/*            system of equations is declared singular. EPSPIV should */
/*            be a "small" real. */

/*   ABMATR(NORDRE+NDIMEN,NORDRE) : Auxiliary matrix containing  */
/*                                  matrix A and matrix B. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   XMATRI : Matrix containing  NORDRE*NDIMEN solutions. */
/*   IERCOD=0 shows that all solutions are calculated. */
/*   IERCOD=1 shows that the matrix is of lower rank than NORDRE */
/*            (the system is singular). */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION : the indices of line and column are inverted */
/*                 compared to usual indices. */
/*                 System : */
/*                        a1*x + b1*y = c1 */
/*                        a2*x + b2*y = c2 */
/*                 should be represented by matrix ABMATR : */

/*                 ABMATR(1,1) = a1  ABMATR(1,2) = a2 */
/*                 ABMATR(2,1) = b1  ABMATR(2,2) = b2 */
/*                 ABMATR(3,1) = c1  ABMATR(3,2) = c2 */

/*     To solve this system, it is necessary to set : */

/*                 NORDRE = 2 (there are 2 equations with 2 unknown values), */
/*                 NDIMEN = 1 (there is only one second member), */
/*                 any NORMAX can be taken >= NORDRE. */

/*     To use this routine, it is recommended to use one of */
/*     interfaces : MMRSLWI or MMMRSLWD. */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

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
/*                  Triangulation of matrix ABMATR. */
/* ********************************************************************* 
*/

    i__1 = *nordre;
    for (kk = 1; kk <= i__1; ++kk) {

/* ---------- Find max pivot in column KK. ------------
--- */

	pivot = *epspiv;
	kpiv = 0;
	i__2 = *nordre;
	for (jj = kk; jj <= i__2; ++jj) {
	    akj = (d__1 = abmatr[kk + jj * abmatr_dim1], advapp_abs(d__1));
	    if (akj > pivot) {
		pivot = akj;
		kpiv = jj;
	    }
/* L100: */
	}
	if (kpiv == 0) {
	    goto L9900;
	}

/* --------- Swapping of line KPIV with line KK. ------
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

/* ---------- Removal and triangularization. -----------
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
/*          Solution of the system of triangular equations. */
/*   Matrix ABMATR(NORDRE+JJ,II), contains second members  */
/*             of the system for 1<=j<=NDIMEN and 1<=i<=NORDRE. */
/* ********************************************************************* 
*/


/* ---------------- Calculation of solutions by ascending. ----------------- 
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

/* ------If the absolute value of a pivot is smaller than -------- */
/* ---------- EPSPIV: return the code of error. ------------ 
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
  integer i__, j;
  integer ibb;

/*      IMPLICIT DOUBLE PRECISION (A-H,O-Z) */
/*      IMPLICIT INTEGER (I-N) */


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Solution of a linear system by Gauss method where */
/*        the second member is a table of vectors. Method of partial pivot. */

/*     KEYWORDS : */
/*     ----------- */
/*        ALL, MATH_ACCES :: */
/*        SYSTEME&,EQUATION&, RESOLUTION,GAUSS ,&VECTEUR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NORMAX : Max. Dimension of AMAT. */
/*        NORDRE :  Order of the matrix. */
/*        NDIM : Number of columns of BMAT and XMAT. */
/*        AMAT(NORMAX,NORDRE) : The processed matrix. */
/*        BMAT(NORMAX,NDIM)   : The matrix of second member. */
/*        XMAT(NORMAX,NDIM)   : The matrix of solutions. */
/*        EPSPIV : Min value of a pivot. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        AAUX(NORDRE+NDIM,NORDRE) : Auxiliary matrix. */
/*        XMAT(NORMAX,NDIM) : Matrix of solutions. */
/*        IERCOD=0 shows that solutions in XMAT are valid. */
/*        IERCOD=1 shows that matrix AMAT is of lower rank than NORDRE. */

/*     COMMONS USED   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES CALLED : */
/*     ---------------------- */
/*     Type  Name */
/*           MAERMSG              MGENMSG              MGSOMSG */
/*           MMRSLW          I*4  MNFNDEB */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*    ATTENTION : lines and columns are located in usual order : */
/*               1st index  = index line */
/*               2nd index = index column */
/*    Example, the system : */
/*                 a1*x + b1*y = c1 */
/*                 a2*x + b2*y = c2 */
/*    is represented by matrix AMAT : */

/*                 AMAT(1,1) = a1  AMAT(2,1) = a2 */
/*                 AMAT(1,2) = b1  AMAT(2,2) = b2 */

/*     The first index is the index of line, the second index */
/*     is the index of columns (Compare with MMRSLWI which is faster). */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

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

/*   Initialization of the auxiliary matrix. */

    i__1 = *nordre;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *nordre;
	for (j = 1; j <= i__2; ++j) {
	    aaux[j + i__ * aaux_dim1] = amat[i__ + j * amat_dim1];
/* L200: */
	}
/* L100: */
    }

/*    Second member. */

    i__1 = *nordre;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *ndim;
	for (j = 1; j <= i__2; ++j) {
	    aaux[j + *nordre + i__ * aaux_dim1] = bmat[i__ + j * bmat_dim1];
/* L400: */
	}
/* L300: */
    }

/*    Solution of the system of equations. */

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
  integer ideb, nmod2, nsur2, ilong, ibb;


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Extracts from Common LDGRTL the STRICTLY positive roots of the */
/*     Legendre polynom of degree NDGLGD, for 2 <= NDGLGD <= 61. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::COMMON&, EXTRACTION, &RACINE, &LEGENDRE. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDGLGD : Mathematic degree of Legendre polynom. */
/*                 This degree should be above or equal to 2 and */
/*                 below or equal to 61. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        RTLEGD : The table of strictly positive roots of */
/*                 Legendre polynom of degree NDGLGD. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     ATTENTION: the condition on NDEGRE ( 2 <= NDEGRE <= 61) is not */
/*     tested. The caller should make the test. */

/* > */
/* ********************************************************************** 
*/
/*   Nome of the routine */


/*   Common MLGDRTL: */
/*   This common includes POSITIVE roots of Legendre polynoms */
/*   AND the weight of Gauss quadrature formulas on all */
/*   POSITIVE roots of Legendre polynoms. */


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*   The common of Legendre roots. */

/*     KEYWORDS : */
/*     ----------- */
/*        BASE LEGENDRE */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */




/*   ROOTAB : Table of all rotts of Legendre polynoms */
/*   between [0,1]. They are ranked for degrees increasing from 2 to 61. */
/*   HILTAB : Table of Legendre interpolators concerning ROOTAB. */
/*   The address is the same. */
/*   HI0TAB : Table of Legendre interpolators for root x=0 */
/*   the polynoms of UNEVEN degree. */
/*   RTLTB0 : Table of Li(uk) where uk are roots of a */
/*   Legendre polynom of EVEN degree. */
/*   RTLTB1 : Table of Li(uk) where uk are roots of a */
/*   Legendre polynom of UNEVEN degree. */


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
    AdvApp2Var_SysBase::mcrfill_(&ilong, 
	     &mlgdrtl_.rootab[ideb + nmod2 * 465 - 1], 
	     &rtlegd[1]);

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
  integer ideb, ifin, imil, ibb;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     -------- */

/*     Find the interval corresponding to a valueb given in  */
/*     increasing order of real numbers with double precision. */

/*     KEYWORDS : */
/*     --------- */
/*     TOUS,MATH_ACCES::TABLEAU&,POINT&,CORRESPONDANCE,&RANG */

/*     INPUT ARGUMENTS : */
/*     ------------------ */

/*     TPARAM  : Value to be tested. */
/*     NBRVAL  : Size of TABLEV */
/*     TABLEV  : Table of reals. */
/*     EPSIL   : Epsilon of precision */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */

/*     NUMINT  : Number of the interval (between 1 and NBRVAL-1). */
/*     ITYPEN  : = 0 TPARAM is inside the interval NUMINT */
/*               = 1 : TPARAM corresponds to the lower limit of */
/*               the provided interval. */
/*               = 2 : TPARAM corresponds to the upper limit of */
/*               the provided interval. */

/*     IERCOD : Error code. */
/*                     = 0 : OK */
/*                     = 1 : TABLEV does not contain enough elements. */
/*                     = 2 : TPARAM out of limits of TABLEV. */

/*     COMMONS USED : */
/*     ---------------- */

/*     REFERENCES CALLED : */
/*     ------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     --------------------------------- */
/*     There are NBRVAL values in TABLEV which stands for NBRVAL-1 intervals. */
/*     One searches the interval containing TPARAM by */
/*     dichotomy. Complexity of the algorithm : Log(n)/Log(2).(RBD). */
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

/* TABLEV should contain at least two values */

    if (*nbrval < 2) {
	*iercod = 1;
	goto L9999;
    }

/* TPARAM should be between extreme limits of TABLEV. */

    if (*tparam < tablev[1] || *tparam > tablev[*nbrval]) {
	*iercod = 2;
	goto L9999;
    }

/* ----------------------- SEARCH OF THE INTERVAL -------------------- 
*/

L1000:

/* Test end of loop (found). */

    if (ideb + 1 == ifin) {
	*numint = ideb;
	goto L2000;
    }

/* Find by dichotomy on increasing values of TABLEV. */

    imil = (ideb + ifin) / 2;
    if (*tparam >= tablev[ideb] && *tparam <= tablev[imil]) {
	ifin = imil;
    } else {
	ideb = imil;
    }

    goto L1000;

/* -------------- TEST IF TPARAM IS NOT A VALUE --------- */
/* ------------------------OF TABLEV UP TO EPSIL ---------------------- 
*/

L2000:
    if ((d__1 = *tparam - tablev[ideb], advapp_abs(d__1)) < *epsil) {
	*itypen = 1;
	goto L9999;
    }
    if ((d__1 = *tparam - tablev[ifin], advapp_abs(d__1)) < *epsil) {
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
  logical ldbg;
  integer imin, imax, i__, j, k;
  doublereal somme;
  integer aux;
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*                          t */
/*      CREATES PRODUCT   G V */
/*      WHERE THE MATRIX IS IN FORM OF PROFILE */

/*     KEYWORDS : */
/*     ----------- */
/*      RESERVE, PRODUCT, MATRIX, PROFILE, VECTOR */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*       NLIGNE : NUMBER OF LINE OF THE MATRIX */
/*       NCOLON : NOMBER OF COLUMN OF THE MATRIX */
/*       GPOSIT: TABLE OF POSITIONING OF TERMS OF STORAGE */
/*               GPOSIT(1,I) CONTAINS THE NUMBER of TERMS-1 ON LINE */
/*               I IN THE PROFILE OF THE MATRIX */
/*              GPOSIT(2,I) CONTAINS THE INDEX OF STORAGE OF THE DIAGONAL TERM*/
/*               OF LINE I */
/*               GPOSIT(3,I) CONTAINS THE INDEX COLUMN OF THE FIRST TERM OF */
/*                           PROFILE OF LINE I */
/*       GNSTOC : NOMBER OF TERM IN THE PROFILE OF GMATRI */
/*       GMATRI : MATRIX OF CONSTRAINTS IN FORM OF PROFILE */
/*       VECIN :  INPUT VECTOR */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*       VECOUT : VECTOR PRODUCT */
/*       IERCOD : ERROR CODE */


/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
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
/*                     PROCESSING */
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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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
  integer ncut, i__;
  doublereal bidon, error;
  integer nd;
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Lowers the degree of a curve defined on (-1,1) in the direction of */
/*        Legendre with a given precision. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE, POLYGON, TRUNCATION, CURVE, SMOOTHING. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max Nb of coeff. of the curve (dimensioning). */
/*        NDIMEN : Dimension of the space. */
/*        NCOEFF : Degree +1 of the polynom. */
/*        EPSI3D : Precision required for the approximation. */
/*        CRVLGD : The curve the degree which of it is required to lower. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        EPSTRC : Precision of the approximation. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */


/* ------- Minimum degree that can be attained : Stop at 1 (RBD) --------- 
*/

    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */
    *ncfnew = 1;
/* ------------------- Init for error calculation ----------------------- 
*/
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Cutting of coefficients. */

    ncut = 2;
/* ------ Loop on the series of Legendre :NCOEFF --> 2 (RBD) ----------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Factor of renormalization. */
	bidon = ((i__ - 1) * 2. + 1.) / 2.;
	bidon = sqrt(bidon);
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   Cutting is stopped if the norm becomes too great. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L9999;
	}

/* ---  Max error cumulee when the I-th coeff is removed. */

	*epstrc = error;

/* L300: */
    }

/* --------------------------------- End -------------------------------- 
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
    integer ncut, i__;
    doublereal bidon, error;
    integer ia, nd;
    doublereal bid, eps1;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Lower the degree of a curve defined on (-1,1) in the direction of */
/*        Legendre with a given precision. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE, POLYGON, TRUNCATION, CURVE, SMOOTHING. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max nb of coeff. of the curve (dimensioning). */
/*        NDIMEN : Dimension of the space. */
/*        NCOEFF : Degree +1 of the polynom. */
/*        EPSI3D : Precision required for the approximation. */
/*        CRVLGD : The curve the degree which of will be lowered. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary table (error max on each dimension). 
*/
/*        EPSTRC : Precision of the approximation. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Minimum degree that can be reached : Stop at IA (RBD). ------------- 
*/
    ia = 2;
    *ncfnew = ia;
/* Init for calculation of error. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Cutting of coefficients. */

    ncut = ia + 1;
/* ------ Loop on the series of Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Factor of renormalization. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   One stops to cut if the norm becomes too great. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* --- Max error cumulated when the I-th coeff is removed. */

	*epstrc = error;

/* L300: */
    }

/* ------- Cutting of zero coeffs of interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- If all coeffs can be removed, this is a point. */
	*ncfnew = 1;
    }

/* --------------------------------- End -------------------------------- 
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
    integer ncut, i__;
    doublereal bidon, error;
    integer ia, nd;
    doublereal bid, eps1;



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Lowers the degree of a curve defined on (-1,1) in the direction of */
/*        Legendre with a given precision. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE, POLYGON, TRONCATION, CURVE, SMOOTHING. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max nb of coeff. of the curve (dimensioning). */
/*        NDIMEN : Dimension of the space. */
/*        NCOEFF : Degree +1 of the polynom. */
/*        EPSI3D : Precision required for the approximation. */
/*        CRVLGD : The curve which wishes to lower the degree. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary table (max error on each dimension). 
*/
/*        EPSTRC : Precision of the approximation. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Minimum degree that can be reached : Stop at IA (RBD). ------------- 
*/
    ia = 4;
    *ncfnew = ia;
/* Init for error calculation. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Cutting of coefficients. */

    ncut = ia + 1;
/* ------ Loop on the series of Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Factor of renormalization. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   Stop cutting if the norm becomes too great. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* -- Error max cumulated when the I-eme coeff is removed. */

	*epstrc = error;

/* L300: */
    }

/* ------- Cutting of zero coeffs of the pole of interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- If all coeffs can be removed, this is a point. */
	*ncfnew = 1;
    }

/* --------------------------------- End -------------------------------- 
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
    integer ncut, i__;
    doublereal bidon, error;
    integer ia, nd;
    doublereal bid, eps1;



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Lowers the degree of a curve defined on (-1,1) in the direction of */
/*        Legendre to a given precision. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,POLYGON,TRUNCATION,CURVE,SMOOTHING. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max nb of coeff. of the curve (dimensioning). */
/*        NDIMEN : Dimension of the space. */
/*        NCOEFF : Degree +1 of the polynom. */
/*        EPSI3D : Precision required for the approximation. */
/*        CRVLGD : The curve the degree which of will be lowered. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        YCVMAX : Auxiliary table (max error on each dimension). */
/*        EPSTRC : Precision of the approximation. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --ycvmax;
    crvlgd_dim1 = *ncofmx;
    crvlgd_offset = crvlgd_dim1 + 1;
    crvlgd -= crvlgd_offset;

    /* Function Body */



/*   Minimum degree that can be reached : Stop at IA (RBD). ------------- 
*/
    ia = 6;
    *ncfnew = ia;
/* Init for error calculation. */
    i__1 = *ndimen;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ycvmax[i__] = 0.;
/* L100: */
    }
    *epstrc = 0.;
    error = 0.;

/*   Cutting of coefficients. */

    ncut = ia + 1;
/* ------ Loop on the series of Jacobi :NCOEFF --> IA+1 (RBD) ---------- 
*/
    i__1 = ncut;
    for (i__ = *ncoeff; i__ >= i__1; --i__) {
/*   Factor of renormalization. */
	bidon = xmaxj[i__ - ncut];
	i__2 = *ndimen;
	for (nd = 1; nd <= i__2; ++nd) {
	    ycvmax[nd] += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1)) *
		     bidon;
/* L310: */
	}
/*   Stop cutting if the norm becomes too great. */
	error = AdvApp2Var_MathBase::mzsnorm_(ndimen, &ycvmax[1]);
	if (error > *epsi3d) {
	    *ncfnew = i__;
	    goto L400;
	}

/* --- Max error cumulated when the I-th coeff is removed. */

	*epstrc = error;

/* L300: */
    }

/* ------- Cutting of zero coeff. of the pole of interpolation (RBD) ------- 
*/

L400:
    if (*ncfnew == ia) {
	AdvApp2Var_MathBase::mmeps1_(&eps1);
	for (i__ = ia; i__ >= 2; --i__) {
	    bid = 0.;
	    i__1 = *ndimen;
	    for (nd = 1; nd <= i__1; ++nd) {
		bid += (d__1 = crvlgd[i__ + nd * crvlgd_dim1], advapp_abs(d__1));
/* L600: */
	    }
	    if (bid > eps1) {
		*ncfnew = i__;
		goto L9999;
	    }
/* L500: */
	}
/* --- If all coeffs can be removed, this is a point. */
	*ncfnew = 1;
    }

/* --------------------------------- End -------------------------------- 
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
    integer ia;
   

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        Lower the degree of a curve defined on (-1,1) in the direction of */
/*        Legendre with a given precision. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE, POLYGON, TRUNCATION, CURVE, SMOOTHING. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Max Nb coeff. of the curve (dimensioning). */
/*        NDIMEN : Dimension of the space. */
/*        NCOEFF : Degree +1 of the polynom. */
/*        EPSI3D : Precision required for the approximation. */
/*        IORDRE : Order of continuity at the extremities. */
/*        CRVLGD : The curve the degree which of should be lowered. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        ERRMAX : Precision of the approximation. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
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

/* ------------------------ End ----------------------------------------- 
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
 
  doublereal c_b2 = 10.;
  
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Local variables */
    integer nchif, iunit = 1, izero;
    doublereal vnorm;
    integer ii;
    doublereal bid;
    doublereal eps0;




/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        CALCULATE THE NORMAL VECTOR BASING ON ANY VECTOR */
/*        WITH PRECISION GIVEN BY THE USER. */

/*     KEYWORDS : */
/*     ----------- */
/*        ALL, MATH_ACCES :: */
/*        VECTEUR&, NORMALISATION, &VECTEUR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMEN   : DIMENSION OF THE SPACE */
/*        VECTOR   : VECTOR TO BE NORMED */
/*        EPSILN   : EPSILON BELOW WHICH IT IS CONSIDERED THAT THE */
/*                 NORM OF THE VECTOR IS NULL. IF EPSILN<=0, A DEFAULT VALUE */
/*                 IS IMPOSED (10.D-17 ON VAX). */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        VECNRM : NORMED VECTOR */
/*        IERCOD  101 : THE VECTOR IS NULL UP TO EPSILN. */
/*                  0 : OK. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     VECTOR and VECNRM can be identic. */

/*     The norm of vector is calculated and each component is divided by */
/*     this norm. After this it is checked if all componentes of the */
/*     vector except for one cost 0 with machine precision. In */
/*     this case the quasi-null components are set to 0.D0. */
/* > */
/* ***********************************************************************
 */


    /* Parameter adjustments */
    --vecnrm;
    --vector;

    /* Function Body */
    *iercod = 0;

/* -------- Precision by default : zero machine 10.D-17 on Vax ------ 
*/

    AdvApp2Var_SysBase::maovsr8_(&nchif);
    if (*epsiln <= 0.) {
	i__1 = -nchif;
	eps0 = AdvApp2Var_MathBase::pow__di(&c_b2, &i__1);
    } else {
	eps0 = *epsiln;
    }

/* ------------------------- Calculation of the norm -------------------- 
*/

    vnorm = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vector[1]);
    if (vnorm <= eps0) {
	AdvApp2Var_SysBase::mvriraz_(ndimen, &vecnrm[1]);
	*iercod = 101;
	goto L9999;
    }

/* ---------------------- Calculation of the vector norm  --------------- 
*/

    izero = 0;
    i__1 = (-nchif - 1) / 2;
    eps0 = AdvApp2Var_MathBase::pow__di(&c_b2, &i__1);
    i__1 = *ndimen;
    for (ii = 1; ii <= i__1; ++ii) {
	vecnrm[ii] = vector[ii] / vnorm;
	if ((d__1 = vecnrm[ii], advapp_abs(d__1)) <= eps0) {
	    ++izero;
	} else {
	    iunit = ii;
	}
/* L20: */
    }

/* ------ Case when all coordinates except for one are almost null ---- 
*/
/* ------------- then one of coordinates costs 1.D0 or -1.D0 -------- 
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
  
  integer ibb;
  


/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*        Extraction of EPS1 from COMMON MPRCSN. */

/*     KEYWORDS : */
/*     ----------- */
/*        MPRCSN,PRECISON,EPS3. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*       Humm. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        EPS3 :  space zero of the denominator (10**-9) */
/*        EPS3 should value 10**-15 */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */



/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*          GIVES TOLERANCES OF NULLITY IN STRIM */
/*          AND LIMITS OF ITERATIVE PROCESSES */

/*          GENERAL CONTEXT, MODIFIABLE BY THE UTILISER */

/*     KEYWORDS : */
/*     ----------- */
/*          PARAMETER , TOLERANCE */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  PROFILE , **VIA MPRFTX** AT INPUT IN STRIM*/
/*       LOADING OF DEFAULT VALUES OF THE PROFILE IN MPRFTX AT INPUT*/
/*       IN STRIM. THEY ARE PRESERVED IN THE LOCAL VARIABLES OF MPRFTX */

/*        RESET DEFAULT VALUES                   : MDFINT */
/*        MODIFICATION INTERACTIVE BY THE USER   : MDBINT */

/*        ACCESS FUNCTION  :  MMEPS1  ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */

/* > */
/* ***********************************************************************
 */

/*     NITERM : MAX NB OF ITERATIONS */
/*     NITERR : NB OF RAPID ITERATIONS */
/*     EPS1   : TOLERANCE OF 3D NULL DISTANCE */
/*     EPS2   : TOLERANCE OF ZERO PARAMETRIC DISTANCE */
/*     EPS3   : TOLERANCE TO AVOID DIVISION BY 0.. */
/*     EPS4   : TOLERANCE ANGULAR */



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
  logical ldbg;
  integer d__;
  doublereal vaux1[3], vaux2[3];
  logical colin;
  doublereal valaux;
  integer aux;
 
/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*       CALCULATE A VECTOR NON-COLINEAR TO A GIVEN NON-NULL VECTOR */

/*     KEYWORDS : */
/*     ----------- */
/*      PUBLIC, VECTOR, FREE */

/*     INPUT ARGUMENTS  : */
/*     -------------------- */
/*       ndimen : dimension of the space */
/*       vecin  : input vector */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */

/*       vecout : vector non colinear to vecin */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
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
/*                     PROCESSING */
/* ***********************************************************************
 */

    if (*ndimen <= 1 || *ndimen > 3) {
	goto L9101;
    }
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
/*                   ERROR PROCESSING */
/* ***********************************************************************
 */


L9101:
    *iercod = 1;
    goto L9999;


/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
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

/*     FUNCTION : */
/*     ---------- */
/*     ACCESS IN WRITING FOR COMMON MPRCSN */

/*     KEYWORDS : */
/*     ----------- */
/*     WRITING */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*     EPSIL1  : TOLERANCE OF 3D NULL DISTANCE */
/*     EPSIL2  : TOLERANCE OF PARAMETRIC NULL DISTANCE */
/*     EPSIL3  : TOLERANCE TO AVOID DIVISION BY 0.. */
/*     EPSIL4  : ANGULAR TOLERANCE */
/*     NITER1  : MAX NB OF ITERATIONS */
/*     NITER2  : NB OF RAPID ITERATIONS */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     NONE */

/*     COMMONS USED : */
/*     ------------------ */


/*     REFERENCES CALLED : */
/*     --------------------- */


/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */
/*                            DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                      INITIALIZATIONS */
/* ***********************************************************************
 */

/* ***********************************************************************
 */
/*                      PROCESSING */
/* ***********************************************************************
 */

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*          GIVES TOLERANCES OF NULLITY IN STRIM */
/*          AND  LIMITS OF ITERATIVE PROCESSES */

/*          GENERAL CONTEXT, MODIFIABLE BY THE UTILISER */

/*     KEYWORDS : */
/*     ----------- */
/*          PARAMETER , TOLERANCE */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*       INITIALISATION   :  PROFILE , **VIA MPRFTX** AT INPUT IN STRIM*/
/*       LOADING OF DEFAULT VALUES OF THE PROFILE IN MPRFTX AT INPUT*/
/*       IN STRIM. THEY ARE PRESERVED IN THE LOCAL VARIABLES OF MPRFTX */

/*        RESET DEFAULT VALUES                   : MDFINT */
/*        MODIFICATION INTERACTIVE BY THE USER   : MDBINT */

/*        ACCESS FUNCTION  :  MMEPS1  ...  EPS1 */
/*                            MEPSPB  ...  EPS3,EPS4 */
/*                            MEPSLN  ...  EPS2, NITERM , NITERR */
/*                            MEPSNR  ...  EPS2 , NITERM */
/*                            MITERR  ...  NITERR */

/* > */
/* ***********************************************************************
 */

/*     NITERM : MAX NB OF ITERATIONS */
/*     NITERR : NB OF RAPID ITERATIONS */
/*     EPS1   : TOLERANCE OF 3D NULL DISTANCE */
/*     EPS2   : TOLERANCE OF ZERO PARAMETRIC DISTANCE */
/*     EPS3   : TOLERANCE TO AVOID DIVISION BY 0.. */
/*     EPS4   : TOLERANCE ANGULAR */


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
  doublereal result ;
  integer    absolute ;
  result = 1.0e0 ;
  if ( *n > 0 ) {absolute = *n;}
  else {absolute = -*n;}
    /* System generated locals */
  for(integer ii = 0 ; ii < absolute ; ii++) {
      result *=  *x ;
   }
  if (*n < 0) {
   result = 1.0e0 / result ;
 }
 return result ;
}
   

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Calculate integer function power not obligatory in the most efficient way ; 
*/

/*     KEYWORDS : */
/*     ----------- */
/*       POWER */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        X      :  argument of X**N */
/*        N      :  power */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        return X**N */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************/

//=======================================================================
//function : pow__ii
//purpose  : 
//=======================================================================
integer pow__ii(integer *x, 
		integer *n)

{
  integer result ;
  integer    absolute ;
  result = 1 ;
  if ( *n > 0 ) {absolute = *n;}
  else {absolute = -*n;}
    /* System generated locals */
  for(integer ii = 0 ; ii < absolute ; ii++) {
      result *=  *x ;
   }
  if (*n < 0) {
   result = 1 / result ;
 }
 return result ;
}
   

/* ********************************************************************** 
*/
/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Calculate integer function power not obligatory in the most efficient way ; 
*/

/*     KEYWORDS : */
/*     ----------- */
/*       POWER */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        X      :  argument of X**N */
/*        N      :  power */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        return X**N */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
  integer i__;
  doublereal x;
  


/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*        Calculate the scalar product of 2 vectors in the space */
/*        of dimension NDIMEN. */

/*     KEYWORDS : */
/*     ----------- */
/*        PRODUCT MSCALAIRE. */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*        NDIMEN : Dimension of the space. */
/*        VECTE1,VECTE2: Vectors. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */

/*     COMMONS USED     : */
/*     ---------------- */

/*     REFERENCES CALLED : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

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
  integer m1jm1, ncfm1, j, k;
  doublereal bid;
  doublereal cij1, cij2;
  


/************************************************************************
*******/

/*     FONCTION : */
/*     ---------- */
/*        INVERSION OF THE PARAMETERS ON CURVE 2D. */

/*     KEYWORDS : */
/*     ----------- */
/*        CURVE,2D,INVERSION,PARAMETER. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOEFF   : NB OF COEFF OF THE CURVE. */
/*        CRVOLD   : CURVE OF ORIGIN */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        CRVNEW   : THE RESULTING CURVE AFTER CHANGE OF T BY 1-T */
/*        IERCOD   :  0 OK, */
/*                   10 NB OF COEFF NULL OR TOO GREAT. */

/*     COMMONS USED   : */
/*     ---------------- */
/*    MCCNP */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*            Neant */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     THE FOLLOWING CALL IS ABSOLUTELY LEGAL : */
/*          CALL MVCVIN2(NCOEFF,CURVE,CURVE,IERCOD), THE TABLE CURVE */
/*     BECOMES INPUT AND OUTPUT ARGUMENT (RBD). */
/*     BECAUSE OF MCCNP, THE NB OF COEFF OF THE CURVE IS LIMITED TO */
/*     NDGCNP+1 = 61. */

/* > */
/* ***********************************************************************
 */


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Serves to provide coefficients of the binome (triangle of Pascal). */

/*     KEYWORDS : */
/*     ----------- */
/*      Coeff of binome from 0 to 60. read only . init par block data */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The coefficients of the binome form a triangular matrix. */
/*     This matrix is completed in table CNP by transposition. */
/*     So: CNP(I,J) = CNP(J,I) for I and J = 0, ..., 60. */

/*     Initialization is done by block-data MMLLL09.RES, */
/*     created by program MQINICNP.FOR (see the team (AC) ). */


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


/* CONSTANT TERM OF THE NEW CURVE */

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

/* INTERMEDIARY POWERS OF THE PARAMETER */

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

/* TERM OF THE HIGHEST  DEGREE */

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
  integer m1jm1, ncfm1, j, k;
  doublereal bid;
  //extern /* Subroutine */ int maermsg_();
  doublereal cij1, cij2, cij3;
  
 
/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        INVERSION OF THE PARAMETER ON A CURBE 3D (I.E. INVERSION */
/*        OF THE DIRECTION OF PARSING). */

/*     KEYWORDS : */
/*     ----------- */
/*        CURVE,INVERSION,PARAMETER. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOEFF   : NB OF COEFF OF THE CURVE. */
/*        CRVOLD   : CURVE OF ORIGIN */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        CRVNEW   : RESULTING CURVE AFTER CHANGE OF T INTO 1-T */
/*        IERCOD   :  0 OK, */
/*                   10 NB OF COEFF NULL OR TOO GREAT. */

/*     COMMONS USED   : */
/*     ---------------- */
/*    MCCNP */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*            Neant */
/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     THE FOLLOWING CALL IS ABSOLUTELY LEGAL : */
/*          CALL MVCVINV(NCOEFF,CURVE,CURVE,IERCOD), TABLE CURVE */
/*     BECOMES INPUT AND OUTPUT ARGUMENT (RBD). */
/*     THE NUMBER OF COEFF OF THE CURVE IS LIMITED TO NDGCNP+1 = 61 */
/*     BECAUSE OF USE OF COMMON MCCNP. */
/* > */
/* ***********************************************************************
 */

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*      Serves to provide the binomial coefficients (triangle of Pascal). */

/*     KEYWORDS : */
/*     ----------- */
/*      Binomial Coeff from 0 to 60. read only . init par block data */

/*     DEMSCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The binomial coefficients form a triangular matrix. */
/*     This matrix is completed in table CNP by its transposition. */
/*     So: CNP(I,J) = CNP(J,I) for I and J = 0, ..., 60. */

/*     Initialisation is done by block-data MMLLL09.RES, */
/*     created by program MQINICNP.FOR (see the team (AC) ). */
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

/* CONSTANT TERM OF THE NEW CURVE */

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

/* INTERMEDIARY POWER OF THE PARAMETER */

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

    /* TERM OF THE HIGHEST DEGREE */

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
    doublereal tampc[40];
    NCollection_Array1<doublereal> tamp (tampc[0], 1, 40);
    integer ndegl, kg, ii;
   
/* ********************************************************************** 
*/

/*      FUNCTION : */
/*      -------- */
/*  Loading of a degree gives roots of LEGENDRE polynom */
/*  DEFINED on [-1,1] and weights of Gauss quadrature formulas */
/*  (based on corresponding LAGRANGIAN interpolators). */
/*  The symmetry relative to 0 is used between [-1,0] and [0,1]. */

/*      KEYWORDS : */
/*      --------- */
/*         . VOLUMIC, LEGENDRE, LAGRANGE, GAUSS */

/*      INPUT ARGUMENTSE : */
/*      ------------------ */

/*  KINDIC : Takes values from 1 to 10 depending of the degree */
/*           of the used polynom. */
/*           The degree of the polynom is equal to 4 k, i.e. 4, 8, */
/*           12, 16, 20, 24, 28, 32, 36 and 40. */

/*      OUTPUT ARGUMENTS : */
/*      ------------------- */

/*  UROOTL : Roots of LEGENDRE polynom in domain [1,0] */
/*           given in decreasing order. For domain [-1,0], it is */
/*           necessary to take the opposite values. */
/*  HILTAB : LAGRANGE interpolators associated to roots. For */
/*           opposed roots, interpolatorsare equal. */
/*  NBRVAL : Nb of coefficients. Is equal to the half of degree */
/*           depending on the symmetry (i.e. 2*KINDIC). */

/*  IERCOD  :  Error code: */
/*          < 0 ==> Attention - Warning */
/*          =-1 ==> Value of false KINDIC. NBRVAL is forced to 20 */
/*                  (order 40) */
/*          = 0 ==> Everything is OK */

/*      COMMON USED : */
/*      ---------------- */

/*      REFERENCES CALLED : */
/*      ------------------- */

/*      DESCRIPTION/NOTES/LIMITATIONS : */
/*      --------------------------------- */
/*      If KINDIC is not correct (i.e < 1 or > 10), the degree is set */
/*      to 40 directly (ATTENTION to overload - to avoid it, */
/*      preview UROOTL and HILTAB dimensioned at least to 20). */

/*      The value of coefficients was calculated with quadruple precision */
/*      by JJM with help of GD. */
/*      Checking of roots was done by GD. */

/*      See detailed explications on the listing */
/* > */
/* ********************************************************************** 
*/


/* ------------------------------------ */
/* ****** Test  validity of KINDIC ** */
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
/* ****** Load NBRVAL positive roots depending on the degree ** 
*/
/* ---------------------------------------------------------------------- 
*/
/* ATTENTION : Sign minus (-) in the loop is intentional. */

    mmextrl_(&ndegl, tamp);
    i__1 = *nbrval;
    for (ii = 1; ii <= i__1; ++ii) {
	urootl[ii] = -tamp(ii);
/* L100: */
    }

/* ------------------------------------------------------------------- */
/* ****** Loading of NBRVAL Gauss weight depending on the degree ** */
/* ------------------------------------------------------------------- */

    mmexthi_(&ndegl, tamp);
    i__1 = *nbrval;
    for (ii = 1; ii <= i__1; ++ii) {
	hiltab[ii] = tamp(ii);
/* L200: */
    }

/* ------------------------------- */
/* ****** End of sub-program ** */
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
  integer ndeg, kk;
  doublereal xxx, yyy;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  POSITIONING ON CURVE (NCF,2) IN SPACE OF DIMENSION 2. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,MATH_ACCES:: COURBE&,POSITIONNEMENT,&POINT. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NCOEFF : NUMBER OF COEFFICIENTS OF THE CURVE */
/*     CURVE2 : EQUATION OF CURVE 2D */
/*     TPARAM : VALUE OF PARAMETER AT GIVEN POINT */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     PNTCRB : COORDINATES OF POINT CORRESPONDING TO PARAMETER */
/*              TPARAM ON CURVE 2D CURVE2. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ---------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     MSCHEMA OF HORNER. */

/* > */
/* ********************************************************************** 
*/


/* -------- INITIALIZATIONS AND PROCESSING OF PARTICULAR CASES ---------- 
*/

/* ---> Cas when NCOEFF > 1 (case STANDARD). */
    /* Parameter adjustments */
    --pntcrb;
    curve2 -= 3;

    /* Function Body */
    if (*ncoeff >= 2) {
	goto L1000;
    }
/* ---> Case when NCOEFF <= 1. */
    if (*ncoeff <= 0) {
	pntcrb[1] = 0.;
	pntcrb[2] = 0.;
	goto L9999;
    } else if (*ncoeff == 1) {
	pntcrb[1] = curve2[3];
	pntcrb[2] = curve2[4];
	goto L9999;
    }

/* -------------------- MSCHEMA OF HORNER (PARTICULAR CASE) --------------
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

/* ---------------------------- MSCHEMA OF HORNER ------------------------
 */
/* ---> TPARAM is different from 1.D0 and 0.D0. */

    ndeg = *ncoeff - 1;
    xxx = curve2[(*ncoeff << 1) + 1];
    yyy = curve2[(*ncoeff << 1) + 2];
    for (kk = ndeg; kk >= 1; --kk) {
	xxx = xxx * *tparam + curve2[(kk << 1) + 1];
	yyy = yyy * *tparam + curve2[(kk << 1) + 2];
/* L200: */
    }
    goto L5000;

/* ------------------------ RECOVER THE CALCULATED POINT --------------- 
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
  integer ndeg, kk;
  doublereal xxx, yyy, zzz;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/* POSITIONING ON A CURVE (3,NCF) IN THE SPACE OF DIMENSION 3. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, MATH_ACCES:: COURBE&,POSITIONNEMENT,&POINT. */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*     NCOEFF : NB OF COEFFICIENTS OF THE CURVE */
/*     CURVE3 : EQUATION OF CURVE 3D */
/*     TPARAM : VALUE OF THE PARAMETER AT THE GIVEN POINT */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     PNTCRB : COORDINATES OF THE POINT CORRESPONDING TO PARAMETER */
/*              TPARAM ON CURVE 3D CURVE3. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*            Neant */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     MSCHEMA OF HORNER. */
/* > */
/* ********************************************************************** 
*/
/*                           DECLARATIONS */
/* ********************************************************************** 
*/


/* -------- INITIALISATIONS AND PROCESSING OF PARTICULAR CASES ---------- 
*/

/* ---> Case when NCOEFF > 1 (cas STANDARD). */
    /* Parameter adjustments */
    --pntcrb;
    curve3 -= 4;

    /* Function Body */
    if (*ncoeff >= 2) {
	goto L1000;
    }
/* ---> Case when NCOEFF <= 1. */
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

/* -------------------- MSCHEMA OF HORNER (PARTICULAR CASE) --------------
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

/* ---------------------------- MSCHEMA OF HORNER ------------------------
 */
/* ---> Here TPARAM is different from 1.D0 and 0.D0. */

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

/* ------------------------ RETURN THE CALCULATED POINT ------------------ 
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
  integer incr;
  doublereal dsave;
  integer i3, i4, i5, incrp1;


/************************************************************************
*******/

/*     FUNCTION : */
/*     ---------- */
/*       PARSING OF COLUMNS OF TABLE OF REAL*8 BY SHELL METHOD*/
/*        (IN INCREASING ORDER) */

/*     KEYWORDS : */
/*     ----------- */
/*        POINT-ENTRY, PARSING, SHELL */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        N      : NUMBER OF COLUMNS OF THE TABLE */
/*        IS     : NUMBER OF LINE OF THE TABLE */
/*        DTAB   : TABLE OF REAL*8 TO BE PARSED */
/*        ICLE   : POSITION OF THE KEY ON THE COLUMN */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        DTAB   : PARSED TABLE */

/*     COMMONS USED   : */
/*     ---------------- */


/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*            Neant */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     CLASSIC SHELL METHOD : PARSING BY SERIES */
/*     Declaration DTAB(IS, 1) corresponds to DTAB(IS, *) */
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

/*  INITIALIZATION OF THE SEQUENCE OF INCREMENTS */
/*  FIND THE GREATEST INCREMENT SO THAT INCR < N/9 */

    incr = 1;
L1001:
    if (incr >= *n / 9) {
	goto L1002;
    }
/*     ----------------------------- */
    incr = incr * 3 + 1;
    goto L1001;

/*  LOOP ON INCREMENTS TILL INCR = 1 */
/*  PARSING BY SERIES DISTANT FROM INCR */

L1002:
    incrp1 = incr + 1;
/*     ----------------- */
    i__1 = *n;
    for (i3 = incrp1; i3 <= i__1; ++i3) {
/*        ---------------------- */

/*  SET ELEMENT I3 AT ITS PLACE IN THE SERIES */

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

/*  PASSAGE TO THE NEXT INCREMENT */

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
  doublereal xsom;
  integer i__, irmax;
  
  

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*        SERVES to calculate the euclidian norm of a vector : */
/*                       ____________________________ */
/*                  Z = V  V(1)**2 + V(2)**2 + ... */

/*     KEYWORDS : */
/*     ----------- */
/*        SURMFACIQUE, */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMEN : Dimension of the vector */
/*        VECTEU : vector of dimension NDIMEN */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        MZSNORM : Value of the euclidian norm of vector VECTEU */

/*     COMMONS USED   : */
/*     ---------------- */

/*      .Neant. */

/*     REFERENCES CALLED   : */
/*     ---------------------- */
/*     Type  Name */
/*      R*8  ABS            R*8  SQRT */

/*     DESCRIPTION/NOTESS/LIMITATIONS : */
/*     ----------------------------------- */
/*     To limit the risks of overflow, */
/*     the term of the strongest absolute value is factorized : */
/*                                _______________________ */
/*                  Z = !V(1)! * V  1 + (V(2)/V(1))**2 + ... */

/* > */
/* ***********************************************************************
 */
/*                      DECLARATIONS */
/* ***********************************************************************
 */


/* ***********************************************************************
 */
/*                     PROCESSING */
/* ***********************************************************************
 */

/* ___ Find the strongest absolute value term */

    /* Parameter adjustments */
    --vecteu;

    /* Function Body */
    irmax = 1;
    i__1 = *ndimen;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if ((d__1 = vecteu[irmax], advapp_abs(d__1)) < (d__2 = vecteu[i__], advapp_abs(d__2)
		)) {
	    irmax = i__;
	}
/* L100: */
    }

/* ___ Calculate the norme */

    if ((d__1 = vecteu[irmax], advapp_abs(d__1)) < 1.) {
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
	ret_val = (d__1 = vecteu[irmax], advapp_abs(d__1)) * sqrt(xsom);
    }

/* ***********************************************************************
 */
/*                   RETURN CALLING PROGRAM */
/* ***********************************************************************
 */

    return ret_val;
} /* mzsnorm_ */

