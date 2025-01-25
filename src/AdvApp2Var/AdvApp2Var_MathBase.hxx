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

// AdvApp2Var_MathBase.hxx
#ifndef AdvApp2Var_MathBase_HeaderFile
#define AdvApp2Var_MathBase_HeaderFile

#include <Standard_Macro.hxx>
#include <AdvApp2Var_Data_f2c.hxx>

class AdvApp2Var_MathBase
{
public:
  ///
  Standard_EXPORT static int mmapcmp_(integer*, integer*, integer*, double*, double*);

  Standard_EXPORT static int mmdrc11_(integer*,
                                      integer*,
                                      integer*,
                                      doublereal*,
                                      doublereal*,
                                      doublereal*);

  Standard_EXPORT static int mmfmca9_(integer*,
                                      integer*,
                                      integer*,
                                      integer*,
                                      integer*,
                                      integer*,
                                      doublereal*,
                                      doublereal*);

  Standard_EXPORT static int mmfmcb5_(integer*,
                                      integer*,
                                      integer*,
                                      doublereal*,
                                      integer*,
                                      integer*,
                                      integer*,
                                      doublereal*,
                                      integer*);

  Standard_EXPORT static void mmwprcs_(doublereal*,
                                       doublereal*,
                                       doublereal*,
                                       doublereal*,
                                       integer*,
                                       integer*);
  ///
  Standard_EXPORT static int mmcglc1_(integer*    ndimax,
                                      integer*    ndimen,
                                      integer*    ncoeff,
                                      doublereal* courbe,
                                      doublereal* tdebut,
                                      doublereal* tfinal,
                                      doublereal* epsiln,
                                      doublereal* xlongc,
                                      doublereal* erreur,
                                      integer*    iercod);

  Standard_EXPORT static int mmbulld_(integer*    nbcoln,
                                      integer*    nblign,
                                      doublereal* dtabtr,
                                      integer*    numcle);

  Standard_EXPORT static int mmcdriv_(integer*    ndimen,
                                      integer*    ncoeff,
                                      doublereal* courbe,
                                      integer*    ideriv,
                                      integer*    ncofdv,
                                      doublereal* crvdrv);

  Standard_EXPORT static int mmcvctx_(integer*    ndimen,
                                      integer*    ncofmx,
                                      integer*    nderiv,
                                      doublereal* ctrtes,
                                      doublereal* crvres,
                                      doublereal* tabaux,
                                      doublereal* xmatri,
                                      integer*    iercod);

  Standard_EXPORT static int mdsptpt_(integer*    ndimen,
                                      doublereal* point1,
                                      doublereal* point2,
                                      doublereal* distan);

  Standard_EXPORT static int mmaperx_(integer*    ncofmx,
                                      integer*    ndimen,
                                      integer*    ncoeff,
                                      integer*    iordre,
                                      doublereal* crvjac,
                                      integer*    ncfnew,
                                      doublereal* ycvmax,
                                      doublereal* errmax,
                                      integer*    iercod);

  Standard_EXPORT static int mmdrvck_(integer*    ncoeff,
                                      integer*    ndimen,
                                      doublereal* courbe,
                                      integer*    ideriv,
                                      doublereal* tparam,
                                      doublereal* pntcrb);

  Standard_EXPORT static int mmeps1_(doublereal* epsilo);

  Standard_EXPORT static int mmfmca8_(const integer* ndimen,
                                      const integer* ncoefu,
                                      const integer* ncoefv,
                                      const integer* ndimax,
                                      const integer* ncfumx,
                                      const integer* ncfvmx,
                                      doublereal*    tabini,
                                      doublereal*    tabres);

  Standard_EXPORT static int mmfmcar_(integer*    ndimen,
                                      integer*    ncofmx,
                                      integer*    ncoefu,
                                      integer*    ncoefv,
                                      doublereal* patold,
                                      doublereal* upara1,
                                      doublereal* upara2,
                                      doublereal* vpara1,
                                      doublereal* vpara2,
                                      doublereal* patnew,
                                      integer*    iercod);

  Standard_EXPORT static int mmfmtb1_(integer*    maxsz1,
                                      doublereal* table1,
                                      integer*    isize1,
                                      integer*    jsize1,
                                      integer*    maxsz2,
                                      doublereal* table2,
                                      integer*    isize2,
                                      integer*    jsize2,
                                      integer*    iercod);

  Standard_EXPORT static int mmgaus1_(
    integer* ndimf,
    int (*bfunx)(integer*    ninteg,
                 doublereal* parame,
                 doublereal* vfunj1,
                 integer*    iercod), // mmfunj1_() from Smoothing.cxx
    integer*    k,
    doublereal* xd,
    doublereal* xf,
    doublereal* saux1,
    doublereal* saux2,
    doublereal* somme,
    integer*    niter,
    integer*    iercod);

