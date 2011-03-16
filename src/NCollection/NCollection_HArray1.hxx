// File:        NCollection_HArray1.hxx
// Created:     Mon Apr 29 11:36:31 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
// Copyright:   Open Cascade 2002

#ifndef NCollection_HArray1_HeaderFile
#define NCollection_HArray1_HeaderFile

#include <NCollection_Array1.hxx>
#include <NCollection_DefineHArray1.hxx>

//      Declaration of Array1 class managed by Handle

#define NCOLLECTION_HARRAY1(HClassName,Type)                                   \
        DEFINE_HARRAY1(HClassName,NCollection_Array1<Type >)

#endif
