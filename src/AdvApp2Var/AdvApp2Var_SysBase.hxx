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

// AdvApp2Var_SysBase.hxx
#ifndef AdvApp2Var_SysBase_HeaderFile
#define AdvApp2Var_SysBase_HeaderFile

#include <Standard_Macro.hxx>
#include <AdvApp2Var_Data_f2c.hxx>
#if _MSC_VER
#include <stddef.h>
#else
#include <stdint.h>
#endif

class AdvApp2Var_SysBase {
 public: 
  //
  Standard_EXPORT static int mainial_();

  Standard_EXPORT static int macinit_(int *, 
				      int *);
  //
  Standard_EXPORT static int mcrdelt_(integer *iunit, 
				      integer *isize, 
				      void *t, 
				      intptr_t *iofset, 
				      integer *iercod);

  Standard_EXPORT static int mcrfill_(integer *size, 
				      void *tin, 
				      void *tout);

  Standard_EXPORT static int mcrrqst_(integer *iunit, 
				      integer *isize, 
				      void *t, 
				      intptr_t *iofset, 
				      integer *iercod);
  Standard_EXPORT static integer mnfndeb_();

  Standard_EXPORT static integer mnfnimp_();
  Standard_EXPORT static int do__fio(); 
  Standard_EXPORT static int do__lio ();
  Standard_EXPORT static int e__wsfe ();
  Standard_EXPORT static int e__wsle ();
  Standard_EXPORT static int s__wsfe ();
  Standard_EXPORT static int s__wsle ();
  Standard_EXPORT static int macrai4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo,
				      intptr_t *iofset,
				      integer *iercod);
  Standard_EXPORT static int macrar8_(integer *nbelem, 
				      integer *maxelm,
				      doublereal *xtablo, 
				      intptr_t *iofset, 
				      integer *iercod);
  Standard_EXPORT static int macrdi4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo, 
				      intptr_t *iofset, 
				      integer *iercod);

  Standard_EXPORT static int macrdr8_(integer *nbelem,
				      integer *maxelm, 
				      doublereal *xtablo, 
				      intptr_t *iofset, 
				      integer *iercod);
  Standard_EXPORT static int maermsg_(const char *cnompg, 
				      integer *icoder, 
				      ftnlen cnompg_len);
  Standard_EXPORT static int maitbr8_(integer *itaill, 
				      doublereal *xtab, 
				      doublereal *xval);
  Standard_EXPORT static int maovsr8_(integer *ivalcs);
  Standard_EXPORT static int mgenmsg_(const char *nomprg, 
				      ftnlen nomprg_len);

  Standard_EXPORT static int mgsomsg_(const char *nomprg, 
				      ftnlen nomprg_len);
  Standard_EXPORT static void miraz_(integer *taille,
				     void *adt);
  Standard_EXPORT static int msifill_(integer *nbintg, 
				      integer *ivecin,
				      integer *ivecou);
  Standard_EXPORT static int msrfill_(integer *nbreel, 
				      doublereal *vecent,
				      doublereal * vecsor);
  Standard_EXPORT static int mswrdbg_(const char *ctexte, 
				      ftnlen ctexte_len);
  Standard_EXPORT static void mvriraz_(integer *taille,
				       void*adt);
  
};

#endif
