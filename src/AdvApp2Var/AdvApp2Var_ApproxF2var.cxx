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

#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_Data.hxx>
#include <AdvApp2Var_ApproxF2var.hxx>

#include <cmath>

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
	     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
	     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
  integer c__1 = 1;

  /* System generated locals */
  integer contr1_dim1, contr1_offset, contr2_dim1, contr2_offset, 
  somtab_dim1, somtab_offset, diftab_dim1, diftab_offset, 
  fpntab_dim1, fpntab_offset, hermit_dim1, hermit_offset, i__1, 
  i__2, i__3;

  /* Local variables */
  integer nroo2, ncfhe, nd, ii, kk;
  integer ibb, kkm, kkp;
  doublereal bid1, bid2, bid3 = 0.;

/* ********************************************************************** 
*/
/*     FUNCTION : */
/*     ---------- */
/*     Discretisation on the parameters of interpolation polynomes */
/*     constraints of order IORDRE. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Space dimension. */
/*     NBROOT: Number of INTERNAL discretisation parameters. */
/*             It is also the root number Legendre polynome where */
/*             the discretization is performed. */
/*     ROOTLG: Table of discretization parameters ON (-1,1). */
/*     IORDRE: Order of constraint imposed to the extremities of the iso. */
/*             = 0, the extremities of the iso are calculated */
/*             = 1, additionally, the 1st derivative in the direction */
/*                  of the iso is calculated. */
/*             = 2, additionally, the 2nd derivative in the direction */
/*                  of the iso is calculated. */
/*     CONTR1: Contains, if IORDRE>=0, values IORDRE+1 in TTABLE(0) 
*/
/*             (1st extremity) of derivatives of F(Uc,Ve) or F(Ue,Vc), */
/*             see below. */
/*     CONTR2: Contains, if IORDRE>=0, values IORDRE+1 in */
/*             TTABLE(NBROOT+1) (2nd extremity) of: */
/*              If ISOFAV=1, derived of order IDERIV by U, derived */
/*             ordre 0 to IORDRE by V of F(Uc,Ve) or Uc=TCONST */
/*             (fixed iso value) and Ve is the fixed extremity. */
/*               If  ISOFAV=2, derivative of order IDERIV by V, derivative */
/*             of order 0 to IORDRE by U of F(Ue,Vc) or Vc=TCONST */
/*             (fixed iso value) and Ue is the fixed extremity. */

/*     SOMTAB: Table of NBROOT/2 sums of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2. */
/*     DIFTAB: Table of NBROOT/2 differences of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     SOMTAB: Table of NBROOT/2 sums of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2 */
/*     DIFTAB: Table of  NBROOT/2 differences of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2 */
/*     FPNTAB: Auxiliary table. */
/*     HERMIT: Table of coeff. 2*(IORDRE+1) Hermite polynoms */
/*             of degree 2*IORDRE+1. */
/*     IERCOD: Error code, */
/*             = 0, Everythig is OK */
/*             = 1, The value of IORDRE is out of (0,2) */
/*     COMMON USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The results of discretization are arranged in 2 tables */
/*     SOMTAB and DIFTAB to earn time during the */
/*     calculation of coefficients of the approximation curve. */

/*     If NBROOT is uneven in SOMTAB(0,*) and DIFTAB(0,*) one stores */
/*     the values of the median root of Legendre (0.D0 in (-1,1)). */

/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* --- Recuperate 2*(IORDRE+1) coeff of 2*(IORDRE+1) of Hermite polynom --- 
*/

    AdvApp2Var_ApproxF2var::mma1her_(iordre, &hermit[hermit_offset], iercod);
    if (*iercod > 0) {
	goto L9100;
    }

/* ------------------- Discretization of Hermite polynoms ----------- 
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

/* ---- Discretizations of boundary polynoms are taken ---- 
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

/* ------------ Cas when discretization is done on the roots of a  ----------- 
*/
/* ---------- Legendre polynom of uneven degree, 0 is root -------- 
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
/* --> IORDRE is not in the authorized zone. */
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
  integer nd, ii, jj, ibb;
  doublereal bid;


  /* ***********************************************************************
   */
  
  /*     FUNCTION : */
  /*     ---------- */
  /*     Add constraint to polynom. */
  
  /*     MOTS CLES : */
  /*     ----------- */
  /*     ALL,AB_SPECIFI::COURE&,APPROXIMATION,ADDITION,&CONSTRAINT */
  
  /*     INPUT ARGUMENTS : */
  /*     -------------------- */
  /*     NDIMEN: Dimension of the space */
  /*     IORDRE: Order of constraint. */
  /*     CONTR1: pt of constraint in -1, from order 0 to IORDRE. */
  /*     CONTR2: Pt of constraint in +1, from order 0 to IORDRE. */
  /*     HERMIT: Table of Hermit polynoms of order IORDRE. */
  /*     CRVJAV: Curve of approximation in Jacobi base. */
  
  /*     OUTPUT ARGUMENTS : */
  /*     --------------------- */
  /*     CRVJAV: Curve of approximation in Jacobi base */
  /*             to which the polynom of interpolation of constraints is added. */
  
  /*     COMMON USED : */
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
/*   Name of the routine */

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
/*                            Processing */
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
/*                   RETURN CALLING PROGRAM */
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
	     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
  integer ideb, ifin, nroo2, ideru, iderv;
  doublereal renor;
  integer ii, nd, ibb, iim, nbp, iip;
  doublereal bid1, bid2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     DiscretiZation of a non-polynomial function F(U,V) or of */
/*     its derivative with fixed isoparameter. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::FONCTION&, DISCRETISATION, &POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Space dimension. */
/*     UVFONC: Limits of the path of definition by U and by V of the approximated function */
/*     FONCNP: The NAME of the non-polynomial function to be approximated */
/*             (external program). */
/*     ISOFAV: Fixed isoparameter for the discretization; */
/*             = 1, discretization with fixed U and variable V. */
/*             = 2, discretization with fixed V and variable U. */
/*     TCONST: Iso value is also fixed. */
/*     NBROOT: Number of INTERNAL discretization parameters. */
/*             (if there are constraints, 2 extremities should be added). 
*/
/*             This is also the root number of the Legendre polynom where */
/*             the discretization is done. */
/*     TTABLE: Table of discretization parameters and of 2 extremities */
/*             (Respectively (-1, NBROOT Legendre roots,1) */
/*             reframed within the adequate interval. */
/*     IORDRE: Order of constraint imposed on the extremities of the iso. */
/*             (If Iso-U, it is necessary to calculate the derivatives by V and vice */
/*             versa). */
/*             = 0, the extremities of the iso are calculated. */
/*             = 1, additionally the 1st derivative in the direction of the iso is calculated */
/*             = 2, additionally the 2nd derivative in the direction of the iso is calculated */
/*     IDERIV: Order of derivative transversal to fixed iso (If Iso-U=Uc */
/*             is fixed, the derivative of order IDERIV is discretized by U of */
/*             F(Uc,v). Same if iso-V is fixed). */
/*             Varies from 0 (positioning) to 2 (2nd derivative). */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     FPNTAB: Auxiliary table. 
       SOMTAB: Table of NBROOT/2 sums of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2 */
/*     DIFTAB: Table of  NBROOT/2 differences of 2 index points */
/*             NBROOT-II+1 and II, for II = 1, NBROOT/2 */
/*     CONTR1: Contains, if IORDRE>=0, values IORDRE+1 in TTABLE(0) 
*/
/*             (1st extremity) of derivatives of F(Uc,Ve) or F(Ue,Vc), */
/*             see below. */
/*     CONTR2: Contains, if IORDRE>=0, values IORDRE+1 in */
/*             TTABLE(NBROOT+1) (2nd extremity) of: */
/*              If ISOFAV=1, derived of order IDERIV by U, derived */
/*             ordre 0 to IORDRE by V of F(Uc,Ve) or Uc=TCONST */
/*             (fixed iso value) and Ve is the fixed extremity. */
/*               If  ISOFAV=2, derivative of order IDERIV by V, derivative */
/*             of order 0 to IORDRE by U of F(Ue,Vc) or Vc=TCONST */
/*             (fixed iso value) and Ue is the fixed extremity. */
/*     IERCOD: Error code > 100; Pb in  evaluation of FONCNP, */
/*             the returned error code is equal to error code of FONCNP + 100. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The results of discretization are arranged in 2 tables */
/*     SOMTAB and DIFTAB to earn time during the */
/*     calculation of coefficients of the approximation curve. */

/*     If NBROOT is uneven in SOMTAB(0,*) and DIFTAB(0,*) one stores */
/*     the values of the median root of Legendre (0.D0 in (-1,1)). */

/*     Function F(u,v) defined in UVFONC is reparameterized in */
/*     (-1,1)x(-1,1). Then 1st and 2nd derivatives are renormalized. */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* --------------- Definition of the nb of points to calculate -------------- 
*/
/* --> If constraints, the limits are also taken */
    if (*iordre >= 0) {
	ideb = 0;
	ifin = *nbroot + 1;
/* --> Otherwise, only Legendre roots (reframed) are used
. */
    } else {
	ideb = 1;
	ifin = *nbroot;
    }
/* --> Nb of point to calculate. */
    nbp = ifin - ideb + 1;
    nroo2 = *nbroot / 2;

/* --------------- Determination of the order of global derivation -------- 
*/
/* --> ISOFAV takes only values 1 or 2. */
/*    if Iso-U, derive by U of order IDERIV */
    if (*isofav == 1) {
	ideru = *ideriv;
	iderv = 0;
	d__1 = (uvfonc[4] - uvfonc[3]) / 2.;
	renor = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
/*    if Iso-V, derive by V of order IDERIV */
    } else {
	ideru = 0;
	iderv = *ideriv;
	d__1 = (uvfonc[6] - uvfonc[5]) / 2.;
	renor = AdvApp2Var_MathBase::pow__di(&d__1, ideriv);
    }

