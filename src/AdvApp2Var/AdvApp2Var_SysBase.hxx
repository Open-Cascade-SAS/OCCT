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
  Standard_EXPORT AdvApp2Var_SysBase();
  Standard_EXPORT ~AdvApp2Var_SysBase();
  
  //
  Standard_EXPORT int mainial_();

  Standard_EXPORT static int macinit_(int *, 
				      int *);
  //
  Standard_EXPORT int mcrdelt_(integer *iunit, 
				      integer *isize, 
				      void *t, 
				      intptr_t *iofset, 
				      integer *iercod);

  Standard_EXPORT static int mcrfill_(integer *size, 
				      void *tin, 
				      void *tout);

  Standard_EXPORT int mcrrqst_(integer *iunit, 
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
  Standard_EXPORT int macrai4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo,
				      intptr_t *iofset,
				      integer *iercod);
  Standard_EXPORT int macrar8_(integer *nbelem, 
				      integer *maxelm,
				      doublereal *xtablo, 
				      intptr_t *iofset, 
				      integer *iercod);
  Standard_EXPORT int macrdi4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo, 
				      intptr_t *iofset, 
				      integer *iercod);

  Standard_EXPORT int macrdr8_(integer *nbelem,
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
  
private:
  int macrchk_();
  int mcrlist_(integer *ier) const;

  /* Maximum number of allowed allocation requests.
     Currently the maximum known number of requests is 7 - see
     AdvApp2Var_MathBase::mmresol_(). So the current value is a safe margin and
     a reasonable balance to not provoke stack overflow (especially in
     multi-threaded execution). Previous number of 1000 was excessive but
     tolerable when used for static memory.
  */
  static const int MAX_ALLOC_NB = 32;
  
  enum {
    static_allocation = 0, /* indicates static allocation, currently not used */
    heap_allocation   = 1  /* indicates heap allocation */
  };
  
  /* Describes an individual memory allocation request.
     See format description in the AdvApp2Var_SysBase.cxx.
     The field order is preserved and the sizes are chosen to minimize
     memory footprint. Fields containing address have the intptr_t type
     for easier arithmetic and to avoid casts in the source code.

     No initialization constructor should be provided to avoid wasting
     time when allocating a field mcrgene_.
  */
  struct mitem {
    unsigned char   prot;
    unsigned char   unit; //unit of allocation: 1, 2, 4 or 8
    integer         reqsize;
    intptr_t        loc;
    intptr_t        offset;
    unsigned char   alloctype; // static_allocation or heap_allocation
    integer         size;
    intptr_t        addr;
    integer         userzone; //not used
    intptr_t        startaddr;
    intptr_t        endaddr;
    integer         rank;
  };
  
  struct {
    mitem           icore[MAX_ALLOC_NB];
    integer         ncore;
    unsigned char   lprot;
  } mcrgene_;

  /* Contains statistics on allocation requests.
     Index 0 corresponds to static_allocation, 1 - to heap allocation.
     nrqst - number of allocation requests;
     ndelt - number of deallocation requests;
     nbyte - current number of allocated bytes;
     mbyte - maximum number of ever allocated bytes.
  */
  struct {
    integer nrqst[2], ndelt[2], nbyte[2], mbyte[2];
  } mcrstac_; 
};

#endif
