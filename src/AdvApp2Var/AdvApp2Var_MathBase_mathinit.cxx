// File:	fdetlibinit.cxx
// Created:	Tue May 21 11:49:45 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>

#include <AdvApp2Var_SysBase.hxx>
#include <AdvApp2Var_MathBase.hxx>


static integer init_STMAT(void)
{
  // Init PRCSN
  doublereal  EPSIL1 =1.e-9,
          EPSIL2=1.e-8,
          EPSIL3=1.e-9,
          EPSIL4=1.e-4;
  integer NITER1=8,
          NITER2=40;
  AdvApp2Var_MathBase::mmwprcs_(&EPSIL1, 
				&EPSIL2, 
				&EPSIL3, 
				&EPSIL4, 
				&NITER1, 
				&NITER2);

  return 1;
}

  class STMATLibInit
 {
   static integer var_STMATLibINIT;
 };

integer STMATLibInit::var_STMATLibINIT = init_STMAT();
 
