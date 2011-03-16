// File:        NCollection_DefineBaseCollection.hxx
// Created:     Tue Apr  9 18:53:36 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//
// Purpose:     NCollection_BaseCollection  is the base  abstract  class for 
//              all collection templates of this package.
//              The set  of collections is similar  to that  of TCollection.
//              Also  the methods of classes  have mostly the same names for
//              easy switch from TCollection <-> NCollection containers.
//              
//              NCollection is a nocdlpack, thus it is compiled without WOK.
//              BaseCollection allows assigning the collections of different
//              kinds  (the  items  type  being the same) with a few obvious
//              exclusions - one can not  assign any  collection to  the map 
//              having double data (two keys or a key  plus value). Only the 
//              maps of the very same type may be assigned through operator=
//              Said maps are: DoubleMap,
//                             DataMap,
//                             IndexedDataMap
//              
//              For the  users  needing  control  over the memory  usage the
//              allocators were added (see NCollection_BaseAllocator header)
//              Others may forget it -  BaseAllocator is used by default and
//              then memory is managed through Standard::Allocate/::Free.
//              

#ifndef NCollection_DefineBaseCollection_HeaderFile
#define NCollection_DefineBaseCollection_HeaderFile

#include <NCollection_BaseCollection.hxx>

// *************************************** Template for BaseCollection class

#define DEFINE_BASECOLLECTION(_ClassName_, TheItemType)                        \
        typedef NCollection_BaseCollection<TheItemType > _ClassName_;

#endif
