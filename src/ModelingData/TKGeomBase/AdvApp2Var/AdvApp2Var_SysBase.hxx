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

// AdvApp2Var_SysBase.hxx
#ifndef AdvApp2Var_SysBase_HeaderFile
#define AdvApp2Var_SysBase_HeaderFile

#include <Standard_TypeDef.hxx>

class AdvApp2Var_SysBase
{
public:
  Standard_EXPORT AdvApp2Var_SysBase();
  Standard_EXPORT ~AdvApp2Var_SysBase();

  //
  Standard_EXPORT int mainial_();

  Standard_EXPORT static int macinit_(int*, int*);
  //
  Standard_EXPORT int mcrdelt_(int* iunit, int* isize, void* t, intptr_t* iofset, int* iercod);

  Standard_EXPORT static int mcrfill_(int* size, void* tin, void* tout);

  Standard_EXPORT int mcrrqst_(int* iunit, int* isize, void* t, intptr_t* iofset, int* iercod);
  Standard_EXPORT static int mnfndeb_();
  Standard_EXPORT int        macrai4_(int*      nbelem,
                                      int*      maxelm,
                                      int*      itablo,
                                      intptr_t* iofset,
                                      int*      iercod);
  Standard_EXPORT int        macrar8_(int*      nbelem,
                                      int*      maxelm,
                                      double*   xtablo,
                                      intptr_t* iofset,
                                      int*      iercod);
  Standard_EXPORT int        macrdi4_(int*      nbelem,
                                      int*      maxelm,
                                      int*      itablo,
                                      intptr_t* iofset,
                                      int*      iercod);

  Standard_EXPORT int        macrdr8_(int*      nbelem,
                                      int*      maxelm,
                                      double*   xtablo,
                                      intptr_t* iofset,
                                      int*      iercod);
  Standard_EXPORT static int maermsg_(const char* cnompg, int* icoder, long cnompg_len);
  Standard_EXPORT static int maitbr8_(int* itaill, double* xtab, double* xval);
  Standard_EXPORT static int maovsr8_(int* ivalcs);
  Standard_EXPORT static int mgenmsg_(const char* nomprg, long nomprg_len);

  Standard_EXPORT static int  mgsomsg_(const char* nomprg, long nomprg_len);
  Standard_EXPORT static void miraz_(int* taille, void* adt);
  Standard_EXPORT static int  msifill_(int* nbintg, int* ivecin, int* ivecou);
  Standard_EXPORT static int  msrfill_(int* nbreel, double* vecent, double* vecsor);
  Standard_EXPORT static int  mswrdbg_(const char* ctexte, long ctexte_len);
  Standard_EXPORT static void mvriraz_(int* taille, void* adt);

private:
  int macrchk_();
  int mcrlist_(int* ier) const;

  /* Maximum number of allowed allocation requests.
     Currently the maximum known number of requests is 7 - see
     AdvApp2Var_MathBase::mmresol_(). So the current value is a safe margin and
     a reasonable balance to not provoke stack overflow (especially in
     multi-threaded execution). Previous number of 1000 was excessive but
     tolerable when used for static memory.
  */
  static const int MAX_ALLOC_NB = 32;

  enum
  {
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
  struct mitem
  {
    unsigned char prot;
    unsigned char unit; // unit of allocation: 1, 2, 4 or 8
    int           reqsize;
    intptr_t      loc;
    intptr_t      offset;
    unsigned char alloctype; // static_allocation or heap_allocation
    int           size;
    intptr_t      addr;
    int           userzone; // not used
    intptr_t      startaddr;
    intptr_t      endaddr;
    int           rank;
  };

  struct
  {
    mitem         icore[MAX_ALLOC_NB];
    int           ncore;
    unsigned char lprot;
  } mcrgene_;

  /* Contains statistics on allocation requests.
     Index 0 corresponds to static_allocation, 1 - to heap allocation.
     nrqst - number of allocation requests;
     ndelt - number of deallocation requests;
     nbyte - current number of allocated bytes;
     mbyte - maximum number of ever allocated bytes.
  */
  struct
  {
    int nrqst[2], ndelt[2], nbyte[2], mbyte[2];
  } mcrstac_;
};

#endif
