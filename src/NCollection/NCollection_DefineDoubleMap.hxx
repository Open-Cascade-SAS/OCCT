// File:        NCollection_DefineDoubleMap.hxx
// Created:     Thu Apr 24 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@opencascade.com>
//
// Purpose:     The DoubleMap  is used to  bind  pairs (Key1,Key2)
//              and retrieve them in linear time.
//              
//              See Map from NCollection for a discussion about the number
//              of buckets
//              

#ifndef NCollection_DefineDoubleMap_HeaderFile
#define NCollection_DefineDoubleMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DoubleMap.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// *********************************************** Class DoubleMap ************

#define DEFINE_DOUBLEMAP(_ClassName_, _BaseCollection_, TheKey1Type, TheKey2Type) \
        typedef NCollection_DoubleMap <TheKey1Type, TheKey2Type > _ClassName_;

#endif
