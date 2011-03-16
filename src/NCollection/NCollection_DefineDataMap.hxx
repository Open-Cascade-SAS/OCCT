// File:        NCollection_DefineDataMap.hxx
// Created:     Thu Apr 24 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@opencascade.com>
//              
// Purpose:     The DataMap is a Map to store keys with associated
//              Items. See Map  from NCollection for  a discussion
//              about the number of buckets.
//
//              The DataMap can be seen as an extended array where
//              the Keys  are the   indices.  For this reason  the
//              operator () is defined on DataMap to fetch an Item
//              from a Key. So the following syntax can be used :
//
//              anItem = aMap(aKey);
//              aMap(aKey) = anItem;
//
//              This analogy has its  limit.   aMap(aKey) = anItem
//              can  be done only  if aKey was previously bound to
//              an item in the map.
//              

#ifndef NCollection_DefineDataMap_HeaderFile
#define NCollection_DefineDataMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_DataMap.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// *********************************************** Class DataMap *************

#define DEFINE_DATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType) \
        typedef NCollection_DataMap<TheKeyType, TheItemType > _ClassName_;

#endif
