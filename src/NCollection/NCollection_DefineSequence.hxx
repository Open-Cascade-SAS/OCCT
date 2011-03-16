// File:        NCollection_DefineSequence.hxx
// Created:     28.03.02 20:41:43
// Author:      Alexander GRIGORIEV
//            Automatically created from NCollection_Sequence.hxx by GAWK
// Copyright:   Open Cascade 2002
//           
// Purpose:     Definition of a sequence of elements indexed by
//              an Integer in range of 1..n
//              


#ifndef NCollection_DefineSequence_HeaderFile
#define NCollection_DefineSequence_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Sequence.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// **************************************** Template for Sequence class ********

#define DEFINE_SEQUENCE(_ClassName_, _BaseCollection_, TheItemType)            \
        typedef NCollection_Sequence<TheItemType > _ClassName_;

#endif