/* ----------- Discretization on roots of the  --------------- 
*/
/* ---------------------- Legendre polynom of degree NBROOT ------------------- 
*/

    (*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (ndimen, 
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

/* ------------ Case when discretisation is done on roots of a ---- 
*/
/* ---------- Legendre polynom of uneven degree, 0 is root -------- 
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


/* --------------------- Take into account constraints ---------------- 
*/

    if (*iordre >= 0) {
/* --> Recover already calculated extremities. */
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    contr1[nd + contr1_dim1] = renor * fpntab[nd];
	    contr2[nd + contr2_dim1] = renor * fpntab[nd + (*nbroot + 1) * 
		    fpntab_dim1];
/* L400: */
	}
/* --> Nb of points to calculate/call to FONCNP */
	nbp = 1;
/*    If Iso-U, derive by V till order IORDRE */
	if (*isofav == 1) {
/* --> Factor of normalisation 1st derivative. */
	    bid1 = (uvfonc[6] - uvfonc[5]) / 2.;
	    i__1 = *iordre;
	    for (iderv = 1; iderv <= i__1; ++iderv) {
		(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
            ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, ttable, &ideru, &iderv, &contr1[(iderv + 1) * 
			contr1_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L500: */
	    }
	    i__1 = *iordre;
	    for (iderv = 1; iderv <= i__1; ++iderv) {
		(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
            ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, &ttable[*nbroot + 1], &ideru, &iderv, &contr2[(
			iderv + 1) * contr2_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L510: */
	    }
/*    If Iso-V, derive by U till order IORDRE */
	} else {
/* --> Factor of normalization  1st derivative. */
	    bid1 = (uvfonc[4] - uvfonc[3]) / 2.;
	    i__1 = *iordre;
	    for (ideru = 1; ideru <= i__1; ++ideru) {
		(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
            ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, ttable, &ideru, &iderv, &contr1[(ideru + 1) * 
			contr1_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L600: */
	    }
	    i__1 = *iordre;
	    for (ideru = 1; ideru <= i__1; ++ideru) {
		(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
            ndimen, &uvfonc[3], &uvfonc[5], isofav, tconst, &
			nbp, &ttable[*nbroot + 1], &ideru, &iderv, &contr2[(
			ideru + 1) * contr2_dim1 + 1], iercod);
		if (*iercod > 0) {
		    goto L9999;
		}
/* L610: */
	    }
	}

/* ------------------------- Normalization of derivatives -------------
---- */
/* (The function is redefined on (-1,1)*(-1,1)) */
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
  integer idim, ncfja, ncfnw, ndses, ii, kk, ibb, ier;
  integer nbr0;


/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*     Calculate the degree and the errors of approximation of a border. */

/*     KEYWORDS : */
/*     ----------- */
/*      TOUS,AB_SPECIFI :: COURBE&,TRONCATURE, &PRECISION */

/*     INPUT ARGUMENTS : */
/*     -------------------- */

/*     NDIMEN: Total Dimension of the space (sum of dimensions of sub-spaces) */
/*     NBSESP: Number of "independent" sub-spaces. */
/*     NDIMSE: Table of dimensions of sub-spaces. */
/*     IORDRE: Order of constraint at the extremities of the border */
/*              -1 = no constraints, */
/*               0 = constraints of passage to limits (i.e. C0), */
/*               1 = C0 + constraintes of 1st derivatives (i.e. C1), */
/*               2 = C1 + constraintes of 2nd derivatives (i.e. C2). */
/*     NDGJAC: Degree of development in series to use for the calculation  */
/*             in the base of Jacobi. */
/*     CRVJAC: Table of coeff. of the curve of approximation in the */
/*             base of Jacobi. */
/*     NCFLIM: Max number of coeff of the polynomial curve */
/*             of approximation (should be above or equal to */
/*             2*IORDRE+2 and below or equal to 50). */
/*     EPSAPR: Table of errors of approximations that cannot be passed, */
/*             sub-space by sub-space. */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     YCVMAX: Auxiliary Table. */
/*     ERRMAX: Table of errors (sub-space by sub-space) */
/*             MAXIMUM made in the approximation of FONCNP by */
/*             COURBE. */
/*     ERRMOY: Table of errors (sub-space by sub-space) */
/*             AVERAGE made in the approximation of FONCNP by */
/*             COURBE. */
/*     NCOEFF: Number of significative coeffs. of the calculated "curve". */
/*     IERCOD: Error code */
/*             = 0, ok, */
/*             =-1, warning, required tolerance can't be */
/*                  met with coefficients NFCLIM. */
/*             = 1, order of constraints (IORDRE) is not within authorised values */


/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* > */
/* ********************************************************************** 
*/

/*  Name of the routine */


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

/* ------------ Calculate the degree of the curve and of the Max error -------- 
*/
/* -------------- of approximation for all sub-spaces -------- 
*/

    i__1 = *nbsesp;
    for (ii = 1; ii <= i__1; ++ii) {
	ndses = ndimse[ii];

/* ------------ cutting of coeff. and calculation of Max error -------
---- */

	AdvApp2Var_MathBase::mmtrpjj_(&ncfja, &ndses, &ncfja, &epsapr[ii], iordre, &crvjac[idim * 
		crvjac_dim1], &ycvmax[1], &errmax[ii], &ncfnw);

/* ******************************************************************
**** */
/* ------------- If precision OK, calculate the average error -------
---- */
/* ******************************************************************
**** */

	if (ncfnw <= *ncflim) {
	    mmaperm_(&ncfja, &ndses, &ncfja, iordre, &crvjac[idim * 
		    crvjac_dim1], &ncfnw, &errmoy[ii]);
	    *ncoeff = advapp_max(ncfnw,*ncoeff);

/* ------------- Set the declined coefficients to 0.D0 -----------
-------- */

	    nbr0 = *ncflim - ncfnw;
	    if (nbr0 > 0) {
		i__2 = ndses;
		for (kk = 1; kk <= i__2; ++kk) {
		  AdvApp2Var_SysBase::mvriraz_(&nbr0, 
			     &crvjac[ncfnw + (idim + kk - 1) * crvjac_dim1]);
/* L200: */
		}
	    }
	} else {

/* **************************************************************
******** */
/* ------------------- If required precision can't be reached----
-------- */
/* **************************************************************
******** */

	    *iercod = -1;

/* ------------------------- calculate the Max error ------------
-------- */

	    AdvApp2Var_MathBase::mmaperx_(&ncfja, &ndses, &ncfja, iordre, &crvjac[idim * 
		    crvjac_dim1], ncflim, &ycvmax[1], &errmax[ii], &ier);
	    if (ier > 0) {
		goto L9100;
	    }

/* -------------------- nb of coeff to be returned -------------
-------- */

	    *ncoeff = *ncflim;

/* ------------------- and calculation of the average error ----
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
/* --> The order of constraints is not within autorized values. */
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
  integer ibb;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculate 2*(IORDRE+1) Hermit polynoms of  degree 2*IORDRE+1 */
/*     on (-1,1) */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::CONTRAINTE&, INTERPOLATION, &POLYNOME */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     IORDRE: Order of constraint. */
/*      = 0, Polynom of interpolation of order C0 on (-1,1). */
/*      = 1, Polynom of interpolation of order C0 and C1 on (-1,1). */
/*      = 2, Polynom of interpolation of order C0, C1 and C2 on (-1,1). 
*/

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     HERMIT: Table of 2*IORDRE+2 coeff. of each of  2*(IORDRE+1) */
/*             HERMIT polynom. */
/*     IERCOD: Error code, */
/*      = 0, Ok */
/*      = 1, required order of constraint is not managed here. */
/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     The part of HERMIT(*,2*i+j) table where  j=1 or 2 and i=0 to IORDRE, */
/*     contains the coefficients of the polynom of degree 2*IORDRE+1 */
/*     such as ALL values in -1 and in +1 of this polynom and its */
/*     derivatives till order of derivation IORDRE are NULL, */
/*     EXCEPT for the derivative of order i: */
/*        - valued 1 in -1 if j=1 */
/*        - valued 1 in +1 if j=2. */
/* > */
/* ********************************************************************** 
*/

/*  Name of the routine */


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

/* --- Recover (IORDRE+2) coeff of 2*(IORDRE+1) Hermit polynoms -- 
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
  crvjac_dim1, crvjac_offset;

  /* Local variables */
  integer ibb;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculate the curve of approximation of a non-polynomial function */
/*     in the base of Jacobi. */

/*     KEYWORDS : */
/*     ----------- */
/*     FUNCTION,DISCRETISATION,APPROXIMATION,CONSTRAINT,CURVE,JACOBI */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Total dimension of the space (sum of dimensions */
/*             of sub-spaces) */
/*     NBROOT: Nb of points of discretization of the iso, extremities not */
/*             included. */
/*     IORDRE: Order of constraint at the extremities of the boundary */
/*              -1 = no constraints, */
/*               0 = constraints of passage of limits (i.e. C0), */
/*               1 = C0 + constraints of 1st derivatives (i.e. C1), */
/*               2 = C1 + constraints of 2nd derivatives (i.e. C2). */
/*     NDGJAC: Degree of development in series to be used for calculation in the  */
/*             base of Jacobi. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     CRVJAC : Curve of approximation of FONCNP with (eventually) */
/*              taking into account of constraints at the extremities. */
/*              This curve is of degree NDGJAC. */
/*     IERCOD : Error code : */
/*               0 = All is ok. */
/*              33 = Pb to return data of du block data */
/*                   of coeff. of integration by GAUSS method. */
/*                   by program MMAPPTT. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

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

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMA1JAK", 7L);
    }
    *iercod = 0;

/* ----------------- Recover coeffs of integration by Gauss ----------- 
*/

    AdvApp2Var_ApproxF2var::mmapptt_(ndgjac, nbroot, iordre, cgauss, iercod);
    if (*iercod > 0) {
	*iercod = 33;
	goto L9999;
    }

/* --------------- Calculate the curve in the base of Jacobi ----------- 
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
  doublereal rider, riord;
  integer nd, ibb;
  doublereal bid;
/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Normalization of constraints of derivatives, defined on DFUVIN */
/*     on block DUVOUT. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL, AB_SPECIFI::VECTEUR&,DERIVEE&,NORMALISATION,&VECTEUR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     DFUVIN: Limits of the block of definition by U and by V where 
*/
/*             constraints CNTRIN are defined. */
/*     NDIMEN: Dimension of the space. */
/*     IORDRE: Order of constraint imposed at the extremities of the iso. */
/*             (if Iso-U, it is necessary to calculate derivatives by V and vice */
/*             versa). */
/*             = 0, the extremities of the iso are calculated */
/*             = 1, additionally the 1st derivative in the direction */
/*                  of the iso is calculated */
/*             = 2, additionally the 2nd derivative in the direction */
/*                  of the iso is calculated */
/*     CNTRIN: Contains, if IORDRE>=0, IORDRE+1 derivatives */
/*             of order IORDRE of F(Uc,v) or of F(u,Vc), following the */
/*             value of ISOFAV, RENORMALIZED by u and v in (-1,1). */
/*     DUVOUT: Limits of the block of definition by U and by V where the */
/*             constraints CNTOUT will be defined. */
/*     ISOFAV: Isoparameter fixed for the discretization; */
/*             = 1, discretization with fixed U=Uc and variable V. */
/*             = 2, discretization with fixed V=Vc and variable U. */
/*     IDERIV: Ordre de derivee transverse a l'iso fixee (Si Iso-U=Uc */
/*             is fixed, the derivative of order IDERIV is discretized by U */
/*             of F(Uc,v). The same if iso-V is fixed). */
/*             Varies from (positioning) to 2 (2nd derivative). */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     CNTOUT: Contains, if IORDRE>=0, IORDRE+1 derivatives */
/*             of order IORDRE of F(Uc,v) or of F(u,Vc), depending on the */
/*             value of ISOFAV, RENORMALIZED for u and v in DUVOUT. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */
/*     CNTRIN can be an output/input  argument, */
/*     so the call: */

/*      CALL MMA1NOC(DFUVIN,NDIMEN,IORDRE,CNTRIN,DUVOUT */
/*     1           ,ISOFAV,IDERIV,CNTRIN) */

/*     is correct. */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* --------------- Determination of coefficients of normalization -------
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

/* ------------- Renormalization of the vector of constraint --------------- 
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
  doublereal alinu, blinu, alinv, blinv;
  integer ii, ibb;

/* ***********************************************************************
 */

/*     FUNCTION : */
/*     ---------- */
/*     Normalization of parameters of an iso, starting from  */
/*     parametric block and parameters on (-1,1). */

/*     KEYWORDS : */
/*     ----------- */
/*      TOUS,AB_SPECIFI :: ISO&,POINT&,NORMALISATION,&POINT,&ISO */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*        NBROOT: Nb of points of discretisation INSIDE the iso */
/*                defined on (-1,1). */
/*        ROOTLG: Table of discretization parameters on )-1,1( */
/*                of the iso. */
/*        UVFONC: Block of definition of the iso */
/*        ISOFAV: = 1, this is iso-u; =2, this is iso-v. */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*        TTABLE: Table of parameters renormalized on UVFONC of the iso. 
*/
/*        IERCOD: = 0, OK */
/*                = 1, ISOFAV is out of allowed values. */

/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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
  logical ldbg;
  integer ndgu, ndgv;
  doublereal bidu1, bidu2, bidv1, bidv2;
  integer ioru1, iorv1, ii, nd, jj, ku, kv;
  doublereal cnt1, cnt2, cnt3, cnt4;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Add polynoms of edge constraints. */

/*     KEYWORDS : */
/*     ----------- */
/*  TOUS,AB_SPECIFI::POINT&,CONTRAINTE&,ADDITION,&POLYNOME */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*   NDIMEN: Dimension of the space. */
/*   MXUJAC: Max degree of the polynom of approximation by U. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   MXVJAC: Max degree of the polynom of approximation by V. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   IORDRU: Order of the base of Jacobi (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints: C0, C1 or C2. */
/*   IORDRV: Order of the base of Jacobi (-1,0,1 or 2) by V. Corresponds */
/*           to the step of constraints: C0, C1 or C2. */
/*   CONTR1: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V0) and its derivatives. */
/*   CONTR2: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V0) and its derivatives. */
/*   CONTR3: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V1) and its derivatives. */
/*   CONTR4: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V1) and its derivatives. */
/*   UHERMT: Coeff. of Hermit polynoms of order IORDRU. */
/*   VHERMT: Coeff. of Hermit polynoms of order IORDRV. */
/*   PATJAC: Table of coefficients of the polynom P(u,v) of approximation */
/*           of F(u,v) WITHOUT taking into account the constraints. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   PATJAC: Table of coefficients of the polynom P(u,v) by approximation */
/*           of F(u,v) WITH taking into account of constraints. */
/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */

/* --------------------------- Initialization -------------------------- 
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

/* ------------ SUBTRACTION OF ANGULAR CONSTRAINTS ------------------- 
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
  logical ldbg;
  integer ndgv1, ndgv2, ii, jj, nd, kk;
  doublereal bid1, bid2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Add polynoms of constraints */

/*     KEYWORDS : */
/*     ----------- */
/*     FUNCTION,APPROXIMATION,COEFFICIENT,POLYNOM */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMEN: Dimension of the space. */
/*   MXUJAC: Max degree of the polynom of approximation by U. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   MXVJAC: Max degree of the polynom of approximation by V. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   IORDRV: Order of the base of Jacobi (-1,0,1 or 2) by V. Corresponds */
/*           to the step of constraints: C0, C1 or C2. */
/*   NCLIMU  LIMIT nb of coeff by u of the solution P(u,v) 
*    NCFIV1: Nb of Coeff. of curves stored in CRBIV1. */
/*   CRBIV1: Table of coeffs of the approximation of iso-V0 and its */
/*           derivatives till order IORDRV. */
/*   NCFIV2: Nb of Coeff. of curves stored in CRBIV2. */
/*   CRBIV2: Table of coeffs of approximation of iso-V1 and its */
/*           derivatives till order IORDRV. */
/*   VHERMT: Coeff. of Hermit polynoms of order IORDRV. */
/*   PATJAC: Table of coefficients of the polynom P(u,v) of approximation */
/*           of F(u,v) WITHOUT taking into account the constraints. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   PATJAC: Table of coefficients of the polynom P(u,v) by approximation */
/*           of F(u,v) WITH taking into account of constraints. */
/* > */


/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */

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

/* ------------ ADDING of coeff by u of curves, by v of Hermit -------- 
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
  logical ldbg;
  integer ndgu1, ndgu2, ii, jj, nd, kk;
  doublereal bid1, bid2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Ajout des polynomes de contraintes */

/*     KEYWORDS : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMEN: Dimension of the space. */
/*   MXUJAC: Max degree of the polynom of approximation by U. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   MXVJAC: Max degree of the polynom of approximation by V. The  */
/*           representation in the orthogonal base starts from degree */
/*           0 to degree MXUJAC-2*(IORDRU+1). The polynomial base is the */
/*           base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*   IORDRU: Order of the base of Jacobi (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints: C0, C1 or C2. */
/*   NCLIMV  LIMIT nb of coeff by v of the solution P(u,v) 
*    NCFIU1: Nb of Coeff. of curves stored in CRBIU1. */
/*   CRBIU1: Table of coeffs of the approximation of iso-U0 and its */
/*           derivatives till order IORDRU. */
/*   NCFIU2: Nb of Coeff. of curves stored in CRBIU2. */
/*   CRBIU2: Table of coeffs of approximation of iso-U1 and its */
/*           derivatives till order IORDRU */
/*   UHERMT: Coeff. of Hermit polynoms of order IORDRU. */
/*   PATJAC: Table of coefficients of the polynom P(u,v) of approximation */
/*           of F(u,v) WITHOUT taking into account the constraints. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   PATJAC: Table of coefficients of the polynom P(u,v) by approximation */
/*           of F(u,v) WITH taking into account of constraints. */


/* > */
/* ********************************************************************** 
*/
/*   The name of the routine */

/* --------------------------- Initializations -------------------------- 
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

/* ------------ ADDING of coeff by u of curves, by v of Hermit -------- 
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
  logical ldbg;
  integer ilon1, ilon2, ii, nd;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Change of Jacobi base to canonical (-1,1) and writing in a greater */
/*     table. */

/*     KEYWORDS : */
/*     ----------- */
/*     ALL,AB_SPECIFI,CARREAU&,CONVERSION,JACOBI,CANNONIQUE,&CARREAU */

/*     INPUT ARGUMENTS : */
/*     -------------------- */
/*     NCFMXU: Dimension by U of resulting table PATCAN */
/*     NCFMXV: Dimension by V of resulting table PATCAN */
/*     NDIMEN: Dimension of the workspace. */
/*     IORDRU: Order of constraint by U */
/*     IORDRV: Order of constraint by V. */
/*     NCOEFU: Nb of coeff by U of square PATJAC */
/*     NCOEFV: Nb of coeff by V of square PATJAC */
/*     PATJAC: Square in the base of Jacobi of order IORDRU by U and */
/*             IORDRV by V. */

/*     OUTPUT ARGUMENTS : */
/*     --------------------- */
/*     PATAUX: Auxiliary Table. */
/*     PATCAN: Table of coefficients in the canonic base. */
/*     IERCOD: Error code. */
/*             = 0, everything goes well, and all things are equal. */
/*             = 1, the program refuses to process with incorrect input arguments */


/*     COMMONS USED : */
/*     ------------------ */

/*     REFERENCES CALLED : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
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

/* --> Pass to canonic base (-1,1) */
    mmjacpt_(ndimen, ncoefu, ncoefv, iordru, iordrv, &patjac[patjac_offset], &
	    pataux[1], &patcan[patcan_offset]);

/* --> Write all in a greater table */
    AdvApp2Var_MathBase::mmfmca8_(ncoefu, 
	     ncoefv, 
	     ndimen, 
	     ncfmxu, 
	     ncfmxv, 
	     ndimen, 
	     &patcan[patcan_offset], 
	     &patcan[patcan_offset]);

/* --> Complete with zeros the resulting table. */
    ilon1 = *ncfmxu - *ncoefu;
    ilon2 = *ncfmxu * (*ncfmxv - *ncoefv);
    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	if (ilon1 > 0) {
	    i__2 = *ncoefv;
	    for (ii = 1; ii <= i__2; ++ii) {
		AdvApp2Var_SysBase::mvriraz_(&ilon1, 
			 &patcan[*ncoefu + 1 + (ii + nd * patcan_dim2) * patcan_dim1]);
/* L110: */
	    }
	}
	if (ilon2 > 0) {
	    AdvApp2Var_SysBase::mvriraz_(&ilon2, 
		     &patcan[(*ncoefv + 1 + nd * patcan_dim2) * patcan_dim1 + 1]);
	}
/* L100: */
    }

    goto L9999;

/* ---------------------- 
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
  integer c__1 = 1;

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
    integer ncfhu, ncfhv, nuroo, nvroo, nd, ii, jj, kk, ll, ibb, kkm, 
	    llm, kkp, llp;
    doublereal bid1, bid2, bid3, bid4;
    doublereal diu1, diu2, div1, div2, sou1, sou2, sov1, sov2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Discretisation on the parameters of polynoms of interpolation */
/*     of constraints at the corners of order IORDRE. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Dimension of the space. */
/*     NBPNTU: Nb of INTERNAL parameters of discretisation by U. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     UROOTL: Table of parameters of discretisation ON (-1,1) by U. 
*/
/*     NBPNTV: Nb of INTERNAL  parameters of discretisation by V. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     VROOTL: Table of discretization parameters on (-1,1) by V. */
/*     IORDRU: Order of constraint imposed at the extremities of iso-V */
/*             = 0, calculate the extremities of iso-V */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-V */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-V */
/*     IORDRV: Order of constraint imposed at the extremities of iso-U */
/*             = 0, calculate the extremities of iso-U */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-U */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-U */
/*   CONTR1: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V0) and its derivatives. */
/*   CONTR2: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V0) and its derivatives. */
/*   CONTR3: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V1) and its derivatives. */
/*   CONTR4: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V1) and its derivatives. */
/*     SOSOTB: Preinitialized table (input/output argument). */
/*     DISOTB: Preinitialized table (input/output argument). */
/*     SODITB: Preinitialized table (input/output argument). */
/*     DIDITB: Preinitialized table (input/output argument) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     FPNTBU: Auxiliary table. */
/*     FPNTBV: Auxiliary table. */
/*     UHERMT: Table of 2*(IORDRU+1) coeff. of 2*(IORDRU+1) polynoms of Hermite. */
/*     VHERMT: Table of 2*(IORDRV+1) coeff. of 2*(IORDRV+1) polynoms of Hermite. */
/*   SOSOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* ------------------- Discretisation of Hermite polynoms ----------- 
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

/* ---- The discretizations of polynoms of constraints are subtracted ---- 
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

/* ------------ Case when the discretization is done only on the roots  
----------- */
/* ----------   of Legendre polynom of uneven degree, 0 is root 
----------- */

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
  integer c__1 = 1;
  /* System generated locals */
  integer sotbv1_dim1, sotbv1_dim2, sotbv1_offset, sotbv2_dim1, sotbv2_dim2,
  sotbv2_offset, ditbv1_dim1, ditbv1_dim2, ditbv1_offset, 
  ditbv2_dim1, ditbv2_dim2, ditbv2_offset, fpntab_dim1, 
  fpntab_offset, vhermt_dim1, vhermt_offset, sosotb_dim1, 
  sosotb_dim2, sosotb_offset, diditb_dim1, diditb_dim2, 
  diditb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  disotb_dim1, disotb_dim2, disotb_offset, i__1, i__2, i__3, i__4;

  /* Local variables */
  integer ncfhv, nuroo, nvroo, ii, nd, jj, kk, ibb, jjm, jjp;
  doublereal bid1, bid2, bid3, bid4;

/* ********************************************************************** 
*/
/*     FUNCTION : */
/*     ---------- */
/*     Discretisation on the parameters of polynoms of interpolation */
/*     of constraints on 2 borders iso-V of order IORDRV. */


/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */



/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Dimension of the space. */
/*     NBPNTU: Nb of INTERNAL parameters of discretisation by U. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     UROOTL: Table of parameters of discretisation ON (-1,1) by U. 
*/
/*     NBPNTV: Nb of INTERNAL  parameters of discretisation by V. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     VROOTL: Table of discretization parameters on (-1,1) by V. */
/*     IORDRV: Order of constraint imposed at the extremities of iso-V */
/*             = 0, calculate the extremities of iso-V */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-V */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-V */
/*     SOTBV1: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V0. */
/*     SOTBV2: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V1. */
/*     DITBV1: Table of NBPNTV/2 differences of 2 index points */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V0. */
/*     DITBV2: Table of NBPNTV/2 differences of 2 index points */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V1. */
/*     SOSOTB: Preinitialized table (input/output argument). */
/*     DISOTB: Preinitialized table (input/output argument). */
/*     SODITB: Preinitialized table (input/output argument). */
/*     DIDITB: Preinitialized table (input/output argument) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     FPNTAB: Auxiliary table. */
/*     VHERMT: Table of 2*(IORDRV+1) coeff. of 2*(IORDRV+1) polynoms of Hermite. */
/*   SOSOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* ------------------- Discretization of Hermit polynoms ----------- 
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

/* ---- The discretizations of polynoms of constraints are subtracted ---- 
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

/* ------------ Case when the discretization is done only on the roots  */
/* ----------   of Legendre polynom of uneven degree, 0 is root */


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
  integer c__1 = 1;

   /* System generated locals */
    integer sotbu1_dim1, sotbu1_dim2, sotbu1_offset, sotbu2_dim1, sotbu2_dim2,
	     sotbu2_offset, ditbu1_dim1, ditbu1_dim2, ditbu1_offset, 
	    ditbu2_dim1, ditbu2_dim2, ditbu2_offset, fpntab_dim1, 
	    fpntab_offset, uhermt_dim1, uhermt_offset, sosotb_dim1, 
	    sosotb_dim2, sosotb_offset, diditb_dim1, diditb_dim2, 
	    diditb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
	    disotb_dim1, disotb_dim2, disotb_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer ncfhu, nuroo, nvroo, ii, nd, jj, kk, ibb, kkm, kkp;
    doublereal bid1, bid2, bid3, bid4;

/* ********************************************************************** 
*/
/*     FUNCTION : */
/*     ---------- */
/*     Discretisation on the parameters of polynoms of interpolation */
/*     of constraints on 2 borders iso-U of order IORDRU. */


/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Dimension of the space. */
/*     NBPNTU: Nb of INTERNAL parameters of discretisation by U. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     UROOTL: Table of parameters of discretisation ON (-1,1) by U. 
*/
/*     NBPNTV: Nb of INTERNAL  parameters of discretisation by V. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     IORDRV: Order of constraint imposed at the extremities of iso-V */
/*             = 0, calculate the extremities of iso-V */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-V */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-V */
/*     SOTBU1: Table of NBPNTU/2 sums of 2 index points  */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V0. */
/*     SOTBU2: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V1. */
/*     DITBU1: Table of NBPNTU/2 differences of 2 index points */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V0. */
/*     DITBU2: Table of NBPNTU/2 differences of 2 index points */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V1. */
/*     SOSOTB: Preinitialized table (input/output argument). */
/*     DISOTB: Preinitialized table (input/output argument). */
/*     SODITB: Preinitialized table (input/output argument). */
/*     DIDITB: Preinitialized table (input/output argument) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     FPNTAB: Auxiliary table. */
/*     UHERMT: Table of 2*(IORDRU+1) coeff. of 2*(IORDRU+1) polynoms of Hermite. */
/*   SOSOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* $    HISTORIQUE DES MODIFICATIONS   : */
/*     -------------------------------- */
/*     08-08-1991: RBD; Creation. */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* ------------------- Discretization of polynoms of Hermit ----------- 
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

/* ---- The discretizations of polynoms of constraints are subtracted ---- 
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

/* ------------ Case when the discretization is done only on the roots  */
/* ----------   of Legendre polynom of uneven degree, 0 is root */



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
  integer c__8 = 8;

    /* System generated locals */
    integer contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
	     contr2_offset, contr3_dim1, contr3_dim2, contr3_offset, 
	    contr4_dim1, contr4_dim2, contr4_offset, sosotb_dim1, sosotb_dim2,
	     sosotb_offset, diditb_dim1, diditb_dim2, diditb_offset, 
	    soditb_dim1, soditb_dim2, soditb_offset, disotb_dim1, disotb_dim2,
	     disotb_offset;

    /* Local variables */
    integer ilong;
    intptr_t iofwr;
    doublereal* wrkar = 0;
    integer iszwr;
    integer ibb, ier = 0;
    integer isz1, isz2, isz3, isz4;
    intptr_t ipt1, ipt2, ipt3, ipt4;




/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Discretisation on the parameters of polynomes of interpolation */
/*     of constraints of order IORDRE. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

//*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDIMEN: Dimension of the space. */
/*     NBPNTU: Nb of INTERNAL parameters of discretisation by U. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     UROOTL: Table of parameters of discretisation ON (-1,1) by U. 
*/
/*     NBPNTV: Nb of INTERNAL  parameters of discretisation by V. */
/*             This is also the nb of root of Legendre polynom where discretization is done. */
/*     VROOTL: Table of parameters of discretisation ON (-1,1) by V.*/ 

/*     IORDRV: Order of constraint imposed at the extremities of iso-U */
/*             = 0, calculate the extremities of iso-U */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-U */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-U */
/*     IORDRU: Order of constraint imposed at the extremities of iso-V */
/*             = 0, calculate the extremities of iso-V */
/*             = 1, calculate, additionally, the 1st derivative in the direction of iso-V */
/*             = 2, calculate, additionally, the 2nd derivative in the direction of iso-V */
/*   CONTR1: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V0) and its derivatives. */
/*   CONTR2: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V0) and its derivatives. */
/*   CONTR3: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U0,V1) and its derivatives. */
/*   CONTR4: Contains, if IORDRU and IORDRV>=0, the values at the */
/*           extremities of F(U1,V1) and its derivatives. */
/*     SOTBU1: Table of NBPNTU/2 sums of 2 index points  */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V0. */
/*     SOTBU2: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V1. */
/*     DITBU1: Table of NBPNTU/2 differences of 2 index points */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V0. */
/*     DITBU2: Table of NBPNTU/2 differences of 2 index points */
/*             NBPNTU-II+1 and II, for II = 1, NBPNTU/2 on iso-V1. */
/*     SOTBV1: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V0. */
/*     SOTBV2: Table of NBPNTV/2 sums of 2 index points  */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V1. */
/*     DITBV1: Table of NBPNTV/2 differences of 2 index points */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V0. */
/*     DITBV2: Table of NBPNTV/2 differences of 2 index points */
/*             NBPNTV-II+1 and II, for II = 1, NBPNTV/2 on iso-V1. */
/*     SOSOTB: Preinitialized table (input/output argument). */
/*     DISOTB: Preinitialized table (input/output argument). */
/*     SODITB: Preinitialized table (input/output argument). */
/*     DIDITB: Preinitialized table (input/output argument) */

/*     ARGUMENTS DE SORTIE : */
/*     ------------------- */
/*   SOSOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) + C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms of constraints are added */
/*           C(ui,vj) + C(ui,-vj) - C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) + C(-ui,vj) - C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms of constraints are added */
/*           C(ui,vj) - C(ui,-vj) - C(-ui,vj) + C(-ui,-vj) */
/*           with ui and vj positive roots of the polynom of Legendre */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   IERCOD: = 0, OK, */
/*           = 1, Value or IORDRV or IORDRU is out of allowed values. */
/*           =13, Pb of dynamic allocation. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED  : */
/*     -------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */

/* > */
/* ********************************************************************** 
*/

/*   The name of the routine */


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
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;

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

/* ------------------------- Set to zero -------------------------------- 
*/

    ilong = (*nbpntu / 2 + 1) * (*nbpntv / 2 + 1) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&ilong, &sosotb[sosotb_offset]);
    AdvApp2Var_SysBase::mvriraz_(&ilong, &diditb[diditb_offset]);
    ilong = *nbpntu / 2 * (*nbpntv / 2) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&ilong, &soditb[soditb_offset]);
    AdvApp2Var_SysBase::mvriraz_(&ilong, &disotb[disotb_offset]);
    if (*iordru == -1 && *iordrv == -1) {
	goto L9999;
    }



    isz1 = ((*iordru + 1) << 2) * (*iordru + 1);
    isz2 = ((*iordrv + 1) << 2) * (*iordrv + 1);
    isz3 = ((*iordru + 1) << 1) * *nbpntu;
    isz4 = ((*iordrv + 1) << 1) * *nbpntv;
    iszwr = isz1 + isz2 + isz3 + isz4;
    anAdvApp2Var_SysBase.mcrrqst_(&c__8, &iszwr, wrkar, &iofwr, &ier);
    if (ier > 0) {
	goto L9013;
    }
    ipt1 = iofwr;
    ipt2 = ipt1 + isz1;
    ipt3 = ipt2 + isz2;
    ipt4 = ipt3 + isz3;

    if (*iordru >= 0 && *iordru <= 2) {

/* --- Return 2*(IORDRU+1) coeff of 2*(IORDRU+1) polynoms of Hermite 
--- */

	AdvApp2Var_ApproxF2var::mma1her_(iordru, &wrkar[ipt1], iercod);
	if (*iercod > 0) {
	    goto L9100;
	}

/* ---- Subract discretizations of polynoms of constraints 
---- */

	mma2cd3_(ndimen, nbpntu, &urootl[1], nbpntv, iordru, &sotbu1[1], &
		sotbu2[1], &ditbu1[1], &ditbu2[1], &wrkar[ipt3], &wrkar[ipt1],
		 &sosotb[sosotb_offset], &soditb[soditb_offset], &disotb[
		disotb_offset], &diditb[diditb_offset]);
    }

    if (*iordrv >= 0 && *iordrv <= 2) {

/* --- Return 2*(IORDRV+1) coeff of 2*(IORDRV+1) polynoms of Hermite 
--- */

	AdvApp2Var_ApproxF2var::mma1her_(iordrv, &wrkar[ipt2], iercod);
	if (*iercod > 0) {
	    goto L9100;
	}

/* ---- Subtract discretisations of polynoms of constraint 
---- */

	mma2cd2_(ndimen, nbpntu, nbpntv, &vrootl[1], iordrv, &sotbv1[1], &
		sotbv2[1], &ditbv1[1], &ditbv2[1], &wrkar[ipt4], &wrkar[ipt2],
		 &sosotb[sosotb_offset], &soditb[soditb_offset], &disotb[
		disotb_offset], &diditb[diditb_offset]);
    }

/* --------------- Subtract constraints of corners ---------------- 
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
/* --> IORDRE is not within the autorised diapason. */
L9100:
    *iercod = 1;
    goto L9999;
/* --> PB of dynamic allocation. */
L9013:
    *iercod = 13;
    goto L9999;

L9999:
    if (iofwr != 0) {
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, &iszwr, wrkar, &iofwr, &ier);
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
  integer c__8 = 8;
  
    /* System generated locals */
    integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
	     disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
	    diditb_dim1, diditb_dim2, diditb_offset, patjac_dim1, patjac_dim2,
	     patjac_offset;

    /* Local variables */
    logical ldbg;
    intptr_t iofwr;
    doublereal* wrkar = 0;
    integer iszwr;
    integer ier;
    integer isz1, isz2, isz3, isz4, isz5, isz6, isz7;
    intptr_t ipt1, ipt2, ipt3, ipt4, ipt5, ipt6, ipt7;



/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculation of coefficients of polynomial approximation of degree */
/*     (NDJACU,NDJACV) of a function F(u,v), starting from its */
/*     discretization on roots of Legendre polynom of degree  */
/*     NBPNTU by U and NBPNTV by V. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::FONCTION&,APPROXIMATION,&POLYNOME,&ERREUR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NUMDEC: Indicates if it is POSSIBLE to cut function F(u,v). */
/*           = 5, It is POSSIBLE to cut by U or by V or in both directions simultaneously. */
/*           = 4, It is POSSIBLE to cut by U or by V BUT NOT in both  */
/*                directions simultaneously (cutting by V is preferable). */
/*           = 3, It is POSSIBLE to cut by U or by V BUT NOT in both */
/*                directions simultaneously (cutting by U is preferable). */
/*           = 2, It is POSSIBLE to cut only by V (i.e. insert parameter */
/*                of cutting Vj). */
/*           = 1, It is POSSIBLE to cut only by U (i.e. insert parameter */
/*                of cutting Ui). */
/*           = 0, It is not POSSIBLE to cut anything */
/*   NDIMEN: Dimension of the space. */
/*   NBSESP: Nb of independent sub-spaces on which the errors are calculated. */
/*   NDIMSE: Table of dimensions of each of sub-spaces. */
/*   NDMINU: Minimum degree by U to be preserved for the approximation. */
/*   NDMINV: Minimum degree by V to be preserved for the approximation. */
/*   NDGULI: Limit of nb of coefficients by U of the solution. */
/*   NDGVLI: Limit of nb of coefficients by V of the solution. */
/*   NDJACU: Max degree of the polynom of approximation by U. */
/*           The representation in the orthogonal base starts from degree */
/*           0 to degree NDJACU-2*(IORDRU+1). The polynomial base is the base of  */
/*           Jacobi of order -1 (Legendre), 0, 1 or 2. */
/*           It is required that 2*IORDRU+1 <= NDMINU <= NDGULI < NDJACU */
/*   NDJACV: Max degree of the polynom of approximation by V. */
/*           The representation in the orthogonal base starts from degree */
/*           0 to degree NDJACV-2*(IORDRV+1). The polynomial base is */
/*           the base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*           It is required that 2*IORDRV+1 <= NDMINV <= NDGVLI < NDJACV */
/*   IORDRU: Order of the Jacobi base (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints C0, C1 or C2. */
/*   IORDRV: Order of the Jacobi base (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints C0, C1 or C2. */
/*   NBPNTU: Degree of Legendre polynom on  the roots which of are */
/*           calculated the coefficients of integration by u */
/*           by Gauss method. It is required that NBPNTU = 30, 40, */
/*           50 or 61 and NDJACU-2*(IORDRU+1) < NBPNTU. */
/*   NBPNTV: Degree of Legendre polynom on  the roots which of are */
/*           calculated the coefficients of integration by u */
/*           by Gauss method. It is required that NBPNTV = 30, 40, */
/*           50 or 61 and NDJACV-2*(IORDRV+1) < NBPNTV. */
/*   EPSAPR: Table of NBSESP tolerances imposed on each sub-spaces. */
/*   SOSOTB: Table of F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           with ui and vj - positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Additionally, */
/*           table SOSOTB(0,j) contains F(0,vj) + F(0,-vj), */
/*           table SOSOTB(i,0) contains F(ui,0) + F(-ui,0) and */
/*           SOSOTB(0,0) contains F(0,0). */
/*   DISOTB: Table of F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table of F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table of F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Additionally, */
/*           table DIDITB(0,j) contains F(0,vj) - F(0,-vj), */
/*           and table DIDITB(i,0) contains F(ui,0) - F(-ui,0). */

/*   OUTPUT ARGUMENTS  */
/*     --------------- */
/*   PATJAC: Table of coefficients of polynom P(u,v) of approximation */
/*           of F(u,v) with eventually taking into account of */
/*           constraints. P(u,v) is of degree (NDJACU,NDJACV). */
/*           This table contains other coeff if ITYDEC = 0. */
/*   ERRMAX: For 1<=i<=NBSESP, ERRMAX(i) contains max errors */
/*           on each of sub-spaces SI ITYDEC = 0. */
/*   ERRMOY: Contains average errors for each of NBSESP sub-spaces SI ITYDEC = 0. */
/*   NDEGPU: Degree by U for square PATJAC. Valable if ITYDEC=0. */
/*   NDEGPV: Degree by V for square PATJAC. Valable if ITYDEC=0. */
/*   ITYDEC: Shows if it is NECESSARY to cut again function F(u,v). */
/*           = 0, it is not NECESSARY to cut anything, PATJAC is OK. */
/*           = 1, it is NECESSARY to cut only by U (i.e. insert parameter of cutting Ui). */
/*           = 2, it is NECESSARY to cut only by V (i.e. insert parameter of cutting Vj). */
/*           = 3, it is NECESSARY to cut both by U AND by V. */
/*   IERCOD: Error code. */
/*           =  0, Everything is OK. */
/*           = -1, There is the best possible solution, but the */
/*                 user tolerance is not satisfactory (3*only) */
/*           =  1, Incoherent entries. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */

/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;
    anAdvApp2Var_SysBase.mcrrqst_(&c__8, &iszwr, wrkar, &iofwr, &ier);
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

/* ----------------- Return Gauss coefficients of integration ---------------- 
*/

    AdvApp2Var_ApproxF2var::mmapptt_(ndjacu, nbpntu, iordru, &wrkar[ipt1], iercod);
    if (*iercod > 0) {
	goto L9999;
    }
    AdvApp2Var_ApproxF2var::mmapptt_(ndjacv, nbpntv, iordrv, &wrkar[ipt2], iercod);
    if (*iercod > 0) {
	goto L9999;
    }

/* ------------------- Return max polynoms of  Jacobi ------------ 
*/

    AdvApp2Var_ApproxF2var::mma2jmx_(ndjacu, iordru, &wrkar[ipt5]);
    AdvApp2Var_ApproxF2var::mma2jmx_(ndjacv, iordrv, &wrkar[ipt6]);

/* ------ Calculate the coefficients and their contribution to the error ---- 
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
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, &iszwr, wrkar, &iofwr, &ier);
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
  logical ldbg;
  integer idim, igsu, minu, minv, maxu, maxv, igsv;
  doublereal vaux[3];
  integer i2rdu, i2rdv, ndses, nd, ii, jj, kk, nu, nv;
  doublereal zu, zv;
  integer nu1, nv1;

/* ********************************************************************** 
*/
/*     FUNCTION : */
/*     ---------- */
/*     Calculation of coefficients of polynomial approximation of degree */
/*     (NDJACU,NDJACV) of a function F(u,v), starting from its */
/*     discretization on roots of Legendre polynom of degree  */
/*     NBPNTU by U and NBPNTV by V. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::FONCTION&,APPROXIMATION,&COEFFICIENT,&POLYNOME */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NUMDEC: Indicates if it is POSSIBLE to cut function F(u,v). */
/*           = 5, It is POSSIBLE to cut by U or by V or in both directions simultaneously. */
/*           = 4, It is POSSIBLE to cut by U or by V BUT NOT in both  */
/*                directions simultaneously (cutting by V is preferable). */
/*           = 3, It is POSSIBLE to cut by U or by V BUT NOT in both */
/*                directions simultaneously (cutting by U is preferable). */
/*           = 2, It is POSSIBLE to cut only by V (i.e. insert parameter */
/*                of cutting Vj). */
/*           = 1, It is POSSIBLE to cut only by U (i.e. insert parameter */
/*                of cutting Ui). */
/*           = 0, It is not POSSIBLE to cut anything */
/*   NDIMEN: Total dimension of the space. */
/*   NBSESP: Nb of independent sub-spaces on which the errors are calculated. */
/*   NDIMSE: Table of dimensions of each of sub-spaces. */
/*   NDMINU: Minimum degree by U to be preserved for the approximation. */
/*   NDMINV: Minimum degree by V to be preserved for the approximation. */
/*   NDGULI: Limit of nb of coefficients by U of the solution. */
/*   NDGVLI: Limit of nb of coefficients by V of the solution. */
/*   NDJACU: Max degree of the polynom of approximation by U. */
/*           The representation in the orthogonal base starts from degree */
/*           0 to degree NDJACU-2*(IORDRU+1). The polynomial base is the base of  */
/*           Jacobi of order -1 (Legendre), 0, 1 or 2. */
/*           It is required that 2*IORDRU+1 <= NDMINU <= NDGULI < NDJACU */
/*   NDJACV: Max degree of the polynom of approximation by V. */
/*           The representation in the orthogonal base starts from degree */
/*           0 to degree NDJACV-2*(IORDRV+1). The polynomial base is */
/*           the base of Jacobi of order -1 (Legendre), 0, 1 or 2 */
/*           It is required that 2*IORDRV+1 <= NDMINV <= NDGVLI < NDJACV */
/*   IORDRU: Order of the Jacobi base (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints C0, C1 or C2. */
/*   IORDRV: Order of the Jacobi base (-1,0,1 or 2) by U. Corresponds */
/*           to the step of constraints C0, C1 or C2. */
/*   NBPNTU: Degree of Legendre polynom on  the roots which of are */
/*           calculated the coefficients of integration by u */
/*           by Gauss method. It is required that NBPNTU = 30, 40, */
/*           50 or 61 and NDJACU-2*(IORDRU+1) < NBPNTU. */
/*   NBPNTV: Degree of Legendre polynom on  the roots which of are */
/*           calculated the coefficients of integration by u */
/*           by Gauss method. It is required that NBPNTV = 30, 40, */
/*           50 or 61 and NDJACV-2*(IORDRV+1) < NBPNTV. */
/*   EPSAPR: Table of NBSESP tolerances imposed on each sub-spaces. */
/*   SOSOTB: Table of F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           with ui and vj - positive roots of the Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Additionally, */
/*           table SOSOTB(0,j) contains F(0,vj) + F(0,-vj), */
/*           table SOSOTB(i,0) contains F(ui,0) + F(-ui,0) and */
/*           SOSOTB(0,0) contains F(0,0). */
/*   DISOTB: Table of F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table of F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table of F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Additionally, */
/*           table DIDITB(0,j) contains F(0,vj) - F(0,-vj), */
/*           and table DIDITB(i,0) contains F(ui,0) - F(-ui,0). */
/*   GSSUTB: Table of coefficients of integration by Gauss method */
/*           by U: i varies from 0 to NBPNTU/2 and k varies from 0 to */
/*           NDJACU-2*(IORDRU+1). */
/*   GSSVTB: Table of coefficients of integration by Gauss method */
/*           by V: i varies from 0 to NBPNTV/2 and k varies from 0 to */
/*           NDJACV-2*(IORDRV+1). */
/*   XMAXJU: Maximum value of Jacobi polynoms of order IORDRU, */
/*           from degree 0 to degree NDJACU - 2*(IORDRU+1) */
/*   XMAXJV: Maximum value of Jacobi polynoms of order IORDRV, */
/*           from degree 0 to degree NDJACV - 2*(IORDRV+1) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   VECERR: Auxiliary table. */
/*   CHPAIR: Auxiliary table of terms connected to degree NDJACU by U */
/*           to calculate the coeff. of approximation of EVEN degree by V. */
/*   CHIMPR: Auxiliary table of terms connected to degree NDJACU by U */
/*           to calculate the coeff. of approximation of UNEVEN degree by V. */
/*   PATJAC: Table of coefficients of polynom P(u,v) of approximation */
/*           of F(u,v) with eventually taking into account of */
/*           constraints. P(u,v) is of degree (NDJACU,NDJACV). */
/*           This table contains other coeff if ITYDEC = 0. */
/*   ERRMAX: For 1<=i<=NBSESP, ERRMAX(i) contains max errors */
/*           on each of sub-spaces SI ITYDEC = 0. */
/*   ERRMOY: Contains average errors for each of NBSESP sub-spaces SI ITYDEC = 0. */
/*   NDEGPU: Degree by U for square PATJAC. Valable if ITYDEC=0. */
/*   NDEGPV: Degree by V for square PATJAC. Valable if ITYDEC=0. */
/*   ITYDEC: Shows if it is NECESSARY to cut again function F(u,v). */
/*           = 0, it is not NECESSARY to cut anything, PATJAC is OK. */
/*           = 1, it is NECESSARY to cut only by U (i.e. insert parameter of cutting Ui). */
/*           = 2, it is NECESSARY to cut only by V (i.e. insert parameter of cutting Vj). */
/*           = 3, it is NECESSARY to cut both by U AND by V. */
/*   IERCOD: Error code. */
/*           =  0, Everything is OK. */
/*           = -1, There is the best possible solution, but the */
/*                 user tolerance is not satisfactory (3*only) */
/*           =  1, Incoherent entries. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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
/* --> A priori everything is OK */
    *iercod = 0;
/* --> test of inputs */
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
/* --> A priori, no cuts to be done */
    *itydec = 0;
/* --> Min. degrees to return: NDMINU,NDMINV */
    *ndegpu = *ndminu;
    *ndegpv = *ndminv;
/* --> For the moment, max errors are null */
    AdvApp2Var_SysBase::mvriraz_(nbsesp, &errmax[1]);
    nd = *ndimen << 2;
    AdvApp2Var_SysBase::mvriraz_(&nd, &vecerr[vecerr_offset]);
/* --> and the square, too. */
    nd = (*ndjacu + 1) * (*ndjacv + 1) * *ndimen;
    AdvApp2Var_SysBase::mvriraz_(&nd, &patjac[patjac_offset]);

    i2rdu = (*iordru + 1) << 1;
    i2rdv = (*iordrv + 1) << 1;

/* ********************************************************************** 
*/
/* -------------------- HERE IT IS POSSIBLE TO CUT ---------------------- 
*/
/* ********************************************************************** 
*/

    if (*numdec > 0 && *numdec <= 5) {

/* ******************************************************************
**** */
/* ---------------------- Calculate coeff of zone 4 -------------
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

/* ---------------- Calculate the terms connected to degree by U ---------
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

/* ------------------- Calculate the coefficients of PATJAC ------------
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

/* ----- Contribution of calculated terms to the approximation error  */
/* for terms (I,J) with MINU <= I <= MAXU, J fixe. */

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
/* ---------------------- Calculate the coeff of zone 2 -------------
---- */

	minu = (*iordru + 1) << 1;
	maxu = *ndguli;
	minv = *ndgvli + 1;
	maxv = *ndjacv;

/* --> If zone 2 is empty, pass to zone 3. */
/*    VECERR(ND,2) was already set to zero. */
	if (minu > maxu) {
	    goto L300;
	}

/* ---------------- Calculate the terms connected to degree by U ------------
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

/* ------------------- Calculate the coefficients of PATJAC ------------
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

/* -----Contribution of calculated terms to the approximation error  */
/* for terms (I,J) with MINU <= I <= MAXU, MINV <= J <= MAXV */

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
/* ---------------------- Calculation of coeff of zone 3 -------------
---- */

L300:
	minu = *ndguli + 1;
	maxu = *ndjacu;
	minv = (*iordrv + 1) << 1;
	maxv = *ndgvli;

/* -> If zone 3 is empty, pass to the test of cutting. */
/*    VECERR(ND,3) was already set to zero */
	if (minv > maxv) {
	    goto L400;
	}

/* ----------- The terms connected to the degree by U are already calculated -----
---- */
/* ------------------- Calculation of coefficients of PATJAC ------------
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

/* ----- Contribution of calculated terms to the approximation error */
/* for terms (I,J) with MINU <= I <= MAXU, MINV <= J <= MAXV. */

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
/* --------------------------- Tests of cutting ---------------------
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
/* --- OK, the square is valid, the coeff of zone 1 are calculated
---- */

	minu = (*iordru + 1) << 1;
	maxu = *ndguli;
	minv = (*iordrv + 1) << 1;
	maxv = *ndgvli;

/* --> If zone 1 is empty, pass to the calculation of Max and Average error. */
	if (minu > maxu || minv > maxv) {
	    goto L600;
	}

/* ----------- The terms connected to degree by U are already calculated -----
---- */
/* ------------------- Calculate the coefficients of PATJAC ------------
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

/* --------------- Now the degree is maximally lowered --------
---- */

L600:
/* Computing MAX */
	i__1 = 1, i__2 = (*iordru << 1) + 1, i__1 = advapp_max(i__1,i__2);
	minu = advapp_max(i__1,*ndminu);
	maxu = *ndguli;
/* Computing MAX */
	i__1 = 1, i__2 = (*iordrv << 1) + 1, i__1 = advapp_max(i__1,i__2);
	minv = advapp_max(i__1,*ndminv);
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

/* --> Calculate the average error. */
	    mma2moy_(ndjacu, ndjacv, &ndses, &nu1, ndjacu, &nv1, ndjacv, 
		    iordru, iordrv, &patjac[idim * patjac_dim2 * patjac_dim1],
		     &errmoy[nd]);

/* --> Set to 0.D0 the rejected coeffs. */
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

/* --> Return the nb of coeffs of approximation. */
	    *ndegpu = advapp_max(*ndegpu,nu);
	    *ndegpv = advapp_max(*ndegpv,nv);
	    idim += ndses;
/* L610: */
	}

/* ******************************************************************
**** */
/* -------------------- IT IS NOT POSSIBLE TO CUT -------------------
---- */
/* ******************************************************************
**** */

    } else {
	minu = (*iordru + 1) << 1;
	maxu = *ndjacu;
	minv = (*iordrv + 1) << 1;
	maxv = *ndjacv;

/* ---------------- Calculate the terms connected to the degree by U ------------
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

/* ---------------------- Calculate all coefficients -------
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

/* ----- Contribution of calculated terms to the approximation error */
/* for  terms (I,J) with MINU <= I <= MAXU, MINV <= J <= MAXV */

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

/* ---------------------------- IF ERRMAX > EPSAPR, stop --------
-------- */

	    if (errmax[nd] > epsapr[nd]) {
		*iercod = -1;
		nu = *ndguli;
		nv = *ndgvli;

/* ------------- Otherwise, try to remove again the coeff 
------------ */

	    } else {
/* Computing MAX */
		i__2 = 1, i__3 = (*iordru << 1) + 1, i__2 = advapp_max(i__2,i__3);
		minu = advapp_max(i__2,*ndminu);
		maxu = *ndguli;
/* Computing MAX */
		i__2 = 1, i__3 = (*iordrv << 1) + 1, i__2 = advapp_max(i__2,i__3);
		minv = advapp_max(i__2,*ndminv);
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

/* --------------------- Calculate the average error -------------
-------- */

	    nu1 = nu + 1;
	    nv1 = nv + 1;
	    mma2moy_(ndjacu, ndjacv, &ndses, &nu1, ndjacu, &nv1, ndjacv, 
		    iordru, iordrv, &patjac[idim * patjac_dim2 * patjac_dim1],
		     &errmoy[nd]);

/* --------------------- Set to 0.D0 the rejected coeffs ----------
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

/* --------------- Return the nb of coeff of approximation ---
-------- */

	    *ndegpu = advapp_max(*ndegpu,nu);
	    *ndegpv = advapp_max(*ndegpv,nv);
	    idim += ndses;
/* L730: */
	}
    }

    goto L9999;

/* ------------------------------ The end ------------------------------- 
*/
/* --> Error in inputs */
L9001:
    *iercod = 1;
    goto L9999;

/* --------- Management of cuts, it is required 0 < NUMDEC <= 5 ------- 
*/

/* --> Here it is possible and necessary to cut, choose by U if it is possible */
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
/* --> Here it is possible and necessary to cut, choose by U if it is possible */
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
/* --> Here it is possible and necessary to cut, choose by 4 if it is possible */
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
  logical ldbg;
  integer nptu2, nptv2, ii, jj;
  doublereal bid0, bid1, bid2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculate the terms connected to degree NDUJAC by U of the polynomial approximation */
/*     of function F(u,v), starting from its discretisation */
/*     on the roots of Legendre polynom of degree */
/*     NBPNTU by U and NBPNTV by V. */

/*     KEYWORDS : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     INPUT ARGUMENTSE : */
/*     ------------------ */
/*   NDUJAC: Fixed degree by U for which the terms */
/*           allowing to obtain the Legendre or Jacobi coeff*/
/*           of even or uneven degree by V are calculated. */
/*   NBPNTU: Degree of Legendre polynom on the roots which of */
/*           the coefficients of integration by U are calculated */
/*           by Gauss method. It is required that NBPNTU = 30, 40, 50 or 61. */
/*   NBPNTV: Degree of Legendre polynom on the roots which of */
/*           the coefficients of integration by V are calculated */
/*           by Gauss method. It is required that NBPNTV = 30, 40, 50 or 61. */
/*   SOSOTB: Table of F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Moreover, */
/*           table SOSOTB(0,j) contains F(0,vj) + F(0,-vj), */
/*           table SOSOTB(i,0) contains F(ui,0) + F(-ui,0) and */
/*           SOSOTB(0,0) contains F(0,0). */
/*   DISOTB: Table of F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table of F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table of F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           avec ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. Moreover, */
/*           table DIDITB(0,j) contains F(0,vj) - F(0,-vj), */
/*           and table DIDITB(i,0) contains F(ui,0) - F(-ui,0). */
/*   GSSUTB: Table of coefficients of integration by Gauss method */
/*           Gauss by U for fixed NDUJAC : i varies from 0 to NBPNTU/2. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   CHPAIR: Table of terms connected to degree NDUJAC by U to calculate the */
/*           coeff. of the approximation of EVEN degree by V. */
/*   CHIMPR: Table of terms connected to degree NDUJAC by U to calculate */
/*           the coeff. of approximation of UNEVEN degree by V. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */


/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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
/*                    CALCULATE COEFFICIENTS BY U */

/* ----------------- Calculate  coefficients of even degree -------------- 
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

/* --------------- Calculate coefficients of uneven degree ----------
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

/* ------- Add terms connected to the supplementary root (0.D0) ------ */
/* ----------- of Legendre polynom of uneven degree NBPNTU ----------- 
*/
/* --> Only even NDUJAC terms are modified as GSSUTB(0) = 0 */
/*     when NDUJAC is uneven. */

    if (*nbpntu % 2 != 0 && *ndujac % 2 == 0) {
	bid0 = gssutb[0];
	i__1 = nptv2;
	for (jj = 1; jj <= i__1; ++jj) {
	    chpair[jj] += sosotb[jj * sosotb_dim1] * bid0;
	    chimpr[jj] += diditb[jj * diditb_dim1] * bid0;
/* L300: */
	}
    }

/* ------ Calculate the terms connected to supplementary roots (0.D0) ------ 
*/
/* ----------- of Legendre polynom of uneven degree NBPNTV ----------- 
*/

    if (*nbpntv % 2 != 0) {
/* --> Only CHPAIR terms are calculated as GSSVTB(0,IH-IDEBV)=0 
*/
/*    when IH is uneven (see MMA2CFV). */

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
  logical ldbg;
  integer nptv2, ii, jj;
  doublereal bid1;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculate the coefficients of polynomial approximation of F(u,v) */
/*     of degree NDVJAC by V and of degree by U varying from MINDGU to MAXDGU. 
*/

/*     Keywords : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     INPUT ARGUMENTS : */
/*     ------------------ */

/*   NDVJAC: Degree of the polynom of approximation by V. */
/*           The representation in the orthogonal base starts from degre 0. */
	     /* The polynomial base is the base of Jacobi of order -1 */
/*           (Legendre), 0, 1 or 2 */
/*   MINDGU: Degree minimum by U of coeff. to calculate. */
/*   MAXDGU: Degree maximum by U of coeff. to calculate. */
/*   NBPNTV: Degree of the Legendre polynom on the roots which of */
/*           the coefficients of integration by V are calculated */
/*           by Gauss method. It is reqired that NBPNTV = 30, 40, 50 or 61 and NDVJAC < NBPNTV. */
/*   GSSVTB: Table of coefficients of integration by Gauss method */
/*           by V for NDVJAC fixed: j varies from 0 to NBPNTV/2. */
/*   CHPAIR: Table of terms connected to degrees from MINDGU to MAXDGU by U to */
/*           calculate the coeff. of approximation of EVEN degree NDVJAC by V. */
/*   CHIMPR: Table of terms connected to degrees from MINDGU to MAXDGU by U to */
/*           calculate the coeff. of approximation of UNEVEN degree NDVJAC by V. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   PATJAC: Table of coefficients by U of the polynom of approximation */
/*           P(u,v) of degree MINDGU to MAXDGU by U and NDVJAC by V. */

/*     COMMONS USED : */
/*     -------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */
/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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

/* --------- Calculate the coefficients for even degree NDVJAC ---------- 
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

/* -------- Calculate the coefficients for uneven degree NDVJAC -----
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

/* ------- Add terms connected to the supplementary root (0.D0) ----- */
/* --------of the Legendre polynom of uneven degree  NBPNTV --------- */

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
				     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
  logical ldbg;
  integer ibid1, ibid2, iuouv, nd;
  integer isz1, isz2;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Discretisation of function F(u,v) on the roots of Legendre polynoms. */

/*     KEYWORDS : */
/*     ----------- */
/*     FONCTION&,DISCRETISATION,&POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*   NDIMEN: Dimension of the space. */
/*   UINTFN: Limits of the interval of definition by u of the function */
/*           to be processed: (UINTFN(1),UINTFN(2)). */
/*   VINTFN: Limits of the interval of definition by v of the function */
/*           to be processed: (VINTFN(1),VINTFN(2)). */
/*   FONCNP: The NAME of the non-polynomial function to be processed. */
/*   NBPNTU: The degree of Legendre polynom on the roots which of */
/*           FONCNP is discretized by u. */
/*   NBPNTV: The degree of Legendre polynom on the roots which of  */
/*           FONCNP is discretized by v. */
/*   UROOTB: Table of STRICTLY POSITIVE roots of the polynom */
/*           of Legendre of degree NBPNTU defined on (-1,1). */
/*   VROOTB: Table of STRICTLY POSITIVE roots of the polynom */
/*           of Legendre of degree NBPNTV defined on (-1,1). */
/*   ISOFAV: Shows the type of iso of F(u,v) to be extracted to improve */
/*           the rapidity of calculation (has no influence on the form */
/*           of result) */
/*           = 1, shows that it is necessary to calculate the points of F(u,v) */
/*           with fixed u (with NBPNTV values different from v). */
/*           = 2, shows that it is necessaty to calculate the points of  F(u,v) */
/*           with fixed v (with NBPNTU values different from u). */
/*   SOSOTB: Preinitialized table (input/output argument). */
/*   DISOTB: Preinitialized table (input/output argument). */ 
/*   SODITB: Preinitialized table (input/output argument).  */
/*   DIDITB: Preinitialized table (input/output argument). */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   SOSOTB: Table where the terms */
/*           F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms */
/*           F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms */
/*           F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms */
/*           F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   FPNTAB: Auxiliary table. */
/*   TTABLE: Auxiliary table. */
/*   IERCOD: Error code >100 Pb in the evaluation of FONCNP, */
/*           the returned error code is equal to error code of FONCNP + 100. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* --> The external function created by the caller of MA2F1K, MA2FDK */
/*    where MA2FXK should be in the following form : */
/*    SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,ISOFAV,TCONST,NBPTAB */
/*                     ,TTABLE,IDERIU,IDERIV,PPNTAB,IERCOD) */
/*    with the following input arguments : */
/*      - NDIMEN is integer defined as the sum of dimensions of */
/*               sub-spaces (i.e. total dimension of the problem). */
/*      - UINTFN(2) is a table of 2 reals containing the interval */
/*                  by u where the function to be approximated is defined */
/*                  (so it is equal to UIFONC). */
/*      - VINTFN(2) is a table of 2 reals containing the interval */
/*                  by v where the function to be approximated is defined */
/*                  (so it is equal to VIFONC). */
/*      - ISOFAV, is 1 if it is necessary to calculate points with constant u, */
/*                is 2 if it is necessary to calculate points with constant v. */
/*                Any other value is an error. */
/*      - TCONST, real, value of the fixed parameter. Takes values */
/*                in (UIFONC(1),UIFONC(2)) if ISOFAV = 1 or  */
/*                ins (VIFONC(1),VIFONC(2)) if ISOFAV = 2. */
/*      - NBPTAB, integer. Shows the number of points to be calculated. */
/*      - TTABLE, a table of reals NBPTAB. These are the values of */
/*                'free' parameter of discretization (v if IISOFAV=1, */
/*                u if IISOFAV=2). */
/*      - IDERIU, integer, takes values between 0 (position) */
/*                and IORDRE(1) (partial derivative of the function by u */
/*                of order IORDRE(1) if IORDRE(1) > 0). */
/*      - IDERIV, integer, takes values between 0 (position) */
/*                and IORDRE(2) (partial derivative of the function by v */
/*                of order IORDRE(2) if IORDRE(2) > 0). */
/*                If IDERIU=i and IDERIV=j, FONCNP should calculate the */
/*                points of the derivative : */
/*                            i+j */
/*                           d     F(u,v) */
/*                        -------- */
/*                           i  j */
/*                         du dv */

/*     and the output arguments aret : */
/*        - FPNTAB(NDIMEN,NBPTAB) contains, at output, the table of */
/*                                NBPTAB points calculated in FONCNP. */
/*        - IERCOD is, at output the error code of FONCNP. This code */
/*                 (integer) should be strictly positive if there is a problem. */

/*     The input arguments SHOULD NOT be modified under FONCNP. 
*/

/* -->As FONCNP is not forcedly defined in (-1,1)*(-1,1), the */
/* values of UROOTB and VROOTB are consequently modified. */

/* -->The results of discretisation are ranked in 4 tables */
/* SOSOTB, DISOTB, SODITB and DIDITB to earn time */
/* during the calculation of coefficients of the polynom of approximation. */

/*     When NBPNTU is uneven : */
/*        table SOSOTB(0,j) contains F(0,vj) + F(0,-vj), */
/*        table DIDITB(0,j) contains F(0,vj) - F(0,-vj), */
/*     When NBPNTV is uneven : */
/*        table SOSOTB(i,0) contains F(ui,0) + F(-ui,0), */
/*        table DIDITB(i,0) contains F(ui,0) - F(-ui,0), */
/*     When NBPNTU and NBPNTV are uneven : */
/*        term SOSOTB(0,0) contains F(0,0). */

/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


/* --------------------------- Initialization -------------------------- 
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
/* --------- Discretization by U on the roots of the polynom of ------ */
/* --------------- Legendre of degree NBPNTU, iso-V by iso-V --------- */
/* ********************************************************************** 
*/

    if (iuouv == 2) {
	mma2ds2_(ndimen, &uintfn[1], &vintfn[1], foncnp, nbpntu, nbpntv, &
		urootb[1], &vrootb[1], &iuouv, &sosotb[sosotb_offset], &
		disotb[disotb_offset], &soditb[soditb_offset], &diditb[
		diditb_offset], &fpntab[fpntab_offset], &ttable[1], iercod);

/* ******************************************************************
**** */
/* --------- Discretization by V on the roots of the polynom of ------ */
/* --------------- Legendre of degree NBPNTV, iso-V by iso-V --------- */
/* ******************************************************************
**** */

    } else {
/* --> Inversion of indices of tables */
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
/* --> Inversion of indices of tables */
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
	     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
  integer c__0 = 0;
  /* System generated locals */
  integer sosotb_dim1, sosotb_dim2, sosotb_offset, disotb_dim1, disotb_dim2,
  disotb_offset, soditb_dim1, soditb_dim2, soditb_offset, 
  diditb_dim1, diditb_dim2, diditb_offset, fpntab_dim1, 
  fpntab_offset, i__1, i__2, i__3;

  /* Local variables */
  integer jdec;
  logical ldbg;
  doublereal alinu, blinu, alinv, blinv, tcons;
  doublereal dbfn1[2], dbfn2[2];
  integer nuroo, nvroo, id, iu, iv;
  doublereal um, up;


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Discretization of function F(u,v) on the roots of polynoms of Legendre. */

/*     KEYWORDS : */
/*     ----------- */
/*     FONCTION&,DISCRETISATION,&POINT */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*   NDIMEN: Dimension of the space. */
/*   UINTFN: Limits of the interval of definition by u of the function */
/*           to be processed: (UINTFN(1),UINTFN(2)). */
/*   VINTFN: Limits of the interval of definition by v of the function */
/*           to be processed: (VINTFN(1),VINTFN(2)). */
/*   FONCNP: The NAME of the non-polynomial function to be processed. */
/*   NBPNTU: The degree of Legendre polynom on the roots which of */
/*           FONCNP is discretized by u. */
/*   NBPNTV: The degree of Legendre polynom on the roots which of  */
/*           FONCNP is discretized by v. */
/*   UROOTB: Table of STRICTLY POSITIVE roots of the polynom */
/*           of Legendre of degree NBPNTU defined on (-1,1). */
/*   VROOTB: Table of STRICTLY POSITIVE roots of the polynom */
/*           of Legendre of degree NBPNTV defined on (-1,1). */
/*   IIUOUV: Shows the type of iso of F(u,v) tom be extracted to improve the */
/*           rapidity of calculation (has no influence on the form of result) */
/*           = 1, shows that it is necessary to calculate the points of F(u,v) */
/*           with fixed u (so with NBPNTV values different from v). */
/*           = 2, shows that it is necessary to calculate the points of F(u,v) */
/*           with fixed v (so with NBPNTV values different from u). */
/*   SOSOTB: Preinitialized table (input/output argument). */
/*   DISOTB: Preinitialized table (input/output argument). */ 
/*   SODITB: Preinitialized table (input/output argument).  */
/*   DIDITB: Preinitialized table (input/output argument). */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*   SOSOTB: Table where the terms */
/*           F(ui,vj) + F(ui,-vj) + F(-ui,vj) + F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DISOTB: Table where the terms */
/*           F(ui,vj) + F(ui,-vj) - F(-ui,vj) - F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   SODITB: Table where the terms */
/*           F(ui,vj) - F(ui,-vj) + F(-ui,vj) - F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   DIDITB: Table where the terms */
/*           F(ui,vj) - F(ui,-vj) - F(-ui,vj) + F(-ui,-vj) */
/*           are added with ui and vj positive roots of Legendre polynom */
/*           of degree NBPNTU and NBPNTV respectively. */
/*   FPNTAB: Auxiliary table. */
/*   TTABLE: Auxiliary table. */
/*   IERCOD: Error code >100 Pb in the evaluation of FONCNP, */
/*           the returned error code is equal to error code of FONCNP + 100. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* --> The external function created by the caller of MA2F1K, MA2FDK */
/*    where MA2FXK should be in the following form : */
/*    SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,IIIUOUV,TCONST,NBPTAB */
/*                     ,TTABLE,IDERIU,IDERIV,PPNTAB,IERCOD) */
/*    with the following input arguments : */
/*      - NDIMEN is integer defined as the sum of dimensions of */
/*               sub-spaces (i.e. total dimension of the problem). */
/*      - UINTFN(2) is a table of 2 reals containing the interval */
/*                  by u where the function to be approximated is defined */
/*                  (so it is equal to UIFONC). */
/*      - VINTFN(2) is a table of 2 reals containing the interval */
/*                  by v where the function to be approximated is defined */
/*                  (so it is equal to VIFONC). */
/*      - IIIUOUV, is 1 if it is necessary to calculate points with constant u, */
/*                 is 2 if it is necessary to calculate points with constant v. */
/*                 Any other value is an error. */
/*      - TCONST, real, value of the fixed parameter. Takes values */
/*                in (UIFONC(1),UIFONC(2)) if ISOFAV = 1 or  */
/*                ins (VIFONC(1),VIFONC(2)) if ISOFAV = 2. */
/*      - NBPTAB, integer. Shows the number of points to be calculated. */
/*      - TTABLE, a table of reals NBPTAB. These are the values of */
/*                'free' parameter of discretization (v if IIIUOUV=1, */
/*                u if IIIUOUV=2). */
/*      - IDERIU, integer, takes values between 0 (position) */
/*                and IORDRE(1) (partial derivative of the function by u */
/*                of order IORDRE(1) if IORDRE(1) > 0). */
/*      - IDERIV, integer, takes values between 0 (position) */
/*                and IORDRE(2) (partial derivative of the function by v */
/*                of order IORDRE(2) if IORDRE(2) > 0). */
/*                If IDERIU=i and IDERIV=j, FONCNP should calculate the */
/*                points of the derivative : */
/*                            i+j */
/*                           d     F(u,v) */
/*                        -------- */
/*                           i  j */
/*                         du dv */

/*     and the output arguments aret : */
/*        - FPNTAB(NDIMEN,NBPTAB) contains, at output, the table of */
/*                                NBPTAB points calculated in FONCNP. */
/*        - IERCOD is, at output the error code of FONCNP. This code */
/*                 (integer) should be strictly positive if there is a problem. */

/*     The input arguments SHOULD NOT be modified under FONCNP. 
*/

/* -->As FONCNP is not forcedly defined in (-1,1)*(-1,1), the */
/* values of UROOTB and VROOTB are consequently modified. */

/* -->The results of discretisation are ranked in 4 tables */
/* SOSOTB, DISOTB, SODITB and DIDITB to earn time */
/* during the calculation of coefficients of the polynom of approximation. */

/*     When NBPNTU is uneven : */
/*        table SOSOTB(0,j) contains F(0,vj) + F(0,-vj), */
/*        table DIDITB(0,j) contains F(0,vj) - F(0,-vj), */
/*     When NBPNTV is uneven : */
/*        table SOSOTB(i,0) contains F(ui,0) + F(-ui,0), */
/*        table DIDITB(i,0) contains F(ui,0) - F(-ui,0), */
/*     When NBPNTU and NBPNTV are uneven : */
/*        term SOSOTB(0,0) contains F(0,0). */

/*   ATTENTION: These 4 tables are filled by varying the */
/*   1st index first. So, the discretizations */
/*   of F(...,t) (for IIUOUV = 2) or of F(t,...) (IIUOUV = 1) */
/*   are stored in SOSOTB(...,t), SODITB(...,t), etc... */
/*   (this allows to gain important time). */
/*   It is required that the caller, in case of IIUOUV=1, */
/*   invert the roles of u and v, of SODITB and DISOTB BEFORE the */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */

/* --> Indices of loops. */

/* --------------------------- Initialization -------------------------- 
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
/* -------- Discretization by U on the roots of Legendre polynom -------- */
/* ---------------- of degree NBPNTU, with Vj fixed  -------------------- */
/* ********************************************************************** 
*/

    nuroo = *nbpntu / 2;
    nvroo = *nbpntv / 2;
    jdec = (*nbpntu + 1) / 2;

/* ----------- Loading of parameters of discretization by U ------------- */

    i__1 = *nbpntu;
    for (iu = 1; iu <= i__1; ++iu) {
	ttable[iu] = blinu + alinu * urootb[iu];
/* L100: */
    }

/* -------------- For Vj fixed, negative root of Legendre ------------- */

    i__1 = nvroo;
    for (iv = 1; iv <= i__1; ++iv) {
	tcons = blinv + alinv * vrootb[iv];
	(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
        ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
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

/* --------- For Vj = 0 (uneven NBPNTV), discretization by U ----------- */

    if (*nbpntv % 2 != 0) {
	tcons = blinv;
	(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
        ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
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

/* -------------- For Vj fixed, positive root of Legendre ------------- */

    i__1 = nvroo;
    for (iv = 1; iv <= i__1; ++iv) {
	tcons = alinv * vrootb[(*nbpntv + 1) / 2 + iv] + blinv;
	(*const_cast <AdvApp2Var_EvaluatorFunc2Var*> (&foncnp)).Evaluate (
        ndimen, dbfn1, dbfn2, iiuouv, &tcons, nbpntu, &
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
  logical ldbg;
  integer minu, minv;
  doublereal vaux[2];
  integer ii, nd, jj;
  doublereal bid0, bid1;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  Calculate max approximation error done when  */
/*  the coefficients of PATJAC such that the degree by U varies between */
/*  MINDGU and MAXDGU and the degree by V varies between MINDGV and MAXDGV are removed. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,AB_SPECIFI:: CARREAU&,CALCUL,&ERREUR */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*     NDJACU: Dimension by U of table PATJAC. */
/*     NDJACV: Dimension by V of table PATJAC. */
/*     NDIMEN: Dimension of the space. */
/*     MINDGU: Lower limit of index by U of coeff. of PATJAC to be taken into account. */
/*     MAXDGU: Upper limit of index by U of coeff. of PATJAC to be taken into account. */
/*     MINDGV: Lower limit of index by V of coeff. of PATJAC to be taken into account. */
/*     MAXDGV: Upper limit of index by V of coeff. of PATJAC to be taken into account. */
/*     IORDRU: Order of continuity by U provided by square PATJAC (from -1 to 2) */
/*     IORDRV: Order of continuity by U provided by square PATJAC (from -1 to 2) */
/*     XMAXJU: Maximum value of Jacobi polynoms of order IORDRU, */
/*             from degree 0 to MAXDGU - 2*(IORDU+1) */
/*     XMAXJV: Maximum value of Jacobi polynoms of order IORDRV, */
/*             from degree 0 to MAXDGV - 2*(IORDV+1) */
/*     PATJAC: Table of coeff. of square of approximation with */
/*             constraints of order IORDRU by U and IORDRV by V. */
/*     VECERR: Auxiliary vector. */
/*     ERREUR: MAX Error commited during removal of ALREADY CALCULATED coeff of PATJAC */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*     ERREUR: MAX Error commited during removal of coeff of PATJAC */
/*             of indices from MINDGU to MAXDGU by U and from MINDGV to MAXDGV by V */
/*             THEN the already calculated error. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Table PATJAC is the place of storage of coeff. Cij of the square of */
/*     approximation of F(U,V). The indices i and j show the degree  */
/*     by U and by V of base polynoms. These polynoms have the form: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), where */

/*     polynom J(i-2*(IORDU+1)(U) is the Jacobi polynom of order */
/*     IORDRU+1 (the same by V by replacing U u V in the expression above). */

/*     The contribution to the error of term Cij when it is */
/*     removed from PATJAC is increased by: */

/*  DABS(Cij)*XMAXJU(i-2*(IORDRU+1))*XMAXJV(J-2*(IORDRV+1)) where */

/*  XMAXJU(i-2*(IORDRU+1) = ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), 
*/
/*  XMAXJV(i-2*(IORDRV+1) = ((1 - V*V)**(IORDRV+1)).J(j-2*(IORDRV+1)(V). 
*/

/* > */
/* ***********************************************************************
 */
/*   Name of the routine */


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

/* ------------------- Calculate the increment of the max error --------------- */
/* ----- during the removal of the coeffs of indices from MINDGU to MAXDGU ---- */
/* ---------------- by U and indices from MINDGV to MAXDGV by V --------------- */

    i__1 = *ndimen;
    for (nd = 1; nd <= i__1; ++nd) {
	bid1 = 0.;
	i__2 = *maxdgv;
	for (jj = *mindgv; jj <= i__2; ++jj) {
	    bid0 = 0.;
	    i__3 = *maxdgu;
	    for (ii = *mindgu; ii <= i__3; ++ii) {
		bid0 += (d__1 = patjac[ii + (jj + nd * patjac_dim2) * 
			patjac_dim1], advapp_abs(d__1)) * xmaxju[ii - minu];
/* L300: */
	    }
	    bid1 = bid0 * xmaxjv[jj - minv] + bid1;
/* L200: */
	}
	vecerr[nd] = bid1;

/* L100: */
    }

/* ----------------------- Calculate the max error  ----------------------*/

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
  logical ldbg;
  doublereal vaux[2];
  integer i2rdu, i2rdv;
  doublereal errnu, errnv;
  integer ii, nd, jj, nu, nv;
  doublereal bid0, bid1;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  Remove coefficients of PATJAC to obtain the minimum degree */
/*  by U and V checking the imposed tolerance. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,AB_SPECIFI:: CARREAU&,CALCUL,&ERREUR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDJACU: Degree by U of table PATJAC. */
/*     NDJACV: Degree by V of table PATJAC. */
/*     NDIMEN: Dimension of the space. */
/*     MINDGU: Limit of index by U of coeff. of PATJAC to be PRESERVED (should be >=0). */
/*     MAXDGU: Upper limit of index by U of coeff. of PATJAC to be taken into account. */
/*     MINDGV: Limit of index by V of coeff. of PATJAC to be PRESERVED (should be >=0). */
/*     MAXDGV: Upper limit of index by V of coeff. of PATJAC to be taken into account. */
/*     IORDRU: Order of continuity by U provided by square PATJAC (from -1 to 2) */
/*     IORDRV: Order of continuity by U provided by square PATJAC (from -1 to 2) */
/*     XMAXJU: Maximum value of Jacobi polynoms of order IORDRU, */
/*             from degree 0 to MAXDGU - 2*(IORDU+1) */
/*     XMAXJV: Maximum value of Jacobi polynoms of order IORDRV, */
/*             from degree 0 to MAXDGV - 2*(IORDV+1) */
/*     PATJAC: Table of coeff. of square of approximation with */
/*             constraints of order IORDRU by U and IORDRV by V. */
/*     EPMSCUT: Tolerance of approximation. */
/*     VECERR: Auxiliary vector. */
/*     ERREUR: MAX Error commited ALREADY CALCULATED  */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*     ERREUR: MAX Error commited by preserving only coeff of PATJAC */
/*             of indices from 0 to NEWDGU by U and from 0 to NEWDGV by V */
/*             PLUS the already calculated error. */
/* NEWDGU: Min. Degree by U such as the square of approximation */
/*         could check the tolerance. There is always NEWDGU >= MINDGU >= 0. */
/* NEWDGV: Min. Degree by V such as the square of approximation */
/*         could check the tolerance. There is always NEWDGV >= MINDGV >= 0. */


/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Table PATJAC is the place of storage of coeff. Cij of the square of */
/*     approximation of F(U,V). The indices i and j show the degree  */
/*     by U and by V of base polynoms. These polynoms have the form: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), where */

/*     polynom J(i-2*(IORDU+1)(U) is the Jacobi polynom of order */
/*     IORDRU+1 (the same by V by replacing U u V in the expression above). */

/*     The contribution to the error of term Cij when it is */
/*     removed from PATJAC is increased by: */

/*  DABS(Cij)*XMAXJU(i-2*(IORDRU+1))*XMAXJV(J-2*(IORDRV+1)) where */

/*  XMAXJU(i-2*(IORDRU+1) = ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), 
*/
/*  XMAXJV(i-2*(IORDRV+1) = ((1 - V*V)**(IORDRV+1)).J(j-2*(IORDRV+1)(V). 
*/

/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */


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
/* -------------------- Cutting of oefficients ------------------------ 
*/
/* ********************************************************************** 
*/

L1001:

/* ------------------- Calculate the increment of max error --------------- */
/* ----- during the removal of coeff. of indices from MINDGU to MAXDGU ------ */
/* ---------------- by U, the degree by V is fixed to NV ----------------- 
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
			patjac_dim1], advapp_abs(d__1)) * xmaxju[ii - i2rdu] * bid0;
/* L200: */
	    }
	    vecerr[nd] = bid1;
/* L100: */
	}
    } else {
	vecerr[1] = *epmscut * 2;
    }
    errnv = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vecerr[1]);

/* ------------------- Calculate the increment of max error --------------- */
/* ----- during the removal of coeff. of indices from MINDGV to MAXDGV ------ */
/* ---------------- by V, the degree by U is fixed to NU ----------------- 
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
			patjac_dim1], advapp_abs(d__1)) * xmaxjv[jj - i2rdv] * bid0;
/* L400: */
	    }
	    vecerr[nd] = bid1;
/* L300: */
	}
    } else {
	vecerr[1] = *epmscut * 2;
    }
    errnu = AdvApp2Var_MathBase::mzsnorm_(ndimen, &vecerr[1]);

/* ----------------------- Calculate the max error ---------------------- 
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

/* -------------------------- Return the degrees ------------------- 
*/

L2001:
    *newdgu = advapp_max(nu,1);
    *newdgv = advapp_max(nv,1);

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
				     const AdvApp2Var_EvaluatorFunc2Var& foncnp,
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
  integer c__8 = 8;

   /* System generated locals */
    integer courbe_dim1, courbe_dim2, courbe_offset, somtab_dim1, somtab_dim2,
	     somtab_offset, diftab_dim1, diftab_dim2, diftab_offset, 
	    contr1_dim1, contr1_dim2, contr1_offset, contr2_dim1, contr2_dim2,
	     contr2_offset, errmax_dim1, errmax_offset, errmoy_dim1, 
	    errmoy_offset, i__1;
    doublereal d__1;

    /* Local variables */
    integer ideb;
    doublereal tmil;
    integer  ideb1, ibid1, ibid2, ncfja, ndgre, ilong, 
	    ndwrk;
    doublereal* wrkar = 0;
    integer nupil;
    intptr_t iofwr;
    doublereal uvpav[4]	/* was [2][2] */;
    integer nd, ii;
    integer ibb;
    integer ier = 0;
    doublereal uv11[4]	/* was [2][2] */;
  integer ncb1;
    doublereal eps3;
    integer isz1, isz2, isz3, isz4, isz5;
    intptr_t ipt1, ipt2, ipt3, ipt4, ipt5,iptt, jptt;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/* Approximation of a limit of non polynomial function F(u,v) */
/* (in the space of dimension NDIMEN) by SEVERAL  */
/* polynomial curves, by the method of least squares. The parameter of the function is preserved. */

/*     KEYWORDS : */
/*     ----------- */
/* TOUS, AB_SPECIFI :: FONCTION&,EXTREMITE&, APPROXIMATION, &COURBE. */

/*     INPUT ARGUMENTS : */
/*     ----------------- */
/*     NDIMEN: Total Dimension of the space (sum of dimensions */
/*             of sub-spaces) */
/*     NBSESP: Number of "independent" sub-spaces. */
/*     NDIMSE: Table of dimensions of sub-spaces. */
/*     UVFONC: Limits of the interval (a,b)x(c,d) of definition of the */
/*             function to be approached by U (UVFONC(*,1) contains (a,b)) */
/*             and by V (UVFONC(*,2) contains (c,d)). */
/*     FONCNP: External function of position on the non polynomial function to be approached. */
/*     TCONST: Value of isoparameter of F(u,v) to be discretized. */
/*     ISOFAV: Type of chosen iso, = 1, shose that discretization is with u */
/*             fixed; = 2, shows that v is fixed. */
/*     NBROOT: Nb of points of discretisation of the iso, extremities not included. */
/*     ROOTLG: Table of roots of the polynom of Legendre defined on */
/*             (-1,1), of degree NBROOT. */
/*     IORDRE: Order of constraint at the extremities of the limit */
/*              -1 = no constraints, */
/*               0 = constraints of passage to limits (i.e. C0), */
/*               1 = C0 + constraints of 1st derivatives (i.e. C1), */
/*               2 = C1 + constraints of 2nd derivatives (i.e. C2). */
/*     IDERIV: Order of derivative of the limit. */
/*     NDGJAC: Degree of serial development to be used for calculation in */
/*             the Jacobi base. */
/*     NBCRMX: Max Nb of curves to be created. */
/*     NCFLIM: Max Nb of coeff of the polynomial curve */
/*             of approximation (should be above or equal to */
/*             2*IORDRE+2 and below or equal to 50). */
/*     EPSAPR: Table of required errors of approximation */
/*             sub-space by sub-space. */

/*     OUTPUT ARGUMENTS  : */
/*     ------------------- */
/*     NCOEFF: Number of significative coeff of calculated curves. */
/*     COURBE: Table of coeff. of calculated polynomial curves. */
/*             Should be dimensioned in (NCFLIM,NDIMEN,NBCRMX). */
/*             These curves are ALWAYS parametrized in (-1,1). */
/*     NBCRBE: Nb of calculated curves. */
/*     SOMTAB: For F defined on (-1,1) (otherwise rescale the */
/*             parameters), this is the table of sums F(u,vj) + F(u,-vj) 
*/
/*             if ISOFAV = 1 (and IDERIV=0, otherwise the derivatives */
/*             by u of order IDERIV are taken) or sumes F(ui,v) + F(-ui,v) if */
/*             ISOFAV = 2 (and IDERIV=0, otherwise the derivatives by */
/*             v of order IDERIV are taken). */
/*     DIFTAB: For F defined on (-1,1) (otherwise rescale the */
/*             parameters), this is the table of sums F(u,vj) - F(u,-vj) 
*/
/*             if ISOFAV = 1 (and IDERIV=0, otherwise the derivatives */
/*             by u of order IDERIV are taken) or sumes F(ui,v) + F(-ui,v) if */
/*             ISOFAV = 2 (and IDERIV=0, otherwise the derivatives by */
/*             v of order IDERIV are taken). */
/*     CONTR1: Contains the coordinates of the left extremity of the iso */
/*             and of its derivatives till order IORDRE */
/*     CONTR2: Contains the coordinates of the right extremity of the iso */
/*             and of its derivatives till order IORDRE */
/*     TABDEC: Table of NBCRBE+1 parameters of cut of UVFONC(1:2,1) 
*/
/*             if ISOFAV=2, or of UVFONC(1:2,2) if ISOFAV=1. */
/*     ERRMAX: Table of MAX errors (sub-space by sub-space) */
/*             committed in the approximation of FONCNP by NBCRBE curves. */
/*     ERRMOY: Table of AVERAGE errors (sub-space by sub-space) */
/*             committed in the approximation of FONCNP by NBCRBE curves. */
/*     IERCOD: Error code: */
/*             -1 = ERRMAX > EPSAPR for at least one sub-space. */
/*                  (the resulting curves of at least mathematic degree NCFLIM-1 */
/*                  are calculated). */
/*              0 = Everything is ok. */
/*              1 = Pb of incoherence of inputs. */
/*             10 = Pb of calculation of the interpolation of constraints. */
/*             13 = Pb in the dynamic allocation. */
/*             33 = Pb in the data recuperation from block data */
/*                  of coeff. of integration by GAUSS method. */
/*             >100 Pb in the evaluation of FONCNP, the returned error code */
/*                  is equal to the error code of FONCNP + 100. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/* --> The approximation part is done in the space of dimension */
/*    NDIMEN (the sum of dimensions of sub-spaces). For example : */
/*        If NBSESP=2 and NDIMSE(1)=3, NDIMSE(2)=2, there is smoothing with */
/*        NDIMEN=5. The result (in COURBE(NDIMEN,NCOEFF,i) ), will be */
/*        composed of the result of smoothing of 3D function in */
/*        COURBE(1:3,1:NCOEFF,i) and of smoothing of 2D function in */
/*        COURBE(4:5,1:NCOEFF,i). */

/* -->  Routine FONCNP should be declared EXTERNAL in the program */
/*     calling MMA2FNC. */

/* -->  Function FONCNP, declared externally, should be declared */
/*     IMPERATIVELY in form : */
/*          SUBROUTINE FONCNP(NDIMEN,UINTFN,VINTFN,IIUOUV,TCONST,NBPTAB */
/*                           ,TTABLE,IDERIU,IDERIV,IERCOD) */
/*     where the input arguments are : */
/*      - NDIMEN is integer defined as the sum of dimensions of */
/*               sub-spaces (i.e. total dimension of the problem). */
/*      - UINTFN(2) is a table of 2 reals containing the interval */
/*                  by u where the function to be approximated is defined */
/*                  (so it is equal to UIFONC). */
/*      - VINTFN(2) is a table of 2 reals containing the interval */
/*                  by v where the function to be approximated is defined */
/*                  (so it is equal to VIFONC). */
/*      - IIUOUV, shows that the points to be calculated have a constant U */
/*                (IIUOUV=1) or a constant V (IIUOUV=2). */
/*      - TCONST, real, value of the fixed discretisation parameter. Takes values */
/*                in  (UINTFN(1),UINTFN(2)) if IIUOUV=1, */
/*                or in (VINTFN(1),VINTFN(2)) if IIUOUV=2. */
/*      - NBPTAB, the nb of point of discretisation following the free variable */
/*                : V if IIUOUV=1 or U if IIUOUV = 2. */
/*      - TTABLE, Table of NBPTAB parametres of discretisation. . */
/*      - IDERIU, integer, takes values between 0 (position) */
/*                and IORDREU (partial derivative of the function by u */
/*                of order IORDREU if IORDREU > 0). */
/*      - IDERIV, integer, takes values between 0 (position) */
/*                and IORDREV (partial derivative of the function by v */
/*                of order IORDREV if IORDREV > 0). */
/*     and the output arguments are : */
/*        - FPNTAB(NDIMEN,NBPTAB) contains, at output, the table of */
/*                                NBPTAB points calculated in FONCNP. */
/*        - IERCOD is, at output the error code of FONCNP. This code */
/*                 (integer) should be strictly positive if there is a problem. */

/*     The input arguments SHOULD NOT BE modified under FONCNP. 
*/

/* --> If IERCOD=-1, the required precision can't be reached (ERRMAX */
/*     is above EPSAPR on at least one sub-space), but 
*/
/*     one gives the best possible result for NCFLIM and EPSAPR */
/*     chosen by the user. In this case (and for IERCOD=0), there is a solution. */

/* > */
/* ********************************************************************** 
*/
/*   Name of the routine */

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
    AdvApp2Var_SysBase anAdvApp2Var_SysBase;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 1) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2FNC", 7L);
    }
    *iercod = 0;
    iofwr = 0;

/* ---------------- Set to zero the coefficients of CURVE -------------- 
*/

    ilong = *ndimen * *ncflim * *nbcrmx;
    AdvApp2Var_SysBase::mvriraz_(&ilong, &courbe[courbe_offset]);

/* ********************************************************************** 
*/
/* -------------------------- Checking of entries ------------------ 
*/
/* ********************************************************************** 
*/

    AdvApp2Var_MathBase::mmveps3_(&eps3);
    if ((d__1 = uvfonc[4] - uvfonc[3], advapp_abs(d__1)) < eps3) {
	goto L9100;
    }
    if ((d__1 = uvfonc[6] - uvfonc[5], advapp_abs(d__1)) < eps3) {
	goto L9100;
    }

    uv11[0] = -1.;
    uv11[1] = 1.;
    uv11[2] = -1.;
    uv11[3] = 1.;

/* ********************************************************************** */
/* ------------- Preparation of parameters of discretisation ----------- */
/* ********************************************************************** 
*/

/* -- Allocation of a table of parameters and points of discretisation -- */
/* --> For the parameters of discretisation. */
    isz1 = *nbroot + 2;
/* --> For the points of discretisation in MMA1FDI and MMA1CDI and
 */
/*    the auxiliary curve for MMAPCMP */
    ibid1 = *ndimen * (*nbroot + 2);
    ibid2 = ((*iordre + 1) << 1) * *nbroot;
    isz2 = advapp_max(ibid1,ibid2);
    ibid1 = (((*ncflim - 1) / 2 + 1) << 1) * *ndimen;
    isz2 = advapp_max(ibid1,isz2);
/* --> To return the polynoms of hermit. */
    isz3 = ((*iordre + 1) << 2) * (*iordre + 1);
/* --> For the Gauss  coeff. of integration. */
    isz4 = (*nbroot / 2 + 1) * (*ndgjac + 1 - ((*iordre + 1) << 1));
/* --> For the coeff of the curve in the base of Jacobi */
    isz5 = (*ndgjac + 1) * *ndimen;

    ndwrk = isz1 + isz2 + isz3 + isz4 + isz5;
    anAdvApp2Var_SysBase.mcrrqst_(&c__8, &ndwrk, wrkar, &iofwr, &ier);
    if (ier > 0) {
	goto L9013;    }
/* --> For the parameters of discretisation (NBROOT+2 extremities). */
    ipt1 = iofwr;
/* --> For the points of discretisation FPNTAB(NDIMEN,NBROOT+2), */
/*    FPNTAB(NBROOT,2*(IORDRE+1)) and for WRKAR of MMAPCMP. */
    ipt2 = ipt1 + isz1;
/* --> For the polynoms of Hermit */
    ipt3 = ipt2 + isz2;
/* --> For the Gauss  coeff of integration. */
    ipt4 = ipt3 + isz3;
/* --> For the curve in Jacobi. */
    ipt5 = ipt4 + isz4;

/* ------------------ Initialisation of management of cuts --------- 
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
/*                       APPROXIMATION WITH CUTS */
/* ********************************************************************** 
*/

L1000:
/* --> When the top is reached, this is the end ! */
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

/* -------------------- Normalization of parameters -------------------- */

    mma1nop_(nbroot, &rootlg[1], uvpav, isofav, &wrkar[ipt1], &ier);
    if (ier > 0) {
	goto L9100;
    }

/* -------------------- Discretisation of FONCNP ------------------------ */

    mma1fdi_(ndimen, uvpav, foncnp, isofav, tconst, nbroot, &wrkar[ipt1], 
	    iordre, ideriv, &wrkar[ipt2], &somtab[(ncb1 * somtab_dim2 + 1) * 
	    somtab_dim1], &diftab[(ncb1 * diftab_dim2 + 1) * diftab_dim1], &
	    contr1[(ncb1 * contr1_dim2 + 1) * contr1_dim1 + 1], &contr2[(ncb1 
	    * contr2_dim2 + 1) * contr2_dim1 + 1], iercod);
    if (*iercod > 0) {
	goto L9900;
    }

/* -----------Cut the discretisation of constraints ------------*/

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
/* -------------------- Calculate the curve of approximation ------------- 
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
/* ---------------- Add polynom of interpolation ------------------- 
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
/* --------------- Calculate Max and Average error ---------------------- 
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
/* -> Compression of result in WRKAR(IPT2) */
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
	AdvApp2Var_SysBase::mvriraz_(&ilong, &wrkar[ipt5]);
/* -> Passage to canonic base (-1,1) (result in WRKAR(IPT5)). 
*/
	ndgre = ncoeff[ncb1] - 1;
	i__1 = *ndimen;
	for (nd = 1; nd <= i__1; ++nd) {
	    iptt = ipt2 + ((nd - 1) << 1) * (ndgre / 2 + 1);
	    jptt = ipt5 + (nd - 1) * ncoeff[ncb1];
	    AdvApp2Var_MathBase::mmjacan_(iordre, &ndgre, &wrkar[iptt], &wrkar[jptt]);
/* L400: */
	}

/* -> Store the calculated curve */
	ibid1 = 1;
	AdvApp2Var_MathBase::mmfmca8_(&ncoeff[ncb1], ndimen, &ibid1, ncflim, ndimen, &ibid1, &
		wrkar[ipt5], &courbe[(ncb1 * courbe_dim2 + 1) * courbe_dim1 + 
		1]);

/* -> Before normalization of constraints on (-1,1), recalculate */
/*    the true constraints. */
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

/* -> Update the nb of already created curves */
	++(*nbcrbe);

/* -> ...otherwise try to cut the current interval in 2... */
    } else {
	tmil = (tabdec[*nbcrbe + 1] + tabdec[*nbcrbe]) / 2.;
	ideb = *nbcrbe + 1;
	ideb1 = ideb + 1;
	ilong = (nupil - *nbcrbe) << 3;
	AdvApp2Var_SysBase::mcrfill_(&ilong, &tabdec[ideb],&tabdec[ideb1]);
	tabdec[ideb] = tmil;
	++nupil;
    }

/* ---------- Make approximation of the rest ----------- 
*/

    goto L1000;

/* --------------------- Return code of error ----------------- 
*/
/* --> Pb with dynamic allocation */
L9013:
    *iercod = 13;
    goto L9900;
/* --> Inputs incoherent. */
L9100:
    *iercod = 1;
    goto L9900;

/* -------------------------- Dynamic desallocation ------------------- 
*/

L9900:
    if (iofwr != 0) {
	anAdvApp2Var_SysBase.mcrdelt_(&c__8, &ndwrk, wrkar, &iofwr, &ier);
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
  integer idim, ncfu, ncfv, id, ii, nd, jj, ku, kv, ns, ibb;
  doublereal bid;
  doublereal tol;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Reduction of degree when the squares are the squares of constraints. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS,AB_SPECIFI::CARREAU&,REDUCTION,&CARREAU */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/* NCFMXU: Max Nb of coeff by u of solution P(u,v) (table */
/*         PATCAN). This argument serves only to declare the size of this table. */
/* NCFMXV: Max Nb of coeff by v of solution P(u,v) (table */
/*         PATCAN). This argument serves only to declare the size of this table. */
/* NDIMEN: Total dimension of the space where the processed function */
/*         takes its values.(sum of dimensions of sub-spaces) */
/* NBSESP: Nb of independent sub-spaces where the errors are measured. */
/* NDIMSE: Table of dimensions of NBSESP sub-spaces. */
/* NBUPAT: Nb of square solution by u. */
/* NBVPAT: Nb of square solution by v. */
/* IORDRU: Order of constraint imposed at the extremities of iso-V */
/*         = 0, the extremities of iso-V are calculated */
/*         = 1, additionally the 1st derivative in the direction of iso-V is calculated */
/*         = 2, additionally the 2nd derivative in the direction of iso-V is calculated  */
/* IORDRV: Ordre de contrainte impose aux extremites de l'iso-U */
/*         = 0, on calcule les extremites de l'iso-U. */
/*         = 1, additionally the 1st derivative in the direction of iso-U is calculated */
/*         = 2, additionally the 2nd derivative in the direction of iso-U is calculated  */
/* EPSAPR: Table of imposed precisions, sub-space by sub-space. */
/* EPSFRO: Table of imposed precisions, sub-space by sub-space on the limits of squares. */
/* PATCAN: Table of coeff. in the canonic base of squares P(u,v) calculated for (u,v) in (-1,1). */
/* ERRMAX: Table of MAX errors (sub-space by sub-space) */
/*         committed in the approximation of F(u,v) by P(u,v). */
/* NCOEFU: Table of Nb of significative coeffs. by u of calculated squares. */
/* NCOEFV: Table of Nb of significative coeffs. by v of calculated squares. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/* NCOEFU: Table of Nb of significative coeffs. by u of calculated squares. */
/* NCOEFV: Table of Nb of significative coeffs. by v of calculated squares. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */
/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


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

/* ********************************************************************** */
/* -------------------- Reduction of degree by U ------------------------- */
/* ********************************************************************** */

L200:
	    if (ncfu <= (*iordru + 1) << 1 && ncfu > 2) {

		idim = 0;
		i__3 = *nbsesp;
		for (ns = 1; ns <= i__3; ++ns) {
		    tol = epsapr[ns];
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 9];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 10];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 11];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 12];
		    tol = advapp_min(d__1,d__2);
		    if (ii == 1 || ii == *nbupat || jj == 1 || jj == *nbvpat) 
			    {
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 5];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 6];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 7];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + (epsfro_dim1 << 3)];
			tol = advapp_min(d__1,d__2);
		    }
		    bid = 0.;

		    i__4 = ndimse[ns];
		    for (nd = 1; nd <= i__4; ++nd) {
			id = idim + nd;
			i__5 = ncfv;
			for (kv = 1; kv <= i__5; ++kv) {
			    bid += (d__1 = patcan[ncfu + (kv + (id + (ii + jj 
				    * patcan_dim4) * patcan_dim3) * 
				    patcan_dim2) * patcan_dim1], advapp_abs(d__1));
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

/* ********************************************************************** */
/* -------------------- Reduction of degree by V ------------------------- */
/* ********************************************************************** */

L300:
	    if (ncfv <= (*iordrv + 1) << 1 && ncfv > 2) {

		idim = 0;
		i__3 = *nbsesp;
		for (ns = 1; ns <= i__3; ++ns) {
		    tol = epsapr[ns];
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 9];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 10];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 11];
		    tol = advapp_min(d__1,d__2);
/* Computing MIN */
		    d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 12];
		    tol = advapp_min(d__1,d__2);
		    if (ii == 1 || ii == *nbupat || jj == 1 || jj == *nbvpat) 
			    {
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 5];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 6];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + epsfro_dim1 * 7];
			tol = advapp_min(d__1,d__2);