  Standard_EXPORT static int mmhjcan_(integer*    ndimen,
                                      integer*    ncourb,
                                      integer*    ncftab,
                                      integer*    orcont,
                                      integer*    ncflim,
                                      doublereal* tcbold,
                                      doublereal* tdecop,
                                      doublereal* tcbnew,
                                      integer*    iercod);

  Standard_EXPORT static int mminltt_(integer*    ncolmx,
                                      integer*    nlgnmx,
                                      doublereal* tabtri,
                                      integer*    nbrcol,
                                      integer*    nbrlgn,
                                      doublereal* ajoute,
                                      doublereal* epseg,
                                      integer*    iercod);

  Standard_EXPORT static int mmjaccv_(const integer*    ncoef,
                                      const integer*    ndim,
                                      const integer*    ider,
                                      const doublereal* crvlgd,
                                      doublereal*       polaux,
                                      doublereal*       crvcan);

  Standard_EXPORT static int mmpobas_(doublereal* tparam,
                                      integer*    iordre,
                                      integer*    ncoeff,
                                      integer*    nderiv,
                                      doublereal* valbas,
                                      integer*    iercod);

  Standard_EXPORT static int mmmpocur_(integer*    ncofmx,
                                       integer*    ndim,
                                       integer*    ndeg,
                                       doublereal* courbe,
                                       doublereal* tparam,
                                       doublereal* tabval);

  Standard_EXPORT static int mmposui_(integer* dimmat,
                                      integer* nistoc,
                                      integer* aposit,
                                      integer* posuiv,
                                      integer* iercod);

  Standard_EXPORT static int mmresol_(integer*    hdimen,
                                      integer*    gdimen,
                                      integer*    hnstoc,
                                      integer*    gnstoc,
                                      integer*    mnstoc,
                                      doublereal* matsyh,
                                      doublereal* matsyg,
                                      doublereal* vecsyh,
                                      doublereal* vecsyg,
                                      integer*    hposit,
                                      integer*    hposui,
                                      integer*    gposit,
                                      integer*    mmposui,
                                      integer*    mposit,
                                      doublereal* vecsol,
                                      integer*    iercod);

  Standard_EXPORT static int mmrtptt_(integer* ndglgd, doublereal* rtlegd);

  Standard_EXPORT static int mmsrre2_(doublereal* tparam,
                                      integer*    nbrval,
                                      doublereal* tablev,
                                      doublereal* epsil,
                                      integer*    numint,
                                      integer*    itypen,
                                      integer*    iercod);

  Standard_EXPORT static int mmtrpjj_(integer*    ncofmx,
                                      integer*    ndimen,
                                      integer*    ncoeff,
                                      doublereal* epsi3d,
                                      integer*    iordre,
                                      doublereal* crvlgd,
                                      doublereal* ycvmax,
                                      doublereal* errmax,
                                      integer*    ncfnew);

  Standard_EXPORT static int mmunivt_(integer*    ndimen,
                                      doublereal* vector,
                                      doublereal* vecnrm,
                                      doublereal* epsiln,
                                      integer*    iercod);

  Standard_EXPORT static int mmvncol_(integer*    ndimen,
                                      doublereal* vecin,
                                      doublereal* vecout,
                                      integer*    iercod);

  Standard_EXPORT static doublereal msc_(integer* ndimen, doublereal* vecte1, doublereal* vecte2);

  Standard_EXPORT static int mvsheld_(integer* n, integer* is, doublereal* dtab, integer* icle);

  Standard_EXPORT static int mmarcin_(integer*    ndimax,
                                      integer*    ndim,
                                      integer*    ncoeff,
                                      doublereal* crvold,
                                      doublereal* u0,
                                      doublereal* u1,
                                      doublereal* crvnew,
                                      integer*    iercod);

  Standard_EXPORT static int mmcvinv_(integer*    ndimax,
                                      integer*    ncoef,
                                      integer*    ndim,
                                      doublereal* curveo,
                                      doublereal* curve);

  Standard_EXPORT static int mmjacan_(const integer* ideriv,
                                      integer*       ndeg,
                                      doublereal*    poljac,
                                      doublereal*    polcan);

  Standard_EXPORT static int mmpocrb_(integer*    ndimax,
                                      integer*    ncoeff,
                                      doublereal* courbe,
                                      integer*    ndim,
                                      doublereal* tparam,
                                      doublereal* pntcrb);

  Standard_EXPORT static int mmmrslwd_(integer*    normax,
                                       integer*    nordre,
                                       integer*    ndim,
                                       doublereal* amat,
                                       doublereal* bmat,
                                       doublereal* epspiv,
                                       doublereal* aaux,
                                       doublereal* xmat,
                                       integer*    iercod);

  Standard_EXPORT static int mmveps3_(doublereal* eps03);

  Standard_EXPORT static doublereal pow__di(doublereal* x, integer* n);

  Standard_EXPORT static doublereal mzsnorm_(integer* ndimen, doublereal* vecteu);
};

#endif
