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

class AdvApp2Var_MathBase
{
public:
  ///
  Standard_EXPORT static int mmapcmp_(int*, int*, int*, double*, double*);

  Standard_EXPORT static int mmdrc11_(int*,
                                      int*,
                                      int*,
                                      double*,
                                      double*,
                                      double*);

  Standard_EXPORT static int mmfmca9_(int*,
                                      int*,
                                      int*,
                                      int*,
                                      int*,
                                      int*,
                                      double*,
                                      double*);

  Standard_EXPORT static int mmfmcb5_(int*,
                                      int*,
                                      int*,
                                      double*,
                                      int*,
                                      int*,
                                      int*,
                                      double*,
                                      int*);

  Standard_EXPORT static void mmwprcs_(double*,
                                       double*,
                                       double*,
                                       double*,
                                       int*,
                                       int*);
  ///
  Standard_EXPORT static int mmcglc1_(int*    ndimax,
                                      int*    ndimen,
                                      int*    ncoeff,
                                      double* courbe,
                                      double* tdebut,
                                      double* tfinal,
                                      double* epsiln,
                                      double* xlongc,
                                      double* erreur,
                                      int*    iercod);

  Standard_EXPORT static int mmbulld_(int*    nbcoln,
                                      int*    nblign,
                                      double* dtabtr,
                                      int*    numcle);

  Standard_EXPORT static int mmcdriv_(int*    ndimen,
                                      int*    ncoeff,
                                      double* courbe,
                                      int*    ideriv,
                                      int*    ncofdv,
                                      double* crvdrv);

  Standard_EXPORT static int mmcvctx_(int*    ndimen,
                                      int*    ncofmx,
                                      int*    nderiv,
                                      double* ctrtes,
                                      double* crvres,
                                      double* tabaux,
                                      double* xmatri,
                                      int*    iercod);

  Standard_EXPORT static int mdsptpt_(int*    ndimen,
                                      double* point1,
                                      double* point2,
                                      double* distan);

  Standard_EXPORT static int mmaperx_(int*    ncofmx,
                                      int*    ndimen,
                                      int*    ncoeff,
                                      int*    iordre,
                                      double* crvjac,
                                      int*    ncfnew,
                                      double* ycvmax,
                                      double* errmax,
                                      int*    iercod);

  Standard_EXPORT static int mmdrvck_(int*    ncoeff,
                                      int*    ndimen,
                                      double* courbe,
                                      int*    ideriv,
                                      double* tparam,
                                      double* pntcrb);

  Standard_EXPORT static int mmeps1_(double* epsilo);

  Standard_EXPORT static int mmfmca8_(const int* ndimen,
                                      const int* ncoefu,
                                      const int* ncoefv,
                                      const int* ndimax,
                                      const int* ncfumx,
                                      const int* ncfvmx,
                                      double*    tabini,
                                      double*    tabres);

  Standard_EXPORT static int mmfmcar_(int*    ndimen,
                                      int*    ncofmx,
                                      int*    ncoefu,
                                      int*    ncoefv,
                                      double* patold,
                                      double* upara1,
                                      double* upara2,
                                      double* vpara1,
                                      double* vpara2,
                                      double* patnew,
                                      int*    iercod);

  Standard_EXPORT static int mmfmtb1_(int*    maxsz1,
                                      double* table1,
                                      int*    isize1,
                                      int*    jsize1,
                                      int*    maxsz2,
                                      double* table2,
                                      int*    isize2,
                                      int*    jsize2,
                                      int*    iercod);

  Standard_EXPORT static int mmgaus1_(
    int* ndimf,
    int (*bfunx)(int*    ninteg,
                 double* parame,
                 double* vfunj1,
                 int*    iercod), // mmfunj1_() from Smoothing.cxx
    int*    k,
    double* xd,
    double* xf,
    double* saux1,
    double* saux2,
    double* somme,
    int*    niter,
    int*    iercod);

  Standard_EXPORT static int mmhjcan_(int*    ndimen,
                                      int*    ncourb,
                                      int*    ncftab,
                                      int*    orcont,
                                      int*    ncflim,
                                      double* tcbold,
                                      double* tdecop,
                                      double* tcbnew,
                                      int*    iercod);

