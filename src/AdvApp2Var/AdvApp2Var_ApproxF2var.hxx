// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// AdvApp2Var_ApproxF2var.hxx
/*---------------------------------------------------------------
|  description de la macro et du prototype des routines 
|  de l'approximation a deux variables
|  a utiliser dans AdvApp2Var
|--------------------------------------------------------------*/
#ifndef ApproxF2var_HeaderFile
#define ApproxF2var_HeaderFile

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
