// File:        NCollection_DefineMap.hxx
// Created:     Thu Apr 23 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//
// Purpose:     Single hashed Map. This  Map is used  to store and
//              retrieve keys in linear time.
//              
//              The ::Iterator class can be  used to explore  the
//              content of the map. It is not  wise to iterate and
//              modify a map in parallel.
//               
//              To compute  the hashcode of  the key the  function
//              ::HashCode must be defined in the global namespace
//               
//              To compare two keys the function ::IsEqual must be
//              defined in the global namespace.
//               
//              The performance of  a Map is conditionned  by  its
//              number of buckets that  should be kept greater  to
//              the number   of keys.  This  map has  an automatic
//              management of the number of buckets. It is resized
//              when  the number of Keys  becomes greater than the
//              number of buckets.
//              
//              If you have a fair  idea of the number of  objects
//              you  can save on automatic   resizing by giving  a
//              number of buckets  at creation or using the ReSize
//              method. This should be  consider only for  crucial
//              optimisation issues.
//              

#ifndef NCollection_DefineMap_HeaderFile
#define NCollection_DefineMap_HeaderFile

#include <NCollection_DefineBaseCollection.hxx>
#include <NCollection_Map.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

// *********************************************** Class Map *****************

#define DEFINE_MAP(_ClassName_, _BaseCollection_, TheKeyType)                  \
        typedef NCollection_Map <TheKeyType > _ClassName_;

#endif