/* Computing MIN */
			d__1 = tol, d__2 = epsfro[ns + (epsfro_dim1 << 3)];
			tol = advapp_min(d__1,d__2);
		    }
		    bid = 0.;

		    i__4 = ndimse[ns];
		    for (nd = 1; nd <= i__4; ++nd) {
			id = idim + nd;
			i__5 = ncfu;
			for (ku = 1; ku <= i__5; ++ku) {
			    bid += (d__1 = patcan[ku + (ncfv + (id + (ii + jj 
				    * patcan_dim4) * patcan_dim3) * 
				    patcan_dim2) * patcan_dim1], advapp_abs(d__1));
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

/* --- Return the nbs of coeff. and pass to the next square --- */

L400:
	    ncoefu[ii + jj * ncoefu_dim1] = advapp_max(ncfu,2);
	    ncoefv[ii + jj * ncoefv_dim1] = advapp_max(ncfv,2);
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
    logical ldbg;
    integer numax, ii;
    doublereal bid;


/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  Calculate the max of Jacobo polynoms multiplied by the weight on */
/*  (-1,1) for order 0,4,6 or Legendre. */

/*     KEYWORDSS : */
/*     ----------- */
/*        LEGENDRE,APPROXIMATION,ERREUR. */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*     NDGJAC: Nb of Jacobi coeff. of approximation. */
/*     IORDRE: Order of continuity (from -1 to 2) */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     XJACMX: Table of maximums of Jacobi polynoms. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ***********************************************************************
 */
/*   Name of the routine */
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
    logical ldbg;
    integer minu, minv, idebu, idebv, ii, nd, jj;
    doublereal bid0, bid1;
    
    
/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*  Calculate the average approximation error made when only */
/*  the coefficients of PATJAC of degree between */
/*  2*(IORDRU+1) and MINDGU by U and 2*(IORDRV+1) and MINDGV by V are preserved. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,APPROXIMATION, AVERAGE ERROR */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NDGUMX: Dimension by U of table PATJAC. */
/*     NDGVMX: Dimension by V of table PATJAC. */
/*     NDIMEN: Dimension of the space. */
/*     MINDGU: Lower limit of the index by U of PATJAC coeff to be taken into account. */
/*     MAXDGU: Upper limit of the index by U of PATJAC coeff to be taken into account. */
/*     MINDGV: Lower limit of the index by V of PATJAC coeff to be taken into account. */
/*     MAXDGV: Upper limit of the index by V of PATJAC coeff to be taken into account. */
/*     IORDRU: Order of continuity by U provided by square PATJAC (from -1 to 2) */
/*     IORDRV: Order of continuity by V provided by square PATJAC (from -1 to 2) */
/*     PATJAC: Table of coeff. of the approximation square with  */
/*             constraints of order IORDRU by U and IORDRV by V. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     ERRMOY: Average error commited by preserving only the coeff of */
/*             PATJAC 2*(IORDRU+1) in MINDGU by U and 2*(IORDRV+1) in MINDGV by V. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Table PATJAC stores the coeff. Cij of */
/*     approximation square F(U,V). Indexes i and j show the degree by  */
/*     U and by V of the base polynoms. These base polynoms are in the form: */

/*          ((1 - U*U)**(IORDRU+1)).J(i-2*(IORDRU+1)(U), where */

/*     polynom J(i-2*(IORDU+1)(U) is the Jacobi polynom of order */
/*     IORDRU+1 (the same by V by replacing U by V in the above expression). */

/*     The contribution to the average error of term Cij when */
/*     it is removed from PATJAC is Cij*Cij. */

/* > */
/* ***********************************************************************
 */
/*   Name of the routine */


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
    minu = advapp_max(idebu,*mindgu);
    minv = advapp_max(idebv,*mindgv);
    bid0 = 0.;
    *errmoy = 0.;

/* ------------------ Calculation  of the upper bound of the average error  ------------ */
/* -------------------- when the coeff. of indexes from MINDGU to MAXDGU ------ */
/* ---------------- by U and of indexes from MINDGV to MAXDGV by V are removed -------------- */

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

/* ----------------------- Calculation of the average error ------------- 
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
  integer ii, ibb;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Return roots of Legendre for discretisations. */

/*     KEYWORDS : */
/*     ----------- */
/*     TOUS, AB_SPECIFI::CONTRAINTE&, DISCRETISATION, &POINT */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*     NBPNTU: Nb of INTERNAL parameters of discretization BY U. */
/*             This is also the nb of root of the Legendre polynom where the discretization is done. */
/*     NBPNTV: Nb of INTERNAL parameters of discretization BY V. */
/*             This is also the nb of root of the Legendre polynom where the discretization is done. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*     UROOTL: Table of parameters of discretisation ON (-1,1) BY U. 
*/
/*     VROOTL: Table of parameters of discretisation ON (-1,1) BY V. 
*/

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */

/* > */
/* ********************************************************************** 
*/

/*   Name of the routine */


    /* Parameter adjustments */
    --urootl;
    --vrootl;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 3) {
	AdvApp2Var_SysBase::mgenmsg_("MMA2ROO", 7L);
    }

/* ---------------- Return the POSITIVE roots on U ------------------ 
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

/* ---------------- Return the POSITIVE roots on V ------------------ 
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
  integer igss, ikdeb;
  doublereal bidon;
  integer nd, ik, ir, nbroot, ibb;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*     Calculate the coefficients of polinomial approximation curve */
/*     of degree NDGJAC by the method of smallest squares starting from */
/*     the discretization of function on the roots of Legendre polynom */
/*     of degree NBPNTS. */

/*     KEYWORDS : */
/*     ----------- */
/*     FONCTION,APPROXIMATION,COEFFICIENT,POLYNOME */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIM   : Dimension of the space. */
/*        NDGJAC : Max Degree of the polynom of approximation. */
/*                 The representation in the orthogonal base starts from degree */
/*                 0 to degree NDGJAC-2*(JORDRE+1). The polynomial base */
/*                 is the base of Jacobi of order -1 (Legendre), 0, 1 and 2 */
/*        IORDRE : Order of the base of Jacobi (-1,0,1 or 2). Corresponds */
/*                 to step of constraints, C0,C1 or C2. */
/*        NBPNTS : Degree of the polynom of Legendre on the roots which of */
/*                 are calculated the coefficients of integration by */
/*                 Gauss method. It is required to set NBPNTS=30,40,50 or 61 */
/*                 and NDGJAC < NBPNTS. */
/*        SOMTAB : Table of F(ti)+F(-ti) with ti in ROOTAB. */
/*        DIFTAB : Table of F(ti)-F(-ti) with ti in ROOTAB. */
/*        GSSTAB(i,k) : Table of coefficients of integration by the Gauss method : */
/*                      i varies from 0 to NBPNTS and */
/*                      k varies from 0 to NDGJAC-2*(JORDRE+1). */

/*     OUTPUT ARGUMENTSE : */
/*     ------------------- */
/*        CRVJAC : Curve of approximation of FONCNP with eventually */
/*                 taking into account of constraints at the extremities. */
/*                 This curve is of degree NDGJAC. */

/*     COMMONS USED   : */
/*     ---------------- */

/*     REFERENCES CALLED   : */
/*     --------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ------------------------------- */
/* > */
/* ********************************************************************** 
*/

/*  Name of the routine */

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

/* ----------------- Calculate the coefficients of even degree ----------
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

/* --------------- Calculate the coefficients of uneven degree ----------
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

/* ------- Add terms connected to the supplementary root (0.D0) ------ */
/* ----------- of Legendre polynom of uneven degree NBPNTS ----------- 
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
  doublereal bidj;
  integer i__, ia, nd, ncfcut, ibb;
  doublereal bid;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Calculate the square root of the average quadratic error */
/*        of approximation done when only the */
/*        first NCFNEW coefficients of a curve of degree NCOEFF-1 */
/*        written in NORMALIZED Jacobi base of order 2*(IORDRE+1) are preserved. */

/*     KEYWORDS : */
/*     ----------- */
/*        LEGENDRE,POLYGONE,APPROXIMATION,ERREUR. */

/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NCOFMX : Maximum degree of the curve. */
/*        NDIM   : Dimension of the space. */
/*        NCOEFF : Degree +1 of the curve. */
/*        IORDRE : Order of constraint of continuity at the extremities. */
/*        CRVJAC : The curve the degree which of will be lowered. */
/*        NCFNEW : Degree +1 of the resulting polynom. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        ERRMOY : Average precision of approximation. */

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

    /* Parameter adjustments */
    crvjac_dim1 = *ncofmx;
    crvjac_offset = crvjac_dim1 + 1;
    crvjac -= crvjac_offset;

    /* Function Body */
    ibb = AdvApp2Var_SysBase::mnfndeb_();
    if (ibb >= 2) {
	AdvApp2Var_SysBase::mgenmsg_("MMAPERM", 7L);
    }

/* --------- Minimum degree that can be reached : Stop at 1 or IA ------- 
*/

    ia = (*iordre + 1) << 1;
    ncfcut = ia + 1;
    if (*ncfnew + 1 > ncfcut) {
	ncfcut = *ncfnew + 1;
    }

/* -------------- Elimination of coefficients of high degree ------------ */
/* ----------- Loop on the series of Jacobi :NCFCUT --> NCOEFF --------- */

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

/* ----------- Square Root of average quadratic error e ----------- 
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
  integer kjac, iptt, ipdb0, infdg, iptdb, mxjac, ilong, ibb;

/* ********************************************************************** 
*/

/*     FUNCTION : */
/*     ---------- */
/*        Load the elements required for integration by */
/*        Gauss method to obtain the coefficients in the base of */
/*        Legendre of the approximation by the least squares of a */
/*        function. The elements are stored in commons MMAPGSS */
/*        (case without constraint), MMAPGS0 (constraints C0), MMAPGS1 */
/*        (constraints C1) and MMAPGS2 (constraints C2). */

/*     KEYWORDS : */
/*     ----------- */
/*        INTEGRATION,GAUSS,JACOBI */

/*     INPUT ARGUMENTS  : */
/*     ------------------ */
/*        NDGJAC : Max degree of the polynom of approximation. */
/*                 The representation in orthogonal base goes from degree */
/*                 0 to degree NDGJAC-2*(JORDRE+1). The polynomial base */
/*                 is the base of Jacobi of order -1 (Legendre), 0, 1 and 2 */
/*        NBPNTS : Degree of the polynom of Legendre on the roots which of */
/*                 are calculated the coefficients of integration by the */
/*                 method of Gauss. It is required that NBPNTS=8,10,15,20,25, */
/*                  30,40,50 or 61 and NDGJAC < NBPNTS. */
/*        JORDRE : Order of the base of Jacobi (-1,0,1 or 2). Corresponds */
/*                 to step of constraints C0,C1 or C2. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        CGAUSS(i,k) : Table of coefficients of integration by */
/*                      Gauss method : i varies from 0 to the integer part */
/*                      of NBPNTS/2 and k varies from 0 to NDGJAC-2*(JORDRE+1). */
/*                      These are the coeff. of integration associated to */
/*                      positive roots of the polynom of Legendre of degree */
/*                      NBPNTS. CGAUSS(0,k) contains coeff. */
/*                      of integration associated to root t = 0 when */
/*                      NBPNTS is uneven. */
/*        IERCOD : Error code. */
/*                 = 0 OK, */
/*                 = 11 NBPNTS is not 8,10,15,20,25,30,40,50 or 61. */
/*                 = 21 JORDRE is not -1,0,1 or 2. */
/*                 = 31 NDGJAC is too great or too small. */

/*     COMMONS USED   : */
/*     ---------------- */
/*        MMAPGSS,MMAPGS0,MMAPGS1,MMAPGS2. */
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

/* ------------------- Tests on the validity of inputs ---------------- 
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

/* --------------- Calculation of the start pointer following NBPNTS ----------- 
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

/* ------------------ Choice of the common depending on JORDRE ------------- 
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

/* ---------------- Common MMAPGSS (case without constraints) ----------------
 */

L1000:
    ilong = *nbpnts / 2 << 3;
    i__1 = *ndgjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &mmapgss_.gslxjs[iptt - 1], 
		 &cgauss[kjac * cgauss_dim1 + 1]);
