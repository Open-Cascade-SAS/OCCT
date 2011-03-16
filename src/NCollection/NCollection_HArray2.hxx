// File:        NCollection_HArray2.hxx
// Created:     Mon Apr 29 11:36:31 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
// Copyright:   Open Cascade 2002

#ifndef NCollection_HArray2_HeaderFile
#define NCollection_HArray2_HeaderFile

#include <NCollection_Array2.hxx>
#include <NCollection_DefineHArray2.hxx>

//      Declaration of Array2 class managed by Handle

#define NCOLLECTION_HARRAY2(HClassName,Type)                                   \
        DEFINE_HARRAY2(HClassName,NCollection_Array2<Type >)

#endif
