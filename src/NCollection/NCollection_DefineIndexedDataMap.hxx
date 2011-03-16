// File:        NCollection_DefineIndexedDataMap.hxx
// Created:     Thu Apr 24 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@opencascade.com>
//              
// Purpose:     An indexed map is used  to store keys and to  bind
//              an index to them.  Each  new key stored in the map
//              gets an index.  Index are  incremented as keys are
//              stored in the map. A key can be found by the index
//              and an index by the key.  No  key but the last can
//              be  removed so the  indices   are in the range 1..
//              Extent.  An Item is stored with each key.
//              
//              This   class is   similar  to  IndexedMap     from
//              NCollection  with the Item as  a new feature. Note
//              the important difference on  the operator  ().  In
//              the IndexedMap this operator returns  the Key.  In
//              the IndexedDataMap this operator returns the Item.
//               
//              See  the  class   Map   from NCollection   for   a
//              discussion about the number of buckets.
//              

#ifndef NCollection_DefineIndexedDataMap_HeaderFile
#define NCollection_DefineIndexedDataMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_IndexedDataMap.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// *********************************************** Class IndexedDataMap ******

#define DEFINE_INDEXEDDATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType) \
        typedef NCollection_IndexedDataMap <TheKeyType, TheItemType > _ClassName_;

#endif