  Standard_EXPORT static int mminltt_(int*    ncolmx,
                                      int*    nlgnmx,
                                      double* tabtri,
                                      int*    nbrcol,
                                      int*    nbrlgn,
                                      double* ajoute,
                                      double* epseg,
                                      int*    iercod);

  Standard_EXPORT static int mmjaccv_(const int*    ncoef,
                                      const int*    ndim,
                                      const int*    ider,
                                      const double* crvlgd,
                                      double*       polaux,
                                      double*       crvcan);

  Standard_EXPORT static int mmpobas_(double* tparam,
                                      int*    iordre,
                                      int*    ncoeff,
                                      int*    nderiv,
                                      double* valbas,
                                      int*    iercod);

  Standard_EXPORT static int mmmpocur_(int*    ncofmx,
                                       int*    ndim,
                                       int*    ndeg,
                                       double* courbe,
                                       double* tparam,
                                       double* tabval);

  Standard_EXPORT static int mmposui_(int* dimmat,
                                      int* nistoc,
                                      int* aposit,
                                      int* posuiv,
                                      int* iercod);

  Standard_EXPORT static int mmresol_(int*    hdimen,
                                      int*    gdimen,
                                      int*    hnstoc,
                                      int*    gnstoc,
                                      int*    mnstoc,
                                      double* matsyh,
                                      double* matsyg,
                                      double* vecsyh,
                                      double* vecsyg,
                                      int*    hposit,
                                      int*    hposui,
                                      int*    gposit,
                                      int*    mmposui,
                                      int*    mposit,
                                      double* vecsol,
                                      int*    iercod);

  Standard_EXPORT static int mmrtptt_(int* ndglgd, double* rtlegd);

  Standard_EXPORT static int mmsrre2_(double* tparam,
                                      int*    nbrval,
                                      double* tablev,
                                      double* epsil,
                                      int*    numint,
                                      int*    itypen,
                                      int*    iercod);

  Standard_EXPORT static int mmtrpjj_(int*    ncofmx,
                                      int*    ndimen,
                                      int*    ncoeff,
                                      double* epsi3d,
                                      int*    iordre,
                                      double* crvlgd,
                                      double* ycvmax,
                                      double* errmax,
                                      int*    ncfnew);

  Standard_EXPORT static int mmunivt_(int*    ndimen,
                                      double* vector,
                                      double* vecnrm,
                                      double* epsiln,
                                      int*    iercod);

  Standard_EXPORT static int mmvncol_(int*    ndimen,
                                      double* vecin,
                                      double* vecout,
                                      int*    iercod);

  Standard_EXPORT static double msc_(int* ndimen, double* vecte1, double* vecte2);

  Standard_EXPORT static int mvsheld_(int* n, int* is, double* dtab, int* icle);

  Standard_EXPORT static int mmarcin_(int*    ndimax,
                                      int*    ndim,
                                      int*    ncoeff,
                                      double* crvold,
                                      double* u0,
                                      double* u1,
                                      double* crvnew,
                                      int*    iercod);

  Standard_EXPORT static int mmcvinv_(int*    ndimax,
                                      int*    ncoef,
                                      int*    ndim,
                                      double* curveo,
                                      double* curve);

  Standard_EXPORT static int mmjacan_(const int* ideriv,
                                      int*       ndeg,
                                      double*    poljac,
                                      double*    polcan);

  Standard_EXPORT static int mmpocrb_(int*    ndimax,
                                      int*    ncoeff,
                                      double* courbe,
                                      int*    ndim,
                                      double* tparam,
                                      double* pntcrb);

  Standard_EXPORT static int mmmrslwd_(int*    normax,
                                       int*    nordre,
                                       int*    ndim,
                                       double* amat,
                                       double* bmat,
                                       double* epspiv,
                                       double* aaux,
                                       double* xmat,
                                       int*    iercod);

  Standard_EXPORT static int mmveps3_(double* eps03);

  Standard_EXPORT static double pow__di(double* x, int* n);

  Standard_EXPORT static double mzsnorm_(int* ndimen, double* vecteu);
};

#endif
