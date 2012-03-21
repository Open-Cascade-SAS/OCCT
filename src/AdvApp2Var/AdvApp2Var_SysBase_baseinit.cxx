// Created on: 1996-05-21
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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
 
