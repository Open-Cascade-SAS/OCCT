// File:      NCollection_HSequence.hxx
// Created:   29.01.01 12:58:53
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2002

#ifndef NCollection_HSequence_HeaderFile
#define NCollection_HSequence_HeaderFile

#include <NCollection_DefineHSequence.hxx>
#include <NCollection_Sequence.hxx>

//      Declaration of Sequence class managed by Handle

#define NCOLLECTION_HSEQUENCE(HClassName,Type)                                 \
        DEFINE_HSEQUENCE(HClassName,NCollection_Sequence<Type >)

#endif
