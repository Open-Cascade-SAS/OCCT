// Created on: 1996-05-21
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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
 
