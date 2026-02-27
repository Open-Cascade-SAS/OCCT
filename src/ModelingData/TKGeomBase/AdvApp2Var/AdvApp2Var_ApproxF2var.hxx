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

// AdvApp2Var_ApproxF2var.hxx
/*---------------------------------------------------------------
|  description de la macro et du prototype des routines
|  de l'approximation a deux variables
|  a utiliser dans AdvApp2Var
|--------------------------------------------------------------*/
#ifndef AdvApp2Var_ApproxF2var_HeaderFile
#define AdvApp2Var_ApproxF2var_HeaderFile

#include <Standard_Macro.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>

//
class AdvApp2Var_ApproxF2var
{
public:
  Standard_EXPORT static int mma2fnc_(int*                            ndimen,
                                      int*                            nbsesp,
                                      int*                            ndimse,
                                      double*                         uvfonc,
                                      const AdvApp2Var_EvaluatorFunc2Var& foncnp,
                                      double*                         tconst,
                                      int*                            isofav,
                                      int*                            nbroot,
                                      double*                         rootlg,
                                      int*                            iordre,
                                      int*                            ideriv,
                                      int*                            ndgjac,
                                      int*                            nbcrmx,
                                      int*                            ncflim,
                                      double*                         epsapr,
                                      int*                            ncoeff,
                                      double*                         courbe,
                                      int*                            nbcrbe,
                                      double*                         somtab,
                                      double*                         diftab,
                                      double*                         contr1,
                                      double*                         contr2,
                                      double*                         tabdec,
                                      double*                         errmax,
                                      double*                         errmoy,
                                      int*                            iercod);

  Standard_EXPORT static int mma2roo_(int*    nbpntu,
                                      int*    nbpntv,
                                      double* urootl,
                                      double* vrootl);

  Standard_EXPORT static int mma2jmx_(int* ndgjac, int* iordre, double* xjacmx);

  Standard_EXPORT static int mmapptt_(const int*,
                                      const int*,
                                      const int*,
                                      double*,
                                      int*);

  Standard_EXPORT static int mma2cdi_(int*    ndimen,
                                      int*    nbpntu,
                                      double* urootl,
                                      int*    nbpntv,
                                      double* vrootl,
                                      int*    iordru,
                                      int*    iordrv,
                                      double* contr1,
                                      double* contr2,
                                      double* contr3,
                                      double* contr4,
                                      double* sotbu1,
                                      double* sotbu2,
                                      double* ditbu1,
                                      double* ditbu2,
                                      double* sotbv1,
                                      double* sotbv2,
                                      double* ditbv1,
                                      double* ditbv2,
                                      double* sosotb,
                                      double* soditb,
                                      double* disotb,
                                      double* diditb,
                                      int*    iercod);

  Standard_EXPORT static int mma2ds1_(int*                            ndimen,
                                      double*                         uintfn,
                                      double*                         vintfn,
                                      const AdvApp2Var_EvaluatorFunc2Var& foncnp,
                                      int*                            nbpntu,
                                      int*                            nbpntv,
                                      double*                         urootb,
                                      double*                         vrootb,
                                      int*                            isofav,
                                      double*                         sosotb,
                                      double*                         disotb,
                                      double*                         soditb,
                                      double*                         diditb,
                                      double*                         fpntab,
                                      double*                         ttable,
                                      int*                            iercod);

  Standard_EXPORT static int mma2ce1_(int*    numdec,
                                      int*    ndimen,
                                      int*    nbsesp,
                                      int*    ndimse,
                                      int*    ndminu,
                                      int*    ndminv,
                                      int*    ndguli,
                                      int*    ndgvli,
                                      int*    ndjacu,
                                      int*    ndjacv,
                                      int*    iordru,
                                      int*    iordrv,
                                      int*    nbpntu,
                                      int*    nbpntv,
                                      double* epsapr,
                                      double* sosotb,
                                      double* disotb,
                                      double* soditb,
                                      double* diditb,
                                      double* patjac,
                                      double* errmax,
                                      double* errmoy,
                                      int*    ndegpu,
                                      int*    ndegpv,
                                      int*    itydec,
                                      int*    iercod);

  Standard_EXPORT static int mma2can_(const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const double*,
                                      double*,
                                      double*,
                                      int*);

  Standard_EXPORT static int mma1her_(const int*, double*, int*);

  Standard_EXPORT static int mma2ac2_(const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const double*,
                                      const int*,
                                      const double*,
                                      const double*,
                                      double*);

  Standard_EXPORT static int mma2ac3_(const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const double*,
                                      const int*,
                                      const double*,
                                      const double*,
                                      double*);

  Standard_EXPORT static int mma2ac1_(const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const int*,
                                      const double*,
                                      const double*,
                                      const double*,
                                      const double*,
                                      const double*,
                                      const double*,
                                      double*);

  Standard_EXPORT static int mma2fx6_(int*    ncfmxu,
                                      int*    ncfmxv,
                                      int*    ndimen,
                                      int*    nbsesp,
                                      int*    ndimse,
                                      int*    nbupat,
                                      int*    nbvpat,
                                      int*    iordru,
                                      int*    iordrv,
                                      double* epsapr,
                                      double* epsfro,
                                      double* patcan,
                                      double* errmax,
                                      int*    ncoefu,
                                      int*    ncoefv);
};

#endif
