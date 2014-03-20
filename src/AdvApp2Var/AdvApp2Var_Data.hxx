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

#ifndef AdvApp2Var_Data_HeaderFile
#define AdvApp2Var_Data_HeaderFile

#include <Standard_Macro.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
//
struct mdnombr_1_ {
  doublereal pi, 
  deuxpi, 
  pisur2, 
  pis180, 
  c180pi, 
  zero, 
  one, 
  a180, 
  a360, 
  a90;
};
//
struct minombr_1_ {
  integer nbr[1001];
};
//
struct maovpar_1_ {
  doublereal r8und, r8ovr, x4und, x4ovr;
    real r4und, r4ovr;
  integer r4nbe, r8nbm, r8nbe, i4ovr, i4ovn, r4exp, r8exp, r4exn, r8exn, 
  r4ncs, r8ncs, r4nbm;
  shortint i2ovr, i2ovn;
};
//
struct maovpch_1_ {
  char cnmmac[16], frmr4[8], frmr8[8], cdcode[8];
};
//
struct mlgdrtl_1_ {
    doublereal rootab[930],// was [465][2]  
    hiltab[930],// was [465][2]  
    hi0tab[31];
  };
//
struct mmjcobi_1_ {
    doublereal plgcan[3968];// was [496][2][4]  
    doublereal canjac[3968];// was [496][2][4] 
};
//
struct mmcmcnp_1_ {
  doublereal cnp[3721];	// was [61][61] ;
};
//
struct mmapgss_1_ {
  doublereal gslxjs[5017], 
  gsl0js[52];
};
//
struct mmapgs0_1_ {
  doublereal gslxj0[4761], gsl0j0[49];
};
//
struct mmapgs1_1_ {
  doublereal gslxj1[4505], gsl0j1[46];
};
//
struct mmapgs2_1_ {
  doublereal gslxj2[4249], gsl0j2[43];
};
////
class AdvApp2Var_Data {
 public: 
  
  Standard_EXPORT static mdnombr_1_& Getmdnombr();
  Standard_EXPORT static minombr_1_& Getminombr();
  Standard_EXPORT static maovpar_1_& Getmaovpar();
  Standard_EXPORT static maovpch_1_& Getmaovpch();
  Standard_EXPORT static mlgdrtl_1_& Getmlgdrtl();
  Standard_EXPORT static mmjcobi_1_& Getmmjcobi();
  Standard_EXPORT static mmcmcnp_1_& Getmmcmcnp();
  Standard_EXPORT static mmapgss_1_& Getmmapgss();
  Standard_EXPORT static mmapgs0_1_& Getmmapgs0();
  Standard_EXPORT static mmapgs1_1_& Getmmapgs1();
  Standard_EXPORT static mmapgs2_1_& Getmmapgs2();
 
};
//
#define mdnombr_ AdvApp2Var_Data::Getmdnombr()
#define minombr_ AdvApp2Var_Data::Getminombr()
#define maovpar_ AdvApp2Var_Data::Getmaovpar()
#define maovpch_ AdvApp2Var_Data::Getmaovpch()
#define mlgdrtl_ AdvApp2Var_Data::Getmlgdrtl()
#define mmjcobi_ AdvApp2Var_Data::Getmmjcobi()
#define mmcmcnp_ AdvApp2Var_Data::Getmmcmcnp()
#define mmapgss_ AdvApp2Var_Data::Getmmapgss()
#define mmapgs0_ AdvApp2Var_Data::Getmmapgs0()
#define mmapgs1_ AdvApp2Var_Data::Getmmapgs1()
#define mmapgs2_ AdvApp2Var_Data::Getmmapgs2()
//
#endif
