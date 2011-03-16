// File:	fdetlibinit.cxx
// Created:	Tue May 21 11:49:45 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>

#include <AdvApp2Var_SysBase.hxx>


static int init_STBAS(void)
{
  int ICODE = 0;
  // Init du Tableau des allocs
  AdvApp2Var_SysBase::mainial_();
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
 
