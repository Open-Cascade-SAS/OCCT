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


static int init_STBAS(void)
{
  int ICODE = 0;
  //allocation tables are now local, so no global initialization is required
#if 0
  // Init du Tableau des allocs
  AdvApp2Var_SysBase::mainial_();
#endif
  // Init de LEC IBB IMP
  AdvApp2Var_SysBase::macinit_(&ICODE, &ICODE);
  //
  return 1;
}
//
class STBASLibInit
{
  static int var_STBASLibINIT;
};

int STBASLibInit::var_STBASLibINIT = init_STBAS();
 