/* L100: */
    }
/* --> Case when the number of points is uneven. */
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

/* ---------------- Common MMAPGS0 (case with constraints C0) -------------
 */

L2000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &mmapgs0_.gslxj0[iptt - 1], 
		 &cgauss[kjac * cgauss_dim1 + 1]);
/* L200: */
    }
/* --> Case when the number of points is uneven. */
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

/* ---------------- Common MMAPGS1 (case with constraints C1) -------------
 */

L3000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &mmapgs1_.gslxj1[iptt - 1], 
		 &cgauss[kjac * cgauss_dim1 + 1]);
/* L300: */
    }
/* --> Case when the number of points is uneven. */
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

/* ---------------- Common MMAPGS2 (case with constraints C2) -------------
 */

L4000:
    mxjac = *ndgjac - infdg;
    ilong = *nbpnts / 2 << 3;
    i__1 = mxjac;
    for (kjac = 0; kjac <= i__1; ++kjac) {
	iptt = iptdb + kjac * (*nbpnts / 2) + 1;
	AdvApp2Var_SysBase::mcrfill_(&ilong, 
		 &mmapgs2_.gslxj2[iptt - 1], 
		 &cgauss[kjac * cgauss_dim1 + 1]);
/* L400: */
    }
/* --> Cas of uneven number of points. */
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

