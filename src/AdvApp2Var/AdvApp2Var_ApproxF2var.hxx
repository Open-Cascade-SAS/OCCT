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
#include <AdvApp2Var_Data_f2c.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
//
class AdvApp2Var_ApproxF2var {
 public:
  
Standard_EXPORT static int mma2fnc_(integer *ndimen, 
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
				    integer *iercod);


Standard_EXPORT static int mma2roo_(integer *nbpntu, 
				    integer *nbpntv, 
				    doublereal *urootl, 
				    doublereal *vrootl);


Standard_EXPORT static int mma2jmx_(integer *ndgjac, 
				    integer *iordre, 
				    doublereal *xjacmx);

Standard_EXPORT static int mmapptt_(const integer * , 
				    const integer * , 
				    const integer *  , 
				    doublereal * , 
				    integer * );

Standard_EXPORT static int mma2cdi_(integer *ndimen, 
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
				    integer *iercod);


Standard_EXPORT static int mma2ds1_(integer *ndimen, 
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
				    integer *iercod);

Standard_EXPORT static int mma2ce1_(integer *numdec, 
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
				    integer *iercod);


Standard_EXPORT static int mma2can_(const integer *  , 
				  const integer *  , 
				  const integer *  ,
				  const integer *  , 
				  const integer *  ,
				  const integer *  , 
				  const integer *  , 
				  const doublereal *,
				  doublereal * , 
				  doublereal * , 
				  integer *  );


Standard_EXPORT static int mma1her_(const integer *  , 
				    doublereal * , 
				    integer *   );


Standard_EXPORT static int mma2ac2_(const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer * , 
				   const doublereal * ,
				   const integer *  , 
				   const doublereal * , 
				   const doublereal * ,
				   doublereal * );


Standard_EXPORT static int mma2ac3_(const integer * , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  ,
				   const doublereal * ,
				   const integer *  , 
				   const doublereal * ,
				   const doublereal * ,
				   doublereal * );


Standard_EXPORT static int mma2ac1_(const integer *  , 
				   const integer *  , 
				   const integer *  , 
				   const integer *  ,
				   const integer *  , 
				   const doublereal * , 
				   const doublereal * ,
				   const doublereal * ,
				   const doublereal * ,
				   const doublereal * , 
				   const doublereal * , 
				   doublereal * );


Standard_EXPORT static int mma2fx6_(integer *ncfmxu,
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
				    integer *ncoefv);
};

#endif
