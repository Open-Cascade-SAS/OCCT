// File:        NCollection_HSet.hxx
// Created:     Mon Apr 29 12:34:55 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>

#ifndef NCollection_HSet_HeaderFile
#define NCollection_HSet_HeaderFile

#include <NCollection_DefineHSet.hxx>
#include <NCollection_Set.hxx>

//      Declaration of Set class managed by Handle

#define NCOLLECTION_HSET(HClassName,Type)                                      \
        DEFINE_HSET(HClassName,NCollection_Set<Type >)

#endif