/* ------------------------- Return the error code --------------
 */
/* --> NBPNTS is not OK */
L9100:
    *iercod = 11;
    goto L9999;
/* --> JORDRE is not OK */
L9200:
    *iercod = 21;
    goto L9999;
/* --> NDGJAC is not OK */
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
  integer kdim, nd, ii, jj, ibb;

/* ***********************************************************************
 */

/*     FONCTION : */
/*     ---------- */
/*        Passage from canonical to Jacobi base for a */
/*        "square" in a space of arbitrary dimension. */

/*     MOTS CLES : */
/*     ----------- */
/*       SMOOTHING,BASE,LEGENDRE */


/*     INPUT ARGUMENTS : */
/*     ------------------ */
/*        NDIMEN   : Dimension of the space. */
/*        NCOEFU : Degree+1 by U. */
/*        NCOEFV : Degree+1 by V. */
/*        IORDRU : Order of Jacobi polynoms by U. */
/*        IORDRV : Order of Jacobi polynoms by V. */
/*        PTCLGD : The square in the Jacobi base. */

/*     OUTPUT ARGUMENTS : */
/*     ------------------- */
/*        PTCAUX : Auxilliary space. */
/*        PTCCAN : The square in the canonic base (-1,1) */

/*     COMMONS USED   : */
/*     ---------------- */

/*     APPLIED REFERENCES  : */
/*     ----------------------- */

/*     DESCRIPTION/NOTES/LIMITATIONS : */
/*     ----------------------------------- */
/*     Cancels and replaces MJACPC */

/* ********************************************************************* 
*/
/*   Name of the routine */


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

/*   Passage into canonical by u. */

    kdim = *ndimen * *ncoefv;
    AdvApp2Var_MathBase::mmjaccv_(ncoefu, 
	     &kdim, 
	     iordru, 
	     &ptclgd[ptclgd_offset], 
	     &ptcaux[ptcaux_offset], 
	     &ptccan[ptccan_offset]);

/*   Swapping of u and v. */

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

/*   Passage into canonical by v. */

    kdim = *ndimen * *ncoefu;
    AdvApp2Var_MathBase::mmjaccv_(ncoefv, 
	     &kdim, 
	     iordrv, 
	     &ptcaux[((ptcaux_dim3 + 1) * ptcaux_dim2 + 1) * ptcaux_dim1 + 1], 
	     &ptccan[ptccan_offset], 
	     &ptcaux[(((ptcaux_dim3 << 1) + 1) * ptcaux_dim2 + 1) * ptcaux_dim1 + 1]);

/*  Swapping of u and v. */

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
